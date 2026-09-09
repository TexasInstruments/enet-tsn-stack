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
 * \file   crf_task.h
 *
 * \brief Transport layer for ClockRec_*: AVTP/CRF registration against the
 * private eval_src Excelfore stack, the ClockRec_*<->task hand-off, talker
 * serialize/transmit, and the RTOS task that drives all of it.
 */

#ifndef CRF_TASK_H_
#define CRF_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "avtpc_crf.h"
#include "clock_rec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defaults, overridable per-instance via CrfTask_Params.
 * CRF_TASK_PRIORITY_DEFAULT is deliberately higher priority (lower number)
 * than AVB_AUDIO_TASK_PRIORITY (16 in demo_task.c) - avoids priority
 * inversion against this timing-sensitive tick-processing task. */
#define CRF_TASK_PRIORITY_DEFAULT    (8U)
#define CRF_TASK_STACK_SIZE_DEFAULT  (8192U)

typedef struct
{
    /* --- passed through to ClockRec_init(), see clock_rec.h --- */
    ClockRec_Role   role;
    ClockRec_Source clkSrc;
    uint32_t        correctionFlags;
    uint64_t        parentClockFrequency;
    uint32_t        nominalFrequency;
    uint32_t        tsInterval;
    uint32_t        tsPerAvtpdu;         /* 0 => CLOCK_REC_DEFAULT_TS_PER_AVTPDU (6), see clock_rec.h */
    int32_t          audioMcaspBaseAddress;
    ClockHw_McaspDir audioMcaspDir;
    int32_t          tsMcaspBaseAddress;
    ClockHw_McaspDir tsMcaspDir;
    CpswCpts_HwPush  mcHwPush;           /* 0 => CPSW_CPTS_HWPUSH_1, see clock_rec.h */
    CpswCpts_HwPush  tsHwPush;           /* 0 => CPSW_CPTS_HWPUSH_2, see clock_rec.h */
    /* NULL => ClockRec_getDefaultTuning() values. CrfTask_create() copies
     * only this pointer, and ClockRec_init() runs later inside the task, so
     * the pointed-to struct must stay valid until the first tick is
     * processed - use static storage (a const global works). */
    const ClockRec_Tuning *tuning;

    /* --- AVTP/CRF transport identity: passed to avtpc_crf_config_t
     * (eval_src). ClockRec_* never sees any of these. --- */
    const char     *netdev;              /* e.g. "tilld0" */
    uint8_t         streamId[8];
    const char     *destMcast;           /* e.g. "91:E0:F0:00:FE:00" */
    const char     *joinMcast;
    uint16_t        vlanId;
    uint8_t         vlanPcp;
    avbtp_crf_pull_t pull;               /* CRF header pull field (talker only);
                                          * 0 == AVBTP_CRF_PULL_MULT_1, so a
                                          * zero-initialized struct keeps the
                                          * old behaviour */

    /* --- task configuration: forwarded into a tsn_combase
     * cb_tsn_thread_attr_t (pri/stack_addr/stack_size) via CB_THREAD_CREATE.
     * Not a TaskP_Params; crf_task.c never includes kernel/dpl/TaskP.h
     * directly. --- */
    uint32_t        taskPriority;        /* 0 => CRF_TASK_PRIORITY_DEFAULT */
    uint8_t        *taskStack;           /* NULL => tsn_combase's own static
                                           * stack pool, shared with any
                                           * other CB_THREAD_CREATE caller */
    uint32_t        taskStackSize;       /* 0 => CRF_TASK_STACK_SIZE_DEFAULT */

    /* When false (default), CrfTask_create() registers its own independent
     * avtpc_crf rx-direct socket for the listener role. When true, that
     * registration is skipped - the application already owns a shared AVTP
     * rx callback and must call CrfTask_rxCallback() itself on
     * AVBTP_SUBTYPE_CRF packets. Listener/loopback roles only; ignored for
     * a pure talker. */
    bool            skipRxRegistration;
} CrfTask_Params;

/**
 * \brief Creates and starts the CRF transport task: waits for gPTP and link
 * readiness, registers with the AVTP/CRF stack, calls ClockRec_init(), then
 * runs the steady-state loop for the life of the application.
 *
 * Failures after task creation are fatal (ub_assert_fatal, tsn_unibase) -
 * this function does not report runtime status back to the caller; use
 * ClockRec_getSyncStatus()/getDiagCounters() for that.
 *
 * \param[in] params Configuration, see CrfTask_Params. Copied internally;
 * the caller's struct does not need to outlive this call. Only one
 * CrfTask instance is supported per core - the strings pointed to by
 * netdev/destMcast/joinMcast, however, DO need to outlive this call (only
 * the containing struct is copied, not the strings themselves - pass
 * string literals or otherwise static storage).
 * \return ENET_SOK on successful task construction, ENET_EFAIL on invalid
 * params or CB_THREAD_CREATE() (cb_lld_task_create()) failure.
 */
int32_t CrfTask_create(const CrfTask_Params *params);

/**
 * \brief AVTP rx callback for CRF (Clock Reference Format) subtype packets:
 * media-clock-restart bit -> ClockRec_restart(), else deserialize and
 * ClockRec_submitCrfTs(). Listener/loopback roles only.
 *
 * Only call this directly when CrfTask_Params.skipRxRegistration was
 * true at CrfTask_create() - otherwise CrfTask already registered this
 * same function against its own independent avtpc_crf rx-direct socket via
 * avtpc_crf_set_rcv_cb(), and calling it a second time double-processes
 * every CRF packet. With skipRxRegistration true, the application owns
 * the single shared AVTP rx callback for this netdev and must call this
 * function itself whenever cbinfo->u.rcrfinfo.subtype == AVBTP_SUBTYPE_CRF.
 *
 * \return ENET_SOK on success, ENET_EFAIL on subtype mismatch or
 * deserialization failure.
 */
int CrfTask_rxCallback(uint8_t *payload, int payload_size,
                               avbtp_rcv_cb_info_t *cbinfo, void *cbdata);

#ifdef __cplusplus
}
#endif

#endif /* CRF_TASK_H_ */
