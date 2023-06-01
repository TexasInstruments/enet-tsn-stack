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

static int num_timer_test=50;
static int ntimers=0;
static uint64_t timeout=50000;//us
static int timeout_diff=10000;//The diff in usec between two continous timer

#define MAX_LOOP_NSEC ((timeout+num_timer_test*timeout_diff+200000)*1000)
/*
 * Normally tolerant error 500usec is enough. However when a system is weak or CPU load is high
 * the tolerant error need to be a little bigger to make sure the test is always passed.
 * And 2msec should be enough.
*/
#define ERROR_TOLERANT_NSEC 2000000

struct test_data
{
	uint64_t start_time;
	uint64_t end_time;
	uint64_t timeout;
	cb_xtimer_t *timer;
};

static void test_start_stop(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);
	timer = cb_xtimer_create(timer_man, NULL, NULL);
	assert_non_null(timer);
	assert_false(cb_xtimer_is_running(timer));
	assert_false(cb_xtimer_is_periodic(timer));
	assert_int_equal(cb_xtimer_start(timer, 10), 0);
	assert_true(cb_xtimer_is_running(timer));
	cb_xtimer_stop(timer);
	assert_false(cb_xtimer_is_running(timer));
	cb_xtimer_delete(timer);
	cb_xtimer_man_delete(timer_man);
}

#define NUM_THREADS 100
#define TTHREAD_EXPIRE 10000 //10msec timer
static void test_multi_expire_cb(cb_xtimer_t *timer, void *arg)
{
	int *expired=(int*)arg;
	*expired=1;
}
static void *timer_one_thread(void *ptr)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int *tmres=(int*)ptr;
	uint64_t st;
	uint64_t et;
	int expired=0;
	int res;

	*tmres=-1;
	timer_man=cb_xtimer_man_create();
	if(timer_man==NULL) return NULL;
	timer = cb_xtimer_create(timer_man, test_multi_expire_cb, &expired);
	if(timer==NULL) return NULL;
	cb_xtimer_start(timer, TTHREAD_EXPIRE);
	st=ub_mt_gettime64();
	while(true){
		res=cb_xtimer_man_schedule(timer_man);
		if((ub_mt_gettime64()-st)>((TTHREAD_EXPIRE+10000lu)*1000lu)){break;}
		if(expired){break;}
		if(res<0){
			UB_LOG(UBL_ERROR, "%s:error in cb_xtimer_man_schedule, res=%d\n",
			       __func__, res);
			break;
		}
		usleep(res);
	}
	et=ub_mt_gettime64();
	cb_xtimer_delete(timer);
	cb_xtimer_man_delete(timer_man);
	if(expired==0){
		UB_LOG(UBL_ERROR, "%s:timer didn't expire\n", __func__);
		return NULL;
	}
	if((et-st)>((TTHREAD_EXPIRE+10000lu)*1000lu)){
		UB_LOG(UBL_ERROR, "%s:timer expired, but passed too long ts=%"PRIu64"usec\n",
		       __func__, (et-st)/1000lu);
		return NULL;
	}
	*tmres=0;
	return NULL;
}

static void test_multi_threads(void **state)
{
	int i;
	int j;
	CB_THREAD_T th[NUM_THREADS];
	int tmres[NUM_THREADS];
	for(j=0;j<10;j++){
		for(i=0;i<NUM_THREADS;i++){
			CB_THREAD_CREATE(&th[i], NULL, timer_one_thread, &tmres[i]);
		}
		for(i=0;i<NUM_THREADS;i++){
			assert_int_equal(CB_THREAD_JOIN(th[i], NULL), 0);
			assert_int_equal(tmres[i], 0);
		}
	}
}

static void test_time_expire_cb(cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_true(diff > data->timeout*1000u);
	assert_true(diff < data->timeout*1000u+ERROR_TOLERANT_NSEC);
	assert_int_equal(cb_xtimer_remain_timeout(timer), 0);
	assert_false(cb_xtimer_is_running(timer));
	assert_false(cb_xtimer_is_periodic(timer));
	ntimers++;
}

static void test_time_expire(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, test_time_expire_cb, &data[i]);
		assert_non_null(timer);
		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC){break;}
	}
	for(i=0; i<num_timer_test; i++){
		assert_false(cb_xtimer_is_running(data[i].timer));
		cb_xtimer_delete(data[i].timer);
	}
	assert_int_equal(ntimers, num_timer_test);
	cb_xtimer_man_delete(timer_man);
}

static void test_time_expire_periodic_option(void **state, xtimer_expirecb_t cb_option)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, cb_option, &data[i]);
		assert_non_null(timer);
		cb_xtimer_set_periodic(timer);
		assert_true(cb_xtimer_is_periodic(timer));
		cb_xtimer_clear_periodic(timer);
		assert_false(cb_xtimer_is_periodic(timer));
		cb_xtimer_set_periodic(timer);
		assert_true(cb_xtimer_is_periodic(timer));

		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC){break;}
	}
	for(i=0; i<num_timer_test; i++){
		assert_true(cb_xtimer_is_running(data[i].timer));
		cb_xtimer_delete(data[i].timer);
	}
	cb_xtimer_man_delete(timer_man);
}

static void test_time_expire_periodic_cb(cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_in_range(diff, data->timeout*1000u-ERROR_TOLERANT_NSEC,
					data->timeout*1000u+ERROR_TOLERANT_NSEC);
	data->start_time=data->end_time;
}

static void test_time_expire_periodic(void **state)
{
	test_time_expire_periodic_option(state, test_time_expire_periodic_cb);
}

static void test_restart_periodic_timer_incallback_cb(
	cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	assert_false(cb_xtimer_is_running(timer));

	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_in_range(diff, data->timeout*1000u-ERROR_TOLERANT_NSEC,
					data->timeout*1000u+ERROR_TOLERANT_NSEC);
	data->start_time=data->end_time;

	//User can start a timer inside a callback
	assert_int_equal(cb_xtimer_start(timer, data->timeout), 0);
}

static void test_restart_periodic_timer_incallback(void **state)
{
	test_time_expire_periodic_option(
		state, test_restart_periodic_timer_incallback_cb);
}

static void test_time_expire_periodic_mix(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, test_time_expire_periodic_cb, &data[i]);
		assert_non_null(timer);

		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;
		if(i%2==0){cb_xtimer_set_periodic(timer);}
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC){break;}
	}
	for(i=0; i<num_timer_test; i++){
		if(i%2==0){
			assert_true(cb_xtimer_is_running(data[i].timer));
		}else{
			assert_false(cb_xtimer_is_running(data[i].timer));
		}
		cb_xtimer_delete(data[i].timer);
	}
	cb_xtimer_man_delete(timer_man);
}

static void test_time_expire_periodic_stop_cb(cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_in_range(diff, data->timeout*1000u-ERROR_TOLERANT_NSEC,
					data->timeout*1000u+ERROR_TOLERANT_NSEC);
	data->start_time=data->end_time;
	if(ntimers%2==0){
		cb_xtimer_clear_periodic(timer);
	}else{
		cb_xtimer_stop(timer);
	}
	ntimers++;
}

static void test_time_expire_periodic_stop(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	ntimers=0;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, test_time_expire_periodic_stop_cb, &data[i]);
		assert_non_null(timer);

		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;

		cb_xtimer_set_periodic(timer);
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC){break;}
	}
	for(i=0; i<num_timer_test; i++){
		assert_false(cb_xtimer_is_running(data[i].timer));
		cb_xtimer_delete(data[i].timer);
	}
	cb_xtimer_man_delete(timer_man);
	assert_int_equal(ntimers, num_timer_test);
}

static void test_remain_timeout(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);
	timer = cb_xtimer_create(timer_man, NULL, NULL);
	assert_non_null(timer);

	assert_int_equal(cb_xtimer_start(timer, 100), 0);
	assert_true(cb_xtimer_remain_timeout(timer) <= 100);
	assert_true(cb_xtimer_remain_timeout(timer) >= 60);
	usleep(100);
	assert_int_equal(cb_xtimer_remain_timeout(timer), 0);
	cb_xtimer_delete(timer);
	cb_xtimer_man_delete(timer_man);
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

int main(int argc, char **argv)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_start_stop),
		cmocka_unit_test(test_time_expire),
		cmocka_unit_test(test_time_expire_periodic),
		cmocka_unit_test(test_restart_periodic_timer_incallback),
		cmocka_unit_test(test_time_expire_periodic_mix),
		cmocka_unit_test(test_time_expire_periodic_stop),
		cmocka_unit_test(test_remain_timeout),
		cmocka_unit_test(test_multi_threads),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
