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
#include "lld_ethernet_private.h"
#include "cb_lld_thread.h"
#include "lldenetext.h"

#define MAC_VALID(mac) (((mac)[0]|(mac)[1]|(mac)[2]|(mac)[3]|(mac)[4]|(mac)[5]) != 0)
#define CB_LLD_PRIORITY_MAX (7U)

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

static netdev_map_t *find_netdev_map(const char *netdev);

static int ovip_socket_open(CB_SOCKET_T *sfd, cb_rawsock_ovip_para_t *ovipp)
{
	UB_LOG(UBL_ERROR,"%s: not supported\n",__func__);
	return -1;
}

static void bufsize_to_num_pkts(uint32_t bufsize, uint32_t *npkts)
{
	*npkts = bufsize / CB_LLD_MAX_ETH_FRAME_SIZE;
}

static int update_enet_cfg(cb_rawsock_paras_t *llrawp, LLDEnetCfg_t *ecfg,
				cb_socket_lldcfg_update_t *update_cfg)
{
	uint32_t i;
	int res;

	update_cfg->dev = llrawp->dev;
	update_cfg->proto = llrawp->proto;
	update_cfg->vlanid = llrawp->vlanid;
	update_cfg->dmaTxChId = -1;
	for (i = 0U; i < MAX_NUM_RX_DMA_CH_PER_INSTANCE; i++) {
		update_cfg->dmaRxChId[i] = -1;
	}
	update_cfg->unusedDmaRx = -1;
	update_cfg->unusedDmaTx = -1;
	update_cfg->dmaRxShared = -1;
	update_cfg->dmaRxOwner = -1;
	update_cfg->numRxChannels = 0;
	if (s_socket_lldcfg_update_cb != NULL) {
		UB_PROTECTED_FUNC(s_socket_lldcfg_update_cb, res, update_cfg);
		if (res != 0) {
			UB_LOG(UBL_ERROR,"%s:update lldcfg failed\n", __func__);
			return -1;
		}
	}
	if (update_cfg->nTxPkts > 0) {
		ecfg->nTxPkts = update_cfg->nTxPkts;
	}
	for (i = 0U; i < MAX_NUM_RX_DMA_CH_PER_INSTANCE; i++) {
		if (update_cfg->nRxPkts[i] > 0) {
			ecfg->nRxPkts[i] = update_cfg->nRxPkts[i];
		}
		if (update_cfg->dmaRxChId[i] >= 0) {
			ecfg->dmaRxChId[i] = update_cfg->dmaRxChId[i];
		}
	}
	if (update_cfg->pktSize > 0) {
		ecfg->pktSize = update_cfg->pktSize;
	}
	if (update_cfg->dmaTxChId >= 0) {
		ecfg->dmaTxChId = update_cfg->dmaTxChId;
	}
	if (update_cfg->numRxChannels >= 0) {
		ecfg->numRxChannels = update_cfg->numRxChannels;
	}
	if (llrawp->proto == ETH_P_NETLINK) {
		ecfg->unusedDmaRx = true; /* always not use DMA */
		ecfg->unusedDmaTx = true; /* always not use DMA */
	} else {
		if (update_cfg->unusedDmaRx >= 0) {
			ecfg->unusedDmaRx = (update_cfg->unusedDmaRx > 0) ? true : false;
		}
		if (update_cfg->unusedDmaTx >= 0) {
			ecfg->unusedDmaTx = (update_cfg->unusedDmaTx > 0) ? true : false;
		}
		if (update_cfg->dmaRxShared >= 0) {
			ecfg->dmaRxShared = (update_cfg->dmaRxShared > 0) ? true : false;
		}
		if (update_cfg->dmaRxOwner >= 0) {
			ecfg->dmaRxOwner = (update_cfg->dmaRxOwner > 0) ? true : false;
		}
	}

	return 0;
}

static void update_mac_addr(CB_SOCKET_T sfd,
				   const char *dev, ub_macaddr_t bmac)
{
	int i;
	for (i = 0; i < s_num_devs; i++) {
		if (!strncmp(dev, s_ndevmap_table[i].netdev, strlen(dev))) {
			memcpy(s_ndevmap_table[i].srcmac, bmac, ETH_ALEN);
			s_ndevmap_table[i].sock = sfd;
			break;
		}
	}
}

static void free_mac_addr(CB_SOCKET_T sfd)
{
	int i;

	for (i = 0; i < s_num_devs; i++) {
		if (s_ndevmap_table[i].sock == sfd) { /* multiple macs can have the same fd */
			if (MAC_VALID(s_ndevmap_table[i].srcmac)) {
				LLDEnetFreeMac(sfd->lldenet, s_ndevmap_table[i].srcmac);
				s_ndevmap_table[i].sock = NULL;
				memset(s_ndevmap_table[i].srcmac, 0, ETH_ALEN);
			}
		}
	}
}

static int find_mac_addr_bydev(CB_SOCKET_T sfd, const char *dev, ub_macaddr_t bmac)
{
	int res = LLDENET_E_PARAM;
	netdev_map_t *ndevmap = find_netdev_map((char *)dev);

	if (ndevmap == NULL) {
		UB_LOG(UBL_ERROR, "%s:no srcmac for netdev %s\n", __func__, dev);
		return res;
	}

	if (!MAC_VALID(ndevmap->srcmac)) {
		res = LLDENET_E_NOAVAIL;
	} else {
		memcpy(bmac, ndevmap->srcmac, ETH_ALEN);
		res = LLDENET_E_OK;
	}

	return res;
}

/* If can not find, will try to allocate a new mac */
static int alloc_mac_addr(const char *ifname,
				 lld_socket_t *sock, ub_macaddr_t bmac)
{
	int res = -1;

	if ((res = find_mac_addr_bydev(sock, ifname, bmac)) != LLDENET_E_NOAVAIL) {
		return (res == LLDENET_E_OK? 0: -1);
	}

	/* user does not init dev with src mac, we will alloc it */
	if (LLDEnetAllocMac(sock->lldenet, bmac) != LLDENET_E_OK) {
		UB_LOG(UBL_ERROR, "Failed to alloc MAC address! \n");
	} else {
		UB_LOG(UBL_INFO,"%s: alloc mac: "UB_PRIhexB6"\n", ifname, UB_ARRAY_B6(bmac));
		/* Update this macaddr to a global table to share mac address
		 * with other socket instance which is openned on the same port */
		update_mac_addr(sock, ifname, bmac);
		res = 0;
	}

	return res;
}

static int get_macports(uint8_t *mac_ports)
{
	int i;
	for (i = 0; i < s_num_devs; i++) {
		mac_ports[i] = s_ndevmap_table[i].macport;
	}
	return i;
}

int cb_rawsock_open(cb_rawsock_paras_t *llrawp, CB_SOCKET_T *fd, CB_SOCKADDR_LL_T *addr,
			int *mtusize, ub_macaddr_t bmac)
{
	lld_socket_t *sock = NULL;
	LLDEnetCfg_t ecfg;
	cb_socket_lldcfg_update_t update_cfg;
	uint32_t i;

	memset(&update_cfg, 0, sizeof(cb_socket_lldcfg_update_t));

	UB_LOG(UBL_INFO, "%s:combase-"TSNPKGVERSION"\n", __func__);

	if ((llrawp == NULL) || (fd == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	if (s_num_devs == 0) {
		UB_LOG(UBL_ERROR,"%s:cb_lld_init_devs_table() is not init yet.\n",
			   __func__);
		return -1;
	}
	sock = (lld_socket_t*)UB_SD_GETMEM(CB_LLDSOCKET_MMEM, sizeof(lld_socket_t));
	if (sock == NULL) {
		UB_LOG(UBL_ERROR,"%s:SOCK is NULL\n",
			   __func__);
		return -1;
	}
	(void)memset(sock, 0, sizeof(lld_socket_t));

	LLDEnetCfgInit(&ecfg);
	cb_lld_get_type_instance(&ecfg.enetType, &ecfg.instId);
	bufsize_to_num_pkts(llrawp->sndbuf, &ecfg.nTxPkts);
	for (i = 0U; i < MAX_NUM_RX_DMA_CH_PER_INSTANCE; i++) {
		bufsize_to_num_pkts(llrawp->rcvbuf, &ecfg.nRxPkts[i]);
	}
	ecfg.ethType = llrawp->proto;
	ecfg.vlanId = llrawp->vlanid;
	if (update_enet_cfg(llrawp, &ecfg, &update_cfg) < 0) {
		UB_LOG(UBL_ERROR,"%s:update_enet_cfg() failure.\n",
			   __func__);
		goto error;
	}
	UB_LOG(UBL_INFO,"%s:dmaTxChId=%d numRxChannels=%d dmaRxChId=%d"
		   " nTxPkts=%u nRxPkts=%u pktSize=%u\n",
		   __func__, ecfg.dmaTxChId, ecfg.numRxChannels, ecfg.dmaRxChId[0],
		   ecfg.nTxPkts, ecfg.nRxPkts[0], ecfg.pktSize);

	sock->lldenet = LLDEnetOpen(&ecfg);
	if (sock->lldenet == NULL) {
		UB_LOG(UBL_ERROR,"%s:sock->lldenet() failure.\n",
			   __func__);
		goto error;
	}
	sock->vlanid = llrawp->vlanid;
	sock->eth_type = llrawp->proto;
	if (cb_get_mac_bydev(sock, llrawp->dev, bmac)) {
		UB_LOG(UBL_ERROR,"%s:alloc_mac_addr() failure.\n",
			   __func__);
		goto error;
	}
	strncpy(sock->devname, llrawp->dev, CB_MAX_NETDEVNAME-1);
	if (addr != NULL) {
		memcpy(addr->sll_addr, bmac, ETH_ALEN);
		addr->macport = cb_lld_netdev_to_macport((char *)llrawp->dev);
		addr->tcid = 0;
	}
	if (update_cfg.rxDefaultDataCb != NULL) {
		if (cb_lld_set_default_rxdata_cb(sock, update_cfg.rxDefaultDataCb,
					update_cfg.rxDefaultCbArg) < 0) {
		UB_LOG(UBL_ERROR,"%s:cb_lld_set_default_rxdata_cb() failure.\n",
			   __func__);
			goto error;
		}
	}

	if (mtusize != NULL) {
		*mtusize = CB_LLD_MAX_ETH_FRAME_SIZE;
	}
	if (sock->eth_type == ETH_P_1588) {
		/* Map DMA Tx channel ID of untagged packets to highest HW queue */
		uint8_t mac_ports[MAX_NUMBER_ENET_DEVS];
		int res;
		UB_PROTECTED_FUNC(get_macports, res, mac_ports);
		LLDEnetEnableQueueDMAChannelMapping(sock->lldenet, mac_ports,
						    res, CB_LLD_PRIORITY_MAX);
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

	UB_PROTECTED_FUNC_VOID(free_mac_addr, fd);

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
	int res;
	UB_PROTECTED_FUNC(alloc_mac_addr, res, dev, sfd, bmac);
	return res;
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
		result = LLDEnetFilter(fd->lldenet, (uint8_t *)mcastmac,
							   fd->vlanid, fd->eth_type);
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
	return 0;
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
static int lld_init_devs_table(lld_ethdev_t *ethdevs, uint32_t ndevs,
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
		strncpy(s_ndevmap_table[i].netdev, ethdevs[i].netdev, CB_MAX_NETDEVNAME-1);
		s_ndevmap_table[i].macport = ethdevs[i].macport;
		memcpy(s_ndevmap_table[i].srcmac, ethdevs[i].srcmac, ETH_ALEN);
		UB_LOG(UBL_INFO,"%s: has mac: "UB_PRIhexB6"\n",
			   s_ndevmap_table[i].netdev, UB_ARRAY_B6(s_ndevmap_table[i].srcmac));
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

int cb_lld_init_devs_table(lld_ethdev_t *ethdevs, uint32_t ndevs,
						   uint32_t enet_type, uint32_t instance_id)
{
	int res;
	UB_PROTECTED_FUNC(lld_init_devs_table, res, ethdevs,
					  ndevs, enet_type, instance_id);
	return res;
}

static netdev_map_t *find_netdev_map(const char *netdev)
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

static int lld_get_netdevs(char* netdevs[], int *len)
{
	int i;

	if (s_num_devs == 0) {
		UB_LOG(UBL_ERROR, "%s:the cb_lld_init_devs_table() must be called first\n",
			   __func__);
		return -1;
	}
	for (i = 0; i < s_num_devs; i++) {
		netdevs[i] = s_ndevmap_table[i].netdev;
	}
	*len = i;
	return 0;
}

int cb_lld_get_netdevs(char* netdevs[], int *len)
{
	int res;
	UB_PROTECTED_FUNC(lld_get_netdevs, res, netdevs, len);
	return res;
}

static int lld_netdev_to_macport(const char *netdev)
{
	int macport = -1;
	netdev_map_t *ndevmap = NULL;

	ndevmap = find_netdev_map(netdev);
	if (ndevmap == NULL) {
		UB_LOG(UBL_ERROR, "%s:no macport for netdev %s\n", __func__, netdev);
	} else {
		macport = ndevmap->macport;
	}

	return macport;
}

int cb_lld_netdev_to_macport(const char *netdev)
{
	int res;
	UB_PROTECTED_FUNC(lld_netdev_to_macport, res, netdev);
	return res;
}

int cb_lld_sendto(CB_SOCKET_T sfd, void *data, int size, int flags,
				 const CB_SOCKADDR_LL_T *addr, int addrsize)
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

int cb_lld_recv(CB_SOCKET_T sfd, void *buf, int size,
		CB_SOCKADDR_LL_T *addr, int addrsize)
{
	LLDEnetFrame_t frame;
	int res;

	if ((sfd == NULL) || (buf == NULL) || (size <= 0) || (addr == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}

	memset(&frame, 0, sizeof(frame));
	frame.buf = (uint8_t *)buf;
	frame.size = size;
	res = LLDEnetRecv(sfd->lldenet, &frame);
	if (res != LLDENET_E_OK) {
		if (res == LLDENET_E_NOAVAIL) {
			return 0;
		}
		if (res == LLDENET_E_NOMATCH) {
			return 0xFFFF;
		}
		UB_LOG(UBL_ERROR,"%s:received error %d\n", __func__, res);
		return -1;
	}
	addr->macport = frame.port;
	addr->rxts = frame.rxts;

	return frame.size;
}

typedef struct {
	cb_lld_zerocopy_recv_cb_t cblld_recv_cb;
	void *cblld_cbarg;
} LLDEnetRecvCbArg_t;

static void LLDEnetRecvCb(LLDEnetFrame_t *frame, void *cbarg)
{
	CB_SOCKADDR_LL_T addr;
	LLDEnetRecvCbArg_t *lldEnetCbArg = (LLDEnetRecvCbArg_t *)cbarg;

	memset(&addr, 0, sizeof(addr));
	addr.macport = frame->port;
	addr.rxts = frame->rxts;
	lldEnetCbArg->cblld_recv_cb(frame->buf, frame->size, &addr,
								lldEnetCbArg->cblld_cbarg);
}

int cb_lld_recv_zerocopy(CB_SOCKET_T sfd, cb_lld_zerocopy_recv_cb_t cblld_recv_cb,
						 void *cbarg)
{
	LLDEnetRecvCbArg_t lldEnetCbArg;
	int res;

	if (sfd == NULL) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}

	memset(&lldEnetCbArg, 0, sizeof(lldEnetCbArg));
	lldEnetCbArg.cblld_recv_cb = cblld_recv_cb;
	lldEnetCbArg.cblld_cbarg = cbarg;
	res = LLDEnetRecvZeroCopy(sfd->lldenet, LLDEnetRecvCb, &lldEnetCbArg);
	if (res != LLDENET_E_OK) {
		if (res == LLDENET_E_NOAVAIL) {
			return 0;
		}
		if (res == LLDENET_E_NOMATCH) {
			return 0xFFFF;
		}
		UB_LOG(UBL_ERROR,"%s:received error %d\n", __func__, res);
		return -1;
	}
	return 1;
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

int cb_lld_set_default_rxdata_cb(CB_SOCKET_T sfd,
	void (*default_rxdata_cb)(void *data, int size, int port, void *arg), void *arg)
{
	if ((sfd == NULL) || (default_rxdata_cb == NULL)) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return -1;
	}
	if(LLDEnetSetDefaultRxDataCb(sfd->lldenet, default_rxdata_cb, arg) != LLDENET_E_OK){
		UB_LOG(UBL_ERROR,"%s:unable to set default rx cb, maybe already set?\n",
			   __func__);
		return -1;
	}
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

int cb_lld_get_port_stats(CB_SOCKET_T sfd, int port, cb_tilld_port_stats_t *stats)
{
	int res;
	res = LLDEnetGetPortStats(sfd->lldenet, port, stats, sizeof(cb_tilld_port_stats_t));

	return (res == LLDENET_E_OK? 0: -1);
}

void cb_lld_reset_port_stats(CB_SOCKET_T sfd, int port)
{
	LLDEnetResetPortStats(sfd->lldenet, port);
}
