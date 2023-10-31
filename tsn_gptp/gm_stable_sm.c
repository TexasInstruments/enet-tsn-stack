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
#include "gm_stable_sm.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	INITIALIZE,
	GM_LOST,
	GM_UNSTABLE,
	GM_STABLE,
	REACTION,
}gm_stable_state_t;

#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static gm_stable_state_t allstate_condition(gm_stable_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->asCapableOrAll ) {
		return INITIALIZE;
	}
	return (gm_stable_state_t)sm->state;
}

static void *initialize_proc(gm_stable_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "gm_stable:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	sm->gm_stable_time=0;
	sm->gm_stable_timer_time=gptpgcfg_get_intitem(
		GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_INITIAL_GM_STABLE_TIME,
		YDBI_CONFIG)*1000000LL;
	sm->ptasg->gm_stable_initdone=false;
	if(sm->ptasg->selectedState[0]!=(uint8_t)SlavePort){
		// if this device is not GM, gmsync must be lost
		(void)gptpclock_set_gmsync(GPTPINSTNUM, sm->domainIndex, GMSYNC_UNSYNC);
	}else{
		(void)gptpclock_set_gmsync(GPTPINSTNUM, sm->domainIndex, GMSYNC_SYNC_STABLE);
	}
	return NULL;
}

static gm_stable_state_t initialize_condition(gm_stable_data_t *sm)
{
	if(sm->ptasg->asCapableOrAll &&
	   (gptpclock_get_gmsync(GPTPINSTNUM, sm->ptasg->domainIndex)>=GMSYNC_SYNC)){
		return GM_UNSTABLE;
	}
	return INITIALIZE;
}

static void *gm_lost_proc(gm_stable_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "gm_stable:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	sm->gm_stable_time=0;
	sm->gm_change=false;
	return NULL;
}

static gm_stable_state_t gm_lost_condition(gm_stable_data_t *sm)
{
	if(gptpclock_get_gmsync(GPTPINSTNUM, sm->ptasg->domainIndex)<GMSYNC_SYNC_STABLE){
		return GM_UNSTABLE;
	}
	return GM_LOST;
}

static void *gm_unstable_proc(gm_stable_data_t *sm, uint64_t cts64)
{
	UB_TLOG(UBL_INFO, "gm_stable:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	sm->gm_stable_time=cts64+sm->gm_stable_timer_time;
	if(gptpclock_get_gmsync(GPTPINSTNUM, sm->ptasg->domainIndex)==GMSYNC_SYNC_STABLE){
		gptpclock_set_gmsync(GPTPINSTNUM, sm->domainIndex, GMSYNC_SYNC);
	}
	return NULL;
}

static gm_stable_state_t gm_unstable_condition(gm_stable_data_t *sm, uint64_t cts64)
{
	if(cts64>sm->gm_stable_time){return GM_STABLE;}
	if(sm->gm_change){return GM_LOST;}
	return GM_UNSTABLE;
}

static void *gm_stable_proc(gm_stable_data_t *sm)
{
	UB_TLOG(UBL_INFO, "gm_stable:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	sm->gm_stable_time=0;
	sm->gm_stable_timer_time=gptpgcfg_get_intitem(
		GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_NORMAL_GM_STABLE_TIME,
		YDBI_CONFIG)*1000000LL;
	if(gptpclock_get_gmsync(GPTPINSTNUM, sm->ptasg->domainIndex)==GMSYNC_SYNC){
		gptpclock_set_gmsync(GPTPINSTNUM, sm->domainIndex, GMSYNC_SYNC_STABLE);
	}
	sm->ptasg->gm_stable_initdone=true;
	return NULL;
}

static gm_stable_state_t gm_stable_condition(gm_stable_data_t *sm)
{
	if(sm->gm_change){return GM_LOST;}
	return GM_STABLE;
}


void *gm_stable_sm(gm_stable_data_t *sm, uint64_t cts64)
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
		case GM_LOST:
			if(state_change){retp=gm_lost_proc(sm);}
			sm->state = gm_lost_condition(sm);
			break;
		case GM_UNSTABLE:
			if(state_change){retp=gm_unstable_proc(sm, cts64);}
			sm->state = gm_unstable_condition(sm, cts64);
			break;
		case GM_STABLE:
			if(state_change){retp=gm_stable_proc(sm);}
			sm->state = gm_stable_condition(sm);
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

void gm_stable_sm_init(gm_stable_data_t **sm,
		       int domainIndex,
		       PerTimeAwareSystemGlobal *ptasg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, domainIndex);
	if(!*sm){
		*sm=(gm_stable_data_t *)UB_SD_GETMEM(SM_DATA_INST, sizeof(gm_stable_data_t));
		if(ub_assert_fatal(*sm!=NULL, __func__, "malloc")){return;}
	}
	(void)memset(*sm, 0, sizeof(gm_stable_data_t));
	(*sm)->ptasg = ptasg;
	(*sm)->domainIndex = domainIndex;
}

int gm_stable_sm_close(gm_stable_data_t **sm)
{
	if(!*sm){return 0;}
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, (*sm)->domainIndex);
	UB_SD_RELMEM(SM_DATA_INST, *sm);
	*sm=NULL;
	return 0;
}

void gm_stable_gm_change(gm_stable_data_t *sm, ClockIdentity clockIdentity, uint64_t cts64)
{
	if(!memcmp(sm->clockIdentity, clockIdentity, sizeof(ClockIdentity))){return;}
	memcpy(sm->clockIdentity, clockIdentity, sizeof(ClockIdentity));
	sm->gm_change=true;
	(void)gm_stable_sm(sm, cts64);
	return;
}
