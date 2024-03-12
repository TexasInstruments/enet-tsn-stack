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
 * ix_ll_avtp_ethernet.c
 * avtp ethernet functions, Linux platform specific part
 *
 */

#include <errno.h>
#include "avtp_ethernet.h"
#include "avtp_ethernet_private.h"
#include "ll_avtp_ethernet.h"

// the receiver thread from avtp_ethernet
void *ll_avtpe_receive_data(void *ptr)
{
	ether_frame_t *rec_eframe;
	int res;
	avtp_ethernet_handle_t reh = (avtp_ethernet_handle_t)ptr;
	fd_set rfds;
	int maxfd __attribute__((unused));
	uint8_t *payload;
	int pldsize;
	bool tagged=0;
	uint16_t pcv=0;

	rec_eframe=(ether_frame_t *)reh->readbuf;
	maxfd=UB_MAX((int)reh->sfd, (int)reh->pipefd[0]);

	while(reh->rec_thread_running==1){
		FD_ZERO(&rfds);
		FD_SET(reh->sfd, &rfds);
		if(reh->pipefd[0]){FD_SET(reh->pipefd[0], &rfds);}
		res=select(maxfd+1,&rfds,NULL,NULL,NULL);
		if (res == -1){
			reh->rec_thread_running=2;
			UB_LOG(UBL_ERROR,"%s: %s\n", __func__, strerror(errno));
			break;
		}
		if(FD_ISSET(reh->pipefd[0],&rfds)){
			reh->rec_thread_running=2;
			UB_LOG(UBL_INFO,"%s:receiver thread was terminated\n",__func__);
			break;
		}

		res=recv(reh->sfd, rec_eframe, reh->max_datasize, 0);
		if(res<0){
			if(reh->sock_mode==AVTP_SOCK_TEST_MODE && errno==ECONNREFUSED){
				usleep(100000); // need to wait a connection
				continue;
			}
			UB_LOG(UBL_ERROR,"%s:failed to receive a packet, %s\n",
			       __func__, strerror(errno));
			break;
		}
		// UB_LOG(UBL_DEBUG,"received %d bytes \n", res); // to show unfiltered packets
		/*
		  either tagged or untagged packets can come here.
		  if 0x8100 and 0x22f0 mathced, it is tagged packet.
		  if 0x22f0 matched, it is untagged packet.
		  otheres are ignored.
		 */
		if (rec_eframe->hdr.H_PROTO == htons(ETH_P_8021Q)){
			if (rec_eframe->u.ed.vlanaf.proto != htons(ETH_P_TSN)){continue;}
			tagged=true;
			pcv=ntohs(rec_eframe->u.ed.vlanaf.pcv);
			payload=rec_eframe->u.ed.data;
			pldsize=res - (sizeof(struct ethhdr) + VLAN_AF_SIZE);
		}else{
			if (rec_eframe->hdr.H_PROTO != htons(ETH_P_TSN)){continue;}
			tagged=false;
			payload=rec_eframe->u.data;
			pldsize=res - sizeof(struct ethhdr);
		}

		UB_LOG(UBL_DEBUG,"%s:%d bytes of %s data has been received from "
		       UB_PRIhexB6" to "UB_PRIhexB6"\n",
		       __func__, res, tagged ? "tagged" : "untagged",
		       UB_ARRAY_B6(rec_eframe->hdr.H_SOURCE),
		       UB_ARRAY_B6(rec_eframe->hdr.H_DEST));
		//ub_hexdump((uint8_t*)&rec_eframe,res,0);

		if (! reh->rcv_cb){
			UB_LOG(UBL_DEBUG,"%s:no callback function, ignore this data\n",__func__);
			continue;
		}

		reh->rcv_cb(payload, pldsize,
			    &rec_eframe->hdr, reh->cbdata, pcv);
	}
	UB_LOG(UBL_INFO,"%s:exit the receiver thread\n",__func__);
	pthread_exit(NULL);
	return NULL;
}

int ll_ethernet_set_cb(int fd, avtp_ethernet_rcv_cb_t rcv_cb,
					   void *cbdata, uint16_t proto)
{
	return -1;
}

// return true if fd is not writable in 1 second
static bool send_timeout_1sec(CB_SOCKET_T fd)
{
	fd_set wfds;
	struct timeval tv={1,0};
	int res;
	FD_ZERO(&wfds);
	FD_SET(fd, &wfds);
	res=select(fd+1, NULL, &wfds, NULL, &tv);
	if(!res){return true;}
	return false;
}

int ll_avtpe_send_data(avtp_ethernet_handle_t reh, uint8_t *data, int size)
{
	int res;

	if(send_timeout_1sec(reh->sfd)){
		UB_TLOG(UBL_WARN,"%s:fd is not writable for 1sec\n", __func__);
		return -1;
	}
	res = CB_SOCK_SENDTO(reh->sfd, data, size, 0,
			     (const struct sockaddr*)&reh->socket_address,
			     sizeof(reh->socket_address));
	if (res == -1){
		if(reh->sock_mode==AVTP_SOCK_TEST_MODE && errno==ECONNREFUSED){
			// need to wait a connection, throw away the data
			return 0;
		}
		UB_LOG(UBL_ERROR,"%s: %s\n", __func__, strerror(errno));
		return -1;
	}
	return res;
}
