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
#ifndef __MRP_BRIDGE_CONFIG_H_
#define __MRP_BRIDGE_CONFIG_H_

#include "mrp_datatypes.h"

#define MAX_TALKERS_REGISTRATION 1024
#define MAX_DYNAMIC_VLAN_REGISTRATION 4096
#define MAX_DYNAMIC_DOMAIN_REGISTRATION 1024

/* how many traffic classes are available is defined staticaly by the configuration.
 * because the maximum traffic classes is 8, 'cb_used' has 8 elements.
 * 'dvlan_ids' keeps track of the dynamically registered VLAN IDs.
 */
typedef struct mrp_bridge_port_data {
	//cb_used[0]: used band width for each class with evmergency rank(0 to 7)
	//cb_used[1]: used band width for each class with regular rank(0 to 7)
	int cb_used[2][8];
	ub_esarray_cstd_t *dvlan_ids;
	ub_esarray_cstd_t *rtable;
	ub_esarray_cstd_t *blacklist;
}mrp_bridge_port_data_t;

typedef struct mrp_bridge_config_data {
	int numofports;
	const char **netdevs;
	ub_bytearray8_t bridge_id;
	mrp_bridge_port_data_t *pdata;
	ub_esarray_cstd_t *talker_regs;
	uint8_t instanceIndex;
	char *bridgename;
} mrp_bridge_config_data_t;

typedef enum {
	MRP_BRIDGE_CONFIG_MMRP,
	MRP_BRIDGE_CONFIG_MVRP,
	MRP_BRIDGE_CONFIG_MSRP,
} mrp_bridge_config_type;

typedef enum {
	BRIDGE_CONFIG_VLAN_JOIN_NEW,
	BRIDGE_CONFIG_VLAN_JOIN,
	BRIDGE_CONFIG_VLAN_LEAVE,
	BRIDGE_CONFIG_MVRP_END,
} mrp_bridge_config_mvrp_cmd;

typedef enum {
	BRIDGE_CONFIG_TALKER_NEW,
	BRIDGE_CONFIG_TALKER_JOIN,
	BRIDGE_CONFIG_TALKER_LEAVE,
	BRIDGE_CONFIG_TALKER_OUTP_REG,
	BRIDGE_CONFIG_TALKER_OUTP_CHECK,
	BRIDGE_CONFIG_TALKER_OUTP_LEAVE,
	BRIDGE_CONFIG_TALKER_FAILED_NEW,
	BRIDGE_CONFIG_TALKER_FAILED_JOIN,
	BRIDGE_CONFIG_TALKER_FAILED_LEAVE,
	BRIDGE_CONFIG_LISTENER_NEW,
	BRIDGE_CONFIG_LISTENER_JOIN,
	BRIDGE_CONFIG_LISTENER_LEAVE,
	BRIDGE_CONFIG_DOMAIN_NEW,
	BRIDGE_CONFIG_DOMAIN_JOIN,
	BRIDGE_CONFIG_DOMAIN_LEAVE,
	BRIDGE_CONFIG_QUERY_RESERVE,
	BRIDGE_CONFIG_MSRP_END,
} mrp_bridge_config_msrp_cmd;

typedef enum {
	BRIDGE_LISTENER_FAIL_NOTALKER = -1,
	BRIDGE_LISTENER_SUCCESS = 0,
	BRIDGE_LISTENER_FAIL_NOBW,
	BRIDGE_LISTENER_FAIL_TALKERFAILED,
	BRIDGE_LISTENER_SUCCESS_REMOVELOW,
} mrp_bridge_config_lister_result;

typedef struct mrp_bridge_config_mvrp_data{
	mrp_bridge_config_mvrp_cmd cmd;
	uint16_t vlan_id;
} mrp_bridge_config_mvrp_data_t;

typedef enum {
	BRIDGE_NORESERVE_NONE = 0,
	BRIDGE_NORESERVE_REGULAR_FAIL,
	BRIDGE_NORESERVE_NOT_AVB_CAPABLE,
} bridge_noreserve_t;

typedef struct mrp_bridge_config_msrp_data{
	mrp_bridge_config_msrp_cmd cmd;
	msrp_stream_data_t stream;
	bridge_noreserve_t noreserve;
} mrp_bridge_config_msrp_data_t;

typedef struct mrp_bridge_config_call_data{
	mrp_bridge_config_type conftype;
	int portIndex;
	union {
		mrp_bridge_config_mvrp_data_t mvrpconf;
		mrp_bridge_config_msrp_data_t msrpconf;
	};
} mrp_bridge_config_call_data_t;


void mrp_bridge_get_bridge_id(mrp_bridge_config_data_t *mrpbrcfg, ub_bytearray8_t bid);

mrp_bridge_config_data_t *mrp_bridge_config_init(int numofports, const char **netdevs,
						 uint8_t instanceIndex);

int mrp_bridge_config_close(mrp_bridge_config_data_t *mrpbrcfg);

/**
 * @brief
 *	this function is called, when mmrp/mvrp/msrp needs to configure the bridge.
 *	when the fuction returns non-zero, it means configuration failed.
 *	For TalkerAdvertisement, non-zero return code is Failure Code(Table 35-6)
 * @return
 *	-1: error, don't propagate to any ports
 *	0: no error, propagate to all the other ports
 *	(portIndex+1 << 16) | 0: no error, propagate to portIndex
 *	error_code: error, propagate to all the other ports
 *	(portIndex+1 << 16) | error_code: error, propagate to portIndex
 *
 *	for TalkerAdvertise, error_code is MRP_MSRP_FAILURE_*
 *	for Listener, error_code is MSRP_FP_*. the four packed value must be changed to prpagate.
 */
int mrp_bridge_config_call(mrp_bridge_config_data_t *mrpbrcfg,
			   mrp_bridge_config_call_data_t *ccdata);

uint32_t  mrp_bridge_get_latency(mrp_bridge_config_data_t *mrpbrcfg, uint8_t portIndex,
				 msrp_stream_data_t *streamd);

int mrp_bridge_set_boundary(mrp_bridge_config_data_t *mrpbrcfg, uint8_t portIndex,
			    uint8_t tc, bool bpTrue);

#endif
