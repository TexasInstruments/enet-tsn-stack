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
#include "excelfore-config-uni.h"

extern uint8_t EXCELFORE_CONFIG_UNI_func(uc_dbald *dbald);
#define EXCELFORE_CONFIG_UNI_RW EXCELFORE_CONFIG_UNI_func(dbald)
#define EXCELFORE_CONFIG_UNI_RO (EXCELFORE_CONFIG_UNI_func(dbald)|0x80u)

const char *excelfore_config_uni_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"xl4-uni", 		// 3(0x3)
	"domain", 		// 4(0x4)
	"domain-id", 		// 5(0x5)
	"event-bridges", 		// 6(0x6)
	"nconf-servers", 		// 7(0x7)
	"name", 		// 8(0x8)
	"address", 		// 9(0x9)
	"port", 		// 10(0xa)
	"transport", 		// 11(0xb)
	"role", 		// 12(0xc)
	"nconf-clients", 		// 13(0xd)
	"cert", 		// 14(0xe)
	"key", 		// 15(0xf)
	"ca-cert", 		// 16(0x10)
	"mac-address-pool", 		// 17(0x11)
	"top-address", 		// 18(0x12)
	"busy-bitmap", 		// 19(0x13)
};
const uint8_t excelfore_config_uni_enum_max=20;

#ifdef GENERATE_INITCONFIG

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "xl4cfguni",
		"http://excelfore.com/ns/excelfore-config-uni")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[7];
	apsd[0]=EXCELFORE_CONFIG_UNI_RW;
	apsd[1]=EXCELFORE_CONFIG_UNI_XL4_UNI;
	apsd[2]=EXCELFORE_CONFIG_UNI_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"xl4cfguni", 10)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int excelfore_config_uni_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[8]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_excelfore-config-uni/xl4-uni/domain/nconf-servers
	aps[0] = EXCELFORE_CONFIG_UNI_RW;
	aps[1] = EXCELFORE_CONFIG_UNI_XL4_UNI;
	aps[2] = EXCELFORE_CONFIG_UNI_DOMAIN;
	aps[3] = EXCELFORE_CONFIG_UNI_NCONF_SERVERS;
	aps[4] = EXCELFORE_CONFIG_UNI_ADDRESS;
	vtype=YANG_VTYPE_UNION;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_CONFIG_UNI_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_CONFIG_UNI_TRANSPORT;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_CONFIG_UNI_RO;
	aps[4] = EXCELFORE_CONFIG_UNI_ROLE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_CONFIG_UNI_RW;
	aps[4] = EXCELFORE_CONFIG_UNI_VALUEKEY;
	aps[5] = EXCELFORE_CONFIG_UNI_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = EXCELFORE_CONFIG_UNI_VALUEKEY;
	aps[4] = EXCELFORE_CONFIG_UNI_DOMAIN_ID;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0001_excelfore-config-uni/xl4-uni/domain/nconf-clients
	aps[0] = EXCELFORE_CONFIG_UNI_RO;
	aps[3] = EXCELFORE_CONFIG_UNI_NCONF_CLIENTS;
	aps[4] = EXCELFORE_CONFIG_UNI_CERT;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_CONFIG_UNI_KEY;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_CONFIG_UNI_CA_CERT;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_CONFIG_UNI_RW;
	aps[4] = EXCELFORE_CONFIG_UNI_VALUEKEY;
	aps[5] = EXCELFORE_CONFIG_UNI_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0002_excelfore-config-uni/xl4-uni/domain/mac-address-pool
	aps[3] = EXCELFORE_CONFIG_UNI_MAC_ADDRESS_POOL;
	aps[4] = EXCELFORE_CONFIG_UNI_TOP_ADDRESS;
	vtype=YANG_VTYPE_MAC_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_CONFIG_UNI_BUSY_BITMAP;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0003_excelfore-config-uni/xl4-uni/domain
	aps[3] = EXCELFORE_CONFIG_UNI_EVENT_BRIDGES;
	vtype=YANG_VTYPE_STRING_LEAF_LIST;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_CONFIG_UNI_RO;
	aps[1] = EXCELFORE_CONFIG_UNI_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

#endif
