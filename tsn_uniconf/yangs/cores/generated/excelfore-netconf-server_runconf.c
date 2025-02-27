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
#include "excelfore-netconf-server.h"

extern uint8_t EXCELFORE_NETCONF_SERVER_func(uc_dbald *dbald);
#define EXCELFORE_NETCONF_SERVER_RW EXCELFORE_NETCONF_SERVER_func(dbald)
#define EXCELFORE_NETCONF_SERVER_RO (EXCELFORE_NETCONF_SERVER_func(dbald)|0x80u)

const char *excelfore_netconf_server_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"nconf-server", 		// 3(0x3)
	"underlying-interface", 		// 4(0x4)
	"address", 		// 5(0x5)
	"ssh", 		// 6(0x6)
	"port", 		// 7(0x7)
	"subsystem", 		// 8(0x8)
	"ipc-type", 		// 9(0x9)
	"tcp-port", 		// 10(0xa)
	"socket-name", 		// 11(0xb)
	"status", 		// 12(0xc)
	"tls", 		// 13(0xd)
	"server-cert", 		// 14(0xe)
	"server-key", 		// 15(0xf)
	"ca-cert", 		// 16(0x10)
	"cert-to-name", 		// 17(0x11)
	"id", 		// 18(0x12)
	"fingerprint", 		// 19(0x13)
	"map-type", 		// 20(0x14)
	"name", 		// 21(0x15)
	"admin-users", 		// 22(0x16)
};
const uint8_t excelfore_netconf_server_enum_max=23;

#ifdef GENERATE_INITCONFIG

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "xl4nconf",
		"http://excelfore.com/ns/excelfore-netconf-server")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[7];
	apsd[0]=EXCELFORE_NETCONF_SERVER_RW;
	apsd[1]=EXCELFORE_NETCONF_SERVER_NCONF_SERVER;
	apsd[2]=EXCELFORE_NETCONF_SERVER_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"xl4nconf", 9)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int excelfore_netconf_server_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[8]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_excelfore-netconf-server/nconf-server/ssh/subsystem
	aps[0] = EXCELFORE_NETCONF_SERVER_RW;
	aps[1] = EXCELFORE_NETCONF_SERVER_NCONF_SERVER;
	aps[2] = EXCELFORE_NETCONF_SERVER_SSH;
	aps[3] = EXCELFORE_NETCONF_SERVER_SUBSYSTEM;
	aps[4] = EXCELFORE_NETCONF_SERVER_IPC_TYPE;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_TCP_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_SOCKET_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0001_excelfore-netconf-server/nconf-server/ssh
	aps[3] = EXCELFORE_NETCONF_SERVER_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_NETCONF_SERVER_RO;
	aps[3] = EXCELFORE_NETCONF_SERVER_STATUS;
	vtype=YANG_VTYPE_TRANSPORT_STATUS;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0002_excelfore-netconf-server/nconf-server/tls/cert-to-name
	aps[0] = EXCELFORE_NETCONF_SERVER_RW;
	aps[2] = EXCELFORE_NETCONF_SERVER_TLS;
	aps[3] = EXCELFORE_NETCONF_SERVER_CERT_TO_NAME;
	aps[4] = EXCELFORE_NETCONF_SERVER_FINGERPRINT;
	vtype=YANG_VTYPE_TLS_FINGERPRINT;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_MAP_TYPE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_VALUEKEY;
	aps[5] = EXCELFORE_NETCONF_SERVER_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0003_excelfore-netconf-server/nconf-server/tls
	aps[3] = EXCELFORE_NETCONF_SERVER_PORT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_NETCONF_SERVER_RO;
	aps[3] = EXCELFORE_NETCONF_SERVER_SERVER_CERT;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = EXCELFORE_NETCONF_SERVER_SERVER_KEY;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = EXCELFORE_NETCONF_SERVER_CA_CERT;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = EXCELFORE_NETCONF_SERVER_STATUS;
	vtype=YANG_VTYPE_TRANSPORT_STATUS;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0004_excelfore-netconf-server/nconf-server/admin-users
	aps[2] = EXCELFORE_NETCONF_SERVER_ADMIN_USERS;
	aps[3] = EXCELFORE_NETCONF_SERVER_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = EXCELFORE_NETCONF_SERVER_VALUEKEY;
	aps[4] = EXCELFORE_NETCONF_SERVER_ID;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0005_excelfore-netconf-server/nconf-server
	aps[0] = EXCELFORE_NETCONF_SERVER_RW;
	aps[2] = EXCELFORE_NETCONF_SERVER_UNDERLYING_INTERFACE;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_NETCONF_SERVER_RO;
	aps[2] = EXCELFORE_NETCONF_SERVER_ADDRESS;
	vtype=YANG_VTYPE_UNION;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[1] = EXCELFORE_NETCONF_SERVER_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

#endif
