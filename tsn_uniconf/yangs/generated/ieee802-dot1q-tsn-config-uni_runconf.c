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
/* Automatically generated file.  Don't edit this file.*/
#include <stdlib.h>
#include <tsn_unibase/unibase.h>
#include "yang_modules.h"
#include "../yang_db_access.h"
#include "../yang_node.h"
#include "ieee802-dot1q-tsn-config-uni.h"

extern uint8_t IEEE802_DOT1Q_TSN_CONFIG_UNI_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RW IEEE802_DOT1Q_TSN_CONFIG_UNI_func(dbald)
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RO (IEEE802_DOT1Q_TSN_CONFIG_UNI_func(dbald)|0x80u)

const char *ieee802_dot1q_tsn_config_uni_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"tsn-uni", 		// 3(0x3)
	"domain", 		// 4(0x4)
	"domain-id", 		// 5(0x5)
	"cnc-enabled", 		// 6(0x6)
	"cuc", 		// 7(0x7)
	"cuc-id", 		// 8(0x8)
	"stream", 		// 9(0x9)
	"stream-id", 		// 10(0xa)
	"stream-status", 		// 11(0xb)
	"talker", 		// 12(0xc)
	"stream-rank", 		// 13(0xd)
	"rank", 		// 14(0xe)
	"end-station-interfaces", 		// 15(0xf)
	"mac-address", 		// 16(0x10)
	"interface-name", 		// 17(0x11)
	"data-frame-specification", 		// 18(0x12)
	"index", 		// 19(0x13)
	"ieee802-mac-addresses", 		// 20(0x14)
	"destination-mac-address", 		// 21(0x15)
	"source-mac-address", 		// 22(0x16)
	"ieee802-vlan-tag", 		// 23(0x17)
	"priority-code-point", 		// 24(0x18)
	"vlan-id", 		// 25(0x19)
	"traffic-specification", 		// 26(0x1a)
	"interval", 		// 27(0x1b)
	"numerator", 		// 28(0x1c)
	"denominator", 		// 29(0x1d)
	"max-frames-per-interval", 		// 30(0x1e)
	"max-frame-size", 		// 31(0x1f)
	"transmission-selection", 		// 32(0x20)
	"time-aware", 		// 33(0x21)
	"earliest-transmit-offset", 		// 34(0x22)
	"latest-transmit-offset", 		// 35(0x23)
	"jitter", 		// 36(0x24)
	"user-to-network-requirements", 		// 37(0x25)
	"num-seamless-trees", 		// 38(0x26)
	"max-latency", 		// 39(0x27)
	"interface-capabilities", 		// 40(0x28)
	"vlan-tag-capable", 		// 41(0x29)
	"cb-stream-iden-type-list", 		// 42(0x2a)
	"cb-sequence-type-list", 		// 43(0x2b)
	"accumulated-latency", 		// 44(0x2c)
	"interface-configuration", 		// 45(0x2d)
	"interface-list", 		// 46(0x2e)
	"config-list", 		// 47(0x2f)
	"time-aware-offset", 		// 48(0x30)
	"listener", 		// 49(0x31)
	"status-info", 		// 50(0x32)
	"talker-status", 		// 51(0x33)
	"listener-status", 		// 52(0x34)
	"failure-code", 		// 53(0x35)
	"failed-interfaces", 		// 54(0x36)
};
const uint8_t ieee802_dot1q_tsn_config_uni_enum_max=55;

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "dot1q-tsn-config-uni",
		"urn:ieee:std:802.1Q:yang:ieee802-dot1q-tsn-config-uni")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[13];
	apsd[0]=IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	apsd[1]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TSN_UNI;
	apsd[2]=IEEE802_DOT1Q_TSN_CONFIG_UNI_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"dot1q-tsn-config-uni", 21)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int ieee802_dot1q_tsn_config_uni_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[14]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/stream-rank
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	aps[1] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TSN_UNI;
	aps[2] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DOMAIN;
	aps[3] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CUC;
	aps[4] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM;
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM_RANK;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CUC_ID;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[4] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DOMAIN_ID;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0001_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/end-station-interfaces
	aps[3] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CUC;
	aps[4] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM;
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_END_STATION_INTERFACES;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0002_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/data-frame-specification/ieee802-mac-addresses
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DATA_FRAME_SPECIFICATION;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_MAC_ADDRESSES;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0003_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/data-frame-specification/ieee802-vlan-tag
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_VLAN_TAG;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0004_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/data-frame-specification
	//0005_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/traffic-specification/interval
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TRAFFIC_SPECIFICATION;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERVAL;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0006_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/traffic-specification/time-aware
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TIME_AWARE;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_EARLIEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_LATEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_JITTER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0007_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/traffic-specification
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0008_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/user-to-network-requirements
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_USER_TO_NETWORK_REQUIREMENTS;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0009_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-capabilities
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CAPABILITIES;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0010_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RO;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CONFIGURATION;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_LIST;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_MAC_ADDRESSES;
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0011_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RO;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_LIST;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_VLAN_TAG;
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0012_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list/config-list
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0013_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list
	//0014_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener
	//0015_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0016_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/end-station-interfaces
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_LISTENER;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_END_STATION_INTERFACES;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0017_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/user-to-network-requirements
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_USER_TO_NETWORK_REQUIREMENTS;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0018_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-capabilities
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CAPABILITIES;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0019_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RO;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CONFIGURATION;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_LIST;
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_MAC_ADDRESSES;
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0020_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[8] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_VLAN_TAG;
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0021_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list/config-list
	aps[9] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0022_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list
	//0023_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/status-info
	//0024_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0025_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/status-info
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RO;
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STATUS_INFO;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0026_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/failed-interfaces
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_FAILED_INTERFACES;
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0027_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0028_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc
	//0029_ieee802-dot1q-tsn-config-uni/tsn-uni/domain
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	aps[3] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CNC_ENABLED;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RO;
	aps[1] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

