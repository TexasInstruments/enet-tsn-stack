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
#include "ieee802-dot1ab-lldp_access.h"
#include "yang_modules.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee802-dot1ab-lldp_access.h"

extern uint8_t IEEE802_DOT1AB_LLDP_func(uc_dbald *dbald);
#define IEEE802_DOT1AB_LLDP_RW IEEE802_DOT1AB_LLDP_func(dbald)
#define IEEE802_DOT1AB_LLDP_RO (IEEE802_DOT1AB_LLDP_func(dbald)|0x80u)

/*functions prototype*/
static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t k1, bool status);
static void set_dpara_k2vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t k1, uint8_t k2, bool status);

/*Functions definition*/
static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t k1, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1AB_LLDP_RO:IEEE802_DOT1AB_LLDP_RW;
	dbpara->aps[1] = IEEE802_DOT1AB_LLDP_LLDP;
	dbpara->aps[2] = k1;
	dbpara->aps[3] = 255u;
	dbpara->kvs[0]=NULL;
}

static void set_dpara_k2vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t k1, uint8_t k2, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1AB_LLDP_RO:IEEE802_DOT1AB_LLDP_RW;
	dbpara->aps[1] = IEEE802_DOT1AB_LLDP_LLDP;
	dbpara->aps[2] = k1;
	dbpara->aps[3] = k2;
	dbpara->aps[4] = 255u;
	dbpara->kvs[0]=NULL;
}

static void set_dpara_knvkn(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t kn[],
                            uint8_t kn_sz,
                            attribute_pair_t attrs[],
                            uint8_t kvs_sz,
                            bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1AB_LLDP_RO:IEEE802_DOT1AB_LLDP_RW;
	dbpara->aps[1] = IEEE802_DOT1AB_LLDP_LLDP;
	for (uint8_t i=0; i<kn_sz; i++)
	{
		dbpara->aps[i+2] = kn[i];
		// UB_LOG(UBL_DEBUG, "%s: key[%s] \n", __func__, ieee802_dot1ab_lldp_get_string(kn[i]));
	}
	dbpara->aps[2 + kn_sz] = 255u; // kn should <=6
	dbpara->kvs[0]=NULL;

	for (uint8_t i=0; i<kvs_sz; i++)
	{
		ADJUST_ENDIAN(&attrs[i].vk, sizeof(uint32_t)-attrs[i].vk_sz, attrs[i].vk_sz);
		dbpara->kvs[i] = attrs[i].vk;
		dbpara->kss[i] = attrs[i].vk_sz;
		dbpara->kvs[i+1] = NULL;
		// UB_LOG(UBL_DEBUG, "%s: vkey size[%d] \n", __func__, attrs[i].vk_sz);
	}
}

int ydbi_get_item_abk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1,  bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_get_item_abk2vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k2vk0(ydbia->dbald, &ydbia->dbpara, k1, k2, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_set_item_abk1vk0(yang_db_item_access_t *ydbia, void *rval, uint32_t vsize, uint8_t k1,  bool status, uint8_t notice)
{
    if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
    set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, k1, status);
    ydbia->dbpara.value=rval;
	ydbia->dbpara.vsize=vsize;
	int err=ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
	return err;
}

int ydbi_set_item_abk2vk0(yang_db_item_access_t *ydbia, void *rval, uint32_t vsize, uint8_t k1, uint8_t k2, bool status, uint8_t notice)
{
    if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
    set_dpara_k2vk0(ydbia->dbald, &ydbia->dbpara, k1, k2, status);
    ydbia->dbpara.value=rval;
	ydbia->dbpara.vsize=vsize;
	int err=ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
	return err;
}


/**
 * @brief Get val base on n key from LLDP until end
 * Also provided vkn as attribute
 */
int ydbi_get_item_abknvkn(yang_db_item_access_t *ydbia,
                            void **rval,
                            uint8_t kn[],
                            uint8_t kn_sz,
                            attribute_pair_t attrs[],
                            uint8_t kvs_sz,
                            bool status)
{
    if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvkn(ydbia->dbald, &ydbia->dbpara, kn, kn_sz, attrs, kvs_sz, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}


int ydbi_set_item_abknvkn(yang_db_item_access_t *ydbia,
                            uint8_t kn[],
                            uint8_t kn_sz,
                            attribute_pair_t attrs[],
                            uint8_t kvs_sz,
                            bool status,
                            void* value,
                            uint32_t vsize,
                            uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvkn(ydbia->dbald, &ydbia->dbpara, kn, kn_sz, attrs, kvs_sz, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	int err=ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
	return err;
}

int ydbi_del_item_abknvkn(yang_db_item_access_t *ydbia,
                            uint8_t kn[],
                            uint8_t kn_sz,
                            attribute_pair_t attrs[],
                            uint8_t kvs_sz,
                            bool status,
                            uint8_t notice)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvkn(ydbia->dbald, &ydbia->dbpara, kn, kn_sz, attrs, kvs_sz, status);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}
