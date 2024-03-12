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
#include "cb_rate_reporter.h"
#include "combase_private.h"

void cb_rate_reporter_init(struct cb_rate_reporter *brater, uint64_t report_period)
{
	if (brater)
	{
		memset(brater, 0, sizeof(struct cb_rate_reporter));
		brater->period_ns = report_period;
	}
}

uint64_t cb_rate_reporter_gen(struct cb_rate_reporter *brater, int mbps)
{
	double size;
	uint64_t dts, ts, size_i;

	if(brater->start_ts <= 0){
		brater->start_ts = ub_mt_gettime64();
		brater->end_ts = brater->start_ts;
		brater->last_ts = brater->start_ts;
	}

	ts = ub_mt_gettime64();
	dts = ts - brater->end_ts;

	brater->end_ts = ts;
	if(dts == 0){return 0;}

	size_i = mbps * 1048576 * dts;
	size_i /= ( (uint64_t)8 * (uint64_t)1e9 );
	size = (double)size_i;

	brater->send_byte += (uint64_t)size;

	return brater->send_byte;
}

void cb_rate_reporter_calc(struct cb_rate_reporter *brater, double *rate, double *prate, uint64_t *latency_avg)
{
	uint64_t dts;

	//total rate
	dts = brater->end_ts - brater->start_ts;
	*rate = ((double)brater->total_size * 8.0)/((double)dts/1.0e9);
	*rate = *rate/1048576.0;

	//period rate
	dts = brater->end_ts - brater->last_ts;
	*prate = ((double)brater->period_size * 8.0)/((double)dts/1.0e9);
	*prate = *prate/1048576.0;

    if (brater->latency_count != 0) // Apply for listener only
    {
	    *latency_avg = brater->latency_accum/brater->latency_count;
    }
}

void cb_rate_reporter_recv_update_bytes(struct cb_rate_reporter *brater, int bytes)
{
	if(brater->start_ts <= 0){
		brater->start_ts = ub_mt_gettime64();
		brater->end_ts = brater->start_ts;
		brater->last_ts = brater->start_ts;
	}
	brater->period_size += bytes;
	brater->total_size += bytes;
	brater->end_ts = ub_mt_gettime64();
}

void cb_rate_reporter_send_update_bytes(struct cb_rate_reporter *brater, int bytes)
{
	brater->period_size += bytes;
	brater->total_size += bytes;
	if(brater->send_byte >= (uint64_t)bytes){
		brater->send_byte -= bytes;
	}else{
		brater->send_byte= 0;
	}
}

void cb_rate_reporter_period_reset(struct cb_rate_reporter *brater)
{
	if(brater->end_ts - brater->last_ts >= brater->period_ns){
		brater->period_size = 0;
		brater->last_ts = brater->end_ts;
		brater->latency_accum = 0;
		brater->latency_count = 0;
	}
}

void cb_rate_reporter_update_latency(struct cb_rate_reporter *brater, uint64_t latency)
{
	if(brater->latency_max == 0){ //first time
		brater->latency_min = latency;
		brater->latency_max = latency;
	}else{
		if(brater->latency_min > latency){brater->latency_min = latency;}

		if(brater->latency_max < latency){brater->latency_max = latency;}
	}

	brater->latency_accum += latency;
	brater->latency_count++;
}

void cb_rate_reporter_period_print(struct cb_rate_reporter *brater, bool talker)
{
	if(brater->total_size == 0){return;}

	if(brater->end_ts - brater->last_ts >= brater->period_ns){
		double rate, prate;
		uint64_t latency_avg;

		cb_rate_reporter_calc(brater, &rate, &prate, &latency_avg);

		if(talker){
			UB_LOG(UBL_INFO,"+++++[Talker] Mbps(period=%f, total=%f)\n", prate, rate);
		}else{
			UB_LOG(UBL_INFO, "---[Listener] Mbps(period=%f, total=%f) Delay usec(min=%d, avg=%d, max=%d)\n",
				   prate,
				   rate,
				   (int)(brater->latency_min/1000),
				   (int)(latency_avg/1000),
				   (int)(brater->latency_max/1000));
		}
	}
}