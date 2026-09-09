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
 * \file   avb_if.h
 *
 * \brief Generic AVTP/AAF talker registration and transmit-scheduling module.
 *
 * Lets a caller register any number of independent talker streams
 * (network params + PCM format + a pre-initialized shared-memory ring),
 * then drains each stream's ring and transmits it as AVTP on its own
 * configured interval from a single internally-owned task. The shared
 * memory at AvbIf_TalkerConfig.shmAddr is assumed already initialized by
 * the peer that produces into it - this module only attaches to it
 * (ShmIdxBuf_create() with forceInit=false), it never formats it.
 *
 * Only 32-bit PCM is supported today (byte-swap step is fixed at 32-bit
 * words), for both talker and listener streams.
 *
 * Listener streams have no task/tick of their own. Unlike talkers, every
 * listener stream shares ONE underlying AVTP listener socket/connection
 * (aaf_avtpc_listener_init() is called once, by AvbIf_initListenerTask()) - the
 * full 8-byte stream ID of each arriving packet is looked up in a small
 * hash table (AVB_IF_MAX_LISTENERS streams -> one RX callback each) to
 * find the registered stream. By default that callback writes the packet into
 * the stream's SHM ring on the spot, from the network stack's own RX context;
 * a caller may supply its own callback per stream instead
 * (AvbIf_ListenerConfig.rxCallback). Only the talker side is
 * timer/tick-driven (AvbIf_talkerTickCallback()).
 */

#ifndef AVB_IF_H_
#define AVB_IF_H_

#include <stdint.h>
#include "tsn_conl2/aaf_avtpc_talker.h"
#include "tsn_conl2/aaf_avtpc_listener.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Compile-time cap on concurrently registered talker streams. */
#define AVB_IF_MAX_TALKERS      (4U)

/** \brief Compile-time cap on concurrently registered listener streams. */
#define AVB_IF_MAX_LISTENERS    (4U)

/** \brief Number of buckets in the listener stream-ID hash table. Must be a
 *  power of 2 and larger than AVB_IF_MAX_LISTENERS (open addressing needs
 *  free buckets to terminate a lookup); 2x or more keeps probe chains short. */
#define AVB_IF_LISTENER_HASH_SIZE   (32U)

/** \brief Largest single AVTPDU payload (bytes) any registered talker or
 *  listener may use per interval; registration rejects a config that exceeds
 *  this. Each talker/listener slot reserves this many bytes of copy buffer. */
#define AVB_IF_MAX_PDU_BYTES    (1536U)

/** \brief Handle returned by AvbIf_registerTalker(), identifying one registered
 *  stream. Always >= 0. */
typedef int32_t AvbIf_TalkerHandle;

/**
 * \brief Per-stream talker configuration passed to AvbIf_registerTalker().
 */
typedef struct
{
    char                 netdev[CB_MAX_NETDEVNAME];  /*!< Ethernet device name, e.g. "tilld0" */
    int                  priority;                    /*!< VLAN PCP */
    int                  vlanID;
    ub_streamid_t        streamID;                    /*!< IEEE 802.1Q stream ID */
    ub_macaddr_t         destMacAddr;                 /*!< Destination (usually multicast) MAC */
    uint32_t             timeInterval_us;             /*!< AVTP send interval, multiple of 125us */
    aaf_avtpc_pcminfo_t  pcminfo;                      /*!< format/channels/bit_depth/srate - only
                                                             32-bit formats are supported today */

    void                *shmAddr;                      /*!< Peer-initialized SHM ring base address.
                                                             NOT (re)initialized by this module. */
    uint32_t             capacityFrames;               /*!< Ring depth, in sample-frames, at shmAddr.
                                                             Must match what the peer producer used. */
    uint32_t             lockNumber;                    /*!< HW spinlock number for this ring; must be
                                                             unique across every concurrently-accessed
                                                             SHM region (talkers and any other users). */
    uint32_t             spinlockBaseAddr;              /*!< HW spinlock module base address; 0 =>
                                                             CSL_SPINLOCK0_BASE. Must match what the SHM
                                                             ring's peer uses. */
    uint64_t             presentationOffset_ns;        /*!< Added to the media-clock timestamp derived
                                                             from the SHM read index to form the AVTP
                                                             presentation time. */
    bool                 statsLogEnable;                /*!< If true, this stream's shared-memory ring
                                                             statistics (see ShmIdxBuf_getStats()) are
                                                             periodically printed by an internal
                                                             diagnostic task. Starts that task, shared
                                                             across every stream that requests it, on
                                                             first use - no cost if left false
                                                             everywhere. */
}AvbIf_TalkerConfig;

/**
 * \brief Network-level parameters for the ONE shared AVTP listener
 * socket/connection every registered listener stream is dispatched
 * through. Passed to AvbIf_initListenerTask(), called once.
 */
typedef struct
{
    char          netdev[CB_MAX_NETDEVNAME];  /*!< Ethernet device name, e.g. "tilld0" */
    int           priority;                    /*!< VLAN PCP */
    int           vlanID;
    ub_macaddr_t  destMacAddr;                 /*!< Multicast group this shared socket joins */
    uint32_t      timeInterval_us;             /*!< Nominal AVTP interval, multiple of 125us,
                                                     used only to set up the shared socket */
}AvbIf_ListenerNetConfig;

/** \brief Handle returned by AvbIf_registerListener(), identifying one registered
 *  stream. Always >= 0. */
typedef int32_t AvbIf_ListenerHandle;

/**
 * \brief Per-stream receive callback, invoked from the network stack's RX
 * context for every AVTP packet whose stream ID matches the registered stream.
 *
 * \param[in] payload  AVTPDU payload; not guaranteed 4-byte aligned, only valid
 *                     for the duration of the call.
 * \param[in] plsize   Payload size in bytes.
 * \param[in] cbinfo   Parsed AVTP header information of this packet.
 * \param[in] cbArg    The rxCallbackArg given at registration.
 * \return ENET_SOK, or an error code (passed back to the network stack).
 */
typedef int32_t (*AvbIf_ListenerRxCallback)(uint8_t *payload, int plsize,
                                               const avbtp_rcv_cb_info_t *cbinfo, void *cbArg);

/**
 * \brief Per-stream listener configuration passed to AvbIf_registerListener().
 * The full 8-byte streamID is the hash-table key used to dispatch an arriving
 * packet to this stream (see AvbIf_initListenerTask() docs) - every stream
 * registered while one shared socket is active must have a distinct streamID.
 */
typedef struct
{
    ub_streamid_t        streamID;                     /*!< IEEE 802.1Q stream ID; all 8 bytes are
                                                              the key used to route a packet to this stream */
    AvbIf_ListenerRxCallback rxCallback;            /*!< Called for each packet of this stream.
                                                              NULL => built-in callback, which validates
                                                              the PDU size and writes it into the SHM ring
                                                              below (pcminfo/timeInterval_us/shmAddr/... then
                                                              required). Non-NULL => only streamID is
                                                              required; pcminfo, timeInterval_us and the SHM
                                                              fields are ignored and no SHM ring is attached. */
    void                *rxCallbackArg;                 /*!< Passed to rxCallback as cbArg. */
    aaf_avtpc_pcminfo_t  pcminfo;                        /*!< Expected format/channels/bit_depth/srate,
                                                               used to validate each received PDU's size -
                                                               only 32-bit formats are supported today */
    uint32_t             timeInterval_us;                /*!< Expected AVTP interval for this stream,
                                                               multiple of 125us */

    void                *shmAddr;                        /*!< SHM ring this listener writes received
                                                               audio into. Peer (consumer) is assumed to
                                                               have already initialized it - NOT
                                                               (re)initialized by this module. */
    uint32_t             capacityFrames;                 /*!< Ring depth, in sample-frames, at shmAddr. */
    uint32_t             lockNumber;                      /*!< HW spinlock number for this ring; must be
                                                               unique across every concurrently-accessed
                                                               SHM region (talkers, listeners, anything else). */
    uint32_t             spinlockBaseAddr;                /*!< HW spinlock module base address; 0 =>
                                                               CSL_SPINLOCK0_BASE. Must match what the SHM
                                                               ring's peer uses. */
    bool                 statsLogEnable;                 /*!< If true, this stream's shared-memory ring
                                                               statistics (see ShmIdxBuf_getStats()) are
                                                               periodically printed by an internal
                                                               diagnostic task. Starts that task, shared
                                                               across every stream that requests it, on
                                                               first use - no cost if left false
                                                               everywhere. Ignored when rxCallback is set. */
}AvbIf_ListenerConfig;

/**
 * \brief Talker task settings passed to AvbIf_initTalkerTask(). Every
 * field has a default, so a zeroed struct (or a NULL pointer) keeps the
 * built-in behaviour.
 */
typedef struct
{
    int       priority;    /*!< Task priority, lower number = higher priority; 0 => 16. Must stay
                                numerically higher than the CRF task's (8) so the clock recovery
                                task is never preempted by audio transmission. */
    uint8_t  *stack;       /*!< Caller-owned stack buffer, must outlive the task; NULL => an internal
                                AVB_IF_TASK_STACK_SIZE_DEFAULT byte stack (stackSize is ignored). */
    uint32_t  stackSize;   /*!< Size of \c stack in bytes; required (> 0) when \c stack is set. */
}AvbIf_TalkerTaskConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief Create the semaphore and internal task that will drain every
 * registered talker stream's shared-memory ring and transmit it as AVTP.
 * Call once, before or after any AvbIf_registerTalker() calls.
 *
 * \param[in] taskConfig Task priority/stack, or NULL for the defaults.
 *
 * The task blocks until AvbIf_talkerTickCallback() is invoked; the caller
 * is responsible for wiring/starting whatever periodic timer drives that
 * (e.g. a SysConfig timer whose timerCallback is set to
 * "AvbIf_talkerTickCallback").
 *
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t AvbIf_initTalkerTask(const AvbIf_TalkerTaskConfig *taskConfig);

/**
 * \brief Register one talker stream: initializes its AVTP talker
 * connection, attaches to its (already peer-initialized) shared-memory
 * ring, and adds it to the set the internal task services every tick.
 *
 * \param[in]  config     Stream configuration. Copied; not retained by pointer.
 * \param[out] outHandle  Set to the new stream's handle on success.
 * \return ENET_SOK on success, ENET_EFAIL on failure (bad
 *         config, AVB_IF_MAX_TALKERS already registered, PDU size
 *         exceeds AVB_IF_MAX_PDU_BYTES, or the talker/SHM attach itself
 *         failed).
 */
int32_t AvbIf_registerTalker(const AvbIf_TalkerConfig *config, AvbIf_TalkerHandle *outHandle);

/**
 * \brief Create the ONE shared AVTP listener socket/connection that every
 * AvbIf_registerListener() stream will be dispatched through. Call once, before
 * any AvbIf_registerListener() calls.
 *
 * A single socket is used (rather than one per stream) because this is how
 * multiple listener streams are handled in this codebase - see
 * aafpcm_audio_etherring_demo/central_compute/central_aaf_app.c's
 * init_aaf_pcm_listener() + registerRxCallback() pattern, which this
 * mirrors. Incoming packets are dispatched to the right registered stream
 * by a hash-table lookup on their full stream ID.
 *
 * \param[in] netConfig  Network parameters for the shared socket. Copied;
 *                       not retained by pointer.
 * \return ENET_SOK on success, ENET_EFAIL on failure (bad
 *         config, already initialized, or the listener init itself failed).
 */
int32_t AvbIf_initListenerTask(const AvbIf_ListenerNetConfig *netConfig);

/**
 * \brief Register one listener stream: attaches to its (already
 * peer-initialized) shared-memory ring and adds it to the shared listener's
 * dispatch hash table, keyed by its full stream ID, together with the RX
 * callback that serves it. Requires AvbIf_initListenerTask() to have been
 * called first. No task or tick of this module's own is involved - each
 * arriving packet on this stream is handed to its callback (by default: written
 * into its SHM ring) synchronously, from the network stack's own RX context,
 * the moment it arrives. A CRF-subtype packet arriving on the shared socket
 * is forwarded to CrfTask_rxCallback() (clock recovery transport, not part
 * of any registered stream), matching every existing listener in this
 * codebase.
 *
 * \param[in]  config     Stream configuration. Copied; not retained by pointer.
 * \param[out] outHandle  Set to the new stream's handle on success.
 * \return ENET_SOK on success, ENET_EFAIL on failure (bad
 *         config, AvbIf_initListenerTask() not yet called, that stream ID
 *         already registered, AVB_IF_MAX_LISTENERS already
 *         registered, or - when rxCallback is NULL - expected PDU size exceeds
 *         AVB_IF_MAX_PDU_BYTES or the SHM attach failed).
 */
int32_t AvbIf_registerListener(const AvbIf_ListenerConfig *config, AvbIf_ListenerHandle *outHandle);

/**
 * \brief Timer callback: posts the tick semaphore that drives one
 * transmit pass over every registered talker. Bind this in the caller's
 * .syscfg (e.g. CONFIG_TIMERx.timerCallback = "AvbIf_talkerTickCallback").
 * A no-op if called before AvbIf_initTalkerTask().
 */
void AvbIf_talkerTickCallback(void *args);

/**
 * \brief Returns the current gPTP-proxied time. Implemented by the
 * application; this module only declares and calls it.
 *
 * \return Current gPTP-proxied time, in nanoseconds.
 */
int64_t AvbIf_getGptpProxyTime(void);

#ifdef __cplusplus
}
#endif

#endif /* AVB_IF_H_ */
