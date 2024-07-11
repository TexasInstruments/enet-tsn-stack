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
 * lld_gptpnet.c
 */
#include "gptpnet.h"
#include "gptpclock.h"
#include "lld_gptp_private.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpcommon.h"

extern char *PTPMsgType_debug[];

extern int gptpgcfg_link_check(uint8_t gptpInstanceIndex, gptpnet_data_netlink_t *edtnl);

typedef struct {
	int ndev_index;
	uint8_t msgtype;
	uint16_t seqid;
	uint8_t domain;
	uint8_t used;
} txts_info_t;

#define MAX_TXTS_INFO 32
typedef struct {
	txts_info_t txts_info[MAX_TXTS_INFO];
	int rdi;
	int wri;
} txts_queue_t;

typedef struct {
	CB_ETHHDR_T ehd;
	uint8_t pdata[GPTP_MAX_PACKET_SIZE];
} __attribute__((packed)) ptpkt_t;

typedef struct netdevice {
	ptpkt_t txbuf;
	event_data_netlink_t nlstatus;
	uint64_t txtslost_time;
	CB_SOCKADDR_LL_T addr;
} netdevice_t;

struct gptpnet_data {
	gptpnet_cb_t cb_func;
	void *cb_data;
	netdevice_t *netdevices;
	int num_netdevs;
	int64_t event_ts64;
	uint64_t next_tout64;
	CB_SEM_T semaphore;
	LLDTSync_t *lldtsync;
	txts_queue_t txts_queue;
	int64_t last_ts64;
	ptpkt_t rxbuf;
	CB_SOCKET_T lldsock;
	uint8_t gptpInstanceIndex;
};

static int push_txts_info(txts_queue_t *q, txts_info_t *in)
{
	txts_info_t *txts_info = &q->txts_info[q->wri];
	if (txts_info->used == 1) {
		return -1;
	}
	memcpy(txts_info, in, sizeof(txts_info_t));
	txts_info->used = 1;
	q->wri = (q->wri + 1) % MAX_TXTS_INFO;
	return 0;
}

static int pop_txts_info(txts_queue_t *q, txts_info_t *out)
{
	txts_info_t *txts_info = &q->txts_info[q->rdi];
	if (txts_info->used == 0) {
		return -1;
	}
	memcpy(out, txts_info, sizeof(txts_info_t));
	txts_info->used = 0;
	q->rdi = (q->rdi + 1) % MAX_TXTS_INFO;
	return 0;
}

/* do not call log or any blocking function inside */
static void txrx_notify_cb(void *arg)
{
	gptpnet_data_t *gpnet = (gptpnet_data_t *)arg;
	CB_SEM_POST(&gpnet->semaphore);
}

static int onenet_init(uint8_t gptpInstanceIndex, gptpnet_data_t *gpnet,
					   netdevice_t *ndev, const char *netdev)
{
	ub_macaddr_t destmac = GPTP_MULTICAST_DEST_ADDR;
	ub_macaddr_t srcmac;

	(void)snprintf(ndev->nlstatus.devname, CB_MAX_NETDEVNAME, "%s", netdev);
	if(cb_get_ptpdev_from_netdev(ndev->nlstatus.devname,
			ndev->nlstatus.ptpdev) < 0) {
		return -1;
	}

	ndev->txtslost_time = gptpgcfg_get_intitem(
		gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_TXTS_LOST_TIME,
		YDBI_CONFIG);

	/* We need a single lldsock for all the ports */
	if (gpnet->lldsock == NULL) {
		cb_rawsock_paras_t llrawp;
		(void)memset(&llrawp, 0, sizeof(llrawp));
		llrawp.dev=ndev->nlstatus.devname;
		llrawp.proto=ETH_P_1588;
		llrawp.vlan_proto=0;
		llrawp.vlanid = 0;
		llrawp.priority= gptpgcfg_get_intitem(
			gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_SOCKET_TXPRIORITY,
			YDBI_CONFIG);
		llrawp.rw_type=CB_RAWSOCK_RDWR;

		gptpgcfg_releasedb(gptpInstanceIndex);
		if(cb_rawsock_open(&llrawp, &gpnet->lldsock, NULL, NULL, srcmac) < 0) {
			return -1;
		}
		cb_lld_set_txnotify_cb(gpnet->lldsock, txrx_notify_cb, gpnet);
		cb_lld_set_rxnotify_cb(gpnet->lldsock, txrx_notify_cb, gpnet);

		if(cb_reg_multicast_address(gpnet->lldsock,
					ndev->nlstatus.devname, destmac, 0)) {
			UB_LOG(UBL_ERROR,"failed to add multicast address\n");
			cb_rawsock_close(gpnet->lldsock);
			gpnet->lldsock = NULL;
			return -1;
		}
	} else {
		if (cb_get_mac_bydev(gpnet->lldsock, ndev->nlstatus.devname, srcmac)) {
			return -1;
		}
	}
	memcpy(ndev->txbuf.ehd.H_SOURCE, srcmac, ETH_ALEN);
	memcpy(ndev->txbuf.ehd.H_DEST, destmac, ETH_ALEN);
	ndev->txbuf.ehd.H_PROTO = htons(ETH_P_1588);
	eui48to64(ndev->txbuf.ehd.H_SOURCE, ndev->nlstatus.portid, NULL);
	ndev->addr.tcid = 0;
	ndev->addr.macport = cb_lld_netdev_to_macport(netdev);
	return ndev->addr.macport;
}

static int onenet_activate(gptpnet_data_t *gpnet, int ndevIndex)
{
	netdevice_t *ndev=&gpnet->netdevices[ndevIndex];
	void *value;
	uint64_t speed=0;

	ndev->nlstatus.up=0;
	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk1vk0, ndev->nlstatus.up, value,
				 ndev->nlstatus.devname, IETF_INTERFACES_OPER_STATUS, YDBI_STATUS);
	ndev->nlstatus.duplex=1;
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk1vk0, ndev->nlstatus.duplex, value,
				 ndev->nlstatus.devname, IETF_INTERFACES_DUPLEX, YDBI_STATUS);

	YDBI_GET_ITEM_VSUBST(uint64_t*, ifk1vk0, speed, value,
				 ndev->nlstatus.devname, IETF_INTERFACES_SPEED, YDBI_STATUS);
	ndev->nlstatus.speed=speed/1000000u;
	if(ndev->nlstatus.speed == 0u){ndev->nlstatus.up = false;}
	UB_LOG(UBL_INFO, "%s:%s status=%d, duplex=%d, speed=%dMbps\n", __func__,
			ndev->nlstatus.devname, ndev->nlstatus.up, ndev->nlstatus.duplex,
			ndev->nlstatus.speed);
	if(!gpnet->cb_func || !ndev->nlstatus.up){return 0;}
	return gpnet->cb_func(gpnet->cb_data, ndevIndex+1, GPTPNET_EVENT_DEVUP,
						  &gpnet->event_ts64, &ndev->nlstatus);
}

gptpnet_data_t *gptpnet_init(uint8_t gptpInstanceIndex, gptpnet_cb_t cb_func,
				 void *cb_data, const char *netdev[], uint8_t num_ports,
				 char *master_ptpdev)
{
	gptpnet_data_t *gpnet;
	LLDTSyncCfg_t tsyncfg;
	int res;
	int i;
	uint32_t ports[LLDENET_MAX_PORTS];
	uint32_t nports = 0;

	if (num_ports == 0) {
		UB_LOG(UBL_ERROR,"%s:at least one netdev need\n",__func__);
		return NULL;
	}
	gpnet = (gptpnet_data_t *)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(gptpnet_data_t));
	if (ub_assert_fatal(gpnet != NULL, __func__, "malloc")) {
		return NULL;
	}
	(void)memset(gpnet, 0, sizeof(gptpnet_data_t));
	gpnet->gptpInstanceIndex=gptpInstanceIndex;
	gpnet->num_netdevs = num_ports;
	gpnet->netdevices =
		(netdevice_t *)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, num_ports * sizeof(netdevice_t));
	if(ub_assert_fatal(gpnet->netdevices, __func__, "malloc")){
		UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpnet);
		return NULL;
	}
	(void)memset(gpnet->netdevices, 0, num_ports * sizeof(netdevice_t));

	for (i = 0; i < gpnet->num_netdevs; i++) {
		res = onenet_init(gptpInstanceIndex, gpnet, &gpnet->netdevices[i], netdev[i]);
		if (res < 0) {
			UB_LOG(UBL_ERROR, "dev:%s open failed\n", netdev[i]);
			goto error;
		} else {
			UB_LOG(UBL_INFO, "dev:%s open success\n", netdev[i]);
		}
		ports[nports] = res;
		nports++;
	}

	gpnet->cb_func = cb_func;
	gpnet->cb_data = cb_data;
	gpnet->event_ts64 = ub_mt_gettime64();

	if(gptpgcfg_set_netdevs(gptpInstanceIndex, netdev, num_ports)!=0){
		UB_LOG(UBL_ERROR,"%s:failed to set netdevs!\n", __func__);
		goto error;
	}

	if (CB_SEM_INIT(&gpnet->semaphore, 0, 0) < 0) {
		UB_LOG(UBL_ERROR,"%s:failed to init sem!\n", __func__);
		goto error;
	}

	LLDTSyncCfgInit(&tsyncfg);
	cb_lld_get_type_instance(&tsyncfg.enetType, &tsyncfg.instId);
	gpnet->lldtsync = LLDTSyncOpen(&tsyncfg);
	if (gpnet->lldtsync == NULL) {
		UB_LOG(UBL_ERROR,"%s:failed to open lldtsync!\n", __func__);
		goto error;
	}

	res = LLDTSyncEnableTsEvent(gpnet->lldtsync, ports, nports);
	if (res != LLDENET_E_OK) {
		UB_LOG(UBL_ERROR,"%s:failed to enable tsevent!\n", __func__);
		goto error;
	}

	UB_LOG(UBL_INFO,"%s:Open lldtsync OK!\n", __func__);

	return gpnet;

error:
	gptpnet_close(gpnet);
	return NULL;
}

int gptpnet_close(gptpnet_data_t *gpnet)
{
	UB_LOG(UBL_DEBUGV, "%s:\n",__func__);
	if (!gpnet) {return -1;}
	gptpgcfg_remove_netdevs(gpnet->gptpInstanceIndex);
	if (gpnet->lldsock) {
		cb_rawsock_close(gpnet->lldsock);
		gpnet->lldsock = NULL;
	}
	if (gpnet->lldtsync) {
		LLDTSyncClose(gpnet->lldtsync);
		gpnet->lldtsync = NULL;
	}
	if (gpnet->semaphore) {
		CB_SEM_DESTROY(&gpnet->semaphore);
		gpnet->semaphore = NULL;
	}
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpnet->netdevices);
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpnet);
	return 0;
}

uint8_t *gptpnet_get_sendbuf(gptpnet_data_t *gpnet, int ndev_index)
{
	return gpnet->netdevices[ndev_index].txbuf.pdata;
}

int gptpnet_activate(gptpnet_data_t *gpnet)
{
	int i;
	for (i = 0; i < gpnet->num_netdevs; i++) {
		onenet_activate(gpnet, i);
	}
	return 0;
}

int gptpnet_send(gptpnet_data_t *gpnet, int ndev_index, uint16_t length)
{
	char *msg;
	int msgtype;
	netdevice_t *ndev;
	uint16_t seqid;
	uint8_t domain;
	int res;

	if (length > GPTP_MAX_PACKET_SIZE) {
		UB_LOG(UBL_ERROR, "%s:deviceIndex = %d, length = %d is too big\n",
			   __func__, ndev_index, length);
		return -1;
	}
	ndev = &gpnet->netdevices[ndev_index];
	msgtype = PTP_HEAD_MSGTYPE(ndev->txbuf.pdata);
	if (msgtype <= 15) {
		msg = PTPMsgType_debug[msgtype];
	} else {
		msg = "unknow";
	}
	seqid = PTP_HEAD_SEQID(ndev->txbuf.pdata);
	domain = PTP_HEAD_DOMAIN_NUMBER(ndev->txbuf.pdata);

	res = CB_SOCK_SENDTO(gpnet->lldsock, &ndev->txbuf, length+sizeof(CB_ETHHDR_T),
						 0, &ndev->addr, sizeof(ndev->addr));
	if (res < 0) {
		UB_LOG(UBL_ERROR,"%s:sent %s failed\n", __func__, msg);
		return -1;
	}
	if (msgtype < 8) {
		txts_info_t txts_info;
		txts_info.ndev_index = ndev_index;
		txts_info.seqid = seqid;
		txts_info.msgtype = msgtype;
		txts_info.domain = domain;
		push_txts_info(&gpnet->txts_queue, &txts_info);
	}
	return res;
}

static int provide_txts(gptpnet_data_t *gpnet, txts_info_t *txts_info, uint64_t ts)
{
	event_data_txts_t edtxts;

	memset(&edtxts, 0, sizeof(edtxts));
	edtxts.msgtype = txts_info->msgtype;
	edtxts.seqid = txts_info->seqid;
	edtxts.domain = txts_info->domain;
	edtxts.ts64 = ts;

	gpnet->cb_func(gpnet->cb_data, txts_info->ndev_index+1, GPTPNET_EVENT_TXTS,
				   &gpnet->event_ts64, &edtxts);
	return 0;
}

static int ndev_index_to_macport(gptpnet_data_t *gpnet, int ndev_index)
{
	if (ndev_index < 0 || ndev_index >= gpnet->num_netdevs) {
		UB_LOG(UBL_ERROR, "%s:ndev_index = %d invalid\n",__func__, ndev_index);
		return -1;
	}
	return gpnet->netdevices[ndev_index].addr.macport;
}

static int macport_to_ndev_index(gptpnet_data_t *gpnet, int macport)
{
	int i;
	for (i = 0; i < gpnet->num_netdevs; i++) {
		if (gpnet->netdevices[i].addr.macport == macport) {
			return i;
		}
	}
	UB_LOG(UBL_ERROR, "%s:no ndev_index for macport %d\n",
		   __func__, macport);
	return -1;
}

static int provide_rxframe(gptpnet_data_t *gpnet, uint8_t *buf,
						   int size, int macport, uint64_t rxts)
{
	event_data_recv_t edtrecv;
	int seqid;
	int res;
	int ndev_index;

	if (size <= sizeof(struct lld_ethhdr)) {
		UB_LOG(UBL_ERROR,"%s:macport=%d, pkt size too small\n",
			   __func__, macport);
		return -1;
	}
	/* The VLAN tag will be stripped if it is presented */
	if(ntohs(*(uint16_t *)(buf + 12))==ETH_P_8021Q){
		struct lld_ethhdr ehdr;
		memcpy((void *)&ehdr, (void *)buf, ETH_ALEN*2);
		buf += 4;
		size -= 4;
		memcpy((void *)buf, (void *)&ehdr, ETH_ALEN*2);
	}
	/* Do not handle non PTP packets */
	if(ntohs(*(uint16_t *)(buf + 12))!=ETH_P_1588){
		UB_LOG(UBL_ERROR, "%s: RX not ETH_P_1588 packet 0x%02X%02X\n",
			   __func__, buf[12], buf[13]);
		return -1;
	}

	memset(&edtrecv, 0, sizeof(edtrecv));
	edtrecv.recbptr = buf+ETH_HLEN;
	edtrecv.domain = PTP_HEAD_DOMAIN_NUMBER(buf+ETH_HLEN);
	edtrecv.msgtype = PTP_HEAD_MSGTYPE(buf+ETH_HLEN);
	seqid = PTP_HEAD_SEQID(buf+ETH_HLEN);
	if (edtrecv.msgtype < 8) {
		if(rxts > 0) {
			edtrecv.ts64 = rxts;
		} else {
			res = LLDTSyncGetRxTime(gpnet->lldtsync, macport, edtrecv.msgtype,
									seqid, edtrecv.domain, (uint64_t *)&edtrecv.ts64);
			if (res != LLDENET_E_OK) {
				UB_LOG(UBL_ERROR,"%s:macport=%d, no RxTs msgtype=%s\n",
					   __func__, macport, PTPMsgType_debug[edtrecv.msgtype]);
				return -1;
			}
		}
	}
	ndev_index = macport_to_ndev_index(gpnet, macport);
	if (ndev_index < 0) {
		return -1;
	}
	return gpnet->cb_func(gpnet->cb_data, ndev_index+1, GPTPNET_EVENT_RECV,
					  &gpnet->event_ts64, &edtrecv);
}

static int process_rxdata(gptpnet_data_t *gpnet)
{
	CB_SOCKADDR_LL_T addr;
	int res;

	while (1) {
		res = cb_lld_recv(gpnet->lldsock, &gpnet->rxbuf,
				  sizeof(gpnet->rxbuf), &addr,
				  sizeof(CB_SOCKADDR_LL_T));
		if (res <= 0) {
			break;
		}
		/* indicate that the call receive data but for other apps */
		if (res == 0xFFFF) {
			continue;
		}
		provide_rxframe(gpnet, (uint8_t*)&gpnet->rxbuf, res, addr.macport, addr.rxts);
	}
	return 0;
}

static int process_txts(gptpnet_data_t *gpnet)
{
	txts_info_t txts_info;
	uint64_t ts;
	int macport;
	int res;

	while (1) {
		memset(&txts_info, 0, sizeof(txts_info));
		if (pop_txts_info(&gpnet->txts_queue, &txts_info) < 0) {
			break;
		}
		macport = ndev_index_to_macport(gpnet, txts_info.ndev_index);
		res = LLDTSyncGetTxTime(gpnet->lldtsync, macport, txts_info.msgtype,
				txts_info.seqid, txts_info.domain, &ts);
		if (res != LLDENET_E_OK) {
			continue;
		}
		provide_txts(gpnet, &txts_info, ts);
	}
	return 0;
}

static int find_netdev(netdevice_t *devices, int dnum, char *netdev)
{
	int i;
	for(i=0;i<dnum;i++){
		if(!strcmp(netdev, devices[i].nlstatus.devname)){return i;}
	}
	return -1;
}

static int gptpnet_link_check(gptpnet_data_t *gpnet, int64_t ts64)
{
	int res;
	gptpnet_data_netlink_t edtnl;
	int ndevIndex;
	gptpnet_event_t event;
	char *ptpdev;
	netdevice_t *ndev;
	res=gptpgcfg_link_check(gpnet->gptpInstanceIndex, &edtnl);
	if(res<0){return -1;}
	if(res!=0){return 0;}
	ndevIndex=find_netdev(gpnet->netdevices, gpnet->num_netdevs, edtnl.devname);
	ndev=&gpnet->netdevices[ndevIndex];
	if(ndev->nlstatus.up==edtnl.up){return 0;}/* link no changes */
	ndev->nlstatus.up=edtnl.up;
	ndev->nlstatus.speed=edtnl.speed;
	ndev->nlstatus.duplex=edtnl.duplex;
	if(edtnl.up!=0u){
		event=GPTPNET_EVENT_DEVUP;
	}else{
		event=GPTPNET_EVENT_DEVDOWN;
	}
	ptpdev=ndev->nlstatus.ptpdev;
	memcpy(edtnl.ptpdev, ptpdev, strlen(ptpdev)+1);
	return gpnet->cb_func(gpnet->cb_data, ndevIndex+1, event, &ts64, &edtnl);
}

static int gptpnet_catch_event(gptpnet_data_t *gpnet)
{
	int64_t ts64, tstout64;
	struct timespec ts;
	int err;

	ts64 = ub_mt_gettime64();
	(void)gptpnet_link_check(gpnet, ts64);

	tstout64 = ts64-gpnet->last_ts64;
	// every 10 seconds, print clock parameters for debug
	if (tstout64>=(10*UB_SEC_NS)){
		gptpclock_print_clkpara(gpnet->gptpInstanceIndex, UBL_INFOV);
		gpnet->last_ts64 = ts64;
	}

	if (gpnet->next_tout64!=0) {
		tstout64 = gpnet->next_tout64-ts64;
		if (tstout64<0) {
			gpnet->next_tout64 = 0;
			UB_LOG(UBL_DEBUG,"%s:call missed or extra TIMEOUT CB\n", __func__);
			return gpnet->cb_func(gpnet->cb_data, 0, GPTPNET_EVENT_TIMEOUT,
						 &ts64, NULL);
		}
	} else {
		gpnet->next_tout64 = ((ts64 / GPTPNET_INTERVAL_TIMEOUT_NSEC) + 1) *
			GPTPNET_INTERVAL_TIMEOUT_NSEC;
	}

	gptpgcfg_releasedb(gpnet->gptpInstanceIndex);

	tstout64 = gpnet->next_tout64-ts64;
	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+tstout64, ts);
	err = CB_SEM_TIMEDWAIT(&gpnet->semaphore, &ts);
	gpnet->event_ts64 = ub_mt_gettime64();
	if (err != 0) {
		if (cb_lld_sem_wait_status(&gpnet->semaphore) == TILLD_TIMEDOUT) {
			gpnet->next_tout64 = 0;
			return gpnet->cb_func(gpnet->cb_data, 0,
						  GPTPNET_EVENT_TIMEOUT,
						  &gpnet->event_ts64, NULL);
		}
		UB_LOG(UBL_ERROR,"%s:CB_SEM_TIMEDWAIT error\n", __func__);
		return -1;
	}
	process_txts(gpnet);

	process_rxdata(gpnet);

	return 0;
}

int gptpnet_eventloop(gptpnet_data_t *gpnet, bool *stoploop)
{
	while (!*stoploop) {
		gptpnet_catch_event(gpnet);
		if (ub_fatalerror()) {return -1;}
	}
	return 0;
}

char *gptpnet_ptpdev(gptpnet_data_t *gpnet, int ndevIndex)
{
	return gpnet->netdevices[ndevIndex].nlstatus.ptpdev;
}

void gptpnet_create_clockid(gptpnet_data_t *gpnet, uint8_t *id,
				int ndevIndex, int8_t domainNumber)
{
	memcpy(id, gpnet->netdevices[ndevIndex].nlstatus.portid, sizeof(ClockIdentity));
	if(domainNumber==0){return;}
	id[3]=0;
	id[4]=domainNumber;
}

int gptpnet_get_nlstatus(gptpnet_data_t *gpnet, int ndevIndex,
						 event_data_netlink_t *nlstatus)
{
	if((ndevIndex < 0) || (ndevIndex >= gpnet->num_netdevs)){
		UB_LOG(UBL_ERROR, "%s:ndevIndex=%d doesn't exist\n",__func__, ndevIndex);
		return -1;
	}
	memcpy(nlstatus, &gpnet->netdevices[ndevIndex].nlstatus,
		   sizeof(event_data_netlink_t));
	return 0;
}

uint64_t gptpnet_txtslost_time(gptpnet_data_t *gpnet, int ndevIndex)
{
	/* give up to read TxTS, if it can't be captured in this time */
	return gpnet->netdevices[ndevIndex].txtslost_time;
}

int gptpnet_tsn_schedule(gptpnet_data_t *gpnet, uint32_t aligntime, uint32_t cycletime)
{
	/* IEEE 802.1qbv (time-aware traffic shaping) not yet supported */
	return 0;
}
