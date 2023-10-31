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
#include "port_sync_sync_receive_sm.h"
#include "gptpcommon.h"

#define RCVD_MDSYNC sm->thisSM->rcvdMDSync
#define RCVD_MDSYNC_PTR sm->thisSM->rcvdMDSyncPtr
#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define AS_CAPABLE sm->ppg->asCapable
#define ASYMMETRY_MEASUREMENT_MODE sm->ppg->forAllDomain->asymmetryMeasurementMode

typedef enum {
	INIT,
	DISCARD,
	RECEIVED_SYNC,
	REACTION,
}port_sync_sync_receive_state_t;

static void setPSSyncPSSR(port_sync_sync_receive_data_t *sm, double rateRatio, uint64_t cts64)
{
	uint64_t interval;
	sm->portSyncSync.localPortIndex = sm->ppg->thisPort;
	sm->portSyncSync.local_ppg = sm->ppg;
	sm->portSyncSync.domainIndex = RCVD_MDSYNC_PTR->domainIndex;
	sm->portSyncSync.followUpCorrectionField = RCVD_MDSYNC_PTR->followUpCorrectionField;
	sm->portSyncSync.sourcePortIdentity = RCVD_MDSYNC_PTR->sourcePortIdentity;
	sm->portSyncSync.logMessageInterval = RCVD_MDSYNC_PTR->logMessageInterval;
	sm->portSyncSync.preciseOriginTimestamp = RCVD_MDSYNC_PTR->preciseOriginTimestamp;
	sm->portSyncSync.upstreamTxTime = RCVD_MDSYNC_PTR->upstreamTxTime;
	sm->portSyncSync.syncReceiptTimeoutTime.nsec =
		cts64 + sm->ppg->syncReceiptTimeoutTimeInterval.nsec;
	sm->portSyncSync.rateRatio = rateRatio;
	sm->portSyncSync.gmTimeBaseIndicator = RCVD_MDSYNC_PTR->gmTimeBaseIndicator;
	sm->portSyncSync.lastGmPhaseChange = RCVD_MDSYNC_PTR->lastGmPhaseChange;
	sm->portSyncSync.lastGmFreqChange = RCVD_MDSYNC_PTR->lastGmFreqChange;
	sm->portSyncSync.lastSyncSeqID = RCVD_MDSYNC_PTR->seqid;

	/* calculate syncNextSendTimeoutTime
	 * upstreamTxTime may be used as an alternative, but since allows certain
	 * level of tolerence due to nature of the network medium and software
	 * processing, use relative to current time for simplicity.
	 *
	 * There is no need to align this timeoutTime to a specific timeslot since
	 * this should be the time when we received the Sync message fro the master.
	 *
	 * AVNU standards allow a variance of interval +50%/-10% (e.g. for 125msec
	 * allowed timer values are 112.5msec to 187.5msec).
	 */
	interval = LOG_TO_NSEC(sm->portSyncSync.logMessageInterval);
	sm->portSyncSync.syncNextSendTimeoutTime.nsec = cts64 + interval + (interval/2u);
}

static void *received_sync(port_sync_sync_receive_data_t *sm, uint64_t cts64)
{
	double rateRatio;
	int a;
	RCVD_MDSYNC = false;
	rateRatio = RCVD_MDSYNC_PTR->rateRatio;
	rateRatio += (sm->ppg->forAllDomain->neighborRateRatio - 1.0);
	a = RCVD_MDSYNC_PTR->logMessageInterval;
	sm->ppg->syncReceiptTimeoutTimeInterval.nsec =
		(uint8_t)sm->ppg->syncReceiptTimeout * LOG_TO_NSEC(a);

	setPSSyncPSSR(sm, rateRatio, cts64);
	sm->ptasg->parentLogSyncInterval = RCVD_MDSYNC_PTR->logMessageInterval;
	return &sm->portSyncSync;
}

static port_sync_sync_receive_state_t allstate_condition(port_sync_sync_receive_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   (RCVD_MDSYNC && (!PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE))){
		sm->last_state=REACTION;
		return DISCARD;
	}
	return (port_sync_sync_receive_state_t)sm->state;
}

static void *discard_proc(port_sync_sync_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_sync_sync_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_MDSYNC = false;
	return NULL;
}

static port_sync_sync_receive_state_t discard_condition(port_sync_sync_receive_data_t *sm)
{
	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE){return RECEIVED_SYNC;}
	return DISCARD;
}

static void *received_sync_proc(port_sync_sync_receive_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "port_sync_sync_receive:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	//UB_LOG(UBL_DEBUG,"%s:txPSSyncPSSR\n", __func__);
	return received_sync(sm, cts64);
}

static port_sync_sync_receive_state_t received_sync_condition(port_sync_sync_receive_data_t *sm)
{
	if(RCVD_MDSYNC && PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	   !ASYMMETRY_MEASUREMENT_MODE){return REACTION;}
	return RECEIVED_SYNC;
}


void *port_sync_sync_receive_sm(port_sync_sync_receive_data_t *sm, uint64_t cts64)
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
			if(state_change){
				retp=discard_proc(sm);
				sm->state = RECEIVED_SYNC;
			}else{
				sm->state = discard_condition(sm);
			}
			break;
		case RECEIVED_SYNC:
			if(state_change){retp=received_sync_proc(sm, cts64);}
			sm->state = received_sync_condition(sm);
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

void port_sync_sync_receive_sm_init(port_sync_sync_receive_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(port_sync_sync_receive_data_t, PortSyncSyncReceiveSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int port_sync_sync_receive_sm_close(port_sync_sync_receive_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *port_sync_sync_receive_sm_recvMDSync(port_sync_sync_receive_data_t *sm,
					   MDSyncReceive *txMDSyncReceivePtr, uint64_t cts64)
{
	RCVD_MDSYNC = true;
	RCVD_MDSYNC_PTR = txMDSyncReceivePtr;
	sm->last_state = REACTION;
	return port_sync_sync_receive_sm(sm, cts64);
}
