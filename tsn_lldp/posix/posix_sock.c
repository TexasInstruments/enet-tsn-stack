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
#include "../interface_mgr.h"
#include "../tlv_data.h"
#include "../lldp_utils.h"
#include <tsn_unibase/ub_llist.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

/*Refer from avtp_ethernet.h*/
/**
 * @brief VLAN tag field used for VLAN Tagging.
 */
struct vlan_af {
	uint16_t pcv;
	uint16_t proto;
} __attribute__ ((packed));


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

static int wait_rx_for_timeout(struct ub_list* hw_if_list, int timeout_usec);

int lldp_raw_socket_open(hw_interface* interface)
{
	int ret;
	cb_rawsock_paras_t rawsock_param;
	cb_rawsock_ovip_para_t ovipp;
	memset(&rawsock_param, 0, sizeof(rawsock_param));
	rawsock_param.dev = interface->if_name;
	rawsock_param.proto = LLDP_PROTO;
	rawsock_param.vlan_proto = 0;
	rawsock_param.priority = 0;
	rawsock_param.rw_type = CB_RAWSOCK_RDWR;
	rawsock_param.sock_mode = 0;

	if(strstr(interface->if_name, CB_VIRTUAL_ETHDEV_PREFIX)==interface->if_name)
	{
		char* ovip_info[2];
		split_str_into_arr(interface->if_name, "_", ovip_info, 2);
		uint32_t ovip_port = (uint32_t)strtol(ovip_info[1], NULL, 0);
		if (ovip_port == 0)
		{
			UB_TLOG(UBL_WARN, "Invalid test rule of lldp. Assign default port 7000 \n");
			ovip_port = 7000;
		}
		memset(&ovipp, 0, sizeof(ovipp));
		ovipp.laddr=127<<24|0<<16|0<<8|1;
		ovipp.daddr=127<<24|0<<16|0<<8|1;
		ovipp.lport=ovip_port;
		ovipp.dport=(ovip_port%2)?ovip_port-1:ovip_port+1;
		rawsock_param.ovipp=&ovipp;
		rawsock_param.sock_mode=CB_SOCK_MODE_OVIP;
		UB_TLOG(UBL_INFO, "%s [%s] as virtual eth interface [%s]-lport[%u]-dport[%u]\n", __func__,interface->if_name, rawsock_param.dev, ovipp.lport, ovipp.dport);
	}

	UB_TLOG(UBL_INFO, "%s [%s] \n", __func__, rawsock_param.dev);
	ret = cb_rawsock_open(&rawsock_param, &interface->lldpsock.fd, &interface->lldpsock.addr, (int*)&interface->lldpsock.mtu, interface->lldpsock.bmac);
	if (ret == 0)
	{
		if (cb_reg_multicast_address(interface->lldpsock.fd, interface->if_name, mcast_neareast_bridge, 0) == 0) // 0 mean add
		{
			UB_TLOG(UBL_INFO, "Added mcast_neareast_bridge address %s \n",interface->if_name);
		}

		if (cb_reg_multicast_address(interface->lldpsock.fd, interface->if_name, mcast_neareast_nontpmr_bridge, 0) == 0) // 0 mean add
		{
			UB_TLOG(UBL_INFO, "Added mcast_neareast_nontpmr_bridge address %s \n",interface->if_name);
		}
		
		if (cb_reg_multicast_address(interface->lldpsock.fd, interface->if_name, mcast_neareast_customer_bridge, 0) == 0) // 0 mean add
		{
			UB_TLOG(UBL_INFO, "Added mcast_neareast_customer_bridge address %s \n",interface->if_name);
		}
		
		UB_TLOG(UBL_INFO, "%s:%s : Open raw socket OK fd\n", __func__, interface->if_name);
	}
	else
	{
		UB_TLOG(UBL_INFO, "%s:%s: Open raw socket NG %s\n", __func__, interface->if_name,  strerror(errno));
	}

	return ret;
}

/// @brief This is only function posix platform dependent
/// @param hw_if_list 
/// @param timeout_usec 
/// @return 
static int wait_rx_for_timeout(struct ub_list* hw_if_list, int timeout_usec)
{
	int max_fd = -1;
	fd_set rfds;
	int res;
	struct ub_list_node *tmp_node;
	
	struct timeval tv={0,0};
	tv.tv_sec=timeout_usec/1000000;
	tv.tv_usec=timeout_usec%1000000;
	FD_ZERO(&rfds);

	UB_LOG(UBL_DEBUG, "%s with tout %d \n", __func__, timeout_usec);

	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface *interface = (hw_interface *) tmp_node;
		if (interface->cfg_port->admin_status == rx_only || interface->cfg_port->admin_status == tx_and_rx)
		{
			interface->lldpsock.is_recv_available = false;// reset this flag
			FD_SET(interface->lldpsock.fd, &rfds);
			if (interface->lldpsock.fd > max_fd) max_fd = interface->lldpsock.fd;
		}
	}

	res = select(max_fd+1, &rfds, NULL, NULL, &tv);
	if(!res){return 0;}
	if(res < 0){return -1;}

	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface *interface = (hw_interface *) tmp_node;
		if(FD_ISSET(interface->lldpsock.fd, &rfds))
		{
			interface->lldpsock.is_recv_available = true;
		}
	}

	return res;
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
		memcpy(eframe.hdr.h_source, interface->lldpsock.bmac, 6);
		memcpy(eframe.hdr.h_dest, mcast_neareast_bridge, 6);
		// TODO: Check vlan
		memcpy(eframe.u.data, buf, len);

		ret = CB_SOCK_SENDTO(interface->lldpsock.fd, &eframe, packet_size, 0,
				 (struct sockaddr*)&interface->lldpsock.addr,
				 sizeof(interface->lldpsock.addr));
		if (ret == -1)
		{
			UB_TLOG(UBL_ERROR,"%s:%s %s\n", interface->if_name, __func__, strerror(errno));
		}
		else
		{
			UB_TLOG(UBL_INFO,"%s:%s %d bytes has been sent.\n", interface->if_name, __func__, ret);
		}
	}
	else
	{
		UB_TLOG(UBL_ERROR, "%s:%s Packet size to big %d\n", __func__, interface->if_name,  (int)packet_size);
	}
	return ret;
}

/// @brief Check RX available for timeout_usec and read rx
/// @param hw_if_list 
/// @param timeout_usec 
/// @return 
int try_recv_packet(hw_interface_list* hw_if_list, int timeout_usec)
{
	int res = -1;
	res = wait_rx_for_timeout(hw_if_list, timeout_usec);

	if (res > 0)
	{
		struct ub_list_node* tmp_node;
		for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
		{
			hw_interface *interface = (hw_interface *) tmp_node;
			if (interface->cfg_port->admin_status == rx_only || interface->cfg_port->admin_status == tx_and_rx)
			{
				if(interface->lldpsock.is_recv_available == true)
				{
					// socklen_t address_length = (socklen_t)sizeof(struct sockaddr_un);
					socklen_t address_length = (socklen_t)sizeof(struct sockaddr_ll);
					struct sockaddr_ll from;
					memset(&from, 0, address_length);

					interface->lldpsock.recv_len = CB_SOCK_RECVFROM(interface->lldpsock.fd, 
																	interface->lldpsock.recv_buf, 
																	MAX_LLDP_SIZE, 
																	0, 
																	(struct sockaddr *)&from,
																	&address_length);

					if(interface->lldpsock.recv_len > 0)
					{
						UB_TLOG(UBL_INFO, "%s Received %d bytes from %s \n", __func__, interface->lldpsock.recv_len, interface->if_name);
						if ( interface->agent_info.rxInfoAge == false)
						{
							interface->agent_info.rxFrame = true;
							PUSH_RX_EVENT(FRAME_RECEIVED, interface);
						}
					}
				}
			}
		}
	}
	return res;
}

/// @brief Do nothing. Below functions are applied for TILLD without select() API
/// @param  
void init_socket_utils(void)
{
}
void deinit_socket_utils(void)
{
}

/// @brief 
/// @param lldpsock 
/// @return 
int lldp_raw_socket_close(hw_interface* interface)
{
	return cb_rawsock_close(interface->lldpsock.fd);
}