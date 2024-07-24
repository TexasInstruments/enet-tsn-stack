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
#ifndef __MRPNET_H_
#define __MRPNET_H_

#include <tsn_combase/combase.h>
#include "mrp_datatypes.h"

#ifndef UB_LOGTSTYPE
#define UB_LOGTSTYPE UB_CLOCK_REALTIME
#endif

#define ETHTYPE_MMRP 0x88F6
#define ETHTYPE_MVRP 0x88F5
#define ETHTYPE_MSRP 0x22EA
#define  MCAST_MMRP { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x20 }
#define  CUSTOMER_MCAST_MVRP { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x21 }
#define  PROVIDER_MCAST_MVRP { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0D }
#define  MCAST_MVRP CUSTOMER_MCAST_MVRP
//#define  MCAST_MVRP PROVIDER_MCAST_MVRP
#define  MCAST_MSRP { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E }

static const uint16_t mrp_ethtypes[NUM_PROTOCOLS]={ETHTYPE_MMRP, ETHTYPE_MVRP, ETHTYPE_MSRP};
static const ub_macaddr_t mrp_mcastmacs[NUM_PROTOCOLS]={MCAST_MMRP, MCAST_MVRP, MCAST_MSRP};

typedef int(* mrpnet_readcb)(void *cbdata, uint8_t *rdata,
			     int size, CB_SOCKADDR_T *addr);
// if the callback:'reccb' returns non zero, it closes the connection
// the caller need to put 'mrpnd' in 'cbdata' to call this layer functions from the callback
typedef struct mrpnet_protocol_para{
	int portIndex;
	protocol_index_t protoIndex;
	mrpnet_readcb reccb;
	void *cbdata;
} mrpnet_protocol_para_t;

typedef struct mrpnet_data mrpnet_data_t;

/**
 * @brief initialize
 * @return the data handle of mrpnet object
 * @param pnum	number of ports
 * @param netdevs	network device list
 */
mrpnet_data_t *mrpnet_init(uint8_t instanceIndex, int pnum, const char *netdevs[]);

int mrpnet_close(mrpnet_data_t *mrpnd);
int mrpnet_open_protocol(mrpnet_data_t *mrpnd, mrpnet_protocol_para_t *mppp);
int mrpnet_send(mrpnet_data_t *mrpnd, int portIndex, protocol_index_t pt,
		  uint8_t *data, int size);
int mrpnet_select_rec(mrpnet_data_t *mrpnd, int toutms);
int mrpnet_get_num_of_ports(mrpnet_data_t *mrpnd);

/**
 * @brief reopen network to recover from network device error
 * @return 0:recovered, -1:can't recover
 */
int mrpnet_port_reopen(mrpnet_data_t *mrpnd, int portIndex, protocol_index_t pt);

#endif
