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
 * @defgroup Rate reporter
 * @{
 * @file cb_rate_reporter.h
 *
 * @brief Ultility for rate reporter
 */

#ifndef CB_RATE_REPORTER_H__
#define CB_RATE_REPORTER_H__
#include <tsn_unibase/unibase.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Rate reporter params
 */
typedef struct cb_rate_reporter {
	uint64_t start_ts;	   //!< Reporting start time
	uint64_t last_ts;	   //!< Reporting last time (work as a pair with start_ts to gen total rate)
	uint64_t end_ts;	   //!< End of every last receiving/sending interval. Use to calculate bytes to be sent b/w (current time - end_ts)
	uint64_t total_size;   //!< Total size sent for whole time
	uint64_t period_size;  //!< Total size sent for period time
	uint64_t send_byte;    //!< Byte need to be sent to guarantee the rate
	uint64_t period_ns;    //!< Period in nanosecond
	uint64_t latency_min;  //!< MIN latency recorded
	uint64_t latency_max;  //!< MAX latency recorded
	uint64_t latency_accum;//!< Total latency recorded
	uint64_t latency_count;//!< Number of latency. Work as a pair with latency_accum to calc average latency
} cb_rate_reporter_t;

/**
 * @brief Init rate_reporter
 *
 * @param brater Bitrate reporter
 * @param report_period report_period in Nanosec
 */
void cb_rate_reporter_init(struct cb_rate_reporter *brater, uint64_t report_period);

/**
 * @brief The function to be call at the beginning of each talker loop
 * 	Record the timestamp of beginning loop cycle and calculate number of bytes 
 *  to be send with duration of beginning talker loop with previous ended loop.
 *
 * @param brater Bitrate reporter
 * @param mbps Mbps
 * @return number of bytes to be send out during duration (start talker loop ts - last ended loop ts)
 */
uint64_t cb_rate_reporter_gen(struct cb_rate_reporter *brater, int mbps);

/**
 * @brief Calculate total rate, period rate and latency
 *
 * @param brater Bitrate reporter
 * @param[out] rate output total rate
 * @param[out] prate output period rate
 * @param[out] latency_avg out average latency (in case of listener)
 */
void cb_rate_reporter_calc(struct cb_rate_reporter *brater, double *rate, double *prate, uint64_t *latency_avg);

/**
 * @brief Update total bytes received every received cycle
 *
 * @param brater Bitrate reporter
 * @param bytes bytes received
 */
void cb_rate_reporter_recv_update_bytes(struct cb_rate_reporter *brater, int bytes);

/**
 * @brief Update total bytes sent every sending cycle
 *
 * @param brater Bitrate reporter
 * @param bytes bytes sent
 */
void cb_rate_reporter_send_update_bytes(struct cb_rate_reporter *brater, int bytes);

/**
 * @brief Reset period counter every last sending/receiving cycle
 *
 * @param brater Bitrate reporter
 */
void cb_rate_reporter_period_reset(struct cb_rate_reporter *brater);

/**
 * @brief Update latency b/w current receive packet and previous received packet
 *
 * @param brater Bitrate reporter
 * @param latency ts diff between current and previous received packet
 */
void cb_rate_reporter_update_latency(struct cb_rate_reporter *brater, uint64_t latency);

/**
 * @brief Call this function at the end of each loop.
 *  in case of reporting time >= reporting interval, the function will print talker/receiver rate with default format
 * @note Client can choose to call this function or self-custom the printing format (for ex: avtp_testclient)
 * @param brater Bitrate reporter
 * @param talker is talker or not
 */
void cb_rate_reporter_period_print(struct cb_rate_reporter *brater, bool talker);

#endif // CB_RATE_REPORTER_H__