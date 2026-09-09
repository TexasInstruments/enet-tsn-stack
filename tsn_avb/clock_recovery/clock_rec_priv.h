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
 * \file   clock_rec_priv.h
 *
 * \brief Shared internal state and cross-file entry points for
 * clock_rec.c / clock_rec_listener.c / clock_rec_talker.c. Not installed,
 * not part of the public API - see clock_rec.h for that.
 */

#ifndef CLOCK_REC_PRIV_H_
#define CLOCK_REC_PRIV_H_

#include <stdint.h>
#include <stdbool.h>
#include <kernel/dpl/QueueP.h>
#include <kernel/dpl/SemaphoreP.h>
#include <tsn_combase/cb_thread.h>
#include "clock_rec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Fixed-capacity ready queue: full queue rejects new submissions, never
 * evicts. */
#define CLOCK_REC_CRF_QUEUE_DEPTH   (16U)

/* Selected once at ClockRec_init() from clkSrc and fixed for the
 * instance's lifetime - no runtime branching per tick on which one is
 * active. */
typedef enum
{
    CLOCK_REC_STRATEGY_FREQ_PID = 0,   /* CDCE backend: ClockHw_adjFreq only */
    CLOCK_REC_STRATEGY_PHASE_NUDGE      /* PHY backend: ClockHw_adjPhaseNs only */
} ClockRec_Strategy;

/* Startup convergence staging, freq-PID strategy only. */
typedef enum
{
    CLOCK_REC_SYNC_STAGE_FREQ = 0,
    CLOCK_REC_SYNC_STAGE_PHASE,
    CLOCK_REC_SYNC_STAGE_BOTH
} ClockRec_SyncStage;

typedef struct
{
    QueueP_Elem elem;
    uint64_t    timestamp;
} ClockRec_TsQueueElem;

/*
 * Internal (non-exported) listener state, owned by clock_rec_listener.c.
 * Holds fields for BOTH correction strategies - only one strategy's
 * fields are ever written at runtime (picked once at init).
 */
typedef struct
{
    /* CRF ready queue: fixed pool of CLOCK_REC_CRF_QUEUE_DEPTH elements
     * split into a free queue and a ready queue - a full ready queue means
     * the free queue is exhausted, so enqueue naturally rejects instead of
     * evicting. */
    QueueP_Object        freeQueueObj;
    QueueP_Object         readyQueueObj;
    QueueP_Handle         freeQueue;
    QueueP_Handle         readyQueue;
    ClockRec_TsQueueElem  queueMem[CLOCK_REC_CRF_QUEUE_DEPTH];

    uint64_t readyCrfTs;      /* latched next candidate CRF ts; 0 = none latched */

    /* Continuously IIR-filtered (tuning.iirAlpha) estimates, updated every
     * tick regardless of strategy. */
    double   currLocalMcFreqHz;
    bool     localFreqInit;
    uint64_t prevMcEdgeTs;

    double   currCrfMcFreqHz;
    bool     crfFreqInit;
    uint64_t prevCrfTs;

    double   currPhaseErrFiltNs;
    bool     phaseErrInit;

    double   currFreqErrFiltPPM;
    bool     freqErrInit;

    /* ClockRec_needsRestartHook() heuristic: consecutive (not cumulative)
     * out-of-tolerance periods. Reset to 0 by any sane tick;
     * restartHeuristicArmed makes the hook fire once per debounce crossing
     * rather than on every tick the streak stays high. */
    uint32_t consecutiveDiscontinuities;
    bool     restartHeuristicArmed;

    /* Unified sync-status hysteresis, shared by both strategies. */
    ClockRec_SyncStatus syncStatus;
    uint32_t            syncHoldCount;

    /* 10Hz correction/hardware-write throttle, shared by both strategies. */
    uint64_t lastCorrectionTimeUs;
    bool     correctionTimeInit;

    /* --- freq-PID + phase-bias-decoupling strategy (CDCE) only --- */
    double             freqErrIntegral;
    double             prevFreqErrPPM;
    bool               useFineKp;
    uint32_t           freqGainStreak;
    double             residualErrPPM;
    bool               usePhaseFineKp;
    uint32_t           phaseGainStreak;
    bool               syncStarted;
    ClockRec_SyncStage syncStage;
    uint64_t           syncStageStartTimeUs;
    uint64_t           syncStageElapsedUs;

    /* --- course/fine phase-nudge strategy (PHY) only ---
     * lockSlipCount/courseCorrCount/fineCorrCount live in the shared
     * ClockRec_Priv_t.diag instead - isLocked is the only extra state the
     * log line/hysteresis need. */
    bool     isLocked;
    uint64_t lastNudgeLogTimeUs;
    bool     nudgeLogTimeInit;

    /* 1Hz phase/frequency error log throttle, shared by both strategies. */
    uint64_t lastErrLogTimeUs;
    bool     errLogTimeInit;
} ClockRecListener_t;

/* Internal (non-exported) talker state, owned by clock_rec_talker.c. */
typedef struct
{
    uint64_t arrayTs[CLOCK_REC_MAX_TS_PER_AVTPDU];
    uint32_t currTsIndex;
    uint64_t previousMediaClockEdge;   /* starts at 0; the first tick's
                                         * "period" vs. a 0 baseline is huge
                                         * and safely fails the sanity check
                                         * below - no separate init flag
                                         * needed */
    double   filtTssPeriod;
    bool     tssPeriodInit;
} ClockRecTalker_t;

/* The module-singleton instance (a handle-based API was rejected). */
typedef struct ClockRec_Priv_s
{
    bool                initialized;
    ClockRec_InitParams params;
    ClockRec_Tuning     tuning;       /* params.tuning resolved + copied at init */
    ClockRec_Strategy   strategy;
    int64_t             edgeDiff;     /* constant Tss->true-MC-edge offset, ns */
    bool                restartRequested;

    /* True once ClockHw_registerTssPushEventCb() has succeeded - lets
     * ClockRec_runHwBootstrap() tell "first-ever bootstrap, nothing
     * registered yet" (skip the unregister) apart from "restart,
     * something is still registered from the previous cycle" (must
     * unregister before the reset). Cleared by ClockRec_deInit(). */
    bool                tssCallbackActive;

    SemaphoreP_Object             tickSem;     /* posted by the ISR-safe Tss shim,
                                        * pended by ClockRec_processTick() -
                                        * design \S13.2 */

    /* Nominal rates derived once from params at init. */
    double nMediaClockFreq;
    double nMediaClockPeriod;
    double nTsFreq;
    double nTsPeriod;

    /* Tuple containing Event counter and Timestamp */
    volatile int64_t eventCounter;
    volatile uint64_t eventTs;

    ClockRec_DiagCounters diag;   /* cumulative - never cleared by restart */

    ClockRecListener_t ld;
    ClockRecTalker_t   td;
} ClockRec_Priv_t;

extern ClockRec_Priv_t ClockRec_priv;

/* Absolute difference between two possibly-negative-relative int64_t
 * timestamps/durations, shared across all three .c files. */
static inline uint64_t ClockRec_absDiffU64(int64_t val1, int64_t val2)
{
    return (val1 > val2) ? (uint64_t)(val1 - val2) : (uint64_t)(val2 - val1);
}

/* ========================================================================== */
/*                 clock_rec_listener.c - internal entry points               */
/* ========================================================================== */

/* One-time construction (queues, etc.) - called once from ClockRec_init(). */
void ClockRecListener_init(ClockRec_Priv_t *inst);

/* Algorithm-state-only reset - called from both ClockRec_init() and
 * ClockRec_restart(). Does not touch the CRF ready queue. */
void ClockRecListener_resetAlgState(ClockRec_Priv_t *inst);

/* Shared by ClockRec_submitCrfTs() (app-facing) and the TALKER_LOOPBACK
 * internal self-enqueue path - rejects the whole batch (no partial
 * enqueue) if the free queue can't hold `count` more entries,
 * incrementing diag.submitRejectedCount exactly once per rejected call. */
int32_t ClockRecListener_submitTs(ClockRec_Priv_t *inst,
                                          const uint64_t *ts,
                                          uint32_t count);

/* Steady-state pipeline for one tick: getWorthyCrfTs, IIR
 * frequency/phase/freq-error updates, and (throttled to 10Hz) the active
 * correction strategy, sync hysteresis, and diagnostics logging. */
void ClockRecListener_onTick(ClockRec_Priv_t *inst,
                                     uint64_t mediaClockEdgeTs);

/* Restart heuristic - true exactly once per debounce crossing. */
bool ClockRecListener_needsRestart(ClockRec_Priv_t *inst);

/* ========================================================================== */
/*                  clock_rec_talker.c - internal entry points                */
/* ========================================================================== */

/* One-time construction - called once from ClockRec_init(). Talker state
 * is deliberately NOT reset by ClockRec_restart(). */
void ClockRecTalker_init(ClockRec_Priv_t *inst);

/* Steady-state pipeline for one tick: local period measurement (IIR),
 * forward-dating, params.tsPerAvtpdu-sample buffering. Returns true (with
 * outTs filled) exactly on the tick that buffer fills. */
bool ClockRecTalker_onTick(ClockRec_Priv_t *inst,
                                   uint64_t mediaClockEdgeTs,
                                   uint64_t outTs[CLOCK_REC_MAX_TS_PER_AVTPDU]);

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_REC_PRIV_H_ */
