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
#include "md_announce_receive_sm.h"
#include "gptpcommon.h"
#include "gptp_perfmon.h"

typedef enum {
	INIT,
	INITIALIZE,
	RECV_ANNOUNCE,
	REACTION,
}md_announce_receive_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define AS_CAPABLE sm->ppg->asCapable
#define RXANN sm->thisSM->rxAnnouncePtr
#define RCVDRXANN sm->thisSM->rcvdRxAnnounce
#define IEEE_1588_ANNOUNCE_NO_TLV_SIZE 64
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static void *recAnnounce(md_announce_receive_data_t *sm)
{
	MDPTPMsgAnnounce *md_announce=(MDPTPMsgAnnounce *)RXANN;
	md_decompose_head((MDPTPMsgHeader *)RXANN, &sm->rcvdAnnounce.header);
	if(!sm->cmlds_mode){
		// 802.1AS-2020 8.1 the value of majorSdoId for gPTP domain must be 0x1
		// When device is accepting message under CMLDS domain, allow values
		// other than 0x1
		if(sm->rcvdAnnounce.header.majorSdoId!=0x1u){
			UB_LOG(UBL_DEBUGV, "%s:port=%d, invalid majorSdoId on gPTP domain, ignore ANNOUNCE\n",
					__func__, sm->portIndex);
			return NULL;
		}
	}

	sm->rcvdAnnounce.currentUtcOffset = ntohs(md_announce->currentUtcOffset_ns);
	sm->rcvdAnnounce.grandmasterPriority1 = md_announce->grandmasterPriority1;
	sm->rcvdAnnounce.grandmasterClockQuality.clockClass =
		md_announce->grandmasterClockQuality.clockClass;
	sm->rcvdAnnounce.grandmasterClockQuality.clockAccuracy =
		md_announce->grandmasterClockQuality.clockAccuracy;
	sm->rcvdAnnounce.grandmasterClockQuality.offsetScaledLogVariance =
		ntohs(md_announce->grandmasterClockQuality.offsetScaledLogVariance_ns);
	sm->rcvdAnnounce.grandmasterPriority2 = md_announce->grandmasterPriority2;
	memcpy(&sm->rcvdAnnounce.grandmasterIdentity,
	       &md_announce->grandmasterIdentity, sizeof(ClockIdentity));
	sm->rcvdAnnounce.stepsRemoved = ntohs(md_announce->stepsRemoved_ns);
	sm->rcvdAnnounce.timeSource = md_announce->timeSource;
	if(ntohs(md_announce->head.messageLength_ns) > IEEE_1588_ANNOUNCE_NO_TLV_SIZE){
		sm->rcvdAnnounce.tlvType = ntohs(md_announce->tlvType_ns);
		sm->rcvdAnnounce.tlvLength = ntohs(md_announce->tlvLength_ns);
		// ??? In the case where the master sent inconsistent value
		// where stepsRemoved does not corresponds
		// to the number (N) of elements in the pathSequence, set missing path TLV to all 1
		(void)memset(&sm->rcvdAnnounce.pathSequence, 0xFF,
			     (int)sizeof(ClockIdentity)*MAX_PATH_TRACE_N);
		// ??? In the case where tlvLength from received Announce message is
		// inconsistent, ignore the Announce message, this includes the malformed cases:
		//    - received tlvLength > received Announce TLV data (from offset 68) length
		//    - received tlvLengh/sizeof(ClockIdentity) > MAX_PATH_TRACE_N
		if((sm->rcvdAnnounce.tlvLength <= (
			    ntohs(md_announce->head.messageLength_ns)-
			    (IEEE_1588_ANNOUNCE_NO_TLV_SIZE+
			     (int)sizeof(sm->rcvdAnnounce.tlvType)+
			     (int)sizeof(sm->rcvdAnnounce.tlvLength)))) &&
		   ((sm->rcvdAnnounce.tlvLength/(int)sizeof(ClockIdentity)) <=
		    MAX_PATH_TRACE_N)){
			(void)memcpy(&sm->rcvdAnnounce.pathSequence,
				     &md_announce->pathSequence,
				     sm->rcvdAnnounce.tlvLength);
		}else{
			UB_LOG(UBL_DEBUG, "%s:port=%d, invalid tlvLength=%d, ignore ANNOUNCE\n",
			__func__, sm->portIndex, sm->rcvdAnnounce.tlvLength);
			return NULL;
		}
	}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, announceForeignMasterRx);
	return &sm->rcvdAnnounce;
}

static md_announce_receive_state_t allstate_condition(md_announce_receive_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   !PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE) {
		sm->last_state=REACTION;
		return INITIALIZE;
	}
	return (md_announce_receive_state_t)sm->state;
}

static void *initialize_proc(md_announce_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_announce_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	return NULL;
}

static md_announce_receive_state_t initialize_condition(md_announce_receive_data_t *sm)
{
	if(RCVDRXANN){return RECV_ANNOUNCE;}
	return INITIALIZE;
}

static void *recv_announce_proc(md_announce_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_announce_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVDRXANN=false;
	return recAnnounce(sm);
}

static md_announce_receive_state_t recv_announce_condition(md_announce_receive_data_t *sm)
{
	if(RCVDRXANN){sm->last_state=REACTION;}
	return RECV_ANNOUNCE;
}


void *md_announce_receive_sm(md_announce_receive_data_t *sm, uint64_t cts64)
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
		case RECV_ANNOUNCE:
			if(state_change){
				retp=recv_announce_proc(sm);
				break;
			}
			sm->state = recv_announce_condition(sm);
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

void md_announce_receive_sm_init(md_announce_receive_data_t **sm,
				 int domainIndex, int portIndex,
				 PerTimeAwareSystemGlobal *ptasg,
				 PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(md_announce_receive_data_t, MDAnnounceReceiveSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
	(*sm)->cmlds_mode = gptpgcfg_get_intitem(
		ptasg->gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_CMLDS_MODE,
		YDBI_CONFIG);
}

int md_announce_receive_sm_close(md_announce_receive_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *md_announce_receive_sm_mdAnnounceRec(md_announce_receive_data_t *sm,
					   event_data_recv_t *edrecv, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVDRXANN=true;
	RXANN=edrecv->recbptr;
	sm->last_state=REACTION;
	return md_announce_receive_sm(sm, cts64);
}
