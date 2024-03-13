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
#include <tsn_combase/cb_tmevent.h>
#include "yangs/yang_db_runtime.h"
#include "uc_private.h"
#include "ucman.h"
#include "yangs/yang_modules.h"
#include "uniconfmon_thread.h"
#include "yangs/yang_node.h"

extern int yang_config_init(uc_dbald *dbald, uc_hwald *hwald);
extern int yang_nconf_config_init(uc_dbald *dbald);

void *uniconf_main(void *ptr)
{
	int i,res;
	uc_data_t ucd;
	ucman_data_t *ucmd=(ucman_data_t *)ptr;
	UC_RUNCONF_DATA *ydrd;
	uint8_t apsd[5]={XL4_DATA_RW, YANG_VALUE_TYPES, XL4_DATA_RO, UC_READY, 255};
	uint8_t vtype=YANG_VTYPE_UINT8;
	uint8_t *uc_rap=&apsd[2];
	uint8_t uc_rv=1;

	(void)memset(&ucd, 0, sizeof(ucd));
	ucmd->rval=-1;
	ucd.dbald=uc_dbal_open(ucmd->dbname, "w", ucmd->ucmode);
	if(!ucd.dbald){
		UB_LOG(UBL_ERROR, "%s:can't open the db\n", __func__);
		goto erexit;
	}
	yang_node_uniconf_init(ucd.dbald);
	uniconf_cleanup_status(ucd.dbald);
	if(!ucmd->hwmod[0]){
		ucd.hwald=uc_hwal_open(ucd.dbald);
	}else if(!strcmp(ucmd->hwmod, "NONE")){
		ucd.hwald=NULL;
	}else{
		UB_LOG(UBL_ERROR, "%s:hwmod=%s is not yet supported\n",
		       __func__, ucmd->hwmod);
		goto erexit;
	}
	if(strcmp(ucmd->hwmod, "NONE") && !ucd.hwald){
		UB_LOG(UBL_ERROR, "%s:can't open the HW\n", __func__);
		goto erexit;
	}

	if(yang_config_init(ucd.dbald, ucd.hwald)!=0){goto erexit;}
	if(yang_nconf_config_init(ucd.dbald)!=0){goto erexit;}
	ydrd=UC_RUNCONF_INIT(ucd.dbald, ucd.hwald);
	if(!ydrd){goto erexit;}
	for(i=0;i<ucmd->numconfigfile;i++){
		if(UC_RUNCONF_READFILE(ydrd, ucmd->configfiles[i])!=0){
			UB_LOG(UBL_ERROR, "%s:can't read run-time cofnig file=%s\n",
			       __func__, ucmd->configfiles[i]);
			goto erexit;
		}
	}
	if(ydrd!=NULL){UC_RUNCONF_CLOSE(ydrd);}

	ucd.ucntd=uc_notice_init(ucmd->ucmode, ucmd->dbname);
	if(!ucd.ucntd){goto erexit;}
	if(uc_notice_start_events_thread(ucd.ucntd, ucd.hwald)!=0){goto erexit;}
	ydbi_access_init(ucd.dbald, ucd.ucntd);
	uc_dbal_releasedb(ucd.dbald);
	if(ucmd->ucmanstart!=NULL){
		(void)uc_notice_sig_post(UC_CALL_THREAD(ucmd->ucmode), ucmd->ucmanstart);
	}
	UB_TLOG(UBL_INFO, "%s:uniconf started\n", __func__);
	if(uc_dbal_create(ucd.dbald, apsd, 4, &vtype, 1)){goto erexit;}
	if(uc_dbal_create(ucd.dbald, uc_rap, 3, &uc_rv, 1)){goto erexit;}
	if(ucmd->ucmon_thread_port && (UC_CALL_THREAD(ucmd->ucmode)!=0)){
		if(uniconfmon_thread_start(ucmd->ucmon_thread_port)!=0){goto erexit;}
	}
	while(!*ucmd->stoprun){
		// 'uc_hwal_detect_notice' is called inside 'uc_nu_proc_asked_actions'
		// this returns '2', when the DB should be saved
		res=uc_nu_proc_asked_actions(ucd.ucntd,
					     ucd.dbald, ucd.hwald, 100);
		if(res<0){
			UB_TLOG(UBL_ERROR, "%s:error in uc_nu_proc_asked_actions\n",
				__func__);
			goto erexit;
		}
		if(res==2){
			// save the DB to a file
			if(uc_dbal_save(ucd.dbald)){goto erexit;}
			if(uc_dbal_create(ucd.dbald, uc_rap, 3, &uc_rv, 1)){goto erexit;}
		}
	}
	ucmd->rval=0;
erexit:
	if(ucmd->ucmon_thread_port && uniconfmon_thread_running()){
		(void)uniconfmon_thread_stop();
	}
	if(ucd.dbald!=NULL){uc_dbal_del(ucd.dbald, uc_rap, 3);}
	UB_TLOG(UBL_INFO, "%s:closing\n", __func__);
	*ucmd->stoprun=true;
	if(ucmd->ucmanstart!=NULL){
		(void)uc_notice_sig_post(UC_CALL_THREAD(ucmd->ucmode), ucmd->ucmanstart);
	}
	if(ucd.ucntd!=NULL){uc_notice_close(ucd.ucntd, ucmd->ucmode);}
	if(ucd.hwald!=NULL){uc_hwal_close(ucd.hwald);}
	if(ucd.dbald!=NULL){uc_dbal_close(ucd.dbald, ucmd->ucmode);}
	ydbi_access_close();
	return NULL;
}

int uniconf_ready(const char *dbname, uint8_t callmode, int tout_ms)
{
	uc_dbald *dbald;
	uint8_t uc_rap[3]={XL4_DATA_RO, UC_READY, 255};
	void *value;
	uint32_t vsize=0;
	uint8_t ready=0;
	do{
		dbald=uc_dbal_open(dbname, "m", callmode);
		if(dbald){break;}
		CB_USLEEP(10000);
		tout_ms-=10;
	}while(tout_ms>0);
	while(dbald){
		if(uc_dbal_get(dbald, uc_rap, 3, &value, &vsize)==0){
			ready=*(uint8_t*)value;
			uc_dbal_get_release(dbald, uc_rap, 3, value, vsize);
			if(ready==1){break;}
		}
		uc_dbal_releasedb(dbald);
		CB_USLEEP(10000);
		tout_ms-=10;
		if(tout_ms<=0){break;}
	}
	if(dbald){
		uc_dbal_close(dbald, callmode);
		if(ready==1){return 0;}
	}
	UB_LOG(UBL_ERROR, "%s:uniconf is not ready\n", __func__);
	return -1;
}

void uniconf_cleanup_status(uc_dbald *dbald)
{
	uc_range *range;
	uint8_t key1[1]={XL4_DATA_RO};
	uint8_t key2[2]={255};
	range=uc_get_range(dbald, key1, 1, key2, 1);
	if(!range){return;}
	while(true){
		if(uc_del_in_range(dbald, range, UC_DBAL_FORWARD)!=0){break;}
	}
}
