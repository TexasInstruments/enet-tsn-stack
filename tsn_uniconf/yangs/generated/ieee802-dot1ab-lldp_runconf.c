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
#include "ieee802-dot1ab-lldp.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee1588-ptp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-netconf-monitoring.h"
#include "ietf-yang-library.h"
#include "excelfore-tsn-remote.h"
#include "excelfore-netconf-server.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *ieee802_dot1ab_lldp_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"lldp", 		// 2(0x2)
	"message-fast-tx", 		// 3(0x3)
	"message-tx-hold-multiplier", 		// 4(0x4)
	"message-tx-interval", 		// 5(0x5)
	"reinit-delay", 		// 6(0x6)
	"tx-credit-max", 		// 7(0x7)
	"tx-fast-init", 		// 8(0x8)
	"remote-statistics", 		// 9(0x9)
	"last-change-time", 		// 10(0xa)
	"remote-inserts", 		// 11(0xb)
	"remote-deletes", 		// 12(0xc)
	"remote-drops", 		// 13(0xd)
	"remote-ageouts", 		// 14(0xe)
	"local-system-data", 		// 15(0xf)
	"chassis-id-subtype", 		// 16(0x10)
	"chassis-id", 		// 17(0x11)
	"system-name", 		// 18(0x12)
	"system-description", 		// 19(0x13)
	"system-capabilities-supported", 		// 20(0x14)
	"system-capabilities-enabled", 		// 21(0x15)
	"port", 		// 22(0x16)
	"name", 		// 23(0x17)
	"dest-mac-address", 		// 24(0x18)
	"admin-status", 		// 25(0x19)
	"tlvs-tx-enable", 		// 26(0x1a)
	"management-address-tx-port", 		// 27(0x1b)
	"address-subtype", 		// 28(0x1c)
	"man-address", 		// 29(0x1d)
	"tx-enable", 		// 30(0x1e)
	"addr-len", 		// 31(0x1f)
	"if-subtype", 		// 32(0x20)
	"if-id", 		// 33(0x21)
	"port-id-subtype", 		// 34(0x22)
	"port-id", 		// 35(0x23)
	"port-desc", 		// 36(0x24)
	"tx-statistics", 		// 37(0x25)
	"total-frames", 		// 38(0x26)
	"total-length-errors", 		// 39(0x27)
	"rx-statistics", 		// 40(0x28)
	"total-ageouts", 		// 41(0x29)
	"total-discarded-frames", 		// 42(0x2a)
	"error-frames", 		// 43(0x2b)
	"total-discarded-tlvs", 		// 44(0x2c)
	"total-unrecognized-tlvs", 		// 45(0x2d)
	"remote-systems-data", 		// 46(0x2e)
	"time-mark", 		// 47(0x2f)
	"remote-index", 		// 48(0x30)
	"remote-too-many-neighbors", 		// 49(0x31)
	"remote-changes", 		// 50(0x32)
	"management-address", 		// 51(0x33)
	"address", 		// 52(0x34)
	"remote-unknown-tlv", 		// 53(0x35)
	"tlv-type", 		// 54(0x36)
	"tlv-info", 		// 55(0x37)
	"remote-org-defined-info", 		// 56(0x38)
	"info-identifier", 		// 57(0x39)
	"info-subtype", 		// 58(0x3a)
	"info-index", 		// 59(0x3b)
	"remote-info", 		// 60(0x3c)
};

uint8_t ieee802_dot1ab_lldp_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)IEEE802_DOT1AB_LLDP_ENUM_END;i++){
		if(!strcmp(astr, ieee802_dot1ab_lldp_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *ieee802_dot1ab_lldp_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)IEEE802_DOT1AB_LLDP_ENUM_END){return NULL;}
	return ieee802_dot1ab_lldp_enum_strings[anum];
}

int ieee802_dot1ab_lldp_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[9]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	//0000_ieee802-dot1ab-lldp/lldp/remote-statistics
	aps[0] = IEEE802_DOT1AB_LLDP_RO;
	aps[1] = IEEE802_DOT1AB_LLDP_LLDP;
	aps[2] = IEEE802_DOT1AB_LLDP_REMOTE_STATISTICS;
	aps[3] = IEEE802_DOT1AB_LLDP_LAST_CHANGE_TIME;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_REMOTE_INSERTS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_REMOTE_DELETES;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_REMOTE_DROPS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_REMOTE_AGEOUTS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0001_ieee802-dot1ab-lldp/lldp/local-system-data
	aps[2] = IEEE802_DOT1AB_LLDP_LOCAL_SYSTEM_DATA;
	aps[3] = IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE;
	vtype=YANG_VTYPE_IEEE_CHASSIS_ID_SUBTYPE_TYPE;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_CHASSIS_ID;
	vtype=YANG_VTYPE_IEEE_CHASSIS_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_SYSTEM_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED;
	vtype=YANG_VTYPE_LLDP_TYPES_SYSTEM_CAPABILITIES_MAP;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED;
	vtype=YANG_VTYPE_LLDP_TYPES_SYSTEM_CAPABILITIES_MAP;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0002_ieee802-dot1ab-lldp/lldp/port/management-address-tx-port
	aps[0] = IEEE802_DOT1AB_LLDP_RW;
	aps[2] = IEEE802_DOT1AB_LLDP_PORT;
	aps[3] = IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS_TX_PORT;
	aps[4] = IEEE802_DOT1AB_LLDP_TX_ENABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1AB_LLDP_RO;
	aps[4] = IEEE802_DOT1AB_LLDP_ADDR_LEN;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_IF_SUBTYPE;
	vtype=YANG_VTYPE_LLDP_TYPES_MAN_ADDR_IF_SUBTYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_IF_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1AB_LLDP_RW;
	aps[4] = IEEE802_DOT1AB_LLDP_VALUEKEY;
	aps[5] = IEEE802_DOT1AB_LLDP_ADDRESS_SUBTYPE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1AB_LLDP_MAN_ADDRESS;
	vtype=YANG_VTYPE_LLDP_TYPES_MAN_ADDR_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_VALUEKEY;
	aps[4] = IEEE802_DOT1AB_LLDP_NAME;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_DEST_MAC_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0003_ieee802-dot1ab-lldp/lldp/port/tx-statistics
	aps[0] = IEEE802_DOT1AB_LLDP_RO;
	aps[3] = IEEE802_DOT1AB_LLDP_TX_STATISTICS;
	aps[4] = IEEE802_DOT1AB_LLDP_TOTAL_FRAMES;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_TOTAL_LENGTH_ERRORS;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0004_ieee802-dot1ab-lldp/lldp/port/rx-statistics
	aps[3] = IEEE802_DOT1AB_LLDP_RX_STATISTICS;
	aps[4] = IEEE802_DOT1AB_LLDP_TOTAL_AGEOUTS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_FRAMES;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_ERROR_FRAMES;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_TOTAL_FRAMES;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_TLVS;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_TOTAL_UNRECOGNIZED_TLVS;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0005_ieee802-dot1ab-lldp/lldp/port/remote-systems-data/management-address
	aps[3] = IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA;
	aps[4] = IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS;
	aps[5] = IEEE802_DOT1AB_LLDP_IF_SUBTYPE;
	vtype=YANG_VTYPE_LLDP_TYPES_MAN_ADDR_IF_SUBTYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1AB_LLDP_IF_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1AB_LLDP_VALUEKEY;
	aps[6] = IEEE802_DOT1AB_LLDP_ADDRESS_SUBTYPE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1AB_LLDP_ADDRESS;
	vtype=YANG_VTYPE_LLDP_TYPES_MAN_ADDR_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_VALUEKEY;
	aps[5] = IEEE802_DOT1AB_LLDP_TIME_MARK;
	vtype=YANG_VTYPE_YANG_TIMETICKS;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1AB_LLDP_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0006_ieee802-dot1ab-lldp/lldp/port/remote-systems-data/remote-unknown-tlv
	aps[4] = IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV;
	aps[5] = IEEE802_DOT1AB_LLDP_TLV_INFO;
	vtype=YANG_VTYPE_BINARY;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1AB_LLDP_VALUEKEY;
	aps[6] = IEEE802_DOT1AB_LLDP_TLV_TYPE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0007_ieee802-dot1ab-lldp/lldp/port/remote-systems-data/remote-org-defined-info
	aps[4] = IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO;
	aps[5] = IEEE802_DOT1AB_LLDP_REMOTE_INFO;
	vtype=YANG_VTYPE_BINARY;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1AB_LLDP_VALUEKEY;
	aps[6] = IEEE802_DOT1AB_LLDP_INFO_IDENTIFIER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1AB_LLDP_INFO_SUBTYPE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1AB_LLDP_INFO_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0008_ieee802-dot1ab-lldp/lldp/port/remote-systems-data
	aps[4] = IEEE802_DOT1AB_LLDP_REMOTE_TOO_MANY_NEIGHBORS;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_REMOTE_CHANGES;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE;
	vtype=YANG_VTYPE_IEEE_CHASSIS_ID_SUBTYPE_TYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_CHASSIS_ID;
	vtype=YANG_VTYPE_IEEE_CHASSIS_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE;
	vtype=YANG_VTYPE_IEEE_PORT_ID_SUBTYPE_TYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_PORT_ID;
	vtype=YANG_VTYPE_IEEE_PORT_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_PORT_DESC;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_SYSTEM_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED;
	vtype=YANG_VTYPE_LLDP_TYPES_SYSTEM_CAPABILITIES_MAP;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED;
	vtype=YANG_VTYPE_LLDP_TYPES_SYSTEM_CAPABILITIES_MAP;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0009_ieee802-dot1ab-lldp/lldp/port
	aps[0] = IEEE802_DOT1AB_LLDP_RW;
	aps[3] = IEEE802_DOT1AB_LLDP_ADMIN_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_TLVS_TX_ENABLE;
	vtype=YANG_VTYPE_BITS;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_REINIT_DELAY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_TX_FAST_INIT;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1AB_LLDP_RO;
	aps[3] = IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE;
	vtype=YANG_VTYPE_IEEE_PORT_ID_SUBTYPE_TYPE;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_PORT_ID;
	vtype=YANG_VTYPE_IEEE_PORT_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1AB_LLDP_PORT_DESC;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0010_ieee802-dot1ab-lldp/lldp
	aps[0] = IEEE802_DOT1AB_LLDP_RW;
	aps[2] = IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[2] = IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[2] = IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[2] = IEEE802_DOT1AB_LLDP_REINIT_DELAY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[2] = IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[2] = IEEE802_DOT1AB_LLDP_TX_FAST_INIT;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1AB_LLDP_RO;
	aps[1] = IEEE802_DOT1AB_LLDP_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
