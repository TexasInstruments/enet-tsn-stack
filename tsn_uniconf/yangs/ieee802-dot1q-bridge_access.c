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
#include <tsn_unibase/unibase.h>
#include "yang_modules.h"
#include "yang_db_access.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1q-bridge_access.h"

extern uint8_t IEEE802_DOT1Q_BRIDGE_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_BRIDGE_RW IEEE802_DOT1Q_BRIDGE_func(dbald)
#define IEEE802_DOT1Q_BRIDGE_RO (IEEE802_DOT1Q_BRIDGE_func(dbald)|0x80u)

#define QBRIDGE_COMP_NAME "cmp00"
#define QBRIDGE_FDB_ID 0
static char compname[6]=QBRIDGE_COMP_NAME;

static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    const char* bridgename, uint8_t instIndex,
			    uint8_t k1, bool status)
{
	if(bridgename==NULL){bridgename=DOT1Q_DEFAULT_BRIDGE_NAME;}
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1Q_BRIDGE_RO:IEEE802_DOT1Q_BRIDGE_RW;
	dbpara->aps[1] = IEEE802_DOT1Q_BRIDGE_BRIDGES;
	dbpara->aps[2] = IEEE802_DOT1Q_BRIDGE_BRIDGE;
	dbpara->aps[3] = IEEE802_DOT1Q_BRIDGE_COMPONENT;
	dbpara->aps[4] = k1;
	dbpara->aps[5] = 255u;
	dbpara->kvs[0]=(void*)bridgename;
	dbpara->kss[0]=strlen(bridgename)+1u;
	(void)ub_bytearray2str(&compname[3], &instIndex, 1);
	dbpara->kvs[1]=compname;
	dbpara->kss[1]=strlen(compname)+1u;
	dbpara->kvs[2]=NULL;
}

int ydbi_get_item_qbk1vk0(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status, void *value, uint32_t vsize,
			  uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

static void set_dpara_vlan_reg(yang_db_item_access_t *ydbia,
			       const char* bridgename, uint8_t instIndex,
			       uint32_t *database_id, uint16_t *vids, const char *netdev)
{
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex,
			IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE, YDBI_CONFIG);
	ydbia->dbpara.aps[5] = IEEE802_DOT1Q_BRIDGE_VLAN_REGISTRATION_ENTRY;
	ydbia->dbpara.aps[6] = IEEE802_DOT1Q_BRIDGE_PORT_MAP;
	ydbia->dbpara.aps[7] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_VLAN_REGISTRATION_ENTRIES;
	ydbia->dbpara.aps[8] = IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT;
	ydbia->dbpara.aps[9] = 255;
	ydbia->dbpara.kvs[2] = database_id;
	ydbia->dbpara.kss[2] = sizeof(uint32_t);
	ydbia->dbpara.kvs[3] = vids;
	ydbia->dbpara.kss[3] = sizeof(uint16_t)*4;
	ydbia->dbpara.kvs[4] = (char*)netdev;
	if(netdev){ydbia->dbpara.kss[4] = strlen(netdev);}
	ydbia->dbpara.kvs[5] = NULL;
}

int ydbi_vlan_regis_qb(yang_db_item_access_t *ydbia,
		       const char* bridgename, uint8_t instIndex,
		       uint16_t vid1, uint16_t vid2, const char *netdev, bool reg)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid1, vid2, 0, 0};
	uint32_t vi=0; // control-element, enum registered=0
	if(reg){
		if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	}else{
		if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	}
	set_dpara_vlan_reg(ydbia, bridgename, instIndex, &database_id, vids, netdev);
	ydbia->dbpara.value = &vi;
	ydbia->dbpara.vsize = sizeof(uint32_t);
	UB_LOG(UBL_DEBUG, "%s:vid1=%d, vid2=%d, netdev=%s, reg=%d\n",
	       __func__, vid1, vid2, netdev, reg);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_PUSH_NOTICE);
}

bool ydbi_vlan_check_qb(yang_db_item_access_t *ydbia,
			const char* bridgename, uint8_t instIndex,
			uint16_t vid1, uint16_t vid2, const char *netdev)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid1, vid2, 0, 0};
	void *vi;
	int res;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_vlan_reg(ydbia, bridgename, instIndex, &database_id, vids, netdev);
	res=ydbi_get_foot(ydbia, __func__, &vi, UBL_INFO);
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	UB_LOG(UBL_DEBUGV, "%s:vid1=%d, vid2=%d, netdev=%s, res=%d\n",
	       __func__, vid1, vid2, netdev, res);
	return res==sizeof(uint32_t);
}
