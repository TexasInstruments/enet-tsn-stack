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
 * \file   clock_rec_talker.c
 *
 * \brief Internal talker state and logic: per-tick local period
 * measurement, forward-dating for transit + remaining-buffer delay, and
 * 6-sample buffering. Nothing here is exported via clock_rec.h - only
 * clock_rec.c calls into it, through clock_rec_priv.h.
 */

#include <math.h>
#include <string.h>
#include <tsn_unibase/unibase.h>
#include "clock_rec.h"
#include "clock_rec_priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Fixed 100ns bound, not a fraction of the period - matches the
 * reference crfApp_local2CrfTs() exactly (design \S2). */
#define CLOCK_REC_TALKER_SAMPLE_TOL_NS    (100U)

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static bool ClockRecTalker_forwardDate(ClockRec_Priv_t *inst, uint64_t *timestamp);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/*
 * Forward-dates a local media-clock-edge timestamp into a CRF timestamp:
 * adds the network transit budget (rounded up to a whole number of local
 * media-clock periods) plus the time needed to fill the remaining 5
 * buffer slots (5 x the IIR-filtered local Tss period).
 *
 * Returns true if this timestamp is consecutive with the previously
 * buffered one (or is the first in a fresh buffer) - false means the
 * whole buffer should be discarded.
 */
static bool ClockRecTalker_forwardDate(ClockRec_Priv_t *inst, uint64_t *timestamp)
{
    ClockRecTalker_t *td = &inst->td;
    double timestampingPeriod = td->filtTssPeriod;
    /* Assumed worst-case network transit, budgeted into every timestamp. */
    uint64_t maxTransitTime = inst->tuning.maxTransitTimeNs;
    uint64_t prevTimestamp = (td->currTsIndex == 0U) ? 0U : td->arrayTs[td->currTsIndex - 1U];
    uint64_t totalAccumTime = (uint64_t)((double)(inst->params.tsPerAvtpdu - 1U) * timestampingPeriod);
    uint64_t timestampOffset;
    bool result = false;

    timestampOffset = (uint64_t)(ceil((double)maxTransitTime / inst->nMediaClockPeriod) * inst->nMediaClockPeriod) +
                       totalAccumTime;
    *timestamp += timestampOffset;

    if ((prevTimestamp == 0U) ||
        (ClockRec_absDiffU64((int64_t)(prevTimestamp + (uint64_t)timestampingPeriod), (int64_t)(*timestamp)) <
         (uint64_t)CLOCK_REC_TALKER_SAMPLE_TOL_NS))
    {
        result = true;
    }

    return result;
}

void ClockRecTalker_init(ClockRec_Priv_t *inst)
{
    (void)memset(&inst->td, 0, sizeof(inst->td));
}

bool ClockRecTalker_onTick(ClockRec_Priv_t *inst, uint64_t mediaClockEdgeTs,
                            uint64_t outTs[CLOCK_REC_MAX_TS_PER_AVTPDU])
{
    ClockRecTalker_t *td = &inst->td;
    uint64_t currentTssPeriod = mediaClockEdgeTs - td->previousMediaClockEdge;
    bool bufferFull = false;

    /* Ignore periods more than 10% off nominal (startup or a glitch). */
    if (ClockRec_absDiffU64((int64_t)currentTssPeriod, (int64_t)inst->nTsPeriod) <
        (uint64_t)(0.1 * inst->nTsPeriod))
    {
        /* Valid candidate to calculate the local period - IIR-filtered
         * (tuning.iirAlpha, shared with the listener-side filters) before
         * ClockRecTalker_forwardDate() bakes it into the outgoing
         * timestamps, so per-tick jitter doesn't ride straight onto the
         * wire. */
        const double alpha = inst->tuning.iirAlpha;

        td->filtTssPeriod = td->tssPeriodInit ?
                                 ((alpha * (double)currentTssPeriod) + ((1.0 - alpha) * td->filtTssPeriod)) :
                                 (double)currentTssPeriod;
        td->tssPeriodInit = true;
    }

    td->previousMediaClockEdge = mediaClockEdgeTs;

    if (ClockRecTalker_forwardDate(inst, &mediaClockEdgeTs))
    {
        td->arrayTs[td->currTsIndex] = mediaClockEdgeTs;
        td->currTsIndex++;
    }
    else
    {
        /* Non-consecutive timestamp: discard the whole buffer. */
        td->currTsIndex = 0U;
    }

    /* Hand out all tsPerAvtpdu at once, one CRF AVTPDU worth. */
    if (td->currTsIndex == inst->params.tsPerAvtpdu)
    {
        (void)memcpy(outTs, td->arrayTs, sizeof(td->arrayTs));
        (void)memset(td->arrayTs, 0, sizeof(td->arrayTs));
        td->currTsIndex = 0U;
        bufferFull = true;
    }

    return bufferFull;
}
