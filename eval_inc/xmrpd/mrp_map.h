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
#ifndef __MRP_MAP_H_
#define __MRP_MAP_H_

#include "mrpnet.h"
#include "mrpman.h"
#include "mrp_mad.h"
#include "mrp_bridge_config.h"
#include "xmrpdconf/mrpgcfg.h"

#define MRP_EVENT_AS_CAPABLE_UP (1<<0)
#define MRP_EVENT_AS_CAPABLE_DOWN (1<<1)
#define MRP_EVENT_NETDEV_UP (1<<2)
#define MRP_EVENT_NETDEV_DOWN (1<<3)
#define MRP_EVENT_RSTP_REDECLARE (1<<4)
#define MRP_EVENT_RSTP_FLUSH (1<<5)

#define MPR_EVNET_AS_NETDEV_MASK (MRP_EVENT_AS_CAPABLE_UP|MRP_EVENT_AS_CAPABLE_DOWN|\
MRP_EVENT_NETDEV_UP|MRP_EVENT_NETDEV_DOWN)
#define MPR_EVNET_RSTP_MASK (MRP_EVENT_RSTP_REDECLARE|MRP_EVENT_RSTP_FLUSH)

mrp_map_data_t *mrp_map_init(mrpgcfgd_t *mgcfgd);
void mrp_map_close(mrp_map_data_t *mapd);

int mrp_map_proto_up(mrp_map_data_t *mapd, int portIndex, protocol_index_t protoIndex);
int mrp_map_proto_down(mrp_map_data_t *mapd, int portIndex, protocol_index_t protoIndex);
bool mrp_map_proto_isup(mrp_map_data_t *mapd, int portIndex, protocol_index_t protoIndex);
void mrp_map_proc(mrp_map_data_t *mapd, int portIndex, protocol_index_t protoIndex,
		  mrp_mad_actions_t madaction, attrstate_t *astate, uint64_t ts);
mrp_bridge_config_data_t *mrp_map_bridge_config_instance(mrp_map_data_t *mapd);
int mrp_map_find_attr_port(mrp_map_data_t *mapd, protocol_index_t protoIndex,
			   attrstate_t *astate, int cmpstart, int cmpsize,
			   int *sport, int decreg);
attrstate_t *mrp_map_find_attr(mrp_map_data_t *mapd, protocol_index_t protoIndex,
			       attrstate_t *astate, int cmpstart, int cmpsize,
			       int *sport, int decreg);
void mrp_map_proc_to_myport(mrp_map_data_t *mapd, int portIndex, protocol_index_t protoIndex,
			    mrp_mad_actions_t madaction, attrstate_t *astate, int cmpsize,
			    int decreg, uint64_t ts);

void mrp_map_set_gptpevent(mrp_map_data_t *mapd, int portIndex, bool eventup);

uint8_t mrp_map_instanceIndex(mrp_map_data_t *mapd);

const char *mrp_map_getnetdev(mrp_map_data_t *mapd, uint8_t portIndex);

int mrp_map_eloop(mrp_map_data_t *mapd, bool *stoprun);

bool mrp_map_afterifupdown(mrp_map_data_t *mapd, int portIndex);

#endif
