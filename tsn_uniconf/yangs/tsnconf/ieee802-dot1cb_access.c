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
#include "yang_node.h"
#include "yang_modules.h"
#include "yang_db_access.h"
#include "ieee802-dot1q-cnc-config.h"
#include "ieee802-dot1q-cnc-config_access.h"
#include "ieee802-dot1cb-frer.h"
#include "ieee802-dot1cb-stream-identification.h"

extern uint8_t IEEE802_DOT1CB_FRER_func(uc_dbald *dbald);
#define IEEE802_DOT1CB_FRER_RW IEEE802_DOT1CB_FRER_func(dbald)
#define IEEE802_DOT1CB_FRER_RO (IEEE802_DOT1CB_FRER_func(dbald)|0x80u)

extern uint8_t IEEE802_DOT1CB_STREAM_IDENTIFICATION_func(uc_dbald *dbald);
#define IEEE802_DOT1CB_STREAM_IDENTIFICATION_RW \
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_func(dbald)
#define IEEE802_DOT1CB_STREAM_IDENTIFICATION_RO \
	(IEEE802_DOT1CB_STREAM_IDENTIFICATION_func(dbald)|0x80u)

static void set_dpara_cbfr(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			   char* brname, uint32_t *index, uint8_t k1, uint8_t k2,
			   bool status, bool ismirror)
{
	uint8_t apsidx=0u, kvsidx=0u;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	if(ismirror && NULL!=brname){
		dbpara->aps[apsidx++]=XL4_DATA_RW;
		dbpara->aps[apsidx++]=UC_MIRROR;
		dbpara->aps[apsidx++]=UC_MIRROR_DEVICE;
	}
	dbpara->aps[apsidx++] = status?IEEE802_DOT1CB_FRER_RO:IEEE802_DOT1CB_FRER_RW;
	dbpara->aps[apsidx++] = IEEE802_DOT1CB_FRER_FRER;
	dbpara->aps[apsidx++] = k1;
	dbpara->aps[apsidx++] = k2;
	dbpara->aps[apsidx++] = 255u;
	if(ismirror && NULL!=brname){
		dbpara->kvs[kvsidx]=(void*)brname;
		dbpara->kss[kvsidx++]=strlen(brname)+1u;
	}
	dbpara->kvs[kvsidx]=index;
	dbpara->kss[kvsidx++]=sizeof(uint32_t);
	dbpara->kvs[kvsidx]=NULL;
	dbpara->kss[kvsidx++]=0u;
}

int ydbi_set_needaction_cbfr(yang_db_item_access_t *ydbia, bool dereg)
{
	uint32_t dummy=0;
	int res;
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, NULL, &dummy,
		       IEEE802_DOT1CB_FRER_SEQUENCE_GENERATION,
		       IEEE802_DOT1CB_FRER_RESET,
		       YDBI_CONFIG, YDBI_NON_MIRROR);
	ydbia->dbpara.kvs[0]=NULL; // don't select index, apply on all indices
	res=yang_db_set_needaction(ydbia->dbald, ydbia->dbpara.aps, ydbia->dbpara.kvs,
				   ydbia->dbpara.kss, dereg);
	ydbia->dbpara.aps[2] = IEEE802_DOT1CB_FRER_SEQUENCE_RECOVERY;
	res|=yang_db_set_needaction(ydbia->dbald, ydbia->dbpara.aps, ydbia->dbpara.kvs,
				   ydbia->dbpara.kss, dereg);
	return res;
}

void ydbi_set_dbpara_cbk2vk0(yang_db_item_access_t *ydbia, char* brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status,
		       bool ismirror)
{
	return set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, brname, index,
			k1, k2, status, ismirror);
}

int ydbi_get_item_cbfr(yang_db_item_access_t *ydbia, void **rval,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cbfr(yang_db_item_access_t *ydbia,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cbfr(yang_db_item_access_t *ydbia,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status,
		       void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cbfr(yang_db_item_access_t *ydbia,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cbfrm(yang_db_item_access_t *ydbia, void **rval, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cbfrm(yang_db_item_access_t *ydbia, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cbfrm(yang_db_item_access_t *ydbia, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status,
		       void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cbfrm(yang_db_item_access_t *ydbia, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbfr(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

static void set_dpara_cbsid(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			   char* brname, uint32_t *index, uint8_t k1, uint8_t k2,
			   bool status, bool ismirror)
{
	uint8_t apsidx=0u, kvsidx=0u;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	if(ismirror && NULL!=brname){
		dbpara->aps[apsidx++]=XL4_DATA_RW;
		dbpara->aps[apsidx++]=UC_MIRROR;
		dbpara->aps[apsidx++]=UC_MIRROR_DEVICE;
	}
	dbpara->aps[apsidx++] = status?IEEE802_DOT1CB_STREAM_IDENTIFICATION_RO:
		IEEE802_DOT1CB_STREAM_IDENTIFICATION_RW;
	dbpara->aps[apsidx++] = IEEE802_DOT1CB_STREAM_IDENTIFICATION_STREAM_IDENTITY;
	dbpara->aps[apsidx++] = k1;
	dbpara->aps[apsidx++] = k2;
	dbpara->aps[apsidx++] = 255u;
	if(ismirror && NULL!=brname){
		dbpara->kvs[kvsidx]=(void*)brname;
		dbpara->kss[kvsidx++]=strlen(brname)+1u;
	}
	dbpara->kvs[kvsidx]=index;
	dbpara->kss[kvsidx++]=sizeof(uint32_t);
	dbpara->kvs[kvsidx]=NULL;
	dbpara->kss[kvsidx++]=0u;
}

int ydbi_get_item_cbsid(yang_db_item_access_t *ydbia, void **rval,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cbsid(yang_db_item_access_t *ydbia,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cbsid(yang_db_item_access_t *ydbia,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status,
		       void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cbsid(yang_db_item_access_t *ydbia,
		       uint32_t index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, NULL, &index, k1, k2, status,
			YDBI_NON_MIRROR);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cbsidm(yang_db_item_access_t *ydbia, void **rval, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cbsidm(yang_db_item_access_t *ydbia, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cbsidm(yang_db_item_access_t *ydbia, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status,
		       void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cbsidm(yang_db_item_access_t *ydbia, char *brname,
		       uint32_t *index, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_cbsid(ydbia->dbald, &ydbia->dbpara, brname, index, k1, k2, status,
			YDBI_MIRROR);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int cbsid_find_handle(uc_dbald *dbald, uint32_t handle)
{
	uc_range *range;
	uint8_t key1[3]={IEEE802_DOT1CB_STREAM_IDENTIFICATION_RW,
		IEEE802_DOT1CB_STREAM_IDENTIFICATION_STREAM_IDENTITY,
		IEEE802_DOT1CB_STREAM_IDENTIFICATION_HANDLE,
	};
	uint8_t key2[3];
	void *kdata;
	uint32_t ksize;
	void *vdata;
	uint32_t vsize;
	int res=-1;
	uint32_t rv;
	memcpy(key2, key1, 3);
	key2[2]+=1;
	range=uc_get_range(dbald, key1, 3, key2, 3);
	if(range==NULL){return -1;}
	while(true){
		if(uc_get_keyvalue_in_range(dbald, range, &kdata, &ksize,
					    &vdata, &vsize,
					    UC_DBAL_FORWARD) != 0){break;}
		if(vsize!=sizeof(uint32_t)){
			UB_LOG(UBL_ERROR, "%s:invalid vsize=%d\n", __func__, vsize);
			break;
		}
		if(ksize!=9 || ((uint8_t*)kdata)[3]!=0xff || ((uint8_t*)kdata)[4]!=4){
			UB_LOG(UBL_ERROR, "%s:invalid kdata\n", __func__);
			break;
		}
		ub_non_aligned_intsubst(vdata, &rv, 4);
		if(rv!=handle){continue;}
		ub_non_aligned_intsubst(&((uint8_t*)kdata)[5], &res, 4);
		break;
	}
	return res;
}
