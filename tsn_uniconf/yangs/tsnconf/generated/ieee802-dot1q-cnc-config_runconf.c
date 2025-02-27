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
#include "../../yang_modules.h"
#include "../../yang_db_access.h"
#include "../../yang_node.h"
#include "ieee802-dot1q-cnc-config.h"

extern uint8_t IEEE802_DOT1Q_CNC_CONFIG_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_CNC_CONFIG_RW IEEE802_DOT1Q_CNC_CONFIG_func(dbald)
#define IEEE802_DOT1Q_CNC_CONFIG_RO (IEEE802_DOT1Q_CNC_CONFIG_func(dbald)|0x80u)

const char *ieee802_dot1q_cnc_config_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"cnc-config", 		// 3(0x3)
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
	// augmented by xl4-ieee802-dot1q-cnc-config
	"accept",		// 55(0x37)
	"status",		// 56(0x38)
	"station-name",		// 57(0x39)
	"update-accept",		// 58(0x3a)
	"frer-enabled",		// 59(0x3b)
	"eventflags",		// 60(0x3c)
	"accept-policy",		// 61(0x3d)
	"accept-connectlist",		// 62(0x3e)
	"cbs-policy",		// 63(0x3f)
	"cbs-uselist",		// 64(0x40)
	"frer-policy",		// 65(0x41)
	"frer-uselist",		// 66(0x42)
	"tas-sched-list",		// 67(0x43)
	"streamid",		// 68(0x44)
	"bridge-sched",		// 69(0x45)
	"bridge-name",		// 70(0x46)
	"bridge-port",		// 71(0x47)
	"gate-parameter-table",		// 72(0x48)
	"queue-max-sdu-table",		// 73(0x49)
	"traffic-class",		// 74(0x4a)
	"queue-max-sdu",		// 75(0x4b)
	"transmission-overrun",		// 76(0x4c)
	"gate-enabled",		// 77(0x4d)
	"admin-gate-states",		// 78(0x4e)
	"oper-gate-states",		// 79(0x4f)
	"admin-control-list",		// 80(0x50)
	"gate-control-entry",		// 81(0x51)
	"operation-name",		// 82(0x52)
	"time-interval-value",		// 83(0x53)
	"gate-states-value",		// 84(0x54)
	"oper-control-list",		// 85(0x55)
	"admin-cycle-time",		// 86(0x56)
	"oper-cycle-time",		// 87(0x57)
	"admin-cycle-time-extension",		// 88(0x58)
	"oper-cycle-time-extension",		// 89(0x59)
	"admin-base-time",		// 90(0x5a)
	"seconds",		// 91(0x5b)
	"nanoseconds",		// 92(0x5c)
	"oper-base-time",		// 93(0x5d)
	"config-change",		// 94(0x5e)
	"config-change-time",		// 95(0x5f)
	"tick-granularity",		// 96(0x60)
	"current-time",		// 97(0x61)
	"config-pending",		// 98(0x62)
	"config-change-error",		// 99(0x63)
	"supported-list-max",		// 100(0x64)
	"supported-cycle-max",		// 101(0x65)
	"supported-interval-max",		// 102(0x66)
	"latency-parameters",		// 103(0x67)
	"internal-processing",		// 104(0x68)
	"wire-propagation",		// 105(0x69)
	"media-access-delay",		// 106(0x6a)
};
const uint8_t ieee802_dot1q_cnc_config_enum_max=107;

#ifdef GENERATE_INITCONFIG

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "dot1q-cnc-config",
		"urn:ieee:std:802.1Q:yang:ieee802-dot1q-cnc-config")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "xl4cnc",
		"http://excelfore.com/ns/xl4-ieee802-dot1q-cnc-config")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[13];
	apsd[0]=IEEE802_DOT1Q_CNC_CONFIG_RW;
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"dot1q-cnc-config", 17)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCEPT;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_STATUS;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_UPDATE_ACCEPT;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCEPT;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_STATUS;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	apsd[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	apsd[8]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_FRER_ENABLED;
	apsd[6]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_EVENTFLAGS;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_ACCEPT_POLICY;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_ACCEPT_CONNECTLIST;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_CBS_POLICY;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_CBS_USELIST;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_FRER_POLICY;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_FRER_USELIST;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	apsd[2]=IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	apsd[3]=IEEE802_DOT1Q_CNC_CONFIG_CUC;
	apsd[4]=IEEE802_DOT1Q_CNC_CONFIG_LATENCY_PARAMETERS;
	apsd[5]=IEEE802_DOT1Q_CNC_CONFIG_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4cnc", 7)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int ieee802_dot1q_cnc_config_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[14]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/stream-rank
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[1] = IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	aps[2] = IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	aps[3] = IEEE802_DOT1Q_CNC_CONFIG_CUC;
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_STREAM_RANK;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_STREAM_ID;
	vtype=YANG_VTYPE_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_CUC_ID;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_DOMAIN_ID;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0001_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/end-station-interfaces
	aps[3] = IEEE802_DOT1Q_CNC_CONFIG_CUC;
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_ACCEPT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_STATUS;
	vtype=YANG_VTYPE_END_STATION_STATUS;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_UPDATE_ACCEPT;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0002_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/data-frame-specification/ieee802-mac-addresses
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_DATA_FRAME_SPECIFICATION;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_IEEE802_MAC_ADDRESSES;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0003_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/data-frame-specification/ieee802-vlan-tag
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_IEEE802_VLAN_TAG;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0004_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/data-frame-specification
	//0005_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/traffic-specification/interval
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_TRAFFIC_SPECIFICATION;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_INTERVAL;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0006_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/traffic-specification/time-aware
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_TIME_AWARE;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_EARLIEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_LATEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_JITTER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0007_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/traffic-specification
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0008_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/user-to-network-requirements
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_USER_TO_NETWORK_REQUIREMENTS;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0009_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/interface-capabilities
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CAPABILITIES;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0010_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CONFIGURATION;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_LIST;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_IEEE802_MAC_ADDRESSES;
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0011_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_IEEE802_VLAN_TAG;
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0012_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/interface-configuration/interface-list/config-list
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0013_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/interface-configuration/interface-list
	//0014_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker/interface-configuration
	//0015_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/talker
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0016_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/end-station-interfaces
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_ACCEPT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_STATUS;
	vtype=YANG_VTYPE_END_STATION_STATUS;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0017_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/user-to-network-requirements
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_USER_TO_NETWORK_REQUIREMENTS;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0018_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/interface-capabilities
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CAPABILITIES;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0019_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CONFIGURATION;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_LIST;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_IEEE802_MAC_ADDRESSES;
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0020_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_IEEE802_VLAN_TAG;
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = IEEE802_DOT1Q_CNC_CONFIG_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0021_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/interface-configuration/interface-list/config-list
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0022_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/interface-configuration/interface-list
	//0023_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener/interface-configuration
	//0024_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/listener
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0025_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/status-info
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_STATUS_INFO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0026_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream/failed-interfaces
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_FAILED_INTERFACES;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0027_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/stream
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_STREAM_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_FRER_ENABLED;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0028_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/bridge-sched
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_BRIDGE_SCHED;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_BRIDGE_PORT;
	vtype=YANG_VTYPE_STRING_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_BRIDGE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0029_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/queue-max-sdu-table
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_GATE_PARAMETER_TABLE;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_QUEUE_MAX_SDU_TABLE;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_QUEUE_MAX_SDU;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_TRANSMISSION_OVERRUN;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_TRAFFIC_CLASS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0030_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/admin-control-list/gate-control-entry
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CONTROL_LIST;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_GATE_CONTROL_ENTRY;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_OPERATION_NAME;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_TIME_INTERVAL_VALUE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_GATE_STATES_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0031_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/admin-control-list
	//0032_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/oper-control-list/gate-control-entry
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_OPER_CONTROL_LIST;
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_OPERATION_NAME;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_TIME_INTERVAL_VALUE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_GATE_STATES_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	aps[9] = IEEE802_DOT1Q_CNC_CONFIG_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0033_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/oper-control-list
	//0034_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/admin-cycle-time
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CYCLE_TIME;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0035_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/oper-cycle-time
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_OPER_CYCLE_TIME;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0036_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/admin-base-time
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_BASE_TIME;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0037_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/oper-base-time
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_OPER_BASE_TIME;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0038_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/config-change-time
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_CHANGE_TIME;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0039_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/current-time
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_CURRENT_TIME;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0040_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table/supported-cycle-max
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_SUPPORTED_CYCLE_MAX;
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE802_DOT1Q_CNC_CONFIG_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0041_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list/gate-parameter-table
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_GATE_ENABLED;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_GATE_STATES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_OPER_GATE_STATES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CYCLE_TIME_EXTENSION;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_OPER_CYCLE_TIME_EXTENSION;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_CHANGE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_TICK_GRANULARITY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_PENDING;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_CHANGE_ERROR;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RW;
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_SUPPORTED_LIST_MAX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE802_DOT1Q_CNC_CONFIG_SUPPORTED_INTERVAL_MAX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0042_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/tas-sched-list
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_STREAMID;
	vtype=YANG_VTYPE_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0043_ieee802-dot1q-cnc-config/cnc-config/domain/cuc/latency-parameters
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_LATENCY_PARAMETERS;
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_INTERNAL_PROCESSING;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_WIRE_PROPAGATION;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE802_DOT1Q_CNC_CONFIG_MEDIA_ACCESS_DELAY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0044_ieee802-dot1q-cnc-config/cnc-config/domain/cuc
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_EVENTFLAGS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_ACCEPT_POLICY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_ACCEPT_CONNECTLIST;
	vtype=YANG_VTYPE_STREAM_ID_TYPE_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_CBS_POLICY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_CBS_USELIST;
	vtype=YANG_VTYPE_STREAM_ID_TYPE_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_FRER_POLICY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE802_DOT1Q_CNC_CONFIG_FRER_USELIST;
	vtype=YANG_VTYPE_STREAM_ID_TYPE_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0045_ieee802-dot1q-cnc-config/cnc-config/domain
	aps[3] = IEEE802_DOT1Q_CNC_CONFIG_CNC_ENABLED;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1Q_CNC_CONFIG_RO;
	aps[1] = IEEE802_DOT1Q_CNC_CONFIG_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

#endif
