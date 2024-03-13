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
#include "combase_private.h"
#include "cb_ethernet.h"
#include "../combase.h"
#include "../combase_link.h"
#include "lld_ethernet_private.h"
#include "lldenetext.h"

struct combase_link_data {
	CB_SOCKET_T sock;
	cbl_cb_t event_cb;
	void *cb_arg;
	char *netdevs[MAX_NUMBER_ENET_DEVS];
	uint32_t netdev_opersts[MAX_NUMBER_ENET_DEVS]; // keep current operation status
	int numof_netdevs;
};

#define CBL_DATA_INSTMEM cbl_data_inst
#ifndef CBL_DATA_INSTNUM
#define CBL_DATA_INSTNUM 1
#endif

UB_SD_GETMEM_DEF(CBL_DATA_INSTMEM, (int)sizeof(struct combase_link_data),
		 CBL_DATA_INSTNUM);

combase_link_data_t *combase_link_init(cbl_cb_t event_cb, void *cb_arg,
				       combase_link_extfd_t *extfdp)
{
	int i;
	combase_link_data_t *cbld;
	ub_macaddr_t srcmac;
	(void)extfdp;

	cbld = (combase_link_data_t*)UB_SD_GETMEM(CBL_DATA_INSTMEM,
						  sizeof(struct combase_link_data));
	if (ub_assert_fatal(cbld!=NULL, __func__, NULL)) { return NULL; }
	memset(cbld, 0, sizeof(struct combase_link_data));
	cbld->event_cb = event_cb;
	cbld->cb_arg = cb_arg;

	if (cb_lld_get_netdevs(cbld->netdevs, &cbld->numof_netdevs)) {
		goto errexit;
	}

	for (i = 0; i < cbld->numof_netdevs; i++) {
		if (cbld->sock != NULL) { break; }
		cb_rawsock_paras_t llrawp = {
			.dev = cbld->netdevs[i],
			.proto = ETH_P_NETLINK,
		};
		if (cb_rawsock_open(&llrawp, &cbld->sock,
				    NULL, NULL, srcmac) < 0) {
			goto errexit;
		}
	}

	for (i = 0; i < cbld->numof_netdevs; i++) {
		cbld->netdev_opersts[i] = 0xFFFFFFFF;
	}

	return cbld;
 errexit:
	UB_SD_RELMEM(CBL_DATA_INSTMEM, cbld);
	return NULL;
}

void combase_link_close(combase_link_data_t *cbld)
{
	if (!cbld) { return; }

	cb_rawsock_close(cbld->sock);
	UB_SD_RELMEM(CBL_DATA_INSTMEM, cbld);
}

int cbl_query_linkstatus(combase_link_data_t *cbld, cbl_cb_event_t *nevent)
{
	uint32_t link_state = 0;

	if (!nevent->ifname[0]) {
		UB_LOG(UBL_ERROR,"%s:invalid ifname : %s\n",
		       __func__, nevent->ifname);
		return -1;
	}

	if (!cbld->sock) {
		return -1;
	}
	if (cb_lld_get_link_state(cbld->sock, nevent->ifname, &link_state)) {
		return -1;
	}
	nevent->eventflags = link_state==1? CBL_EVENT_DEVUP : CBL_EVENT_DEVDOWN;
	if (link_state) {
		if (cb_lld_get_link_info(cbld->sock, nevent->ifname,
					 &nevent->u.linkst.speed, &nevent->u.linkst.duplex)) {
			UB_LOG(UBL_WARN,"%s:failed to get speed and duplex : %s\n",
			       __func__, nevent->ifname);
			return -1;
		}
	}

	return 1;
}

int cbl_query_response(combase_link_data_t *cbld, int tout_ms)
{
	cbl_cb_event_t nevent = {};
	int i;
	uint32_t link_state = 0;

	for (i = 0; i < cbld->numof_netdevs; i++) {
		if (cb_lld_get_link_state(cbld->sock, cbld->netdevs[i], &link_state)) {
			return -1;
		}
		if (cbld->netdev_opersts[i] == link_state){continue;}
		(void)ub_strncpy(nevent.ifname, cbld->netdevs[i], sizeof(nevent.ifname));
		nevent.eventflags=CBL_EVENT_CHECKENABLED;
		if(cbld->event_cb(cbld->cb_arg, &nevent)){
			return 0; //ifname is not enabled, ignore it.
		}
		if (link_state) {
			if (cb_lld_get_link_info(cbld->sock, nevent.ifname,
						 &nevent.u.linkst.speed, &nevent.u.linkst.duplex)) {
				UB_LOG(UBL_ERROR,"%s:failed to get speed and duplex : %s\n",
				       __func__, nevent.ifname);
			}
			nevent.eventflags = CBL_EVENT_DEVUP;
			UB_LOG(UBL_INFO, "%s:%s: link UP, speed=%d, duplex=%d !!!!\n", __func__,
				   cbld->netdevs[i], nevent.u.linkst.speed, nevent.u.linkst.duplex);
		} else {
			nevent.eventflags=CBL_EVENT_DEVDOWN;
			UB_LOG(UBL_INFO, "%s:%s link DOWN !!!!\n", __func__, cbld->netdevs[i]);
		}

		cbld->netdev_opersts[i] = link_state;
		nevent.ifindex=i+1; //if-index range "1..2147483647"
		cb_get_mac_bydev(cbld->sock, nevent.ifname, nevent.u.linkst.address);
		cbld->event_cb(cbld->cb_arg, &nevent);
	}

	return 0;
}

int cbl_tc_queue_map(combase_link_data_t *cbld, cbl_tcinit_params_t *tip)
{
	uint8_t mac_port = cb_lld_netdev_to_macport((char*)tip->ifname);
	int res =  LLDEnetEnableTCQueueMapping(cbld->sock->lldenet,
					       mac_port, &tip->qmap);
	return (res == LLDENET_E_OK? 1: -1);
}

int cbl_cbs_setup(combase_link_data_t *cbld, cbl_cbs_params_t *cbsp)
{
	uint8_t mac_port = cb_lld_netdev_to_macport((char*)cbsp->ifname);
	int res =  LLDEnetSetCreditBasedShaping(cbld->sock->lldenet,
						mac_port, cbsp);
	return (res == LLDENET_E_OK? 1: -1);
}

int cbl_tas_setup(combase_link_data_t *cbld, cbl_tas_sched_params_t *ctsp)
{
	int res;

	if (!cbld->sock) {
		return -1;
	}
	uint8_t mac_port = cb_lld_netdev_to_macport((char*)ctsp->ifname);
	res = LLDEnetTasSetConfig(cbld->sock->lldenet, mac_port, ctsp);
	return (res == LLDENET_E_OK? 1: -1);
}

int cbl_preempt_setup(combase_link_data_t *cbld, cbl_preempt_params_t *cpemp,
		      cbl_cb_event_t *nevent)
{
	if (!cbld->sock) {
		return -1;
	}
	uint8_t mac_port = cb_lld_netdev_to_macport((char*)cpemp->ifname);
	int res = LLDEnetIETSetConfig(cbld->sock->lldenet, mac_port, cpemp, nevent);
	nevent->eventflags = (res == LLDENET_E_OK)? CBL_EVENT_PREEMPT_STATUS: 0;
	return (res == LLDENET_E_OK? 1: -1);
}

static int check_linkstate_change(combase_link_data_t *cbld)
{
	int i;
	uint32_t link_state = 0;

	for (i = 0; i < cbld->numof_netdevs; i++) {
		if (cb_lld_get_link_state(cbld->sock, cbld->netdevs[i], &link_state)) {
			return -1;
		}
		if (cbld->netdev_opersts[i] != link_state){
			return 1;
		}
	}
	return 0;
}

void *cbl_query_thread(void *ptr)
{
	cbl_query_thread_data_t *cqtd=(cbl_query_thread_data_t *)ptr;
	combase_link_data_t *cbld=(combase_link_data_t *)cqtd->cbld;
	int res;
	if(!cbld){return NULL;}
	cqtd->running=true;
	while(cqtd->running){
		CB_USLEEP(100000); /* 100msec polling should be okay */
		res=check_linkstate_change(cbld);
		if(res<=0){continue;}
		if(cqtd->sigp!=NULL){
			CB_SEM_POST( cqtd->sigp);
		}
	}
	return NULL;
}
