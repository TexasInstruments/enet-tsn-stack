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
#include <string.h>
#include <tsn_unibase/unibase.h>
#include "yang_modules.h"
#include "yang_db_access.h"
#include "ieee802-dot1q-cnc-config.h"
#include "ieee802-dot1q-cnc-config_access.h"

extern uint8_t IEEE802_DOT1Q_CNC_CONFIG_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_CNC_CONFIG_RW IEEE802_DOT1Q_CNC_CONFIG_func(dbald)
#define IEEE802_DOT1Q_CNC_CONFIG_RW_Y IEEE802_DOT1Q_CNC_CONFIG_func(ydbia->dbald)
#define IEEE802_DOT1Q_CNC_CONFIG_RO (IEEE802_DOT1Q_CNC_CONFIG_func(dbald)|0x80u)

static char domain_id[]=TSNUNI_DOMAIN_ID;

static void set_dpara_k0vk3(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t *instIndex, const char *cuc_id,
			    uint8_t *streamid, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1Q_CNC_CONFIG_RO:
		IEEE802_DOT1Q_CNC_CONFIG_RW;
	dbpara->aps[1] = IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG;
	dbpara->aps[2] = IEEE802_DOT1Q_CNC_CONFIG_DOMAIN;
	dbpara->aps[3] = IEEE802_DOT1Q_CNC_CONFIG_CUC;
	dbpara->aps[4] = IEEE802_DOT1Q_CNC_CONFIG_STREAM;
	dbpara->aps[5] = 255u;
	dbpara->kvs[0]=domain_id;
	dbpara->kss[0]=strlen(domain_id)+1u;
	(void)ub_bytearray2str(&domain_id[6], instIndex, 1);
	dbpara->kvs[1]=NULL;
	dbpara->kvs[2]=NULL;
	dbpara->kvs[3]=NULL;
	dbpara->kss[1]=0u;
	dbpara->kss[2]=0u;
	dbpara->kss[3]=0u;
	if (NULL != cuc_id) {
		dbpara->kvs[1]=(void*)cuc_id;
		dbpara->kss[1]=strlen(cuc_id)+1u;
		if (NULL != streamid) {
			dbpara->kvs[2]=streamid;
			dbpara->kss[2]=8;
		}
	}
}

int ydbi_set_needaction_cc(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, const char *cuc_id, bool dereg)
{
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, YDBI_CONFIG);
	ydbia->dbpara.aps[4]=IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_CNC_CONFIG_GATE_PARAMETER_TABLE;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_GATE_ENABLED;
	ydbia->dbpara.aps[7]=255u;
	return yang_db_set_needaction(ydbia->dbald, ydbia->dbpara.aps, ydbia->dbpara.kvs,
				      ydbia->dbpara.kss, dereg);
}

int ydbi_get_item_cck1st(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = 255u;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cck1st(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = 255u;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cck1st(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status,
			 void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = 255u;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cck1st(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = 255u;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cck1ns(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = 255u;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cck1ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = 255u;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cck1ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status,
			 void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = 255u;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cck1ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = 255u;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cck2ns(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = k2;
	ydbia->dbpara.aps[6] = 255u;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cck2ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = k2;
	ydbia->dbpara.aps[6] = 255u;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cck2ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status,
			 void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = k2;
	ydbia->dbpara.aps[6] = 255u;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cck2ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = k1;
	ydbia->dbpara.aps[5] = k2;
	ydbia->dbpara.aps[6] = 255u;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cck2vk2(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	if(streamid!=NULL){
		ydbia->dbpara.aps[5] = k1;
		ydbia->dbpara.aps[6] = k2;
		ydbia->dbpara.aps[7] = 255u;
		ydbia->dbpara.kvs[3] = kv1;
		ydbia->dbpara.kss[3] = kvs1;
		ydbia->dbpara.kvs[4] = kv2;
		ydbia->dbpara.kss[4] = kvs2;
		ydbia->dbpara.kvs[5] = NULL;
		ydbia->dbpara.kss[5] = 0;
	}else{
		ydbia->dbpara.aps[4] = k1;
		ydbia->dbpara.aps[5] = k2;
		ydbia->dbpara.aps[6] = 255u;
		ydbia->dbpara.kvs[2] = kv1;
		ydbia->dbpara.kss[2] = kvs1;
		ydbia->dbpara.kvs[3] = kv2;
		ydbia->dbpara.kss[3] = kvs2;
		ydbia->dbpara.kvs[4] = NULL;
		ydbia->dbpara.kss[4] = 0;
	}
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cck2vk2(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	if(streamid!=NULL){
		ydbia->dbpara.aps[5] = k1;
		ydbia->dbpara.aps[6] = k2;
		ydbia->dbpara.aps[7] = 255u;
		ydbia->dbpara.kvs[3] = kv1;
		ydbia->dbpara.kss[3] = kvs1;
		ydbia->dbpara.kvs[4] = kv2;
		ydbia->dbpara.kss[4] = kvs2;
		ydbia->dbpara.kvs[5] = NULL;
		ydbia->dbpara.kss[5] = 0;
	}else{
		ydbia->dbpara.aps[4] = k1;
		ydbia->dbpara.aps[5] = k2;
		ydbia->dbpara.aps[6] = 255u;
		ydbia->dbpara.kvs[2] = kv1;
		ydbia->dbpara.kss[2] = kvs1;
		ydbia->dbpara.kvs[3] = kv2;
		ydbia->dbpara.kss[3] = kvs2;
		ydbia->dbpara.kvs[4] = NULL;
		ydbia->dbpara.kss[4] = 0;
	}
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cck2vk2(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status, void *value, uint32_t vsize,
			 uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	if(streamid!=NULL){
		ydbia->dbpara.aps[5] = k1;
		ydbia->dbpara.aps[6] = k2;
		ydbia->dbpara.aps[7] = 255u;
		ydbia->dbpara.kvs[3] = kv1;
		ydbia->dbpara.kss[3] = kvs1;
		ydbia->dbpara.kvs[4] = kv2;
		ydbia->dbpara.kss[4] = kvs2;
		ydbia->dbpara.kvs[5] = NULL;
		ydbia->dbpara.kss[5] = 0;
	}else{
		ydbia->dbpara.aps[4] = k1;
		ydbia->dbpara.aps[5] = k2;
		ydbia->dbpara.aps[6] = 255u;
		ydbia->dbpara.kvs[2] = kv1;
		ydbia->dbpara.kss[2] = kvs1;
		ydbia->dbpara.kvs[3] = kv2;
		ydbia->dbpara.kss[3] = kvs2;
		ydbia->dbpara.kvs[4] = NULL;
		ydbia->dbpara.kss[4] = 0;
	}
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cck2vk2(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, streamid, status);
	if(streamid!=NULL){
		ydbia->dbpara.aps[5] = k1;
		ydbia->dbpara.aps[6] = k2;
		ydbia->dbpara.aps[7] = 255u;
		ydbia->dbpara.kvs[3] = kv1;
		ydbia->dbpara.kss[3] = kvs1;
		ydbia->dbpara.kvs[4] = kv2;
		ydbia->dbpara.kss[4] = kvs2;
		ydbia->dbpara.kvs[5] = NULL;
		ydbia->dbpara.kss[5] = 0;
	}else{
		ydbia->dbpara.aps[4] = k1;
		ydbia->dbpara.aps[5] = k2;
		ydbia->dbpara.aps[6] = 255u;
		ydbia->dbpara.kvs[2] = kv1;
		ydbia->dbpara.kss[2] = kvs1;
		ydbia->dbpara.kvs[3] = kv2;
		ydbia->dbpara.kss[3] = kvs2;
		ydbia->dbpara.kvs[4] = NULL;
		ydbia->dbpara.kss[4] = 0;
	}
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cck2ic(yang_db_item_access_t *ydbia, void **rval,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2)
{
	int ki=3;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &endst->instIndex,
			endst->cuc_id, endst->streamid, YDBI_STATUS);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CONFIGURATION;
	ydbia->dbpara.aps[7]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_LIST;
	ydbia->dbpara.aps[8]  = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	ydbia->dbpara.aps[9]  = k1;
	ydbia->dbpara.aps[10] = k2;
	ydbia->dbpara.aps[11] = 255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=(void*)&cindex;
	ydbia->dbpara.kss[ki++]=sizeof(uint8_t);
	ydbia->dbpara.kvs[ki]=NULL;
	ydbia->dbpara.kss[ki++]=0u;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cck2ic(yang_db_item_access_t *ydbia,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2)
{
	int ki=3;
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &endst->instIndex,
			endst->cuc_id, endst->streamid, YDBI_STATUS);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CONFIGURATION;
	ydbia->dbpara.aps[7]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_LIST;
	ydbia->dbpara.aps[8]  = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	ydbia->dbpara.aps[9]  = k1;
	ydbia->dbpara.aps[10] = k2;
	ydbia->dbpara.aps[11] = 255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=(void*)&cindex;
	ydbia->dbpara.kss[ki++]=sizeof(uint8_t);
	ydbia->dbpara.kvs[ki]=NULL;
	ydbia->dbpara.kss[ki++]=0u;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cck2ic(yang_db_item_access_t *ydbia,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2, void *value, uint32_t vsize,
			 uint8_t notice)
{
	int ki=3;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &endst->instIndex,
			endst->cuc_id, endst->streamid, YDBI_STATUS);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CONFIGURATION;
	ydbia->dbpara.aps[7]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_LIST;
	ydbia->dbpara.aps[8]  = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	ydbia->dbpara.aps[9]  = k1;
	ydbia->dbpara.aps[10] = k2;
	ydbia->dbpara.aps[11] = 255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=(void*)&cindex;
	ydbia->dbpara.kss[ki++]=sizeof(uint8_t);
	ydbia->dbpara.kvs[ki]=NULL;
	ydbia->dbpara.kss[ki++]=0u;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cck2ic(yang_db_item_access_t *ydbia,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2)
{
	int ki=3;
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &endst->instIndex,
			endst->cuc_id, endst->streamid, YDBI_STATUS);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_CONFIGURATION;
	ydbia->dbpara.aps[7]  = IEEE802_DOT1Q_CNC_CONFIG_INTERFACE_LIST;
	ydbia->dbpara.aps[8]  = IEEE802_DOT1Q_CNC_CONFIG_CONFIG_LIST;
	ydbia->dbpara.aps[9]  = k1;
	ydbia->dbpara.aps[10] = k2;
	ydbia->dbpara.aps[11] = 255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=(void*)&cindex;
	ydbia->dbpara.kss[ki++]=sizeof(uint8_t);
	ydbia->dbpara.kvs[ki]=NULL;
	ydbia->dbpara.kss[ki++]=0u;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cck1tas(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = k2;
	ydbia->dbpara.aps[7] = k3;
	ydbia->dbpara.aps[8] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = NULL;
	ydbia->dbpara.kss[3] = 0u;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cck1tas(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = k2;
	ydbia->dbpara.aps[7] = k3;
	ydbia->dbpara.aps[8] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = NULL;
	ydbia->dbpara.kss[3] = 0u;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cck1tas(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status,
			 void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = k2;
	ydbia->dbpara.aps[7] = k3;
	ydbia->dbpara.aps[8] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = NULL;
	ydbia->dbpara.kss[3] = 0u;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cck1tas(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = k1;
	ydbia->dbpara.aps[6] = k2;
	ydbia->dbpara.aps[7] = k3;
	ydbia->dbpara.aps[8] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = NULL;
	ydbia->dbpara.kss[3] = 0u;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cck2acl(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = IEEE802_DOT1Q_CNC_CONFIG_GATE_PARAMETER_TABLE;
	ydbia->dbpara.aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CONTROL_LIST;
	ydbia->dbpara.aps[7] = k1;
	ydbia->dbpara.aps[8] = k2;
	ydbia->dbpara.aps[9] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = gceIndex;
	ydbia->dbpara.kss[3] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[4] = NULL;
	ydbia->dbpara.kss[4] = 0u;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cck2acl(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = IEEE802_DOT1Q_CNC_CONFIG_GATE_PARAMETER_TABLE;
	ydbia->dbpara.aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CONTROL_LIST;
	ydbia->dbpara.aps[7] = k1;
	ydbia->dbpara.aps[8] = k2;
	ydbia->dbpara.aps[9] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = gceIndex;
	ydbia->dbpara.kss[3] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[4] = NULL;
	ydbia->dbpara.kss[4] = 0u;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cck2acl(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status,
			 void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = IEEE802_DOT1Q_CNC_CONFIG_GATE_PARAMETER_TABLE;
	ydbia->dbpara.aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CONTROL_LIST;
	ydbia->dbpara.aps[7] = k1;
	ydbia->dbpara.aps[8] = k2;
	ydbia->dbpara.aps[9] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = gceIndex;
	ydbia->dbpara.kss[3] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[4] = NULL;
	ydbia->dbpara.kss[4] = 0u;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cck2acl(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex,
			cuc_id, NULL, status);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
	ydbia->dbpara.aps[5] = IEEE802_DOT1Q_CNC_CONFIG_GATE_PARAMETER_TABLE;
	ydbia->dbpara.aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CONTROL_LIST;
	ydbia->dbpara.aps[7] = k1;
	ydbia->dbpara.aps[8] = k2;
	ydbia->dbpara.aps[9] = 255u;
	ydbia->dbpara.kvs[2] = schIndex;
	ydbia->dbpara.kss[2] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[3] = gceIndex;
	ydbia->dbpara.kss[3] = yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[4] = NULL;
	ydbia->dbpara.kss[4] = 0u;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

// /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id|/
// stream|streami-id:|/talker/end-station-interfaces|mac-address:|interface-name:|/
// accept
static int ydbi_set_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, bool talker, cc_endstation_info_t *endst,
			      bool update)
{
	int ki=3;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	if(update){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_UPDATE_ACCEPT;
	}else{
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCEPT;
	}
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	if(update){
		ydbia->dbpara.value=(void*)&endst->update_accept;
	}else{
		ydbia->dbpara.value=(void*)&endst->accept;
	}
	ydbia->dbpara.vsize=1;
	// when accept is updated, eventflags needs to be updated to send a notice to CNC.
	// here we don't set YDBI_PUSH_NOTICE.
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, false);
}

static int ydbi_get_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, bool talker, cc_endstation_info_t *endst,
			      bool update)
{
	void *value;
	int res;
	int ki=3;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	if(update){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_UPDATE_ACCEPT;
	}else{
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCEPT;
	}
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;

	res=ydbi_get_foot(ydbia, __func__, &value, UBL_DEBUG);
	if(update){
		if(res<0){
			// set false with "no data"
			endst->update_accept=false;
			return 0;
		}
		endst->update_accept=*((bool*)value);
	}else{
		if(res<0){
			// error with "no data"
			return -1;
		}
		endst->accept=*((uint8_t*)value);
	}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return 0;
}

static int ydbi_del_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, bool talker, cc_endstation_info_t *endst,
			      bool update)
{
	int ki=3;
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	if(update){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_UPDATE_ACCEPT;
	}else{
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCEPT;
	}
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

// /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id|/
// stream|streami-id:|/talker/end-station-interfaces|mac-address:|interface-name:|/
// status
static int ydbi_set_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	int ki=3;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_STATUS;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	ydbia->dbpara.value=(void*)&endst->status;
	ydbia->dbpara.vsize=sizeof(uint32_t);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_PUSH_NOTICE);
}

static int ydbi_get_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	void *value;
	int res;
	int ki=3;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_STATUS;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;

	res=ydbi_get_foot(ydbia, __func__, &value, UBL_INFO);
	if(res<0){return -1;}
	endst->status=*((cc_endst_status_t*)value);
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return 0;
}

static int ydbi_del_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	int ki=3;
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_STATUS;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

// /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id|/
// stream|streami-id:|/talker/end-station-interfaces|mac-address:|interface-name:|/
// station-name
static int ydbi_set_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	int ki=3;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	ydbia->dbpara.value=(void*)&endst->station_name;
	ydbia->dbpara.vsize=strlen(endst->station_name)+1;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, false);
}

static int ydbi_get_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	void *value;
	int res;
	int ki=3;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;

	res=ydbi_get_foot(ydbia, __func__, &value, UBL_INFO);
	if(res<0){return -1;}
	endst->station_name=((const char*)value);
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return 0;
}

static int ydbi_del_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	int ki=3;
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

// /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id|/
// stream|streami-id:|/talker/end-station-interfaces|mac-address:|interface-name:|/
// accumulated-latency
static int ydbi_set_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	int ki=3;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	ydbia->dbpara.value=(void*)&endst->accumulated_latency;
	ydbia->dbpara.vsize=sizeof(uint32_t);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, false);
}

static int ydbi_get_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	void *value;
	int res;
	int ki=3;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;

	res=ydbi_get_foot(ydbia, __func__, &value, UBL_INFO);
	if(res<0){return -1;}
	endst->accumulated_latency=*((uint32_t*)value);
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return 0;
}

static int ydbi_del_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, bool talker, cc_endstation_info_t *endst)
{
	int ki=3;
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			endst->streamid, false);
	ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
		IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_ACCUMULATED_LATENCY;
	ydbia->dbpara.aps[8]=255u;
	if(!talker){
		ydbia->dbpara.kvs[ki]=&endst->lindex;
		ydbia->dbpara.kss[ki++]=sizeof(uint32_t);
	}
	ydbia->dbpara.kvs[ki]=(void*)endst->mac_address;
	ydbia->dbpara.kss[ki++]=strlen(endst->mac_address)+1;
	ydbia->dbpara.kvs[ki]=(void*)endst->interface_name;
	ydbia->dbpara.kss[ki++]=strlen(endst->interface_name)+1;
	ydbia->dbpara.kvs[ki]=NULL;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

static int ydbi_set_eventflags_notice(yang_db_item_access_t *ydbia, uint8_t instIndex,
                                      UC_NOTICE_SIG_T **sem, const char *semname,
                                      notice_register_value_t regval)
{
	uint8_t aps[6]={
		IEEE802_DOT1Q_CNC_CONFIG_RW_Y,
		IEEE802_DOT1Q_CNC_CONFIG_CNC_CONFIG,
		IEEE802_DOT1Q_CNC_CONFIG_DOMAIN,
		IEEE802_DOT1Q_CNC_CONFIG_CUC,
		IEEE802_DOT1Q_CNC_CONFIG_EVENTFLAGS,
		255u
	};
	void *kvs[3]={domain_id, (char*)semname, NULL};
	uint8_t kss[3]={strlen(domain_id)+1u, 0u, 0u};
	bool dereg=(regval==UC_NOTICE_DBVAL_DEL)?true:false;
	if(NULL==semname){ return -1; }
	kss[1]=strlen(semname)+1;
	(void)ub_bytearray2str(&domain_id[6], &instIndex, 1);
	int res=uc_nc_notice_register(ydbia->ucntd,ydbia->dbald,aps,kvs,kss,regval,sem);
	if(res==0){
		kvs[1]=NULL;
		kss[1]=0u;
		res=yang_db_set_needaction(ydbia->dbald, aps, kvs, kss, dereg);
		if(res!=0){
			UB_LOG(UBL_ERROR, "%s:yang_db_set_needaction() failed\n", __func__);
		}
	} else {
		UB_LOG(UBL_ERROR, "%s:uc_nc_notice_register() failed\n", __func__);
	}
	uc_dbal_releasedb(ydbia->dbald);
	return res;
}

// scan /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id|/
// stream|streami-id:|/talker/end-station-interfaces|mac-address:|interface-name:|/
// accept
// station-name
// status
#define FIND_TALKER true
#define FIND_LISTENER false
#define ACCEPT_SCAN IEEE802_DOT1Q_CNC_CONFIG_ACCEPT
#define STATUS_SCAN IEEE802_DOT1Q_CNC_CONFIG_STATUS
#define STATION_NAME_SCAN IEEE802_DOT1Q_CNC_CONFIG_STATION_NAME
static int ydbi_iterate_endstations_cc(yang_db_item_access_t *ydbia, uc_range **range,
				       uint8_t instIndex, const char *cuc_id,
				       bool talker, cc_endstation_info_t *endst,
				       uint8_t k1)
{
	uint8_t aps[8];
	void *kdata;
	uint32_t ksize;
	uint8_t *kv;
	char mydomain_id[]=TSNUNI_DOMAIN_ID;
	int res;
	void *vdata;
	uint32_t vsize;

	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	if(*range==NULL){
		bool status=false;
		set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
				NULL, status);
		ydbia->dbpara.aps[5]=talker?IEEE802_DOT1Q_CNC_CONFIG_TALKER:
			IEEE802_DOT1Q_CNC_CONFIG_LISTENER;
		ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_END_STATION_INTERFACES;
		ydbia->dbpara.aps[7]=k1;
		ydbia->dbpara.aps[8]=255u;
		memcpy(aps, ydbia->dbpara.aps, 8);
		aps[7]+=1;
		*range=uc_get_range(ydbia->dbald, ydbia->dbpara.aps, 8, aps, 8);
		if(*range==NULL){
			res=1;
			goto erexit;
		}
	}
	(void)ub_bytearray2str(&mydomain_id[6], &instIndex, 1);
	while(true){
		if(uc_get_keyvalue_in_range(ydbia->dbald, *range, &kdata, &ksize,
					    &vdata, &vsize, UC_DBAL_FORWARD) != 0){
			// no more data
			res=1;
			goto erexit;
		}
		kv=(uint8_t*)kdata;
		if((ksize<=10) || (kv[8]!=255u)){continue;}
		// check domain-id
		kv=kv+9; // node key(aps part) size is 9
		ksize-=9;
		if(ksize<=kv[0]){continue;}
		if(strcmp(((char*)&kv[1]), mydomain_id)!=0){continue;}
		endst->instIndex=instIndex;
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// check cuc-id
		if(ksize<=kv[0]){continue;}
		if(strcmp(((char*)&kv[1]), cuc_id)!=0){continue;}
		endst->cuc_id=(const char*)&kv[1];
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// get stream-id
		if(ksize<=kv[0]){continue;}
		endst->streamid=&kv[1];
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		if(!talker){
			// get listener index
			if(ksize<=kv[0]){continue;}
			ub_non_aligned_intsubst(&kv[1], &endst->lindex, sizeof(uint32_t));
			ksize-=kv[0]+1;
			kv=kv+kv[0]+1;
		}
		// get mac-address
		if(ksize<=kv[0]){continue;}
		endst->mac_address=(const char*)&kv[1];
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// get interface-name
		if(ksize<=kv[0]){continue;}
		endst->interface_name=(const char*)&kv[1];
		switch(k1){
		case STATION_NAME_SCAN:
			endst->station_name=(const char*)vdata;
			break;
		case STATUS_SCAN:
			ub_non_aligned_intsubst(vdata, &endst->status,
						sizeof(cc_endst_status_t));
			break;
		default:
			endst->accept=*((uint8_t*)vdata);
			break;
		}
		break;
	}
	res=0;
erexit:
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	if((*range!=NULL) && (res!=0)){
		uc_get_range_release(ydbia->dbald, *range);
		*range=NULL;
	}
	return res;
}

int ydbi_set_talker_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_accept_cc(ydbia, instIndex, cuc_id, true, endst, false);
}

int ydbi_set_listener_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_accept_cc(ydbia, instIndex, cuc_id, false, endst, false);
}

int ydbi_get_talker_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_accept_cc(ydbia, instIndex, cuc_id, true, endst, false);
}

int ydbi_get_listener_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_accept_cc(ydbia, instIndex, cuc_id, false, endst, false);
}

int ydbi_del_talker_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_accept_cc(ydbia, instIndex, cuc_id, true, endst, false);
}

int ydbi_del_listener_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_accept_cc(ydbia, instIndex, cuc_id, false, endst, false);
}

int ydbi_set_talker_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				     const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_accept_cc(ydbia, instIndex, cuc_id, true, endst, true);
}

int ydbi_set_listener_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				       const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_accept_cc(ydbia, instIndex, cuc_id, false, endst, true);
}

int ydbi_get_talker_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				     const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_accept_cc(ydbia, instIndex, cuc_id, true, endst, true);
}

int ydbi_get_listener_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				       const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_accept_cc(ydbia, instIndex, cuc_id, false, endst, true);
}

int ydbi_set_talker_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_status_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_set_listener_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_status_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_get_talker_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_status_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_get_listener_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_status_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_del_talker_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_status_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_del_listener_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_status_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_set_talker_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_station_name_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_set_listener_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_station_name_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_get_talker_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_station_name_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_get_listener_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_station_name_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_del_talker_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_station_name_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_del_listener_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_station_name_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_set_talker_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_accumulated_latency_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_set_listener_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_set_accumulated_latency_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_get_talker_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_accumulated_latency_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_get_listener_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_get_accumulated_latency_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_del_talker_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				    const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_accumulated_latency_cc(ydbia, instIndex, cuc_id, true, endst);
}

int ydbi_del_listener_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				      const char *cuc_id, cc_endstation_info_t *endst)
{
	return ydbi_del_accumulated_latency_cc(ydbia, instIndex, cuc_id, false, endst);
}

int ydbi_get_eventflags_info(yang_db_item_access_t *ydbia, uint8_t instIndex,
                             cc_eventflag_info_t *event)
{
	void *value;
	int res;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, event->cuc_id,
	                NULL, false);
	ydbia->dbpara.aps[4]=IEEE802_DOT1Q_CNC_CONFIG_EVENTFLAGS;
	res=ydbi_get_foot(ydbia, __func__, &value, UBL_INFO);
	if(res<0){return -1;}
	event->eventflag=*((uint8_t*)value);
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return 0;
}

int ydbi_set_eventflags_info(yang_db_item_access_t *ydbia, uint8_t instIndex,
                             cc_eventflag_info_t *event, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, event->cuc_id,
	                NULL, false);
	ydbia->dbpara.aps[4]=IEEE802_DOT1Q_CNC_CONFIG_EVENTFLAGS;
	ydbia->dbpara.value=(void*)&event->eventflag;
	ydbia->dbpara.vsize=1;
	// when eventflags is updated, CNC need to get the notice. So use notice=true.
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_eventflags_info(yang_db_item_access_t *ydbia, uint8_t instIndex,
                             cc_eventflag_info_t *event, uint8_t notice)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, event->cuc_id,
	                NULL, false);
	ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_EVENTFLAGS;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_reg_eventflags_notice(yang_db_item_access_t *ydbia, uint8_t instIndex,
                               UC_NOTICE_SIG_T **sem, const char *semname)
{
	return ydbi_set_eventflags_notice(ydbia, instIndex, sem, semname,
	                                  UC_NOTICE_DBVAL_ADD);
}

void ydbi_unreg_eventflags_notice(yang_db_item_access_t *ydbia, uint8_t instIndex,
                                  const char *semname)
{
	int res=ydbi_set_eventflags_notice(ydbia, instIndex, NULL, semname,
	                                   UC_NOTICE_DBVAL_DEL);
	if(res != 0) {
		UB_LOG(UBL_WARN, "%s:failed to clear even flag notice\n", __func__);
	}
}

int ydbi_get_eventflags_notice_info(yang_db_item_access_t *ydbia, uint8_t *instIndex,
                                    char *semname)
{
	uint32_t ksize;
	uint8_t key[UC_MAX_KEYSIZE];
	char *retdomid=NULL;
	int res=-1;
	res=uc_nc_get_notice_act(ydbia->ucntd, ydbia->dbald, semname, key, &ksize);
	if(res){
		UB_LOG(UBL_DEBUG, "%s:no data, res=%d\n", __func__, res);
		goto erexit;
	}
	// key=/ieee802-dot1q-cnc-config/cnc-config/domain/cuc/eventflags 255 SIZE VALUE
	if((ksize<=7u) || (ksize<(7u+key[6]))){
		UB_LOG(UBL_ERROR, "%s:received a notice from different key\n", __func__);
		goto erexit;
	}
	if(strlen(domain_id)+1 != key[6]){
		UB_LOG(UBL_ERROR, "%s:invalid domain-id:[%s]\n", __func__, (char*)&key[6]);
		goto erexit;
	}
	retdomid=(char*)(&key[7]);
	(void)ub_str2bytearray(instIndex, &retdomid[6], 16);
	uc_dbal_releasedb(ydbia->dbald);
	res=0;
erexit:
	return res;
}

int ydbi_iterate_talkers_cc(yang_db_item_access_t *ydbia, uc_range **range,
			    uint8_t instIndex, const char *cuc_id,
			    cc_endstation_info_t *endst)
{
	return ydbi_iterate_endstations_cc(ydbia, range, instIndex,
					   cuc_id, FIND_TALKER, endst,
					   ACCEPT_SCAN);
}

int ydbi_iterate_listeners_cc(yang_db_item_access_t *ydbia, uc_range **range,
			      uint8_t instIndex, const char *cuc_id,
			      cc_endstation_info_t *endst)
{
	return ydbi_iterate_endstations_cc(ydbia, range, instIndex,
					   cuc_id, FIND_LISTENER, endst,
					   ACCEPT_SCAN);
}

int ydbi_iterate_talker_status_cc(yang_db_item_access_t *ydbia, uc_range **range,
				  uint8_t instIndex, const char *cuc_id,
				  cc_endstation_info_t *endst)
{
	return ydbi_iterate_endstations_cc(ydbia, range, instIndex,
					   cuc_id, FIND_TALKER, endst,
					   STATUS_SCAN);
}

int ydbi_iterate_listener_status_cc(yang_db_item_access_t *ydbia, uc_range **range,
				    uint8_t instIndex, const char *cuc_id,
				    cc_endstation_info_t *endst)
{
	return ydbi_iterate_endstations_cc(ydbia, range, instIndex,
					   cuc_id, FIND_LISTENER, endst,
					   STATUS_SCAN);
}

int ydbi_iterate_talker_names_cc(yang_db_item_access_t *ydbia, uc_range **range,
				 uint8_t instIndex, const char *cuc_id,
				 cc_endstation_info_t *endst)
{
	return ydbi_iterate_endstations_cc(ydbia, range, instIndex,
					   cuc_id, FIND_TALKER, endst,
					   STATION_NAME_SCAN);
}

int ydbi_iterate_listener_names_cc(yang_db_item_access_t *ydbia, uc_range **range,
				   uint8_t instIndex, const char *cuc_id,
				   cc_endstation_info_t *endst)
{
	return ydbi_iterate_endstations_cc(ydbia, range, instIndex,
					   cuc_id, FIND_LISTENER, endst,
					   STATION_NAME_SCAN);
}

// scan /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id|/eventflags
int ydbi_iterate_event_notice(yang_db_item_access_t *ydbia, uc_range **range,
			      uint8_t instIndex, cc_eventflag_info_t *event)
{
	uint8_t aps[6];
	void *kdata;
	uint32_t ksize;
	uint8_t *kv;
	char mydomain_id[]=TSNUNI_DOMAIN_ID;
	int res;
	void *vdata;
	uint32_t vsize;

	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	if(*range==NULL){
		set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, NULL,
				NULL, false);
		ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_EVENTFLAGS;
		memcpy(aps, ydbia->dbpara.aps, sizeof(aps));
		aps[4]+=1;
		*range=uc_get_range(ydbia->dbald, ydbia->dbpara.aps, 5, aps, 5);
		if(*range==NULL){
			res=1;
			goto erexit;
		}
	}
	(void)ub_bytearray2str(&mydomain_id[6], &instIndex, 1);
	while(true){
		if(uc_get_keyvalue_in_range(ydbia->dbald, *range, &kdata, &ksize,
					    &vdata, &vsize, UC_DBAL_FORWARD) != 0){
			// no more data
			res=1;
			goto erexit;
		}
		kv=(uint8_t*)kdata;
		if((ksize<=7u) || (kv[5]!=255u)){continue;}
		// check domain-id
		kv=kv+6; // node key(aps part) size is 9
		ksize-=6;
		if(ksize<=kv[0]){continue;}
		if(strcmp(((char*)&kv[1]), mydomain_id)!=0){continue;}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// check cuc-id
		if(ksize<=kv[0]){continue;}
		event->cuc_id=(const char*)&kv[1];
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		event->eventflag=*((uint8_t*)vdata);
		break;
	}
	res=0;
erexit:
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	if((*range!=NULL) && (res!=0)){
		uc_get_range_release(ydbia->dbald, *range);
		*range=NULL;
	}
	return res;
}

int ydbi_get_streaminfo_cc(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, const char *cuc_id, uint8_t *streamid,
			   cc_stream_info_t *stinfo, uint8_t dfindex)
{
	int res=-1;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			streamid, false);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_DATA_FRAME_SPECIFICATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_IEEE802_MAC_ADDRESSES;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_DESTINATION_MAC_ADDRESS;
	ydbia->dbpara.aps[9]=255u;
	ydbia->dbpara.kvs[3]=&dfindex;
	ydbia->dbpara.kss[3]=1;
	ydbia->dbpara.kvs[4]=NULL;

	// copy the data-frame-specification to the resulting structure
	stinfo->dfinfo.index=dfindex;

	// data-frame-specification, reading 2 choice fields here, it is not compliant
	// we may need to augment to use both 'mac_address' and 'vlan_tag' fields
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		memcpy(stinfo->dfinfo.dest_mac_address, ydbia->dbpara.value,
		       sizeof(stinfo->dfinfo.dest_mac_address));
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_SOURCE_MAC_ADDRESS;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		memcpy(stinfo->dfinfo.src_mac_address, ydbia->dbpara.value,
		       sizeof(stinfo->dfinfo.src_mac_address));
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_IEEE802_VLAN_TAG;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_PRIORITY_CODE_POINT;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->dfinfo.pcp=*((uint8_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_VLAN_ID;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->dfinfo.vlanid=*((uint16_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	// traffic-specification
	ydbia->dbpara.kvs[3]=NULL;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_TRAFFIC_SPECIFICATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_INTERVAL;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_NUMERATOR;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->tsinfo.interval_numer=*((uint32_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_DENOMINATOR;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->tsinfo.interval_denom=*((uint32_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAMES_PER_INTERVAL;
	ydbia->dbpara.aps[8]=255u;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->tsinfo.max_frame_interval=*((uint16_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAME_SIZE;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->tsinfo.max_frame_size=*((uint16_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_TRANSMISSION_SELECTION;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->tsinfo.transmission_selection=*((uint8_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	// stream-rank
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_STREAM_RANK;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_RANK;
	ydbia->dbpara.atype=YANG_DB_ACTION_READ;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)==0){
		stinfo->rank=*((uint8_t*)ydbia->dbpara.value);
		ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
		res=0;
	}

	ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	return res;
}

int ydbi_set_streaminfo_cc(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, const char *cuc_id, uint8_t *streamid,
			   cc_stream_info_t *stinfo, uint8_t dfindex, uint32_t set_flags)
{
	int res=-1;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			streamid, false);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_DATA_FRAME_SPECIFICATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_IEEE802_MAC_ADDRESSES;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_DESTINATION_MAC_ADDRESS;
	ydbia->dbpara.aps[9]=255u;
	ydbia->dbpara.kvs[3]=&dfindex;
	ydbia->dbpara.kss[3]=1;
	ydbia->dbpara.kvs[4]=NULL;

	// data-frame-specification, setting 2 choice fields here, it is not compliant
	// we may need to augment to use both 'mac_address' and 'vlan_tag' fields

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_DESTMAC))!=0){
		ydbia->dbpara.value=stinfo->dfinfo.dest_mac_address;
		ydbia->dbpara.vsize=sizeof(stinfo->dfinfo.dest_mac_address);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_SRCMAC))!=0){
		ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_SOURCE_MAC_ADDRESS;
		ydbia->dbpara.value=stinfo->dfinfo.src_mac_address;
		ydbia->dbpara.vsize=sizeof(stinfo->dfinfo.src_mac_address);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_PCP))!=0){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_IEEE802_VLAN_TAG;
		ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_PRIORITY_CODE_POINT;
		ydbia->dbpara.value=&stinfo->dfinfo.pcp;
		ydbia->dbpara.vsize=sizeof(uint8_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_VLANID))!=0){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_IEEE802_VLAN_TAG;
		ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_VLAN_ID;
		ydbia->dbpara.value=&stinfo->dfinfo.vlanid;
		ydbia->dbpara.vsize=sizeof(uint16_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	// traffic-specification
	ydbia->dbpara.kvs[3]=NULL;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_TRAFFIC_SPECIFICATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_INTERVAL;

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_INTERVAL_NUMER))!=0){
		ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_NUMERATOR;
		ydbia->dbpara.value=&stinfo->tsinfo.interval_numer;
		ydbia->dbpara.vsize=sizeof(uint32_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_INTERVAL_DENOM))!=0){
		ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_DENOMINATOR;
		ydbia->dbpara.value=&stinfo->tsinfo.interval_denom;
		ydbia->dbpara.vsize=sizeof(uint32_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	ydbia->dbpara.aps[8]=255u;
	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_MAX_FRAME_INTERVAL))!=0){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAMES_PER_INTERVAL;
		ydbia->dbpara.value=&stinfo->tsinfo.max_frame_interval;
		ydbia->dbpara.vsize=sizeof(uint16_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_MAX_FRAME_SIZE))!=0){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAME_SIZE;
		ydbia->dbpara.value=&stinfo->tsinfo.max_frame_size;
		ydbia->dbpara.vsize=sizeof(uint16_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_TRM_SELECTION))!=0){
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_TRANSMISSION_SELECTION;
		ydbia->dbpara.value=&stinfo->tsinfo.transmission_selection;
		ydbia->dbpara.vsize=sizeof(uint8_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	if((set_flags & (1<<CC_STRAMINFO_FLAGBIT_STREAM_RANK))!=0){
		// stream-rank
		ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_STREAM_RANK;
		ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_RANK;
		ydbia->dbpara.value=&stinfo->rank;
		ydbia->dbpara.vsize=sizeof(uint8_t);
		if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}
	}

	res=0;
erexit:
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	return res;
}

int ydbi_del_streaminfo_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			   const char *cuc_id, uint8_t *streamid, uint8_t dfindex)
{
	int res=-1;
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			streamid, false);
	ydbia->dbpara.aps[5]=IEEE802_DOT1Q_CNC_CONFIG_TALKER;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_DATA_FRAME_SPECIFICATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_IEEE802_MAC_ADDRESSES;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_DESTINATION_MAC_ADDRESS;
	ydbia->dbpara.aps[9]=255u;
	ydbia->dbpara.kvs[3]=&dfindex;
	ydbia->dbpara.kss[3]=1;
	ydbia->dbpara.kvs[4]=NULL;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	// data-frame-specification, setting 2 choice fields here, it is not compliant
	// we may need to augment to use both 'mac_address' and 'vlan_tag' fields
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_SOURCE_MAC_ADDRESS;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_IEEE802_VLAN_TAG;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_PRIORITY_CODE_POINT;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_VLAN_ID;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	// traffic-specification
	ydbia->dbpara.kvs[3]=NULL;
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_TRAFFIC_SPECIFICATION;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_INTERVAL;
	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_NUMERATOR;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	ydbia->dbpara.aps[8]=IEEE802_DOT1Q_CNC_CONFIG_DENOMINATOR;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAMES_PER_INTERVAL;
	ydbia->dbpara.aps[8]=255u;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_MAX_FRAME_SIZE;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_TRANSMISSION_SELECTION;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	// stream-rank
	ydbia->dbpara.aps[6]=IEEE802_DOT1Q_CNC_CONFIG_STREAM_RANK;
	ydbia->dbpara.aps[7]=IEEE802_DOT1Q_CNC_CONFIG_RANK;
	if(yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara)!=0){goto erexit;}

	// all operation successful
	res=0;
erexit:
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	return res;
}

void ydbi_set_dbpara_ccknvkn(yang_db_item_access_t *ydbia, uint8_t instIndex,
			     const char *cuc_id, uint8_t *streamid)
{
	return set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
			       streamid, false);
}

uint8_t found_in_streamid_leaflist(yang_db_item_access_t *ydbia,
				   uint8_t instIndex, const char *cuc_id,
				   uint8_t *streamid, uint8_t k4)
{
	int32_t i;
	void *vdata;
	int vsize;
	uint8_t res=0;
	vsize=ydbi_get_item_cck1ns(ydbia, &vdata, instIndex, cuc_id, k4, YDBI_CONFIG);
	i=0;
	while(i+8<=vsize){
		if(memcmp(&((uint8_t*)vdata)[i], streamid, 8)==0){
			res=1;
			UB_LOG(UBL_DEBUG, "%s:found sid="UB_PRIhexB8"\n", __func__,
			       UB_ARRAY_B8(streamid));
			break;
		}
		i+=8;
	}
	if(vsize>0){
		ydbi_rel_item_cck1ns(ydbia, instIndex, cuc_id, k4, YDBI_CONFIG);
	}
	return res;
}

// scan /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id:|/
// tas-sched-list|index|/streamid
// or
// scan /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id:|/
// tas-sched-list|index|/bridge-sched/bridge-name
static int ydbi_iterate_tas_apply_info(yang_db_item_access_t *ydbia,
                              uc_range **range, uint8_t instIndex,
                              const char *cuc_id, const char *brname,
                              cc_tas_sched_list_t *schlist)
{
	uint8_t aps[8];
	void *kdata;
	uint32_t ksize;
	uint32_t vsize;
	uint32_t asize;
	uint8_t *kv;
	char mydomain_id[]=TSNUNI_DOMAIN_ID;
	void *vdata;
	int res;

	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	asize=(brname != NULL) ? 7u : 6u;
	if(*range==NULL){
		set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
				NULL, false);
		ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
		if(brname != NULL){
			ydbia->dbpara.aps[5] = IEEE802_DOT1Q_CNC_CONFIG_BRIDGE_SCHED;
			ydbia->dbpara.aps[6] = IEEE802_DOT1Q_CNC_CONFIG_BRIDGE_PORT;
			ydbia->dbpara.aps[7] = 255u;
		}else{
			ydbia->dbpara.aps[5] = IEEE802_DOT1Q_CNC_CONFIG_STREAMID;
			ydbia->dbpara.aps[6] = 255u;
		}
		memcpy(aps, ydbia->dbpara.aps, sizeof(aps));
		aps[asize-1]+=1;
		*range=uc_get_range(ydbia->dbald, ydbia->dbpara.aps, asize, aps, asize);
		if(*range==NULL){
			res=1;
			goto erexit;
		}
	}
	(void)ub_bytearray2str(&mydomain_id[6], &instIndex, 1);
	while(true){
		if(uc_get_keyvalue_in_range(ydbia->dbald, *range, &kdata, &ksize,
					    &vdata, &vsize, UC_DBAL_FORWARD) != 0){
			// no more data
			res=1;
			goto erexit;
		}
		kv=(uint8_t*)kdata;
		if((ksize<=(asize+2u)) || (kv[asize]!=255u)){continue;}
		// check domain-id
		kv=kv+(asize+1); // node key(aps part) size is 9
		ksize-=(asize+1);
		if(ksize<=kv[0]){continue;}
		if(strcmp(((char*)&kv[1]), mydomain_id)!=0){continue;}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// check cuc-id
		if(ksize<=kv[0]){continue;}
		if(strcmp(((char*)&kv[1]), cuc_id)!=0){continue;}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// get index sched-list index
		if(ksize<=kv[0]){continue;}
		ub_non_aligned_intsubst(&kv[1], &schlist->index, sizeof(uint32_t));
		if(brname != NULL){
			ksize-=kv[0]+1;
			kv=kv+kv[0]+1;
			if(ksize<=kv[0]){continue;}
			if(strcmp(((char*)&kv[1]), brname)!=0){continue;}
			schlist->bridge_port=(const char*)vdata;
			schlist->brport_size=vsize;
		} else {
			schlist->streamid=(uint8_t*)vdata;
		}
		break;
	}
	res=0;
erexit:
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	if((*range!=NULL) && (res!=0)){
		uc_get_range_release(ydbia->dbald, *range);
		*range=NULL;
	}
	return res;
}

int ydbi_iterate_tas_apply_stream(yang_db_item_access_t *ydbia,
		uc_range **range, uint8_t instIndex, const char *cuc_id,
		cc_tas_sched_list_t *schlist)
{
	return ydbi_iterate_tas_apply_info(ydbia, range, instIndex, cuc_id,
	        NULL, schlist);
}

int ydbi_iterate_tas_apply_bridge_port(yang_db_item_access_t *ydbia,
		uc_range **range, uint8_t instIndex, const char *cuc_id,
		const char *bridge_name, cc_tas_sched_list_t *schlist)
{
	return ydbi_iterate_tas_apply_info(ydbia, range, instIndex, cuc_id,
	        bridge_name, schlist);
}

// scan /ieee802-dot1q-cnc-config/cnc-config/domain|domain-id:|/cuc|cuc-id:|/
// tas-sched-list|index|/admin-control-list/gate-control-entry|index|/
// operation-name
int ydbi_iterate_tas_gate_ctrl_oper_name(yang_db_item_access_t *ydbia,
                              uc_range **range, uint8_t instIndex,
                              const char *cuc_id, uint32_t schdIndex,
                              cc_gate_control_entry_t *gce)
{
	uint8_t aps[10];
	void *kdata;
	uint32_t ksize;
	uint32_t vsize;
	uint32_t asize;
	uint32_t sindex;
	uint8_t *kv;
	char mydomain_id[]=TSNUNI_DOMAIN_ID;
	void *vdata;
	int res;

	asize=9; /* Fixed Value */
	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	if(*range==NULL){
		set_dpara_k0vk3(ydbia->dbald, &ydbia->dbpara, &instIndex, cuc_id,
				NULL, false);
		ydbia->dbpara.aps[4] = IEEE802_DOT1Q_CNC_CONFIG_TAS_SCHED_LIST;
		ydbia->dbpara.aps[5] = IEEE802_DOT1Q_CNC_CONFIG_GATE_PARAMETER_TABLE;
		ydbia->dbpara.aps[6] = IEEE802_DOT1Q_CNC_CONFIG_ADMIN_CONTROL_LIST;
		ydbia->dbpara.aps[7] = IEEE802_DOT1Q_CNC_CONFIG_GATE_CONTROL_ENTRY;
		ydbia->dbpara.aps[8] = IEEE802_DOT1Q_CNC_CONFIG_OPERATION_NAME;
		ydbia->dbpara.aps[9] = 255;
		memcpy(aps, ydbia->dbpara.aps, sizeof(aps));
		aps[asize-1]+=1;
		*range=uc_get_range(ydbia->dbald, ydbia->dbpara.aps, asize, aps, asize);
		if(*range==NULL){
			res=1;
			goto erexit;
		}
	}
	(void)ub_bytearray2str(&mydomain_id[6], &instIndex, 1);
	while(true){
		if(uc_get_keyvalue_in_range(ydbia->dbald, *range, &kdata, &ksize,
					    &vdata, &vsize, UC_DBAL_FORWARD) != 0){
			// no more data
			res=1;
			goto erexit;
		}
		kv=(uint8_t*)kdata;
		if((ksize<=(asize+2u)) || (kv[asize]!=255u)){continue;}
		// check domain-id
		kv=kv+(asize+1); // node key(aps part) size is 9
		ksize-=(asize+1);
		if(ksize<=kv[0]){continue;}
		if(strcmp(((char*)&kv[1]), mydomain_id)!=0){continue;}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// check cuc-id
		if(ksize<=kv[0]){continue;}
		if(strcmp(((char*)&kv[1]), cuc_id)!=0){continue;}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// get tas-sched-list index
		if(ksize<=kv[0]){continue;}
		ub_non_aligned_intsubst(&kv[1], &sindex, sizeof(uint32_t));
		if(sindex!=schdIndex){continue;}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		// get gate-control-entry index
		if(ksize<=kv[0]){continue;}
		ub_non_aligned_intsubst(&kv[1], &gce->index, sizeof(uint32_t));
		// get gate-control-entry operation-name (identiref)
		ub_non_aligned_intsubst(vdata, &gce->opername, sizeof(uint32_t));
		break;
	}
	res=0;
erexit:
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	if((*range!=NULL) && (res!=0)){
		uc_get_range_release(ydbia->dbald, *range);
		*range=NULL;
	}
	return res;
}

