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
#include "ieee802-dot1q-tsn-config-uni.h"

extern uint8_t IEEE802_DOT1Q_TSN_CONFIG_UNI_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RW IEEE802_DOT1Q_TSN_CONFIG_UNI_func(dbald)
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RW_Y IEEE802_DOT1Q_TSN_CONFIG_UNI_func(ydbia->dbald)
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RO (IEEE802_DOT1Q_TSN_CONFIG_UNI_func(dbald)|0x80u)

#define TSNUNI_CUC_ID "cuc0" // multiple-CUC is out of the scope
#define TSNUNI_DOMAIN_ID "domain00" // replace this '00' to instanceIndex
static char cuc_id[]=TSNUNI_CUC_ID;
static char domain_id[]=TSNUNI_DOMAIN_ID;

static void set_dpara_k0vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t instIndex, uint8_t *streamid, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1Q_TSN_CONFIG_UNI_RO:
		IEEE802_DOT1Q_TSN_CONFIG_UNI_RW;
	dbpara->aps[1] = IEEE802_DOT1Q_TSN_CONFIG_UNI_TSN_UNI;
	dbpara->aps[2] = IEEE802_DOT1Q_TSN_CONFIG_UNI_DOMAIN;
	dbpara->aps[3] = IEEE802_DOT1Q_TSN_CONFIG_UNI_CUC;
	dbpara->aps[4] = IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM;
	dbpara->aps[5] = 255u;
	dbpara->kvs[0]=domain_id;
	dbpara->kss[0]=strlen(domain_id)+1u;
	(void)ub_bytearray2str(&domain_id[6], &instIndex, 1);
	dbpara->kvs[1]=cuc_id;
	dbpara->kss[1]=strlen(cuc_id)+1u;
	dbpara->kvs[2]=streamid;
	dbpara->kss[2]=8;
	dbpara->kvs[3]=NULL;

}

// this call set/reset 'priority-code-point' and 'vlan-id'
// uniconf gets a notice with 'vlan-id'
int ydbi_listener_tu(yang_db_item_access_t *ydbia, uint8_t instIndex, uint8_t *streamid,
		     uint32_t ltindex, const char *netdev, uint32_t cfindex,
		     uint8_t pcp, uint16_t vid, bool reg)
{
	uint8_t zmac[6]={0,};
	if(reg){
		if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	}else{
		if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CONFIGURATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_LIST;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_TSN_CONFIG_UNI_CONFIG_LIST;
	ydbia->dbpara.aps[9]=IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_VLAN_TAG;
	ydbia->dbpara.aps[10]=IEEE802_DOT1Q_TSN_CONFIG_UNI_PRIORITY_CODE_POINT;
	ydbia->dbpara.aps[11]=255u;
	ydbia->dbpara.kvs[3]=&ltindex;
	ydbia->dbpara.kss[3]=sizeof(uint32_t);
	ydbia->dbpara.kvs[4]=zmac;
	ydbia->dbpara.kss[4]=6;
	ydbia->dbpara.kvs[5]=(char*)netdev;
	ydbia->dbpara.kss[5]=strlen(netdev)+1;
	ydbia->dbpara.kvs[6]=&cfindex;
	ydbia->dbpara.kss[6]=sizeof(uint32_t);
	ydbia->dbpara.kvs[7]=NULL;
	ydbia->dbpara.value=&pcp;
	ydbia->dbpara.vsize=sizeof(uint8_t);
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)){
		UB_LOG(UBL_INFO, "%s:can't updte pcp\n", __func__);
		(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
		return -1;
	}
	ydbia->dbpara.value=&vid;
	ydbia->dbpara.vsize=sizeof(uint16_t);
	ydbia->dbpara.aps[10]=IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_ID;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_PUSH_NOTICE);
}

// this call set/reset 'destination-mac-address' and 'source-mac-address'
// uniconf gets a notice with 'destination-mac-address'
int ydbi_talker_tu(yang_db_item_access_t *ydbia, uint8_t instIndex, ub_bytearray8_t streamid,
		   const char *netdev, uint32_t cfindex,
		   ub_bytearray6_t smac, ub_bytearray6_t dmac, bool reg)
{
	uint8_t zmac[6]={0,};
	if(!dmac){
		UB_LOG(UBL_ERROR, "%s:dmac is null\n", __func__);
		return -1;
	}
	if(!smac){smac=zmac;}
	if(reg){
		if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	}else{
		if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CONFIGURATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_LIST;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_TSN_CONFIG_UNI_CONFIG_LIST;
	ydbia->dbpara.aps[9]=IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_MAC_ADDRESSES;
	ydbia->dbpara.aps[10]=IEEE802_DOT1Q_TSN_CONFIG_UNI_SOURCE_MAC_ADDRESS;
	ydbia->dbpara.aps[11]=255u;
	ydbia->dbpara.kvs[3]=zmac;
	ydbia->dbpara.kss[3]=6;
	ydbia->dbpara.kvs[4]=(char*)netdev;
	ydbia->dbpara.kss[4]=strlen(netdev)+1;
	ydbia->dbpara.kvs[5]=&cfindex;
	ydbia->dbpara.kss[5]=sizeof(uint32_t);
	ydbia->dbpara.kvs[6]=NULL;
	ydbia->dbpara.value=&smac;
	ydbia->dbpara.vsize=6;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)){
		UB_LOG(UBL_INFO, "%s:can't updte smac\n", __func__);
		(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
		return -1;
	}
	ydbia->dbpara.value=dmac;
	ydbia->dbpara.vsize=6;
	ydbia->dbpara.aps[10]=IEEE802_DOT1Q_TSN_CONFIG_UNI_DESTINATION_MAC_ADDRESS;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_PUSH_NOTICE);
}

int ydbi_find_end_station_tu(yang_db_item_access_t *ydbia, uint8_t instIndex,
			     uint8_t count, char t_or_l,
			     uint8_t **stream_id, uint8_t **mac_address, char **ifname)
{
	uc_range *range;
	uint32_t ksize=0;
	void *key;
	uint8_t *p;
	uint32_t i;
	int res=-1;
	uint8_t aps1[5] = {IEEE802_DOT1Q_TSN_CONFIG_UNI_RW_Y,
		IEEE802_DOT1Q_TSN_CONFIG_UNI_TSN_UNI,
		IEEE802_DOT1Q_TSN_CONFIG_UNI_DOMAIN,
		IEEE802_DOT1Q_TSN_CONFIG_UNI_CUC,
		IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM};
	uint8_t aps2[5];
	memcpy(aps2, aps1, 5);
	aps2[4]+=1;
	range=uc_get_range(ydbia->dbald, aps1, 5, aps2, 5);
	if(range==NULL){return -1;}
	while(true){
		if(uc_get_key_in_range(ydbia->dbald, range, &key, &ksize,
				       UC_DBAL_FORWARD)){break;}
		if(((uint8_t*)key)[6]!=IEEE802_DOT1Q_TSN_CONFIG_UNI_END_STATION_INTERFACES){
			continue;
		}
		if(t_or_l=='t' && ((uint8_t*)key)[5]!=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER){
			continue;
		}
		if(t_or_l=='l' && ((uint8_t*)key)[5]!=IEEE802_DOT1Q_TSN_CONFIG_UNI_LISTENER){
			continue;
		}
		if(count-- != 0){continue;}
		// found it
		p=(uint8_t*)key;
		for(i=0;i<ksize;i++){
			if(p[i]==255u){break;}
		}
		i++;
		if(i>=ksize){continue;}
		i+=p[i]-2; // 1st key bottom must be '\0'
		if(i>=ksize){continue;}
		if(instIndex!=strtol((char*)&p[i], NULL, 16)){continue;}
		i+=3; // skip 1st key
		if(i>=ksize){continue;}
		i+=p[i]+1; // skip 2nd key
		if(i>=ksize){continue;}
		*stream_id=&p[i+1];// 3rd key
		i+=p[i]+1;
		if(i>=ksize){continue;}
		*mac_address=&p[i+1];// 4th key
		i+=p[i]+1;
		if(i>=ksize){continue;}
		*ifname=(char*)&p[i+1];// 5th key
		res=0;
		break;
	}
	uc_get_range_release(ydbia->dbald, range);
	return res;
}

int ydbi_get_item_tufspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t dfs_index,
			  uint8_t k1, uint8_t k2)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_DATA_FRAME_SPECIFICATION;
	ydbia->dbpara.aps[7]=k1;
	ydbia->dbpara.aps[8]=k2;
	ydbia->dbpara.aps[9]=255u;
	ydbia->dbpara.kvs[3]=&dfs_index;
	ydbia->dbpara.kvs[4]=NULL;
	ydbia->dbpara.kss[3]=sizeof(uint8_t);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_tufspec(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  ub_bytearray8_t streamid, uint8_t dfs_index,
			  uint8_t k1, uint8_t k2)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_DATA_FRAME_SPECIFICATION;
	ydbia->dbpara.aps[7]=k1;
	ydbia->dbpara.aps[8]=k2;
	ydbia->dbpara.aps[9]=255u;
	ydbia->dbpara.kvs[3]=&dfs_index;
	ydbia->dbpara.kvs[4]=NULL;
	ydbia->dbpara.kss[3]=sizeof(uint8_t);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_tufspec(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  ub_bytearray8_t streamid, uint8_t dfs_index,
			  uint8_t k1, uint8_t k2,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_DATA_FRAME_SPECIFICATION;
	ydbia->dbpara.aps[7]=k1;
	ydbia->dbpara.aps[8]=k2;
	ydbia->dbpara.aps[9]=255u;
	ydbia->dbpara.kvs[3]=&dfs_index;
	ydbia->dbpara.kvs[4]=NULL;
	ydbia->dbpara.kss[3]=sizeof(uint8_t);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_get_item_tutspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t k1, uint8_t k2)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TRAFFIC_SPECIFICATION;
	ydbia->dbpara.aps[7]=k1;
	ydbia->dbpara.aps[8]=k2;
	ydbia->dbpara.aps[9]=255u;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_tutspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t k1, uint8_t k2)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TRAFFIC_SPECIFICATION;
	ydbia->dbpara.aps[7]=k1;
	ydbia->dbpara.aps[8]=k2;
	ydbia->dbpara.aps[9]=255u;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_tutspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t k1, uint8_t k2,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamid, YDBI_CONFIG);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_TSN_CONFIG_UNI_TRAFFIC_SPECIFICATION;
	ydbia->dbpara.aps[7]=k1;
	ydbia->dbpara.aps[8]=k2;
	ydbia->dbpara.aps[9]=255u;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}
