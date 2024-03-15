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
 * tilld_ll_avtp_ethernet.c
 */
#include "avtp_ethernet.h"
#include "avtp_ethernet_private.h"
#include "ll_avtp_ethernet.h"

#ifndef AVTP_USE_TILLD_RX_ZERO_COPY
#define AVTP_USE_TILLD_RX_ZERO_COPY 1
#endif

struct avtp_tilld {
	CB_SEM_T semaphore;
	bool init;
};

static struct avtp_tilld s_tilld;

static void rx_notify_cb(void *arg)
{
	struct avtp_tilld *tilld = (struct avtp_tilld *)arg;
	CB_SEM_POST(&tilld->semaphore);
}

static int tilld_init(void *arg)
{
	struct avtp_tilld *tilld = (struct avtp_tilld *)arg;
	if (tilld->init) {return -1;}
	if (CB_SEM_INIT(&tilld->semaphore, 0, 0) < 0) {
		UB_LOG(UBL_ERROR,"%s:failed to open sem!\n", __func__);
		return -2;
	}
	tilld->init = true;
#if AVTP_USE_TILLD_RX_ZERO_COPY == 1
	UB_LOG(UBL_INFO,"%s: enable Rx Zero-Copy\n", __func__);
#else
	UB_LOG(UBL_INFO,"%s: disable Rx Zero-Copy!\n", __func__);
#endif

	return 0;
}

static void consume_avtp_packet(avtp_ethernet_handle_t reh,
								ether_frame_t *rec_eframe, int eframe_size)
{
	uint8_t *payload;
	int pldsize;
	bool tagged=0;
	uint16_t pcv=0;

	if (eframe_size < (sizeof(struct ethhdr) + VLAN_AF_SIZE)) {return;}
	/*
	 * either tagged or untagged packets can come here.
	 * if 0x8100 and 0x22f0 mathced, it is tagged packet.
	 * if 0x22f0 matched, it is untagged packet.
	 * otheres are ignored.
	*/
	if (rec_eframe->hdr.H_PROTO == htons(ETH_P_8021Q)){
		if (rec_eframe->u.ed.vlanaf.proto != htons(ETH_P_TSN)){return;}
		tagged=true;
		pcv=ntohs(rec_eframe->u.ed.vlanaf.pcv);
		payload=rec_eframe->u.ed.data;
		pldsize=eframe_size- (sizeof(struct ethhdr) + VLAN_AF_SIZE);
	}else{
		if (rec_eframe->hdr.H_PROTO != htons(ETH_P_TSN)){return;}
		tagged=false;
		payload=rec_eframe->u.data;
		pldsize=eframe_size- sizeof(struct ethhdr);
	}

	UB_LOG(UBL_DEBUG,"%s:%d bytes of %s data has been received from "
		   UB_PRIhexB6" to "UB_PRIhexB6"\n",
		   __func__, eframe_size, tagged ? "tagged" : "untagged",
		   UB_ARRAY_B6(rec_eframe->hdr.H_SOURCE),
		   UB_ARRAY_B6(rec_eframe->hdr.H_DEST));

	if (reh->rcv_cb){
		reh->rcv_cb(payload, pldsize, &rec_eframe->hdr, reh->cbdata, pcv);
	}
#if !UB_LOG_IS_COMPILED(UBL_DEBUG)
	(void)tagged;
#endif
}

#if AVTP_USE_TILLD_RX_ZERO_COPY == 1
static void zerocopy_recv_cb(void *buf, int size,
				CB_SOCKADDR_LL_T *addr, void *cbarg)
{
	consume_avtp_packet((avtp_ethernet_handle_t)cbarg, (ether_frame_t *)buf, size);
}

static void poll_data_until_empty(avtp_ethernet_handle_t reh)
{
	int res;
	while(reh->rec_thread_running==1){
		res = cb_lld_recv_zerocopy(reh->sfd, zerocopy_recv_cb, reh);
		if (res < 0) {
			UB_LOG(UBL_ERROR,"%s:failed to receive data\n", __func__);
			break;
		}
		if (res == 0) {break;} /* no more data */

		/* indicate that the call receive data but for other apps */
		if (res == 0xFFFF) {continue;}
	}
}
#else //AVTP_USE_TILLD_RX_ZERO_COPY != 1
static void poll_data_until_empty(avtp_ethernet_handle_t reh)
{
	ether_frame_t *rec_eframe;
	int res;
	CB_SOCKADDR_LL_T addr;

	rec_eframe=(ether_frame_t *)reh->readbuf;

	while(reh->rec_thread_running==1){
		res = cb_lld_recv(reh->sfd, rec_eframe, reh->max_datasize,
						&addr, sizeof(CB_SOCKADDR_LL_T));
		if (res < 0) {
			UB_LOG(UBL_ERROR,"%s:failed to receive data\n", __func__);
			break;
		}
		if (res == 0) {break;} /* no more data */

		/* indicate that the call receive data but for other apps */
		if (res == 0xFFFF) {continue;}

		consume_avtp_packet(reh, rec_eframe, res);
	}
}
#endif //AVTP_USE_TILLD_RX_ZERO_COPY != 1

// the receiver thread from avtp_ethernet
void *ll_avtpe_receive_data(void *ptr)
{
	avtp_ethernet_handle_t reh = (avtp_ethernet_handle_t)ptr;
	int res;
	uint64_t ts64;
	struct timespec ts;

	/* We can open only single thread to receive data from all the ports.*/
	UB_PROTECTED_FUNC(tilld_init, res, (void *)&s_tilld);
	if (res < 0) {
		UB_LOG(UBL_ERROR,"%s:can't use multiple netdevs, exit recv thread!\n",__func__);
		return NULL;
	}

	cb_lld_set_rxnotify_cb(reh->sfd, rx_notify_cb, &s_tilld);

	while(reh->rec_thread_running==1){
		ts64=ub_rt_gettime64();
		UB_NSEC2TS(ts64+UB_SEC_NS , ts);
		res = CB_SEM_TIMEDWAIT(&s_tilld.semaphore, &ts);
		if (res) {continue;}
		poll_data_until_empty(reh);
	}
	UB_LOG(UBL_INFO,"%s:exit the receiver thread\n",__func__);
	return NULL;
}

int ll_ethernet_set_cb(CB_SOCKET_T fd, avtp_ethernet_rcv_cb_t rcv_cb,
					   void *cbdata, uint16_t proto)
{
	return -1;
}

int ll_avtpe_send_data(avtp_ethernet_handle_t reh, uint8_t *data, int size)
{
	return CB_SOCK_SENDTO(reh->sfd, data, size, 0,
			     (const struct sockaddr*)&reh->socket_address,
			     sizeof(reh->socket_address));
}
