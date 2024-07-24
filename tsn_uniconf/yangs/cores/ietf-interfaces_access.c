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
#include "ietf-interfaces.h"

extern uint8_t IETF_INTERFACES_func(uc_dbald *dbald);
#define IETF_INTERFACES_RW IETF_INTERFACES_func(dbald)
#define IETF_INTERFACES_RO (IETF_INTERFACES_func(dbald)|0x80u)
#define IETF_INTERFACES_RO_Y (IETF_INTERFACES_func(ydbia->dbald)|0x80u)

static void set_dpara_k3vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IETF_INTERFACES_RO:IETF_INTERFACES_RW;
	dbpara->aps[1] = IETF_INTERFACES_INTERFACES;
	dbpara->aps[2] = IETF_INTERFACES_INTERFACE;
	dbpara->aps[3] = k1;
	dbpara->aps[4] = k2;
	dbpara->aps[5] = k3;
	dbpara->aps[6] = 255u;
	dbpara->kvs[0]=name;
	if(name){dbpara->kss[0]=strlen(name)+1u;}
	dbpara->kvs[1]=NULL;
}

static void set_dpara_k4vk1(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			    uint8_t k4, void *kv1, uint32_t kvs1, bool status,
			    uint8_t onhw)
{
	dbpara->onhw=onhw;
	dbpara->aps[0] = status?IETF_INTERFACES_RO:IETF_INTERFACES_RW;
	dbpara->aps[1] = IETF_INTERFACES_INTERFACES;
	dbpara->aps[2] = IETF_INTERFACES_INTERFACE;
	dbpara->aps[3] = IETF_INTERFACES_BRIDGE_PORT;
	dbpara->aps[4] = k1;
	dbpara->aps[5] = k2;
	dbpara->aps[6] = k3;
	dbpara->aps[7] = k4;
	dbpara->aps[8] = 255u;
	dbpara->kvs[0]=name;
	if(name){dbpara->kss[0]=strlen(name)+1u;}
	dbpara->kvs[1]=kv1;
	dbpara->kss[1]=kvs1;
	dbpara->kvs[2]=NULL;
}

// vk_sz indicates how many bytes used to encode the parameter `vk`.
// when the vk_sz is equal to 0, the value of `vk` shall not be used by this
// function.
static void set_dpara_knvk1(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    char *name, uint32_t vk, uint8_t vk_sz,
			    uint8_t kn[], uint8_t kn_sz, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0]=status?IETF_INTERFACES_RO:IETF_INTERFACES_RW;
	dbpara->aps[1]=IETF_INTERFACES_INTERFACES;
	dbpara->aps[2]=IETF_INTERFACES_INTERFACE;

	int i;
	for(i=0;i<kn_sz;i++){
		dbpara->aps[3+i]=kn[i];
	}
	dbpara->aps[3+i]=255u;

	dbpara->kvs[0]=name;
	dbpara->kss[0]=strlen(name)+1u;
	dbpara->kvs[1]=NULL;

	if (vk_sz > 0) {
		ADJUST_ENDIAN(&vk, sizeof(uint32_t)-vk_sz, vk_sz);
		dbpara->kvs[1]=&vk;
		dbpara->kss[1]=vk_sz;

		dbpara->kvs[2]=NULL;
	}
}

int ydbi_get_item_ifknvk0(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t kn[], uint8_t sz, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvk1(ydbia->dbald, &ydbia->dbpara, name, 0, 0, kn, sz, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_set_item_ifknvk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t kn[], uint8_t sz, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvk1(ydbia->dbald, &ydbia->dbpara, name, 0, 0, kn, sz, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_ifknvk0(yang_db_item_access_t *ydbia, char *name,
			  uint8_t kn[], uint8_t sz, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvk1(ydbia->dbald, &ydbia->dbpara, name, 0, 0, kn, sz, status);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_ifknvk1(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint32_t vk, uint8_t vk_sz,
			  uint8_t kn[], uint8_t sz, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvk1(ydbia->dbald, &ydbia->dbpara, name, vk, vk_sz, kn, sz, status);
	int err=ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
	return err;
}

int ydbi_set_item_ifknvk1(yang_db_item_access_t *ydbia,
			  char *name, uint32_t vk, uint8_t vk_sz,
			  uint8_t kn[], uint8_t sz, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvk1(ydbia->dbald, &ydbia->dbpara, name, vk, vk_sz, kn, sz, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	int err=ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
	return err;
}

int ydbi_del_item_ifknvk1(yang_db_item_access_t *ydbia,
			  char *name, uint32_t vk, uint8_t vk_sz,
			  uint8_t kn[], uint8_t sz, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_knvk1(ydbia->dbald, &ydbia->dbpara, name, vk, vk_sz, kn, sz, status);
	int err=ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
	return err;
}

int ydbi_get_item_ifk3vk0(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k3vk0(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_ifk3vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k3vk0(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_ifk3vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k3vk0(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_ifk3vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k3vk0(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3, status);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_ifk4vk1(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3,
			k4, kv1, kvs1, status, YANG_DB_ONHW_NOACTION);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_rel_item_ifk4vk1(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3,
			k4, kv1, kvs1, status, YANG_DB_ONHW_NOACTION);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_ifk4vk1(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status,
			  void *value, uint32_t vsize, uint8_t notice,
			  uint8_t onhw)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3,
			k4, kv1, kvs1, status, onhw);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_ifk4vk1(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, name, k1, k2, k3,
			k4, kv1, kvs1, status, YANG_DB_ONHW_NOACTION);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_ifk1vk0(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t k1, bool status)
{
	return ydbi_get_item_ifk3vk0(ydbia, rval, name, k1, 255, 255, status);
}

int ydbi_rel_item_ifk1vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, bool status)
{
	return ydbi_rel_item_ifk3vk0(ydbia, name, k1, 255, 255, status);
}

int ydbi_set_item_ifk1vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice)
{
	return ydbi_set_item_ifk3vk0(ydbia, name, k1, 255, 255, status,
				     value, vsize, notice);
}

int ydbi_del_item_ifk1vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, bool status)
{
	return ydbi_del_item_ifk3vk0(ydbia, name, k1, 255, 255, status);
}

int ydbi_get_ifupdown(yang_db_item_access_t *ydbia, const char *netdev)
{
	void *kvs[2]={(void*)netdev, NULL};
	uint8_t kss[1]={strlen(netdev)};
	uint8_t aps[5]={IETF_INTERFACES_RO_Y, IETF_INTERFACES_INTERFACES,
			IETF_INTERFACES_INTERFACE, IETF_INTERFACES_OPER_STATUS, 255};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
				      NULL, aps, kvs, kss, NULL, 0};
	uint32_t oper_status;
	if(yang_db_action(ydbia->dbald, NULL, &dbpara)){return -1;}
	oper_status=*((uint32_t*)dbpara.value);
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	(void)yang_db_action(ydbia->dbald, NULL, &dbpara);
	if(oper_status==1) return 1; // up
	if(oper_status==2) return 0; // down
	return -1;
}

int ydbi_set_ifupdown_ucnotice(yang_db_item_access_t *ydbia, const char *netdev,
			       UC_NOTICE_SIG_T **sem, const char *semname)
{
	void *kvs[3]={(void*)netdev, (char*)semname, NULL};
	uint8_t kss[2]={strlen(netdev)+1, 0};
	uint8_t aps[5]={IETF_INTERFACES_RO_Y, IETF_INTERFACES_INTERFACES,
			IETF_INTERFACES_INTERFACE, IETF_INTERFACES_OPER_STATUS, 255};
	kss[1]=strlen(semname)+1;
	return uc_nc_notice_register(ydbia->ucntd, ydbia->dbald, aps, kvs, kss,
				     UC_NOTICE_DBVAL_ADD, sem);
}

// this delete all of registeration with the same semname by ydbi_set_ucnotice
int ydbi_clear_ifupdown_ucnotice(yang_db_item_access_t *ydbia, const char *semname)
{
	return uc_nc_notice_deregister_all(ydbia->ucntd, ydbia->dbald, semname);
}

int ydbi_get_ifupdown_ucnotice(yang_db_item_access_t *ydbia, char *netdev,
			       uint8_t *ifupdown, const char *semname)
{
	uint32_t ksize, vsize=0;
	uint8_t key[UC_MAX_KEYSIZE];
	int res;
	void *value;

	res=uc_nc_get_notice_act(ydbia->ucntd, ydbia->dbald, semname, key, &ksize);
	if(res){
		UB_LOG(UBL_DEBUG, "%s:no data, res=%d\n", __func__, res);
		return -1;
	}
	// key=/ietf-interfaces/interfaces/interface/oper-status 255 SIZE string
	if((ksize<=6u) || (ksize<(6u+key[5]))){
		UB_LOG(UBL_ERROR, "%s:too small ksize=%d\n", __func__, ksize);
		return -1;
	}
	res=uc_dbal_get(ydbia->dbald, key, ksize, &value, &vsize);
	if(res || (vsize!=4) ){
		UB_LOG(UBL_ERROR, "%s:no value OR wrong vsize, res=%d, vsize=%d\n",
		       __func__, res, vsize);
		return -1;
	}
	if(*((uint32_t*)value)==1){
		*ifupdown=1;
	}else{
		*ifupdown=0;
	}
	if(key[5]<=CB_MAX_NETDEVNAME){
		memcpy(netdev, &key[6], key[5]);
	}else{
		res=-1;
	}
	uc_dbal_get_release(ydbia->dbald, key, ksize, value, vsize);
	return res;
}
