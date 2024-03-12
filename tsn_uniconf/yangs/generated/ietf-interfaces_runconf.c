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
#include "ietf-interfaces.h"

extern uint8_t IETF_INTERFACES_func(uc_dbald *dbald);
#define IETF_INTERFACES_RW IETF_INTERFACES_func(dbald)
#define IETF_INTERFACES_RO (IETF_INTERFACES_func(dbald)|0x80u)

const char *ietf_interfaces_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"interfaces", 		// 3(0x3)
	"interface", 		// 4(0x4)
	"name", 		// 5(0x5)
	"enabled", 		// 6(0x6)
	"oper-status", 		// 7(0x7)
	"if-index", 		// 8(0x8)
	"phys-address", 		// 9(0x9)
	"speed", 		// 10(0xa)
	// augmented by ieee802-dot1q-bridge
	"bridge-port",		// 11(0xb)
	"bridge-name",		// 12(0xc)
	"component-name",		// 13(0xd)
	"pvid",		// 14(0xe)
	"default-priority",		// 15(0xf)
	"priority-regeneration",		// 16(0x10)
	"priority0",		// 17(0x11)
	"priority1",		// 18(0x12)
	"priority2",		// 19(0x13)
	"priority3",		// 20(0x14)
	"priority4",		// 21(0x15)
	"priority5",		// 22(0x16)
	"priority6",		// 23(0x17)
	"priority7",		// 24(0x18)
	"traffic-class",		// 25(0x19)
	"traffic-class-table",		// 26(0x1a)
	"number-of-traffic-classes",		// 27(0x1b)
	"transmission-selection-algorithm-table",		// 28(0x1c)
	"transmission-selection-algorithm-map",		// 29(0x1d)
	"transmission-selection-algorithm",		// 30(0x1e)
	"address",		// 31(0x1f)
	// augmented by xl4interfaces
	"tc-data",		// 32(0x20)
	"tc",		// 33(0x21)
	"lqueue",		// 34(0x22)
	"admin-idleslope",		// 35(0x23)
	"oper-idleslope",		// 36(0x24)
	"max-interference-size",		// 37(0x25)
	"max-frame-size",		// 38(0x26)
	"cbs-enabled",		// 39(0x27)
	"number-of-pqueues",		// 40(0x28)
	"pqueue-map",		// 41(0x29)
	"pqueue",		// 42(0x2a)
	"duplex",		// 43(0x2b)
	"discon-workaround-time",		// 44(0x2c)
	// augmented by ieee802-dot1q-sched-bridge
	"gate-parameter-table",		// 45(0x2d)
	"queue-max-sdu-table",		// 46(0x2e)
	"queue-max-sdu",		// 47(0x2f)
	"transmission-overrun",		// 48(0x30)
	"gate-enabled",		// 49(0x31)
	"admin-gate-states",		// 50(0x32)
	"oper-gate-states",		// 51(0x33)
	"admin-control-list",		// 52(0x34)
	"gate-control-entry",		// 53(0x35)
	"index",		// 54(0x36)
	"operation-name",		// 55(0x37)
	"time-interval-value",		// 56(0x38)
	"gate-states-value",		// 57(0x39)
	"oper-control-list",		// 58(0x3a)
	"admin-cycle-time",		// 59(0x3b)
	"numerator",		// 60(0x3c)
	"denominator",		// 61(0x3d)
	"oper-cycle-time",		// 62(0x3e)
	"admin-cycle-time-extension",		// 63(0x3f)
	"oper-cycle-time-extension",		// 64(0x40)
	"admin-base-time",		// 65(0x41)
	"seconds",		// 66(0x42)
	"nanoseconds",		// 67(0x43)
	"oper-base-time",		// 68(0x44)
	"config-change",		// 69(0x45)
	"config-change-time",		// 70(0x46)
	"tick-granularity",		// 71(0x47)
	"current-time",		// 72(0x48)
	"config-pending",		// 73(0x49)
	"config-change-error",		// 74(0x4a)
	"supported-list-max",		// 75(0x4b)
	"supported-cycle-max",		// 76(0x4c)
	"supported-interval-max",		// 77(0x4d)
	// augmented by ieee802-dot1q-preemption-bridge
	"frame-preemption-parameters",		// 78(0x4e)
	"frame-preemption-status-table",		// 79(0x4f)
	"hold-advance",		// 80(0x50)
	"release-advance",		// 81(0x51)
	"preemption-active",		// 82(0x52)
	"hold-request",		// 83(0x53)
};
const uint8_t ietf_interfaces_enum_max=84;

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "preempt-bridge",
		"urn:ieee:std:802.1Q:yang:ieee802-dot1q-preemption-bridge")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "dot1q",
		"urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "xl4if",
		"http://excelfore.com/ns/xl4interfaces")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "if",
		"urn:ietf:params:xml:ns:yang:ietf-interfaces")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "sched-bridge",
		"urn:ieee:std:802.1Q:yang:ieee802-dot1q-sched-bridge")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[10];
	apsd[0]=IETF_INTERFACES_RW;
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"if", 3)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_BRIDGE_PORT;
	apsd[4]=IETF_INTERFACES_NSASC;
	apsd[5]=255u;
	if(uc_dbal_create(dbald, apsd, 6, (void*)"dot1q", 6)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_BRIDGE_PORT;
	apsd[4]=IETF_INTERFACES_TRAFFIC_CLASS;
	apsd[5]=IETF_INTERFACES_TC_DATA;
	apsd[6]=IETF_INTERFACES_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"xl4if", 6)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_BRIDGE_PORT;
	apsd[4]=IETF_INTERFACES_TRAFFIC_CLASS;
	apsd[5]=IETF_INTERFACES_CBS_ENABLED;
	apsd[6]=IETF_INTERFACES_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"xl4if", 6)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_BRIDGE_PORT;
	apsd[4]=IETF_INTERFACES_TRAFFIC_CLASS;
	apsd[5]=IETF_INTERFACES_NUMBER_OF_PQUEUES;
	apsd[6]=IETF_INTERFACES_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"xl4if", 6)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_BRIDGE_PORT;
	apsd[4]=IETF_INTERFACES_TRAFFIC_CLASS;
	apsd[5]=IETF_INTERFACES_PQUEUE_MAP;
	apsd[6]=IETF_INTERFACES_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"xl4if", 6)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_BRIDGE_PORT;
	apsd[4]=IETF_INTERFACES_GATE_PARAMETER_TABLE;
	apsd[5]=IETF_INTERFACES_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"sched-bridge", 13)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_BRIDGE_PORT;
	apsd[4]=IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS;
	apsd[5]=IETF_INTERFACES_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"preempt-bridge", 15)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_DUPLEX;
	apsd[4]=IETF_INTERFACES_NSASC;
	apsd[5]=255u;
	if(uc_dbal_create(dbald, apsd, 6, (void*)"xl4if", 6)!=0){
		return -1;
	}
	apsd[1]=IETF_INTERFACES_INTERFACES;
	apsd[2]=IETF_INTERFACES_INTERFACE;
	apsd[3]=IETF_INTERFACES_DISCON_WORKAROUND_TIME;
	apsd[4]=IETF_INTERFACES_NSASC;
	apsd[5]=255u;
	if(uc_dbal_create(dbald, apsd, 6, (void*)"xl4if", 6)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int ietf_interfaces_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[11]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_ietf-interfaces/interfaces/interface/bridge-port/priority-regeneration
	aps[0] = IETF_INTERFACES_RW;
	aps[1] = IETF_INTERFACES_INTERFACES;
	aps[2] = IETF_INTERFACES_INTERFACE;
	aps[3] = IETF_INTERFACES_BRIDGE_PORT;
	aps[4] = IETF_INTERFACES_PRIORITY_REGENERATION;
	aps[5] = IETF_INTERFACES_PRIORITY0;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PRIORITY1;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PRIORITY2;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PRIORITY3;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PRIORITY4;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PRIORITY5;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PRIORITY6;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PRIORITY7;
	vtype=YANG_VTYPE_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_INTERFACES_VALUEKEY;
	aps[4] = IETF_INTERFACES_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0001_ietf-interfaces/interfaces/interface/bridge-port/traffic-class/traffic-class-table
	aps[3] = IETF_INTERFACES_BRIDGE_PORT;
	aps[4] = IETF_INTERFACES_TRAFFIC_CLASS;
	aps[5] = IETF_INTERFACES_TRAFFIC_CLASS_TABLE;
	aps[6] = IETF_INTERFACES_NUMBER_OF_TRAFFIC_CLASSES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY0;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY1;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY2;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY3;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY4;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY5;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY6;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY7;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0002_ietf-interfaces/interfaces/interface/bridge-port/traffic-class/tc-data
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_TC_DATA;
	aps[6] = IETF_INTERFACES_LQUEUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RW;
	aps[6] = IETF_INTERFACES_ADMIN_IDLESLOPE;
	vtype=YANG_VTYPE_INT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[6] = IETF_INTERFACES_OPER_IDLESLOPE;
	vtype=YANG_VTYPE_INT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_MAX_INTERFERENCE_SIZE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RW;
	aps[6] = IETF_INTERFACES_VALUEKEY;
	aps[7] = IETF_INTERFACES_TC;
	vtype=YANG_VTYPE_DOT1QTYPES_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0003_ietf-interfaces/interfaces/interface/bridge-port/traffic-class/pqueue-map
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_PQUEUE_MAP;
	aps[6] = IETF_INTERFACES_LQUEUE;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_VALUEKEY;
	aps[7] = IETF_INTERFACES_PQUEUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0004_ietf-interfaces/interfaces/interface/bridge-port/traffic-class
	aps[0] = IETF_INTERFACES_RW;
	aps[5] = IETF_INTERFACES_CBS_ENABLED;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_NUMBER_OF_PQUEUES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0005_ietf-interfaces/interfaces/interface/bridge-port/transmission-selection-algorithm-table/transmission-selection-algorithm-map
	aps[0] = IETF_INTERFACES_RW;
	aps[4] = IETF_INTERFACES_TRANSMISSION_SELECTION_ALGORITHM_TABLE;
	aps[5] = IETF_INTERFACES_TRANSMISSION_SELECTION_ALGORITHM_MAP;
	aps[6] = IETF_INTERFACES_TRANSMISSION_SELECTION_ALGORITHM;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_VALUEKEY;
	aps[7] = IETF_INTERFACES_TRAFFIC_CLASS;
	vtype=YANG_VTYPE_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0006_ietf-interfaces/interfaces/interface/bridge-port/address
	//0007_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/queue-max-sdu-table
	aps[4] = IETF_INTERFACES_GATE_PARAMETER_TABLE;
	aps[5] = IETF_INTERFACES_QUEUE_MAX_SDU_TABLE;
	aps[6] = IETF_INTERFACES_QUEUE_MAX_SDU;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[6] = IETF_INTERFACES_TRANSMISSION_OVERRUN;
	vtype=YANG_VTYPE_YANG_COUNTER64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RW;
	aps[6] = IETF_INTERFACES_VALUEKEY;
	aps[7] = IETF_INTERFACES_TRAFFIC_CLASS;
	vtype=YANG_VTYPE_DOT1Q_TYPES_TRAFFIC_CLASS_TYPE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0008_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-control-list/gate-control-entry
	aps[5] = IETF_INTERFACES_ADMIN_CONTROL_LIST;
	aps[6] = IETF_INTERFACES_GATE_CONTROL_ENTRY;
	aps[7] = IETF_INTERFACES_OPERATION_NAME;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IETF_INTERFACES_TIME_INTERVAL_VALUE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IETF_INTERFACES_GATE_STATES_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IETF_INTERFACES_VALUEKEY;
	aps[8] = IETF_INTERFACES_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0009_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-control-list
	//0010_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-control-list/gate-control-entry
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_OPER_CONTROL_LIST;
	aps[7] = IETF_INTERFACES_OPERATION_NAME;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IETF_INTERFACES_TIME_INTERVAL_VALUE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IETF_INTERFACES_GATE_STATES_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IETF_INTERFACES_VALUEKEY;
	aps[8] = IETF_INTERFACES_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0011_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-cycle-time
	//0012_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-cycle-time
	aps[0] = IETF_INTERFACES_RW;
	aps[5] = IETF_INTERFACES_ADMIN_CYCLE_TIME;
	aps[6] = IETF_INTERFACES_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0013_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-cycle-time
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_OPER_CYCLE_TIME;
	aps[6] = IETF_INTERFACES_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0014_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-base-time
	aps[0] = IETF_INTERFACES_RW;
	aps[5] = IETF_INTERFACES_ADMIN_BASE_TIME;
	aps[6] = IETF_INTERFACES_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0015_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-base-time
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_OPER_BASE_TIME;
	aps[6] = IETF_INTERFACES_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0016_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/config-change-time
	aps[5] = IETF_INTERFACES_CONFIG_CHANGE_TIME;
	aps[6] = IETF_INTERFACES_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0017_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/current-time
	aps[5] = IETF_INTERFACES_CURRENT_TIME;
	aps[6] = IETF_INTERFACES_SECONDS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_NANOSECONDS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0018_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/supported-cycle-max
	aps[0] = IETF_INTERFACES_RW;
	aps[5] = IETF_INTERFACES_SUPPORTED_CYCLE_MAX;
	aps[6] = IETF_INTERFACES_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0019_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table
	aps[5] = IETF_INTERFACES_GATE_ENABLED;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_ADMIN_GATE_STATES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_OPER_GATE_STATES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RW;
	aps[5] = IETF_INTERFACES_ADMIN_CYCLE_TIME_EXTENSION;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_OPER_CYCLE_TIME_EXTENSION;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RW;
	aps[5] = IETF_INTERFACES_CONFIG_CHANGE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_TICK_GRANULARITY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_CONFIG_PENDING;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_CONFIG_CHANGE_ERROR;
	vtype=YANG_VTYPE_YANG_COUNTER64;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RW;
	aps[5] = IETF_INTERFACES_SUPPORTED_LIST_MAX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_SUPPORTED_INTERVAL_MAX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0020_ietf-interfaces/interfaces/interface/bridge-port/frame-preemption-parameters/frame-preemption-status-table
	aps[4] = IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS;
	aps[5] = IETF_INTERFACES_FRAME_PREEMPTION_STATUS_TABLE;
	aps[6] = IETF_INTERFACES_PRIORITY0;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY1;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY2;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY3;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY4;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY5;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY6;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_INTERFACES_PRIORITY7;
	vtype=YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0021_ietf-interfaces/interfaces/interface/bridge-port/frame-preemption-parameters
	aps[0] = IETF_INTERFACES_RO;
	aps[5] = IETF_INTERFACES_HOLD_ADVANCE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_RELEASE_ADVANCE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_PREEMPTION_ACTIVE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_INTERFACES_HOLD_REQUEST;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0022_ietf-interfaces/interfaces/interface/bridge-port
	aps[0] = IETF_INTERFACES_RW;
	aps[4] = IETF_INTERFACES_BRIDGE_NAME;
	vtype=YANG_VTYPE_LEAFREF;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_INTERFACES_COMPONENT_NAME;
	vtype=YANG_VTYPE_LEAFREF;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_INTERFACES_PVID;
	vtype=YANG_VTYPE_DOT1QTYPES_VLAN_INDEX_TYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_INTERFACES_DEFAULT_PRIORITY;
	vtype=YANG_VTYPE_DOT1QTYPES_PRIORITY_TYPE;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[4] = IETF_INTERFACES_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0023_ietf-interfaces/interfaces/interface
	aps[0] = IETF_INTERFACES_RW;
	aps[3] = IETF_INTERFACES_ENABLED;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[3] = IETF_INTERFACES_OPER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_INTERFACES_IF_INDEX;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_INTERFACES_PHYS_ADDRESS;
	vtype=YANG_VTYPE_YANG_PHYS_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_INTERFACES_SPEED;
	vtype=YANG_VTYPE_YANG_GAUGE64;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_INTERFACES_DUPLEX;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RW;
	aps[3] = IETF_INTERFACES_DISCON_WORKAROUND_TIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_INTERFACES_RO;
	aps[1] = IETF_INTERFACES_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

