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
#include "excelfore-netconf-server.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee1588-ptp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-netconf-monitoring.h"
#include "ietf-yang-library.h"
#include "excelfore-tsn-remote.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *excelfore_netconf_server_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"x4nconf-server", 		// 2(0x2)
	"transport", 		// 3(0x3)
	"tls", 		// 4(0x4)
	"port", 		// 5(0x5)
	"server-cert", 		// 6(0x6)
	"server-key", 		// 7(0x7)
	"capath", 		// 8(0x8)
	"cert-to-name", 		// 9(0x9)
	"id", 		// 10(0xa)
	"fingerprint", 		// 11(0xb)
	"map-type", 		// 12(0xc)
	"name", 		// 13(0xd)
	"ssh", 		// 14(0xe)
	"use-unix-domain-socket", 		// 15(0xf)
	"sub-ipc-port", 		// 16(0x10)
	"sub-socket-name", 		// 17(0x11)
};

uint8_t excelfore_netconf_server_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)EXCELFORE_NETCONF_SERVER_ENUM_END;i++){
		if(!strcmp(astr, excelfore_netconf_server_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *excelfore_netconf_server_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)EXCELFORE_NETCONF_SERVER_ENUM_END){return NULL;}
	return excelfore_netconf_server_enum_strings[anum];
}

int excelfore_netconf_server_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[9]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	//0000_excelfore-netconf-server/x4nconf-server/transport/tls/cert-to-name
	aps[0] = EXCELFORE_NETCONF_SERVER_RW;
	aps[1] = EXCELFORE_NETCONF_SERVER_X4NCONF_SERVER;
	aps[2] = EXCELFORE_NETCONF_SERVER_TRANSPORT;
	aps[3] = EXCELFORE_NETCONF_SERVER_TLS;
	aps[4] = EXCELFORE_NETCONF_SERVER_CERT_TO_NAME;
	aps[5] = EXCELFORE_NETCONF_SERVER_FINGERPRINT;
	vtype=YANG_VTYPE_X509C2N_TLS_FINGERPRINT;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = EXCELFORE_NETCONF_SERVER_MAP_TYPE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = EXCELFORE_NETCONF_SERVER_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = EXCELFORE_NETCONF_SERVER_VALUEKEY;
	aps[6] = EXCELFORE_NETCONF_SERVER_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0001_excelfore-netconf-server/x4nconf-server/transport/tls
	aps[4] = EXCELFORE_NETCONF_SERVER_PORT;
	vtype=YANG_VTYPE_INET_PORT_NUMBER;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_SERVER_CERT;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_SERVER_KEY;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_CAPATH;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0002_excelfore-netconf-server/x4nconf-server/transport/ssh
	aps[3] = EXCELFORE_NETCONF_SERVER_SSH;
	aps[4] = EXCELFORE_NETCONF_SERVER_PORT;
	vtype=YANG_VTYPE_INET_PORT_NUMBER;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_USE_UNIX_DOMAIN_SOCKET;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_SUB_IPC_PORT;
	vtype=YANG_VTYPE_INET_PORT_NUMBER;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = EXCELFORE_NETCONF_SERVER_SUB_SOCKET_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = EXCELFORE_NETCONF_SERVER_RO;
	aps[1] = EXCELFORE_NETCONF_SERVER_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
