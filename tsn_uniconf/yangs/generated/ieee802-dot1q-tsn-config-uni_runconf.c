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
#include "../yang_modules.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee1588-ptp.h"
#include "ietf-netconf-monitoring.h"
#include "ietf-yang-library.h"
#include "excelfore-tsn-remote.h"
#include "excelfore-netconf-server.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *ieee802_dot1q_tsn_config_uni_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"tsn-uni", 		// 2(0x2)
	"domain", 		// 3(0x3)
	"domain-id", 		// 4(0x4)
	"cnc-enabled", 		// 5(0x5)
	"cuc", 		// 6(0x6)
	"cuc-id", 		// 7(0x7)
	"stream", 		// 8(0x8)
	"stream-id", 		// 9(0x9)
	"stream-status", 		// 10(0xa)
	"talker", 		// 11(0xb)
	"stream-rank", 		// 12(0xc)
	"rank", 		// 13(0xd)
	"end-station-interfaces", 		// 14(0xe)
	"mac-address", 		// 15(0xf)
	"interface-name", 		// 16(0x10)
	"data-frame-specification", 		// 17(0x11)
	"index", 		// 18(0x12)
	"ieee802-mac-addresses", 		// 19(0x13)
	"destination-mac-address", 		// 20(0x14)
	"source-mac-address", 		// 21(0x15)
	"ieee802-vlan-tag", 		// 22(0x16)
	"priority-code-point", 		// 23(0x17)
	"vlan-id", 		// 24(0x18)
	"traffic-specification", 		// 25(0x19)
	"interval", 		// 26(0x1a)
	"numerator", 		// 27(0x1b)
	"denominator", 		// 28(0x1c)
	"max-frames-per-interval", 		// 29(0x1d)
	"max-frame-size", 		// 30(0x1e)
	"transmission-selection", 		// 31(0x1f)
	"time-aware", 		// 32(0x20)
	"earliest-transmit-offset", 		// 33(0x21)
	"latest-transmit-offset", 		// 34(0x22)
	"jitter", 		// 35(0x23)
	"user-to-network-requirements", 		// 36(0x24)
	"num-seamless-trees", 		// 37(0x25)
	"max-latency", 		// 38(0x26)
	"interface-capabilities", 		// 39(0x27)
	"vlan-tag-capable", 		// 40(0x28)
	"cb-stream-iden-type-list", 		// 41(0x29)
	"cb-sequence-type-list", 		// 42(0x2a)
	"accumulated-latency", 		// 43(0x2b)
	"interface-configuration", 		// 44(0x2c)
	"interface-list", 		// 45(0x2d)
	"config-list", 		// 46(0x2e)
	"time-aware-offset", 		// 47(0x2f)
	"listener", 		// 48(0x30)
	"status-info", 		// 49(0x31)
	"talker-status", 		// 50(0x32)
	"listener-status", 		// 51(0x33)
	"failure-code", 		// 52(0x34)
	"failed-interfaces", 		// 53(0x35)
};

uint8_t ieee802_dot1q_tsn_config_uni_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)IEEE802_DOT1Q_TSN_CONFIG_UNI_ENUM_END;i++){
		if(!strcmp(astr, ieee802_dot1q_tsn_config_uni_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *ieee802_dot1q_tsn_config_uni_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)IEEE802_DOT1Q_TSN_CONFIG_UNI_ENUM_END){return NULL;}
	return ieee802_dot1q_tsn_config_uni_enum_strings[anum];
}

int ieee802_dot1q_tsn_config_uni_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[14]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
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
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	aps[5] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0028_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc
	//0029_ieee802-dot1q-tsn-config-uni/tsn-uni/domain
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
