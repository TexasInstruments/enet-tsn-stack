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
#include "excelfore-aed_access.h"

extern uint8_t EXCELFORE_AED_func(uc_dbald *dbald);
#define EXCELFORE_AED_RW    EXCELFORE_AED_func(dbald)
#define EXCELFORE_AED_RW_Y  EXCELFORE_AED_func(ydbia->dbald)
#define EXCELFORE_AED_RO   (EXCELFORE_AED_func(dbald)|0x80u)
#define EXCELFORE_AED_RO_Y (EXCELFORE_AED_func(ydbia->dbald)|0x80u)

static uint8_t ydbi_instIndex;
static uint8_t ydbi_streamType;
static uint8_t ydbi_streamIndex;

static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t instIndex, uint8_t streamType, uint8_t streamIndex,
			    uint8_t k1, bool status)
{
	ydbi_instIndex=instIndex;
	ydbi_streamType=streamType;
	ydbi_streamIndex=streamIndex;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?EXCELFORE_AED_RO:EXCELFORE_AED_RW;
	dbpara->aps[1] = EXCELFORE_AED_INSTANCE;
	dbpara->aps[2] = EXCELFORE_AED_AED_EVENT;
	dbpara->aps[3] = EXCELFORE_AED_STREAM;
	dbpara->aps[4] = k1;
	dbpara->aps[5] = 255;
	dbpara->kvs[0]=&ydbi_instIndex;
	dbpara->kvs[1]=&ydbi_streamType;
	dbpara->kvs[2]=&ydbi_streamIndex;
	dbpara->kvs[3]=NULL;
	dbpara->kss[0]=sizeof(uint8_t);
	dbpara->kss[1]=sizeof(uint8_t);
	dbpara->kss[2]=sizeof(uint8_t);
}

int ydbi_get_item_aedk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  uint8_t streamType, uint8_t streamIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamType, streamIndex, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_aedk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t streamType, uint8_t streamIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamType, streamIndex, k1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_aedk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t streamType, uint8_t streamIndex,
			  uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, streamType, streamIndex, k1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_set_stream_status_ucnotice(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t streamType, uint8_t streamIndex,
			  UC_NOTICE_SIG_T **sem, const char *semname)
{
	void *kvs[5]={&instIndex, &streamType, &streamIndex, (char*)semname, NULL};
	uint8_t kss[4]={sizeof(uint8_t), sizeof(uint8_t), sizeof(uint8_t),  0};
	uint8_t aps[6]={EXCELFORE_AED_RO_Y, EXCELFORE_AED_INSTANCE, EXCELFORE_AED_AED_EVENT,
			EXCELFORE_AED_STREAM, EXCELFORE_AED_STATUS, 255};
	kss[3]=strlen(semname)+1;
	return uc_nc_notice_register(ydbia->ucntd, ydbia->dbald, aps, kvs, kss,
				     UC_NOTICE_DBVAL_ADD, sem);
}

// this delete all of registeration with the same semname by ydbi_set_ucnotice
int ydbi_clear_stream_status_ucnotice(yang_db_item_access_t *ydbia, const char *semname)
{
	return uc_nc_notice_deregister_all(ydbia->ucntd, ydbia->dbald, semname);
}

int ydbi_get_stream_status_ucnotice(yang_db_item_access_t *ydbia, uint8_t *instIndex,
				uint8_t *streamType, uint8_t *streamIndex,
				uint32_t *status, const char *semname)
{
	uint32_t ksize, esize, vsize=0;
	uint8_t key[UC_MAX_KEYSIZE];
	int res;
	void *value;

	res=uc_nc_get_notice_act(ydbia->ucntd, ydbia->dbald, semname, key, &ksize);
	if(res){
		UB_LOG(UBL_DEBUG, "%s:no data, res=%d\n", __func__, res);
		return -1;
	}
	esize=6+sizeof(uint8_t)+1+sizeof(uint8_t)+1+sizeof(uint8_t)+1;
	if(ksize!=esize){
		UB_LOG(UBL_ERROR, "%s:ksize=%d != %d\n", __func__, ksize, esize);
		return -1;
	}

	res=uc_dbal_get(ydbia->dbald, key, ksize, &value, &vsize);
	if(res || (vsize!=4) ){
		UB_LOG(UBL_ERROR, "%s:no value OR wrong vsize, res=%d, vsize=%d\n",
		       __func__, res, vsize);
		return -1;
	}
	*status=*((uint32_t*)value);
	*streamIndex=*((uint8_t*)&key[esize-sizeof(uint8_t)]);
	*streamType=*((uint8_t*)&key[esize-sizeof(uint8_t)-sizeof(uint8_t)-1]);
	*instIndex=*((uint8_t*)&key[esize-sizeof(uint8_t)-1-sizeof(uint8_t)-1-sizeof(uint8_t)]);

	uc_dbal_get_release(ydbia->dbald, key, ksize, value, vsize);
	return res;
}