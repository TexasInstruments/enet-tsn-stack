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
 * \file   clock_rec.c
 *
 * \brief ClockRec_init/deInit/restart, role dispatch, ClockHw_* wiring, the
 * edge-diff bootstrap, and the ISR-safe tick shim / ClockRec_processTick()
 * hand-off. See CLOCK_REC_DESIGN.md \S5, \S6.1, \S9, \S13.2.
 */

#include <math.h>
#include <string.h>
#include <time.h>
#include <tsn_combase/cb_thread.h>
#include "clock_hw.h"
#include "clock_rec.h"
#include "clock_rec_priv.h"
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/HwiP.h>
#include <enet_types.h>

#ifndef UB_LOGCAT
#define UB_LOGCAT 5
#endif
#include <tsn_unibase/unibase.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* A consecutive sample pair is "good" when its gap is within this percent
 * of that signal's own nominal period. */
#define CLOCK_REC_BOOTSTRAP_JITTER_PCT    (10U)

/* Samples captured per signal during bootstrap. */
#define CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT  (20U)

/* Built-in ClockRec_Tuning defaults - the values used before these became
 * configurable. See ClockRec_getDefaultTuning(). */
#define CLOCK_REC_DEFAULT_IIR_ALPHA              (0.05)
#define CLOCK_REC_DEFAULT_FREQ_KP_AGGRESSIVE     (0.9)
#define CLOCK_REC_DEFAULT_FREQ_KP_FINE           (0.05)
#define CLOCK_REC_DEFAULT_FREQ_KD_FINE           (0.001)
#define CLOCK_REC_DEFAULT_FREQ_KI                (0.05)
#define CLOCK_REC_DEFAULT_DCO_MAX_PPM            (1000.0)
#define CLOCK_REC_DEFAULT_DCO_MIN_WRITE_PPM      (0.01)
#define CLOCK_REC_DEFAULT_FREQ_ERR_THRESHOLD_PPM (3.0)
#define CLOCK_REC_DEFAULT_PHASE_KP_AGGRESSIVE    (0.004)
#define CLOCK_REC_DEFAULT_PHASE_KP_FINE          (0.0002)
#define CLOCK_REC_DEFAULT_PHASE_ERR_THRESHOLD_NS (200.0)
#define CLOCK_REC_DEFAULT_CORRECTION_PERIOD_US   (100000U)
/* Max wait for the Mcs/Tss bootstrap samples (20 Mcs edges at the media-clock
 * rate, 20 Tss events at the Tss rate) before the bootstrap is failed instead
 * of blocking the task forever on a source that never starts. */
#define CLOCK_REC_DEFAULT_BOOTSTRAP_TIMEOUT_MS   (2000U)
#define CLOCK_REC_DEFAULT_MAX_TRANSIT_TIME_NS    (2000000ULL)

/* Max wait for the next Tss tick in ClockRec_processTick() before it returns
 * ENET_EFAIL, so a stopped Tss source is reported instead of blocking forever. */
#define CLOCK_REC_TICK_TIMEOUT_MS         (1000U)

typedef struct
{
    ClockRec_Priv_t   *inst;
    uint64_t          mcTs[CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT];
    uint64_t          tsTs[CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT];
    volatile uint32_t          mcsCount;
    volatile uint32_t          tssCount;
    CB_SEM_T          mcsDoneSem;
    CB_SEM_T          tssDoneSem;
} ClockRec_BootstrapState;

/* Static storage: ClockRec_bootstrapTssCb() stays registered with
 * ClockHw_* for the life of the application, so this can't be
 * stack-local. Reset via memset() at the top of every
 * ClockRec_runHwBootstrap() call. */
ClockRec_BootstrapState ClockRec_bootstrapState;

/* ========================================================================== */
/*                              Global Variables                              */
/* ========================================================================== */

ClockRec_Priv_t ClockRec_priv = {0};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void ClockRec_bootstrapMcsCb(void *arg, uint64_t tsVal);
static void ClockRec_bootstrapTssCb(void *arg, uint64_t tsVal);
static int32_t ClockRec_findGoodSampleIdx(const uint64_t *ts, uint64_t periodNs);
static int64_t ClockRec_computeEdgeDiff(const uint64_t *mcTs, const uint64_t *tsTs,
                                         double mcFreqHz, double tsFreqHz);
static bool ClockRec_tuningValid(const ClockRec_Tuning *t);
static int32_t ClockRec_waitBootstrapSem(CB_SEM_T *sem);
static int32_t ClockRec_runHwBootstrap(ClockRec_Priv_t *inst);
static void ClockRec_executeRestart(ClockRec_Priv_t *inst);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* Bootstrap-only Mcs push callback - records the edge timestamp at push
 * time (ISR context) into ClockRec_bootstrapState.mcTs. */
static void ClockRec_bootstrapMcsCb(void *arg, uint64_t tsVal)
{
    ClockRec_BootstrapState *bs = (ClockRec_BootstrapState *)arg;

    if (bs->mcsCount < CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT)
    {
        bs->mcTs[bs->mcsCount] = tsVal;
        bs->mcsCount++;
        if (bs->mcsCount == CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT)
        {
            CB_SEM_POST(&bs->mcsDoneSem);
        }
    }
}

/* Permanent, steady-state Tss push callback (ISR context): captures the
 * bootstrap samples, then posts inst->tickSem every push - the minimal
 * ISR-safe shim design \S13.2 requires. */
static void ClockRec_bootstrapTssCb(void *arg, uint64_t tsVal)
{
    ClockRec_BootstrapState *bs = (ClockRec_BootstrapState *)arg;

    if (bs->tssCount < CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT)
    {
        bs->tsTs[bs->tssCount] = tsVal;
        bs->tssCount++;
        if (bs->tssCount == CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT)
        {
            CB_SEM_POST(&bs->tssDoneSem);
        }
    }

    /* Keep (event count, timestamp) of the latest Tss push together, so the
     * edge count <-> time helpers always read a matching pair. */
    uintptr_t key = HwiP_disable();
    ClockRec_priv.eventCounter += 1;
    ClockRec_priv.eventTs = tsVal;
    HwiP_restore(key);

    SemaphoreP_post(&ClockRec_priv.tickSem);
}

/* First sample index whose gap to the next sample is within
 * CLOCK_REC_BOOTSTRAP_JITTER_PCT of periodNs, or -1 if none qualify. */
static int32_t ClockRec_findGoodSampleIdx(const uint64_t *ts, uint64_t periodNs)
{
    int32_t result = -1;
    uint64_t toleranceNs = (periodNs * CLOCK_REC_BOOTSTRAP_JITTER_PCT) / 100U;
    uint32_t i;

    for (i = 0U; i < (CLOCK_REC_BOOTSTRAP_SAMPLE_COUNT - 1U); i++)
    {
        if (ClockRec_absDiffU64((int64_t)(ts[i + 1U] - ts[i]), (int64_t)periodNs) < toleranceNs)
        {
            result = (int32_t)i;
            break;
        }
    }

    return result;
}

/* Offset between the Tss edge and the media-clock edge: find each
 * signal's own first good sample independently, then reduce the raw
 * difference modulo one media-clock period - the two samples aren't
 * guaranteed to land within one media-clock period of each other before
 * this reduction. Same cycles/floor()/fold-into-(-period/2, period/2]
 * pattern as ClockRecListener_updatePhaseError(). */
static int64_t ClockRec_computeEdgeDiff(const uint64_t *mcTs, const uint64_t *tsTs,
                                         double mcFreqHz, double tsFreqHz)
{
    int64_t result = 0;
    uint64_t mcPeriodNs = (uint64_t)(1e9 / mcFreqHz);
    uint64_t tsPeriodNs = (uint64_t)(1e9 / tsFreqHz);
    int32_t  mcIdx = ClockRec_findGoodSampleIdx(mcTs, mcPeriodNs);
    int32_t  tsIdx = ClockRec_findGoodSampleIdx(tsTs, tsPeriodNs);

    /* A failed search returns 0, which the caller treats as "bootstrap failed". */
    if ((mcIdx < 0) || (tsIdx < 0))
    {
        UB_LOG(UBL_ERROR, "%s: could not find a consistent Mcs/Tss sample pair\r\n", __func__);
        result = 0;
    }
    else
    {
        result = (int64_t)(tsTs[tsIdx] - mcTs[mcIdx]);
    }

    return result;
}

void ClockRec_getDefaultTuning(ClockRec_Tuning *out)
{
    if (out != NULL)
    {
        out->iirAlpha            = CLOCK_REC_DEFAULT_IIR_ALPHA;
        out->freqKpAggressive    = CLOCK_REC_DEFAULT_FREQ_KP_AGGRESSIVE;
        out->freqKpFine          = CLOCK_REC_DEFAULT_FREQ_KP_FINE;
        out->freqKdFine          = CLOCK_REC_DEFAULT_FREQ_KD_FINE;
        out->freqKi              = CLOCK_REC_DEFAULT_FREQ_KI;
        out->dcoMaxPpm           = CLOCK_REC_DEFAULT_DCO_MAX_PPM;
        out->dcoMinWritePpm      = CLOCK_REC_DEFAULT_DCO_MIN_WRITE_PPM;
        out->freqErrThresholdPpm = CLOCK_REC_DEFAULT_FREQ_ERR_THRESHOLD_PPM;
        out->phaseKpAggressive   = CLOCK_REC_DEFAULT_PHASE_KP_AGGRESSIVE;
        out->phaseKpFine         = CLOCK_REC_DEFAULT_PHASE_KP_FINE;
        out->phaseErrThresholdNs = CLOCK_REC_DEFAULT_PHASE_ERR_THRESHOLD_NS;
        out->correctionPeriodUs  = CLOCK_REC_DEFAULT_CORRECTION_PERIOD_US;
        out->bootstrapTimeoutMs  = CLOCK_REC_DEFAULT_BOOTSTRAP_TIMEOUT_MS;
        out->maxTransitTimeNs    = CLOCK_REC_DEFAULT_MAX_TRANSIT_TIME_NS;
    }
}

/* Rejects values that would divide by zero or invert a threshold pair
 * (freqKpFine and freqKi are divisors in the freq PID). */
static bool ClockRec_tuningValid(const ClockRec_Tuning *t)
{
    return (t->iirAlpha > 0.0) && (t->iirAlpha <= 1.0) &&
           (t->freqKpAggressive >= 0.0) &&
           (t->freqKpFine > 0.0) &&
           (t->freqKdFine >= 0.0) &&
           (t->freqKi > 0.0) &&
           (t->dcoMaxPpm > 0.0) &&
           (t->dcoMinWritePpm >= 0.0) && (t->dcoMinWritePpm < t->dcoMaxPpm) &&
           (t->freqErrThresholdPpm > 0.0) &&
           (t->phaseKpAggressive >= 0.0) &&
           (t->phaseKpFine >= 0.0) &&
           (t->phaseErrThresholdNs > 0.0) &&
           (t->correctionPeriodUs > 0U) &&
           (t->bootstrapTimeoutMs > 0U) &&
           (t->maxTransitTimeNs > 0U);
}

/* Waits on a bootstrap semaphore for at most tuning.bootstrapTimeoutMs.
 * The deadline is absolute on the same realtime clock CB_SEM_TIMEDWAIT uses. */
static int32_t ClockRec_waitBootstrapSem(CB_SEM_T *sem)
{
    uint64_t deadlineNs = ub_rt_gettime64() +
                          ((uint64_t)ClockRec_priv.tuning.bootstrapTimeoutMs * (uint64_t)UB_MSEC_NS);
    struct timespec abstime;

    abstime.tv_sec  = (time_t)(deadlineNs / (uint64_t)UB_SEC_NS);
    abstime.tv_nsec = (long)(deadlineNs % (uint64_t)UB_SEC_NS);

    return (CB_SEM_TIMEDWAIT(sem, &abstime) == 0) ? ENET_SOK : ENET_EFAIL;
}

/*
 * Full hardware bootstrap: ClockHw_configClock/configAtlMux, enables the
 * Tss time-sync router route, wires up the push callbacks. Used by both
 * ClockRec_init() and ClockRec_restart().
 */
static int32_t ClockRec_runHwBootstrap(ClockRec_Priv_t *inst)
{
    int64_t edgeDiff = 0;
    int32_t status;

    /* Tss stays registered for the life of the application once bootstrap
     * completes (it doubles as the permanent tick-ISR shim - see the
     * comment on ClockRec_bootstrapState above), so on a restart it is
     * still actively firing from ISR context right up until this point -
     * unlike Mcs, which is already guaranteed unregistered by now
     * (unregistered at the end of the previous run - see
     * ClockHw_unregisterMcsPushEventCb() below). Unregister it before the
     * memset so the ISR can't write into ClockRec_bootstrapState
     * concurrently with the reset. Skipped on the first-ever bootstrap
     * (inst->tssCallbackActive still false - nothing registered yet to
     * unregister); both callbacks are re-registered below either way. */
    if (inst->tssCallbackActive)
    {
        (void)ClockHw_unregisterTssPushEventCb();
    }

    memset(&ClockRec_bootstrapState, 0, sizeof(ClockRec_bootstrapState));
    ClockRec_bootstrapState.inst = inst;
    status = CB_SEM_INIT(&ClockRec_bootstrapState.mcsDoneSem, 0, 0);

    status += CB_SEM_INIT(&ClockRec_bootstrapState.tssDoneSem, 0, 0);

    ub_assert_fatal(status == 0, "Sem Init failed\r\n", ":/");

    /* Must precede configAtlMux and the router/callback calls below: they
     * resolve the ATL mux register, router lines and push event from it. */
    if (status == ENET_SOK)
    {
        status = ClockHw_configPush(
            (inst->params.mcHwPush != (CpswCpts_HwPush)0) ? inst->params.mcHwPush : CPSW_CPTS_HWPUSH_1,
            (inst->params.tsHwPush != (CpswCpts_HwPush)0) ? inst->params.tsHwPush : CPSW_CPTS_HWPUSH_2);
    }
    if (status == ENET_SOK)
    {
        status = ClockHw_configClock(inst->params.parentClockFrequency,
                                     (inst->params.clkSrc == CLOCK_REC_SOURCE_PHY) ?
                                     CLOCK_HW_CLKSRC_PHY : CLOCK_HW_CLKSRC_CDCE);
    }
    if (status == ENET_SOK)
    {
        status = ClockHw_configAtlMux(inst->params.audioMcaspBaseAddress, inst->params.audioMcaspDir,
                                       inst->params.tsMcaspBaseAddress, inst->params.tsMcaspDir);
    }
    if (status == ENET_SOK)
    {
        status = ClockHw_tsrMcSignal(1);
    }
    if (status == ENET_SOK)
    {
        status = ClockHw_tsrTsSignal(1);
    }
    if (status == ENET_SOK)
    {
        status = ClockHw_registerMcsPushEventCb(ClockRec_bootstrapMcsCb, &ClockRec_bootstrapState);
    }
    if (status == ENET_SOK)
    {
        status = ClockHw_registerTssPushEventCb(ClockRec_bootstrapTssCb, &ClockRec_bootstrapState);
        if (status == ENET_SOK)
        {
            inst->tssCallbackActive = true;
        }
    }

    if (status != ENET_SOK)
    {
        UB_LOG(UBL_ERROR, "%s: ClockHw_* wiring failed (%d)\r\n", __func__, status);
    }
    else
    {
        /* App starts its McASPs / audio pipeline here so the media clock
         * is actually running by the time edges need to be captured. */
        (void)ClockRec_clockStartedHook();

        status = ClockRec_waitBootstrapSem(&ClockRec_bootstrapState.mcsDoneSem);
        /* Mcs push released here - avoids a 48,000-interrupt/sec flood
         * (design \S3/\S6.1), and on a timeout too so it can't keep firing.
         * Tss push (ClockRec_bootstrapTssCb) is left registered as-is on
         * success; its own tssCount is already at target, so from here on
         * every further push falls into its else-branch and posts
         * inst->tickSem. */
        (void)ClockHw_unregisterMcsPushEventCb();
        (void)ClockHw_tsrMcSignal(0);

        if (status == ENET_SOK)
        {
            status = ClockRec_waitBootstrapSem(&ClockRec_bootstrapState.tssDoneSem);
        }

        if (status != ENET_SOK)
        {
            UB_LOG(UBL_ERROR, "%s: timed out after %u ms waiting for Mcs/Tss bootstrap samples\r\n",
                   __func__, (unsigned int)inst->tuning.bootstrapTimeoutMs);
            /* The sems are destroyed below, so the Tss ISR must not be left
             * able to post tssDoneSem later. */
            (void)ClockHw_unregisterTssPushEventCb();
            inst->tssCallbackActive = false;
        }
        else
        {
            edgeDiff = ClockRec_computeEdgeDiff(ClockRec_bootstrapState.mcTs, ClockRec_bootstrapState.tsTs,
                                                 inst->nMediaClockFreq, inst->nTsFreq);
            if (edgeDiff == 0)
            {
                status = ENET_EFAIL;
            }
            else
            {
                inst->edgeDiff = edgeDiff;
                UB_LOG(UBL_INFO, "Calculated Edge Difference = %lld\r\n", edgeDiff);
            }
        }

    }

    CB_SEM_DESTROY(&ClockRec_bootstrapState.mcsDoneSem);
    CB_SEM_DESTROY(&ClockRec_bootstrapState.tssDoneSem);

    return status;
}

/* ClockRec_restart()'s hard reset: re-run the hardware bootstrap (the
 * edgeDiff offset can shift across a real media-clock restart) and reset
 * algorithm state. Diagnostics counters and the CRF ready queue are left
 * untouched. */
static void ClockRec_executeRestart(ClockRec_Priv_t *inst)
{
    int32_t status;

    UB_LOG(UBL_INFO, "%s: restarting\r\n", __func__);

    status = ClockRec_runHwBootstrap(inst);
    if (status != ENET_SOK)
    {
        UB_LOG(UBL_ERROR, "%s: hardware re-bootstrap failed (%d)\r\n", __func__, status);
        /* Not modeled as fatal - the app can call ClockRec_restart() again. */
    }

    ClockRecListener_resetAlgState(inst);
}

int32_t ClockRec_init(ClockRec_InitParams params)
{
    int32_t status = ENET_EFAIL;

    if (ClockRec_priv.initialized)
    {
        UB_LOG(UBL_WARN, "%s: already initialized\r\n", __func__);
        status = ENET_EFAIL;
    }
    else if ((params.nominalFrequency == 0U) || (params.tsInterval == 0U))
    {
        UB_LOG(UBL_ERROR, "%s: invalid params\r\n", __func__);
        status = ENET_EFAIL;
    }
    else if (params.tsPerAvtpdu > CLOCK_REC_MAX_TS_PER_AVTPDU)
    {
        UB_LOG(UBL_ERROR, "%s: tsPerAvtpdu %u exceeds CLOCK_REC_MAX_TS_PER_AVTPDU (%u)\r\n",
               __func__, (unsigned int)params.tsPerAvtpdu,
               (unsigned int)CLOCK_REC_MAX_TS_PER_AVTPDU);
        status = ENET_EFAIL;
    }
    else if ((params.tuning != NULL) && !ClockRec_tuningValid(params.tuning))
    {
        UB_LOG(UBL_ERROR, "%s: invalid tuning (zero/negative gain, period or timeout; "
                          "start from ClockRec_getDefaultTuning())\r\n", __func__);
        status = ENET_EFAIL;
    }
    else
    {
        status = ENET_SOK;
        memset(&ClockRec_priv, 0, sizeof(ClockRec_priv));
        ClockRec_priv.params = params;
        /* Resolve once: the caller's tuning struct need not outlive this call. */
        if (params.tuning != NULL)
        {
            ClockRec_priv.tuning = *params.tuning;
        }
        else
        {
            ClockRec_getDefaultTuning(&ClockRec_priv.tuning);
        }
        ClockRec_priv.params.tuning = &ClockRec_priv.tuning;
        if (ClockRec_priv.params.tsPerAvtpdu == 0U)
        {
            ClockRec_priv.params.tsPerAvtpdu = CLOCK_REC_DEFAULT_TS_PER_AVTPDU;
        }
        ClockRec_priv.strategy = (params.clkSrc == CLOCK_REC_SOURCE_PHY) ?
                                 CLOCK_REC_STRATEGY_PHASE_NUDGE :
                                 CLOCK_REC_STRATEGY_FREQ_PID;

        ClockRec_priv.nMediaClockFreq   = (double)params.nominalFrequency;
        ClockRec_priv.nMediaClockPeriod = 1e9 / ClockRec_priv.nMediaClockFreq;
        ClockRec_priv.nTsFreq = ClockRec_priv.nMediaClockFreq /
                                (double)params.tsInterval;
        ClockRec_priv.nTsPeriod = 1e9 / ClockRec_priv.nTsFreq;
        /* Starts at -1 so the first Tss push brings it to 0. */
        ClockRec_priv.eventCounter = -1;

        SemaphoreP_constructBinary(&ClockRec_priv.tickSem, 0);

        ClockRecListener_init(&ClockRec_priv);
        ClockRecTalker_init(&ClockRec_priv);

        status = ClockRec_runHwBootstrap(&ClockRec_priv);
        if (status == ENET_SOK)
        {
            ClockRec_priv.initialized = true;
        }
        else
        {
            SemaphoreP_destruct(&ClockRec_priv.tickSem);
            status = ENET_EFAIL;
        }
    }

    return status;
}

int32_t ClockRec_deInit(void)
{
    int32_t status = ENET_SOK;

    if (!ClockRec_priv.initialized)
    {
        UB_LOG(UBL_WARN, "%s: not initialized\r\n", __func__);
        status = ENET_EFAIL;
    }
    else
    {
        (void)ClockHw_unregisterTssPushEventCb();
        ClockRec_priv.tssCallbackActive = false;

        status = ClockHw_tsrTsSignal(0);
        if (status != ENET_SOK)
        {
            UB_LOG(UBL_ERROR, "%s: ClockHw_tsrTsSignal release failed (%d)\r\n",
                   __func__, status);
        }

        SemaphoreP_destruct(&ClockRec_priv.tickSem);
        ClockRec_priv.initialized = false;
    }

    return status;
}

int32_t ClockRec_restart(void)
{
    int32_t status = ENET_SOK;

    if (!ClockRec_priv.initialized)
    {
        UB_LOG(UBL_WARN, "%s: not initialized\r\n", __func__);
        status = ENET_EFAIL;
    }
    else
    {
        ClockRec_priv.restartRequested = true;
    }

    return status;
}

int32_t ClockRec_submitCrfTs(const uint64_t *ts, uint32_t count)
{
    int32_t status = ENET_EFAIL;

    if (ClockRec_priv.initialized)
    {
        status = ClockRecListener_submitTs(&ClockRec_priv, ts, count);
    }

    return status;
}

int32_t ClockRec_processTick(void)
{
    uint64_t rawTs;
    uint64_t mediaClockEdgeTs;
    uint64_t outTs[CLOCK_REC_MAX_TS_PER_AVTPDU];

    int32_t tickStatus = ENET_EFAIL;

    if (!ClockRec_priv.initialized)
    {
        tickStatus = ENET_EFAIL;
    }
    /* Sleep here until the Tss ISR shim posts; one wakeup per tick. Bounded,
     * so a stopped Tss source surfaces as an error instead of a hung task. */
    else if (SemaphoreP_pend(&ClockRec_priv.tickSem,
                             (uint32_t)ClockP_usecToTicks((uint64_t)CLOCK_REC_TICK_TIMEOUT_MS * 1000U)) !=
             SystemP_SUCCESS)
    {
        UB_LOG(UBL_WARN, "%s: no Tss tick within %u ms\r\n", __func__,
               (unsigned int)CLOCK_REC_TICK_TIMEOUT_MS);
        tickStatus = ENET_EFAIL;
    }
    else
    {
        /* Restart is done here (task context) instead of in the caller of
         * ClockRec_restart(), so it never races with a tick in progress. */
        if (ClockRec_priv.restartRequested)
        {
            ClockRec_priv.restartRequested = false;
            ClockRec_executeRestart(&ClockRec_priv);
            tickStatus = ENET_SOK;
        }
        else
        {
            tickStatus = ENET_SOK;
            rawTs = ClockHw_getTsTimestamp();
            if (rawTs == 0U)
            {
                UB_LOG(UBL_WARN, "%s: no Tss timestamp available this tick\r\n",
                       __func__);
                tickStatus = ENET_SOK;
            }
            else
            {
                /* Shift the Tss capture back to where the real media-clock edge was. */
                mediaClockEdgeTs = rawTs - ClockRec_priv.edgeDiff;

                if (ClockRec_priv.params.role == CLOCK_REC_ROLE_LISTENER)
                {
                    ClockRecListener_onTick(&ClockRec_priv, mediaClockEdgeTs);
                    ClockRec_tsAvailableHook();
                    if (ClockRecListener_needsRestart(&ClockRec_priv))
                    {
                        (void)ClockRec_needsRestartHook();
                    }
                }
                else if (ClockRec_priv.params.role == CLOCK_REC_ROLE_TALKER)
                {
                    if (ClockRecTalker_onTick(&ClockRec_priv, mediaClockEdgeTs, outTs))
                    {
                        ClockRec_crfTsReadyHook(outTs, ClockRec_priv.params.tsPerAvtpdu);
                    }
                }
                else if (ClockRec_priv.params.role == CLOCK_REC_ROLE_TALKER_LOOPBACK)
                {
                    if (ClockRecTalker_onTick(&ClockRec_priv, mediaClockEdgeTs, outTs))
                    {
                        ClockRec_crfTsReadyHook(outTs, ClockRec_priv.params.tsPerAvtpdu);
                        /* Self-loop into this instance's own listener ready queue -
                         * internal call, not via ClockRec_submitCrfTs() (design
                         * \S6.3), mirroring crfApp_spinTalker's self-loop today. */
                        (void)ClockRecListener_submitTs(&ClockRec_priv, outTs, ClockRec_priv.params.tsPerAvtpdu);
                    }
                    ClockRecListener_onTick(&ClockRec_priv, mediaClockEdgeTs);
                    ClockRec_tsAvailableHook();
                    if (ClockRecListener_needsRestart(&ClockRec_priv))
                    {
                        (void)ClockRec_needsRestartHook();
                    }
                }
                else
                {
                    UB_LOG(UBL_ERROR, "%s: invalid role %d\r\n", __func__, (int)ClockRec_priv.params.role);
                }
            }
        }
    }

    return tickStatus;
}

ClockRec_SyncStatus ClockRec_getSyncStatus(void)
{
    return ClockRec_priv.ld.syncStatus;
}

int32_t ClockRec_getMeasuredParams(ClockRec_MeasuredParams *out)
{
    int32_t result = ENET_EFAIL;

    if ((out == NULL) || (!ClockRec_priv.initialized))
    {
        result = ENET_EFAIL;
    }
    else
    {
        result = ENET_SOK;
        if (ClockRec_priv.params.role == CLOCK_REC_ROLE_TALKER)
        {
            out->measuredPeriodNs = ClockRec_priv.td.tssPeriodInit ? ClockRec_priv.td.filtTssPeriod : 0.0;
            out->lastEdgeTs = ClockRec_priv.td.previousMediaClockEdge;
        }
        else
        {
            out->measuredPeriodNs = (ClockRec_priv.ld.currLocalMcFreqHz > 0.0) ?
                                     (1e9 / ClockRec_priv.ld.currLocalMcFreqHz) : 0.0;
            out->lastEdgeTs = ClockRec_priv.ld.prevMcEdgeTs;
        }
    }

    return result;
}

int32_t ClockRec_getDiagCounters(ClockRec_DiagCounters *out)
{
    int32_t result = ENET_EFAIL;

    if ((out == NULL) || (!ClockRec_priv.initialized))
    {
        result = ENET_EFAIL;
    }
    else
    {
        result = ENET_SOK;
        *out = ClockRec_priv.diag;
    }

    return result;
}

uint64_t ClockRec_getMediaClockTimestamp(uint64_t edgeCount)
{
    uint64_t tsCount = 0;
    uint64_t tsTimestamp = 0;
    uintptr_t key = HwiP_disable();
    tsCount = ClockRec_priv.eventCounter;
    tsTimestamp = ClockRec_priv.eventTs - ClockRec_priv.edgeDiff;
    HwiP_restore(key);
    /* Edges since the last Tss event (modular subtraction, read back as
     * signed so it can be negative), times the nominal edge period derived
     * from the init params, added to that event's time. Extrapolates (fwd or
     * back) from the latest anchor. */
    int64_t edgesFromAnchor = (int64_t)(edgeCount - (tsCount * (uint64_t)ClockRec_priv.params.tsInterval));
    int64_t offsetNs = (int64_t)llround((double)edgesFromAnchor * ClockRec_priv.nMediaClockPeriod);
    uint64_t samplingTimeStamp = tsTimestamp + (uint64_t)offsetNs;
    return samplingTimeStamp;
}

uint64_t ClockRec_getMediaClockEdgeCount(uint64_t timestamp)
{
    uint64_t tsCount = 0;
    uint64_t tsTimestamp = 0;
    uintptr_t key = HwiP_disable();
    tsCount = ClockRec_priv.eventCounter;
    tsTimestamp = ClockRec_priv.eventTs - ClockRec_priv.edgeDiff;
    HwiP_restore(key);
    uint64_t edgeCount = 0U;

    /* Period is 0 before ClockRec_init(); nothing to convert with. */
    if (ClockRec_priv.nMediaClockPeriod > 0.0)
    {
        int64_t dtNs = (int64_t)(timestamp - tsTimestamp);
        int64_t edgesFromAnchor = (int64_t)llround((double)dtNs / ClockRec_priv.nMediaClockPeriod);

        edgeCount = (uint64_t)edgesFromAnchor +
                    (tsCount * (uint64_t)ClockRec_priv.params.tsInterval);
    }
    return edgeCount;
}