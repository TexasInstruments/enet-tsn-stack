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
#include "gptp_perfmon.h"
#include "mind.h"
#include "mdeth.h"
#include "gptpnet.h"
#include "gptpclock.h"
#include "clock_slave_sync_sm.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	INITIALIZING,
	SEND_SYNC_INDICATION,
	REACTION,
}clock_slave_sync_state_t;

#define RCVD_PSSYNC sm->thisSM->rcvdPSSync
#define RCVD_PSSYNC_PTR sm->thisSM->rcvdPSSyncPtr
#define RCVD_LOCAL_CLOCK_TICK sm->thisSM->rcvdLocalClockTick
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static void updateSlaveTime(clock_slave_sync_data_t *sm)
{
	int64_t ts64;
	struct timespec ts;
	UB_LOG(UBL_DEBUGV, "clock_slave_sync:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	// ??? regardless of sm->ptasg->gmPresent, do this way
	ts64=gptpclock_getts64(GPTPINSTNUM, sm->ptasg->thisClockIndex, sm->ptasg->domainIndex);
	UB_NSEC2TS(ts64, ts);
	sm->ptasg->clockSlaveTime.seconds.lsb=ts.tv_sec;
	sm->ptasg->clockSlaveTime.fractionalNanoseconds.msb=ts.tv_nsec;
}

static clock_slave_sync_state_t allstate_condition(clock_slave_sync_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ) {
		return INITIALIZING;
	}
	return (clock_slave_sync_state_t)sm->state;
}

static void *initializing_proc(clock_slave_sync_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "clock_slave_sync:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	RCVD_PSSYNC = false;
	return NULL;
}

static clock_slave_sync_state_t initializing_condition(clock_slave_sync_data_t *sm)
{
	if(RCVD_PSSYNC || RCVD_LOCAL_CLOCK_TICK){return SEND_SYNC_INDICATION;}
	return INITIALIZING;
}

static void *send_sync_indication_proc(clock_slave_sync_data_t *sm)
{
	uint64_t nsec;
	void *smret=NULL;
	UB_LOG(UBL_DEBUGV, "clock_slave_sync:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	// localPortIndex==0 means, sync by ClockMasterSyncSend,
	// and don't need to issue syncReciptTime event
	if (RCVD_PSSYNC && RCVD_PSSYNC_PTR->localPortIndex ) {
		nsec=((RCVD_PSSYNC_PTR->preciseOriginTimestamp.seconds.lsb * (uint64_t)UB_SEC_NS) +
		      RCVD_PSSYNC_PTR->preciseOriginTimestamp.nanoseconds) +
			(uint64_t)RCVD_PSSYNC_PTR->followUpCorrectionField.nsec;
		if(RCVD_PSSYNC_PTR->local_ppg!=NULL){
			nsec += RCVD_PSSYNC_PTR->local_ppg->forAllDomain->neighborPropDelay.nsec *
				(RCVD_PSSYNC_PTR->rateRatio /
				 RCVD_PSSYNC_PTR->local_ppg->forAllDomain->neighborRateRatio) +
				RCVD_PSSYNC_PTR->local_ppg->forAllDomain->delayAsymmetry.nsec;
		}

		sm->ptasg->lastSyncSeqID = RCVD_PSSYNC_PTR->lastSyncSeqID;
		sm->ptasg->syncReceiptTime.seconds.lsb = nsec / (uint64_t)UB_SEC_NS;
		sm->ptasg->syncReceiptTime.fractionalNanoseconds.msb = nsec % (uint64_t)UB_SEC_NS;

		sm->ptasg->syncReceiptLocalTime.nsec = RCVD_PSSYNC_PTR->upstreamTxTime.nsec;
		if(RCVD_PSSYNC_PTR->local_ppg!=NULL){
			sm->ptasg->syncReceiptLocalTime.nsec +=
				(RCVD_PSSYNC_PTR->local_ppg->forAllDomain->neighborPropDelay.nsec /
				 RCVD_PSSYNC_PTR->local_ppg->forAllDomain->neighborRateRatio) +
				(RCVD_PSSYNC_PTR->local_ppg->forAllDomain->delayAsymmetry.nsec /
				 RCVD_PSSYNC_PTR->rateRatio);
		}

		sm->ptasg->gmTimeBaseIndicator = RCVD_PSSYNC_PTR->gmTimeBaseIndicator;
		sm->ptasg->lastGmPhaseChange = RCVD_PSSYNC_PTR->lastGmPhaseChange;
		sm->ptasg->lastGmFreqChange = RCVD_PSSYNC_PTR->lastGmFreqChange;

		/* 'static port config' and 'this is not GM'
		   Since BMCS is not performed, port_announce port_state_selection SMs
		   will not be able to get GM information.
		   For static port slave mode, consider the peer clockId as the GM.
		*/
		if(gptpgcfg_get_intitem(
			   GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_STATIC_PORT_STATE_SLAVE_PORT,
			   YDBI_CONFIG)>0){
			if(memcmp(sm->ptasg->gmIdentity, RCVD_PSSYNC_PTR->sourcePortIdentity.clockIdentity,
				  sizeof(ClockIdentity))!=0){
				memcpy(sm->ptasg->gmIdentity,
				       RCVD_PSSYNC_PTR->sourcePortIdentity.clockIdentity,
				       sizeof(ClockIdentity));
				(void)gptpclock_set_gmchange(GPTPINSTNUM, sm->ptasg->domainIndex,
							     sm->ptasg->gmIdentity, false);
			}
		}

		// this may need IPC notice
		//invokeApplicationInterfaceFunction (ClockTargetPhaseDiscontinuity.result);
		UB_LOG(UBL_DEBUGV, "clock_slave_sync: syncReceiptTime\n");
		smret = &sm->ptasg->syncReceiptTime;
	}
	if(RCVD_LOCAL_CLOCK_TICK){updateSlaveTime(sm);}
	RCVD_PSSYNC = false;
	RCVD_LOCAL_CLOCK_TICK = false;
	return smret;
}

static clock_slave_sync_state_t send_sync_indication_condition(clock_slave_sync_data_t *sm)
{
	if(RCVD_PSSYNC || RCVD_LOCAL_CLOCK_TICK) {
		sm->last_state = REACTION;
	}
	return SEND_SYNC_INDICATION;
}


void *clock_slave_sync_sm(clock_slave_sync_data_t *sm, uint64_t cts64)
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
			if(state_change){retp=initializing_proc(sm);}
			sm->state = initializing_condition(sm);
			break;
		case SEND_SYNC_INDICATION:
			if(state_change){retp=send_sync_indication_proc(sm);}
			sm->state = send_sync_indication_condition(sm);
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

void clock_slave_sync_sm_init(clock_slave_sync_data_t **sm,
			      int domainIndex,
			      PerTimeAwareSystemGlobal *ptasg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, domainIndex);
	INIT_SM_DATA(clock_slave_sync_data_t, ClockSlaveSyncSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->domainIndex = domainIndex;
}

int clock_slave_sync_sm_close(clock_slave_sync_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, (*sm)->domainIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *clock_slave_sync_sm_loclClockUpdate(clock_slave_sync_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, sm->domainIndex);
	RCVD_LOCAL_CLOCK_TICK = true;
	return clock_slave_sync_sm(sm, cts64);
}

void *clock_slave_sync_sm_portSyncSync(clock_slave_sync_data_t *sm,
				       PortSyncSync *portSyncSync, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, sm->domainIndex);
	RCVD_PSSYNC = true;
	RCVD_PSSYNC_PTR = portSyncSync;
	return clock_slave_sync_sm(sm, cts64);
}
