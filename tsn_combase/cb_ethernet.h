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
/**
 * @ingroup  TSN_COMBASE_MODULE
 * @defgroup network Network functions binding
 * @{
 * @file cb_ethernet.h
 *
 * @brief Bindings to POSIX network functions
 */

#ifndef CB_ETHERNET_H_
#define CB_ETHERNET_H_

#ifdef CB_ETHERNET_NON_POSIX_H
/* non-posix platforms need to support necessary POSIX compatible
 * functions and types which are defined as CB_* macros below.
 * And provide them in a header file defined as CB_SOCKET_NON_POSIX_H */
#include CB_ETHERNET_NON_POSIX_H
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>

#define CB_MAX_NETDEVNAME IFNAMSIZ
#define CB_SOCKET_T int
#define CB_SOCKET_VALID(x) ((x)>=0)
#define CB_SOCKET_INVALID_VALUE -1
#define CB_ETHHDR_T struct ethhdr
#define CB_SOCKLEN_T socklen_t
#define CB_SOCKADDR_T struct sockaddr
#define CB_SOCKADDR_LL_T struct sockaddr_ll
#define CB_SOCKADDR_IN_T struct sockaddr_in
#define CB_SOCKADDR_IN6_T struct sockaddr_in6
#define CB_SOCKADDR_STORAGE_T struct sockaddr_storage
#define CB_IN_ADDR_T struct in_addr
#define CB_IN6_ADDR_T struct in6_addr
#define CB_IFREQ_T struct ifreq

#define CB_OPEN open
#define CB_CLOSE close
#define CB_WRITE write
#define CB_SOCKET socket
#define CB_IF_NAMETOINDEX if_nametoindex
#define CB_SOCK_BIND bind
#define CB_SOCK_IOCTL ioctl
#define CB_SOCK_CLOSE close
#define CB_SETSOCKOPT setsockopt
#define CB_SOCK_SENDTO sendto
#define CB_SELECT select
#define CB_SOCK_RECVFROM recvfrom
#define CB_SOCK_RECVMSG recvmsg
#define CB_SOCK_WRITE write
#define CB_SOCK_CONNECT connect

#define CB_FCNTL fcntl
#define CB_FD_SET_T fd_set
#define CB_FDSET FD_SET
#define CB_FDCLR FD_CLR
#define CB_FDISSET FD_ISSET
#define CB_FDZERO FD_ZERO

#endif // CB_ETHERNET_NON_POSIX_H

/* virtual eth ports and ptp devices are used in the ovip mode.
 * common suffix should be added after these prefixes */
/**
 *@brief prefix of virtual network device, which supports
 * raw ethernet packet over udp
 */
#define CB_VIRTUAL_ETHDEV_PREFIX "cbeth"

/**
 *@brief prefix of virtual ptp device.
 * the suffix must be common with the virtual network device.
 */
#define CB_VIRTUAL_PTPDEV_PREFIX "cbptp"

/**
 *@brief virtual MAC address of the virtual network device.
 * the lower 2 bytes are calculated from the suffix
 */
#define CB_VIRTUAL_ETHDEV_MACU32 {0x02,0x01,0x45,0x10}

/* the following definitions have some variations for platforms */
#ifndef H_SOURCE
#define H_SOURCE h_source
#endif

#ifndef H_DEST
#define H_DEST h_dest
#endif

#ifndef H_PROTO
#define H_PROTO h_proto
#endif

/************************************************************
 * definitions to handle PTP messages
 ************************************************************/
/**
 *@brief forms ptp header msgtype.
 */
#define PTP_HEAD_MSGTYPE(x) ((*(uint8_t *) (&((uint8_t *)(x))[0])) & 0x0F)

/**
 * @brief macro which is used to form ptp header sequence id.
 */
#define PTP_HEAD_SEQID(x) ((((uint8_t *)(x))[30]<<8u)|(((uint8_t *)(x))[31]))

/**
 * @brief macro used to form ptp header domain number.
 */
#define PTP_HEAD_DOMAIN_NUMBER(x) (*(uint8_t *)(&((uint8_t *)(x))[4]))

/** @brief maximum character number of ptp device name */
#define MAX_PTPDEV_NAME 32
/**
 * @brief ptpdevice name.
 */
typedef char ptpdevname_t[MAX_PTPDEV_NAME];


/************************************************************
 * definitions to handle raw socket
 ************************************************************/
/**
 * @brief this enumeration defines permission for raw socket.
 * @verbatim for example @endverbatim
 * CB_RAWSOCK_RDWR for read and write.
 * CB_RAWSOCK_RDONLY for read only.
 * CB_RAWSOCK_WRONLY for write only.
 */
typedef enum {
	CB_RAWSOCK_RDWR = 0,
	CB_RAWSOCK_RDONLY,
	CB_RAWSOCK_WRONLY,
} cb_rawsock_rw_t;

/**
 * @brief parameters to open the over IP mode raw socket, the values are in host order
 */
typedef struct cb_rawsock_ovip_para {
	uint32_t laddr; //!< local IP address
	uint16_t lport; //!< local IP port
	uint32_t daddr; //!< destination IP address
	uint16_t dport; //!< destination IP port
} cb_rawsock_ovip_para_t;

typedef enum {
	CB_SOCK_MODE_OVIP = -1,
	CB_SOCK_MODE_NORMAL,
} cb_sock_mode_t;

/**
 * @brief raw socket parameters.to open or create raw socket this structure must be filled.
 */
typedef struct cb_rawsock_paras{
	const char *dev; //!< ethernet device name
	uint16_t proto; //!< protocol value like ETH_P_1588
	uint16_t vlan_proto; //!< protocol value in VLAN tag, not used in non-tagged
	int priority; //!< PCP priority value in VLAN tag
	cb_rawsock_rw_t rw_type; //!< one of RAWSOCK_WRONLY, RAWSOCK_RDONLY, RAWSOCK_RDWR
	cb_sock_mode_t sock_mode; //!< -1:raw socket over udp, 0:normal,
	cb_rawsock_ovip_para_t *ovipp; //!< over-udp mode parameter
	uint16_t vlanid; //!< vlan identifier
	uint32_t sndbuf; //!< socket send buffer size, set to 0 to use default value
	uint32_t rcvbuf; //!< socket receive buffer size, set to 0 to use default value
} cb_rawsock_paras_t;

/**
 * @brief network device name.
 */
typedef char netdevname_t[CB_MAX_NETDEVNAME];

/************************************************************
 * functions
 ************************************************************/
/**
 * @brief get mac address from device name like 'eth0'
 * @param sfd	if sfd!=-1, pre-opened socket is used to get the mac.
 * if sfd==-1, a newly opened udp socket is used to get the mac Address.
 * @param dev	ethernet device name like 'eth0'
 * @param bmac refernce to buffer which is used to store mac address
 * of ethernet device.
 * @return 0 on success, -1 on error
 */
int cb_get_mac_bydev(CB_SOCKET_T sfd, const char *dev, ub_macaddr_t bmac);

/**
 * @brief get ip address from device name like 'eth0'
 * @param sfd	if sfd!=-1, pre-opened socket is used to get the mac.
 * if sfd==-1, a newly opened udp socket is used to get the MAC Address.
 * @param dev	ethenert device name like 'eth0'
 * @param inp reference to 'CB_IN_ADDR_T' where IP address is saved
 * @return 0 on success, -1 on error
 */
int cb_get_ip_bydev(CB_SOCKET_T sfd, const char *dev, CB_IN_ADDR_T *inp);

/**
 * @brief get broadcast ip address from device name like 'eth0'
 * @param sfd	if sfd!=-1, pre-opened socket is used to get the mac.
 * if sfd==-1, a newly opened udp socket is used.
 * @param dev	ethenert device name like 'eth0'
 * @param inp reference to 'CB_IN_ADDR_T' where IP address is saved
 * @return 0 on success, -1 on error
 */
int cb_get_brdip_bydev(CB_SOCKET_T sfd, const char *dev, CB_IN_ADDR_T *inp);

/**
 * @brief generic raw ethernet open
 * @param llrawp cb_rawsock_paras_t -> raw socket open parameters,
 * this parameter is passed as refernce to cb_rawsock_paras_t.
 * @param fd	return a descriptor of opened socket
 * @param addr	return sockaddr information which is used to open the socket
 * @param mtusize	MTU size including ETH header size.
 *	if *mtusize>default size, try to resize MTU size.
 * @param bmac	the mac address of 'dev' is returned in 'bmac'
 * @return 0 on success, -1 on error
 * @note for general this function support the both of 'avtp raw' and
 *	  'general raw', for 'nos'(no OS or primitive OS), this is for
 *	  'avtp raw' and nos_rawe_socket_open is for 'general raw'
 * @note before calling to this function, llrawp must be filled.
 * @see @c cb_rawsock_paras_t
 */
int cb_rawsock_open(cb_rawsock_paras_t *llrawp, CB_SOCKET_T *fd, CB_SOCKADDR_LL_T *addr,
		    int *mtusize, ub_macaddr_t bmac);

/**
 * @brief close the socket opened by cb_rawsock_open
 * @param fd	descriptor of the opened socket
 * @return 0 on success, -1 on error.
 */
int cb_rawsock_close(CB_SOCKET_T fd);

/**
 * @brief set socket priority
 * @param fd	descriptor of the opened socket
 * @param priority	priority number
 * @return 0 on success, -1 on error.
 */
int cb_sock_set_priority(CB_SOCKET_T fd, int priority);

/**
 * @brief expand mtusize
 * @param fd	descriptor of the opened socket
 * @param dev	ethernet device name like eth0
 * @param mtusize	new mtusize, return a new mtu size in *mtusize
 * @return 0 on success, -1 on error.
 */
int cb_expand_mtusize(CB_SOCKET_T fd, const char *dev, int *mtusize);

/**
 * @brief set the promiscuous mode on the socket
 * @param sfd	descriptor of the socket
 * @param dev	ethernet device name like eth0
 * @param enable	true:enable, false:disable
 * @return 0 on success, -1 on error.
 */
int cb_set_promiscuous_mode(CB_SOCKET_T sfd, const char *dev, bool enable);

/**
 * @brief register/deregister multicast address to receive
 * @param fd	descriptor of the socket
 * @param dev	ethernet device name like eth0
 * @param mcastmac	multicast address
 * @param del	0:register 1:deregister
 * @return 0 on success, -1 on error.
 */
int cb_reg_multicast_address(CB_SOCKET_T fd, const char *dev,
			     const unsigned char *mcastmac, int del);

/**
 * @brief get all network devices name availble in systems.
 * @param maxdevnum number of devices presents in systems.
 * @param netdevs reference to netdevname_t which stores net devices name.
 * @return index number of network devices presents on system.
 */
int cb_get_all_netdevs(int maxdevnum, netdevname_t *netdevs);

/**
 * @brief get ethtool link state from device name like 'eth0'
 * @param cfd    if cfd!=-1, pre-opened socket is used to get the ethtool info.
 * if cfd==-1, a newly opened udp socket is used.
 * @param dev        ethernet device name like 'eth0'
 * @param linkstate  to store link state value (0:down, 1:up)
 * @return 0 on success, -1 on error
 * @note Linux platform supports this function.  Other platform must suport
 * in the outside of this layer.
 * this function will be deprecated.
 * read the status in uniconf DB which is managed by combase_link.h and low layer functinos.
 */
int cb_get_ethtool_linkstate(CB_SOCKET_T cfd, const char *dev, uint32_t *linkstate);

/**
 * @brief get ethtool info(speed and duplex) from device name like 'eth0'
 * @param cfd	if cfd!=-1, pre-opened socket is used to get the ethtool info.
 * if cfd==-1, a newly opened udp socket is used.
 * @param dev	ethenert device name like 'eth0'
 * @param speed	to store speed value(0:unknow, 10:10Mbps, 100:100Mbps, 1000:1Gbps)
 * @param duplex to store duplex value(0:unknow, 1:Full, 2:Half)
 * @return 0 on success, -1 on error
 * @note Linux platform supports this function.  Other platform must suport
 * in the outside of this layer.
 * this function will be deprecated.
 * read the status in uniconf DB which is managed by combase_link.h and low layer functinos.
 */
int cb_get_ethtool_info(CB_SOCKET_T cfd, const char *dev, uint32_t *speed, uint32_t *duplex);

/**
 * @brief find network device name from ptp device name
 * @return 0 on success, -1 on error
 * @param ptpdev	ptpdevice name(either format of '/dev/ptp0' or 'ptp0')
 * @param netdev	the result of network device name, must have enough space
 * @note Linux platform supports this function.  Other platform must suport
 * in the outside of this layer.
 */
int cb_get_netdev_from_ptpdev(char *ptpdev, char *netdev);

/**
 * @brief find ptp device name from network device name
 * @return 0 on success, -1 on error
 * @param netdev	network device name (like 'eth0')
 * @param ptpdev	the result of ptpdevice name(like 'ptp0', no preceding '/dev/'), \n
 * must have enough space.
 * @note Linux platform supports this function.  Other platform must suport
 * in the outside of this layer.
 */
int cb_get_ptpdev_from_netdev(char *netdev, char *ptpdev);

#endif
/** @}*/
