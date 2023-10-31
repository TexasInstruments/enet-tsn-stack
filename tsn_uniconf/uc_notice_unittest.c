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
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/cb_thread.h>
#include <cmocka.h>
#include "yangs/yang_db_runtime.h"
#include "uc_private.h"
#include "ucman.h"

#define TESTDBFILE ".uc_notice_unittest_db"

UB_SD_GETMEM_DEF_EXTERN(UC_NOTICE_SIGM);

typedef struct db_init_data {
	uint8_t d;
	uint8_t aps[4];
	int32_t ikeys[2];
	void *kvs[4];
	uint8_t kss[3];
	yang_db_access_para_t dbpara;
}db_init_data_t;

static UC_NOTICE_SIG_T *intasem[2];
static const char* intsem_name[2]={"/uc_intasem", "/cu_intasem"};
static CB_THREAD_T ucthread;

static int wait_semaphore(UC_NOTICE_SIG_T *sem, int tout_ms, const char *fname)
{
	struct timespec ts;
	uint64_t ts64;
	ts64=ub_rt_gettime64()+(uint64_t)tout_ms*UB_MSEC_NS;
	UB_NSEC2TS(ts64, ts);
	if(CB_SEM_TIMEDWAIT(sem, &ts)){
		UB_LOG(UBL_ERROR, "%s:timed out\n", fname);
		return -1;
	}
	return 0;
}

static void crate_db_data(db_init_data_t *did)
{
	memset(did, 0, sizeof(db_init_data_t));
	did->d=100;
	did->aps[0]=10;
	did->aps[1]=11;
	did->aps[2]=12;
	did->aps[3]=255;
	did->ikeys[0]=100;
	did->ikeys[1]=0;
	did->kvs[0]=did->ikeys;
	did->kvs[1]=did->ikeys+1;
	did->kvs[2]=NULL;
	did->kvs[3]=NULL;
	did->kss[0]=sizeof(int32_t);
	did->kss[1]=sizeof(int32_t);
	did->dbpara.atype=YANG_DB_ACTION_APPEND;
	did->dbpara.onhw=YANG_DB_ONHW_NOACTION;
	did->dbpara.paps=NULL;
	did->dbpara.aps=did->aps;
	did->dbpara.kvs=did->kvs;
	did->dbpara.kss=did->kss;
	did->dbpara.value=&did->d;
	did->dbpara.vsize=1;
}

static int init_db(uc_dbald *dbald)
{
	int i, res;
	db_init_data_t did;
	crate_db_data(&did);
	res=0;
	for(i=0;i<10;i++){
		did.ikeys[1]=i; // change the last part of key from 0 to 9
		res|=yang_db_action(dbald, NULL, &did.dbpara);
		did.d++;
	}
	return res;
}

static int read_db(uc_dbald *dbald)
{
	int i, res;
	db_init_data_t did;
	crate_db_data(&did);
	res=0;
	did.dbpara.value=NULL;
	did.dbpara.vsize=0;
	for(i=0;i<10;i++){
		did.dbpara.atype=YANG_DB_ACTION_READ;
		did.ikeys[1]=i; // change the last part of key from 0 to 9
		if(yang_db_action(dbald, NULL, &did.dbpara)){
			res=-1;
			UB_LOG(UBL_ERROR, "%s:failed yang_db_action, i=%d\n", __func__, i);
			break;
		}
		if(*((uint8_t*)did.dbpara.value)!=did.d){
			res=-1;
			UB_LOG(UBL_ERROR, "%s:read d=%d, should be %d\n", __func__,
			       *((uint8_t*)did.dbpara.value),did.d);
			break;
		}
		did.dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		yang_db_action(dbald, NULL, &did.dbpara);
		did.d++;
	}
	return res;
}

static int child_interact(uc_dbald *dbald, uc_notice_data_t *ucntd)
{
	int res;
	db_init_data_t did;
	UC_NOTICE_SIG_T *sem;
	const char* noticesem="/client1sem";
	uint8_t key[UC_MAX_KEYSIZE];
	void *value;
	uint32_t ksize, vsize;

	UB_TLOG(UBL_INFO, "%s:start\n", __func__);
	crate_db_data(&did);
	did.kvs[2]=(void*)noticesem;
	did.kss[2]=strlen(did.kvs[2])+1;

	/* 1st test: 'uc_client' asks notices from 'uniconf' by status updates */
	// set notice for key=0, 'sem' is returned
	res=uc_nc_notice_register(ucntd, dbald, did.aps, did.kvs, did.kss,
				  UC_NOTICE_DBVAL_ADD, &sem);
	if(res){return res;}
	// add one more notice with the same semaphore
	did.aps[0]+=1;
	res=uc_nc_notice_register(ucntd, dbald, did.aps, did.kvs, did.kss,
				  UC_NOTICE_DBVAL_ADD, NULL);
	if(res){return res;}
	did.aps[0]-=1;

	uc_dbal_releasedb(dbald);
	// 3-c, send a signal to 'uniconf'
	CB_SEM_POST(intasem[1]);

	// 4-c1, wait a signal on 'sem' from 'uniconf'
	res=wait_semaphore(sem, 100, __func__);
	if(res<0){goto erexit;}

	// get a key of signaled data
	res=uc_nc_get_notice_act(ucntd, dbald, noticesem, key, &ksize);
	UB_LOG(UBL_INFO, "got the first data, key[0]=%d\n", key[0]);
	if(res){
		UB_LOG(UBL_ERROR, "%s:failed in uc_nc_get_notice_act, res=%d\n", __func__, res);
		goto erexit;
	}

	// 4-c2, wait another signal on 'sem' from 'uniconf'
	res=wait_semaphore(sem, 100, __func__);
	if(res<0){goto erexit;}

	// get one more key of signaled data
	res=uc_nc_get_notice_act(ucntd, dbald, noticesem, key, &ksize);
	UB_LOG(UBL_INFO, "got the second data, key[0]=%d\n", key[0]);
	if(res){
		UB_LOG(UBL_ERROR, "%s:failed in uc_nc_get_notice_act, res=%d\n", __func__, res);
		goto erexit;
	}

	// get data of the key, it should be '101' by the update
	res=uc_dbal_get(dbald, key, ksize, &value, &vsize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:failed in uc_dbal_get, res=%d\n", __func__, res);
		goto erexit;
	}
	if(*(uint8_t*)value!=101){
		UB_LOG(UBL_ERROR, "%s:wrong value=%d\n", __func__, *(uint8_t*)value);
		res=-1;
	}
	uc_dbal_get_release(dbald, key, ksize, value, vsize);

	/* 2nd test: 'uc_client' asks actions(like HW configs) on 'uniconf' */
	// update a data
	UB_LOG(UBL_INFO, "%s:2nd test\n", __func__);
	crate_db_data(&did);
	did.dbpara.atype=YANG_DB_ACTION_READ;
	did.ikeys[1]=1;
	res=yang_db_action(dbald, NULL, &did.dbpara);
	if(res){
		UB_TLOG(UBL_ERROR, "%s:can't read the 2nd key's data\n", __func__);
		goto erexit;
	}
	if(*((uint8_t*)did.dbpara.value)!=101) {
		UB_TLOG(UBL_ERROR, "%s:the 2nd key's data is %d\n", __func__,
			*((uint8_t*)did.dbpara.value));
		res=-1;
		goto erexit;
	}
	did.dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, NULL, &did.dbpara);

	did.d=201; // update value of 2nd key to '201'
	did.dbpara.value=&did.d;
	did.dbpara.vsize=1;
	did.dbpara.atype=YANG_DB_ACTION_CREATE;
	res=yang_db_action(dbald, NULL, &did.dbpara);
	if(res){
		UB_TLOG(UBL_ERROR, "%s:can't update the 2nd key's data\n", __func__);
		goto erexit;
	}

	// 11-c, ask 'uniconf' to update the HW with the new data
	res=uc_nc_askaction_push(ucntd, dbald, did.dbpara.aps, did.dbpara.kvs,
				 did.dbpara.kss);
	if(res){
		UB_TLOG(UBL_ERROR, "%s:error in uc_nc_askaction_push\n", __func__);
		goto erexit;
	}
	UB_TLOG(UBL_INFO, "%s:pushed by 'uc_nc_askaction_push'\n", __func__);

	// end, wait a signal from 'uc_client'
	if(wait_semaphore(intasem[0], 100, __func__)) {
		UB_TLOG(UBL_ERROR, "%s:no end signal from uniconf\n", __func__);
		goto erexit;
	}
	res=0;
erexit:
	uc_nc_notice_deregister_all(ucntd, dbald, noticesem);
	return res;
}

static void *child_proc(void *ptr)
{
	uc_dbald *dbald;
	uc_notice_data_t *ucntd;
	int res=0;
	char *pname=(char *)ptr;
	uint8_t runmode;
	int i;

	if(strstr(pname, "pr_")==pname){
		runmode=0;
		for(i=0;i<2;i++) intasem[i]=CB_SEM_OPEN(intsem_name[i], 0, 0660, 0);
	}else if(strstr(pname, "th_")==pname){
		runmode=UC_CALLMODE_THREAD;
	}else{
		return (void*)-1;
	}
	pname+=3;

	dbald=uc_dbal_open(TESTDBFILE, "r", runmode);
	assert_non_null(dbald);
	ucntd=uc_notice_init(runmode, TESTDBFILE);
	assert_non_null(ucntd);

	// 2-c, wait a signal from
	wait_semaphore(intasem[0], 100, __func__);

	if(!strcmp(pname, "proc1") ){
		res=read_db(dbald);
	}else if(!strcmp(pname, "proc2")){
		res=child_interact(dbald, ucntd);
	}

	CB_SEM_POST(intasem[1]);
	if(ucntd){
		uc_notice_close(ucntd, runmode);
	}
	if(dbald){
		uc_dbal_releasedb(dbald);
		uc_dbal_close(dbald, runmode);
	}
	if(res){return (void*)-1;}
	return NULL;
}

static int run_uc_client(uint8_t runmode, const char *cmdstr, FILE **outf)
{
	int i;
	static char pstr[64];
	if(UC_CALL_CLIENT(runmode) && !UC_CALL_THREAD(runmode)){
		sprintf(pstr, "./uc_notice_unittest pr_%s", cmdstr);
		*outf=popen(pstr, "w");
		for(i=0;i<2;i++){
			CB_SEM_UNLINK(intsem_name[i]);
			intasem[i]=CB_SEM_OPEN(intsem_name[i], O_CREAT|O_EXCL, 0660, 0);
		}
	}else if(UC_CALL_CLIENT(runmode) && UC_CALL_THREAD(runmode)){
		for(i=0;i<2;i++) {
			intasem[i]=uc_notice_sig_reallocate(UC_CALL_THREAD(runmode), NULL, 1);
			CB_SEM_INIT(intasem[i], 0, 0);
		}
		sprintf(pstr, "th_%s", cmdstr);
		CB_THREAD_CREATE(&ucthread, NULL, child_proc, pstr);
		return 0;
	}else{
		return -1;
	}
	assert_non_null(*outf);
	usleep(50000); // starting up 'uc_client' takes time
	return 0;
}
static void close_uc_client(uint8_t runmode, const char *cmdstr, FILE *outf)
{
	int i;
	int res=0;
	if(UC_CALL_CLIENT(runmode) && !UC_CALL_THREAD(runmode)){
		res=pclose(outf);
		for(i=0;i<2;i++){
			CB_SEM_CLOSE(intasem[i]);
			CB_SEM_UNLINK(intsem_name[i]);
		}
	}else{
		res=CB_THREAD_JOIN(ucthread, NULL);
		for(i=0;i<2;i++){
			CB_SEM_DESTROY(intasem[i]);
			UB_SD_RELMEM(UC_NOTICE_SIGM, intasem[i]);
		}
	}
	assert_int_equal(res, 0);
}

static int run_process_simpleread(uint8_t runmode)
{
	FILE *outf;
	int res;

	// 1. let 'uc_client' start
	run_uc_client(runmode, "proc1", &outf);

	// 2-u, send the first signal
	CB_SEM_POST(intasem[0]);

	res=wait_semaphore(intasem[1], 100, __func__);
	// end, close 'uc_client'
	close_uc_client(runmode, "proc1", outf);
	return res;
}

static int run_process_interact(uc_dbald *dbald, uc_notice_data_t *ucntd, uint8_t runmode)
{
	int res=-1;
	FILE *outf;
	db_init_data_t did;

	crate_db_data(&did);
	// 1. let 'uc_client' start
	if(run_uc_client(runmode, "proc2", &outf)){return -1;}

	/* 1st test: 'uc_client' asks notices from 'uniconf' by status updates */
	// 2-u, send the first signal
	CB_SEM_POST(intasem[0]);

	// update the value from 100 -> 101
	did.d=101;
	did.dbpara.atype=YANG_DB_ACTION_CREATE;

	yang_db_action(dbald, NULL, &did.dbpara);
	// one more update on the different key
	did.aps[0]+=1;
	yang_db_action(dbald, NULL, &did.dbpara);
	did.aps[0]-=1;
	uc_dbal_releasedb(dbald);

	// 3-u, wait a signal from 'uc_client'
	if(wait_semaphore(intasem[1], 100, __func__)) {
		UB_TLOG(UBL_ERROR, "%s:no registered signal from uc_client\n", __func__);
		goto erexit;
	}

	// 4-u1, push updated notice
	uc_nu_putnotice_push(ucntd, dbald, did.aps, did.kvs, did.kss);
	did.aps[0]+=1;
	// 4-u2, one more push with a different key
	uc_nu_putnotice_push(ucntd, dbald, did.aps, did.kvs, did.kss);
	did.aps[0]-=1;
	uc_dbal_releasedb(dbald);

	/* 2nd test: 'uc_client' asks actions(like HW configs) on 'uniconf' */
	// 11-u, check any asking actions from 'uc_client'
	UB_LOG(UBL_INFO, "%s:2nd test\n", __func__);
	res=uc_nu_proc_asked_actions(ucntd, dbald, NULL, 100);
	if(res){
		UB_LOG(UBL_ERROR, "%s:uc_nu_proc_asked_actions, res=%d\n", __func__, res);
		goto erexit;
	}

	// confirm the data was updated in 'uc_client'
	crate_db_data(&did);
	did.dbpara.atype=YANG_DB_ACTION_READ;
	did.ikeys[1]=1;
	res=yang_db_action(dbald, NULL, &did.dbpara);
	if(res){
		UB_TLOG(UBL_ERROR, "%s:can't read the 2nd key's data\n", __func__);
		goto erexit;
	}
	if(*((uint8_t*)did.dbpara.value)!=201) {
		UB_TLOG(UBL_ERROR, "%s:the 2nd key's data is %d, vsize=%d\n", __func__,
			*((uint8_t*)did.dbpara.value), did.dbpara.vsize);
		res=-1;
		goto erexit;
	}
	did.dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, NULL, &did.dbpara);
	res=0;
erexit:
	uc_dbal_releasedb(dbald);
	CB_SEM_POST(intasem[0]); // send an end signal
	// end, close 'uc_client'
	close_uc_client(runmode, "proc2", outf);
	return res;
}

#define XSTRINGFY(x) STRINGFY(x)
#define STRINGFY(x) #x
static void test_action_push(uint8_t runmode)
{
	int res;
	uc_dbald *dbald;
	uc_notice_data_t *ucntd;
	if(!UC_CALL_THREAD(runmode) && \
	   !strcmp(XSTRINGFY(DBNAME), "SIMPLEDB")){
		UB_LOG(UBL_INFO, "%s doesn't support multiple processes\n", XSTRINGFY(DBNAME));
		return;
	}
	uniconf_remove_dbfile(TESTDBFILE);
	dbald=uc_dbal_open(TESTDBFILE, "w", UC_CALLMODE_UNICONF | runmode);
	assert_non_null(dbald);
	ucntd=uc_notice_init(UC_CALLMODE_UNICONF | runmode, TESTDBFILE);
	assert_non_null(ucntd);
	res=init_db(dbald);
	assert_int_equal(res, 0);
	res=read_db(dbald);
	assert_int_equal(res, 0);

	uc_dbal_releasedb(dbald);
	UB_LOG(UBL_INFO, "%s:start simpleread test\n", __func__);
	res=run_process_simpleread(runmode);
	assert_int_equal(res, 0);
	UB_LOG(UBL_INFO, "%s:### PASS simpleread test ###\n", __func__);

	UB_LOG(UBL_INFO, "%s:start interact test\n", __func__);
	res=run_process_interact(dbald, ucntd, runmode);
	assert_int_equal(res, 0);
	UB_LOG(UBL_INFO, "%s:### PASS interact test ###\n", __func__);

	if(ucntd){uc_notice_close(ucntd, UC_CALLMODE_UNICONF | runmode);}
	if(dbald){uc_dbal_close(dbald, UC_CALLMODE_UNICONF | runmode);}
}

static void test_action_push_mp(void **state)
{
	test_action_push(0);
}

static void test_action_push_mt(void **state)
{
	test_action_push(UC_CALLMODE_THREAD);
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:66", "UBL_UNICONF");
	unibase_init(&init_para);
	return 0;
}

static int teardown(void **state)
{
	ubb_memory_out_close();
	unibase_close();
	return 0;
}

int main(int argc, char *argv[])
{
	void *res;
	if(argc>1){
		setup(NULL);
		res=child_proc(argv[1]);
		teardown(NULL);
		if(res){return -1;}
		return 0;
	}
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_action_push_mp),
		cmocka_unit_test(test_action_push_mt),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
