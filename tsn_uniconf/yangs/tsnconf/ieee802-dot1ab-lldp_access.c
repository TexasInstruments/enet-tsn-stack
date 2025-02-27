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
#include "yang_node.h"
#include "yang_modules.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee802-dot1ab-lldp_access.h"

extern uint8_t IEEE802_DOT1AB_LLDP_func(uc_dbald *dbald);
#define IEEE802_DOT1AB_LLDP_RW IEEE802_DOT1AB_LLDP_func(dbald)
#define IEEE802_DOT1AB_LLDP_RW_Y IEEE802_DOT1AB_LLDP_func(ydbia->dbald)
#define IEEE802_DOT1AB_LLDP_RO (IEEE802_DOT1AB_LLDP_func(dbald)|0x80u)
#define IEEE802_DOT1AB_LLDP_RO_Y (IEEE802_DOT1AB_LLDP_func(ydbia->dbald)|0x80u)

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
	uint8_t i=0;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1AB_LLDP_RO:IEEE802_DOT1AB_LLDP_RW;
	dbpara->aps[1] = IEEE802_DOT1AB_LLDP_LLDP;
	for (i=0; i<kn_sz; i++)
	{
		dbpara->aps[i+2] = kn[i];
		// UB_LOG(UBL_DEBUG, "%s: key[%s] \n", __func__, ieee802_dot1ab_lldp_get_string(kn[i]));
	}
	dbpara->aps[2 + kn_sz] = 255u; // kn should <=6
	dbpara->kvs[0]=NULL;

	for (i=0; i<kvs_sz; i++)
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


void ydbi_set_dbpara_abk1vk1(yang_db_item_access_t *ydbia, char* brname,
                             char *name, uint8_t *destmac, uint8_t k1,
                             bool ismirror, bool iswildcard)
{
	uint8_t apsidx=0u, kvsidx=0u;
	if(ismirror && NULL!=brname){
		ydbia->dbpara.aps[apsidx++]=XL4_DATA_RW;
		ydbia->dbpara.aps[apsidx++]=UC_MIRROR;
		ydbia->dbpara.aps[apsidx++]=UC_MIRROR_DEVICE;
	}
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_RO_Y;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_LLDP;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_PORT;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA;
	ydbia->dbpara.aps[apsidx++]=k1;
	ydbia->dbpara.aps[apsidx++]=255;
	if(ismirror && NULL!=brname){
		ydbia->dbpara.kvs[kvsidx]=(void*)brname;
		ydbia->dbpara.kss[kvsidx++]=strlen(brname)+1u;
	}
	if(NULL != name){
		ydbia->dbpara.kvs[kvsidx]=name;
		ydbia->dbpara.kss[kvsidx++]=strlen(name)+1u;
		if(NULL != destmac){
			ydbia->dbpara.kvs[kvsidx]=destmac;
			ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_MAC_ADDRESS);
			if(iswildcard){
				/* set time-marks and remote-index kss if for wildcard search */
				ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
				ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
			}
		}else if(iswildcard){
			/* set dest-mac-address, time-marks and remote-index kss if for
			 * wildcard search */
			ydbia->dbpara.kvs[kvsidx]=NULL;
			ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_MAC_ADDRESS);
			ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
			ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
		}
	}
	ydbia->dbpara.kvs[kvsidx]=NULL;
	ydbia->dbpara.kss[kvsidx++]=0u;
}

int ydbi_iterate_portid(yang_db_item_access_t *ydbia, uc_range **range,
                        lldp_remote_portid_info_t *portid, bool ismirror)
{
	uint8_t aps[9];
	uint8_t apsidx=0u;
	void *kdata;
	uint32_t ksize;
	uint8_t *kv;
	int res;
	void *vdata;
	uint32_t vsize;

	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	if(*range==NULL){
		if(ismirror) {
			ydbia->dbpara.aps[apsidx++]=XL4_DATA_RW;
			ydbia->dbpara.aps[apsidx++]=UC_MIRROR;
			ydbia->dbpara.aps[apsidx++]=UC_MIRROR_DEVICE;
		}
		ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_RO_Y;
		ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_LLDP;
		ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_PORT;
		ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA;
		ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_PORT_ID;
		ydbia->dbpara.aps[apsidx]=255;
		memcpy(aps, ydbia->dbpara.aps, sizeof(aps));
		aps[apsidx-1]+=1;
		*range=uc_get_range(ydbia->dbald, ydbia->dbpara.aps, apsidx, aps, apsidx);
		if(*range==NULL){
			res=1;
			UB_LOG(UBL_ERROR, "%s:uc_get_range() returned NULL\n", __func__);
			goto erexit;
		}
	}
	while(range != NULL){
		if(uc_get_keyvalue_in_range(ydbia->dbald, *range, &kdata, &ksize,
					    &vdata, &vsize, UC_DBAL_FORWARD) != 0){
			// no more data
			res=1;
			goto erexit;
		}
		kv=(uint8_t*)kdata;
		if(ismirror){
			if((ksize<=10u) || (kv[8]!=255u)){continue;}
			kv=kv+9; // node key(aps part) size is 9
			ksize-=9;
			if(ksize<=kv[0]){continue;}
			if(NULL!=portid->brname){
				if(strcmp(((char*)&kv[1]), portid->brname)!=0){continue;}
			} else {
				portid->brname=(char*)&kv[1];
			}
			ksize-=kv[0]+1;
			kv=kv+kv[0]+1;
		}else{
			if((ksize<=7u) || (kv[5]!=255u)){continue;}
			kv=kv+6; // node key(aps part) size is 6
			ksize-=6;
		}
		if(ksize<=kv[0]){continue;}
		if(NULL!=portid->ifname){
			if(strcmp(((char*)&kv[1]), portid->ifname)!=0){continue;}
		}else{
			portid->ifname=(char*)&kv[1];
		}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		if(ksize<=kv[0]){continue;}
		if(NULL!=portid->dstmacaddr){
			if(memcmp(((uint8_t*)&kv[1]), portid->dstmacaddr, 6)!=0){continue;}
		}else{
			portid->dstmacaddr=(uint8_t*)&kv[1];
		}
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		if(ksize<=kv[0]){continue;}
		ub_non_aligned_intsubst(&kv[1], &portid->timemarks, sizeof(uint32_t));
		ksize-=kv[0]+1;
		kv=kv+kv[0]+1;
		if(ksize<=kv[0]){continue;}
		ub_non_aligned_intsubst(&kv[1], &portid->remoteidx, sizeof(uint32_t));
		portid->portid=((const char *)vdata);
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

int ydbi_get_portid_subtype(yang_db_item_access_t *ydbia,
                            lldp_remote_portid_info_t *portid, bool ismirror)
{
	int res;
	void *value=NULL;
	uint8_t apsidx=0u, kvsidx=0u;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	if(ismirror){
		ydbia->dbpara.aps[apsidx++]=XL4_DATA_RW;
		ydbia->dbpara.aps[apsidx++]=UC_MIRROR;
		ydbia->dbpara.aps[apsidx++]=UC_MIRROR_DEVICE;
	}
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_RO_Y;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_LLDP;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_PORT;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE;
	ydbia->dbpara.aps[apsidx++]=255;
	if(ismirror){
		ydbia->dbpara.kvs[kvsidx]=(void*)portid->brname;
		if(NULL!=portid->brname){
			ydbia->dbpara.kss[kvsidx++]=strlen(portid->brname)+1u;
		}
	}
	ydbia->dbpara.kvs[kvsidx]=(void*)portid->ifname;
	ydbia->dbpara.kss[kvsidx++]=strlen(portid->ifname)+1;
	ydbia->dbpara.kvs[kvsidx]=(void*)portid->dstmacaddr;
	ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_MAC_ADDRESS);
	ydbia->dbpara.kvs[kvsidx]=(void*)(&portid->timemarks);
	ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[kvsidx]=(void*)(&portid->remoteidx);
	ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[kvsidx]=NULL;
	ydbia->dbpara.kss[kvsidx++]=0u;
	res=ydbi_get_foot(ydbia, __func__, &value, UBL_INFO);
	if(res<0){return -1;}
	portid->subtype=*((lldp_port_id_subtype_t*)value);
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return 0;
}

int ydbi_del_remote_systems_data(yang_db_item_access_t *ydbia, uint8_t k1,
                                 lldp_remote_portid_info_t *portid, bool ismirror)
{
	uint8_t apsidx=0u, kvsidx=0u;
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	if(ismirror){
		ydbia->dbpara.aps[apsidx++]=XL4_DATA_RW;
		ydbia->dbpara.aps[apsidx++]=UC_MIRROR;
		ydbia->dbpara.aps[apsidx++]=UC_MIRROR_DEVICE;
	}
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_RO_Y;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_LLDP;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_PORT;
	ydbia->dbpara.aps[apsidx++]=IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA;
	ydbia->dbpara.aps[apsidx++]=k1;
	ydbia->dbpara.aps[apsidx++]=255u;
	if(ismirror){
		ydbia->dbpara.kvs[kvsidx]=(void*)portid->brname;
		if(NULL!=portid->brname){
			ydbia->dbpara.kss[kvsidx++]=strlen(portid->brname)+1u;
		}
	}
	ydbia->dbpara.kvs[kvsidx]=(void*)portid->ifname;
	ydbia->dbpara.kss[kvsidx++]=strlen(portid->ifname)+1;
	ydbia->dbpara.kvs[kvsidx]=(void*)portid->dstmacaddr;
	ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_MAC_ADDRESS);
	ydbia->dbpara.kvs[kvsidx]=(void*)(&portid->timemarks);
	ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[kvsidx]=(void*)(&portid->remoteidx);
	ydbia->dbpara.kss[kvsidx++]=yang_sizeof_vtype(YANG_VTYPE_UINT32);
	ydbia->dbpara.kvs[kvsidx]=NULL;
	ydbia->dbpara.kss[kvsidx++]=0u;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

