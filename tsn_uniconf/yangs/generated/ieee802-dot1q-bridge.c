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
#include "ieee802-dot1q-bridge.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee1588-ptp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-netconf-monitoring.h"
#include "ietf-yang-library.h"
#include "excelfore-tsn-remote.h"
#include "excelfore-netconf-server.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

int ieee802_dot1q_bridge_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t aps[10];
	int i;
	int res=-1;
	uint32_t vsize=0;
	void *kvs[7]={NULL};
	uint8_t kss[7];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,YANG_DB_ONHW_ALWAYS,
	                              NULL,aps,kvs,kss,NULL,0};
	dbpara.atype=YANG_DB_ACTION_READ;
	aps[0] = IEEE802_DOT1Q_BRIDGE_RO;
	aps[1] = IEEE802_DOT1Q_BRIDGE_VALUEKEY;
	aps[2] = 255;
	if(yang_db_action(dbald, hwald, &dbpara)==0){
		if(dbpara.vsize==1 && (*(uint8_t*)dbpara.value==1)){return 0;}
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, hwald, &dbpara);
	dbpara.atype=YANG_DB_ACTION_CREATE;
	//0000_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/static-filtering-entries
	//0001_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/static-vlan-registration-entries
	//0002_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/mac-address-registration-entries
	//0003_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/dynamic-vlan-registration-entries
	//0004_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/dynamic-reservation-entries
	//0005_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map/dynamic-filtering-entries
	//0006_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry/port-map
	//0007_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/filtering-entry
	//0008_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/static-filtering-entries
	//0009_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/static-vlan-registration-entries
	//0010_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/mac-address-registration-entries
	//0011_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/dynamic-vlan-registration-entries
	//0012_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/dynamic-reservation-entries
	//0013_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map/dynamic-filtering-entries
	//0014_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry/port-map
	//0015_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database/vlan-registration-entry
	//0016_ieee802-dot1q-bridge/bridges/bridge/component/filtering-database
	//0017_ieee802-dot1q-bridge/bridges/bridge/component/permanent-database/filtering-entry
	//0018_ieee802-dot1q-bridge/bridges/bridge/component/permanent-database
	//0019_ieee802-dot1q-bridge/bridges/bridge/component/bridge-vlan/vlan
	//0020_ieee802-dot1q-bridge/bridges/bridge/component/bridge-vlan
	//0021_ieee802-dot1q-bridge/bridges/bridge/component
	//0022_ieee802-dot1q-bridge/bridges/bridge
	if(kvs[0]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[0]);}
	kvs[0] = NULL;
	dbpara.atype=YANG_DB_ACTION_CREATE;
	res=yang_value_conv(YANG_VTYPE_UINT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	res=0;
erexit:
	if(dbpara.value!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, dbpara.value);}
	for(i=0;i<6;i++){
		if(kvs[i]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[i]);}
	}
	return res;
}
