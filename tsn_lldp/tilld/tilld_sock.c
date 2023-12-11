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
#include "../socket_util.h"
#include "../lldp_utils.h"
#include "../tlv_data.h"
#include "../interface_mgr.h"
#include <tsn_unibase/ub_llist.h>

/*Refer from avtp_ethernet.h*/
/**
 * @brief VLAN tag field used for VLAN Tagging.
 */
struct vlan_af {
	uint16_t pcv;
	uint16_t proto;
} __attribute__ ((packed));

#define RX_CHECK_INTERVAL_US	(10000)		//10ms 
typedef struct exh_data{
	struct vlan_af vlanaf;
	uint8_t data[ETH_DATA_LEN];
}exh_data_t;

typedef struct ether_frame{
	CB_ETHHDR_T hdr;
	union {
		exh_data_t ed;
		uint8_t data[ETH_DATA_LEN];
	}u;
} __attribute__ ((packed)) ether_frame_t;
ether_frame_t eframe;

const ub_macaddr_t mcast_neareast_bridge 			= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E};
const ub_macaddr_t mcast_neareast_nontpmr_bridge	= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
const ub_macaddr_t mcast_neareast_customer_bridge	= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};

CB_SOCKET_T lldpsock = NULL;	// One sock per whole App (using one DMA)
ub_macaddr_t srcmac;
int lldp_raw_sock_number = 0;
lldp_socket_t lldp_rawsocks[LLDP_CFG_PORT_INSTNUM];

static void cb_lld_rx_notify_cb(void* arg);
static int raw_sock_open(char* ndev, int rawsock_index);

static CB_SEM_T rx_available_signal;

int lldp_raw_socket_open(hw_interface* interface)
{
	int ret = 0;
	cb_rawsock_paras_t rawsock_param;
	memset(&rawsock_param, 0, sizeof(rawsock_param));
	rawsock_param.dev = interface->if_name;
	rawsock_param.proto = LLDP_PROTO;
	rawsock_param.vlan_proto = 0;
	rawsock_param.priority = 0;
	rawsock_param.rw_type = CB_RAWSOCK_RDWR;
	rawsock_param.sock_mode = CB_SOCK_MODE_NORMAL;

	UB_LOG(UBL_INFO, "%s [%s] \n", __func__, rawsock_param.dev);

	// Init only one time, the fd will corresponding to MAC Port
	if (lldpsock == NULL)
	{
		ret = cb_rawsock_open(&rawsock_param, &lldpsock, NULL, NULL, srcmac);
		if (ret == 0)
		{
			ret = cb_lld_set_rxnotify_cb(lldpsock, cb_lld_rx_notify_cb, NULL);
			if (ret < 0)
			{
				UB_LOG(UBL_INFO, "%s:%s: Can't register rxnotify cb %s\n", __func__, interface->if_name,  strerror(errno));
			}
			UB_LOG(UBL_INFO, "%s:%s: Registered CB ret %d\n", __func__, interface->if_name,  ret);
		}
		else
		{
				UB_LOG(UBL_INFO, "Added mcast_neareast_bridge address %s \n",interface->if_name);
		}
	}

	if (ret == 0)
	{
		memcpy(interface->lldpsock->bmac, srcmac, sizeof(srcmac));
		interface->lldpsock->fd = cb_lld_netdev_to_macport(interface->if_name);
		interface->lldpsock->addr.tcid = 0;
		interface->lldpsock->addr.macport = interface->lldpsock->fd;
				UB_LOG(UBL_INFO, "Added mcast_neareast_nontpmr_bridge address %s \n",interface->if_name);
	}

	return ret;
}

static int raw_sock_open(char* ndev, int rawsock_index)
{
	int ret = 0;
	cb_rawsock_paras_t rawsock_param;
	memset(&rawsock_param, 0, sizeof(rawsock_param));
	rawsock_param.dev = ndev;
	rawsock_param.proto = LLDP_PROTO;
	rawsock_param.vlan_proto = 0;
	rawsock_param.priority = 0;
	rawsock_param.rw_type = CB_RAWSOCK_RDWR;
	rawsock_param.sock_mode = CB_SOCK_MODE_NORMAL;

	UB_LOG(UBL_INFO, "%s [%s] \n", __func__, rawsock_param.dev);

	// Init only one time, the fd will corresponding to MAC Port
	if (lldpsock == NULL)
	{
		ret = cb_rawsock_open(&rawsock_param, &lldpsock, NULL, NULL, srcmac);
		if (ret == 0)
		{
			ret = cb_lld_set_rxnotify_cb(lldpsock, cb_lld_rx_notify_cb, NULL);
			if (ret < 0)
			{
				UB_LOG(UBL_INFO, "%s:%s: Cannot RX Callback %s\n", __func__, ndev,  strerror(errno));
			}
			else
			{
				UB_LOG(UBL_INFO, "%s:%s: Registered CB ret %d\n", __func__, ndev,  ret);
			}
		}
		else
		{
			UB_LOG(UBL_INFO, "%s:%s: Open raw socket NG %s\n", __func__, ndev,  strerror(errno));
		}
	}

	if (ret == 0)
	{
		memset(lldp_rawsocks[rawsock_index].name, 0, sizeof(lldp_rawsocks[rawsock_index].name));
		memcpy(lldp_rawsocks[rawsock_index].name, ndev, strlen(ndev));

		memcpy(lldp_rawsocks[rawsock_index].bmac, srcmac, sizeof(srcmac));
		lldp_rawsocks[rawsock_index].fd = cb_lld_netdev_to_macport(ndev);
		lldp_rawsocks[rawsock_index].addr.tcid = 0;
		lldp_rawsocks[rawsock_index].addr.macport = lldp_rawsocks[rawsock_index].fd;
		UB_LOG(UBL_INFO, "%s:%s : Open raw socket OK fd %d\n", __func__, ndev, lldp_rawsocks[rawsock_index].fd);
	}

	return ret;
}

/// @brief Once txNow is true. This function will be called from txStateMachine
/// @param  
/// @param buf 
/// @param len 
/// @param pcv 
/// @return 
int lldp_send_packet(hw_interface* interface, uint8_t* buf, size_t len, uint16_t pcv)
{
	int ret = -1;
	size_t packet_size = sizeof(CB_ETHHDR_T) + len;
	if (packet_size <= ETH_DATA_LEN)
	{
		eframe.hdr.h_proto = htons(LLDP_PROTO);
		memcpy(eframe.hdr.h_source, interface->cfg_port->src_mac_address, 6);
		memcpy(eframe.hdr.h_dest, interface->cfg_port->dest_mac_address, 6);
		memcpy(eframe.u.data, buf, len);

		ret = CB_SOCK_SENDTO(lldpsock, 
							&eframe, 
							packet_size, 
							0,
							&interface->lldpsock->addr,
							sizeof(interface->lldpsock->addr));

		if (ret == -1)
		{
			UB_LOG(UBL_ERROR,"%s:%s %s\n", interface->if_name, __func__, strerror(errno));
		}
		else
		{
			UB_LOG(UBL_DEBUG,"%s:%s %d bytes has been sent.\n", interface->if_name, __func__, ret);
		}
	}
	else
	{
		UB_LOG(UBL_ERROR, "%s:%s Packet size to big %d\n", __func__, interface->if_name,  (int)packet_size);
	}
	return ret;
}

hw_interface *find_receiving_port(hw_interface_list* hw_if_list, int rx_mac_port, ub_macaddr_t dst_mac)
{
	struct ub_list_node* tmp_node;
	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface *interface = (hw_interface *) tmp_node;
		if (interface->lldpsock->fd == rx_mac_port && memcmp(interface->cfg_port->dest_mac_address, dst_mac, 6) == 0)
		{
			return interface;
		}
	}

	return NULL;
}

int try_recv_packet(hw_interface_list* hw_if_list, int timeout_usec)
{
	int res = -1;
	struct timespec ts;
	int64_t ts64_ns;
	int64_t tout_ts64_ns = ((int64_t)timeout_usec) * UB_USEC_NS;
	ts64_ns = ub_rt_gettime64();
	UB_NSEC2TS(ts64_ns + tout_ts64_ns, ts);
	
	UB_LOG(UBL_DEBUG, "%s with tout %d \n", __func__, timeout_usec);
	res = CB_SEM_TIMEDWAIT(&rx_available_signal, &ts);
	if (res == 0)
	{
		while(1)
		{
			int macport = 0;
			uint8_t buf[MAX_LLDP_SIZE] = {0};
			int recv_len = cb_lld_recv(lldpsock, 
										buf, 
										MAX_LLDP_SIZE, 
										&macport);

			if(recv_len > 0 && recv_len != 0xFFFF)
			{
				int read_idx = 0;
				ub_macaddr_t dst_mac = {0, 0, 0, 0, 0, 0};
				get_dest_mac_addr((const char*)buf, &read_idx, dst_mac);

				hw_interface *interface = find_receiving_port(hw_if_list, macport, dst_mac);
				if (interface && (interface->cfg_port->admin_status == rx_only || interface->cfg_port->admin_status == tx_and_rx) )
				{
					UB_LOG(UBL_DEBUG, "%s Received %d bytes from %s:macport %d \n", __func__, interface->lldpsock->recv_len, interface->if_name, macport);
					if ( interface->agent_info.rxInfoAge == false)
					{
						interface->lldpsock->recv_len = recv_len;
						memcpy(interface->lldpsock->recv_buf, buf, recv_len);
						interface->agent_info.rxFrame = true;
						PUSH_RX_EVENT(FRAME_RECEIVED, interface);
					}
				}
			}
			/* indicate that the call receive data but for other apps */
			else if (recv_len == 0xFFFF) {
				continue;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		if (cb_lld_sem_wait_status(&rx_available_signal) == TILLD_TIMEDOUT) {
			res = 0; // Indicate nothing recv. Continue
		}
		else
		{
			UB_LOG(UBL_INFO, "%s Semtime wait failure ret = %d\n", __func__, res);
			res = -1; // Indicate nothing Error
		}
	}

	return res;
}

/// @brief 
/// @param arg 
static void cb_lld_rx_notify_cb(void* arg)
{
	CB_SEM_POST(&rx_available_signal);
}

/// @brief 
/// @param lldpsock 
/// @return 
int lldp_raw_socket_close(hw_interface* interface)
{
	return 0; // cb_rawsock_close(interface->lldpsock->fd);
}

int init_socket_utils(netdevname_t *netdevs, int ndev_size)
{
	int i=0;
	if (CB_SEM_INIT(&rx_available_signal, 0, 0) < 0) {
		UB_LOG(UBL_ERROR,"%s:to init SEM rx_available_signal\n", __func__);
	}

	lldp_raw_sock_number = (ndev_size > LLDP_CFG_PORT_INSTNUM) ? LLDP_CFG_PORT_INSTNUM : ndev_size;
	UB_LOG(UBL_INFO, "%s:lldp_raw_sock_number %d  ndev_size %d\n", __func__, lldp_raw_sock_number, ndev_size);

	for (i=0; i<lldp_raw_sock_number; i++)
	{
		UB_LOG(UBL_INFO, "%s:Openning %s \n", __func__, netdevs[i]);
		if (raw_sock_open(netdevs[i], i) != 0)
		{
			UB_LOG(UBL_ERROR, "%s:%s Cannot open raw socket \n", __func__, netdevs[i]);
			return -1;
		}
	}

	return 0;
}

/// @brief Since one port can have multiple cfg port, so each hw_interface just keep the reference to opened socket fd.
void assign_raw_socket_to_port(struct _hw_interface* interface)
{
	int i=0;
	
	for (i=0; i<lldp_raw_sock_number; i++)
	{
		if (strcmp(interface->cfg_port->name, lldp_rawsocks[i].name) == 0)
		{
			interface->lldpsock = &lldp_rawsocks[i];
			char dst_mac[18] = {0};
			lldp_bmac2smac(interface->cfg_port->dest_mac_address, dst_mac, '-');
			UB_LOG(UBL_INFO, "%s:%s assigned to fd %d\n", interface->cfg_port->name, dst_mac, lldp_rawsocks[i].fd);
			break;
		}
	}

	if (i == lldp_raw_sock_number)
	{
		UB_LOG(UBL_WARN, "%s: No such port[%s]\n", __func__, interface->cfg_port->name);
	}
}

void deinit_socket_utils(void)
{
	if (rx_available_signal) {
		CB_SEM_DESTROY(&rx_available_signal);
		rx_available_signal = NULL;
	}

	cb_rawsock_close(lldpsock);
}
