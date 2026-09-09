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

#include <string.h>
#include <inttypes.h>
#include <kernel/dpl/SemaphoreP.h>
#include <kernel/dpl/SystemP.h>
#include <kernel/dpl/ClockP.h>
#include <enet_types.h>
#include <drivers/hw_include/cslr_soc.h>
#include <tsn_combase/cb_thread.h>
#include <tsn_unibase/unibase.h>
#include "shm_idxbuf.h"
#include "clock_rec.h"
#include "crf_task.h"
#include "tsn_gptp/gptpmasterclock.h"
#include "avb_if.h"

/* AAF samples go on the wire big-endian but the cores are little-endian, so
 * every word gets swapped. R5F has a REV instruction, other cores use shifts. */
#if defined(__ARM_ARCH_7R__)
#include "arm_acle.h"
static inline uint32_t AvbIf_swap32(uint32_t v)
{
    return __rev(v);
}
#else
static inline uint32_t AvbIf_swap32(uint32_t v)
{
    return ((v & 0x000000FFU) << 24U) | ((v & 0x0000FF00U) <<  8U) |
           ((v & 0x00FF0000U) >>  8U) | ((v & 0xFF000000U) >> 24U);
}
#endif

/* Byte-swap each 32-bit word in place; byteLen must be a multiple of 4
 * (guaranteed by AVB_IF requiring 32-bit PCM). */
static inline void AvbIf_swap32Buffer(void *buf, uint32_t byteLen)
{
    uint32_t *p = (uint32_t *)buf;
    uint32_t count = byteLen / (uint32_t)sizeof(uint32_t);
    uint32_t i;

    for (i = 0U; i < count; i++)
    {
        p[i] = AvbIf_swap32(p[i]);
    }
}

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define AVB_IF_TASK_PRIORITY_DEFAULT     (16)
#define AVB_IF_TASK_STACK_SIZE_DEFAULT   (8U * 1024U)

/* Diagnostic-only: low priority, small stack, coarse period - never on the
 * audio-timing-critical path. */
#define AVB_IF_STATS_LOG_TASK_PRIORITY     (4)
#define AVB_IF_STATS_LOG_TASK_STACK_SIZE   (2U * 1024U)
#define AVB_IF_STATS_LOG_PERIOD_US         (100000U)

typedef struct
{
    AvbIf_TalkerConfig      config;
    aaf_avtpc_talker_data_t *talkerHandle;
    ShmIdxBuf_Handle         shmHandle;
    uint32_t                 frameSize;      /* bytes per sample-frame */
    uint32_t                 pduSize;        /* bytes per AVTPDU payload */
    uint32_t                 samplesPerPdu;  /* sample-frames per AVTPDU payload */
    uint8_t                  copyBuf[AVB_IF_MAX_PDU_BYTES] __attribute__((aligned(4)));
}AvbIf_TalkerSlot;

/* Listener dispatch: full 8-byte streamID -> AvbIf_listenerSlots index, via
 * an open-addressing (linear probing) hash table of AVB_IF_LISTENER_HASH_SIZE
 * buckets. Entries are only ever added, never removed, so no tombstones. */
#define AVB_IF_LISTENER_HASH_EMPTY      (-1)

typedef struct
{
    AvbIf_ListenerConfig   config;
    AvbIf_ListenerRxCallback rxCallback;  /* user callback, or AvbIf_shmRxCallback */
    void                    *rxCallbackArg;
    ShmIdxBuf_Handle         shmHandle;      /* NULL when a user rxCallback is set */
    uint32_t                 frameSize;   /* bytes per sample-frame */
    uint32_t                 pduSize;     /* expected bytes per received AVTPDU payload */
    uint8_t                  copyBuf[AVB_IF_MAX_PDU_BYTES] __attribute__((aligned(4)));
}AvbIf_ListenerSlot;

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static AvbIf_TalkerSlot   AvbIf_talkerSlots[AVB_IF_MAX_TALKERS];
/* Only ever grows; a slot is fully populated by AvbIf_registerTalker() before
 * this is incremented, so the task reading [0, AvbIf_numTalkers) concurrently
 * never observes a partially-built slot. AvbIf_registerTalker() itself is not
 * safe to call concurrently with itself (single-writer registration,
 * matching how every existing caller registers streams up front). */
static uint32_t            AvbIf_numTalkers = 0U;

/* Same growth/visibility contract as AvbIf_talkerSlots/AvbIf_numTalkers above, except
 * a listener slot is read from the shared dispatch callback (network stack
 * context) instead of the talker task. */
static AvbIf_ListenerSlot AvbIf_listenerSlots[AVB_IF_MAX_LISTENERS];
static uint32_t            AvbIf_numListeners = 0U;

/* One shared AVTP listener connection for every registered listener stream -
 * see AvbIf_initListenerTask(). NULL until AvbIf_initListenerTask() succeeds. */
static aaf_avtpc_listener_data_t *AvbIf_sharedListenerHandle = NULL;
/* bucket -> AvbIf_listenerSlots index, AVB_IF_LISTENER_HASH_EMPTY if free.
 * A bucket is written only after its slot is fully populated. */
static int16_t              AvbIf_listenerHash[AVB_IF_LISTENER_HASH_SIZE];

static SemaphoreP_Object    AvbIf_tickSem;
static bool                 AvbIf_initDone = false;

/* Fallback talker stack, used when the application does not supply one. Kept
 * static rather than taken from tsn_combase's shared pool, which only has a
 * few slots that other tasks (gPTP etc.) also draw from. */
static uint8_t AvbIf_talkerTaskStack[AVB_IF_TASK_STACK_SIZE_DEFAULT];

/* Started lazily, at most once, by whichever AvbIf_registerTalker()/
 * AvbIf_registerListener() call is the first to pass statsLogEnable=true. */
static bool    AvbIf_statsLogTaskStarted = false;
static uint8_t AvbIf_statslogTaskStack[AVB_IF_STATS_LOG_TASK_STACK_SIZE];

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void *AvbIf_taskMain(void *args);
static int32_t AvbIf_buildConl2Params(const char *netdev, int32_t priority, int32_t vlanID,
                                          const ub_streamid_t streamID, const ub_macaddr_t destMacAddr,
                                          uint32_t timeInterval_us, conl2_basic_conparas_t *bcp);
static int AvbIf_listenerDispatchCallback(uint8_t *payload, int plsize,
                                              avbtp_rcv_cb_info_t *cbinfo, void *cbdata);
static int32_t AvbIf_shmRxCallback(uint8_t *payload, int plsize,
                                       const avbtp_rcv_cb_info_t *cbinfo, void *cbArg);
static uint32_t AvbIf_hashStreamId(const ub_streamid_t streamID);
static int16_t AvbIf_hashLookup(const ub_streamid_t streamID);
static void AvbIf_hashInsert(const ub_streamid_t streamID, int16_t slotIdx);
static void *AvbIf_statsLogTaskMain(void *args);
static void AvbIf_ensureStatsLogTaskStarted(void);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static int32_t AvbIf_buildConl2Params(const char *netdev, int32_t priority, int32_t vlanID,
                                          const ub_streamid_t streamID, const ub_macaddr_t destMacAddr,
                                          uint32_t timeInterval_us, conl2_basic_conparas_t *bcp)
{
    memset(bcp, 0, sizeof(*bcp));

    bcp->vid       = (uint16_t)vlanID;
    bcp->time_intv = timeInterval_us;
    bcp->pcp       = (uint8_t)priority;

    memcpy(&bcp->mcastmac, destMacAddr, sizeof(ub_macaddr_t));
    memcpy(&bcp->streamid, streamID, sizeof(ub_streamid_t));
    strncpy(bcp->netdev, netdev, sizeof(bcp->netdev) - 1U);

    /* Hardcoded network defaults, matching every existing talker in this
     * codebase (central_aaf_app.c / autoamp_avb.c) - not yet exposed as
     * per-stream config since neither varies them. */
    bcp->max_frm_sz        = 1500U;
    bcp->max_intv_frames   = 1U;
    bcp->avtpd_bufftime_us = 10000U;
    bcp->is_direct         = true;
    bcp->send_ahead_ts     = 20000;

    return ENET_SOK;
}

int32_t AvbIf_registerTalker(const AvbIf_TalkerConfig *config, AvbIf_TalkerHandle *outHandle)
{
    int32_t status = ENET_SOK;
    conl2_basic_conparas_t conl2Params;
    aaf_avtpc_pcminfo_t    pcmInfo;
    AvbIf_TalkerSlot     *slot = NULL;
    ShmIdxBuf_Config       shmCfg;
    uint32_t               idx;

    if ((config == NULL) || (outHandle == NULL))
    {
        UB_LOG(UBL_ERROR, "%s: NULL config/outHandle\r\n", __func__);
        status = ENET_EFAIL;
    }
    else if (AvbIf_numTalkers >= AVB_IF_MAX_TALKERS)
    {
        UB_LOG(UBL_ERROR, "%s: AVB_IF_MAX_TALKERS (%u) already registered\r\n",
               __func__, AVB_IF_MAX_TALKERS);
        status = ENET_EFAIL;
    }
    else
    {
        idx  = AvbIf_numTalkers;
        slot = &AvbIf_talkerSlots[idx];

        slot->config = *config;

        (void)AvbIf_buildConl2Params(config->netdev, config->priority,
                                         config->vlanID, config->streamID,
                                         config->destMacAddr,
                                         config->timeInterval_us, &conl2Params);
        memcpy(&pcmInfo, &config->pcminfo, sizeof(aaf_avtpc_pcminfo_t));

        slot->talkerHandle = aaf_avtpc_talker_init(&conl2Params);
        if (slot->talkerHandle == NULL)
        {
            UB_LOG(UBL_ERROR, "%s: aaf_avtpc_talker_init failed\r\n",
                   __func__);
            status = ENET_EFAIL;
        }

        if (status == ENET_SOK)
        {
            aaf_avtpc_set_pcm_info(slot->talkerHandle, &pcmInfo);
        }

        if (status == ENET_SOK)
        {
            slot->frameSize = (config->pcminfo.bit_depth / 8U) *
                              config->pcminfo.channels;
            if (slot->frameSize == 0U)
            {
                UB_LOG(UBL_ERROR, "%s: pcminfo.bit_depth/channels yield "
                       "frameSize 0\r\n", __func__);
                aaf_avtpc_talker_close(slot->talkerHandle);
                status = ENET_EFAIL;
            }
        }

        if (status == ENET_SOK)
        {
            /* Frames per PDU = srate * interval, e.g. 48000 Hz * 250 us = 12 frames. */
            slot->pduSize = slot->frameSize *
                ((config->pcminfo.srate * config->timeInterval_us) /
                 (uint32_t)UB_SEC_US);
            slot->samplesPerPdu = slot->pduSize / slot->frameSize;

            if ((slot->pduSize == 0U) ||
                (slot->pduSize > AVB_IF_MAX_PDU_BYTES))
            {
                UB_LOG(UBL_ERROR, "%s: computed pduSize %u invalid or exceeds "
                       "AVB_IF_MAX_PDU_BYTES (%u)\r\n",
                       __func__, slot->pduSize,
                       AVB_IF_MAX_PDU_BYTES);
                aaf_avtpc_talker_close(slot->talkerHandle);
                status = ENET_EFAIL;
            }
        }

        if (status == ENET_SOK)
        {
            /* forceInit stays false so we never wipe a ring the audio side
             * has already started filling. */
            shmCfg.startAddr             =
                (uint32_t)(uintptr_t)config->shmAddr;
            shmCfg.regionSize = ShmIdxBuf_getRequiredSize(
                config->capacityFrames, slot->frameSize);
            shmCfg.frameSize             = slot->frameSize;
            shmCfg.capacityFrames        = config->capacityFrames;
            shmCfg.spinlockBaseAddr      = (config->spinlockBaseAddr != 0U) ?
                                           config->spinlockBaseAddr : CSL_SPINLOCK0_BASE;
            shmCfg.lockNumber            = config->lockNumber;
            shmCfg.lockTimeoutIterations = 0U;
            shmCfg.maxForwardJumpFrames  = 0U;
            shmCfg.forceInit             = false;
            shmCfg.clearStatsOnRead      = false;

            slot->shmHandle = ShmIdxBuf_create(&shmCfg);
            if (slot->shmHandle == NULL)
            {
                UB_LOG(UBL_ERROR, "%s: ShmIdxBuf_create failed\r\n",
                       __func__);
                aaf_avtpc_talker_close(slot->talkerHandle);
                status = ENET_EFAIL;
            }
        }

        if (status == ENET_SOK)
        {
            *outHandle = (AvbIf_TalkerHandle)idx;
            AvbIf_numTalkers = idx + 1U;

            if (config->statsLogEnable)
            {
                AvbIf_ensureStatsLogTaskStarted();
            }
        }
    }

    return status;
}

/* FNV-1a over the 8 stream ID bytes, folded to a bucket index. */
static uint32_t AvbIf_hashStreamId(const ub_streamid_t streamID)
{
    uint32_t hash = 2166136261U;
    uint32_t i;

    for (i = 0U; i < (uint32_t)sizeof(ub_streamid_t); i++)
    {
        hash ^= (uint32_t)streamID[i];
        hash *= 16777619U;
    }

    return hash & (AVB_IF_LISTENER_HASH_SIZE - 1U);
}

/* Returns the slot index registered for streamID, or AVB_IF_LISTENER_HASH_EMPTY. */
static int16_t AvbIf_hashLookup(const ub_streamid_t streamID)
{
    uint32_t bucket = AvbIf_hashStreamId(streamID);
    int16_t  found  = AVB_IF_LISTENER_HASH_EMPTY;
    uint32_t probes;

    for (probes = 0U; probes < AVB_IF_LISTENER_HASH_SIZE; probes++)
    {
        int16_t slotIdx = AvbIf_listenerHash[bucket];

        if (slotIdx == AVB_IF_LISTENER_HASH_EMPTY)
        {
            break;   /* chain ends at the first free bucket */
        }
        if (memcmp(AvbIf_listenerSlots[slotIdx].config.streamID, streamID,
                   sizeof(ub_streamid_t)) == 0)
        {
            found = slotIdx;
            break;
        }
        bucket = (bucket + 1U) & (AVB_IF_LISTENER_HASH_SIZE - 1U);
    }

    return found;
}

/* Caller guarantees streamID is not present and a free bucket exists
 * (AVB_IF_LISTENER_HASH_SIZE > AVB_IF_MAX_LISTENERS). */
static void AvbIf_hashInsert(const ub_streamid_t streamID, int16_t slotIdx)
{
    uint32_t bucket = AvbIf_hashStreamId(streamID);

    while (AvbIf_listenerHash[bucket] != AVB_IF_LISTENER_HASH_EMPTY)
    {
        bucket = (bucket + 1U) & (AVB_IF_LISTENER_HASH_SIZE - 1U);
    }
    AvbIf_listenerHash[bucket] = slotIdx;
}

/* Default per-stream callback: validates the PDU size and writes it into the
 * stream's SHM ring at the media-clock position of its presentation time. */
static int32_t AvbIf_shmRxCallback(uint8_t *payload, int plsize,
                                       const avbtp_rcv_cb_info_t *cbinfo, void *cbArg)
{
    AvbIf_ListenerSlot *slot = (AvbIf_ListenerSlot *)cbArg;

    if ((uint32_t)plsize != slot->pduSize)
    {
        UB_LOG(UBL_INFO, "%s: unexpected payload size %d (expected %u)\r\n",
               __func__, plsize, slot->pduSize);
    }
    else
    {
        uint64_t edgeIndex;
        uint32_t numFrames;

        /* payload is not guaranteed 4-byte aligned; copy to aligned buffer. */
        memcpy(slot->copyBuf, payload, (size_t)plsize);
        /* Swap big-endian IEEE 1722 samples to little-endian for the consumer. */
        AvbIf_swap32Buffer(slot->copyBuf, (uint32_t)plsize);

        /* Expand the AVTP presentation timestamp (32-bit) to full
         * 64-bit gPTP time, convert to the media-clock edge count it
         * lands on, and write this PDU at that index so the
         * consumer plays it out exactly on time. */

        const uint64_t gptpNow = AvbIf_getGptpProxyTime();
        const uint64_t presentationTimeGptp = gptpmasterclock_expand_timestamp_ts64(
                                                (uint32_t)cbinfo->u.rsdinfo.timestamp, gptpNow);
        edgeIndex = ClockRec_getMediaClockEdgeCount(presentationTimeGptp);
        numFrames = (uint32_t)plsize / slot->frameSize;

        (void)ShmIdxBuf_put(slot->shmHandle, edgeIndex, slot->copyBuf, numFrames);
    }

    return ENET_SOK;
}

/* Single callback for every registered listener stream: looks up the arriving
 * packet's full stream ID in the hash table and calls that stream's callback. */
static int AvbIf_listenerDispatchCallback(uint8_t *payload, int plsize,
                                              avbtp_rcv_cb_info_t *cbinfo, void *cbdata)
{
    int status = ENET_SOK;

    (void)cbdata;

    /* This shared socket carries any CRF (clock reference) traffic too;
     * CrfTask owns no independent rx-direct socket, so CRF-subtype packets
     * must be forwarded to it here, matching every existing AAF listener in
     * this codebase. */
    if (cbinfo->u.rcrfinfo.subtype == AVBTP_SUBTYPE_CRF)
    {
        CrfTask_rxCallback(payload, plsize, cbinfo, NULL);
    }
    else
    {
        int16_t slotIdx = AvbIf_hashLookup(cbinfo->u.rsdinfo.stream_id);

        if (slotIdx < 0)
        {
            const uint8_t *id = cbinfo->u.rsdinfo.stream_id;

            UB_LOG(UBL_INFO, "%s: no listener registered for stream id "
                   "%02X%02X%02X%02X%02X%02X%02X%02X\r\n", __func__,
                   id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
        }
        else
        {
            const AvbIf_ListenerSlot *slot = &AvbIf_listenerSlots[slotIdx];

            status = (int)slot->rxCallback(payload, plsize, cbinfo, slot->rxCallbackArg);
        }
    }

    return status;
}

int32_t AvbIf_initListenerTask(const AvbIf_ListenerNetConfig *netConfig)
{
    int32_t status = ENET_SOK;
    conl2_basic_conparas_t conl2Params;
    static const ub_streamid_t unusedStreamId = {0};   /* dispatch is by hash lookup in the
                                                           callback, not by this field */
    uint32_t i;

    if (netConfig == NULL)
    {
        UB_LOG(UBL_ERROR, "%s: NULL netConfig\r\n", __func__);
        status = ENET_EFAIL;
    }
    else if (AvbIf_sharedListenerHandle != NULL)
    {
        UB_LOG(UBL_ERROR, "%s: already initialized\r\n", __func__);
        status = ENET_EFAIL;
    }
    else
    {
        for (i = 0U; i < AVB_IF_LISTENER_HASH_SIZE; i++)
        {
            AvbIf_listenerHash[i] = AVB_IF_LISTENER_HASH_EMPTY;
        }

        (void)AvbIf_buildConl2Params(netConfig->netdev, netConfig->priority,
                                       netConfig->vlanID, unusedStreamId,
                                       netConfig->destMacAddr,
                                       netConfig->timeInterval_us, &conl2Params);

        AvbIf_sharedListenerHandle = aaf_avtpc_listener_init(
            &conl2Params, AvbIf_listenerDispatchCallback, NULL);
        if (AvbIf_sharedListenerHandle == NULL)
        {
            UB_LOG(UBL_ERROR, "%s: aaf_avtpc_listener_init failed\r\n",
                   __func__);
            status = ENET_EFAIL;
        }
    }

    return status;
}

int32_t AvbIf_registerListener(const AvbIf_ListenerConfig *config,
                                 AvbIf_ListenerHandle *outHandle)
{
    int32_t status = ENET_SOK;
    AvbIf_ListenerSlot *slot = NULL;
    ShmIdxBuf_Config      shmCfg;
    uint32_t              idx;

    if ((config == NULL) || (outHandle == NULL))
    {
        UB_LOG(UBL_ERROR, "%s: NULL config/outHandle\r\n", __func__);
        status = ENET_EFAIL;
    }
    else if (AvbIf_sharedListenerHandle == NULL)
    {
        UB_LOG(UBL_ERROR, "%s: AvbIf_initListenerTask() must be called first\r\n",
               __func__);
        status = ENET_EFAIL;
    }
    else
    {
        if (AvbIf_hashLookup(config->streamID) != AVB_IF_LISTENER_HASH_EMPTY)
        {
            UB_LOG(UBL_ERROR, "%s: stream id already registered\r\n", __func__);
            status = ENET_EFAIL;
        }
        else if (AvbIf_numListeners >= AVB_IF_MAX_LISTENERS)
        {
            UB_LOG(UBL_ERROR, "%s: AVB_IF_MAX_LISTENERS (%u) already registered\r\n",
                   __func__, AVB_IF_MAX_LISTENERS);
            status = ENET_EFAIL;
        }
        else
        {
            idx  = AvbIf_numListeners;
            slot = &AvbIf_listenerSlots[idx];

            slot->config    = *config;
            slot->shmHandle = NULL;

            if (config->rxCallback != NULL)
            {
                /* User handles the packets itself; no SHM ring involved. */
                slot->rxCallback    = config->rxCallback;
                slot->rxCallbackArg = config->rxCallbackArg;
            }
            else
            {
                slot->rxCallback    = AvbIf_shmRxCallback;
                slot->rxCallbackArg = slot;

                slot->frameSize = (config->pcminfo.bit_depth / 8U) *
                                  config->pcminfo.channels;
                if (slot->frameSize == 0U)
                {
                    UB_LOG(UBL_ERROR, "%s: pcminfo.bit_depth/channels yield "
                           "frameSize 0\r\n", __func__);
                    status = ENET_EFAIL;
                }

                if (status == ENET_SOK)
                {
                    slot->pduSize = slot->frameSize *
                        ((config->pcminfo.srate *
                          config->timeInterval_us) / (uint32_t)UB_SEC_US);

                    if ((slot->pduSize == 0U) ||
                        (slot->pduSize > AVB_IF_MAX_PDU_BYTES))
                    {
                        UB_LOG(UBL_ERROR, "%s: computed pduSize %u invalid or exceeds "
                               "AVB_IF_MAX_PDU_BYTES (%u)\r\n",
                               __func__, slot->pduSize,
                               AVB_IF_MAX_PDU_BYTES);
                        status = ENET_EFAIL;
                    }
                }

                if (status == ENET_SOK)
                {
                    shmCfg.startAddr             =
                        (uint32_t)(uintptr_t)config->shmAddr;
                    shmCfg.regionSize = ShmIdxBuf_getRequiredSize(
                        config->capacityFrames, slot->frameSize);
                    shmCfg.frameSize             = slot->frameSize;
                    shmCfg.capacityFrames        = config->capacityFrames;
                    shmCfg.spinlockBaseAddr      = (config->spinlockBaseAddr != 0U) ?
                                                   config->spinlockBaseAddr : CSL_SPINLOCK0_BASE;
                    shmCfg.lockNumber            = config->lockNumber;
                    shmCfg.lockTimeoutIterations = 0U;
                    shmCfg.maxForwardJumpFrames  = 0U;
                    shmCfg.forceInit             = false;
                    shmCfg.clearStatsOnRead      = false;

                    slot->shmHandle = ShmIdxBuf_create(&shmCfg);
                    if (slot->shmHandle == NULL)
                    {
                        UB_LOG(UBL_ERROR, "%s: ShmIdxBuf_create failed\r\n",
                               __func__);
                        status = ENET_EFAIL;
                    }
                }
            }

            if (status == ENET_SOK)
            {
                /* Slot is complete; publish it to the dispatch table last. */
                *outHandle = (AvbIf_ListenerHandle)idx;
                AvbIf_numListeners = idx + 1U;
                AvbIf_hashInsert(config->streamID, (int16_t)idx);

                if (config->statsLogEnable && (slot->shmHandle != NULL))
                {
                    AvbIf_ensureStatsLogTaskStarted();
                }
            }
        }
    }

    return status;
}

static void *AvbIf_taskMain(void *args)
{
    (void)args;

    while (1)
    {
        uint32_t i;

        SemaphoreP_pend(&AvbIf_tickSem, SystemP_WAIT_FOREVER);

        for (i = 0U; i < AvbIf_numTalkers; i++)
        {
            AvbIf_TalkerSlot *slot = &AvbIf_talkerSlots[i];
            uint64_t outBaseIndex;

            /* Read from SHM (indexed in sample-frames), then swap
             * little-endian samples to big-endian per IEEE 1722.
             * Anything but OK (ring empty, producer behind) just skips this tick. */
            ShmIdxBuf_Status status = ShmIdxBuf_getFifo(slot->shmHandle, slot->copyBuf,
                                                         slot->samplesPerPdu, &outBaseIndex);
            if (status == SHMIDXBUF_OK)
            {
                uint64_t gptpTime;

                AvbIf_swap32Buffer(slot->copyBuf, slot->pduSize);

                /* outBaseIndex is the media-clock edge count of this PDU's
                 * first sample-frame; convert to gPTP time and add the
                 * stream's fixed send-ahead offset for the AVTP
                 * presentation time. */
                gptpTime = ClockRec_getMediaClockTimestamp(outBaseIndex) +
                           slot->config.presentationOffset_ns;
                (void)aaf_avtpc_talker_write(slot->talkerHandle, gptpTime, slot->copyBuf,
                                             (int)slot->pduSize);
            }
        }
    }

    return NULL;   /* unreachable - the loop above never exits */
}

int32_t AvbIf_initTalkerTask(const AvbIf_TalkerTaskConfig *taskConfig)
{
    int32_t result = ENET_SOK;
    cb_tsn_thread_attr_t attr;
    CB_THREAD_T           thread;
    int                   status;
    int       priority  = AVB_IF_TASK_PRIORITY_DEFAULT;
    uint8_t  *stack     = AvbIf_talkerTaskStack;
    uint32_t  stackSize = (uint32_t)sizeof(AvbIf_talkerTaskStack);

    if (taskConfig != NULL)
    {
        if (taskConfig->priority != 0)
        {
            priority = taskConfig->priority;
        }
        if (taskConfig->stack != NULL)
        {
            stack     = taskConfig->stack;
            stackSize = taskConfig->stackSize;
        }
    }

    if (AvbIf_initDone)
    {
        UB_LOG(UBL_ERROR, "%s: already initialized\r\n", __func__);
        result = ENET_EFAIL;
    }
    else if (stackSize == 0U)
    {
        UB_LOG(UBL_ERROR, "%s: stack given with stackSize 0\r\n", __func__);
        result = ENET_EFAIL;
    }
    else
    {
        /* Counting, so a few ticks that fire while the task is busy are not lost. */
        SemaphoreP_constructCounting(&AvbIf_tickSem, 0, 10);
        AvbIf_initDone = true;

        (void)cb_tsn_thread_attr_init(&attr, priority, (int)stackSize, "AvbTalkerTask");
        attr.stack_addr = stack;
        attr.stack_size = stackSize;

        status = CB_THREAD_CREATE(&thread, &attr, AvbIf_taskMain, NULL);
        if (status != 0)
        {
            UB_LOG(UBL_ERROR, "%s: CB_THREAD_CREATE failed\r\n", __func__);
            AvbIf_initDone = false;
            result = ENET_EFAIL;
        }
    }

    return result;
}

void AvbIf_talkerTickCallback(void *args)
{
    (void)args;

    if (AvbIf_initDone)
    {
        SemaphoreP_post(&AvbIf_tickSem);
    }
}

/* Prints one line per statsLogEnable=true stream, every
 * AVB_IF_STATS_LOG_PERIOD_US, for as long as any such stream exists.
 * Reads AvbIf_numTalkers/AvbIf_numListeners and their slots without locking - same
 * append-only/fully-populated-before-published contract as AvbIf_taskMain
 * and AvbIf_listenerDispatchCallback rely on for those arrays. */
static void *AvbIf_statsLogTaskMain(void *args)
{
    (void)args;

    while (1)
    {
        uint32_t i;

        ClockP_usleep(AVB_IF_STATS_LOG_PERIOD_US);

        for (i = 0U; i < AvbIf_numTalkers; i++)
        {
            AvbIf_TalkerSlot *slot = &AvbIf_talkerSlots[i];

            if (slot->config.statsLogEnable)
            {
                ShmIdxBuf_Stats stats;

                /* backlog = frames sitting in the ring that nobody has read yet. */
                ShmIdxBuf_getStats(slot->shmHandle, &stats);
                UB_LOG(UBL_INFO,
                       "%s: talker[%u] streamIdLast=0x%02X put=%u get=%u discard=%u "
                       "staleWr=%u gapFill=%u tooOld=%u notReady=%u lockTO=%u badIdx=%u "
                       "base=%" PRIu64 " head=%" PRIu64 " backlog=%" PRIu64 "\r\n",
                       __func__, i, slot->config.streamID[7],
                       stats.putCount, stats.getCount, stats.discardedFrameCount,
                       stats.staleWriteCount, stats.gapFrameCount, stats.tooOldCount,
                       stats.notReadyCount, stats.lockTimeoutCount, stats.invalidIndexCount,
                       stats.baseIndex, stats.headIndex, stats.headIndex - stats.baseIndex);
            }
        }

        for (i = 0U; i < AvbIf_numListeners; i++)
        {
            AvbIf_ListenerSlot *slot = &AvbIf_listenerSlots[i];

            if (slot->config.statsLogEnable && (slot->shmHandle != NULL))
            {
                ShmIdxBuf_Stats stats;

                ShmIdxBuf_getStats(slot->shmHandle, &stats);
                UB_LOG(UBL_INFO,
                       "%s: listener[%u] streamIdLast=0x%02X put=%u get=%u discard=%u "
                       "staleWr=%u gapFill=%u tooOld=%u notReady=%u lockTO=%u badIdx=%u "
                       "base=%" PRIu64 " head=%" PRIu64 " backlog=%" PRIu64 "\r\n",
                       __func__, i, slot->config.streamID[7],
                       stats.putCount, stats.getCount, stats.discardedFrameCount,
                       stats.staleWriteCount, stats.gapFrameCount, stats.tooOldCount,
                       stats.notReadyCount, stats.lockTimeoutCount, stats.invalidIndexCount,
                       stats.baseIndex, stats.headIndex, stats.headIndex - stats.baseIndex);
            }
        }
    }

    return NULL;   /* unreachable - the loop above never exits */
}

static void AvbIf_ensureStatsLogTaskStarted(void)
{
    cb_tsn_thread_attr_t attr;
    CB_THREAD_T           thread;
    int                   status;

    if (AvbIf_statsLogTaskStarted)
    {
        return;
    }
    AvbIf_statsLogTaskStarted = true;

    (void)cb_tsn_thread_attr_init(&attr, AVB_IF_STATS_LOG_TASK_PRIORITY,
                                  (int)AVB_IF_STATS_LOG_TASK_STACK_SIZE, "AvbStatsLogTask");
    attr.stack_addr = AvbIf_statslogTaskStack;
    attr.stack_size = sizeof(AvbIf_statslogTaskStack);

    status = CB_THREAD_CREATE(&thread, &attr, AvbIf_statsLogTaskMain, NULL);
    if (status != 0)
    {
        UB_LOG(UBL_ERROR, "%s: CB_THREAD_CREATE failed\r\n", __func__);
        AvbIf_statsLogTaskStarted = false;
    }
}
