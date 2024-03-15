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
 * avtpc.c
 * AVB Transport Protocol Client Process
 *
 */

#include <stdlib.h>
#include <errno.h>
#include "avtpc.h"
#include "avtpc_private.h"

static int rec_listener_data(avtpc_data_t *avtpc)
{
	client_connection_data_t *ccd=avtpc->ccds;
	struct timespec ts;
	avbtp_rcv_cb_info_t cb_info;
	int i;
	ccdbuf_data_t *ccdbuf;
	int dlength, headersize;
	int res=0;
	avbtp_cm_control_header_t *cmch;
	int bufused;
	uint64_t ts64;
	uint64_t ptpts64=0;

	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+RECV_DATA_CHECK_INTERVAL, ts);
	if(CB_SEM_TIMEDWAIT(&ccd->rsem_set, &ts)){res=1;}
	if(ccd->bufused==0) {
		if(avtpc->rcvtimeout_cb){avtpc->rcvtimeout_cb(avtpc->timeout_cbdata);}
		return res;
	}

	if(ccd->ccr.con_mode==AVTP_CONTROL_CONNECTION){
		headersize=sizeof(avbtp_cm_control_header_t);
	}else if(ccd->ccr.con_mode==AVTP_NTSCF_CONNECTION){
		// NTSCF uses the Alternative Header, but including NTSCF specific part,
		// it becomes the same like the Control Header
		headersize=sizeof(avbtp_cm_control_header_t);
	}else if(ccd->ccr.con_mode==AVTP_CRF_LISTENER_CONNECTION){
		headersize=sizeof(avbtp_crf_header_t);
	}else{
		headersize=sizeof(avbtp_cm_stream_header_t);
	}

	bufused=ccd->bufused;
	if(ccd->ccr.avtpc_expandts32){ptpts64=gptpmasterclock_getts64();}
	/* handle data from the client */
	for(i=0;i<bufused;i++){
		ccdbuf = (ccdbuf_data_t *)(ccd->buffer +
					   (CCDBUFF_PAYLOAD_OFFSET + ccd->ccr.max_frame_size +
					    sizeof(ccdbuf_data_info_t)) * ccd->readp);
		if(ccd->ccr.con_mode==AVTP_CONTROL_CONNECTION ||
		   ccd->ccr.con_mode==AVTP_NTSCF_CONNECTION){
			cmch=(avbtp_cm_control_header_t *)ccdbuf->payload;
			memcpy(cb_info.u.rcsinfo.s_addr,ccdbuf->header+ETH_ALEN,ETH_ALEN);
			memcpy(cb_info.u.rcsinfo.stream_id, cmch->stream_id, sizeof(ub_streamid_t));
			if(ccd->ccr.con_mode==AVTP_CONTROL_CONNECTION){
				cb_info.u.rcsinfo.fsd0=cmch_fsd0_bit_field(cmch->bf);
				dlength=cmch_cd_length_bit_field(cmch->bf);
				cb_info.u.rcsinfo.stream_id_valid =
					(cmch_sv_bit_field(cmch->bf)==1);
				cb_info.u.rcsinfo.version =
					cmch_version_bit_field(cmch->bf);
			}else{
				//AVTP_NTSCF_CONNECTION
				cb_info.u.rcsinfo.fsd0=ntscf_seqnum_bit_field(cmch->bf);
				dlength=ntscf_data_length_bit_field(cmch->bf);
				cb_info.u.rcsinfo.stream_id_valid =
					(ntscf_sv_bit_field(cmch->bf)==1);
				cb_info.u.rcsinfo.version =
					ntscf_version_bit_field(cmch->bf);
			}
		}else if(ccd->ccr.con_mode==AVTP_CRF_LISTENER_CONNECTION){
			dlength=avbtp_set_crf_rinfo(&cb_info.u.rcrfinfo,
							ccdbuf->payload, &avtpc->seqn);
		}else{
			dlength=avbtp_set_sd_rinfo(&cb_info.u.rsdinfo, ccdbuf->payload,
						   &avtpc->seqn, ccdbuf->info.timestamp);
			/* when avtpd pass the 32bits timestamp, we will expand here */
			if(ptpts64 && cb_info.u.rsdinfo.timestamp_valid &&
			   !cb_info.u.rsdinfo.timestamp_uncertain){

				cb_info.u.rsdinfo.timestamp=gptpmasterclock_expand_timestamp_ts64(
					ccdbuf->info.timestamp, ptpts64);
				cb_info.u.rsdinfo.current_timestamp = ptpts64;
			}
		}
		if(avtpc->rcv_cb){
			avtpc->rcv_cb(ccdbuf->payload+headersize,
				      dlength, &cb_info, avtpc->rcv_cbdata);
		}
		if(++ccd->readp==ccd->bufnum){ccd->readp=0;}
	}
	CB_THREAD_MUTEX_LOCK(&ccd->rmutex);
	ccd->bufused-=bufused;
	if(ccd->bufused==0){while(CB_SEM_TRYWAIT(&ccd->rsem_set)==0) ;} //set semaphore to zero
	else
		CB_SEM_POST(&ccd->rsem_set); // need another time
	CB_THREAD_MUTEX_UNLOCK(&ccd->rmutex);

	if(ccd->bufused==0 && avtpc->rcv_done_cb) {
		avtpc->rcv_done_cb(avtpc->rcv_cbdata);
	}

	CB_SEM_POST(&ccd->rsem_rel);

	return 0;
}

static int rec_talker_data(avtpc_data_t *avtpc)
{
	CB_USLEEP(RECV_DATA_CHECK_INTERVAL/1000);
	return 1;
}

static void *receive_data_thread(void *ptr)
{
	int res;
	avtpc_data_t *avtpc=(avtpc_data_t *)ptr;
	client_connection_data_t *ccd=avtpc->ccds;

	while(ccd->stop_thread<CC_THREAD_IMMEDIATE_STOP){
		ccd->health_check=0;
		switch(ccd->ccr.con_mode){
		case AVTP_LISTENER_CONNECTION:
		case AVTP_CONTROL_CONNECTION:
		case AVTP_NTSCF_CONNECTION:
		case AVTP_TSCF_CONNECTION:
		case AVTP_CRF_LISTENER_CONNECTION:
			res=rec_listener_data(avtpc);
			if (res > 0){UB_LOG(UBL_DEBUGV,"%s: timeout %d rec_listener_data\n",
				       __func__,res);}
			if (res < 0){UB_LOG(UBL_ERROR,"%s: error %d  rec_listener_data\n",
				       __func__,res);}
			break;
		case AVTP_TALKER_CONNECTION:
		case AVTP_TALKONLY_CONTROL_CONNECTION:
		case AVTP_CRF_TALKER_CONNECTION:
			res=rec_talker_data(avtpc);
			break;
		default:
			UB_LOG(UBL_ERROR,"%s:not supported connection mode: %d\n",
			       __func__, ccd->ccr.con_mode);
			res=-1;
			break;
		}
		if(res<0){
			ccd->stop_thread=CC_THREAD_IMMEDIATE_STOP;
		}else if(res==1 && ccd->stop_thread==CC_THREAD_BUFFLASH_STOP){
			ccd->stop_thread=CC_THREAD_IMMEDIATE_STOP;
		}
	}
	UB_LOG(UBL_ERROR, "%s: receiver thread stopped with code=%d\n",
			__func__, ccd->stop_thread);
	return NULL;
}

static int mcd_request_response(avtpc_data_t *avtpc, ccr_request_type_t request,
			    client_connect_result_t result, const char *funcname)
{
	struct timespec ts;
	uint64_t ts64;

	avtpc->mcd->reqtype=request;
	CB_SEM_POST(&avtpc->mcd->sem_request);
	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+AVTPD_RESPONSE_TIMEOUT, ts);
	if(CB_SEM_TIMEDWAIT(&avtpc->mcd->sem_response, &ts)) {
		UB_LOG(UBL_ERROR,"%s:no response from avtpd\n",funcname);
		return -1;
	}
	if(avtpc->mcd->ccres.result!=result){
		UB_LOG(UBL_INFO,"%s:not expected response:%d\n",funcname, avtpc->mcd->ccres.result);
		return -1;
	}
	return 0;
}

static int disconnection_request(avtpc_data_t *avtpc, client_connect_request_t *ccr)
{
	int res=-1;

	CB_THREAD_MUTEX_LOCK(&avtpc->mcd->mutex);
	memcpy(&avtpc->mcd->ccr, ccr, sizeof(client_connect_request_t));
	res=mcd_request_response(avtpc, CCR_DISCONNECTION_REQUEST,
				 AVTP_CCR_ACCEPTED, __func__);
	CB_THREAD_MUTEX_UNLOCK(&avtpc->mcd->mutex);
	return res;
}

static int ccds_close(avtpc_data_t *avtpc)
{
	int res=0;
	char shmname[32];

	if(!avtpc->ccds){return 0;}

	// In case of no health-check, avtpd cannot self-terminate the connection
	// So better to disconnect with avtpd before cleanup the avtpc threads
	disconnection_request(avtpc, &avtpc->ccds->ccr);

	if(avtpc->rcv_thread){
		if(avtpc->ccds->ccr.con_mode==AVTP_LISTENER_CONNECTION ||
		   avtpc->ccds->ccr.con_mode==AVTP_CONTROL_CONNECTION ||
		   avtpc->ccds->ccr.con_mode==AVTP_NTSCF_CONNECTION ||
		   avtpc->ccds->ccr.con_mode==AVTP_TSCF_CONNECTION ||
		   avtpc->ccds->ccr.con_mode==AVTP_TALKONLY_CONTROL_CONNECTION ||
		   avtpc->ccds->ccr.con_mode==AVTP_CRF_LISTENER_CONNECTION){
			avtpc->ccds->stop_thread=CC_THREAD_BUFFLASH_STOP;
		}else{
			avtpc->ccds->stop_thread=CC_THREAD_IMMEDIATE_STOP;
		}
		CB_THREAD_JOIN(avtpc->rcv_thread, NULL);
	}

	switch(avtpc->ccds->ccr.con_mode){
	case AVTP_TALKER_CONNECTION:
	case AVTP_TALKONLY_CONTROL_CONNECTION:
	case AVTP_CRF_TALKER_CONNECTION:
		snprintf(shmname, 32, AVBTP_CCD_SHMEM_PREFIX"%s%d-%d", avtpc->shsuf,
			 TALKER_SHM_SUFFIX, avtpc->mcd->ccres.ccd);
		break;
	case AVTP_LISTENER_CONNECTION:
	case AVTP_CONTROL_CONNECTION:
	case AVTP_NTSCF_CONNECTION:
	case AVTP_TSCF_CONNECTION:
	case AVTP_CRF_LISTENER_CONNECTION:
		snprintf(shmname, 32, AVBTP_CCD_SHMEM_PREFIX"%s%d-%d", avtpc->shsuf,
			 LISTENER_SHM_SUFFIX, avtpc->mcd->ccres.ccd);
		break;
	}

	res=cb_close_shared_mem(avtpc->ccds, &avtpc->ccd_shmem, shmname,
				get_ccd_size(&avtpc->ccds->ccr, NULL), false);
	avtpc->ccds=NULL;
	return res;
}

int avtpc_close(avtpc_data_t *avtpc)
{
	int res;
	char shmname[32];

	if(!avtpc){return -1;}
	res=ccds_close(avtpc);
	snprintf(shmname, 32, "%s%s", AVBTP_MCD_SHMEM_NAME, avtpc->shsuf);
	res|=cb_close_shared_mem(avtpc->mcd, &avtpc->mcd_shmem, shmname,
				sizeof(master_connection_data_t), false);
	if(avtpc->mcd_shmem>0){close(avtpc->mcd_shmem);}
	free(avtpc);
	return res;
}

avtpc_data_t *avtpc_init(char *shsuf)
{
	avtpc_data_t *avtpc=NULL;
	char shmname[32];
	UB_LOG(UBL_INFO, "%s: l2-"TSNPKGVERSION"\n", __func__);
	avtpc=(avtpc_data_t *)malloc(sizeof(avtpc_data_t));
	if(ub_assert_fatal(avtpc!=NULL, __func__, NULL)) return NULL;
	memset(avtpc, 0, sizeof(avtpc_data_t));
	if(shsuf){snprintf(avtpc->shsuf, sizeof(avtpc->shsuf), "%s", shsuf);}
	snprintf(shmname, 32, "%s%s", AVBTP_MCD_SHMEM_NAME, avtpc->shsuf);
	avtpc->mcd=(master_connection_data_t *)cb_get_shared_mem(&avtpc->mcd_shmem, shmname,
				     sizeof(master_connection_data_t), O_RDWR);
	if(!avtpc->mcd){goto erexit;}
	return avtpc;
erexit:
	avtpc_close(avtpc);
	return NULL;
}

void avtpc_set_rcv_cb(avtpc_data_t *avtpc,
		      avbtp_rcv_cb_t rcv_cb, void *rcv_cbdata)
{
	avtpc->rcv_cb=rcv_cb;
	avtpc->rcv_cbdata=rcv_cbdata;
}

void avtpc_set_rcvtimeout_cb(avtpc_data_t *avtpc,
			avbtp_rcvtimeout_cb_t rcvtimeout_cb, void *timeout_cbdata)
{
	avtpc->rcvtimeout_cb=rcvtimeout_cb;
	avtpc->timeout_cbdata=timeout_cbdata;
}

int avtpc_connection_request(avtpc_data_t *avtpc, client_connect_request_t *ccr)
{
	int res=-1;
	int ci;
	char shmname[32];
	uint8_t *mcast=NULL;
	bool create_thread=true;

	if(!ccr->avtpd_bufftime) {
		UB_LOG(UBL_INFO,"%s:ccr->avtpd_bufftime is zero, default value:%dusec is used\n",
		       __func__, DEFAULT_AVTPD_BUFFTIME);
		ccr->avtpd_bufftime=DEFAULT_AVTPD_BUFFTIME;
	}
	if(ccr_sanity_check(ccr)){return -1;}

	switch(ccr->con_mode){
	case AVTP_TALKER_CONNECTION:
	case AVTP_CRF_TALKER_CONNECTION:
		mcast=ccr->mcast_addr;
		break;
	case AVTP_LISTENER_CONNECTION:
	case AVTP_CRF_LISTENER_CONNECTION:
		mcast=ccr->join_mcast;
		break;
	default:
		break;
	}
	if(ccr->netdev[strlen(ccr->netdev)-1]=='/' && mcast){
                char astr[CB_MAX_NETDEVNAME]; //ccr->netdev
                /* if device name ends with '/', cange it QNX lsm device name */
		ccr->netdev[CB_MAX_NETDEVNAME-1]=0; // force to truncate the size
                strcpy(astr, ccr->netdev);
                snprintf(ccr->netdev, sizeof(ccr->netdev), "%s"UB_PRIhexB6,
                         astr,UB_ARRAY_B6(mcast));
	}

	CB_THREAD_MUTEX_LOCK(&avtpc->mcd->mutex);
	memcpy(&avtpc->mcd->ccr, ccr, sizeof(client_connect_request_t));
	if(mcd_request_response(avtpc, CCR_CONNECTION_REQUEST,
				AVTP_CCR_ACCEPTED, __func__)){goto erexit;}
	ci=avtpc->mcd->ccres.ccd;
	switch(ccr->con_mode){
	case AVTP_TALKER_CONNECTION:
	case AVTP_TALKONLY_CONTROL_CONNECTION:
	case AVTP_CRF_TALKER_CONNECTION:
		snprintf(shmname, 32, AVBTP_CCD_SHMEM_PREFIX"%s%d-%d", avtpc->shsuf,
			 TALKER_SHM_SUFFIX, ci);
		if(ccr->no_healthcheck){
			create_thread=false;
		}
		break;
	case AVTP_LISTENER_CONNECTION:
	case AVTP_CONTROL_CONNECTION:
	case AVTP_NTSCF_CONNECTION:
	case AVTP_TSCF_CONNECTION:
	case AVTP_CRF_LISTENER_CONNECTION:
		snprintf(shmname, 32, AVBTP_CCD_SHMEM_PREFIX"%s%d-%d", avtpc->shsuf,
			 LISTENER_SHM_SUFFIX, ci);
		break;
	}

	avtpc->ccds=(client_connection_data_t *)cb_get_shared_mem(&avtpc->ccd_shmem, shmname,
				      get_ccd_size(ccr, NULL), O_RDWR);
	if(!avtpc->ccds){goto erexit;}

	if(create_thread){
		cb_tsn_thread_attr_t attr;
		char thread_name[CB_TSN_THREAD_NAME_SIZE]={0};
		/* use "avtpc_"+strem_id as the thread_name, for control connection
		* it may not be unique */
		strcpy(thread_name, "avtpc_"); //strlen("avtpc_")=6
		sprintf(thread_name+6, UB_PRIhexB8, UB_ARRAY_B8(ccr->ht.preset_cmsh.stream_id));
		cb_tsn_thread_attr_init(&attr, THREAD_NORM_PRI, THREAD_NORM_STACK, thread_name);
		if(CB_THREAD_CREATE(&avtpc->rcv_thread, &attr, receive_data_thread, avtpc)){
			UB_LOG(UBL_ERROR,"%s:CB_THREAD_CREATE, %s\n", __func__, strerror(errno));
			avtpc->rcv_thread=0;
			goto erexit;
		}
	}
	res=0;

erexit:
	CB_THREAD_MUTEX_UNLOCK(&avtpc->mcd->mutex);
	return res;
}

int avtpc_disconnection_request(avtpc_data_t *avtpc)
{
	return ccds_close(avtpc);
}

int avtpc_write_sd_packet(avtpc_data_t *avtpc, int sd_size, uint8_t *sd_payload,
			  uint64_t timestamp)
{
	payload_multi_bufs_t plmbs[2];
	plmbs[0].data=sd_payload;
	plmbs[0].size=sd_size;
	plmbs[1].size=0;
	return avtpc_write_sd_packet_mb(avtpc, plmbs, timestamp);
}


int avtpc_write_sd_packet_mb(avtpc_data_t *avtpc, payload_multi_bufs_t plmbs[],
			     uint64_t timestamp)
{
	struct timespec ts;
	uint64_t ts64;
	ccdbuf_data_t *cdata;
	avbtp_cm_stream_header_t *cmsh;
	int i;
	uint32_t tsize=0;
	for(i=0;i<AVTPC_MAX_MEM_BLOCKS+1;i++){
		if(plmbs[i].size==0){break;}
		tsize+=plmbs[i].size;
	}

	if(tsize > avtpc->ccds->ccr.max_frame_size - sizeof(avbtp_cm_stream_header_t)){
		UB_LOG(UBL_ERROR,"%s:1 size=%"PRIu32" is too big  max = %d - %"PRIuPTR"\n",
		       __func__, tsize, avtpc->ccds->ccr.max_frame_size,
		       sizeof(avbtp_cm_stream_header_t));
		return -1;
	}

	if(!UB_NON_ZERO_B8(avtpc->ccds->ccr.ht.preset_cmsh.stream_id)){
		UB_LOG(UBL_ERROR,"%s:steram_id is all zero\n",__func__);
		return -1;
	}

	while(avtpc->ccds->bufused==avtpc->ccds->bufnum){
		ts64=ub_rt_gettime64();
		UB_NSEC2TS(ts64+WRITE_PACKET_TIMEOUT*1000, ts);
		if(CB_SEM_TIMEDWAIT(&avtpc->ccds->tsem_rel, &ts)) {
			UB_LOG(UBL_ERROR,"%s:timedout, streamid=0x%02hhX%02hhX\n", __func__,
			       avtpc->ccds->ccr.ht.preset_cmsh.stream_id[6],
			       avtpc->ccds->ccr.ht.preset_cmsh.stream_id[7]);
			return -1;
		}
	}

	CB_THREAD_MUTEX_LOCK(&avtpc->ccds->tmutex);
	cdata = (ccdbuf_data_t *)(avtpc->ccds->buffer +
				  (CCDBUFF_PAYLOAD_OFFSET + avtpc->ccds->ccr.max_frame_size +
				   sizeof(ccdbuf_data_info_t)) * avtpc->ccds->writep);
	cdata->info.size=tsize+sizeof(avbtp_cm_stream_header_t);
	cmsh=(avbtp_cm_stream_header_t *)cdata->payload;
	cdata->info.timestamp=timestamp;
	avbtp_generate_stream_cm_header(cmsh, &avtpc->ccds->ccr.ht.preset_cmsh,
					timestamp, tsize);

	tsize=0;
	for(i=0;i<AVTPC_MAX_MEM_BLOCKS+1;i++){
		if(plmbs[i].size==0){break;}
		memcpy(cdata->payload+sizeof(avbtp_cm_stream_header_t)+tsize,
		       plmbs[i].data, plmbs[i].size);
		tsize+=plmbs[i].size;
	}
	if(++avtpc->ccds->writep==avtpc->ccds->bufnum){avtpc->ccds->writep=0;}
	avtpc->ccds->bufused++;
	if(avtpc->ccds->bufused==avtpc->ccds->bufnum){while(CB_SEM_TRYWAIT(&avtpc->ccds->tsem_rel)==0) ;} //set semaphore to zero
	CB_THREAD_MUTEX_UNLOCK(&avtpc->ccds->tmutex);
	CB_SEM_POST(&avtpc->ccds->tsem_set);
	return 0;
}

int avtpc_write_control_packet(avtpc_data_t *avtpc, int cd_size, uint8_t *cd_payload)
{
	struct timespec ts;
	uint64_t ts64;
	ccdbuf_data_t *cdata;
	avbtp_cm_control_header_t *cmch;

	if(avtpc->ccds->ccr.con_mode!=AVTP_CONTROL_CONNECTION &&
	   avtpc->ccds->ccr.con_mode!=AVTP_TALKONLY_CONTROL_CONNECTION){
		UB_LOG(UBL_ERROR,"%s: not control connection\n",__func__);
		return -1;
	}

	if((unsigned int)cd_size > avtpc->ccds->ccr.max_frame_size -
	   sizeof(avbtp_cm_control_header_t)){
		UB_LOG(UBL_ERROR,"%s: size=%d is too big\n",__func__, cd_size);
		return -1;
	}
	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+WRITE_PACKET_TIMEOUT*1000, ts);
	if(CB_SEM_TIMEDWAIT(&avtpc->ccds->tsem_rel, &ts)) {
		UB_LOG(UBL_ERROR,"%s:timedout\n", __func__);
		return -1;
	}

	CB_THREAD_MUTEX_LOCK(&avtpc->ccds->tmutex);
	if(avtpc->ccds->ccr.con_mode==AVTP_TALKONLY_CONTROL_CONNECTION){
		cdata = (ccdbuf_data_t *)(avtpc->ccds->buffer +
					  (CCDBUFF_PAYLOAD_OFFSET +
					   avtpc->ccds->ccr.max_frame_size +
					   sizeof(ccdbuf_data_info_t)) *
					  avtpc->ccds->writep);
	}else{
		cdata = (ccdbuf_data_t *)(avtpc->ccds->buffer +
					  (CCDBUFF_PAYLOAD_OFFSET +
					   avtpc->ccds->ccr.max_frame_size +
					   sizeof(ccdbuf_data_info_t)) *
					  avtpc->ccds->ctrlbufp);
	}
	memcpy(cdata->payload+sizeof(avbtp_cm_control_header_t),
	       cd_payload, cd_size);
	cdata->info.size=cd_size+sizeof(avbtp_cm_control_header_t);
	cdata->info.timestamp=0;
	cmch=(avbtp_cm_control_header_t *)cdata->payload;
	avbtp_generate_control_cm_header(cmch, &avtpc->ccds->ccr.ht.preset_cmch, cd_size);

	if(avtpc->ccds->ccr.con_mode==AVTP_TALKONLY_CONTROL_CONNECTION){
		if(++avtpc->ccds->writep==avtpc->ccds->bufnum){avtpc->ccds->writep=0;}
		avtpc->ccds->bufused++;
		if(avtpc->ccds->bufused==avtpc->ccds->bufnum){while(CB_SEM_TRYWAIT(&avtpc->ccds->tsem_rel)==0) ;} //set semaphore to zero
	}

	CB_THREAD_MUTEX_UNLOCK(&avtpc->ccds->tmutex);
	CB_SEM_POST(&avtpc->ccds->tsem_set);
	return 0;
}

int avtpc_update_mcast_addr(avtpc_data_t *avtpc, ub_macaddr_t newaddr, bool wait_prev_done)
{
	if(!memcmp(avtpc->ccds->ccr.mcast_addr, newaddr, sizeof(ub_macaddr_t))){
		return 0;
	}
	if(wait_prev_done && avtpc->ccds->ccr.mcast_addr_update){
		CB_USLEEP(10000); // 10msec
		if(avtpc->ccds->ccr.mcast_addr_update) {
			UB_LOG(UBL_INFO,"%s:previous update is not done in 10msec\n",__func__);
			return -1;
		}
	}
	CB_THREAD_MUTEX_LOCK(&avtpc->ccds->tmutex);
	memcpy(avtpc->ccds->ccr.mcast_addr, newaddr, sizeof(ub_macaddr_t));
	avtpc->ccds->ccr.mcast_addr_update=true;
	CB_THREAD_MUTEX_UNLOCK(&avtpc->ccds->tmutex);
	return 0;
}

avbtp_cm_stream_header_t *avtpc_cmsh_in_ccd(avtpc_data_t *avtpc)
{
	return &avtpc->ccds->ccr.ht.preset_cmsh;
}

avbtp_cm_control_header_t *avtpc_cmch_in_ccd(avtpc_data_t *avtpc)
{
	return &avtpc->ccds->ccr.ht.preset_cmch;
}

int avtpc_rec_ts_check(avtpc_data_t *avtpc, avbtp_sd_info_t *rsdinfo)
{
	return avbtp_rec_ts_check(&avtpc->last_pts, rsdinfo);
}

int avtpc_set_tshaper(client_connect_request_t *ccr, uint32_t tshape_rate, int tshape_minsleep)
{
	ccr->tshape_rate=tshape_rate;
	ccr->tshape_minsleep=tshape_minsleep;
	return 0;
}

int avtpc_get_bufnum(avtpc_data_t *avtpc)
{
	/* We just read value, mutex is not needed */
	return avtpc->ccds->bufnum;
}

int avtpc_get_bufused(avtpc_data_t *avtpc)
{
	/* We just read value, mutex is not needed */
	return avtpc->ccds->bufused;
}

bool avtpc_rcv_thread_is_running(avtpc_data_t *avtpc)
{
	if (!avtpc || !avtpc->ccds)
		return false;
	return (avtpc->ccds->stop_thread == CC_THREAD_RUN);
}
