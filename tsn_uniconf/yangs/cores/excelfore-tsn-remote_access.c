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
#include "excelfore-tsn-remote.h"

extern uint8_t EXCELFORE_TSN_REMOTE_func(uc_dbald *dbald);
#define EXCELFORE_TSN_REMOTE_RW EXCELFORE_TSN_REMOTE_func(dbald)
#define EXCELFORE_TSN_REMOTE_RO (EXCELFORE_TSN_REMOTE_func(dbald)|0x80u)

static uint8_t ydbi_instIndex;
static uint32_t ydbi_protn;
static void set_dpara_k4vk1(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t instIndex, char *portif, int protn,
			    uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			    void *kv1, uint32_t kvs1,
			    bool status)
{
	ydbi_instIndex=instIndex;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?EXCELFORE_TSN_REMOTE_RO:EXCELFORE_TSN_REMOTE_RW;
	dbpara->aps[1] = EXCELFORE_TSN_REMOTE_TSN_REMOTE;
	dbpara->aps[2] = EXCELFORE_TSN_REMOTE_INSTANCES;
	dbpara->aps[3] = EXCELFORE_TSN_REMOTE_INSTANCE;
	dbpara->aps[4] = EXCELFORE_TSN_REMOTE_EXTERNAL_CONTROL_MAN;
	dbpara->aps[5] = k1;
	dbpara->aps[6] = k2;
	dbpara->aps[7] = k3;
	dbpara->aps[8] = k4;
	dbpara->aps[9] = 255u;
	dbpara->kvs[0]=&ydbi_instIndex;
	dbpara->kss[0]=sizeof(uint8_t);
	dbpara->kvs[1]=portif;
	if(portif){dbpara->kss[1]=strlen(portif)+1;}
	if(protn>=0){
		ydbi_protn=protn;
		dbpara->kvs[2]=&ydbi_protn;
	}else{
		dbpara->kvs[2]=NULL;
	}
	if(protn){dbpara->kss[2]=sizeof(uint32_t);}
	dbpara->kvs[3]=kv1;
	dbpara->kss[3]=kvs1;
	dbpara->kvs[4]=NULL;
}

static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t instIndex,  uint8_t k1, bool status)
{
	ydbi_instIndex=instIndex;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?EXCELFORE_TSN_REMOTE_RO:EXCELFORE_TSN_REMOTE_RW;
	dbpara->aps[1] = EXCELFORE_TSN_REMOTE_TSN_REMOTE;
	dbpara->aps[2] = EXCELFORE_TSN_REMOTE_INSTANCES;
	dbpara->aps[3] = EXCELFORE_TSN_REMOTE_INSTANCE;
	dbpara->aps[4] = EXCELFORE_TSN_REMOTE_UPDATE;
	dbpara->aps[5] = k1;
	dbpara->aps[6] = 255;
	dbpara->kvs[0]=&ydbi_instIndex;
	dbpara->kss[0]=sizeof(uint8_t);
	dbpara->kvs[1]=NULL;
}

int ydbi_get_item_trk4vk1(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  char *portif, uint32_t protn,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex,
			portif, protn, k1, k2, k3, k4,
			kv1, kvs1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_trk4vk1(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  char *portif, uint32_t protn,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex,
			portif, protn, k1, k2, k3, k4,
			kv1, kvs1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_trk4vk1(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  char *portif, uint32_t protn,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1,
			  bool status, void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex,
			portif, protn, k1, k2, k3, k4,
			kv1, kvs1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_get_item_trk2vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex,
			NULL, -1, k1, k2, 255, 255, NULL, 0, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_trk2vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex,
			NULL, -1, k1, k2, 255, 255, NULL, 0, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_trk2vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex,
			NULL, -1, k1, k2, 255, 255, NULL, 0, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_get_item_trk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_trk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_trk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}
