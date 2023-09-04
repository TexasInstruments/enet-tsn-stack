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
#include <signal.h>
#include <errno.h>
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/combase.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/yangs/yang_db_runtime.h>
#include <tsn_uniconf/ucman.h>
#include "xl4-extmod-ucpra0.h"
#include "xl4-extmod-ucpra0_runconf.h"

#define UCPRA_DBNAME "ucpradb"

static int one_read_print(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			  const char* iname, uint8_t vtype, uint8_t *index0)
{
	int i;
	char *vstr;
	char *tabsp;
	char *delim;

	tabsp=((strlen(iname)+1)/8)?"\t":"\t\t";
	UB_CONSOLE_PRINT("%s:%s", iname, tabsp);
	delim="";
	for(i=0;;i++){
		if(index0) *index0=i;
		dbpara->atype=YANG_DB_ACTION_READ;
		if(yang_db_action(dbald, NULL, dbpara)) break;
		vstr=yang_value_string(vtype, dbpara->value, dbpara->vsize, 0, NULL);
		UB_CONSOLE_PRINT("%s%s", delim, vstr);
		dbpara->atype=YANG_DB_ACTION_READ_RELEASE;
		if(yang_db_action(dbald, NULL, dbpara)) break;
		delim=",";
		if(!index0) break;
	}
	UB_CONSOLE_PRINT("\n");
	return 0;
}

static int print_list_values(uc_dbald *dbald, xl4_data_data_t *xdd)
{
	int res=-1;
	uint8_t aps[6]={XL4_EXTMOD_RO, 0, 0, 255, 255, 255};
	uint8_t index0;
	void *kvs[3]={NULL, &index0, NULL};
	uint8_t kss[2]={0, 1};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ,YANG_DB_ONHW_NOACTION,
	                              NULL,aps,kvs,kss,NULL,0};
	uint8_t aps1[4]={XL4_EXTMOD_RW, 0, 0, 0};
	uint8_t aps2[4]={XL4_EXTMOD_RW, 0, 0, 0};
	uc_range *range;
	uint8_t *eaps=NULL;
	kvs_t ekvs[UC_MAX_VALUEKEYS];
	uint8_t ekss[UC_MAX_VALUEKEYS];

	void *key, *value;
	uint32_t ksize, vsize;

	// the key information is unknown, but value type of the key is known
	// scan 'sysid' for all keys
	aps1[1]=xl4_data_get_modid(xdd, "ucpra0");
	aps1[2]=XL4_EXTMOD_UCPRA0_SUBSYSTEM;
	aps1[3]=XL4_EXTMOD_UCPRA0_SYSID;
	aps2[1]=aps1[1];
	aps2[2]=aps1[2];
	aps2[3]=aps1[3]+1;
	aps[1]=aps1[1];
	aps[2]=aps1[2];
	range=uc_get_range(dbald, aps1, 4, aps2, 4);
	while(range){
		if(uc_get_key_in_range(dbald, range, &key, &ksize, UC_DBAL_NOMOVE)){
			res=0;
			break;
		}
		if(uc_get_value_in_range(dbald, range, &value, &vsize, UC_DBAL_FORWARD))
			break;
		if(yang_db_extract_key(key, ksize, &eaps, ekvs, ekss)) break;
		// sysname:kvs[0], index0:kvs[1]
		UB_CONSOLE_PRINT("subsystem sysname:%s\n", (char*)ekvs[0]);
		UB_CONSOLE_PRINT("subsystem sysid:  %d\n", *((uint32_t*)value));

		kvs[0]=ekvs[0];
		kss[0]=strlen((char*)ekvs[0])+1;
		aps[3]=XL4_EXTMOD_UCPRA0_DEVNAMES;
		aps[4]=XL4_EXTMOD_UCPRA0_VALUE;
		if(one_read_print(dbald, &dbpara, "devnames", YANG_VTYPE_STRING, &index0)) break;
		aps[3]=XL4_EXTMOD_UCPRA0_DEVVALUES;
		if(one_read_print(dbald, &dbpara, "devvalues", YANG_VTYPE_UINT8, &index0)) break;

		yang_db_extract_key_free(eaps, ekvs, ekss);
	}
	if(res){
		if(eaps) yang_db_extract_key_free(eaps, ekvs, ekss);
		UB_LOG(UBL_ERROR, "%s:error in the loop\n", __func__);
	}
	return res;
}

static int print_all_values(uc_dbald *dbald, xl4_data_data_t *xdd)
{
	int res=-1;
	uint8_t aps[6]={XL4_EXTMOD_RW, 0, 0, 255, 255, 255};
	uint8_t index0;
	void *kvs[2]={&index0, NULL};
	uint8_t kss[1]={1};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ,YANG_DB_ONHW_NOACTION,
	                              NULL,aps,NULL,NULL,NULL,0};
	aps[1]=xl4_data_get_modid(xdd, "ucpra0");

	dbpara.atype=YANG_DB_ACTION_READ;
	aps[2]=XL4_EXTMOD_UCPRA0_SYSNAME;
	if(one_read_print(dbald, &dbpara, "sysname", YANG_VTYPE_STRING, NULL)){
		goto erexit;
	}

	dbpara.atype=YANG_DB_ACTION_READ;
	aps[2]=XL4_EXTMOD_UCPRA0_SYSID;
	if(one_read_print(dbald, &dbpara, "sysid", YANG_VTYPE_UINT32, NULL)){
		goto erexit;
	}

	dbpara.kvs=kvs;
	dbpara.kss=kss;

	aps[0]=XL4_EXTMOD_RO;
	aps[2]=XL4_EXTMOD_UCPRA0_DEVNAMES;
	aps[3]=XL4_EXTMOD_UCPRA0_VALUE;
	if(one_read_print(dbald, &dbpara, "devnames", YANG_VTYPE_STRING, &index0)){
		goto erexit;
	}

	aps[0]=XL4_EXTMOD_RO;
	aps[2]=XL4_EXTMOD_UCPRA0_DEVVALUES;
	if(one_read_print(dbald, &dbpara, "devvalues", YANG_VTYPE_UINT8, &index0)){
		goto erexit;
	}
	if(print_list_values(dbald, xdd)){goto erexit;}

	res=0;
erexit:
	return res;
}

static int monitor_loop(uc_dbald *dbald, xl4_data_data_t *xdd)
{
	char rbuf[128];
	int res=0;
	yang_db_runtime_dataq_t *ydrd;

	ydrd=yang_db_runtime_init(xdd, dbald, NULL);
	while(true){
		print_all_values(dbald, xdd);
		UB_CONSOLE_PRINT("\nconfig file('q' to quit the program) ?\n");
		res=read(1, rbuf, 127);
		if(res<=0){
			UB_LOG(UBL_ERROR, "%s:error in read, %s\n",
			       __func__, strerror(errno));
			res=-1;
			break;
		}
		if(rbuf[res-1]=='\n')
			rbuf[res-1]=0;
		else
			rbuf[res]=0;
		if(!strcmp(rbuf, "q")){
			res=0;
			break;
		}
		yang_db_runtime_readfile(ydrd, rbuf);
	}
	yang_db_runtime_close(ydrd);
	return res;
}

static int start_uniconf(ucman_data_t *ucmd, CB_THREAD_T *ucthreadt)
{
	ucmd->dbname=UCPRA_DBNAME;
	ucmd->ucmode=UC_CALLMODE_UNICONF|UC_CALLMODE_THREAD;
	ucmd->ucmanstart=malloc(sizeof(CB_SEM_T));
	ucmd->hwmod="NONE";
	CB_SEM_INIT(ucmd->ucmanstart, 0, 0);
	if(CB_THREAD_CREATE(ucthreadt, NULL, uniconf_main, ucmd)) return -1;
	CB_SEM_WAIT(ucmd->ucmanstart);
	UB_LOG(UBL_INFO, "uniconf started\n");
	return 0;
}

static int ucpra0_config_init(xl4_data_data_t *xdd, uc_dbald *dbald, uc_hwald *hwald)
{
	int res=0;

	if(xl4_data_set_xl4ext(xdd, "ucpra0",
			       xl4_extmod_ucpra0_get_enum,
			       xl4_extmod_ucpra0_get_string)<0) return -1;
	res|=xl4_extmod_ucpra0_config_init(xdd, dbald, hwald);
	res|=xl4_extmod_ucpra0_runconf_config_init(xdd, dbald, hwald);
	return res;
}

int main(int argc, char* argv[])
{
	uc_dbald *dbald=NULL;
	xl4_data_data_t *xdd=NULL;
	int res=-1;
	unibase_init_para_t init_para;
	ucman_data_t ucmd;
	bool stoprun=false;
	CB_THREAD_T ucthreadt;

	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46,ucpra:66",
						   "UBL_UCPRA");
	unibase_init(&init_para);
	ubb_memory_out_init(NULL, 512);
	memset(&ucmd, 0, sizeof(ucmd));
	ucmd.stoprun=&stoprun;
	if(start_uniconf(&ucmd, &ucthreadt)) goto erexit;
	dbald=uc_dbal_open(UCPRA_DBNAME, "w", UC_CALLMODE_THREAD);
	if(!dbald) goto erexit;
	xdd=xl4_data_init(dbald);
	if(!xdd) goto erexit;
	if(ucpra0_config_init(xdd, dbald, NULL)) goto erexit;
	res=monitor_loop(dbald, xdd);
erexit:
	stoprun=true;
	if(dbald) CB_THREAD_JOIN(ucthreadt, NULL);
	UB_LOG(UBL_INFO, "ucpra0 closing\n");
	if(xdd) xl4_data_close(xdd);
	if(dbald) uc_dbal_close(dbald, UC_CALLMODE_THREAD);
	ubb_memory_file_out("ucpra0_debug.log");
	ubb_memory_out_close();
	unibase_close();
	return res;
}
