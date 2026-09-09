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
 * \file   clock_rec.h
 *
 * \brief Hardware-agnostic media clock recovery algorithm module for IEEE
 * 1722 CRF-based clock distribution over AVB. See CLOCK_REC_DESIGN.md.
 *
 * \c ClockRec_* owns the recovery control law (gain-scheduled frequency
 * PID + decoupled phase-bias correction for the CDCE backend, course/fine
 * phase-nudge for the PHY backend), staged startup convergence, and sync
 * hysteresis. All hardware access goes through \c ClockHw_* (clock_hw.h).
 * CRF timestamps cross the API boundary as plain uint64_t arrays;
 * AVTPDU parsing/serialization and transport config are the caller's
 * responsibility (see crf_task.h).
 */

#ifndef CLOCK_REC_H_
#define CLOCK_REC_H_

#include <stdint.h>
#include "clock_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Timestamps per CRF AVTPDU. IEEE 1722 recommends 6, which stays the default
 * (ClockRec_InitParams.tsPerAvtpdu == 0). The MAX only sizes buffers, so raise
 * it at build time (-DCLOCK_REC_MAX_TS_PER_AVTPDU=n) if an application needs
 * more. Talker and listener must be configured with the same count. */
#define CLOCK_REC_DEFAULT_TS_PER_AVTPDU   (6U)
#ifndef CLOCK_REC_MAX_TS_PER_AVTPDU
#define CLOCK_REC_MAX_TS_PER_AVTPDU       (16U)
#endif

/* correctionFlags - listener/loopback roles only; ignored for a pure talker */
#define CLOCK_REC_CORRECT_FREQUENCY   (1U << 0)
#define CLOCK_REC_CORRECT_PHASE       (1U << 1)

typedef enum
{
    CLOCK_REC_ROLE_LISTENER = 0,
    CLOCK_REC_ROLE_TALKER,
    CLOCK_REC_ROLE_TALKER_LOOPBACK   /* talker that also self-tracks its own
                                        transmitted stream, matching the
                                        standalone-Talker self-test path in
                                        crfApp_spinTalker today */
} ClockRec_Role;

typedef enum
{
    CLOCK_REC_SOURCE_CDCE = 0,
    CLOCK_REC_SOURCE_PHY
} ClockRec_Source;

/* Servo and talker tuning. Fill with ClockRec_getDefaultTuning() first, then
 * override only what the application needs - an all-zero struct is rejected
 * by ClockRec_init(). */
typedef struct
{
    double   iirAlpha;              /* IIR coefficient for all filtered estimates, (0, 1] */
    double   freqKpAggressive;      /* freq PID Kp while the error is large */
    double   freqKpFine;            /* freq PID Kp near lock, must be > 0 (Kd ratio base) */
    double   freqKdFine;            /* freq PID Kd near lock (aggressive Kd keeps the Kd/Kp ratio) */
    double   freqKi;                /* freq PID integral gain, must be > 0 (anti-windup divisor) */
    double   dcoMaxPpm;             /* hard limit on the ppm sent to the hardware */
    double   dcoMinWritePpm;        /* smaller corrections are not worth a hardware write */
    double   freqErrThresholdPpm;   /* residual error that switches the freq Kp set */
    double   phaseKpAggressive;     /* phase-bias ppm per ns, large phase error */
    double   phaseKpFine;           /* phase-bias ppm per ns, near lock */
    double   phaseErrThresholdNs;   /* phase error that switches the phase Kp set */
    uint32_t correctionPeriodUs;    /* PID / nudge / hardware-write period */
    uint32_t bootstrapTimeoutMs;    /* max wait for the Mcs and Tss bootstrap samples */
    uint64_t maxTransitTimeNs;      /* talker: network transit budgeted into every timestamp */
} ClockRec_Tuning;

typedef struct
{
    ClockRec_Role   role;
    ClockRec_Source clkSrc;
    uint64_t        parentClockFrequency;   /* Hz, reference the media clock is generated/recovered from */
    uint32_t        nominalFrequency;       /* Hz, e.g. 48000 */
    uint32_t        tsInterval;             /* media-clock edges per CRF timestamp, e.g. 160 */
    uint32_t        tsPerAvtpdu;            /* CRF timestamps per AVTPDU, 1..CLOCK_REC_MAX_TS_PER_AVTPDU;
                                               0 => CLOCK_REC_DEFAULT_TS_PER_AVTPDU. Must match the peer. */
    uint32_t        correctionFlags;        /* CLOCK_REC_CORRECT_* bits */
    int32_t          audioMcaspBaseAddress; /* passed through to ClockHw_configAtlMux */
    ClockHw_McaspDir audioMcaspDir;         /* which frame-sync pin the media clock is on */
    int32_t          tsMcaspBaseAddress;    /* passed through to ClockHw_configAtlMux */
    ClockHw_McaspDir tsMcaspDir;            /* which frame-sync pin the timestamping signal is on */
    CpswCpts_HwPush  mcHwPush;              /* CPTS HW push (1..8) the media clock signal (ATL BWS1)
                                               is routed to; 0 => CPSW_CPTS_HWPUSH_1. */
    CpswCpts_HwPush  tsHwPush;              /* CPTS HW push (1..8) the timestamping signal (ATL BWS2)
                                               is routed to; 0 => CPSW_CPTS_HWPUSH_2.
                                               Must differ from mcHwPush. */
    const ClockRec_Tuning *tuning;         /* NULL => ClockRec_getDefaultTuning() values; copied
                                               by ClockRec_init(), need not outlive the call */
} ClockRec_InitParams;

typedef struct
{
    double   measuredPeriodNs;   /* IIR-filtered local media clock period */
    uint64_t lastEdgeTs;         /* last processed local edge estimate, ns, gPTP-relative */
} ClockRec_MeasuredParams;

typedef struct
{
    uint32_t tsMissCount;             /* getWorthyCrfTs found nothing comparable */
    uint32_t edgeDiscontinuityCount;  /* a measured period was >10% off nominal, dropped */
    uint32_t dcoDroppedCount;         /* freq-PID strategy only: ClockHw_adjFreq requested a
                                          step that truncated to zero at the hardware */
    uint32_t lockSlipCount;           /* nudge strategy only: transitions out of "locked" */
    uint32_t courseCorrCount;         /* nudge strategy only */
    uint32_t fineCorrCount;           /* nudge strategy only */
    uint32_t submitRejectedCount;     /* ClockRec_submitCrfTs rejected: ready queue latched
                                          full, oldest queued entries preserved, not evicted -
                                          see design \S10 */
} ClockRec_DiagCounters;

typedef enum
{
    CLOCK_REC_SYNC_UNSYNCED = 0,  /* no correction has run yet, or actively out of tolerance */
    CLOCK_REC_SYNC_ACQUIRING,     /* correcting, not yet within the assert band */
    CLOCK_REC_SYNC_LOCKED,        /* within tolerance, not yet held long enough to call stable */
    CLOCK_REC_SYNC_STABLE         /* within tolerance for the full hold time -
                                      safe to gate downstream audio on this */
} ClockRec_SyncStatus;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* --- app -> module --- */

/**
 * \brief Fills \p out with the built-in tuning defaults (the values this
 * module used before they became configurable).
 *
 * \param[out] out Destination, must not be NULL.
 */
void ClockRec_getDefaultTuning(ClockRec_Tuning *out);

/**
 * \brief Initializes the clock recovery instance: validates params, selects
 * the correction strategy from clkSrc (freq-PID vs. course/fine nudge),
 * wires up ClockHw_*, and runs the edge-diff bootstrap. Fires
 * ClockRec_clockStartedHook() on success before returning.
 *
 * \param[in] params Configuration, see ClockRec_InitParams.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockRec_init(ClockRec_InitParams params);

/**
 * \brief Tears down the clock recovery instance: unregisters ClockHw_*
 * callbacks, releases hardware routes. Safe to call ClockRec_init() again
 * afterward.
 *
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockRec_deInit(void);

/**
 * \brief Hard reset: resets algorithm state (filters, PID memory, gain
 * schedules, startup staging, sync status) and hardware calibration
 * (re-runs ClockHw_configClock() + the edge-diff bootstrap). Diagnostics
 * counters and the CRF ready queue are NOT cleared.
 *
 * A restart requested while a tick is being processed is latched and
 * applied at the start of the next tick.
 *
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockRec_restart(void);

/**
 * \brief Submits CRF timestamps received from the transport layer into the
 * internal ready queue (listener / loopback roles only). Async-context
 * safe - only touches the queue.
 *
 * On a full ready queue, the new submission is rejected (returns ENET_EFAIL)
 * and every already-queued entry is left untouched.
 *
 * \param[in] ts Array of CRF timestamps, ns, gPTP-relative.
 * \param[in] count Number of entries in ts.
 * \return ENET_SOK on success, ENET_EFAIL on failure (before init/after
 * deInit, or insufficient free queue capacity for the whole submission).
 */
int32_t ClockRec_submitCrfTs(const uint64_t *ts, uint32_t count);

/**
 * \brief Blocks in the calling task until the next Tss tick, then runs the
 * per-tick recovery/sampling pipeline and fires hooks from this task's
 * context. The function registered with ClockHw_registerTssPushEventCb by
 * clock_rec.c is a minimal ISR-safe shim that only marks a tick pending -
 * all IIR/PID/logging work happens here, in the caller's task context, not
 * in that ISR callback.
 *
 * \return ENET_SOK on success, ENET_EFAIL on failure (before init/after deInit).
 */
int32_t ClockRec_processTick(void);

/**
 * \brief Returns the current unified sync status.
 */
ClockRec_SyncStatus ClockRec_getSyncStatus(void);

/**
 * \brief Retrieves the two Confluence-specified measured parameters.
 *
 * \param[out] out Destination.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockRec_getMeasuredParams(ClockRec_MeasuredParams *out);

/**
 * \brief Retrieves cumulative diagnostics counters. Not cleared by
 * ClockRec_restart().
 *
 * \param[out] out Destination.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockRec_getDiagCounters(ClockRec_DiagCounters *out);

/* --- module -> app: application must implement these --- */

/**
 * \brief Fired once the hardware bootstrap (init or restart) completes.
 * The app should start its McASPs / audio pipeline here. Not implemented
 * by crf_task.c - belongs to a future audio-pipeline module.
 *
 * \return Application-defined status (unused by ClockRec_*).
 */
int ClockRec_clockStartedHook(void);

/**
 * \brief Fired after the module has consumed a tick (fetched the raw
 * timestamp, run recovery/sampling logic) - a "tick happened, here's your
 * chance to react" notification, not a "go fetch it yourself" signal. Not
 * implemented by crf_task.c - belongs to a future audio-pipeline module.
 */
void ClockRec_tsAvailableHook(void);

/**
 * \brief Advisory, data-driven heuristic hook: fired when several
 * consecutive ClockRec_submitCrfTs() submissions measured a grossly
 * out-of-tolerance period, which is evidence (independent of any protocol
 * bit) that the remote talker likely restarted. The app decides whether to
 * call ClockRec_restart() in response.
 *
 * \return Application-defined status (unused by ClockRec_*).
 */
int ClockRec_needsRestartHook(void);

/**
 * \brief Fired when the talker/loopback role's timestamp buffer fills.
 * The app is expected to serialize and transmit these as a CRF AVTPDU.
 *
 * \param[in] ts Array of forward-dated timestamps, ns, gPTP-relative.
 * \param[in] count ClockRec_InitParams.tsPerAvtpdu (6 by default).
 */
void ClockRec_crfTsReadyHook(const uint64_t *ts, uint32_t count);

/**
 * \brief Converts a media-clock edge count into its corresponding gPTP-relative
 * timestamp, by projecting forward/backward from the last latched CRF
 * timestamp (edge count + ns) using the nominal media-clock period. Reads the
 * latched reference under HwiP_disable() for atomicity.
 *
 * \param[in] edgeCount Media-clock edge count to convert.
 * \return Corresponding timestamp, ns, gPTP-relative.
 */
uint64_t ClockRec_getMediaClockTimestamp(uint64_t edgeCount);

/**
 * \brief Converts a gPTP-relative timestamp into its corresponding media-clock
 * edge count - the inverse of ClockRec_getMediaClockTimestamp(). Reads the
 * latched reference under HwiP_disable() for atomicity.
 *
 * \param[in] timestamp Timestamp to convert, ns, gPTP-relative.
 * \return Corresponding media-clock edge count.
 */
uint64_t ClockRec_getMediaClockEdgeCount(uint64_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_REC_H_ */
