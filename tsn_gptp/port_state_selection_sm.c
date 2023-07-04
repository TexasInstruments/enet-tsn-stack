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
#include "mind.h"
#include "mdeth.h"
#include "gptpnet.h"
#include "gptpclock.h"
#include "port_state_selection_sm.h"

extern char *PTPPortState_debug[];

#define SELECTED_STATE	    sm->ptasg->selectedState
#define PATH_TRACE	    sm->bptasg->pathTrace
#define LAST_GM_PRIORITY    sm->bptasg->lastGmPriority
#define GM_PRIORITY	    sm->bptasg->gmPriority
#define SYSTEM_PRIORITY	    sm->bptasg->systemPriority
#define PATH_TRACE	    sm->bptasg->pathTrace
#define RESELECT	    sm->bptasg->reselect
#define SELECTED	    sm->bptasg->selected

typedef enum {
	INIT,
	INIT_BRIDGE,
	STATE_SELECTION,
	REACTION,
}port_state_selection_state_t;

struct port_state_selection_data{
	PerTimeAwareSystemGlobal *ptasg;
	PerPortGlobal **ppgl;
	BmcsPerTimeAwareSystemGlobal *bptasg;
	BmcsPerPortGlobal **bppgl;
	port_state_selection_state_t state;
	port_state_selection_state_t last_state;
	PortStateSelectionSM *thisSM;
	int domainIndex;
	int max_ports;
	int64_t init_slave_ts;
	bool deferred_gmsync;
};

void updateStateDisabledTree(port_state_selection_data_t *sm)
{
	/* 10.3.12.2.1 ... sets all elements of selectedState to DisablePort
	   and lastGmPriority to all ones, set pathTrace to thisClock */
	int i;
	for(i=0;i<MAX_PORT_NUMBER_LIMIT;i++){
		SELECTED_STATE[i] = DisabledPort;
	}
	memset(&LAST_GM_PRIORITY, 1, sizeof(UInteger224));
	memcpy(&PATH_TRACE[0], sm->ptasg->thisClock, sizeof(ClockIdentity));
	sm->bptasg->pathTraceCount = 1;
}

void clearReselectTree(port_state_selection_data_t *sm)
{
	/* 10.3.12.2.2 set all reselect element to false */
	int i;
	for(i=0;i<MAX_PORT_NUMBER_LIMIT;i++){
		RESELECT[i] = false;
	}
}

void setSelectedTree(port_state_selection_data_t *sm)
{
	/* 10.3.12.2.3 set all selected element to true */
	int i;
	for(i=0;i<MAX_PORT_NUMBER_LIMIT;i++){
		SELECTED[i] = true;
	}
}

static int portStateUpdate(port_state_selection_data_t *sm,
			   Enumeration2 *selected_state, BmcsPerPortGlobal *bppgl,
			   PerPortGlobal *ppgl, UInteger224 *gmPathPriority)
{
	Enumeration2 oldState;
	int N;

	oldState = *selected_state;
	if(bppgl->infoIs == Disabled){
		*selected_state = DisabledPort;
		bppgl->updtInfo = false;
	} else if(ppgl->forAllDomain->asymmetryMeasurementMode == true){
		*selected_state = PassivePort;
		bppgl->updtInfo = false;
	} else if(bppgl->infoIs == Aged){
		bppgl->updtInfo = true;
		*selected_state = MasterPort;
	} else if(bppgl->infoIs == Mine){
		*selected_state = MasterPort;
		if ((memcmp(&bppgl->masterPriority,&bppgl->portPriority,
			    sizeof(UInteger224))!=0) ||
		    (sm->bptasg->masterStepsRemoved != bppgl->portStepsRemoved)){
			bppgl->updtInfo = true;
		}
	} else if(bppgl->infoIs == Received){
		// gmPriority is derived from portPriority
		if (memcmp(&GM_PRIORITY, gmPathPriority, sizeof(UInteger224))==0){
			*selected_state = SlavePort;
			bppgl->updtInfo = false;
		}else{
			if (SUPERIOR_PRIORITY !=
			    compare_priority_vectors(&bppgl->masterPriority,
						     &bppgl->portPriority)){
				*selected_state = PassivePort;
				bppgl->updtInfo = false;
			} else {
				// masterPriority is better then portPriority
				*selected_state = MasterPort;
				bppgl->updtInfo = true;
			}
		}
	}
	if((*selected_state==SlavePort) && (oldState!=SlavePort)){
		// ??? transition from none SlavePort to this port as SlavePort must
		// update the global pathTrace
		N = bppgl->annPathSequenceCount < MAX_PATH_TRACE_N ?
			bppgl->annPathSequenceCount : MAX_PATH_TRACE_N;
		if(N+1 <= MAX_PATH_TRACE_N){
			// copy pathSequence to pathTrace
			memcpy(PATH_TRACE, &bppgl->annPathSequence, sizeof(ClockIdentity)*N);
			// append thisClock to pathTrace
			memcpy(&(PATH_TRACE[N]), sm->ptasg->thisClock, sizeof(ClockIdentity));
			sm->bptasg->pathTraceCount = N+1;
		}else{
			UB_LOG(UBL_WARN, "port_state_selection:%s:pathTrace=%d (including thisClock) exceeds limit=%d\n",
			       __func__, N+1, MAX_PATH_TRACE_N);
			/* 10.3.8.23 ... a path trace TLV is not appended to an Announce message
			   and the pathTrace array is empty, once appending a clockIdentity
			   to the TLV would cause the frame carrying the Announce to exceed
			   its maximum size. */
			sm->bptasg->pathTraceCount = 0;
		}
	}

	if(oldState != *selected_state){return 1;}
	return 0;
}

static void update_lastGmInfo(PerTimeAwareSystemGlobal *ptasg)
{
	ptasg->gmTimeBaseIndicator++;
	memset(&ptasg->lastGmPhaseChange, 0, sizeof(ScaledNs));
	if(gptpconf_get_intitem(CONF_RESET_FREQADJ_BECOMEGM)){
		ptasg->lastGmFreqChange =
			(double)(-gptpclock_get_adjppb(0, ptasg->domainNumber))	/ 1.0E9;
	}else{
		ptasg->lastGmFreqChange = 0.0;
	}
}

static void *updtStatesTree(port_state_selection_data_t *sm, int64_t cts64)
{
	int i;
	UInteger224 gmPathPriority[sm->max_ports];
	bool slavePortAvail = false;
	Enumeration2 oldState;
	void *rval=NULL;
	bool gmchange=false;

	/* 10.3.12.2.3 */
	// compute gmPathPriority vector for each port
	for(i=0;i<sm->max_ports;i++){
		// initialize gmPathPriority as inferior (set all to 0xFF)
		memset(&gmPathPriority[i], 0xFF, sizeof(UInteger224));
		if (HAS_PORT_PRIORITY(sm->bppgl[i]->portPriority) &&
		    (sm->bppgl[i]->infoIs != Aged)){
			// gmPathPriority = {RM: SRM+1: PM: PNS}
			memcpy(&gmPathPriority[i], &sm->bppgl[i]->portPriority,
			       sizeof(UInteger224));
			gmPathPriority[i].stepsRemoved += 1;
		}
	}
	// save gmPriority to lastGmPriority
	memcpy(&LAST_GM_PRIORITY, &GM_PRIORITY, sizeof(UInteger224));

	// chose gmPriority as the best (superior) from the set of systemPriority and gmPathPriority
	memcpy(&GM_PRIORITY, &SYSTEM_PRIORITY, sizeof(UInteger224));
	sm->bptasg->leap61 = sm->bptasg->sysLeap61;
	sm->bptasg->leap59 = sm->bptasg->sysLeap59;
	sm->bptasg->currentUtcOffsetValid = sm->bptasg->sysCurrentUTCOffsetValid;
	sm->bptasg->ptpTimescale = sm->bptasg->sysPtpTimescale;
	sm->bptasg->timeTraceable = sm->bptasg->sysTimeTraceable;
	sm->bptasg->frequencyTraceable = sm->bptasg->sysFrequencyTraceable;
	sm->bptasg->currentUtcOffset = sm->bptasg->sysCurrentUtcOffset;
	sm->bptasg->timeSource = sm->bptasg->sysTimeSource;

	for(i=0;i<sm->max_ports;i++){
		if (sm->bppgl[i]->infoIs == Disabled){continue;}
		if ((memcmp(gmPathPriority[i].sourcePortIdentity.clockIdentity,
			   &sm->ptasg->thisClock, sizeof(ClockIdentity))!=0) &&
		    (SUPERIOR_PRIORITY == compare_priority_vectors(&gmPathPriority[i],
								   &GM_PRIORITY))){
			UB_LOG(UBL_DEBUGV, "port_state_selection:%s:"
			       "new gmPriority from portIndex=%d\n", __func__, i);
			memcpy(&GM_PRIORITY, &gmPathPriority[i], sizeof(UInteger224));
			sm->bptasg->leap61 = sm->bppgl[i]->annLeap61;
			sm->bptasg->leap59 = sm->bppgl[i]->annLeap59;
			sm->bptasg->currentUtcOffsetValid = sm->bppgl[i]->annCurrentUtcOffsetValid;
			sm->bptasg->ptpTimescale = sm->bppgl[i]->annPtpTimescale;
			sm->bptasg->timeTraceable = sm->bppgl[i]->annTimeTraceable;
			sm->bptasg->frequencyTraceable = sm->bppgl[i]->annFrequencyTraceable;
			sm->bptasg->currentUtcOffset = sm->bppgl[i]->annCurrentUtcOffset;
			sm->bptasg->timeSource = sm->bppgl[i]->annTimeSource;
			UB_LOG(UBL_DEBUG, "port_state_selection:%s:"
			       "new GM from domainIndex=%d portIndex=%d, state=%s\n", __func__,
			       sm->domainIndex, i, PTPPortState_debug[SELECTED_STATE[i]]);
			print_priority_vector(UBL_DEBUG,"GM", &GM_PRIORITY);
		}
	}
	if(memcmp(&LAST_GM_PRIORITY, &GM_PRIORITY, sizeof(UInteger224))){
		UB_TLOG(UBL_INFO, "domainIndex=%d, GM changed old="UB_PRIhexB8", new="UB_PRIhexB8"\n",
			 sm->domainIndex,
			 UB_ARRAY_B8(LAST_GM_PRIORITY.rootSystemIdentity.clockIdentity),
			 UB_ARRAY_B8(GM_PRIORITY.rootSystemIdentity.clockIdentity));
		gptpclock_set_gmchange(sm->ptasg->domainNumber,
				       GM_PRIORITY.rootSystemIdentity.clockIdentity);
		gmchange=true;
		rval=GM_PRIORITY.rootSystemIdentity.clockIdentity;
	}

	print_priority_vector( UBL_DEBUGV,"gmPathPriority", &GM_PRIORITY);
	// compute masterPriority for each port
	for(i=0;i<sm->max_ports;i++){
		// masterPriority Vector = { SS : 0: { CS: PNQ}: PNQ} or
		// masterPriority Vector = { RM : SRM+1: { CS: PNQ}: PNQ} or
		memcpy(&sm->bppgl[i]->masterPriority, &GM_PRIORITY, sizeof(UInteger224));
		memcpy(&sm->bppgl[i]->masterPriority.sourcePortIdentity.clockIdentity,
		       &sm->ptasg->thisClock, sizeof(ClockIdentity));
		sm->bppgl[i]->masterPriority.sourcePortIdentity.portNumber =
			sm->ppgl[i]->thisPort;
		sm->bppgl[i]->masterPriority.portNumber = sm->ppgl[i]->thisPort;
	}
	// compute masterStepsRemoved
	sm->bptasg->masterStepsRemoved = GM_PRIORITY.stepsRemoved;
	// assign port state
	for(i=1;i<sm->max_ports;i++){
		oldState=SELECTED_STATE[i];
		if(portStateUpdate(sm, &SELECTED_STATE[i],
				   sm->bppgl[i], sm->ppgl[i], &gmPathPriority[i])){
			UB_LOG(UBL_DEBUG, "port_state_selection:%s: domainIndex=%d portIndex=%d "
			       "state %s -> %s\n",
			       __func__, sm->domainIndex, i, PTPPortState_debug[oldState],
			       PTPPortState_debug[SELECTED_STATE[i]]);
			if(oldState==SlavePort){
				gptpclock_reset_gmsync(0, sm->ptasg->domainNumber);
			}
			continue;
		}
		UB_LOG(UBL_DEBUGV, "port_state_selection:%s: domainIndex=%d portIndex=%d "
		       "selectedState=%s updtInfo=%d\n",
		       __func__, sm->domainIndex, i, PTPPortState_debug[SELECTED_STATE[i]],
		       sm->bppgl[i]->updtInfo);
	}
	// update gmPresent
	if (GM_PRIORITY.rootSystemIdentity.priority1 < 255){
		sm->ptasg->gmPresent = true;
	} else {
		sm->ptasg->gmPresent = false;
	}

	// assign portState for port 0
	for(i=1;i<sm->max_ports;i++){
		if(SELECTED_STATE[i]==SlavePort){
			slavePortAvail = true;
			break;
		}
	}

	// update master port
	oldState=SELECTED_STATE[0];
	if(slavePortAvail){
		SELECTED_STATE[0] = PassivePort; // we are not GM
	}else{
		SELECTED_STATE[0] = SlavePort; // we are GM
		if(gmchange){update_lastGmInfo(sm->ptasg);}
	}
	if(oldState != SELECTED_STATE[0]){
		UB_LOG(UBL_DEBUG, "port_state_selection:%s: domainIndex=%d portIndex=0 "
		       "state %s -> %s\n",
		       __func__, sm->domainIndex, PTPPortState_debug[oldState],
		       PTPPortState_debug[SELECTED_STATE[0]]);
		if(SELECTED_STATE[0] == SlavePort){
			/* we are GM and the clock doesn't have to sync to the other,
			   which means the master clock is synced status.
			   if init_slave_ts is set, defer gptpclock_set_gmsync */
			if(!sm->deferred_gmsync){
				gptpclock_set_gmsync(0, sm->ptasg->domainNumber,
						     sm->ptasg->thisClock, true);
			}
		}else{
			gptpclock_reset_gmsync(0, sm->ptasg->domainNumber);
		}
	}

	// system is the grandmaster, set pathTrace to thisClock
	if (memcmp(&sm->ptasg->thisClock, &GM_PRIORITY.rootSystemIdentity.clockIdentity,
		   sizeof(ClockIdentity))==0){
		memcpy(&PATH_TRACE[0], &sm->ptasg->thisClock, sizeof(ClockIdentity));
		sm->bptasg->pathTraceCount = 1;
	}
	return rval;
}

static void *proc_init_slave(port_state_selection_data_t *sm, int64_t cts64)
{
	if(SELECTED_STATE[0] != SlavePort && gptpclock_get_gmsync(0, 0)==1 &&
	   sm->ptasg->gm_stable_initdone){
		UB_LOG(UBL_INFO, "%s:already synced, "
		       "Domain0-priority1 returns to the configured value\n",__func__);
		goto clearexit;
	}else if(sm->init_slave_ts<cts64){
		UB_LOG(UBL_INFO, "%s:expired, "
		       "Domain0-priority1 returns to the configured value\n",__func__);
		if(SELECTED_STATE[0] == SlavePort){
			// this is GM, call the deferred process
			gptpclock_set_gmsync(0, sm->ptasg->domainNumber, sm->ptasg->thisClock, true);
		}
		goto clearexit;
	} else {
		return NULL;
	}
clearexit:
	sm->deferred_gmsync=false;
	sm->init_slave_ts=0;
	SYSTEM_PRIORITY.rootSystemIdentity.priority1=
		gptpconf_get_intitem(CONF_PRIMARY_PRIORITY1);
	return updtStatesTree(sm, cts64);
}

static void *proc_deferred_gmsync(port_state_selection_data_t *sm, int64_t cts64)
{
	if(sm->init_slave_ts) {
		// domain0 only
		return proc_init_slave(sm, cts64);
	}
	if(sm->domainIndex==0){return NULL;}

	if(gptpclock_get_gmstable(0)){
		if(SELECTED_STATE[0] == SlavePort){
			// this is GM on dominaIndex > 0
			gptpclock_set_gmsync(0, sm->ptasg->domainNumber, sm->ptasg->thisClock, true);
		}
		sm->deferred_gmsync=false;
	}
	return NULL;
}

static port_state_selection_state_t allstate_condition(port_state_selection_data_t *sm)
{
	if ((sm->ptasg->BEGIN || !sm->ptasg->instanceEnable) &&
	    (sm->bptasg->externalPortConfiguration == VALUE_DISABLED)) {
		return INIT_BRIDGE;
	}
	return sm->state;
}

static void *init_bridge_proc(port_state_selection_data_t *sm, int64_t cts64)
{
	int i;
	int static_slave=gptpconf_get_intitem(CONF_STATIC_PORT_STATE_SLAVE_PORT);
	UB_LOG(UBL_DEBUGV, "port_state_selection:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	if(static_slave>=0){
		for(i=0;i<MAX_PORT_NUMBER_LIMIT;i++){
			if(i==static_slave){
				SELECTED_STATE[i] = SlavePort;
			}else{
				SELECTED_STATE[i] = MasterPort;
			}
		}
		sm->ptasg->gmPresent = true;
		if(static_slave==0){
			gptpclock_set_gmsync(0, sm->ptasg->domainNumber,
					     sm->ptasg->thisClock, true);
		}
		return NULL;
	}
	updateStateDisabledTree(sm);
	return NULL;
}

static port_state_selection_state_t init_bridge_condition(port_state_selection_data_t *sm)
{
	if(gptpconf_get_intitem(CONF_STATIC_PORT_STATE_SLAVE_PORT)>=0){return INIT_BRIDGE;}
	return STATE_SELECTION;
}

static void *state_selection_proc(port_state_selection_data_t *sm, int64_t cts64)
{
	void *rval;
	UB_LOG(UBL_DEBUGV, "port_state_selection:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	sm->thisSM->systemIdentityChange = false;
	sm->thisSM->forAllDomain->asymmetryMeasurementModeChange = false;
	clearReselectTree(sm);

	if(sm->domainIndex==0 && gptpconf_get_intitem(CONF_INITIAL_SLAVE_TIME) &&
	   !sm->ptasg->gm_stable_initdone){
		UB_LOG(UBL_INFO, "%s:Domain0 start with priority1=254\n", __func__);
		SYSTEM_PRIORITY.rootSystemIdentity.priority1=254; // set lowest
		sm->init_slave_ts=cts64;
		sm->init_slave_ts+=gptpconf_get_intitem(CONF_INITIAL_SLAVE_TIME)*UB_SEC_NS;
		sm->deferred_gmsync=true;
	}
	if(sm->domainIndex!=0 && !sm->ptasg->gm_stable_initdone) {
		sm->deferred_gmsync=true;
	}

	rval=updtStatesTree(sm, cts64);
	setSelectedTree(sm);
	return rval;
}

static port_state_selection_state_t state_selection_condition(port_state_selection_data_t *sm)
{
	int i;
	bool reselected = false;
	for(i=0;i<MAX_PORT_NUMBER_LIMIT;i++){
		if (RESELECT[i]) {
			reselected=true;
			break;
		}
	}
	if ((reselected) || (sm->thisSM->systemIdentityChange) ||
	    (sm->thisSM->forAllDomain->asymmetryMeasurementModeChange)){
		sm->last_state = REACTION;
		return STATE_SELECTION;
	}
	return sm->state;
}

// return clockIdentity when GM has changed
void *port_state_selection_sm(port_state_selection_data_t *sm, uint64_t cts64)
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
			sm->state = INIT_BRIDGE;
			break;
		case INIT_BRIDGE:
			if(state_change){retp=init_bridge_proc(sm, cts64);}
			sm->state = init_bridge_condition(sm);
			break;
		case STATE_SELECTION:
			if(sm->deferred_gmsync) {
				if((retp=proc_deferred_gmsync(sm, cts64))){break;}
			}
			if(state_change){retp=state_selection_proc(sm, cts64);}
			sm->state = state_selection_condition(sm);
			break;
		case REACTION:
			break;
		}
		if(retp){return retp;}
		if(sm->last_state == sm->state){break;}
	}
	return retp;
}

void port_state_selection_sm_init(port_state_selection_data_t **sm,
	int domainIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal **ppgl,
	BmcsPerTimeAwareSystemGlobal *bptasg,
	BmcsPerPortGlobal **bppgl,
	int max_ports,
	port_state_selection_data_t **forAllDomainSM)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, domainIndex);
	if(INIT_SM_DATA(port_state_selection_data_t, PortStateSelectionSM, sm)){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppgl = ppgl;
	(*sm)->bptasg = bptasg;
	(*sm)->bppgl = bppgl;
	(*sm)->domainIndex = domainIndex;
	(*sm)->max_ports = max_ports;

	if (domainIndex==0){
		if(ub_assert_fatal((*sm)==(*forAllDomainSM), __func__, NULL)){return;}
		// initialize forAllDomain
		(*sm)->thisSM->forAllDomain=(PortStateSelectionSMforAllDomain *)
			malloc(sizeof(PortStateSelectionSMforAllDomain));
		if(ub_assert_fatal((*sm)->thisSM->forAllDomain!=NULL,
				   __func__, "malloc")){return;}
		memset((*sm)->thisSM->forAllDomain, 0, sizeof(PortStateSelectionSMforAllDomain));
		(*sm)->thisSM->forAllDomain->asymmetryMeasurementModeChange = false;
	}else{
		if(ub_assert_fatal((*forAllDomainSM!=NULL), __func__, "forAllDomain")){return;}
		if(ub_assert_fatal(((*forAllDomainSM)->thisSM->forAllDomain!=NULL),
				   __func__, "forAllDomain")){return;}
		(*sm)->thisSM->forAllDomain = (*forAllDomainSM)->thisSM->forAllDomain;
	}
}

int port_state_selection_sm_close(port_state_selection_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, (*sm)->domainIndex);
	if ((*sm)->domainIndex==0){free((*sm)->thisSM->forAllDomain);}
	CLOSE_SM_DATA(sm);
	return 0;
}
