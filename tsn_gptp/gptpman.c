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
#include <signal.h>
#include <tsn_unibase/unibase.h>
#include <tsn_combase/combase_link.h>
#include "gptpcommon.h"
#include "gptpman_private.h"

extern char *PTPMsgType_debug[16];
extern char *gptpnet_event_debug[8];

#define DOMAIN_DATA_EXIST(di) (((di)>=0) && ((di)<gpmand->max_domains) && gpmand->tasds[di].tasglb)
#define PORT_DATA_EXIST(di,pi) (DOMAIN_DATA_EXIST(di) && ((pi)>=0) && ((pi)<gpmand->max_ports) && \
				gpmand->tasds[di].ptds[pi].ppglb)

UB_SD_GETMEM_DEF(SM_DATA_INST, SM_DATA_FSIZE, SM_DATA_FNUM);
UB_SD_GETMEM_DEF(GPTP_SMALL_ALLOC, GPTP_SMALL_AFSIZE, GPTP_SMALL_AFNUM);
UB_SD_GETMEM_DEF(GPTP_MEDIUM_ALLOC, GPTP_MEDIUM_AFSIZE, GPTP_MEDIUM_AFNUM);

static void set_asCapable(uint8_t gptpInstanceIndex, PerTimeAwareSystemGlobal *tasg,
			  gptpsm_ptd_t *ptd)
{
	if(ptd->mdeglb->forAllDomain->asCapableAcrossDomains ||
	    ptd->ppglb->neighborGptpCapable){
		if(ptd->ppglb->asCapable){return;}
		ptd->ppglb->asCapable = true;
		UB_LOG(UBL_INFO,
		       "set asCapable for domainIndex=%d, portIndex=%d\n",
		       tasg->domainIndex, ptd->ppglb->thisPort);
	}else{
		if(!ptd->ppglb->asCapable){return;}
		ptd->ppglb->asCapable = false;
		UB_LOG(UBL_INFO,
		       "reset asCapable for domainIndex=%d, portIndex=%d\n",
		       tasg->domainIndex, ptd->ppglb->thisPort);
	}

	(void)gptpgcfg_set_asCapable(gptpInstanceIndex, tasg->domainIndex,
				     ptd->ppglb->thisPort, ptd->ppglb->asCapable);
}

static void update_asCapable_for_all(gptpman_data_t *gpmand, int domainIndex,
				     int portIndex)
{
	/* Update asCapable for all domains.
	 * Should be invoked whenever an event received is perceived to set
	 * asCapable for a given port.
	 * For instance:
	 * - receipt of Pdelay_Resp_Fup, ideally case
	 * - receipt of Pdelay_Resp, out-of-order case when Pdelay_Resp_Fup
	 *   is processed first
	 */
	int di = domainIndex;
	gpmand->tasds[di].tasglb->asCapableOrAll=false;
	for(di=0;di<gpmand->max_domains;di++){
		if(!DOMAIN_DATA_EXIST(di)){continue;}
		set_asCapable(gpmand->gptpInstanceIndex, gpmand->tasds[di].tasglb,
			      &gpmand->tasds[di].ptds[portIndex]);
		gpmand->tasds[di].tasglb->asCapableOrAll |=
			gpmand->tasds[di].ptds[portIndex].ppglb->asCapable;
	}
}

static int portSyncSync_for_all(gptpman_data_t *gpmand, int domainIndex,
				void *smret, uint64_t cts64)
{
	int pi;
	void *smretp;
	for(pi=1;pi<gpmand->max_ports;pi++){
		smretp=port_sync_sync_send_sm_portSyncSync(
			gpmand->tasds[domainIndex].ptds[pi].psssendd, (PortSyncSync *)smret, cts64);
		if(smretp!=NULL){
			(void)md_sync_send_sm_mdSyncSend(
				gpmand->tasds[domainIndex].ptds[pi].mdssendd,
				(MDSyncSend *)smretp, cts64);
		}

		// send smret to BMCS PortAnnounceInformation
		port_announce_information_sm_SyncReceiptTimeoutTime(
			gpmand->tasds[domainIndex].ptds[pi].painfd, (PortSyncSync *)smret);
	}

	// send the same smret to ClockSlaveSync
	(void)clock_slave_sync_sm_portSyncSync(
		gpmand->tasds[domainIndex].cssd, (PortSyncSync *)smret, cts64);
	(void)clock_master_sync_receive_sm_ClockSourceReq(
		gpmand->tasds[domainIndex].cmsrecd, cts64);
	(void)clock_master_sync_offset_sm_SyncReceiptTime(
		gpmand->tasds[domainIndex].cmsoffsetd, cts64);

	return 0;
}

static int update_portSyncSync_for_all(gptpman_data_t *gpmand, int di,
				       int portIndex, void *smret, uint64_t cts64)
{
	if(smret!=NULL){
		smret=port_sync_sync_receive_sm_recvMDSync(
			gpmand->tasds[di].ptds[portIndex].pssrecd, (MDSyncReceive *)smret, cts64);
	}
	if(smret!=NULL){
		smret=site_sync_sync_sm_portSyncSync(
			gpmand->tasds[di].sssd, (PortSyncSync *)smret, cts64);
	}
	if(smret!=NULL){
		(void)portSyncSync_for_all(gpmand, di, smret, cts64);
	}
	return 0;
}

static int get_domain_index(gptpman_data_t *gpmand, uint8_t domainIndex)
{
	int i;
	if(domainIndex==0u){return 0;} // 0 is always index 0
	for(i=0;i<gpmand->max_domains;i++){
		if(gpmand->tasds[i].tasglb &&
		   (gpmand->tasds[i].tasglb->domainIndex == domainIndex)){return i;}
	}
	return -1;
}

static int sm_bmcs_perform(gptpman_data_t *gpmand, int domainIndex,
		                   int portIndex, uint64_t cts64)
{
	void *smret;
	gptpsm_tasd_t *tasd = &gpmand->tasds[domainIndex];

	if(tasd->btasglb->externalPortConfiguration==VALUE_DISABLED){
		do{
			(void)port_announce_information_sm(tasd->ptds[portIndex].painfd, cts64);
			smret=port_state_selection_sm(tasd->pssd, cts64);
			if(smret!=NULL){
				gm_stable_gm_change(tasd->gmsd, (uint8_t *)smret, cts64);
				// update clock source after GM change
				(void)clock_master_sync_receive_sm_ClockSourceReq(tasd->cmsrecd, cts64);
			}
		}while(tasd->ptds[portIndex].bppglb->updtInfo);
	}else{ // btasglb->externalPortConfiguration==VALUE_ENABLED
		smret = port_announce_information_ext_sm(tasd->ptds[portIndex].paiextd, cts64);
		if(smret!=NULL){
			port_state_setting_ext_sm_messagePriority(
				tasd->ptds[portIndex].pssextd, (UInteger224 *)smret);
		}
		(void)port_state_setting_ext_sm(tasd->ptds[portIndex].pssextd, cts64);
	}

	smret = port_announce_transmit_sm(tasd->ptds[portIndex].patransd, cts64);
	if(smret!=NULL){
		(void)md_announce_send_sm_mdAnnouncSend(
			tasd->ptds[portIndex].mdansendd, (PTPMsgAnnounce *)smret, cts64);
	}
	return 0;
}

static int sm_bmcs_domain_port_update(gptpman_data_t *gpmand, int domainIndex,
		                              int portIndex, uint64_t cts64)
{
	// update state machine on event occurence
	// ie. portOper = true, asCapable = true
	(void)announce_interval_setting_sm(gpmand->tasds[domainIndex].ptds[portIndex].aisetd, cts64);
	(void)sm_bmcs_perform(gpmand, domainIndex, portIndex, cts64);
	return 0;
}

static int sm_close_for_domain_zero_port(gptpman_data_t *gpmand, int pi)
{
	SM_CLOSE(md_pdelay_resp_sm_close, gpmand->tasds[0].ptds[pi].mdpdrespd);
	SM_CLOSE(md_pdelay_req_sm_close, gpmand->tasds[0].ptds[pi].mdpdreqd);
	SM_CLOSE(link_delay_interval_setting_sm_close, gpmand->tasds[0].ptds[pi].ldisetd);
	return 0;
}

static int sm_close_for_domain_port(gptpman_data_t *gpmand, int di, int pi)
{
	if(!DOMAIN_DATA_EXIST(di) ||
	   ((di!=0) && !gpmand->tasds[di].tasglb->domainIndex)){
		UB_LOG(UBL_DEBUG, "%s:di=%d, pi=%d, not initialized\n", __func__, di, pi);
		return -1;
	}
	if(di==0){(void)sm_close_for_domain_zero_port(gpmand, pi);}
	SM_CLOSE(port_announce_information_ext_sm_close, gpmand->tasds[di].ptds[pi].paiextd);
	SM_CLOSE(port_announce_information_sm_close, gpmand->tasds[di].ptds[pi].painfd);
	if(pi==0){return 0;}
	SM_CLOSE(announce_interval_setting_sm_close, gpmand->tasds[di].ptds[pi].aisetd);
	SM_CLOSE(port_state_setting_ext_sm_close, gpmand->tasds[di].ptds[pi].pssextd);
	SM_CLOSE(port_announce_receive_sm_close, gpmand->tasds[di].ptds[pi].parecd);
	SM_CLOSE(port_announce_transmit_sm_close, gpmand->tasds[di].ptds[pi].patransd);
	SM_CLOSE(md_sync_receive_sm_close, gpmand->tasds[di].ptds[pi].mdsrecd);
	SM_CLOSE(md_sync_send_sm_close, gpmand->tasds[di].ptds[pi].mdssendd);
	SM_CLOSE(port_sync_sync_receive_sm_close, gpmand->tasds[di].ptds[pi].pssrecd);
	SM_CLOSE(port_sync_sync_send_sm_close, gpmand->tasds[di].ptds[pi].psssendd);
	SM_CLOSE(gptp_capable_transmit_sm_close, gpmand->tasds[di].ptds[pi].gctransd);
	SM_CLOSE(gptp_capable_receive_sm_close, gpmand->tasds[di].ptds[pi].gcrecd);
	SM_CLOSE(sync_interval_setting_sm_close, gpmand->tasds[di].ptds[pi].sisetd);
	SM_CLOSE(one_step_tx_oper_setting_sm_close, gpmand->tasds[di].ptds[pi].ostxopd);
	SM_CLOSE(md_announce_send_sm_close, gpmand->tasds[di].ptds[pi].mdansendd);
	SM_CLOSE(md_announce_receive_sm_close, gpmand->tasds[di].ptds[pi].mdanrecd);
	SM_CLOSE(md_signaling_send_sm_close, gpmand->tasds[di].ptds[pi].mdsigsendd);
	SM_CLOSE(md_signaling_receive_sm_close, gpmand->tasds[di].ptds[pi].mdsigrecd);
	return 0;
}

static int gptpnet_cb_devup(gptpman_data_t *gpmand, int portIndex,
			    event_data_netlink_t *ed, uint64_t cts64)
{
	char *dup;
	int di;

	switch(ed->duplex){
	case CB_DUPLEX_FULL: dup="full";
		if(ed->speed<100u){break;}
		gpmand->tasds[0].ptds[portIndex].ppglb->forAllDomain->portOper=true;

		for(di=0;di<gpmand->max_domains;di++){
			if(!DOMAIN_DATA_EXIST(di)){continue;}
			if((di!=0) && !gpmand->tasds[di].tasglb->domainIndex){continue;}
			(void)sm_bmcs_domain_port_update(gpmand, di, portIndex, cts64);
		}

		break;
	case CB_DUPLEX_HALF: dup="half"; break;
	default: dup="unknown"; break;
	}
	UB_LOG(UBL_INFO, "index=%d speed=%d, duplex=%s\n", portIndex, (int)ed->speed, dup);

	if((ed->speed<100u) || (ed->duplex!=1u)){
		UB_LOG(UBL_WARN,"!!! Full duplex link with "
				"Speed above 100 Mbps needed for gptp to run !!!\n");
	}

	return 0;
}

static int gptpnet_cb_devdown(gptpman_data_t *gpmand, int portIndex,
			      event_data_netlink_t *ed, uint64_t cts64)
{
	int di;
	UB_LOG(UBL_INFO, "%s:portIndex=%d\n", __func__, portIndex);

	gpmand->tasds[0].ptds[portIndex].ppglb->forAllDomain->portOper=false;

	gpmand->tasds[0].ptds[portIndex].mdeglb->forAllDomain->asCapableAcrossDomains=false;
	gpmand->tasds[0].ptds[portIndex].ppglb->forAllDomain->receivedNonCMLDSPdelayReq=0;
	for(di=0;di<gpmand->max_domains;di++){
		if(!gpmand->tasds[di].ptds[portIndex].ppglb->asCapable){continue;}
		gpmand->tasds[di].ptds[portIndex].ppglb->asCapable = false;
		(void)gptpgcfg_set_asCapable(gpmand->gptpInstanceIndex,
					     gpmand->tasds[di].tasglb->domainIndex,
					     portIndex, false);
	}
	return 0;
}


// return 1 when TxTS is expected after this call
static int gptpnet_cb_timeout(gptpman_data_t *gpmand, uint64_t cts64)
{
	int pi, di;
	void *smret;

	for(di=0;di<gpmand->max_domains;di++){
		if(!DOMAIN_DATA_EXIST(di)){continue;}

		if(di==0){
			for(pi=1;pi<gpmand->max_ports;pi++){
				(void)md_pdelay_req_sm(gpmand->tasds[di].ptds[pi].mdpdreqd, cts64);
				(void)md_pdelay_resp_sm(gpmand->tasds[di].ptds[pi].mdpdrespd, cts64);
			}
		}
		smret=clock_master_sync_send_sm(gpmand->tasds[di].cmssendd, cts64);
		if(smret!=NULL){
			smret=site_sync_sync_sm_portSyncSync(
				gpmand->tasds[di].sssd, (PortSyncSync *)smret, cts64);
		}
		if(smret!=NULL){(void)portSyncSync_for_all(gpmand, di, smret, cts64);}

		gpmand->tasds[di].tasglb->asCapableOrAll=false;
		for(pi=1;pi<gpmand->max_ports;pi++){
			set_asCapable(gpmand->gptpInstanceIndex, gpmand->tasds[di].tasglb,
				      &gpmand->tasds[di].ptds[pi]);
			gpmand->tasds[di].tasglb->asCapableOrAll |=
				gpmand->tasds[di].ptds[pi].ppglb->asCapable;
			smret=gptp_capable_transmit_sm(gpmand->tasds[di].ptds[pi].gctransd, cts64);
			if(smret!=NULL){
				(void)md_signaling_send_sm_mdSignalingSend(
					gpmand->tasds[di].ptds[pi].mdsigsendd, smret, cts64);
			}

			// The next 4 md_* calls are to check and send defered msg.
			// However, ensure that this are called after the calling
			// set_asCapable above so that no defered messages are not sent
			// after asCapable has been toggled to false already.
			(void)md_sync_send_sm(gpmand->tasds[di].ptds[pi].mdssendd, cts64);
			(void)md_announce_send_sm(gpmand->tasds[di].ptds[pi].mdansendd, cts64);
			(void)md_signaling_send_sm(gpmand->tasds[di].ptds[pi].mdsigsendd, cts64);

			(void)md_sync_receive_sm(gpmand->tasds[di].ptds[pi].mdsrecd, cts64);
			// asCapable might be set, inform other state machines
			if((di!=0) && (!gpmand->tasds[di].tasglb->domainIndex)){continue;}
			(void)sm_bmcs_domain_port_update(gpmand, di, pi, cts64);
			gptp_port_perfmon(gpmand->tasds[di].ptds[pi].ppglb->perfmonDS, di, pi,
				cts64, gpmand->tasds[di].tasglb);
		}
		(void)gm_stable_sm(gpmand->tasds[di].gmsd, cts64);
		gptp_clock_perfmon(gpmand->tasds[di].tasglb->perfmonClockDS, cts64, gpmand->tasds[di].tasglb, di);
	}
	return 0;
}

// return 1 when TxTS is expected after this call
static int gptpnet_cb_recv(gptpman_data_t *gpmand, int portIndex,
			   event_data_recv_t *ed, uint64_t cts64)
{
	int di=0;
	int pi;
	char *msg;
	void *smret;
	uint32_t stype;
	if(ed->domain!=0u){
		/* get domainIndex from  domainIndex */
		di=get_domain_index(gpmand, ed->domain);
		if(di<0) {
			UB_LOG(UBL_DEBUG, "%s:domainIndex=%d is not yet initialized\n",
			       __func__, ed->domain);
			return 0;
		}
	}
	if(ed->msgtype<=15u){
		msg=PTPMsgType_debug[ed->msgtype];
	}else{
		msg="unknown";
	}

	UB_LOG(UBL_DEBUGV, "RECV: pindex=%d msgtype=%s, TS=%"PRIi64"nsec\n",
	       portIndex, msg, ed->ts64);

	switch(ed->msgtype){
	case SYNC:
		/* in a TAS, all the TSs work based on 'thisClock', so when a TS is not
		 * based on 'thisClock', it must be converted.
		 * When this TAS is synced to GM, 'thisClock' has the same freq. rate as GM,
		 * but the phase is different. The phase sync happens in the TAS master clock,
		 * which is gptpclock entity of (ClockIndex=0, tasglb->domainIndex)
		 */
		pi=gptpgcfg_get_intitem(gpmand->gptpInstanceIndex,
					XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE,
					YDBI_CONFIG)?1:portIndex;
		(void)gptpclock_tsconv(gpmand->gptpInstanceIndex, &ed->ts64, pi, 0,
				       gpmand->tasds[di].tasglb->thisClockIndex,
				       gpmand->tasds[di].tasglb->domainIndex);
		smret=md_sync_receive_sm_recv_sync(gpmand->tasds[di].ptds[portIndex].mdsrecd,
						   ed, cts64);
		/* In case sync information becomes available after SYNC receiption,
		 * update and notify portSyncSync.
		 * This may happen when out-of-order SYNC and FOLLOW is occuring.
		*/
		(void)update_portSyncSync_for_all(gpmand, di, portIndex, smret, cts64);
		return 0;
	case PDELAY_REQ:
		if(di!=0){goto not_allowed_domain;}
		pi=gptpgcfg_get_intitem(gpmand->gptpInstanceIndex,
					XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE,
					YDBI_CONFIG)?1:portIndex;
		(void)gptpclock_tsconv(gpmand->gptpInstanceIndex, &ed->ts64, pi, 0,
				       gpmand->tasds[di].tasglb->thisClockIndex,
				       gpmand->tasds[di].tasglb->domainIndex);
		(void)md_pdelay_resp_sm_recv_req(gpmand->tasds[0].ptds[portIndex].mdpdrespd,
						 ed, cts64);
		return 0;
	case PDELAY_RESP:
		if(di!=0){goto not_allowed_domain;}
		pi=gptpgcfg_get_intitem(gpmand->gptpInstanceIndex,
					XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE,
					YDBI_CONFIG)?1:portIndex;
		(void)gptpclock_tsconv(gpmand->gptpInstanceIndex, &ed->ts64, pi, 0,
				       gpmand->tasds[di].tasglb->thisClockIndex,
				       gpmand->tasds[di].tasglb->domainIndex);
		(void)md_pdelay_req_sm_recv_resp(gpmand->tasds[0].ptds[portIndex].mdpdreqd,
						 ed, cts64);
		update_asCapable_for_all(gpmand, di, portIndex);
		return 0;
	case FOLLOW_UP:
		smret=md_sync_receive_sm_recv_fup(
			gpmand->tasds[di].ptds[portIndex].mdsrecd, ed, cts64);
		/* In case sync information becomes available after FOLLOW_UP receiption,
		 * update and notify portSyncSync. This is the usual case.
		 */
		(void)update_portSyncSync_for_all(gpmand, di, portIndex, smret, cts64);
		return 0;
	case PDELAY_RESP_FOLLOW_UP:
		if(di!=0){goto not_allowed_domain;}
		md_pdelay_req_sm_recv_respfup(gpmand->tasds[0].ptds[portIndex].mdpdreqd,
					      ed, cts64);
		update_asCapable_for_all(gpmand, di, portIndex);
		return 0;
	case ANNOUNCE:
		smret=md_announce_receive_sm_mdAnnounceRec(
			gpmand->tasds[0].ptds[portIndex].mdanrecd, ed, cts64);
		if(smret!=NULL){
			port_announce_receive_sm_recv_announce(
				gpmand->tasds[di].ptds[portIndex].parecd,
				(PTPMsgAnnounce *)smret, cts64);
			(void)sm_bmcs_perform(gpmand, di, portIndex, cts64);
		}
		return 0;
	case SIGNALING:
		smret=md_signaling_receive_sm_mdSignalingRec(
			gpmand->tasds[di].ptds[portIndex].mdsigrecd, ed, cts64);
		if(!smret){return 0;}
		stype=((PTPMsgIntervalRequestTLV *)smret)->organizationSubType;
		if(stype==2u){
			sync_interval_setting_SignalingMsg1(
				gpmand->tasds[di].ptds[portIndex].sisetd,
				(PTPMsgIntervalRequestTLV *)smret, cts64);
			link_delay_interval_setting_SignalingMsg1(
				gpmand->tasds[di].ptds[portIndex].ldisetd,
				(PTPMsgIntervalRequestTLV *)smret, cts64);

		}else if(stype==4u){
			gptp_capable_receive_rcvdSignalingMsg(
				gpmand->tasds[di].ptds[portIndex].gcrecd,
				(PTPMsgGPTPCapableTLV *)smret, cts64);
		}else{
			UB_LOG(UBL_WARN,"%s:unknown signaling message, stype=%u\n",
			       __func__, (unsigned int)stype);
		}
		return 0;
	default:
		return 0;
	}
not_allowed_domain:
	UB_LOG(UBL_WARN, "%s:msgtype=%s is not allowed for dominNumber=%d",
	       __func__, msg, ed->domain);
	return 0;
}

static int gptpnet_cb_txts(gptpman_data_t *gpmand, int portIndex,
			   event_data_txts_t *ed, uint64_t cts64)
{
	int di=0;
	char *msg;
	if(ed->domain!=0u){
		/* get domainIndex from  domainIndex */
		di=get_domain_index(gpmand, ed->domain);
		if(di<0) {
			UB_LOG(UBL_WARN, "%s:domainIndex=%d is not yet initialized\n",
			       __func__, ed->domain);
			return 0;
		}
	}
	if(ed->msgtype<=15u){
		msg=PTPMsgType_debug[ed->msgtype];
	}else{
		msg="unknown";
	}

	switch(ed->msgtype){
	case SYNC:
		md_sync_send_sm_txts(gpmand->tasds[di].ptds[portIndex].mdssendd,
				     ed, cts64);
		return 0;
	case PDELAY_REQ:
		if(di!=0){goto not_allowed_domain;}
		md_pdelay_req_sm_txts(gpmand->tasds[0].ptds[portIndex].mdpdreqd,
				      ed, cts64);
		return 0;
	case PDELAY_RESP:
		if(di!=0){goto not_allowed_domain;}
		md_pdelay_resp_sm_txts(gpmand->tasds[0].ptds[portIndex].mdpdrespd,
				       ed, cts64);
		return 0;
	case FOLLOW_UP:
	case PDELAY_RESP_FOLLOW_UP:
	case ANNOUNCE:
	default:
		UB_LOG(UBL_WARN, "%s: msgtype=%s shouldn't have TxTS\n", __func__, msg);
		return 0;
	}
not_allowed_domain:
	UB_LOG(UBL_WARN, "%s:msgtype=%s is not allowed for dominNumber=%d",
	       __func__, msg, ed->domain);
	return 0;
}


static int gptpnet_cb(void *cb_data, int portIndex, gptpnet_event_t event,
		      int64_t *event_ts64, void *event_data)
{
	gptpman_data_t *gpmand=(gptpman_data_t*)cb_data;
	uint64_t cts64=*event_ts64;
	int res=0;

	UB_TLOG(UBL_DEBUGV, "%s:index=%d event=%s\n",
		__func__, portIndex, gptpnet_event_debug[event]);
	cts64 = GPTP_ALIGN_TIME(cts64);
	switch(event){
	case GPTPNET_EVENT_NONE:
		break;
	case GPTPNET_EVENT_TIMEOUT:
		(void)gptpnet_cb_timeout(gpmand, cts64);
		break;
	case GPTPNET_EVENT_DEVUP:
		res = gptpnet_cb_devup(gpmand, portIndex,
					(event_data_netlink_t *)event_data, cts64);
		break;
	case GPTPNET_EVENT_DEVDOWN:
		res = gptpnet_cb_devdown(gpmand, portIndex,
					  (event_data_netlink_t *)event_data, cts64);
		break;
	case GPTPNET_EVENT_TXTS:
		res = gptpnet_cb_txts(gpmand, portIndex,
				       (event_data_txts_t *)event_data, cts64);
		break;
	case GPTPNET_EVENT_RECV:
		res = gptpnet_cb_recv(gpmand, portIndex,
				       (event_data_recv_t *)event_data, cts64);
		break;
	case GPTPNET_EVENT_GUARDUP:
		gpmand->tasds[0].ptds[portIndex].ppglb->forAllDomain->portOper=true;
		break;
	case GPTPNET_EVENT_GUARDDOWN:
		gpmand->tasds[0].ptds[portIndex].ppglb->forAllDomain->portOper=false;
		break;
	default:
		break;
	}
	ub_log_flush();
	if(res!=0){return res;}
	return 0;
}

static int domain_zero_port_sm_init(gptpsm_tasd_t *tasd, gptpnet_data_t *gpnetd, int pi)
{
	md_pdelay_req_sm_init(&tasd->ptds[pi].mdpdreqd, pi, gpnetd,
			      tasd->tasglb, tasd->ptds[pi].ppglb, tasd->ptds[pi].mdeglb);
	md_pdelay_resp_sm_init(&tasd->ptds[pi].mdpdrespd, pi, gpnetd,
			       tasd->tasglb, tasd->ptds[pi].ppglb);
	link_delay_interval_setting_sm_init(&tasd->ptds[pi].ldisetd, pi,
					    tasd->tasglb, tasd->ptds[pi].ppglb,
					    tasd->ptds[pi].mdeglb);
	return 0;
}

static int domain_port_sm_init(gptpsm_tasd_t *tasd, gptpnet_data_t *gpnetd, int di, int pi)
{
	port_announce_information_sm_init(&tasd->ptds[pi].painfd, di, pi,
			tasd->tasglb, tasd->ptds[pi].ppglb,
			tasd->btasglb, tasd->ptds[pi].bppglb);
	port_announce_information_ext_sm_init(&tasd->ptds[pi].paiextd, di, pi,
			tasd->tasglb, tasd->ptds[pi].ppglb,
			tasd->btasglb, tasd->ptds[pi].bppglb);
	if(pi==0){return 0;}
	md_sync_receive_sm_init(&tasd->ptds[pi].mdsrecd,di, pi,	tasd->tasglb,
			tasd->ptds[pi].ppglb, tasd->ptds[pi].mdeglb);
	port_sync_sync_receive_sm_init(&tasd->ptds[pi].pssrecd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	port_sync_sync_send_sm_init(&tasd->ptds[pi].psssendd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	md_sync_send_sm_init(&tasd->ptds[pi].mdssendd, di, pi, gpnetd, tasd->tasglb,
			tasd->ptds[pi].ppglb, tasd->ptds[pi].mdeglb);
	port_announce_receive_sm_init(&tasd->ptds[pi].parecd, di, pi,
			tasd->tasglb, tasd->ptds[pi].ppglb,
			tasd->btasglb, tasd->ptds[pi].bppglb);
	port_announce_transmit_sm_init(&tasd->ptds[pi].patransd, di, pi,
			tasd->tasglb, tasd->ptds[pi].ppglb,
			tasd->btasglb, tasd->ptds[pi].bppglb);
	gptp_capable_transmit_sm_init(&tasd->ptds[pi].gctransd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	gptp_capable_receive_sm_init(&tasd->ptds[pi].gcrecd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	sync_interval_setting_sm_init(&tasd->ptds[pi].sisetd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	announce_interval_setting_sm_init(&tasd->ptds[pi].aisetd, di, pi,
			tasd->tasglb, tasd->ptds[pi].ppglb,
			tasd->ptds[pi].bppglb);
	one_step_tx_oper_setting_sm_init(&tasd->ptds[pi].ostxopd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb, tasd->ptds[pi].mdeglb);
	md_announce_send_sm_init(&tasd->ptds[pi].mdansendd, di, pi, gpnetd, tasd->tasglb,
			tasd->ptds[pi].ppglb, tasd->ptds[pi].bppglb);
	md_announce_receive_sm_init(&tasd->ptds[pi].mdanrecd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	md_signaling_send_sm_init(&tasd->ptds[pi].mdsigsendd, di, pi, gpnetd, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	md_signaling_receive_sm_init(&tasd->ptds[pi].mdsigrecd, di, pi, tasd->tasglb,
			tasd->ptds[pi].ppglb);
	return 0;
}

static int gptpman_port_init(gptpman_data_t *gpmand, uint8_t di, int pi)
{
	gptpsm_tasd_t *tasd=&gpmand->tasds[di];
	if(di==0u){
		md_entity_glb_init(gpmand->gptpInstanceIndex, &tasd->ptds[pi].mdeglb, NULL, pi);
		pp_glb_init(gpmand->gptpInstanceIndex, &tasd->ptds[pi].ppglb, NULL, tasd->tasglb, di, pi);
	}else{
		md_entity_glb_init(gpmand->gptpInstanceIndex, &tasd->ptds[pi].mdeglb,
				   gpmand->tasds[0].ptds[pi].mdeglb->forAllDomain, pi);
		pp_glb_init(gpmand->gptpInstanceIndex, &tasd->ptds[pi].ppglb,
			    gpmand->tasds[0].ptds[pi].ppglb->forAllDomain, tasd->tasglb, di, pi);
	}
	bmcs_pp_glb_init(gpmand->gptpInstanceIndex, &tasd->ptds[pi].bppglb, di, pi);
	if(di==0u){
		(void)domain_zero_port_sm_init(tasd, gpmand->gpnetd, pi);
	}
	(void)domain_port_sm_init(tasd, gpmand->gpnetd, di, pi);
	if(pi!=0){
		port_state_setting_ext_sm_init(&gpmand->tasds[di].ptds[pi].pssextd, di, pi,
					       gpmand->tasds[di].tasglb, gpmand->tasds[di].ppglbl,
					       gpmand->tasds[di].btasglb, gpmand->tasds[di].bppglbl,
					       gpmand->max_ports,
					       &gpmand->tasds[0].ptds[pi].pssextd);
	}

	tasd->ptds[pi].cmldsLinkPortEnabled = true;
	tasd->ppglbl[pi] = tasd->ptds[pi].ppglb;
	tasd->bppglbl[pi] = tasd->ptds[pi].bppglb;
	if(ub_fatalerror()){return -1;}
	return 0;
}

/* allocate data in gptpsm_tasd_t */
int gptpman_domain_init(gptpman_data_t *gpmand, uint8_t domainIndex)
{
	int di;
	int pi;

	if(domainIndex==0u){
		di=0;
	}else{
		for(di=1;di<gpmand->max_domains;di++){
			if(gpmand->tasds[di].tasglb &&
			   (gpmand->tasds[di].tasglb->domainIndex == domainIndex)){
				UB_LOG(UBL_ERROR,"%s:data of domainIndex=%d already exists\n",
				       __func__, domainIndex);
				return -1;
			}
			if(!gpmand->tasds[di].tasglb){break;}
		}
	}
	if(di>=gpmand->max_domains){
		UB_LOG(UBL_ERROR,"%s:no space for a new domain\n", __func__);
		return -1;
	}
	ptas_glb_init(&gpmand->tasds[di].tasglb, gpmand->gptpInstanceIndex,
		      di);

	bmcs_ptas_glb_init(gpmand->gptpInstanceIndex,
			   &gpmand->tasds[di].btasglb, gpmand->tasds[di].tasglb, di);

	site_sync_sync_sm_init(&gpmand->tasds[di].sssd, di, gpmand->tasds[di].tasglb);
	clock_master_sync_send_sm_init(&gpmand->tasds[di].cmssendd, di, gpmand->tasds[di].tasglb);
	clock_slave_sync_sm_init(&gpmand->tasds[di].cssd, di, gpmand->tasds[di].tasglb);
	clock_master_sync_receive_sm_init(&gpmand->tasds[di].cmsrecd, di,
					  gpmand->tasds[di].tasglb);
	clock_master_sync_offset_sm_init(&gpmand->tasds[di].cmsoffsetd, di,
					 gpmand->tasds[di].tasglb);

	for(pi=0;pi<gpmand->max_ports;pi++){
		if(gptpman_port_init(gpmand, di, pi)!=0){return -1;}
	}

	gm_stable_sm_init(&gpmand->tasds[di].gmsd, di, gpmand->tasds[di].tasglb);
	port_state_selection_sm_init(&gpmand->tasds[di].pssd, di,
			gpmand->tasds[di].tasglb, gpmand->tasds[di].ppglbl,
			gpmand->tasds[di].btasglb, gpmand->tasds[di].bppglbl,
			gpmand->max_ports,
			&gpmand->tasds[0].pssd);
	return 0;
}

static void free_tasds(gptpman_data_t *gpmand)
{
	int di;
	if(gpmand==NULL) {return;}
	if(gpmand->tasds==NULL) {return;}
	for(di=0;di<gpmand->max_domains;di++){
		if(gpmand->tasds[di].ptds!=NULL){
			UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpmand->tasds[di].ptds);
			gpmand->tasds[di].ptds=NULL;
		}
		if(gpmand->tasds[di].ppglbl!=NULL){
			UB_SD_RELMEM(GPTP_SMALL_ALLOC, gpmand->tasds[di].ppglbl);
			gpmand->tasds[di].ppglbl=NULL;
		}
		if(gpmand->tasds[di].bppglbl!=NULL){
			UB_SD_RELMEM(GPTP_SMALL_ALLOC, gpmand->tasds[di].bppglbl);
			gpmand->tasds[di].bppglbl=NULL;
		}
	}
}

static int all_sm_close(gptpman_data_t *gpmand)
{
	int di, pi;
	for(di=0;di<gpmand->max_domains;di++){
		for(pi=0;pi<gpmand->max_ports;pi++){
			if(gpmand->tasds[di].ptds && gpmand->tasds[di].ptds[pi].bppglb){
				bmcs_pp_glb_close(&gpmand->tasds[di].ptds[pi].bppglb);
				pp_glb_close(&gpmand->tasds[di].ptds[pi].ppglb, di);
				md_entity_glb_close(&gpmand->tasds[di].ptds[pi].mdeglb, di);
			}
			(void)sm_close_for_domain_port(gpmand, di, pi);
		}
		SM_CLOSE(site_sync_sync_sm_close, gpmand->tasds[di].sssd);
		SM_CLOSE(clock_master_sync_send_sm_close, gpmand->tasds[di].cmssendd);
		SM_CLOSE(clock_slave_sync_sm_close, gpmand->tasds[di].cssd);
		SM_CLOSE(clock_master_sync_receive_sm_close, gpmand->tasds[di].cmsrecd);
		SM_CLOSE(clock_master_sync_offset_sm_close, gpmand->tasds[di].cmsoffsetd);
		SM_CLOSE(gm_stable_sm_close, gpmand->tasds[di].gmsd);
		SM_CLOSE(port_state_selection_sm_close, gpmand->tasds[di].pssd);
		ptas_glb_close(&gpmand->tasds[di].tasglb);
		bmcs_ptas_glb_close(&gpmand->tasds[di].btasglb);
	}
	free_tasds(gpmand);
	return 0;
}

static int gptpman_close(gptpman_data_t *gpmand)
{
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpmand->tasds);
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpmand);
	return 0;
}

static int init_domain_clock(gptpman_data_t *gpmand, int domainIndex, int thisClockIndex)
{
	int i;
	struct timespec ts;
	char *ptpdev;
	ClockIdentity clkid;
	int num_clocks;
	// clocks are added for the domain
	(void)memset(&ts, 0, sizeof(ts));
	num_clocks=gptpgcfg_get_intitem(
		gpmand->gptpInstanceIndex,
		XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE,
		YDBI_CONFIG)?2:gpmand->max_ports;
	for(i=1;i<num_clocks;i++){
		// for domainIndex!=0, add only thisClock
		if(domainIndex && (i!=thisClockIndex)){continue;}
		// for domainIndex==0, add clocks for all ports
		ptpdev=gptpnet_ptpdev(gpmand->gpnetd, i-1);
		// actually every netdevice has assigned ptpdevice, done in gptpnet_init
		if(!ptpdev[0]){continue;}
		gptpnet_create_clockid(gpmand->gpnetd, clkid, i-1, domainIndex);
		if(gptpclock_add_clock(gpmand->gptpInstanceIndex,
				       i, ptpdev, domainIndex, clkid)!=0){
			UB_LOG(UBL_ERROR,"%s:clock can't be added, ptpdev=%s\n",
			       __func__, ptpdev);
			return -1;
		}
	}

	/* add the master clock for the domainIndex.
	   get ptpdev from the network device of thisClockIndex. */
	gptpnet_create_clockid(gpmand->gpnetd, clkid, thisClockIndex-1, domainIndex);

	ptpdev=gptpnet_ptpdev(gpmand->gpnetd, thisClockIndex-1);
	if(gptpclock_add_clock(gpmand->gptpInstanceIndex,
			       0, ptpdev, domainIndex, clkid)!=0){
		UB_LOG(UBL_ERROR,"%s:master clock can't be added, ptpdev=%s\n",
		       __func__, ptpdev);
		return -1;
	}
	return 0;
}

static int set_domain_thisClock(PerTimeAwareSystemGlobal *tasglb, int domainIndex,
				int thisClockIndex)
{
	uint8_t *clockid;
	tasglb->thisClockIndex=thisClockIndex;
	(void)gptpclock_set_thisClock(tasglb->gptpInstanceIndex,
				tasglb->thisClockIndex, domainIndex, false);

	/* initialize adjustment rate of the master clock to 0 */
	(void)gptpclock_setadj(tasglb->gptpInstanceIndex, 0, thisClockIndex, domainIndex);

	clockid=gptpclock_clockid(tasglb->gptpInstanceIndex,
				  tasglb->thisClockIndex, domainIndex);
	if(!clockid){return -1;}
	memcpy(tasglb->thisClock, clockid, sizeof(ClockIdentity));
	memcpy(tasglb->gmIdentity, clockid, sizeof(ClockIdentity));
	return 0;
}

static int static_domains_init(gptpman_data_t *gpmand, uint8_t gptpInstanceIndex, char *inittm)
{
	/* for domainIndex=0 , initialize here.
	   For other domains, it will be done at a receive
	   or an action to create a new domain */
	int this_ci;
	int di;

	di=0; // domainIndex=0
	this_ci=gptpgcfg_get_intitem(
		gpmand->gptpInstanceIndex,
		XL4_EXTMOD_XL4GPTP_FIRST_DOMAIN_THIS_CLOCK, YDBI_CONFIG);
	if(gptpman_domain_init(gpmand, di)!=0){return -1;}
	if(init_domain_clock(gpmand, di, this_ci)!=0){
		UB_LOG(UBL_ERROR,"%s:domain clock init failed, di=%d\n",
		       __func__, di);
		return -1;
	}
	(void)set_domain_thisClock(gpmand->tasds[di].tasglb, di, this_ci);
	if(inittm!=NULL){
		(void)gptpclock_setadj(gpmand->gptpInstanceIndex,
				 0, this_ci, di); // Freq. adj=0
		(void)gptpclock_settime_str(gpmand->gptpInstanceIndex,
				      inittm, 0, di); // Phase set to inittm
	}
	bmcs_ptas_glb_update(gpmand->gptpInstanceIndex,
			     &gpmand->tasds[di].btasglb,
			     gpmand->tasds[di].tasglb, di);

	di=gptpgcfg_get_intitem(
		gpmand->gptpInstanceIndex,
		XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_NUMBER, YDBI_CONFIG);
	di=md_domain_number2index(di);
	this_ci=gptpgcfg_get_intitem(
		gpmand->gptpInstanceIndex,
		XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_THIS_CLOCK, YDBI_CONFIG);
	if(this_ci<0){return 0;}
	if(gptpman_domain_init(gpmand, di)!=0){return -1;}
	if(init_domain_clock(gpmand, di, this_ci)!=0){
		UB_LOG(UBL_ERROR,"%s:domain clock init failed, di=%d\n",
		       __func__, di);
		return -1;
	}
	(void)set_domain_thisClock(gpmand->tasds[di].tasglb, di, this_ci);
	if(inittm!=NULL){
		(void)gptpclock_setadj(gpmand->gptpInstanceIndex,
				 0, this_ci, di); // Freq. adj=0
		(void)gptpclock_settime_str(
			gpmand->gptpInstanceIndex, inittm, 0, di); // Phase set to inittm
	}
	bmcs_ptas_glb_update(gpmand->gptpInstanceIndex,
			     &gpmand->tasds[di].btasglb,
			     gpmand->tasds[di].tasglb, di);

	return 0;
}

int gptpman_run(uint8_t gptpInstanceIndex, const char *netdevs[],
		int max_ports, char *inittm, bool *stopgptp)
{
	int i;
	gptpman_data_t *gpmand;
	int res=-1;
	char *masterptpdev=NULL;
	void *value;
	uint32_t vsize;
	int max_domains;

	if(max_ports==0) return -1;
	gpmand=(gptpman_data_t *)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(gptpman_data_t));
	if(ub_assert_fatal(gpmand!=NULL, __func__, "malloc error")) {goto erexit;}
	(void)memset(gpmand, 0, sizeof(gptpman_data_t));
	gpmand->gptpInstanceIndex=gptpInstanceIndex;

	// provide seed to pseudo random generator rand()
	srand(time(NULL));

	max_domains=gptpgcfg_get_intitem(
		gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_MAX_DOMAIN_NUMBER,
		YDBI_CONFIG);
	gpmand->max_domains=max_domains;
	UB_LOG(UBL_INFO, "%s:max_domains=%d, max_ports=%d\n", __func__,
	       gpmand->max_domains, max_ports);
	// add one on max_ports for clockIndex=0
	if(gptpclock_init(gptpInstanceIndex, gpmand->max_domains, max_ports+1)){
		goto erexit;
	}

	/* a network device which has master ptpdev becomes the first device,
	   so that clockIndex=1 is safe to use for thisClockIndex */
	YDBI_GET_ITEM_PSUBST(nyptk1vk0, masterptpdev, vsize, value,
			     gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_MASTER_PTPDEV, YDBI_CONFIG);
	gpmand->gpnetd=gptpnet_init(gptpInstanceIndex, gptpnet_cb,
				    gpmand, netdevs, max_ports, masterptpdev);
	YDBI_REL_ITEM(nyptk1vk0, gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_MASTER_PTPDEV, YDBI_CONFIG);
	if(!gpmand->gpnetd){goto erexit;}
	// increment max_ports to add the ClockMaster port as port=0
	if(++max_ports>=XL4_DATA_ABS_MAX_NETDEVS){
		UB_LOG(UBL_ERROR, "%s:max_ports number is too big, must be less than %d\n",
		       __func__,XL4_DATA_ABS_MAX_NETDEVS);
		goto erexit;
	}

	gpmand->max_ports=max_ports;

	gpmand->tasds=(gptpsm_tasd_t*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC,
						   (unsigned int)max_domains * sizeof(gptpsm_tasd_t));
	if(ub_assert_fatal(gpmand->tasds!=NULL, __func__, "malloc error")) {goto erexit;}
	(void)memset(gpmand->tasds, 0, (unsigned int)max_domains * sizeof(gptpsm_tasd_t));

	// reserve ptds(port data array) for the number of domain
	for(i=0;i<max_domains;i++){
		gpmand->tasds[i].ptds=(gptpsm_ptd_t*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC,
						   (unsigned int)max_ports * sizeof(gptpsm_ptd_t));
		if(ub_assert_fatal(gpmand->tasds[i].ptds!=NULL, __func__, "malloc")){break;}
		(void)memset(gpmand->tasds[i].ptds, 0, (unsigned int)max_ports * sizeof(gptpsm_ptd_t));
		// create per-port global lists
		gpmand->tasds[i].ppglbl=(PerPortGlobal**)UB_SD_GETMEM(GPTP_SMALL_ALLOC,
						     (unsigned int)max_ports * sizeof(PerPortGlobal*));
		if(ub_assert_fatal(gpmand->tasds[i].ppglbl!=NULL, __func__, "malloc")){break;}
		(void)memset(gpmand->tasds[i].ppglbl, 0,
			     (unsigned int)max_ports * sizeof(PerPortGlobal*));
		gpmand->tasds[i].bppglbl=(BmcsPerPortGlobal**)UB_SD_GETMEM(GPTP_SMALL_ALLOC,
						      (unsigned int)max_ports * sizeof(BmcsPerPortGlobal*));
		if(ub_assert_fatal(gpmand->tasds[i].bppglbl!=NULL, __func__, "malloc")){break;}
		(void)memset(gpmand->tasds[i].bppglbl, 0,
			     (unsigned int)max_ports * sizeof(BmcsPerPortGlobal*));
	}

	if(static_domains_init(gpmand, gptpInstanceIndex, inittm)!=0){goto erexit;}

	if(gptpnet_activate(gpmand->gpnetd)!=0){goto erexit;}
	if(gptpgcfg_get_intitem(gptpInstanceIndex,
				XL4_EXTMOD_XL4GPTP_ACTIVATE_ABNORMAL_HOOKS,
				YDBI_CONFIG)!=0){md_abnormal_init();}
	GPTP_READY_NOTICE;
	UB_SD_PRINT_USAGE(GPTP_MEDIUM_ALLOC, UBL_INFO);
	UB_SD_PRINT_USAGE(GPTP_SMALL_ALLOC, UBL_INFO);
	UB_SD_PRINT_USAGE(SM_DATA_INST, UBL_INFO);
	UB_LOG(UBL_INFO, "%s:GPTPNET_INTERVAL_TIMEOUT_NSEC=%d\n",
		   __func__,(int)GPTPNET_INTERVAL_TIMEOUT_NSEC);
	res=gptpnet_eventloop(gpmand->gpnetd, stopgptp);
	(void)all_sm_close(gpmand);
	md_abnormal_close();
erexit:
	if(gpmand->gpnetd!=NULL){(void)gptpnet_close(gpmand->gpnetd);}
	gptpclock_close(gpmand->gptpInstanceIndex);
	(void)gptpman_close(gpmand);
	return res;
}
