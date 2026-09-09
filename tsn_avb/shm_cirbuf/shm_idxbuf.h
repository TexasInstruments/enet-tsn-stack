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

/**
 * \file shm_idxbuf.h
 *
 * \brief Index-addressed circular buffer for random-access frame retrieval.
 *
 * Unlike shm_rowbuf.c (a FIFO row-table -- the reader can only ever ask for
 * "whatever's next"), this module stores a flat circular array of
 * fixed-size "frames" and lets the reader request an arbitrary window by
 * absolute frame index: "give me numFrames frames starting at index X."
 *
 * The producer tags every write with the absolute index of its first frame
 * (e.g. a media-clock edge count). The consumer tracks its own expected
 * index and asks for exactly that window every time, rather than trusting
 * FIFO order -- this lets the consumer detect and resynchronize past a hole
 * (ShmIdxBuf_get() returns SHMIDXBUF_ERR_TOO_OLD and hands back the buffer's
 * current oldest-resident index) instead of silently drifting forever.
 *
 * Layout: one table header followed by `capacityFrames * frameSize` bytes
 * of frame storage, no per-frame metadata (unlike shm_rowbuf's per-row
 * status/sequence word) -- validity of a window is determined purely from
 * the header's baseIndex/headIndex range, checked by the caller-visible
 * status code on every ShmIdxBuf_get() call. The buffer drains on read:
 * baseIndex advances past every window a get() successfully delivers, not
 * just when put() evicts on capacity overflow -- headIndex - baseIndex is
 * therefore the real live gap between producer and consumer, not a
 * constant pinned at capacityFrames.
 *
 * Same concurrency model as shm_rowbuf.c: a single writer, a single reader,
 * serialized with a HW spinlock (source/drivers/spinlock) while local
 * interrupts are disabled for the (short, bounded) critical section. In a
 * single-core deployment (writer = task context, reader = ISR context, same
 * CPU) the spinlock always resolves immediately and HwiP_disable() alone is
 * what actually provides the mutual exclusion; the spinlock (and the
 * CacheP_wb/CacheP_inv calls around shared-field access) are kept anyway so
 * this module could be reused unmodified by a genuinely 2-core deployment.
 */

#ifndef SHM_IDXBUF_H_
#define SHM_IDXBUF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================== */
/*                             Macros & Typedefs                              */
/* ========================================================================== */

/** \brief Opaque handle to a created index buffer instance. */
typedef void *ShmIdxBuf_Handle;

/** \brief Status/error codes returned by this module. */
typedef enum ShmIdxBuf_Status
{
    SHMIDXBUF_OK                = 0,   /*!< Success */
    SHMIDXBUF_ERR_NULL_PARAM    = -1,  /*!< A required pointer/handle was NULL */
    SHMIDXBUF_ERR_BAD_SIZE      = -2,  /*!< numFrames was 0, exceeded capacityFrames,
                                             or region/buffer supplied was too small */
    SHMIDXBUF_ERR_BAD_HANDLE    = -3,  /*!< Handle does not point at a valid/initialized table */
    SHMIDXBUF_ERR_LOCK_TIMEOUT  = -4,  /*!< Could not acquire the hardware lock in time */
    SHMIDXBUF_ERR_TOO_OLD       = -5,  /*!< get(): requested window is (partially) already
                                             evicted -- some/all of it will never be available */
    SHMIDXBUF_ERR_NOT_READY     = -6,  /*!< get(): requested window is (partially) not
                                             produced yet -- retry later */
} ShmIdxBuf_Status;

/**
 * \brief Configuration used to create (or attach to) an index buffer.
 *
 * Both producer and consumer must be given the *same* startAddr (translated
 * to each core's own local address space, if ever used cross-core), the
 * same frameSize/capacityFrames, and the same spinlockBaseAddr/lockNumber.
 */
typedef struct ShmIdxBuf_Config
{
    uint32_t startAddr;              /*!< Local-core address of the shared memory region */
    uint32_t regionSize;             /*!< Total bytes available at startAddr for this buffer.
                                           Must be >= ShmIdxBuf_getRequiredSize(capacityFrames, frameSize) */
    uint32_t frameSize;               /*!< Bytes per frame (one index unit), must be >= 1 */
    uint32_t capacityFrames;         /*!< Table depth in frames, must be >= 1 */
    uint32_t spinlockBaseAddr;       /*!< HW spinlock module base address (e.g. CSL_SPINLOCK0_BASE) */
    uint32_t lockNumber;             /*!< HW lock number reserved for this buffer.
                                           Must be unique per buffer/direction. */
    uint32_t lockTimeoutIterations;  /*!< Max spin retries before giving up on the lock.
                                           0 = use SHMIDXBUF_DEFAULT_LOCK_TIMEOUT_ITER */
    uint32_t maxForwardJumpFrames;   /*!< Sanity cap on put(): reject (see ShmIdxBuf_put()) any
                                           startIndex that would advance the buffer's headIndex by
                                           more than this many frames in one call. Guards against a
                                           producer-side index bug (e.g. an unsigned-subtraction
                                           underflow) injecting an astronomically large index that
                                           would otherwise instantly evict all valid data and
                                           permanently desync the consumer. Does not affect the
                                           first-ever put() (which defines the buffer's origin) or
                                           legitimate small gaps (real packet loss). Pick a value
                                           comfortably larger than the worst realistic gap your
                                           producer can legitimately skip. 0 = no cap (disabled). */
    bool     forceInit;              /*!< true = always (re)initialize even if it already looks
                                           valid. false = only initialize if not already valid
                                           (safe for both sides to call create() at boot). */
    bool     clearStatsOnRead;       /*!< true = counters reset to 0 after each getStats() snapshot */
} ShmIdxBuf_Config;

/** \brief Runtime statistics/counters for diagnostics and tests. */
typedef struct ShmIdxBuf_Stats
{
    uint32_t putCount;              /*!< Total put() calls that wrote data */
    uint32_t getCount;              /*!< Total get() calls that returned SHMIDXBUF_OK */
    uint32_t discardedFrameCount;   /*!< Frames evicted by capacity overflow */
    uint32_t staleWriteCount;       /*!< put() windows dropped as entirely/partially too old */
    uint32_t gapFrameCount;         /*!< Frames zero-filled to bridge a producer gap */
    uint32_t tooOldCount;           /*!< get() calls that returned SHMIDXBUF_ERR_TOO_OLD */
    uint32_t notReadyCount;         /*!< get() calls that returned SHMIDXBUF_ERR_NOT_READY */
    uint32_t lockTimeoutCount;      /*!< Times the HW lock could not be acquired in time */
    uint32_t invalidIndexCount;     /*!< put() windows dropped for exceeding maxForwardJumpFrames */
    uint64_t baseIndex;             /*!< Instantaneous snapshot: current oldest-resident index */
    uint64_t headIndex;             /*!< Instantaneous snapshot: current one-past-newest index */
} ShmIdxBuf_Stats;

/** \brief Default number of spin iterations before a lock attempt times out. */
#define SHMIDXBUF_DEFAULT_LOCK_TIMEOUT_ITER   (200000U)

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief Compute how many bytes of shared memory a buffer needs.
 *
 * \param capacityFrames  Desired buffer depth, in frames
 * \param frameSize       Desired bytes per frame
 * \return Required region size in bytes, or 0 if either argument is 0
 */
uint32_t ShmIdxBuf_getRequiredSize(uint32_t capacityFrames,
                                           uint32_t frameSize);

/**
 * \brief Create (or attach to) an index buffer at the given shared memory address.
 *
 * If a valid buffer (matching magic/version) already exists at startAddr and
 * forceInit is false, the existing buffer is left untouched and a handle to
 * it is returned -- this lets both the producer and consumer call create()
 * during boot without one of them wiping data the other already
 * produced/consumed.
 *
 * \param cfg  Buffer configuration. Not retained by pointer; contents are
 *             copied into the table header in shared memory.
 * \return Handle on success, NULL on invalid configuration.
 */
ShmIdxBuf_Handle ShmIdxBuf_create(const ShmIdxBuf_Config *cfg);

/**
 * \brief Write numFrames frames of data starting at the given absolute index.
 *
 * Never blocks and never fails on data content. If the window would overflow
 * capacity, the oldest frames are evicted (ShmIdxBuf_Stats.discardedFrameCount).
 * If the window is entirely/partially before the buffer's current oldest
 * resident frame, it is silently dropped as stale
 * (ShmIdxBuf_Stats.staleWriteCount) -- this is not a caller error. If the
 * window starts after the buffer's current newest frame (a producer gap,
 * e.g. a lost packet), the skipped frames are zero-filled
 * (ShmIdxBuf_Stats.gapFrameCount) so a later get() into that gap returns
 * defined silence rather than stale leftover bytes. If the window starts
 * more than maxForwardJumpFrames past the buffer's current newest frame
 * (see ShmIdxBuf_Config), it is rejected outright as a sanity failure
 * (ShmIdxBuf_Stats.invalidIndexCount) instead of being treated as a gap --
 * this is what actually protects the buffer from a corrupted index.
 *
 * \param handle      Handle from ShmIdxBuf_create()
 * \param startIndex  Absolute index of the first frame in data
 * \param data        Source buffer, exactly numFrames * frameSize bytes
 * \param numFrames   Number of frames to write; must be in [1, capacityFrames]
 * \return SHMIDXBUF_OK, or an error code (only for bad parameters/lock timeout)
 */
ShmIdxBuf_Status ShmIdxBuf_put(ShmIdxBuf_Handle handle,
                                       uint64_t startIndex,
                                       const void *data, uint32_t numFrames);

/**
 * \brief Read numFrames frames of data starting at the given absolute index.
 *
 * Drains on read: a successful read advances baseIndex past the returned
 * window (baseIndex = startIndex + numFrames), since this module is
 * single-writer/single-reader and a delivered window will never be
 * requested again. This means SHMIDXBUF_ERR_TOO_OLD covers two distinct
 * causes that both mean "gone": the window was evicted by put() overflow
 * (writer outran capacity), or it was already delivered by a prior get()
 * call (reader re-requesting/going backward). put()'s capacity-overflow
 * eviction remains as the only safety net for a stalled reader (one that
 * stops calling get() while the writer keeps going).
 *
 * \param handle       Handle from ShmIdxBuf_create()
 * \param startIndex   Absolute index of the first frame requested
 * \param numFrames    Number of frames requested; must be in [1, capacityFrames]
 * \param dst          Destination buffer, at least numFrames * frameSize bytes
 * \param outBaseIndex Out: the buffer's current oldest-resident absolute index
 *                      (after this call's own advance-on-success, if any),
 *                      written on every call (including error returns) so the
 *                      caller can resynchronize a drifted expected-index
 *                      counter without a second call.
 * \return SHMIDXBUF_OK if the full window was resident and copied out,
 *         SHMIDXBUF_ERR_TOO_OLD if (part of) the window was already evicted
 *         or already delivered by a prior read,
 *         SHMIDXBUF_ERR_NOT_READY if (part of) the window is not produced yet,
 *         or another error code.
 */
ShmIdxBuf_Status ShmIdxBuf_get(ShmIdxBuf_Handle handle,
                                       uint64_t startIndex,
                                       uint32_t numFrames, void *dst,
                                       uint64_t *outBaseIndex);

/**
 * \brief FIFO-mode put: append numFrames frames at the buffer's current
 *  head (bottom), without the caller tracking an absolute index.
 *
 * Equivalent to ShmIdxBuf_put(handle, <current headIndex>, data, numFrames).
 * Safe without an extra lock: headIndex is only ever advanced by put(), so
 * on the single producer that calls this function, the value observed here
 * cannot be stale relative to that same producer's own prior calls.
 *
 * \param handle     Handle from ShmIdxBuf_create()
 * \param data       Source buffer, exactly numFrames * frameSize bytes
 * \param numFrames  Number of frames to write; must be in [1, capacityFrames]
 * \return SHMIDXBUF_OK, or an error code (only for bad parameters/lock timeout)
 */
ShmIdxBuf_Status ShmIdxBuf_putFifo(ShmIdxBuf_Handle handle,
                                           const void *data,
                                           uint32_t numFrames);

/**
 * \brief FIFO-mode get: read numFrames frames from the buffer's current top
 *  (oldest resident, i.e. baseIndex), without the caller tracking an
 *  absolute index. Drains on read like ShmIdxBuf_get(), so each call
 *  returns the next frames in produced order.
 *
 * Equivalent to ShmIdxBuf_get(handle, <current baseIndex>, numFrames, dst,
 * outBaseIndex). If the producer evicts frames (capacity overflow) between
 * this function reading baseIndex and the underlying get() re-checking it
 * under lock, the call safely reports SHMIDXBUF_ERR_TOO_OLD with a fresh
 * outBaseIndex instead of returning stale data -- simply call again.
 *
 * \param handle       Handle from ShmIdxBuf_create()
 * \param dst          Destination buffer, at least numFrames * frameSize bytes
 * \param numFrames    Number of frames requested; must be in [1, capacityFrames]
 * \param outBaseIndex Out: the buffer's current oldest-resident absolute index,
 *                      written on every call (including error returns)
 * \return SHMIDXBUF_OK if the full window was resident and copied out,
 *         SHMIDXBUF_ERR_TOO_OLD if the oldest frames were evicted between the
 *         read of baseIndex and the locked check -- retry,
 *         SHMIDXBUF_ERR_NOT_READY if nothing new has been produced yet,
 *         or another error code.
 */
ShmIdxBuf_Status ShmIdxBuf_getFifo(ShmIdxBuf_Handle handle,
                                           void *dst, uint32_t numFrames,
                                           uint64_t *outBaseIndex);

/**
 * \brief Snapshot the buffer's runtime statistics/counters.
 *
 * \param handle  Handle from ShmIdxBuf_create()
 * \param stats   Out: filled with the current counters
 */
void ShmIdxBuf_getStats(ShmIdxBuf_Handle handle,
                                ShmIdxBuf_Stats *stats);

#ifdef __cplusplus
}
#endif

#endif /* SHM_IDXBUF_H_ */
