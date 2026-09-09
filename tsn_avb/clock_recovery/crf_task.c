/*
 *  Copyright (C) 2026 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file   crf_task.c
 *
 * \brief AVTP/CRF transport registration against eval_src, the RTOS task
 * (via tsn_combase's CB_THREAD_CREATE, not TaskP_construct directly), and
 * the two transport-facing ClockRec_* hooks
 * (ClockRec_crfTsReadyHook/ClockRec_needsRestartHook). The audio-facing
 * hooks (ClockRec_clockStartedHook/ClockRec_tsAvailableHook) are
 * deliberately NOT implemented here - they belong to a future
 * audio-pipeline module.
 *
 * clock_rec.c/.h are the only files in this directory that touch
 * ClockHw_*; this file never does. This file is the only one in this
 * directory that touches eval_src's avtpc_crf_ and avbtp_ APIs -
 * clock_rec.c/_listener.c/_talker.c never do.
 */

#include <string.h>
#include <stdbool.h>
#include <kernel/dpl/ClockP.h>
#include <tsn_unibase/ub_strutils.h>
#include <tsn_combase/cb_thread.h>
#include <tsn_combase/cb_tmevent.h>
#include <enet_types.h>
#include "tsn_gptp/gptpmasterclock.h"
#include "avtpc_crf.h"
#include "clock_rec.h"
#include "crf_task.h"
#ifndef UB_LOGCAT
#define UB_LOGCAT 5
#endif
#include <tsn_unibase/unibase.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                              Global Variables                              */
/* ========================================================================== */

/* Single-instance, matching ClockRec_*'s own module-singleton design -
 * one-per-core static app config, not a dynamic pool. */
static CrfTask_Params CrfTask_params;
/* tsPerAvtpdu with the 0 => default already resolved by CrfTask_create(), so
 * the rx path (running before ClockRec_init) and the tx hook agree on it. */
static uint32_t CrfTask_tsPerAvtpdu = CLOCK_REC_DEFAULT_TS_PER_AVTPDU;
static avtpc_crf_data_t *CrfTask_avtpcCrf;
static avtpc_crf_config_t CrfTask_config;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void *CrfTask_main(void *args);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/*
 * Registered via avtpc_crf_set_rcv_cb() (listener role only). Runs on the
 * AVBTP stack's own receiver thread, a separate execution context from
 * both the CPTS ISR and this file's own task - ClockRec_submitCrfTs() is
 * documented as safe to call from exactly this kind of async context.
 */
int CrfTask_rxCallback(uint8_t *payload, int payload_size,
                        avbtp_rcv_cb_info_t *cbinfo, void *cbdata)
{
    avbtp_crf_info_t *rcrfinfo = &cbinfo->u.rcrfinfo;
    uint64_t crfTsArray[CLOCK_REC_MAX_TS_PER_AVTPDU];
    int numTs;
    int result = ENET_SOK;

    if (rcrfinfo->subtype != AVBTP_SUBTYPE_CRF)
    {
        UB_LOG(UBL_INFO, "%s: unsupported subtype: %d\r\n", __func__, (int)rcrfinfo->subtype);
        result = ENET_EFAIL;
    }
    else if (rcrfinfo->media_restart)
    {
        /* Transport layer parses the CRF media-clock-restart bit and
         * calls ClockRec_restart() itself, directly. */
        UB_LOG(UBL_INFO, "%s: media clock restart bit seen, restarting\r\n", __func__);
        (void)ClockRec_restart();
        result = ENET_SOK;
    }
    else
    {
        numTs = avbtp_deserialize_stdtype_crf_data(crfTsArray, CLOCK_REC_MAX_TS_PER_AVTPDU,
                                                    payload, (uint32_t)payload_size);
        if (numTs < 0)
        {
            UB_LOG(UBL_ERROR, "%s: deserialization CRF failed\r\n", __func__);
            result = ENET_EFAIL;
        }
        else if ((uint32_t)numTs != CrfTask_tsPerAvtpdu)
        {
            UB_LOG(UBL_ERROR, "%s: received %d CRF timestamps, expected %u\r\n",
                   __func__, numTs, (unsigned int)CrfTask_tsPerAvtpdu);
            result = ENET_EFAIL;
        }
        else
        {
            (void)ClockRec_submitCrfTs(crfTsArray, (uint32_t)numTs);
        }
    }

    return result;
}

/*
 * Task body: runs the init sequence, then the steady-state loop, for the
 * life of the application. Signature matches CB_THREAD_CREATE's void
 * *(*func)(void*) (tilld/cb_lld_thread.h) - this task never returns in
 * practice.
 */
static void *CrfTask_main(void *args)
{
    CrfTask_Params *params = (CrfTask_Params *)args;
    ClockRec_InitParams clockRecParams;
    client_connect_request_t ccr;
    char streamIdStr[3U * 8U];
    int retStatus = 0;
    int status = 1;

    /* gptpd starts up on its own schedule, so just poll until it's there. */
    while (gptpmasterclock_init(NULL) != 0)
    {
        UB_LOG(UBL_INFO, "%s: waiting for tsn_gptpd to be ready...\r\n", __func__);
        CB_USLEEP(100000);
    }

    CrfTask_avtpcCrf = avtpc_crf_init(NULL);
    if (CrfTask_avtpcCrf == NULL)
    {
        UB_LOG(UBL_ERROR, "%s: avtpc_crf_init failed\r\n", __func__);
    }
    if (ub_assert_fatal(CrfTask_avtpcCrf != NULL, __func__,
                         "avtpc_crf_init failed"))
    {
        status = 0;
    }

    if (status)
    {
        (void)memset(&CrfTask_config, 0, sizeof(CrfTask_config));
        CrfTask_config.listener           =
            (params->role == CLOCK_REC_ROLE_LISTENER);
        CrfTask_config.netdev             = (char *)params->netdev;
        CrfTask_config.dest_mcast         = (char *)params->destMcast;
        CrfTask_config.join_mcast         = (char *)params->joinMcast;
        CrfTask_config.vlan_id            = (int16_t)params->vlanId;
        CrfTask_config.vlan_pcp           = params->vlanPcp;
        CrfTask_config.type               = AVBTP_CRF_TYPE_AUDIO_SAMPLE;
        CrfTask_config.pull               = params->pull;
        CrfTask_config.mr_pdus_toggle     = AVTPC_MR_PDUS_TOGGLE_MIN;
        CrfTask_config.base_frequency     = params->nominalFrequency;
        CrfTask_config.timestamp_interval = (uint16_t)params->tsInterval;
        /* The stack config wants the stream id as a string, not raw bytes. */
        (void)ub_bsid2ssid(params->streamId, streamIdStr);
        CrfTask_config.stream_id = streamIdStr;
    }

    if (status)
    {
        if (CrfTask_config.listener)
        {
            /* skipRxRegistration means the app already owns a shared AVTP
             * rx callback and calls CrfTask_rxCallback() itself - registering
             * our own rx-direct socket here too would double-claim the rx path. */
            if (!params->skipRxRegistration)
            {
                avtpc_crf_set_rcv_cb(CrfTask_avtpcCrf, CrfTask_rxCallback, params);
                retStatus = avtpc_crf_set_rxdirect(CrfTask_avtpcCrf);
            }
        }
        else
        {
            /* TALKER and TALKER_LOOPBACK both transmit - loopback self-tracks
             * its own output via ClockRec_*'s internal loop, not by receiving
             * network CRF traffic, so no rcv_cb here either way. */
            retStatus = avtpc_crf_set_txdirect(CrfTask_avtpcCrf);
        }
        if (retStatus != 0)
        {
            UB_LOG(UBL_ERROR, "%s: avtpc_crf_set_rxdirect/txdirect failed\r\n", __func__);
        }
        if (ub_assert_fatal(retStatus == 0, __func__, "avtpc_crf_set_rxdirect/txdirect failed"))
        {
            status = 0;
        }
    }

    if (status && !CrfTask_config.listener)
    {
        retStatus = avtpc_crf_set_ccr(CrfTask_avtpcCrf, &CrfTask_config, &ccr);
        if (retStatus == 0)
        {
            ccr.echoback = false;
            retStatus = avtpc_crf_connection_request(CrfTask_avtpcCrf, &ccr);
        }
        if (retStatus != 0)
        {
            UB_LOG(UBL_ERROR, "%s: avtpc_crf_set_ccr/connection_request failed\r\n", __func__);
        }
        if (ub_assert_fatal(retStatus == 0, __func__, "avtpc_crf_set_ccr/connection_request failed"))
        {
            status = 0;
        }
    }

    if (status)
    {
        /* Waits for a stable gPTP Grandmaster before configuring ClockRec_* -
         * this module assumes gPTP is already stable before being asked to
         * recover. */
        while (gptpmasterclock_gm_domainIndex() < 0)
        {
            CB_SLEEP(1);
            UB_LOG(UBL_INFO, "%s: waiting for gPTP GM sync...\r\n", __func__);
        }
    }

    if (status)
    {
        clockRecParams.role                  = params->role;
        clockRecParams.clkSrc                = params->clkSrc;
        clockRecParams.parentClockFrequency  = params->parentClockFrequency;
        clockRecParams.nominalFrequency      = params->nominalFrequency;
        clockRecParams.tsInterval            = params->tsInterval;
        clockRecParams.tsPerAvtpdu           = params->tsPerAvtpdu;
        clockRecParams.correctionFlags       = params->correctionFlags;
        clockRecParams.audioMcaspBaseAddress = params->audioMcaspBaseAddress;
        clockRecParams.audioMcaspDir         = params->audioMcaspDir;
        clockRecParams.tsMcaspBaseAddress    = params->tsMcaspBaseAddress;
        clockRecParams.tsMcaspDir            = params->tsMcaspDir;
        clockRecParams.mcHwPush              = params->mcHwPush;
        clockRecParams.tsHwPush              = params->tsHwPush;
        clockRecParams.tuning                = params->tuning;

        retStatus = ClockRec_init(clockRecParams);
        if (ub_assert_fatal(retStatus == ENET_SOK, __func__, "ClockRec_init failed"))
        {
            status = 0;
        }
    }

    if (status)
    {
        while (1)
        {
            /* This is the entire tick side of this task's job.
             * ClockRec_tsAvailableHook()/ClockRec_crfTsReadyHook()/
             * ClockRec_needsRestartHook() are invoked synchronously by
             * ClockRec_* from inside this call, in this task's context, as
             * needed per tick. */
            retStatus = ClockRec_processTick();
            if (retStatus != ENET_SOK)
            {
                /* Persistent error here means the tick source itself is gone -
                 * recoverable-by-restart, not fatal to the task. */
                UB_LOG(UBL_ERROR, "%s: ClockRec_processTick failed\r\n", __func__);
                CB_USLEEP(10000);
            }
        }
    }

    return NULL;   /* unreachable - loop never exits */
}

int32_t CrfTask_create(const CrfTask_Params *params)
{
    cb_tsn_thread_attr_t attr;
    CB_THREAD_T thread;
    int status;
    int32_t result = ENET_SOK;

    if (params == NULL)
    {
        UB_LOG(UBL_ERROR, "%s: NULL params\r\n", __func__);
        result = ENET_EFAIL;
    }

    else if (params->tsPerAvtpdu > CLOCK_REC_MAX_TS_PER_AVTPDU)
    {
        UB_LOG(UBL_ERROR, "%s: tsPerAvtpdu %u exceeds CLOCK_REC_MAX_TS_PER_AVTPDU (%u)\r\n",
               __func__, (unsigned int)params->tsPerAvtpdu,
               (unsigned int)CLOCK_REC_MAX_TS_PER_AVTPDU);
        result = ENET_EFAIL;
    }
    else
    {
        CrfTask_params = *params;
        CrfTask_tsPerAvtpdu = (params->tsPerAvtpdu != 0U) ?
                              params->tsPerAvtpdu : CLOCK_REC_DEFAULT_TS_PER_AVTPDU;

        (void)cb_tsn_thread_attr_init(&attr,
            (CrfTask_params.taskPriority != 0U) ? (int)CrfTask_params.taskPriority : (int)CRF_TASK_PRIORITY_DEFAULT,
            (CrfTask_params.taskStackSize != 0U) ? (int)CrfTask_params.taskStackSize : (int)CRF_TASK_STACK_SIZE_DEFAULT,
            "CrfTask");

        if (CrfTask_params.taskStack != NULL)
        {
            (void)cb_tsn_thread_attr_set_stackaddr(&attr, CrfTask_params.taskStack);
        }

        status = CB_THREAD_CREATE(&thread, &attr, CrfTask_main, &CrfTask_params);
        if (ub_assert_fatal(status == 0, __func__, "CB_THREAD_CREATE failed"))
        {
            result = ENET_EFAIL;
        }
    }

    return result;
}

/* --- ClockRec_* transport-facing hooks --- */

void ClockRec_crfTsReadyHook(const uint64_t *ts, uint32_t count)
{
    uint8_t tsBuffer[CLOCK_REC_MAX_TS_PER_AVTPDU * 8U];
    int serializedLen;
    /* Logged (rate-limited to 1Hz, since this can otherwise fire once per
     * 6-sample buffer fill), buffer discarded, no retransmit. Shared
     * throttle covers both failure branches below so a persistent failure
     * can't flood the log. */
    static uint64_t lastLogTimeUs;
    uint64_t nowUs = ClockP_getTimeUsec();
    bool canLog = (nowUs - lastLogTimeUs) > (uint64_t)UB_SEC_US;

    serializedLen = avbtp_serialize_stdtype_crf_data((uint64_t *)ts, count, tsBuffer, sizeof(tsBuffer));
    if (serializedLen <= 0)
    {
        if (canLog)
        {
            UB_LOG(UBL_ERROR, "%s: serialization failed\r\n", __func__);
            lastLogTimeUs = nowUs;
        }
        return;
    }

    if (avtpc_crf_write_packet(CrfTask_avtpcCrf, NULL, tsBuffer, (uint32_t)serializedLen) < 0)
    {
        if (canLog)
        {
            UB_LOG(UBL_INFO, "%s: sending CRF frame failed\r\n", __func__);
            lastLogTimeUs = nowUs;
        }
    }
}

