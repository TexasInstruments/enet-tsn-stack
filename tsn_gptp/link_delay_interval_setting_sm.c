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
#include "link_delay_interval_setting_sm.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	NOT_ENABLED,
	INITIALIZE,
	SET_INTERVAL,
	REACTION,
}link_delay_interval_setting_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define USE_MGTSETTABLE_LOG_PDELAY_REQ_INTERVAL \
	sm->ppg->forAllDomain->useMgtSettableLogPdelayReqInterval
#define CURRENT_LOG_PDELAY_REQ_INTERVAL sm->mdeg->forAllDomain->currentLogPdelayReqInterval
#define INITIAL_LOG_PDELAY_REQ_INTERVAL sm->mdeg->forAllDomain->initialLogPdelayReqInterval
#define RCVD_SIGNALING_PTR sm->thisSM->rcvdSignalingPtr
#define RCVD_SIGNALING_MSG1 sm->thisSM->rcvdSignalingMsg1

static link_delay_interval_setting_state_t allstate_condition(
	link_delay_interval_setting_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->thisSM->portEnabled3 ||
	   !PORT_OPER ||  USE_MGTSETTABLE_LOG_PDELAY_REQ_INTERVAL){return NOT_ENABLED;}
	return (link_delay_interval_setting_state_t)sm->state;
}

static void *not_enabled_proc(link_delay_interval_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "link_delay_interval_setting:%s:portIndex=%d\n",
	       __func__, sm->portIndex);

	if(USE_MGTSETTABLE_LOG_PDELAY_REQ_INTERVAL) {
		CURRENT_LOG_PDELAY_REQ_INTERVAL =
			sm->ppg->forAllDomain->mgtSettableLogPdelayReqInterval;
		sm->mdeg->forAllDomain->pdelayReqInterval.nsec =
			LOG_TO_NSEC(CURRENT_LOG_PDELAY_REQ_INTERVAL);
	}
	return NULL;
}

static link_delay_interval_setting_state_t not_enabled_condition(
	link_delay_interval_setting_data_t *sm)
{
	if(PORT_OPER && sm->thisSM->portEnabled3 &&
	   !USE_MGTSETTABLE_LOG_PDELAY_REQ_INTERVAL){return INITIALIZE;}
	return NOT_ENABLED;
}

static void *initialize_proc(link_delay_interval_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "link_delay_interval_setting:%s:portIndex=%d\n",
	       __func__, sm->portIndex);

	CURRENT_LOG_PDELAY_REQ_INTERVAL = INITIAL_LOG_PDELAY_REQ_INTERVAL;
	sm->mdeg->forAllDomain->pdelayReqInterval.nsec =
		LOG_TO_NSEC(INITIAL_LOG_PDELAY_REQ_INTERVAL);
	sm->ppg->forAllDomain->computeNeighborRateRatio = true;
	sm->ppg->forAllDomain->computeNeighborPropDelay = true;
	RCVD_SIGNALING_MSG1 = false;
	return NULL;
}

static link_delay_interval_setting_state_t initialize_condition(
	link_delay_interval_setting_data_t *sm)
{
	if(RCVD_SIGNALING_MSG1){return SET_INTERVAL;}
	return INITIALIZE;
}

static void *set_interval_proc(link_delay_interval_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "link_delay_interval_setting:%s:portIndex=%d\n",
	       __func__, sm->portIndex);

	switch (RCVD_SIGNALING_PTR->linkDelayInterval) {
	case (-128): /* don't change the interval */
		break;
	case 126: /* set interval to initial value */
		CURRENT_LOG_PDELAY_REQ_INTERVAL = INITIAL_LOG_PDELAY_REQ_INTERVAL;
		sm->mdeg->forAllDomain->pdelayReqInterval.nsec =
			LOG_TO_NSEC(INITIAL_LOG_PDELAY_REQ_INTERVAL);
		break;
	default: /* use indicated value; note that the value of 127 instructs the Pdelay
		  * requester to stop sending, in accordance with Table 10-12. */
		sm->mdeg->forAllDomain->pdelayReqInterval.nsec =
			LOG_TO_NSEC(RCVD_SIGNALING_PTR->linkDelayInterval);
		CURRENT_LOG_PDELAY_REQ_INTERVAL = RCVD_SIGNALING_PTR ->linkDelayInterval;
		break;
	}
	sm->ppg->forAllDomain->computeNeighborRateRatio=
		GET_FLAG_BIT(RCVD_SIGNALING_PTR->flags,
			     COMPUTE_NEIGHBOR_RATE_RATIO_BIT)?true:false;
	sm->ppg->forAllDomain->computeNeighborPropDelay =
		GET_FLAG_BIT(RCVD_SIGNALING_PTR->flags,
			     COMPUTE_NEIGHBOR_PROP_DELAY_BIT)?true:false;
	RCVD_SIGNALING_MSG1 = false;
	return NULL;
}

static link_delay_interval_setting_state_t set_interval_condition(
	link_delay_interval_setting_data_t *sm)
{
	if(RCVD_SIGNALING_MSG1){sm->last_state=REACTION;}
	return SET_INTERVAL;
}


void *link_delay_interval_setting_sm(link_delay_interval_setting_data_t *sm, uint64_t cts64)
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
			sm->state = NOT_ENABLED;
			break;
		case NOT_ENABLED:
			if(state_change){retp=not_enabled_proc(sm);}
			sm->state = not_enabled_condition(sm);
			break;
		case INITIALIZE:
			if(state_change){retp=initialize_proc(sm);}
			sm->state = initialize_condition(sm);
			break;
		case SET_INTERVAL:
			if(state_change){retp=set_interval_proc(sm);}
			sm->state = set_interval_condition(sm);
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

void link_delay_interval_setting_sm_init(link_delay_interval_setting_data_t **sm,
	int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	MDEntityGlobal *mdeg)
{
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d\n", __func__, portIndex);
	INIT_SM_DATA(link_delay_interval_setting_data_t,
		     LinkDelayIntervalSettingSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->mdeg = mdeg;
	(*sm)->portIndex = portIndex;
}

int link_delay_interval_setting_sm_close(link_delay_interval_setting_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d\n", __func__, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *link_delay_interval_setting_SignalingMsg1(
	link_delay_interval_setting_data_t *sm,
	PTPMsgIntervalRequestTLV *rcvdSignalingPtr, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d\n", __func__, sm->portIndex);
	RCVD_SIGNALING_MSG1=true;
	RCVD_SIGNALING_PTR=rcvdSignalingPtr;
	return link_delay_interval_setting_sm(sm, cts64);
}
