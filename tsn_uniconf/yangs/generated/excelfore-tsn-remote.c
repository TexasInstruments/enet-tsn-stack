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

int excelfore_tsn_remote_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t aps[14];
	int i;
	int res=-1;
	uint32_t vsize=0;
	void *kvs[8]={NULL};
	uint8_t kss[8];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,YANG_DB_ONHW_ALWAYS,
	                              NULL,aps,kvs,kss,NULL,0};
	dbpara.atype=YANG_DB_ACTION_READ;
	aps[0] = EXCELFORE_TSN_REMOTE_RO;
	aps[1] = EXCELFORE_TSN_REMOTE_VALUEKEY;
	aps[2] = 255;
	if(yang_db_action(dbald, hwald, &dbpara)==0){
		if(dbpara.vsize==1 && (*(uint8_t*)dbpara.value==1)){return 0;}
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, hwald, &dbpara);
	dbpara.atype=YANG_DB_ACTION_CREATE;
	//0000_excelfore-tsn-remote/tsn-remote/instances/instance/update
	//0001_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ta/indication-list
	//0002_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ta/admin-request-list
	//0003_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ta/oper-request-list
	//0004_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf
	//0005_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf/indication-list
	//0006_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf/admin-request-list
	//0007_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-tf/oper-request-list
	//0008_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te
	//0009_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/stream-rank
	//0010_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/end-station-interfaces
	//0011_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification/ieee802-mac-addresses
	//0012_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification/ieee802-vlan-tag
	//0013_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/data-frame-specification
	//0014_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification/interval
	//0015_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification/time-aware
	//0016_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/traffic-specification
	//0017_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/user-to-network-requirements
	//0018_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-capabilities
	//0019_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0020_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0021_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list/config-list
	//0022_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/interface-configuration/interface-list
	//0023_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/status-info
	//0024_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/status-info
	//0025_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list/failed-interfaces
	//0026_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/indication-list
	//0027_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/stream-rank
	//0028_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/end-station-interfaces
	//0029_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ieee802-mac-addresses
	//0030_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification/ieee802-vlan-tag
	//0031_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/data-frame-specification
	//0032_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification/interval
	//0033_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification/time-aware
	//0034_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/traffic-specification
	//0035_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/user-to-network-requirements
	//0036_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-capabilities
	//0037_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0038_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0039_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list/config-list
	//0040_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/interface-configuration/interface-list
	//0041_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/status-info
	//0042_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/status-info
	//0043_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list/failed-interfaces
	//0044_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/admin-request-list
	//0045_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/stream-rank
	//0046_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/end-station-interfaces
	//0047_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ieee802-mac-addresses
	//0048_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification/ieee802-vlan-tag
	//0049_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/data-frame-specification
	//0050_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification/interval
	//0051_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification/time-aware
	//0052_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/traffic-specification
	//0053_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/user-to-network-requirements
	//0054_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-capabilities
	//0055_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0056_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0057_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list/config-list
	//0058_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/interface-configuration/interface-list
	//0059_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/status-info
	//0060_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/status-info
	//0061_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list/failed-interfaces
	//0062_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-te/oper-request-list
	//0063_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls
	//0064_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/indication-list
	//0065_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/admin-request-list
	//0066_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-ls/oper-request-list
	//0067_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le
	//0068_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/end-station-interfaces
	//0069_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/user-to-network-requirements
	//0070_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-capabilities
	//0071_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0072_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0073_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list/config-list
	//0074_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/interface-configuration/interface-list
	//0075_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/status-info
	//0076_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/status-info
	//0077_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list/failed-interfaces
	//0078_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/indication-list
	//0079_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/end-station-interfaces
	//0080_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/user-to-network-requirements
	//0081_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-capabilities
	//0082_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0083_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0084_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list/config-list
	//0085_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/interface-configuration/interface-list
	//0086_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/status-info
	//0087_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/status-info
	//0088_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list/failed-interfaces
	//0089_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/admin-request-list
	//0090_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/end-station-interfaces
	//0091_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/user-to-network-requirements
	//0092_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-capabilities
	//0093_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ieee802-mac-addresses
	//0094_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list/ieee802-vlan-tag
	//0095_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list/config-list
	//0096_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/interface-configuration/interface-list
	//0097_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/status-info
	//0098_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/status-info
	//0099_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list/failed-interfaces
	//0100_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-le/oper-request-list
	//0101_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm
	//0102_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/indication-list
	//0103_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/admin-request-list
	//0104_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/msrp-dm/oper-request-list
	//0105_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp
	//0106_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/indication-list
	//0107_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/admin-request-list
	//0108_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects/mvrp/oper-request-list
	//0109_excelfore-tsn-remote/tsn-remote/instances/instance/external-control-man/control-objects
	//0110_excelfore-tsn-remote/tsn-remote/instances/instance
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
	for(i=0;i<7;i++){
		if(kvs[i]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[i]);}
	}
	return res;
}
