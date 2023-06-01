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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <setjmp.h>
#include <cmocka.h>
#include "unibase.h"
#include "unibase_binding.h"

#define MAX_ES_ELEMENTS 1000

#define TD_DATA_SIZE 10
typedef struct test_data1 {
	int index;
	int actime;
	uint8_t data[TD_DATA_SIZE];
} test_data1_t;

typedef struct th_data1 {
	ub_esarray_cstd_t *eah;
	int thindex;
	int elenum;
	int thnum;
}th_data1_t;

static void *test_thread1(void *ptr)
{
	th_data1_t *thd=(th_data1_t *)ptr;
	test_data1_t td;
	int i;
	for(i=0;i<thd->elenum;i++){
		assert_int_equal(0, ub_esarray_pop_ele(thd->eah, (ub_esarray_element_t *)&td));
		td.actime++;
		assert_int_equal(0,ub_esarray_add_ele(thd->eah, (ub_esarray_element_t *)&td));
	}
	return NULL;
}

static void test_esarray1(void **state)
{
	ub_esarray_cstd_t *eah;
	int i,j;
	const int thnum=10;
	const int elenum=500;
	pthread_t th[thnum];
	th_data1_t thd[thnum];
	test_data1_t td;
	int resd[elenum];

	eah = ub_esarray_init(8, sizeof(test_data1_t), MAX_ES_ELEMENTS);
	for(i=0;i<elenum;i++){
		resd[i]=0;
		td.index=i;
		td.actime=0;
		for(j=0;j<TD_DATA_SIZE;j++) td.data[j]=i+j;
		ub_esarray_add_ele(eah, (ub_esarray_element_t *)&td);
	}
	for(i=0;i<thnum;i++){
		thd[i].thindex=i;
		thd[i].eah=eah;
		thd[i].thnum=thnum;
		thd[i].elenum=elenum;
		pthread_create(&th[i], NULL, test_thread1, &thd[i]);
	}
	for(i=0;i<thnum;i++){
		pthread_join(th[i], NULL);
	}
	for(i=0;i<elenum;i++){
		assert_int_equal(0, ub_esarray_pop_ele(thd->eah, (ub_esarray_element_t *)&td));
		resd[td.index]=td.actime;
		for(j=0;j<TD_DATA_SIZE;j++){
			assert_int_equal((td.index+j)&0xff, td.data[j]);
		}
	}
	for(i=0;i<elenum;i++){
		/* possibly some data get more accesses than the others.
		 * when a data is checked out, the other thread check out and check in
		 * the other data.  the first checked out data has less access than
		 * the others.  How often it happens depends on context switching.
		 * (1, 2*thnum-1), this range is used by that reason. the most of times,
		 * it must be in (thnum-1,thnum+1) range.
		 */
		assert_in_range(resd[i], 1, 2*thnum-1);
	}
	ub_esarray_close(eah);
}

static void *test_thread2(void *ptr)
{
	th_data1_t *thd=(th_data1_t *)ptr;
	test_data1_t *td;
	int i;
	int *ip;
	ip=(int *)malloc(sizeof(int)*thd->elenum);

	while(ub_esarray_data_lock(thd->eah)) usleep(1000);
	for(i=0;i<ub_esarray_ele_nums(thd->eah);i++){
		td=(test_data1_t *)ub_esarray_get_ele(thd->eah, i);
		if(td->index%thd->thnum != thd->thindex){
			ip[i]=td->actime;
		}else{
			td->actime+=1;
		}
	}
	for(i=0;i<ub_esarray_ele_nums(thd->eah);i++){
		td=(test_data1_t *)ub_esarray_get_ele(thd->eah, i);
		if(td->index%thd->thnum != thd->thindex){td->actime=ip[i];}
	}
	ub_esarray_data_unlock(thd->eah);
	free(ip);
	return NULL;
}

static void test_esarray2(void **state)
{
	ub_esarray_cstd_t *eah;
	const int thnum=50;
	const int elenum=500;
	pthread_t th[thnum];
	th_data1_t thd[thnum];
	test_data1_t td;
	int resd[elenum];
	int i,j;

	eah = ub_esarray_init(15, sizeof(test_data1_t), MAX_ES_ELEMENTS);
	for(i=0;i<elenum;i++){
		resd[i]=0;
		td.index=i;
		td.actime=0;
		for(j=0;j<TD_DATA_SIZE;j++) td.data[j]=i+j;
		ub_esarray_add_ele(eah, (ub_esarray_element_t *)&td);
	}
	for(i=0;i<thnum;i++){
		thd[i].thindex=i;
		thd[i].eah=eah;
		thd[i].thnum=thnum;
		thd[i].elenum=elenum;
		pthread_create(&th[i], NULL, test_thread2, &thd[i]);
	}
	for(i=0;i<thnum;i++){
		pthread_join(th[i], NULL);
	}
	for(i=0;i<elenum;i++){
		assert_int_equal(0, ub_esarray_pop_ele(thd->eah, (ub_esarray_element_t *)&td));
		resd[td.index]=td.actime;
		for(j=0;j<TD_DATA_SIZE;j++){
			assert_int_equal((td.index+j)&0xff, td.data[j]);
		}
	}
	for(i=0;i<elenum;i++){
		assert_int_equal(resd[i], 1);
	}
	ub_esarray_close(eah);
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	unibase_init(&init_para);
	return 0;
}

static int teardown(void **state)
{
	unibase_close();
	return 0;
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_esarray1),
		cmocka_unit_test(test_esarray2),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
