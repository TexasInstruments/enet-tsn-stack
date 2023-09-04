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
 * @defgroup lld_thread TI ENET LLD timer and event specific
 * @{
 * @file cb_lld_tmevent.h
 *
 * @brief TI ENET LLD specific timer and event part
 */

#ifndef CB_LLD_TMEVENT_H_
#define CB_LLD_TMEVENT_H_

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif
#define CB_MKTIME mktime

/**
 *@brief micro second unit sleep
 */
#define CB_USLEEP cb_lld_usleep

/**
 *@brief second unit sleep
 */
#define CB_SLEEP cb_lld_sleep

/**
 * @brief Performs a nanosecond sleep for the specified duration.
 *
 * @param ts64 The duration in nanoseconds.
 * @param rts64 Pointer to store the remaining time if interrupted.
 * @return 0 on success, or -1 on error.
 */
int cb_nanosleep64(int64_t ts64, int64_t *rts64);

/**
 * @brief Sleeps for the specified number of seconds.
 *
 * @param sec The number of seconds to sleep.
 */
int cb_lld_sleep(uint32_t sec);

/**
 * @brief Sleeps for the specified number of microseconds.
 *
 * @param usec The number of microseconds to sleep.
 */
int cb_lld_usleep(uint32_t usec);

/**
 * @brief Retrieves the current time in nanoseconds.
 *
 * @param ctype The clock type to use.
 * @return The current time in nanoseconds.
 */
uint64_t cb_lld_gettime64(ub_clocktype_t ctype);

#ifdef __cplusplus
}
#endif

#endif

/** @}*/
