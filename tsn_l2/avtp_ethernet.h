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
 * @addtogroup xl4avbl2
 * @{
 * @file avtp_ethernet.h
 * @brief avtp ethernet socket layer for AVTP
 */

#ifndef __AVTP_ETHERNET_H_
#define __AVTP_ETHERNET_H_

#include <stdio.h>
#include <tsn_combase/combase.h>

#ifdef POSIX_PLATFORM
#include <unistd.h>
#define MKPIPE pipe
#endif

#ifndef DEFAULT_PTPDEVICE
#define DEFAULT_PTPDEVICE "/dev/ptp0"
#define DEFAULT_NETDEV "eth0"
#endif

/* the most of platforms use un-tagged packets as received data */
#define VLAN_REC_TAGGED false

#if defined(GHINTEGRITY)
#define THREAD_HIGH_PRI 190
#define THREAD_NORM_PRI 148
#define THREAD_HIGH_STACK 0
#define THREAD_NORM_STACK 0
#else
#define THREAD_HIGH_PRI 0
#define THREAD_NORM_PRI 0
#define THREAD_HIGH_STACK 0
#define THREAD_NORM_STACK 0
#endif

/**
 * @brief traffic shaper structure
 */
typedef struct tsshaper{
	uint64_t last_send_ts; //!< internal use
	int last_send_size; //!< internal use
	int tshape_rate; //!< rate limit in bytes per msec unit
	int tshape_minsleep; //!< minimum sleep time to keep the rate limit in nsec unit
}tsshaper_t;

/**
 * @brief VLAN tag field used for VLAN Tagging.
 */
struct vlan_af {
	uint16_t pcv;
	uint16_t proto;
} __attribute__ ((packed));

/**
 * @brief VLAN tag extended data field
 */
typedef struct exh_data{
	struct vlan_af vlanaf;
	uint8_t data[ETH_DATA_LEN];
}exh_data_t;

/**
 * @brief ethernet frame data structure
 */
typedef struct ether_frame{
	CB_ETHHDR_T hdr;
	union {
		exh_data_t ed;
		uint8_t data[ETH_DATA_LEN];
	}u;
} __attribute__ ((packed)) ether_frame_t;

/**
 * @brief avtp parameters.which contains following fields.\n
 */
typedef struct avtp_eth_paras{
	const char *dev; //!< network device name
	bool promisc; //!< true to use promiscuous mode socket
	const unsigned char *mcastmac; //!< multicast address
	bool rec_tagged; //!< true:open a socket with ETH_P_8021Q. false: with ETH_P_TSN
	int priority; //!< VLAN pcp priority value
	cb_rawsock_rw_t rw_type; //!< one of RAWSOCK_WRONLY, RAWSOCK_RDONLY, RAWSOCK_RDWR
	uint16_t tm_sport; //!<  source port in the test mode
	uint16_t tm_dport; //!<  destination port in the test mode
	uint16_t vlanid; //!< vlan identifier
}avtp_eth_paras_t;

typedef struct avtp_traffic_shaper_prams{
	uint8_t delta_bw; // percent unit
	uint32_t idleslope; // kbps unit
	uint32_t traffic_interval; // usec unit
	uint32_t max_num_packets;
}avtp_traffic_shaper_prams_t;

#define EFRAME_VLANAF eframe.u.ed.vlanaf
#define VLAN_AF_SIZE sizeof(struct vlan_af)

/**
 * @brief a base udp port number for the test mode
 */
#define UDP_PORT_BASE 43255

/**
 * @brief callback funtion type,called from the raw ethernet receiver thread
 */
typedef int (*avtp_ethernet_rcv_cb_t)(uint8_t *data, int size,
				      CB_ETHHDR_T *ethdr, void *cbdata, uint16_t pcv);

/**
 * @brief the data handle of avtp_ethernet object, the inside of it is private
 */
typedef struct avtp_ethernet_handle *avtp_ethernet_handle_t;

/**
 * @brief open avtp ethernet socket for AVTP, and start receiver thread.
 * @return the data handle of avtp_ethernet object
 * @param ethp	avtp raw ethernet socket open parameters
 * @param rcv_cb	a callback functions which is called fromt the receiver
 * thread.  and 'cbdata' is passed to that callback function.
 * @param cbdata	passed to that callback function:rcv_cb
 *
 * @note
 * PROTOCOL AVTP:0x22f0 is used for the communication. \n
 *
 */
avtp_ethernet_handle_t avtp_ethernet_open(avtp_eth_paras_t *ethp,
					  avtp_ethernet_rcv_cb_t rcv_cb, void *cbdata);

/**
 * @brief terminate running receiver thread, and close the avtp ethernet socket.
 * @param reh	the data handle of avtp_ethernet object
 * @return void
 */
void avtp_ethernet_close(avtp_ethernet_handle_t reh);

/**
 * @brief set PROTOCOL in VLAN field
 * @param reh	the data handle of avtp_ethernet object
 * @param proto	16-bit protocol value
 * @note This layer is designed for AVTP specific,
 * so that almost always the default protocol:ETH_P_TSN
 * is used and this function is not used.
 * @return void
 *
 */
void avtp_ethernet_set_vlan_proto(avtp_ethernet_handle_t reh, uint16_t proto);

/**
 * @brief set source mac address
 * @param reh	the data handle of avtp_ethernet object
 * @param bmac mac address in binary format
 * @param strmac mac address in string format
 * @note Either binary:bmac or string:strmac can be used to set the value. \n
 * unused one should be NULL. \n
 * Normally, this is set by device name in avtp_ethernet_open, and this function is not used.
 * @return void
 */
void avtp_ethernet_set_smac(uint8_t *h_source, ub_macaddr_t bmac, const char *strmac);

/**
 * @brief set source mac address by device name
 * @return 0 on success, -1 on error
 * @param h_source	the result comes this pointer
 * @param dev ethernet device name like 'eth0'
 * @note When the mac of the device is unknown, \n
 * this function can be used instead of avtp_ethernet_set_smac.
 */
int avtp_ethernet_set_smac_bydev(avtp_ethernet_handle_t reh, const char *dev);

/**
 * @brief get source mac address by binary
 * @return source mac address in binary form
 * @param reh	the data handle of avtp_ethernet object
 * @note the returned value is a pointer in place of using header,
 * and it shouldn't be changed by the caller
 */
uint8_t *avtp_ethernet_get_smac(avtp_ethernet_handle_t reh);

/**
 * @brief set destination mac
 * @param reh	the data handle of avtp_ethernet object
 * @param bmac mac address in binary format
 * @param strmac mac address in string format
 * @note Either binary:bmac or string: strmac can be used to set the value. \n
 * unused one should be NULL. \n
 * The destination mac address should be set whenever it has a change
 */
void avtp_ethernet_set_dmac(avtp_ethernet_handle_t reh, ub_macaddr_t bmac, const char *strmac);

/**
 * @brief return max packet size including the ethernet header
 * @param reh	the data handle of avtp_ethernet object
 * @return maximum packet size, this is normally 1500+18
 */
int avtp_ethernet_maxpacket_size(avtp_ethernet_handle_t reh);

/**
 * @brief send a packet of data on the avtp socket
 * @return 0:success, -1:error
 * @param reh	the data handle of avtp_ethernet object
 * @param data	data pointer
 * @param size	byte number to be sent
 * @param pcv	consits of pcp(3 bits),cfi(1 bit),vid(12 bits), and is host order 16-bit value
 * @note size must be less than or equal to the value by avtp_ethernet_maxpacket_size \n
 * if pcv==0, it sends untagged data, which is not in normal usage \n
 * source mac addres must be pre-defined by avtp_ethernet_open or avtp_ethernet_set_smac \n
 * destination mac addres must be pre-defined by avtp_ethernet_set_dmac \n
 * this function blocks the call untill the whole data has been sent
 */
int avtp_ethernet_write_nc(avtp_ethernet_handle_t reh, uint8_t *data, int size, uint16_t pcv);

/**
 * @brief add multicast mac to receive
 * @return 0 on success, -1 on error
 * @param reh	the data handle of avtp_ethernet object
 * @param dev	ethernet device name like 'eth0'
 * @param mcastmac a new multicast mac to be added, in string format
 * @note platform specific, resides in *specific.c
 */
int avtp_ethernet_add_mcast(avtp_ethernet_handle_t reh, const char *dev,
			    const unsigned char *mcastmac);

/**
 * @brief remove multicast mac to receive
 * @return 0 on success, -1 on error
 * @param reh	the data handle of avtp_ethernet object
 * @param dev	ethenert device name like 'eth0'
 * @param mcastmac a new multicast mac to be added, in string format
 * @note platform specific, resides in *specific.c
 */
int avtp_ethernet_del_mcast(avtp_ethernet_handle_t reh, const char *dev,
			    const unsigned char *mcastmac);

/**
 * @brief software traffic shaper; if sending data becomes overrated, make a sleep in the function
 * @return always return 0
 * @param tshd	tshaper data, this data must be preserved outside of this function
 * @param tsize	the size of data to send
 * @note this is used in 'per-stream shaper' which is different from 'per-traffic_class shaper'
 */
int avtp_stream_tshaper_sleep(tsshaper_t *tshd, int tsize);

/**
 * @brief lock mutex to avtp_ethernet_handle_t in the shared connections
 * especially needed when call avtp_ethernet_set_dmac with multiple connections
 * @param reh	the data handle of avtp_ethernet object
 */

void avtp_ethernet_mutex_lock(avtp_ethernet_handle_t reh);
/**
 * @brief unlock mutex to avtp_ethernet_handle_t in the shared connections
 * @param reh	the data handle of avtp_ethernet object
 */
void avtp_ethernet_mutex_unlock(avtp_ethernet_handle_t reh);

/**
 * @brief set up the software traffic shaper(per praffic_class shaper)
 * @param reh	the data handle of avtp_ethernet object
 * @param numtc	number of traffic classes, setting 0 diaables the shaper
 * @param prate	port bitrate, setting 0 lets ethtool detect
 * @param pcp2tc	mapping to pcp to traffic class, must have numtc elements
 * @param tsp	parameters to initialize the shaper
 */
void avtp_set_traffic_shaper(avtp_ethernet_handle_t reh, int numtc,
			     uint32_t prate, uint8_t* pcp2tc,
			     avtp_traffic_shaper_prams_t *tsp);

/**
 * @brief update running traffic for the traffic shaper
 * @param reh	the data handle of avtp_ethernet object
 * @param tc	traffic class
 * @param add_slope	add this rate(kbps unit) in the used traffic, use negative to remove
 */
int avtp_update_traffic(avtp_ethernet_handle_t reh, uint8_t tc, int32_t add_slope);

#endif
/** @}*/
