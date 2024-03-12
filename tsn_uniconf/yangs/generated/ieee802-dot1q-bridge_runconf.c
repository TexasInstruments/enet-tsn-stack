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
#include "ieee802-dot1q-bridge.h"

extern uint8_t IEEE802_DOT1Q_BRIDGE_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_BRIDGE_RW IEEE802_DOT1Q_BRIDGE_func(dbald)
#define IEEE802_DOT1Q_BRIDGE_RO (IEEE802_DOT1Q_BRIDGE_func(dbald)|0x80u)

const char *ieee802_dot1q_bridge_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"bridges", 		// 3(0x3)
	"bridge", 		// 4(0x4)
	"name", 		// 5(0x5)
	"address", 		// 6(0x6)
	"ports", 		// 7(0x7)
	"up-time", 		// 8(0x8)
	"components", 		// 9(0x9)
	"component", 		// 10(0xa)
	"id", 		// 11(0xb)
	"traffic-class-enabled", 		// 12(0xc)
	"bridge-port", 		// 13(0xd)
	"filtering-database", 		// 14(0xe)
	"size", 		// 15(0xf)
	"static-entries", 		// 16(0x10)
	"dynamic-entries", 		// 17(0x11)
	"static-vlan-registration-entries", 		// 18(0x12)
	"dynamic-vlan-registration-entries", 		// 19(0x13)
	"mac-address-registration-entries", 		// 20(0x14)
	"filtering-entry", 		// 21(0x15)
	"database-id", 		// 22(0x16)
	"vids", 		// 23(0x17)
	"entry-type", 		// 24(0x18)
	"port-map", 		// 25(0x19)
	"port-ref", 		// 26(0x1a)
	"static-filtering-entries", 		// 27(0x1b)
	"control-element", 		// 28(0x1c)
	"connection-identifier", 		// 29(0x1d)
	"registrar-admin-control", 		// 30(0x1e)
	"vlan-transmitted", 		// 31(0x1f)
	"dynamic-reservation-entries", 		// 32(0x20)
	"dynamic-filtering-entries", 		// 33(0x21)
	"status", 		// 34(0x22)
	"vlan-registration-entry", 		// 35(0x23)
	"permanent-database", 		// 36(0x24)
	"bridge-vlan", 		// 37(0x25)
	"max-vids", 		// 38(0x26)
	"vlan", 		// 39(0x27)
	"vid", 		// 40(0x28)
	"untagged-ports", 		// 41(0x29)
	"egress-ports", 		// 42(0x2a)
};
const uint8_t ieee802_dot1q_bridge_enum_max=43;

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "dot1q",
		"urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[11];
	apsd[0]=IEEE802_DOT1Q_BRIDGE_RW;
	apsd[1]=IEEE802_DOT1Q_BRIDGE_BRIDGES;
	apsd[2]=IEEE802_DOT1Q_BRIDGE_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"dot1q", 6)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int ieee802_dot1q_bridge_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[12]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/static-filtering-entries
	aps[0] = IEEE802_DOT1Q_BRIDGE_RW;
	aps[1] = IEEE802_DOT1Q_BRIDGE_BRIDGES;
	aps[2] = IEEE802_DOT1Q_BRIDGE_BRIDGE;
	aps[3] = IEEE802_DOT1Q_BRIDGE_COMPONENT;
	aps[4] = IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE;
	aps[5] = IEEE802_DOT1Q_BRIDGE_FILTERING_ENTRY;
	aps[6] = IEEE802_DOT1Q_BRIDGE_PORT_MAP;
	aps[7] = IEEE802_DOT1Q_BRIDGE_STATIC_FILTERING_ENTRIES;
	aps[8] = IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_BRIDGE_CONNECTION_IDENTIFIER;
	vtype=YANG_VTYPE_PORT_NUMBER_TYPE;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_BRIDGE_PORT_REF;
	vtype=YANG_VTYPE_PORT_NUMBER_TYPE;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_BRIDGE_DATABASE_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_BRIDGE_VIDS;
	vtype=YANG_VTYPE_DOT1QTYPES_VID_RANGE_TYPE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_BRIDGE_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[5] = IEEE802_DOT1Q_BRIDGE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[4] = IEEE802_DOT1Q_BRIDGE_NAME;
	vtype=YANG_VTYPE_DOT1QTYPES_NAME_TYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0001_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/static-vlan-registration-entries
	aps[3] = IEEE802_DOT1Q_BRIDGE_COMPONENT;
	aps[4] = IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE;
	aps[5] = IEEE802_DOT1Q_BRIDGE_FILTERING_ENTRY;
	aps[6] = IEEE802_DOT1Q_BRIDGE_PORT_MAP;
	aps[7] = IEEE802_DOT1Q_BRIDGE_STATIC_VLAN_REGISTRATION_ENTRIES;
	aps[8] = IEEE802_DOT1Q_BRIDGE_REGISTRAR_ADMIN_CONTROL;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_BRIDGE_VLAN_TRANSMITTED;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0002_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/mac-address-registration-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_MAC_ADDRESS_REGISTRATION_ENTRIES;
	aps[8] = IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0003_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/dynamic-vlan-registration-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_VLAN_REGISTRATION_ENTRIES;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0004_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/dynamic-reservation-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_RESERVATION_ENTRIES;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0005_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/dynamic-filtering-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_FILTERING_ENTRIES;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0006_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map
	//0007_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry
	aps[6] = IEEE802_DOT1Q_BRIDGE_ENTRY_TYPE;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[6] = IEEE802_DOT1Q_BRIDGE_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0008_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/static-filtering-entries
	aps[0] = IEEE802_DOT1Q_BRIDGE_RW;
	aps[5] = IEEE802_DOT1Q_BRIDGE_VLAN_REGISTRATION_ENTRY;
	aps[6] = IEEE802_DOT1Q_BRIDGE_PORT_MAP;
	aps[7] = IEEE802_DOT1Q_BRIDGE_STATIC_FILTERING_ENTRIES;
	aps[8] = IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_BRIDGE_CONNECTION_IDENTIFIER;
	vtype=YANG_VTYPE_PORT_NUMBER_TYPE;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_BRIDGE_PORT_REF;
	vtype=YANG_VTYPE_PORT_NUMBER_TYPE;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_BRIDGE_DATABASE_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_BRIDGE_VIDS;
	vtype=YANG_VTYPE_DOT1QTYPES_VID_RANGE_TYPE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0009_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/static-vlan-registration-entries
	aps[6] = IEEE802_DOT1Q_BRIDGE_PORT_MAP;
	aps[7] = IEEE802_DOT1Q_BRIDGE_STATIC_VLAN_REGISTRATION_ENTRIES;
	aps[8] = IEEE802_DOT1Q_BRIDGE_REGISTRAR_ADMIN_CONTROL;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_BRIDGE_VLAN_TRANSMITTED;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0010_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/mac-address-registration-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_MAC_ADDRESS_REGISTRATION_ENTRIES;
	aps[8] = IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0011_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/dynamic-vlan-registration-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_VLAN_REGISTRATION_ENTRIES;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0012_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/dynamic-reservation-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_RESERVATION_ENTRIES;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0013_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/dynamic-filtering-entries
	aps[7] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_FILTERING_ENTRIES;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0014_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map
	//0015_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry
	aps[6] = IEEE802_DOT1Q_BRIDGE_ENTRY_TYPE;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0016_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[5] = IEEE802_DOT1Q_BRIDGE_SIZE;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_BRIDGE_STATIC_ENTRIES;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_ENTRIES;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_BRIDGE_STATIC_VLAN_REGISTRATION_ENTRIES;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_VLAN_REGISTRATION_ENTRIES;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_BRIDGE_MAC_ADDRESS_REGISTRATION_ENTRIES;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0017_ieee802-dot1q-bridge/bridges/bridge/component/permanent-database/filtering-entry
	aps[4] = IEEE802_DOT1Q_BRIDGE_PERMANENT_DATABASE;
	aps[5] = IEEE802_DOT1Q_BRIDGE_FILTERING_ENTRY;
	aps[6] = IEEE802_DOT1Q_BRIDGE_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_BRIDGE_RW;
	aps[6] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_BRIDGE_DATABASE_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_BRIDGE_VIDS;
	vtype=YANG_VTYPE_DOT1QTYPES_VID_RANGE_TYPE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_BRIDGE_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0018_ieee802-dot1q-bridge/bridges/bridge/component/permanent-database
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[5] = IEEE802_DOT1Q_BRIDGE_SIZE;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_BRIDGE_STATIC_ENTRIES;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_BRIDGE_STATIC_VLAN_REGISTRATION_ENTRIES;
	vtype=YANG_VTYPE_YANG_GAUGE32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0019_ieee802-dot1q-bridge/bridges/bridge/component/bridge-vlan/vlan
	aps[0] = IEEE802_DOT1Q_BRIDGE_RW;
	aps[4] = IEEE802_DOT1Q_BRIDGE_BRIDGE_VLAN;
	aps[5] = IEEE802_DOT1Q_BRIDGE_VLAN;
	aps[6] = IEEE802_DOT1Q_BRIDGE_NAME;
	vtype=YANG_VTYPE_DOT1QTYPES_NAME_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[6] = IEEE802_DOT1Q_BRIDGE_UNTAGGED_PORTS;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_BRIDGE_EGRESS_PORTS;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_BRIDGE_RW;
	aps[6] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_BRIDGE_VID;
	vtype=YANG_VTYPE_DOT1QTYPES_VLAN_INDEX_TYPE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0020_ieee802-dot1q-bridge/bridges/bridge/component/bridge-vlan
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[5] = IEEE802_DOT1Q_BRIDGE_MAX_VIDS;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0021_ieee802-dot1q-bridge/bridges/bridge/component
	aps[0] = IEEE802_DOT1Q_BRIDGE_RW;
	aps[4] = IEEE802_DOT1Q_BRIDGE_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_BRIDGE_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_BRIDGE_TRAFFIC_CLASS_ENABLED;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[4] = IEEE802_DOT1Q_BRIDGE_PORTS;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_BRIDGE_BRIDGE_PORT;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0022_ieee802-dot1q-bridge/bridges/bridge
	aps[0] = IEEE802_DOT1Q_BRIDGE_RW;
	aps[3] = IEEE802_DOT1Q_BRIDGE_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[3] = IEEE802_DOT1Q_BRIDGE_PORTS;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1Q_BRIDGE_UP_TIME;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1Q_BRIDGE_COMPONENTS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[1] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

