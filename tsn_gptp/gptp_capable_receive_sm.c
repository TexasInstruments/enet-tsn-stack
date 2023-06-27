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
#include "mind.h"
#include "mdeth.h"
#include "gptpnet.h"
#include "gptpclock.h"
#include "gptp_capable_receive_sm.h"

typedef enum {
	INIT,
	NOT_ENABLED,
	INITIALIZE,
	RECEIVED_TLV,
	REACTION,
}gptp_capable_receive_state_t;

struct gptp_capable_receive_data{
	PerTimeAwareSystemGlobal *ptasg;
	PerPortGlobal *ppg;
	gptp_capable_receive_state_t state;
	gptp_capable_receive_state_t last_state;
	gPtpCapableReceiveSM *thisSM;
	int domainIndex;
	int portIndex;
};

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
// ??? !domainEnabled, Figure 10-20—gPtpCapableReceive state machine
// we set domainEnabled by receiving CMLDS mode PdelayReq
#define DOMAIN_ENABLED (sm->ppg->forAllDomain->receivedNonCMLDSPdelayReq==-1)

static gptp_capable_receive_state_t allstate_condition(gptp_capable_receive_data_t *sm)
{

	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable || !DOMAIN_ENABLED ||
	   !PORT_OPER || !PTP_PORT_ENABLED){return NOT_ENABLED;}
	return sm->state;
}

static void *not_enabled_proc(gptp_capable_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "gptp_capable_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	return NULL;
}

static gptp_capable_receive_state_t not_enabled_condition(gptp_capable_receive_data_t *sm)
{
	if(PORT_OPER && PTP_PORT_ENABLED && DOMAIN_ENABLED){return INITIALIZE;}
	return NOT_ENABLED;
}

static void *initialize_proc(gptp_capable_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "gptp_capable_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	if(!sm->ppg->neighborGptpCapable){return NULL;}
	UB_LOG(UBL_INFO, "reset neighborGptpCapable, domainIndex=%d, portIndex=%d\n",
	       sm->domainIndex, sm->portIndex);
	sm->ppg->neighborGptpCapable = false;
	return NULL;
}

static gptp_capable_receive_state_t initialize_condition(gptp_capable_receive_data_t *sm)
{
	if(sm->thisSM->rcvdGptpCapableTlv){return RECEIVED_TLV;}
	return INITIALIZE;
}

static void *received_tlv_proc(gptp_capable_receive_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "gptp_capable_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);

	sm->thisSM->rcvdGptpCapableTlv = false;
	sm->thisSM->gPtpCapableReceiptTimeoutInterval.nsec =
		sm->ppg->gPtpCapableReceiptTimeout *
		LOG_TO_NSEC(sm->thisSM->rcvdSignalingMsgPtr->logGptpCapableMessageInterval);
	sm->thisSM->timeoutTime.nsec = cts64 +
		sm->thisSM->gPtpCapableReceiptTimeoutInterval.nsec;
	if(sm->ppg->neighborGptpCapable){return NULL;}
	UB_LOG(UBL_INFO, "set neighborGptpCapable, domainIndex=%d, portIndex=%d\n",
	       sm->domainIndex, sm->portIndex);
	sm->ppg->neighborGptpCapable = true;
	return NULL;
}

static gptp_capable_receive_state_t received_tlv_condition(gptp_capable_receive_data_t *sm,
							   uint64_t cts64)
{
	if(cts64 >= sm->thisSM->timeoutTime.nsec){return INITIALIZE;}
	if(sm->thisSM->rcvdGptpCapableTlv){sm->last_state=REACTION;}
	return RECEIVED_TLV;
}


void *gptp_capable_receive_sm(gptp_capable_receive_data_t *sm, uint64_t cts64)
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
		case RECEIVED_TLV:
			if(state_change){retp=received_tlv_proc(sm, cts64);}
			sm->state = received_tlv_condition(sm, cts64);
			break;
		case REACTION:
			break;
		}
		if(retp){return retp;}
		if(sm->last_state == sm->state){break;}
	}
	return retp;
}

void gptp_capable_receive_sm_init(gptp_capable_receive_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	if(INIT_SM_DATA(gptp_capable_receive_data_t, gPtpCapableReceiveSM, sm)){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
	gptp_capable_receive_sm(*sm, 0);
}

int gptp_capable_receive_sm_close(gptp_capable_receive_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *gptp_capable_receive_rcvdSignalingMsg(gptp_capable_receive_data_t *sm,
					    PTPMsgGPTPCapableTLV *gGPTPCapableTLV,
					    uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "gptp_capable_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	sm->thisSM->rcvdGptpCapableTlv = true;
	sm->thisSM->rcvdSignalingMsgPtr = gGPTPCapableTLV;
	return gptp_capable_receive_sm(sm, cts64);
}
