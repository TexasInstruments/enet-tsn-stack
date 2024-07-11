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
 * @defgroup lld_ethernet TI ENET LLD ethernet specific
 * @{
 * @file cb_lld_ethernet.h
 *
 * @brief TI ENET LLD specific network layer part
 */
#ifndef CB_LLD_ETHERNET_H
#define CB_LLD_ETHERNET_H

#include <sys/types.h>
#include "lldenet.h"
#include "lldtsync.h"
#ifdef __cplusplus
extern "C" {
#endif

// To prevent dupplicated declaration of below APIs
#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
#ifndef htons
#define htons __htons
#endif
#ifndef ntohs
#define ntohs __ntohs
#endif
#ifndef htonl
#define htonl __htonl
#endif
#ifndef ntohl
#define ntohl __ntohl
#endif

#define MAX_NUMBER_ENET_DEVS LLDENET_MAX_PORTS

#define CB_SOCKET_VALID(x) ((x)!=NULL)
#define CB_SOCKET_INVALID_VALUE NULL
#define CB_SOCKET_T lld_socket_t*
#define CB_ETHHDR_T struct lld_ethhdr
#define CB_SOCKADDR_LL_T struct lld_sockaddr
#define CB_SOCKADDR_T struct lld_sockaddr
#define CB_SOCK_SENDTO cb_lld_sendto
#define CB_SOCK_CLOSE cb_rawsock_close

#define CB_OPEN open
#define CB_CLOSE close
#define CB_WRITE write

/* does not support */
#define CB_IN_ADDR_T void*

#define ETH_ALEN		6		/* Octets in one ethernet addr	 */
#define ETH_HLEN		14		/* Total octets in header.	 */
#define ETH_DATA_LEN	1500	/* Max. octets in payload	 */

#define ETH_P_8021Q		0x8100  /* 802.1Q VLAN Extended Header  */
#define ETH_P_1588		0x88F7	/* IEEE 1588 Timesync */
#define ETH_P_LLDP		0x88CC	/* 802.1AB LLDP */
#define ETH_P_TSN		0x22F0	/* TSN (IEEE 1722) packet	*/
#define ETH_P_NETLINK		0x22F1  /* A virtual prototol for netlink rawsockt on TI platforms */

#define CB_MAX_NETDEVNAME 16

#define MKPIPE(x)		(-1)

#define CB_LLD_MAX_ETH_FRAME_SIZE (1522U)

/**
 * @brief Ethernet header structure.
 */
struct lld_ethhdr {
	/**
	 * Destination Ethernet address.
	 */
	uint8_t h_dest[ETH_ALEN];
	/**
	 * Source Ethernet address.
	 */
	uint8_t h_source[ETH_ALEN];
	/**
	 * Ethernet type in the ethernet frame
	 */
	uint16_t h_proto;
} __attribute__((packed));

/**
 * @brief Structure representing the lld_sockaddr instance.
 */
struct lld_sockaddr {
	/**
	 * Source MAC address of the socket.
	 */
	unsigned char sll_addr[6];
	/**
	 * MAC port number.
	 */
	int macport;
	/**
	 * Traffic class ID. Valid values start from 0.
	 * Set to -1 to use the default assigned by the implementation.
	 */
	int tcid;
	/**
	 * timestamp of rx packet which was captured by host port.
	 */
	uint64_t rxts;
};

/**
 * @brief Structure representing the lld_socket instance.
 */
typedef struct lld_socket lld_socket_t;

/**
 * @brief Structure representing the lld_ethdev instance.
 */
typedef struct {
	/**
	 * Network device name.
	 */
	char *netdev;
	/**
	 * MAC port number.
	 */
	uint8_t macport;
	/**
	 * Source MAC address.
	 */
	uint8_t srcmac[ETH_ALEN];
} lld_ethdev_t;

/**
 * @brief Structure representing the cb_socket_lldcfg_update instance.
 */
typedef struct {
	/* below are keys to match the expected socket */
	/**
	 * Device name to match the expected socket.
	 */
	const char *dev;
	/**
	 * Protocol or ethernet type in the ethernet frame header
	 * to match the expected socket.
	 */
	uint16_t proto;
	/**
	 * VLAN ID to match the expected socket.
	 */
	uint16_t vlanid;
	/* below are params can be updated to LLDEnetCfg_t */
	/**
	 * Number of buffers allocated for transmit packets
	 * Positive value will be updated to LLDEnetCfg_t.
	 */
	uint32_t nTxPkts;
	/**
	 * Number of buffers allocated for receive packets
	 * Positive value will be updated to LLDEnetCfg_t.
	 */
	uint32_t nRxPkts[MAX_NUM_RX_DMA_CH_PER_INSTANCE];
	/**
	 * Transmit and receive maximum packet size.
	 * Positive value will be updated to LLDEnetCfg_t.
	 */
	uint32_t pktSize;
	/**
	 * DMA transmit channel ID.
	 * 0 or positive value will be updated to LLDEnetCfg_t
	 */
	int dmaTxChId;
	/**
	 * DMA receive channel ID.
	 * 0 or positive value will be updated to LLDEnetCfg_t
	 */
	int dmaRxChId[MAX_NUM_RX_DMA_CH_PER_INSTANCE];
	/**
	 * >0: won't use DMA RX; 0: will use DMA RX
	 * 0 or positive value will be updated to LLDEnetCfg_t
	 */
	int unusedDmaRx;
	/**
	 * >0: won't use DMA TX; 0: will use DMA TX
	 * 0 or positive value will be updated to LLDEnetCfg_t
	 */
	int unusedDmaTx;
	/**
	 * Only Sitara AM273x support this param.
	 * >0: the dmaRxChId is shared between apps; 0: not shared
	 * 0 or positive value will be updated to LLDEnetCfg_t
	 */
	int dmaRxShared;
	/**
	 * Only Sitara AM273x support this param.
	 * >0: the socket owns this RX DMA channel; 0: not own
	 * 0 or positive value will be updated to LLDEnetCfg_t
	 */
	int dmaRxOwner;
	/**
	 * Only Sitara AM273x support this param.
	 * Default RX data callback when there is no matching RX filter.
	 */
	void (*rxDefaultDataCb)(void *data, int size, int port, void *cbArg);
	/**
	 * Only Sitara AM273x support this param.
	 * Default RX data callback arg
	 */
	void *rxDefaultCbArg;
	/**
	 * Number of Rx DMA channels, only ICSSG peripheral has more than 1 Rx DMA channels.
	 */
	uint32_t numRxChannels;
} cb_socket_lldcfg_update_t;

/* Number of elements in a statistics block */
#define CB_TILLD_STATS_BLOCK_ELEM_NUM             (128U)
typedef struct cb_tilld_port_stats {
	uint64_t val[CB_TILLD_STATS_BLOCK_ELEM_NUM];
} cb_tilld_port_stats_t;

/**
 * @typedef cb_socket_lldcfg_update_cb_t
 * @brief Callback function type for cb_socket_lldcfg_update.
 * @param update_cfg Pointer to the cb_socket_lldcfg_update instance.
 * @return Integer value indicating the result of the callback. 0: OK, <0: error
 */
typedef int (*cb_socket_lldcfg_update_cb_t)(cb_socket_lldcfg_update_t *update_cfg);

/**
 * @brief Initialize a LLD device table.
 *
 * This function should only be called once, before calling any net functions from
 * the combase. It is recommended to call once in the main() application, not in any
 * TSN module source.
 * If the ethdevs[i].srcmac is all zero, the srcmac will be alloced dynamically
 * by the the LLD layer.
 * Please note that ethdevs[i].netdev must point to a global memory address,
 * not a stack memory address.
 *
 * @param ethdevs an arrays of ethernet devices
 * @param ndevs number of devices in the array
 * @param enet_type LLD ethernet type
 * @param instance_id LLD ethernet instance ID
 * @return 0: OK, <0: error, detailed error will be printed out.
 */
int cb_lld_init_devs_table(lld_ethdev_t *ethdevs, uint32_t ndevs,
						   uint32_t enet_type, uint32_t instance_id);

/**
 * @brief This function sets the callback for updating LLD config parameters.
 * It enables the user to update config parameters for the Enet LLD layer that
 * are not supported by the CB socket.
 * It is important to note that this function should only be called once.
 * @param lldcfg_update_cb update callback
 * @return 0: OK, <0: error, detailed error will be printed out.
 */
int cb_socket_set_lldcfg_update_cb(cb_socket_lldcfg_update_cb_t lldcfg_update_cb);

/**
 * @brief Convert net device name to mac port
 * @param netdev net device name e.g. eth0
 * @return >=0: MAC port, <0: error, detailed error will be printed out.
 */
int cb_lld_netdev_to_macport(const char *netdev);

/**
 * @brief Send a TX ethernet L2 packet.
 * @param sfd socket fd
 * @param sdata ethernet L2 packet
 * @param psize packet size
 * @param flags it is unused now, keep it to make it compatible with POSIX sendto
 * @param addr destination address info
 * @param addrsize address size
 * @return 0: OK, <0: error, detailed error will be printed out.
 */
int cb_lld_sendto(CB_SOCKET_T sfd, void *sdata, int psize, int flags,
				 const CB_SOCKADDR_LL_T *addr, int addrsize);

/**
 * @brief Receive a RX ethernet L2 packet.
 * This should be called after a rxnotify_cb is invoked.
 * @param sfd socket fd
 * @param buf packet buffer
 * @param size buffer size
 * @param addr index of received port and packets info
 * @param addrsize size of the addr structure.
 * @return <0: error; 0: No data available; 0xFFFF: Unmatched data filter; >0: data len
 */
int cb_lld_recv(CB_SOCKET_T sfd, void *buf, int size,
		CB_SOCKADDR_LL_T *addr, int addrsize);

typedef void (*cb_lld_zerocopy_recv_cb_t)(void *buf, int size,
					CB_SOCKADDR_LL_T *addr, void *cbarg);
/**
 * @brief Receive a RX ethernet L2 packet n the zero-copy way.
 * This should be called after a rxnotify_cb is invoked.
 * @param sfd socket fd
 * @param cblld_recv_cb the receive callback
 * @param cbarg the callback argument
 * @return <0: error; 0: No data available; 0xFFFF: Unmatched data filter; 1: received data
 */
int cb_lld_recv_zerocopy(CB_SOCKET_T sfd, cb_lld_zerocopy_recv_cb_t cblld_recv_cb,
						void *cbarg);

/**
 * @brief Set the TX notify callback that is invoked when an TX packet
 * is sent successfully.
 * @param sfd socket fd
 * @param txnotify_cb callback
 * @param arg callback argument
 * @return 0: OK, <0: error, detailed error will be printed out
 */
int cb_lld_set_txnotify_cb(CB_SOCKET_T sfd, void (*txnotify_cb)(void *arg), void *arg);

/**
 * @brief Set the RX notify callback that is invoked when an RX packet is
 * ready to be received.
 * @param sfd socket fd
 * @param rxnotify_cb callback
 * @param arg callback argument
 * @return 0: OK, <0: error, detailed error will be printed out
 */
int cb_lld_set_rxnotify_cb(CB_SOCKET_T sfd, void (*rxnotify_cb)(void *arg), void *arg);

/**
 * @brief Set the default RX data callback that is invoked when an RX packet does not
 * match any filters.
 * @param sfd socket fd
 * @param default_rxdata_cb callback
 * @param arg callback argument
 * @return 0: OK, <0: error, detailed error will be printed out
 */
int cb_lld_set_default_rxdata_cb(CB_SOCKET_T sfd,
		void (*default_rxdata_cb)(void *data, int size, int port, void *arg), void *arg);

/**
 * @brief Get enet type and instance that is set via cb_lld_init_devs_table()
 * @param enet_type ethernet type
 * @param instance_id ethernet instance ID
 * @return 0: OK, <0: error, detailed error will be printed out
 */
int cb_lld_get_type_instance(uint32_t *enet_type, uint32_t *instance_id);

/**
 * @brief Get all virtual network interfaces which were defined for the TI platform.
 * @param netdevs an array of pointer points to the net devices acquired by this api.
 * @param len  how many net devices are returned by this api.
 * @return 0 on success; -1 on failure
 */
int cb_lld_get_netdevs(char* netdevs[], int *len);

/**
 * @brief get link state from device name like 'eth0'
 * @param cfd	a socket handle coressponding to the ethernet
 *			   device name specified via the  second argument `dev`
 * @param dev	ethernet device name like 'eth0'
 * @param linkstate a pointer for storing link state value (0:down, 1:up)
 * @return 0 on success, -1 on error
 */
int cb_lld_get_link_state(CB_SOCKET_T cfd, const char *dev, uint32_t *linkstate);

/**
 * @brief get link info(speed and duplex) from device name like 'eth0'
 * @param cfd	a socket handle coressponding to the ethernet
 *			   device name specified via the  second argument `dev`
 * @param dev	ethenert device name like 'eth0'
 * @param speed  a pointer for storing speed value (0:unknow, 10:10Mbps, 100:100Mbps, 1000:1Gbps)
 * @param duplex a pointer for storing duplex value (0:unknow, 1:Full, 2:Half)
 * @return 0 on success, -1 on error
 */
int cb_lld_get_link_info(CB_SOCKET_T cfd, const char *dev, uint32_t *speed, uint32_t *duplex);

/**
 * @brief Get stats info from host or mac port.
 * @param sfd socket fd
 * @param port 0xFF: host port;  less than 0xFF:  mac port.
 * @param stats stats info of the port.
 * @note valid index of mac port depends on HW platform
 * @return 0 on success, -1 on failure
 */
int cb_lld_get_port_stats(CB_SOCKET_T sfd, int port, cb_tilld_port_stats_t *stats);

/**
 * @brief reset stats info from host or mac port.
 * @param sfd socket fd
 * @param port 0xFF: host port;  less than:  mac port.
 * @note valid index of mac port depends on HW platform
 */
void cb_lld_reset_port_stats(CB_SOCKET_T sfd, int port);

#ifdef __cplusplus
}
#endif
#endif //CB_LLD_ETHERNET_H

/** @}*/
