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
#include "port_state_setting_ext_sm.h"
#include "gptpcommon.h"

#define SELECTED_STATE	    sm->ptasg->selectedState
#define PATH_TRACE	    sm->bptasg->pathTrace
#define GM_PRIORITY	    sm->bptasg->gmPriority
#define SYSTEM_PRIORITY	    sm->bptasg->systemPriority

typedef enum {
	INIT,
	INITIALIZE,
	STATE_SETTING,
	REACTION,
}port_state_setting_ext_state_t;

void resetStateDisabledTree(port_state_setting_ext_data_t *sm)
{
	int i;
	bool slavePortAvail = false;
	SELECTED_STATE[sm->portIndex] = DisabledPort;
	for(i=0;i<sm->max_ports;i++){
		if (SELECTED_STATE[i]==(uint8_t)SlavePort){
			slavePortAvail = true;
			break;
		}
	}
	if(!slavePortAvail){
		(void)memset(PATH_TRACE, 0, sizeof(ClockIdentity) * (uint8_t)MAX_PATH_TRACE_N);
		memcpy(&PATH_TRACE[0], sm->ptasg->thisClock, sizeof(ClockIdentity));
	}
}

void updtPortState(port_state_setting_ext_data_t *sm)
{
	int i,slaveIndex=-1;
	bool slavePortAvail = false;
	for(i=1;i<sm->max_ports;i++){
		if (SELECTED_STATE[i]==(uint8_t)SlavePort){
			slavePortAvail = true;
			slaveIndex=i;
			break;
		}
	}
	if(slavePortAvail){
		// use announce information
		sm->bptasg->leap61 = sm->bppgl[sm->portIndex]->annLeap61;
		sm->bptasg->leap59 = sm->bppgl[sm->portIndex]->annLeap59;
		sm->bptasg->currentUtcOffsetValid = sm->bppgl[sm->portIndex]->annCurrentUtcOffsetValid;
		sm->bptasg->ptpTimescale = sm->bppgl[sm->portIndex]->annPtpTimescale;
		sm->bptasg->timeTraceable = sm->bppgl[sm->portIndex]->annTimeTraceable;
		sm->bptasg->frequencyTraceable = sm->bppgl[sm->portIndex]->annFrequencyTraceable;
		sm->bptasg->currentUtcOffset = sm->bppgl[sm->portIndex]->annCurrentUtcOffset;
		sm->bptasg->timeSource = sm->bppgl[sm->portIndex]->annTimeSource;

		sm->bptasg->masterStepsRemoved = sm->bppgl[slaveIndex]->portStepsRemoved;

		if(sm->messagePriority.rootSystemIdentity.priority1 < 255u){
			sm->ptasg->gmPresent = true;

			memcpy(sm->ptasg->gmIdentity,
					sm->bppgl[sm->portIndex]->masterPriority.rootSystemIdentity.clockIdentity,
					sizeof(ClockIdentity));
		}else{
			sm->ptasg->gmPresent = false;
		}
	} else {
		// use system information
		memcpy(&GM_PRIORITY, &SYSTEM_PRIORITY, sizeof(UInteger224));
		sm->bptasg->leap61 = sm->bptasg->sysLeap61;
		sm->bptasg->leap59 = sm->bptasg->sysLeap59;
		sm->bptasg->currentUtcOffsetValid = sm->bptasg->sysCurrentUTCOffsetValid;
		sm->bptasg->ptpTimescale = sm->bptasg->sysPtpTimescale;
		sm->bptasg->timeTraceable = sm->bptasg->sysTimeTraceable;
		sm->bptasg->frequencyTraceable = sm->bptasg->sysFrequencyTraceable;
		sm->bptasg->currentUtcOffset = sm->bptasg->sysCurrentUtcOffset;
		sm->bptasg->timeSource = sm->bptasg->sysTimeSource;

		sm->bptasg->masterStepsRemoved = 0;

		if(sm->bptasg->systemPriority.rootSystemIdentity.priority1 < 255u){
			sm->ptasg->gmPresent = true;

			memcpy(sm->ptasg->gmIdentity,
					sm->bptasg->systemPriority.rootSystemIdentity.clockIdentity,
					sizeof(ClockIdentity));

		}else{
			sm->ptasg->gmPresent = false;
		}
	}
	// assign port state
	if(sm->thisSM->disabledExt){
		SELECTED_STATE[sm->portIndex] = DisabledPort;
	}else if(sm->ppgl[sm->portIndex]->forAllDomain->asymmetryMeasurementMode){
		SELECTED_STATE[sm->portIndex] = PassivePort;
	}else{
		SELECTED_STATE[sm->portIndex] = sm->thisSM->portStateInd;
	}
	UB_LOG(UBL_DEBUGV, "port_state_setting_ext:%s: portIndex=%d selectedState=%d\n",
	       __func__, sm->portIndex, SELECTED_STATE[sm->portIndex]);

	// assign portState for port 0
	if(SELECTED_STATE[sm->portIndex]==(uint8_t)SlavePort){
		SELECTED_STATE[0] = PassivePort;
	}else if(!slavePortAvail){
		SELECTED_STATE[0] = SlavePort;
	}else{}

	// compute gmPriority Vector
	if(SELECTED_STATE[sm->portIndex]==(uint8_t)SlavePort){
		memcpy(&GM_PRIORITY, &sm->messagePriority, sizeof(UInteger224));
	}
	if(!slavePortAvail){
		memcpy(&GM_PRIORITY, &SYSTEM_PRIORITY, sizeof(UInteger224));
	}

	// compute masterPriority vector
	// masterPriority Vector = { SS : 0: { CS: PNQ}: PNQ} or
	// masterPriority Vector = { RM : SRM+1: { CS: PNQ}: PNQ} or
	memcpy(&sm->bppgl[sm->portIndex]->masterPriority, &GM_PRIORITY, sizeof(UInteger224));
	memcpy(&sm->bppgl[sm->portIndex]->masterPriority.sourcePortIdentity.clockIdentity,
	       &sm->ptasg->thisClock, sizeof(ClockIdentity));
	sm->bppgl[sm->portIndex]->masterPriority.sourcePortIdentity.portIndex =
		sm->ppgl[sm->portIndex]->thisPort;
	sm->bppgl[sm->portIndex]->masterPriority.portNumber =
		md_port_index2number(sm->ppgl[sm->portIndex]->thisPort);

	// set pathTrace
	if(!slavePortAvail){
		(void)memset(PATH_TRACE, 0, sizeof(ClockIdentity) * (uint8_t)MAX_PATH_TRACE_N);
		memcpy(&PATH_TRACE[0], sm->ptasg->thisClock, sizeof(ClockIdentity));
	}

}

static port_state_setting_ext_state_t allstate_condition(port_state_setting_ext_data_t *sm)
{
	if((sm->ptasg->BEGIN || !sm->ptasg->instanceEnable) &&
	   (sm->bptasg->externalPortConfiguration == VALUE_ENABLED)) {
		return INITIALIZE;
	}
	return (port_state_setting_ext_state_t)sm->state;
}

static void *initialize_proc(port_state_setting_ext_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_state_setting_ext:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	resetStateDisabledTree(sm);
	return NULL;
}

static port_state_setting_ext_state_t initialize_condition(port_state_setting_ext_data_t *sm)
{
	if(sm->thisSM->rcvdPortStateInd ||
	   sm->thisSM->forAllDomain->asymmetryMeasurementModeChangeThisPort){
		return STATE_SETTING;
	}
	return INITIALIZE;
}

static void *state_setting_proc(port_state_setting_ext_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "port_state_setting_ext:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	updtPortState(sm);
	sm->thisSM->forAllDomain->asymmetryMeasurementModeChangeThisPort = false;
	sm->thisSM->rcvdPortStateInd = false;
	return NULL;
}

static port_state_setting_ext_state_t state_setting_condition(port_state_setting_ext_data_t *sm)
{
	if(sm->thisSM->rcvdPortStateInd ||
	   sm->thisSM->disabledExt ||
	   sm->thisSM->forAllDomain->asymmetryMeasurementModeChangeThisPort){
		sm->last_state = REACTION;
		return STATE_SETTING;
	}
	return STATE_SETTING;
}


void *port_state_setting_ext_sm(port_state_setting_ext_data_t *sm, uint64_t cts64)
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
		case STATE_SETTING:
			if(state_change){retp=state_setting_proc(sm);}
			sm->state = state_setting_condition(sm);
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

void port_state_setting_ext_sm_init(port_state_setting_ext_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal **ppgl,
	BmcsPerTimeAwareSystemGlobal *bptasg,
	BmcsPerPortGlobal **bppgl,
	int max_ports,
	port_state_setting_ext_data_t **forAllDomainSM)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(port_state_setting_ext_data_t, PortStateSettingExtSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppgl = ppgl;
	(*sm)->bptasg = bptasg;
	(*sm)->bppgl = bppgl;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
	(*sm)->max_ports = max_ports;
	(void)memset(&(*sm)->messagePriority, 0xFF, sizeof(UInteger224));

	if(domainIndex==0){
		if(ub_assert_fatal((*sm)==(*forAllDomainSM), __func__, NULL)){return;}
		//initialize forAllDomain
		(*sm)->thisSM->forAllDomain=(PortStateSettingExtSMforAllDomain*)UB_SD_GETMEM(GPTP_SMALL_ALLOC,
							 sizeof(PortStateSettingExtSMforAllDomain));
		if(ub_assert_fatal((*sm)->thisSM->forAllDomain, __func__, "malloc")){return;}
		(void)memset((*sm)->thisSM->forAllDomain, 0, sizeof(PortStateSettingExtSMforAllDomain));
		(*sm)->thisSM->forAllDomain->asymmetryMeasurementModeChangeThisPort = false;
	}else{
		if(ub_assert_fatal(*forAllDomainSM && ((*forAllDomainSM)->thisSM->forAllDomain),
				   __func__, "forAllDomain")){return;}
		(*sm)->thisSM->forAllDomain = (*forAllDomainSM)->thisSM->forAllDomain;
	}
}

int port_state_setting_ext_sm_close(port_state_setting_ext_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	if((*sm)->domainIndex==0){UB_SD_RELMEM(GPTP_SMALL_ALLOC, (*sm)->thisSM->forAllDomain);}
	CLOSE_SM_DATA(sm);
	return 0;
}

void port_state_setting_ext_sm_messagePriority(port_state_setting_ext_data_t *sm,
					       UInteger224 *messagePriority)
{	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
	memcpy(&sm->messagePriority, messagePriority, sizeof(UInteger224));
}
