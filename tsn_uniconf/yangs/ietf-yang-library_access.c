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
#include "yang_modules.h"
#include "yang_db_access.h"
#include "ietf-yang-library.h"
#include "ietf-yang-library_access.h"

static void set_dpara_k5vk4(yang_db_access_para_t *dbpara,
			    uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			    void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			    void *kv3, uint32_t kvs3, void *kv4, uint32_t kvs4,
			    bool status, uint8_t onhw)
{
	dbpara->onhw=onhw;
	dbpara->aps[0] = status?IETF_YANG_LIBRARY_RO:IETF_YANG_LIBRARY_RW;
	dbpara->aps[1] = k1;
	dbpara->aps[2] = k2;
	dbpara->aps[3] = k3;
	dbpara->aps[4] = k4;
	dbpara->aps[5] = k5;
	dbpara->aps[6] = 255u;
	dbpara->kvs[0] = kv1;
	dbpara->kss[0] = kvs1;
	dbpara->kvs[1] = kv2;
	dbpara->kss[1] = kvs2;
	dbpara->kvs[2] = kv3;
	dbpara->kss[2] = kvs3;
	dbpara->kvs[3] = kv4;
	dbpara->kss[3] = kvs4;
	dbpara->kvs[4] = NULL;
	dbpara->kss[4] = 0u;
}

static int get_item_ylk5vk4(yang_db_item_access_t *ydbia, void **rval,
			    uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			    void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			    void *kv3, uint32_t kvs3, void *kv4, uint32_t kvs4)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k5vk4(&ydbia->dbpara, k1, k2, k3, k4, k5,
			kv1, kvs1, kv2, kvs2, kv3, kvs3, kv4, kvs4,
			IETF_YANG_LIBRARY_RO, YANG_DB_ONHW_NOACTION);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_get_item_ylk5vk3(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2,  uint8_t k3, uint8_t k4, uint8_t k5,
			  char *key1, char *key2, char *key3)
{
	return get_item_ylk5vk4(ydbia, rval, k1, k2, k3, k4, k5,
			key1, strlen(key1)+1, key2, strlen(key2)+1, key3, strlen(key3)+1,
			NULL, 0);
}

int ydbi_get_item_ylk4vk4(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2,  uint8_t k3, uint8_t k4,
			  char *key1, char *key2, char *key3, char *key4)
{
	return get_item_ylk5vk4(ydbia, rval, k1, k2, k3, k4, 255,
			key1, strlen(key1)+1, key2, strlen(key2)+1, key3, strlen(key3)+1,
			key4, strlen(key4)+1);
}

int ydbi_get_item_ylk4vk3(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2,  uint8_t k3, uint8_t k4,
			  char *key1, char *key2, char *key3)
{
	return get_item_ylk5vk4(ydbia, rval, k1, k2, k3, k4, 255,
			key1, strlen(key1)+1, key2, strlen(key2)+1, key3, strlen(key3)+1,
			NULL, 0);
}

int ydbi_get_item_ylk4vk2(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2,  uint8_t k3, uint8_t k4,
			  char *key1, char *key2)
{
	return get_item_ylk5vk4(ydbia, rval, k1, k2, k3, k4, 255,
			key1, strlen(key1)+1, key2, strlen(key2)+1, NULL, 0, NULL, 0);
}

int ydbi_get_item_ylk3vk2(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2,  uint8_t k3, char *key1, char *key2)
{
	return get_item_ylk5vk4(ydbia, rval, k1, k2, k3, 255, 255,
			key1, strlen(key1)+1, key2, strlen(key2)+1, NULL, 0, NULL, 0);
}

int ydbi_get_item_ylk3vk1(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2,  uint8_t k3, char *key1)
{
	return get_item_ylk5vk4(ydbia, rval, k1, k2, k3, 255, 255,
			key1, strlen(key1)+1, NULL, 0, NULL, 0, NULL, 0);
}

int ydbi_get_item_ylk2vk0(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2)
{
	return get_item_ylk5vk4(ydbia, rval, k1, k2, 255, 255, 255,
			NULL, 0, NULL, 0, NULL, 0, NULL, 0);
}

