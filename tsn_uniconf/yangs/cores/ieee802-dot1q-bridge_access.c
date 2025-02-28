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
#include <tsn_combase/combase.h>
#include "yang_node.h"
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

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *qb_get_bridge_name(uc_dbald *dbald, uint8_t instIndex)
{
	uint8_t aps1[5]={IEEE802_DOT1Q_BRIDGE_RO,
		IEEE802_DOT1Q_BRIDGE_BRIDGES,
		IEEE802_DOT1Q_BRIDGE_BRIDGE,
		IEEE802_DOT1Q_BRIDGE_COMPONENT,
		IEEE802_DOT1Q_BRIDGE_PORTS};
	uint8_t aps2[5];
	uc_range *range;
	void *kdata;
	uint32_t i, ksize;
	uint8_t nsize;
	const char *nstr, *cstr;
	char mycompname[6]=QBRIDGE_COMP_NAME;
	(void)ub_bytearray2str(&mycompname[3], &instIndex, 1);

	memcpy(aps2, aps1, 5);
	aps2[4]+=1;
	range=uc_get_range(dbald, aps1, 5, aps2, 5);
	if(!range){return NULL;}
	while(true){
		nstr=NULL;
		if(uc_get_key_in_range(dbald, range, &kdata, &ksize,
				       UC_DBAL_FORWARD) != 0){break;}
		for(i=0;i<ksize;i++){
			if(((uint8_t*)kdata)[i]==255u){break;};
		}
		if(i==ksize){continue;}
		nsize=((uint8_t*)kdata)[i+1];
		nstr=&((const char*)kdata)[i+2];
		if(nsize!=strlen(nstr)+1){continue;}
		if(ksize<=i+3+nsize){continue;}
		cstr=&((const char*)kdata)[i+3+nsize];
		if(strcmp(cstr, mycompname)!=0){continue;}
		break;
	}
	uc_get_range_release(dbald, range);
	return nstr;
}

static void set_dpara_k1noinst(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    const char* bridgename, uint8_t k1, bool status, bool ismirror)
{
	uint8_t apsidx=0u, kvsidx=0u;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	if(ismirror) {
		dbpara->aps[apsidx++]=XL4_DATA_RW;
		dbpara->aps[apsidx++]=UC_MIRROR;
		dbpara->aps[apsidx++]=UC_MIRROR_DEVICE;
	}
	dbpara->aps[apsidx++] = status?IEEE802_DOT1Q_BRIDGE_RO:IEEE802_DOT1Q_BRIDGE_RW;
	dbpara->aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_BRIDGES;
	dbpara->aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_BRIDGE;
	dbpara->aps[apsidx++] = k1;
	dbpara->aps[apsidx++] = 255u;
	if(ismirror) {
		//uc-mirror-device name is usually the same as bridge-name
		dbpara->kvs[kvsidx]=(void*)bridgename;
		dbpara->kss[kvsidx++]=strlen(bridgename)+1u;
	}
	dbpara->kvs[kvsidx]   = (void*)bridgename;
	dbpara->kss[kvsidx++] = strlen(bridgename)+1u;
	dbpara->kvs[kvsidx]   = NULL;
	dbpara->kss[kvsidx++] = 0u;
}

static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    const char* bridgename, uint8_t instIndex,
			    uint8_t k1, bool status, bool ismirror)
{
	uint8_t apsidx=0u, kvsidx=0u;
	if(bridgename==NULL){
		bridgename=qb_get_bridge_name(dbald, instIndex);
		if(bridgename==NULL){
			bridgename=DOT1Q_DEFAULT_BRIDGE_NAME;
		}
	}
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	if(ismirror) {
		dbpara->aps[apsidx++]=XL4_DATA_RW;
		dbpara->aps[apsidx++]=UC_MIRROR;
		dbpara->aps[apsidx++]=UC_MIRROR_DEVICE;
	}
	dbpara->aps[apsidx++] = status?IEEE802_DOT1Q_BRIDGE_RO:IEEE802_DOT1Q_BRIDGE_RW;
	dbpara->aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_BRIDGES;
	dbpara->aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_BRIDGE;
	dbpara->aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_COMPONENT;
	dbpara->aps[apsidx++] = k1;
	dbpara->aps[apsidx++] = 255u;
	if(ismirror) {
		//uc-mirror-device name is usually the same as bridge-name
		dbpara->kvs[kvsidx]=(void*)bridgename;
		dbpara->kss[kvsidx++]=strlen(bridgename)+1u;
	}
	dbpara->kvs[kvsidx]   = (void*)bridgename;
	dbpara->kss[kvsidx++] = strlen(bridgename)+1u;
	(void)ub_bytearray2str(&compname[3], &instIndex, 1);
	dbpara->kvs[kvsidx]   = compname;
	dbpara->kss[kvsidx++] = strlen(compname)+1u;
	dbpara->kvs[kvsidx]   = NULL;
	dbpara->kss[kvsidx++] = 0u;
}

void ydbi_set_dbpara_qbk1vk0(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint8_t k1, bool status, bool ismirror)
{
	return set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename,
			instIndex, k1, status, ismirror);
}

int ydbi_get_item_qbk1vk0(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_NON_MIRROR);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_NON_MIRROR);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status, void *value, uint32_t vsize,
			  uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_NON_MIRROR);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_NON_MIRROR);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

void ydbi_set_dbpara_qbk1noinst(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t k1, bool status, bool ismirror)
{
	return set_dpara_k1noinst(ydbia->dbald, &ydbia->dbpara, bridgename,
			k1, status, ismirror);
}

int ydbi_get_item_qbk1noinst(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1noinst(ydbia->dbald, &ydbia->dbpara, bridgename, k1,
			status, YDBI_NON_MIRROR);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_qbk1noinst(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1noinst(ydbia->dbald, &ydbia->dbpara, bridgename, k1,
			status, YDBI_NON_MIRROR);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_qbk1noinst(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1noinst(ydbia->dbald, &ydbia->dbpara, bridgename, k1,
			status, YDBI_NON_MIRROR);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_qbk1noinst(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1noinst(ydbia->dbald, &ydbia->dbpara, bridgename, k1,
			status, YDBI_NON_MIRROR);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_qbk1vk0m(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_MIRROR);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_set_item_qbk1vk0m(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status, void *value, uint32_t vsize,
			  uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_MIRROR);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_qbk1vk0m(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_MIRROR);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_rel_item_qbk1vk0m(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1,
			status, YDBI_MIRROR);
	return ydbi_rel_foot(ydbia, __func__);
}

static void set_dpara_vlan_reg(yang_db_item_access_t *ydbia,
			       const char* bridgename, uint8_t instIndex,
			       uint32_t *database_id, uint16_t vids[4],
			       uint32_t *port_ref, bool ismirror)
{
	uint8_t apsidx=ismirror ? 8u : 5u;
	uint8_t kvsidx=ismirror ? 3u : 2u;
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex,
			IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE, YDBI_CONFIG, ismirror);
	ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_VLAN_REGISTRATION_ENTRY;
	ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_PORT_MAP;
	ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_VLAN_REGISTRATION_ENTRIES;
	ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT;
	ydbia->dbpara.aps[apsidx++] = 255;
	ydbia->dbpara.kvs[kvsidx]   = database_id;
	ydbia->dbpara.kss[kvsidx++] = sizeof(uint32_t);
	ydbia->dbpara.kvs[kvsidx]   = vids;
	ydbia->dbpara.kss[kvsidx++] = sizeof(uint16_t)*4;
	ydbia->dbpara.kvs[kvsidx]   = port_ref;
	ydbia->dbpara.kss[kvsidx++] = sizeof(uint32_t);
	ydbia->dbpara.kvs[kvsidx]   = NULL;
	ydbia->dbpara.kss[kvsidx++] = 0u;
}

static void set_dpara_filtering_entry(yang_db_item_access_t *ydbia,
			       bool istalker, const char* bridgename, uint8_t instIndex,
			       uint32_t *database_id, uint16_t vids[4], ub_macaddr_t destmac,
			       uint32_t *port_ref, bool ismirror)
{
	uint8_t apsidx=ismirror ? 8u : 5u;
	uint8_t kvsidx=ismirror ? 3u : 2u;
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex,
			IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE, YDBI_CONFIG, ismirror);
	ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_FILTERING_ENTRY;
	ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_PORT_MAP;
	if(istalker){
		ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_RESERVATION_ENTRIES;
	}else{
		ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_DYNAMIC_FILTERING_ENTRIES;
	}
	ydbia->dbpara.aps[apsidx++] = IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT;
	ydbia->dbpara.aps[apsidx++] = 255;
	ydbia->dbpara.kvs[kvsidx]   = destmac;
	ydbia->dbpara.kss[kvsidx++] = 6;
	ydbia->dbpara.kvs[kvsidx]   = database_id;
	ydbia->dbpara.kss[kvsidx++] = sizeof(uint32_t);
	ydbia->dbpara.kvs[kvsidx]   = vids;
	ydbia->dbpara.kss[kvsidx++] = sizeof(uint16_t)*4;
	ydbia->dbpara.kvs[kvsidx]   = port_ref;
	ydbia->dbpara.kss[kvsidx++] = sizeof(uint32_t);
	ydbia->dbpara.kvs[kvsidx]   = NULL;
	ydbia->dbpara.kss[kvsidx++] = 0u;
}

void ydbi_set_dbpara_vlan_reg(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint16_t vid1, uint16_t vid2, uint32_t port_ref,
			  bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid1, vid2, 0, 0};
	return set_dpara_vlan_reg(ydbia, bridgename, instIndex, &database_id,
			vids, &port_ref, ismirror);
}

void ydbi_set_dbpara_filtering_entry(yang_db_item_access_t *ydbia, bool istalker,
			  const char* bridgename, uint8_t instIndex, uint16_t vid1,
			  uint16_t vid2, ub_macaddr_t destmac, uint32_t port_ref,
			  bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid1, vid2, 0, 0};
	return set_dpara_filtering_entry(ydbia, istalker, bridgename, instIndex,
			&database_id, vids, destmac, &port_ref, ismirror);
}

int ydbi_vlan_regis_qb_ext(yang_db_item_access_t *ydbia, const char *bridgename,
		       uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
		       uint32_t *port_ref, uint32_t *value, bool reg, bool ismirror)
{
	if(reg){
		if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	}else{
		if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	}
	set_dpara_vlan_reg(ydbia, bridgename, instIndex, database_id, vids,
	        port_ref, ismirror);
	ydbia->dbpara.value = value;
	ydbia->dbpara.vsize = sizeof(uint32_t);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_PUSH_NOTICE);
}

int ydbi_vlan_regis_qb(yang_db_item_access_t *ydbia,
		       const char* bridgename, uint8_t instIndex,
		       uint16_t vid1, uint16_t vid2, uint32_t port_ref,
		       bool reg, bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid1, vid2, 0, 0};
	uint32_t vi=0; // control-element, enum registered=0
	UB_LOG(UBL_DEBUG, "%s:vid1=%d, vid2=%d, port_ref=%d, reg=%d\n",
	       __func__, vid1, vid2, port_ref, reg);
	return ydbi_vlan_regis_qb_ext(ydbia, bridgename, instIndex, &database_id,
	        vids, &port_ref, &vi, reg, ismirror);
}

bool ydbi_vlan_check_qb_ext(yang_db_item_access_t *ydbia, const char *bridgename,
			uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
			uint32_t *port_ref, bool ismirror)
{
	void *vi;
	int res;
	if(ydbi_get_head(ydbia, __func__)!=0){return false;}
	set_dpara_vlan_reg(ydbia, bridgename, instIndex, database_id, vids,
	        port_ref, ismirror);
	res=ydbi_get_foot(ydbia, __func__, &vi, UBL_INFO);
	if(res<0){return false;}
	if(ydbi_rel_head(ydbia, __func__)!=0){return false;}
	ydbi_rel_foot(ydbia, __func__);
	UB_LOG(UBL_DEBUGV, "%s:vid1=%d, vid2=%d, port_ref=%d, res=%d\n",
	       __func__, vids[0], vids[1], *port_ref, res);
	return res==sizeof(uint32_t);
}

bool ydbi_vlan_check_qb(yang_db_item_access_t *ydbia,
			const char* bridgename, uint8_t instIndex,
			uint16_t vid1, uint16_t vid2, uint32_t port_ref, bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid1, vid2, 0, 0};
	return ydbi_vlan_check_qb_ext(ydbia, bridgename, instIndex, &database_id,
	        vids, &port_ref, ismirror);
}

// read "/ieee802-dot1q-bridge/bridges/bridge|name:|/component|name:|/ports"
int ydbi_bridge_ports_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			 uint8_t instIndex, bool ismirror)
{
	void *vi;
	int res;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex,
			IEEE802_DOT1Q_BRIDGE_PORTS, YDBI_STATUS, ismirror);
	res=ydbi_get_foot(ydbia, __func__, &vi, UBL_INFO);
	if(res<0){return -1;}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	if(res!=sizeof(uint16_t)){return -1;}
	return *((uint16_t *)vi);
}

void qb_builtin_close(uc_dbald *dbald, uc_hwald *hwald)
{
	uc_range *range;
	uint8_t key1[5]={IEEE802_DOT1Q_BRIDGE_RO, IEEE802_DOT1Q_BRIDGE_BRIDGES,
		IEEE802_DOT1Q_BRIDGE_BRIDGE,IEEE802_DOT1Q_BRIDGE_COMPONENT,
		IEEE802_DOT1Q_BRIDGE_PORTS};
	uint8_t key2[5];
	void *kdata;
	uint32_t ksize;
	uint8_t *aap;
	void *akvs[UC_MAX_KV_DEPTH+1];
	uint8_t akss[UC_MAX_KV_DEPTH];
	memcpy(key2, key1, 5);
	key2[4]+=1;
	range=uc_get_range(dbald, key1, 5, key2, 5);
	if(!range){return;}
	while(true){
		if(uc_get_key_in_range(dbald, range, &kdata, &ksize,
				       UC_DBAL_FORWARD) != 0){break;}
		if(yang_db_extract_key(kdata, ksize, &aap, akvs, akss)){break;}
		uc_hwal_dereghw(hwald, aap, akvs, akss);
		yang_db_extract_key_free(aap, akvs, akss);
	}
	uc_get_range_release(dbald, range);
	return;
}

static int ydbi_talker_listener_qb(bool istalker, yang_db_item_access_t *ydbia,
				   const char* bridgename, uint8_t instIndex, uint32_t *dbase_id,
				   uint16_t vids[4], ub_macaddr_t destmac, uint32_t *port_ref,
				   uint32_t *value, bool reg, bool ismirror)
{
	if(reg){
		if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	}else{
		if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	}
	set_dpara_filtering_entry(ydbia, istalker, bridgename, instIndex,
			dbase_id, vids, destmac, port_ref, ismirror);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=sizeof(uint32_t);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO,
			istalker?YDBI_NO_NOTICE:YDBI_PUSH_NOTICE);
}

// '.../filtering-entry/.../dynamic-filtering-entries/control-element'
// uniconf gets a notice
int ydbi_listener_qb(yang_db_item_access_t *ydbia,
		     const char* bridgename, uint8_t instIndex,
		     ub_macaddr_t destmac, uint16_t vid,
		     uint32_t port_ref, bool reg, bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid, vid, 0, 0};
	uint32_t ctrl_element=0u;
	return ydbi_talker_listener_qb(false, ydbia, bridgename, instIndex,
				       &database_id, vids, destmac, &port_ref, &ctrl_element,
				       reg, ismirror);
}

int ydbi_listener_qb_ext(yang_db_item_access_t *ydbia, const char* bridgename,
		     uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
		     ub_macaddr_t destmac, uint32_t *port_ref, uint32_t *value,
		     bool reg, bool ismirror)
{
	return ydbi_talker_listener_qb(false, ydbia, bridgename, instIndex,
				       database_id, vids, destmac, port_ref, value,
				       reg, ismirror);
}

bool ydbi_listener_check_qb_ext(yang_db_item_access_t *ydbia, const char *bridgename,
			uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
			ub_macaddr_t destmac, uint32_t *port_ref, bool ismirror)
{
	void *vi;
	int res;
	if(ydbi_get_head(ydbia, __func__)!=0){return false;}
	set_dpara_filtering_entry(ydbia, false, bridgename, instIndex,
	        database_id, vids, destmac, port_ref, ismirror);
	res=ydbi_get_foot(ydbia, __func__, &vi, UBL_INFO);
	if(res<0){return false;}
	if(ydbi_rel_head(ydbia, __func__)!=0){return false;}
	ydbi_rel_foot(ydbia, __func__);
	return res==sizeof(uint32_t);
}

bool ydbi_listener_check_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			uint8_t instIndex, ub_macaddr_t destmac, uint16_t vid,
			uint32_t port_ref, bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid, vid, 0, 0};
	return ydbi_listener_check_qb_ext(ydbia, bridgename, instIndex, &database_id,
	        vids, destmac, &port_ref, ismirror);
}

// '.../filtering-entry/.../dynamic-reservation-entries/control-element'
// uniconf doesn't get a notice
int ydbi_talker_qb(yang_db_item_access_t *ydbia,
		   const char* bridgename, uint8_t instIndex,
		   ub_macaddr_t destmac, uint16_t vid, uint8_t prir,
		   uint32_t port_ref, bool reg, bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid, vid, 0, 0};
	uint32_t ctrl_element=prir;
	return ydbi_talker_listener_qb(true, ydbia, bridgename, instIndex,
				       &database_id, vids, destmac, &port_ref, &ctrl_element,
				       reg, ismirror);
}

int ydbi_talker_qb_ext(yang_db_item_access_t *ydbia, const char* bridgename,
		   uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
		   ub_macaddr_t destmac, uint32_t *port_ref, uint32_t *value,
		   bool reg, bool ismirror)
{
	return ydbi_talker_listener_qb(true, ydbia, bridgename, instIndex,
				       database_id, vids, destmac, port_ref, value,
				       reg, ismirror);
}

bool ydbi_talker_check_qb_ext(yang_db_item_access_t *ydbia, const char *bridgename,
			uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
			ub_macaddr_t destmac, uint32_t *port_ref, bool ismirror)
{
	void *vi;
	int res;
	if(ydbi_get_head(ydbia, __func__)!=0){return false;}
	set_dpara_filtering_entry(ydbia, true, bridgename, instIndex,
	        database_id, vids, destmac, port_ref, ismirror);
	res=ydbi_get_foot(ydbia, __func__, &vi, UBL_INFO);
	if(res<0){return false;}
	if(ydbi_rel_head(ydbia, __func__)!=0){return false;}
	ydbi_rel_foot(ydbia, __func__);
	return res==sizeof(uint32_t);
}

bool ydbi_talker_check_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			uint8_t instIndex, ub_macaddr_t destmac, uint16_t vid,
			uint32_t port_ref, bool ismirror)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uint16_t vids[4]={vid, vid, 0, 0};
	return ydbi_talker_check_qb_ext(ydbia, bridgename, instIndex, &database_id,
	        vids, destmac, &port_ref, ismirror);
}

int qb_get_talker_port_prir(uc_dbald *dbald,
			    const char* bridgename, const char* compname,
			    ub_macaddr_t destmac, uint16_t vid,
			    dq_port_prir_t **port_prir, int *ppsize)
{
	uint32_t database_id=QBRIDGE_FDB_ID;
	uc_range *range;
	uint8_t key1[9]={IEEE802_DOT1Q_BRIDGE_RW, IEEE802_DOT1Q_BRIDGE_BRIDGES,
		IEEE802_DOT1Q_BRIDGE_BRIDGE, IEEE802_DOT1Q_BRIDGE_COMPONENT,
		IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE,
		IEEE802_DOT1Q_BRIDGE_FILTERING_ENTRY,
		IEEE802_DOT1Q_BRIDGE_PORT_MAP,
		IEEE802_DOT1Q_BRIDGE_DYNAMIC_RESERVATION_ENTRIES,
		IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT};
	uint8_t key2[9];
	void *kdata;
	uint32_t ksize;
	uint8_t *aap;
	void *akvs[UC_MAX_KV_DEPTH+1];
	uint8_t akss[UC_MAX_KV_DEPTH];
	int res=-1;
	uint16_t vids[4];
	bool skip=false;
	void *vdata;
	uint32_t vsize;
	memcpy(key2, key1, 9);
	key2[8]+=1;
	range=uc_get_range(dbald, key1, 9, key2, 9);
	if(!range){return -1;}
	if(compname==NULL){compname=QBRIDGE_COMP_NAME;}
	*port_prir=NULL;
	*ppsize=0;
	while(true){
		if(uc_get_keyvalue_in_range(dbald, range, &kdata, &ksize,
					    &vdata, &vsize,
					    UC_DBAL_FORWARD) != 0){break;}
		if(yang_db_extract_key(kdata, ksize, &aap, akvs, akss)){break;}
		skip=(strncmp(bridgename, (const char *)akvs[0], akss[0])!=0);
		if(!skip){
			skip=(strncmp(compname, (const char *)akvs[1], akss[1])!=0);
		}
		if(!skip){
			skip=(memcmp(destmac, akvs[2], 6)!=0);
		}
		if(!skip){
			skip=(database_id!=*((uint32_t *)akvs[3]));
		}
		if(!skip){
			ub_non_aligned_intsubst(akvs[4], vids, sizeof(uint16_t)*4);
			skip=(vids[0]!=vid);
		}
		if(!skip){
			skip=(vsize!=sizeof(uint32_t));
		}
		if(!skip){
			*ppsize+=1;
			*port_prir=(dq_port_prir_t*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *port_prir,
						  *ppsize*sizeof(dq_port_prir_t));
			if(ub_assert_fatal(*port_prir!=NULL, __func__, NULL)){break;}
			ub_non_aligned_intsubst(akvs[5], &(*port_prir)[*ppsize-1].port,
						sizeof(uint32_t));
			ub_non_aligned_intsubst(vdata, &(*port_prir)[*ppsize-1].prir,
						sizeof(uint32_t));
			res=0;
		}
		yang_db_extract_key_free(aap, akvs, akss);
	}
	uc_get_range_release(dbald, range);
	return res;
}

int ydbi_port_name2ref_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, const char* name, bool ismirror)
{
	void *vi;
	int res;
	int i;
	char *pi;
	int port_ref=0;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex,
			IEEE802_DOT1Q_BRIDGE_BRIDGE_PORT, YDBI_STATUS, ismirror);
	res=ydbi_get_foot(ydbia, __func__, &vi, UBL_INFO);
	if(res<0){return -1;}
	pi=(char*)vi;
	for(i=0;i<res;i++){
		if(((char*)vi)[i]!=0){continue;}
		port_ref++;
		if(strcmp(pi, name)==0){break;}
		pi=(char*)vi+i+1;
	}
	if(i==res){port_ref=0;}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return port_ref;
}

int ydbi_port_ref2name_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint16_t port_ref, char name[CB_MAX_NETDEVNAME],
			  bool ismirror)
{
	void *vi;
	int res;
	int i;
	char *pi;
	int ref=0;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex,
			IEEE802_DOT1Q_BRIDGE_BRIDGE_PORT, YDBI_STATUS, ismirror);
	res=ydbi_get_foot(ydbia, __func__, &vi, UBL_INFO);
	if(res<0){return -1;}
	pi=(char*)vi;
	for(i=0;i<res;i++){
		if(((char*)vi)[i]!=0){continue;}
		ref++;
		if(ref==port_ref){
			ub_strncpy(name, pi, CB_MAX_NETDEVNAME);
			break;
		}
		pi=(char*)vi+i+1;
	}
	if(i==res){
		res=1;
	}else{
		res=0;
	}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return res;
}
