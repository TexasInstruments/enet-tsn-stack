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

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <string.h>
#include <kernel/dpl/CacheP.h>
#include <kernel/dpl/HwiP.h>
#include <drivers/spinlock.h>
#include "shm_idxbuf.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define SHMIDXBUF_MAGIC             (0x58444249U)  /* 'IDBX' - marks a valid table header */
#define SHMIDXBUF_VERSION           (1U)

/* Safe upper-bound cache line size covering both R5F (32B) and C7x (up to
 * 128B) L1/L2 lines, so the header and the frame data region never share a
 * cache line. */
#define SHMIDXBUF_CACHE_LINE_SIZE   (128U)

#define SHMIDXBUF_ROUND_UP(val, align)  ((((uint32_t)(val)) + ((uint32_t)(align) - 1U)) & ~((uint32_t)(align) - 1U))

/** \brief Table-wide header, lives at the very start of the shared region,
 *  immediately followed by capacityFrames * frameSize bytes of flat frame
 *  storage (no per-frame metadata -- validity of a window is decided purely
 *  from baseIndex/headIndex). */
typedef struct ShmIdxBuf_TableHdr
{
    uint32_t magic;
    uint32_t version;
    uint32_t frameSize;
    uint32_t capacityFrames;
    uint64_t baseIndex;              /*!< Absolute index of the oldest resident frame */
    uint64_t headIndex;              /*!< Absolute index one-past the newest resident frame */
    uint32_t count;                  /*!< headIndex - baseIndex, cached for readability */
    uint32_t initialized;            /*!< 0 until the first-ever put() (true cold start);
                                           stays 1 forever after, even across count==0 caused
                                           by a caught-up reader draining the buffer -- see
                                           ShmIdxBuf_put() */
    uint32_t spinlockBaseAddr;
    uint32_t lockNumber;
    uint32_t lockTimeoutIterations;
    uint32_t maxForwardJumpFrames;
    bool clearStats;
    uint32_t putCount;
    uint32_t getCount;
    uint32_t discardedFrameCount;
    uint32_t staleWriteCount;
    uint32_t gapFrameCount;
    uint32_t tooOldCount;
    uint32_t notReadyCount;
    uint32_t lockTimeoutCount;
    uint32_t invalidIndexCount;
    uint32_t tail;                   /*!< second magic, cheap corruption check */
} ShmIdxBuf_TableHdr;

/* ========================================================================== */
/*                          Static Helper Functions                           */
/* ========================================================================== */

static inline uint32_t ShmIdxBuf_headerSpan(void)
{
    return SHMIDXBUF_ROUND_UP(sizeof(ShmIdxBuf_TableHdr), SHMIDXBUF_CACHE_LINE_SIZE);
}

static inline bool ShmIdxBuf_isValidHandle(ShmIdxBuf_TableHdr *hdr)
{
    return (hdr != NULL) &&
           (hdr->magic == SHMIDXBUF_MAGIC) &&
           (hdr->tail == SHMIDXBUF_MAGIC) &&
           (hdr->version == SHMIDXBUF_VERSION) &&
           (hdr->capacityFrames != 0U);
}

/**
 * \brief Split a [startIndex, startIndex+numFrames) window into at most two
 *  contiguous slot spans within a capacityFrames-deep circular array.
 *
 * \param capacityFrames  Buffer depth in frames
 * \param startIndex      Absolute index of the first frame in the window
 * \param numFrames       Frames in the window (must be <= capacityFrames)
 * \param slot0  Out: starting slot (0..capacityFrames-1) of the first span
 * \param len0   Out: frame count of the first span (always > 0)
 * \param len1   Out: frame count of the second (wrapped) span; 0 if the
 *               window did not wrap
 */
static void ShmIdxBuf_wrapSpans(uint32_t capacityFrames, uint64_t startIndex, uint32_t numFrames,
                                 uint32_t *slot0, uint32_t *len0, uint32_t *len1)
{
    uint32_t framesToEnd;

    *slot0 = (uint32_t)(startIndex % (uint64_t)capacityFrames);
    framesToEnd = capacityFrames - *slot0;

    if (numFrames <= framesToEnd)
    {
        *len0 = numFrames;
        *len1 = 0U;
    }
    else
    {
        *len0 = framesToEnd;
        *len1 = numFrames - framesToEnd;
    }
}

/**
 * \brief Acquire the buffer's hardware lock, with a bounded number of retries.
 *
 * Also disables local interrupts for the duration the lock is held, so that
 * an ISR on *this* core cannot re-enter this module and spin forever trying
 * to take a lock this same core already holds (the HW spinlock has no idea
 * two requests came from the same CPU). See shm_rowbuf.c for the identical
 * pattern this is copied from.
 *
 * \param hdr      Table header
 * \param oldIntState  Out: interrupt state to pass to ShmIdxBuf_unlock()
 * \return SHMIDXBUF_OK on success, SHMIDXBUF_ERR_LOCK_TIMEOUT on timeout
 */
static ShmIdxBuf_Status ShmIdxBuf_lock(ShmIdxBuf_TableHdr *hdr, uintptr_t *oldIntState)
{
    uint32_t timeoutIter = hdr->lockTimeoutIterations;
    uint32_t tries = 0U;
    int32_t lockStatus;
    ShmIdxBuf_Status status = SHMIDXBUF_ERR_LOCK_TIMEOUT;

    *oldIntState = HwiP_disable();

    if (timeoutIter == 0U)
    {
        timeoutIter = SHMIDXBUF_DEFAULT_LOCK_TIMEOUT_ITER;
    }

    do
    {
        lockStatus = Spinlock_lock(hdr->spinlockBaseAddr, hdr->lockNumber);
        if (lockStatus == (int32_t)SPINLOCK_LOCK_STATUS_FREE)
        {
            status = SHMIDXBUF_OK;
            break;
        }
        tries++;
    } while (tries < timeoutIter);

    if (status != SHMIDXBUF_OK)
    {
        HwiP_restore(*oldIntState);
    }

    return status;
}

static void ShmIdxBuf_unlock(ShmIdxBuf_TableHdr *hdr, uintptr_t oldIntState)
{
    Spinlock_unlock(hdr->spinlockBaseAddr, hdr->lockNumber);
    HwiP_restore(oldIntState);
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

uint32_t ShmIdxBuf_getRequiredSize(uint32_t capacityFrames, uint32_t frameSize)
{
    uint32_t result = 0U;

    if ((capacityFrames != 0U) && (frameSize != 0U))
    {
        result = ShmIdxBuf_headerSpan() + (capacityFrames * frameSize);
    }

    return result;
}

ShmIdxBuf_Handle ShmIdxBuf_create(const ShmIdxBuf_Config *cfg)
{
    ShmIdxBuf_TableHdr *hdr = NULL;
    uint32_t requiredSize = 0U;
    ShmIdxBuf_Handle result = NULL;

    if ((cfg != NULL) && (cfg->startAddr != 0U) &&
        (cfg->capacityFrames != 0U) && (cfg->frameSize != 0U))
    {
        requiredSize = ShmIdxBuf_getRequiredSize(cfg->capacityFrames,
                                                  cfg->frameSize);
        if ((requiredSize != 0U) && (cfg->regionSize >= requiredSize))
        {
            hdr = (ShmIdxBuf_TableHdr *)(uintptr_t)cfg->startAddr;

            /* Refresh our view of the header before deciding whether it is
             * already a valid, live buffer (the other side may have created
             * it already). */
            CacheP_inv((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                       CacheP_TYPE_ALL);

            if (cfg->forceInit || (!ShmIdxBuf_isValidHandle(hdr)) ||
                (hdr->capacityFrames != cfg->capacityFrames) ||
                (hdr->frameSize != cfg->frameSize))
            {
                uint8_t *dataBase = (uint8_t *)hdr +
                                     ShmIdxBuf_headerSpan();

                memset(dataBase, 0,
                       (size_t)cfg->capacityFrames * cfg->frameSize);

                hdr->frameSize             = cfg->frameSize;
                hdr->capacityFrames        = cfg->capacityFrames;
                hdr->baseIndex             = 0U;
                hdr->headIndex             = 0U;
                hdr->count                = 0U;
                hdr->initialized           = 0U;
                hdr->spinlockBaseAddr      = cfg->spinlockBaseAddr;
                hdr->lockNumber            = cfg->lockNumber;
                hdr->lockTimeoutIterations =
                    (cfg->lockTimeoutIterations != 0U) ?
                    cfg->lockTimeoutIterations :
                    SHMIDXBUF_DEFAULT_LOCK_TIMEOUT_ITER;
                hdr->maxForwardJumpFrames  = cfg->maxForwardJumpFrames;
                hdr->clearStats            = cfg->clearStatsOnRead;
                hdr->putCount              = 0U;
                hdr->getCount              = 0U;
                hdr->discardedFrameCount   = 0U;
                hdr->staleWriteCount       = 0U;
                hdr->gapFrameCount         = 0U;
                hdr->tooOldCount           = 0U;
                hdr->notReadyCount         = 0U;
                hdr->lockTimeoutCount      = 0U;
                hdr->invalidIndexCount     = 0U;
                /* magic/tail go in last so a half-built header never looks valid
                 * to the other side. */
                hdr->version               = SHMIDXBUF_VERSION;
                hdr->magic                 = SHMIDXBUF_MAGIC;
                hdr->tail                  = SHMIDXBUF_MAGIC;

                CacheP_wb((void *)hdr, requiredSize, CacheP_TYPE_ALL);
            }

            result = (ShmIdxBuf_Handle)hdr;
        }
    }

    return result;
}

ShmIdxBuf_Status ShmIdxBuf_put(ShmIdxBuf_Handle handle, uint64_t startIndex,
                                const void *data, uint32_t numFrames)
{
    ShmIdxBuf_TableHdr *hdr = (ShmIdxBuf_TableHdr *)handle;
    ShmIdxBuf_Status lockStatus;
    ShmIdxBuf_Status result = SHMIDXBUF_OK;
    uintptr_t intKey;
    uint8_t *dataBase;
    uint64_t endIndex;
    uint32_t slot0, len0, len1;
    bool locked = false;

    if ((hdr == NULL) || (data == NULL))
    {
        result = SHMIDXBUF_ERR_NULL_PARAM;
    }
    else if (!ShmIdxBuf_isValidHandle(hdr))
    {
        result = SHMIDXBUF_ERR_BAD_HANDLE;
    }
    else if ((numFrames == 0U) || (numFrames > hdr->capacityFrames))
    {
        result = SHMIDXBUF_ERR_BAD_SIZE;
    }

    if (result == SHMIDXBUF_OK)
    {
        lockStatus = ShmIdxBuf_lock(hdr, &intKey);
        if (lockStatus != SHMIDXBUF_OK)
        {
            hdr->lockTimeoutCount++;
            result = lockStatus;
        }
        else
        {
            locked = true;
        }
    }

    if (locked)
    {
        bool rejected = false;

        CacheP_inv((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                   CacheP_TYPE_ALL);

        dataBase  = (uint8_t *)hdr + ShmIdxBuf_headerSpan();
        endIndex  = startIndex + (uint64_t)numFrames;

        if ((hdr->count == 0U) && (hdr->initialized != 0U) &&
            (hdr->maxForwardJumpFrames != 0U) && (startIndex > hdr->headIndex) &&
            ((startIndex - hdr->headIndex) > (uint64_t)hdr->maxForwardJumpFrames))
        {
            /* count==0 here does NOT mean true cold start (initialized != 0) --
             * it means a caught-up reader has drained the buffer via get()'s
             * drain-on-read. Without this check, a caught-up buffer would
             * re-anchor on whatever startIndex arrives next with no sanity
             * check at all, reopening exactly the hole maxForwardJumpFrames
             * exists to close (a corrupted index landing right when the
             * buffer happens to be empty). Reject it the same way the
             * steady-state case below does; buffer state left untouched. */
            hdr->invalidIndexCount++;
            CacheP_wb((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                       CacheP_TYPE_ALL);
            result = SHMIDXBUF_OK;
            rejected = true;
        }
        else if (hdr->count == 0U)
        {
            /* True cold start (never initialized -- nothing to sanity-check
             * against yet), or a legitimate continuation after the reader
             * fully drained the buffer: this call (re-)anchors where the
             * buffer starts. */
            hdr->baseIndex   = startIndex;
            hdr->headIndex   = startIndex;
            hdr->initialized = 1U;
        }
        else if ((endIndex <= hdr->baseIndex) || (startIndex < hdr->baseIndex))
        {
            /* Behind the current anchor -- normally this is a late/duplicate
             * arrival and would just be a stale no-op. But it's also exactly
             * what happens after the anchor itself is corrupted (e.g. a bogus
             * cold-start index): every subsequent legitimate write would look
             * "behind" a base that's wrong, and get silently dropped forever.
             * Re-anchor to the new write instead of dropping it -- newer data
             * must always win over whatever the buffer currently holds. */
            hdr->staleWriteCount++;
            hdr->baseIndex = startIndex;
            hdr->headIndex = startIndex;
        }
        else if ((hdr->maxForwardJumpFrames != 0U) &&
                 (startIndex > hdr->headIndex) &&
                 ((startIndex - hdr->headIndex) >
                  (uint64_t)hdr->maxForwardJumpFrames))
        {
            /* startIndex jumps further ahead of headIndex than any legitimate
             * gap could -- almost certainly a corrupted/miscomputed index
             * (e.g. an unsigned-subtraction underflow upstream) rather than
             * real packet loss. Reject outright instead of treating it as a
             * gap: accepting it would evict every valid frame and permanently
             * poison baseIndex/headIndex with a bogus value. Buffer state is
             * left completely untouched. */
            hdr->invalidIndexCount++;
            CacheP_wb((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                       CacheP_TYPE_ALL);
            result = SHMIDXBUF_OK;
            rejected = true;
        }

        if (!rejected)
        {
            if (startIndex > hdr->headIndex)
            {
                /* Producer gap (e.g. a lost packet): zero-fill the exposed-but-
                 * never-written region so a later get() into the hole returns
                 * defined silence, not stale bytes from a previous wrap cycle.
                 * Bound the fill to at most capacityFrames frames back from
                 * startIndex -- anything older than that is moot, it gets
                 * evicted by the headIndex/baseIndex advance below regardless.
                 */
                uint64_t gap       = startIndex - hdr->headIndex;
                uint32_t fillCount = (gap > (uint64_t)hdr->capacityFrames) ?
                                      hdr->capacityFrames : (uint32_t)gap;
                uint64_t fillStart = startIndex - (uint64_t)fillCount;
                uint32_t fSlot0, fLen0, fLen1;

                ShmIdxBuf_wrapSpans(hdr->capacityFrames, fillStart,
                                     fillCount, &fSlot0, &fLen0, &fLen1);
                memset(dataBase +
                       ((uintptr_t)fSlot0 * hdr->frameSize),
                       0,
                       (size_t)fLen0 * hdr->frameSize);
                CacheP_wb((void *)(dataBase +
                                   ((uintptr_t)fSlot0 * hdr->frameSize)),
                           (size_t)fLen0 * hdr->frameSize,
                           CacheP_TYPE_ALL);
                if (fLen1 != 0U)
                {
                    memset(dataBase, 0,
                           (size_t)fLen1 * hdr->frameSize);
                    CacheP_wb((void *)dataBase,
                               (size_t)fLen1 * hdr->frameSize,
                               CacheP_TYPE_ALL);
                }
                /* Count what was actually zero-filled (fillCount), not the raw
                 * gap: the fill is capped at capacityFrames and the raw gap
                 * is a uint64_t that would truncate. */
                hdr->gapFrameCount += fillCount;
            }

            if (endIndex > hdr->headIndex)
            {
                hdr->headIndex = endIndex;
            }
            if ((hdr->headIndex - hdr->baseIndex) >
                (uint64_t)hdr->capacityFrames)
            {
                uint64_t newBase = hdr->headIndex -
                                    (uint64_t)hdr->capacityFrames;
                hdr->discardedFrameCount +=
                    (uint32_t)(newBase - hdr->baseIndex);
                hdr->baseIndex = newBase;
            }
            hdr->count = (uint32_t)(hdr->headIndex - hdr->baseIndex);

            ShmIdxBuf_wrapSpans(hdr->capacityFrames, startIndex,
                                 numFrames, &slot0, &len0, &len1);
            memcpy(dataBase + ((uintptr_t)slot0 * hdr->frameSize),
                   data,
                   (size_t)len0 * hdr->frameSize);
            CacheP_wb((void *)(dataBase +
                               ((uintptr_t)slot0 * hdr->frameSize)),
                       (size_t)len0 * hdr->frameSize,
                       CacheP_TYPE_ALL);
            if (len1 != 0U)
            {
                memcpy(dataBase,
                       (const uint8_t *)data +
                       ((size_t)len0 * hdr->frameSize),
                       (size_t)len1 * hdr->frameSize);
                CacheP_wb((void *)dataBase,
                           (size_t)len1 * hdr->frameSize,
                           CacheP_TYPE_ALL);
            }

            hdr->putCount++;
            CacheP_wb((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                       CacheP_TYPE_ALL);
        }

        ShmIdxBuf_unlock(hdr, intKey);
    }

    return result;
}

ShmIdxBuf_Status ShmIdxBuf_get(ShmIdxBuf_Handle handle, uint64_t startIndex,
                                uint32_t numFrames, void *dst,
                                uint64_t *outBaseIndex)
{
    ShmIdxBuf_TableHdr *hdr = (ShmIdxBuf_TableHdr *)handle;
    ShmIdxBuf_Status lockStatus;
    ShmIdxBuf_Status result = SHMIDXBUF_OK;
    uintptr_t intKey;
    uint8_t *dataBase;
    uint64_t endIndex;
    bool locked = false;

    if ((hdr == NULL) || (dst == NULL) || (outBaseIndex == NULL))
    {
        result = SHMIDXBUF_ERR_NULL_PARAM;
    }
    else if (!ShmIdxBuf_isValidHandle(hdr))
    {
        result = SHMIDXBUF_ERR_BAD_HANDLE;
    }
    else if ((numFrames == 0U) || (numFrames > hdr->capacityFrames))
    {
        result = SHMIDXBUF_ERR_BAD_SIZE;
    }

    if (result == SHMIDXBUF_OK)
    {
        lockStatus = ShmIdxBuf_lock(hdr, &intKey);
        if (lockStatus != SHMIDXBUF_OK)
        {
            hdr->lockTimeoutCount++;
            *outBaseIndex = 0U;
            result = lockStatus;
        }
        else
        {
            locked = true;
        }
    }

    if (locked)
    {
        CacheP_inv((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                   CacheP_TYPE_ALL);

        dataBase       = (uint8_t *)hdr + ShmIdxBuf_headerSpan();
        endIndex       = startIndex + (uint64_t)numFrames;
        *outBaseIndex  = hdr->baseIndex;

        if (hdr->count == 0U)
        {
            /* Nothing produced yet at all. */
            result = SHMIDXBUF_ERR_NOT_READY;
            hdr->notReadyCount++;
        }
        else if ((endIndex <= hdr->baseIndex) ||
                 (startIndex < hdr->baseIndex))
        {
            /* Fully, or partially at the old edge, already evicted. */
            result = SHMIDXBUF_ERR_TOO_OLD;
            hdr->tooOldCount++;
        }
        else if ((startIndex >= hdr->headIndex) ||
                 (endIndex > hdr->headIndex))
        {
            /* Fully, or partially at the new edge, not produced yet. */
            result = SHMIDXBUF_ERR_NOT_READY;
            hdr->notReadyCount++;
        }
        else
        {
            uint32_t slot0, len0, len1;

            ShmIdxBuf_wrapSpans(hdr->capacityFrames, startIndex,
                                 numFrames, &slot0, &len0, &len1);

            CacheP_inv((void *)(dataBase +
                               ((uintptr_t)slot0 * hdr->frameSize)),
                       (size_t)len0 * hdr->frameSize,
                       CacheP_TYPE_ALL);
            memcpy(dst,
                   dataBase + ((uintptr_t)slot0 * hdr->frameSize),
                   (size_t)len0 * hdr->frameSize);
            if (len1 != 0U)
            {
                CacheP_inv((void *)dataBase,
                           (size_t)len1 * hdr->frameSize,
                           CacheP_TYPE_ALL);
                memcpy((uint8_t *)dst +
                       ((size_t)len0 * hdr->frameSize),
                       dataBase,
                       (size_t)len1 * hdr->frameSize);
            }

            /* Drain on read: this window has now been delivered to the
             * (single) reader and will never be requested again, so it is
             * gone -- advance baseIndex past it. endIndex > baseIndex
             * always holds here (this branch is only reached when
             * baseIndex <= startIndex < endIndex), so this is a plain
             * advance, never a decrease. */
            hdr->baseIndex = endIndex;
            hdr->count     = (uint32_t)(hdr->headIndex - hdr->baseIndex);

            hdr->getCount++;
            result = SHMIDXBUF_OK;
        }

        CacheP_wb((void *)hdr, sizeof(ShmIdxBuf_TableHdr), CacheP_TYPE_ALL);
        ShmIdxBuf_unlock(hdr, intKey);
    }

    return result;
}

ShmIdxBuf_Status ShmIdxBuf_putFifo(ShmIdxBuf_Handle handle, const void *data,
                                    uint32_t numFrames)
{
    ShmIdxBuf_TableHdr *hdr = (ShmIdxBuf_TableHdr *)handle;
    ShmIdxBuf_Status result = SHMIDXBUF_OK;

    if (hdr == NULL)
    {
        result = SHMIDXBUF_ERR_NULL_PARAM;
    }
    else if (!ShmIdxBuf_isValidHandle(hdr))
    {
        result = SHMIDXBUF_ERR_BAD_HANDLE;
    }

    if (result == SHMIDXBUF_OK)
    {
        /* Unlocked read is fine, only the producer (us) ever moves headIndex. */
        result = ShmIdxBuf_put(handle, hdr->headIndex, data, numFrames);
    }

    return result;
}

ShmIdxBuf_Status ShmIdxBuf_getFifo(ShmIdxBuf_Handle handle, void *dst,
                                    uint32_t numFrames,
                                    uint64_t *outBaseIndex)
{
    ShmIdxBuf_TableHdr *hdr = (ShmIdxBuf_TableHdr *)handle;
    ShmIdxBuf_Status result = SHMIDXBUF_OK;

    if (hdr == NULL)
    {
        result = SHMIDXBUF_ERR_NULL_PARAM;
    }
    else if (!ShmIdxBuf_isValidHandle(hdr))
    {
        result = SHMIDXBUF_ERR_BAD_HANDLE;
    }

    if (result == SHMIDXBUF_OK)
    {
        /* Unlocked peek at baseIndex; get() re-checks it under the lock, so a
         * racing eviction just comes back as TOO_OLD. */
        result = ShmIdxBuf_get(handle, hdr->baseIndex, numFrames,
                               dst, outBaseIndex);
    }

    return result;
}

void ShmIdxBuf_getStats(ShmIdxBuf_Handle handle, ShmIdxBuf_Stats *stats)
{
    ShmIdxBuf_TableHdr *hdr = (ShmIdxBuf_TableHdr *)handle;
    uintptr_t intKey;
    bool locked;

    if ((hdr == NULL) || (stats == NULL) ||
        (!ShmIdxBuf_isValidHandle(hdr)))
    {
        /* Nothing to report; single exit point preserved. */
    }
    else
    {
        locked = (ShmIdxBuf_lock(hdr, &intKey) == SHMIDXBUF_OK);

        /* Even if the lock timed out, best-effort report a fresh snapshot.
         * Counters are only for diagnostics, so a torn read is acceptable. */
        CacheP_inv((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                   CacheP_TYPE_ALL);
        stats->putCount            = hdr->putCount;
        stats->getCount            = hdr->getCount;
        stats->discardedFrameCount = hdr->discardedFrameCount;
        stats->staleWriteCount     = hdr->staleWriteCount;
        stats->gapFrameCount       = hdr->gapFrameCount;
        stats->tooOldCount         = hdr->tooOldCount;
        stats->notReadyCount       = hdr->notReadyCount;
        stats->lockTimeoutCount    = hdr->lockTimeoutCount;
        stats->invalidIndexCount   = hdr->invalidIndexCount;
        stats->baseIndex           = hdr->baseIndex;
        stats->headIndex           = hdr->headIndex;

        if (hdr->clearStats)
        {
            hdr->putCount            = 0;
            hdr->getCount            = 0;
            hdr->discardedFrameCount = 0;
            hdr->staleWriteCount     = 0;
            hdr->gapFrameCount       = 0;
            hdr->tooOldCount         = 0;
            hdr->notReadyCount       = 0;
            hdr->lockTimeoutCount    = 0;
            hdr->invalidIndexCount   = 0;
            CacheP_wb((void *)hdr, sizeof(ShmIdxBuf_TableHdr),
                       CacheP_TYPE_ALL);
        }

        if (locked)
        {
            ShmIdxBuf_unlock(hdr, intKey);
        }
    }
}
