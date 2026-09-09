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

#ifndef CLOCK_HW_H_
#define CLOCK_HW_H_

#include <stdint.h>
#include <include/mod/cpsw_cpts.h>   /* CpswCpts_HwPush */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Time sync router lines: ATL BWS outputs in, CPTS HW push inputs out. */
#define TS_ROUTER_IN_ATL_BWS_SEL_0          (20)
#define TS_ROUTER_IN_ATL_BWS_SEL_1          (21)
#define TS_ROUTER_IN_ATL_BWS_SEL_2          (22)
#define TS_ROUTER_IN_ATL_BWS_SEL_3          (23)

#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_1   (8)
#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_2   (9)
#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_3   (10)
#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_4   (11)
#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_5   (12)
#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_6   (13)
#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_7   (14)
#define TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_8   (15)

/* Registers that pick which McASP frame-sync pin feeds ATL BWS1/BWS2. */
#define CTRL_MMR0_CFG0_ATL_BWS1_SEL         (0x001082C4)
#define CTRL_MMR0_CFG0_ATL_BWS2_SEL         (0x001082C8)

#define ATL_BWSX_MCASP0_AFSR_IN             (0b0000)
#define ATL_BWSX_MCASP1_AFSR_IN             (0b0001)
#define ATL_BWSX_MCASP2_AFSR_IN             (0b0010)
#define ATL_BWSX_MCASP3_AFSR_IN             (0b0011)
#define ATL_BWSX_MCASP4_AFSR_IN             (0b0100)
#define ATL_BWSX_MCASP0_AFSX_IN             (0b0101)
#define ATL_BWSX_MCASP1_AFSX_IN             (0b0110)
#define ATL_BWSX_MCASP2_AFSX_IN             (0b0111)
#define ATL_BWSX_MCASP3_AFSX_IN             (0b1000)
#define ATL_BWSX_MCASP4_AFSX_IN             (0b1001)

/* Called with the arg passed at registration time when a CPTS HW-push
 * event fires for the corresponding signal. */
typedef void (*ClockHw_PushEventCb)(void *arg, uint64_t tsVal);

/* Which McASP frame-sync pin (Tx/AFSX or Rx/AFSR) a signal is wired to -
 * an application can wire either the media clock or the timestamping
 * signal to either pin, so this is not fixed per-role. */
typedef enum
{
    CLOCK_HW_MCASP_DIR_TX = 0,  /* AFSX - transmit frame sync */
    CLOCK_HW_MCASP_DIR_RX = 1   /* AFSR - receive frame sync */
} ClockHw_McaspDir;

/* Media clock source backend, chosen at runtime by ClockHw_configClock(). */
typedef enum
{
    CLOCK_HW_CLKSRC_CDCE = 0,   /* CDCE frequency tuning (ClockHw_adjFreq) */
    CLOCK_HW_CLKSRC_PHY  = 1    /* PHY-recovered clock phase nudge (ClockHw_adjPhaseNs) */
} ClockHw_ClkSrc;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief Chooses which CPTS HW push event carries the media clock signal and
 * which carries the timestamping signal. The media clock signal always comes
 * from ATL BWS1 and the timestamping signal from ATL BWS2; this only picks
 * which CPTS HW push each one is routed to. The driver maps each push to its
 * time-sync router output line (TS_ROUTER_OUT_CPSW_CPTS_HW_PUSH_n) and uses
 * the push for callbacks and timestamp reads.
 *
 * Must be called before the ClockHw_tsr signal calls, the push event
 * callback register/unregister calls and the timestamp getters; they fail
 * (or return 0) until it has succeeded.
 *
 * \param[in] mcPush HW push carrying the media clock signal (Mcs).
 * \param[in] tsPush HW push carrying the timestamping signal (Tss).
 * \return ENET_SOK on success, ENET_EFAIL if a value is not
 * CPSW_CPTS_HWPUSH_1..8 or both are the same.
 */
int32_t ClockHw_configPush(CpswCpts_HwPush mcPush, CpswCpts_HwPush tsPush);

/**
 * \brief Selects the media clock source (CDCE or PHY-recovered, chosen at
 * runtime) and stores the parent clock frequency for later ppb->ppm math.
 * ClockHw_adjFreq() is only valid for CLOCK_HW_CLKSRC_CDCE and
 * ClockHw_adjPhaseNs() only for CLOCK_HW_CLKSRC_PHY; the other returns
 * ENET_EFAIL.
 *
 * \param[in] parentClockFrequency Reference frequency (Hz) the media clock
 * is generated/recovered from.
 * \param[in] clkSrc Media clock source backend.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_configClock(uint64_t parentClockFrequency, ClockHw_ClkSrc clkSrc);

/**
 * \brief Configures the ATL mux to tap the media clock and timestamping
 * signal from the given McASP instances, on the given frame-sync pin of
 * each.
 *
 * \param[in] audioMcaspBaseAddress CFG base address of the McASP generating
 * the media clock.
 * \param[in] audioMcaspDir Frame-sync pin the media clock is wired to on
 * that McASP (Tx/AFSX or Rx/AFSR).
 * \param[in] tsMcaspBaseAddress CFG base address of the McASP carrying the
 * timestamping signal.
 * \param[in] tsMcaspDir Frame-sync pin the timestamping signal is wired to
 * on that McASP (Tx/AFSX or Rx/AFSR).
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_configAtlMux(int32_t audioMcaspBaseAddress,
                                     ClockHw_McaspDir audioMcaspDir,
                                     int32_t tsMcaspBaseAddress,
                                     ClockHw_McaspDir tsMcaspDir);

/**
 * \brief Enables/disables the time-sync router route from the ATL
 * timestamping signal input to its CPTS HW push (see ClockHw_configPush()).
 *
 * \param[in] state Non-zero to enable the route, zero to release it.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_tsrTsSignal(int32_t state);

/**
 * \brief Enables/disables the time-sync router route from the ATL media
 * clock signal input to its CPTS HW push (see ClockHw_configPush()).
 *
 * \param[in] state Non-zero to enable the route, zero to release it.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_tsrMcSignal(int32_t state);

/**
 * \brief Registers a callback invoked whenever the timestamping signal
 * (CPTS HWPUSH_2) captures a new edge.
 *
 * \param[in] cb Callback invoked with arg on each push event.
 * \param[in] arg Opaque argument passed to cb.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_registerTssPushEventCb(ClockHw_PushEventCb cb,
                                               void *arg);

/**
 * \brief Registers a callback invoked whenever the media clock signal
 * (CPTS HWPUSH_1) captures a new edge.
 *
 * \param[in] cb Callback invoked with arg on each push event.
 * \param[in] arg Opaque argument passed to cb.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_registerMcsPushEventCb(ClockHw_PushEventCb cb,
                                               void *arg);

/**
 * \brief Unregisters the timestamping signal push-event callback.
 *
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_unregisterTssPushEventCb(void);

/**
 * \brief Unregisters the media clock signal push-event callback.
 *
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_unregisterMcsPushEventCb(void);

/**
 * \brief Fetches the latest timestamping signal edge timestamp directly
 * from CPTS (CPSW_CPTS_HWPUSH_2).
 *
 * \return Timestamp in nanoseconds, or 0 if no event was found.
 */
uint64_t ClockHw_getTsTimestamp(void);

/**
 * \brief Fetches the latest media clock signal edge timestamp directly
 * from CPTS (CPSW_CPTS_HWPUSH_1).
 *
 * \return Timestamp in nanoseconds, or 0 if no event was found.
 */
uint64_t ClockHw_getMcTimestamp(void);

/**
 * \brief Fine-tunes the media clock frequency (CDCE clock source only).
 *
 * \param[in] ppb Relative frequency adjustment in parts per billion.
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_adjFreq(int32_t ppb);

/**
 * \brief Nudges the media clock phase by a number of clock cycles
 * (PHY-recovered clock source only).
 *
 * \param[in] count Number of cycles to nudge by (positive or negative).
 * \return ENET_SOK on success, ENET_EFAIL on failure.
 */
int32_t ClockHw_adjPhaseNs(int32_t count);

#endif /* CLOCK_HW_H_ */
