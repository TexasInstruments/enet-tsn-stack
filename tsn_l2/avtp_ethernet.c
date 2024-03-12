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
 * avtp_ethernet.c
 * avtp(IEEE1722) Ethenert layer for AVBTP
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "avtp_ethernet.h"
#include "ll_avtp_ethernet.h"
#include "avtp_ethernet_private.h"

#define AVTP_ETHHANDLE_MMEM avtp_ethhanle_mem
#ifndef AVTP_ETHHANDLE_NUM
#define AVTP_ETHHANDLE_NUM 4
#endif

#define AVTP_ETHPKT_MMEM avtp_ethpkt_mem
#ifndef AVTP_ETHPKT_SIZE
#define AVTP_ETHPKT_SIZE 1522
#endif
#ifndef AVTP_ETHPKT_NUM
#define AVTP_ETHPKT_NUM 4
#endif

UB_SD_GETMEM_DEF(AVTP_ETHHANDLE_MMEM, (int)sizeof(struct avtp_ethernet_handle),
		 AVTP_ETHHANDLE_NUM);

UB_SD_GETMEM_DEF(AVTP_ETHPKT_MMEM, (int)AVTP_ETHPKT_SIZE, AVTP_ETHHANDLE_NUM);

/**
 * @brief multicast address per network device
 */
typedef struct mcast_filter_t{
    char netdev[CB_MAX_NETDEVNAME+1];
    char mcastmac[ETH_ALEN+1];
} mcast_filter_t;

/**
 * @brief New multicast filters are tracked in mcast_filt_list.
 * @note If the multicast filter was already added in a prev avtpc connection,
 *		no need to call ioctl(SIOCADDMULTI) again. This limits unnecessary
 *		access to the network device.
 */
static ub_esarray_cstd_t *mcast_filt_list;

static mcast_filter_t *find_mcast_filter(const char *netdev, const char *mcastmac)
{
	int i;
	mcast_filter_t *mcast_entry=NULL;
	if(!netdev || !mcastmac || !mcast_filt_list){return NULL;}
	for(i=0;i<ub_esarray_ele_nums(mcast_filt_list);i++){
		mcast_entry=(mcast_filter_t *)ub_esarray_get_ele(mcast_filt_list, i);
		if((mcast_entry) &&
			!strcmp(netdev, mcast_entry->netdev)&&
			!memcmp(mcastmac, mcast_entry->mcastmac, ETH_ALEN)) {
			return mcast_entry;
		}
	}
	return NULL;
}

static void add_mcast_filter(const char *netdev, const char *mcastmac)
{
	mcast_filter_t *mcast_entry;
	if(!netdev || !mcastmac){return;}
	if (!mcast_filt_list) {
		mcast_filt_list = ub_esarray_init(4, sizeof(mcast_filter_t), 16);
	}
	mcast_entry=(mcast_filter_t *)ub_esarray_get_newele(mcast_filt_list);
	if(mcast_entry) {
		strncpy(mcast_entry->netdev, netdev, CB_MAX_NETDEVNAME);
		memcpy(mcast_entry->mcastmac, mcastmac, ETH_ALEN);
	}
	return;
}

static void del_mcast_filter(const char *netdev, const char *mcastmac)
{
	mcast_filter_t *mcast_entry;
	if(!netdev || !mcastmac || !mcast_filt_list){return;}
	mcast_entry = find_mcast_filter(netdev, mcastmac);
	if (mcast_entry){
		ub_esarray_del_pointer(mcast_filt_list,	(ub_esarray_element_t *) mcast_entry);
	}
	if(!ub_esarray_ele_nums(mcast_filt_list)){
		ub_esarray_close(mcast_filt_list);
		mcast_filt_list=NULL;
	}

	return;
}

static bool check_lsm_device(const char *dev, const unsigned char *mcastmac)
{
	char mstr[16];
	sprintf(mstr, "%02X%02X%02X%02X%02X%02X",
		mcastmac[0],mcastmac[1],mcastmac[2],mcastmac[3],mcastmac[4],mcastmac[5]);
	if(strstr(dev,mstr)){return true;}
	return false;
}

int avtp_ethernet_add_mcast(avtp_ethernet_handle_t reh, const char *dev,
			    const unsigned char *mcastmac)
{
	if(check_lsm_device(dev,mcastmac)){return 0;} // QNX lsm device case

	if (!find_mcast_filter(dev, (const char*) mcastmac)) {
		if(UB_NON_ZERO_B6(mcastmac) && cb_reg_multicast_address(reh->sfd, dev, mcastmac, 0)){
			UB_LOG(UBL_ERROR,"%s:failed to set multicast address\n",__func__);
			return -1;
		}
		add_mcast_filter(dev, (const char*) mcastmac);
	} else {
		UB_LOG(UBL_INFO,"%s:multicast address already registered\n",__func__);
	}
	return 0;
}

int avtp_ethernet_del_mcast(avtp_ethernet_handle_t reh, const char *dev,
			    const unsigned char *mcastmac)
{
	if(check_lsm_device(dev,mcastmac)){return 0;} // QNX lsm device case
	if(UB_NON_ZERO_B6(mcastmac) && cb_reg_multicast_address(reh->sfd, dev, mcastmac, 1)){
		UB_LOG(UBL_ERROR,"%s:failed to remove multicast address\n",__func__);
		return -1;
	}
	del_mcast_filter(dev, (const char*) mcastmac);

	return 0;
}

static int normal_mode_socket_open(avtp_ethernet_handle_t reh, avtp_eth_paras_t *ethp)
{
	int res;
	int mtusize;
	cb_rawsock_paras_t llrawp;
	cb_rawsock_ovip_para_t ovipp;

	memset(&llrawp, 0, sizeof(llrawp));
	llrawp.dev=ethp->dev;
	llrawp.proto=ETH_P_TSN;
	llrawp.vlan_proto=ETH_P_TSN;
	llrawp.priority=ethp->priority;
	llrawp.rw_type=ethp->rw_type;
	llrawp.sock_mode=(cb_sock_mode_t)reh->sock_mode;
	llrawp.vlanid=ethp->vlanid;
	if(ethp->rec_tagged){llrawp.proto=ETH_P_8021Q;}
	if(reh->sock_mode==AVTP_SOCK_TEST_MODE){
		memset(&ovipp, 0, sizeof(ovipp));
		ovipp.laddr=127<<24|0<<16|0<<8|1;
		ovipp.daddr=127<<24|0<<16|0<<8|1;
		ovipp.lport=ethp->tm_sport;
		ovipp.dport=ethp->tm_dport;
		llrawp.ovipp=&ovipp;
		llrawp.sock_mode=CB_SOCK_MODE_OVIP;
	}

	/* Don't expand MTU size, unless sure not to affect othere opened sockets */
	mtusize = 0;
	res=cb_rawsock_open(&llrawp, &reh->sfd, &reh->socket_address, &mtusize,
			    reh->eframe.hdr.H_SOURCE);
	reh->sock_mode=llrawp.sock_mode;

	if(res<0){return res;}

	if(reh->readbuf){UB_SD_RELMEM(AVTP_ETHPKT_MMEM, reh->readbuf);}
	reh->readbuf=(void*)UB_SD_GETMEM(AVTP_ETHPKT_MMEM, mtusize);
	if(ub_assert_fatal(reh->readbuf!=NULL, __func__, NULL)) return -1;
	reh->max_datasize = mtusize;

	// some PCs can't receive data without setting promiscuous mode
	if(ethp->promisc){res=cb_set_promiscuous_mode(reh->sfd, ethp->dev, true);}

	return res;
}

avtp_ethernet_handle_t avtp_ethernet_open(avtp_eth_paras_t *ethp,
					  avtp_ethernet_rcv_cb_t rcv_cb, void *cbdata)
{
	struct avtp_ethernet_handle *reh;
	cb_tsn_thread_attr_t attr;
	char thread_name[CB_TSN_THREAD_NAME_SIZE]={0};
	uint32_t duplex;

	reh=(struct avtp_ethernet_handle*)
		UB_SD_GETMEM(AVTP_ETHHANDLE_MMEM,sizeof(struct avtp_ethernet_handle));
	if(ub_assert_fatal(reh!=NULL, __func__, NULL)) return NULL;
	memset(reh,0,sizeof(struct avtp_ethernet_handle));

	if(strstr(ethp->dev, CB_VIRTUAL_ETHDEV_PREFIX)==ethp->dev){
		reh->sock_mode=AVTP_SOCK_TEST_MODE;
	}
	if(normal_mode_socket_open(reh, ethp)){goto erexit;}
	if(cb_get_ethtool_info(reh->sfd, ethp->dev, &reh->speed, &duplex)) {
		reh->speed=0;
		UB_LOG(UBL_INFO, "%s:ethtool failed, reh->speed is set to 0\n", __func__);
	}
	if(CB_THREAD_MUTEX_INIT(&reh->eth_mutex, NULL)){goto erexit;}
	avtp_ethernet_add_mcast(reh, ethp->dev, ethp->mcastmac);
	if(!ll_ethernet_set_cb(reh->sfd, rcv_cb, cbdata, ethp->rec_tagged)) {
		// when ll_avtpe_receive_data thread is not used,
		// rcv_cb should be called from somewhere else
		return reh;
	}

	reh->rcv_cb=rcv_cb;
	reh->cbdata=cbdata;

	if(MKPIPE(reh->pipefd)<0){
		UB_LOG(UBL_INFO,"%s:go ahead without the pipe, it may make problems for closing\n",
		       __func__);
	}
	reh->rec_thread_running=1;
	snprintf(thread_name, CB_TSN_THREAD_NAME_SIZE-1,
		 "avtp_%s_"UB_PRIhexB6, ethp->dev, UB_ARRAY_B6(ethp->mcastmac));
	cb_tsn_thread_attr_init(&attr, THREAD_HIGH_PRI, THREAD_HIGH_STACK, thread_name);
	if(CB_THREAD_CREATE(&reh->rec_thread, &attr, ll_avtpe_receive_data, (void *)reh)){
		UB_LOG(UBL_ERROR,"%s:can't start the receiver thread\n",__func__);
		reh->rec_thread_running=0;
		avtp_ethernet_close(reh);
		goto erexit;
	}
	return reh;
erexit:
	UB_SD_RELMEM(AVTP_ETHHANDLE_MMEM, reh);
	return NULL;
}

void avtp_ethernet_close(avtp_ethernet_handle_t reh)
{
	if(reh->rec_thread_running){
		reh->rec_thread_running=2;

		UB_TLOG(UBL_DEBUG,"%s: closing\n",__func__);
		if(reh->pipefd[1]) {
			CB_WRITE(reh->pipefd[1],"0",1);
		}
		CB_THREAD_JOIN(reh->rec_thread, NULL);
		if(reh->pipefd[1]) {
			CB_CLOSE(reh->pipefd[1]);
			reh->pipefd[1]=0;
		}
		if(reh->pipefd[0]) {
			CB_CLOSE(reh->pipefd[0]);
			reh->pipefd[0]=0;
		}
		UB_LOG(UBL_DEBUG,"joined after killing 'avtp ethrenet receive' thread\n");
	}else{
		UB_LOG(UBL_INFO,"reh->rec_thread_running=%d\n",reh->rec_thread_running);
	}

	CB_SOCK_CLOSE(reh->sfd);
	CB_THREAD_MUTEX_DESTROY(&reh->eth_mutex);
	if(reh->readbuf){UB_SD_RELMEM(AVTP_ETHPKT_MMEM, reh->readbuf);}
	UB_SD_RELMEM(AVTP_ETHHANDLE_MMEM, reh);
}

void avtp_ethernet_set_vlan_proto(avtp_ethernet_handle_t reh, uint16_t proto)
{
	reh->EFRAME_VLANAF.proto = htons(proto);
}

int avtp_ethernet_maxpacket_size(avtp_ethernet_handle_t reh)
{
	return reh->max_datasize;
}

uint8_t *avtp_ethernet_get_smac(avtp_ethernet_handle_t reh)
{
	return reh->eframe.hdr.H_SOURCE;
}

void avtp_ethernet_set_smac(uint8_t *h_source, ub_macaddr_t bmac, const char *strmac)
{
	ub_macaddr_t mac;
	if (strmac){
		sscanf(strmac,"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		       mac, mac+1, mac+2, mac+3, mac+4, mac+5);
		memcpy(h_source,mac,6);
	}
	if (bmac){memcpy(h_source,bmac,6);}
	UB_LOG(UBL_DEBUG,"set new source address: "UB_PRIhexB6"\n", UB_ARRAY_B6(h_source));
}

int avtp_ethernet_set_smac_bydev(avtp_ethernet_handle_t reh, const char *dev)
{
	ub_macaddr_t mac;
	if(cb_get_mac_bydev(CB_SOCKET_INVALID_VALUE, dev, mac)) {
		UB_LOG(UBL_ERROR,"%s:can't get mac for dev=%s\n", __func__, dev);
		return -1;
	}
	avtp_ethernet_set_smac(reh->eframe.hdr.H_SOURCE, mac, NULL);
	return 0;
}

void avtp_ethernet_set_dmac(avtp_ethernet_handle_t reh, ub_macaddr_t bmac, const char *strmac)
{
	ub_macaddr_t mac;

	if (strmac){
		sscanf(strmac,"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		       mac, mac+1, mac+2, mac+3, mac+4, mac+5);
		memcpy(reh->eframe.hdr.H_DEST,mac,6);
		memcpy(reh->socket_address.sll_addr,mac,6);
	}
	if (bmac){
		memcpy(reh->eframe.hdr.H_DEST,bmac,6);
		memcpy(reh->socket_address.sll_addr,bmac,6);
	}
	UB_LOG(UBL_DEBUGV,"set new dst address: "UB_PRIhexB6"\n",
	       UB_ARRAY_B6(reh->eframe.hdr.H_DEST));
}

static void recalc_besteffort_slope(avtp_ethernet_handle_t reh)
{
	int ctc;
	uint32_t tused=0;
	for(ctc=reh->numtc-1;ctc>0;ctc--){
		tused+=reh->tcd[ctc].used_slope;
	}
	reh->tcd[0].tsp.idleslope=UB_MAX(reh->prate*1000-tused, 0);
	reh->tcd[0].credit_max=reh->tcd[0].tsp.traffic_interval*
		reh->tcd[0].tsp.idleslope/1000;
	UB_LOG(UBL_DEBUG, "%s:for tc=0, idleslope=%d credit_max=%"PRIi32"\n",__func__,
	       reh->tcd[0].tsp.idleslope,
	       reh->tcd[0].credit_max);
}

static int traffic_shaper(avtp_ethernet_handle_t reh, int size, uint8_t pcp)
{
	uint8_t tc;
	uint64_t ts;
	int64_t dt; // usec
	bool ininterval=false;

	if(pcp>=8){
		UB_LOG(UBL_ERROR, "%s:invalid pcp=%d\n", __func__, pcp);
		return -1;
	}
	tc=reh->pcp2tc[pcp];
	if(tc>=reh->numtc){
		UB_LOG(UBL_DEBUG, "%s:pcp=%d in not shaped, use tc=0\n", __func__, pcp);
		tc=0;
	}
	size+=12+8+4; // gap + preamble + CRC
	ts=ub_mt_gettime64();
	dt=UB_MIN((ts-reh->tcd[tc].last_ts)/UB_USEC_NS,
		  (uint64_t)reh->tcd[tc].tsp.traffic_interval*10);
	if(dt<reh->tcd[tc].tsp.traffic_interval){
		ininterval=true;
		if(tc>0 && reh->tcd[tc].sent_packets>=reh->tcd[tc].tsp.max_num_packets){
			// too short interval, should wait for return value usec
			UB_LOG(UBL_DEBUGV, "%s:tc=%d, too short interval, sleep %"PRIi64" usec\n",
			       __func__, tc, reh->tcd[tc].tsp.traffic_interval-dt);
			return reh->tcd[tc].tsp.traffic_interval-dt;
		}
	}else{
		reh->tcd[tc].sent_packets=0;
		reh->tcd[tc].credit+=dt*reh->tcd[tc].tsp.idleslope/1000;
		reh->tcd[tc].credit=UB_MIN(reh->tcd[tc].credit, reh->tcd[tc].credit_max);
	}
	if(reh->tcd[tc].credit<0){
		//no TC credit
		if(dt<reh->tcd[tc].tsp.traffic_interval){
			dt=reh->tcd[tc].tsp.traffic_interval-dt;
		}else{
			dt=1;
		}
		if(tc>0) {
			UB_LOG(UBL_DEBUGV, "%s:tc=%d, credit=%"PRIi32", sleep %d usec\n",
			       __func__, tc, reh->tcd[tc].credit,
			       (int)(-reh->tcd[tc].credit*1000/reh->tcd[tc].tsp.idleslope));
		}
		if(tc==0) {
			UB_LOG(UBL_DEBUGV, "%s:no bandwidth for BestEffort, drop a packet\n",
			       __func__);
			return -1;
		}
		return UB_MAX(dt, -reh->tcd[tc].credit*1000/reh->tcd[tc].tsp.idleslope);
	}

	reh->tcd[tc].credit-=size*8;
	reh->tcd[tc].sent_packets++;
	if(!ininterval){reh->tcd[tc].last_ts=ts;}
	return 0;
}

int avtp_ethernet_write_nc(avtp_ethernet_handle_t reh, uint8_t *data,
						   int size, uint16_t pcv)
{
	int res;
	int psize;
	uint8_t *sdata;

	// reh->eframe.hdr.H_SOURCE, H_DEST must be already set
	// reh->eframe.hdr.H_PROTO = 0
	if(pcv){
		//tagged
		reh->eframe.hdr.H_PROTO = htons(ETH_P_8021Q);
		reh->EFRAME_VLANAF.proto = htons(ETH_P_TSN);
		reh->EFRAME_VLANAF.pcv=htons(pcv);
		psize = size + sizeof(CB_ETHHDR_T) + VLAN_AF_SIZE;
	}else{
		//untagged
		data += VLAN_AF_SIZE;
		reh->eframe.hdr.H_PROTO = htons(ETH_P_TSN);
		psize = size + sizeof(CB_ETHHDR_T);
	}
	if (psize > reh->max_datasize){
		UB_LOG(UBL_ERROR,"%s: size=%d is too big, max_datasize=%d\n",
		       __func__, psize, reh->max_datasize);
		return -1;
	}

	if(reh->numtc && pcv){
		res=traffic_shaper(reh, psize, pcv>>13);
		if(res){return res;}
	}

	sdata=data;
	memcpy(data, &reh->eframe.hdr, sizeof(CB_ETHHDR_T));
	data += sizeof(CB_ETHHDR_T);

	if(pcv) {
		memcpy(data, &reh->eframe.u.ed, VLAN_AF_SIZE);
		data += VLAN_AF_SIZE;
	}
	res = ll_avtpe_send_data(reh, sdata, psize);
	UB_TLOG(UBL_DEBUGV,"%s: %d bytes sent.\n", __func__, res);

	if (res < psize){
		/* In blocking mode, this shouldn't happen. */
		UB_LOG(UBL_ERROR,"can't send full size of ethernet packet.\n");
		return -1;
	}
	return 0;
}

// in this function the sign of credit value(cb) is opposite to 802.1Q FQTSS
// when data is sent cb goes positive, when time passes cb goes negative
int avtp_stream_tshaper_sleep(tsshaper_t *tshd, int tsize)
{
	int64_t ts, dts;
	int64_t cb;
	// add L2 header size + gap + preamble + CRC
	tsize+=sizeof(CB_ETHHDR_T) + VLAN_AF_SIZE + 12 + 8 + 4;
	ts=ub_rt_gettime64();
	dts=ts-tshd->last_send_ts;
	if(dts>UB_SEC_NS) {
		tshd->last_send_size=0;
		goto sendexit; // if longer than 1sec, send it anyway
	}

	cb = (int64_t)tshd->tshape_rate * dts / 1000000LL;
	cb = tshd->last_send_size - cb;
	tshd->last_send_size = cb;
	if (cb<=0) {
		tshd->last_send_size=0;
		goto sendexit; // credit became minus, send it
	}

	dts = cb * 1000000LL / (int64_t)tshd->tshape_rate;
	if(dts>UB_SEC_NS){
		UB_LOG(UBL_ERROR,"tshape_rate=%d bytes/msec may be too small, no shaper applied\n",
		       tshd->tshape_rate);
		tshd->last_send_size=0;
		goto sendexit;
	}
	if(dts > tshd->tshape_minsleep){
		return dts;
	}
sendexit:
	tshd->last_send_ts = ts;
	tshd->last_send_size+=tsize;
	return 0;
}

void avtp_ethernet_mutex_lock(avtp_ethernet_handle_t reh)
{
	CB_THREAD_MUTEX_LOCK(&reh->eth_mutex);
}

void avtp_ethernet_mutex_unlock(avtp_ethernet_handle_t reh)
{
	CB_THREAD_MUTEX_UNLOCK(&reh->eth_mutex);
}

void avtp_set_traffic_shaper(avtp_ethernet_handle_t reh, int numtc,
			     uint32_t prate, uint8_t* pcp2tc,
			     avtp_traffic_shaper_prams_t *tsp)
{
	int i;
	int tdbw=0;
	reh->numtc=numtc;
	if(!reh->numtc){return;}
	if(!prate){prate=reh->speed;}
	if(!prate){
		UB_LOG(UBL_WARN, "%s:no speed information, no TC shaper is used\n",
		       __func__);
		reh->numtc=0;
		return;
	}
	reh->prate=prate;
	for(i=0;i<8;i++) reh->pcp2tc[i]=pcp2tc[i];
	for(i=0;i<numtc;i++) {
		memcpy(&reh->tcd[i].tsp, &tsp[i], sizeof(avtp_traffic_shaper_prams_t));
		if(i>0){
			reh->tcd[i].credit_max=reh->tcd[i].tsp.traffic_interval*
				reh->tcd[i].tsp.idleslope/1000;
			tdbw+=reh->tcd[i].tsp.delta_bw;
		}
	}
	if(tdbw>100){
		UB_LOG(UBL_ERROR, "%s:total delta bandwidh=%d>100, disable shaper\n",
		       __func__, tdbw);
		reh->numtc=0;
		return;
	}
	// set for the best effort
	reh->tcd[0].tsp.delta_bw=100-tdbw;
	reh->tcd[0].tsp.idleslope=reh->prate*1000; // start with the full bandwidth
	reh->tcd[0].credit_max=reh->tcd[0].tsp.traffic_interval*
		(reh->tcd[0].tsp.idleslope/1000);
}

int avtp_update_traffic(avtp_ethernet_handle_t reh, uint8_t tc, int32_t add_slope)
{
	int32_t tdbw=0;
	int32_t tused=0;
	int ctc;
	if(tc>=reh->numtc){
		UB_LOG(UBL_ERROR, "%s:invalid tc=%d\n", __func__, tc);
		return -1;
	}

	// tc=0 can allow overflow, and always allow this update
	if(tc==0){return 0;}

	for(ctc=reh->numtc-1;ctc>=tc;ctc--){
		tdbw+=reh->tcd[ctc].tsp.delta_bw;
		tused+=reh->tcd[ctc].used_slope;
	}
	if(tused+add_slope > (int)(tdbw*reh->prate*10u)){
		UB_LOG(UBL_WARN, "%s:tc=%d, total used=%"PRIi32"kbps, available=%dkbps "
		       "can't add %dkbps\n",
		       __func__, tc, tused, (int)(tdbw*reh->prate*10), add_slope);
		return -1;
	}
	if(reh->tcd[tc].used_slope+add_slope > (int)reh->tcd[tc].tsp.idleslope){
		UB_LOG(UBL_WARN, "%s:tc=%d, used in tc=%"PRIi32"kbps, available=%dkbps "
		       "can't add %dkbps\n",
		       __func__, tc, reh->tcd[tc].used_slope,
		       reh->tcd[tc].tsp.idleslope, add_slope);
		return -1;
	}

	reh->tcd[tc].used_slope+=add_slope;
	recalc_besteffort_slope(reh);
	if(reh->tcd[tc].used_slope < 0){
		UB_LOG(UBL_DEBUG, "%s:tc=%d, used_slope=%"PRIi32", change to 0\n", __func__,
		       tc, reh->tcd[tc].used_slope);
		reh->tcd[tc].used_slope=0;
		return 0;
	}
	UB_LOG(UBL_DEBUG, "%s:tc=%d, add_slope=%d, used_slope changed to %"PRIi32"\n", __func__,
	       tc, add_slope, reh->tcd[tc].used_slope);
	return 0;
}
