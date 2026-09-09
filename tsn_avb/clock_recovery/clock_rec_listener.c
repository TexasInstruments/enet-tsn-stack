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
 * \file   clock_rec_listener.c
 *
 * \brief Internal listener state and logic: CRF ready-queue management,
 * worthy-CRF-ts matching, IIR frequency/phase/freq-error estimation, the
 * two correction strategies (freq-PID + phase-bias decoupling for CDCE;
 * course/fine phase nudge for PHY), startup staging, sync-status
 * hysteresis, and diagnostics. See CLOCK_REC_DESIGN.md \S5.1, \S6.2, \S8,
 * \S9.
 *
 * Nothing in this file is exported via clock_rec.h - only clock_rec.c
 * calls into it, through clock_rec_priv.h.
 */

#include <math.h>
#include <string.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/QueueP.h>
#include <kernel/dpl/SystemP.h>
#include <kernel/dpl/DebugP.h>
#include <enet_types.h>
/* UB_LOGCAT must be defined before <tsn_unibase/unibase.h> - category 4 is
 * unused by any other tsn-stack module today (unibase=0, combase=1,
 * gptp=2/3), matching gptp_private.h/combase_private.h's own guarded
 * pattern. */
#ifndef UB_LOGCAT
#define UB_LOGCAT 5
#endif
#include <tsn_unibase/unibase.h>
#include "clock_hw.h"
#include "clock_rec.h"
#include "clock_rec_priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ClockRecListener_correctPhaseBias() gain-schedule streak thresholds:
 * consecutive throttled ticks below/above phaseErrThresholdNs before
 * switching between the aggressive and fine phase-bias Kp. */
#define CLOCK_REC_PHASE_FINE_GAIN_ENTER_STREAK  (30U)
#define CLOCK_REC_PHASE_FINE_GAIN_EXIT_STREAK   (20U)

/* ClockRecListener_correctFrequency() startup staging: max time spent in
 * SYNC_STAGE_FREQ before advancing to SYNC_STAGE_PHASE, and in
 * SYNC_STAGE_PHASE before advancing to SYNC_STAGE_BOTH. */
#define CLOCK_REC_SYNC_STAGE_FREQ_MAX_US        (10U * (uint32_t)UB_SEC_US)
#define CLOCK_REC_SYNC_STAGE_PHASE_MAX_US       (500U * (uint32_t)UB_MSEC_US)

/* ClockRecListener_correctFrequency() freq-PID gain-schedule: the
 * consecutive-tick streak (both directions) before switching between the
 * aggressive and fine Kp/Ki/Kd. The residual error threshold itself is
 * tuning.freqErrThresholdPpm. */
#define CLOCK_REC_FREQ_FINE_GAIN_STREAK           (10U)

/* ClockRecListener_updateSyncStatus() Locked->Stable hold count (throttled
 * ticks, i.e. 200 ticks @10Hz == 20s). */
#define CLOCK_REC_SYNC_STABLE_HOLD_COUNT         (200U)

/* ClockRecListener_correctPhaseNudge() course/fine/locked bands, percent
 * of the nominal media-clock period. */
#define CLOCK_REC_NUDGE_LOCKED_BAND_PCT           (1.0)
#define CLOCK_REC_NUDGE_COURSE_BAND_PCT           (5.0)

/* ClockRecListener_updateSyncStatus() assert/de-assert thresholds (ppm),
 * freq-PID strategy only. */
#define CLOCK_REC_FREQ_SYNC_ASSERT_PPM            (0.4)
#define CLOCK_REC_FREQ_SYNC_DEASSERT_PPM          (0.6)

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static int ClockRecListener_enqueueOne(ClockRec_Priv_t *inst, uint64_t timestamp);
static int ClockRecListener_dequeueOne(ClockRec_Priv_t *inst, uint64_t *timestamp);
static uint64_t ClockRecListener_getWorthyCrfTs(ClockRec_Priv_t *inst, uint64_t currentMediaClockTs);
static void ClockRecListener_updateClockFrequencies(ClockRec_Priv_t *inst, uint64_t mediaClockEdgeTs,
                                                      uint64_t crfTs);
static void ClockRecListener_updatePhaseError(ClockRec_Priv_t *inst, uint64_t mediaClockEdgeTs, uint64_t crfTs);
static void ClockRecListener_updateFreqError(ClockRec_Priv_t *inst);
static void ClockRecListener_updateSyncStatus(ClockRec_Priv_t *inst, double absErr, bool haveRealSample,
                                                double assertThresh, double deassertThresh);
static double ClockRecListener_correctPhaseBias(ClockRec_Priv_t *inst);
static void ClockRecListener_correctFrequency(ClockRec_Priv_t *inst);
static void ClockRecListener_correctPhaseNudge(ClockRec_Priv_t *inst);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static int ClockRecListener_enqueueOne(ClockRec_Priv_t *inst,
                                       uint64_t timestamp)
{
    ClockRec_TsQueueElem *elem;
    int status = SystemP_FAILURE;

    if (QueueP_isEmpty(inst->ld.freeQueue) == true)
    {
        status = SystemP_FAILURE;
    }
    else
    {
        elem = (ClockRec_TsQueueElem *)QueueP_get(inst->ld.freeQueue);
        if (elem == NULL)
        {
            status = SystemP_FAILURE;
        }
        else
        {
            elem->timestamp = timestamp;
            status = QueueP_put(inst->ld.readyQueue, elem);
        }
    }

    return status;
}

static int ClockRecListener_dequeueOne(ClockRec_Priv_t *inst,
                                      uint64_t *timestamp)
{
    ClockRec_TsQueueElem *elem;
    int status = SystemP_FAILURE;

    if (QueueP_isEmpty(inst->ld.readyQueue) == true)
    {
        status = SystemP_FAILURE;
    }
    else
    {
        elem = (ClockRec_TsQueueElem *)QueueP_get(inst->ld.readyQueue);
        if (elem == NULL)
        {
            status = SystemP_FAILURE;
        }
        else
        {
            *timestamp = elem->timestamp;
            status = QueueP_put(inst->ld.freeQueue, elem);
        }
    }

    return status;
}

/* Dequeues until a queued CRF ts lands within one nTsPeriod of
 * currentMediaClockTs (a worthy match), or catches up to/passes it.
 * tsMissCount++ on both "queue exhausted mid-drain" and "still in the
 * future" outcomes. */
static uint64_t ClockRecListener_getWorthyCrfTs(
    ClockRec_Priv_t *inst, uint64_t currentMediaClockTs)
{
    ClockRecListener_t *ld = &inst->ld;
    uint64_t matchCrfTimestamp = 0U;

    bool miss = false;

    /* Anything older than the current edge by more than one period can't
     * match, so just throw it away and look at the next one. */
    while (!miss &&
           (ClockRec_absDiffU64(
               (int64_t)ld->readyCrfTs,
               (int64_t)currentMediaClockTs) > inst->nTsPeriod) &&
           (ld->readyCrfTs < currentMediaClockTs))
    {
        if (ClockRecListener_dequeueOne(inst, &ld->readyCrfTs)
            != SystemP_SUCCESS)
        {
            ld->readyCrfTs = 0U;
            inst->diag.tsMissCount++;
            miss = true;
        }
    }

    if (!miss)
    {
        if (ClockRec_absDiffU64(
               (int64_t)ld->readyCrfTs,
               (int64_t)currentMediaClockTs) <= inst->nTsPeriod)
        {
            matchCrfTimestamp = ld->readyCrfTs;

            if (ClockRecListener_dequeueOne(inst, &ld->readyCrfTs)
                != SystemP_SUCCESS)
            {
                ld->readyCrfTs = 0U;
            }
        }
        else
        {
            inst->diag.tsMissCount++;
        }
    }

    return matchCrfTimestamp;
}

/* Running IIR-filtered (inst->tuning.iirAlpha) estimate of the local media
 * clock frequency and the Talker's CRF-derived frequency, updated every
 * tick. A period landing outside 10% of nTsPeriod is dropped
 * (edgeDiscontinuityCount++, previous filtered value retained) instead of
 * folded into the filter; consecutiveDiscontinuities feeds
 * ClockRec_needsRestartHook(). */
static void ClockRecListener_updateClockFrequencies(ClockRec_Priv_t *inst, uint64_t mediaClockEdgeTs,
                                                      uint64_t crfTs)
{
    ClockRecListener_t *ld = &inst->ld;
    bool anyDiscontinuity = false;

    if (!ld->localFreqInit)
    {
        ld->prevMcEdgeTs = mediaClockEdgeTs;
        ld->localFreqInit = true;
    }
    else
    {
        uint64_t localPeriod = mediaClockEdgeTs - ld->prevMcEdgeTs;
        bool localSane = ClockRec_absDiffU64((int64_t)localPeriod, (int64_t)inst->nTsPeriod) <
                          (uint64_t)(0.1 * inst->nTsPeriod);

        ld->prevMcEdgeTs = mediaClockEdgeTs;

        if (localSane)
        {
            /* One tick spans tsInterval media-clock edges, so divide down to
             * a per-edge period before turning it into Hz. */
            double instLocalFreqHz = 1e9 / ((double)localPeriod / (double)inst->params.tsInterval);

            ld->currLocalMcFreqHz = (ld->currLocalMcFreqHz <= 0.0) ?
                                         instLocalFreqHz :
                                         ((inst->tuning.iirAlpha * instLocalFreqHz) +
                                          ((1.0 - inst->tuning.iirAlpha) * ld->currLocalMcFreqHz));
        }
        else
        {
            anyDiscontinuity = true;
        }
    }

    if (crfTs != 0U)
    {
        if (!ld->crfFreqInit)
        {
            ld->prevCrfTs = crfTs;
            ld->crfFreqInit = true;
        }
        else
        {
            uint64_t crfPeriod = crfTs - ld->prevCrfTs;
            bool crfSane = ClockRec_absDiffU64((int64_t)crfPeriod, (int64_t)inst->nTsPeriod) <
                            (uint64_t)(0.1 * inst->nTsPeriod);

            ld->prevCrfTs = crfTs;

            if (crfSane)
            {
                double instCrfFreqHz = 1e9 / ((double)crfPeriod / (double)inst->params.tsInterval);

                ld->currCrfMcFreqHz = (ld->currCrfMcFreqHz <= 0.0) ?
                                           instCrfFreqHz :
                                           ((inst->tuning.iirAlpha * instCrfFreqHz) +
                                            ((1.0 - inst->tuning.iirAlpha) * ld->currCrfMcFreqHz));
            }
            else
            {
                anyDiscontinuity = true;
            }
        }
    }
    /* else: crfTs == 0 is a miss (no worthy match this tick) - prevCrfTs
     * is deliberately left untouched. */

    if (anyDiscontinuity)
    {
        inst->diag.edgeDiscontinuityCount++;
        ld->consecutiveDiscontinuities++;
    }
    else
    {
        ld->consecutiveDiscontinuities = 0U;
    }
}

/* Phase difference between the local media clock edge and the matched CRF
 * timestamp, wrapped into (-measuredPeriod/2, measuredPeriod/2]. Only runs
 * on a worthy tick once the local frequency tracker has produced a
 * measured period. */
static void ClockRecListener_updatePhaseError(
    ClockRec_Priv_t *inst, uint64_t mediaClockEdgeTs, uint64_t crfTs)
{
    ClockRecListener_t *ld = &inst->ld;
    double measuredPeriod;
    int64_t timeDiff;
    double cycles;
    double rawPhaseErrNs;
    bool run = true;

    if ((crfTs == 0U) || (ld->currLocalMcFreqHz <= 0.0))
    {
        run = false;
    }

    if (run)
    {
        measuredPeriod = 1e9 / ld->currLocalMcFreqHz;
        timeDiff = (int64_t)mediaClockEdgeTs - (int64_t)crfTs;
        /* Only the fractional part of a cycle matters, whole cycles are just
         * the two clocks being offset by an integer number of edges. */
        cycles = (double)timeDiff / measuredPeriod;
        rawPhaseErrNs = (cycles - floor(cycles)) * measuredPeriod;

        if (rawPhaseErrNs > (measuredPeriod / 2.0))
        {
            rawPhaseErrNs -= measuredPeriod;
        }

        ld->currPhaseErrFiltNs = ld->phaseErrInit ?
                                      ((inst->tuning.iirAlpha * rawPhaseErrNs) +
                                       ((1.0 - inst->tuning.iirAlpha) *
                                        ld->currPhaseErrFiltNs)) :
                                      rawPhaseErrNs;
        ld->phaseErrInit = true;
    }
}

/* Relative frequency error (local vs. CRF-derived, ppm) from the two
 * tracked frequency estimates above. Safe to run every tick. */
static void ClockRecListener_updateFreqError(ClockRec_Priv_t *inst)
{
    ClockRecListener_t *ld = &inst->ld;
    double rawFreqErrPPM;
    bool run = true;

    if ((ld->currLocalMcFreqHz <= 0.0) ||
        (ld->currCrfMcFreqHz <= 0.0))
    {
        run = false;
    }

    if (run)
    {
        /* Positive means the local clock is running faster than the talker's. */
        rawFreqErrPPM = ((ld->currLocalMcFreqHz - ld->currCrfMcFreqHz) *
                         1e6) / ld->currCrfMcFreqHz;

        ld->currFreqErrFiltPPM = ld->freqErrInit ?
                                      ((inst->tuning.iirAlpha * rawFreqErrPPM) +
                                       ((1.0 - inst->tuning.iirAlpha) *
                                        ld->currFreqErrFiltPPM)) :
                                      rawFreqErrPPM;
        ld->freqErrInit = true;
    }
}

/* Unified sync-status hysteresis, shared by both correction strategies -
 * each calls this with its own error metric and assert/de-assert
 * thresholds. Acquiring has no de-assert edge (only Locked/Stable do). */
static void ClockRecListener_updateSyncStatus(
    ClockRec_Priv_t *inst, double absErr, bool haveRealSample,
    double assertThresh, double deassertThresh)
{
    ClockRecListener_t *ld = &inst->ld;
    bool process = true;

    if (!haveRealSample)
    {
        ld->syncStatus = CLOCK_REC_SYNC_UNSYNCED;
        ld->syncHoldCount = 0U;
        process = false;
    }

    if (process)
    {
        if (ld->syncStatus == CLOCK_REC_SYNC_UNSYNCED)
        {
            ld->syncStatus = CLOCK_REC_SYNC_ACQUIRING;
        }

        switch (ld->syncStatus)
        {
        case CLOCK_REC_SYNC_ACQUIRING:
            if (absErr < assertThresh)
            {
                ld->syncStatus = CLOCK_REC_SYNC_LOCKED;
                ld->syncHoldCount = 0U;
            }
            break;

        case CLOCK_REC_SYNC_LOCKED:
            if (absErr > deassertThresh)
            {
                ld->syncStatus = CLOCK_REC_SYNC_UNSYNCED;
                ld->syncHoldCount = 0U;
            }
            else
            {
                ld->syncHoldCount++;
                if (ld->syncHoldCount >= CLOCK_REC_SYNC_STABLE_HOLD_COUNT)
                {
                    ld->syncStatus = CLOCK_REC_SYNC_STABLE;
                }
            }
            break;

        case CLOCK_REC_SYNC_STABLE:
            if (absErr > deassertThresh)
            {
                ld->syncStatus = CLOCK_REC_SYNC_UNSYNCED;
                ld->syncHoldCount = 0U;
            }
            break;

        case CLOCK_REC_SYNC_UNSYNCED:
        default:
            break;
        }
    }
}

/* Frequency bias (ppm) derived from the filtered phase error, folded into
 * the PID output before the single ClockHw_adjFreq() call - the CDCE
 * backend has no phase actuator, and phase is the time-integral of
 * frequency, so biasing frequency converges phase without a separate
 * actuator. Proportional-only, gain-scheduled (aggressive/fine) the same
 * way as the frequency PID's own Kp/Kd. */
static double ClockRecListener_correctPhaseBias(ClockRec_Priv_t *inst)
{
    ClockRecListener_t *ld = &inst->ld;
    /* Kp is in ppm of bias per ns of phase error. */
    const double KpPhaseAggressive = inst->tuning.phaseKpAggressive;
    const double KpPhaseFine = inst->tuning.phaseKpFine;
    const double phaseErrThresholdNs = inst->tuning.phaseErrThresholdNs;
    bool phaseErrAboveThreshold = (fabs(ld->currPhaseErrFiltNs) > phaseErrThresholdNs);
    double KpPhase;

    if (!ld->usePhaseFineKp)
    {
        if (!phaseErrAboveThreshold)
        {
            ld->phaseGainStreak++;
            if (ld->phaseGainStreak >= CLOCK_REC_PHASE_FINE_GAIN_ENTER_STREAK)
            {
                ld->usePhaseFineKp = true;
                ld->phaseGainStreak = 0U;
            }
        }
        else
        {
            ld->phaseGainStreak = 0U;
        }
    }
    else
    {
        if (phaseErrAboveThreshold)
        {
            ld->phaseGainStreak++;
            if (ld->phaseGainStreak >= CLOCK_REC_PHASE_FINE_GAIN_EXIT_STREAK)
            {
                ld->usePhaseFineKp = false;
                ld->phaseGainStreak = 0U;
            }
        }
        else
        {
            ld->phaseGainStreak = 0U;
        }
    }

    KpPhase = ld->usePhaseFineKp ? KpPhaseFine : KpPhaseAggressive;

    return KpPhase * ld->currPhaseErrFiltNs;
}

/* Gain-scheduled Kp/Ki/Kd PID on the (phase-bias-decoupled) frequency
 * error, staged FREQ->PHASE->BOTH on startup when phase correction is
 * also requested. Output only ever goes to ClockHw_adjFreq(); called at
 * the shared 10Hz throttle, not every tick. */
static void ClockRecListener_correctFrequency(ClockRec_Priv_t *inst)
{
    ClockRecListener_t *ld = &inst->ld;
    double errorPPM = ld->currFreqErrFiltPPM;
    bool phaseFlagSet = (inst->params.correctionFlags & CLOCK_REC_CORRECT_PHASE) != 0U;
    bool wantFreq = true;
    bool wantPhase = false;
    bool doFreq;
    bool doPhase;
    double phaseBiasPPM;
    double residualErrorPPM;
    double pidOutPPM;
    /* Hard limit on the final ppm sent to the hardware. */
    const double outputClamp = inst->tuning.dcoMaxPpm;

    if (phaseFlagSet && ld->freqErrInit)
    {
        uint64_t nowUs = ClockP_getTimeUsec();
        uint64_t stageElapsedUs;

        if (!ld->syncStarted)
        {
            ld->syncStarted = true;
            ld->syncStage = CLOCK_REC_SYNC_STAGE_FREQ;
            ld->syncStageStartTimeUs = nowUs;
        }

        stageElapsedUs = nowUs - ld->syncStageStartTimeUs;

        if ((ld->syncStage == CLOCK_REC_SYNC_STAGE_FREQ) &&
            (ld->useFineKp ||
             (stageElapsedUs >= CLOCK_REC_SYNC_STAGE_FREQ_MAX_US)))
        {
            ld->syncStage = CLOCK_REC_SYNC_STAGE_PHASE;
            ld->syncStageStartTimeUs = nowUs;
            stageElapsedUs = 0U;
        }

        if ((ld->syncStage == CLOCK_REC_SYNC_STAGE_PHASE) &&
            (stageElapsedUs >= CLOCK_REC_SYNC_STAGE_PHASE_MAX_US))
        {
            ld->syncStage = CLOCK_REC_SYNC_STAGE_BOTH;
        }

        ld->syncStageElapsedUs = stageElapsedUs;

        switch (ld->syncStage)
        {
            case CLOCK_REC_SYNC_STAGE_FREQ:
                wantFreq = true;
                wantPhase = false;
                break;
            case CLOCK_REC_SYNC_STAGE_PHASE:
                wantFreq = false;
                wantPhase = true;
                break;
            case CLOCK_REC_SYNC_STAGE_BOTH:
            default:
                wantFreq = true;
                wantPhase = true;
                break;
        }
    }

    doFreq = wantFreq && ld->freqErrInit;
    doPhase = wantPhase && ld->phaseErrInit;

    phaseBiasPPM = doPhase ? ClockRecListener_correctPhaseBias(inst) : 0.0;
    residualErrorPPM = errorPPM - phaseBiasPPM;
    ld->residualErrPPM = residualErrorPPM;

    if (!doFreq)
    {
        /* Freq PID paused (STAGE_PHASE) or no real frequency sample yet -
         * freqErrIntegral/prevFreqErrPPM left untouched so it resumes
         * cleanly. */
        pidOutPPM = phaseBiasPPM;
    }
    else
    {
        const double KpAggressive = inst->tuning.freqKpAggressive;
        const double KpFine = inst->tuning.freqKpFine;
        const double KdFine = inst->tuning.freqKdFine;
        /* Keeps the Kd/Kp ratio the same in both gain sets. */
        const double KdAggressive = KpAggressive * (KdFine / KpFine);
        const double Ki = inst->tuning.freqKi;
        /* Anti-windup: Ki * integral can never exceed the output clamp. */
        const double integralClamp = outputClamp / Ki;
        bool errAboveThreshold =
            (fabs(residualErrorPPM) > inst->tuning.freqErrThresholdPpm);
        double Kp;
        double Kd;
        double derivativePPM;

        if (!ld->useFineKp)
        {
            if (!errAboveThreshold)
            {
                ld->freqGainStreak++;
                if (ld->freqGainStreak >= CLOCK_REC_FREQ_FINE_GAIN_STREAK)
                {
                    ld->useFineKp = true;
                    ld->freqGainStreak = 0U;
                }
            }
            else
            {
                ld->freqGainStreak = 0U;
            }
        }
        else
        {
            if (errAboveThreshold)
            {
                ld->freqGainStreak++;
                if (ld->freqGainStreak >= CLOCK_REC_FREQ_FINE_GAIN_STREAK)
                {
                    ld->useFineKp = false;
                    ld->freqGainStreak = 0U;
                }
            }
            else
            {
                ld->freqGainStreak = 0U;
            }
        }

        Kp = ld->useFineKp ? KpFine : KpAggressive;
        Kd = ld->useFineKp ? KdFine : KdAggressive;

        ld->freqErrIntegral += residualErrorPPM;
        if (ld->freqErrIntegral > integralClamp)
        {
            ld->freqErrIntegral = integralClamp;
        }
        else if (ld->freqErrIntegral < -integralClamp)
        {
            ld->freqErrIntegral = -integralClamp;
        }

        derivativePPM = residualErrorPPM - ld->prevFreqErrPPM;
        ld->prevFreqErrPPM = residualErrorPPM;

        /* Negative sign: a positive error means we're fast, so slow down. */
        pidOutPPM = -((Kp * residualErrorPPM) + (Ki * ld->freqErrIntegral) + (Kd * derivativePPM));

        /* Add the phase bias back on, untouched by Kp/Ki/Kd. */
        pidOutPPM += phaseBiasPPM;
    }

    if (pidOutPPM > outputClamp)
    {
        pidOutPPM = outputClamp;
    }
    else if (pidOutPPM < -outputClamp)
    {
        pidOutPPM = -outputClamp;
    }

    /* Skip tiny outputs, not worth a hardware write. */
    if (fabs(pidOutPPM) >= inst->tuning.dcoMinWritePpm)
    {
        /* ClockHw_adjFreq() takes ppb; Board_CdceFineTuneFreq() (beneath
         * it) divides by 1000 to recover ppm, so scale up by 1000 here. */
        int ppb = (int)(pidOutPPM * 1000.0);
        int status = ClockHw_adjFreq(ppb);

        if (status != ENET_SOK)
        {
            inst->diag.dcoDroppedCount++;
        }
    }

    /* Sync status uses the raw errorPPM, not residualErrorPPM - it should
     * reflect true physical frequency deviation, intentional bias or not. */
    ClockRecListener_updateSyncStatus(inst, fabs(errorPPM), ld->freqErrInit,
                                       CLOCK_REC_FREQ_SYNC_ASSERT_PPM,
                                       CLOCK_REC_FREQ_SYNC_DEASSERT_PPM);
}

/* Threshold-banded course/fine cycle-nudge loop for the PHY backend (no
 * PID), via ClockHw_adjPhaseNs(). Called at the shared 10Hz throttle. */
static void ClockRecListener_correctPhaseNudge(ClockRec_Priv_t *inst)
{
    ClockRecListener_t *ld = &inst->ld;
    double percent;
    bool wasLocked;
    bool run = true;

    if (!ld->phaseErrInit)
    {
        run = false;
    }

    if (run)
    {
        /* Phase error as a percent of one media-clock period. */
        percent = (ld->currPhaseErrFiltNs * 100.0) /
                  inst->nMediaClockPeriod;

        if (fabs(percent) < CLOCK_REC_NUDGE_LOCKED_BAND_PCT)
        {
            /* No correction needed. */
        }
        else if (fabs(percent) < CLOCK_REC_NUDGE_COURSE_BAND_PCT)
        {
            /* Small miss: nudge by 1 step, against the sign of the error. */
            (void)ClockHw_adjPhaseNs((percent > 0.0) ? -1 : 1);
            inst->diag.fineCorrCount++;
        }
        else
        {
            /* Big miss: same idea but 5 steps at a time to close faster. */
            (void)ClockHw_adjPhaseNs(
                (percent > 0.0) ? -5 : 5);
            inst->diag.courseCorrCount++;
        }

        wasLocked = ld->isLocked;
        ClockRecListener_updateSyncStatus(
            inst, fabs(percent), true,
            CLOCK_REC_NUDGE_LOCKED_BAND_PCT,
            CLOCK_REC_NUDGE_COURSE_BAND_PCT);
        ld->isLocked = (ld->syncStatus == CLOCK_REC_SYNC_LOCKED) ||
                       (ld->syncStatus == CLOCK_REC_SYNC_STABLE);
        if (wasLocked && !ld->isLocked)
        {
            inst->diag.lockSlipCount++;
        }

        {
            uint64_t nowUs = ClockP_getTimeUsec();

            if ((!ld->nudgeLogTimeInit) ||
                ((nowUs - ld->lastNudgeLogTimeUs) >
                 (uint64_t)UB_SEC_US))
            {
                /* ASTRA-parseable log format - do not change
                 * field names/order. */
                UB_LOG(UBL_INFO,
                       "Lock = %d, SlipCounter = %u, "
                       "Course Correction = %u, "
                       "Fine Correction = %u\n",
                       (int)ld->isLocked,
                       inst->diag.lockSlipCount,
                       inst->diag.courseCorrCount,
                       inst->diag.fineCorrCount);
                ld->lastNudgeLogTimeUs = nowUs;
                ld->nudgeLogTimeInit = true;
            }
        }
    }
}

void ClockRecListener_init(ClockRec_Priv_t *inst)
{
    uint32_t i;

    inst->ld.freeQueue = QueueP_create(&inst->ld.freeQueueObj);
    inst->ld.readyQueue = QueueP_create(&inst->ld.readyQueueObj);

    for (i = 0U; i < CLOCK_REC_CRF_QUEUE_DEPTH; i++)
    {
        (void)QueueP_put(inst->ld.freeQueue, &inst->ld.queueMem[i]);
    }

    ClockRecListener_resetAlgState(inst);
}

void ClockRecListener_resetAlgState(ClockRec_Priv_t *inst)
{
    ClockRecListener_t *ld = &inst->ld;

    /* IIR filter state and *Init latches. */
    ld->currLocalMcFreqHz = 0.0;
    ld->localFreqInit = false;
    ld->prevMcEdgeTs = 0U;

    ld->currCrfMcFreqHz = 0.0;
    ld->crfFreqInit = false;
    ld->prevCrfTs = 0U;

    ld->currPhaseErrFiltNs = 0.0;
    ld->phaseErrInit = false;

    ld->currFreqErrFiltPPM = 0.0;
    ld->freqErrInit = false;

    ld->consecutiveDiscontinuities = 0U;
    ld->restartHeuristicArmed = false;

    /* PID memory: integral accumulator, previous-error sample. */
    ld->freqErrIntegral = 0.0;
    ld->prevFreqErrPPM = 0.0;
    ld->residualErrPPM = 0.0;

    /* Gain-scheduling streaks - forced back to the aggressive gain on
     * both the frequency and phase-bias schedules. */
    ld->useFineKp = false;
    ld->freqGainStreak = 0U;
    ld->usePhaseFineKp = false;
    ld->phaseGainStreak = 0U;

    /* Startup staging - forced back to Freq, syncStarted cleared. */
    ld->syncStarted = false;
    ld->syncStage = CLOCK_REC_SYNC_STAGE_FREQ;
    ld->syncStageStartTimeUs = 0U;
    ld->syncStageElapsedUs = 0U;

    /* Sync status - forced back to Unsynced, hold counters cleared. */
    ld->syncStatus = CLOCK_REC_SYNC_UNSYNCED;
    ld->syncHoldCount = 0U;

    ld->lastCorrectionTimeUs = 0U;
    ld->correctionTimeInit = false;

    /* Nudge-strategy status - isLocked mirrors the unified sync status
     * above and is reset the same way. */
    ld->isLocked = false;
    ld->lastNudgeLogTimeUs = 0U;
    ld->nudgeLogTimeInit = false;

    ld->lastErrLogTimeUs = 0U;
    ld->errLogTimeInit = false;

    /* Deliberately NOT touched here: the CRF ready/free queues,
     * ld.readyCrfTs, and inst->diag (cumulative operational history, not
     * algorithm state). */
}

int32_t ClockRecListener_submitTs(ClockRec_Priv_t *inst,
                                 const uint64_t *ts,
                                 uint32_t count)
{
    uint32_t i;
    int32_t status = ENET_EFAIL;

    if ((ts == NULL) || (count == 0U))
    {
        status = ENET_EFAIL;
    }
    else if (QueueP_getSize(inst->ld.freeQueue) < count)
    {
        inst->diag.submitRejectedCount++;
        status = ENET_EFAIL;
    }
    else
    {
        status = ENET_SOK;
        for (i = 0U; i < count; i++)
        {
            if (ClockRecListener_enqueueOne(inst, ts[i])
                != SystemP_SUCCESS)
            {
                /* Free queue was drained by a concurrent submitter
                 * between the up-front size check above and this
                 * iteration (TOCTOU). Warn only. */
                UB_LOG(UBL_WARN,
                       "%s: free queue exhausted mid-batch at "
                       "ts[%u/%u] - remaining timestamps in this "
                       "submission dropped\r\n",
                       __func__, i, count);
                break;
            }
        }
    }

    return status;
}

void ClockRecListener_onTick(ClockRec_Priv_t *inst, uint64_t mediaClockEdgeTs)
{
    uint64_t crfTs = ClockRecListener_getWorthyCrfTs(inst, mediaClockEdgeTs);
    uint64_t nowUs;
    bool runCorrection;

    ClockRecListener_updateClockFrequencies(inst, mediaClockEdgeTs, crfTs);
    ClockRecListener_updatePhaseError(inst, mediaClockEdgeTs, crfTs);
    ClockRecListener_updateFreqError(inst);

    /* Throttle the correction/hardware-write to tuning.correctionPeriodUs
     * (10Hz by default; design \S6.2's "alt every 10Hz tick" block) for both
     * strategies - the IIR updates above still run every tick so the
     * filtered estimates stay fresh. */
    nowUs = ClockP_getTimeUsec();
    runCorrection = (!inst->ld.correctionTimeInit) ||
                    ((nowUs - inst->ld.lastCorrectionTimeUs) > (uint64_t)inst->tuning.correctionPeriodUs);

    if (runCorrection)
    {
        if (inst->strategy == CLOCK_REC_STRATEGY_FREQ_PID)
        {
            if ((inst->params.correctionFlags & CLOCK_REC_CORRECT_FREQUENCY) != 0U)
            {
                ClockRecListener_correctFrequency(inst);
            }
        }
        else /* CLOCK_REC_STRATEGY_PHASE_NUDGE */
        {
            if ((inst->params.correctionFlags & CLOCK_REC_CORRECT_PHASE) != 0U)
            {
                ClockRecListener_correctPhaseNudge(inst);
            }
        }

        inst->ld.lastCorrectionTimeUs = nowUs;
        inst->ld.correctionTimeInit = true;
    }

    /* 1Hz phase/frequency error print, shared by both strategies -
     * decoupled from the 10Hz correction throttle above. */
    if ((!inst->ld.errLogTimeInit) ||
        ((nowUs - inst->ld.lastErrLogTimeUs) > (uint64_t)UB_SEC_US))
    {
        UB_LOG(UBL_INFO, "phase_err_ns:%lf,freq_err_ppm:%lf,"
                         "stage_elapsed_us:%llu,freq_fine:%d,phase_fine:%d\r\n",
                inst->ld.currPhaseErrFiltNs, inst->ld.currFreqErrFiltPPM,
                inst->ld.syncStageElapsedUs, (int)inst->ld.useFineKp, (int)inst->ld.usePhaseFineKp);

        inst->ld.lastErrLogTimeUs = nowUs;
        inst->ld.errLogTimeInit = true;
    }
}

bool ClockRecListener_needsRestart(ClockRec_Priv_t *inst)
{
    ClockRecListener_t *ld = &inst->ld;
    /* Roughly 5 bad ticks in a row before we believe the talker restarted. */
    const uint32_t debounceCount = 5U;
    bool needsRestart = false;

    if (ld->consecutiveDiscontinuities >= debounceCount)
    {
        if (!ld->restartHeuristicArmed)
        {
            ld->restartHeuristicArmed = true;
            needsRestart = true;
        }
    }
    else
    {
        ld->restartHeuristicArmed = false;
    }

    return needsRestart;
}
