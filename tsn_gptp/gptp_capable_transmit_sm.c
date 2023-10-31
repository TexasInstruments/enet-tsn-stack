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
#include "gptp_capable_transmit_sm.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	NOT_ENABLED,
	INITIALIZE,
	TRANSMIT_TLV,
	REACTION,
}gptp_capable_transmit_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
// ??? domainEnabled, Figure 10-19—gPtpCapableTransmit state machine
// we set domainEnabled by receiving CMLDS mode PdelayReq
#define DOMAIN_ENABLED (sm->ppg->forAllDomain->receivedNonCMLDSPdelayReq==-1)

static void *setGptpCapableTlv(gptp_capable_transmit_data_t *sm)
{
	sm->signalingMsg.tlvType=0x03;
	sm->signalingMsg.lengthField=12;
	sm->signalingMsg.organizationId[0]=0x00;
	sm->signalingMsg.organizationId[1]=0x80;
	sm->signalingMsg.organizationId[2]=0xC2;
	sm->signalingMsg.organizationSubType=4;
	sm->signalingMsg.logGptpCapableMessageInterval = sm->ppg->logGptpCapableMessageInterval;
	// Table 10-15 - Definitions of bits of flags field of message interval request TLV
	SET_RESET_FLAG_BIT(sm->ppg->forAllDomain->computeNeighborPropDelay,
			   sm->signalingMsg.flags, (uint8_t)COMPUTE_NEIGHBOR_PROP_DELAY_BIT);
	SET_RESET_FLAG_BIT(sm->ppg->forAllDomain->computeNeighborRateRatio,
			   sm->signalingMsg.flags, (uint8_t)COMPUTE_NEIGHBOR_RATE_RATIO_BIT);
	// ??? bit2 of flags,  oneStepReceiveCapable
	return &sm->signalingMsg;
}

static gptp_capable_transmit_state_t allstate_condition(gptp_capable_transmit_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable || !DOMAIN_ENABLED ||
	   !PORT_OPER || !PTP_PORT_ENABLED){return NOT_ENABLED;}
	return (gptp_capable_transmit_state_t)sm->state;
}

static void *not_enabled_proc(gptp_capable_transmit_data_t *sm)
{
	if(PORT_OPER){
		UB_LOG(UBL_DEBUGV, "gptp_capable_transmit:%s:domainIndex=%d, portIndex=%d\n",
		       __func__, sm->domainIndex, sm->portIndex);
	}
	return NULL;
}

static gptp_capable_transmit_state_t not_enabled_condition(gptp_capable_transmit_data_t *sm)
{
	if(PORT_OPER && PTP_PORT_ENABLED && DOMAIN_ENABLED){return INITIALIZE;}
	return NOT_ENABLED;
}

static void *initialize_proc(gptp_capable_transmit_data_t *sm, uint64_t cts64)
{
	if(PORT_OPER){
		UB_LOG(UBL_DEBUGV, "gptp_capable_transmit:%s:domainIndex=%d, portIndex=%d\n",
		       __func__, sm->domainIndex, sm->portIndex);
	}
	// the default of logGptpCapableMessageInterval is TBD
	sm->thisSM->signalingMsgTimeInterval.nsec =
		(uint64_t)UB_SEC_NS * (1u << (uint8_t)sm->ppg->logGptpCapableMessageInterval);
	sm->thisSM->intervalTimer.nsec = cts64;
	return NULL;
}

static gptp_capable_transmit_state_t initialize_condition(gptp_capable_transmit_data_t *sm)
{
	return TRANSMIT_TLV;
}

static void *transmit_tlv_proc(gptp_capable_transmit_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "gptp_capable_transmit:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	sm->thisSM->intervalTimer.nsec += sm->thisSM->signalingMsgTimeInterval.nsec;
	//txGptpCapableSignalingMsg (&txSignalingMsgPtr);
	//UB_LOG(UBL_DEBUG, "gptp_capable_transmit:txGptpCapableSignalingMsg\n");
	return setGptpCapableTlv(sm);
}

static gptp_capable_transmit_state_t transmit_tlv_condition(gptp_capable_transmit_data_t *sm,
							    uint64_t cts64)
{
	if(cts64 >= sm->thisSM->intervalTimer.nsec){sm->last_state=REACTION;}
	return TRANSMIT_TLV;
}


void *gptp_capable_transmit_sm(gptp_capable_transmit_data_t *sm, uint64_t cts64)
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
			if(state_change){retp=initialize_proc(sm, cts64);}
			sm->state = initialize_condition(sm);
			break;
		case TRANSMIT_TLV:
			if(state_change){
				retp=transmit_tlv_proc(sm);
				break;
			}
			sm->state = transmit_tlv_condition(sm, cts64);
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

void gptp_capable_transmit_sm_init(gptp_capable_transmit_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(gptp_capable_transmit_data_t, gPtpCapableTransmitSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int gptp_capable_transmit_sm_close(gptp_capable_transmit_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}
