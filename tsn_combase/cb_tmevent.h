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
 * @defgroup timer Timer and events utility functions
 * @{
 * @file cb_tmevent.h
 *
 * @brief Timer and events utility functions
 */

#ifndef CB_EVENT_H_
#define CB_EVENT_H_

#ifdef CB_EVENT_NON_POSIX_H
/* non-posix platforms need to support necessary POSIX compatible
 * functions and types which are defined as CB_* macros below.
 * And provide them in a header file defined as CB_EVENT_NON_POSIX_H */
#include CB_EVENT_NON_POSIX_H
#else
#include <unistd.h>
#include <time.h>

/**
 *@brief micro second unit sleep
 */
#define CB_USLEEP(x) usleep(x)

/**
 *@brief second unit sleep
 */
#define CB_SLEEP(x) sleep(x)

/**
 *@brief seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)
 */
#define CB_MKTIME(x) mktime(x)

/**
 *@brief nano second unit sleep
 */
static inline int cb_nanosleep64(int64_t ts64, int64_t *rts64)
{
	struct timespec ts, rts;
	int res;
	UB_NSEC2TS(ts64, ts);
	res=nanosleep(&ts, &rts);
	if(rts64!=NULL) {*rts64=(int64_t)(UB_TS2NSEC(rts));}
	return res;
}
#endif // CB_EVENT_NON_POSIX_H

/**
 * @brief handler for cb_timer_object
 * @note cb_timer is signal event based timer,
 * 	no thread, no callback happens with this.
 *	users need to call 'cb_timer_expired' to check the configured timer
 */
typedef struct cb_timer_object cb_timer_object_t;

/**
 * @brief enumeration used for timer to clear, decrement or no clear.
 * This enumeration has following members:
 * 1. CB_TIMER_NO_CLEAR		-> no action on timer value
 * 2. CB_TIMER_DECREMENT	-> timer value decrement
 * 3. CB_TIMER_ALL_CLEAR	-> set timer value to 0.
 */
typedef enum {
	CB_TIMER_NO_CLEAR,
	CB_TIMER_DECREMENT,
	CB_TIMER_ALL_CLEAR,
} cb_timer_clear_t;

/**
 * @brief creates timer.
 * @param tname	timer name
 * @return hanler of cb_timer_object_t
 */
cb_timer_object_t *cb_timer_create(char *tname);

/**
 * @brief close the timer
 * @param mtmo reference to cb_timer_object_t, returned by cb_timer_create().
 */
void cb_timer_close(cb_timer_object_t *mtmo);

/**
 * @brief start timer interval.
 * @param mtmo reference to cb_timer_object_t, created and returned by cb_timer_create().
 * @param value_ms initial value in msec.
 * @param interval_ms time interval in msec.
 * @return 0 on success, -1 on error
 * @note both value_ms and interval_ms is used by timer_settime.
 */
int cb_timer_start_interval(cb_timer_object_t *mtmo,
			    uint32_t value_ms, uint32_t interval_ms);

/**
 * @brief timer stop.
 * @param mtmo reference to cb_timer_object_t, which is created
 * and returned by cb_timer_create().
 * @return 0 on success, -1 on error.
 */
int cb_timer_stop(cb_timer_object_t *mtmo);

/**
 * @brief timer expired
 * @param mtmo reference to cb_timer_object_t, which is created
 * and returned by cb_timer_create().
 * @param clear	action by the definition of cb_timer_clear_t
 * @return true if timer not expired, false if timer expired.
 */
bool cb_timer_expired(cb_timer_object_t *mtmo, cb_timer_clear_t clear);


#endif
/** @}*/
