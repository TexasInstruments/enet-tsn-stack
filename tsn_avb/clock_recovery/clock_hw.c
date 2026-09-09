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

#include <stddef.h>
#include <stdint.h>
#include <enet_mcm.h>
#include <enet_apputils_k3.h>
#include <drivers/soc.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/DebugP.h>
#include "clock_hw.h"

#if defined(SOC_AM275X)
#include <drivers/hw_include/am275x/cslr_soc.h>
#else
#error "Not Supported for this SOC"
#endif

#ifndef UB_LOGCAT
#define UB_LOGCAT 5
#endif
#include <tsn_unibase/unibase.h>

extern int32_t Board_MuxSelPhyRefClk(void);
extern int32_t Board_CdceConfig(void);
extern int32_t Board_CdceFineTuneFreq(double relPPM);

/* ========================================================================== */
/*                              Global Variables                              */
/* ========================================================================== */

/* CPTS HW push -> the time-sync router output line that feeds it. */
typedef struct
{
    CpswCpts_HwPush push;
    uint32_t        routerOut;
} ClockHw_PushMap;

static const ClockHw_PushMap ClockHw_pushMap[] =
{
    { CPSW_CPTS_HWPUSH_1, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_1 },
    { CPSW_CPTS_HWPUSH_2, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_2 },
    { CPSW_CPTS_HWPUSH_3, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_3 },
    { CPSW_CPTS_HWPUSH_4, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_4 },
    { CPSW_CPTS_HWPUSH_5, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_5 },
    { CPSW_CPTS_HWPUSH_6, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_6 },
    { CPSW_CPTS_HWPUSH_7, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_7 },
    { CPSW_CPTS_HWPUSH_8, TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_8 },
};

static struct
{
    Enet_Handle hEnet;
    uint32_t coreId;
    uint64_t parentClockFreq;
    ClockHw_ClkSrc clkSrc;
    const ClockHw_PushMap *mcPush;   /* media clock signal; NULL until ClockHw_configPush() */
    const ClockHw_PushMap *tsPush;   /* timestamping signal; NULL until ClockHw_configPush() */
    ClockHw_PushEventCb tssCb;
    void *tssCbArg;
    ClockHw_PushEventCb mcsCb;
    void *mcsCbArg;
    double cumulativeFreqPpm;   /* total ppm applied so far, adjFreq() is absolute */
} ClockHw_state = {0};

static const uint32_t ClockHw_mcaspCfgBaseAddr[] =
{
    CSL_MCASP0_CFG_BASE,
    CSL_MCASP1_CFG_BASE,
    CSL_MCASP2_CFG_BASE,
    CSL_MCASP3_CFG_BASE,
    CSL_MCASP4_CFG_BASE,
};

static const uint32_t ClockHw_mcaspAfsxMux[] =
{
    ATL_BWSX_MCASP0_AFSX_IN,
    ATL_BWSX_MCASP1_AFSX_IN,
    ATL_BWSX_MCASP2_AFSX_IN,
    ATL_BWSX_MCASP3_AFSX_IN,
    ATL_BWSX_MCASP4_AFSX_IN,
};

static const uint32_t ClockHw_mcaspAfsrMux[] =
{
    ATL_BWSX_MCASP0_AFSR_IN,
    ATL_BWSX_MCASP1_AFSR_IN,
    ATL_BWSX_MCASP2_AFSR_IN,
    ATL_BWSX_MCASP3_AFSR_IN,
    ATL_BWSX_MCASP4_AFSR_IN,
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static void ClockHw_setAtlMux(uint32_t atlBwsReg, uint32_t muxVal)
{
    volatile uint32_t *ctrlMmrAddr =
        (volatile uint32_t*)AddrTranslateP_getLocalAddr(atlBwsReg);

    /* Control MMRs are write protected, so unlock just around the write. */
    SOC_controlModuleUnlockMMR(SOC_DOMAIN_ID_MAIN, 2);
    *ctrlMmrAddr = muxVal;
    SOC_controlModuleLockMMR(SOC_DOMAIN_ID_MAIN, 2);
}

static uint64_t ClockHw_getCptsHwPushEventTs(CpswCpts_HwPush hwPushNum)
{
    CpswCpts_Event inArgs =
    {
        .eventType = CPSW_CPTS_EVENTTYPE_HW_TS_PUSH,
        .hwPushNum = hwPushNum,
    };
    CpswCpts_Event outArgs;
    Enet_IoctlPrms prms;
    uint64_t timestamp = 0;
    int32_t status = ENET_SOK;

    ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &outArgs);
    ENET_IOCTL(ClockHw_state.hEnet, ClockHw_state.coreId, CPSW_CPTS_IOCTL_LOOKUP_EVENT, &prms, status);
    if (status != ENET_SOK)
    {
        UB_LOG(UBL_ERROR, "ClockHw: No timestamp found for HWPUSH%d\r\n", (int)hwPushNum);
    }
    else
    {
        timestamp = outArgs.tsVal;
    }

    return timestamp;
}

/* Both notify callbacks below run in CPTS interrupt context, keep them short. */
static void ClockHw_tssPushNotifyCb(void *cbArg, CpswCpts_HwPush hwPushNum, uint64_t tsVal)
{
    if (ClockHw_state.tssCb != NULL)
    {
        ClockHw_state.tssCb(ClockHw_state.tssCbArg, tsVal);
    }
}

static void ClockHw_mcsPushNotifyCb(void *cbArg, CpswCpts_HwPush hwPushNum, uint64_t tsVal)
{
    if (ClockHw_state.mcsCb != NULL)
    {
        ClockHw_state.mcsCb(ClockHw_state.mcsCbArg, tsVal);
    }
}

static int32_t ClockHw_registerCptsPushEvent(CpswCpts_HwPush pushNum,
                                             CpswCpts_HwPushNotifyCb cb)
{
    CpswCpts_RegisterHwPushCbInArgs inArgs;
    Enet_IoctlPrms prms;
    int32_t status = ENET_SOK;

    inArgs.hwPushNum = pushNum;
    inArgs.hwPushNotifyCb = cb;
    inArgs.hwPushNotifyCbArg = NULL;

    ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
    ENET_IOCTL(ClockHw_state.hEnet, ClockHw_state.coreId, CPSW_CPTS_IOCTL_REGISTER_HWPUSH_CALLBACK, &prms, status);
    if (status != ENET_SOK)
    {
        UB_LOG(UBL_ERROR, "[Error] ClockHw: Failed to register HWPUSH callback!!!\r\n");
    }

    return status;
}

static int32_t ClockHw_unregisterCptsPushEvent(CpswCpts_HwPush pushNum)
{
    Enet_IoctlPrms prms;
    int32_t status = ENET_SOK;

    ENET_IOCTL_SET_IN_ARGS(&prms, &pushNum);
    ENET_IOCTL(ClockHw_state.hEnet, ClockHw_state.coreId, CPSW_CPTS_IOCTL_UNREGISTER_HWPUSH_CALLBACK, &prms, status);
    if (status != ENET_SOK)
    {
        UB_LOG(UBL_ERROR, "[Error] ClockHw: Failed to unregister HWPUSH callback!!!\r\n");
    }

    return status;
}

/* Resolves an McASP CFG base address to its ATL_BWSX_MCASPn_AFSX/AFSR_IN mux
 * code. TODO: confirm AFSX(audio)/AFSR(TS) role mapping with board owner. */
static int32_t ClockHw_mcaspBaseToMux(int32_t mcaspBaseAddress,
                                       const uint32_t *muxTable,
                                       uint32_t *outMuxVal)
{
    size_t i;
    int32_t status = ENET_EFAIL;

    for (i = 0; i < (sizeof(ClockHw_mcaspCfgBaseAddr) / sizeof(ClockHw_mcaspCfgBaseAddr[0])); i++)
    {
        if (ClockHw_mcaspCfgBaseAddr[i] == (uint32_t)mcaspBaseAddress)
        {
            *outMuxVal = muxTable[i];
            status = ENET_SOK;
            break;
        }
    }

    if (status != ENET_SOK)
    {
        UB_LOG(UBL_ERROR, "[Error] ClockHw: Unknown McASP base address 0x%x\r\n", mcaspBaseAddress);
    }

    return status;
}

static const ClockHw_PushMap *ClockHw_findPush(CpswCpts_HwPush push)
{
    const ClockHw_PushMap *entry = NULL;
    size_t i;

    for (i = 0; i < (sizeof(ClockHw_pushMap) / sizeof(ClockHw_pushMap[0])); i++)
    {
        if (ClockHw_pushMap[i].push == push)
        {
            entry = &ClockHw_pushMap[i];
            break;
        }
    }

    return entry;
}

int32_t ClockHw_configPush(CpswCpts_HwPush mcPush, CpswCpts_HwPush tsPush)
{
    const ClockHw_PushMap *mcEntry = ClockHw_findPush(mcPush);
    const ClockHw_PushMap *tsEntry = ClockHw_findPush(tsPush);
    int32_t status = ENET_SOK;

    if ((mcEntry == NULL) || (tsEntry == NULL))
    {
        UB_LOG(UBL_ERROR, "ClockHw: unsupported HW push (mc=%d, ts=%d)\r\n",
               (int)mcPush, (int)tsPush);
        status = ENET_EFAIL;
    }
    else if (mcEntry == tsEntry)
    {
        UB_LOG(UBL_ERROR, "ClockHw: media clock and timestamping cannot share HW push %d\r\n",
               (int)mcPush);
        status = ENET_EFAIL;
    }
    else
    {
        ClockHw_state.mcPush = mcEntry;
        ClockHw_state.tsPush = tsEntry;
    }

    return status;
}

int32_t ClockHw_configClock(uint64_t parentClockFrequency, ClockHw_ClkSrc clkSrc)
{
    EnetApp_HandleInfo handleInfo;
    int32_t status;

    ClockHw_state.coreId = EnetSoc_getCoreId();
    EnetApp_acquireHandleInfo(ENET_CPSW_3G, 0, &handleInfo);
    DebugP_assert(handleInfo.hEnet != NULL);
    ClockHw_state.hEnet = handleInfo.hEnet;

    ClockHw_state.parentClockFreq = parentClockFrequency;
    ClockHw_state.clkSrc = clkSrc;

    if (clkSrc == CLOCK_HW_CLKSRC_PHY)
    {
        status = Board_MuxSelPhyRefClk();
    }
    else
    {
        /* The Board mux is selected */
        status = Board_CdceConfig();
        ClockHw_state.cumulativeFreqPpm = 0.0;
    }

    return status;
}

int32_t ClockHw_configAtlMux(int32_t audioMcaspBaseAddress,
                              ClockHw_McaspDir audioMcaspDir,
                              int32_t tsMcaspBaseAddress,
                              ClockHw_McaspDir tsMcaspDir)
{
    uint32_t audioMux;
    uint32_t tsMux;
    int32_t status;
    const uint32_t *audioMuxTable =
        (audioMcaspDir == CLOCK_HW_MCASP_DIR_RX) ?
            ClockHw_mcaspAfsrMux : ClockHw_mcaspAfsxMux;
    const uint32_t *tsMuxTable =
        (tsMcaspDir == CLOCK_HW_MCASP_DIR_RX) ?
            ClockHw_mcaspAfsrMux : ClockHw_mcaspAfsxMux;

    status = ClockHw_mcaspBaseToMux(audioMcaspBaseAddress, audioMuxTable, &audioMux);
    if (status == ENET_SOK)
    {
        status = ClockHw_mcaspBaseToMux(tsMcaspBaseAddress, tsMuxTable, &tsMux);
    }

    if (status == ENET_SOK)
    {
        /* Media clock is always ATL BWS1, timestamping always BWS2; which CPTS
         * HW push each one reaches is chosen at the time-sync router. */
        ClockHw_setAtlMux(CTRL_MMR0_CFG0_ATL_BWS1_SEL, audioMux);
        ClockHw_setAtlMux(CTRL_MMR0_CFG0_ATL_BWS2_SEL, tsMux);
    }

    return status;
}

/* Enables/disables the time-sync router route from an ATL BWS input to the
 * router output of the chosen CPTS HW push. */
static int32_t ClockHw_tsrSignal(uint32_t routerIn, const ClockHw_PushMap *push, int32_t state)
{
    int32_t status = ENET_EFAIL;

    if (push == NULL)
    {
        UB_LOG(UBL_ERROR, "%s: ClockHw_configPush() has not been called\r\n", __func__);
    }
    else if (state != 0)
    {
        status = EnetAppUtils_setTimeSyncRouter(ENET_CPSW_3G, 0U, routerIn, push->routerOut);
    }
    else
    {
        status = EnetAppUtils_releaseTimeSyncRouter(ENET_CPSW_3G, 0U, routerIn, push->routerOut);
    }

    return status;
}

int32_t ClockHw_tsrTsSignal(int32_t state)
{
    return ClockHw_tsrSignal(TS_ROUTER_IN_ATL_BWS_SEL_2, ClockHw_state.tsPush, state);
}

int32_t ClockHw_tsrMcSignal(int32_t state)
{
    return ClockHw_tsrSignal(TS_ROUTER_IN_ATL_BWS_SEL_1, ClockHw_state.mcPush, state);
}

int32_t ClockHw_registerTssPushEventCb(ClockHw_PushEventCb cb, void *arg)
{
    int32_t status = ENET_EFAIL;

    if (ClockHw_state.tsPush != NULL)
    {
        ClockHw_state.tssCb = cb;
        ClockHw_state.tssCbArg = arg;
        status = ClockHw_registerCptsPushEvent(ClockHw_state.tsPush->push, ClockHw_tssPushNotifyCb);
    }

    return status;
}

int32_t ClockHw_registerMcsPushEventCb(ClockHw_PushEventCb cb, void *arg)
{
    int32_t status = ENET_EFAIL;

    if (ClockHw_state.mcPush != NULL)
    {
        ClockHw_state.mcsCb = cb;
        ClockHw_state.mcsCbArg = arg;
        status = ClockHw_registerCptsPushEvent(ClockHw_state.mcPush->push, ClockHw_mcsPushNotifyCb);
    }

    return status;
}

int32_t ClockHw_unregisterTssPushEventCb(void)
{
    int32_t status = ENET_EFAIL;

    if (ClockHw_state.tsPush != NULL)
    {
        status = ClockHw_unregisterCptsPushEvent(ClockHw_state.tsPush->push);
    }

    ClockHw_state.tssCb = NULL;
    ClockHw_state.tssCbArg = NULL;

    return status;
}

int32_t ClockHw_unregisterMcsPushEventCb(void)
{
    int32_t status = ENET_EFAIL;

    if (ClockHw_state.mcPush != NULL)
    {
        status = ClockHw_unregisterCptsPushEvent(ClockHw_state.mcPush->push);
    }

    ClockHw_state.mcsCb = NULL;
    ClockHw_state.mcsCbArg = NULL;

    return status;
}

uint64_t ClockHw_getTsTimestamp(void)
{
    return (ClockHw_state.tsPush != NULL) ?
           ClockHw_getCptsHwPushEventTs(ClockHw_state.tsPush->push) : 0U;
}

uint64_t ClockHw_getMcTimestamp(void)
{
    return (ClockHw_state.mcPush != NULL) ?
           ClockHw_getCptsHwPushEventTs(ClockHw_state.mcPush->push) : 0U;
}

int32_t ClockHw_adjFreq(int32_t ppb)
{
    int32_t status = ENET_EFAIL;

    if (ClockHw_state.clkSrc == CLOCK_HW_CLKSRC_CDCE)
    {
        /* The caller gives an absolute offset from nominal, the board call only
         * takes a delta, so subtract what we already applied. */
        double absPpm = (double)ppb / 1000.0;
        double relPpm = absPpm - ClockHw_state.cumulativeFreqPpm;

        status = Board_CdceFineTuneFreq(relPpm);
        if (status == ENET_SOK)
        {
            ClockHw_state.cumulativeFreqPpm = absPpm;
        }
    }
    else
    {
        UB_LOG(UBL_WARN, "%s: Not Supported\r\n", __func__);
    }

    return status;
}

int32_t ClockHw_adjPhaseNs(int32_t count)
{
    int32_t status = ENET_EFAIL;

    if (ClockHw_state.clkSrc != CLOCK_HW_CLKSRC_PHY)
    {
        UB_LOG(UBL_WARN, "%s: Not Supported\r\n", __func__);
    }
    else if ((count < (int32_t)INT8_MIN) || (count > (int32_t)INT8_MAX))
    {
        /* The PHY nudge field is 8-bit signed; refuse instead of wrapping. */
        UB_LOG(UBL_ERROR, "%s: nudge %d out of int8 range\r\n", __func__, (int)count);
    }
    else
    {
        EnetPhy_NudgeCodecClockInArgs inArgs =
        {
            .macPort = ENET_MAC_PORT_1,   /* PHY on this port provides the codec clock */
            .nudgeValue = (int8_t)count,
        };
        Enet_IoctlPrms prms;

        ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
        ENET_IOCTL(ClockHw_state.hEnet, ClockHw_state.coreId, ENET_PHY_IOCTL_NUDGE_CODEC_CLOCK, &prms, status);
    }

    return status;
}
