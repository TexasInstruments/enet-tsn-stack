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
#include "excelfore-tsn-remote.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee1588-ptp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-netconf-monitoring.h"
#include "ietf-yang-library.h"
#include "excelfore-netconf-server.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *excelfore_tsn_remote_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"tsn-remote", 		// 2(0x2)
	"instances", 		// 3(0x3)
	"instance", 		// 4(0x4)
	"instance-index", 		// 5(0x5)
	"update", 		// 6(0x6)
	"port", 		// 7(0x7)
	"protocol", 		// 8(0x8)
	"request-status", 		// 9(0x9)
	"result-status", 		// 10(0xa)
	"external-control-man", 		// 11(0xb)
	"control-objects", 		// 12(0xc)
	"external-control", 		// 13(0xd)
	"indication-change-counter", 		// 14(0xe)
	"msrp-ta", 		// 15(0xf)
	"indication-list", 		// 16(0x10)
	"stream-id", 		// 17(0x11)
	"dest-address", 		// 18(0x12)
	"vlan-id", 		// 19(0x13)
	"max-frame-size", 		// 20(0x14)
	"max-interval-frames", 		// 21(0x15)
	"pcp", 		// 22(0x16)
	"rank", 		// 23(0x17)
	"accumulated-latency", 		// 24(0x18)
	"admin-request-list", 		// 25(0x19)
	"oper-request-list", 		// 26(0x1a)
	"msrp-tf", 		// 27(0x1b)
	"system-id", 		// 28(0x1c)
	"failuer-code", 		// 29(0x1d)
	"msrp-te", 		// 30(0x1e)
	"stream-rank", 		// 31(0x1f)
	"end-station-interfaces", 		// 32(0x20)
	"mac-address", 		// 33(0x21)
	"interface-name", 		// 34(0x22)
	"data-frame-specification", 		// 35(0x23)
	"index", 		// 36(0x24)
	"ieee802-mac-addresses", 		// 37(0x25)
	"destination-mac-address", 		// 38(0x26)
	"source-mac-address", 		// 39(0x27)
	"ieee802-vlan-tag", 		// 40(0x28)
	"priority-code-point", 		// 41(0x29)
	"traffic-specification", 		// 42(0x2a)
	"interval", 		// 43(0x2b)
	"numerator", 		// 44(0x2c)
	"denominator", 		// 45(0x2d)
	"max-frames-per-interval", 		// 46(0x2e)
	"transmission-selection", 		// 47(0x2f)
	"time-aware", 		// 48(0x30)
	"earliest-transmit-offset", 		// 49(0x31)
	"latest-transmit-offset", 		// 50(0x32)
	"jitter", 		// 51(0x33)
	"user-to-network-requirements", 		// 52(0x34)
	"num-seamless-trees", 		// 53(0x35)
	"max-latency", 		// 54(0x36)
	"interface-capabilities", 		// 55(0x37)
	"vlan-tag-capable", 		// 56(0x38)
	"cb-stream-iden-type-list", 		// 57(0x39)
	"cb-sequence-type-list", 		// 58(0x3a)
	"interface-configuration", 		// 59(0x3b)
	"interface-list", 		// 60(0x3c)
	"config-list", 		// 61(0x3d)
	"time-aware-offset", 		// 62(0x3e)
	"status-info", 		// 63(0x3f)
	"talker-status", 		// 64(0x40)
	"listener-status", 		// 65(0x41)
	"failure-code", 		// 66(0x42)
	"failed-interfaces", 		// 67(0x43)
	"msrp-ls", 		// 68(0x44)
	"four_packed_event", 		// 69(0x45)
	"msrp-le", 		// 70(0x46)
	"msrp-dm", 		// 71(0x47)
	"srclass-id", 		// 72(0x48)
	"srclass-priority", 		// 73(0x49)
	"srclass-vid", 		// 74(0x4a)
	"mvrp", 		// 75(0x4b)
};

uint8_t excelfore_tsn_remote_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)EXCELFORE_TSN_REMOTE_ENUM_END;i++){
		if(!strcmp(astr, excelfore_tsn_remote_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *excelfore_tsn_remote_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)EXCELFORE_TSN_REMOTE_ENUM_END){return NULL;}
	return excelfore_tsn_remote_enum_strings[anum];
}

int excelfore_tsn_remote_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[16]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
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
	//0013_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification
	//0014_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification/interval
	aps[8] = EXCELFORE_TSN_REMOTE_TRAFFIC_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERVAL;
	aps[10] = EXCELFORE_TSN_REMOTE_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0015_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification/time-aware
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
	//0016_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0017_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0018_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-capabilities
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
	//0019_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
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
	//0020_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0021_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0022_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list
	//0023_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/status-info
	//0024_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/status-info
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
	//0025_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/failed-interfaces
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
	//0026_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0027_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/stream-rank
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
	//0028_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/end-station-interfaces
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
	//0029_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ieee802-mac-addresses
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
	//0030_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ieee802-vlan-tag
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[10] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0031_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification
	//0032_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification/interval
	aps[8] = EXCELFORE_TSN_REMOTE_TRAFFIC_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERVAL;
	aps[10] = EXCELFORE_TSN_REMOTE_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0033_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification/time-aware
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
	//0034_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0035_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0036_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-capabilities
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
	//0037_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
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
	//0038_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0039_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0040_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list
	//0041_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/status-info
	//0042_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/status-info
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
	//0043_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/failed-interfaces
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
	//0044_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0045_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/stream-rank
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
	//0046_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/end-station-interfaces
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
	//0047_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ieee802-mac-addresses
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
	//0048_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ieee802-vlan-tag
	aps[9] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[10] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0049_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification
	//0050_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification/interval
	aps[8] = EXCELFORE_TSN_REMOTE_TRAFFIC_SPECIFICATION;
	aps[9] = EXCELFORE_TSN_REMOTE_INTERVAL;
	aps[10] = EXCELFORE_TSN_REMOTE_NUMERATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	aps[10] = EXCELFORE_TSN_REMOTE_DENOMINATOR;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 13, &vtype, 1)!=0){goto erexit;}
	//0051_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification/time-aware
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
	//0052_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAMES_PER_INTERVAL;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_TRANSMISSION_SELECTION;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0053_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0054_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-capabilities
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
	//0055_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
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
	//0056_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0057_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0058_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list
	//0059_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/status-info
	//0060_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/status-info
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
	//0061_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/failed-interfaces
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
	//0062_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0063_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls
	//0064_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/indication-list
	aps[6] = EXCELFORE_TSN_REMOTE_MSRP_LS;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_FOUR_PACKED_EVENT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0065_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/admin-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_FOUR_PACKED_EVENT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0066_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/oper-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_FOUR_PACKED_EVENT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_STREAM_ID;
	vtype=YANG_VTYPE_TSN_STREAM_ID_TYPE;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0067_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le
	//0068_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/end-station-interfaces
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
	//0069_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0070_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-capabilities
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
	//0071_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
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
	//0072_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0073_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0074_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list
	//0075_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/status-info
	//0076_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/status-info
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
	//0077_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/failed-interfaces
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
	//0078_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0079_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/end-station-interfaces
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
	//0080_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0081_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-capabilities
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
	//0082_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
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
	//0083_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0084_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0085_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list
	//0086_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/status-info
	//0087_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/status-info
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
	//0088_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/failed-interfaces
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
	//0089_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0090_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/end-station-interfaces
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
	//0091_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/user-to-network-requirements
	aps[8] = EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS;
	aps[9] = EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	aps[9] = EXCELFORE_TSN_REMOTE_MAX_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0092_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-capabilities
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
	//0093_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
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
	//0094_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	aps[10] = EXCELFORE_TSN_REMOTE_CONFIG_LIST;
	aps[11] = EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG;
	aps[12] = EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	aps[12] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 15, &vtype, 1)!=0){goto erexit;}
	//0095_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list
	aps[11] = EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 14, &vtype, 1)!=0){goto erexit;}
	//0096_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list
	//0097_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/status-info
	//0098_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/status-info
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
	//0099_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/failed-interfaces
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
	//0100_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list
	aps[8] = EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	//0101_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm
	//0102_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/indication-list
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
	//0103_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/admin-request-list
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
	//0104_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/oper-request-list
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
	//0105_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp
	//0106_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/indication-list
	aps[6] = EXCELFORE_TSN_REMOTE_MVRP;
	aps[7] = EXCELFORE_TSN_REMOTE_INDICATION_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0107_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/admin-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[7] = EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0108_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/oper-request-list
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[7] = EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST;
	aps[8] = EXCELFORE_TSN_REMOTE_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[9] = EXCELFORE_TSN_REMOTE_VLAN_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0109_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects
	aps[0] = EXCELFORE_TSN_REMOTE_RW;
	aps[6] = EXCELFORE_TSN_REMOTE_EXTERNAL_CONTROL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[6] = EXCELFORE_TSN_REMOTE_INDICATION_CHANGE_COUNTER;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0110_excelfore-tsn-remote/tsn-remote/instances/instance
	aps[1] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
