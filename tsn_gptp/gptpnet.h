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
#ifndef GPTPNET_H_
#define GPTPNET_H_
#include "gptpbasetypes.h"

// 10.4.3 Addresses
#define GPTP_MULTICAST_DEST_ADDR {0x01,0x80,0xC2,0x00,0x00,0x0E}

/* Maximum buffer size for PTP packets
 * This size is refered to by receive buffer queue and send buffer which may
 * result to the following total size statically allocated for these buffers:
 *  = (receive queue allocations) + (send buffer allocation)
 *  = (number of ports x GPTP_MAX_PACKET_SIZE x 2) + (GPTP_MAX_PACKET_SIZEx1)
 *
 * For optimization purposes, value of 256 should be enough for usual 802.1AS
 * frames. This value can be changed to this optimized value when memory
 * constraints is an issue.
 *
 * However, for Announce message which can contain variable size TLV, the
 * specified maximum TLV size is 1428 thus technically, the maximum frame for
 * Announce should be 1428+4+68=1500 bytes. It is also a mandatory requirement
 * for AVNU to support TLV of maximum length (gPTP.br.c.24.01 and
 * gPTP.br.c.24.02).
 */
#define GPTP_MAX_PACKET_SIZE 1500

typedef struct gptpnet_data gptpnet_data_t;

typedef enum {
	GPTPNET_EVENT_NONE=0,
	GPTPNET_EVENT_TIMEOUT,
	GPTPNET_EVENT_DEVUP,
	GPTPNET_EVENT_DEVDOWN,
	GPTPNET_EVENT_RECV,
	GPTPNET_EVENT_TXTS,
	GPTPNET_EVENT_GUARDUP,
	GPTPNET_EVENT_GUARDDOWN,
} gptpnet_event_t;

/*
 * callback function uses portIndex which is ndevIndex+1
 * when portIndex=0, the event is not network device specific
 */
typedef int (*gptpnet_cb_t)(void *cb_data, int portIndex, gptpnet_event_t event,
			    int64_t *event_ts64, void *event_data);

typedef struct event_data_txts {
	uint8_t msgtype;
	uint8_t domain;
	uint16_t seqid;
	int64_t ts64;
} event_data_txts_t;

typedef struct event_data_recv {
	uint8_t msgtype;
	uint8_t domain;
	int64_t ts64;
	uint8_t *recbptr;
} event_data_recv_t;

/**
 * @brief gptp data for netlink. this structure is formed with following details:
 *  1. up -> true or false, used to know that device is up or not.
 *  2. devname -> character array is used to hold device name (interface name i.e eth0,... ).
 *  3. ptpdev -> array used to hold ptp device name.
 *  4. speed -> integer used to notify speed.
 *  5. duplex -> integer used to notify type of communication.
 *  6. portid -> ClockIdentity is used to hold port identity number.
 */
typedef struct gptpnet_data_netlink {
	uint32_t speed;
	uint32_t duplex;
	ClockIdentity portid;
	uint8_t up;
	char devname[XL4_DATA_ABS_MAX_NETDEVS];
	char ptpdev[XL4_DATA_PATH_STR_SHORTLEN];
	uint64_t downts;
} gptpnet_data_netlink_t;

typedef gptpnet_data_netlink_t event_data_netlink_t;

gptpnet_data_t *gptpnet_init(uint8_t gptpInstanceIndex, gptpnet_cb_t cb_func,
			     void *cb_data, const char *netdev[], uint8_t num_ports,
			     char *master_ptpdev);
int gptpnet_activate(gptpnet_data_t *gpnet);
int gptpnet_close(gptpnet_data_t *gpnet);
int gptpnet_eventloop(gptpnet_data_t *gpnet, bool *stoploop);
uint8_t *gptpnet_get_sendbuf(gptpnet_data_t *gpnet, int ndevIndex);
int gptpnet_send(gptpnet_data_t *gpnet, int ndevIndex, uint16_t length);
char *gptpnet_ptpdev(gptpnet_data_t *gpnet, int ndevIndex);
int gptpnet_num_netdevs(gptpnet_data_t *gpnet);
int gptpnet_tsn_schedule(gptpnet_data_t *gpnet, uint32_t aligntime, uint32_t cycletime);

/**
 * @brief return portid, which is extended from MAC address by inserting FF:FE
 * @param ndevIndex	index of a network device
 */
uint8_t *gptpnet_portid(gptpnet_data_t *gpnet, int ndevIndex);

/**
 * @brief copy portid, and then replace 4th,5th byte to domainNumber for domainNumber!=0
 *	  for domainNumber==0, return copy of portid
 * @param ndevIndex	index of a network device
 * @param domainNumber	domain Number
 */
void gptpnet_create_clockid(gptpnet_data_t *gpnet, uint8_t *id,
			    int ndevIndex, int8_t domainNumber);

uint64_t gptpnet_txtslost_time(gptpnet_data_t *gpnet, int ndevIndex);
int gptpnet_get_nlstatus(gptpnet_data_t *gpnet, int ndevIndex, event_data_netlink_t *nlstatus);

/**
 * @brief make the next timeout happen in toutns (nsec)
 * @param toutns	if 0, use the default(GPTPNET_EXTRA_TOUTNS)
 */
void gptpnet_extra_timeout(gptpnet_data_t *gpnet, int toutns);

#endif
