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
#include "one_step_tx_oper_setting_sm.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	NOT_ENABLED,
	INITIALIZE,
	SET_ONE_STEP_TX_OPER,
	REACTION,
}one_step_tx_oper_setting_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define USE_MGT_SETTABLE_ONE_STEP_TX_OPER sm->ppg->useMgtSettableOneStepTxOper
#define MGT_SETTABLE_ONE_STEP_TX_OPER sm->ppg->mgtSettableOneStepTxOper
#define CURRENT_ONE_STEP_TX_OPER sm->ppg->currentOneStepTxOper
#define INITIAL_ONE_STEP_TX_OPER sm->ppg->initialOneStepTxOper
#define ONE_STEP_TX_OPER sm->mdeg->oneStepTxOper
#define ONE_STEP_TRANSMIT sm->mdeg->oneStepTransmit

static one_step_tx_oper_setting_state_t allstate_condition(one_step_tx_oper_setting_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   !PORT_OPER || !PTP_PORT_ENABLED ||
	   USE_MGT_SETTABLE_ONE_STEP_TX_OPER ){return NOT_ENABLED;}
	return (one_step_tx_oper_setting_state_t)sm->state;
}

static void *not_enabled_proc(one_step_tx_oper_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "one_step_tx_oper_setting:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);

	if(USE_MGT_SETTABLE_ONE_STEP_TX_OPER){
		CURRENT_ONE_STEP_TX_OPER = MGT_SETTABLE_ONE_STEP_TX_OPER;
		ONE_STEP_TX_OPER = CURRENT_ONE_STEP_TX_OPER && ONE_STEP_TRANSMIT;
	}

	return NULL;
}

static one_step_tx_oper_setting_state_t not_enabled_condition(one_step_tx_oper_setting_data_t *sm)
{
	if(PORT_OPER && PTP_PORT_ENABLED &&
	   !USE_MGT_SETTABLE_ONE_STEP_TX_OPER){return INITIALIZE;}
	return NOT_ENABLED;
}

static void *initialize_proc(one_step_tx_oper_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "one_step_tx_oper_setting:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	CURRENT_ONE_STEP_TX_OPER = INITIAL_ONE_STEP_TX_OPER;
	ONE_STEP_TX_OPER = CURRENT_ONE_STEP_TX_OPER && ONE_STEP_TRANSMIT;
	return NULL;
}

static one_step_tx_oper_setting_state_t initialize_condition(one_step_tx_oper_setting_data_t *sm)
{
	if(sm->thisSM->rcvdSignalingMsg4){return SET_ONE_STEP_TX_OPER;}
	return INITIALIZE;
}

static void *set_one_step_tx_oper_proc(one_step_tx_oper_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "one_step_tx_oper_setting:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);

	CURRENT_ONE_STEP_TX_OPER =
		GET_FLAG_BIT(sm->thisSM->rcvdSignalingPtr->flags,
			     ONE_STEP_RECEIVE_CAPABLE_BIT)?true:false;
	ONE_STEP_TX_OPER = CURRENT_ONE_STEP_TX_OPER && ONE_STEP_TRANSMIT;
	sm->thisSM->rcvdSignalingMsg4 = false;
	return NULL;
}

static one_step_tx_oper_setting_state_t set_one_step_tx_oper_condition(
	one_step_tx_oper_setting_data_t *sm)
{
	if(sm->thisSM->rcvdSignalingMsg4){sm->last_state=REACTION;}
	return SET_ONE_STEP_TX_OPER;
}


void *one_step_tx_oper_setting_sm(one_step_tx_oper_setting_data_t *sm, uint64_t cts64)
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
		case SET_ONE_STEP_TX_OPER:
			if(state_change){retp=set_one_step_tx_oper_proc(sm);}
			sm->state = set_one_step_tx_oper_condition(sm);
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

void one_step_tx_oper_setting_sm_init(one_step_tx_oper_setting_data_t **sm,
				      int domainIndex, int portIndex,
				      PerTimeAwareSystemGlobal *ptasg,
				      PerPortGlobal *ppg,
				      MDEntityGlobal *mdeg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(one_step_tx_oper_setting_data_t, OneStepTxOperSettingSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->mdeg = mdeg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int one_step_tx_oper_setting_sm_close(one_step_tx_oper_setting_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *one_step_tx_oper_setting_SignalingMsg4(one_step_tx_oper_setting_data_t *sm,
					     PTPMsgIntervalRequestTLV *rcvdSignalingPtr,
					     uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	sm->thisSM->rcvdSignalingMsg4 = true;
	sm->thisSM->rcvdSignalingPtr = rcvdSignalingPtr;
	return one_step_tx_oper_setting_sm(sm, cts64);
}
