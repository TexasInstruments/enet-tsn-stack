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
#include "md_signaling_receive_sm.h"
#include "gptp_perfmon.h"

typedef enum {
	INIT,
	INITIALIZE,
	RECV_SIGNALING,
	REACTION,
}md_signaling_receive_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled

static void *recv_gptp_capable(md_signaling_receive_data_t *sm)
{
	MDPTPMsgGPTPCapableTLV *gcmsg=(MDPTPMsgGPTPCapableTLV *)sm->rcvd_rxmsg;
	//PTPMsgHeader header;
	//md_decompose_head((MDPTPMsgHeader *)sm->rcvd_rxmsg, &header);
	sm->gctlm.tlvType = ntohs(gcmsg->tlvType_ns);
	sm->gctlm.lengthField = ntohs(gcmsg->lengthField_ns);
	memcpy(&sm->gctlm.organizationId, gcmsg->organizationId, 3);
	sm->gctlm.organizationSubType =
		(gcmsg->organizationSubType_nb[0] << 16) |
		(gcmsg->organizationSubType_nb[1] << 8) |
		gcmsg->organizationSubType_nb[2];
	sm->gctlm.logGptpCapableMessageInterval=gcmsg->logGptpCapableMessageInterval;
	sm->gctlm.flags = gcmsg->flags;
	return &sm->gctlm;
}

static void *recv_msg_interval_req(md_signaling_receive_data_t *sm)
{
	MDPTPMsgIntervalRequestTLV *mrmsg=(MDPTPMsgIntervalRequestTLV *)sm->rcvd_rxmsg;
	//PTPMsgHeader header;
	//md_decompose_head((MDPTPMsgHeader *)sm->rcvd_rxmsg, &header);
	sm->mrtlm.tlvType = ntohs(mrmsg->tlvType_ns);
	sm->mrtlm.lengthField = ntohs(mrmsg->lengthField_ns);
	memcpy(&sm->mrtlm.organizationId, mrmsg->organizationId, 3);
	sm->mrtlm.organizationSubType =
		(mrmsg->organizationSubType_nb[0] << 16) |
		(mrmsg->organizationSubType_nb[1] << 8) |
		mrmsg->organizationSubType_nb[2];
	sm->mrtlm.linkDelayInterval = mrmsg->linkDelayInterval;
	sm->mrtlm.timeSyncInterval = mrmsg->timeSyncInterval;
	sm->mrtlm.announceInterval = mrmsg->announceInterval;
	sm->mrtlm.flags = mrmsg->flags;
	return &sm->mrtlm;
}

static md_signaling_receive_state_t allstate_condition(md_signaling_receive_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   !PORT_OPER || !PTP_PORT_ENABLED) {
		sm->last_state=REACTION;
		return INITIALIZE;
	}
	return (md_signaling_receive_state_t)sm->state;
}

static void *initialize_proc(md_signaling_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_signaling_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	return NULL;
}

static md_signaling_receive_state_t initialize_condition(md_signaling_receive_data_t *sm)
{
	if(sm->recv){return RECV_SIGNALING;}
	return INITIALIZE;
}

static void *recv_signaling_proc(md_signaling_receive_data_t *sm)
{
	uint8_t stype;
	UB_LOG(UBL_DEBUGV, "md_signaling_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	sm->recv=false;
	stype=((MDPTPMsgGPTPCapableTLV*)(sm->rcvd_rxmsg))->organizationSubType_nb[2];
	if(stype==2u){
		PERFMON_PPMSDR_INC(sm->ppg->perfmonDS, msgIntervalRx);
		return recv_msg_interval_req(sm);
	}
	else if(stype==4u){
		PERFMON_PPMSDR_INC(sm->ppg->perfmonDS, asCapableRx);
		return recv_gptp_capable(sm);
	}else{}
	UB_LOG(UBL_WARN, "%s:unknown tlv type = %d\n", __func__, stype);
	return NULL;
}

static md_signaling_receive_state_t recv_signaling_condition(md_signaling_receive_data_t *sm)
{
	if(sm->recv){sm->last_state=REACTION;}
	return RECV_SIGNALING;
}


void *md_signaling_receive_sm(md_signaling_receive_data_t *sm, uint64_t cts64)
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
		case RECV_SIGNALING:
			if(state_change){
				retp=recv_signaling_proc(sm);
				break;
			}
			sm->state = recv_signaling_condition(sm);
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

void md_signaling_receive_sm_init(md_signaling_receive_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	if(!*sm){
		*sm=(md_signaling_receive_data_t *)UB_SD_GETMEM(SM_DATA_INST, sizeof(md_signaling_receive_data_t));
		if(ub_assert_fatal(*sm, __func__, "malloc")){return;}
	}
	(void)memset(*sm, 0, sizeof(md_signaling_receive_data_t));
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int md_signaling_receive_sm_close(md_signaling_receive_data_t **sm)
{
	if(!*sm){return 0;}
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	UB_SD_RELMEM(SM_DATA_INST, *sm);
	*sm=NULL;
	return 0;
}

void *md_signaling_receive_sm_mdSignalingRec(md_signaling_receive_data_t *sm,
					     event_data_recv_t *edrecv, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	sm->recv=true;
	sm->rcvd_rxmsg=edrecv->recbptr;
	sm->last_state=REACTION;
	PERFMON_PPMSDR_INC(sm->ppg->perfmonDS, signalingRx);
	return md_signaling_receive_sm(sm, cts64);
}
