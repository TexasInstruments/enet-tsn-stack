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
#include "port_announce_receive_sm.h"
#include "gptpcommon.h"

#define SELECTED_STATE	    sm->ptasg->selectedState
#define PATH_TRACE	    sm->bptasg->pathTrace
#define RCVD_ANNOUNCE_PTR   sm->bppg->rcvdAnnouncePtr
#define RCVD_MSG	    sm->bppg->rcvdMsg
#define PORT_OPER	    sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED    sm->ppg->ptpPortEnabled
#define AS_CAPABLE	    sm->ppg->asCapable
#define RCVD_ANNOUNCE	    sm->thisSM->rcvdAnnounce

typedef enum {
	INIT,
	DISCARD,
	RECEIVE,
	REACTION,
}port_announce_receive_state_t;

static bool qualifyAnnounce(port_announce_receive_data_t *sm)
{
	uint16_t i, N;

	if(memcmp(RCVD_ANNOUNCE_PTR->header.sourcePortIdentity.clockIdentity,
		   sm->ptasg->thisClock, sizeof(ClockIdentity))==0){
		UB_LOG(UBL_DEBUGV, "port_announce_receive:%s:Announce sent by self\n", __func__);
		return false;
	}
	if(RCVD_ANNOUNCE_PTR->stepsRemoved >= 255u){
		UB_LOG(UBL_DEBUGV, "port_announce_receive:%s:stepsRemoved=%d\n", __func__,
		       RCVD_ANNOUNCE_PTR->stepsRemoved);
		return false;
	}
	if (RCVD_ANNOUNCE_PTR->tlvType == 0x8u){
		/* 10.6.3.3.4 pathSequence (ClockIdentify[N]
		   N = stepsRemoved+1 (pathSequence increases by 1 for each system traversed.
		   We also limit the pathSequence to MAX_. */
		// ??? Ideally we use stepsRemoved+1 to determine the length of the TLV
		// however, this is not always the case, some MasterPort implementation when
		// grandMaster priority has changed and the candidate GM is in the same pathTrace
		// the clockPriority vectors are not updated (resulting to Repeated or Inferior)
		// but pathTrace will be updated and is not equal to step remove anymore
		N = (RCVD_ANNOUNCE_PTR->tlvLength/sizeof(ClockIdentity)) < (uint8_t)MAX_PATH_TRACE_N ?
			(RCVD_ANNOUNCE_PTR->tlvLength/sizeof(ClockIdentity)) : (uint8_t)MAX_PATH_TRACE_N;
		for(i=0;i<N;i++){
			if(memcmp(RCVD_ANNOUNCE_PTR->pathSequence[i], sm->ptasg->thisClock,
				  sizeof(ClockIdentity))==0){
				UB_LOG(UBL_DEBUGV, "port_announce_receive:%s:Announce already traversed\n",
				       __func__);
				return false;
			}
		}
		if(SELECTED_STATE[sm->ppg->thisPort] == (uint8_t)SlavePort){
			if ((N+1u) <= (uint8_t)MAX_PATH_TRACE_N){
				/* path trace TLV copy to pathTrace */
				memcpy(PATH_TRACE, RCVD_ANNOUNCE_PTR->pathSequence,
				       sizeof(ClockIdentity) * N);
				/* append thisClock to pathTrace */
				memcpy(&(PATH_TRACE[N]), sm->ptasg->thisClock, sizeof(ClockIdentity));
				sm->bptasg->pathTraceCount = N+1u;
			}else{
				UB_LOG(UBL_WARN, "port_announce_receive:%s:pathTrace=%d "
				       "(including thisClock) exceeds limit=%d\n",
				       __func__, N+1u, MAX_PATH_TRACE_N);
				/* 10.3.8.23 ... a path trace TLV is not appended to an Announce message
				   and the pathTrace array is empty, once appending a clockIdentity
				   to the TLV would cause the frame carrying the Announce to exceed
				   its maximum size. */
				sm->bptasg->pathTraceCount = 0;
			}
		}
	}else {
		/* path trace TLV not present, empty pathTrace */
		(void)memset(PATH_TRACE, 0, sizeof(ClockIdentity) * (uint8_t)MAX_PATH_TRACE_N);
	}
	UB_LOG(UBL_DEBUGV, "port_announce_receive:%s:announce qualified\n", __func__);
	return true;
}

static port_announce_receive_state_t allstate_condition(port_announce_receive_data_t *sm)
{
	if((sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	    (RCVD_ANNOUNCE && (!PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE))) &&
	    (sm->bptasg->externalPortConfiguration == VALUE_DISABLED)){
			return DISCARD;
	}
	return (port_announce_receive_state_t)sm->state;
}

static void *discard_proc(port_announce_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_ANNOUNCE = false;
	RCVD_MSG = false;
	return NULL;
}

static port_announce_receive_state_t discard_condition(port_announce_receive_data_t *sm)
{
	if(RCVD_ANNOUNCE && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE){return RECEIVE;}
	return DISCARD;
}

static void *receive_proc(port_announce_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_ANNOUNCE = false;
	RCVD_MSG = qualifyAnnounce(sm);
	return NULL;
}

static port_announce_receive_state_t receive_condition(port_announce_receive_data_t *sm)
{
	if(RCVD_ANNOUNCE && PORT_OPER && PTP_PORT_ENABLED &&
	   AS_CAPABLE && !RCVD_MSG){return RECEIVE;}
	return (port_announce_receive_state_t)sm->state;
}

void *port_announce_receive_sm(port_announce_receive_data_t *sm, uint64_t cts64)
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
			sm->state = DISCARD;
			break;
		case DISCARD:
			if(state_change){retp=discard_proc(sm);}
			sm->state = discard_condition(sm);
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

void port_announce_receive_sm_init(port_announce_receive_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	BmcsPerTimeAwareSystemGlobal *bptasg,
	BmcsPerPortGlobal *bppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(port_announce_receive_data_t, PortAnnounceReceiveSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->bptasg = bptasg;
	(*sm)->bppg = bppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;

	(void)port_announce_receive_sm(*sm, 0);
}

int port_announce_receive_sm_close(port_announce_receive_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void port_announce_receive_sm_recv_announce(port_announce_receive_data_t *sm,
					    PTPMsgAnnounce *rcvdAnnounce,
					    uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d, state=%d\n",
	       __func__, sm->domainIndex, sm->portIndex, sm->state);
	RCVD_ANNOUNCE = true;
	memcpy(&sm->rcvdAnnounce, rcvdAnnounce, sizeof(PTPMsgAnnounce));
	RCVD_ANNOUNCE_PTR = &sm->rcvdAnnounce;
	sm->last_state = DISCARD;
	(void)port_announce_receive_sm(sm, cts64);
}
