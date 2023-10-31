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
#include "site_sync_sync_sm.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	INITIALIZING,
	RECEIVING_SYNC,
	REACTION,
}site_sync_sync_state_t;

#define RCVD_PSSYNC sm->thisSM->rcvdPSSync
#define RCVD_PSSYNC_PTR sm->thisSM->rcvdPSSyncPtr
#define TX_PSSYNC_PTR_SSS sm->thisSM->txPSSyncPtrSSS
#define SELECTED_STATE sm->ptasg->selectedState
#define GM_PRESENT sm->ptasg->gmPresent
#define PARENT_LOG_SYNC_INTERVAL sm->ptasg->parentLogSyncInterval
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

static site_sync_sync_state_t allstate_condition(site_sync_sync_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ) {
		sm->last_state=REACTION;
		return INITIALIZING;
	}
	return (site_sync_sync_state_t)sm->state;
}

static void *initializing_proc(site_sync_sync_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "site_sync_sync:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	RCVD_PSSYNC = false;
	return NULL;
}

static site_sync_sync_state_t initializing_condition(site_sync_sync_data_t *sm)
{
	if(RCVD_PSSYNC &&
	   (((SELECTED_STATE[RCVD_PSSYNC_PTR->localPortIndex] == (uint8_t)SlavePort) &&
	     GM_PRESENT) || (gptpgcfg_get_intitem(
				     GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_TEST_SYNC_REC_PORT,
				     YDBI_CONFIG) ==
			     RCVD_PSSYNC_PTR->localPortIndex))){
		return RECEIVING_SYNC;
	}
	return INITIALIZING;
}

static PortSyncSync *setPSSyncSend(site_sync_sync_data_t *sm, PortSyncSync *pssptr)
{
	/* 8021AS-2020 10.2.7.2.1 setPSSyncSend
	 * This function create a PortSyncSync structure (sm->portSyncSync) which
	 * will contain the Sync information to be transmitted.
	 * In our interpretation of the standards, the txPSSyncPtrSSS comes from the
	 * received PortSyncSync structure which in this case the same as
	 * rcvdPSSyncPtr */
	memcpy(&sm->portSyncSync, pssptr, sizeof(PortSyncSync));

	sm->site_sync_timeout=RCVD_PSSYNC_PTR->syncReceiptTimeoutTime.nsec;
	sm->site_sync_sendtime=RCVD_PSSYNC_PTR->syncNextSendTimeoutTime.nsec;
	return &sm->portSyncSync;
}

static void *receiving_sync_proc(site_sync_sync_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "site_sync_sync:%s:domainIndex=%d\n", __func__, sm->domainIndex);
	RCVD_PSSYNC = false;
	TX_PSSYNC_PTR_SSS = setPSSyncSend(sm, RCVD_PSSYNC_PTR);
	PARENT_LOG_SYNC_INTERVAL = RCVD_PSSYNC_PTR->logMessageInterval;
	return &sm->portSyncSync;
}

static site_sync_sync_state_t receiving_sync_condition(site_sync_sync_data_t *sm,
						       uint64_t cts64)
{
	if(RCVD_PSSYNC &&
	   (((SELECTED_STATE[RCVD_PSSYNC_PTR->localPortIndex] == (uint8_t)SlavePort) &&
	     GM_PRESENT)  || (gptpgcfg_get_intitem(
				      GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_TEST_SYNC_REC_PORT,
				      YDBI_CONFIG) ==
			      RCVD_PSSYNC_PTR->localPortIndex))){
		sm->last_state=REACTION;
	}

	if(RCVD_PSSYNC &&
	   (SELECTED_STATE[TX_PSSYNC_PTR_SSS->localPortIndex] == (uint8_t)SlavePort) &&
	   GM_PRESENT && (sm->site_sync_timeout && !(cts64 > sm->site_sync_timeout)) &&
	   sm->site_sync_sendtime && (cts64 > sm->site_sync_sendtime)){
		/* Lost Sync message from GM, force reaction and refer to txPSSyncPtrSSS
		 * as reference for sending the Sync and FollowUp messages. */
		UB_LOG(UBL_DEBUGV,"%s:domainIndex=%d, site_sync_sendtime\n",
		       __func__, sm->domainIndex);
		if((RCVD_PSSYNC_PTR->syncReceiptTimeoutTime.nsec == (uint64_t)-1)||
			(sm->site_sync_sendtime>=RCVD_PSSYNC_PTR->syncReceiptTimeoutTime.nsec)){
			/* In the case that a PSSSync is received but not yet stored via
			 * setPSSyncSend, check if it comes from the PortSyncSyncReceiveSM
			 * via checking the value of syncReceiptTimeoutTime.
			 * In case an outstanding PSSSync is available, use it instead of
			 * the previously stored txPSSyncPtr.
			 */
			RCVD_PSSYNC_PTR = TX_PSSYNC_PTR_SSS;
		}
		sm->last_state=REACTION;
	}
	if(sm->site_sync_timeout && (cts64 > sm->site_sync_timeout)){
		sm->site_sync_timeout=0;
		if(gptpgcfg_get_intitem(
			   GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_STATIC_PORT_STATE_SLAVE_PORT,
			   YDBI_CONFIG)>0){
			/*
			'static port config' and 'this is not GM'
			syncReceiptTimeoutTime is not checked in port_announce_information_state
			gmsync should be reset here for a quick response
			*/
			UB_LOG(UBL_DEBUGV,"%s:domainIndex=%d, site_sync_timeout\n",
			       __func__, sm->domainIndex);
			(void)gptpclock_set_gmsync(GPTPINSTNUM, sm->ptasg->domainIndex,
						   GMSYNC_UNSYNC);
		}
	}
	return RECEIVING_SYNC;
}


void *site_sync_sync_sm(site_sync_sync_data_t *sm, uint64_t cts64)
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
			sm->state = INITIALIZING;
			break;
		case INITIALIZING:
			if(state_change){retp=initializing_proc(sm);}
			sm->state = initializing_condition(sm);
			break;
		case RECEIVING_SYNC:
			if(state_change){retp=receiving_sync_proc(sm);}
			sm->state = receiving_sync_condition(sm, cts64);
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

void site_sync_sync_sm_init(site_sync_sync_data_t **sm,
	int domainIndex,
	PerTimeAwareSystemGlobal *ptasg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, domainIndex);
	INIT_SM_DATA(site_sync_sync_data_t, SiteSyncSyncSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->domainIndex = domainIndex;
	(void)site_sync_sync_sm(*sm, 0);
}

int site_sync_sync_sm_close(site_sync_sync_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, (*sm)->domainIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *site_sync_sync_sm_portSyncSync(site_sync_sync_data_t *sm,
				     PortSyncSync *portSyncSync, uint64_t cts64)
{
	RCVD_PSSYNC = true;
	RCVD_PSSYNC_PTR = portSyncSync;
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d, localPortIndex=%d\n", __func__, sm->domainIndex,
	       RCVD_PSSYNC_PTR->localPortIndex);
	return site_sync_sync_sm(sm, cts64);
}
