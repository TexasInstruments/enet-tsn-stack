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
#include "md_signaling_send_sm.h"
#include "md_abnormal_hooks.h"
#include "gptp_perfmon.h"

typedef enum {
	INIT,
	INITIALIZE,
	SEND_SIGNALING,
	REACTION,
}md_signaling_send_state_t;

typedef enum {
	MD_SIGNALING_NONE = 0,
	MD_SIGNALING_MSG_INTERVAL_REQ,
	MD_SIGNALING_GPTP_CAPABLE,
} md_signaling_type_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static void setup_gptp_capable_tlv(md_signaling_send_data_t *sm, void *sdata)
{
	MDPTPMsgGPTPCapableTLV *gcmsg=(MDPTPMsgGPTPCapableTLV *)sdata;
	PTPMsgGPTPCapableTLV *gctlm=(PTPMsgGPTPCapableTLV *)sm->rcvd_txmsg;
	gcmsg->tlvType_ns = htons(gctlm->tlvType);
	gcmsg->lengthField_ns = htons(gctlm->lengthField);
	memcpy(gcmsg->organizationId, gctlm->organizationId, 3);
	gcmsg->organizationSubType_nb[0] = (gctlm->organizationSubType >> 16u) & 0xffu;
	gcmsg->organizationSubType_nb[1] = (gctlm->organizationSubType >> 8u) & 0xffu;
	gcmsg->organizationSubType_nb[2] = gctlm->organizationSubType & 0xffu;
	gcmsg->logGptpCapableMessageInterval = gctlm->logGptpCapableMessageInterval;
	gcmsg->flags = gctlm->flags;
}

static void setup_msg_interval_req_tlv(md_signaling_send_data_t *sm, void *sdata)
{
	MDPTPMsgIntervalRequestTLV *mrmsg=(MDPTPMsgIntervalRequestTLV *)sdata;
	PTPMsgIntervalRequestTLV *mrtlm=(PTPMsgIntervalRequestTLV *)sm->rcvd_txmsg;
	mrmsg->tlvType_ns = htons(mrtlm->tlvType);
	mrmsg->lengthField_ns = htons(mrtlm->lengthField);
	memcpy(mrmsg->organizationId, mrtlm->organizationId, 3);
	mrmsg->organizationSubType_nb[0] = (mrtlm->organizationSubType >> 16u) & 0xffu;
	mrmsg->organizationSubType_nb[1] = (mrtlm->organizationSubType >> 8u) & 0xffu;
	mrmsg->organizationSubType_nb[2] = mrtlm->organizationSubType & 0xffu;
	mrmsg->linkDelayInterval = mrtlm->linkDelayInterval;
	mrmsg->timeSyncInterval = mrtlm->timeSyncInterval;
	mrmsg->announceInterval = mrtlm->announceInterval;
	mrmsg->flags = mrtlm->flags;
}

static int sendSignaling(md_signaling_send_data_t *sm)
{
	int size;
	void *sdata;
	switch(sm->stype){
	case MD_SIGNALING_MSG_INTERVAL_REQ:
		size=sizeof(MDPTPMsgIntervalRequestTLV);
		break;
	case MD_SIGNALING_GPTP_CAPABLE:
		size=sizeof(MDPTPMsgGPTPCapableTLV);
		break;
	case MD_SIGNALING_NONE:
	default:
		return -1;
	}

    sdata=md_header_compose(GPTPINSTNUM, sm->gpnetd, sm->portIndex, SIGNALING, size,
                            sm->ptasg->thisClock,
                            sm->ppg->thisPort,
                            sm->sequenceId,
                            0x7f);
	if(!sdata){return -1;}
	// 10.6.4.2.1 targetPortIdentity is 0xff
	(void)memset(&((uint8_t*)sdata)[sizeof(MDPTPMsgHeader)], 0xff, sizeof(MDPortIdentity));
	((MDPTPMsgHeader*)sdata)->domainNumber=
		md_domain_index2number(sm->ptasg->domainIndex);
	switch(sm->stype){
	case MD_SIGNALING_MSG_INTERVAL_REQ:
		setup_msg_interval_req_tlv(sm, sdata);
		// performance monitor incremented prior to actual send, should not be critical
		PERFMON_PPMSDR_INC(sm->ppg->perfmonDS, msgIntervalTx);
		break;
	case MD_SIGNALING_GPTP_CAPABLE:
		setup_gptp_capable_tlv(sm, sdata);
		// performance monitor incremented prior to actual send, should not be critical
		PERFMON_PPMSDR_INC(sm->ppg->perfmonDS, asCapableTx);
		break;
	default:
		return -1;
	}
	if(gptpnet_send_whook(sm->gpnetd, sm->portIndex-1, size)==-1){return -2;}
	sm->sequenceId++;
	return 0;
}

static md_signaling_send_state_t allstate_condition(md_signaling_send_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   !PORT_OPER || !PTP_PORT_ENABLED) {
		sm->last_state=REACTION;
		return INITIALIZE;
	}
	return (md_signaling_send_state_t)sm->state;
}

static void *initialize_proc(md_signaling_send_data_t *sm)
{
	if(PORT_OPER){
		UB_LOG(UBL_DEBUGV, "md_signaling_send:%s:domainIndex=%d, portIndex=%d\n",
		       __func__, sm->domainIndex, sm->portIndex);
	}
	sm->sequenceId=(uint16_t)rand();
	return NULL;
}

static md_signaling_send_state_t initialize_condition(md_signaling_send_data_t *sm)
{
	if((int)sm->stype!=0){return SEND_SIGNALING;}
	return INITIALIZE;
}

static int send_signaling_proc(md_signaling_send_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_signaling_send:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	if(sendSignaling(sm)==-2){return -1;}
	sm->stype=MD_SIGNALING_NONE;
	return 0;
}

static md_signaling_send_state_t send_signaling_condition(md_signaling_send_data_t *sm)
{
	if((int)sm->stype!=0){sm->last_state=REACTION;}
	return SEND_SIGNALING;
}


void *md_signaling_send_sm(md_signaling_send_data_t *sm, uint64_t cts64)
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
		case SEND_SIGNALING:
			if(state_change){
				if(send_signaling_proc(sm)!=0) {
					sm->last_state=REACTION;
					return NULL;
				}
				break;
			}
			sm->state = send_signaling_condition(sm);
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

void md_signaling_send_sm_init(md_signaling_send_data_t **sm,
	int domainIndex, int portIndex,
	gptpnet_data_t *gpnetd,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	if(!*sm){
		*sm=(md_signaling_send_data_t *)UB_SD_GETMEM(SM_DATA_INST, sizeof(md_signaling_send_data_t));
		if(ub_assert_fatal(*sm, __func__, "malloc")){return;}
	}
	(void)memset(*sm, 0, sizeof(md_signaling_send_data_t));
	(*sm)->gpnetd = gpnetd;
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int md_signaling_send_sm_close(md_signaling_send_data_t **sm)
{
	if(!*sm){return 0;}
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	UB_SD_RELMEM(SM_DATA_INST, *sm);
	*sm=NULL;
	return 0;
}

void *md_signaling_send_sm_mdSignalingSend(md_signaling_send_data_t *sm, void *msg,
					   uint64_t cts64)
{
	uint32_t stype;
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	stype=((PTPMsgGPTPCapableTLV *)msg)->organizationSubType;
	sm->stype=MD_SIGNALING_NONE;
	if(stype==2u){
		sm->stype=MD_SIGNALING_MSG_INTERVAL_REQ;
	}else if(stype==4u){
		sm->stype=MD_SIGNALING_GPTP_CAPABLE;
	}else{
		UB_LOG(UBL_WARN,
		       "%s:domainIndex=%d, portIndex=%d unknown organizationSubType=%d\n",
		       __func__, sm->domainIndex, sm->portIndex, (int)stype);
		return NULL;
	}
	sm->rcvd_txmsg=msg;
	sm->last_state=REACTION;
	return md_signaling_send_sm(sm, cts64);
}
