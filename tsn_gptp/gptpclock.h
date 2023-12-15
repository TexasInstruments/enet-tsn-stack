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
#ifndef GPTPCLOCK_H_
#define GPTPCLOCK_H_
#include <stdlib.h>
#include "gptpbasetypes.h"
#include "ll_gptpsupport.h"

#define GPTP_VIRTUAL_PTPDEV_FDBASE 3018
#define GPTP_VIRTUAL_PTPDEV_FDMAX 3117

typedef struct gptpclock_data gptpclock_data_t;

typedef enum {
	PTPCLOCK_NOWORK = 0,
	PTPCLOCK_NOACCESS,
	PTPCLOCK_RDONLY,
	PTPCLOCK_RDWR,
} ptpclock_state_t;

typedef struct gptp_clock_ppara {
	char ptpdev[MAX_PTPDEV_NAME];
	uint8_t domainIndex; //when accessed by domainIndex, need this domainIndex
	int64_t offset64;
	uint8_t gmsync;
	uint32_t gmchange_ind;
	int64_t last_setts64;
	double adjrate;
} gptp_clock_ppara_t;

typedef struct gptp_master_clock_shm_head {
	int max_domains;
	int active_domain;
	CB_THREAD_MUTEX_T mcmutex;
}gptp_master_clock_shm_head_t;

typedef struct gptp_master_clock_shm {
	gptp_master_clock_shm_head_t head;
	gptp_clock_ppara_t gcpp[1];
} gptp_master_clock_shm_t;

#define GPTP_MAX_SIZE_SHARED_MEMNAME 32
#define GPTP_MASTER_CLOCK_SHARED_MEM "/gptp_mc_shm"
#define GPTP_MASTER_CLOCK_MUTEX_TIMEOUT (10*UB_MSEC_NS)
static inline int gptpclock_mutex_trylock(CB_THREAD_MUTEX_T *mutex)
{
	struct timespec mtots;
	int64_t mtout;
	if(CB_THREAD_MUTEX_TRYLOCK(mutex)!=0){
		mtout=(int64_t)ub_rt_gettime64()+GPTP_MASTER_CLOCK_MUTEX_TIMEOUT;
		UB_NSEC2TS(mtout, mtots);
		if(CB_THREAD_MUTEX_TIMEDLOCK(mutex, &mtots)!=0){
			// the mutex is on hold such a long time; the holder must crash
			UB_LOG(UBL_WARN, "%s:the process is very slow,"
			       " or some tsn_gptpd client may crash\n", __func__);
			return -1;
		}
	}
	return 0;
}

int gptpclock_init(uint8_t gptpInstanceIndex, int max_domains, int max_ports);
void gptpclock_close(uint8_t gptpInstanceIndex);
int gptpclock_add_clock(uint8_t gptpInstanceIndex, int clockIndex, char *ptpdev, int domainIndex,
			ClockIdentity id);
int gptpclock_del_clock(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex);
int gptpclock_apply_offset(uint8_t gptpInstanceIndex, int64_t *ts64,
			   int clockIndex, uint8_t domainIndex);
int gptpclock_setts64(uint8_t gptpInstanceIndex, int64_t ts64,
		      int clockIndex, uint8_t domainIndex);
int gptpclock_setadj(uint8_t gptpInstanceIndex, int32_t adjvppb,
		     int clockIndex, uint8_t domainIndex);
void gptpclock_print_clkpara(uint8_t gptpInstanceIndex, ub_dbgmsg_level_t level);
int gptpclock_mode_master(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex);
int gptpclock_mode_slave_main(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex);
int gptpclock_mode_slave_sub(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex);

int64_t gptpclock_getts64(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex);
int64_t gptpclock_gethwts64(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex);
int gptpclock_tsconv(uint8_t gptpInstanceIndex, int64_t *ts64, int clockIndex,
		     uint8_t domainIndex, int clockIndex1, uint8_t domainIndex1);
uint8_t *gptpclock_clockid(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex);
int gptpclock_rate_same(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex,
			int clockIndex1, uint8_t domainIndex1);
int gptpclock_setoffset64(uint8_t gptpInstanceIndex, int64_t ts64, int clockIndex,
			  uint8_t domainIndex);
int gptpclock_active_domain_switch(uint8_t gptpInstanceIndex, int domainIndex);
int gptpclock_active_domain_status(uint8_t gptpInstanceIndex);
bool gptpclock_we_are_gm(uint8_t gptpInstanceIndex, int domainIndex);
int gptpclock_set_gmsync(uint8_t gptpInstanceIndex, uint8_t domainIndex, uint32_t gmstate);
gmsync_status_t gptpclock_get_gmsync(uint8_t gptpInstanceIndex, uint8_t domainIndex);
int gptpclock_set_gmchange(uint8_t gptpInstanceIndex, uint8_t domainIndex,
			   ClockIdentity gmIdentity, bool becomeGM);
int gptpclock_get_gmchange_ind(uint8_t gptpInstanceIndex, int domainIndex);
uint32_t gptpclock_get_event_flags(uint8_t gptpInstanceIndex, int clockIndex,
				   uint8_t domainIndex);
uint32_t gptpclock_set_event_flags(uint8_t gptpInstanceIndex, int clockIndex,
				   uint8_t domainIndex, uint32_t event);
uint32_t gptpclock_reset_event_flags(uint8_t gptpInstanceIndex, int clockIndex,
				     uint8_t domainIndex, uint32_t event);
void gptpclock_set_gmstable(uint8_t gptpInstanceIndex, int domainIndex, bool stable);
bool gptpclock_get_gmstable(uint8_t gptpInstanceIndex, int domainIndex);
int gptpclock_active_domain(uint8_t gptpInstanceIndex);
int64_t gptpclock_d0ClockfromRT(uint8_t gptpInstanceIndex, int clockIndex);

/**
 * @brief set clockIndex to the thisClock, clockIndex=0 is set as the master clock
 * @result 0:success, -1:error
 * @param clockIndex
 * @param comainNumber
 * @param set_clock_para	if true, the phase offset in the master clock is moved
 * to thisClock. the parameters in SLAVE_SUB mode time are also moved.
 * @Note the offset in the master clock is moved into thisClock, which prevent of a big
 * jump from the old GM
 * clockIndex=0 is set to PTPCLOCK_MASTER mode
 * thisClock becomes PTPCLOCK_SLAVE_MAIN mode, but when the same clockIndex is already
 * used as PTPCLOCK_SLAVE_MAIN in a different domain, it becomes PTPCLOCK_SLAVE_SUB mode.
 *
 * when thisClock becomes PTPCLOCK_SLAVE_MAIN, offset and adjrate are set into HW config.
 * when thisClock becomes PTPCLOCK_SLAVE_SUB, offset and adjrate are set into SW config.
 *
 * When this device becomes a new GM, this function should be called, then GM time phase
 * continues from the previous GM.
 */
int gptpclock_set_thisClock(uint8_t gptpInstanceIndex, int clockIndex,
			    uint8_t domainIndex, bool set_clock_para);

/***************************************
 * functions supported in lower layers
 ***************************************/
ptpclock_state_t gptp_get_ptpfd(uint8_t gptpInstanceIndex, char *ptpdev, PTPFD_TYPE *ptpfd);
int gptp_close_ptpfd(PTPFD_TYPE ptpfd);
int gptp_clock_adjtime(PTPFD_TYPE ptpfd, int adjppb);

/**
 * @brief settimeofday by "year:mon:day:hour:min:sec"
 * @result 0:success, -1:error
 * @param tstr	timeofday string in "year:mon:day:hour:min:sec" string format
 */
int gptpclock_settime_str(uint8_t gptpInstanceIndex, char *tstr,
			  int clockIndex, uint8_t domainIndex);

int gptpclock_get_adjppb(uint8_t gptpInstanceIndex, int clockIndex, int domainIndex);

#endif
