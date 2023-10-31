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
#include <cmocka.h>
#include "simpledb.h"

static void put_get_nocp(void)
{
	simpledb_data_t *sdbd;
	char key[10][32], vstr[10][32];
	simpledb_keydata_t kd[10];
	char *rv;
	uint32_t rlen;
	int i, res;

	srand((unsigned int)ub_rt_gettime64());
	sdbd=simpledb_open(NULL);
	assert_non_null(sdbd);
	for(i=0;i<10;i++){
		sprintf(key[i], "key%d", rand());
		sprintf(vstr[i], "value%d", rand());
		printf("key=%s, value=%s\n", key[i],vstr[i]);
		kd[i].ksize=strlen(key[i])+1;
		kd[i].kdata=key[i];
		simpledb_put(sdbd, &kd[i], vstr[i], strlen(vstr[i])+1,
			     SIMPLEDB_FLAG_CREATE);
	}
	for(i=0;i<10;i++){
		res=simpledb_get(sdbd, &kd[i], (void**)&rv, &rlen);
		assert_int_equal(res, 0);
		assert_string_equal(vstr[i], rv);
		res=simpledb_put(sdbd, &kd[i], vstr[i], strlen(vstr[i])+1,
				 SIMPLEDB_FLAG_CREATE);
		assert_int_equal(res, -1); // it is locked, can't write
		simpledb_get_release(sdbd, &kd[i]); // unlock
		res=simpledb_put(sdbd, &kd[i], vstr[i], strlen(vstr[i])+1,
				 SIMPLEDB_FLAG_CREATE);
		assert_int_equal(res, 0); // can write
	}

	for(i=0;i<10;i++){
		if(i%3==1){
			res=simpledb_del(sdbd, &kd[i]);
			assert_int_equal(res, 0);
		}
	}
	for(i=0;i<10;i++){
		res=simpledb_get(sdbd, &kd[i], (void**)&rv, &rlen);
		if(i%3==1){
			assert_int_equal(res, -1);
		}else{
			assert_int_equal(res, 0);
			assert_string_equal(vstr[i], rv);
		}
		if(res){continue;}
		else{simpledb_get_release(sdbd, &kd[i]);}
	}
	simpledb_close(sdbd);
}

static void test_put_get(void **state)
{
	put_get_nocp();
}
static void test_persistent_put_get(void **state)
{
	simpledb_data_t *sdbd;
	char key[10][32], vstr[10][32];
	simpledb_keydata_t kd[10];
	char *rv;
	uint32_t rlen;
	int i, res;
	key_range_t krange;
	key_range_t krange2;
	key_range_t *kranges[3]={&krange, NULL, NULL};

	memset(&krange, 0, sizeof(krange));
	srand((unsigned int)ub_rt_gettime64());
	unlink(".simpledb_testdata");
	sdbd=simpledb_open(".simpledb_testdata");
	assert_non_null(sdbd);
	for(i=0;i<10;i++){
		if(i<5){sprintf(key[i], "Akey%04d", rand()%100+i*1000);}
		else{sprintf(key[i], "Zkey%04d", rand()%100+(i-5)*1000);}
		sprintf(vstr[i], "value%d", rand());
		printf("key=%s, value=%s\n", key[i],vstr[i]);
		kd[i].ksize=strlen(key[i])+1;
		kd[i].kdata=key[i];
		simpledb_put(sdbd, &kd[i], vstr[i], strlen(vstr[i])+1,
			     SIMPLEDB_FLAG_CREATE);
	}
	for(i=0;i<10;i++){
		res=simpledb_get(sdbd, &kd[i], (void**)&rv, &rlen);
		assert_int_equal(res, 0);
		assert_string_equal(vstr[i], rv);
		if(res==0){simpledb_get_release(sdbd, &kd[i]);}
	}
	krange.kd2=&kd[4];
	simpledb_savedata(sdbd, kranges);
	simpledb_close(sdbd);

	sdbd=simpledb_open(".simpledb_testdata");
	assert_non_null(sdbd);
	for(i=0;i<10;i++){
		res=simpledb_get(sdbd, &kd[i], (void**)&rv, &rlen);
		if(i<5){
			assert_int_equal(res, 0);
			assert_string_equal(vstr[i], rv);
		}else{
			assert_int_equal(res, -1);
			simpledb_put(sdbd, &kd[i], vstr[i], strlen(vstr[i])+1,
				     SIMPLEDB_FLAG_CREATE);
		}
		if(res==0){simpledb_get_release(sdbd, &kd[i]);}
	}

	krange.kd1=&kd[1];
	krange.kd2=&kd[3];
	krange2.kd1=&kd[6];
	krange2.kd2=&kd[9];
	kranges[1]=&krange2;
	simpledb_savedata(sdbd, kranges);
	simpledb_close(sdbd);

	sdbd=simpledb_open(".simpledb_testdata");
	assert_non_null(sdbd);
	for(i=0;i<10;i++){
		res=simpledb_get(sdbd, &kd[i], (void**)&rv, &rlen);
		if((i>=1 && i<=3) || (i>=6 && i<=9)){
			assert_int_equal(res, 0);
			assert_string_equal(vstr[i], rv);
		}else{
			assert_int_equal(res, -1);
		}
		if(res==0){simpledb_get_release(sdbd, &kd[i]);}
	}
	simpledb_close(sdbd);
}

static void test_range(void **state)
{
	int i;
	int a[10];
	void *value=NULL;
	uint32_t vsize;
	simpledb_data_t *sdbd;
	simpledb_range_t *rangedp;
	simpledb_keydata_t kd;
	simpledb_keydata_t kdl[10];
	uint8_t key[10][4]={
		{0x00,0x00,0x00,0x00},//0,1
		{0x00,0x00,0x00,0x01},//1,2
		{0x01,0x01,0x00,0x01},//2,7
		{0x01,0x00,0x00,0x01},//3,5
		{0x01,0x00,0x00,0x00},//4,4
		{0x02,0x00,0x00},//5,8
		{0x01,0x00,0x00},//6,3
		{0x02,0x01,0x01},//7,9
		{0x01,0x01,0x00},//8,6
		{0x00,0x00,0x00},//9,0
	};
	int keylen[10]={4,4,4,4,4,3,3,3,3,3};
	int values[10]={0,1,2,3,4,5,6,7,8,9};
	key_range_t kr;
	for(i=0;i<10;i++){
		kdl[i].ksize=keylen[i];
		kdl[i].kdata=key[i];
	}
	sdbd=simpledb_open(NULL);
	assert_non_null(sdbd);
	for(i=0;i<10;i++){
		simpledb_put(sdbd, &kdl[i], &values[i], sizeof(int),
			     SIMPLEDB_FLAG_CREATE);
	}
	kr.kd2=&kdl[0];
	kr.kd1=&kdl[2];
	rangedp=simpledb_get_range(sdbd, &kr);
	assert_null(rangedp);


	kr.kd1=NULL;
	kr.kd2=&kdl[9];
	rangedp=simpledb_get_range(sdbd, &kr); // 0
	assert_non_null(rangedp);
	a[0]=9;
	for(i=0;;i++){
		if(simpledb_get_from_range(sdbd, rangedp, &kd.kdata, &kd.ksize,
					   NULL, NULL, SIMPLEDB_NOMOVE)){break;}
		if(simpledb_get_from_range(sdbd, rangedp, NULL, NULL,
					   &value, &vsize, SIMPLEDB_FORWARD)){break;}
		assert_int_equal(kd.ksize, keylen[a[i]]);
		assert_memory_equal(kd.kdata, key[a[i]], keylen[a[i]]);
		assert_memory_equal(value, &values[a[i]], sizeof(int));
	}
	assert_int_equal(i, 1);
	simpledb_get_range_release(sdbd, rangedp);

	kr.kd1=&kdl[4];
	kr.kd2=&kdl[2];
	rangedp=simpledb_get_range(sdbd, &kr); // 4,3,8,2
	assert_non_null(rangedp);
	a[0]=4;a[1]=3;a[2]=8;a[3]=2;
	for(i=0;;i++){
		if(simpledb_get_from_range(sdbd, rangedp, &kd.kdata, &kd.ksize,
					   NULL, NULL, SIMPLEDB_NOMOVE)){break;}
		if(simpledb_get_from_range(sdbd, rangedp, NULL, NULL,
					   &value, &vsize, SIMPLEDB_FORWARD)){break;}
		assert_int_equal(kd.ksize, keylen[a[i]]);
		assert_memory_equal(kd.kdata, key[a[i]], keylen[a[i]]);
		assert_memory_equal(value, &values[a[i]], sizeof(int));
	}
	assert_int_equal(i, 4);
	simpledb_get_range_release(sdbd, rangedp);

	kr.kd1=&kdl[6];
	kr.kd2=&kdl[8];
	rangedp=simpledb_get_range(sdbd, &kr); // 6,4,3,8
	assert_non_null(rangedp);
	a[0]=6;a[1]=4;a[2]=3;a[3]=8;
	for(i=0;;i++){
		if(simpledb_get_from_range(sdbd, rangedp, &kd.kdata, &kd.ksize,
					   NULL, NULL, SIMPLEDB_NOMOVE)){break;}
		if(simpledb_get_from_range(sdbd, rangedp, NULL, NULL,
					   &value, &vsize, SIMPLEDB_FORWARD)){break;}
		assert_int_equal(kd.ksize, keylen[a[i]]);
		assert_memory_equal(kd.kdata, key[a[i]], keylen[a[i]]);
		assert_memory_equal(value, &values[a[i]], sizeof(int));
	}
	assert_int_equal(i, 4);

	simpledb_move_bottom_range(sdbd, rangedp);
	a[0]=8;a[1]=3;a[2]=4;a[3]=6;
	for(i=0;;i++){
		if(simpledb_get_from_range(sdbd, rangedp, &kd.kdata, &kd.ksize,
					   NULL, NULL, SIMPLEDB_NOMOVE)){break;}
		if(simpledb_get_from_range(sdbd, rangedp, NULL, NULL,
					   &value, &vsize, SIMPLEDB_BACKWARD)){break;}
		assert_int_equal(kd.ksize, keylen[a[i]]);
		assert_memory_equal(kd.kdata, key[a[i]], keylen[a[i]]);
		assert_memory_equal(value, &values[a[i]], sizeof(int));
	}
	assert_int_equal(i, 4);

	simpledb_get_range_release(sdbd, rangedp);

	simpledb_close(sdbd);
}

static void test_append(void **state)
{
	simpledb_data_t *sdbd;
	simpledb_keydata_t kd;
	char *value="value1";
	uint32_t vsize=strlen(value)+1;
	uint32_t rlen;
	char *rv;
	int res;
	kd.kdata="key1";
	kd.ksize=strlen((char*)kd.kdata);
	sdbd=simpledb_open(NULL);
	assert_non_null(sdbd);
	simpledb_put(sdbd, &kd, value, vsize, SIMPLEDB_FLAG_CREATE);
	value="value2";
	vsize=strlen(value)+1;
	simpledb_put(sdbd, &kd, value, vsize, SIMPLEDB_FLAG_APPEND);
	simpledb_get(sdbd, &kd, (void**)&rv, &rlen);
	assert_string_equal(rv, "value1");
	assert_string_equal(rv+7, "value2");
	simpledb_get_release(sdbd, &kd);

	res=simpledb_get(sdbd, &kd, (void**)&rv, &rlen);
	assert_int_equal(res, 0);
	res=simpledb_put(sdbd, &kd, value, vsize, SIMPLEDB_FLAG_CREATE);
	assert_int_equal(res, -1); // it is locked, and fails to write
	res=simpledb_wait_release(sdbd, 1);
	assert_int_equal(res, 0); // the last release set the semaphore, and no timeout
	res=simpledb_wait_release(sdbd, 1);
	assert_int_equal(res, -1); // get timeout
	simpledb_get_release(sdbd, &kd); // release the lock
	res=simpledb_wait_release(sdbd, 1);
	assert_int_equal(res, 0);
	res=simpledb_put(sdbd, &kd, value, vsize, SIMPLEDB_FLAG_CREATE);
	assert_int_equal(res, 0);

	simpledb_close(sdbd);
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
		cmocka_unit_test(test_put_get),
		cmocka_unit_test(test_persistent_put_get),
		cmocka_unit_test(test_range),
		cmocka_unit_test(test_append),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
