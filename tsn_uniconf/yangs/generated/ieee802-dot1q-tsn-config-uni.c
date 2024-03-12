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
#include "ieee802-dot1q-tsn-config-uni.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);
YANG_NODE_CACHED_MODULE_GET_ENUM_DEF(IEEE802_DOT1Q_TSN_CONFIG_UNI, "ieee802-dot1q-tsn-config-uni");
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RW IEEE802_DOT1Q_TSN_CONFIG_UNI_func(dbald)
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RO (IEEE802_DOT1Q_TSN_CONFIG_UNI_func(dbald)|0x80u)

int ieee802_dot1q_tsn_config_uni_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t aps[12];
	int i;
	int res=-1;
	uint32_t vsize=0;
	void *kvs[9]={NULL};
	uint8_t kss[9];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,YANG_DB_ONHW_ALWAYS,
	                              NULL,aps,kvs,kss,NULL,0};
	if(yang_node_mod_set_enum(dbald, "ieee802-dot1q-tsn-config-uni")<0){return -1;};
	dbpara.atype=YANG_DB_ACTION_READ;
	aps[0] = IEEE802_DOT1Q_TSN_CONFIG_UNI_RO;
	aps[1] = IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY;
	aps[2] = 255;
	if(yang_db_action(dbald, hwald, &dbpara)==0){
		if(dbpara.vsize==1 && (*(uint8_t*)dbpara.value==1)){return 0;}
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, hwald, &dbpara);
	dbpara.atype=YANG_DB_ACTION_CREATE;
	//0000_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/stream-rank
	//0001_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/end-station-interfaces
	//0002_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/data-frame-specification/ieee802-mac-addresses
	//0003_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/data-frame-specification/ieee802-vlan-tag
	//0004_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/data-frame-specification
	//0005_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/traffic-specification/interval
	//0006_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/traffic-specification/time-aware
	//0007_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/traffic-specification
	//0008_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/user-to-network-requirements
	//0009_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-capabilities
	//0010_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0011_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0012_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list/config-list
	//0013_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker/interface-configuration/interface-list
	//0014_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener
	//0015_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/talker
	//0016_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/end-station-interfaces
	//0017_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/user-to-network-requirements
	//0018_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-capabilities
	//0019_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0020_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0021_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list/config-list
	//0022_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener/interface-configuration/interface-list
	//0023_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/status-info
	//0024_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/listener
	//0025_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/status-info
	//0026_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream/failed-interfaces
	//0027_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc/stream
	//0028_ieee802-dot1q-tsn-config-uni/tsn-uni/domain/cuc
	//0029_ieee802-dot1q-tsn-config-uni/tsn-uni/domain
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
	for(i=0;i<8;i++){
		if(kvs[i]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[i]);}
	}
	return res;
}

