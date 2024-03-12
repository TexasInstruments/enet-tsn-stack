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
#include "ieee1588-ptp-tt.h"
#include "ieee1588-ptp-tt_access.h"

extern uint8_t IEEE1588_PTP_TT_func(uc_dbald *dbald);
#define IEEE1588_PTP_TT_RW IEEE1588_PTP_TT_func(dbald)
#define IEEE1588_PTP_TT_RW_Y IEEE1588_PTP_TT_func(ydbia->dbald)
#define IEEE1588_PTP_TT_RO (IEEE1588_PTP_TT_func(dbald)|0x80u)
#define IEEE1588_PTP_TT_RO_Y (IEEE1588_PTP_TT_func(ydbia->dbald)|0x80u)

static uint32_t ydbi_instIndex;
static void set_dpara_k4vk1(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint32_t instIndex,
			    uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			    void *kv1, uint32_t kvs1,
			    bool status)
{
	ydbi_instIndex=instIndex;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE1588_PTP_TT_RO:IEEE1588_PTP_TT_RW;
	dbpara->aps[1] = IEEE1588_PTP_TT_PTP;
	dbpara->aps[2] = IEEE1588_PTP_TT_INSTANCES;
	dbpara->aps[3] = IEEE1588_PTP_TT_INSTANCE;
	dbpara->aps[4] = k1;
	dbpara->aps[5] = k2;
	dbpara->aps[6] = k3;
	dbpara->aps[7] = k4;
	dbpara->aps[8] = 255u;
	dbpara->kvs[0]=&ydbi_instIndex;
	dbpara->kss[0]=sizeof(uint32_t);
	dbpara->kvs[1]=kv1;
	dbpara->kss[1]=kvs1;
	dbpara->kvs[2]=NULL;
}

int ydbi_get_item_ptk4vk1(yang_db_item_access_t *ydbia, void **rval, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex, k1, k2, k3, k4,
			kv1, kvs1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_ptk4vk1(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex, k1, k2, k3, k4,
			kv1, kvs1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_ptk4vk1(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1,
			  bool status, void *value, uint32_t vsize, uint8_t notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex, k1, k2, k3, k4,
			kv1, kvs1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

int ydbi_del_item_ptk4vk1(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1,
			  bool status)
{
	if(ydbi_del_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k4vk1(ydbia->dbald, &ydbia->dbpara, instIndex, k1, k2, k3, k4,
			kv1, kvs1, status);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_get_item_ptk3vk0(yang_db_item_access_t *ydbia, void **rval, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	return ydbi_get_item_ptk4vk1(ydbia, rval, instIndex,
				     k1, k2, k3, 255, NULL, 0, status);
}

int ydbi_rel_item_ptk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, bool status)
{
	return ydbi_rel_item_ptk4vk1(ydbia, instIndex,
				     k1, k2, k3, 255, NULL, 0, status);
}

int ydbi_set_item_ptk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3,
			  bool status, void *value, uint32_t vsize, uint8_t notice)
{
	return ydbi_set_item_ptk4vk1(ydbia, instIndex,
				     k1, k2, k3, 255, NULL, 0, status, value, vsize, notice);
}

int ydbi_del_item_ptk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3,
			  bool status)
{
	return ydbi_del_item_ptk4vk1(ydbia, instIndex,
				     k1, k2, k3, 255, NULL, 0, status);
}

/*
 * gptp has the map, but the other module doesn't have it.
 * To access ieee1588-ptp instance, get instIndex by calling this function
 */
int ydbi_gptpinstdomain2dbinst_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance, uint8_t domainIndex)
{
	uint8_t aps[]={IEEE1588_PTP_TT_RW_Y, IEEE1588_PTP_TT_PTP, IEEE1588_PTP_TT_INSTANCE_DOMAIN_MAP,
		       255};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
				      NULL, aps, NULL, NULL, NULL, 0};
	uint16_t *mapv;
	int i, n;
	int res=-1;
	if(yang_db_action(ydbia->dbald, NULL, &dbpara)!=0){return -1;}
	mapv=(uint16_t *)dbpara.value;
	n=dbpara.vsize/sizeof(uint16_t);
	for(i=0;i<n;i++){
		if((gptpInstance==(mapv[i]>>8u)) && ((mapv[i]&0xffu)==domainIndex)){
			res=i;
			break;
		}
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	(void)yang_db_action(ydbia->dbald, NULL, &dbpara);
	return res;
}

// reverse of the above
int ydbi_dbinst2gptpinstdomain_pt(yang_db_item_access_t *ydbia, uint16_t instIndex,
		      uint8_t *gptpInstance, uint8_t *domainIndex)
{
	uint8_t aps[]={IEEE1588_PTP_TT_RW_Y, IEEE1588_PTP_TT_PTP, IEEE1588_PTP_TT_INSTANCE_DOMAIN_MAP,
		       255};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
				      NULL, aps, NULL, NULL, NULL, 0};
	uint16_t *mapv;
	int n;
	if(yang_db_action(ydbia->dbald, NULL, &dbpara)!=0){return -1;}
	mapv=(uint16_t *)dbpara.value;
	n=dbpara.vsize/sizeof(uint16_t);
	if(instIndex>=n){return -1;}
	*gptpInstance=mapv[instIndex]>>8u;
	*domainIndex=mapv[instIndex]&0xff;
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	(void)yang_db_action(ydbia->dbald, NULL, &dbpara);
	return 0;
}

#define IEEE1588_DEVICES ieee1588_devices
UB_SD_GETMEM_DEF(IEEE1588_DEVICES, 8, XL4_DATA_ABS_MAX_NETDEVS*2);
int ydbi_portdevices_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			uint8_t domainIndex, ieee1588_netdev_t **portdevs)
{
	uint32_t ksize;
	int32_t instIndex;
	uint8_t key[32];
	uint8_t aps[8]={IEEE1588_PTP_TT_RW_Y, IEEE1588_PTP_TT_PTP, IEEE1588_PTP_TT_INSTANCES,
			IEEE1588_PTP_TT_INSTANCE, IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			IEEE1588_PTP_TT_UNDERLYING_INTERFACE, 255};
	void *kvs[2]={&instIndex, NULL};
	uint8_t kss[1]={sizeof(uint32_t)};
	void *value;
	uint32_t vsize;
	int i,ki=0;
	uc_range *range;

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0){
		UB_LOG(UBL_ERROR, "%s:no instance for ginst=%d, domain=%d\n",
		       __func__, gptpInstance, domainIndex);
		return -1;
	}
	ksize=yang_db_create_key(NULL, aps, kvs, kss, key);
	key[ksize]=255;
	range=uc_get_range(ydbia->dbald, key, ksize, key, ksize+1);
	if(!range){return 0;}
	for(i=0;i<XL4_DATA_ABS_MAX_NETDEVS;i++){
		if(uc_move_in_range(ydbia->dbald, range, UC_DBAL_FORWARD)){
			ki=i;
			break;
		}
	}
	uc_move_top_in_range(ydbia->dbald, range);
	*portdevs=(ieee1588_netdev_t*)UB_SD_GETMEM(IEEE1588_DEVICES,
						   ki*(sizeof(ieee1588_netdev_t*)));
	for(i=0;i<ki;i++){
		if(uc_get_value_in_range(ydbia->dbald, range, &value,
				       &vsize, UC_DBAL_FORWARD)){break;}
		(*portdevs)[i]=(ieee1588_netdev_t)UB_SD_GETMEM(IEEE1588_DEVICES, vsize);
		memcpy((void*)(*portdevs)[i], value, vsize);
	}
	uc_get_range_release(ydbia->dbald, range);
	return ki;
}

void ydbi_portdevices_pt_release(ieee1588_netdev_t **portdevs, int devnum)
{
	int i;
	if(portdevs==NULL || *portdevs==NULL) return;
	for(i=0;i<devnum;i++){
		if((*portdevs)[i]){
			UB_SD_RELMEM(IEEE1588_DEVICES, (void*)(*portdevs)[i]);
		}
	}
	UB_SD_RELMEM(IEEE1588_DEVICES, *portdevs);
	*portdevs=NULL;
}

int ydbi_port_index2device_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			      uint8_t domainIndex, uint16_t portIndex, char *netdev)
{
	int32_t vsize;
	void *value;
	int instance;
	int res;
	instance=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instance<0){
		UB_LOG(UBL_ERROR, "%s:no instance for ginst=%d, domain=%d\n",
		       __func__, gptpInstance, domainIndex);
		return -1;
	}
	vsize=YDBI_GET_ITEM(ptk4vk1, value, instance, IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			    IEEE1588_PTP_TT_UNDERLYING_INTERFACE, 255, &portIndex, sizeof(uint16_t),
			    YDBI_CONFIG);
	if(vsize<0){
		UB_LOG(UBL_ERROR, "%s:no device, portIndex=%d\n", __func__, portIndex);
		return -1;
	}
	if(vsize>CB_MAX_NETDEVNAME){
		UB_LOG(UBL_ERROR, "%s:too long size\n", __func__);
		res=-1;
	}else{
		memcpy(netdev, value, vsize);
		res=0;
	}
	ydbi_get_item_release(ydbia, false);
	return res;
}

int ydbi_port_device2index_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			      uint8_t domainIndex, const char *netdev, uint16_t *portIndex)
{
	int32_t instIndex;
	uint8_t key[32];
	uint8_t aps[8]={IEEE1588_PTP_TT_RW_Y, IEEE1588_PTP_TT_PTP, IEEE1588_PTP_TT_INSTANCES,
			IEEE1588_PTP_TT_INSTANCE, IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			IEEE1588_PTP_TT_UNDERLYING_INTERFACE, 255};
	void *kvs[2]={&instIndex, NULL};
	uint8_t kss[1]={sizeof(uint32_t)};
	void *kdata;
	void *value;
	uint32_t vsize=0, ksize;
	int i, res;
	uc_range *range;
	uint32_t ndlen=strlen(netdev)+1;

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0){
		UB_LOG(UBL_ERROR, "%s:no instance for ginst=%d, domain=%d\n",
		       __func__, gptpInstance, domainIndex);
		return -1;
	}
	ksize=yang_db_create_key(NULL, aps, kvs, kss, key);
	key[ksize]=255;
	range=uc_get_range(ydbia->dbald, key, ksize, key, ksize+1);
	res=-1;
	for(i=0;i<XL4_DATA_ABS_MAX_NETDEVS;i++){
		if(uc_get_keyvalue_in_range(ydbia->dbald, range, &kdata, &ksize,
					    &value, &vsize, UC_DBAL_FORWARD)){break;}
		if(vsize!=ndlen){continue;}
		if(memcmp(netdev, value, vsize)!=0){continue;}
		memcpy(portIndex, &((char*)kdata)[ksize-2], 2);
		res=0;
		break;
	}
	uc_get_range_release(ydbia->dbald, range);
	return res;
}

int ydbi_port_index2ptpdev_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			      uint8_t domainIndex, uint16_t portIndex, char *ptpdev)
{
	int32_t vsize;
	void *value;
	int instance;
	int res;
	instance=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instance<0){
		UB_LOG(UBL_ERROR, "%s:no instance for ginst=%d, domain=%d\n",
		       __func__, gptpInstance, domainIndex);
		return -1;
	}
	vsize=YDBI_GET_ITEM(ptk4vk1, value, instance, IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			    IEEE1588_PTP_TT_PORT_CLOCK, IEEE1588_PTP_TT_PTPDEV_NAME, &portIndex, sizeof(uint16_t),
			    YDBI_CONFIG);
	if(vsize<0){
		UB_LOG(UBL_DEBUGV, "%s:no ptpdev, portIndex=%d\n", __func__, portIndex);
		return -1;
	}
	if(vsize>XL4_DATA_PATH_STR_SHORTLEN){
		UB_LOG(UBL_ERROR, "%s:too long size\n", __func__);
		res=-1;
	}else{
		memcpy(ptpdev, value, vsize);
		res=0;
	}
	ydbi_get_item_release(ydbia, false);
	return res;
}

// portIndex=0 is always internal logical port
bool ydbi_get_asCapable(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			uint8_t domainIndex, uint16_t portIndex)
{
	uint8_t bas;
	int32_t instIndex;
	uint8_t aps[9]={IEEE1588_PTP_TT_RO_Y, IEEE1588_PTP_TT_PTP, IEEE1588_PTP_TT_INSTANCES,
			IEEE1588_PTP_TT_INSTANCE, IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			IEEE1588_PTP_TT_PORT_DS, IEEE1588_PTP_TT_AS_CAPABLE, 255};
	void *kvs[3]={&instIndex, &portIndex, NULL};
	uint8_t kss[2]={sizeof(uint32_t), sizeof(uint16_t)};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
				      NULL, aps, kvs, kss, NULL, 0};
	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0) return false;
	if(yang_db_action(ydbia->dbald, NULL, &dbpara)){return false;}
	bas=*((uint8_t*)dbpara.value);
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	(void)yang_db_action(ydbia->dbald, NULL, &dbpara);
	return bas!=0;
}

int ydbi_set_asCapable_ucnotice(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
				uint8_t domainIndex, uint16_t portIndex,
				UC_NOTICE_SIG_T **sem, const char *semname)
{
	int32_t instIndex;
	void *kvs[4]={&instIndex, &portIndex, (char*)semname, NULL};
	uint8_t kss[3]={sizeof(uint32_t), sizeof(uint16_t), 0};
	uint8_t aps[9]={IEEE1588_PTP_TT_RO_Y, IEEE1588_PTP_TT_PTP, IEEE1588_PTP_TT_INSTANCES,
			IEEE1588_PTP_TT_INSTANCE, IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			IEEE1588_PTP_TT_PORT_DS, IEEE1588_PTP_TT_AS_CAPABLE, 255};
	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0){return -1;}
	kss[2]=strlen(semname)+1;
	return uc_nc_notice_register(ydbia->ucntd, ydbia->dbald, aps, kvs, kss,
				     UC_NOTICE_DBVAL_ADD, sem);
}

// this delete all of registeration with the same semname by ydbi_set_ucnotice
int ydbi_clear_asCapable_ucnotice(yang_db_item_access_t *ydbia, const char *semname)
{
	return uc_nc_notice_deregister_all(ydbia->ucntd, ydbia->dbald, semname);
}

int ydbi_get_asCapable_ucnotice(yang_db_item_access_t *ydbia, uint8_t *gptpInstance,
				uint8_t *domainIndex, uint8_t *portIndex,
				bool *asCapable, const char *semname)
{
	uint32_t ksize, esize, vsize=0;
	uint8_t key[UC_MAX_KEYSIZE];
	uint16_t pindex;
	int res;
	void *value;
	uint32_t instIndex;

	res=uc_nc_get_notice_act(ydbia->ucntd, ydbia->dbald, semname, key, &ksize);
	if(res){
		UB_LOG(UBL_DEBUG, "%s:no data, res=%d\n", __func__, res);
		return -1;
	}
	esize=9+sizeof(uint32_t)+1+sizeof(uint16_t)+1; // this calculation is by our key data spec
	if(ksize!=esize){
		UB_LOG(UBL_ERROR, "%s:ksize=%d != %d\n", __func__, ksize, esize);
		return -1;
	}
	res=uc_dbal_get(ydbia->dbald, key, ksize, &value, &vsize);
	if(res || (vsize!=1) ){
		UB_LOG(UBL_ERROR, "%s:no value OR wrong vsize, res=%d, vsize=%d\n",
		       __func__, res, vsize);
		return -1;
	}
	*asCapable=*((uint8_t*)value)!=0;
	pindex=*((uint16_t*)&key[esize-sizeof(uint16_t)]);
	instIndex=*((uint32_t*)&key[esize-(sizeof(uint16_t)+1+sizeof(uint32_t))]);
	*portIndex=(uint8_t)pindex;
	res=ydbi_dbinst2gptpinstdomain_pt(ydbia, instIndex, gptpInstance, domainIndex);
	uc_dbal_get_release(ydbia->dbald, key, ksize, value, vsize);
	return res;
}

int ydbi_set_perfmon_clock_item(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
                                uint8_t domainIndex, uint16_t index, uint8_t confitem,
                                void *value, uint32_t vsize)
{
	int32_t instIndex;

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0){return -1;}
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}

	ydbia->dbpara.onhw=YANG_DB_ONHW_NOACTION;
	ydbia->dbpara.aps[0] = IEEE1588_PTP_TT_RO_Y;
	ydbia->dbpara.aps[1] = IEEE1588_PTP_TT_PTP;
	ydbia->dbpara.aps[2] = IEEE1588_PTP_TT_INSTANCES;
	ydbia->dbpara.aps[3] = IEEE1588_PTP_TT_INSTANCE;
	ydbia->dbpara.aps[4] = IEEE1588_PTP_TT_PERFORMANCE_MONITORING_DS;
	ydbia->dbpara.aps[5] = IEEE1588_PTP_TT_RECORD_LIST;
	ydbia->dbpara.aps[6] = confitem;
	ydbia->dbpara.aps[7] = 255u;
	ydbia->dbpara.kvs[0]=&instIndex;
	ydbia->dbpara.kss[0]=sizeof(uint32_t);
	ydbia->dbpara.kvs[1]=&index;
	ydbia->dbpara.kss[1]=sizeof(uint16_t);
	ydbia->dbpara.kvs[2]=NULL;

	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

int ydbi_set_perfmon_port_item(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
                               uint8_t domainIndex,	uint16_t portIndex, uint8_t list,
                               uint16_t index, uint8_t confitem,
                               void *value, uint32_t vsize)
{
	int32_t instIndex;

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0){return -1;}
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}

	ydbia->dbpara.onhw=YANG_DB_ONHW_NOACTION;
	ydbia->dbpara.aps[0] = IEEE1588_PTP_TT_RO_Y;
	ydbia->dbpara.aps[1] = IEEE1588_PTP_TT_PTP;
	ydbia->dbpara.aps[2] = IEEE1588_PTP_TT_INSTANCES;
	ydbia->dbpara.aps[3] = IEEE1588_PTP_TT_INSTANCE;
	ydbia->dbpara.aps[4] = IEEE1588_PTP_TT_PORTS;
	ydbia->dbpara.aps[5] = IEEE1588_PTP_TT_PORT;
	ydbia->dbpara.aps[6] = IEEE1588_PTP_TT_PERFORMANCE_MONITORING_PORT_DS;
	ydbia->dbpara.aps[7] = list;
	ydbia->dbpara.aps[8] = confitem;
	ydbia->dbpara.aps[9] = 255u;
	ydbia->dbpara.kvs[0]=&instIndex;
	ydbia->dbpara.kss[0]=sizeof(uint32_t);
	ydbia->dbpara.kvs[1]=&portIndex;
	ydbia->dbpara.kss[1]=sizeof(uint16_t);
	ydbia->dbpara.kvs[2]=&index;
	ydbia->dbpara.kss[2]=sizeof(uint16_t);
	ydbia->dbpara.kvs[3]=NULL;

	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_NO_NOTICE);
}

static keymod_rcode_t _keymod_list_del_all(uint8_t *ap, kvs_t *kvs, uint8_t *kss)
{
	return KEYMOD_NOMOVE_DELETE;
}

int ydbi_clear_perfmon_clock_ds(yang_db_item_access_t *ydbia,
                                uint8_t gptpInstance, uint8_t domainIndex)
{
	int res;
	int32_t instIndex=0;
	uint8_t aps[UC_MAX_AP_DEPTH]={IEEE1588_PTP_TT_RO_Y, IEEE1588_PTP_TT_PTP,
		IEEE1588_PTP_TT_INSTANCES, IEEE1588_PTP_TT_INSTANCE,
		IEEE1588_PTP_TT_PERFORMANCE_MONITORING_DS,
		IEEE1588_PTP_TT_RECORD_LIST, 255};
	void *kvs[2]={&instIndex, NULL};
	uint8_t kss[1]={sizeof(uint32_t)};

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0) return false;
	res=yang_db_listmove(ydbia->dbald, aps, kvs, kss, LISTMOVE_FORWARD, _keymod_list_del_all);
	return res;
}

int ydbi_clear_perfmon_port_ds(yang_db_item_access_t *ydbia, uint8_t list,
                               uint8_t gptpInstance, uint8_t domainIndex, uint16_t portIndex)
{
	int res;
	int32_t instIndex=0;
	uint8_t aps[UC_MAX_AP_DEPTH]={IEEE1588_PTP_TT_RO_Y, IEEE1588_PTP_TT_PTP,
		IEEE1588_PTP_TT_INSTANCES, IEEE1588_PTP_TT_INSTANCE,
		IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
		IEEE1588_PTP_TT_PERFORMANCE_MONITORING_PORT_DS, list,
		255};
	void *kvs[3]={&instIndex, &portIndex, NULL};
	uint8_t kss[2]={sizeof(uint32_t), sizeof(uint16_t)};

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0) return false;
	res=yang_db_listmove(ydbia->dbald, aps, kvs, kss, LISTMOVE_FORWARD, _keymod_list_del_all);
	return res;
}

/**
 * Moves performance-monitoring records with index=0..95->1..96.
 * Note: When an outstanding records exists on index=96 and there is no index=95
 * the old records remains. But this case should never happen for historical records
 * since records must be cascaded without skipping indices.
 */
static keymod_rcode_t _keymod_list_clock_move(uint8_t *ap, kvs_t *kvs, uint8_t *kss)
{
	if((ap[4]!=IEEE1588_PTP_TT_PERFORMANCE_MONITORING_DS)||
		(ap[5]!=IEEE1588_PTP_TT_RECORD_LIST)){return KEYMOD_ERROR;}
	if(kvs[0]==NULL){return KEYMOD_ERROR;}

	if(kss[1]!=sizeof(uint16_t)){return KEYMOD_ERROR;}
	*((uint16_t*)kvs[1])+=1;
	if(*((uint16_t*)kvs[1])>=97){return KEYMOD_NOMOVE;}
	return KEYMOD_MOVE_OVERWRITE; // can overwrite
}

int ydbi_cascade_perfmon_clock_ds(yang_db_item_access_t *ydbia,
                                  uint8_t gptpInstance, uint8_t domainIndex)
{
	int res;
	int32_t instIndex=0;
	uint8_t aps[UC_MAX_AP_DEPTH]={IEEE1588_PTP_TT_RO_Y, IEEE1588_PTP_TT_PTP,
		IEEE1588_PTP_TT_INSTANCES, IEEE1588_PTP_TT_INSTANCE,
		IEEE1588_PTP_TT_PERFORMANCE_MONITORING_DS,
		IEEE1588_PTP_TT_RECORD_LIST, 255, 255};
	void *kvs[3]={&instIndex, NULL, NULL};
	uint8_t kss[2]={sizeof(uint32_t), sizeof(uint16_t)};

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0) return false;

	res=yang_db_listmove(ydbia->dbald, aps, kvs, kss, LISTMOVE_BACKWARD, _keymod_list_clock_move);
	return res;
}

static keymod_rcode_t _keymod_list_port_move(uint8_t *ap, kvs_t *kvs, uint8_t *kss)
{
	if((ap[4]!=IEEE1588_PTP_TT_PORTS)||(ap[6]!=IEEE1588_PTP_TT_PERFORMANCE_MONITORING_PORT_DS)||
		((ap[7]!=IEEE1588_PTP_TT_RECORD_LIST)&&(ap[7]!=IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY)))
	{
		return KEYMOD_ERROR;
	}
	if(kvs[0]==NULL){return KEYMOD_ERROR;}
	if(kvs[1]==NULL){return KEYMOD_ERROR;}

	if(kss[2]!=sizeof(uint16_t)){return KEYMOD_ERROR;}
	*((uint16_t*)kvs[2])+=1;
	if(*((uint16_t*)kvs[2])>=97){return KEYMOD_NOMOVE;}
	return KEYMOD_MOVE_OVERWRITE; // can overwrite
}

int ydbi_cascade_perfmon_port_ds(yang_db_item_access_t *ydbia, uint8_t list,
                                 uint8_t gptpInstance, uint8_t domainIndex, uint16_t portIndex)
{
	int res;
	int32_t instIndex=0;
	uint8_t aps[UC_MAX_AP_DEPTH]={IEEE1588_PTP_TT_RO_Y, IEEE1588_PTP_TT_PTP,
		IEEE1588_PTP_TT_INSTANCES, IEEE1588_PTP_TT_INSTANCE,
		IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
		IEEE1588_PTP_TT_PERFORMANCE_MONITORING_PORT_DS, list, 255, 255};

	void *kvs[4]={&instIndex, &portIndex, NULL, NULL};
	uint8_t kss[3]={sizeof(uint32_t), sizeof(uint16_t), sizeof(uint16_t)};

	instIndex=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstance, domainIndex);
	if(instIndex<0) return false;

	res=yang_db_listmove(ydbia->dbald, aps, kvs, kss, LISTMOVE_BACKWARD, _keymod_list_port_move);
	return res;
}
