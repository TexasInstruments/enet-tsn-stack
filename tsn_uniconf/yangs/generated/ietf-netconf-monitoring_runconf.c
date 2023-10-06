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
#include "ietf-netconf-monitoring.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee1588-ptp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-yang-library.h"
#include "excelfore-tsn-remote.h"
#include "excelfore-netconf-server.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *ietf_netconf_monitoring_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"netconf-state", 		// 2(0x2)
	"capabilities", 		// 3(0x3)
	"capability", 		// 4(0x4)
	"datastores", 		// 5(0x5)
	"datastore", 		// 6(0x6)
	"name", 		// 7(0x7)
	"locks", 		// 8(0x8)
	"global-lock", 		// 9(0x9)
	"locked-by-session", 		// 10(0xa)
	"locked-time", 		// 11(0xb)
	"partial-lock", 		// 12(0xc)
	"lock-id", 		// 13(0xd)
	"select", 		// 14(0xe)
	"locked-node", 		// 15(0xf)
	"schemas", 		// 16(0x10)
	"schema", 		// 17(0x11)
	"identifier", 		// 18(0x12)
	"version", 		// 19(0x13)
	"format", 		// 20(0x14)
	"namespace", 		// 21(0x15)
	"location", 		// 22(0x16)
	"sessions", 		// 23(0x17)
	"session", 		// 24(0x18)
	"session-id", 		// 25(0x19)
	"transport", 		// 26(0x1a)
	"username", 		// 27(0x1b)
	"source-host", 		// 28(0x1c)
	"login-time", 		// 29(0x1d)
	"in-rpcs", 		// 30(0x1e)
	"in-bad-rpcs", 		// 31(0x1f)
	"out-rpc-errors", 		// 32(0x20)
	"out-notifications", 		// 33(0x21)
	"statistics", 		// 34(0x22)
	"netconf-start-time", 		// 35(0x23)
	"in-bad-hellos", 		// 36(0x24)
	"in-sessions", 		// 37(0x25)
	"dropped-sessions", 		// 38(0x26)
};

uint8_t ietf_netconf_monitoring_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)IETF_NETCONF_MONITORING_ENUM_END;i++){
		if(!strcmp(astr, ietf_netconf_monitoring_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *ietf_netconf_monitoring_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)IETF_NETCONF_MONITORING_ENUM_END){return NULL;}
	return ietf_netconf_monitoring_enum_strings[anum];
}

int ietf_netconf_monitoring_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[10]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	//0000_ietf-netconf-monitoring/netconf-state/capabilities
	aps[0] = IETF_NETCONF_MONITORING_RO;
	aps[1] = IETF_NETCONF_MONITORING_NETCONF_STATE;
	aps[2] = IETF_NETCONF_MONITORING_CAPABILITIES;
	aps[3] = IETF_NETCONF_MONITORING_CAPABILITY;
	vtype=YANG_VTYPE_INET_URI;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	//0001_ietf-netconf-monitoring/netconf-state/datastores/datastore/locks/global-lock
	aps[2] = IETF_NETCONF_MONITORING_DATASTORES;
	aps[3] = IETF_NETCONF_MONITORING_DATASTORE;
	aps[4] = IETF_NETCONF_MONITORING_LOCKS;
	aps[5] = IETF_NETCONF_MONITORING_GLOBAL_LOCK;
	aps[6] = IETF_NETCONF_MONITORING_LOCKED_BY_SESSION;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_NETCONF_MONITORING_LOCKED_TIME;
	vtype=YANG_VTYPE_YANG_DATE_AND_TIME;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_VALUEKEY;
	aps[5] = IETF_NETCONF_MONITORING_NAME;
	vtype=YANG_VTYPE_NETCONF_DATASTORE_TYPE;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0002_ietf-netconf-monitoring/netconf-state/datastores/datastore/locks/partial-lock
	aps[4] = IETF_NETCONF_MONITORING_LOCKS;
	aps[5] = IETF_NETCONF_MONITORING_PARTIAL_LOCK;
	aps[6] = IETF_NETCONF_MONITORING_LOCKED_BY_SESSION;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_NETCONF_MONITORING_LOCKED_TIME;
	vtype=YANG_VTYPE_YANG_DATE_AND_TIME;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_NETCONF_MONITORING_SELECT;
	vtype=YANG_VTYPE_YANG_XPATH1_0;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_NETCONF_MONITORING_LOCKED_NODE;
	vtype=YANG_VTYPE_INSTANCE_IDENTIFIER;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IETF_NETCONF_MONITORING_VALUEKEY;
	aps[7] = IETF_NETCONF_MONITORING_LOCK_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0003_ietf-netconf-monitoring/netconf-state/schemas
	//0004_ietf-netconf-monitoring/netconf-state/datastores/datastore
	//0005_ietf-netconf-monitoring/netconf-state/schemas
	//0006_ietf-netconf-monitoring/netconf-state/schemas/schema
	aps[2] = IETF_NETCONF_MONITORING_SCHEMAS;
	aps[3] = IETF_NETCONF_MONITORING_SCHEMA;
	aps[4] = IETF_NETCONF_MONITORING_NAMESPACE;
	vtype=YANG_VTYPE_INET_URI;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_LOCATION;
	vtype=YANG_VTYPE_UNION;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_VALUEKEY;
	aps[5] = IETF_NETCONF_MONITORING_IDENTIFIER;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_NETCONF_MONITORING_VERSION;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IETF_NETCONF_MONITORING_FORMAT;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0007_ietf-netconf-monitoring/netconf-state/sessions
	//0008_ietf-netconf-monitoring/netconf-state/sessions/session
	aps[2] = IETF_NETCONF_MONITORING_SESSIONS;
	aps[3] = IETF_NETCONF_MONITORING_SESSION;
	aps[4] = IETF_NETCONF_MONITORING_TRANSPORT;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_USERNAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_SOURCE_HOST;
	vtype=YANG_VTYPE_INET_HOST;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_LOGIN_TIME;
	vtype=YANG_VTYPE_YANG_DATE_AND_TIME;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_IN_RPCS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_IN_BAD_RPCS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_OUT_RPC_ERRORS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_OUT_NOTIFICATIONS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = IETF_NETCONF_MONITORING_VALUEKEY;
	aps[5] = IETF_NETCONF_MONITORING_SESSION_ID;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0009_ietf-netconf-monitoring/netconf-state/statistics
	//0010_ietf-netconf-monitoring/netconf-state/statistics
	aps[2] = IETF_NETCONF_MONITORING_STATISTICS;
	aps[3] = IETF_NETCONF_MONITORING_NETCONF_START_TIME;
	vtype=YANG_VTYPE_YANG_DATE_AND_TIME;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_MONITORING_IN_BAD_HELLOS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_MONITORING_IN_SESSIONS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_MONITORING_DROPPED_SESSIONS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_MONITORING_IN_RPCS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_MONITORING_IN_BAD_RPCS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_MONITORING_OUT_RPC_ERRORS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = IETF_NETCONF_MONITORING_OUT_NOTIFICATIONS;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[1] = IETF_NETCONF_MONITORING_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
