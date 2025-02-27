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
#include "ieee802-dot1cb-stream-identification.h"

extern uint8_t IEEE802_DOT1CB_STREAM_IDENTIFICATION_func(uc_dbald *dbald);
#define IEEE802_DOT1CB_STREAM_IDENTIFICATION_RW IEEE802_DOT1CB_STREAM_IDENTIFICATION_func(dbald)
#define IEEE802_DOT1CB_STREAM_IDENTIFICATION_RO (IEEE802_DOT1CB_STREAM_IDENTIFICATION_func(dbald)|0x80u)

const char *ieee802_dot1cb_stream_identification_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"stream-identity", 		// 3(0x3)
	"index", 		// 4(0x4)
	"handle", 		// 5(0x5)
	"in-facing", 		// 6(0x6)
	"input-port", 		// 7(0x7)
	"output-port", 		// 8(0x8)
	"out-facing", 		// 9(0x9)
	"null-stream-identification", 		// 10(0xa)
	"destination-mac", 		// 11(0xb)
	"tagged", 		// 12(0xc)
	"vlan", 		// 13(0xd)
	// augmented by xl4-ieee802-dot1cb-stream-identification
	"config-status",		// 14(0xe)
};
const uint8_t ieee802_dot1cb_stream_identification_enum_max=15;

#ifdef GENERATE_INITCONFIG

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "dot1cb-sid",
		"urn:ieee:std:802.1Q:yang:ieee802-dot1cb-stream-identification")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "xl4cb-sid",
		"http://excelfore.com/ns/xl4-ieee802-dot1cb-stream-identification")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[6];
	apsd[0]=IEEE802_DOT1CB_STREAM_IDENTIFICATION_RW;
	apsd[1]=IEEE802_DOT1CB_STREAM_IDENTIFICATION_STREAM_IDENTITY;
	apsd[2]=IEEE802_DOT1CB_STREAM_IDENTIFICATION_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"dot1cb-sid", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE802_DOT1CB_STREAM_IDENTIFICATION_STREAM_IDENTITY;
	apsd[2]=IEEE802_DOT1CB_STREAM_IDENTIFICATION_CONFIG_STATUS;
	apsd[3]=IEEE802_DOT1CB_STREAM_IDENTIFICATION_NSASC;
	apsd[4]=255u;
	if(uc_dbal_create(dbald, apsd, 5, (void*)"xl4cb-sid", 10)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int ieee802_dot1cb_stream_identification_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[7]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_ieee802-dot1cb-stream-identification/stream-identity/in-facing
	aps[0] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_RW;
	aps[1] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_STREAM_IDENTITY;
	aps[2] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_IN_FACING;
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_INPUT_PORT;
	vtype=YANG_VTYPE_STRING_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_OUTPUT_PORT;
	vtype=YANG_VTYPE_STRING_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[2] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_VALUEKEY;
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0001_ieee802-dot1cb-stream-identification/stream-identity/out-facing
	aps[2] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_OUT_FACING;
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_INPUT_PORT;
	vtype=YANG_VTYPE_STRING_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_OUTPUT_PORT;
	vtype=YANG_VTYPE_STRING_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0002_ieee802-dot1cb-stream-identification/stream-identity/null-stream-identification
	aps[2] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_NULL_STREAM_IDENTIFICATION;
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_DESTINATION_MAC;
	vtype=YANG_VTYPE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_TAGGED;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_VLAN;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0003_ieee802-dot1cb-stream-identification/stream-identity
	aps[2] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_HANDLE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_RO;
	aps[2] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_CONFIG_STATUS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[1] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

#endif
