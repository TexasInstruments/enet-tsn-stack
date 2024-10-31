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
// The file is to handle Figure 10-23 GptpCapableIntervalSetting state machine
#include <tsn_unibase/unibase.h>
#include "mind.h"
#include "mdeth.h"
#include "gptpnet.h"
#include "gptpclock.h"
#include "gptp_capable_interval_setting_sm.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	NOT_ENABLED,
	INITIALIZE,
	SET_INTERVAL,
	REACTION,
}gptp_capable_interval_setting_state_t;

#define PORT_OPER sm->ppg->forAllDomain->portOper
#define USE_MGTSETTABLE_LOG_GPTP_CAPABLE_INTERVAL_REQ_INTERVAL \
	sm->ppg->forAllDomain->useMgtSettableLogGptpCapableMessageInterval
#define PTP_PORT_ENABLED sm->ppg->ptpPortEnabled

static gptp_capable_interval_setting_state_t allstate_condition(gptp_capable_interval_setting_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ||
	   !PORT_OPER || !PTP_PORT_ENABLED || 
	   USE_MGTSETTABLE_LOG_GPTP_CAPABLE_INTERVAL_REQ_INTERVAL)
	{
		return NOT_ENABLED;
	}
	return (gptp_capable_interval_setting_state_t)sm->state;
}

static void *not_enabled_proc(gptp_capable_interval_setting_data_t *sm)
{
	if(USE_MGTSETTABLE_LOG_GPTP_CAPABLE_INTERVAL_REQ_INTERVAL){
		sm->ppg->currentLogGptpCapableMessageInterval = sm->ppg->forAllDomain->mgtSettableLogGptpCapableMessageInterval;
		// gPtpCapableMessageInterval is used in figure 10-21 gptp ascapable tx sm
		sm->ppg->gPtpCapableMessageInterval.nsec = LOG_TO_NSEC(sm->ppg->currentLogGptpCapableMessageInterval);
	}
	return NULL;
}

static gptp_capable_interval_setting_state_t not_enabled_condition(gptp_capable_interval_setting_data_t *sm)
{
	if(PORT_OPER && PTP_PORT_ENABLED &&
		!USE_MGTSETTABLE_LOG_GPTP_CAPABLE_INTERVAL_REQ_INTERVAL){
			return INITIALIZE;
	}
	return NOT_ENABLED;
}

static void *initialize_proc(gptp_capable_interval_setting_data_t *sm)
{
	sm->ppg->currentLogGptpCapableMessageInterval = sm->ppg->initialLogGptpCapableMessageInterval;
	sm->ppg->gPtpCapableMessageInterval.nsec = LOG_TO_NSEC(sm->ppg->currentLogGptpCapableMessageInterval);
	sm->thisSM->rcvdSignalingMsg4 = false;
	sm->ppg->oldAnnounceInterval.nsec = sm->ppg->gPtpCapableMessageInterval.nsec;
	sm->ppg->gPtpCapableMessageSlowdown = false;
	return NULL;
}

static gptp_capable_interval_setting_state_t initialize_condition(gptp_capable_interval_setting_data_t *sm)
{
	if (sm->thisSM->rcvdSignalingMsg4)
	{
		return SET_INTERVAL;
	}
	return INITIALIZE;
}

static void *set_interval_proc(gptp_capable_interval_setting_data_t *sm)
{
	if (!USE_MGTSETTABLE_LOG_GPTP_CAPABLE_INTERVAL_REQ_INTERVAL)
	{
		if (!LOG_INTERVAL_IN_RESERVED_RANGE(sm->thisSM->rcvdSignalingPtr->logGptpCapableMessageInterval))
		{
			switch(sm->thisSM->rcvdSignalingPtr->logGptpCapableMessageInterval){
				case (-128): /* don't change the interval */
					break;
				case 126: /* set interval to initial value */
					sm->ppg->currentLogGptpCapableMessageInterval = sm->ppg->initialLogGptpCapableMessageInterval;
					sm->ppg->gPtpCapableMessageInterval.nsec = LOG_TO_NSEC(sm->ppg->initialLogGptpCapableMessageInterval);
					break;
				default: /* use indicated value; note that the value of 127 instructs the receiving
						* port to stop sending, in accordance with Table 10-18. */
					sm->ppg->currentLogGptpCapableMessageInterval = sm->thisSM->rcvdSignalingPtr->logGptpCapableMessageInterval;
					sm->ppg->gPtpCapableMessageInterval.nsec = LOG_TO_NSEC(sm->ppg->currentLogGptpCapableMessageInterval);
					break;
			}
		}
	}
	if (sm->ppg->gPtpCapableMessageInterval.nsec < sm->ppg->oldAnnounceInterval.nsec)
	{
		sm->ppg->gPtpCapableMessageSlowdown = true;
	}
	else
	{
		sm->ppg->gPtpCapableMessageSlowdown = false;
	}
	sm->thisSM->rcvdSignalingMsg4 = false;
	return NULL;
}

static gptp_capable_interval_setting_state_t set_interval_condition(gptp_capable_interval_setting_data_t *sm)
{
	if(sm->thisSM->rcvdSignalingMsg4){
			return REACTION;
	}
	return SET_INTERVAL;
}

void gptp_capable_interval_setting_sm_init(gptp_capable_interval_setting_data_t **sm,
	int domainIndex, int portIndex,
	PerTimeAwareSystemGlobal *ptasg,
	PerPortGlobal *ppg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
	__func__, domainIndex, portIndex);
	INIT_SM_DATA(gptp_capable_interval_setting_data_t, GPTPCapableIntervalSettingSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->ppg = ppg;
	(*sm)->domainIndex = domainIndex;
	(*sm)->portIndex = portIndex;
}

void *gptp_capable_interval_setting_sm(gptp_capable_interval_setting_data_t *sm, uint64_t cts64)
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
		case SET_INTERVAL:
			if(state_change){retp=set_interval_proc(sm);}
			sm->state = set_interval_condition(sm);
			break;
		case REACTION:
			sm->state = SET_INTERVAL;
		default:
			break;
		}
		if(retp!=NULL){return retp;}
		if(sm->last_state == sm->state){break;}
	}
	return retp;
}

int gptp_capable_interval_setting_sm_close(gptp_capable_interval_setting_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, portIndex=%d\n",
		__func__, (*sm)->domainIndex, (*sm)->portIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *gptp_capable_interval_setting_sm_SignalingMsg4(gptp_capable_interval_setting_data_t *sm,
                                                PTPMsgGPTPCapableMsgIntervalReqTLV *rcvdSignalingPtr,
                                                uint64_t cts64)
{
	UB_LOG(UBL_INFO, ":%s:domainIndex=%d, portIndex=%d SignalingMsg4 RX logGptpCapInterval=%d\n",
		__func__, sm->domainIndex, sm->portIndex, rcvdSignalingPtr->logGptpCapableMessageInterval);
        sm->thisSM->rcvdSignalingPtr = rcvdSignalingPtr;
        sm->thisSM->rcvdSignalingMsg4 = true;
        return gptp_capable_interval_setting_sm(sm, cts64);
}

