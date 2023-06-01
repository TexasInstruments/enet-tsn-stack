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
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define UNIT_TESTING
#include <cmocka.h>
#include "combase.h"
#include <tsn_unibase/unibase_binding.h>
#define UB_LOGCAT 2

static char *test_string1="cfo9(eb@jbf!t-+=@wi+x3pew7#e^xai*q6k_igi55t$%&1xwt";
static char *test_string2=")zj_(_brpo)kcb=&6r0@zpdrv42pe4#-754^s=-)jr#xm72u3_";
static void *test_noipcshmem_thread1(void *ptr)
{
	void *shmem;
	int memfd;
	int i;

	shmem=cb_get_shared_mem(&memfd, "testshmem1", 1024, O_RDWR);
	assert_non_null(shmem);
	strcpy((char*)shmem, test_string1);
	for(i=0;i<10;i++){
		if(strcmp((char*)shmem, test_string1)==0){break;}
		usleep(1000);
	}
	assert_true(i<10);
	return NULL;
}

static void *test_noipcshmem_thread2(void *ptr)
{
	void *shmem;
	int memfd;
	int i;
	shmem=cb_get_shared_mem(&memfd, "testshmem1", 1024, O_RDWR);
	assert_non_null(shmem);
	for(i=0;i<10;i++){
		if(strcmp((char*)shmem, test_string1)==0){break;}
		usleep(1000);
	}
	assert_true(i<10);
	strcpy((char*)shmem, test_string2);
	return NULL;
}

static void test_noipcshmem1(void **state)
{
	int memfd;
	void *shmem;
	shmem=cb_get_shared_mem(&memfd, "testshmem1", 1024, O_RDWR);
	assert_null(shmem);
	shmem=cb_get_shared_mem(&memfd, "testshmem1", 1024, O_RDWR|O_CREAT);
	assert_non_null(shmem);
	shmem=cb_get_shared_mem(&memfd, "testshmem2", 1024, O_RDWR|O_CREAT);
	assert_non_null(shmem);
	strcpy((char*)shmem, test_string1);
	shmem=cb_get_shared_mem(&memfd, "testshmem1", 1024, O_RDONLY);
	assert_non_null(shmem);
	assert_string_not_equal((char*)shmem, test_string1);
	shmem=cb_get_shared_mem(&memfd, "testshmem2", 1024, O_RDONLY);
	assert_non_null(shmem);
	assert_string_equal((char*)shmem, test_string1);
}

static void test_noipcshmem2(void **state)
{
	CB_THREAD_T th1, th2;
	int memfd;
	void *shmem;

	CB_THREAD_CREATE(&th1, NULL, test_noipcshmem_thread1, NULL);
	CB_THREAD_CREATE(&th2, NULL, test_noipcshmem_thread2, NULL);

	CB_THREAD_JOIN(th1, NULL);
	CB_THREAD_JOIN(th2, NULL);

	shmem=cb_get_shared_mem(&memfd, "testshmem1", 1024, O_RDONLY);
	assert_int_not_equal(memfd,0);
	cb_close_shared_mem(shmem, &memfd, "testshmem1", 1024, true);
	assert_int_equal(memfd,0);

	shmem=cb_get_shared_mem(&memfd, "testshmem2", 1024, O_RDONLY);
	assert_int_not_equal(memfd,0);
	cb_close_shared_mem(shmem, &memfd, "testshmem2", 1024, true);
	assert_int_equal(memfd,0);
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr="4,ubase:45,combase:45,noipcsm:55";
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
		cmocka_unit_test(test_noipcshmem1),
		cmocka_unit_test(test_noipcshmem2),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
