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
#include "port_announce_information_ext_sm.h"
#include "gptpcommon.h"

#define SELECTED_STATE	    sm->ptasg->selectedState
#define PATH_TRACE	    sm->bptasg->pathTrace
#define RCVD_ANNOUNCE_PTR   sm->bppg->rcvdAnnouncePtr
#define UPDT_INFO	    sm->bppg->updtInfo
#define PORT_OPER	    sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED    sm->ppg->ptpPortEnabled
#define AS_CAPABLE	    sm->ppg->asCapable
#define MESSAGE_PRIORITY    sm->thisSM->messagePriority

typedef enum {
	INIT,
	INITIALIZE,
	RECEIVE,
	REACTION,
}port_announce_information_ext_state_t;

static void* rcvdInfoExt(port_announce_information_ext_data_t *sm)
{
	uint16_t N;
	/* Store the messagePriorityVector and stepRemoved received */
	MESSAGE_PRIORITY.rootSystemIdentity.priority1 = RCVD_ANNOUNCE_PTR->grandmasterPriority1;
	MESSAGE_PRIORITY.rootSystemIdentity.clockClass =
		RCVD_ANNOUNCE_PTR->grandmasterClockQuality.clockClass;
	MESSAGE_PRIORITY.rootSystemIdentity.clockAccuracy = RCVD_ANNOUNCE_PTR->
		grandmasterClockQuality.clockAccuracy;
	MESSAGE_PRIORITY.rootSystemIdentity.offsetScaledLogVariance = RCVD_ANNOUNCE_PTR->
		grandmasterClockQuality.offsetScaledLogVariance;
	MESSAGE_PRIORITY.rootSystemIdentity.priority2 = RCVD_ANNOUNCE_PTR->grandmasterPriority2;
	memcpy(&MESSAGE_PRIORITY.rootSystemIdentity.clockIdentity,
	       RCVD_ANNOUNCE_PTR->grandmasterIdentity, sizeof(ClockIdentity));
	MESSAGE_PRIORITY.stepsRemoved = RCVD_ANNOUNCE_PTR->stepsRemoved;
	memcpy(&MESSAGE_PRIORITY.sourcePortIdentity, &RCVD_ANNOUNCE_PTR->header.sourcePortIdentity,
	       sizeof(PortIdentity));
	MESSAGE_PRIORITY.portNumber = md_port_index2number(sm->ppg->thisPort);
	sm->bppg->messageStepsRemoved = RCVD_ANNOUNCE_PTR->stepsRemoved;

	if (SELECTED_STATE[sm->ppg->thisPort] == (uint8_t)SlavePort){
		N = (((RCVD_ANNOUNCE_PTR->stepsRemoved)+1u) < (uint8_t)MAX_PATH_TRACE_N) ?
			(RCVD_ANNOUNCE_PTR->stepsRemoved+1u) : (uint8_t)MAX_PATH_TRACE_N;
		if (N < (uint8_t)MAX_PATH_TRACE_N) {
			/* path trace TLV copy to pathTrace */
			memcpy(PATH_TRACE, RCVD_ANNOUNCE_PTR->pathSequence,
			       sizeof(ClockIdentity) * N);
			/* append thisClock to pathTrace */
			memcpy(&(PATH_TRACE[N]), sm->ptasg->thisClock, sizeof(ClockIdentity));
		}
		else {
			UB_LOG(UBL_WARN, "port_announce_receive:%s:pathTrace=%d exceeds limit=%d\n", __func__,
			       N, MAX_PATH_TRACE_N);
			/* 10.3.8.23 ... a path trace TLV is not appended to an Announce message
			   and the pathTrace array is empty, once appending a clockIdentity
			   to the TLV would cause the frame carrying the Announce to exceed
			   its maximum size. */
			(void)memset(PATH_TRACE, 0, sizeof(ClockIdentity) * (uint8_t)MAX_PATH_TRACE_N);
		}
	}
	// return messagePriority PortStateSettingExtSM reference
	return &MESSAGE_PRIORITY;
}

static void recordOtherAnnounceInfo(port_announce_information_ext_data_t *sm)
{
	sm->bppg->annLeap61 = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x01u) == 0x01u;
	sm->bppg->annLeap59 = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x02u) == 0x02u;
	sm->bppg->annCurrentUtcOffsetValid = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x04u) == 0x04u;
	sm->bppg->annPtpTimescale = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x08u) == 0x08u;
	sm->bppg->annTimeTraceable = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x10u) == 0x10u;
	sm->bppg->annFrequencyTraceable = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x20u) == 0x20u;
	sm->bppg->annCurrentUtcOffset = RCVD_ANNOUNCE_PTR->currentUtcOffset;
	sm->bppg->annTimeSource = RCVD_ANNOUNCE_PTR->timeSource;
}

static port_announce_information_ext_state_t allstate_condition(port_announce_information_ext_data_t *sm)
{
	if (((!PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE) ||
	     sm->ptasg->BEGIN || !sm->ptasg->instanceEnable) &&
	    (sm->bptasg->externalPortConfiguration == VALUE_ENABLED)){
		return INITIALIZE;
	}
	return (port_announce_information_ext_state_t)sm->state;
}

static void *initialize_proc(port_announce_information_ext_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information_ext:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	sm->thisSM->rcvdAnnounce = false;
	UPDT_INFO = false;
	return NULL;
}

static port_announce_information_ext_state_t initialize_condition(port_announce_information_ext_data_t *sm)
{
	if (PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE && sm->thisSM->rcvdAnnounce){
		return RECEIVE;
	}
	return INITIALIZE;
}

static void *receive_proc(port_announce_information_ext_data_t *sm)
{
	void *retv;
	UB_LOG(UBL_DEBUGV, "port_announce_information_ext:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	retv = rcvdInfoExt(sm);
	recordOtherAnnounceInfo(sm);
	sm->bppg->portStepsRemoved = sm->bppg->messageStepsRemoved + 1u;
	// messageStepsRemoved is set by rcvInfoExt()
	return retv;
}

static port_announce_information_ext_state_t receive_condition(port_announce_information_ext_data_t *sm)
{
	if (PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE && sm->thisSM->rcvdAnnounce){
		return REACTION;
	}
	return RECEIVE;
}

void *port_announce_information_ext_sm(port_announce_information_ext_data_t *sm, uint64_t cts64)
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
			sm->state = INITIALIZE;
			break;
		case INITIALIZE:
			if(state_change){retp=initialize_proc(sm);}
			sm->state = initialize_condition(sm);
			break;
		case RECEIVE:
			if(state_change){retp=receive_proc(sm);}
			sm->state = receive_condition(sm);
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

void port_announce_information_ext_sm_init(port_announce_information_ext_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	BmcsPerTimeAwareSystemGlobal *bptasg,
	BmcsPerPortGlobal *bppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(port_announce_information_ext_data_t,
		     PortAnnounceInformationExtSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->bptasg = bptasg;
	(*sm)->bppg = bppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int port_announce_information_ext_sm_close(port_announce_information_ext_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}
