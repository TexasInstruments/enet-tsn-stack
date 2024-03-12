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
 * avbtp.c
 * AVB Transport Protocol
 *
 * helper functions for avtpd.c and avtpc.c
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "avtp_ethernet.h"
#include "avbtp.h"
#include "tsn_gptp/gptpmasterclock.h"

/*****************************************
     private functions
*****************************************/
/*****************************************
     public functions
*****************************************/
void avbtp_generate_stream_cm_header(avbtp_cm_stream_header_t *cmsh,
				     avbtp_cm_stream_header_t *cmsh_temp,
				     uint32_t timestamp, uint16_t payload_size)
{
	/* increment sequence number */
	cmsh_temp->hh.seqn += 1;

	/* copy preset information */
	memcpy(cmsh, cmsh_temp, sizeof(avbtp_cm_stream_header_t));

	/* timestamp is nanosecond unit, use lower 32 bits */
	set_header_timestamp(cmsh,timestamp);

	set_header_pd_length(cmsh, payload_size);

	/* sv must be 1 for stream data */
	cmsh->bf=cmsh_sv_set_bit_field(cmsh->bf, 1);
}

int avbtp_serialize_stdtype_crf_data(uint64_t *array_ts, uint32_t num_ts,
				uint8_t *buffer, uint32_t buffer_size)
{
	uint32_t i;
	uint32_t j;
	uint8_t *pbuf = buffer;

	if (!array_ts || !buffer) {
		UB_LOG(UBL_ERROR,"%s:invalid args\n",__func__);
		return -1;
	}

	if ((num_ts * 8) > buffer_size) {
		UB_LOG(UBL_ERROR,"%s:insufficient buffer size\n",__func__);
		return -1;
	}

	for (i = 0; i < num_ts; i++) {
		for (j = 0; j < 8; j++) {
			pbuf[j] = (array_ts[i] >> ((7 - j) * 8)) & 0xFF;
		}
		pbuf += 8;
	}

	return (num_ts * 8);
}

int avbtp_deserialize_stdtype_crf_data(uint64_t *array_ts, uint32_t array_size,
				uint8_t *crf_data, uint32_t data_length)
{
	uint32_t i;
	uint32_t j;
	uint32_t num_ts;
	uint8_t *pbuf = crf_data;

	if (!array_ts || !crf_data) {
		UB_LOG(UBL_ERROR,"%s:invalid args\n",__func__);
		return -1;
	}

	if ((data_length % 8) != 0) {
		UB_LOG(UBL_ERROR,"%s:length is not aligned by 8 bytes\n",__func__);
		return -1;
	}

	num_ts = data_length / 8;
	if (array_size < num_ts) {
		UB_LOG(UBL_ERROR,"%s:insufficient array size\n",__func__);
		return -1;
	}

	for (i = 0; i < num_ts; i++) {
		array_ts[i] = 0;
		for (j = 0; j < 8; j++) {
			array_ts[i] |= (uint64_t)pbuf[j] << ((7 - j) * 8);
		}
		pbuf += 8;
	}

	return num_ts;
}

void avbtp_generate_control_cm_header(avbtp_cm_control_header_t *cmch,
				      avbtp_cm_control_header_t *cmch_temp,
				      uint16_t payload_size)
{
	/* copy preset information */
	memcpy(cmch, cmch_temp, sizeof(avbtp_cm_control_header_t));
	cmch->bf=cmch_cd_length_set_bit_field(cmch->bf, payload_size);

	/* clear sv bit if strem_id is all zero */
	if(!UB_NON_ZERO_B8(cmch->stream_id)){cmch->bf=cmch_sv_set_bit_field(cmch->bf, 0);}
}

void avbtp_generate_ntscf_header(avbtp_cm_control_header_t *cmch,
				 avbtp_cm_control_header_t *cmch_temp,
				 uint16_t payload_size)
{
	int seqn;
	/* increment sequence number */
	seqn=ntscf_seqnum_bit_field(cmch_temp->bf)+1;
	cmch_temp->bf=ntscf_seqnum_set_bit_field(cmch_temp->bf, seqn);

	/* copy preset information */
	memcpy(cmch, cmch_temp, sizeof(avbtp_cm_control_header_t));
	cmch->bf=ntscf_data_length_set_bit_field(cmch->bf, payload_size);

	/* clear sv bit if strem_id is all zero */
	if(!UB_NON_ZERO_B8(cmch->stream_id)){cmch->bf=ntscf_sv_set_bit_field(cmch->bf, 0);}
}

void avbtp_generate_crf_header(avbtp_crf_header_t *crfh,
				avbtp_crf_header_t *crfh_temp, uint16_t payload_size)
{
	/* increment sequence number */
	crfh_temp->hh.seqn += 1;

	/* copy preset information */
	memcpy(crfh, crfh_temp, sizeof(avbtp_crf_header_t));

	set_header_pd_length(crfh, payload_size);

	/* sv must be 1 */
	cmsh_sv_set_bit_field(crfh->bf, 1);
}

float avbtp_crf_nominal_frequency(avbtp_crf_pull_t pull, uint32_t base_freq)
{
	switch(pull) {
	case AVBTP_CRF_PULL_MULT_1:
		return (float)base_freq;
	case AVBTP_CRF_PULL_MULT_1_OVER_1_0001:
		return (float)base_freq / 1.001;
	case AVBTP_CRF_PULL_MULT_1_001:
		return (float)base_freq * 1.001;
	case AVBTP_CRF_PULL_MULT_24_OVER_25:
		return ((float)base_freq * 24) / 25;
	case AVBTP_CRF_PULL_MULT_25_OVER_24:
		return ((float)base_freq * 25) / 24;
	case AVBTP_CRF_PULL_MULT_1_OVER_8:
		return (float)base_freq / 8;
	default:
		return (float)base_freq;
	}
}

void avbtp_set_stream_id(avbtp_cm_stream_header_t *cmsh, ub_streamid_t stream_id)
{
	memcpy(cmsh->stream_id, stream_id, sizeof(ub_streamid_t));
	if (UB_NON_ZERO_B8(stream_id)){
		cmsh->bf=cmsh_sv_set_bit_field(cmsh->bf, 1);
	}else{
		cmsh->bf=cmsh_sv_set_bit_field(cmsh->bf, 0);
	}
}

void avbtp_set_stream_id_withmac(avbtp_cm_stream_header_t *cmsh, const char* dev,
				 uint16_t sid)
{
	cb_get_mac_bydev(CB_SOCKET_INVALID_VALUE, dev, cmsh->stream_id);
	cmsh->stream_id[6]=(sid>>8)&0xff;
	cmsh->stream_id[7]=sid&0xff;
	cmsh->bf=cmsh_sv_set_bit_field(cmsh->bf, 1);
}

int avbtp_header_size(void)
{
	return sizeof(struct avbtp_cm_stream_header);
}

int avbtp_set_sd_rinfo(avbtp_sd_info_t *sd_rinfo, uint8_t *payload,
		       int *seqn, int64_t timestamp)
{
	avbtp_cm_stream_header_t *cmsh=(avbtp_cm_stream_header_t *)payload;
	uint32_t fsd2;
	uint16_t fsd3;
	/* collect some information from the header, and put them into sd_rinfo */
	memset(sd_rinfo, 0, sizeof(avbtp_sd_info_t));
	sd_rinfo->subtype = (avbtp_subtype_def_t)cmsh->hh.subtype;
	sd_rinfo->stream_id_valid = (cmsh->hh.bf0>>7)&0x1;
	sd_rinfo->version = (cmsh->hh.bf0>>4)&0x7;
	sd_rinfo->media_restart = (cmsh->hh.bf0>>3)&0x1;
	sd_rinfo->timestamp_valid = cmsh->hh.bf0&0x1;
	sd_rinfo->timestamp_uncertain = cmsh->hh.bf1&0x1;
	memcpy(sd_rinfo->stream_id, cmsh->stream_id, sizeof(ub_streamid_t));

	/* calculate the difference btw the current seqn and the new one */
	sd_rinfo->seqn_diff = (cmsh->hh.seqn - *seqn) & 0xff;
	*seqn = cmsh->hh.seqn; // update the current seqn
	sd_rinfo->timestamp=timestamp;

	fsd2=get_header_fsd2(cmsh);
	fsd3=get_header_fsd3(cmsh);
	switch(sd_rinfo->subtype){
	case AVBTP_SUBTYPE_61883:
		sd_rinfo->u.iec.gv=(cmsh->hh.bf0>>1)&0x1;
		sd_rinfo->u.iec.gwinfo=fsd2;
		sd_rinfo->u.iec.tag=(fsd3>>14)&0x3;
		sd_rinfo->u.iec.channel=(fsd3>>8)&0x3f;
		sd_rinfo->u.iec.tcode=(fsd3>>4)&0xf;
		sd_rinfo->u.iec.sy=fsd3&0xf;
		break;
	case AVBTP_SUBTYPE_AAF:
		sd_rinfo->u.aaf.format=(avbtp_aaf_format_t)(fsd2>>24);
		sd_rinfo->u.aaf.afsd1=fsd2&0xffffff;
		sd_rinfo->u.aaf.afsd=fsd3>>13;
		sd_rinfo->u.aaf.sp=(fsd3>>12)&0x1;
		sd_rinfo->u.aaf.evt=(fsd3>>8)&0xf;
		sd_rinfo->u.aaf.afsd2=fsd3&0xff;
		break;
	case AVBTP_SUBTYPE_CVF:
		sd_rinfo->u.cvf.format=(avbtp_cvf_format_t)(fsd2>>24);
		sd_rinfo->u.cvf.format_subtype=(avbtp_cvf_format_subtype_t)((fsd2>>16)&0xff);
		sd_rinfo->u.cvf.evt=(fsd3>>8)&0xf;
		sd_rinfo->u.cvf.m=(fsd3>>12)&0x1;
		break;
	case AVBTP_SUBTYPE_TSCF:
		break;
	default:
		UB_LOG(UBL_INFO,"%s:subtype=%d is not supported\n",__func__,sd_rinfo->subtype);
		sd_rinfo->u.gen.fsd = (cmsh->hh.bf0>>1)&0x3;
		sd_rinfo->u.gen.fsd1 = (cmsh->hh.bf1>>1)&0x7f;
		sd_rinfo->u.gen.fsd2 = fsd2;
		sd_rinfo->u.gen.s_dlen = get_header_pd_length(cmsh);
		sd_rinfo->u.gen.fsd3 = fsd3;
		break;
	}
	return get_header_pd_length(cmsh);
}

int avbtp_set_crf_rinfo(avbtp_crf_info_t *crf_info, uint8_t *payload, int *seqn)
{
	avbtp_crf_header_t *crfh = (avbtp_crf_header_t *)payload;
	uint32_t bf1_host;

	/* collect some information from the header, and put them into crf_rinfo */
	memset(crf_info, 0, sizeof(avbtp_crf_info_t));
	crf_info->subtype = (avbtp_subtype_def_t)crfh->hh.subtype;
	crf_info->stream_id_valid = (crfh->hh.bf0 >> 7) & 0x01;
	crf_info->version = (crfh->hh.bf0 >> 4) & 0x7;
	crf_info->media_restart = (crfh->hh.bf0 >> 3) & 0x1;
	crf_info->frame_sync = (crfh->hh.bf0 >> 1) & 0x1;
	crf_info->timestamp_uncertain = crfh->hh.bf0 & 0x1;
	/* calculate the difference btw the current seqn and the new one */
	crf_info->seqn_diff = (crfh->hh.seqn - *seqn) & 0xff;
	*seqn = crfh->hh.seqn; // update the current seqn
	crf_info->type = (avbtp_crf_type_t)crfh->hh.type;
	memcpy(crf_info->stream_id, crfh->stream_id, sizeof(ub_streamid_t));
	bf1_host= ntohl(crfh->bf1);
	crf_info->pull = (avbtp_crf_pull_t)((bf1_host >> 29) & 0x7);
	crf_info->base_frequency = bf1_host & 0x1FFFFFFF;
	crf_info->timestamp_interval = ntohs(crfh->timestamp_interval);

	if (crf_info->subtype != AVBTP_SUBTYPE_CRF) {
		UB_LOG(UBL_WARN,"%s:subtype=%d invalid in CRF\n",
			   __func__, crf_info->subtype);
	}
	if (crf_info->stream_id_valid == false) {
		UB_LOG(UBL_WARN,"%s:CRF stream invalid\n", __func__);
	}
	if (crf_info->pull > AVBTP_CRF_PULL_MULT_1_OVER_8) {
		UB_LOG(UBL_WARN,"%s:CRF wrong pull field\n", __func__);
	}
	if (crf_info->type > AVBTP_CRF_TYPE_MACHINE_CYCLE) {
		UB_LOG(UBL_WARN,"%s:CRF wrong type\n", __func__);
	}

	return get_header_pd_length(crfh);
}

size_t get_ccd_size(client_connect_request_t *ccr, unsigned int *ccdbufnum)
{
	unsigned int bfnum;
	size_t ccdsize;
	if (ccr->time_intv <= 0) {
		UB_LOG(UBL_ERROR,"%s:invalid ccr->time_intv=%d\n",
			   __func__, ccr->time_intv);
		return 0;
	}
	bfnum = ccr->avtpd_bufftime / ccr->time_intv;
	ccdsize = (CCDBUFF_PAYLOAD_OFFSET + ccr->max_frame_size +
		   sizeof(ccdbuf_data_info_t)) * ccr->max_intv_frames *
		bfnum + sizeof(client_connection_data_t);
	if(ccdbufnum){*ccdbufnum = ccr->max_intv_frames * bfnum;}
	return ccdsize;
}

int ccr_sanity_check(client_connect_request_t *ccr)
{
	unsigned int ccdbufnum = 0;
	// any parameters which possibly kill avtpd should be rejected here
	if(get_ccd_size(ccr, &ccdbufnum) <= sizeof(client_connection_data_t)){
		UB_LOG(UBL_ERROR,"%s:ccdsize too small\n",__func__);
		return -1;
	}
	if(ccdbufnum<2) {
		UB_LOG(UBL_ERROR,"%s:ccdbufnum:%d too small\n",__func__, ccdbufnum);
		return -1;
	}
	return 0;
}

avbtp_header_type_t get_header_type(avbtp_subtype_t subtype)
{
	if(subtype<=AVBTP_SUBTYPE_CVF){return AVBTP_STREAM_HEADER;}
	if(subtype>=AVBTP_SUBTYPE_ADP){return AVBTP_CONTROL_HEADER;}
	switch(subtype){
	case AVBTP_SUBTYPE_CRF:
		return AVBTP_ALTERNATIVE_HEADER;
	case AVBTP_SUBTYPE_TSCF:
	case AVBTP_SUBTYPE_SVF:
	case AVBTP_SUBTYPE_RVF:
		return AVBTP_STREAM_HEADER;
	case AVBTP_SUBTYPE_AEF_CONTINUOUS:
		return AVBTP_ALTERNATIVE_HEADER;
	case AVBTP_SUBTYPE_VSF_STREAM:
	case AVBTP_SUBTYPE_EF_STREAM:
	case AVBTP_SUBTYPE_MMA_CONTROL:
		return AVBTP_STREAM_HEADER;
	case AVBTP_SUBTYPE_NTSCF:
	case AVBTP_SUBTYPE_ESCF:
	case AVBTP_SUBTYPE_EECF:
	case AVBTP_SUBTYPE_AEF_DISCRETE:
		return AVBTP_ALTERNATIVE_HEADER;
	default:
		return AVBTP_UNKNOWN_HEADER;
	}
}

int avbtp_set_control_ccr(client_connect_request_t *ccr, const char *netdev,
			  avbtp_subtype_t subtype, uint32_t fsd0, uint16_t vid)
{
	memset(ccr,0,sizeof(client_connect_request_t));
	ccr->con_mode=AVTP_CONTROL_CONNECTION;
	strcpy(ccr->netdev,netdev);
	/* 4 packets in 10msec, one for a write buffer */
	ccr->max_frame_size=IEEE1722_DEFAULT_MAX_FRAME_SIZE;
	ccr->max_intv_frames=4;
	ccr->time_intv=10000;
	ccr->lsem_post_time=DEFAULT_LSEM_POST_TIME;
	/* for control packets, the default priority=0 */
	ccr->priority=0;
	ccr->rank=1;
	ccr->ht.preset_cmch.hh.subtype=subtype;
	ccr->ht.preset_cmch.bf=cmch_fsd0_set_bit_field(ccr->ht.preset_cmch.bf, fsd0);
	if(vid){ccr->tagged=true;}
	ccr->vlanid=vid;
	switch(subtype){
	case AVBTP_SUBTYPE_MAAP:
		ub_smac2bmac(AVBTP_MCAST_MAC_MAAP, ccr->mcast_addr);
		ub_smac2bmac(AVBTP_MCAST_MAC_MAAP, ccr->join_mcast);
		break;
	case AVBTP_SUBTYPE_ADP:
	case AVBTP_SUBTYPE_ACMP:
	case AVBTP_SUBTYPE_AECP:
		ub_smac2bmac(AVBTP_MCAST_MAC_AVDECC, ccr->mcast_addr);
		ub_smac2bmac(AVBTP_MCAST_MAC_AVDECC, ccr->join_mcast);
		break;
	default:
		UB_LOG(UBL_ERROR,"%s:subtype=%d is not supported, "
		       "mcast_addr and join_mcast are not configured\n",__func__, subtype);
		break;
	}
	return 0;
}

int avbtp_set_stream_ccr(client_connect_request_t *ccr, const char *netdev,
                         avbtp_subtype_t subtype, bool talker, uint16_t vid,
                         uint8_t pcp, uint32_t max_frm_sz,
                         uint32_t time_intv, uint32_t max_intv_frm)
{
	memset(ccr,0,sizeof(client_connect_request_t));
	if(talker){
		ccr->con_mode=AVTP_TALKER_CONNECTION;
	}else{
		ccr->con_mode=AVTP_LISTENER_CONNECTION;
	}
	strcpy(ccr->netdev,netdev);
	/* default for 1 packet in ClassA:250usec */
	ccr->max_frame_size=(max_frm_sz)? max_frm_sz: IEEE1722_DEFAULT_MAX_FRAME_SIZE;
	ccr->max_intv_frames=max_intv_frm;
	ccr->time_intv=time_intv;
	ccr->lsem_post_time=DEFAULT_LSEM_POST_TIME;
	ccr->priority=pcp;
	ccr->rank=1;
	ccr->ht.preset_cmch.hh.subtype=subtype;
	if(vid){ccr->tagged=true;}
	ccr->rec_tagged=VLAN_REC_TAGGED;
	ccr->vlanid=vid;
	return 0;
}

int avbtp_rec_ts_check(int64_t *last_pts, avbtp_sd_info_t *rsdinfo)
{
	int64_t tdiff;
	uint64_t avbts;
	int res=0;

	if (!rsdinfo->timestamp_valid || (rsdinfo->timestamp == (uint64_t)*last_pts)){return AVBTP_REC_TS_NOTVALID;}
	avbts = gptpmasterclock_getts64();
	UB_LOG(UBL_DEBUG,"ts=%"PRIu64", ts_diff=%"PRId64"\n",avbts, avbts-rsdinfo->timestamp);
	tdiff=avbts-rsdinfo->timestamp;
	if(tdiff>0){
		UB_LOG(UBL_DEBUG,"%s:TS to render is in the past for %"PRId64
		       " nsec\n", __func__, tdiff);
		res=AVBTP_REC_TS_IN_PAST<<4;
	}else{
		// tdiff could be a few frames in future for B frames
		if(tdiff<-500000000){
			// it shouldn't < -500msec, even for B frames
			UB_LOG(UBL_INFO,"%s:TS to render is in the future for %"PRId64
			       " usec %"PRIu64"\n", __func__, -tdiff/1000, avbts%10000000000);
		}
		res=AVBTP_REC_TS_IN_FUTURE<<4;
	}

	if(rsdinfo->timestamp < (uint64_t)*last_pts){
		// negative move happens when there are B frames
		UB_LOG(UBL_DEBUG,"%s:negative move from old_pts=%"PRIu64" to timestamp=%"PRIu64"\n",
		       __func__, *last_pts, rsdinfo->timestamp);
		res|=AVBTP_REC_TS_NEGATIVE_MOVE;
	}else if(rsdinfo->timestamp > (uint64_t)*last_pts + 1000000000 ){
		UB_LOG(UBL_INFO,"%s:big skip from old_pts=%"PRIu64" to timestamp=%"PRIu64"\n",
		       __func__, *last_pts, rsdinfo->timestamp);
		res|=AVBTP_REC_TS_BIG_MOVE;
	}
	UB_LOG(UBL_DEBUG,"%s:packet ts=%"PRIu32" current avbts=%"PRIu32"\n",
	       __func__, (uint32_t)rsdinfo->timestamp, (uint32_t)avbts);
	*last_pts = rsdinfo->timestamp;
	return res;
}

int avbtp_set_cmsh_iec_info(avbtp_cm_stream_header_t *cmsh, avbtp_sd_iec_info_t *iec)
{
	set_header_fsd2(cmsh, iec->gwinfo);
	set_header_fsd3(cmsh,(((iec->tag&0x3)<<14) |
				((iec->channel&0x3F)<<8) |
				((iec->tcode&0xF)<<4) |
				((iec->sy&0xF)<<0)));
	return 0;
}

int avbtp_set_iec_cip_header(uint8_t *payload, uint16_t payload_size, avbtp_iec_cip_info_t *cip)
{
	if(!payload){return -1;}
	if(payload_size<AVBTP_IEC61883_CIP_HEADERLEN){return -1;}

	/* Figure 24: IEC 61883 with source packet header PDU  (CIP header)
     *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  |qi1|     SID   |  DBS          | FN| QPC |S|rsv|     DBC       |
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  |qi2|     FMT   |     FDF                                       |
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	memset(payload, 0, AVBTP_IEC61883_CIP_HEADERLEN);
	// CIP#1
	payload[0] = ((cip->qi_1&0x3)<<6)|(cip->sid&0x3F);
	payload[1] = cip->dbs;
	payload[2] = ((cip->fn&0x3)<<6)|((cip->qpc&0x7)<<3)|((cip->sph&0x1)<<2);
	payload[3] = cip->dbc;
	// CIP#2
	payload[4] = ((cip->qi_2&0x3)<<6)|(cip->fmt&0x3F);
	memcpy(payload+5, cip->fdf, 3);
	return 0;
}

int avbtp_set_iec_sph_header(uint8_t *payload, uint16_t payload_size, uint64_t sph)
{
	if(!payload){return -1;}
	if(payload_size<4){return -1;}

	/* Figure 24: IEC 61883 with source packet header PDU  (SPH timestamp)
     *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  |            avtp_source_packet_header_timestamp                |
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	memcpy(payload, (uint8_t *)&sph, 4);
	return 0;
}
