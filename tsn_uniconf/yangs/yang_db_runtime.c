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
#include "yang_modules.h"
#include "yang_db_access.h"
#include "yang_db_runtime.h"
#include "ieee1588-ptp-tt.h"
#include <tsn_combase/cb_tmevent.h>
#include "yang_node.h"

extern uint8_t IEEE1588_PTP_TT_func(uc_dbald *dbald);
#define IEEE1588_PTP_TT_RW IEEE1588_PTP_TT_func(ydrd->dbald)
#define IEEE1588_PTP_TT_RO (IEEE1588_PTP_TT_func(ydrd->dbald)|0x80)


UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

struct yang_db_runtime_data{
	uc_dbald *dbald;
	uc_hwald *hwald;
	uint8_t apsd[UC_MAX_AP_DEPTH+5];
	uint8_t *aps;
	uint8_t api;
	void *kvs[UC_MAX_KV_DEPTH+1];
	uint8_t kss[UC_MAX_KV_DEPTH+1];
	uint8_t kpi[UC_MAX_AP_DEPTH];
	bool changtoRO;
};

#define KEYV_DELIMITER '|'

static int total_kpi(yang_db_runtime_dataq_t *ydrd)
{
	unsigned int i;
	int res=0;
	// the last node:ydrd->api must be 'leaf' and has no keys
	for(i=0;i<ydrd->api;i++){res+=(int)ydrd->kpi[i];}
	return res;
}

// ieee1588 instance is converted from the domainmap
static int get_1588ptp_instance(yang_db_runtime_dataq_t *ydrd, char *kp)
{
	uint32_t g,d;
	char *astr;
	uint8_t aps[]={IEEE1588_PTP_TT_RW, IEEE1588_PTP_TT_PTP,
		       IEEE1588_PTP_TT_INSTANCE_DOMAIN_MAP,
		       255};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ,
				      YANG_DB_ONHW_NOACTION,
				      NULL, aps, NULL, NULL, NULL, 0};
	uint16_t *mapv;
	int i, n, x=-1;

	g=strtol(kp, NULL, 0);
	astr=strchr(kp, ',');
	if(!astr){
		UB_LOG(UBL_DEBUG, "%s:%d -> %d\n", __func__, g, g);
		return 0;
	}

	d=strtol(&astr[1], NULL, 0);
	if(yang_db_action(ydrd->dbald, NULL, &dbpara)!=0){goto erexit;}
	mapv=(uint16_t *)dbpara.value;
	n=dbpara.vsize/sizeof(uint16_t);
	for(i=0;i<n;i++){
		if((g==(mapv[i]>>8u)) && (d==(mapv[i]&0xffu))){
			x=i;
			break;
		}
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	(void)yang_db_action(ydrd->dbald, NULL, &dbpara);
	if(x>=0 && x<100){
		UB_LOG(UBL_DEBUG, "%s:%d,%d -> %d\n", __func__, g, d, x);
		sprintf(kp,"%d",x);
		return 0;
	}
erexit:
	UB_LOG(UBL_ERROR, "%s:invalid key value string:%s\n", __func__, kp);
	return -1;
}

// retrun 0:get all key values, 1:no key values, -1:error
// if vstr==NULL, use kv="keyname:keyvalue" format
// if vstr!=NULL, use kv="keyname", vstr="keyvalue" format
static int proc_get_keyv(yang_db_runtime_dataq_t *ydrd, char *kv, char *vstr, bool noerrmsg)
{
	uint8_t rv[2], kvtype;
	// kv fromt: key_name:key_value
	char *kn=kv; // key name
	char *kp=kv;
	void *value;
	uint32_t vsize;
	int res, tkpi;
	UB_LOG(UBL_DEBUG, "%s:kv=%s\n", __func__, kv);

	if(ydrd->api==0u){return -1;}
	if(vstr!=NULL){
		kp=vstr;
	}else{
		while((*kp!=0) && (*kp!=':')){kp++;}
		if(*kp==0){
			// 'key value' doesn't exist, no string after ':'
			// go ahead, and return NULL as the value key data
		}else{
			*kp++=0; // kp is key value
		}
	}

	if(kn[0]>='0' && kn[0]<='9'){
		rv[0]=0; // "valuekey" enum should be 0
	}else{
		// "valuekey" enum is alwyas 0, so this part can be replaced with "rv[0]=0"
		// get "valuekey" enum in rv[0]
		rv[0]=ydrd->aps[0];
		if((rv[0]==(uint8_t)XL4_EXTMOD_RW) || (rv[0]==(uint8_t)XL4_EXTMOD_RO)){
			if(ydrd->api<2u){return -1;}
			rv[1]=ydrd->aps[1];
		}
		if(yang_node_get_node_enums(ydrd->dbald, "valuekey", rv, 2)!=1){return -1;}
	}
	ydrd->aps[ydrd->api]=rv[0];
	if(kn[0]>='0' && kn[0]<='9'){
		res=strtol(kn, NULL, 0);
		if(res<0 || res>0xff){return -1;}
		rv[0]=res;
	}else{
		rv[0]=ydrd->aps[0];
		if((rv[0]==(uint8_t)XL4_EXTMOD_RW) || (rv[0]==(uint8_t)XL4_EXTMOD_RO)){
			if(ydrd->api<2u){return -1;}
			rv[1]=ydrd->aps[1];
		}
		if(yang_node_get_node_enums(ydrd->dbald, kn, rv, 2)!=1u){
			UB_LOG(UBL_ERROR, "%s:unknown key name:%s, rv=%d,%d\n",
			       __func__, kn, rv[0], rv[1]);
			return -1;
		}
	}
	ydrd->aps[ydrd->api+1u]=rv[0];
	if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+4u, &value, &vsize)!=0){
		ydrd->aps[0]+=XL4_DATA_RO; // 2nd try with '_RO'
		if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+4u, &value, &vsize)!=0){
			char *rstr=NULL;
			if(!noerrmsg){
				ub_hexdump(true, true, ydrd->apsd, ydrd->api+4u, 0);
				if(ub_assert_fatal((ydrd->api+4u) < (sizeof(ydrd->apsd)/sizeof(uint8_t)), __func__, "invalid ap index")){return -1;}
				ydrd->apsd[ydrd->api+4u]=255;
				(void)yang_node_get_node_string(ydrd->dbald, &rstr, &ydrd->apsd[2]);
				if(rstr!=NULL) {
					UB_LOG(UBL_ERROR, "%s:%s\n", __func__, rstr);
					UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr);
				}
				UB_LOG(UBL_ERROR, "%s:unknown key value type:%s\n", __func__, kn);
			}
			ydrd->aps[0]-=XL4_DATA_RO; // back to the '_RW'
			return -1;
		}
		ydrd->aps[0]-=XL4_DATA_RO; // back to the '_RW'
	}
	kvtype=*(uint8_t*)value;
	(void)uc_dbal_get_release(ydrd->dbald, ydrd->apsd, ydrd->api+4u, value, vsize);
	vsize=0;
	tkpi=total_kpi(ydrd);
	if(*kp==0){
		if(ydrd->kvs[tkpi]!=NULL){
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, ydrd->kvs[tkpi]);
			ydrd->kvs[tkpi]=NULL;
		}
		// set 1 to indicate "ydrd->kvs[tkpi]=NULL, but kv should exit"
		// this kss=1 is used only for "no key value" case
		ydrd->kss[tkpi]=1;
		res=1;
	}else{
		if(((ydrd->apsd[2]==(uint8_t)IEEE1588_PTP_TT_RW) ||
		    (ydrd->apsd[2]==(uint8_t)IEEE1588_PTP_TT_RO)) && tkpi==0){
			if(get_1588ptp_instance(ydrd, kp)){return -1;}
		}
		res=yang_value_conv(kvtype, kp, &ydrd->kvs[tkpi], &vsize, kn);
		if(res<0){
			UB_LOG(UBL_ERROR, "%s:can't convert key value:%s\n", __func__, kp);
			return -1;
		}
		ydrd->kss[tkpi]=res;
		res=0;
	}
	if(ub_assert_fatal((ydrd->api-1u) < (sizeof(ydrd->kpi)/sizeof(uint8_t)), __func__, "invalid kpi index")){return -1;}
	ydrd->kpi[ydrd->api-1u]++; // key belongs to one upper node
	tkpi++;
	if(tkpi>=UC_MAX_KV_DEPTH){
		UB_LOG(UBL_ERROR, "%s:too many value-keys\n",
		       __func__);
		return -1;
	}
	if(ydrd->kvs[tkpi]!=NULL){
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, ydrd->kvs[tkpi]);
		ydrd->kvs[tkpi]=NULL;
		ydrd->kss[tkpi]=0;
	}
	return res;
}

// retrurn 0:get all key values, 1: some key values are not there, -1:error
static int proc_get_keyvals(yang_db_runtime_dataq_t *ydrd, char *kvstr)
{
	// kvstr fromt: kc0|kc1|
	int kvi=0;
	int kpi=0;
	int gkres;
	int res=0;
	UB_LOG(UBL_DEBUG, "%s:kvstr=%s, api=%d\n", __func__, kvstr, ydrd->api);
	while(kvstr[kpi]!=0){
		while(kvstr[kpi]!='|'){
			if(kvstr[kpi]==0){return -1;}
			kpi++;
		}
		kvstr[kpi]=0;
		kpi++;
		gkres=proc_get_keyv(ydrd, &kvstr[kvi], NULL, false);
		if(gkres<0){return -1;}
		if(gkres==1){res=1;}
		kvi=kpi;
	}
	return res;
}

static int proc_set_onenode(yang_db_runtime_dataq_t *ydrd, char *kv, bool nokv)
{
	int i, tkpi;
	uint8_t rv[2];
	if(kv[0]>='0' && kv[0]<='9'){
		// the string is a number
		if(ub_assert_fatal(ydrd->api < (sizeof(ydrd->kpi)/sizeof(uint8_t)), __func__, "invalid kpi index")){return -1;}
		ydrd->kpi[ydrd->api]=0;
		i=strtol(kv, NULL, 0);
		if(i<0 || i>0xff){return -1;}
		ydrd->aps[ydrd->api++]=i;
		return 0;
	}
	if(ydrd->api==0u){
		ydrd->aps[0]=yang_node_mod_get_enum(ydrd->dbald, kv);
		if(ydrd->aps[0]==0xffu){
			UB_LOG(UBL_ERROR, "%s:invalid first key name:%s\n",
			       __func__, kv);
			return -1;
		}
		ydrd->kpi[ydrd->api++]=0;
		return 0;
	}
	i=1;
	if(!strcmp(kv, "..")){
		if(ub_assert_fatal((ydrd->api-1u) < (sizeof(ydrd->kpi)/sizeof(uint8_t)), __func__, "invalid kpi index")){return -1;}
		ydrd->kpi[ydrd->api-1u]=0;
		if(nokv){ydrd->api--;}
		return 0;
	}
	rv[0]=ydrd->aps[0];
	if(ydrd->api>1u){
		rv[1]=ydrd->aps[1];
		i=2;
	}
	if(yang_node_get_node_enums(ydrd->dbald, kv, rv, i)!=1u){
		UB_LOG(UBL_ERROR, "%s:invalid key name:%s, api=%d\n",
		       __func__, kv, ydrd->api);
		return -1;
	}
	if(ub_assert_fatal(ydrd->api < (sizeof(ydrd->kpi)/sizeof(uint8_t)), __func__, "invalid kpi index")){return -1;}
	ydrd->kpi[ydrd->api]=0;
	tkpi=total_kpi(ydrd);
	if(ydrd->kvs[tkpi]!=NULL){
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, ydrd->kvs[tkpi]);
		ydrd->kvs[tkpi]=NULL;
		ydrd->kss[tkpi]=0;
	}
	ydrd->aps[ydrd->api++]=rv[0];
	return 0;
}

// return -1:error, 1:key ends with '/', 2:key ends with '+', 0:normal
//         3:key ends with '-'
static int proc_get_keys(yang_db_runtime_dataq_t *ydrd, char *kstr)
{
	// kstr formt: /a/b/c|kc0|kc1|/d|kd0|/e|ke0|
	bool nokv;
	int res=0;
	int kpi=0;
	int kvi=0;
	int gkres;
	UB_LOG(UBL_DEBUG, "%s:kstr=%s\n", __func__, kstr);
	if(kstr[kpi]=='/') {
		ydrd->api=0;
		kpi++;
	}

	res=0;
	while(kstr[kpi] && (ydrd->api<(uint8_t)UC_MAX_AP_DEPTH)){
		kvi=kpi;
		nokv=false;
		while((kstr[kpi]!=0) && (kstr[kpi]!='/') && (kstr[kpi]!=KEYV_DELIMITER)){kpi++;}
		if((kpi>kvi) && (kstr[kpi-1]=='+')){
			res=2;
			kstr[kpi-1]=0;
		}
		if((kpi>kvi) && (kstr[kpi-1]=='-')){
			res=3;
			kstr[kpi-1]=0;
		}
		if(kstr[kpi]==0){
			nokv=true;
		}else if(kstr[kpi]=='/'){
			kstr[kpi]=0;
			kpi++;
			if(kstr[kpi]==0){res=1;}// it ends with '/'
			nokv=true;
		}else{
			//*kp==KEYV_DELIMITER
			kstr[kpi]=0;
			kpi++;
		}
		if(proc_set_onenode(ydrd, &kstr[kvi], nokv)!=0){return -1;}
		if(nokv){continue;}

		kvi=kpi;
		kpi++;
		while(true){
			while((kstr[kpi]!=0) && (kstr[kpi]!=KEYV_DELIMITER)){kpi++;}
			if(kstr[kpi]==0){
				UB_LOG(UBL_ERROR, "%s:missing the key value end delimiter\n",
				       __func__);
				return -1;
			}
			kpi++;
			if(kstr[kpi]==0){break;}
			if(kstr[kpi]=='/'){
				kstr[kpi]=0;
				kpi++;
				if(kstr[kpi]==0){res=1;}// it ends with '/'
				break;
			}
		}

		gkres=proc_get_keyvals(ydrd, &kstr[kvi]);
		if(gkres<0){return -1;}
		if(gkres==1){res=1;}// no key values
	}
	return res;
}

static int copy_list(yang_db_runtime_dataq_t *ydrd, char *vstr)
{
	int i, res;
	void *okvs[UC_MAX_KV_DEPTH+1];
	int tkpi=total_kpi(ydrd);
	if(tkpi<1){return -1;}
	for(i=0;i<tkpi;i++){okvs[i]=ydrd->kvs[i];}
	okvs[tkpi]=NULL;
	ydrd->kvs[tkpi-1]=NULL;
	ydrd->kss[tkpi-1]=0;

	for(i=ydrd->api-1;i>=0;i--){
		if(ydrd->kpi[i]>0){
			ydrd->kpi[i]--;
			break;
		}
	}
	// vstr part is |*|, list copy case
	vstr[strlen(vstr)-1]=0;
	res=proc_get_keyv(ydrd, &vstr[1], NULL, false);
	if(res==0){
		ydrd->aps[ydrd->api]=255;
		//yang_db_keydump_log(UBL_DEBUG, ydrd->dbald, ydrd->aps, okvs, ydrd->kss);
		//yang_db_keydump_log(UBL_DEBUG, ydrd->dbald, ydrd->aps, ydrd->kvs, ydrd->kss);
		res=yang_db_listcopy(ydrd->dbald, ydrd->aps, okvs, ydrd->kss,
				     ydrd->kvs, ydrd->kss);
		ydrd->aps[0]+=XL4_DATA_RO;
		res=yang_db_listcopy(ydrd->dbald, ydrd->aps, okvs, ydrd->kss,
				     ydrd->kvs, ydrd->kss);
		ydrd->aps[0]-=XL4_DATA_RO;
	}
	// release the source side value key, and keep the new value key in 'ydrd->kvs'
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, okvs[tkpi-1]);
	return res;
}

static int get_value_type(yang_db_runtime_dataq_t *ydrd)
{
	uint8_t vtype;
	void *value;
	uint32_t vsize;
	if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+2u, &value, &vsize)!=0){
		ydrd->changtoRO=true;
		ydrd->aps[0]+=XL4_DATA_RO;
		if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+2u, &value, &vsize)!=0){
			char *rstr=NULL;
			ub_hexdump(true, true, ydrd->apsd, ydrd->api+2u, 0);
			if(ub_assert_fatal((ydrd->api+2u) < (sizeof(ydrd->apsd)/sizeof(uint8_t)), __func__, "invalid ap index")){return -1;}
			ydrd->apsd[ydrd->api+2u]=255;
			(void)yang_node_get_node_string(ydrd->dbald, &rstr,
							   &ydrd->apsd[2]);
			if(rstr!=NULL){
				UB_LOG(UBL_ERROR, "%s:%s\n", __func__, rstr);
				UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr);
			}
			UB_LOG(UBL_ERROR, "%s:unknown value type\n", __func__);
			ydrd->api--;
			ydrd->aps[0]-=XL4_DATA_RO;
			return -1;
		}
	}
	vtype=*((uint8_t*)value);
	(void)uc_dbal_get_release(ydrd->dbald, ydrd->apsd, ydrd->api+2u, value, vsize);
	return vtype;
}

static int proc_one_item(yang_db_runtime_dataq_t *ydrd, char *kstr, char *vstr, uint8_t onhw)
{
	int res;
	uint8_t vtype;
	uint32_t vsize;
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,onhw,
				      NULL,NULL,NULL,NULL,NULL,0};
	char *btkey;

	UB_LOG(UBL_DEBUGV, "%s:kstr=%s, vstr=%s\n", __func__, kstr, vstr ? vstr : "NULL");
	btkey=strrchr(kstr, '/');
	if(btkey!=NULL){
		btkey=&btkey[1];
	}else{
		btkey=kstr;
	}
	res=proc_get_keys(ydrd, kstr);
	ydrd->changtoRO=false;
	if(res<0){return -1;}
	if(res==1){return 0;} // this is key only line
	if(vstr && (vstr[0]==KEYV_DELIMITER) && (strlen(vstr)>1) &&
	   (vstr[strlen(vstr)-1]==KEYV_DELIMITER)){
		return copy_list(ydrd, vstr);
	}
	if(res==2){dbpara.atype=YANG_DB_ACTION_APPEND;} // key end with '+"
	if(res==3){
		// key end with '-"
		dbpara.atype=YANG_DB_ACTION_DELETE;
	}
	res=get_value_type(ydrd);
	if(res<0){return res;}
	vtype=(uint8_t)res;
	res=0;
	if(dbpara.atype!=YANG_DB_ACTION_DELETE){
		vsize=0;
		res=yang_value_conv(vtype, vstr, &dbpara.value, &vsize, btkey);
		if(res<0){
			UB_LOG(UBL_ERROR, "%s:invalid value:%s\n", __func__, vstr ? vstr : "NULL");
			ydrd->api--;
			if(ydrd->changtoRO){ydrd->aps[0]-=XL4_DATA_RO;}
			return -1;
		}
	}
	ydrd->aps[ydrd->api--]=255; // decrement api to set yard->aps without the last leaf
	dbpara.aps=ydrd->aps;
	dbpara.kvs=ydrd->kvs;
	dbpara.kss=ydrd->kss;
	dbpara.vsize=res;
	res=yang_db_action(ydrd->dbald, ydrd->hwald, &dbpara);
	if(res!=0){
		UB_LOG(UBL_ERROR, "%s:yang_db_action to create failed\n", __func__);
	}
	if(dbpara.value!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, dbpara.value);}
	if(ydrd->changtoRO){ydrd->aps[0]-=XL4_DATA_RO;}
	return res;
}

// return -1:error, 0:normal without vstr, 1:normal with vstr
static int proc_one_line(yang_db_runtime_dataq_t *ydrd, char *line)
{
	int lpi=0;
	int kpi=0;
	int vpi=0;
	char *emes="";
	int res;
	// line formt: "#comment" | "key_string value_string #comment"
	// skip trailing spaces, and ignore a comment line
	res=ub_find_nospace(&line[lpi], (int)strlen(&line[lpi]));
	if(res<0){return 0;} // ignore space only line
	lpi+=res;
	if(line[lpi]=='#'){return 0;} // ignore comment line
	kpi=lpi; // key part string
	res=ub_find_space(&line[lpi], (int)strlen(&line[lpi]));
	if(res<0) {
		lpi+=(int)strlen(&line[lpi]);
		if(line[lpi-1]=='/' || line[lpi-1]=='-'){
			return proc_one_item(ydrd, &line[kpi], &line[lpi],
					     YANG_DB_ONHW_ALWAYS);
		}
		emes="no value";
		goto erexit;
	}
	lpi+=res;
	line[lpi]=0;
	lpi++;
	res=ub_find_nospace(&line[lpi], (int)strlen(&line[lpi]));
	if(res<0) {
		if(line[lpi-1]=='/' || line[lpi-1]=='-'){
			return proc_one_item(ydrd, &line[kpi], &line[lpi],
							  YANG_DB_ONHW_ALWAYS);
		}
		emes="no value";
		goto erexit;
	}
	lpi+=res;
	vpi=lpi; // value part string
	res=ub_find_space(&line[lpi], (int)strlen(&line[lpi]));
	if(res>=0) {
		lpi+=res;
		line[lpi]=0;
		lpi++;
		res=ub_find_nospace(&line[lpi], (int)strlen(&line[lpi]));
		if((res>=0) && (line[lpi+res]!='#')){
			emes="3rd part is not comment";
			goto erexit;
		}
	}
	// remove double quote at the top and bottom
	if(((line[vpi]=='"') && (line[vpi+(int)strlen(&line[vpi])-1]=='"'))){
		line[vpi+(int)strlen(&line[vpi])-1]=0;
		vpi++;
	}
	res=proc_one_item(ydrd, &line[kpi], &line[vpi], YANG_DB_ONHW_ALWAYS);
	if(res==0){return 1;}
	return res;
erexit:
	UB_LOG(UBL_ERROR, "%s:invalid line format - %s:%s\n",
	       __func__, emes, line);
	return -1;
}

// this instance must be only 1
UB_SD_GETMEM_DEF(YANG_DB_RTINST, sizeof(yang_db_runtime_dataq_t), 1);
yang_db_runtime_dataq_t *yang_db_runtime_init(uc_dbald *dbald, uc_hwald *hwald)
{
	yang_db_runtime_dataq_t *ydrd;
	ydrd=(yang_db_runtime_dataq_t*)UB_SD_GETMEM(YANG_DB_RTINST, sizeof(yang_db_runtime_dataq_t));
	if(ub_assert_fatal(ydrd!=NULL, __func__, NULL)){return NULL;}
	(void)memset(ydrd, 0, sizeof(yang_db_runtime_dataq_t));
	ydrd->aps=&ydrd->apsd[2];
	ydrd->apsd[0]=XL4_DATA_RW;
	ydrd->apsd[1]=YANG_VALUE_TYPES;
	ydrd->dbald=dbald;
	ydrd->hwald=hwald;
	return ydrd;
}

void yang_db_runtime_close(yang_db_runtime_dataq_t *ydrd)
{
	int i;
	if(!ydrd){return;}
	for(i=0;i<UC_MAX_KV_DEPTH;i++){
		if(ydrd->kvs[i]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, ydrd->kvs[i]);}
	}
	UB_SD_RELMEM(YANG_DB_RTINST, ydrd);
}

int yang_db_runtime_put_oneline(yang_db_runtime_dataq_t *ydrd,
				char *kstr, char *vstr, uint8_t onhw)
{
	int res;
	res=proc_one_item(ydrd, kstr, vstr, onhw);
	if(res){return res;}
	if(ydrd->changtoRO){ydrd->aps[0]+=XL4_DATA_RO;}
	return 0;
}

int yang_db_runtime_askaction(yang_db_runtime_dataq_t *ydrd,
			      uc_notice_data_t *ucntd)
{
	return uc_nc_askaction_push(ucntd, ydrd->dbald, ydrd->aps,
				    ydrd->kvs, ydrd->kss);
}

uc_range *yang_db_runtime_range_fromline(yang_db_runtime_dataq_t *ydrd,
					 const char* line, void **kvs, uint8_t *kss,
					 bool status)
{
	char *lstr;
	uint8_t key2[UC_MAX_AP_DEPTH];
	int i;
	if((line==NULL) || (line[0]==0)){return NULL;}
	lstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, strlen(line)+1u);
	if(ub_assert_fatal(lstr!=NULL, __func__, NULL)){return NULL;}
	memcpy(lstr, line, strlen(line)+1u);
	proc_get_keys(ydrd, lstr);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, lstr);
	if(ydrd->api==0 || ydrd->api>UC_MAX_AP_DEPTH){return NULL;}
	memcpy(key2, ydrd->aps, ydrd->api);
	key2[ydrd->api-1]++;
	for(i=0;i<UC_MAX_KV_DEPTH+1;i++){
		if(i==UC_MAX_KV_DEPTH || ydrd->kss[i]==0){
			kss[i]=0;
			kvs[i]=NULL;
			break;
		}
		kvs[i]=ydrd->kvs[i];
		kss[i]=ydrd->kss[i];
	}
	if(status){
		ydrd->aps[0]+=XL4_DATA_RO;
		key2[0]+=XL4_DATA_RO;
	}
	return uc_get_range(ydrd->dbald, ydrd->aps, ydrd->api, key2, ydrd->api);
}

int yang_db_runtime_get_oneline(yang_db_runtime_dataq_t *ydrd,
				const char* line, void **value, uint32_t *vsize)
{
	char *lstr;
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ,YANG_DB_ONHW_NOACTION,
				      NULL,ydrd->aps,ydrd->kvs,ydrd->kss,NULL,0};
	int vtype;
	int res;
	if((line==NULL) || (line[0]==0)){return -1;}

	lstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, strlen(line)+1u);
	if(ub_assert_fatal(lstr!=NULL, __func__, NULL)){return -1;}
	memcpy(lstr, line, strlen(line)+1u);
	res=proc_get_keys(ydrd, lstr);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, lstr);
	if(res!=0 && res!=2){
		UB_LOG(UBL_ERROR, "%s:can't process line:%s\n", __func__, line);
		return -1;
	}
	vtype=get_value_type(ydrd);
	if(vtype<0){
		UB_LOG(UBL_ERROR, "%s:can't get vtype:%s\n", __func__, line);
		return -1;
	}
	ydrd->aps[ydrd->api]=255;
	res=yang_db_action(ydrd->dbald, ydrd->hwald, &dbpara);
	if(res!=0){
		UB_LOG(UBL_DEBUG, "%s:can't get data\n", __func__);
		yang_db_keydump_log(UBL_DEBUG, ydrd->dbald, ydrd->aps, ydrd->kvs, ydrd->kss);
		return -1;
	}
	*value=UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *value, dbpara.vsize);
	if(ub_assert_fatal(*value!=NULL, __func__, "realloc")){return -1;}
	memcpy(*value, dbpara.value, dbpara.vsize);
	*vsize=dbpara.vsize;
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	(void)yang_db_action(ydrd->dbald, ydrd->hwald, &dbpara);
	return vtype;
}

int yang_db_runtime_notice_register(yang_db_runtime_dataq_t *ydrd, uc_notice_data_t *ucntd,
				    const char* line, char *semname, UC_NOTICE_SIG_T **sem)
{
	char *lstr;
	int res, i, sp;
	void *vtv;
	uint32_t vts;
	if(!line || !semname){return -1;}
	lstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, strlen(line)+1u);
	if(ub_assert_fatal(lstr!=NULL, __func__, NULL)){return -1;}
	memcpy(lstr, line, strlen(line)+1u);
	res=proc_get_keys(ydrd, lstr);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, lstr);
	if(res!=0){
		UB_LOG(UBL_ERROR, "%s:can't process line:%s\n", __func__, line);
		return -1;
	}
	ydrd->aps[0]&=0x7f;
	if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+2u, &vtv, &vts)!=0){
		ydrd->aps[0]+=XL4_DATA_RO;
		if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+2u, &vtv, &vts)!=0){
			UB_LOG(UBL_DEBUG, "%s:can't get vtype\n", __func__);
			return -1;
		}
	}
	(void)uc_dbal_get_release(ydrd->dbald, ydrd->apsd, ydrd->api+2u, vtv, vts);

	ydrd->aps[ydrd->api]=255;
	sp=-1;
	for(i=0;i<UC_MAX_KV_DEPTH;i++){
		if(!ydrd->kvs[i]){
			ydrd->kvs[i]=semname;
			ydrd->kss[i]=strlen(semname)+1u;
			sp=i;
			break;
		}
	}
	if(sp==-1){
		UB_LOG(UBL_ERROR, "%s:can't set semname\n", __func__);
		return -1;
	}
	if(ydrd->kvs[sp+1]!=NULL){
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, ydrd->kvs[sp+1]);
		ydrd->kvs[sp+1]=NULL;
		ydrd->kss[sp+1]=0;
	}
	UB_LOG(UBL_DEBUG, "%s:set semname:%s\n", __func__, semname);
	res=uc_nc_notice_register(ucntd, ydrd->dbald, ydrd->aps, ydrd->kvs, ydrd->kss,
				  UC_NOTICE_DBVAL_ADD, sem);
	ydrd->kvs[sp]=NULL;
	ydrd->kss[sp]=0;
	return res;
}

static int remove_bslash(char *line)
{
	int i;
	int len=strlen(line);
	bool bs=false;
	int shift=0;
	for(i=0;i<len;i++){
		if(shift>0){
			line[i-shift]=line[i];
		}
		if(bs){
			if(line[i-shift]!='\n'){
				bs=false;
			}else{
				shift+=2;
				bs=false;
			}
		}else{
			if(line[i]=='\\'){
				bs=true;
			}
		}
	}
	return shift;
}

// read line by line, connect lines if the line end is '\'
#define LINE_BUF_SIZE 512u
int yang_db_runtime_readfile(yang_db_runtime_dataq_t *ydrd, const char* fname,
			     uc_notice_data_t *ucntd)
{
	void *inf;
	char *cp;
	char linebuf[LINE_BUF_SIZE+1u];
	int np;
	int nlen;
	int rsize;
	int psize=0;
	int res=0;

	inf=ub_fioopen(fname, "r");
	if(inf==NULL){
		UB_LOG(UBL_ERROR, "%s:can't open file=%s\n", __func__, fname);
		return -1;
	}
	while(true){
		rsize=ub_fioread(inf, &linebuf[psize], LINE_BUF_SIZE-1-psize);
		if(rsize<0){
			UB_LOG(UBL_ERROR, "%s:read error\n", __func__);
			res=-1;
			break;
		}
		if((psize==0) && (rsize==0)){break;}
		linebuf[psize+rsize]='\0';
		rsize-=remove_bslash(linebuf);
		cp=strchr(linebuf, '\n');
		if(cp!=NULL){
			*cp='\0';
		}else{
			if(psize+rsize==LINE_BUF_SIZE-1){
				UB_LOG(UBL_ERROR, "%s:too long line\n", __func__);
				res=-1;
				break;
			}
		}
		np=(int)strlen(linebuf)+1;
		nlen=psize+rsize-np;
		res=proc_one_line(ydrd, linebuf);
		if(res<0){break;}
		if(ucntd && (res==1)){
			if(ydrd->changtoRO){ydrd->aps[0]+=XL4_DATA_RO;}
			(void)yang_db_runtime_askaction(ydrd, ucntd);
		}
		res=0;

		if(nlen>0){
			memmove(linebuf, &linebuf[np], nlen);
			psize=nlen;
		}else{
			psize=0;
		}
	}
	(void)ub_fioclose(inf);
	return res;
}

int yang_db_runtime_get_vtype(uc_dbald *dbald, uint8_t *aps)
{
	int i;
	uint8_t vtype;
	uint8_t *apsd;
	void *value;
	uint32_t vsize;
	for(i=0;i<255;i++){
		if(aps[i]==255u){break;}
	}
	if((i<1)||(i==255)){return -1;}
	apsd=(uint8_t*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, 2+i);
	if(ub_assert_fatal(apsd!=NULL, __func__, NULL)){return -1;}
	(void)memset(apsd, 0, 2+i);
	apsd[0]=XL4_DATA_RW;
	apsd[1]=YANG_VALUE_TYPES;
	memcpy(&apsd[2], aps, i);
	if(uc_dbal_get(dbald, apsd, i+2, &value, &vsize)!=0){
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, apsd);
		return -1;
	}
	vtype=*((uint8_t*)value);
	(void)uc_dbal_get_release(dbald, apsd, i+2, value, vsize);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, apsd);
	return vtype;
}

static uc_range *get_vkrange(uc_dbald *dbald, uint8_t *aps, uint8_t **apsd)
{
	int i,j;
	uint32_t ksize;

	uc_range *range;
	for(i=0;i<255;i++){
		if(aps[i]==255u){break;}
	}
	if((i<1)||(i==255)){return NULL;}
	ksize=i+4;
	*apsd=(uint8_t*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, 2*ksize);
	if(ub_assert_fatal(*apsd!=NULL, __func__, NULL)){return NULL;}
	(void)memset(*apsd, 0, 2*ksize);
	for(j=0;j<2;j++){
		(*apsd)[j*ksize+0]=XL4_DATA_RW;
		(*apsd)[j*ksize+1]=YANG_VALUE_TYPES;
		memcpy(&((*apsd)[j*ksize+2]), aps, i);
		(*apsd)[j*ksize+i+2]=0; // *_VALUEKEY
		(*apsd)[j*ksize+i+3]=0+j*254; // 0 / 254
	}
	range=uc_get_range(dbald, *apsd, ksize, &(*apsd)[ksize], ksize);
	if(!range){UB_SD_RELMEM(YANGINIT_GEN_SMEM, *apsd);}
	return range;
}

uint8_t yang_db_runtime_getvknum(uc_dbald *dbald, uint8_t *aps)
{
	int ki;
	uint8_t *apsd=NULL;
	uc_range *range;
	range=get_vkrange(dbald, aps, &apsd);
	if(!range){return 0;}
	for(ki=0;ki<255;ki++){
		if(uc_move_in_range(dbald, range, UC_DBAL_FORWARD)!=0) break;
	}
	uc_get_range_release(dbald, range);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, apsd);
	return ki;
}

static uint8_t get_aps_len(uint8_t *aps)
{
	uint8_t kl;
	for(kl=0;kl<255u;kl++){
		if(aps[kl]==255u){break;}
	}
	if((kl<1u)||(kl==255u)){return 255;}
	return kl;
}

int yang_db_runtime_getvkvtype(uc_dbald *dbald, uint8_t *aps, uint8_t vkindex,
			       uint8_t *vkey, uint8_t *vtype)
{
	uint8_t kl, ki;
	int res=-1;
	uint8_t *apsd=NULL;
	uc_range *range;
	uint32_t size;
	void *value;
	kl=get_aps_len(aps);
	if(kl==255u){return 0;}
	range=get_vkrange(dbald, aps, &apsd);
	if(!range){return 0;}
	for(ki=0;ki<255;ki++){
		if(ki<vkindex){
			if(uc_move_in_range(dbald, range, UC_DBAL_FORWARD)!=0) break;
			continue;
		}
		if(uc_get_key_in_range(dbald, range, &value, &size,
				       UC_DBAL_NOMOVE)!=0) break;
		if(size!=kl+4u){
			UB_LOG(UBL_ERROR, "%s:wrong ksize=%d, kl+4=%d\n", __func__,
			       size, kl+4);
		}
		*vkey=((uint8_t*)value)[kl+3u];
		if(uc_get_value_in_range(dbald, range, &value, &size,
					 UC_DBAL_FORWARD)!=0) break;
		if(size!=1){
			UB_LOG(UBL_ERROR, "%s:wrong size=%d\n", __func__, size);
			break;
		}
		*vtype=*(uint8_t*)value;
		res=0;
		break;
	}
	uc_get_range_release(dbald, range);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, apsd);
	return res;
}

int yang_db_runtime_getvkstr(uc_dbald *dbald,
			     uint8_t *aps, uint8_t vkindex, char **rstr)
{
	uint8_t kl, ki, i;
	int res=-1;
	uint8_t *apsd=NULL;
	uc_range *range;
	uint32_t size;
	uint8_t *v;
	void *value;
	kl=get_aps_len(aps);
	if(kl==255u){return -1;}
	range=get_vkrange(dbald, aps, &apsd);
	if(!range){return -1;}
	for(ki=0;ki<255;ki++){
		if(ki<vkindex){
			if(uc_move_in_range(dbald, range, UC_DBAL_FORWARD)!=0) break;
			continue;
		}
		if(uc_get_key_in_range(dbald, range, &value, &size,
				       UC_DBAL_FORWARD)!=0) break;
		if(size!=kl+4u){
			UB_LOG(UBL_ERROR, "%s:wrong ksize=%d, kl+4=%d\n", __func__,
			       size, kl+4);
		}
		v=(uint8_t*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, size);
		for(i=0; i<size; i++) {
			if (i==(kl+2u)) {
				v[i]=((uint8_t*)value)[i+1];
			}else if (i==(kl+3u)) {
				v[i]=255;
			}else {
				v[i]=((uint8_t*)value)[i];
			}
		}
		res=yang_node_get_node_string(dbald, rstr, &(v[2u]));
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, v);
		break;
	}
	uc_get_range_release(dbald, range);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, apsd);
	return res;
}

int yang_db_runtime_waititem(yang_db_runtime_dataq_t *ydrd, const char* witem,
			     void *waitv, uint32_t wvsize, int tout_ms)
{
	void *value=NULL;
	uint32_t vsize=0;
	int res=-1;
	if(!ydrd || !witem){return -1;}
	while(true){
		res=yang_db_runtime_get_oneline(ydrd, witem, &value, &vsize);
		if(res>=0){
			res=0;
			if(waitv==NULL){break;}
			if((vsize==wvsize) && !memcmp(waitv, value, vsize)){break;}
		}
		tout_ms-=10;
		if(tout_ms<0){
			res=1;
			break;
		}
		uc_dbal_releasedb(ydrd->dbald);
		CB_USLEEP(10000);
	}
	uc_dbal_releasedb(ydrd->dbald);
	return res;
}

static char *vkey_on_node(uc_dbald *dbald,
			  uint8_t *caps, uint8_t ki,
			  kvs_t *ckvs, uint8_t *ckss, uint8_t *kvi)
{
	int i, nkv;
	uint8_t ap, vkey, vtype;
	uint8_t raps[3]={caps[0]&0x7f,255,255};
	char *astr, *vstr, *vkstr=NULL;
	char *nstr=NULL;
	int rlen=1;
	char erstr[10];
	char *rstr=NULL;
	bool change_ap0=false;
	// check if keyvalue is on this node
	ap=caps[ki+1];
	caps[ki+1]=255;
	nkv=yang_db_runtime_getvknum(dbald, caps);
	if(nkv<=0){
		caps[0]^=0x80;
		change_ap0=true;
		nkv=yang_db_runtime_getvknum(dbald, caps);
		if(nkv<=0){goto erexit;}
	}
	rstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, 8);
	if(!rstr){goto erexit;}
	rstr[0]=0;
	for(i=0;i<nkv;i++){
		if(yang_db_runtime_getvkvtype(dbald, caps, i, &vkey, &vtype)){
			goto erexit;
		}
		raps[1]=vkey;
		if(yang_node_get_node_string(dbald, &nstr, raps)==0){
			astr=strchr(&nstr[1], '/');
			if(astr!=NULL){vkstr=astr+1;}
		}
		if(vkstr==NULL){
			snprintf(erstr, sizeof(erstr), "%d", raps[1]);
			vkstr=erstr;
		}
		rlen+=strlen(vkstr)+3;
		rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, rstr, rlen);
		if(ub_assert_fatal(rstr!=NULL, __func__, NULL)){goto erexit;}
		if(i==0){strcat(rstr, "|");}
		strcat(rstr, vkstr);
		strcat(rstr, ":");
		if(nstr!=NULL){
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, nstr);
		}

		vstr=yang_value_string(vtype, ckvs[*kvi], ckss[*kvi], 0, rstr);
		(*kvi)++;
		if(vstr!=NULL){
			rlen+=strlen(vstr);
			rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, rstr, rlen);
			if(ub_assert_fatal(rstr!=NULL, __func__, NULL)){goto erexit;}
			strcat(rstr, vstr);
		}
		strcat(rstr, "|");
	}
erexit:
	caps[ki+1]=ap;
	if(change_ap0){caps[0]^=0x80;}
	return rstr;
}

int yang_db_runtime_getkeyvkstr(uc_dbald *dbald,
				void *key, uint32_t ksize, char **rstr)
{
	uint8_t *caps;
	uint8_t raps[3]={255,255,255};
	uint8_t ckss[UC_MAX_VALUEKEYS];
	kvs_t ckvs[UC_MAX_VALUEKEYS+1]={NULL}; // +1 for NULL termination
	uint8_t ki, kvi;
	char erstr[10];
	int rlen=1;
	char *nstr=NULL, *vkstr, *pstr;
	if(yang_db_extract_key(key, ksize, &caps, ckvs, ckss)!=0){return -1;}
	raps[0]=caps[0];
	kvi=0;
	*rstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, 8);
	if(!rstr){return -1;}
	(*rstr)[0]=0;
	for(ki=0;ki<ksize;ki++){
		raps[1]=caps[ki+1];
		if(raps[1]==255){break;}
		if(ki>0){
			vkstr=vkey_on_node(dbald, caps, ki, ckvs, ckss, &kvi);
			if(vkstr){
				rlen+=strlen(vkstr);
				*rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *rstr, rlen);
				if(ub_assert_fatal(*rstr!=NULL, __func__, NULL)){
					UB_SD_RELMEM(YANGINIT_GEN_SMEM, vkstr);
					break;
				}
				strcat(*rstr, vkstr);
				UB_SD_RELMEM(YANGINIT_GEN_SMEM, vkstr);
			}
		}
		if(yang_node_get_node_string(dbald, &nstr, raps)==0){
			pstr=(ki==0)?nstr:strchr(&nstr[1], '/');
		}else{
			if(ki==0){
				snprintf(erstr, sizeof(erstr), "/%d/%d", raps[0], raps[1]);
			}else{
				snprintf(erstr, sizeof(erstr), "/%d", raps[1]);
			}
			pstr=erstr;
		}
		if(pstr!=NULL){
			rlen+=strlen(pstr);
			*rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *rstr, rlen);
			if(ub_assert_fatal(*rstr!=NULL, __func__, NULL)){break;}
			strcat(*rstr, pstr);
		}
		if(nstr!=NULL){
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, nstr);
		}
	}
	yang_db_extract_key_free(caps, ckvs, ckss);
	if(*rstr){return 0;}
	return -1;
}

// return N>=0:set node and need N value keys, -1:set one value key, -2:pass leaf,  -3:error
int yang_db_runtime_proc_nodestring(yang_db_runtime_dataq_t *ydrd, bool reset,
				    char *kstr, char *vstr)
{
	int kn;
	if(reset){ydrd->api=0;}
	if(ydrd->api>=UC_MAX_AP_DEPTH){return -2;}
	if(vstr!=NULL){
		if(proc_get_keyv(ydrd, kstr, vstr, true)==0){
			UB_LOG(UBL_DEBUG, "%s:kstr=%s, vstr=%s, ydrd->api=%d, set a vkey\n",
			       __func__, kstr, vstr, ydrd->api);
			return -1;
		}
		return -2;
	}
	if(proc_set_onenode(ydrd, kstr, true)==0){
		ydrd->aps[ydrd->api]=255;
		kn=yang_db_runtime_getvknum(ydrd->dbald, ydrd->aps);
		if(kn<=0){
			ydrd->aps[0]^=0x80;
			kn=yang_db_runtime_getvknum(ydrd->dbald, ydrd->aps);
			ydrd->aps[0]^=0x80;
		}
		UB_LOG(UBL_DEBUG, "%s:kstr=%s, ydrd->api=%d, set a node, needed num of vkey=%d\n",
		       __func__, kstr, ydrd->api, kn);
		return kn;
	}
	UB_LOG(UBL_DEBUG, "%s:invalid, kstr=%s\n", __func__, kstr);
	return -3;
}

int yang_db_runtime_state_keyvkstr(yang_db_runtime_dataq_t *ydrd, char **rstr)
{
	return yang_db_runtime_getkeyvkstr(ydrd->dbald, ydrd->aps, ydrd->api, rstr);
}
