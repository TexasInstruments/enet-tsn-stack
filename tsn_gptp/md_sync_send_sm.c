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
#include "md_sync_send_sm.h"
#include "md_abnormal_hooks.h"
#include <math.h>
#include "gptpcommon.h"
#include "gptp_perfmon.h"

typedef enum {
	INIT,
	INITIALIZING,
	SEND_SYNC_TWO_STEP,
	SEND_FOLLOW_UP,
	SEND_SYNC_ONE_STEP,
	SET_CORRECTION_FIELD,
	REACTION,
}md_sync_send_state_t;

#define RCVD_MDSYNC sm->thisSM->rcvdMDSync
#define RCVD_MDSYNC_PTR sm->rcvdMDSyncPtr
#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define AS_CAPABLE sm->ppg->asCapable
#define ASYMMETRY_MEASUREMENT_MODE sm->ppg->forAllDomain->asymmetryMeasurementMode
#define ONESTEP_TX_OPER sm->mdeg->oneStepTxOper
#define SYNC_SEQUENCE_ID sm->mdeg->syncSequenceId
#define RCVD_MDTIMESTAMP_RECEIVE sm->thisSM->rcvdMDTimestampReceive
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static void *composeSyncHeader(md_sync_send_data_t *sm, uint64_t cts64, int ssize)
{
	void *sdata;
	MDPTPMsgHeader *head;

	sdata=(MDPTPMsgSync *)md_header_compose(GPTPINSTNUM, sm->gpnetd,
						sm->portIndex, SYNC, ssize,
				RCVD_MDSYNC_PTR->sourcePortIdentity.clockIdentity,
				RCVD_MDSYNC_PTR->sourcePortIdentity.portIndex,
				SYNC_SEQUENCE_ID, sm->ppg->currentLogSyncInterval);
	if(sdata==NULL){return NULL;}
	head=(MDPTPMsgHeader*)sdata;
	head->logMessageInterval=RCVD_MDSYNC_PTR->logMessageInterval;
	head->domainNumber=
		md_domain_index2number(RCVD_MDSYNC_PTR->domainIndex);
	UB_LOG(UBL_DEBUGV, "md_sync_send:txSync domainIndex=%d, portIndex=%d\n",
	       sm->domainIndex, sm->portIndex);

	if(sm->pgap_ts==0u){sm->pgap_ts=cts64;}
	return sdata;
}

static void composeSyncFupTLV(md_sync_send_data_t *sm, uint8_t *sdata, uint64_t cf)
{
	uint32_t tsns, tslsb;
	uint16_t tsmsb;
	uint16_t sd;
	uint32_t ud;

	tsmsb=RCVD_MDSYNC_PTR->preciseOriginTimestamp.seconds.msb;
	tslsb=RCVD_MDSYNC_PTR->preciseOriginTimestamp.seconds.lsb;
	tsns=RCVD_MDSYNC_PTR->preciseOriginTimestamp.nanoseconds+cf;
	if(tsns>=(uint32_t)UB_SEC_NS){
		tsns-=(uint32_t)UB_SEC_NS;
		if(++tslsb==0){tsmsb++;}
	}
	sd=htons(tsmsb);
	memcpy(&sdata[MDPTPMSG_PRECISEORIGINTIMESTAMP], &sd, 2);
	ud=htonl(tslsb);
	memcpy(&sdata[MDPTPMSG_PRECISEORIGINTIMESTAMP+2], &ud, 4);
	ud=htonl(tsns);
	memcpy(&sdata[MDPTPMSG_PRECISEORIGINTIMESTAMP+6], &ud, 4);

	md_followup_information_tlv_compose(&sdata[MDPTPMSG_FUPINFOTLV],
					    RCVD_MDSYNC_PTR->rateRatio,
					    RCVD_MDSYNC_PTR->gmTimeBaseIndicator,
					    RCVD_MDSYNC_PTR->lastGmPhaseChange,
					    RCVD_MDSYNC_PTR->lastGmFreqChange);
}

static int setSyncTwoStep_txSync(md_sync_send_data_t *sm, uint64_t cts64)
{
	MDPTPMsgSync *sdata;
	int ssize=sizeof(MDPTPMsgSync);
	int64_t dts;
	int res;

	sdata=(MDPTPMsgSync*)composeSyncHeader(sm, cts64, ssize);
	if(sdata==NULL){return -1;}
	res=gptpnet_send_whook(sm->gpnetd, sm->portIndex-1, ssize);
	if(res==-1){return res;}
	if(res<0) {
		sm->mock_txts64=gptpclock_getts64(GPTPINSTNUM, sm->ptasg->thisClockIndex,
						  sm->ptasg->domainIndex);
		res=-res;
	}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, syncTx);
	dts=cts64-sm->tsync_ts;
	if(dts>175000000) {
		UB_TLOG(UBL_INFO, "%s:domainIndex=%d, portIndex=%d, sync gap=%dmsec\n",
			__func__, sm->domainIndex, sm->portIndex, (int)(dts/1000000));
	}
	sm->tsync_ts=cts64;

	dts=cts64-sm->pgap_ts;
	if(dts>60000000){
		UB_TLOG(UBL_INFO, "%s:domainIndex=%d, portIndex=%d, sync blocked=%dmsec\n",
			__func__, sm->domainIndex, sm->portIndex,
			(int)(dts/1000000));
	}
	sm->pgap_ts=0;
	return res;
}

static int setFollowUp_txFollowUp(md_sync_send_data_t *sm, uint64_t cts64)
{
	uint8_t *sdata;
	int ssize=sizeof(MDPTPMsgFollowUp);
	PTPMsgHeader head;
	int64_t dts;
	uint64_t cf;

	sdata=gptpnet_get_sendbuf(sm->gpnetd, sm->portIndex-1);
	(void)memset(sdata, 0, ssize);
	md_header_template(GPTPINSTNUM, sm->portIndex, &head, FOLLOW_UP, ssize,
			   &RCVD_MDSYNC_PTR->sourcePortIdentity, SYNC_SEQUENCE_ID,
			   sm->ppg->currentLogSyncInterval);
	if(gptpclock_we_are_gm(GPTPINSTNUM, sm->domainIndex)){
		head.correctionField=0;
		cf = sm->sync_ts - RCVD_MDSYNC_PTR->upstreamTxTime.nsec;
		// we assume cf<1sec
	}else{
		head.correctionField = ((uint64_t)RCVD_MDSYNC_PTR->followUpCorrectionField.nsec<<16u);
		head.correctionField += (RCVD_MDSYNC_PTR->rateRatio *
					 ((sm->sync_ts -
					   RCVD_MDSYNC_PTR->upstreamTxTime.nsec)<<16));
		cf=0;
	}

	head.domainIndex = RCVD_MDSYNC_PTR->domainIndex;

	md_compose_head(&head, sdata);

	composeSyncFupTLV(sm, sdata, cf);
	UB_LOG(UBL_DEBUGV, "md_sync_send:txFollowUp domainIndex=%d, portIndex=%d\n",
	       sm->domainIndex, sm->portIndex);

	dts=cts64-sm->tfup_ts;
	if(dts>175000000) {
		UB_TLOG(UBL_INFO, "%s:domainIndex=%d, portIndex=%d, fup gap=%dmsec\n",
			 __func__, sm->domainIndex, sm->portIndex, (int)(dts/1000000));
	}
	if(gptpnet_send_whook(sm->gpnetd, sm->portIndex-1, ssize)==-1){return -1;}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, followUpTx);
	sm->tfup_ts=cts64;
	return 0;
}

static int setSyncOneStep(md_sync_send_data_t *sm, uint64_t cts64)
{
	MDPTPMsgSyncOneStep *sdata;
	int ssize=sizeof(MDPTPMsgSyncOneStep);
	int64_t dts;
	int res;
	uint64_t cf;
	int64_t correctionField;
	uint64_t ld;

	sdata=(MDPTPMsgSyncOneStep*)composeSyncHeader(sm, cts64, ssize);
	if(sdata==NULL){return -1;}
	if(gptpclock_we_are_gm(GPTPINSTNUM, sm->domainIndex)){
		correctionField=0;
		// when we are GM, upstreamTxTime is normally our gptp TS.
		// cf should be (sync_ts - upstreamTxTime) in nsec
		cf = 0;
	}else{
		// upstreamTxTime is TxTS of upsream Sync,
		// RCVD_MDSYNC_PTR->rateRatio * (sync_ts - upstreamTxTime) must be added
		// on correctionField
		correctionField =
			((uint64_t)RCVD_MDSYNC_PTR->followUpCorrectionField.nsec<<16u);
		cf=0;
	}
	ld=UB_HTONLL((uint64_t)correctionField);
	sdata->head.correctionField_nll=ld;
	composeSyncFupTLV(sm, (uint8_t*)sdata, cf);

	res=gptpnet_send_whook(sm->gpnetd, sm->portIndex-1, ssize);
	if(res==-1){return res;}
	if(res<0) {
		sm->mock_txts64=gptpclock_getts64(GPTPINSTNUM, sm->ptasg->thisClockIndex,
						  sm->ptasg->domainIndex);
		res=-res;
	}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, syncTx);
	dts=cts64-sm->tsync_ts;
	if(dts>175000000) {
		UB_TLOG(UBL_INFO, "%s:domainIndex=%d, portIndex=%d, sync gap=%dmsec\n",
			__func__, sm->domainIndex, sm->portIndex, (int)(dts/1000000));
	}
	sm->tsync_ts=cts64;

	dts=cts64-sm->pgap_ts;
	if(dts>60000000){
		UB_TLOG(UBL_INFO, "%s:domainIndex=%d, portIndex=%d, sync blocked=%dmsec\n",
			__func__, sm->domainIndex, sm->portIndex,
			(int)(dts/1000000));
	}
	sm->pgap_ts=0;
	return res;
}

static md_sync_send_state_t allstate_condition(md_sync_send_data_t *sm)
{
	/*
	  from 'MDSyncSendSM state machine' in the standard,
	  (sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	  (RCVD_MDSYNC && (!PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE)))
	  is the right condition, but we remove 'RCVD_MDSYNC &&' part to avoid some issues
	 */
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   !PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE) {
		sm->last_state=REACTION;
		return INITIALIZING;
	}
	return (md_sync_send_state_t)sm->state;
}

static void *initializing_proc(md_sync_send_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_sync_send:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_MDSYNC = false;
	RCVD_MDTIMESTAMP_RECEIVE = false;
	SYNC_SEQUENCE_ID = rand() & 0xffff;
	return NULL;
}

static md_sync_send_state_t initializing_condition(md_sync_send_data_t *sm)
{
	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !ASYMMETRY_MEASUREMENT_MODE && !ONESTEP_TX_OPER){return SEND_SYNC_TWO_STEP;}

	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !ASYMMETRY_MEASUREMENT_MODE && ONESTEP_TX_OPER){return SEND_SYNC_ONE_STEP;}

	return INITIALIZING;
}

static int send_sync_two_step_proc(md_sync_send_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "md_sync_send:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_MDSYNC = false;
	if(setSyncTwoStep_txSync(sm, cts64)<0){return -1;}
	sm->txSync_time = cts64;
	return 0;
}

static md_sync_send_state_t send_sync_two_step_condition(md_sync_send_data_t *sm, uint64_t cts64)
{
	if(RCVD_MDTIMESTAMP_RECEIVE){return SEND_FOLLOW_UP;}
	if(sm->mock_txts64!=0u){
		sm->sync_ts=sm->mock_txts64;
		sm->mock_txts64=0;
		return SEND_FOLLOW_UP;
	}
	if((cts64 - sm->txSync_time) >=
	    gptpnet_txtslost_time(sm->gpnetd, sm->portIndex-1)){
		UB_TLOG(UBL_WARN,"%s:missing TxTS, domainIndex=%d, portIndex=%d, seqID=%d\n",
			 __func__, sm->domainIndex, sm->portIndex, SYNC_SEQUENCE_ID);
		// send SYNC with the same SequenceID again
		sm->last_state=REACTION;
		return SEND_SYNC_TWO_STEP;
	}
	return SEND_SYNC_TWO_STEP;
}

static int send_follow_up_proc(md_sync_send_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "md_sync_send:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_MDTIMESTAMP_RECEIVE = false;
	if(setFollowUp_txFollowUp(sm, cts64)!=0){return -1;}
	SYNC_SEQUENCE_ID += 1u;
	return 0;
}

static md_sync_send_state_t send_follow_up_condition(md_sync_send_data_t *sm)
{
	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !ASYMMETRY_MEASUREMENT_MODE && !ONESTEP_TX_OPER ){return SEND_SYNC_TWO_STEP;}

	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !ASYMMETRY_MEASUREMENT_MODE && ONESTEP_TX_OPER ){return SEND_SYNC_ONE_STEP;}

	return SEND_FOLLOW_UP;
}

static void *send_sync_one_step_proc(md_sync_send_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "md_sync_send:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_MDSYNC = false;
	if(setSyncOneStep(sm, cts64)){return NULL;}
	SYNC_SEQUENCE_ID += 1u;
	return NULL;
}

static md_sync_send_state_t send_sync_one_step_condition(md_sync_send_data_t *sm)
{
	if(RCVD_MDTIMESTAMP_RECEIVE){return SET_CORRECTION_FIELD;}
	return SEND_SYNC_ONE_STEP;
}

static void *set_correction_field_proc(md_sync_send_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_sync_send:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_MDTIMESTAMP_RECEIVE = false;
	//modifySync();
	return NULL;
}

static md_sync_send_state_t set_correction_field_condition(md_sync_send_data_t *sm)
{
	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !ASYMMETRY_MEASUREMENT_MODE && !ONESTEP_TX_OPER ){return SEND_SYNC_TWO_STEP;}

	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !ASYMMETRY_MEASUREMENT_MODE && !ONESTEP_TX_OPER ){return SEND_SYNC_ONE_STEP;}

	return SET_CORRECTION_FIELD;
}

// return 1 when SYNC(two step) is sent, oterwise return 0
int md_sync_send_sm(md_sync_send_data_t *sm, uint64_t cts64)
{
	bool state_change;

	if(!sm){return 0;}
	sm->state = allstate_condition(sm);

	while(true){
		state_change=(sm->last_state != sm->state);
		sm->last_state = sm->state;
		switch(sm->state){
		case INIT:
			sm->state = INITIALIZING;
			break;
		case INITIALIZING:
			if(state_change){(void)initializing_proc(sm);}
			sm->state = initializing_condition(sm);
			break;
		case SEND_SYNC_TWO_STEP:
			if(state_change){
				if(send_sync_two_step_proc(sm, cts64)<0){
					sm->last_state = REACTION;
					return 0;
				}
				return 1;
			}
			sm->state = send_sync_two_step_condition(sm, cts64);
			break;
		case SEND_FOLLOW_UP:
			if(state_change){
				if(send_follow_up_proc(sm, cts64)<0){
					sm->last_state = REACTION;
					return 0;
				}
				break;
			}
			sm->state = send_follow_up_condition(sm);
			break;
		case SEND_SYNC_ONE_STEP:
			if(state_change){(void)send_sync_one_step_proc(sm, cts64);}
			sm->state = send_sync_one_step_condition(sm);
			break;
		case SET_CORRECTION_FIELD:
			if(state_change){(void)set_correction_field_proc(sm);}
			sm->state = set_correction_field_condition(sm);
			break;
		case REACTION:
		default:
			break;
		}
		if(sm->last_state == sm->state){break;}
	}
	return 0;
}

void md_sync_send_sm_init(md_sync_send_data_t **sm,
			  int domainIndex, int portIndex,
			  gptpnet_data_t *gpnetd,
			  PerTimeAwareSystemGlobal *ptasg,
			  PerPortGlobal *ppg,
			  MDEntityGlobal *mdeg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	       __func__, domainIndex, portIndex);
	INIT_SM_DATA(md_sync_send_data_t, MDSyncSendSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->gpnetd = gpnetd;
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->mdeg = mdeg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
	(void)md_sync_send_sm((*sm),0);
}

int md_sync_send_sm_close(md_sync_send_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	       __func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

int md_sync_send_sm_mdSyncSend(md_sync_send_data_t *sm,
			       MDSyncSend *mdSyncSend, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_MDSYNC = true;
	RCVD_MDSYNC_PTR = mdSyncSend;
	return md_sync_send_sm(sm, cts64);
}

void md_sync_send_sm_txts(md_sync_send_data_t *sm, event_data_txts_t *edtxts,
			  uint64_t cts64)
{
	int pi;
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d, seqID=%d\n",
	       __func__, sm->domainIndex, sm->portIndex, edtxts->seqid);
	if(md_abnormal_timestamp(SYNC, sm->portIndex-1, sm->ptasg->domainIndex)!=0){return;}
	RCVD_MDTIMESTAMP_RECEIVE = true;
	pi=gptpgcfg_get_intitem(
		GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE,
		YDBI_CONFIG)?1:sm->portIndex;
	(void)gptpclock_tsconv(GPTPINSTNUM, &edtxts->ts64, pi, 0,
			 sm->ptasg->thisClockIndex, sm->ptasg->domainIndex);
	sm->sync_ts=edtxts->ts64;
	(void)md_sync_send_sm(sm, cts64);
}
