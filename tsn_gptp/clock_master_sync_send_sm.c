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
#include <tsn_unibase/unibase.h>
#include "mind.h"
#include "mdeth.h"
#include "gptpnet.h"
#include "gptpclock.h"
#include "clock_master_sync_send_sm.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	INITIALIZING,
	SEND_SYNC_INDICATION,
	REACTION,
}clock_master_sync_send_state_t;

#define SYNC_SEND_TIME sm->thisSM->syncSendTime
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static void *setPSSyncCMSS(clock_master_sync_send_data_t *sm)
{
	int64_t ts64;
	struct timespec ts;
	sm->portSyncSync.localPortIndex = 0;
	sm->portSyncSync.local_ppg = NULL;
	sm->portSyncSync.domainIndex = sm->ptasg->domainIndex;

	/* for upstreamTxTime, use sm->ptasg->thisClockIndex,
	   for preciseOriginTimestamp, use masterclock(clockIndex=0),
	   the both time must be exactly the same time, so that 'gptpclock_apply_offset'
	   is needed. */
	ts64=gptpclock_getts64(GPTPINSTNUM, sm->ptasg->thisClockIndex, sm->ptasg->domainIndex);
	sm->portSyncSync.upstreamTxTime.nsec = ts64;
	(void)gptpclock_apply_offset(GPTPINSTNUM, &ts64, 0, sm->ptasg->domainIndex);
	UB_NSEC2TS(ts64, ts);
	sm->portSyncSync.preciseOriginTimestamp.seconds.lsb = ts.tv_sec;
	sm->portSyncSync.preciseOriginTimestamp.nanoseconds = ts.tv_nsec;

	sm->portSyncSync.followUpCorrectionField.subns = 0;
	// gmRateRatio * (currentTime – localTime) must be 0

	memcpy(sm->portSyncSync.sourcePortIdentity.clockIdentity, sm->ptasg->thisClock,
	       sizeof(ClockIdentity));
	sm->portSyncSync.sourcePortIdentity.portIndex = 0;
	sm->portSyncSync.logMessageInterval = sm->ptasg->clockMasterLogSyncInterval;
	sm->portSyncSync.syncReceiptTimeoutTime.nsec = (uint64_t)(-1LL);
	sm->portSyncSync.rateRatio = sm->ptasg->gmRateRatio;
	sm->portSyncSync.gmTimeBaseIndicator = sm->ptasg->clockSourceTimeBaseIndicator;
	sm->portSyncSync.lastGmPhaseChange = sm->ptasg->clockSourceLastGmPhaseChange;
	sm->portSyncSync.lastGmFreqChange = sm->ptasg->clockSourceLastGmFreqChange;

	/* syncNextSendTimeoutTime from ClockMasterSyncSend can be indefinite, is
	 * not expected to miss or lose sending of Sync when we are ClockMaster
	 */
	sm->portSyncSync.syncNextSendTimeoutTime.nsec = (uint64_t)(-1LL);

	return &sm->portSyncSync;
}

static clock_master_sync_send_state_t allstate_condition(clock_master_sync_send_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ) {
		return INITIALIZING;
	}
	return (clock_master_sync_send_state_t)sm->state;
}

static void *initializing_proc(clock_master_sync_send_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "clock_master_sync_send:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	SYNC_SEND_TIME.nsec = cts64 + sm->ptasg->clockMasterSyncInterval.nsec;
	return NULL;
}

static clock_master_sync_send_state_t initializing_condition(clock_master_sync_send_data_t *sm,
	uint64_t cts64)
{
	if(cts64 >= SYNC_SEND_TIME.nsec){return SEND_SYNC_INDICATION;}
	return INITIALIZING;
}

static void *send_sync_indication_proc(clock_master_sync_send_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "clock_master_sync_send:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	SYNC_SEND_TIME.nsec = cts64 + sm->ptasg->clockMasterSyncInterval.nsec;
	return setPSSyncCMSS(sm);
}

static clock_master_sync_send_state_t send_sync_indication_condition(
	clock_master_sync_send_data_t *sm, uint64_t cts64)
{
	if(cts64 >= SYNC_SEND_TIME.nsec){
		sm->last_state = REACTION;
	}
	return SEND_SYNC_INDICATION;
}


void *clock_master_sync_send_sm(clock_master_sync_send_data_t *sm, uint64_t cts64)
{
	bool state_change;
	void *retp=NULL;

	if(!sm){return NULL;}
	sm->state = allstate_condition(sm);

	while(true){
		state_change=(sm->last_state != sm->state);
		sm->last_state = sm->state;
		switch(sm->state){
		case INIT:
			sm->state = INITIALIZING;
			break;
		case INITIALIZING:
			if(state_change){retp=initializing_proc(sm, cts64);}
			sm->state = initializing_condition(sm, cts64);
			break;
		case SEND_SYNC_INDICATION:
			if(state_change){
				retp=send_sync_indication_proc(sm, cts64);
				if(retp!=NULL){break;}
			}
			sm->state = send_sync_indication_condition(sm, cts64);
			break;
		case REACTION:
		default:
			break;
		}
		if(retp!=NULL){return retp;}
		if(sm->last_state == sm->state){break;}
	}
	return retp;
}

void clock_master_sync_send_sm_init(clock_master_sync_send_data_t **sm,
				    int domainIndex,
				    PerTimeAwareSystemGlobal *ptasg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, domainIndex);
	INIT_SM_DATA(clock_master_sync_send_data_t, ClockMasterSyncSendSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->domainIndex = domainIndex;
}

int clock_master_sync_send_sm_close(clock_master_sync_send_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, (*sm)->domainIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}
