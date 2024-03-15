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
/*
 * avtpc_crf.c
 * AVB Transport Protocol CRF Client Process
 *
 */
#include "avtpc.h"
#include "avtpc_crf.h"
#include "avtpc_private.h"

#define VLAN_ID_MAX 4095

typedef enum {
	MR_STABLE_STATE,
	MR_WAITING_STATE //waiting to toggle 'mr' bit
} mr_state_t;

typedef struct mr_state_machine {
	uint16_t pdus_toggle;
	uint16_t pdus_count;
	mr_state_t state;
} mr_state_machine_t;

struct avtpc_crf_data {
	avtpc_data_t *avtpc;
	mr_state_machine_t mr_sm;
};

static void init_mr_sm(mr_state_machine_t *mr_sm)
{
	memset(mr_sm, 0, sizeof(*mr_sm));
	mr_sm->state = MR_STABLE_STATE;
}

/*
 * This state machine implements the 1722 requirement:
 * "Once this bit toggles, it shall remain in its new state for a
 * minimum of eight (8) CRF AVTPDUs for a given continuous stream."
*/
static bool do_mr_sm(mr_state_machine_t *sm, bool mr)
{
	bool toggle = false;

	if (sm->pdus_count <= sm->pdus_toggle){sm->pdus_count++;}

	if (mr) {
		if (sm->pdus_count > sm->pdus_toggle) {
			sm->state = MR_STABLE_STATE;
			sm->pdus_count = 1;
			toggle = true;
		} else {
			if (sm->state != MR_WAITING_STATE){sm->state = MR_WAITING_STATE;}
		}
	} else {
		if (sm->state == MR_WAITING_STATE
			&& sm->pdus_count > sm->pdus_toggle) {
			sm->state = MR_STABLE_STATE;
			sm->pdus_count = 1;
			toggle = true;
		}
	}

	return toggle;
}

avtpc_crf_data_t *avtpc_crf_init(char *shsuf)
{
	avtpc_crf_data_t *avtpc_crf;

	avtpc_crf = (avtpc_crf_data_t *)malloc(sizeof(avtpc_crf_data_t));
	if(ub_assert_fatal(avtpc_crf!=NULL, __func__, NULL)) return NULL;
	memset(avtpc_crf, 0, sizeof(avtpc_crf_data_t));

	init_mr_sm(&avtpc_crf->mr_sm);

	avtpc_crf->avtpc = avtpc_init(shsuf);
	if (!avtpc_crf->avtpc) {
		avtpc_crf_close(avtpc_crf);
		return NULL;
	}

	return avtpc_crf;
}

int avtpc_crf_connection_request(avtpc_crf_data_t *avtpc_crf,
			client_connect_request_t *ccr)
{
	if(ccr->con_mode != AVTP_CRF_TALKER_CONNECTION &&
	   ccr->con_mode != AVTP_CRF_LISTENER_CONNECTION) {
		UB_LOG(UBL_ERROR,"%s: not CRF connection\n",__func__);
		return -1;
	}
	if (!avtpc_crf->avtpc) {
		UB_LOG(UBL_ERROR,"%s: No avtpc object\n",__func__);
		return -1;
	}
	return avtpc_connection_request(avtpc_crf->avtpc, ccr);
}

int avtpc_crf_write_packet(avtpc_crf_data_t *avtpc_crf,
		avtpc_crf_control_t *control, uint8_t *crf_data, uint32_t crf_data_length)
{
	struct timespec ts;
	uint64_t ts64;
	ccdbuf_data_t *cdata;
	avbtp_crf_header_t *crfh;
	avtpc_data_t *avtpc;
	bool mr_toggle = false;

	if (!avtpc_crf || !avtpc_crf->avtpc || !crf_data || !crf_data_length) {
		UB_LOG(UBL_ERROR,"%s: invalid args\n",__func__);
		return -1;
	}

	avtpc = avtpc_crf->avtpc;

	if(avtpc->ccds->ccr.con_mode!=AVTP_CRF_TALKER_CONNECTION &&
	   avtpc->ccds->ccr.con_mode!=AVTP_CRF_LISTENER_CONNECTION){
		UB_LOG(UBL_ERROR,"%s: not CRF connection\n",__func__);
		return -1;
	}

	if(crf_data_length > avtpc->ccds->ccr.max_frame_size -
	   CCDBUFF_PAYLOAD_OFFSET - sizeof(avbtp_crf_header_t)){
		UB_LOG(UBL_ERROR,"%s: size=%"PRIu32" is too big\n",__func__, crf_data_length);
		return -1;
	}

	if(!UB_NON_ZERO_B8(avtpc->ccds->ccr.ht.preset_crfh.stream_id)){
		UB_LOG(UBL_ERROR,"%s:steram_id is all zero\n",__func__);
		return -1;
	}

	while(avtpc->ccds->bufused==avtpc->ccds->bufnum){
		ts64=ub_rt_gettime64();
		UB_NSEC2TS(ts64+WRITE_PACKET_TIMEOUT*1000, ts);
		if(CB_SEM_TIMEDWAIT(&avtpc->ccds->tsem_rel, &ts)) {
			UB_LOG(UBL_ERROR,"%s:timedout, streamid=0x%02hhX%02hhX\n", __func__,
			       avtpc->ccds->ccr.ht.preset_crfh.stream_id[6],
			       avtpc->ccds->ccr.ht.preset_crfh.stream_id[7]);
			return -1;
		}
	}

	CB_THREAD_MUTEX_LOCK(&avtpc->ccds->tmutex);
	cdata = (ccdbuf_data_t *)(avtpc->ccds->buffer +
				  (avtpc->ccds->ccr.max_frame_size +
				   sizeof(ccdbuf_data_info_t)) *
				  avtpc->ccds->writep);
	cdata->info.size=crf_data_length+sizeof(avbtp_crf_header_t);
	crfh=(avbtp_crf_header_t *)cdata->payload;
	cdata->info.timestamp=0;

	mr_toggle = do_mr_sm(&avtpc_crf->mr_sm,
					(control && control->media_restart));
	if (mr_toggle) {
		if (avtpc->ccds->ccr.ht.preset_crfh.hh.bf0 & (uint8_t)(1 << 3)){avtpc->ccds->ccr.ht.preset_crfh.hh.bf0 &= ~(uint8_t)(1 << 3);}
		else{avtpc->ccds->ccr.ht.preset_crfh.hh.bf0 |= (uint8_t)(1 << 3);}
	}
	if (control && control->frame_sync){avtpc->ccds->ccr.ht.preset_crfh.hh.bf0 |= (uint8_t)(1 << 1);}
	else{avtpc->ccds->ccr.ht.preset_crfh.hh.bf0 &= ~(uint8_t)(1 << 1);}

	avbtp_generate_crf_header(crfh, &avtpc->ccds->ccr.ht.preset_crfh,
							  crf_data_length);
	memcpy(cdata->payload+sizeof(avbtp_crf_header_t), crf_data, crf_data_length);

	if(++avtpc->ccds->writep==avtpc->ccds->bufnum){avtpc->ccds->writep=0;}

	avtpc->ccds->bufused++;
	if(avtpc->ccds->bufused==avtpc->ccds->bufnum) {
		while(CB_SEM_TRYWAIT(&avtpc->ccds->tsem_rel)==0){;} //set semaphore to zero
	}
	CB_THREAD_MUTEX_UNLOCK(&avtpc->ccds->tmutex);
	CB_SEM_POST(&avtpc->ccds->tsem_set);

	return 0;
}

int avtpc_crf_set_ccr(avtpc_crf_data_t *avtpc_crf,
		avtpc_crf_config_t *crfcfg, client_connect_request_t *ccr)
{
	ub_streamid_t stream_id;

	if (!avtpc_crf || !crfcfg || !ccr) {
		UB_LOG(UBL_ERROR,"%s: invalid args\n",__func__);
		return -1;
	}

	if (crfcfg->vlan_id > VLAN_ID_MAX) {
		UB_LOG(UBL_ERROR,"%s: invalid vlan_id %d\n",
			   __func__, crfcfg->vlan_id);
		return -1;
	}

	UB_LOG(UBL_INFO,"%s: dev=%s vid=%d pcp=%d listener=%d dest_mcast=%s\n",
		   __func__, crfcfg->netdev, crfcfg->vlan_id, crfcfg->vlan_pcp,
		   crfcfg->listener, crfcfg->dest_mcast);

	/* Using UB_CONSOLE_PRINT() is intended to show that the following logs are
	 * connected to the previous one without adding the log header for each line.
	 * Do not change this code because it is needed for platform using "\r\n"
	 * as the break line. */
	if(ub_clog_on(UB_LOGCAT, UBL_INFO)){
		UB_CONSOLE_PRINT("\tjoin_mcast=%s stream=%s\n",
						 crfcfg->join_mcast, crfcfg->stream_id);
		UB_CONSOLE_PRINT("\ttype=%d pull=%d base_freq=%"PRIu32
						 " tsintv=%d mr_pdus=%d\n",
						 crfcfg->type, crfcfg->pull, crfcfg->base_frequency,
						 crfcfg->timestamp_interval, crfcfg->mr_pdus_toggle);
	};

	memset(ccr, 0, sizeof(client_connect_request_t));
	if(crfcfg->listener){
		ccr->con_mode=AVTP_CRF_LISTENER_CONNECTION;
	}else{
		ccr->con_mode=AVTP_CRF_TALKER_CONNECTION;
	}
	strcpy(ccr->netdev, crfcfg->netdev);

	ccr->max_frame_size=IEEE1722_DEFAULT_MAX_FRAME_SIZE;
	ccr->max_intv_frames=1;
	ccr->time_intv=1000;
	if (crfcfg->vlan_id >= 0) {
		ccr->priority=crfcfg->vlan_pcp;
		ccr->vlanid=crfcfg->vlan_id;
		ccr->tagged=true;
	}
	ccr->rank=1;
	ccr->rec_tagged=VLAN_REC_TAGGED;
	ccr->lsem_post_time=0; //receive immediately with a single packet
	ub_ssid2bsid(crfcfg->stream_id, stream_id);
	ub_smac2bmac(crfcfg->dest_mcast, ccr->mcast_addr);
	ub_smac2bmac(crfcfg->join_mcast, ccr->join_mcast);
	/*
	 * Stream id and stream valid are the same position as common stream header.
	 * We will reuse it.
	 */
	avbtp_set_stream_id((avbtp_cm_stream_header_t *)&ccr->ht.preset_crfh, stream_id);
	ccr->ht.preset_crfh.hh.subtype = AVBTP_SUBTYPE_CRF;

	if (crfcfg->listener == false) {
		uint32_t bf1_host;
		bf1_host = ((uint32_t)crfcfg->pull & 0x07) << 29;
		bf1_host |= crfcfg->base_frequency & 0x1FFFFFFF;
		ccr->ht.preset_crfh.bf1 = htonl(bf1_host);
		ccr->ht.preset_crfh.hh.type = crfcfg->type;
		ccr->ht.preset_crfh.timestamp_interval = htons(crfcfg->timestamp_interval);

		if (crfcfg->mr_pdus_toggle < AVTPC_MR_PDUS_TOGGLE_MIN) {
			UB_LOG(UBL_ERROR,"%s: invalid mr_toggle_avtpdus=%d\n",
				__func__, crfcfg->mr_pdus_toggle);
			return -1;
		}
		avtpc_crf->mr_sm.pdus_toggle = crfcfg->mr_pdus_toggle;
	}

	return 0;
}

void avtpc_crf_set_rcv_cb(avtpc_crf_data_t *avtpc_crf,
		      avbtp_rcv_cb_t rcv_cb, void *rcv_cbdata)
{
	if (!avtpc_crf) {
		UB_LOG(UBL_ERROR,"%s: invalid args\n",__func__);
		return;
	}
	avtpc_set_rcv_cb(avtpc_crf->avtpc, rcv_cb, rcv_cbdata);
}

int avtpc_crf_disconnection_request(avtpc_crf_data_t *avtpc_crf)
{
	if (!avtpc_crf) {
		UB_LOG(UBL_ERROR,"%s: invalid args\n",__func__);
		return -1;
	}
	if (avtpc_crf->avtpc){return avtpc_disconnection_request(avtpc_crf->avtpc);}
	return 0;
}

int avtpc_crf_close(avtpc_crf_data_t *avtpc_crf)
{
	if (!avtpc_crf ) {
		UB_LOG(UBL_ERROR,"%s: invalid args\n",__func__);
		return -1;
	}
	if (avtpc_crf->avtpc)
		avtpc_close(avtpc_crf->avtpc);
	free(avtpc_crf);
	return 0;
}
