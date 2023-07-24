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
#include "ietf-keychain.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee1588-ptp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-netconf-server.h"
#include "excelfore-tsn-remote.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *ietf_keychain_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"keychain", 		// 2(0x2)
	"private-keys", 		// 3(0x3)
	"private-key", 		// 4(0x4)
	"name", 		// 5(0x5)
	"algorithm", 		// 6(0x6)
	"key-length", 		// 7(0x7)
	"public-key", 		// 8(0x8)
	"certificates", 		// 9(0x9)
	"certificate", 		// 10(0xa)
	"chain", 		// 11(0xb)
	"trusted-certificates", 		// 12(0xc)
	"description", 		// 13(0xd)
	"trusted-certificate", 		// 14(0xe)
	// augmented by xl4-keychain
	"location",		// 15(0xf)
};

uint8_t ietf_keychain_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)IETF_KEYCHAIN_ENUM_END;i++){
		if(!strcmp(astr, ietf_keychain_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *ietf_keychain_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)IETF_KEYCHAIN_ENUM_END){return NULL;}
	return ietf_keychain_enum_strings[anum];
}

int ietf_keychain_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[10]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	//0000_ietf-keychain/keychain/private-keys/private-key/certificates/certificate
	aps[0] = IETF_KEYCHAIN_RW;
	aps[1] = IETF_KEYCHAIN_KEYCHAIN;
	aps[2] = IETF_KEYCHAIN_PRIVATE_KEYS;
	aps[3] = IETF_KEYCHAIN_PRIVATE_KEY;
	aps[4] = IETF_KEYCHAIN_CERTIFICATES;
	aps[5] = IETF_KEYCHAIN_CERTIFICATE;
	aps[6] = IETF_KEYCHAIN_CHAIN;
	vtype=YANG_VTYPE_BINARY;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_KEYCHAIN_VALUEKEY;
	aps[7] = IETF_KEYCHAIN_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_KEYCHAIN_VALUEKEY;
	aps[5] = IETF_KEYCHAIN_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0001_ietf-keychain/keychain/private-keys/private-key/location
	//0002_ietf-keychain/keychain/private-keys/private-key
	aps[0] = IETF_KEYCHAIN_RO;
	aps[4] = IETF_KEYCHAIN_ALGORITHM;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_KEYCHAIN_KEY_LENGTH;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_KEYCHAIN_PUBLIC_KEY;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_KEYCHAIN_RW;
	aps[4] = IETF_KEYCHAIN_LOCATION;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0003_ietf-keychain/keychain/trusted-certificates
	//0004_ietf-keychain/keychain/trusted-certificates/trusted-certificate
	aps[2] = IETF_KEYCHAIN_TRUSTED_CERTIFICATES;
	aps[3] = IETF_KEYCHAIN_TRUSTED_CERTIFICATE;
	aps[4] = IETF_KEYCHAIN_CERTIFICATE;
	vtype=YANG_VTYPE_BINARY;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_KEYCHAIN_VALUEKEY;
	aps[5] = IETF_KEYCHAIN_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_KEYCHAIN_VALUEKEY;
	aps[4] = IETF_KEYCHAIN_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0005_ietf-keychain/keychain/trusted-certificates
	aps[3] = IETF_KEYCHAIN_DESCRIPTION;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = IETF_KEYCHAIN_RO;
	aps[1] = IETF_KEYCHAIN_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
