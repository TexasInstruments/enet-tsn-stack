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
#include <stdint.h>
#include <stdlib.h>
#include <tsn_unibase/unibase.h>
#include "mdeth.h"
#include "md_abnormal_hooks.h"
#include "gptpnet.h"
#include "gptpcommon.h"

extern char *PTPMsgType_debug[16];

typedef struct event_data {
	md_abn_event_t evd;
	int repeat_count;
	int interval_count;
} event_data_t;

typedef struct md_abnormal_data {
	ub_esarray_cstd_t *events;
	ub_esarray_cstd_t *tsfifo;
} md_abnormal_data_t;

#define MD_EVENT_ARRAY_EXPUNIT 2
#define MD_EVENT_ARRAY_MAXUNIT 16

static md_abnormal_data_t *gmdabnd;

static int event_by_rate(event_data_t *event)
{
	float v;

	if(event->evd.eventrate<1.0){
		v=(float)rand()/(float)RAND_MAX;
		if(v>event->evd.eventrate){return 0;}
	}
	return 1;
}

static int event_happen(event_data_t *event)
{
	if(!event->evd.repeat){return event_by_rate(event);} // if repeat=0, happen every time

	if(event->repeat_count == 0){
		event->repeat_count++;
		return event_by_rate(event);
	}

	if(!event->evd.interval){
		if(event->repeat_count++ < event->evd.repeat){return 1;}
		return 0;
	}

	if(event->interval_count++ < event->evd.interval){return 0;}
	event->interval_count=0;
	if(event->repeat_count++  < event->evd.repeat){return 1;}
	return 0;
}

static md_abn_eventp_t proc_event(event_data_t *event, uint8_t *dbuf, bool domain_aware)
{
	if(domain_aware &&
	   (event->evd.domainNumber != PTP_HEAD_DOMAIN_NUMBER(dbuf))){return MD_ABN_EVENTP_NONE;}
	switch(event->evd.eventtype){
	case MD_ABN_EVENT_SKIP:
		if(!event_happen(event)){break;}
		return MD_ABN_EVENTP_SKIP;
	case MD_ABN_EVENT_DUP:
		if(!event_happen(event)){break;}
		return MD_ABN_EVENTP_DUPLICATE;
	case MD_ABN_EVENT_BADSEQN:
		if(!event_happen(event)){break;}
		if(event->evd.eventpara1==0){
			dbuf[31]^=0xff; //invert lower 8 bits of SequenceID
		}else{
			uint16_t n,m;
			memcpy(&m, &dbuf[30], 2);
			n=ntohs(m)+event->evd.eventpara1;
			m=htons(n);
			memcpy(&dbuf[30], &m, 2);
		}
		return MD_ABN_EVENTP_MANIPULATE;
	case MD_ABN_EVENT_SENDERR:
		if(!event_happen(event)){break;}
		return MD_ABN_EVENTP_SENDERR;
	default:
		break;
	}
	return MD_ABN_EVENTP_NONE;
}

static md_abn_eventp_txts_t proc_txts_event(event_data_t *event, PTPMsgType msgtype, int ndevIndex,
                                       int domainNumber, event_data_txts_t *edtxts)
{
	int64_t *nts;
	switch(event->evd.eventtype){
	case MD_ABN_EVENT_TXTSMISSING:
		if(!event_happen(event)){break;}
		if(event->evd.msgtype!=msgtype){break;}
		if(event->evd.ndevIndex!=ndevIndex){break;}
		if((event->evd.eventpara1!=-1)&&(event->evd.eventpara1!=edtxts->seqid)){break;}
		return MD_ABN_EVENTP_TXTS_ERR;
	case MD_ABN_EVENT_TXTSBADFIFO:
		if(!event_happen(event)){break;}
		nts=(int64_t *)ub_esarray_get_newele(gmdabnd->tsfifo);
		if(!nts) {break;}
		*nts=edtxts->ts64;
		nts=(int64_t *)ub_esarray_get_ele(gmdabnd->tsfifo, 0);
		if(!nts) {break;}
		edtxts->ts64=*nts;
		(void)ub_esarray_del_index(gmdabnd->tsfifo, 0);
		return MD_ABN_EVENTP_TXTS_MANIPULATE;
	case MD_ABN_EVENT_TXTSOFFSET:
		if(!event_happen(event)){break;}
		if(event->evd.msgtype!=msgtype){break;}
		if(event->evd.ndevIndex!=ndevIndex){break;}
		if(event->evd.eventpara1!=-1){}
		if((event->evd.eventpara1!=-1)&&(event->evd.eventpara1!=edtxts->seqid)){break;}
		edtxts->ts64+=event->evd.eventpara2;
		return MD_ABN_EVENTP_TXTS_MANIPULATE;
	default:
		break;
	}
	return MD_ABN_EVENTP_TXTS_NONE;
}

void md_abnormal_init(void)
{
	if(gmdabnd!=NULL){md_abnormal_close();}

	gmdabnd=(md_abnormal_data_t *)UB_SD_GETMEM(GPTP_SMALL_ALLOC, sizeof(md_abnormal_data_t));
	if(ub_assert_fatal(gmdabnd, __func__, "malloc error")){return;}
	(void)memset(gmdabnd, 0, sizeof(md_abnormal_data_t));
	gmdabnd->events=ub_esarray_init(MD_EVENT_ARRAY_EXPUNIT, sizeof(event_data_t),
					MD_EVENT_ARRAY_MAXUNIT);

	gmdabnd->tsfifo=ub_esarray_init(MD_EVENT_ARRAY_EXPUNIT, sizeof(int64_t),
					MD_EVENT_ARRAY_MAXUNIT);
	return;
}

void md_abnormal_close(void)
{
	if(!gmdabnd){return;}
	if(gmdabnd->tsfifo!=NULL){ub_esarray_close(gmdabnd->tsfifo);}
	if(gmdabnd->events!=NULL){ub_esarray_close(gmdabnd->events);}
	UB_SD_RELMEM(GPTP_SMALL_ALLOC, gmdabnd);
	gmdabnd=NULL;
	return;
}

static int check_new_event_entry(md_abn_event_t *event){
	int i;
	int elen;
	event_data_t *nevent;

	switch(event->eventtype){
	case MD_ABN_EVENT_TXTSBADFIFO:
		// check if already registered
		elen=ub_esarray_ele_nums(gmdabnd->events);
		for(i=0;i<elen;i++) {
			nevent=(event_data_t *)ub_esarray_get_ele(gmdabnd->events, i);
			if(nevent && (nevent->evd.eventtype==event->eventtype)){
				return -1;
			}
		}
		if(event->eventpara1<=0){ return -1;} // eventpara1 must be greater than 0
		if(event->eventpara1>MD_EVENT_ARRAY_MAXUNIT-1){ return -1; } // limit eventpara1
		event->msgtype=MANAGEMENT; // ignored, set to anything not particular
		break;
	default:
		break;
	}

	if(event->msgtype>(PTPMsgType)15){return -1;}
	return 0;
}

static int proc_post_event_register(md_abn_event_t *event){
	int i;
	int64_t *nts;
	switch(event->eventtype){
	case MD_ABN_EVENT_TXTSBADFIFO:
		event->msgtype=MANAGEMENT; // ignored, set to anything not particular
		for(i=0;i<event->eventpara1;i++){
			nts=(int64_t *)ub_esarray_get_newele(gmdabnd->tsfifo);
			if (!nts) {return -1;}
			*nts=event->eventpara2;
		}
		break;
	default:
		break;
	}
	return 0;
}

int md_abnormal_register_event(md_abn_event_t *event)
{
	event_data_t *nevent;
	if(!gmdabnd){return -1;}
	if(check_new_event_entry(event)) {return -1;}

	nevent=(event_data_t *)ub_esarray_get_newele(gmdabnd->events);
	if(!nevent){return -1;}
	(void)memset(nevent, 0, sizeof(event_data_t));
	memcpy(&nevent->evd, event, sizeof(md_abn_event_t));
	UB_LOG(UBL_INFO, "%s:dn=%d, ni=%d, msgtype=%s, eventtype=%d,"
	       "eventrate=%f, repeat=%d interval=%d eventpara1=%d eventpara2=%"PRId64"\n",__func__,
	       event->domainNumber,
	       event->ndevIndex,
	       PTPMsgType_debug[event->msgtype],
	       event->eventtype,
	       event->eventrate,
	       event->repeat,
	       event->interval,
	       event->eventpara1,
	       event->eventpara2);
	if(proc_post_event_register(event)) {return -1;}
	return 0;
}

int md_abnormal_deregister_all_events(void)
{
	int i;
	int elen;
	if(!gmdabnd){return -1;}
	UB_LOG(UBL_DEBUG, "%s:\n",__func__);

	elen=ub_esarray_ele_nums(gmdabnd->tsfifo);
	for(i=elen-1;i>=0;i--){(void)ub_esarray_del_index(gmdabnd->tsfifo, i);}

	elen=ub_esarray_ele_nums(gmdabnd->events);
	for(i=elen-1;i>=0;i--){(void)ub_esarray_del_index(gmdabnd->events, i);}

	return 0;
}


int md_abnormal_deregister_event(md_abn_event_type eventtype)
{
	int i, j;
	event_data_t *event;
	int elen, tselen;
	if(!gmdabnd){return -1;}
	UB_LOG(UBL_DEBUG, "%s:eventtype=%d\n",__func__, eventtype);
	elen=ub_esarray_ele_nums(gmdabnd->events);
	for(i=elen-1;i>=0;i--) {
		event=(event_data_t *)ub_esarray_get_ele(gmdabnd->events, i);
		if(event && (event->evd.eventtype==eventtype)){
			// clear tsfifo if MD_ABN_EVENT_TXTSBADFIFO event is deregistered
			if(eventtype==MD_ABN_EVENT_TXTSBADFIFO){
				tselen=ub_esarray_ele_nums(gmdabnd->tsfifo);
				for(j=tselen-1;j>=0;j--){(void)ub_esarray_del_index(gmdabnd->tsfifo, j);}
			}
			(void)ub_esarray_del_index(gmdabnd->events, i);
		}
	}
	return 0;
}

md_abn_eventp_t md_abnormal_gptpnet_send_hook(gptpnet_data_t *gpnet, int ndevIndex,
					      uint16_t length)
{
	int i;
	md_abn_eventp_t res=MD_ABN_EVENTP_NONE;
	event_data_t *event;
	int elen;
	uint8_t *dbuf;
	PTPMsgType msgtype;
	if(!gmdabnd){return res;}
	dbuf=gptpnet_get_sendbuf(gpnet, ndevIndex);
	msgtype=(PTPMsgType)PTP_HEAD_MSGTYPE(dbuf);
	elen=ub_esarray_ele_nums(gmdabnd->events);
	for(i=0;i<elen;i++) {
		event=(event_data_t *)ub_esarray_get_ele(gmdabnd->events, i);
		if(!event){continue;}
		if(event->evd.msgtype!=msgtype){continue;}
		if(event->evd.ndevIndex!=ndevIndex){continue;}
		switch(msgtype){
		case SYNC:
			res=proc_event(event, dbuf, true);
			break;
		case PDELAY_REQ:
			res=proc_event(event, dbuf, false);
			break;
		case PDELAY_RESP:
			res=proc_event(event, dbuf, false);
			break;
		case FOLLOW_UP:
			res=proc_event(event, dbuf, true);
			break;
		case PDELAY_RESP_FOLLOW_UP:
			res=proc_event(event, dbuf, false);
			break;
		case ANNOUNCE:
			res=proc_event(event, dbuf, true);
			break;
		case SIGNALING:
			res=proc_event(event, dbuf, true);
			break;
		case DELAY_REQ:
		case DELAY_RESP:
		case MANAGEMENT:
		default:
			break;
		}
	}
	return res;
}

int md_abnormal_timestamp(PTPMsgType msgtype, int ndevIndex, int domainNumber,
                          event_data_txts_t *edtxts)
{
	int i, elen;
	event_data_t *event;
	uint8_t res=(uint8_t)MD_ABN_EVENTP_TXTS_NONE;

	if(!gmdabnd){return 0;}
	elen=ub_esarray_ele_nums(gmdabnd->events);
	for(i=0;i<elen;i++) {
		event=(event_data_t *)ub_esarray_get_ele(gmdabnd->events, i);
		if(!event){continue;}
		res|=(uint8_t)proc_txts_event(event, msgtype, ndevIndex, domainNumber, edtxts);
	}
	if(res&MD_ABN_EVENTP_TXTS_ERR){
		return -1;
	}
	return 0;
}
