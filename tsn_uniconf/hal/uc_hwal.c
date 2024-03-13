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
#include <math.h>
#include <tsn_combase/combase.h>
#include "../yangs/yang_modules.h"
#include "../yangs/ietf-interfaces.h"
#include "../yangs/ietf-interfaces_access.h"
#include "../yangs/ieee802-dot1q-bridge.h"
#include "../yangs/ieee802-dot1q-bridge_access.h"
#include "../uc_notice.h"
#include "uc_hwal.h"
#include <tsn_combase/combase_link.h>

extern uint8_t IETF_INTERFACES_func(uc_dbald *dbald);
#define IETF_INTERFACES_RW IETF_INTERFACES_func(dbald)
#define IETF_INTERFACES_RO (IETF_INTERFACES_func(dbald)|0x80u)
#define IETF_INTERFACES_RW_H IETF_INTERFACES_func(hwald->dbald)
#define IETF_INTERFACES_RO_H (IETF_INTERFACES_func(hwald->dbald)|0x80u)

extern uint8_t IEEE802_DOT1Q_BRIDGE_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_BRIDGE_RW_H IEEE802_DOT1Q_BRIDGE_func(hwald->dbald)
#define IEEE802_DOT1Q_BRIDGE_RO_H (IEEE802_DOT1Q_BRIDGE_func(hwald->dbald)|0x80u)

struct uc_hwald {
	combase_link_data_t *hwctx;
	uc_dbald *dbald;
	uc_notice_data_t *ucntd;
	uint8_t operating_tc;
	CB_THREAD_T catch_event;
	cbl_query_thread_data_t cqtd;
};

static int notice_cb(void *cbdata, cbl_cb_event_t *nevent);

/*
 * read idlesslpe, max_frame_size, max_interference_size of this class
 * calculate, sendslope, hicredit, locredit,
 * and max_interference_size of one lower TC.
 * idlesslpe, max_frame_size must be provided
 * max_interference_size is calculated from upper most TC to the lower TC.
 * To get right values, this must be called from the upper most TC to lower
 * by the reverse order of TC.
 */
static int get_tc_cbs_parameters(cbl_cbs_params_t *cbsp, uint8_t tc,
				 uint64_t admin_idleslope)
{
	uint8_t num_tc=0;
	uint32_t maxifsize=0;
	uint32_t maxfsize=0;
	uint32_t lc_maxifsize;
	void *value;
	uint64_t speed=0;

	if(!cbsp){return -1;}
	cbsp->idleslope=0;
	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, num_tc, value,
			     (char*)cbsp->ifname,
			     IETF_INTERFACES_TRAFFIC_CLASS,
			     IETF_INTERFACES_TRAFFIC_CLASS_TABLE,
			     IETF_INTERFACES_NUMBER_OF_TRAFFIC_CLASSES, 255,
			     NULL, 0, YDBI_CONFIG);
	if(tc>=num_tc){return -1;}
	if(tc==0){return 0;} // no need to calculate for TC0

	// speed in bps unit
	YDBI_GET_ITEM_VSUBST(uint64_t*, ifk1vk0, speed, value,
			     (char*)cbsp->ifname,
			     IETF_INTERFACES_SPEED, YDBI_STATUS);
	if(speed==0){
		UB_LOG(UBL_ERROR, "%s:%s, speed is 0, Link may Down\n",
		       __func__, cbsp->ifname);
		return -1;
	}

	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk4vk1, maxfsize, value,
			     (char*)cbsp->ifname,
			     IETF_INTERFACES_TRAFFIC_CLASS,
			     IETF_INTERFACES_TC_DATA,
			     IETF_INTERFACES_MAX_FRAME_SIZE, 255,
			     &tc, 1, YDBI_STATUS);
	maxfsize+=CBL_L2_OVERHEAD;

	if(tc==num_tc-1){
		maxifsize=maxfsize;
		YDBI_SET_ITEM(ifk4vk1, (char*)cbsp->ifname,
			      IETF_INTERFACES_TRAFFIC_CLASS,
			      IETF_INTERFACES_TC_DATA,
			      IETF_INTERFACES_MAX_INTERFERENCE_SIZE, 255,
			      &tc, 1, YDBI_STATUS,
			      &maxifsize, sizeof(uint32_t), YDBI_NO_NOTICE,
			      YANG_DB_ONHW_NOACTION);
		UB_LOG(UBL_DEBUG, "%s:%s, tc=%d, maxifsize=%d\n",
		       __func__, cbsp->ifname, tc, maxifsize);
	}else{
		// this tc's maxifsize is already there, by a call of upper class
		YDBI_GET_ITEM_VSUBST(uint32_t*, ifk4vk1, maxifsize, value,
				     (char*)cbsp->ifname,
				     IETF_INTERFACES_TRAFFIC_CLASS,
				     IETF_INTERFACES_TC_DATA,
				     IETF_INTERFACES_MAX_INTERFERENCE_SIZE, 255,
				     &tc, 1, YDBI_STATUS);
	}
	cbsp->idleslope=admin_idleslope;
	cbsp->sendslope=cbsp->idleslope-speed; // this is negative
	cbsp->locredit=maxfsize*cbsp->sendslope/(double)speed;
	cbsp->hicredit=maxifsize*cbsp->idleslope/(double)speed;
	lc_maxifsize=ceil(maxifsize-speed*(cbsp->hicredit-cbsp->locredit)/
			       (double)cbsp->sendslope);
	UB_LOG(UBL_DEBUG, "%s:%s, tc=%d, idleslope=%"PRIi64", sendslope=%"PRIi64
	       ", hic=%"PRIi64", loc=%"PRIi64", maxifsize=%d(tc=%d), maxfsize=%d\n",
	       __func__, cbsp->ifname, tc, cbsp->idleslope, cbsp->sendslope,
	       cbsp->hicredit, cbsp->locredit, lc_maxifsize, tc-1, maxfsize);
	tc--;
	YDBI_SET_ITEM(ifk4vk1, (char*)cbsp->ifname,
		      IETF_INTERFACES_TRAFFIC_CLASS,
		      IETF_INTERFACES_TC_DATA,
		      IETF_INTERFACES_MAX_INTERFERENCE_SIZE, 255,
		      &tc, 1, YDBI_STATUS,
		      &lc_maxifsize, sizeof(uint32_t), YDBI_NO_NOTICE,
		      YANG_DB_ONHW_NOACTION);
	return 0;
}

/* traffic class initialization happesn by writing to 'cbs-enabled'.
 * All the needed parameters must be set up before writing to 'cbs-enabled'.
 * Currently we don't support 'list available-traffic-class', and
 * set up the initialization parameters as static values.
 * the pysical queue related parameters must be better to set up
 * by probing the hardware, but we use conifguration for now.
 *
 * priority to TC, IEEE802.1Q Table 34-1
 * TC doesn't always match to logical queue number. It may be TC:0,1,2 -> LQ:2,1,0
 * LQ must be mapped to physical queue to configure HW FQTSS.
 */
#define QDISC_HANDLE_NUMBER 100 // use a hardcoded value for now
static int get_queue_map_params(cbl_qmap_params_t *qmap, const char *ifname)
{
	uint8_t tc=0, lq=0, pi;
	void *value;
	qmap->handle=QDISC_HANDLE_NUMBER;
	qmap->num_tc=0;
	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, qmap->num_tc, value,
			     (char*)ifname, IETF_INTERFACES_TRAFFIC_CLASS,
			     IETF_INTERFACES_TRAFFIC_CLASS_TABLE,
			     IETF_INTERFACES_NUMBER_OF_TRAFFIC_CLASSES, 255,
			     NULL, 0, YDBI_CONFIG);
	UB_LOG(UBL_INFO, "%s:netdev=%s, num_tc=%d\n",
	       __func__, ifname, qmap->num_tc);
	if(qmap->num_tc<=1){return 1;}
	// priority to logical queue map
	for(pi=0;pi<8;pi++){
		YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, tc, value, (char*)ifname,
				     IETF_INTERFACES_TRAFFIC_CLASS,
				     IETF_INTERFACES_TRAFFIC_CLASS_TABLE,
				     IETF_INTERFACES_PRIORITY0+pi,
				     255, NULL, 0, YDBI_CONFIG);
		YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, lq, value, (char*)ifname,
				     IETF_INTERFACES_TRAFFIC_CLASS,
				     IETF_INTERFACES_TC_DATA,
				     IETF_INTERFACES_LQUEUE,
				     255, &tc, 1, YDBI_STATUS);
		qmap->pri_lq_map[pi]=lq;
	}
	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, qmap->num_pq, value, (char*)ifname,
			     IETF_INTERFACES_TRAFFIC_CLASS,
			     IETF_INTERFACES_NUMBER_OF_PQUEUES,
			     255, 255, NULL, 0, YDBI_STATUS);
	lq=0;
	// physical queue to logical queue map
	for(pi=0;pi<qmap->num_pq;pi++){
		YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, lq, value, (char*)ifname,
				     IETF_INTERFACES_TRAFFIC_CLASS,
				     IETF_INTERFACES_PQUEUE_MAP,
				     IETF_INTERFACES_LQUEUE,
				     255, &pi, 1, YDBI_STATUS);
		qmap->pq_lq_map[pi]=lq;
	}
	return 0;
}

static int init_traffic_classes(uc_hwald *hwald, cbl_cb_event_t *nevent, bool delete_flg)
{
	cbl_tcinit_params_t tip;
	memset(&tip, 0, sizeof(tip));
	tip.action=delete_flg?CBL_ACTION_DEL:CBL_ACTION_SET;
	if(get_queue_map_params(&tip.qmap, nevent->ifname)!=0){
		return 0; // no traffic class, no actions are needed
	}
	tip.ifname=nevent->ifname;
	return cbl_tc_queue_map(hwald->hwctx, &tip);
}

static int update_cbs_idle_slope(combase_link_data_t *hwctx, const char *ifname,
				 uint8_t tc, int64_t admin_idleslope)
{
	void *value;
	uint8_t lq, pi, num_pq, mlq;
	cbl_cbs_params_t cbsp;
	if(tc==0){return -1;} // no settings for tc=0
	memset(&cbsp, 0, sizeof(cbsp));
	cbsp.ifname=ifname;
	cbsp.handle=QDISC_HANDLE_NUMBER;
	cbsp.offload=1; // assume HW offload is supported
	// when admin_idleslope==0, use CBL_ACTION_DEL to delete the cbs queue
	cbsp.action=(admin_idleslope<=0)?CBL_ACTION_DEL:CBL_ACTION_SET;

	if(get_tc_cbs_parameters(&cbsp, tc, admin_idleslope)){
		UB_LOG(UBL_ERROR, "%s:tc=%d, no cbs parameters\n", __func__, tc);
		return -1;
	}
	// get num_pq:number of pq(physical queues)
	num_pq=0xff;
	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, num_pq, value, (char*)ifname,
			     IETF_INTERFACES_TRAFFIC_CLASS,
			     IETF_INTERFACES_NUMBER_OF_PQUEUES,
			     255, 255, NULL, 0, YDBI_STATUS);
	// tc -> lq(logical queue)
	lq=0xff;
	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, lq, value,
			     (char*)ifname,
			     IETF_INTERFACES_TRAFFIC_CLASS,
			     IETF_INTERFACES_TC_DATA,
			     IETF_INTERFACES_LQUEUE, 255,
			     &tc, 1, YDBI_STATUS);
	if((lq==0xff) || (num_pq==0xff)){
		UB_LOG(UBL_ERROR, "%s:can't find lqueue for tc=%d\n", __func__, tc);
		return -1;
	}
	/* scan pi=0,1,,(physical queue index), get the mapped mlq(logical queue)
	   if mlq(mapped logical queue) matches to pi(pysical queue index):mlq==lq,
	   pi(pysical queue index) is the queue to configure.
	   'pysical queue index' starts from '0', lower layer may add '1' when it is indexed
	   starting with '1'
	*/
	for(pi=0;pi<num_pq;pi++){
		mlq=0xff;
		YDBI_GET_ITEM_VSUBST(uint8_t*, ifk4vk1, mlq, value, (char*)ifname,
				     IETF_INTERFACES_TRAFFIC_CLASS,
				     IETF_INTERFACES_PQUEUE_MAP,
				     IETF_INTERFACES_LQUEUE,
				     255, &pi, 1, YDBI_STATUS);
		if(mlq==lq){
			cbsp.qindex=pi;
			break;
		}
	}
	if(pi==num_pq){
		UB_LOG(UBL_ERROR, "%s:can't find pqueue for tc=%d\n", __func__, tc);
		return -1;
	}
	UB_LOG(UBL_INFO, "%s:cbs setup tc=%d, lqueue=%d, pqueue=%d\n",
	       __func__, tc, lq, cbsp.qindex);
	return cbl_cbs_setup(hwctx, &cbsp);
}

static int set_netdev_linkstatus(uc_hwald *hwald, cbl_cb_event_t *nevent)
{
	uint8_t aps[5]={IETF_INTERFACES_RO_H, IETF_INTERFACES_INTERFACES,
			IETF_INTERFACES_INTERFACE, IETF_INTERFACES_IF_INDEX, 255};
	void *kvs[2]={NULL,NULL};
	uint8_t kss[1];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE, YANG_DB_ONHW_NOACTION,
				      NULL, aps, kvs, kss, NULL, 0};
	uint8_t value[8];
	int32_t res, oper_status;
	char *emes="";

	UB_LOG(UBL_DEBUG, "%s:ifname=%s\n", __func__, nevent->ifname);
	kvs[0]=nevent->ifname;
	kss[0]=strlen(nevent->ifname)+1;
	// update IETF_INTERFACES_IF_INDEX
	dbpara.vsize=4;
	dbpara.value=&nevent->ifindex;
	res=yang_db_action(hwald->dbald, NULL, &dbpara);
	if(res){
		emes="IF_INDEX";
		goto erexit;
	}
	// update IETF_INTERFACES_PHYS_ADDRESS
	aps[3]=IETF_INTERFACES_PHYS_ADDRESS;
	dbpara.vsize=6;
	dbpara.value=nevent->u.linkst.address;
	res=yang_db_action(hwald->dbald, NULL, &dbpara);
	if(res){
		emes="PHYS_ADDRESS";
		goto erexit;
	}
	// update IETF_INTERFACES_OPER_STATUS
	aps[3]=IETF_INTERFACES_OPER_STATUS;
	if(nevent->eventflags&CBL_EVENT_DEVUP){
		oper_status=1; //up
	}else{
		oper_status=2; //down
	}
	UB_LOG(UBL_DEBUG, "%s:IETF_INTERFACES_OPER_STATUS %s, speed=%d, duplex=%s\n",
	       __func__, (oper_status==1)?"UP":"DOWN", nevent->u.linkst.speed,
	       (nevent->u.linkst.duplex==1)?"full":"not-full");
	dbpara.vsize=4;
	dbpara.value=&oper_status;
	res=yang_db_action(hwald->dbald, NULL, &dbpara);
	if(res){
		emes="OPER_STATUS";
		goto erexit;
	}
	if(oper_status==1){
		// update speed and duplex only at UP time
		// update IETF_INTERFACES_SPEED
		aps[3]=IETF_INTERFACES_SPEED;
		*((uint64_t*)value)=nevent->u.linkst.speed*1000000LL;
		dbpara.vsize=8;
		dbpara.value=value;
		res=yang_db_action(hwald->dbald, NULL, &dbpara);
		if(res){
			emes="SPEED";
			goto erexit;
		}
		// update IETF_INTERFACES_DUPLEX
		aps[3]=IETF_INTERFACES_DUPLEX;
		res=nevent->u.linkst.duplex; // 1:full, 2:half, 3:unknown
		dbpara.vsize=4;
		dbpara.value=&res;
		res=yang_db_action(hwald->dbald, NULL, &dbpara);
		if(res){
			emes="DUPLEX";
			goto erexit;
		}
	}

	// notice only OPER_STATUS, uc_client will read other status by this notice
	aps[3]=IETF_INTERFACES_OPER_STATUS;
	uc_nu_putnotice_push(hwald->ucntd, hwald->dbald, aps, kvs, kss);
erexit:
	if(res){
		UB_LOG(UBL_ERROR, "%s:error, %s\n", __func__, emes);
		return -1;
	}
	return 0;
}

static int tc_hw_action(uc_hwald *hwald, const char *ifname, uint8_t *cbs_enabled)
{
	int res;
	cbl_cb_event_t nevent;
	bool delete_flg;
	if((ifname==NULL) || (cbs_enabled==NULL)){
		UB_LOG(UBL_ERROR, "%s:no value\n", __func__);
		return -1;
	}
	memset(&nevent, 0, sizeof(nevent));
	memcpy(nevent.ifname, ifname, UB_MIN(CB_MAX_NETDEVNAME-1, strlen(ifname)));
	if(*cbs_enabled==0){
		delete_flg=true;
	}else if(*cbs_enabled==1){
		delete_flg=false;
	}else{
		return -1;
	}
	res=init_traffic_classes(hwald, &nevent, delete_flg);
	if(res<1) return res;
	nevent.eventflags=(!delete_flg)?
		CBL_EVENT_TCHW_ENABLED:CBL_EVENT_TCHW_DISABLED;
	return notice_cb(hwald, &nevent);
}

static int cbs_hw_action(uc_hwald *hwald, const char *ifname, uint8_t *tc,
			 int64_t *admin_idleslope)
{
	int res;
	cbl_cb_event_t nevent;
	if((ifname==NULL) || (tc==NULL) || (admin_idleslope==NULL)){
		UB_LOG(UBL_ERROR, "%s:no value\n", __func__);
		return -1;
	}
	UB_LOG(UBL_DEBUG, "%s:cbs setup %s, tc=%d\n", __func__, ifname, *tc);
	hwald->operating_tc=*tc;
	res=update_cbs_idle_slope(hwald->hwctx, ifname, *tc, *admin_idleslope);
	if(res<1) return res;
	memset(&nevent, 0, sizeof(nevent));
	memcpy(nevent.ifname, ifname,
	       UB_MIN(CB_MAX_NETDEVNAME-1, strlen(ifname)));
	nevent.eventflags=(*admin_idleslope>=0)?
		CBL_EVENT_CBS_ENABLED:CBL_EVENT_CBS_DISABLED;
	return notice_cb(hwald, &nevent);
}

static uc_range *get_gate_entry_range(uc_dbald *dbald, const char *ifname)
{
	uc_range *range;
	uint8_t abs[9+1+CB_MAX_NETDEVNAME]={
		IETF_INTERFACES_RW, IETF_INTERFACES_INTERFACES,
		IETF_INTERFACES_INTERFACE, IETF_INTERFACES_BRIDGE_PORT,
		IETF_INTERFACES_GATE_PARAMETER_TABLE, IETF_INTERFACES_ADMIN_CONTROL_LIST,
		IETF_INTERFACES_GATE_CONTROL_ENTRY, IETF_INTERFACES_OPERATION_NAME,
		255};
	uint8_t bbs[9+1+CB_MAX_NETDEVNAME];
	const uint32_t ksize=9+1+strlen(ifname)+1;
	if(ksize>9+1+CB_MAX_NETDEVNAME){return NULL;}// shouldn't happen
	abs[9]=strlen(ifname)+1;
	memcpy(&abs[10], ifname, strlen(ifname)+1);
	memcpy(bbs, abs, sizeof(bbs));
	bbs[ksize-1]+=1;
	range=uc_get_range(dbald, abs, ksize, bbs, ksize);
	if(!range){
		UB_LOG(UBL_WARN, "%s:no gate schedule entry\n", __func__);
		return NULL;
	}
	return range;
}

static int set_oper_gate_schedules(uc_dbald *dbald, const char *ifname, bool enabled)
{
	uint32_t index;
	void *nkey, *value;
	uint8_t aps[9+1+CB_MAX_NETDEVNAME];
	uint32_t nksize, vsize;
	const uint32_t ksize=9+1+strlen(ifname)+1;//node key=9, 1 delimiter
	uc_range *range=get_gate_entry_range(dbald, ifname);
	if(!range){return 0;}
	while(true){
		if(uc_get_keyvalue_in_range(dbald, range, &nkey, &nksize, &value,
					    &vsize, UC_DBAL_FORWARD)!=0){break;}
		if(nksize<ksize+5){continue;} // need an index key
		if(nksize>sizeof(aps)){continue;}
		memcpy(aps, nkey, nksize);
		if(aps[ksize]!=sizeof(uint32_t)){continue;}// index is uint32_t
		index=*((uint32_t*)&aps[ksize+1]);

		// OPERATION_NAME, read from admin write to oper
		aps[0]=IETF_INTERFACES_RO;
		aps[5]=IETF_INTERFACES_OPER_CONTROL_LIST;
		//aps[7]=IETF_INTERFACES_OPERATION_NAME; read is already done
		if(enabled){
			(void)uc_dbal_create(dbald, aps, nksize, value, vsize);
		}else{
			(void)uc_dbal_del(dbald, aps, nksize);
		}

		// TIME_INTERVAL_VALUE, read from admin write to oper
		aps[0]=IETF_INTERFACES_RW;
		aps[5]=IETF_INTERFACES_ADMIN_CONTROL_LIST;
		aps[7]=IETF_INTERFACES_TIME_INTERVAL_VALUE;
		if(uc_dbal_get(dbald, aps, nksize, &value, &vsize)){
			UB_LOG(UBL_ERROR, "%s:no time-interval-value\n", __func__);
			continue;
		}
		aps[0]=IETF_INTERFACES_RO;
		aps[5]=IETF_INTERFACES_OPER_CONTROL_LIST;
		if(enabled){
			(void)uc_dbal_create(dbald, aps, nksize, value, vsize);
		}else{
			(void)uc_dbal_del(dbald, aps, nksize);
		}
		aps[0]=IETF_INTERFACES_RW;
		aps[5]=IETF_INTERFACES_ADMIN_CONTROL_LIST;
		(void)uc_dbal_get_release(dbald, aps, nksize, value, vsize);

		// GATE_STATES_VALUE, read from admin write to oper
		aps[7]=IETF_INTERFACES_GATE_STATES_VALUE;
		if(uc_dbal_get(dbald, aps, nksize, &value, &vsize)){
			UB_LOG(UBL_ERROR, "%s:no gate-states-value\n", __func__);
			continue;
		}
		aps[0]=IETF_INTERFACES_RO;
		aps[5]=IETF_INTERFACES_OPER_CONTROL_LIST;
		if(enabled){
			(void)uc_dbal_create(dbald, aps, nksize, value, vsize);
		}else{
			(void)uc_dbal_del(dbald, aps, nksize);
		}
		aps[0]=IETF_INTERFACES_RW;
		aps[5]=IETF_INTERFACES_ADMIN_CONTROL_LIST;
		(void)uc_dbal_get_release(dbald, aps, nksize, value, vsize);

		UB_LOG(UBL_DEBUG, "%s:entry index=%d enabled=%d\n", __func__,
		       index, enabled);
	}
	uc_get_range_release(dbald, range);
#if !UB_LOG_IS_COMPILED(UBL_DEBUG)
	(void)index;
#endif
	return 0;
}

static int update_admin2oper(uc_dbald *dbald, uint8_t *aps, void **kvs, uint8_t *kss,
			     uint8_t apadmin, uint8_t apoper, uint8_t opindex,
			     bool enabled)
{
	int res;
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
		NULL, aps, kvs, kss, NULL, 0};
	// read admin
	aps[opindex]=apadmin;
	res=yang_db_action(dbald, NULL, &dbpara);
	if(res){
		UB_LOG(UBL_ERROR, "%s:read admin failed\n", __func__);
		return -1;
	}
	// write oper
	aps[0]=IETF_INTERFACES_RO;
	aps[opindex]=apoper;
	res=0;
	if(enabled){
		dbpara.atype=YANG_DB_ACTION_CREATE;
		// fail to update is not okay
		res=yang_db_action(dbald, NULL, &dbpara);
	}else{
		dbpara.atype=YANG_DB_ACTION_DELETE;
		// fail to delete is okay, the data may not exist
		(void)yang_db_action(dbald, NULL, &dbpara);
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	aps[0]=IETF_INTERFACES_RW;
	aps[opindex]=apadmin;
	yang_db_action(dbald, NULL, &dbpara);
	return res;
}

static int set_tas_status(uc_hwald *hwald, cbl_cb_event_t *nevent)
{
	uint8_t aps[]={
		IETF_INTERFACES_RW_H, IETF_INTERFACES_INTERFACES,
		IETF_INTERFACES_INTERFACE, IETF_INTERFACES_BRIDGE_PORT,
		IETF_INTERFACES_GATE_PARAMETER_TABLE, IETF_INTERFACES_ADMIN_BASE_TIME,
		IETF_INTERFACES_SECONDS, 255};
	void *kvs[2]={nevent->ifname, NULL};
	uint8_t kss[1]={strlen(nevent->ifname)+1};
	bool enabled=(nevent->eventflags&CBL_EVENT_TAS_ENABLED)!=0;

	// update admin-control-list -> oper-control-list
	if(set_oper_gate_schedules(hwald->dbald, nevent->ifname, enabled)){
		return -1;
	}

	// update admin-base-time -> oper-base-time seconds
	if(update_admin2oper(hwald->dbald, aps, kvs, kss,
			     IETF_INTERFACES_ADMIN_BASE_TIME,
			     IETF_INTERFACES_OPER_BASE_TIME, 5, enabled)!=0){
		return -1;
	}
	// update admin-base-time -> oper-base-time nanoseconds
	aps[6]=IETF_INTERFACES_NANOSECONDS;
	if(update_admin2oper(hwald->dbald, aps, kvs, kss,
			     IETF_INTERFACES_ADMIN_BASE_TIME,
			     IETF_INTERFACES_OPER_BASE_TIME, 5, enabled)!=0){
		return -1;
	}
	// update admin-cycle-time -> oper-cycle-time numerator
	aps[6]=IETF_INTERFACES_NUMERATOR;
	if(update_admin2oper(hwald->dbald, aps, kvs, kss,
			     IETF_INTERFACES_ADMIN_CYCLE_TIME,
			     IETF_INTERFACES_OPER_CYCLE_TIME, 5, enabled)!=0){
		return -1;
	}
	// update admin-cycle-time -> oper-cycle-time denominator
	aps[6]=IETF_INTERFACES_DENOMINATOR;
	if(update_admin2oper(hwald->dbald, aps, kvs, kss,
			     IETF_INTERFACES_ADMIN_CYCLE_TIME,
			     IETF_INTERFACES_OPER_CYCLE_TIME, 5, enabled)!=0){
		return -1;
	}
	return uc_nu_askaction_push(hwald->ucntd, hwald->dbald, aps, kvs, kss);
}

static int set_preempt_status(uc_hwald *hwald, cbl_cb_event_t *nevent)
{
	UB_TLOG(UBL_DEBUG, "%s:ifname=%s, preempt_active=%d\n", __func__,
		nevent->ifname, nevent->u.preempt.preempt_active);

	YDBI_SET_ITEM(ifk4vk1, nevent->ifname,
		      IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS,
		      IETF_INTERFACES_HOLD_ADVANCE,
		      255, 255, NULL, 0, YDBI_STATUS,
		      &nevent->u.preempt.hold_advance, sizeof(uint32_t),
		      YDBI_NO_NOTICE, YANG_DB_ONHW_NOACTION);
	YDBI_SET_ITEM(ifk4vk1, nevent->ifname,
		      IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS,
		      IETF_INTERFACES_RELEASE_ADVANCE,
		      255, 255, NULL, 0, YDBI_STATUS,
		      &nevent->u.preempt.release_advance, sizeof(uint32_t),
		      YDBI_NO_NOTICE, YANG_DB_ONHW_NOACTION);
	YDBI_SET_ITEM(ifk4vk1, nevent->ifname,
		      IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS,
		      IETF_INTERFACES_HOLD_REQUEST,
		      255, 255, NULL, 0, YDBI_STATUS,
		      &nevent->u.preempt.hold_request, sizeof(uint32_t),
		      YDBI_NO_NOTICE, YANG_DB_ONHW_NOACTION);
	// a notice on IETF_INTERFACES_PREEMPTION_ACTIVE
	YDBI_SET_ITEM(ifk4vk1, nevent->ifname,
		      IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS,
		      IETF_INTERFACES_PREEMPTION_ACTIVE,
		      255, 255, NULL, 0, YDBI_STATUS,
		      &nevent->u.preempt.preempt_active, sizeof(uint8_t),
		      YDBI_PUSH_NOTICE_IN_UNICONF, YANG_DB_ONHW_NOACTION);
	return 0;
}

/*
 * /ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/gate-enabled
 * when this 'gate-enabled' is set, all admin-* parameters are read here, and pass
 * the parameters to HW configuration.
 * When the HW configuration returns a success with a callback, admin-* items
 * are copied to oper-*. at that time, set a notice on 'oper-cycle-time' denominator.
 *
 * when it is disabled, stop the HW operation.
 * When the HW configuration returns a success with a callback, oper-* items are
 * cleaned up. set a notice on oper-cycle-time.
 */
#define MAX_TAS_GATE_SCHEDULES 100
static int get_gate_schedules(uc_hwald *hwald, ub_esarray_cstd_t *clst, const char *ifname)
{
	uint32_t index;
	cbl_tas_gate_cmd_entry_t entry;
	void *nkey, *value;
	uint32_t nksize, vsize;
	uint8_t aps[9+1+CB_MAX_NETDEVNAME];
	uc_range *range=get_gate_entry_range(hwald->dbald, ifname);
	const uint32_t ksize=9+1+strlen(ifname)+1;//node key=9, 1 delimiter
	if(!range){
		UB_LOG(UBL_WARN, "%s:no gate schedule entry\n", __func__);
		return 0;
	}
	while(true){
		if(uc_get_keyvalue_in_range(hwald->dbald, range, &nkey, &nksize, &value,
					    &vsize, UC_DBAL_FORWARD)!=0){break;}
		if(nksize<ksize+5){continue;} // need an index key
		if(nksize>sizeof(aps)){continue;}
		memcpy(aps, nkey, nksize);
		if(aps[ksize]!=sizeof(uint32_t)){continue;}
		memset(&entry, 0, sizeof(entry));
		index=*((uint32_t*)&aps[ksize+1]);
		entry.operation=*((uint32_t*)value);
		if(entry.operation>CB_TAS_GATE_SET_RELEASE){
			UB_LOG(UBL_ERROR, "%s:invalid operation=%d\n",
			       __func__, entry.operation);
			continue;
		}

		aps[7]=IETF_INTERFACES_TIME_INTERVAL_VALUE;
		if(uc_dbal_get(hwald->dbald, aps, nksize, &value, &vsize)){
			UB_LOG(UBL_ERROR, "%s:no time-interval-value\n", __func__);
			continue;
		}
		entry.interval=*((uint32_t*)value);
		(void)uc_dbal_get_release(hwald->dbald, aps, nksize, value, vsize);

		aps[7]=IETF_INTERFACES_GATE_STATES_VALUE;
		if(uc_dbal_get(hwald->dbald, aps, nksize, &value, &vsize)){
			UB_LOG(UBL_ERROR, "%s:no gate-states-value\n", __func__);
			continue;
		}
		entry.gate=*((uint8_t*)value);
		(void)uc_dbal_get_release(hwald->dbald, aps, nksize, value, vsize);

		if(ub_esarray_add_ele(clst, (ub_esarray_element_t*)&entry)){
			UB_LOG(UBL_ERROR, "%s:ub_esarray_add_ele failed\n", __func__);
			return -1;
		}
		UB_LOG(UBL_DEBUG, "%s:gate schedule %d:operation=%d interval=%d gate=0x%x\n",
		       __func__, index, entry.operation, entry.interval, entry.gate);
	}
	uc_get_range_release(hwald->dbald, range);
#if !UB_LOG_IS_COMPILED(UBL_DEBUG)
	(void)index;
#endif
	return ub_esarray_ele_nums(clst);
}

static int tas_hw_action(uc_hwald *hwald, const char *ifname, uint8_t *enabled)
{
	cbl_tas_sched_params_t ctsp;
	ub_esarray_cstd_t *clst;
	void *value;
	int res;
	cbl_cb_event_t nevent;
	if((enabled==NULL) || !ifname){
		UB_LOG(UBL_ERROR, "%s:no ifname or value\n", __func__);
		return -1;
	}
	memset(&ctsp, 0, sizeof(ctsp));
	if(get_queue_map_params(&ctsp.qmap, ifname)) return 0;
	ctsp.ifname=ifname;
	if(*enabled==0){
		ctsp.action=CBL_ACTION_DEL;
		res=cbl_tas_setup(hwald->hwctx, &ctsp);
		if(res<1){return res;}
		goto exnotice;
	}
	ctsp.action=CBL_ACTION_SET;
	YDBI_GET_ITEM_VSUBST(uint64_t*, ifk4vk1, ctsp.base_time_sec, value,
			     (char*)ifname,
			     IETF_INTERFACES_GATE_PARAMETER_TABLE,
			     IETF_INTERFACES_ADMIN_BASE_TIME,
			     IETF_INTERFACES_SECONDS, 255,
			     NULL, 0, YDBI_CONFIG);
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk4vk1, ctsp.base_time_nsec, value,
			     (char*)ifname,
			     IETF_INTERFACES_GATE_PARAMETER_TABLE,
			     IETF_INTERFACES_ADMIN_BASE_TIME,
			     IETF_INTERFACES_NANOSECONDS, 255,
			     NULL, 0, YDBI_CONFIG);
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk4vk1, ctsp.cycle_time_numerator, value,
			     (char*)ifname,
			     IETF_INTERFACES_GATE_PARAMETER_TABLE,
			     IETF_INTERFACES_ADMIN_CYCLE_TIME,
			     IETF_INTERFACES_NUMERATOR, 255,
			     NULL, 0, YDBI_CONFIG);
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk4vk1, ctsp.cycle_time_denominator, value,
			     (char*)ifname,
			     IETF_INTERFACES_GATE_PARAMETER_TABLE,
			     IETF_INTERFACES_ADMIN_CYCLE_TIME,
			     IETF_INTERFACES_DENOMINATOR, 255,
			     NULL, 0, YDBI_CONFIG);

	clst=ub_esarray_init(4, sizeof(cbl_tas_gate_cmd_entry_t),
			     MAX_TAS_GATE_SCHEDULES);
	if(ub_assert_fatal(clst!=NULL, __func__, NULL)){return -1;}
	res=get_gate_schedules(hwald, clst, ifname);
	if(res>0){
		ctsp.entries=clst;
		res=cbl_tas_setup(hwald->hwctx, &ctsp);
	}
	ub_esarray_close(clst);
	if(res<1) return res;
exnotice:
	memset(&nevent, 0, sizeof(nevent));
	if(ctsp.action==CBL_ACTION_DEL){
		nevent.eventflags=CBL_EVENT_TAS_DISABLED;
	}else{
		nevent.eventflags=CBL_EVENT_TAS_ENABLED;
	}
	memcpy(nevent.ifname, ifname,
	       UB_MIN(CB_MAX_NETDEVNAME-1, strlen(ifname)));
	return notice_cb(hwald, &nevent);
}

static int preempt_hw_action(uc_hwald *hwald, const char *ifname)
{
	int i, res;
	cbl_preempt_params_t cpemp;
	void *value;
	cbl_cb_event_t nevent;
	uint8_t size = 0U;
	if(!ifname){
		UB_LOG(UBL_ERROR, "%s:no ifname\n", __func__);
		return -1;
	}
	size = UB_MIN(CB_MAX_NETDEVNAME-1, strlen(ifname));
	memset(&cpemp, 0, sizeof(cpemp));
	memset(&nevent, 0, sizeof(nevent));
	memcpy(nevent.ifname, ifname, size);
	if(get_queue_map_params(&cpemp.qmap, nevent.ifname)) return 0;
	// read all priorities
	for(i=0;i<8;i++){
		cpemp.prioiry_preempt[i]=2;// default is preemptable
		YDBI_GET_ITEM_VSUBST(uint32_t*, ifk4vk1, cpemp.prioiry_preempt[i], value,
				     nevent.ifname,
				     IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS,
				     IETF_INTERFACES_FRAME_PREEMPTION_STATUS_TABLE,
				     i+IETF_INTERFACES_PRIORITY0, 255,
				     NULL, 0, YDBI_CONFIG);
	}
	cpemp.ifname=nevent.ifname; // redundant, but set it
	res=cbl_preempt_setup(hwald->hwctx, &cpemp, &nevent);
	if(res<1) return res;
	return notice_cb(hwald, &nevent);
}

static int ietf_interfaces_writehw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss,
				   void *value, uint32_t vsize)
{
	int res;
	if(aps[0]!=IETF_INTERFACES_RW_H){return 1;}
	if(aps[1]!=IETF_INTERFACES_INTERFACES){return -1;}
	if(aps[2]!=IETF_INTERFACES_INTERFACE){return -1;}
	if(!hwald->hwctx){return 0;}
	UB_LOG(UBL_DEBUG, "%s:\n", __func__);
	if(aps[3]==IETF_INTERFACES_ENABLED){
		bool enabled;
		if((value==NULL) || (kvs[0]==NULL)){
			UB_LOG(UBL_ERROR, "%s:no value\n", __func__);
			return 0;
		}
		// kvs[0] is the network device name
		enabled=(*((uint8_t*)value)!=0);
		if(enabled){
			cbl_cb_event_t nevent;
			memset(&nevent, 0, sizeof(nevent));
			memcpy(nevent.ifname, kvs[0],
			       UB_MIN(CB_MAX_NETDEVNAME-1, strlen((char*)kvs[0])));
			// update 'enabled' by reading link status
			res=cbl_query_linkstatus(hwald->hwctx, &nevent);
			if(res<=0){return res;}// if 0, the result comes later
			notice_cb(hwald, &nevent);
		}
		return 0;
	}
	if(aps[3]==IETF_INTERFACES_BRIDGE_PORT &&
	   aps[4]==IETF_INTERFACES_TRAFFIC_CLASS &&
	   aps[5]==IETF_INTERFACES_CBS_ENABLED){
		// kvs[0] is the network device name, value is 0:disable, 1:enable
		return tc_hw_action(hwald, (const char*)kvs[0], (uint8_t*)value);
	}

	if(aps[3]==IETF_INTERFACES_BRIDGE_PORT &&
	   aps[4]==IETF_INTERFACES_TRAFFIC_CLASS &&
	   aps[5]==IETF_INTERFACES_TC_DATA &&
	   aps[6]==IETF_INTERFACES_ADMIN_IDLESLOPE){
		// kvs[0] is the network device name, kvs[1] is tc
		return cbs_hw_action(hwald, (const char*)kvs[0], (uint8_t*)kvs[1],
				     (int64_t*)value);
	}
	if(aps[3]==IETF_INTERFACES_BRIDGE_PORT &&
	   aps[4]==IETF_INTERFACES_GATE_PARAMETER_TABLE &&
	   aps[5]==IETF_INTERFACES_GATE_ENABLED){
		// kvs[0] is the network device name
		return tas_hw_action(hwald, (const char*)kvs[0], (uint8_t*)value);
	}
	if(aps[3]==IETF_INTERFACES_BRIDGE_PORT &&
	   aps[4]==IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS &&
	   aps[5]==IETF_INTERFACES_FRAME_PREEMPTION_STATUS_TABLE){
		return preempt_hw_action(hwald, (const char*)kvs[0]);
	}
	return 1;
}

static int vlan_register(uc_hwald *hwald, const char *bridgename, const char *ifname,
			 uint16_t vid, bool reg)
{
	if(reg){
		// register vlan
		UB_LOG(UBL_INFO, "%s:bridgename=%s, netdev=%s, register vid=%d\n",
		       __func__, bridgename, ifname, vid);
	}else{
		// deregister vlan
		UB_LOG(UBL_INFO, "%s:bridgename=%s, netdev=%s, deregister vid=%d\n",
		       __func__, bridgename, ifname, vid);
	}
	return 0;
}

static int vlans_register(uc_hwald *hwald, const char *bridgename, const char *ifname,
			  uint16_t vid1, uint16_t vid2, bool reg)
{
	int res=0;
	uint16_t vid;
	for(vid=vid1;vid<=vid2;vid++){
		res|=vlan_register(hwald, bridgename, ifname, vid, reg);
	}
	return res;
}

static int dot1q_bridge_writehw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss,
				void *value, uint32_t vsize)
{
	if(aps[0]!=IEEE802_DOT1Q_BRIDGE_RW_H){return 1;}
	if(aps[1]!=IEEE802_DOT1Q_BRIDGE_BRIDGES){return -1;}
	if(aps[2]!=IEEE802_DOT1Q_BRIDGE_BRIDGE){return -1;}
	if(aps[3]!=IEEE802_DOT1Q_BRIDGE_COMPONENT){return -1;}
	if(!hwald->hwctx){return 0;}
	UB_LOG(UBL_DEBUG, "%s:\n", __func__);
	if(aps[4]==IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE &&
	   aps[5]==IEEE802_DOT1Q_BRIDGE_VLAN_REGISTRATION_ENTRY &&
	   aps[6]==IEEE802_DOT1Q_BRIDGE_PORT_MAP &&
	   aps[7]==IEEE802_DOT1Q_BRIDGE_DYNAMIC_VLAN_REGISTRATION_ENTRIES &&
	   aps[8]==IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT){
		uint16_t *vids;
		// dynamic VLAN registration
		// kvs[0]:bridgename, kvs[1]:component name, kvs[2]:dtabase_id,
		// kvs[3]:vids(start, end, 0, 0), kvs[4]:netdev
		if((kvs[3]==NULL) || (kvs[4]==NULL)){return -1;}
		vids=(uint16_t*)kvs[3];
		return vlans_register(hwald, (const char*)kvs[0], (const char*)kvs[4],
				      vids[0], vids[1], value!=NULL);
	}
	return 1;
}


/* the instance must be only 1 */
#define UC_HWAL_NLINST uc_hwal_nlinst
UB_SD_GETMEM_DEF(UC_HWAL_NLINST, sizeof(uc_hwald), 1);

uc_hwald *uc_hwal_open(uc_dbald *dbald)
{
	uc_hwald *hwald;
	UB_LOG(UBL_INFO, "%s:\n", __func__);
	hwald=(uc_hwald*)UB_SD_GETMEM(UC_HWAL_NLINST, sizeof(uc_hwald));
	if(ub_assert_fatal(hwald!=NULL, __func__, NULL)){return NULL;}
	memset(hwald, 0, sizeof(uc_hwald));
	hwald->dbald=dbald;
	hwald->hwctx=combase_link_init(notice_cb, hwald, NULL);
	if(!hwald->hwctx){
		UB_SD_RELMEM(UC_HWAL_NLINST, hwald);
		return NULL;
	}
	return hwald;
}

void uc_hwal_close(uc_hwald *hwald)
{
	UB_LOG(UBL_INFO, "%s:\n", __func__);
	if(!hwald){return;}
	if(hwald->cqtd.running){
		hwald->cqtd.running=false;
		CB_THREAD_JOIN(hwald->catch_event, NULL);
	}
	combase_link_close(hwald->hwctx);
	UB_SD_RELMEM(UC_HWAL_NLINST, hwald);
}

int uc_hwal_reghw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss,
		  void *value, uint32_t vsize)
{
	int res;
	const char *emes="";
	if(!hwald){return 0;}

	/* Do it here for the initialization at the first time access.
	 * When it is not needed, just call *_writehw.
	 */

	res=ietf_interfaces_writehw(hwald, aps, kvs, kss, value, vsize);
	if(res<0){
		UB_LOG(UBL_ERROR, "%s:error in %s\n", __func__, emes);
		return res;
	}
	return 0;
}

int uc_hwal_dereghw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss)
{
	/* this is called when the DB item to update the HW is deleted.
	 * If HW needs some action to de-register, do it here.
	 */
	int res;
	const char *emes;
	if(!hwald){return 0;}
	emes="dot1q_bridge";
	res=dot1q_bridge_writehw(hwald, aps, kvs, kss, NULL, 0);
	if(res<0){
		UB_LOG(UBL_ERROR, "%s:error in %s\n", __func__, emes);
		return res;
	}
	return 0;
}

int uc_hwal_writehw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss,
		     void *value, uint32_t vsize)
{
	int res;
	if(!hwald){return 0;}
	const char *emes="ietf_interfaces";
	res=ietf_interfaces_writehw(hwald, aps, kvs, kss, value, vsize);
	if(res<0){goto erexit;}
	emes="dot1q_bridge";
	res=dot1q_bridge_writehw(hwald, aps, kvs, kss, value, vsize);
erexit:
	if(res<0){
		UB_LOG(UBL_ERROR, "%s:error in %s\n", __func__, emes);
		return res;
	}
	return 0;
}

int uc_hwal_catch_events_thread(uc_hwald *hwald, CB_SEM_T *sigp)
{
	cb_tsn_thread_attr_t attr;
	if(hwald->cqtd.running){return 0;}
	hwald->cqtd.cbld=hwald->hwctx;
	hwald->cqtd.sigp=sigp;
	cb_tsn_thread_attr_init(&attr, 0, 0, "uniconf_hwal_thread");
	return CB_THREAD_CREATE(&hwald->catch_event, &attr, cbl_query_thread,
				&hwald->cqtd);
}

int uc_hwal_detect_notice(uc_hwald *hwald, uc_notice_data_t *ucntd)
{
	int res;
	if(!hwald){return 0;}
	hwald->ucntd=ucntd;
	res=cbl_query_response(hwald->hwctx, 0);
	if(res<0){
		UB_LOG(UBL_WARN, "%s:error in cbl_query_response\n", __func__);
		return res;
	}
	return res;
}

int notice_cb(void *cbdata, cbl_cb_event_t *nevent)
{
	int res=-1;
	uint8_t pi;
	uc_hwald *hwald=(uc_hwald *)cbdata;
	if(nevent->eventflags==CBL_EVENT_CHECKENABLED){
		// CBL_EVENT_CHECKENABLED is a special case and it is not a notice.
		// HW action may check this first,
		// if it is not enabled, no actions happen.
		uint8_t aps[5]={IETF_INTERFACES_RW_H, IETF_INTERFACES_INTERFACES,
				IETF_INTERFACES_INTERFACE, IETF_INTERFACES_ENABLED, 255};
		void *kvs[2]={NULL,NULL};
		uint8_t kss[1];
		yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
					      NULL, aps, kvs, kss, NULL, 0};
		kvs[0]=nevent->ifname;
		kss[0]=strlen(nevent->ifname)+1;
		res=yang_db_action(hwald->dbald, NULL, &dbpara);
		if(res){return -1;}
		if(!*((uint8_t*)dbpara.value)){res=-1;}
		dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(hwald->dbald, NULL, &dbpara);
		return res;
	}
	if(nevent->eventflags&CBL_EVENT_DEVUP ||
	   nevent->eventflags&CBL_EVENT_DEVDOWN){
		res=set_netdev_linkstatus(hwald, nevent);
		if(res<0){return -1;}
	}
	if((nevent->eventflags&CBL_EVENT_TCHW_ENABLED)||
	   (nevent->eventflags&CBL_EVENT_TCHW_DISABLED)){
		pi=(nevent->eventflags&CBL_EVENT_TCHW_ENABLED)?
			UC_HWAL_CBS_ENABLE_COMPLETE:UC_HWAL_CBS_DISABLE_COMPLETE;
		res=YDBI_SET_ITEM(ifk4vk1, nevent->ifname,
				  IETF_INTERFACES_TRAFFIC_CLASS,
				  IETF_INTERFACES_CBS_ENABLED,
				  255, 255, NULL, 0, YDBI_CONFIG,
				  &pi, 1, YDBI_PUSH_NOTICE_IN_UNICONF,
				  YANG_DB_ONHW_NOACTION);
		if(res<0){return -1;}
		UB_TLOG(UBL_INFO, "%s:%s, TC map setup completed, v=%d\n",
			__func__, nevent->ifname, pi);
		return 0;
	}
	if(((nevent->eventflags&CBL_EVENT_CBS_ENABLED)||
	    (nevent->eventflags&CBL_EVENT_CBS_DISABLED)) &&
		hwald->operating_tc>0){
		int64_t idleslope=0;
		void *value;
		if(nevent->eventflags&CBL_EVENT_CBS_ENABLED){
			YDBI_GET_ITEM_VSUBST(int64_t*, ifk4vk1, idleslope, value,
					     nevent->ifname,
					     IETF_INTERFACES_TRAFFIC_CLASS,
					     IETF_INTERFACES_TC_DATA,
					     IETF_INTERFACES_ADMIN_IDLESLOPE, 255,
					     &hwald->operating_tc, 1, YDBI_CONFIG);
		}
		res=YDBI_SET_ITEM(ifk4vk1, nevent->ifname,
				  IETF_INTERFACES_TRAFFIC_CLASS,
				  IETF_INTERFACES_TC_DATA,
				  IETF_INTERFACES_OPER_IDLESLOPE,
				  255, &hwald->operating_tc, 1, YDBI_STATUS,
				  &idleslope, sizeof(int64_t), YDBI_PUSH_NOTICE_IN_UNICONF,
				  YANG_DB_ONHW_NOACTION);

		UB_TLOG(UBL_INFO, "%s:%s, CBS setup completed, tc=%d, "
			"idleslope=%"PRIi64"\n",
			__func__, nevent->ifname, hwald->operating_tc,
			idleslope);
		hwald->operating_tc=0;
		if(res<0){return -1;}
		return 0;
	}
	if(nevent->eventflags&CBL_EVENT_TAS_ENABLED ||
	   nevent->eventflags&CBL_EVENT_TAS_DISABLED){
		res=set_tas_status(hwald, nevent);
		if(res<0){return -1;}
	}
	if(nevent->eventflags&CBL_EVENT_PREEMPT_STATUS){
		res=set_preempt_status(hwald, nevent);
		if(res<0){return -1;}
	}
	if(res<0){
		UB_TLOG(UBL_ERROR, "%s:unkown eventflags=0x%x\n",
		       __func__, nevent->eventflags);
		return -1;
	}
	return 0;
}
