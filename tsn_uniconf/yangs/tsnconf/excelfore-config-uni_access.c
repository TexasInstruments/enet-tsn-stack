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
#include "../ieee802-dot1q-bridge_access.h"
#include "../ieee802-dot1q-cnc-config_access.h"
#include "excelfore-config-uni_access.h"
#include "excelfore-config-uni.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

extern uint8_t EXCELFORE_CONFIG_UNI_func(uc_dbald *dbald);
#define EXCELFORE_CONFIG_UNI_RW EXCELFORE_CONFIG_UNI_func(dbald)
#define EXCELFORE_CONFIG_UNI_RO (EXCELFORE_CONFIG_UNI_func(dbald)|0x80u)

static char domain_id[]=TSNUNI_DOMAIN_ID;
static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    const char* bridgename, uint8_t instIndex,
			    uint8_t k1, bool status)
{
	if(bridgename==NULL){
		bridgename=qb_get_bridge_name(dbald, instIndex);
		if(bridgename==NULL){
			bridgename=DOT1Q_DEFAULT_BRIDGE_NAME;
		}
	}
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?EXCELFORE_CONFIG_UNI_RO:EXCELFORE_CONFIG_UNI_RW;
	dbpara->aps[1] = EXCELFORE_CONFIG_UNI_XL4_UNI;
	dbpara->aps[2] = EXCELFORE_CONFIG_UNI_DOMAIN;
	dbpara->aps[3] = EXCELFORE_CONFIG_UNI_NCONF_SERVERS;
	dbpara->aps[4] = k1;
	dbpara->aps[5] = 255u;
	dbpara->kvs[0]=domain_id;
	dbpara->kss[0]=strlen(domain_id)+1u;
	(void)ub_bytearray2str(&domain_id[6], &instIndex, 1);
	dbpara->kvs[1]=(void*)bridgename;
	dbpara->kss[1]=strlen(bridgename)+1u;
	dbpara->kvs[2]=NULL;
}

static void set_dpara_nbk1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			      uint8_t instIndex, uint8_t k1, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?EXCELFORE_CONFIG_UNI_RO:EXCELFORE_CONFIG_UNI_RW;
	dbpara->aps[1] = EXCELFORE_CONFIG_UNI_XL4_UNI;
	dbpara->aps[2] = EXCELFORE_CONFIG_UNI_DOMAIN;
	dbpara->aps[3] = k1;
	dbpara->aps[4] = 255u;
	dbpara->kvs[0]=domain_id;
	dbpara->kss[0]=strlen(domain_id)+1u;
	(void)ub_bytearray2str(&domain_id[6], &instIndex, 1);
	dbpara->kvs[1]=NULL;
}

int ydbi_get_item_cunbk1vk0(yang_db_item_access_t *ydbia, void **rval,
			    uint8_t instIndex, uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cunbk1vk0(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cunbk1vk0(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, uint8_t k1, bool status,
			    void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cunbk1vk0(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cuk1vk0(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cuk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cuk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cuk1vk0(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_cuck1vk0(yang_db_item_access_t *ydbia, void **rval,
			   const char* bridgename, uint8_t instIndex,
			   uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	ydbia->dbpara.aps[3] = EXCELFORE_CONFIG_UNI_NCONF_CLIENTS;
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_cuck1vk0(yang_db_item_access_t *ydbia,
			   const char* bridgename, uint8_t instIndex,
			   uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	ydbia->dbpara.aps[3] = EXCELFORE_CONFIG_UNI_NCONF_CLIENTS;
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_cuck1vk0(yang_db_item_access_t *ydbia,
			   const char* bridgename, uint8_t instIndex,
			   uint8_t k1, bool status,
			   void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	ydbia->dbpara.aps[3] = EXCELFORE_CONFIG_UNI_NCONF_CLIENTS;
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_cuck1vk0(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint8_t k1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, bridgename, instIndex, k1, status);
	ydbia->dbpara.aps[3] = EXCELFORE_CONFIG_UNI_NCONF_CLIENTS;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_iterate_item_cuk1vk0(yang_db_item_access_t *ydbia,
				  uc_range **range, const char **brname,
				  void **vdata, uint32_t *vsize,
				  uint8_t instIndex,
				  uint8_t k1, bool status)
{
	uint8_t aps[5];
	void *kdata;
	uint32_t ksize;
	char mydomain_id[]=TSNUNI_DOMAIN_ID;
	int res=-1;
	uint8_t dmsize;
	const char *dmstr;
	uint32_t i;

	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	if(*range==NULL){
		set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, NULL, instIndex, k1, status);
		memcpy(aps, ydbia->dbpara.aps, 5);
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
					    vdata, vsize, UC_DBAL_FORWARD) != 0){
			// no more data
			res=1;
			goto erexit;
		}
		for(i=0;i<ksize;i++){
			if(((uint8_t*)kdata)[i]==255u){break;};
		}
		if(i==ksize){goto erexit;}
		dmsize=((uint8_t*)kdata)[i+1];
		dmstr=&((const char*)kdata)[i+2];
		if(dmsize!=strlen(dmstr)+1){goto erexit;}
		if(ksize<=i+3+dmsize){goto erexit;}
		if(strcmp(dmstr, mydomain_id)!=0){continue;}
		*brname=&((const char*)kdata)[i+3+dmsize];
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

bool ydbi_is_cuc_cnc_name(yang_db_item_access_t *ydbia, uint8_t instIndex,
                          const char* brname, uint8_t pos)
{
	bool bret=false;
	void *value=NULL;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, brname, instIndex,
			EXCELFORE_CONFIG_UNI_ROLE, YDBI_STATUS);
	int res=ydbi_get_foot(ydbia, __func__, &value, UBL_DEBUGV);
	if(res>0){
		if(IS_ROLE_ENABLED((*((uint8_t*)value)), pos)>0U){bret=true;}
	}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	return bret;
}

// non-CUC calls this to push event
int ydbi_push_event_bridges(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, const char* bridgename)
{
	uc_dbald *dbald=ydbia->dbald;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	ydbia->dbpara.atype=YANG_DB_ACTION_APPEND;
	ydbia->dbpara.onhw=YANG_DB_ONHW_NOACTION;
	ydbia->dbpara.aps[0] = EXCELFORE_CONFIG_UNI_RW;
	ydbia->dbpara.aps[1] = EXCELFORE_CONFIG_UNI_XL4_UNI;
	ydbia->dbpara.aps[2] = EXCELFORE_CONFIG_UNI_DOMAIN;
	ydbia->dbpara.aps[3] = EXCELFORE_CONFIG_UNI_EVENT_BRIDGES;
	ydbia->dbpara.aps[4] = 255u;
	ydbia->dbpara.kvs[0]=domain_id;
	ydbia->dbpara.kss[0]=strlen(domain_id)+1u;
	ydbia->dbpara.kvs[1]=NULL;
	(void)ub_bytearray2str(&domain_id[6], &instIndex, 1);
	ydbia->dbpara.value=(void*)bridgename;
	ydbia->dbpara.vsize=strlen(bridgename)+1;
	// a notice goes to uniconf, and then to 'CUC'
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, true);
}

// CUC calls this to pull a pushed event, bridgename must have 32+1 bytes size
int ydbi_pop_event_bridges(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, char *bridgename)
{
	int len;
	uc_dbald *dbald=ydbia->dbald;
	int res;
	char *ndata;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	ydbia->dbpara.onhw=YANG_DB_ONHW_NOACTION;
	ydbia->dbpara.aps[0] = EXCELFORE_CONFIG_UNI_RW;
	ydbia->dbpara.aps[1] = EXCELFORE_CONFIG_UNI_XL4_UNI;
	ydbia->dbpara.aps[2] = EXCELFORE_CONFIG_UNI_DOMAIN;
	ydbia->dbpara.aps[3] = EXCELFORE_CONFIG_UNI_EVENT_BRIDGES;
	ydbia->dbpara.aps[4] = 255u;
	ydbia->dbpara.kvs[0]=domain_id;
	ydbia->dbpara.kss[0]=strlen(domain_id)+1u;
	ydbia->dbpara.kvs[1]=NULL;
	(void)ub_bytearray2str(&domain_id[6], &instIndex, 1);
	res=yang_db_action(dbald, NULL, &ydbia->dbpara);
	if(res!=0){
		(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
		return -1;
	}
	ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
	if(ydbia->dbpara.vsize<=1){
		res=-1;
		goto erexit;
	}
	len=strlen((char*)ydbia->dbpara.value)+1;
	if((int)ydbia->dbpara.vsize<len){
		UB_LOG(UBL_ERROR, "%s:invalid data, vsize=%d, len=%d\n",
		       __func__, ydbia->dbpara.vsize, len);
		res=-1;
		goto erexit;
	}
	ub_strncpy(bridgename, (char*)ydbia->dbpara.value, UB_MIN(33, len));
	ydbia->dbpara.value=&(((char*)ydbia->dbpara.value)[len]);
	ydbia->dbpara.vsize-=len;
	ndata=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, ydbia->dbpara.vsize);
	if(ub_assert_fatal(ndata!=NULL, __func__, NULL)){
		res=-1;
		goto erexit;
	}
	memcpy(ndata, ydbia->dbpara.value, ydbia->dbpara.vsize);
	ydbia->dbpara.value=ndata;
	ydbia->dbpara.atype=YANG_DB_ACTION_CREATE;
	res=yang_db_action(dbald, NULL, &ydbia->dbpara);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, ndata);
erexit:
	ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	ydbi_rel_foot(ydbia, __func__);
	return res;
}

#define DEFAULT_MAC_POOL_START {0x91,0xE0,0xF0,0x00,0xFE,0x00}
int ydbi_init_mac_pool(yang_db_item_access_t *ydbia,
		       uint8_t instIndex, uint8_t *addr, int num)
{
	int res;
	uint64_t busy=0;
	int i;
	ub_macaddr_t default_addr=DEFAULT_MAC_POOL_START;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex,
			  EXCELFORE_CONFIG_UNI_MAC_ADDRESS_POOL, YDBI_CONFIG);
	ydbia->dbpara.aps[4]=EXCELFORE_CONFIG_UNI_TOP_ADDRESS;
	ydbia->dbpara.aps[5]=255u;
	if(addr==NULL){addr=default_addr;}
	ydbia->dbpara.value=addr;
	ydbia->dbpara.vsize=6;
	res=ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
	if(res!=0){return -1;}

	for(i=0;i<num;i++){
		busy|=1<<i;
	}
	busy=~busy;
	ydbia->dbpara.aps[4]=EXCELFORE_CONFIG_UNI_BUSY_BITMAP;
	ydbia->dbpara.value=&busy;
	ydbia->dbpara.vsize=8;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_mac_in_pool(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, ub_macaddr_t rmac)
{
	void *rval;
	uint64_t busy;
	int i,j,k,res;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex,
			  EXCELFORE_CONFIG_UNI_MAC_ADDRESS_POOL, YDBI_CONFIG);
	ydbia->dbpara.aps[4]=EXCELFORE_CONFIG_UNI_TOP_ADDRESS;
	ydbia->dbpara.aps[5]=255u;
	res=ydbi_get_foot(ydbia, __func__, &rval, UBL_ERROR);
	if(res<0){
		UB_LOG(UBL_ERROR, "%s:not initialized\n", __func__);
		return -1;
	}
	if(res==6){memcpy(rmac, rval, 6);}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	if(res!=6){return -1;}

	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex,
			  EXCELFORE_CONFIG_UNI_MAC_ADDRESS_POOL, YDBI_CONFIG);
	ydbia->dbpara.aps[4]=EXCELFORE_CONFIG_UNI_BUSY_BITMAP;
	ydbia->dbpara.aps[5]=255u;
	res=ydbi_get_foot(ydbia, __func__, &rval, UBL_ERROR);
	if(res<0){return res;}
	if(res==8){ub_non_aligned_intsubst(rval, &busy, 8);}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	if(res!=8){return -1;}

	for(i=0;i<64;i++){
		if((busy & (uint64_t)(1<<i))==0){break;}
	}
	if(i==64){
		UB_LOG(UBL_ERROR, "%s:all busy\n", __func__);
		return -1;
	}
	busy|=(uint64_t)(1<<i);
	ydbia->dbpara.value=&busy;
	ydbia->dbpara.vsize=8;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	res=ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
	if(res!=0){return -1;}
	for(j=5;j>=0;j--){
		k=rmac[j]+i;
		if(k<=255){
			rmac[j]=k;
			UB_LOG(UBL_DEBUG, "%s:get %d from the start, "UB_PRIhexB6"\n",
			       __func__, i, UB_ARRAY_B6(rmac));
			return 0;
		}
		rmac[j]=rmac[j]+i-256;
		i=1;
	}
	UB_LOG(UBL_ERROR, "%s:outside range\n", __func__);
	return -1;
}

int ydbi_rel_mac_in_pool(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, ub_macaddr_t rmac)
{
	void *rval;
	uint64_t busy;
	int i,j,n,res;
	ub_macaddr_t tmac;
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex,
			  EXCELFORE_CONFIG_UNI_MAC_ADDRESS_POOL, YDBI_CONFIG);
	ydbia->dbpara.aps[4]=EXCELFORE_CONFIG_UNI_TOP_ADDRESS;
	ydbia->dbpara.aps[5]=255u;
	res=ydbi_get_foot(ydbia, __func__, &rval, UBL_ERROR);
	if(res<0){
		UB_LOG(UBL_ERROR, "%s:not initialized\n", __func__);
		return -1;
	}
	if(res==6){memcpy(tmac, rval, 6);}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	if(res!=6){return -1;}
	if(rmac[5]>=tmac[5]){
		n=rmac[5]-tmac[5];
		i=0;
	}else{
		n=rmac[5]+256-tmac[5];
		i=1;
	}
	for(j=4;j>=0;j--){
		if((i==1) && (rmac[j]==0) && (tmac[j]==0xFF)){continue;}
		if((rmac[j]-i)!=tmac[j]){return -1;}
		i=0;
	}
	UB_LOG(UBL_DEBUG, "%s:rel %d from the start,"UB_PRIhexB6"\n",
	       __func__, n, UB_ARRAY_B6(rmac));

	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_nbk1vk0(ydbia->dbald, &ydbia->dbpara, instIndex,
			  EXCELFORE_CONFIG_UNI_MAC_ADDRESS_POOL, YDBI_CONFIG);
	ydbia->dbpara.aps[4]=EXCELFORE_CONFIG_UNI_BUSY_BITMAP;
	ydbia->dbpara.aps[5]=255u;
	res=ydbi_get_foot(ydbia, __func__, &rval, UBL_ERROR);
	if(res<0){return res;}
	if(res==8){ub_non_aligned_intsubst(rval, &busy, 8);}
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	ydbi_rel_foot(ydbia, __func__);
	if(res!=8){return -1;}
	busy&=~(uint64_t)(1<<n);
	ydbia->dbpara.value=&busy;
	ydbia->dbpara.vsize=8;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}
