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
#include "announce_interval_setting_sm.h"
#include "gptpcommon.h"

#define PTP_PORT_ENABLED    sm->ppg->ptpPortEnabled
#define PORT_OPER           sm->ppg->forAllDomain->portOper

typedef enum {
	INIT,
	NOT_ENABLED,
	INITIALIZE,
	SET_INTERVALS,
	REACTION,
}announce_interval_setting_state_t;

static announce_interval_setting_state_t allstate_condition(announce_interval_setting_data_t *sm)
{
        if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable || !PORT_OPER ||
            !PTP_PORT_ENABLED ||
            sm->ppg->forAllDomain->useMgtSettableLogAnnounceInterval){
                   return NOT_ENABLED;
        }
        return (announce_interval_setting_state_t)sm->state;
}

static void *not_enabled_proc(announce_interval_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "announce_interval_setting:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
        if(sm->ppg->forAllDomain->useMgtSettableLogAnnounceInterval){
                sm->bppg->currentLogAnnounceInterval =
                        sm->ppg->forAllDomain->mgtSettableLogAnnounceInterval;
                sm->bppg->announceInterval.nsec = LOG_TO_NSEC(sm->bppg->currentLogAnnounceInterval);
        }
	return NULL;
}

static announce_interval_setting_state_t not_enabled_condition(announce_interval_setting_data_t *sm)
{
        if(PORT_OPER && PTP_PORT_ENABLED &&
           !sm->ppg->forAllDomain->useMgtSettableLogAnnounceInterval){
                return INITIALIZE;
        }
        return NOT_ENABLED;
}

static void *initialize_proc(announce_interval_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "announce_interval_setting:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
        sm->bppg->currentLogAnnounceInterval = sm->bppg->initialLogAnnounceInterval;
        sm->bppg->announceInterval.nsec = LOG_TO_NSEC(sm->bppg->initialLogAnnounceInterval);
        sm->thisSM->rcvdSignalingMsg2 = false;
        memcpy(&sm->bppg->oldAnnounceInterval, &sm->bppg->announceInterval, sizeof(UScaledNs));
        sm->bppg->announceSlowdown = false;
	UB_LOG(UBL_DEBUGV, "announce_interval_setting:%s:announceInterval=%"PRIu64"\n",
               __func__, sm->bppg->announceInterval.nsec);
	return NULL;
}

static announce_interval_setting_state_t initialize_condition(announce_interval_setting_data_t *sm)
{
        if(sm->thisSM->rcvdSignalingMsg2){
                return SET_INTERVALS;
        }
	return INITIALIZE;
}

static void *set_intervals_proc(announce_interval_setting_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "announce_interval_setting:%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
        switch(sm->thisSM->rcvdSignalingPtr->announceInterval){
        case (-128): /* don't change the interval */
                break;
        case 126: /* set interval to initial value */
                sm->bppg->currentLogAnnounceInterval = sm->bppg->initialLogAnnounceInterval;
                sm->bppg->announceInterval.nsec = LOG_TO_NSEC(sm->bppg->initialLogAnnounceInterval);
                break;
        default: /* use indicated value */
                sm->bppg->announceInterval.nsec = LOG_TO_NSEC(sm->thisSM->rcvdSignalingPtr->announceInterval);
                sm->bppg->currentLogAnnounceInterval = sm->thisSM->rcvdSignalingPtr->announceInterval;
                break;
        }
        sm->thisSM->rcvdSignalingMsg2 = false;
        if(sm->bppg->announceInterval.nsec < sm->bppg->oldAnnounceInterval.nsec){
                sm->bppg->announceSlowdown = true;
        }else{
                sm->bppg->announceSlowdown = false;
        }
	return NULL;
}

static announce_interval_setting_state_t set_intervals_condition(announce_interval_setting_data_t *sm)
{
        if(sm->thisSM->rcvdSignalingMsg2){
                return REACTION;
        }
        return SET_INTERVALS;
}


void *announce_interval_setting_sm(announce_interval_setting_data_t *sm, uint64_t cts64)
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
			sm->state = NOT_ENABLED;
			break;
		case NOT_ENABLED:
			if(state_change){retp=not_enabled_proc(sm);}
			sm->state = not_enabled_condition(sm);
			break;
		case INITIALIZE:
			if(state_change){retp=initialize_proc(sm);}
			sm->state = initialize_condition(sm);
			break;
		case SET_INTERVALS:
			if(state_change){retp=set_intervals_proc(sm);}
			sm->state = set_intervals_condition(sm);
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

void announce_interval_setting_sm_init(announce_interval_setting_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg,
	BmcsPerPortGlobal *bppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, domainIndex, portIndex);
	INIT_SM_DATA(announce_interval_setting_data_t, AnnounceIntervalSettingSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->bppg = bppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

int announce_interval_setting_sm_close(announce_interval_setting_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *announce_interval_setting_sm_SignalingMsg2(announce_interval_setting_data_t *sm,
                                                PTPMsgIntervalRequestTLV *rcvdSignalingPtr,
                                                uint64_t cts64)
{
	UB_LOG(UBL_DEBUGV, ":%s:domainIndex=%d, portIndex=%d\n",
		__func__, sm->domainIndex, sm->portIndex);
        sm->thisSM->rcvdSignalingPtr = rcvdSignalingPtr;
        sm->thisSM->rcvdSignalingMsg2 = true;
        return announce_interval_setting_sm(sm, cts64);
}
