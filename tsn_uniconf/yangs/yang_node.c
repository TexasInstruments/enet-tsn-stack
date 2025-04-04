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
#include "../uc_notice.h"
#include "yang_node.h"
#include "yang_db_access.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

#ifdef UC_RUNCONF
extern const char *yang_static_get_name_by_id(uc_dbald *dbald, uint8_t nameid, uint8_t ap);
#define YANG_STATIC_GET_NAME_BY_ID yang_static_get_name_by_id
#else
#define YANG_STATIC_GET_NAME_BY_ID(...) NULL
#endif

extern uint8_t get_onenode_enum(uc_dbald *dbald, uint8_t *aps, const char *nstr,
				uc_range **range, int *anumdepth, int level);

extern int set_init_aps(uc_dbald *dbald, const char *astr, uint8_t *aps,
			int maxele, int *thispi, int anumdepth);

const char *xl4data_enum_strings[XL4_DATA_ENUM_END]={
	"yang-value-types",
	"uc-ready",
	"uc-askaction-reg",
	"uc-notice-reg",
	"uc-notice-act",
	"uc-yangmod-reg",
	"xl4-extmod-reg",
	"XL4-enum-table",
	"xl4-rtime-reg",
	"xl4-rtime-data",
	"uc-last-db-access",
	"uc-prefix-namespace",
	"uc-mirror",
	"uc-needaction-reg",
	"uc-static-work",
};

#define REG_NEXTID 0u
static int get_next_nameid(uc_dbald *dbald, uint8_t ap, bool add, const char *name)
{
	uint8_t nameid;
	void *value;
	uint32_t vsize;
	uint8_t aps[4]={XL4_DATA_RW, ap, REG_NEXTID, 255};
	const char *mtype="";
	switch(ap){
	case XL4_RTIME_REG:
		aps[0]=XL4_DATA_RO;
		mtype="rtime";
		break;
	case UC_YANGMOD_REG:
		mtype="mod";
		break;
	case XL4_EXTMOD_REG:
		mtype="extmod";
		break;
	}
	if(!uc_dbal_get(dbald, aps, 4, &value, &vsize)){
		nameid=*((uint8_t *)value);
		uc_dbal_get_release(dbald, aps, 4, value, vsize);
	}else{
		nameid=0u;
	}
	if(!add){return nameid;}
	nameid+=1u;
	if(nameid==255u){
		UB_LOG(UBL_ERROR, "%s:no more space\n", __func__);
		return -1;
	}
	value=&nameid;
	vsize=1;
	if(uc_dbal_create(dbald, aps, 4, value, vsize)!=0){
		if(name==NULL){name="unknown";}
		UB_LOG(UBL_ERROR, "%s:can't update NEXTID in db, modname=%s\n",
		       __func__, name);
		return -1;
	}
	UB_LOG(UBL_DEBUG, "%s:a new %s=%s, enum=%d\n",
	       __func__, mtype, name, nameid-1);
	(void)mtype;
	return 	nameid-1;
}

static int setget_id_by_name(uc_dbald *dbald, const char *name, uint8_t ap, bool reg)
{
	uint8_t aps[4]={XL4_DATA_RW, ap, 255, 255};
	void *kvs[2];
	uint8_t kss[1];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ,YANG_DB_ONHW_NOACTION,
	                              NULL,aps,kvs,kss,NULL,0};
	int nameid;

	if(!dbald || name==NULL){return -1;}
	switch(ap){
	case XL4_RTIME_REG:
		aps[0]=XL4_DATA_RO;
		break;
	case UC_YANGMOD_REG:
	case XL4_EXTMOD_REG:
		break;
	default:
		return -1;
	}
	kss[0]=strlen(name)+1;
	kvs[0]=(void*)name;
	kvs[1]=NULL;
	if(!yang_db_action(dbald, NULL, &dbpara)){
		nameid=*((uint8_t *)dbpara.value);
		dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		(void)yang_db_action(dbald, NULL, &dbpara);
		UB_LOG(UBL_DEBUGV, "%s:get id=%d from name=%s\n",
		       __func__, nameid, name);
		return nameid;
	}
	if(!reg){return -1;}
	nameid=get_next_nameid(dbald, ap, true, name);
	if(nameid<0){return -1;}
	dbpara.atype=YANG_DB_ACTION_CREATE;
	dbpara.value=&nameid;
	dbpara.vsize=1;
	if(yang_db_action(dbald, NULL, &dbpara)!=0){
		UB_LOG(UBL_ERROR, "%s:can't update, modname=%s, nameid=%d\n",
		       __func__, name, nameid);
		return -1;
	}
	return nameid;
}

static const char *get_name_by_id(uc_dbald *dbald, uint8_t nameid, uint8_t ap)
{
	uc_range *range;
	uint8_t *nkey;
	uint32_t nksize;
	char *vdata;
	uint32_t vsize;
	uint8_t aps1[4]={XL4_DATA_RW, ap, 255u, 0};
	uint8_t aps2[4]={XL4_DATA_RW, ap, 255u, 255u};
	char *resc=NULL;
	if(ap==XL4_RTIME_REG){
		aps1[0]=XL4_DATA_RO;
		aps2[0]=XL4_DATA_RO;
	}
	range=uc_get_range(dbald, aps1, 4, aps2, 4);
	while(range){
		if(uc_get_keyvalue_in_range(dbald, range, (void**)&nkey, &nksize,
					    (void**)&vdata, &vsize,
					    UC_DBAL_FORWARD)!=0){break;}
		if(vsize==1 && *((uint8_t*)vdata)==nameid){
			// returning the pointer of key part.
			resc=(char*)&nkey[4];
			break;
		}
	}
	if(range){uc_get_range_release(dbald, range);}
	return resc;
}

int yang_node_uniconf_init(uc_dbald *dbald)
{
	uint8_t anums[1]={255};
	if(dbald==NULL){return -1;}
	if(setget_id_by_name(dbald, "xl4-data", UC_YANGMOD_REG, true)!=0){return -1;}
	if(yang_node_get_node_enums(dbald, "/xl4-data", anums, 1)!=1){return -1;}
	if(anums[0]!=0){return -1;}
	if(setget_id_by_name(dbald, "xl4-extmod", UC_YANGMOD_REG, true)!=1){return -1;}
	// yang module id start from '2'
	return 0;
}

uint8_t yang_node_get_xl4data_enum(const char *astr)
{
	int i;
	for(i=0;i<XL4_DATA_ENUM_END;i++){
		if(!strcmp(astr, xl4data_enum_strings[i])){return i;}
	}
	return 0xff;
}

const char *yang_node_get_xl4data_string(uint8_t anum)
{
	if(anum>=XL4_DATA_ENUM_END){return NULL;}
	return xl4data_enum_strings[anum];
}

int yang_node_get_node_enums(uc_dbald *dbald, const char *astr, uint8_t *anums, int maxele)
{
	char *nstr=NULL;
	const char *q;
	int i;
	uc_range *range=NULL;
	int thispi=0;
	int slen;
	int anumi=0;
	int anumdepth, level;
	uint8_t aps[UC_MAX_AP_DEPTH+1]={0};
	if(astr[0]=='/'){
		anumdepth=0;
		level=0;
	}else{
		for(i=0;i<maxele;i++){
			aps[i]=anums[i];
			if(aps[i]==255u){break;}
			if(i==UC_MAX_AP_DEPTH){
				break;
			}
		}
		aps[0]&=0x7f;
		anumdepth=i;
		level=i;
	}
	// anumdepth is depth of preset nodes, start from '1', max is '5'
	anumdepth=set_init_aps(dbald, astr, aps, UC_MAX_AP_DEPTH, &thispi, anumdepth);
	aps[anumdepth]=255u; // terminate with 255
	UB_LOG(UBL_DEBUGV, "%s:astr=%s, anumdepth=%d\n", __func__, astr, anumdepth);
	thispi=0;
	while(astr[thispi]!=0){
		if(astr[thispi]=='/'){
			thispi++;
		}
		q=(char*)strchr(&astr[thispi], '/');
		if(q!=NULL){
			slen=q-&astr[thispi];
		}else{
			slen=strlen(&astr[thispi]);
		}
		nstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, nstr, slen+1);
		memcpy(nstr, &astr[thispi], slen);
		nstr[slen]=0;
		anums[anumi]=get_onenode_enum(dbald, aps, nstr, &range, &anumdepth,
					      level);
		if(anums[anumi]==255u){break;}
		anumi++;
		if(anumi>=maxele){break;}
		thispi+=slen;
		level++;
	}
	if(nstr!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, nstr);}
	if(range!=NULL){uc_get_range_release(dbald, range);}
	if(anumi<maxele){anums[anumi]=255u;}
	return anumi;
}

static const char *get_regnode_string(uc_dbald *dbald, int level,
				      uint8_t *aps, uint8_t *anums,
				      int *relbstr, uint32_t *bsize)
{
	const char *bstr;

	if((anums[0]==XL4_EXTMOD_RW) || (anums[0]==XL4_EXTMOD_RO)){
		if(level==0){
			return "xl4-extmod";
		}else if(level==1){
			bstr=get_name_by_id(dbald, anums[1], XL4_EXTMOD_REG);
			if(bstr==NULL){
				return NULL;
			}
			aps[2]=XL4_EXTMOD_RW;
			aps[3]=anums[level];
			aps[4]=255u;
			aps[5]=1u;
			return bstr;
		}else{
			aps[6]=anums[level];
			if(uc_dbal_get(dbald, aps, 7, (void**)&bstr, bsize)!=0){
				UB_LOG(UBL_WARN,
				       "%s:no node string in DB,  mod=%d, node=%d\n",
				       __func__, anums[1], anums[level]);
				return NULL;
			}
			*relbstr=7;
			return bstr;
		}
	}else{
		if(level==0){
			bstr=get_name_by_id(dbald, anums[0]&~0x80, UC_YANGMOD_REG);
			if(bstr==NULL){
				// this is error
				return NULL;
			}
			aps[2]=anums[0]&0x7f;
			return bstr;
		}else{
			bstr=YANG_STATIC_GET_NAME_BY_ID(dbald, anums[level], aps[2]);
			if(bstr!=NULL){
				return bstr;
			}
			aps[3]=255;
			aps[4]=1;
			aps[5]=anums[level];
			if(uc_dbal_get(dbald, aps, 6, (void**)&bstr, bsize)!=0){
				UB_LOG(UBL_WARN,
				       "%s:no node string in DB,  mod=%d, node=%d\n",
				       __func__, anums[0], anums[level]);
				return NULL;
			}
			*relbstr=6;
			return bstr;
		}
	}
}

// level > 1
static const char *xl4data_get_ext_string(uc_dbald *dbald, int level,
					  uint8_t *aps, uint8_t *anums,
					  int *relbstr, uint32_t *bsize)
{
	switch(anums[1]){
	default:
		return NULL;
	case UC_MIRROR:
		if(level==2){
			return UC_MIRROR_DEVICE_STR;
		}
		return get_regnode_string(dbald, level-3, aps,
					  &anums[3], relbstr, bsize);
	}
}

int yang_node_get_node_string(uc_dbald *dbald, char **rstr, uint8_t *anums)
{
	uint8_t aps[7]={XL4_DATA_RW, XL4_ENUM_TABLE, 0, };
	int i;
	const char *bstr;
	uint32_t bsize;
	uint32_t asize=1;
	int relbstr=0;
	int res=-1;
	char estr[8];
	if(dbald==NULL){return -1;}
        *rstr=NULL;
	for(i=0;i<UC_MAX_AP_DEPTH;i++){
		if(anums[i]==255u){break;}
		bstr=NULL;
		res=0;
		switch(anums[0]){
		case 255u:
			res=-1;
			break;
		case XL4_DATA_RW:
		case XL4_DATA_RO:
			if(i==0){
				bstr="xl4-data";
			}else if(i==1){
				bstr=yang_node_get_xl4data_string(anums[i]);
			}else{
				bstr=xl4data_get_ext_string(dbald, i, aps, anums,
							    &relbstr, &bsize);
			}
			if(bstr==NULL){
				res=1;
				break;
			}
			bsize=strlen(bstr)+1;
			break;
		default:
			bstr=get_regnode_string(dbald, i, aps, anums, &relbstr, &bsize);
			if(bstr==NULL){
				res=1;
				break;
			}
			bsize=strlen(bstr)+1;
			break;
		}
		if(res==1){
			snprintf(estr, 4, "%d", anums[i]);
			bstr=estr;
			bsize=strlen(bstr)+1;
		}
		if(res<0){break;}
		if(bstr!=NULL){
			*rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *rstr, asize+bsize);
			if(ub_assert_fatal(*rstr!=NULL, __func__, "realloc")){return -1;}
			(*rstr)[asize-1]='/';
			memcpy(&((*rstr)[asize]), bstr, bsize);
			asize=asize+bsize;
			if(relbstr){
				uc_dbal_get_release(dbald, aps, relbstr,
						    (void*)bstr, bsize);
				relbstr=0;
			}
		}
	}
	if(res<0){
		if(*rstr!=NULL){
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, *rstr);
			*rstr=NULL;
		}
		UB_LOG(UBL_ERROR, "%s:failed i=%d\n", __func__, i);
	}
	if(*rstr==NULL){return -1;}
	return 0;
}

int yang_node_mod_get_string(uc_dbald *dbald, uint8_t modid, char *modname, int nsize)
{
	const char *astr;
	astr=get_name_by_id(dbald, modid, UC_YANGMOD_REG);
	if(astr==NULL){return -1;}
	ub_strncpy(modname, astr, nsize);
	return 0;
}

int yang_node_extmod_get_string(uc_dbald *dbald, uint8_t modid, char *modname, int nsize)
{
	const char *astr;
	astr=get_name_by_id(dbald, modid, XL4_EXTMOD_REG);
	if(astr==NULL){return -1;}
	ub_strncpy(modname, astr, nsize);
	return 0;
}

uint8_t yang_node_mod_get_enum(uc_dbald *dbald, const char *emodname)
{
	int res=setget_id_by_name(dbald, emodname, UC_YANGMOD_REG, false);
	if(res<0){return 0xff;}
	return (uint8_t)res;
}

int yang_node_mod_set_enum(uc_dbald *dbald, const char *emodname)
{
	return setget_id_by_name(dbald, emodname, UC_YANGMOD_REG, true);
}

int yang_node_mod_numofids(uc_dbald *dbald)
{
	return get_next_nameid(dbald, UC_YANGMOD_REG, false, NULL);
}

uint8_t yang_node_extmod_get_enum(uc_dbald *dbald, const char *emodname)
{
	int res=setget_id_by_name(dbald, emodname, XL4_EXTMOD_REG, false);
	if(res<0){return 0xff;}
	return (uint8_t)res;
}

int yang_node_extmod_set_enum(uc_dbald *dbald, const char *emodname)
{
	return setget_id_by_name(dbald, emodname, XL4_EXTMOD_REG, true);
}

int yang_node_extmod_numofids(uc_dbald *dbald)
{
	return get_next_nameid(dbald, XL4_EXTMOD_REG, false, NULL);
}

int yang_node_rtime_get_string(uc_dbald *dbald, uint8_t nameid, char *name, int nsize)
{
	const char *astr;
	astr=get_name_by_id(dbald, nameid, XL4_RTIME_REG);
	if(astr==NULL){return -1;}
	ub_strncpy(name, astr, nsize);
	return 0;
}

uint8_t yang_node_rtime_get_enum(uc_dbald *dbald, const char *name)
{
	int res=setget_id_by_name(dbald, name, XL4_RTIME_REG, false);
	if(res<0){return 0xff;}
	return (uint8_t)res;
}

int yang_node_rtime_set_enum(uc_dbald *dbald, const char *name)
{
	return setget_id_by_name(dbald, name, XL4_EXTMOD_REG, true);
}

// this returns direct DB data, effective only in the same DB session
const char *yang_node_get_prefix_namespace(uc_dbald *dbald, const char *prefix)
{
	uint8_t aps[3]={XL4_DATA_RW, UC_PREFIX_NAMESPACE, 255};
	void *kvs[2]={(void*)prefix, NULL};
	uint8_t kss[1]={strlen(prefix)+1};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ,YANG_DB_ONHW_NOACTION,
		NULL,aps,kvs,kss,NULL,0};
	if(yang_db_action(dbald, NULL, &dbpara)!=0){return NULL;}
	// namespace value is never changed, so no need YANG_DB_ACTION_READ_RELEASE
	return (const char*)dbpara.value;
}

int yang_node_set_prefix_namespace(uc_dbald *dbald, const char *prefix,
				   const char *name_space)
{
	uint8_t aps[3]={XL4_DATA_RW, UC_PREFIX_NAMESPACE, 255};
	void *kvs[2]={(void*)prefix, NULL};
	uint8_t kss[1]={strlen(prefix)+1};
	void *value=(void*)name_space;
	uint32_t vsize=strlen(name_space)+1;
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ,YANG_DB_ONHW_NOACTION,
		NULL,aps,kvs,kss,NULL,0};
	if(yang_db_action(dbald, NULL, &dbpara)==0){
		if((vsize!=dbpara.vsize) || (memcmp(value, dbpara.value, vsize)!=0)){
			UB_LOG(UBL_ERROR, "existing data=%s is different from %s\n",
			       (char*)dbpara.value, name_space);
			return -1;
		}
		return 0;
	}
	dbpara.atype=YANG_DB_ACTION_CREATE;
	dbpara.value=value;
	dbpara.vsize=vsize;
	return yang_db_action(dbald, NULL, &dbpara);
}

// this returns direct DB data, effective only in the same DB session
const char *yang_node_get_npath_namespace(uc_dbald *dbald, char *npath)
{
	uint8_t anums[UC_MAX_AP_DEPTH];
	int res;
	void *value;
	uint32_t vsize;
	res=yang_node_get_node_enums(dbald, npath, anums, UC_MAX_AP_DEPTH-2);
	if(res<0){
		UB_LOG(UBL_ERROR, "%s:invalid npath=%s\n", __func__, npath);
		return NULL;
	}
	anums[res]=2; // *_NSASC = 2
	anums[res+1]=255u;
	if(uc_dbal_get(dbald, anums, res+2, &value, &vsize)!=0){
		UB_LOG(UBL_DEBUGV, "%s:no registered namespace for %s\n", __func__, npath);
		return NULL;
	}
	return yang_node_get_prefix_namespace(dbald, (const char*)value);
}
