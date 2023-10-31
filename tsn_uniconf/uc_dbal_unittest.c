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
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/combase.h>
#include <cmocka.h>
#include "uc_dbal.h"
#include "ucman.h"

static void test_keycmp(void **state)
{
	int res;
	uint8_t key1[10]={1,2,3,4,5,6,7,8,9,10};
	uint8_t key2[10]={1,2,3,4,5,6,7,8,9,10};
	res=uc_dbal_keycmp(key1, 10, key2, 10);
	assert_int_equal(res, 0);
	// when the value is the same but the lenght is different
	// the result is 1 or -1
	res=uc_dbal_keycmp(key1, 9, key2, 10);
	assert_int_equal(res, -1);
	res=uc_dbal_keycmp(key1, 10, key2, 9);
	assert_int_equal(res, 1);

	key1[2]=30;
	// when the value is different
	// the result is memcmp value
	res=uc_dbal_keycmp(key1, 10, key2, 10);
	assert_true(res>0);
	res=uc_dbal_keycmp(key1, 3, key2, 10);
	assert_true(res>0);
	res=uc_dbal_keycmp(key2, 10, key1, 3);
	assert_true(res<0);
	// this case the difference is only the lenght
	res=uc_dbal_keycmp(key1, 2, key2, 10);
	assert_int_equal(res, -1);
}

#define NUMBER_OF_THREADS 10
typedef struct thread_data{
	int thread_num;
	int result;
	CB_THREAD_T td;
	CB_SEM_T *sem_alldone;
	CB_SEM_T *sem_un2uc;
	CB_SEM_T *sem_uc2un;
}thread_data_t;

static int create_data(uc_dbald *udbd, int tnum)
{
	int res;
	uint8_t cdkey[10]={1,2,3,4,5,6,7,8,9,10};
	uint8_t value[10]={10,20,30,40,50,60,70,80,90,100};
	// create key,data
	cdkey[9]=tnum;
	value[9]=tnum*10;
	res=uc_dbal_create(udbd, cdkey, 10, value, 10);
	if(res){
		UB_LOG(UBL_ERROR, "%s:tnum=%d, can't create data, res=%d\n", __func__, tnum, res);
		return -1;
	}
	// create key,NULL data
	cdkey[0]=0;
	res=uc_dbal_create(udbd, cdkey, 10, NULL, 0);
	if(res){
		UB_LOG(UBL_ERROR, "%s:tnum=%d, can't create NULL data\n", __func__, tnum);
		return -1;
	}
	return 0;
}

static int read_data(uc_dbald *udbd, int tnum)
{
	int res=0;;
	void *rvalue;
	uint32_t rvsize;
	uint8_t cdkey[10]={1,2,3,4,5,6,7,8,9,10};
	uint8_t value[10]={10,20,30,40,50,60,70,80,90,100};
	uc_range *range;
	void *key;
	uint32_t ksize;
	// create key,data
	cdkey[9]=tnum;
	value[9]=tnum*10;
	res=uc_dbal_get(udbd, cdkey, 10, &rvalue, &rvsize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:tnum=%d, can't read data\n", __func__, tnum);
		return -1;
	}
	if(rvsize!=10){
		UB_LOG(UBL_ERROR, "%s:tnum=%d, wrong rvsize=%d\n",
		       __func__, tnum, rvsize);
		res=-1;
		goto ernext;
	}
	if(memcmp(rvalue, value, rvsize)){
		UB_LOG(UBL_ERROR, "%s:tnum=%d, rvalue\n", __func__, tnum);
		res=-1;
		goto ernext;
	}
ernext:
	uc_dbal_get_release(udbd, cdkey, 10, rvalue, rvsize);

	cdkey[0]=0;
	range=uc_get_range(udbd, cdkey, 10, cdkey, 10);
	if(!range){
		UB_LOG(UBL_ERROR, "%s:tnum=%d, no range\n", __func__, tnum);
		res=-1;
		goto ernext2;
	}
	uc_get_key_in_range(udbd, range, &key, &ksize, UC_DBAL_NOMOVE);
	if(ksize!=10 || memcmp(cdkey, key, ksize)){
		ub_hexdump(true, true, key, ksize, 0);
		UB_LOG(UBL_ERROR, "%s:tnum=%d, wrong keydata, ksize=%d\n",
		       __func__, tnum, ksize);
		res=-1;
	}
	uc_get_range_release(udbd, range);
ernext2:
	return res;
}

static void *one_thread(void *ptr)
{
	int i;
	uc_dbald *udbd;
	uint8_t runmode;
	thread_data_t *thd=(thread_data_t*)ptr;

	if(thd->thread_num==0){
		runmode=UC_CALLMODE_UNICONF|UC_CALLMODE_THREAD;
	}else{
		CB_SEM_WAIT(thd->sem_un2uc);
		runmode=UC_CALLMODE_THREAD;
	}

	udbd=uc_dbal_open(".uc_dbal_unittestdb", "w", runmode);
	if(thd->thread_num==0){
		for(i=1;i<NUMBER_OF_THREADS;i++) CB_SEM_POST(thd->sem_un2uc);
	}
	if(create_data(udbd, thd->thread_num)){thd->result=-1;}
	if(thd->thread_num!=0){
		CB_SEM_POST(thd->sem_uc2un);
	}else{
		for(i=1;i<NUMBER_OF_THREADS;i++) CB_SEM_WAIT(thd->sem_uc2un);
		for(i=1;i<NUMBER_OF_THREADS;i++)
			if(read_data(udbd, i)){thd->result=-1;}
	}


	uc_dbal_close(udbd, runmode);
	if(thd->thread_num==0){
		CB_SEM_POST(thd->sem_alldone);
	}
	return NULL;
}

static void test_run_in_threads(void **state)
{
	int i, res;
	thread_data_t thds[NUMBER_OF_THREADS];
	CB_SEM_T sem_alldone;
	CB_SEM_T sem_un2uc;
	CB_SEM_T sem_uc2un;
	uniconf_remove_dbfile(".uc_dbal_unittestdb");
	memset(thds, 0, sizeof(thread_data_t)*NUMBER_OF_THREADS);
	CB_SEM_INIT(&sem_alldone, 0, 0);
	CB_SEM_INIT(&sem_un2uc, 0, 0);
	CB_SEM_INIT(&sem_uc2un, 0, 0);
	for(i=0;i<NUMBER_OF_THREADS;i++){
		thds[i].thread_num=i;
		thds[i].sem_alldone=&sem_alldone;
		thds[i].sem_un2uc=&sem_un2uc;
		thds[i].sem_uc2un=&sem_uc2un;
		res=CB_THREAD_CREATE(&thds[i].td, NULL, one_thread, &thds[i]);
		assert_int_equal(res, 0);
	}
	CB_SEM_WAIT(&sem_alldone);
	for(i=0;i<NUMBER_OF_THREADS;i++){
		UB_LOG(UBL_INFO, "%s:i=%d, result=%d\n", __func__, i, thds[i].result);
		assert_int_equal(thds[i].result, 0);
		CB_THREAD_JOIN(thds[i].td, NULL);
	}
	CB_SEM_DESTROY(&sem_uc2un);
	CB_SEM_DESTROY(&sem_un2uc);
	CB_SEM_DESTROY(&sem_alldone);
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
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_keycmp),
		cmocka_unit_test(test_run_in_threads),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
