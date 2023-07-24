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
#include "ietf-keychain.h"
#include "ietf-keychain_access.h"

static void set_dpara_k5vk2(yang_db_access_para_t *dbpara,
			    uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			    uint8_t k5, void *kv1, uint32_t kvs1, void *kv2,
			    uint32_t kvs2, bool status, uint8_t onhw)
{
	dbpara->onhw=onhw;
	dbpara->aps[0]=status?IETF_KEYCHAIN_RO:IETF_KEYCHAIN_RW;
	dbpara->aps[1]=IETF_KEYCHAIN_KEYCHAIN;
	dbpara->aps[2]=k1;
	dbpara->aps[3]=k2;
	dbpara->aps[4]=k3;
	dbpara->aps[5]=k4;
	dbpara->aps[6]=k5;
	dbpara->aps[7]=255u;
	dbpara->kvs[0]=kv1;
	dbpara->kss[0]=kvs1;
	dbpara->kvs[1]=kv2;
	dbpara->kss[1]=kvs2;
	dbpara->kvs[2]=NULL;
}

static int get_item_kck5vk2(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			  void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			  bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k5vk2(&ydbia->dbpara, k1, k2, k3, k4, k5, kv1, kvs1, kv2, kvs2,
			status, YANG_DB_ONHW_NOACTION);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

static int set_item_kck5vk2(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			  void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k5vk2(&ydbia->dbpara, k1, k2, k3, k4, k5, kv1, kvs1, kv2, kvs2,
			status, YANG_DB_ONHW_NOACTION);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

static int del_item_kck5vk2(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			  void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			  bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k5vk2(&ydbia->dbpara, k1, k2, k3, k4, k5, kv1, kvs1, kv2, kvs2,
			status, YANG_DB_ONHW_NOACTION);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_kck5vk2(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			  char *privKeyName, char *certName, bool status)
{
	return get_item_kck5vk2(ydbia, rval, k1, k2, k3, k4, k5,
				privKeyName, strlen(privKeyName)+1, certName, strlen(certName)+1,
				status);
}

int ydbi_set_item_kck5vk2(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			  char *privKeyName, char *certName, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	return set_item_kck5vk2(ydbia, k1, k2, k3, k4, k5,
				privKeyName, strlen(privKeyName)+1, certName, strlen(certName)+1,
				status, value, vsize, notice);
}

int ydbi_del_item_kck5vk2(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, uint8_t k5,
			  char *privKeyName, char *certName, bool status)
{
	return del_item_kck5vk2(ydbia, k1, k2, k3, k4, k5,
				privKeyName, strlen(privKeyName)+1, certName, strlen(certName)+1,
				status);
}

int ydbi_get_item_kck3vk2(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2, uint8_t k3,
			  char *trustedCertGroupNames, char *trustedCertName, bool status)
{
	return get_item_kck5vk2(ydbia, rval, k1, k2, k3, 255, 255,
				trustedCertGroupNames, strlen(trustedCertGroupNames)+1,
				trustedCertName, strlen(trustedCertName)+1, status);
}

int ydbi_set_item_kck3vk2(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3,
			  char *trustedCertGroupNames, char *trustedCertName, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	return set_item_kck5vk2(ydbia, k1, k2, k3, 255, 255,
				trustedCertGroupNames, strlen(trustedCertGroupNames)+1,
				trustedCertName, strlen(trustedCertName)+1, status,
				value, vsize, notice);
}

int ydbi_del_item_kck3vk2(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3,
			  char *trustedCertGroupNames, char *trustedCertName, bool status)
{
	return del_item_kck5vk2(ydbia, k1, k2, k3, 255, 255,
				trustedCertGroupNames, strlen(trustedCertGroupNames)+1,
				trustedCertName, strlen(trustedCertName)+1, status);
}

int ydbi_get_item_kck3vk1(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2, uint8_t k3, char *privKeyName,
			  bool status)
{
	return get_item_kck5vk2(ydbia, rval, k1, k2, k3, 255, 255,
				privKeyName, strlen(privKeyName)+1, NULL, 0, status);
}

int ydbi_set_item_kck3vk1(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3, char *privKeyName,
			  bool status, void *value, uint32_t vsize, uint8_t notice)
{
	return set_item_kck5vk2(ydbia, k1, k2, k3, 255, 255,
				privKeyName, strlen(privKeyName)+1, NULL, 0, status,
				value, vsize, notice);
}

int ydbi_del_item_kck3vk1(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, uint8_t k3, char *privKeyName,
			  bool status)
{
	return del_item_kck5vk2(ydbia, k1, k2, k3, 255, 255,
				privKeyName, strlen(privKeyName)+1, NULL, 0, status);
}

int ydbi_get_item_kck2vk1(yang_db_item_access_t *ydbia, void **rval,
			  uint8_t k1, uint8_t k2, char *trustedCertGroupNames, bool status)
{
	return get_item_kck5vk2(ydbia, rval, k1, k2, 255, 255, 255,
				trustedCertGroupNames, strlen(trustedCertGroupNames)+1,
				NULL, 0, status);
}

int ydbi_set_item_kck2vk1(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, char *trustedCertGroupNames, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	return set_item_kck5vk2(ydbia, k1, k2, 255, 255, 255,
				trustedCertGroupNames, strlen(trustedCertGroupNames)+1,
				NULL, 0, status, value, vsize, notice);
}

int ydbi_del_item_kck2vk1(yang_db_item_access_t *ydbia,
			  uint8_t k1, uint8_t k2, char *trustedCertGroupNames, bool status)
{
	return del_item_kck5vk2(ydbia, k1, k2, 255, 255, 255,
				trustedCertGroupNames, strlen(trustedCertGroupNames)+1,
				NULL, 0, status);
}
