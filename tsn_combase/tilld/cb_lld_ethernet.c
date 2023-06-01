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
 * cb_lld_ethernet.c
 */
#include "combase_private.h"
#include "cb_ethernet.h"

#define MAC_VALID(mac) (((mac)[0]|(mac)[1]|(mac)[2]|(mac)[3]|(mac)[4]|(mac)[5]) != 0)

struct lld_socket {
	LLDEnet_t *lldenet;
	uint16_t vlanid;
	char devname[IFNAMSIZ];
	uint8_t alloc_srcmac[ETH_ALEN];
};

typedef struct {
	char netdev[IFNAMSIZ];
	uint8_t srcmac[ETH_ALEN];
	uint8_t macport;
} netdev_map_t;

static netdev_map_t s_ndevmap_table[MAX_NUMBER_ENET_DEVS];
static int s_num_devs;
static uint32_t s_enet_type;
static uint32_t s_instance_id;
static cb_socket_lldcfg_update_cb_t s_socket_lldcfg_update_cb;

/*
 * static memory allocation case('UB_SD_STATIC' is defined)
 * the memory is statically allocated by the following definitions.
 * the numbers defined here should be overridden by your needs.
 * when 'UB_SD_STATIC' is not defined, the number doesn't have meaning.
 */
#define CB_LLDSOCKET_MMEM cb_lldsocket
#ifndef CB_LLDSOCKET_INSTNUM
// how many this socket instances are used as maximum
#define CB_LLDSOCKET_INSTNUM 10
#endif

UB_SD_GETMEM_DEF(CB_LLDSOCKET_MMEM, (int)sizeof(lld_socket_t),
		 CB_LLDSOCKET_INSTNUM);

static netdev_map_t* find_netdev_map(char *netdev);

static int ovip_socket_open(CB_SOCKET_T *sfd, cb_rawsock_ovip_para_t *ovipp)
{
	UB_LOG(UBL_ERROR,"%s: not supported\n",__func__);
	return -1;
}

static void bufsize_to_num_pkts(uint32_t bufsize, uint32_t *npkts)
{
	*npkts = bufsize / CB_LLD_MAX_ETH_FRAME_SIZE;
}

static int update_enet_cfg(cb_rawsock_paras_t *llrawp, LLDEnetCfg_t *ecfg)
{
	cb_socket_lldcfg_update_t update_cfg;

	if (s_socket_lldcfg_update_cb == NULL) {
		return 0;
	}
	memset(&update_cfg, 0, sizeof(update_cfg));
	update_cfg.dev = llrawp->dev;
	update_cfg.proto = llrawp->proto;
	update_cfg.vlanid = llrawp->vlanid;
	update_cfg.dmaTxChId = -1;
	update_cfg.dmaRxChId = -1;
	if (s_socket_lldcfg_update_cb(&update_cfg) < 0) {
		UB_LOG(UBL_ERROR,"%s:update lldcfg failed\n", __func__);
		return -1;
	}
	if (update_cfg.nTxPkts > 0) {
		ecfg->nTxPkts = update_cfg.nTxPkts;
	}
	if (update_cfg.nRxPkts > 0) {
		ecfg->nRxPkts = update_cfg.nRxPkts;
	}
	if (update_cfg.pktSize > 0) {
		ecfg->pktSize = update_cfg.pktSize;
	}
	if (update_cfg.dmaTxChId >= 0) {
		ecfg->dmaTxChId = update_cfg.dmaTxChId;
	}
	if (update_cfg.dmaRxChId >= 0) {
		ecfg->dmaRxChId = update_cfg.dmaRxChId;
	}
	return 0;
}

int cb_rawsock_open(cb_rawsock_paras_t *llrawp, CB_SOCKET_T *fd, CB_SOCKADDR_LL_T *addr,
		    int *mtusize, ub_macaddr_t bmac)
{
	lld_socket_t *sock = NULL;
	LLDEnetCfg_t ecfg;

	if ((llrawp == NULL) || (fd == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	if (s_num_devs == 0) {
		UB_LOG(UBL_ERROR,"%s:cb_lld_init_devs_table() is not init yet.\n",
			   __func__);
		return -1;
	}
	sock = (lld_socket_t*) UB_SD_GETMEM(CB_LLDSOCKET_MMEM, sizeof(lld_socket_t));
	if (sock == NULL) {
		return -1;
	}
	(void)memset(sock, 0, sizeof(lld_socket_t));

	LLDEnetCfgInit(&ecfg);
	cb_lld_get_type_instance(&ecfg.enetType, &ecfg.instId);
	bufsize_to_num_pkts(llrawp->sndbuf, &ecfg.nTxPkts);
	bufsize_to_num_pkts(llrawp->rcvbuf, &ecfg.nRxPkts);
	if (update_enet_cfg(llrawp, &ecfg) < 0) {
		goto error;
	}

	UB_LOG(UBL_INFO,"%s:dmaTxChId=%d dmaRxChId=%d nTxPkts=%u nRxPkts=%u pktSize=%u\n",
		   __func__, ecfg.dmaTxChId, ecfg.dmaRxChId,
		   ecfg.nTxPkts, ecfg.nRxPkts, ecfg.pktSize);

	sock->lldenet = LLDEnetOpen(&ecfg);
	if (sock->lldenet == NULL) {
		goto error;
	}
	sock->vlanid = llrawp->vlanid;
	strncpy(sock->devname, llrawp->dev, IFNAMSIZ-1);
	if (cb_get_mac_bydev(sock, llrawp->dev, bmac) < 0) {
		goto error;
	}
	/* user does not init dev with src mac, we will alloc it */
	if (MAC_VALID(bmac) == false) {
		if (LLDEnetAllocMac(sock->lldenet, sock->alloc_srcmac) != LLDENET_E_OK) {
			goto error;
		}
		memcpy(bmac, sock->alloc_srcmac, ETH_ALEN);
	}

	if (addr != NULL) {
		memcpy(addr->sll_addr, bmac, ETH_ALEN);
		addr->macport = cb_lld_netdev_to_macport((char *)llrawp->dev);
		addr->tcid = 0;
	}

	*fd = sock;
	return 0;

error:
	cb_rawsock_close(sock);
	return -1;
}

int cb_rawsock_close(CB_SOCKET_T fd)
{
	if (fd == NULL) {
		return -1;
	}
	if (MAC_VALID(fd->alloc_srcmac)) {
		LLDEnetFreeMac(fd->lldenet, fd->alloc_srcmac);
		memset(fd->alloc_srcmac, 0, sizeof(fd->alloc_srcmac));
	}
	if (fd->lldenet != NULL) {
		LLDEnetClose(fd->lldenet);
		fd->lldenet = NULL;
	}
	UB_SD_RELMEM(CB_LLDSOCKET_MMEM, fd);
	return 0;
}

int cb_expand_mtusize(CB_SOCKET_T fd, const char *dev, int *mtusize)
{
	UB_LOG(UBL_ERROR,"%s:unsupported\n",__func__);
	return -1;
}

int cb_set_promiscuous_mode(CB_SOCKET_T sfd, const char *dev, bool enable)
{
	UB_LOG(UBL_ERROR,"%s:unsupported\n",__func__);
	return -1;
}

int cb_get_mac_bydev(CB_SOCKET_T sfd, const char *dev, ub_macaddr_t bmac)
{
	netdev_map_t *ndevmap = find_netdev_map((char*)dev);
	if (ndevmap == NULL) {
		UB_LOG(UBL_ERROR, "%s:no srcmac for netdev %s\n", __func__, dev);
		return -1;
	}

	memcpy(bmac, ndevmap->srcmac, ETH_ALEN);
	if (MAC_VALID(bmac) == false) {
		memcpy(bmac, sfd->alloc_srcmac, ETH_ALEN);
	}
	return 0;
}

int cb_get_ip_bydev(CB_SOCKET_T sfd, const char *dev, CB_IN_ADDR_T *inp)
{
	UB_LOG(UBL_ERROR,"%s:unsupported\n",__func__);
	return -1;
}

int cb_get_brdip_bydev(CB_SOCKET_T sfd, const char *dev, CB_IN_ADDR_T *inp)
{
	UB_LOG(UBL_ERROR,"%s:unsupported\n",__func__);
	return -1;
}

int cb_reg_multicast_address(CB_SOCKET_T fd, const char *dev,
			     const unsigned char *mcastmac, int del)
{
	int result = LLDENET_E_OK;

	if ((fd == NULL) || (dev == NULL) || (mcastmac == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	if (del == 0) {
		/* register multicast and vlan */
		result = LLDEnetFilter(fd->lldenet, (uint8_t *)mcastmac, fd->vlanid);
	} else {
		//Unsupported
	}
	if (result != LLDENET_E_OK) {
		return -1;
	}
	return 0;
}

int cb_get_all_netdevs(int maxdevnum, netdevname_t *netdevs)
{
	UB_LOG(UBL_ERROR,"%s:unsupported\n",__func__);
	return -1;
}

// This API is deprecated and will be removed.
int cb_get_ethtool_info(CB_SOCKET_T cfd, const char *dev,
			uint32_t *speed, uint32_t *duplex)
{
	return cb_lld_get_link_info(cfd, dev, speed, duplex);
}

int cb_lld_get_link_info(CB_SOCKET_T cfd, const char *dev,
		     uint32_t *speed, uint32_t *duplex)
{
	int macport;
	int res;

	if ((cfd == NULL) || (dev == NULL) || (speed == NULL) || (duplex == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}

	macport = cb_lld_netdev_to_macport((char*)dev);
	if (macport < 0) {
		UB_LOG(UBL_ERROR,"%s:no macport for dev %s\n",__func__, dev);
		return -1;
	}
	res = LLDEnetGetLinkInfo(cfd->lldenet, macport, speed, duplex);
	if (res != LLDENET_E_OK) {
		return -1;
	}
	return 0;
}

// This API is deprecated and will be removed.
int cb_get_ethtool_linkstate(CB_SOCKET_T cfd, const char *dev, uint32_t *linkstate)
{
	return cb_lld_get_link_state(cfd, dev, linkstate);
}

int cb_lld_get_link_state(CB_SOCKET_T cfd, const char *dev, uint32_t *linkstate)
{
	int macport;
	bool up;

	if ((cfd == NULL) || (dev == NULL) || (linkstate == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}

	macport = cb_lld_netdev_to_macport((char*)dev);
	if (macport < 0) {
		UB_LOG(UBL_ERROR,"%s:no macport for dev %s\n",
			   __func__, dev);
		return -1;
	}
	up = LLDEnetIsPortUp(cfd->lldenet, macport);
	*linkstate = up ? 1 : 0;
	return -1;
}

int cb_get_netdev_from_ptpdev(char *ptpdev, char *netdev)
{
	if ((ptpdev == NULL) || (netdev == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	strcpy(netdev, ptpdev);
	return 0;
}

int cb_get_ptpdev_from_netdev(char *netdev, char *ptpdev)
{
	if ((ptpdev == NULL) || (netdev == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	strcpy(ptpdev, netdev);
	return 0;
}

/* TI lld specific combase APIs */
int cb_lld_init_devs_table(lld_ethdev_t *ethdevs, uint32_t ndevs,
						   uint32_t enet_type, uint32_t instance_id)
{
	int i;

	if ((ethdevs == NULL) || (ndevs == 0)) {
		UB_LOG(UBL_ERROR, "%s:invalid param\n", __func__);
		return -1;
	}
	if (s_num_devs > 0) {
		UB_LOG(UBL_INFO, "%s: already init %d\n", __func__, s_num_devs);
		return 0;
	}

	if (ndevs > MAX_NUMBER_ENET_DEVS) {
		ndevs = MAX_NUMBER_ENET_DEVS;
	}

	for (i = 0; i < ndevs; i++) {
		if ((ethdevs[i].netdev == NULL) || (ethdevs[i].netdev[0] == 0)) {
			continue;
		}
		strncpy(s_ndevmap_table[i].netdev, ethdevs[i].netdev, IFNAMSIZ-1);
		s_ndevmap_table[i].macport = ethdevs[i].macport;
		memcpy(s_ndevmap_table[i].srcmac, ethdevs[i].srcmac, ETH_ALEN);
		s_num_devs++;
	}
	if (s_num_devs == 0) {
		UB_LOG(UBL_ERROR, "%s:no valid number of devs\n", __func__);
		return -1;
	}
	s_enet_type = enet_type;
	s_instance_id = instance_id;

	return 0;
}

static netdev_map_t* find_netdev_map(char *netdev)
{
	int i;

	if ((netdev == NULL) || (s_num_devs == 0)) {
		return NULL;
	}

	for (i = 0; i < s_num_devs; i++) {
		if (strcmp(s_ndevmap_table[i].netdev, netdev) == 0) {
			return &s_ndevmap_table[i];
		}
	}
	return NULL;
}

// This is a specific API to get all virtual network interfaces
// which were defined for the TI platform.
char** const cb_lld_get_netdevs(void)
{
	int i;
	static char *netdevs[MAX_NUMBER_ENET_DEVS+1];

	if (s_num_devs == 0) {
		UB_LOG(UBL_ERROR, "%s:the cb_lld_init_devs_table() must be called first\n",
		       __func__);
		return NULL;
	}
	for (i = 0; i < s_num_devs; i++) {
		netdevs[i] = s_ndevmap_table[i].netdev;
	}
	netdevs[i] = NULL;

	return netdevs;
}

int cb_lld_netdev_to_macport(char *netdev)
{
	netdev_map_t *ndevmap = find_netdev_map(netdev);
	if (ndevmap == NULL) {
		UB_LOG(UBL_ERROR, "%s:no macport for netdev %s\n", __func__, netdev);
		return -1;
	}
	return ndevmap->macport;
}

int cb_lld_sendto(CB_SOCKET_T sfd, void *data, int size, int flags,
				  CB_SOCKADDR_LL_T *addr, int addrsize)
{
	int result;
	LLDEnetFrame_t eframe = {0};

	if ((data == NULL) || (size == 0) || (addr == NULL) || (addr->macport < 0)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}

	eframe.buf = (uint8_t*)data;
	eframe.size = size;
	eframe.port = addr->macport;
	eframe.tc = addr->tcid - 1;

	result = LLDEnetSend(sfd->lldenet, &eframe);
	if (result != LLDENET_E_OK) {
		UB_LOG(UBL_ERROR,"%s:sent failed %d\n", __func__, result);
		return -1;
	}
	return size;
}

int cb_lld_recv(CB_SOCKET_T sfd, void *buf, int size, int *port)
{
	LLDEnetFrame_t frame;
	int res;

	if ((sfd == NULL) || (buf == NULL) || (size <= 0) || (port == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}

	frame.buf = (uint8_t *)buf;
	frame.size = size;
	res = LLDEnetRecv(sfd->lldenet, &frame);
	if (res != LLDENET_E_OK) {
		if (res == LLDENET_E_NOAVAIL) {
			return 0;
		}
		UB_LOG(UBL_ERROR,"%s:received error %d\n", __func__, res);
		return -1;
	}
	*port = frame.port;
	return frame.size;
}

/* This txnotify_cb and rxnotify_cb callback should be set when calling LLDEnetCfgInit()
 * inside cb_rawsock_open(). However, to keep the cb_rawsock_open() portable with
 * other platforms, user need to call these callbacks after cb_rawsock_open().
 */
int cb_lld_set_txnotify_cb(CB_SOCKET_T sfd, void (*txnotify_cb)(void *arg), void *arg)
{
	if ((sfd == NULL) || (txnotify_cb == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	LLDEnetSetTxNotifyCb(sfd->lldenet, txnotify_cb, arg);
	return 0;
}

int cb_lld_set_rxnotify_cb(CB_SOCKET_T sfd, void (*rxnotify_cb)(void *arg), void *arg)
{
	if ((sfd == NULL) || (rxnotify_cb == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	LLDEnetSetRxNotifyCb(sfd->lldenet, rxnotify_cb, arg);
	return 0;
}

int cb_lld_get_type_instance(uint32_t *enet_type, uint32_t *instance_id)
{
	if ((enet_type == NULL) || (instance_id == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	if (s_num_devs == 0) {
		UB_LOG(UBL_ERROR, "%s:no valid number of devs\n", __func__);
		return -1;
	}
	*enet_type = s_enet_type;
	*instance_id = s_instance_id;
	return 0;
}

int cb_socket_set_lldcfg_update_cb(cb_socket_lldcfg_update_cb_t lldcfg_update_cb)
{
	if (s_socket_lldcfg_update_cb != NULL) {
		UB_LOG(UBL_ERROR, "%s:already set\n", __func__);
		return -1;
	}
	s_socket_lldcfg_update_cb = lldcfg_update_cb;
	return 0;
}
