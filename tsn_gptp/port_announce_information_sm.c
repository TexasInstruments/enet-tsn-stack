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
#include "port_announce_information_sm.h"
#include "gptpcommon.h"
#include "gptp_perfmon.h"

#define SELECTED_STATE	    sm->ptasg->selectedState
#define RESELECT	    sm->bptasg->reselect
#define SELECTED	    sm->bptasg->selected
#define RCVD_ANNOUNCE_PTR   sm->bppg->rcvdAnnouncePtr
#define INFO_IS		    sm->bppg->infoIs
#define RCVD_MSG	    sm->bppg->rcvdMsg
#define PORT_PRIORITY	    sm->bppg->portPriority
#define MASTER_PRIORITY	    sm->bppg->masterPriority
#define UPDT_INFO	    sm->bppg->updtInfo
#define ANN_RECEIPT_TIMEOUT_TIME_INTERVAL sm->bppg->announceReceiptTimeoutTimeInterval
#define PORT_OPER	    sm->ppg->forAllDomain->portOper
#define PTP_PORT_ENABLED    sm->ppg->ptpPortEnabled
#define AS_CAPABLE	    sm->ppg->asCapable
#define MESSAGE_PRIORITY    sm->thisSM->messagePriority
#define ANN_RECEIPT_TIMEOUT_TIME sm->thisSM->announceReceiptTimeoutTime

typedef enum {
	INIT,
	DISABLED,
	AGED,
	UPDATE,
	CURRENT,
	RECEIVE,
	SUPERIOR_MASTER_PORT,
	REPEATED_MASTER_PORT,
	INFERIOR_MASTER_OR_OTHER_PORT,
	REACTION,
}port_announce_information_state_t;

static uint8_t rcvdInfo(port_announce_information_data_t *sm)
{
	uint8_t info = OtherInfo;
	/* Store the messagePriorityVector and stepRemoved received */
	MESSAGE_PRIORITY.rootSystemIdentity.priority1 = RCVD_ANNOUNCE_PTR->grandmasterPriority1;
	MESSAGE_PRIORITY.rootSystemIdentity.clockClass =
		RCVD_ANNOUNCE_PTR->grandmasterClockQuality.clockClass;
	MESSAGE_PRIORITY.rootSystemIdentity.clockAccuracy = RCVD_ANNOUNCE_PTR->
		grandmasterClockQuality.clockAccuracy;
	MESSAGE_PRIORITY.rootSystemIdentity.offsetScaledLogVariance = RCVD_ANNOUNCE_PTR->
		grandmasterClockQuality.offsetScaledLogVariance;
	MESSAGE_PRIORITY.rootSystemIdentity.priority2 = RCVD_ANNOUNCE_PTR->grandmasterPriority2;
	memcpy(&MESSAGE_PRIORITY.rootSystemIdentity.clockIdentity,
	       RCVD_ANNOUNCE_PTR->grandmasterIdentity, sizeof(ClockIdentity));
	MESSAGE_PRIORITY.stepsRemoved = RCVD_ANNOUNCE_PTR->stepsRemoved;
	memcpy(&MESSAGE_PRIORITY.sourcePortIdentity, &RCVD_ANNOUNCE_PTR->header.sourcePortIdentity,
	       sizeof(PortIdentity));
	MESSAGE_PRIORITY.portNumber = md_port_index2number(sm->ppg->thisPort);
	sm->bppg->messageStepsRemoved = RCVD_ANNOUNCE_PTR->stepsRemoved;

	print_priority_vector( UBL_DEBUGV, "portPriority   ", &PORT_PRIORITY);
	print_priority_vector( UBL_DEBUGV, "messagePriority", &MESSAGE_PRIORITY);
	// ??? receipt of a Announce message conveys always conveys a MasterPort
	if (SELECTED_STATE[sm->ppg->thisPort] != (uint8_t)DisabledPort){
		if (memcmp(&PORT_PRIORITY, &MESSAGE_PRIORITY, sizeof(UInteger224))==0){
			UB_LOG(UBL_DEBUGV, "port_announce_information:%s:rcvdInfo=%s\n",
			       __func__, "RepeatedMasterInfo");
			info = RepeatedMasterInfo;
		}else if ((uint8_t)SUPERIOR_PRIORITY == compare_priority_vectors(&MESSAGE_PRIORITY,
									   &PORT_PRIORITY)){
			UB_LOG(UBL_DEBUGV, "port_announce_information:%s:rcvdInfo=%s\n",
			       __func__, "SuperiorMasterInfo");
			info = SuperiorMasterInfo;
			PERFMON_PPMDR_RST(sm->ppg->perfmonDS, announceRx);
		}
		else
		{
			UB_LOG(UBL_DEBUGV, "port_announce_information:%s:rcvdInfo=%s\n",
			       __func__, "InferiorMasterInfo");
			info = InferiorMasterInfo;
		}
	}
	PERFMON_PPMDR_INC(sm->ppg->perfmonDS, announceRx);
	return info;
}

static void recordOtherAnnounceInfo(port_announce_information_data_t *sm)
{
	sm->bppg->annLeap61 = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x01u) == 0x01u;
	sm->bppg->annLeap59 = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x02u) == 0x02u;
	sm->bppg->annCurrentUtcOffsetValid = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x04u) == 0x04u;
	sm->bppg->annPtpTimescale = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x08u) == 0x08u;
	sm->bppg->annTimeTraceable = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x10u) == 0x10u;
	sm->bppg->annFrequencyTraceable = (RCVD_ANNOUNCE_PTR->header.flags[1] & 0x20u) == 0x20u;
	sm->bppg->annCurrentUtcOffset = RCVD_ANNOUNCE_PTR->currentUtcOffset;
	sm->bppg->annTimeSource = RCVD_ANNOUNCE_PTR->timeSource;

	// ??? Global pathTrace is updated only when portState is known
	// to be SlavePort, in the case when system is grandmaster (no SlavePort)
	// and the Announce received may convey transition of portState to SlavePort.
	// A copy of the announce pathSequence should be used for global pathTrace
	// and a copy of the GM clockIdentity should be placed in global gmIdentity.
	sm->bppg->annPathSequenceCount = RCVD_ANNOUNCE_PTR->tlvLength / sizeof(ClockIdentity);
	memcpy(&sm->bppg->annPathSequence, &RCVD_ANNOUNCE_PTR->pathSequence,
	       RCVD_ANNOUNCE_PTR->tlvLength);
	memcpy(sm->ptasg->gmIdentity, RCVD_ANNOUNCE_PTR->grandmasterIdentity,
			sizeof(ClockIdentity));
}

static port_announce_information_state_t allstate_condition(port_announce_information_data_t *sm)
{
	if ((((!PORT_OPER || !PTP_PORT_ENABLED || !AS_CAPABLE) &&
	     (INFO_IS != (uint8_t)Disabled)) || sm->ptasg->BEGIN ||
	     !sm->ptasg->instanceEnable) && (sm->bptasg->externalPortConfiguration == VALUE_DISABLED)) {
		return DISABLED;
	}
	return (port_announce_information_state_t)sm->state;
}

static void *disabled_proc(port_announce_information_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	RCVD_MSG = false;
	// announceReceiptTimeoutTime = currentTime
	ANN_RECEIPT_TIMEOUT_TIME.nsec = cts64;
	INFO_IS = Disabled;
	RESELECT[sm->portIndex] = true;
	SELECTED[sm->portIndex] = false;

	return NULL;
}

static port_announce_information_state_t disabled_condition(port_announce_information_data_t *sm)
{
	if (PORT_OPER && PTP_PORT_ENABLED && AS_CAPABLE){
		return AGED;
	}
	/* Order is important since asCapable may not be notified prior to receiving
	   ANNOUNCE message, in such case, this state machine should chech for AGED first.
	 */
	if (RCVD_MSG){
		return DISABLED;
	}
	return (port_announce_information_state_t)sm->state;
}

static void *aged_proc(port_announce_information_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	INFO_IS = Aged;
	RESELECT[sm->portIndex] = true;
	SELECTED[sm->portIndex] = false;

	// Clear global copies of pathTrace and set gmIdentity to own clockIdentity
	sm->bppg->annPathSequenceCount = 0;
	memcpy(sm->ptasg->gmIdentity, sm->ptasg->thisClock, sizeof(ClockIdentity));

	return NULL;
}

static port_announce_information_state_t aged_condition(port_announce_information_data_t *sm)
{
	if (SELECTED[sm->portIndex] && UPDT_INFO){
		return UPDATE;
	}
	return AGED;
}

static void *update_proc(port_announce_information_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	memcpy(&PORT_PRIORITY, &MASTER_PRIORITY, sizeof(UInteger224));
	sm->bppg->portStepsRemoved = sm->bptasg->masterStepsRemoved;
	UPDT_INFO = false;
	INFO_IS = Mine;
	sm->bppg->newInfo = true;
	return NULL;
}

static port_announce_information_state_t update_condition(port_announce_information_data_t *sm)
{
	/* unconditional transfer (UCT) */
	return CURRENT;
}

static void *current_proc(port_announce_information_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	/* Do nothing */
	return NULL;
}

static port_announce_information_state_t current_condition(port_announce_information_data_t *sm,
							   uint64_t cts64)
{
	if (SELECTED[sm->portIndex] && UPDT_INFO){
		return UPDATE;
	}
	if ((INFO_IS == (uint8_t)Received) &&
	    ((cts64 >= ANN_RECEIPT_TIMEOUT_TIME.nsec) ||
	     ((cts64 >= sm->syncReceiptTimeoutTime.nsec) &&
	      sm->ptasg->gmPresent)) && !UPDT_INFO && !RCVD_MSG){
		UB_LOG(UBL_INFO, "port_announce_information:%s:domainIndex=%d, portIndex=%d, "
		       "CT=%"PRIu64", ARTT=%"PRIu64", SRTT=%"PRIu64"\n",
		       __func__, sm->domainIndex, sm->portIndex,
		       cts64, ANN_RECEIPT_TIMEOUT_TIME.nsec, sm->syncReceiptTimeoutTime.nsec);
		return AGED;
	}
	if (RCVD_MSG && !UPDT_INFO){
		return RECEIVE;
	}
	return CURRENT;
}

static void *receive_proc(port_announce_information_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	sm->thisSM->rcvdInfo = rcvdInfo(sm);
	return NULL;
}

static port_announce_information_state_t receive_condition(port_announce_information_data_t *sm)
{
	if ((sm->thisSM->rcvdInfo==(uint8_t)RepeatedMasterInfo) &&
	    !sm->ppg->forAllDomain->asymmetryMeasurementMode){
		return REPEATED_MASTER_PORT;
	}
	if (((sm->thisSM->rcvdInfo==(uint8_t)InferiorMasterInfo) ||
	     (sm->thisSM->rcvdInfo==(uint8_t)OtherInfo)) &&
	    !sm->ppg->forAllDomain->asymmetryMeasurementMode){
		return INFERIOR_MASTER_OR_OTHER_PORT;
	}
	if ((sm->thisSM->rcvdInfo==(uint8_t)SuperiorMasterInfo) &&
	    !sm->ppg->forAllDomain->asymmetryMeasurementMode){
		return SUPERIOR_MASTER_PORT;
	}
	return RECEIVE;
}

static void *superior_master_port_proc(port_announce_information_data_t *sm, uint64_t cts64)
{
	int A,B;

	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
	       __func__, sm->domainIndex, sm->portIndex);
	memcpy(&PORT_PRIORITY, &MESSAGE_PRIORITY, sizeof(UInteger224));
	sm->bppg->portStepsRemoved = RCVD_ANNOUNCE_PTR->stepsRemoved;
	recordOtherAnnounceInfo(sm);

	/* A = 16*logMessageInterval, but since we do not use subns, we remove 16 bit shift */
	A = RCVD_ANNOUNCE_PTR->header.logMessageInterval;
	ANN_RECEIPT_TIMEOUT_TIME_INTERVAL.nsec =
		(uint8_t)sm->bppg->announceReceiptTimeout * LOG_TO_NSEC(A);
	ANN_RECEIPT_TIMEOUT_TIME.nsec = cts64 + ANN_RECEIPT_TIMEOUT_TIME_INTERVAL.nsec;

	if (sm->ppg->useMgtSettableLogSyncInterval){
		B = sm->ppg->mgtSettableLogSyncInterval;
	}
	else {
		B = sm->ppg->initialLogSyncInterval;
	}
	sm->ppg->syncReceiptTimeoutTimeInterval.nsec =
		(uint8_t)sm->ppg->syncReceiptTimeout * LOG_TO_NSEC(B);
	sm->syncReceiptTimeoutTime.nsec = cts64 + sm->ppg->syncReceiptTimeoutTimeInterval.nsec;

	INFO_IS = Received;
	RESELECT[sm->portIndex] = true;
	SELECTED[sm->portIndex] = false;
	RCVD_MSG = false;
	RCVD_ANNOUNCE_PTR = NULL;
	return NULL;
}

static port_announce_information_state_t superior_master_port_condition(port_announce_information_data_t *sm)
{
	/* unconditional transfer (UCT) */
	return CURRENT;
}

static void *repeated_master_port_proc(port_announce_information_data_t *sm, uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	ANN_RECEIPT_TIMEOUT_TIME.nsec = cts64 + ANN_RECEIPT_TIMEOUT_TIME_INTERVAL.nsec;
	RCVD_MSG = false;
	RCVD_ANNOUNCE_PTR = NULL;
	return NULL;
}

static port_announce_information_state_t repeated_master_port_condition(port_announce_information_data_t *sm)
{
	/* unconditional transfer (UCT) */
	return CURRENT;
}

static void *inferior_master_or_other_port_proc(port_announce_information_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_announce_information:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	RCVD_MSG = false;
	RCVD_ANNOUNCE_PTR = NULL;
	return NULL;
}

static port_announce_information_state_t inferior_master_or_other_port_condition(port_announce_information_data_t *sm)
{
	// ?? Check if current portPriority conveys the same sourcePort as
	// the newly received inferior/other master
	if(memcmp(&MESSAGE_PRIORITY.sourcePortIdentity, &PORT_PRIORITY.sourcePortIdentity,
		  sizeof(PortIdentity))==0){
		UB_LOG(UBL_INFO, "%s: current master priority updated to InferiorMasterInfo/OtherInfo\n",
		       __func__);
		// When BMCS quick update is enabled, set the current portPriority as AGED.
		// This should restart the BMCS to try to find better master port.
		// Otherwise, follow standard with UTC to CURRENT.
		if(sm->bptasg->bmcsQuickUpdate!=0){
			return AGED;
		}
	}
	/* unconditional transfer (UCT) */
	return CURRENT;
}

void *port_announce_information_sm(port_announce_information_data_t *sm, uint64_t cts64)
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
			sm->state = DISABLED;
			break;
		case DISABLED:
			if(state_change){retp=disabled_proc(sm, cts64);}
			sm->state = disabled_condition(sm);
			break;
		case AGED:
			if(state_change){retp=aged_proc(sm);}
			sm->state = aged_condition(sm);
			break;
		case UPDATE:
			if(state_change){retp=update_proc(sm);}
			sm->state = update_condition(sm);
			break;
		case CURRENT:
			if(state_change){retp=current_proc(sm);}
			sm->state = current_condition(sm, cts64);
			break;
		case RECEIVE:
			if(state_change){retp=receive_proc(sm);}
			sm->state = receive_condition(sm);
			break;
		case SUPERIOR_MASTER_PORT:
			if(state_change){retp=superior_master_port_proc(sm, cts64);}
			sm->state = superior_master_port_condition(sm);
			break;
		case REPEATED_MASTER_PORT:
			if(state_change){retp=repeated_master_port_proc(sm, cts64);}
			sm->state = repeated_master_port_condition(sm);
			break;
		case INFERIOR_MASTER_OR_OTHER_PORT:
			if(state_change){retp=inferior_master_or_other_port_proc(sm);}
			sm->state = inferior_master_or_other_port_condition(sm);
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

void port_announce_information_sm_init(port_announce_information_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	BmcsPerTimeAwareSystemGlobal *bptasg,
	BmcsPerPortGlobal *bppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(port_announce_information_data_t, PortAnnounceInformationSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->bptasg = bptasg;
	(*sm)->bppg = bppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;

	(*sm)->syncReceiptTimeoutTime.nsec = (uint64_t)(-1LL);
}

int port_announce_information_sm_close(port_announce_information_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void port_announce_information_sm_SyncReceiptTimeoutTime(port_announce_information_data_t *sm,
							  PortSyncSync *portSyncSync)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	memcpy(&sm->syncReceiptTimeoutTime, &portSyncSync->syncReceiptTimeoutTime, sizeof(UScaledNs));
	UB_LOG(UBL_DEBUGV, "%s:syncReceiptTimeoutTime=%"PRIu64"\n", __func__, sm->syncReceiptTimeoutTime.nsec);
}
