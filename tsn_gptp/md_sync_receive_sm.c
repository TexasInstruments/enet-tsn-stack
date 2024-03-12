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
#include "md_sync_receive_sm.h"
#include <math.h>
#include "gptpcommon.h"
#include "gptp_perfmon.h"

typedef enum {
	INIT,
	DISCARD,
	WAITING_FOR_FOLLOW_UP,
	WAITING_FOR_SYNC,
	REACTION,
}md_sync_receive_state_t;

#define RCVD_SYNC sm->thisSM->rcvdSync
#define RCVD_SYNC_PTR sm->thisSM->u.rcvdSyncPtr
#define RCVD_SYNC_ONESETP_PTR sm->thisSM->u.rcvdSyncOneStepPtr
#define RCVD_FOLLOWUP sm->thisSM->rcvdFollowUp
#define RCVD_FOLLOWUP_PTR sm->thisSM->rcvdFollowUpPtr
#define PORT_OPER sm->ppg->forAllDomain->portOper
#define ASYMMETRY_MEASUREMENT_MODE sm->ppg->forAllDomain->asymmetryMeasurementMode
#define ONE_STEP_RECEIVE sm->mdeg->oneStepReceive
#define AS_CAPABLE sm->ppg->asCapable
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define TWO_STEP_FLAG GET_TWO_STEP_FLAG(RCVD_SYNC_PTR->head)

static MDSyncReceive *setMDSyncReceive(md_sync_receive_data_t *sm)
{
	int64_t offset=0;
	sm->mdSyncReceive.domainIndex =
		md_domain_number2index(RCVD_SYNC_PTR->head.domainNumber);
	sm->mdSyncReceive.seqid = ntohs(RCVD_SYNC_PTR->head.sequenceId_ns);
	sm->mdSyncReceive.followUpCorrectionField.nsec =
		(UB_NTOHLL((uint64_t)RCVD_SYNC_PTR->head.correctionField_nll)>>16);

	if(TWO_STEP_FLAG!=0u){
		sm->mdSyncReceive.followUpCorrectionField.nsec +=
			(UB_NTOHLL((uint64_t)RCVD_FOLLOWUP_PTR->head.correctionField_nll)>>16);
		sm->mdSyncReceive.preciseOriginTimestamp.seconds.lsb =
			(uint64_t)ntohl(RCVD_FOLLOWUP_PTR->preciseOriginTimestamp.seconds_lsb_nl);
		sm->mdSyncReceive.preciseOriginTimestamp.seconds.msb =
			(uint64_t)ntohs(RCVD_FOLLOWUP_PTR->preciseOriginTimestamp.seconds_msb_ns);
		sm->mdSyncReceive.preciseOriginTimestamp.nanoseconds =
			(uint64_t)ntohl(RCVD_FOLLOWUP_PTR->preciseOriginTimestamp.nanoseconds_nl);

		sm->mdSyncReceive.rateRatio =
			1.0 + ldexp((int32_t)ntohl(RCVD_FOLLOWUP_PTR->
						  FUpInfoTLV.cumulativeScaledRateOffset_nl),
				    -41);
		sm->mdSyncReceive.gmTimeBaseIndicator =
			ntohs(RCVD_FOLLOWUP_PTR->FUpInfoTLV.gmTimeBaseIndicator_ns);

		/* 10.2.2.1.10 lastGmPhaseChange (ScaledNs
		 * The lastGmPhaseChange is the value of the lastGmPhaseChange member
		 * of the most recently received PortSyncSync structure...
		 * It is set equal to the lastGmPhaseChange of the received
		 * time-synchronization information.
		 * Both have MDScaledNs structure, copy the value as is. */
		sm->mdSyncReceive.lastGmPhaseChange.nsec_msb =
			ntohs(RCVD_FOLLOWUP_PTR->FUpInfoTLV.lastGmPhaseChange.nsec_msb);
		sm->mdSyncReceive.lastGmPhaseChange.nsec =
			(int64_t)UB_NTOHLL(
				(uint64_t)RCVD_FOLLOWUP_PTR->FUpInfoTLV.lastGmPhaseChange.nsec_nll);
		sm->mdSyncReceive.lastGmPhaseChange.subns =
			ntohs(RCVD_FOLLOWUP_PTR->FUpInfoTLV.lastGmPhaseChange.subns_ns);

		sm->mdSyncReceive.lastGmFreqChange =
			ldexp((int32_t)ntohl(
				      RCVD_FOLLOWUP_PTR->FUpInfoTLV.scaledLastGmFreqChange_nl),
			      -41);
	}else{
		sm->mdSyncReceive.preciseOriginTimestamp.seconds.lsb =
			(uint64_t)ntohl(RCVD_SYNC_ONESETP_PTR->originTimestamp.seconds_lsb_nl);
		sm->mdSyncReceive.preciseOriginTimestamp.seconds.msb =
			(uint64_t)ntohs(RCVD_SYNC_ONESETP_PTR->originTimestamp.seconds_msb_ns);
		sm->mdSyncReceive.preciseOriginTimestamp.nanoseconds =
			(uint64_t)ntohl(RCVD_SYNC_ONESETP_PTR->originTimestamp.nanoseconds_nl);

		sm->mdSyncReceive.rateRatio =
			1.0 + ldexp((int32_t)ntohl(RCVD_SYNC_ONESETP_PTR->
						  FUpInfoTLV.cumulativeScaledRateOffset_nl),
				    -41);
		sm->mdSyncReceive.gmTimeBaseIndicator =
			ntohs(RCVD_SYNC_ONESETP_PTR->FUpInfoTLV.gmTimeBaseIndicator_ns);
		sm->mdSyncReceive.lastGmPhaseChange.nsec_msb =
			ntohs(RCVD_FOLLOWUP_PTR->FUpInfoTLV.lastGmPhaseChange.nsec_msb);
		sm->mdSyncReceive.lastGmPhaseChange.nsec =
			(int64_t)UB_NTOHLL(
				(uint64_t)RCVD_FOLLOWUP_PTR->FUpInfoTLV.lastGmPhaseChange.nsec_nll);
		sm->mdSyncReceive.lastGmPhaseChange.subns =
			ntohs(RCVD_FOLLOWUP_PTR->FUpInfoTLV.lastGmPhaseChange.subns_ns);
		sm->mdSyncReceive.lastGmFreqChange =
			ldexp((int32_t)
			      ntohl(RCVD_SYNC_ONESETP_PTR->FUpInfoTLV.scaledLastGmFreqChange_nl),
			      -41);
	}

	memcpy(sm->mdSyncReceive.sourcePortIdentity.clockIdentity,
	       RCVD_SYNC_PTR->head.sourcePortIdentity.clockIdentity,
	       sizeof(ClockIdentity));
	sm->mdSyncReceive.sourcePortIdentity.portIndex =
		md_port_number2index(ntohs(RCVD_SYNC_PTR->head.sourcePortIdentity.portNumber_ns));
	sm->mdSyncReceive.logMessageInterval = RCVD_SYNC_PTR->head.logMessageInterval;

	sm->mdSyncReceive.upstreamTxTime.nsec = sm->syncEventIngressTimestamp -
		(uint64_t)((double)sm->ppg->forAllDomain->neighborPropDelay.nsec /
		 sm->ppg->forAllDomain->neighborRateRatio) -
		(uint64_t)((double)sm->ppg->forAllDomain->delayAsymmetry.nsec /
		 sm->mdSyncReceive.rateRatio);

	/* IEEE1588-2019 MasterSlaveDelay calculation (J.2 Timestamping monitoring, p. 409)
	 *  MasterSlaveDelay = syncEventIngressTimestamp - preciseOriginTimestamp - correctionField
	*/
	offset = sm->syncEventIngressTimestamp -
	       ((sm->mdSyncReceive.preciseOriginTimestamp.seconds.lsb*(uint64_t)UB_SEC_NS)+
	       sm->mdSyncReceive.preciseOriginTimestamp.nanoseconds) -
	       sm->mdSyncReceive.followUpCorrectionField.nsec;
	PERFMON_CPMDR_ADD(sm->ptasg->perfmonClockDS, CPMDR_masterSlaveDelay, offset);

	/* IEEE1588-2019 offsetFromMaster calculation (11.5.3.3)
	 *  offsetFromMaster = syncEventIngressTimestamp - originTimestamp -
	 *                     correctionField - ratio(syncEventIngressTimestamp-
	 *                     upstreamTxTime)
	 * Where variables used are members of MDSyncReceive structure.
	*/
	offset = offset - (sm->mdSyncReceive.rateRatio*
	       (sm->syncEventIngressTimestamp - sm->mdSyncReceive.upstreamTxTime.nsec));
	PERFMON_CPMDR_ADD(sm->ptasg->perfmonClockDS, CPMDR_offsetFromMaster, offset);

	return &sm->mdSyncReceive;
}

static md_sync_receive_state_t allstate_condition(md_sync_receive_data_t *sm)
{
	/*
	  from 'MDSyncReceiveSM state machine' in the standard,
	  (sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	  (RCVD_SYNC && (!PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE)))
	  is the right condition, but we remove 'RCVD_SYNC &&' part to avoid some issues
	 */
	if (sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	    !PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE){
		sm->last_state=REACTION;
		return DISCARD;
	}
	return (md_sync_receive_state_t)sm->state;
}

static void *discard_proc(md_sync_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "md_sync_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_SYNC = false;
	RCVD_FOLLOWUP = false;
	sm->rsync_ts = 0;
	sm->rfup_ts = 0;
	return NULL;
}

static md_sync_receive_state_t discard_condition(md_sync_receive_data_t *sm)
{
	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !TWO_STEP_FLAG && !ASYMMETRY_MEASUREMENT_MODE && !ONE_STEP_RECEIVE) {
		sm->last_state=REACTION;
		return DISCARD;
	}

	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   TWO_STEP_FLAG && !ASYMMETRY_MEASUREMENT_MODE){return WAITING_FOR_FOLLOW_UP;}

	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !TWO_STEP_FLAG && !ASYMMETRY_MEASUREMENT_MODE && ONE_STEP_RECEIVE){
		return WAITING_FOR_SYNC;
	}

	return DISCARD;
}

static void *waiting_for_follow_up_proc(md_sync_receive_data_t *sm, uint64_t cts64)
{
	int64_t dts;
	UB_LOG(UBL_DEBUGV, "md_sync_receive:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	if(RCVD_SYNC) {
		dts=cts64-sm->rsync_ts;
		if(sm->rsync_ts && (dts>175000000)) {
			UB_TLOG(UBL_INFO, "%s:domainIndex=%d, portIndex=%d, sync gap=%"PRIi64"\n",
				 __func__, sm->domainIndex, sm->portIndex, dts);
		}
		sm->rsync_ts=cts64;
	}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, syncRx);
	RCVD_SYNC = false;
	sm->thisSM->upstreamSyncInterval.nsec =
		LOG_TO_NSEC(RCVD_SYNC_PTR->head.logMessageInterval);
	sm->thisSM->followUpReceiptTimeoutTime.nsec =
		cts64 + sm->thisSM->upstreamSyncInterval.nsec;
	return NULL;
}

static md_sync_receive_state_t waiting_for_follow_up_condition(md_sync_receive_data_t *sm,
							       uint64_t cts64)
{
	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   TWO_STEP_FLAG){
		UB_TLOG(UBL_WARN, "%s:domainIndex=%d, portIndex=%d, missed one FUP\n",
			 __func__, sm->domainIndex, sm->portIndex);
		sm->last_state=REACTION;
		return WAITING_FOR_FOLLOW_UP;
	}

	if(RCVD_FOLLOWUP){
		if(RCVD_FOLLOWUP_PTR->head.sequenceId_ns ==
		   RCVD_SYNC_PTR->head.sequenceId_ns){return WAITING_FOR_SYNC;}
		UB_TLOG(UBL_WARN, "%s:domainIndex=%d, portIndex=%d, Sync SqID=%d, FUP SqID=%d\n",
			 __func__, sm->domainIndex, sm->portIndex,
			ntohs(RCVD_SYNC_PTR->head.sequenceId_ns),
			ntohs(RCVD_FOLLOWUP_PTR->head.sequenceId_ns));
		RCVD_FOLLOWUP=false;
	}

	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !TWO_STEP_FLAG && ONE_STEP_RECEIVE){return WAITING_FOR_SYNC;}

	if(((cts64 >= sm->thisSM->followUpReceiptTimeoutTime.nsec) && !ASYMMETRY_MEASUREMENT_MODE) ||
	   (RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	    !TWO_STEP_FLAG && !ASYMMETRY_MEASUREMENT_MODE && !ONE_STEP_RECEIVE )) {
		UB_TLOG(UBL_WARN, "%s:domainIndex=%d, portIndex=%d, timed out for FUP\n",
			 __func__, sm->domainIndex, sm->portIndex);
		return DISCARD;
	}

	return WAITING_FOR_FOLLOW_UP;
}

static void *waiting_for_sync_proc(md_sync_receive_data_t *sm, uint64_t cts64)
{
	int64_t dts;
	UB_LOG(UBL_DEBUGV, "md_sync_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_SYNC = false;
	if(RCVD_FOLLOWUP) {
		dts=cts64-sm->rfup_ts;
		if(sm->rfup_ts && (dts>175000000)) {
			UB_TLOG(UBL_INFO, "%s:domainIndex=%d, portIndex=%d, fup gap=%"PRIi64"\n",
				 __func__, sm->domainIndex, sm->portIndex, dts);
		}
		sm->rfup_ts=cts64;
	}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, followUpRx);
	RCVD_FOLLOWUP = false;
	sm->thisSM->txMDSyncReceivePtr = setMDSyncReceive(sm);
	//UB_LOG(UBL_DEBUG, "%s:txMDSyncReceive\n", __func__);
	return sm->thisSM->txMDSyncReceivePtr; // txMDSyncReceive();
}

static md_sync_receive_state_t waiting_for_sync_condition(md_sync_receive_data_t *sm)
{
	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !TWO_STEP_FLAG && !ASYMMETRY_MEASUREMENT_MODE && ONE_STEP_RECEIVE){
		sm->last_state=REACTION;
		return WAITING_FOR_SYNC;
	}

	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   TWO_STEP_FLAG && !ASYMMETRY_MEASUREMENT_MODE){
		return WAITING_FOR_FOLLOW_UP;
	}

	if(RCVD_SYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !TWO_STEP_FLAG && !ASYMMETRY_MEASUREMENT_MODE && !ONE_STEP_RECEIVE){return DISCARD;}

	if(RCVD_FOLLOWUP){
		UB_TLOG(UBL_DEBUGV, "%s:waiting Sync but received SyncFup, "
			"received.seqId=%d\n", __func__,
			ntohs(RCVD_FOLLOWUP_PTR->head.sequenceId_ns));
			/* Although Sync must be sent first before sending SyncFup,
			 * there is a posibility that SyncFup is received earlier than the
			 * corresponding Sync on the receiving end. This includes the following:
			 * - Driver limitation where where processing of RxTS takes long time,
			 *   causing non event event frame SyncFup to be served first than the
			 *   event requiring RxTS like Sync
			 *   (e.g. SJA1105 DSA driver)
			 *
			 * Thus, receiving SyncFup when expecting Sync should not throw away
			 * the SyncFup to consider the above possibility.
			 * We will allow out-of-order SyncFup and Sync at this point by disabling
			 * the following code:
			 *   RCVD_FOLLOWUP=false;
			 *
			 * Checking of matching Sync and SyncFup is performed afterwards.
			 */
	}
	return WAITING_FOR_SYNC;
}


void *md_sync_receive_sm(md_sync_receive_data_t *sm, uint64_t cts64)
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
			sm->state = DISCARD;
			break;
		case DISCARD:
			if(state_change){retp=discard_proc(sm);}
			sm->state = discard_condition(sm);
			break;
		case WAITING_FOR_FOLLOW_UP:
			if(state_change){retp=waiting_for_follow_up_proc(sm, cts64);}
			sm->state = waiting_for_follow_up_condition(sm, cts64);
			break;
		case WAITING_FOR_SYNC:
			if(state_change){retp=waiting_for_sync_proc(sm, cts64);}
			sm->state = waiting_for_sync_condition(sm);
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

void md_sync_receive_sm_init(md_sync_receive_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	MDEntityGlobal *mdeg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(md_sync_receive_data_t, MDSyncReceiveSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->mdeg = mdeg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int md_sync_receive_sm_close(md_sync_receive_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *md_sync_receive_sm_recv_sync(md_sync_receive_data_t *sm, event_data_recv_t *edrecv,
				  uint64_t cts64)
{
	int size;
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_SYNC=true;
	size=GET_TWO_STEP_BYTE_FLAG(edrecv->recbptr)?
		sizeof(MDPTPMsgSync):sizeof(MDPTPMsgSyncOneStep);
	if(ub_assert_fatal(size <= (int)(sizeof(sm->u.recSync)/sizeof(uint8_t)), __func__, NULL)){return NULL;}
	memcpy(&sm->u.recSync, edrecv->recbptr, size);
	RCVD_SYNC_PTR = &sm->u.recSync;
	sm->syncEventIngressTimestamp = edrecv->ts64;
	return md_sync_receive_sm(sm, cts64);
}

void *md_sync_receive_sm_recv_fup(md_sync_receive_data_t *sm, event_data_recv_t *edrecv,
				  uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_FOLLOWUP=true;
	memcpy(&sm->recFollowUp, edrecv->recbptr, sizeof(MDPTPMsgFollowUp));
	RCVD_FOLLOWUP_PTR = &sm->recFollowUp;
	return md_sync_receive_sm(sm, cts64);
}
