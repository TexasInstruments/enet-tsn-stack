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
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include "tsn_data.h"
#include "yang_modules.h"
#include "yang_db_access.h"
#include "../uc_notice.h"

#define MAX_XL4EXTMOD_NAME 16
#define XL4DATA_DATALOCK "/xl4data_datalock"

typedef struct xl4extmod_data{
	char modname[MAX_XL4EXTMOD_NAME];
	get_string_func get_string;
	get_enum_func get_enum;
	uint8_t modid;
} xl4extmod_data_t;

struct xl4_data_data{
	ub_esarray_cstd_t *xl4mods;
	uc_dbald *dbald;
	UC_NOTICE_SIG_T *datalock;
	bool datalock_created;
	CB_THREAD_MUTEX_T dmutex;
	int refcounter;
};

const char *xl4_data_enum_strings[XL4_DATA_ENUM_END]={
	"yang-value-types",
	"uc-ready",
	"uc-askaction-reg",
	"uc-notice-reg",
	"uc-notice-act",
};

/* generally used statically allocated memory.
   this should be used only for quickly released cases. */
UB_SD_GETMEM_DEF(YANGINIT_GEN_SMEM, 8, 320);

static xl4extmod_data_t *find_xl4emd(ub_esarray_cstd_t *xl4mods, char *emodname, uint8_t emodid)
{
	int i, en;
	xl4extmod_data_t *xl4emd;
	en=ub_esarray_ele_nums(xl4mods);
	for(i=0;i<en;i++){
		xl4emd=(xl4extmod_data_t *)ub_esarray_get_ele(xl4mods, i);
		if(ub_assert_fatal(xl4emd != NULL, __func__, NULL)){return NULL;}
		if((emodname!=NULL) && !strcmp(xl4emd->modname, emodname)){return xl4emd;}
		if((emodid!=0xffu) && (xl4emd->modid==emodid)){return xl4emd;}
	}
	return NULL;
}

/* the instace is only 1 */
static xl4_data_data_t sxdd;
static CB_THREAD_MUTEX_T CB_STATIC_MUTEX_INITIALIZER(xdmutex);
xl4_data_data_t *xl4_data_init(uc_dbald *dbald)
{
	int master=1;
	CB_STATIC_MUTEX_CONSTRUCTOR(xdmutex);
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	UB_LOG(UBL_DEBUG, "%s:refcounter=%d\n", __func__, sxdd.refcounter);
	if(sxdd.refcounter>0){
		sxdd.refcounter+=1;
		CB_THREAD_MUTEX_UNLOCK(&xdmutex);
		return &sxdd;
	}
	sxdd.refcounter=1;
	sxdd.xl4mods=ub_esarray_init(1, sizeof(xl4extmod_data_t), 100);
	sxdd.dbald=dbald;
	if(uc_notice_sig_open(false, &sxdd.datalock, NULL, XL4DATA_DATALOCK)!=0){
		if(uc_notice_sig_open(false, &sxdd.datalock, &master, XL4DATA_DATALOCK)!=0){
			CB_THREAD_MUTEX_UNLOCK(&xdmutex);
			xl4_data_close(&sxdd);
			UB_LOG(UBL_ERROR, "%s:can't initialize\n", __func__);
			return NULL;
		}
	}
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	return &sxdd;
}

void xl4_data_set_dbald(xl4_data_data_t *xdd, uc_dbald *dbald)
{
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xdd->dbald=dbald;
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
}

void xl4_data_close(xl4_data_data_t *xdd)
{
	if(!xdd){return;}
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xdd->refcounter--;
	UB_LOG(UBL_DEBUG, "%s:refcounter=%d\n", __func__, xdd->refcounter);
	if(xdd->refcounter==0){
		ub_esarray_close(xdd->xl4mods);
		uc_notice_sig_close(false, xdd->datalock,
				    xdd->datalock_created, XL4DATA_DATALOCK);
	}
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
}

int xl4_data_lock(xl4_data_data_t *xdd, int tout_ms)
{
	return uc_notice_sig_check(false, xdd->datalock, tout_ms, __func__);
}

void xl4_data_unlock(xl4_data_data_t *xdd)
{
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	if(xdd->dbald!=NULL){uc_dbal_releasedb(xdd->dbald);}
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	(void)uc_notice_sig_post(false, xdd->datalock);
}

uint8_t xl4_data_get_enum(char *astr)
{
	int i;
	for(i=0;i<XL4_DATA_ENUM_END;i++){
		if(!strcmp(astr, xl4_data_enum_strings[i])){return i;}
	}
	return 0xff;
}

get_enum_func xl4_extmod_get_enum_func(xl4_data_data_t *xdd, char *emodname, uint8_t emodid)
{
	xl4extmod_data_t *xl4emd;
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xl4emd=find_xl4emd(xdd->xl4mods, emodname, emodid);
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	if(xl4emd!=NULL){return xl4emd->get_enum;}
	return NULL;
}

const char *xl4_data_get_string(uint8_t anum)
{
	if(anum>=XL4_DATA_ENUM_END){return NULL;}
	return xl4_data_enum_strings[anum];
}

get_string_func xl4_extmod_get_string_func(xl4_data_data_t *xdd, char *emodname, uint8_t emodid)
{
	xl4extmod_data_t *xl4emd;
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xl4emd=find_xl4emd(xdd->xl4mods, emodname, emodid);
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	if(xl4emd!=NULL){return xl4emd->get_string;}
	return NULL;
}

uint8_t xl4_data_get_modid(xl4_data_data_t *xdd, char *emodname)
{
	xl4extmod_data_t *xl4emd;
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xl4emd=find_xl4emd(xdd->xl4mods, emodname, 0xff);
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	if(xl4emd!=NULL){return xl4emd->modid;}
	return 0xff;
}

char *xl4_data_get_modname(xl4_data_data_t *xdd, uint8_t emodid)
{
	xl4extmod_data_t *xl4emd;
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xl4emd=find_xl4emd(xdd->xl4mods, NULL, emodid);
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	if(xl4emd!=NULL){return xl4emd->modname;}
	return NULL;
}

#define XL4_EXTMOD_REG_NEXTID 0
static int get_exmodid_in_db(xl4_data_data_t *xdd, char *emodname)
{
	yang_db_access_para_t dbpara;
	uint8_t aps[4];
	void *end=NULL;
	void *kvs[2];
	uint8_t kss[1];
	uint32_t vsize=0;
	int res=-1;
	uint8_t exmodid;

	if(!xdd->dbald){return -1;}
	res=yang_value_conv(YANG_VTYPE_STRING, emodname,
			    &end, &vsize, NULL);
	if(res<0){return -1;}
	kss[0]=res;
	kvs[0]=end;
	kvs[1]=NULL;
	aps[0]=XL4_DATA_RW;
	aps[1]=XL4_EXTMOD_REG;
	aps[2]=255u;
	(void)memset(&dbpara, 0, sizeof(dbpara));
	YANG_DB_READ_SETUP(dbpara, aps, kvs, kss);
	if(!yang_db_action(xdd->dbald, NULL, &dbpara)){
		exmodid=*((uint8_t *)dbpara.value);
		dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		(void)yang_db_action(xdd->dbald, NULL, &dbpara);
		UB_LOG(UBL_INFO, "%s:get %s:exmodid from db, id=%d\n",
		       __func__, emodname, exmodid);
		res=exmodid;
		goto erexit;
	}
	kvs[0]=NULL;
	aps[2]=(uint8_t)XL4_EXTMOD_REG_NEXTID;
	aps[3]=255u;
	if(!yang_db_action(xdd->dbald, NULL, &dbpara)){
		exmodid=*((uint8_t *)dbpara.value);
		dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		(void)yang_db_action(xdd->dbald, NULL, &dbpara);
		UB_LOG(UBL_INFO, "%s:a new %s:exmodid=%d\n",
		       __func__, emodname, exmodid);
	}else{
		exmodid=0u;
		UB_LOG(UBL_INFO, "%s:first %s:exmodid=0\n",
		       __func__, emodname);
	}
	exmodid+=1u;
	dbpara.atype=YANG_DB_ACTION_CREATE;
	dbpara.value=&exmodid;
	dbpara.vsize=1;
	if(yang_db_action(xdd->dbald, NULL, &dbpara)!=0){
		UB_LOG(UBL_ERROR, "%s:can't update NEXTID in db, emodname=%s\n",
		       __func__, emodname);
		goto erexit;
	}
	kvs[0]=end;
	kvs[1]=NULL;
	aps[2]=255u;
	exmodid-=1u;
	res=exmodid;
	if(yang_db_action(xdd->dbald, NULL, &dbpara)!=0){
		UB_LOG(UBL_ERROR, "%s:can't update EXTMOD_REG in db, emodname=%s\n",
		       __func__, emodname);
		res=-1;
	}
erexit:
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, end);
	return res;
}

int xl4_data_set_xl4ext(xl4_data_data_t *xdd, char *emodname,
			get_enum_func get_enum, get_string_func get_string)
{
	int res=-1;;
	xl4extmod_data_t *xl4emd;
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xl4emd=(xl4extmod_data_t *)ub_esarray_get_newele(xdd->xl4mods);
	if(!xl4emd){
		UB_LOG(UBL_ERROR, "%s:can't set\n", __func__);
		goto erexit;
	}
	res=snprintf(xl4emd->modname, MAX_XL4EXTMOD_NAME, "%s", emodname);
	if(res!=(int)strlen(emodname)){
		UB_LOG(UBL_ERROR, "%s:wrong name:%s\n", __func__, emodname);
		(void)ub_esarray_del_ele(xdd->xl4mods, (ub_esarray_element_t*)xl4emd);
		goto erexit;
	}
	xl4emd->get_enum=get_enum;
	xl4emd->get_string=get_string;
	res=get_exmodid_in_db(xdd, emodname);
	if(res>=0){
		xl4emd->modid=res;
	}
erexit:
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	return res;

}

int xl4_data_remove_xl4ext(xl4_data_data_t *xdd, char *emodname)
{
	xl4extmod_data_t *xl4emd;
	int res=-1;
	CB_THREAD_MUTEX_LOCK(&xdmutex);
	xl4emd=find_xl4emd(xdd->xl4mods, emodname, 0xff);
	if(!xl4emd){
		UB_LOG(UBL_ERROR, "%s:doesn't exist:%s\n", __func__, emodname);
	}else{
		res=ub_esarray_del_pointer(xdd->xl4mods, (ub_esarray_element_t*)xl4emd);
	}
	CB_THREAD_MUTEX_UNLOCK(&xdmutex);
	return res;
}
