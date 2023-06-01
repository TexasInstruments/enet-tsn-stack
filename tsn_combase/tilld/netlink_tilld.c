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

struct socket_handle {
	CB_SOCKET_T fd;
	char *netdev; // We don't need this if the cb_lld_ethernet exposes the `lld_socket` to public
};

struct combase_link_data {
	struct socket_handle fds[MAX_NUMBER_ENET_DEVS];
	uint8_t num_fds; // Number of active raw sockets
	cbl_cb_t event_cb;
	void *cb_arg;
};

#define CBL_DATA_INSTMEM cbl_data_inst
#ifndef CBL_DATA_INSTNUM
#define CBL_DATA_INSTNUM 1
#endif

UB_SD_GETMEM_DEF(CBL_DATA_INSTMEM, (int)sizeof(struct combase_link_data),
		 CBL_DATA_INSTNUM);

combase_link_data_t *combase_link_init(cbl_cb_t event_cb, void *cb_arg)
{
	combase_link_data_t *cbld;
	ub_macaddr_t srcmac;

	cbld = UB_SD_GETMEM(CBL_DATA_INSTMEM,
			    sizeof(struct combase_link_data));
	if (ub_assert_fatal(cbld!=NULL, __func__, NULL)) { return NULL; }
	memset(cbld, 0, sizeof(struct combase_link_data));
	cbld->event_cb = event_cb;
	cbld->cb_arg = cb_arg;

	char ** const netdevs = cb_lld_get_netdevs();
	while (*netdevs) {
		cb_rawsock_paras_t llrawp = {
			.dev = *netdevs,
		};
		if (cb_rawsock_open(&llrawp, &cbld->fds[cbld->num_fds].fd,
				    NULL, NULL, srcmac) < 0) {
			return NULL;
		}
		cbld->fds[cbld->num_fds].netdev = *netdevs;
		cbld->num_fds++;
	}

	return cbld;
}

void combase_link_close(combase_link_data_t *cbld)
{
	int i;

	if (!cbld) { return; }
	for (i = 0; i < cbld->num_fds; i++) {
		cb_rawsock_close(cbld->fds[i].fd);
	}
	UB_SD_RELMEM(CBL_DATA_INSTMEM, cbld);
}

int cbl_query_linkstatus(combase_link_data_t *cbld, char *ifname)
{
	uint32_t link_state = 0;
	int i;

	if (!ifname) {
		return -1;
	}
	for (i = 0; i < cbld->num_fds; i++) {
		if (!strcmp(ifname, cbld->fds[i].netdev)) {
			break;
		}
	}
	if (i == cbld->num_fds) {
		UB_LOG(UBL_ERROR,"%s:invalid ifname : %s\n", __func__, ifname);
		return -1;
	}
	if (cb_lld_get_link_state(cbld->fds[i].fd, ifname, &link_state)) {
		return -1;
	}
	return (link_state == 1? 1: 2); // 1: up; 2: down
}

int cbl_query_response(combase_link_data_t *cbld, int tout_ms)
{
	cbl_cb_event_t nevent;
	int i;
	uint32_t link_state;

	for (i = 0; i < cbld->num_fds; i++) {
		if (cb_lld_get_link_state(cbld->fds[i].fd,
					     cbld->fds[i].netdev,
					     &link_state)) {
			return -1;
		}
		strncpy(nevent.ifname, cbld->fds[i].netdev, strlen(cbld->fds[i].netdev));
		if (link_state) {
			if (cb_lld_get_link_info(cbld->fds[i].fd, nevent.ifname,
						&nevent.speed, &nevent.duplex)) {
				UB_LOG(UBL_WARN,"%s:failed to get speed and duplex : %s\n",
				       __func__, nevent.ifname);
			}
			nevent.eventflags = CBL_EVENT_DEVUP;
			UB_LOG(UBL_DEBUG, "%s:DEVUP, speed=%d, duplex=%d\n",
			       __func__, nevent.speed, nevent.duplex);
		} else {
			nevent.eventflags=CBL_EVENT_DEVDOWN;
		}
		cb_get_mac_bydev(cbld->fds[i].fd, nevent.ifname, nevent.address);
		cbld->event_cb(cbld->cb_arg, &nevent);
	}

	return 0;
}
