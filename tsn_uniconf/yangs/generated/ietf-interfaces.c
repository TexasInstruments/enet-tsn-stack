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
#include "ietf-interfaces.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);
YANG_NODE_CACHED_MODULE_GET_ENUM_DEF(IETF_INTERFACES, "ietf-interfaces");
#define IETF_INTERFACES_RW IETF_INTERFACES_func(dbald)
#define IETF_INTERFACES_RO (IETF_INTERFACES_func(dbald)|0x80u)

int ietf_interfaces_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t aps[9];
	int i;
	int res=-1;
	uint32_t vsize=0;
	void *kvs[3]={NULL};
	uint8_t kss[3];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,YANG_DB_ONHW_ALWAYS,
	                              NULL,aps,kvs,kss,NULL,0};
	if(yang_node_mod_set_enum(dbald, "ietf-interfaces")<0){return -1;};
	dbpara.atype=YANG_DB_ACTION_READ;
	aps[0] = IETF_INTERFACES_RO;
	aps[1] = IETF_INTERFACES_VALUEKEY;
	aps[2] = 255;
	if(yang_db_action(dbald, hwald, &dbpara)==0){
		if(dbpara.vsize==1 && (*(uint8_t*)dbpara.value==1)){return 0;}
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, hwald, &dbpara);
	dbpara.atype=YANG_DB_ACTION_CREATE;
	//0000_ietf-interfaces/interfaces/interface/bridge-port/priority-regeneration
	//0001_ietf-interfaces/interfaces/interface/bridge-port/traffic-class/traffic-class-table
	//0002_ietf-interfaces/interfaces/interface/bridge-port/traffic-class/tc-data
	//0003_ietf-interfaces/interfaces/interface/bridge-port/traffic-class/pqueue-map
	//0004_ietf-interfaces/interfaces/interface/bridge-port/traffic-class
	//0005_ietf-interfaces/interfaces/interface/bridge-port/transmission-selection-algorithm-table/transmission-selection-algorithm-map
	//0006_ietf-interfaces/interfaces/interface/bridge-port/address
	//0007_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/queue-max-sdu-table
	//0008_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-control-list/gate-control-entry
	//0009_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-control-list
	//0010_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-control-list/gate-control-entry
	//0011_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-cycle-time
	//0012_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-cycle-time
	//0013_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-cycle-time
	//0014_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/admin-base-time
	//0015_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/oper-base-time
	//0016_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/config-change-time
	//0017_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/current-time
	//0018_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table/supported-cycle-max
	//0019_ietf-interfaces/interfaces/interface/bridge-port/gate-parameter-table
	//0020_ietf-interfaces/interfaces/interface/bridge-port/frame-preemption-parameters/frame-preemption-status-table
	//0021_ietf-interfaces/interfaces/interface/bridge-port/frame-preemption-parameters
	//0022_ietf-interfaces/interfaces/interface/bridge-port
	//0023_ietf-interfaces/interfaces/interface
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
	for(i=0;i<2;i++){
		if(kvs[i]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[i]);}
	}
	return res;
}

