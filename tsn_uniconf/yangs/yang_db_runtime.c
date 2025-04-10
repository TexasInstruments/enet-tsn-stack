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
#include <tsn_combase/cb_tmevent.h>
#include "yang_node.h"
#ifndef NO_YANG_CORES
#include "ieee1588-ptp-tt_access.h"
#else
static int ydbi_get_1588ptp_instance(uc_dbald *dbald, uint8_t ap0, const char *ginst_di)
{
	return -1;
}
#endif

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

static char* uc_strdup(char *src)
{
	size_t sz=strlen(src)+1;
	char *dst=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, sz);
	if(ub_assert_fatal(dst!=NULL, __func__, "alloc error")){return NULL;}
	memset(dst, 0, sz);
	memcpy(dst, src, sz);
	return dst;
}

static int total_kpi(yang_db_runtime_dataq_t *ydrd)
{
	unsigned int i;
	int res=0;
	// the last node:ydrd->api must be 'leaf' and has no keys
	for(i=0;i<ydrd->api;i++){res+=(int)ydrd->kpi[i];}
	return res;
}

/*
 retrun 0:get all key values, 1:no key values, -1:error
 if vstr==NULL, use kv="keyname:keyvalue" format
 if vstr!=NULL, use kv="keyname", vstr="keyvalue" format
 Yydrd->kvs[tkpi],Yydrd->kss[tkpi] are set.
 Yydrd->kvs[tkpi]=NULL,Yydrd->kss[tkpi]=1 are set when no key value exist.
 increment ydrd->kpi[ydrd->api-1u]
*/
static int proc_get_keyv(yang_db_runtime_dataq_t *ydrd, char *kv, char *vstr,
			 char *hints, bool noerrmsg)
{
	uint8_t rv[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	uint8_t kvtype;
	// kv fromt: key_name:key_value
	char *kn=kv; // key name
	char *kp=kv;
	char *tmp=NULL;
	void *value;
	uint32_t vsize;
	int res, tkpi;
	int rvi=0;
	UB_LOG(UBL_DEBUG, "%s:kv=%s, hints=%s\n", __func__, kv, hints);

	if(ydrd->api==0u){return -1;}
	if(vstr!=NULL){
		kp=vstr;
	}else{
		while((*kp!=0) && (*kp!=':')){kp++;}
		if(*kp==0){
			// 'key value' doesn't exist, no string after ':'
			// go ahead, and return NULL as the value key data
		}else{
			tmp=kp;
			*kp++=0; // kp is key value
		}
	}
	if(ydrd->aps[0]==XL4_DATA_RW){
		// this happens for prefexed data
		if(ydrd->api<3u){return -1;}
		if(ydrd->aps[1]!=UC_MIRROR){return -1;}
		if(ydrd->aps[2]!=UC_MIRROR_DEVICE){return -1;}
		if(ydrd->api==3){
			tkpi=total_kpi(ydrd);
			if(tkpi!=0){
				UB_LOG(UBL_ERROR, "%s:xl4data, tkpi=%d\n", __func__, tkpi);
				return -1;
			}
			ydrd->kvs[0]=UB_SD_REGETMEM(YANGINIT_GEN_SMEM,
						    ydrd->kvs[0], strlen(kp)+1);
			if(ub_assert_fatal(ydrd->kvs[0]!=NULL, __func__, NULL)){return -1;}
			strcpy((char*)ydrd->kvs[0], kp);
			ydrd->kss[0]=strlen(kp)+1;
			ydrd->kpi[2]=1;
			UB_LOG(UBL_DEBUGV, "%s:UC_MIRROR_DEVICE=%s\n",
			       __func__, (char*)ydrd->kvs[0]);
			return 0;
		}
		rv[0]=ydrd->aps[0];
		rv[1]=ydrd->aps[1];
		rv[2]=ydrd->aps[2];
		rvi=3;
	}

	ydrd->aps[ydrd->api]=0; // "valuekey" enum should be always 0
	if(kn[0]>='0' && kn[0]<='9'){
		res=strtol(kn, NULL, 0);
		if(res<0 || res>0xff){return -1;}
		rv[0+rvi]=res;
	}else{
		rv[0+rvi]=ydrd->aps[0+rvi];
		if((rv[0+rvi]==(uint8_t)XL4_EXTMOD_RW) || (rv[0+rvi]==(uint8_t)XL4_EXTMOD_RO)){
			if(ydrd->api<2u){return -1;}
			rv[1+rvi]=ydrd->aps[1+rvi];
		}
		if((rv[0+rvi]==(uint8_t)XL4_DATA_RW) || (rv[0+rvi]==(uint8_t)XL4_DATA_RO)){
			if(ydrd->api<2u){return -1;}
			rv[0+rvi]=(uint8_t)XL4_DATA_RW;
			rv[1+rvi]=ydrd->aps[1+rvi];
		}
		if(yang_node_get_node_enums(ydrd->dbald, kn, rv, 6)!=1u){
			UB_LOG(UBL_ERROR, "%s:unknown key name:%s, rv=%d,%d\n",
			       __func__, kn, rv[0+rvi], rv[1+rvi]);
			return -1;
		}
	}
	ydrd->aps[ydrd->api+1u]=rv[0];
	if(rvi){
		ydrd->api-=rvi;
		memcpy(rv, ydrd->aps, rvi); // save the data to recover later
		memmove(ydrd->aps, &ydrd->aps[rvi], ydrd->api+2u);
	}
	if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+4u, &value, &vsize)!=0){
		ydrd->aps[0]+=XL4_DATA_RO; // 2nd try with '_RO'
		if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+4u, &value, &vsize)!=0){
			char *rstr=NULL;
			if(!noerrmsg){
				ub_hexdump(true, true, ydrd->apsd, ydrd->api+4u, 0);
				if(ub_assert_fatal((ydrd->api+4u) <
						   (sizeof(ydrd->apsd)/sizeof(uint8_t)),
						   __func__, "invalid ap index")){return -1;}
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
	if(rvi){
		memmove(&ydrd->aps[rvi], ydrd->aps, ydrd->api+2u);
		memcpy(ydrd->aps, rv, rvi); // recover from the saved data
		ydrd->api+=rvi;
	}
	vsize=0;
	tkpi=total_kpi(ydrd);
	if(*kp==0){
		if(ydrd->kvs[tkpi]!=NULL){
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, ydrd->kvs[tkpi]);
			ydrd->kvs[tkpi]=NULL;
		}
		// set 1 to indicate "ydrd->kvs[tkpi]=NULL, but kv should exist"
		// this kss=1 is used only for "no key value" case
		ydrd->kss[tkpi]=1;
		res=1;
	}else{
		if(tkpi==0){
			res=ydbi_get_1588ptp_instance(ydrd->dbald, ydrd->apsd[2], kp);
			if(res>=0){
				sprintf(kp, "%d", res);
			}
		}
		if(hints==NULL){hints=kn;}
		res=yang_value_conv(kvtype, kp, &ydrd->kvs[tkpi], &vsize, hints);
		if(res<0){
			UB_LOG(UBL_ERROR, "%s:can't convert key value:%s\n", __func__, kp);
			return -1;
		}
		ydrd->kss[tkpi]=res;
		res=0;
	}
	if(ub_assert_fatal((ydrd->api-1u) < (sizeof(ydrd->kpi)/sizeof(uint8_t)),
			   __func__, "invalid kpi index")){return -1;}
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
	if(tmp!=NULL){*tmp=':';} //restore
	return res;
}

// retrurn 0:get all key values, 1: some key values are not there, -1:error
static int proc_get_keyvals(yang_db_runtime_dataq_t *ydrd, char *kvstr, char *hints)
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
		gkres=proc_get_keyv(ydrd, &kvstr[kvi], NULL, hints, false);
		if(gkres<0){return -1;}
		if(gkres==1){res=1;}
		kvstr[kpi]='|'; //restore
		kpi++;
		kvi=kpi;
	}
	return res;
}

static int proc_set_prefixed_onenode(yang_db_runtime_dataq_t *ydrd, char *kv, bool nokv)
{
	switch(ydrd->api){
	case 1u:
		// prefixed node has Uc_MIRROR as the second key
		if(strcmp(kv, "uc-mirror")!=0){
			UB_LOG(UBL_ERROR, "%s:invalid 2nd key:%s after xl4-data\n",
			       __func__, kv);
			return -1;
		}
		ydrd->aps[ydrd->api++]=UC_MIRROR;
		return 0;
	case 2u:
		// prefixed node has UC_MIRROR_DEVICE as the 3rd key
		if(strcmp(kv, UC_MIRROR_DEVICE_STR)!=0){
			UB_LOG(UBL_ERROR, "%s:invalid 3rd key:%s after xl4-data\n",
			       __func__, kv);
			return -1;
		}
		ydrd->aps[ydrd->api++]=UC_MIRROR_DEVICE;
		return 0;
	case 3u:
		if(strcmp(kv, "xl4-extmod")==0){
			ydrd->aps[ydrd->api++]=XL4_EXTMOD_RW;
			return 0;
		}
		ydrd->aps[ydrd->api++]=yang_node_mod_get_enum(ydrd->dbald, kv);
		return 0;
	case 4u:
		if(ydrd->aps[3]==XL4_EXTMOD_RW){
			ydrd->aps[ydrd->api++]=yang_node_extmod_get_enum(ydrd->dbald, kv);
			return 0;
		}
		return 1;
	}
	return 1;
}

static int proc_set_onenode(yang_db_runtime_dataq_t *ydrd, char *kv, bool nokv)
{
	int tkpi, res;
	int anumi;
	int roi=0;
	uint8_t rv[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	if(kv[0]>='0' && kv[0]<='9'){
		// the string is a number
		if(ub_assert_fatal(ydrd->api < (sizeof(ydrd->kpi)/sizeof(uint8_t)),
				   __func__, "invalid kpi index")){return -1;}
		ydrd->kpi[ydrd->api]=0;
		res=strtol(kv, NULL, 0);
		if(res<0 || res>0xff){return -1;}
		ydrd->aps[ydrd->api++]=res;
		return 0;
	}
	if(ydrd->api==0u){
		if(strcmp(kv, "xl4-data")==0){
			// to set prefixed data, the following must be "uc-mirror"
			ydrd->aps[0]=XL4_DATA_RW;
		}else if(strcmp(kv, "xl4-extmod")==0){
			ydrd->aps[0]=XL4_EXTMOD_RW;
		}else{
			ydrd->aps[0]=yang_node_mod_get_enum(ydrd->dbald, kv);
			if(ydrd->aps[0]==0xffu){
				UB_LOG(UBL_ERROR, "%s:invalid first key:%s\n",
				       __func__, kv);
				return -1;
			}
		}
		ydrd->kpi[ydrd->api++]=0;
		return 0;
	}
	anumi=1;
	if(!strcmp(kv, "..")){
		if(ub_assert_fatal((ydrd->api-1u) < (sizeof(ydrd->kpi)/sizeof(uint8_t)),
				   __func__, "invalid kpi index")){return -1;}
		ydrd->kpi[ydrd->api-1u]=0;
		if(nokv){ydrd->api--;}
		return 0;
	}
	rv[0]=ydrd->aps[0];
	if(ydrd->api>1u){
		rv[1]=ydrd->aps[1];
		anumi=2;
	}
	if(ydrd->aps[0]==XL4_DATA_RW){
		res=proc_set_prefixed_onenode(ydrd, kv, nokv);
		if(res<=0){return res;}
		rv[2]=ydrd->aps[2];
		if(ydrd->api>3){rv[3]=ydrd->aps[3];}
		if(ydrd->api>4){rv[4]=ydrd->aps[4];}
		roi=3;
	}else if(ydrd->aps[0]==XL4_EXTMOD_RW){
		if(ydrd->api==1u){
			ydrd->aps[ydrd->api++]=yang_node_extmod_get_enum(ydrd->dbald, kv);
			return 0;
		}
	}
	if(yang_node_get_node_enums(ydrd->dbald, kv, &rv[roi], anumi)!=1u){
		UB_LOG(UBL_ERROR, "%s:invalid key name:%s, api=%d\n",
		       __func__, kv, ydrd->api);
		return -1;
	}
	if(ub_assert_fatal(ydrd->api < (sizeof(ydrd->kpi)/sizeof(uint8_t)),
			   __func__, "invalid kpi index")){return -1;}
	ydrd->kpi[ydrd->api]=0;
	tkpi=total_kpi(ydrd);
	if(ydrd->kvs[tkpi]!=NULL){
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, ydrd->kvs[tkpi]);
		ydrd->kvs[tkpi]=NULL;
		ydrd->kss[tkpi]=0;
	}
	ydrd->aps[ydrd->api++]=rv[roi];
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
	int i;
	char *knwkv;
	UB_LOG(UBL_DEBUG, "%s:kstr=%s\n", __func__, kstr);
	if(kstr[kpi]=='/') {
		for(i=0;i<ydrd->api;i++){ydrd->kpi[i]=0;}
		ydrd->api=0;
		kpi++;
	}

	res=0;
	while(kstr[kpi] && (ydrd->api<(uint8_t)UC_MAX_AP_DEPTH)){
		kvi=kpi;
		nokv=false;
		knwkv=NULL;
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
		kstr[kpi-1]='|';
		knwkv=&kstr[kvi]; // save to use as a hing string

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

		gkres=proc_get_keyvals(ydrd, &kstr[kvi], knwkv);
		if(gkres<0){return -1;}
		if(gkres==1){res=1;}// no key values
	}
	ydrd->aps[ydrd->api]=255;
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
	res=proc_get_keyv(ydrd, &vstr[1], NULL, NULL, false);
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
	uint8_t rv[3]={XL4_DATA_RW, UC_MIRROR, UC_MIRROR_DEVICE};
	bool uc_mirror_mode=false;
	if(memcmp(ydrd->aps, rv, 3)==0){
		ydrd->api-=3;
		memmove(ydrd->aps, &ydrd->aps[3], ydrd->api);
		uc_mirror_mode=true;
	}
	if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+2u, &value, &vsize)!=0){
		ydrd->changtoRO=true;
		ydrd->aps[0]+=XL4_DATA_RO;
		if(uc_dbal_get(ydrd->dbald, ydrd->apsd, ydrd->api+2u, &value, &vsize)!=0){
			char *rstr=NULL;
			if(ydrd->aps[ydrd->api-1]==1){
				// this must be "dummy" data
				vtype=YANG_VTYPE_UINT8;
				goto erexit;
			}
			if(ydrd->aps[ydrd->api-1]==2){
				// this must be "nsasc" data
				vtype=YANG_VTYPE_STRING;
				goto erexit;
			}
			ub_hexdump(true, true, ydrd->apsd, ydrd->api+2u, 0);
			if(ub_assert_fatal((ydrd->api+2u) < (sizeof(ydrd->apsd)/sizeof(uint8_t)),
					   __func__, "invalid ap index")){return -1;}
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
erexit:
	if(uc_mirror_mode){
		memmove(&ydrd->aps[3], ydrd->aps, ydrd->api);
		memcpy(ydrd->aps, rv, 3); // recover from the saved data
		ydrd->api+=3;
	}
	return vtype;
}

static int proc_one_item(yang_db_runtime_dataq_t *ydrd, char *kstr, char *vstr, uint8_t onhw)
{
	int res;
	uint8_t vtype;
	uint32_t vsize;
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,onhw,
				      NULL,NULL,NULL,NULL,NULL,0};
	char *btkey=NULL, *kstrdup=NULL;

	UB_LOG(UBL_DEBUGV, "%s:kstr=%s, vstr=%s\n", __func__, kstr, vstr ? vstr : "NULL");
	kstrdup=uc_strdup(kstr);
	if(kstrdup!=NULL){
		btkey=kstrdup;
	}else{
		btkey=kstr;
	}
	res=proc_get_keys(ydrd, kstr);
	ydrd->changtoRO=false;
	if(res<0){res=-1; goto erexit;}
	if(res==1){res=0; goto erexit;} // this is key only line
	if(vstr && (vstr[0]==KEYV_DELIMITER) && (strlen(vstr)>1) &&
	   (vstr[strlen(vstr)-1]==KEYV_DELIMITER)){
		if(kstrdup!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kstrdup);}
		return copy_list(ydrd, vstr);
	}
	if(res==2){dbpara.atype=YANG_DB_ACTION_APPEND;} // key end with '+"
	if(res==3){
		// key end with '-"
		dbpara.atype=YANG_DB_ACTION_DELETE;
	}
	res=get_value_type(ydrd);
	if(res<0){goto erexit;}
	vtype=(uint8_t)res;
	res=0;
	if(dbpara.atype!=YANG_DB_ACTION_DELETE){
		vsize=0;
		res=yang_value_conv(vtype, vstr, &dbpara.value, &vsize, btkey);
		if(res<0){
			UB_LOG(UBL_ERROR, "%s:invalid value:%s\n", __func__, vstr ? vstr : "NULL");
			ydrd->api--;
			if(ydrd->changtoRO){ydrd->aps[0]-=XL4_DATA_RO;}
			if(kstrdup!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kstrdup);}
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
erexit:
	if(kstrdup!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kstrdup);}
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

void yang_db_runtime_set_hwadl(yang_db_runtime_dataq_t *ydrd, uc_hwald *hwald)
{
	ydrd->hwald=hwald;
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
			kss[i]=0; // end mark
			kvs[i]=NULL;
			break;
		}
		// when ydrd->kvs[i]==NULL, ydrd->kss[i]==1
		kvs[i]=ydrd->kvs[i];
		kss[i]=ydrd->kss[i];
	}
	if(status){
		ydrd->aps[0]+=XL4_DATA_RO;
		key2[0]+=XL4_DATA_RO;
	}
	return uc_get_range(ydrd->dbald, ydrd->aps, ydrd->api, key2, ydrd->api);
}

static int cmp_kvs(uint8_t *vp, void **kvs, uint8_t *kss,
		   uint32_t kp, uint32_t ksize)
{
	int i;
	for (i=0;i<UC_MAX_KV_DEPTH;i++){
		if(kp>=ksize){return 0;}
		if(kss[i]==0){return 0;}
		if(kvs[i]==NULL){
			// skip this
			kp+=vp[kp]+1;
			continue;
		}
		if(kss[i]!=vp[kp]){return 1;}
		if(memcmp(&vp[kp+1], kvs[i], kss[i])!=0){return 1;}
		kp+=vp[kp]+1;
	}
	return -1;
}

int yang_db_runtime_iterate_fromline(yang_db_runtime_dataq_t *ydrd, uc_range **range,
				     const char* line, void *kvs[], uint8_t kss[],
				     bool status, void **value,
				     uint32_t *vsize, char **qstr)
{
	uint8_t akey[UC_MAX_AP_DEPTH];
	void *key;
	uint32_t ksize;
	unsigned int i;
	char *prstr=NULL;
	int vtype=0;

	if(*range==NULL){
		kvs[0]=NULL;
		*range=yang_db_runtime_range_fromline(ydrd, line, kvs, kss, status);
		if(*range==NULL){
			UB_LOG(UBL_DEBUG, "%s:no data\n", __func__);
			return -1;
		}
	}
	while(true){
		if(uc_get_keyvalue_in_range(ydrd->dbald, *range, &key, &ksize,
					    value, vsize,
					    UC_DBAL_FORWARD)){break;}
		if(kss[0]!=0){
			for(i=0;i<ksize;i++){
				if(((uint8_t *)key)[i]==255){break;}
			}
			if(cmp_kvs((uint8_t*)key, kvs, kss, i+1, ksize)){
				continue;
			}
		}
		if(ksize>2 && ((uint8_t*)key)[0]==0 && ((uint8_t*)key)[1]==0){
			// *key is immutable
			// value type, valuekey type case, it is not terminated with 255
			for(i=ksize-1;i>=2;i--){
				// if '0':valuekey is there, it should be skipped
				if(((uint8_t*)key)[i]==0){
					break;
				}
			}
			if(i>=2){continue;} // skip valuekey
			memcpy(akey, &((uint8_t*)key)[2], ksize-2);
			akey[ksize-2]=255;
			ksize-=1;
			key=akey;
			*value=NULL;
		}
		if(yang_db_runtime_getkeyvkstr(ydrd->dbald, key, ksize, &prstr)!=0){
			continue;
		}
		vtype=yang_db_runtime_get_vtype(ydrd->dbald, (uint8_t*)key);
		if(vtype>=0 && qstr!=NULL){
			*qstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, strlen(prstr)+1);
			if(*qstr!=NULL){strcpy(*qstr, prstr);}
		}
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, prstr);
		if(vtype<0){
			// skip this: data exists, but vtype is not defined.
			// it happens for *_NSASC.
			continue;
		}
		return vtype;
	}
	uc_get_range_release(ydrd->dbald, *range);
	*range=NULL;
	return -1;
}

int yang_db_runtime_cache_fromline(yang_db_runtime_dataq_t *ydrd,
				   const char* line, bool status,
				   char **rvalue, int *rvsize)
{
	void *value=NULL;
	uint32_t vsize=0;
	void *kvs[UC_MAX_KV_DEPTH+1];
	uint8_t kss[UC_MAX_KV_DEPTH];
	uc_range *range=NULL;
	char *qstr;
	int vtype;
	char *vstr;
	int qlen, vlen;
	while(true){
		vtype=yang_db_runtime_iterate_fromline(
			ydrd, &range, line, kvs, kss, status,
			&value, &vsize, &qstr);
		if(vtype<0){break;}
		vstr=yang_value_string(vtype, value, vsize, 0, qstr);
		if(vstr==NULL){continue;}
		qlen=strlen(qstr)+1;
		vlen=strlen(vstr)+1;
		*rvalue=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *rvalue,
					      *rvsize+qlen+vlen);
		if(ub_assert_fatal(*rvalue!=NULL, __func__, NULL)){return -1;}
		strcpy(&((char*)*rvalue)[*rvsize], qstr);
		*rvsize+=qlen;
		strcpy(&((char*)*rvalue)[*rvsize], vstr);
		*rvsize+=vlen;
	}
	return *rvsize;
}

void yang_db_runtime_cache_fromline_release(char *rvalue)
{
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, rvalue);
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
int yang_db_runtime_readfile(yang_db_runtime_dataq_t *ydrd, const char* fname)
{
	void *inf;
	char *cp;
	char linebuf[YANG_DB_LINE_BUF_SIZE+1u];
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
		rsize=ub_fioread(inf, &linebuf[psize], YANG_DB_LINE_BUF_SIZE-1-psize);
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
			if(psize+rsize==YANG_DB_LINE_BUF_SIZE-1){
				UB_LOG(UBL_ERROR, "%s:too long line\n", __func__);
				res=-1;
				break;
			}
		}
		np=(int)strlen(linebuf)+1;
		nlen=psize+rsize-np;
		res=proc_one_line(ydrd, linebuf);
		if(res<0){break;}
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

int yang_db_runtime_read_conffile(uc_dbald *dbald, const char* fname)
{
	int res;
	yang_db_runtime_dataq_t *ydrd;
	if(strstr(fname, ".conf")==NULL){return -1;}
	ydrd=yang_db_runtime_init(dbald, NULL);
	if(!ydrd){return -1;}
	res=yang_db_runtime_readfile(ydrd, fname);
	yang_db_runtime_close(ydrd);
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

static char *vkey_on_reg_node(uc_dbald *dbald,
			      uint8_t *caps, uint8_t ki,
			      kvs_t *ckvs, uint8_t *ckss, uint8_t *kvi)
{
	int i, nkv, nckv=0;
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
	while(ckvs[nckv]!=NULL){nckv++;} //get number of ckvs provided by caller
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
		/* skip index that is more than ckvs count, to prevent access */
		if((*kvi)<nckv){
			vstr=yang_value_string(vtype, ckvs[*kvi], ckss[*kvi], 0, rstr);
			(*kvi)++;
			if(vstr!=NULL){
				rlen+=strlen(vstr);
				rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, rstr, rlen);
				if(ub_assert_fatal(rstr!=NULL, __func__, NULL)){goto erexit;}
				strcat(rstr, vstr);
			}
		}
		strcat(rstr, "|");
	}
erexit:
	caps[ki+1]=ap;
	if(change_ap0){caps[0]^=0x80;}
	return rstr;
}

static char *vkey_on_xl4data_node(uc_dbald *dbald,
				  uint8_t *caps, uint8_t ki,
				  kvs_t *ckvs, uint8_t *ckss, uint8_t *kvi)
{
	char *rstr;
	if(ki<2) return NULL;
	if(caps[1]==UC_MIRROR){
		if(ki==2){
			char *vkname="|name:";
			int rlen=strlen(vkname)+2;
			char *vkstr="";
			if((ckvs!=NULL) && (ckvs[0]!=NULL)){
				vkstr=(char*)ckvs[0];
				rlen+=strlen(vkstr);
			}
			rstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, rlen);
			if(ub_assert_fatal(rstr!=NULL, __func__, NULL)){return NULL;}
			sprintf(rstr, "%s%s|", vkname, vkstr);
			(*kvi)++;
			return rstr;
		}
		return vkey_on_reg_node(dbald, &caps[3], ki-3, ckvs, ckss, kvi);
	}
	return NULL;
}

static char *vkey_on_node(uc_dbald *dbald,
			  uint8_t *caps, uint8_t ki,
			  kvs_t *ckvs, uint8_t *ckss, uint8_t *kvi)
{
	if((caps[0]==XL4_DATA_RW) || (caps[0]==XL4_DATA_RO)){
		return vkey_on_xl4data_node(dbald, caps, ki, ckvs, ckss, kvi);
	}
	return vkey_on_reg_node(dbald, caps, ki, ckvs, ckss, kvi);
}

int yang_db_runtime_apkv2keyvkstr(uc_dbald *dbald, uint8_t *aps,
				  kvs_t *kvs, uint8_t *kss, char **rstr)
{
	uint8_t raps[6]={255,255,255,255,255,255};
	uint8_t ki, kvi;
	char erstr[10];
	int rlen=1;
	int rki;
	char *nstr=NULL, *vkstr, *pstr;
	if(aps[0]==255){return -1;} // no aps
	raps[0]=aps[0];
	kvi=0;
	if(!rstr){return -1;}
	*rstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, 8);
	if(!(*rstr)){return -1;}
	(*rstr)[0]=0;
	rki=1;
	for(ki=0;ki<255u;ki++){
		if(aps[1+ki]==255u){break;}
		/*
		 * in normal cases, get category by the first key,
		 * then the target enum code by the second key.
		 * in uc_mirror case, the first 3 keys are prefix keys,
		 * another 2 keys are needed to get the target enum code.
		 */
		if((aps[0]==XL4_DATA_RW) || (aps[0]==XL4_DATA_RO)){
			if(ki==1){
				rki=2;
			}else if(ki==2){
				rki=3;
			}else if(ki==3){
				rki=4;
			}
		}
		raps[rki]=aps[1+ki];
		if(ki>0){
			vkstr=vkey_on_node(dbald, aps, ki, kvs, kss, &kvi);
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
			pstr=(ki==0)?nstr:strrchr(nstr, '/');
		}else{
			if(ki==0){
				snprintf(erstr, sizeof(erstr), "/%d/%d", raps[0], raps[1]);
			}else{
				snprintf(erstr, sizeof(erstr), "/%d", raps[rki]);
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
	if(*rstr){
		return 0;
	}
	return -1;
}

int yang_db_runtime_getkeyvkstr(uc_dbald *dbald,
				void *key, uint32_t ksize, char **rstr)
{
	uint8_t *caps;
	uint8_t ckss[UC_MAX_VALUEKEYS];
	kvs_t ckvs[UC_MAX_VALUEKEYS+1]={NULL}; // +1 for NULL termination
	int res;

	if(yang_db_extract_key(key, ksize, &caps, ckvs, ckss)!=0){return -1;}
	res=yang_db_runtime_apkv2keyvkstr(dbald, caps, ckvs, ckss, rstr);
	yang_db_extract_key_free(caps, ckvs, ckss);
	return res;
}


int yang_db_runtime_nodestr2apkv(yang_db_runtime_dataq_t *ydrd,
				 const char* line, uint8_t *aps, void **kvs, uint8_t *kss)
{
	char *lstr;
	int vtype;
	int res;
	int i;
	if((line==NULL) || (line[0]==0)){return -1;}

	lstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, strlen(line)+1u);
	if(ub_assert_fatal(lstr!=NULL, __func__, NULL)){return -1;}
	memcpy(lstr, line, strlen(line)+1u);
	res=proc_get_keys(ydrd, lstr);
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, lstr);
	if(res==1){
		// end with "/", copy only aps part
		memcpy(aps, ydrd->aps, ydrd->api);
		aps[ydrd->api]=255;
		return -1;
	}
	if(res!=0 && res!=2){
		UB_LOG(UBL_ERROR, "%s:can't process line:%s\n", __func__, line);
		return -1;
	}
	if(line[strlen(line)-1]=='|'){
		UB_LOG(UBL_INFO, "%s:copy line:%s\n", __func__, line);
		vtype=-1;
	}else{
		vtype=get_value_type(ydrd);
		if(vtype<0){
			UB_LOG(UBL_ERROR, "%s:can't get vtype:%s\n", __func__, line);
			return -1;
		}
	}
	memcpy(aps, ydrd->aps, ydrd->api);
	aps[ydrd->api--]=255; // decrement api to set yard->aps without the last leaf
	for(i=0;i<UC_MAX_KV_DEPTH+1;i++){
		if(i==UC_MAX_KV_DEPTH || ydrd->kss[i]==0){
			kss[i]=0;
			kvs[i]=NULL;
			break;
		}
		kvs[i]=ydrd->kvs[i];
		kss[i]=ydrd->kss[i];
	}
	return vtype;
}

// return N>=0:set node and need N value keys, -1:set one value key, -2:pass leaf,  -3:error
int yang_db_runtime_proc_nodestring(yang_db_runtime_dataq_t *ydrd, bool reset,
				    char *kstr, char *vstr)
{
	int kn;
	if(reset){ydrd->api=0;}
	if(ydrd->api>=UC_MAX_AP_DEPTH){return -2;}
	if(vstr!=NULL){
		if(proc_get_keyv(ydrd, kstr, vstr, NULL, true)==0){
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
	uint8_t aps[UC_MAX_AP_DEPTH+2];
	if ((ydrd->api+1)>(UC_MAX_AP_DEPTH+2)) {return -1;}
	memcpy(aps, ydrd->aps, ydrd->api);
	// appending '0' here is a strange action
	// it is workaround to fix the issue in convxml2conf.c:convxml2conf_getconf
	aps[ydrd->api]=0;
	aps[ydrd->api+1]=255u;
	return yang_db_runtime_getkeyvkstr(ydrd->dbald, aps, ydrd->api+2, rstr);
}

bool yang_db_runtime_needaction(yang_db_runtime_dataq_t *ydrd)
{
	return yang_db_needaction(ydrd->dbald, ydrd->aps, ydrd->kvs, ydrd->kss);
}

void yang_db_runtime_readdb_log(bool consoleprint, uc_dbald *dbald, const char *headmsg,
				uint8_t aps[], void *kvs[], uint8_t kss[])
{
	yang_vtype_enum_t vtype;
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
		NULL, aps, kvs, kss, NULL, 0};
	int64_t v;
	if(headmsg==NULL){headmsg="";}
	vtype=(yang_vtype_enum_t)yang_db_runtime_get_vtype(dbald, (uint8_t *)aps);
	if(yang_db_action(dbald, NULL, &dbpara)!=0){return;}
	switch(vtype){
	case YANG_VTYPE_MAC_ADDRESS:
		UB_SELECT_PRINT(consoleprint, true, "%s "UB_PRIhexB6"\n", headmsg,
				UB_ARRAY_B6((uint8_t*)dbpara.value));
		break;
	case YANG_VTYPE_STREAMID:
		UB_SELECT_PRINT(consoleprint, true, "%s "UB_PRIhexB8"\n", headmsg,
				UB_ARRAY_B8((uint8_t*)dbpara.value));
		break;
	case YANG_VTYPE_INT8:
	case YANG_VTYPE_INT16:
	case YANG_VTYPE_INT32:
	case YANG_VTYPE_INT64:
	case YANG_VTYPE_UINT8:
	case YANG_VTYPE_UINT16:
	case YANG_VTYPE_UINT32:
	case YANG_VTYPE_UINT64:
		v=ub_int64_from_non_aligned(dbpara.value, dbpara.vsize, NULL);
		UB_SELECT_PRINT(consoleprint, true, "%s %"PRIi64"\n", headmsg, v);
		break;
	case YANG_VTYPE_STRING:
		UB_SELECT_PRINT(consoleprint, true, "%s %s\n", headmsg, (char*)dbpara.value);
		break;
	default:
		UB_SELECT_PRINT(consoleprint, true, "%s unknown vtype=%d\n", headmsg, vtype);
		ub_hexdump(consoleprint, true, (unsigned char*)dbpara.value, dbpara.vsize, 0);
		break;
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, NULL, &dbpara);
	return;
}
