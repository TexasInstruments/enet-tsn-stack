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
#include "md_announce_send_sm.h"
#include "md_abnormal_hooks.h"
#include "gptpcommon.h"
#include "gptp_perfmon.h"

typedef enum {
	INIT,
	INITIALIZE,
	SEND_ANNOUNCE,
	REACTION,
}md_announce_send_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define AS_CAPABLE sm->ppg->asCapable
#define TXANN sm->thisSM->txAnnouncePtr
#define RCVDTXANN sm->thisSM->rcvdTxAnnounce
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static int sendAnnounce(md_announce_send_data_t *sm)
{
	MDPTPMsgAnnounce *sdata;
	int N;
	int ssize=sizeof(MDPTPMsgAnnounce);

	// truncate uncessary container */
	N = TXANN->tlvLength / sizeof(ClockIdentity);
	ssize=ssize-((MAX_PATH_TRACE_N-N)*(int)sizeof(ClockIdentity));
	sdata=(MDPTPMsgAnnounce *)md_header_compose(GPTPINSTNUM,
				sm->gpnetd, sm->portIndex, ANNOUNCE, ssize,
				sm->ptasg->thisClock,
				sm->ppg->thisPort,
				TXANN->header.sequenceId,
				sm->bppg->currentLogAnnounceInterval);
	if(!sdata){return -1;}
	sdata->head.domainNumber=
		md_domain_index2number(sm->ptasg->domainIndex);
	sdata->head.flags[0] = TXANN->header.flags[0];
	sdata->head.flags[1] = TXANN->header.flags[1];
	sdata->currentUtcOffset_ns = htons(TXANN->currentUtcOffset);
	sdata->grandmasterPriority1 = TXANN->grandmasterPriority1;
	sdata->grandmasterClockQuality.clockClass =
		TXANN->grandmasterClockQuality.clockClass;
	sdata->grandmasterClockQuality.clockAccuracy =
		TXANN->grandmasterClockQuality.clockAccuracy;
	sdata->grandmasterClockQuality.offsetScaledLogVariance_ns =
		htons(TXANN->grandmasterClockQuality.offsetScaledLogVariance);
	sdata->grandmasterPriority2 = TXANN->grandmasterPriority2;
	memcpy(&sdata->grandmasterIdentity, &TXANN->grandmasterIdentity,
			sizeof(ClockIdentity));
	sdata->stepsRemoved_ns = htons(TXANN->stepsRemoved);
	sdata->timeSource = TXANN->timeSource;
	sdata->tlvType_ns = htons(TXANN->tlvType);
	sdata->tlvLength_ns = htons(TXANN->tlvLength);
	if (TXANN->tlvLength > 0u){
		memcpy(&sdata->pathSequence, &TXANN->pathSequence, (int)sizeof(ClockIdentity) * N);
	} else {
		/* 802.1AS and AVNU differs in the behavior when there is no PathTrace
		 * TLV appended in the Announce message.
		 * 802.1AS requires the PathTrace field to exist, with length=0 to
		 * indicate empty PathTrace.
		 * But AVNU, requires the PathTrace field to be absent in cases where
		 * there is no PathTrace TLV.
		 *
		 * Check configuration if it is necessary to conform with AVNU.
		 */
		if(sm->ptasg->conformToAvnu){
			// truncate PathTrace field
			ssize-=(int)sizeof(sdata->tlvType_ns);
			ssize-=(int)sizeof(sdata->tlvLength_ns);
		}
	}

	if(gptpnet_send_whook(sm->gpnetd, sm->portIndex-1, ssize)==-1){return -2;}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, announceTx);
	return 0;
}

static md_announce_send_state_t allstate_condition(md_announce_send_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   !PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE) {
		sm->last_state=REACTION;
		return INITIALIZE;
	}
	return (md_announce_send_state_t)sm->state;
}

static void *initialize_proc(md_announce_send_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_announce_send:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	return NULL;
}

static md_announce_send_state_t initialize_condition(md_announce_send_data_t *sm)
{
	if(RCVDTXANN){return SEND_ANNOUNCE;}
	return INITIALIZE;
}

static int send_announce_proc(md_announce_send_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_announce_send:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVDTXANN=false;
	return sendAnnounce(sm);
}

static md_announce_send_state_t send_announce_condition(md_announce_send_data_t *sm)
{
	if(RCVDTXANN){sm->last_state=REACTION;}
	return SEND_ANNOUNCE;
}


void *md_announce_send_sm(md_announce_send_data_t *sm, uint64_t cts64)
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
		case SEND_ANNOUNCE:
			if(state_change){
				if(send_announce_proc(sm)==-2) {
					sm->last_state=REACTION;
					return NULL;
				}
				break;
			}
			sm->state = send_announce_condition(sm);
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

void md_announce_send_sm_init(md_announce_send_data_t **sm,
	int domainIndex, int portIndex,
	gptpnet_data_t *gpnetd,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	BmcsPerPortGlobal *bppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(md_announce_send_data_t, MDAnnounceSendSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->gpnetd = gpnetd;
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->bppg = bppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int md_announce_send_sm_close(md_announce_send_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *md_announce_send_sm_mdAnnouncSend(md_announce_send_data_t *sm,
					PTPMsgAnnounce *msgAnnounce, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVDTXANN=true;
	TXANN=msgAnnounce;
	sm->last_state=REACTION;
	return md_announce_send_sm(sm, cts64);
}
