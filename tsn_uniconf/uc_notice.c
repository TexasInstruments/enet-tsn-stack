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
/*
 * Let's say 'uniconf client' which works with 'uniconf' like 'xmrpd'.
 * Abbreviate it as 'uc_client'.
 *
 * uc_* : functions called from 'uniconf' and 'uc_client'
 * uc_nu_* : functions called from 'uniconf'
 * uc_nc_* : functions called from 'uc_client'
 *
 ************************************************************************
 * Simple cases, no action is needed.
 *   'uc_client' -> 'uniconf': direct write on the DB
 *   'uc_client' <- 'uniconf': direct write on the DB
 *
 ************************************************************************
 * 'uc_client' asks actions(like HW configs) on 'uniconf'
 *   uc_nc_askaction_push: 'uc_client' writes on a notice in the notice area
 *                         (/XL4_DATA_RO/UC_ASKACTION_REG/[notice key]) of the DB
 *   uc_nc_askaction_push: 'uc_client' increments a shared semaphore(getnotice_sem)
 *   uc_nu_proc_asked_actions:
 *     'uniconf' is triggered by the semaphore
 *   uc_nu_proc_asked_actions:
 *     'uniconf' reads the notice from the notice area of the DB
 *     'uniconf' completes actions
 *     'uniconf' delete the notice in the notice area of the DB
 *   uc_nu_putnotice_push: push the notice to the other uc_client
 *
 * 'uniconf' internally had some resuls, and send a notice to registered 'uc_client'
 *   uc_nu_askaction_push
 *   the following process is the same as 'uc_nc_askaction_push'
 ************************************************************************
 * 'uc_client' asks notices from 'uniconf' by status updates
 *   uc_nc_notice_register:
 *     'uc_client' registers 'Asking Notice' in the registration
 *     area(/XL4_DATA_RO/UC_NOTICE_REG/aps,,,/kvs,,,/semname) of the DB.
 *     this in the initialization. the semaphore value start with 0
 *
 *   'uniconf' detects a status update
 *   'uniconf' updates the DB
 *   uc_nu_putnotice_push:
 *     'uniconf' checks the registration area(/XL4_DATA_RO/UC_NOTICE_REG) of the DB,
 *      get semname from the key.
 *      'uniconf' writes in the notice area(/XL4_DATA_RO/UC_NOTICE_ACT/aps,,,/kvs,,,/semname)
 *      of the DB
 *   uc_nu_putnotice_push:
 *     'uniconf' increments a shared semaphore(in putnotice_list) of the registered client
 *
 *   'uc_client' is triggered by the semaphore
 *   uc_nc_get_notice_act:
 *     'uc_client' reads the notice area of the DB and clears it, knows which key data is updated.
 *     'uc_client' reads the key data and knows the the updated data
 */

#include <errno.h>
#include "uc_notice.h"
#include "yangs/yang_modules.h"
#include "yangs/yang_node.h"

#define MAX_NOTICE_PUSH 1000

typedef struct putnotice_data {
	char *semname;
	UC_NOTICE_SIG_T *putnotice_sem;
	int actcounter;
	int refcounter;
}putnotice_data_t;

struct uc_notice_data {
	bool fromthread;
	UC_NOTICE_SIG_T *getnotice_sem;
	ub_esarray_cstd_t *putnotice_list;
	char semname[UC_SEMNAME_MAX];
};

enum {
	UC_ASKACTION_HW	= 0,
	UC_ASKACTION_NOHW,
};

/*
 * the number of pushing notice, which is estimated in UB_ESARRAY_DFNUM.
 * 'semname' and 'putnotice_sem' use it, set the fragment size is around half.
 */
UB_SD_GETMEM_DEF(UC_NOTICE_PUT, 20, (UB_ESARRAY_DFNUM*2));

static void delete_putsemaphore(bool fromthread, putnotice_data_t *pnd)
{
	if(!pnd){return;}
	uc_notice_sig_close(fromthread, pnd->putnotice_sem, true, pnd->semname);
	if(pnd->semname!=NULL){UB_SD_RELMEM(UC_NOTICE_PUT, pnd->semname);}
}

// this returns 2, when uc_client signals on '/xl4-data/uc-ready'
static int respond_getnotice(uc_notice_data_t *ucntd, uc_dbald *dbald, uc_hwald *hwald)
{
	uc_range *range;
	uint8_t key1[2]={XL4_DATA_RO, UC_ASKACTION_REG};
	uint8_t key2[2]={XL4_DATA_RO, UC_ASKACTION_REG+1};
	void *nkey;
	uint32_t nksize;
	int res;
	yang_db_access_para_t dbpara;
	uint8_t *caps;
	uint8_t ckss[UC_MAX_VALUEKEYS];
	kvs_t ckvs[UC_MAX_VALUEKEYS+1]; // +1 for NULL termination
	void *value;
	uint32_t vsize;
	uint8_t nv;
	// check in /XL4_DATA_RO/UC_ASKACTION_REG
	range=uc_get_range(dbald, key1, 2, key2, 2);
	if(!range){return 0;}
	res=0;
	while(true){
		if(uc_get_keyvalue_in_range(dbald, range, &nkey, &nksize,
					    &value, &vsize,
					    UC_DBAL_NOMOVE)!=0){
			break;
		}
		if(vsize!=1){continue;}
		nv=*((uint8_t*)value);
		// the registered key has 2-byte prefix
		if(yang_db_extract_key(&((uint8_t*)nkey)[2], nksize-2u, &caps, ckvs, ckss)!=0){
			continue;
		}
		(void)memset(&dbpara, 0, sizeof(dbpara));
		dbpara.atype=YANG_DB_ACTION_READ;
		dbpara.onhw=YANG_DB_ONHW_NOACTION;
		dbpara.aps=caps;
		dbpara.kvs=ckvs;
		dbpara.kss=ckss;
		yang_db_keydump_log(UBL_DEBUGV, dbald, dbpara.aps, dbpara.kvs, dbpara.kss);
		/* data in '/XL4_DATA_RO/UC_ASKACTION_REG' is pushed by a notice like
		 * 'YDBI_PUSH_NOTICE'.  Update or Create case, the data can be found,
		 * Delete case, the data can't be found.
		 * As the notice is deleted at the bottom of this loop, there shouldn't be
		 * any other cases as spurious notices.
		 */
		res=yang_db_action(dbald, hwald, &dbpara);
		if(res==0){
			if(nv==UC_ASKACTION_HW){
				if((dbpara.aps[0]==XL4_DATA_RO) && (dbpara.aps[1]==UC_READY)){
					UB_LOG(UBL_DEBUG, "%s:action on UC_READY, save DB\n",
					       __func__);
					res=2;
				}else{
					res=uc_hwal_writehw(hwald, dbpara.aps, dbpara.kvs,
							    dbpara.kss, dbpara.value,
							    dbpara.vsize);
				}
			}
			dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
			dbpara.onhw=YANG_DB_ONHW_NOACTION;
			(void)yang_db_action(dbald, hwald, &dbpara);
		}else{
			if(nv==UC_ASKACTION_HW){
				res=uc_hwal_dereghw(hwald, dbpara.aps, dbpara.kvs,
						    dbpara.kss);
			}
			yang_db_keydump_log(UBL_DEBUG, dbald, dbpara.aps, dbpara.kvs, dbpara.kss);
			UB_LOG(UBL_INFO, "%s:a notice with a deletion\n", __func__);
		}
		(void)uc_nu_putnotice_push(ucntd, dbald, caps, ckvs, ckss);
		yang_db_extract_key_free(caps, ckvs, ckss);
		if(res!=2){res=0;}
		if(uc_del_in_range(dbald, range,UC_DBAL_FORWARD)!=0){break;}
	}
	uc_get_range_release(dbald, range);
	return res;
}

static UC_NOTICE_SIG_T *create_new_putsemaphore(bool fromthread,
						ub_esarray_cstd_t *putnotice_list,
						const char *semname, bool create, int actcounter)
{
	putnotice_data_t pnd;
	int master=0;
	(void)memset(&pnd, 0, sizeof(pnd));
	if(fromthread){
		if(!create){
			UB_LOG(UBL_ERROR, "%s:create=true in thread mode", __func__);
			return NULL;
		}
		if(uc_notice_sig_open(fromthread, &pnd.putnotice_sem, &master, semname)!=0){
			return NULL;
		}
	}else{
		if(create){
			if(uc_notice_sig_open(fromthread, &pnd.putnotice_sem, &master, semname)!=0){
				return NULL;
			}
		}else{
			if(uc_notice_sig_open(fromthread, &pnd.putnotice_sem, NULL, semname)!=0){
				return NULL;
			}
		}
	}
	pnd.semname=(char*)UB_SD_GETMEM(UC_NOTICE_PUT, strlen(semname)+1u);
	if(ub_assert_fatal(pnd.semname!=NULL, __func__, NULL)){return NULL;}
	(void)memset(pnd.semname, 0, strlen(semname)+1u);
	pnd.actcounter=actcounter;
	pnd.refcounter=1;
	memcpy(pnd.semname, semname, strlen(semname)+1u);
	if(ub_esarray_add_ele(putnotice_list, (ub_esarray_element_t*)&pnd)!=0){
		UB_LOG(UBL_ERROR, "%s:no more space in putnotice_list\n", __func__);
		delete_putsemaphore(fromthread, &pnd);
		return NULL;
	}
	UB_LOG(UBL_DEBUG, "%s:new semname=%s is created\n", __func__, semname);
	return pnd.putnotice_sem;
}

typedef enum {
	PUTNOTICE_FIND=0,
	PUTNOTICE_INC, // increment refcounter
	PUTNOTICE_INC_ADD,  // increment refcounter, if it doesn't exist create it
	PUTNOTICE_DEC, // decrement refcounter
	PUTNOTICE_DEC_DELETE, // decrement refcounter, if it becomes 0 delete it
	PUTNOTICE_FORCE_DELETE // delete anyway
} putnotice_action_t;
// return 0:action done, 1:action not done, -1:error
static int find_semname_in_putnoticelist(uc_notice_data_t *ucntd, const char *semname,
					 putnotice_action_t action, UC_NOTICE_SIG_T **sem)
{
	putnotice_data_t *pnd=NULL;
	int i;
	int res=1;
	char *msg="not found";
	int refcounter=-1;;
	for(i=0; i<ub_esarray_ele_nums(ucntd->putnotice_list); i++){
		pnd=(putnotice_data_t*)ub_esarray_get_ele(ucntd->putnotice_list, i);
		if(ub_assert_fatal(pnd!=NULL, __func__, "pnd is NULL")){return -1;}
		if(strcmp(pnd->semname, semname)==0){
			switch(action){
			case PUTNOTICE_FIND:
				msg="found";
				if(sem){*sem=pnd->putnotice_sem;}
				res=0;break;
			case PUTNOTICE_INC:
			case PUTNOTICE_INC_ADD:
				msg="found_inc";
				pnd->refcounter++;
				refcounter=pnd->refcounter;
				if(sem){*sem=pnd->putnotice_sem;}
				res=0;break;
			case PUTNOTICE_DEC:
				msg="found_dec";
				pnd->refcounter--;
				refcounter=pnd->refcounter;
				res=0;break;
			case PUTNOTICE_DEC_DELETE:
				msg="found_ddec";
				pnd->refcounter--;
				refcounter=pnd->refcounter;
				if(pnd->refcounter==0){
					delete_putsemaphore(ucntd->fromthread, pnd);
					(void)ub_esarray_del_index(ucntd->putnotice_list, i);
					msg="deleted";
				}
				res=0;break;
			case PUTNOTICE_FORCE_DELETE:
				pnd->refcounter--;
				refcounter=pnd->refcounter;
				delete_putsemaphore(ucntd->fromthread, pnd);
				(void)ub_esarray_del_index(ucntd->putnotice_list, i);
				msg="force deleted";
				res=0;break;
			default:
				res=-1;break;
			}
		}
	}
	if((res==1) && (action==PUTNOTICE_INC_ADD)){
		if(sem){
			*sem=create_new_putsemaphore(ucntd->fromthread, ucntd->putnotice_list,
						     semname, true, 0);
			refcounter=1;
			res=0;
		}else{
			UB_LOG(UBL_ERROR, "%s:need to add new semaphore, but no return value\n",
			       __func__);
			res=-1;
		}

	}
	UB_LOG(UBL_DEBUG, "%s:semname=\"%s\", refcount=%d %s\n",
	       __func__, semname, refcounter, msg);

#if !UB_LOG_IS_COMPILED(UBL_DEBUG)
	(void)msg;
	(void)refcounter;
#endif
	return res;
}

static int find_semname_in_db(uc_dbald *dbald, const char *semname, bool delete_flg)
{
	uc_range *range;
	uint8_t key1[2]={XL4_DATA_RO, UC_NOTICE_REG};
	uint8_t key2[2]={XL4_DATA_RO, UC_NOTICE_REG+1};
	uint8_t *nkey;
	uint32_t nksize;
	uint32_t snsize=strlen(semname);
	int count=0;
	void *vdata;
	uint32_t vsize;
	uint8_t action;
	range=uc_get_range(dbald, key1, 2, key2, 2);
	if(!range){return -1;}
	while(true){
		if(uc_get_keyvalue_in_range(dbald, range, (void**)&nkey,
					    &nksize, &vdata, &vsize,
					    UC_DBAL_FORWARD)!=0){
			break;
		}
		if((nksize>snsize+2) && (nkey[nksize-(snsize+2u)]==(snsize+1u)) &&
		   !memcmp(&nkey[nksize-(snsize+1u)], semname, snsize) &&
		   (vsize==sizeof(uint8_t))){
			action=*((uint8_t*)vdata);
			if(action!=UC_NOTICE_DBVAL_ADD){continue;}
			count++;
			if(delete_flg){
				UB_LOG(UBL_DEBUG,
				       "%s:mark delete semname in the DB:%s\n",
				       __func__, semname);
				action=UC_NOTICE_DBVAL_DEL;
				if(uc_dbal_create(dbald, nkey, nksize,
						  &action, vsize)!=0){
					UB_LOG(UBL_ERROR, "%s:can't delete\n", __func__);
				}
			}
		}
	}
	uc_get_range_release(dbald, range);
	return count;
}

// return -2 case, the semaphore doesn't exist and it is likely zombie semname.
// in the process mode, a new semaphore may be created.
static int incremant_actcounter(bool fromthread,
				ub_esarray_cstd_t *putnotice_list, char *semname)
{
	int i, en;
	putnotice_data_t *pnd;
	en=ub_esarray_ele_nums(putnotice_list);
	for(i=0;i<en;i++){
		pnd=(putnotice_data_t*)ub_esarray_get_ele(putnotice_list, i);
		if(ub_assert_fatal(pnd!=NULL, __func__, "pnd is NULL")){return -1;}
		if(!strcmp(pnd->semname, semname)) {
			pnd->actcounter+=1;
			break;
		}
	}
	if(i<en){return 0;}
	// semaphore is not yet created, it is the process case only
	if(fromthread){
		UB_LOG(UBL_ERROR,
		       "%s:'uniconf' thread should already have sem:%s\n",
		       __func__, semname);
		return -1;
	}
	if(!create_new_putsemaphore(fromthread, putnotice_list, semname, false, 1)){
		return -2;
	}
	return 0;
}

static void proc_refactcounter(bool fromthread, ub_esarray_cstd_t *putnotice_list)
{
	int i, en;
	putnotice_data_t *pnd;
	en=ub_esarray_ele_nums(putnotice_list);
	for(i=0;i<en;i++){
		pnd=(putnotice_data_t*)ub_esarray_get_ele(putnotice_list, i);
		if(ub_assert_fatal(pnd!=NULL, __func__, "pnd is NULL")){return;}
		// the waiting process is responsible to handle multiple updates
		// with one event.  Here we post one time instead of pnd->actcounter times
		if(pnd->refcounter==0){
			UB_LOG(UBL_DEBUG, "%s:%s refcounter=0, delete this\n", __func__,
			       pnd->semname);
			ub_esarray_del_index(putnotice_list, i);
			i--;
			en--;
			continue;
		}
		if(pnd->actcounter!=0){
			UB_LOG(UBL_DEBUG, "%s:%s actcounter=%d\n", __func__,
			       pnd->semname, pnd->actcounter);
			if(uc_notice_sig_post(fromthread, pnd->putnotice_sem)!=0){
				UB_LOG(UBL_ERROR, "%s:error in sem_post, %s\n", __func__,
				       strerror(errno));
			}
			pnd->actcounter=0;
		}
	}
	return;
}

static void nu_clear_refcounter(bool fromthread, ub_esarray_cstd_t *putnotice_list)
{
	int i, en;
	putnotice_data_t *pnd;
	en=ub_esarray_ele_nums(putnotice_list);
	for(i=0;i<en;i++){
		pnd=(putnotice_data_t*)ub_esarray_get_ele(putnotice_list, i);
		if(ub_assert_fatal(pnd!=NULL, __func__, "pnd is NULL")){return;}
		pnd->refcounter=0;
		UB_LOG(UBL_DEBUGV, "%s:%s cleared\n", __func__, pnd->semname);
	}
	return;
}

static void nu_increment_refcounter(bool fromthread,
				   ub_esarray_cstd_t *putnotice_list, char *semname)
{
	int i, en;
	putnotice_data_t *pnd;
	en=ub_esarray_ele_nums(putnotice_list);
	for(i=0;i<en;i++){
		pnd=(putnotice_data_t*)ub_esarray_get_ele(putnotice_list, i);
		if(ub_assert_fatal(pnd!=NULL, __func__, "pnd is NULL")){return;}
		if(!strcmp(pnd->semname, semname)){
			pnd->refcounter++;
			UB_LOG(UBL_DEBUGV, "%s:semname=%s, refcounter=%d\n", __func__,
			       semname, pnd->refcounter);
			break;
		}
	}
	return;
}

static uc_notice_data_t sucntd;
static CB_THREAD_MUTEX_T CB_STATIC_MUTEX_INITIALIZER(ntmutex);
static int create_semname_with_dbname(char *semname, const char *dbname)
{
	const char *dnp;
	if (!dbname){
		UB_LOG(UBL_INFO, "%s:null dbname is specified.\n", __func__);
		return 0;
	}
	dnp=strrchr(dbname, '/');
	if(dnp==NULL){
		dnp=dbname;
	}else{
		dnp=&dnp[1];
	}
	if(strlen(dnp)>=UC_SEMNAME_MAX-strlen(UC_GETNOTICE_SEM)){
		UB_LOG(UBL_ERROR, "%s:dbname is too long\n", __func__);
		return -1;
	}
	snprintf(semname, UC_SEMNAME_MAX, "%s%s", UC_GETNOTICE_SEM, dnp);
	return 0;
}

uc_notice_data_t *uc_notice_init(uint8_t callmode, const char *dbname)
{
	int master=0;
	int res=-1;
	if(create_semname_with_dbname(sucntd.semname, dbname)!=0){return NULL;}
	CB_STATIC_MUTEX_CONSTRUCTOR(ntmutex);
	CB_THREAD_MUTEX_LOCK(&ntmutex);
	if(UC_CALL_UNICONF(callmode)!=0u){
		// from 'uniconf'
		sucntd.fromthread=(UC_CALL_THREAD(callmode)!=0u);
		if(uc_notice_sig_open(sucntd.fromthread,
				      &sucntd.getnotice_sem, &master, sucntd.semname)!=0){
			goto erexit;
		}
	}else{
		// from 'uc_client'
		if(UC_CALL_THREAD(callmode)!=0u){
			UB_TLOG(UBL_DEBUG,"%s:uc_client,thread\n", __func__);
			if(sucntd.fromthread){
				// this means 'uniconf' side has already initialized.
				// 'uc_client' side can go ahead with the shared instance.
				res=0;
				goto erexit;
			}
			UB_LOG(UBL_ERROR, "%s:'uniconf' side should call first\n",
			       __func__);
			goto erexit;
		}
		// called from a process, initialize a new instance
		sucntd.fromthread=false;
		if(uc_notice_sig_open(sucntd.fromthread,
				      &sucntd.getnotice_sem, NULL, sucntd.semname)!=0){
			goto erexit;
		}
	}
	sucntd.putnotice_list=ub_esarray_init(4, sizeof(putnotice_data_t), 100);
	res=0;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&ntmutex);
	if(res!=0){
		uc_notice_close(&sucntd, callmode);
		return NULL;
	}
	return &sucntd;

}

int uc_notice_start_events_thread(uc_notice_data_t *ucntd, uc_hwald *hwald)
{
	if(hwald==NULL){return 0;}
	return uc_hwal_catch_events_thread(hwald, (CB_SEM_T *)ucntd->getnotice_sem);
}

void uc_notice_close(uc_notice_data_t *ucntd, uint8_t callmode)
{
	int i, en=0;
	putnotice_data_t *pnd;
	if(!ucntd){return;}
	if(UC_CALL_CLIENT(callmode) && ucntd->fromthread){return;} // 'uc_client' and thread

	CB_THREAD_MUTEX_LOCK(&ntmutex);
	if(ucntd->putnotice_list!=NULL){
		en=ub_esarray_ele_nums(ucntd->putnotice_list);
		for(i=0;i<en;i++){
			pnd=(putnotice_data_t*)ub_esarray_get_ele(ucntd->putnotice_list, i);
			delete_putsemaphore(ucntd->fromthread, pnd);
		}
		ub_esarray_close(ucntd->putnotice_list);
		ucntd->putnotice_list=NULL;
	}
	uc_notice_sig_close(ucntd->fromthread, ucntd->getnotice_sem,
				    UC_CALL_UNICONF(callmode), ucntd->semname);
	ucntd->getnotice_sem=NULL;
	ucntd->fromthread=false;
	CB_THREAD_MUTEX_UNLOCK(&ntmutex);
	CB_STATIC_MUTEX_DESTRUCTOR(ntmutex);
	return;
}

static int askaction_push(uc_notice_data_t *ucntd, uc_dbald *dbald,
			  uint8_t *aps, void **kvs, uint8_t *kss, bool from_nc)
{
	int sval;
	uint8_t preap[3]={XL4_DATA_RO, UC_ASKACTION_REG, 255};
	uint8_t d;
	yang_db_access_para_t dbpara={0};
	int res=-1;
	d=from_nc?UC_ASKACTION_HW:UC_ASKACTION_NOHW;
	dbpara.atype=YANG_DB_ACTION_CREATE;
	dbpara.onhw=YANG_DB_ONHW_NOACTION;
	dbpara.paps=preap;
	dbpara.aps=aps;
	dbpara.kvs=kvs;
	dbpara.kss=kss;
	dbpara.value=&d;
	dbpara.vsize=1;
	(void)yang_db_action(dbald, NULL, &dbpara);
	uc_dbal_releasedb(dbald);
	CB_THREAD_MUTEX_LOCK(&ntmutex);
	if(uc_notice_sig_getvalue(ucntd->fromthread, ucntd->getnotice_sem, &sval)==0){
		if(sval==0){
			(void)uc_notice_sig_post(ucntd->fromthread, ucntd->getnotice_sem);
		}
		res=0;
	}
	CB_THREAD_MUTEX_UNLOCK(&ntmutex);
	return res;
}

// uc_client -> uniconf
// a key is pushed to /XL4_DATA_RO/UC_ASKACTiON_REG/key, and signal the semaphore
int uc_nc_askaction_push(uc_notice_data_t *ucntd, uc_dbald *dbald,
			 uint8_t *aps, void **kvs, uint8_t *kss)
{
	return askaction_push(ucntd, dbald, aps, kvs, kss, true);
}

int uc_nu_askaction_push(uc_notice_data_t *ucntd, uc_dbald *dbald,
			 uint8_t *aps, void **kvs, uint8_t *kss)
{
	return askaction_push(ucntd, dbald, aps, kvs, kss, false);
}

// 'uniconf' checks the semaphore and process notices to update HW configs.
// tout_ms=-1:wait forever, 0:no timeout, >0:msec timeout
int uc_nu_proc_asked_actions(uc_notice_data_t *ucntd,
			     uc_dbald *dbald, uc_hwald *hwald, int tout_ms)
{
	int res;
	uc_dbal_releasedb(dbald);
	res=uc_notice_sig_check(ucntd->fromthread, ucntd->getnotice_sem, tout_ms, __func__);
	if(res==0){
		if(hwald){
			res=uc_hwal_detect_notice(hwald, ucntd);
			if(res<0){return res;}
		}
		// ntmutex is locked inside respond_getnotice
		UB_TLOG(UBL_DEBUG, "%s:got a signal\n", __func__);
		res=respond_getnotice(ucntd, dbald, hwald);
	}
	return res;
}

/*
 * uc_client asks to be noticed by uniconf.  called only in 'uc_client'
 * the last kvs element must be 'semname' then NULL: kvs={,,,semname,NULL}
 * [XL4_DATA_RO, UC_NOTICE_REG, aps, kvs, semname] becomes the key
 * the value is set by notice_register_action_t action.
 * for UC_NOTICE_DBVAL_ADD, a new semaphore may be created.
 * the created semaphore is registered in putnotice_list, and the DB is updated.
 * To share one semaphore by mulitiple db items, refcounter is used.
 * for UC_NOTICE_DBVAL_DEL, 'sem' can be NULL
 */
int uc_nc_notice_register(uc_notice_data_t *ucntd, uc_dbald *dbald,
			  uint8_t *aps, void **kvs, uint8_t *kss,
			  notice_register_value_t regval, UC_NOTICE_SIG_T **sem)
{
	int i, res;
	yang_db_access_para_t dbpara;
	int snindex=-1;
	uint8_t preap[3]={XL4_DATA_RO, UC_NOTICE_REG, 255};
	putnotice_action_t paction;
	uint8_t dbval;
	if(!aps || !kvs || !kss){return -1;}
	for(i=0;;i++) {
		if(kvs[i]==NULL){snindex=i-1; break;}
	}
	if(snindex<0){return -1;}
	if((strlen((const char*)kvs[snindex])+1u) != kss[snindex]){
		UB_LOG(UBL_ERROR, "%s:the last element in kvs must be a semaphore name\n",
		       __func__);
		return -1;
	}
	switch(regval){
	case UC_NOTICE_DBVAL_ADD:
		paction=PUTNOTICE_INC_ADD;
		dbpara.atype=YANG_DB_ACTION_CREATE;
		break;
	case UC_NOTICE_DBVAL_DEL:
		paction=PUTNOTICE_DEC_DELETE;
		dbpara.atype=YANG_DB_ACTION_DELETE;
		break;
	default:
		return -1;
	}

	CB_THREAD_MUTEX_LOCK(&ntmutex);
	res=find_semname_in_putnoticelist(ucntd, (const char*)kvs[snindex], paction, sem);
	CB_THREAD_MUTEX_UNLOCK(&ntmutex);
	if(res!=0){return -1;}
	if(!aps){return 0;} // only semaphore registration.
	(void)memset(&dbpara, 0, sizeof(dbpara));
	dbpara.onhw=YANG_DB_ONHW_NOACTION;
	dbpara.paps=preap;
	dbpara.aps=aps;
	dbpara.kvs=kvs;
	dbpara.kss=kss;
	dbval=regval;
	dbpara.value=&dbval;
	dbpara.vsize=1;
	res=yang_db_action(dbald, NULL, &dbpara);
	UB_TLOG(UBL_DEBUG, "%s:register semname='%s' to the DB, res=%d\n",
		__func__, (char*)kvs[snindex], res);
	return 0;
}

// mark UC_NOTICE_REG_REMOVE on all the registered items with 'semname'
int uc_nc_notice_deregister_all(uc_notice_data_t *ucntd, uc_dbald *dbald,
				const char *semname)
{
	int res=-1;
	if(!dbald || !ucntd) return -1;
	CB_THREAD_MUTEX_LOCK(&ntmutex);
	if(find_semname_in_putnoticelist(ucntd, semname, PUTNOTICE_FORCE_DELETE, NULL)<0){
		goto erexit;
	}
	res=find_semname_in_db(dbald, semname, true);
	UB_LOG(UBL_DEBUG, "%s:deleted %d of items in DB\n", __func__, res);
	res=0;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&ntmutex);
	return res;
}

// uniconf -> uc_client
int uc_nu_putnotice_push(uc_notice_data_t *ucntd, uc_dbald *dbald,
			 uint8_t *aps, void **kvs, uint8_t *kss)
{
	uint8_t key[UC_MAX_KEYSIZE];
	uint32_t ksize;
	uc_range *range;
	uint8_t key1[2]={XL4_DATA_RO, UC_NOTICE_REG};
	uint8_t key2[2]={XL4_DATA_RO, UC_NOTICE_REG+1};
	uint8_t *nkey;
	uint8_t rkey[UC_MAX_KEYSIZE];
	char *semname;
	uint32_t nksize;
	int res=-1;
	uint32_t csize;
	uint8_t d=0;
	void *vdata;
	uint32_t vsize;
	uint8_t action=0xff;
	// scan all in /XL4_DATA_RO/UC_NOTICE_REG
	// get semname from the bottom at the key, then get semaphre and signal on it
	range=uc_get_range(dbald, key1, 2, key2, 2);
	if(!range){
		UB_TLOG(UBL_DEBUG, "%s:no data in UC_NOTICE_REG\n", __func__);
		return 0;
	}
	ksize=yang_db_create_key(NULL, aps, kvs, kss, key);
	CB_THREAD_MUTEX_LOCK(&ntmutex);
	nu_clear_refcounter(ucntd->fromthread, ucntd->putnotice_list);
	while(ksize>0u){
		if(action!=0xff){
			if(uc_move_in_range(dbald, range, UC_DBAL_FORWARD)){break;}
		}
		if(uc_get_keyvalue_in_range(dbald, range, (void**)&nkey, &nksize,
					    &vdata, &vsize, UC_DBAL_NOMOVE)!=0){break;}
		if(vsize!=sizeof(uint8_t)){break;}
		// the registered key has 2-byte prefix and 'semname' at the end
		// the end 'semname' is '1(size byte)+strlen+1'='semname[0]+1'
		action=*((uint8_t*)vdata);
		if(action==UC_NOTICE_DBVAL_DEL){
			if(uc_del_in_range(dbald, range, UC_DBAL_FORWARD)){break;}
			action=0xff;
			continue;
		}
		semname=(char*)yang_db_key_bottomp(nkey, nksize);
		if(!semname){continue;}
		nu_increment_refcounter(ucntd->fromthread, ucntd->putnotice_list,
					&semname[1]);
		csize=nksize-2u-((uint32_t)semname[0]+1u);
		if(csize>ksize){continue;}
		if(memcmp(&nkey[2], key, csize)!=0){continue;}
		if(nksize>(uint32_t)UC_MAX_KEYSIZE){goto erexit;}
		memcpy(rkey, nkey, nksize);
		// 'nkey' data may be changed in the next DB action(uc_dbal_create),
		// the same 'semname' should be in the copied contents.
		semname=(char*)yang_db_key_bottomp(rkey, nksize);
		// ceate a new key: /XL4_DATA_RO/UC_NOTICE_ACT/aps,,,/kvs,,,/semname
		// each cient has a different 'semname', this key becomes unique for the client
		rkey[1]=UC_NOTICE_ACT;
		(void)uc_dbal_create(dbald, rkey, nksize, &d, 1);
		semname=&semname[1]; // the first position is the size, skip it
		UB_LOG(UBL_DEBUG, "%s:action on semname:%s\n", __func__, semname);
		res=incremant_actcounter(ucntd->fromthread, ucntd->putnotice_list, semname);
		if(res==-2){
			UB_LOG(UBL_INFO, "%s:delete likely zombie semname:%s\n",
			       __func__, semname);
			(void)uc_del_in_range(dbald, range, UC_DBAL_FORWARD);
			action=0xff;
		}
	}
	proc_refactcounter(ucntd->fromthread, ucntd->putnotice_list);
	res=0;
erexit:
	uc_get_range_release(dbald, range);
	CB_THREAD_MUTEX_UNLOCK(&ntmutex);
	return res;
}

// uc_client scans in /XL4_DATA_RO/UC_NOTICE_ACT/.../semname
int uc_nc_get_notice_act(uc_notice_data_t *ucntd, uc_dbald *dbald, const char *semname,
			 void *key, uint32_t *ksize)
{
	uc_range *range;
	uint8_t key1[2]={XL4_DATA_RO, UC_NOTICE_ACT};
	uint8_t key2[2]={XL4_DATA_RO, UC_NOTICE_ACT+1};
	uint8_t *nkey;
	uint32_t nksize;
	char *actsemname;
	bool found=false;
	range=uc_get_range(dbald, key1, 2, key2, 2);
	if(!range){
		UB_TLOG(UBL_DEBUG, "%s:no data in UC_NOTICE_ACT\n", __func__);
		return 1;
	}
	while(true){
		if(uc_get_key_in_range(dbald, range, (void**)&nkey,
				       &nksize, UC_DBAL_FORWARD)!=0){break;}
		actsemname=(char*)yang_db_key_bottomp(nkey, nksize);
		if(actsemname && !strcmp(&actsemname[1], semname)){
			found=true;
			break;
		}
	}
	if(found){
		*ksize=nksize-2u-(uint32_t)actsemname[0]-1u;
		if(*ksize>(uint32_t)UC_MAX_KEYSIZE){
			found=false;
		}else{
			memcpy(key, &nkey[2], *ksize);
		}
	}
	uc_get_range_release(dbald, range);
	if(!found){return -1;}
	// 'nkey' contentes have no change by 'uc_get_range_release'.
	// It must be true.  Check the DB implementation.
	(void)uc_dbal_del(dbald, nkey, nksize);
	return 0;
}
