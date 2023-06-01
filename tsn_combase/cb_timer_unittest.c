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
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <cmocka.h>
#include "combase.h"
#include <tsn_unibase/unibase_binding.h>

#define UB_LOGCAT 2

static int check_one_expire(cb_timer_object_t *mtmo, int tms, int64_t ts1)
{
	int res=0;
	int64_t ts2;
	res=select(1, NULL, NULL, NULL, NULL);
	if(!res){return -1;}
	if(errno!=EINTR){
		UB_LOG(UBL_ERROR,"%s:wrong interrupt of select\n",__func__);
		return -1;
	}
	ts2=ub_rt_gettime64();
	if(!cb_timer_expired(mtmo, CB_TIMER_ALL_CLEAR)){
		UB_LOG(UBL_INFO,"%s:timer is not expired\n",__func__);
		return 1;
	}
	ts2=ts2-ts1;
	UB_LOG(UBL_INFO,"%dmsec timer, %"PRIi64"nsec\n", tms, ts2);
	if(ts2>(tms+1)*UB_MSEC_NS || ts2<(tms-1)*UB_MSEC_NS){
		UB_LOG(UBL_ERROR,"%s:wrong passed time, %"PRIi64"nsec\n",__func__, ts2);
		return -1;
	}
	return 0;
}

static void unit_test1(void **state)
{
	cb_timer_object_t *mtmo;
	int64_t ts1;
	mtmo=cb_timer_create(NULL);
	ts1=ub_rt_gettime64();
	cb_timer_start_interval(mtmo, 100, 0);
	assert_false(check_one_expire(mtmo, 100, ts1));
}

static void unit_test2(void **state)
{
	cb_timer_object_t *mtmo[2];
	int i;
	int tms;
	int64_t ts1;
	mtmo[0]=cb_timer_create("tm1");
	mtmo[1]=cb_timer_create("tm2");

	ts1=ub_rt_gettime64();
	cb_timer_start_interval(mtmo[0], 10, 0);
	cb_timer_start_interval(mtmo[1], 100, 0);
	for(i=0;i<2;i++){
		tms=i==0?10:100;
		assert_false(check_one_expire(mtmo[i], tms, ts1));
	}
}

static void unit_test3(void **state)
{
	cb_timer_object_t *mtmo[2];
	int i;
	int tms;
	int64_t ts1;
	mtmo[0]=cb_timer_create("tm1");
	mtmo[1]=cb_timer_create("tm2");

	ts1=ub_rt_gettime64();
	cb_timer_start_interval(mtmo[0], 10, 10);
	cb_timer_start_interval(mtmo[1], 100, 100);
	UB_LOG(UBL_INFO, "%s:the first 9 times of 10 msec timer\n", __func__);
	for(i=0;i<9;i++){
		tms=10+i*10;
		assert_false(check_one_expire(mtmo[0], tms, ts1));
	}

	UB_LOG(UBL_INFO, "%s:the 10th time of 10 msec timer\n", __func__);
	assert_false(check_one_expire(mtmo[0], 100, ts1));

	UB_LOG(UBL_INFO, "%s:the first time of 100 msec timer\n", __func__);
	assert_true(cb_timer_expired(mtmo[1],CB_TIMER_ALL_CLEAR));

	for(i=0;i<5;i++){
		tms=110+i*10;
		UB_LOG(UBL_INFO, "%s:5 times of 10 msec timer after 100msec\n", __func__);
		assert_false(check_one_expire(mtmo[0], tms, ts1));
	}
	for(i=0;i<6;i++){
		UB_LOG(UBL_INFO, "%s:the 2nd time of 100 msec timer\n", __func__);
		tms=check_one_expire(mtmo[1], 200, ts1);
		if(tms==0){break;}
		if(tms==1){continue;}
		assert_true(false);
	}

	for(i=0;i<5;i++){
		UB_LOG(UBL_INFO, "%s:should have expired 5 times\n", __func__);
		assert_true(cb_timer_expired(mtmo[0], CB_TIMER_DECREMENT));
	}
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr="4,ubase:45,combase:45";
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
		cmocka_unit_test(unit_test1),
		cmocka_unit_test(unit_test2),
		cmocka_unit_test(unit_test3),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
