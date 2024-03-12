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
#include "excelfore-tsn-remote.h"

extern uint8_t EXCELFORE_TSN_REMOTE_func(uc_dbald *dbald);
#define EXCELFORE_TSN_REMOTE_RW EXCELFORE_TSN_REMOTE_func(dbald)
#define EXCELFORE_TSN_REMOTE_RO (EXCELFORE_TSN_REMOTE_func(dbald)|0x80u)

const char *excelfore_tsn_remote_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"tsn-remote", 		// 3(0x3)
	"instances", 		// 4(0x4)
	"instance", 		// 5(0x5)
	"instance-index", 		// 6(0x6)
	"update", 		// 7(0x7)
	"port", 		// 8(0x8)
	"protocol", 		// 9(0x9)
	"request-status", 		// 10(0xa)
	"result-status", 		// 11(0xb)
	"external-control-man", 		// 12(0xc)
	"control-objects", 		// 13(0xd)
	"external-control", 		// 14(0xe)
	"indication-change-counter", 		// 15(0xf)
	"msrp-ta", 		// 16(0x10)
	"indication-list", 		// 17(0x11)
	"stream-id", 		// 18(0x12)
	"dest-address", 		// 19(0x13)
	"vlan-id", 		// 20(0x14)
	"max-frame-size", 		// 21(0x15)
	"max-interval-frames", 		// 22(0x16)
	"pcp", 		// 23(0x17)
	"rank", 		// 24(0x18)
	"accumulated-latency", 		// 25(0x19)
	"admin-request-list", 		// 26(0x1a)
	"oper-request-list", 		// 27(0x1b)
	"msrp-tf", 		// 28(0x1c)
	"system-id", 		// 29(0x1d)
	"failuer-code", 		// 30(0x1e)
	"msrp-te", 		// 31(0x1f)
	"stream-rank", 		// 32(0x20)
	"end-station-interfaces", 		// 33(0x21)
	"mac-address", 		// 34(0x22)
	"interface-name", 		// 35(0x23)
	"data-frame-specification", 		// 36(0x24)
	"index", 		// 37(0x25)
	"ieee802-mac-addresses", 		// 38(0x26)
	"destination-mac-address", 		// 39(0x27)
	"source-mac-address", 		// 40(0x28)
	"ieee802-vlan-tag", 		// 41(0x29)
	"priority-code-point", 		// 42(0x2a)
	"ipv4-tuple", 		// 43(0x2b)
	"source-ip-address", 		// 44(0x2c)
	"destination-ip-address", 		// 45(0x2d)
	"dscp", 		// 46(0x2e)
	"source-port", 		// 47(0x2f)
	"destination-port", 		// 48(0x30)
	"ipv6-tuple", 		// 49(0x31)
	"traffic-specification", 		// 50(0x32)
	"interval", 		// 51(0x33)
	"numerator", 		// 52(0x34)
	"denominator", 		// 53(0x35)
	"max-frames-per-interval", 		// 54(0x36)
	"transmission-selection", 		// 55(0x37)
	"time-aware", 		// 56(0x38)
	"earliest-transmit-offset", 		// 57(0x39)
	"latest-transmit-offset", 		// 58(0x3a)
	"jitter", 		// 59(0x3b)
	"user-to-network-requirements", 		// 60(0x3c)
	"num-seamless-trees", 		// 61(0x3d)
	"max-latency", 		// 62(0x3e)
	"interface-capabilities", 		// 63(0x3f)
	"vlan-tag-capable", 		// 64(0x40)
	"cb-stream-iden-type-list", 		// 65(0x41)
	"cb-sequence-type-list", 		// 66(0x42)
	"interface-configuration", 		// 67(0x43)
	"interface-list", 		// 68(0x44)
	"config-list", 		// 69(0x45)
	"time-aware-offset", 		// 70(0x46)
	"status-info", 		// 71(0x47)
	"talker-status", 		// 72(0x48)
	"listener-status", 		// 73(0x49)
	"failure-code", 		// 74(0x4a)
	"failed-interfaces", 		// 75(0x4b)
	"msrp-ls", 		// 76(0x4c)
	"four_packed_event", 		// 77(0x4d)
	"msrp-le", 		// 78(0x4e)
	"msrp-dm", 		// 79(0x4f)
	"srclass-id", 		// 80(0x50)
	"srclass-priority", 		// 81(0x51)
	"srclass-vid", 		// 82(0x52)
	"mvrp", 		// 83(0x53)
};
const uint8_t excelfore_tsn_remote_enum_max=84;

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "xl4tsnrmt",
		"http://excelfore.com/ns/excelfore-tsn-remote")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[15];
	apsd[0]=EXCELFORE_TSN_REMOTE_RW;
	apsd[1]=EXCELFORE_TSN_REMOTE_TSN_REMOTE;
	apsd[2]=EXCELFORE_TSN_REMOTE_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"xl4tsnrmt", 10)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int excelfore_tsn_remote_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[16]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_excelfore-tsn-remote/tsn-remote/instances/instance/update
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[1] = EXCELFORE_TSN_REMOTE_TSN_REMOTE;
	aps[2] = EXCELFORE_TSN_REMOTE_INSTANCES;
	aps[3] = EXCELFORE_TSN_REMOTE_INSTANCE;
	aps[4] = EXCELFORE_TSN_REMOTE_UPDATE;
	aps[5] = EXCELFORE_TSN_REMOTE_PORT;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_MRP_PROTOCOL;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = EXCELFORE_TSN_REMOTE_REQUEST_STATUS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[5] = EXCELFORE_TSN_REMOTE_RESULT_STATUS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[4] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[5] = EXCELFORE_TSN_REMOTE_INSTANCE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0001_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ta/indication-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[4] = EXCELFORE_TSN_REMOTE_EXTERNAL_CONTROL_MAN;
	aps[5] = EXCELFORE_TSN_REMOTE_CONTROL_OBJECTS;
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_TA;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DEST_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_INTERVAL_FRAMES;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_PCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[6] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[7] = EXCELFORE_TSN_REMOTE_PORT;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_MRP_PROTOCOL;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0002_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ta/admin-request-list
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_TA;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DEST_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_INTERVAL_FRAMES;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_PCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0003_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ta/oper-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DEST_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_INTERVAL_FRAMES;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_PCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0004_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf
	//0005_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf/indication-list
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_TF;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DEST_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_INTERVAL_FRAMES;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_PCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_SYSTEM_ID;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_FAILUER_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0006_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf/admin-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DEST_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_INTERVAL_FRAMES;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_PCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_SYSTEM_ID;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_FAILUER_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0007_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf/oper-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DEST_ADDRESS;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_MAX_INTERVAL_FRAMES;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_PCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_SYSTEM_ID;
	vtype=YANG_VTYPE_IEEE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_FAILUER_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0008_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te
	//0009_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/stream-rank
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_TE;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_STREAM_RANK;
	aps[9] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0010_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/end-station-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_END_STATION_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0011_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_DATA_FRAME_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0012_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification/ieee802-vlan-tag
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[10] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0013_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification/ipv4-tuple
	aps[9] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0014_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification/ipv6-tuple
	aps[9] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0015_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification
	//0016_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification/interval
	aps[8] = EXCELFORE_TSN_REMOTE_TRAFFIC_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERVAL;
	aps[10] = EXCELFORE_TSN_REMOTE_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0017_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification/time-aware
	aps[9] = EXCELFORE_TSN_REMOTE_TIME_AWARE;
	aps[10] = EXCELFORE_TSN_REMOTE_EARLIEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_LATEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_JITTER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0018_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0019_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0020_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-capabilities
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CAPABILITIES;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0021_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CONFIGURATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERFACE_LIST;
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[12] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[11] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0022_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0023_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ipv4-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0024_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ipv6-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0025_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0026_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list
	//0027_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/status-info
	//0028_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/status-info
	aps[8] = EXCELFORE_TSN_REMOTE_STATUS_INFO;
	aps[9] = EXCELFORE_TSN_REMOTE_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0029_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/failed-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_FAILED_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0030_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0031_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/stream-rank
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_STREAM_RANK;
	aps[9] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0032_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/end-station-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_END_STATION_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0033_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_DATA_FRAME_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0034_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ieee802-vlan-tag
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[10] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0035_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ipv4-tuple
	aps[9] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0036_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ipv6-tuple
	aps[9] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0037_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification
	//0038_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification/interval
	aps[8] = EXCELFORE_TSN_REMOTE_TRAFFIC_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERVAL;
	aps[10] = EXCELFORE_TSN_REMOTE_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0039_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification/time-aware
	aps[9] = EXCELFORE_TSN_REMOTE_TIME_AWARE;
	aps[10] = EXCELFORE_TSN_REMOTE_EARLIEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_LATEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_JITTER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0040_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0041_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0042_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-capabilities
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CAPABILITIES;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0043_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CONFIGURATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERFACE_LIST;
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[12] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[11] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0044_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0045_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ipv4-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0046_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ipv6-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0047_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0048_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list
	//0049_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/status-info
	//0050_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/status-info
	aps[8] = EXCELFORE_TSN_REMOTE_STATUS_INFO;
	aps[9] = EXCELFORE_TSN_REMOTE_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0051_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/failed-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_FAILED_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0052_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0053_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/stream-rank
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_STREAM_RANK;
	aps[9] = EXCELFORE_TSN_REMOTE_RANK;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0054_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/end-station-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_END_STATION_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0055_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_DATA_FRAME_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0056_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ieee802-vlan-tag
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[10] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0057_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ipv4-tuple
	aps[9] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0058_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ipv6-tuple
	aps[9] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0059_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification
	//0060_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification/interval
	aps[8] = EXCELFORE_TSN_REMOTE_TRAFFIC_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERVAL;
	aps[10] = EXCELFORE_TSN_REMOTE_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0061_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification/time-aware
	aps[9] = EXCELFORE_TSN_REMOTE_TIME_AWARE;
	aps[10] = EXCELFORE_TSN_REMOTE_EARLIEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_LATEST_TRANSMIT_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_JITTER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0062_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0063_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0064_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-capabilities
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CAPABILITIES;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0065_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CONFIGURATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERFACE_LIST;
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[12] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[11] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0066_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0067_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ipv4-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0068_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ipv6-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0069_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0070_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list
	//0071_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/status-info
	//0072_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/status-info
	aps[8] = EXCELFORE_TSN_REMOTE_STATUS_INFO;
	aps[9] = EXCELFORE_TSN_REMOTE_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0073_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/failed-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_FAILED_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0074_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0075_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls
	//0076_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/indication-list
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_LS;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_FOUR_PACKED_EVENT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0077_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/admin-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_FOUR_PACKED_EVENT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0078_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/oper-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_FOUR_PACKED_EVENT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0079_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le
	//0080_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/end-station-interfaces
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_LE;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_END_STATION_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0081_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0082_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-capabilities
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CAPABILITIES;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0083_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CONFIGURATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERFACE_LIST;
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[12] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[11] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0084_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0085_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ipv4-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0086_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ipv6-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0087_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0088_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list
	//0089_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/status-info
	//0090_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/status-info
	aps[8] = EXCELFORE_TSN_REMOTE_STATUS_INFO;
	aps[9] = EXCELFORE_TSN_REMOTE_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0091_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/failed-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_FAILED_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0092_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0093_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/end-station-interfaces
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_END_STATION_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0094_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0095_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-capabilities
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CAPABILITIES;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0096_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CONFIGURATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERFACE_LIST;
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[12] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[11] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0097_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0098_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ipv4-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0099_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ipv6-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0100_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0101_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list
	//0102_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/status-info
	//0103_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/status-info
	aps[8] = EXCELFORE_TSN_REMOTE_STATUS_INFO;
	aps[9] = EXCELFORE_TSN_REMOTE_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0104_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/failed-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_FAILED_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0105_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0106_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/end-station-interfaces
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_END_STATION_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0107_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0108_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-capabilities
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CAPABILITIES;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_TAG_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_STREAM_IDEN_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_CB_SEQUENCE_TYPE_LIST;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0109_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	aps[8] = EXCELFORE_TSN_REMOTE_INTERFACE_CONFIGURATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERFACE_LIST;
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES;
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[12] = EXCELFORE_TSN_REMOTE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[11] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	aps[11] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0110_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0111_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ipv4-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV4_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV4_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0112_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ipv6-tuple
	aps[11] = EXCELFORE_TSN_REMOTE_IPV6_TUPLE;
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS;
	vtype=YANG_VTYPE_INET_IPV6_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DSCP;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_PROTOCOL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_SOURCE_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_DESTINATION_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0113_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0114_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list
	//0115_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/status-info
	//0116_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/status-info
	aps[8] = EXCELFORE_TSN_REMOTE_STATUS_INFO;
	aps[9] = EXCELFORE_TSN_REMOTE_TALKER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_LISTENER_STATUS;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_FAILURE_CODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0117_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/failed-interfaces
	aps[8] = EXCELFORE_TSN_REMOTE_FAILED_INTERFACES;
	aps[9] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[10] = EXCELFORE_TSN_REMOTE_MAC_ADDRESS;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_INTERFACE_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0118_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0119_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm
	//0120_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/indication-list
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_DM;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_SRCLASS_VID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_SRCLASS_ID;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_SRCLASS_PRIORITY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0121_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/admin-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_SRCLASS_VID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_SRCLASS_ID;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_SRCLASS_PRIORITY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0122_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/oper-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_SRCLASS_VID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_SRCLASS_ID;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_SRCLASS_PRIORITY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0123_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp
	//0124_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/indication-list
	aps[6] = EXCELFORE_TSN_REMOTE_MVRP;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0125_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/admin-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0126_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/oper-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0127_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[6] = EXCELFORE_TSN_REMOTE_EXTERNAL_CONTROL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[6] = EXCELFORE_TSN_REMOTE_INDICATION_CHANGE_COUNTER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0128_excelfore-tsn-remote/tsn-remote/instances/instance
	aps[1] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

