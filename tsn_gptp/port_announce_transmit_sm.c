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
#include "port_announce_transmit_sm.h"
#include "gptpcommon.h"

#define SELECTED_STATE	    sm->ptasg->selectedState
#define OLD_ANNOUNCE_INTERVAL sm->ppg->oldAnnounceInterval
#define ANNOUNCE_SLOWDOWN   sm->bppg->announceSlowdown
#define NEW_INFO	    sm->bppg->newInfo
#define UPDT_INFO	    sm->bppg->updtInfo
#define MASTER_PRIORITY	    sm->bppg->masterPriority
#define ANNOUNCE_RECEIPT_TIMEOUT sm->bppg->announceReceiptTimeout
#define ANNOUNCE_INTERVAL   sm->bppg->announceInterval
#define SELECTED	    sm->bptasg->selected
#define AS_CAPABLE sm->ppg->asCapable

typedef enum {
	INIT,
	TRANSMIT_INIT,
	TRANSMIT_PERIODIC,
	IDLE,
	TRANSMIT_ANNOUNCE,
	REACTION,
}port_announce_transmit_state_t;

static void txAnnounce(port_announce_transmit_data_t *sm)
{
	int N=0;
	sm->announceTx.header.sequenceId++;
	if(sm->bptasg->leap61){sm->announceTx.header.flags[1] |= 0x01;}
	if(sm->bptasg->leap59){sm->announceTx.header.flags[1] |= 0x02;}
	if(sm->bptasg->currentUtcOffsetValid){sm->announceTx.header.flags[1] |= 0x04;}
	if(sm->bptasg->ptpTimescale){sm->announceTx.header.flags[1] |= 0x08;}
	if(sm->bptasg->timeTraceable){sm->announceTx.header.flags[1] |= 0x10;}
	if(sm->bptasg->frequencyTraceable){sm->announceTx.header.flags[1] |= 0x20;}

	sm->announceTx.currentUtcOffset = sm->bptasg->currentUtcOffset;
	sm->announceTx.grandmasterPriority1 = MASTER_PRIORITY.rootSystemIdentity.priority1;
	sm->announceTx.grandmasterClockQuality.clockClass = MASTER_PRIORITY.rootSystemIdentity.clockClass;
	sm->announceTx.grandmasterClockQuality.clockAccuracy =
		MASTER_PRIORITY.rootSystemIdentity.clockAccuracy;
	sm->announceTx.grandmasterClockQuality.offsetScaledLogVariance =
		MASTER_PRIORITY.rootSystemIdentity.offsetScaledLogVariance;
	sm->announceTx.grandmasterPriority2 = MASTER_PRIORITY.rootSystemIdentity.priority2;
	memcpy(&sm->announceTx.grandmasterIdentity, &MASTER_PRIORITY.rootSystemIdentity.clockIdentity,
	       sizeof(ClockIdentity));
	sm->announceTx.stepsRemoved = sm->bptasg->masterStepsRemoved;
	sm->announceTx.timeSource = sm->bptasg->timeSource;
	sm->announceTx.tlvType = 0x8;
	N = sm->bptasg->pathTraceCount;
	if (N <= MAX_PATH_TRACE_N){
		sm->announceTx.tlvLength = sizeof(ClockIdentity)*(uint8_t)N;
		memcpy(&sm->announceTx.pathSequence, &sm->bptasg->pathTrace,
		       sm->announceTx.tlvLength);
	}else{
		UB_LOG(UBL_WARN, "port_announce_transmit:%s:pathTrace=%d exceeds limit=%d\n",
		       __func__, N, MAX_PATH_TRACE_N);
		/* 10.3.8.23 ... a path trace TLV is not appended to an Announce message
		   and the pathTrace array is empty, once appending a clockIdentity
		   to the TLV would cause the frame carrying the Announce to exceed
		   its maximum size. */
		sm->announceTx.tlvLength = 0;
	}
}

static port_announce_transmit_state_t allstate_condition(port_announce_transmit_data_t *sm)
{
	/* 802.1AS-2020 PortAnnounceTransmitSM TRANSMIT_INIT does not
	 * require asCapable condition as per the specification.
	 * However, we opt to add it to minimize procedures done during IDLE when
	 * asCapable is false. */
	if (sm->ptasg->BEGIN || !sm->ptasg->instanceEnable || !AS_CAPABLE){
		return TRANSMIT_INIT;
	}
	return (port_announce_transmit_state_t)sm->state;
}

static void *transmit_init_proc(port_announce_transmit_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_transmit:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);

	NEW_INFO = true;
	ANNOUNCE_SLOWDOWN = false;
	sm->thisSM->numberAnnounceTransmissions = false;
	return NULL;
}

static port_announce_transmit_state_t transmit_init_condition(port_announce_transmit_data_t *sm)
{
	/* unconditional transfer (UCT) */
	return IDLE;
}

static void *transmit_periodic_proc(port_announce_transmit_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_transmit:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	NEW_INFO = NEW_INFO || (SELECTED_STATE[sm->portIndex] == (uint8_t)MasterPort);
	return NULL;
}

static port_announce_transmit_state_t transmit_periodic_condition(port_announce_transmit_data_t *sm)
{
	/* unconditional transfer (UCT) */
	return IDLE;
}

static void *idle_proc(port_announce_transmit_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "port_announce_transmit:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	/* announceSendTime = currentTime + interval2 */
	sm->thisSM->announceSendTime.nsec = cts64 + sm->thisSM->interval2.nsec;
	return NULL;
}

static port_announce_transmit_state_t idle_condition(port_announce_transmit_data_t *sm, uint64_t cts64)
{
	if(NEW_INFO && (SELECTED_STATE[sm->portIndex]==(uint8_t)MasterPort) &&
	   (cts64 < sm->thisSM->announceSendTime.nsec) &&
	   ((SELECTED[sm->portIndex] && !UPDT_INFO) ||
	    (sm->bptasg->externalPortConfiguration == VALUE_ENABLED)) &&
	   !sm->ppg->forAllDomain->asymmetryMeasurementMode){
		return TRANSMIT_ANNOUNCE;
	}
	if((cts64 >= sm->thisSM->announceSendTime.nsec) &&
	   ((SELECTED[sm->portIndex] && !UPDT_INFO) ||
	    (sm->bptasg->externalPortConfiguration == VALUE_DISABLED))){
		return TRANSMIT_PERIODIC;
	}
	return (port_announce_transmit_state_t)sm->state;
}

static void *transmit_announce_proc(port_announce_transmit_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_transmit:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	NEW_INFO = false;
	txAnnounce(sm);

	if(ANNOUNCE_SLOWDOWN){
		if (sm->thisSM->numberAnnounceTransmissions >= (uint8_t)ANNOUNCE_RECEIPT_TIMEOUT){
			memcpy(&sm->thisSM->interval2, &ANNOUNCE_INTERVAL, sizeof(UScaledNs));
			sm->thisSM->numberAnnounceTransmissions = 0;
			ANNOUNCE_SLOWDOWN = false;
		}else{
			memcpy(&sm->thisSM->interval2, &OLD_ANNOUNCE_INTERVAL, sizeof(UScaledNs));
			sm->thisSM->numberAnnounceTransmissions++;
		}
	}else{
		sm->thisSM->numberAnnounceTransmissions = 0;
		memcpy(&sm->thisSM->interval2, &ANNOUNCE_INTERVAL, sizeof(UScaledNs));
	}
	return &sm->announceTx;
}

static port_announce_transmit_state_t transmit_announce_condition(port_announce_transmit_data_t *sm)
{
	/* To prevent calling the idle_proc() function during subsequent IDLE state
	 * processing, which would unexpectedly increase the
	 * sm->thisSM->announceSendTime.nsec */
	sm->last_state = IDLE;
	/* unconditional transfer (UCT) */
	return IDLE;
}


void *port_announce_transmit_sm(port_announce_transmit_data_t *sm, uint64_t cts64)
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
			sm->state = TRANSMIT_INIT;
			break;
		case TRANSMIT_INIT:
			if(state_change){retp=transmit_init_proc(sm);}
			sm->state = transmit_init_condition(sm);
			break;
		case TRANSMIT_PERIODIC:
			if(state_change){retp=transmit_periodic_proc(sm);}
			sm->state = transmit_periodic_condition(sm);
			break;
		case IDLE:
			if(state_change){retp=idle_proc(sm, cts64);}
			sm->state = idle_condition(sm, cts64);
			break;
		case TRANSMIT_ANNOUNCE:
			if(state_change){retp=transmit_announce_proc(sm);}
			sm->state = transmit_announce_condition(sm);
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

void port_announce_transmit_sm_init(port_announce_transmit_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	BmcsPerTimeAwareSystemGlobal *bptasg,
	BmcsPerPortGlobal *bppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(port_announce_transmit_data_t, PortAnnounceTransmitSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->bptasg = bptasg;
	(*sm)->bppg = bppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;

	(void)memset(&(*sm)->announceTx, 0, sizeof(PTPMsgAnnounce));
	(*sm)->thisSM->interval2.nsec = 1000000000LL;
}

int port_announce_transmit_sm_close(port_announce_transmit_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}
