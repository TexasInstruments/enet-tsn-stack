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
#include "md_pdelay_resp_sm.h"
#include "md_abnormal_hooks.h"
#include "gptpcommon.h"
#include "gptp_perfmon.h"

typedef enum {
	INIT,
	NOT_ENABLED,
	INITIAL_WAITING_FOR_PDELAY_REQ,
	WAITING_FOR_PDELAY_REQ,
	SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP,
	REACTION,
}md_pdelay_resp_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

/**
 * 802.1AS 2020 CMLDS mode
 * NOTE 4 In general, a port can receive:
a) Peer delay messages of the CMLDS,
b) PTP Instance-specific peer delay messages of domain 0 (with sdoId of 0x100), and
c) If there are other PTP profiles on the neighbor port that use instance-specific peer delay, peer delay messages of
those profiles.
The port responds to the messages of type a) if it invokes CMLDS, the messages of type b) if it invokes gPTP domain 0,
and the messages of type c) if it invokes the respective other PTP profiles.

Basically:
1/ CMLDS mode: we will copy pdelayreq MajorSdoId to pdelayresp/pdelayresp fup
2/ Domain mode (non CMLDS): it means we only have 1 Domain (#0) active, we will just reply with our domain #0.
*/
static void setMajorSdoIdPdelayResp(md_pdelay_resp_data_t *sm, MDPTPMsgPdelayResp *sdata)
{
	if(sm->cmlds_mode){
		sdata->head.majorSdoId_messageType = (sdata->head.majorSdoId_messageType & 0x0Fu) |
			(sm->thisSM->rcvdPdelayReqPtr->head.majorSdoId_messageType & 0xF0u);
		sdata->head.domainNumber = sm->thisSM->rcvdPdelayReqPtr->head.domainNumber;
	}else{
		sdata->head.majorSdoId_messageType =
			(sdata->head.majorSdoId_messageType & 0x0Fu) | 0x10u;
		sdata->head.domainNumber = 0;
	}

}

static void setMajorSdoIdPdelayRespFollowUp(md_pdelay_resp_data_t *sm, MDPTPMsgPdelayRespFollowUp *sdata)
{
	if(sm->cmlds_mode){
		sdata->head.majorSdoId_messageType = (sdata->head.majorSdoId_messageType & 0x0Fu) |
			(sm->thisSM->rcvdPdelayReqPtr->head.majorSdoId_messageType & 0xF0u);
		sdata->head.domainNumber = sm->thisSM->rcvdPdelayReqPtr->head.domainNumber;
	}else{
		sdata->head.majorSdoId_messageType =
			(sdata->head.majorSdoId_messageType & 0x0Fu) | 0x10u;
		sdata->head.domainNumber = 0;
	}
}

static MDPTPMsgPdelayRespFollowUp *setPdelayRespFollowUp(md_pdelay_resp_data_t *sm)
{
	MDPTPMsgPdelayRespFollowUp *sdata;
	int ssize=sizeof(MDPTPMsgPdelayRespFollowUp);
	struct timespec ts;

	sdata=(MDPTPMsgPdelayRespFollowUp *)md_header_compose(
		GPTPINSTNUM, sm->gpnetd, sm->portIndex,
		PDELAY_RESP_FOLLOW_UP, ssize,
		sm->ptasg->thisClock, sm->ppg->thisPort,
		ntohs(sm->thisSM->rcvdPdelayReqPtr->head.sequenceId_ns),
		0x7f);
	if(!sdata){return NULL;}
	setMajorSdoIdPdelayRespFollowUp(sm, sdata);
	// As the fractional ns portion can't be measure, leave the 'correctionField' as 0
	UB_NSEC2TS(sm->ts3, ts);
	sdata->requestOriginTimestamp.seconds_lsb_nl=htonl(ts.tv_sec);
	sdata->requestOriginTimestamp.nanoseconds_nl=htonl(ts.tv_nsec);
	memcpy(&sdata->requestingPortIdentity,
	       &sm->thisSM->rcvdPdelayReqPtr->head.sourcePortIdentity,
	       sizeof(MDPortIdentity));
	memcpy(&sm->txPdelayRespFollowUp, sdata, sizeof(MDPTPMsgPdelayRespFollowUp));
	return &sm->txPdelayRespFollowUp;
}

static int txPdelayRespFollowUp(gptpnet_data_t *gpnetd, int portIndex)
{
	int ssize=sizeof(MDPTPMsgPdelayRespFollowUp);
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d\n",__func__, portIndex);
	return gptpnet_send_whook(gpnetd, portIndex-1, ssize);
}

static MDPTPMsgPdelayResp *setPdelayResp(md_pdelay_resp_data_t *sm)
{
	MDPTPMsgPdelayResp *sdata;
	int ssize=sizeof(MDPTPMsgPdelayResp);
	struct timespec ts;

	sdata=(MDPTPMsgPdelayResp *)md_header_compose(
		GPTPINSTNUM, sm->gpnetd, sm->portIndex, PDELAY_RESP, ssize,
		sm->ptasg->thisClock, sm->ppg->thisPort,
		ntohs(sm->thisSM->rcvdPdelayReqPtr->head.sequenceId_ns),
		0x7f);
	if(!sdata){return NULL;}
	setMajorSdoIdPdelayResp(sm, sdata);
	// As the fractional ns portion can't be measure, leave the 'correctionField' as 0
	UB_NSEC2TS(sm->ts2, ts);
	sdata->requestReceiptTimestamp.seconds_lsb_nl=htonl(ts.tv_sec);
	sdata->requestReceiptTimestamp.nanoseconds_nl=htonl(ts.tv_nsec);
	memcpy(&sdata->requestingPortIdentity,
	       &sm->thisSM->rcvdPdelayReqPtr->head.sourcePortIdentity,
	       sizeof(MDPortIdentity));
	memcpy(&sm->txPdelayResp, sdata, sizeof(MDPTPMsgPdelayResp));
	return &sm->txPdelayResp;
}

static int txPdelayResp(gptpnet_data_t *gpnetd, int portIndex)
{
	int ssize=sizeof(MDPTPMsgPdelayResp);
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d\n",__func__, portIndex);
	return gptpnet_send_whook(gpnetd, portIndex-1, ssize);
}

static md_pdelay_resp_state_t allstate_condition(md_pdelay_resp_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ppg->forAllDomain->portOper ||
	   !sm->thisSM->portEnabled1){return NOT_ENABLED;}
	return (md_pdelay_resp_state_t)sm->state;
}

static void *not_enabled_proc(md_pdelay_resp_data_t *sm)
{
	if(PORT_OPER){
		UB_LOG(UBL_DEBUGV, "md_pdelay_resp:%s:portIndex=%d\n",
		       __func__, sm->portIndex);
	}
	return NULL;
}

static md_pdelay_resp_state_t not_enabled_condition(md_pdelay_resp_data_t *sm)
{
	if(sm->thisSM->portEnabled1){return INITIAL_WAITING_FOR_PDELAY_REQ;}
	return NOT_ENABLED;
}

static void *initial_waiting_for_pdelay_req_proc(md_pdelay_resp_data_t *sm)
{
	if(PORT_OPER){
		UB_LOG(UBL_DEBUGV, "md_pdelay_resp:%s:portIndex=%d\n",
		       __func__, sm->portIndex);
	}
	sm->thisSM->rcvdPdelayReq = false;
	sm->last_seqid = -1;
	return NULL;
}

static md_pdelay_resp_state_t initial_waiting_for_pdelay_req_condition(md_pdelay_resp_data_t *sm)
{
	if(sm->thisSM->rcvdPdelayReq){return SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP;}
	return INITIAL_WAITING_FOR_PDELAY_REQ;
}

static int waiting_for_pdelay_req_proc(md_pdelay_resp_data_t *sm)
{
	if(PORT_OPER){
		UB_LOG(UBL_DEBUGV, "md_pdelay_resp:%s:portIndex=%d\n",
		       __func__, sm->portIndex);
	}
	sm->thisSM->txPdelayRespFollowUpPtr =
		setPdelayRespFollowUp(sm);
	if(txPdelayRespFollowUp(sm->gpnetd, sm->portIndex)==-1){return -1;}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, pDelayRespFollowUpTx);
	return 0;
}

static md_pdelay_resp_state_t waiting_for_pdelay_req_condition(md_pdelay_resp_data_t *sm)
{
	if(sm->thisSM->rcvdPdelayReq){return SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP;}
	return WAITING_FOR_PDELAY_REQ;
}

static int sent_pdelay_resp_waiting_for_timestamp_proc(md_pdelay_resp_data_t *sm,
						       uint64_t cts64)
{
	int res;
	UB_LOG(UBL_DEBUGV, "md_pdelay_resp:%s:portIndex=%d\n", __func__, sm->portIndex);
	sm->thisSM->rcvdPdelayReq = false;
	sm->thisSM->txPdelayRespPtr = setPdelayResp(sm);
	sm->thisSM->rcvdMDTimestampReceive = false;
	res=txPdelayResp(sm->gpnetd, sm->portIndex);
	if(res==-1){return -1;}
	sm->txPdelayResp_time=cts64;
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, pDelayRespTx);
	if(res<0){
		sm->mock_txts64=gptpclock_getts64(GPTPINSTNUM, sm->ptasg->thisClockIndex,0);
		res=-res;
	}
	return res;
}

static md_pdelay_resp_state_t sent_pdelay_resp_waiting_for_timestamp_condition(
	md_pdelay_resp_data_t *sm, uint64_t cts64)
{
	if(sm->thisSM->rcvdMDTimestampReceive){return WAITING_FOR_PDELAY_REQ;}
	if((cts64 - sm->txPdelayResp_time) >=
	   gptpnet_txtslost_time(sm->gpnetd, sm->portIndex-1)){
		if(sm->mock_txts64!=0u){
			sm->ts3=sm->mock_txts64;
			sm->mock_txts64=0;
			return WAITING_FOR_PDELAY_REQ;
		}
		UB_TLOG(UBL_WARN, "md_pdelay_resp:%s:missing TxTS, portIndex=%d, seqID=%d\n",
			__func__, sm->portIndex,
			ntohs(sm->thisSM->rcvdPdelayReqPtr->head.sequenceId_ns));
		// send again the same PdelayResp
		sm->last_state=REACTION;
		return SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP;
		// Or ignore this PdelayReq, and wait next one
		//return INITIAL_WAITING_FOR_PDELAY_REQ
	}
	return SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP;
}

// return 1 when PdelayResp is sent, otherwise return 0;
int md_pdelay_resp_sm(md_pdelay_resp_data_t *sm, uint64_t cts64)
{
	bool state_change;

	if(!sm){return 0;}
	sm->state = allstate_condition(sm);

	while(true){
		state_change=(sm->last_state != sm->state);
		sm->last_state = sm->state;
		switch(sm->state){
		case INIT:
			sm->state = NOT_ENABLED;
			break;
		case NOT_ENABLED:
			if(state_change){(void)not_enabled_proc(sm);}
			sm->state = not_enabled_condition(sm);
			break;
		case INITIAL_WAITING_FOR_PDELAY_REQ:
			if(state_change){(void)initial_waiting_for_pdelay_req_proc(sm);}
			sm->state = initial_waiting_for_pdelay_req_condition(sm);
			break;
		case WAITING_FOR_PDELAY_REQ:
			if(state_change){
				if(waiting_for_pdelay_req_proc(sm)!=0){
					sm->last_state=REACTION;
					return 0;
				}
			}
			sm->state = waiting_for_pdelay_req_condition(sm);
			break;
		case SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP:
			if(state_change){
				if(sent_pdelay_resp_waiting_for_timestamp_proc(sm, cts64)<0){
					sm->last_state=REACTION;
					return 0;
				}
				return 1;
			}
			sm->state = sent_pdelay_resp_waiting_for_timestamp_condition(sm, cts64);
			break;
		case REACTION:
		default:
			break;
		}
		if(sm->last_state == sm->state){break;}
	}
	return 0;
}

void md_pdelay_resp_sm_init(md_pdelay_resp_data_t **sm,
			    int portIndex,
			    gptpnet_data_t *gpnetd,
			    PerTimeAwareSystemGlobal *ptasg,
			    PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d\n", __func__, portIndex);
	INIT_SM_DATA(md_pdelay_resp_data_t, MDPdelayRespSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->gpnetd = gpnetd;
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->portIndex = portIndex;
	(*sm)->cmlds_mode = gptpgcfg_get_intitem(
		ptasg->gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_CMLDS_MODE,
		YDBI_CONFIG);
	// 11.2.18.2.5
	if((*sm)->cmlds_mode!=0){
		// ??? cmldsLinkPortEnabled for CMLDS, but we use ptpPortEnabled here
		(*sm)->thisSM->portEnabled1 = (*sm)->ppg->ptpPortEnabled;
	}else{
		(*sm)->thisSM->portEnabled1 = (*sm)->ppg->ptpPortEnabled;
	}
	(*sm)->is2011BackwardCompatible=true; // not receive any msg can be considered as true
}

int md_pdelay_resp_sm_close(md_pdelay_resp_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d\n", __func__, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

int md_pdelay_resp_sm_recv_req(md_pdelay_resp_data_t *sm, event_data_recv_t *edrecv,
			       uint64_t cts64)
{
	uint16_t recsqid;
	uint8_t majorSdoId;
	UB_TLOG(UBL_DEBUGV, "%s:port=%d, state=%d\n",__func__, sm->portIndex, sm->state);
	memcpy(&sm->rcvdPdelayReq, edrecv->recbptr, sizeof(MDPTPMsgPdelayReq));
	sm->thisSM->rcvdPdelayReqPtr = &sm->rcvdPdelayReq;

	// Currently will validate only majorSdoId 0x2 (CMLDS) and 0x1(Domain)
	majorSdoId = (sm->thisSM->rcvdPdelayReqPtr->head.majorSdoId_messageType & 0xF0u);
	if(majorSdoId !=0x10u &&  majorSdoId !=0x20u){
			UB_LOG(UBL_DEBUGV,
			       "%s:port=%d, invalid majorSdoId=0x%x on gPTP domain, ignore event.\n",
			       __func__, sm->portIndex, majorSdoId);
			return 0;
	}
	sm->is2011BackwardCompatible= (majorSdoId==0x10 &&
		sm->thisSM->rcvdPdelayReqPtr->head.domainNumber == 0x00)?true:false;

	recsqid=ntohs(sm->thisSM->rcvdPdelayReqPtr->head.sequenceId_ns);
	if(sm->last_seqid >= 0){
		uint16_t expsqid;
		expsqid=(sm->last_seqid+1)&0xffff;
		if(expsqid != recsqid){
			UB_TLOG(UBL_WARN, "%s: port=%d, expected SeqID=%u, but received=%u\n",
				__func__, sm->portIndex, expsqid, recsqid);
			sm->last_seqid=-1;
			return 0;
		}
	}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, pDelayReqRx);
	sm->last_seqid=recsqid;
	sm->thisSM->rcvdPdelayReq = true;
	sm->ts2=edrecv->ts64 - sm->ppg->ingressLatency.scaledNanoseconds;

	UB_LOG(UBL_DEBUGV, "%s:ts64=%"PRIu64", ingressLatency=%"PRId64", ts2=%"PRIu64"\n",
	       __func__, edrecv->ts64, sm->ppg->ingressLatency.scaledNanoseconds, sm->ts2);
	return md_pdelay_resp_sm(sm, cts64);
}

void md_pdelay_resp_sm_txts(md_pdelay_resp_data_t *sm, event_data_txts_t *edtxts,
			    uint64_t cts64)
{
	int pi;
	UB_LOG(UBL_DEBUGV, "%s:portIndex=%d, received seqID=%d\n",
	       __func__, sm->portIndex, edtxts->seqid);
	if(md_abnormal_timestamp(PDELAY_RESP, sm->portIndex-1, -1, edtxts)!=0){return;}
	if((sm->state!=SENT_PDELAY_RESP_WAITING_FOR_TIMESTAMP) ||
	   (edtxts->seqid != ntohs(sm->thisSM->rcvdPdelayReqPtr->head.sequenceId_ns))){
		UB_TLOG(UBL_WARN, "%s:TxTS is not expected, seqid expected=%d, received=%d\n",
			__func__, ntohs(sm->thisSM->rcvdPdelayReqPtr->head.sequenceId_ns),
			edtxts->seqid);
		return;
	}
	pi=gptpgcfg_get_intitem(GPTPINSTNUM,
				XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE,
				YDBI_CONFIG)?1:sm->portIndex;
	(void)gptpclock_tsconv(GPTPINSTNUM, &edtxts->ts64, pi, 0,
			       sm->ptasg->thisClockIndex, sm->ptasg->domainIndex);
	sm->ts3=edtxts->ts64 + sm->ppg->egressLatency.scaledNanoseconds;
	UB_LOG(UBL_DEBUGV, "%s:ts64=%"PRIu64", egressLatency=%"PRId64", ts3=%"PRIu64"\n",
	       __func__, edtxts->ts64, sm->ppg->egressLatency.scaledNanoseconds, sm->ts3);
	sm->thisSM->rcvdMDTimestampReceive = true;
	(void)md_pdelay_resp_sm(sm, cts64);
}
