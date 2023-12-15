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
#include "port_sync_sync_send_sm.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	TRANSMIT_INIT,
	SEND_MD_SYNC,
	SYNC_RECEIPT_TIMEOUT,
	REACTION,
}port_sync_sync_send_state_t;

#define RCVD_PSSYNC sm->thisSM->rcvdPSSync
#define RCVD_PSSYNC_PTR sm->thisSM->rcvdPSSyncPtr
#define PORT_OPER sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled
#define AS_CAPABLE sm->ppg->asCapable
#define SYNC_SLOWDOWN sm->ppg->syncSlowdown
#define NUMBER_SYNC_TRANSMISSIONS sm->thisSM->numberSyncTransmissions
#define THIS_PORT sm->ppg->thisPort
#define SELECTED_STATE sm->ptasg->selectedState
#define INTERVAL1 sm->thisSM->interval1
#define SYNC_LOCKED sm->ppg->syncLocked
// oldSyncInterval is not defined in 802.1AS-rev-d5
//#define OLD_SYNC_INTERVAL oldSyncInterval
#define OLD_SYNC_INTERVAL sm->ppg->syncInterval;
#define LAST_SYNC_SENT_TIME sm->thisSM->lastSyncSentTime
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static void setMDSync(port_sync_sync_send_data_t *sm)
{
	sm->mdSyncSend.domainIndex = sm->ptasg->domainIndex;
	sm->mdSyncSend.followUpCorrectionField = sm->thisSM->lastFollowUpCorrectionField;
	sm->mdSyncSend.sourcePortIdentity.portIndex = THIS_PORT ;
	memcpy(sm->mdSyncSend.sourcePortIdentity.clockIdentity, sm->ptasg->thisClock,
	       sizeof(ClockIdentity));
	sm->mdSyncSend.logMessageInterval = sm->ppg->currentLogSyncInterval;
	sm->mdSyncSend.preciseOriginTimestamp = sm->thisSM->lastPreciseOriginTimestamp;
	sm->mdSyncSend.upstreamTxTime = sm->thisSM->lastUpstreamTxTime;
	sm->mdSyncSend.rateRatio = sm->thisSM->lastRateRatio;
	sm->mdSyncSend.gmTimeBaseIndicator = sm->thisSM->lastGmTimeBaseIndicator;
	sm->mdSyncSend.lastGmPhaseChange = sm->thisSM->lastGmPhaseChange;
	sm->mdSyncSend.lastGmFreqChange = sm->thisSM->lastGmFreqChange;
}

static port_sync_sync_send_state_t allstate_condition(port_sync_sync_send_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   (RCVD_PSSYNC &&  (!PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE))){
		sm->last_state=REACTION;
		return TRANSMIT_INIT;
	}
	return (port_sync_sync_send_state_t)sm->state;
}

static void *transmit_init_proc(port_sync_sync_send_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_sync_sync_send:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_PSSYNC = false;
	SYNC_SLOWDOWN = false;
	NUMBER_SYNC_TRANSMISSIONS = 0;
	return NULL;
}

static port_sync_sync_send_state_t transmit_init_condition(port_sync_sync_send_data_t *sm)
{
	if(RCVD_PSSYNC &&
	   ((uint16_t)RCVD_PSSYNC_PTR->localPortIndex != THIS_PORT) &&
	   PORT_OPER && PTP_PORT_ENABLED &&
	   AS_CAPABLE &&
	   ((SELECTED_STATE[sm->portIndex] == (uint8_t)MasterPort) ||
	    (gptpgcfg_get_intitem(
		    GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_TEST_SYNC_SEND_PORT,
		    YDBI_CONFIG) == sm->portIndex))){
		return SEND_MD_SYNC;
	}

	return TRANSMIT_INIT;
}

static void *send_md_sync_proc(port_sync_sync_send_data_t *sm, uint64_t cts64)
{
	void *retv = NULL;
	UB_LOG(UBL_DEBUGV, "port_sync_sync_send:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	if(RCVD_PSSYNC) {
		sm->thisSM->lastRcvdPortNum = RCVD_PSSYNC_PTR->localPortIndex;
		sm->thisSM->lastPreciseOriginTimestamp = RCVD_PSSYNC_PTR->preciseOriginTimestamp;
		sm->thisSM->lastFollowUpCorrectionField = RCVD_PSSYNC_PTR->followUpCorrectionField;
		sm->thisSM->lastRateRatio = RCVD_PSSYNC_PTR->rateRatio;
		sm->thisSM->lastUpstreamTxTime = RCVD_PSSYNC_PTR->upstreamTxTime;
		sm->thisSM->lastGmTimeBaseIndicator = RCVD_PSSYNC_PTR->gmTimeBaseIndicator;
		sm->thisSM->lastGmPhaseChange = RCVD_PSSYNC_PTR->lastGmPhaseChange;
		sm->thisSM->lastGmFreqChange = RCVD_PSSYNC_PTR->lastGmFreqChange;
		sm->thisSM->syncReceiptTimeoutTime = RCVD_PSSYNC_PTR->syncReceiptTimeoutTime;
		SYNC_LOCKED = (sm->ptasg->parentLogSyncInterval == sm->ppg->currentLogSyncInterval);
	}
	RCVD_PSSYNC = false;
	LAST_SYNC_SENT_TIME.nsec = cts64;
	setMDSync(sm);
	//UB_LOG(UBL_DEBUG, "%s:txMDSync\n", __func__);
	retv = &sm->mdSyncSend;
	if (SYNC_SLOWDOWN) {
		if (NUMBER_SYNC_TRANSMISSIONS >= (uint8_t)sm->ppg->syncReceiptTimeout) {
			INTERVAL1 = sm->ppg->syncInterval;
			NUMBER_SYNC_TRANSMISSIONS = 0;
			SYNC_SLOWDOWN = false;
		} else {
			INTERVAL1 = OLD_SYNC_INTERVAL;
			NUMBER_SYNC_TRANSMISSIONS++;
		}
	} else {
		NUMBER_SYNC_TRANSMISSIONS = 0;
		INTERVAL1 = sm->ppg->syncInterval;
	}
	return retv;
}

static port_sync_sync_send_state_t send_md_sync_condition(port_sync_sync_send_data_t *sm,
							  uint64_t cts64)
{
	if((cts64 >= sm->thisSM->syncReceiptTimeoutTime.nsec) &&
	   !SYNC_LOCKED){return SYNC_RECEIPT_TIMEOUT;}

	if( ( (RCVD_PSSYNC && SYNC_LOCKED &&
	       ((uint16_t)RCVD_PSSYNC_PTR->localPortIndex != THIS_PORT)) ||
	      (!SYNC_LOCKED && ((cts64 - LAST_SYNC_SENT_TIME.nsec) >= INTERVAL1.nsec) &&
	       (sm->thisSM->lastRcvdPortNum != THIS_PORT )) ) &&
	    PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE &&
	    ((SELECTED_STATE[sm->portIndex] == (uint8_t)MasterPort) ||
	     (gptpgcfg_get_intitem(GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_TEST_SYNC_SEND_PORT,
				   YDBI_CONFIG) == sm->portIndex))){
		sm->last_state = REACTION;
	}

	return SEND_MD_SYNC;
}

static void *sync_receipt_timeout_proc(port_sync_sync_send_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_sync_sync_send:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_PSSYNC = false;
	return NULL;
}

static port_sync_sync_send_state_t sync_receipt_timeout_condition(port_sync_sync_send_data_t *sm)
{
	if(RCVD_PSSYNC &&
	   ((uint16_t)RCVD_PSSYNC_PTR->localPortIndex != THIS_PORT) &&
	   PORT_OPER && PTP_PORT_ENABLED &&
	   AS_CAPABLE &&
	   (SELECTED_STATE[sm->portIndex] == (uint8_t)MasterPort ||
	    gptpgcfg_get_intitem(
		    GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_TEST_SYNC_SEND_PORT,
		    YDBI_CONFIG) == sm->portIndex)){
		return SEND_MD_SYNC;
	}
	return SYNC_RECEIPT_TIMEOUT;
}


void *port_sync_sync_send_sm(port_sync_sync_send_data_t *sm, uint64_t cts64)
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
			sm->state = TRANSMIT_INIT;
			break;
		case TRANSMIT_INIT:
			if(state_change){retp=transmit_init_proc(sm);}
			sm->state = transmit_init_condition(sm);
			break;
		case SEND_MD_SYNC:
			if(state_change){
				retp=send_md_sync_proc(sm, cts64);
				break;
			}
			sm->state = send_md_sync_condition(sm, cts64);
			break;
		case SYNC_RECEIPT_TIMEOUT:
			if(state_change){retp=sync_receipt_timeout_proc(sm);}
			sm->state = sync_receipt_timeout_condition(sm);
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

void port_sync_sync_send_sm_init(port_sync_sync_send_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(port_sync_sync_send_data_t, PortSyncSyncSendSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int port_sync_sync_send_sm_close(port_sync_sync_send_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *port_sync_sync_send_sm_portSyncSync(port_sync_sync_send_data_t *sm,
					  PortSyncSync *portSyncSync, uint64_t cts64)
{
	RCVD_PSSYNC = true;
	RCVD_PSSYNC_PTR = portSyncSync;
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d, localPortIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex, RCVD_PSSYNC_PTR->localPortIndex);
	return port_sync_sync_send_sm(sm, cts64);
}
