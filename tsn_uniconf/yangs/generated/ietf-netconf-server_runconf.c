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
#include "ietf-netconf-server.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee1588-ptp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-keychain.h"
#include "excelfore-tsn-remote.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *ietf_netconf_server_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"netconf-server", 		// 2(0x2)
	"session-options", 		// 3(0x3)
	"hello-timeout", 		// 4(0x4)
	"listen", 		// 5(0x5)
	"max-sessions", 		// 6(0x6)
	"idle-timeout", 		// 7(0x7)
	"endpoint", 		// 8(0x8)
	"name", 		// 9(0x9)
	"tls", 		// 10(0xa)
	"address", 		// 11(0xb)
	"port", 		// 12(0xc)
	"certificates", 		// 13(0xd)
	"certificate", 		// 14(0xe)
	"client-auth", 		// 15(0xf)
	"trusted-ca-certs", 		// 16(0x10)
	"trusted-client-certs", 		// 17(0x11)
	"cert-maps", 		// 18(0x12)
	"cert-to-name", 		// 19(0x13)
	"id", 		// 20(0x14)
	"fingerprint", 		// 21(0x15)
	"map-type", 		// 22(0x16)
	// augmented by xl4-netconf-server
	"private-key",		// 23(0x17)
};

uint8_t ietf_netconf_server_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)IETF_NETCONF_SERVER_ENUM_END;i++){
		if(!strcmp(astr, ietf_netconf_server_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *ietf_netconf_server_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)IETF_NETCONF_SERVER_ENUM_END){return NULL;}
	return ietf_netconf_server_enum_strings[anum];
}

int ietf_netconf_server_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[12]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	//0000_ietf-netconf-server/netconf-server/session-options
	aps[0] = IETF_NETCONF_SERVER_RW;
	aps[1] = IETF_NETCONF_SERVER_NETCONF_SERVER;
	aps[2] = IETF_NETCONF_SERVER_SESSION_OPTIONS;
	aps[3] = IETF_NETCONF_SERVER_HELLO_TIMEOUT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0001_ietf-netconf-server/netconf-server/listen/endpoint/tls/certificates/certificate
	aps[2] = IETF_NETCONF_SERVER_LISTEN;
	aps[3] = IETF_NETCONF_SERVER_ENDPOINT;
	aps[4] = IETF_NETCONF_SERVER_TLS;
	aps[5] = IETF_NETCONF_SERVER_CERTIFICATES;
	aps[6] = IETF_NETCONF_SERVER_CERTIFICATE;
	aps[7] = IETF_NETCONF_SERVER_DUMMY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IETF_NETCONF_SERVER_VALUEKEY;
	aps[8] = IETF_NETCONF_SERVER_NAME;
	vtype=YANG_VTYPE_LEAFREF;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_SERVER_VALUEKEY;
	aps[5] = IETF_NETCONF_SERVER_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0002_ietf-netconf-server/netconf-server/listen/endpoint/tls/client-auth
	//0003_ietf-netconf-server/netconf-server/listen/endpoint/tls/client-auth/cert-maps/cert-to-name
	aps[4] = IETF_NETCONF_SERVER_TLS;
	aps[5] = IETF_NETCONF_SERVER_CLIENT_AUTH;
	aps[6] = IETF_NETCONF_SERVER_CERT_MAPS;
	aps[7] = IETF_NETCONF_SERVER_CERT_TO_NAME;
	aps[8] = IETF_NETCONF_SERVER_FINGERPRINT;
	vtype=YANG_VTYPE_X509C2N_TLS_FINGERPRINT;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IETF_NETCONF_SERVER_MAP_TYPE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IETF_NETCONF_SERVER_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IETF_NETCONF_SERVER_VALUEKEY;
	aps[9] = IETF_NETCONF_SERVER_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0004_ietf-netconf-server/netconf-server/listen/endpoint/tls/private-key
	//0005_ietf-netconf-server/netconf-server/listen/endpoint/tls/client-auth
	aps[6] = IETF_NETCONF_SERVER_TRUSTED_CA_CERTS;
	vtype=YANG_VTYPE_LEAFREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_NETCONF_SERVER_TRUSTED_CLIENT_CERTS;
	vtype=YANG_VTYPE_LEAFREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0006_ietf-netconf-server/netconf-server/listen/endpoint/tls
	aps[5] = IETF_NETCONF_SERVER_ADDRESS;
	vtype=YANG_VTYPE_INET_IP_ADDRESS;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_NETCONF_SERVER_PORT;
	vtype=YANG_VTYPE_INET_PORT_NUMBER;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_NETCONF_SERVER_PRIVATE_KEY;
	vtype=YANG_VTYPE_LEAFREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0007_ietf-netconf-server/netconf-server/listen/endpoint
	//0008_ietf-netconf-server/netconf-server/listen
	aps[3] = IETF_NETCONF_SERVER_MAX_SESSIONS;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_SERVER_IDLE_TIMEOUT;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_NETCONF_SERVER_RO;
	aps[1] = IETF_NETCONF_SERVER_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
