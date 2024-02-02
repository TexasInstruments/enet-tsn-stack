/*
 * Copyright (c) 2023 Texas Instruments Incorporated
 * Copyright (c) 2023 Excelfore Corporation (https://excelfore.com)
 *
 * All rights reserved not granted herein.
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 * this software subject to the terms herein. With respect to the foregoing patent
 * license, such license is granted solely to the extent that any such patent is necessary
 * to Utilize the software alone. The patent license shall not apply to any combinations which
 * include this software, other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license (including the
 * above copyright notice and the disclaimer and (if applicable) source code license limitations below)
 * in the documentation and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided that the following
 * conditions are met:
 *
 * * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any
 * software provided in binary form.
 * * any redistribution and use are licensed by TI for use only with TI Devices.
 * * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the source code are permitted
 * provided that the following conditions are met:
 *
 * * any redistribution and use of the source code, including any resulting derivative works, are licensed by
 * TI for use only with TI Devices.
 * * any redistribution and use of any object code compiled from the source code and any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or
 * promote products derived from this software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI"S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI"S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/
/**
 * @ingroup  TSN_COMBASE_MODULE
 * @defgroup xtimer xTimer utility functions
 * @{
 * @file cb_xtimer.h
 *
 * @brief xTimer utility functions
 */

#ifndef CB_XTIMER_H_
#define CB_XTIMER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Data handle of a timer manager, inside of it is private
 */
typedef struct cb_xtimer_man cb_xtimer_man_t;

/**
 * @brief Data handle of a timer, inside of it is private
 */
typedef struct cb_xtimer cb_xtimer_t;

/**
 * @brief Data handle of a timer data, inside of it is private
 */
typedef struct cb_xtimer_ndata cb_xtimer_ndata_t;

/**
 * @brief A callback is called when a timer expired.
 */
typedef void (*xtimer_expirecb_t)(cb_xtimer_t *timer, void *exparg);


/**
 * @brief Create a timer manager.
 *
 * The xtimer is non-thread safe function.
 * Each thread is expected to own a timer manager and all timers are processed in this thread.
 *
 * @return a timer manager: success; NULL: error
 */
cb_xtimer_man_t *cb_xtimer_man_create(void);

/**
 * @brief Delete a timer manager.
 * @param xtimer_man timer manager data
 */
void cb_xtimer_man_delete(cb_xtimer_man_t *xtimer_man);

/**
 * @brief The timer manager consumes expired timers and schedule for next timer to be expired.
 *
 * The function is expected to run in a thread loop.
 * It returns nearest timeout that expects for next timer to be expired.
 * The thread loop should use this timeout as its wait time for event.
 *
 * @param xtimer_man timer manager data
 * @return >= 0: nearest timeout; -1: error; -2: no running timer
 */
int cb_xtimer_man_schedule(cb_xtimer_man_t *xtimer_man);

/**
 * @brief Get a timeout of nearest timer.
 * @param xtimer_man timer manager data
 * @return >= 0: nearest timeout ; -1: error; -2: no running timer
 */
int cb_xtimer_man_nearest_timeout(cb_xtimer_man_t *xtimer_man);

/**
 * @brief Create a timer.
 * @param xtimer_man timer manager data
 * @param expirecb expire callback
 * @param exparg expired callback argument
 * @return a timer: success; NULL: error
 */
cb_xtimer_t *cb_xtimer_create(cb_xtimer_man_t *xtimer_man,
				xtimer_expirecb_t expirecb, void *exparg);

/**
 * @brief Delete a timer.
 * @param xtimer a timer data
 */
void cb_xtimer_delete(cb_xtimer_t *xtimer);

/**
 * @brief Set a periodic timer flag.
 *
 * If this flag is set, the timer will be restarted when it expired.
 *
 * @param xtimer a timer data
 */
void cb_xtimer_set_periodic(cb_xtimer_t *xtimer);

/**
 * @brief Clear periodic timer flag.
 * @param xtimer a timer data
 */
void cb_xtimer_clear_periodic(cb_xtimer_t *xtimer);

/**
 * @brief Check if a timer is periodic
 * @param xtimer a timer data
 * @return true: periodic; false: not periodic or error
 */
bool cb_xtimer_is_periodic(cb_xtimer_t *xtimer);

/**
 * @brief Get remaining timeout of a running timer.
 * @param xtimer a timer data
 * @return >= 0: remaining timeout; -1: error
 */
int cb_xtimer_remain_timeout(cb_xtimer_t *xtimer);

/**
 * @brief Start a timer.
 * @param xtimer a timer data
 * @param timeout_us timeout value in microsec
 * @return 0: success; -1: error; -2: timer is running
 */
int cb_xtimer_start(cb_xtimer_t *xtimer, uint32_t timeout_us);

/**
 * @brief Stop a running timer, the periodic flag is cleared as well.
 * @param xtimer a timer data
 */
void cb_xtimer_stop(cb_xtimer_t *xtimer);

/**
 * @brief Check if a timer is running.
 * @param xtimer a timer data
 * @return true: running; false: stopped or error
 */
bool cb_xtimer_is_running(cb_xtimer_t *xtimer);

#endif /*CB_XTIMER_H_*/

/** @}*/
