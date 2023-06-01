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
#include <sched.h>
#define UNIT_TESTING
#include <cmocka.h>
#include "combase.h"
#include <tsn_unibase/unibase_binding.h>

#define UB_LOGCAT 2

#define WPNUM 2
cb_waitpoint_t wp[WPNUM];

#define THREAD_COUNT 12
#define ITERATIONS 64
#define DATALEN 100
int shared_data[DATALEN];
int errors=0;

static int setup(void **state){
	int i;
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr="4,ubase:45,combase:45,cbwp:55m";
	unibase_init(&init_para);

	for(i=0;i<WPNUM;i++){
		cb_waitpoint_init(&wp[i]);
	}
	return 0;
}

static int teardown(void **state){
	int i;
	for(i=0;i<WPNUM;i++){
		cb_waitpoint_deinit(&wp[i]);
	}
	unibase_close();
	return 0;
}

void nonatomic_operation(){
	int i;
	memset(&shared_data, 0, sizeof(shared_data));
	for(i=0; i<DATALEN; i++){
		// allow context switch
		nanosleep((const struct timespec[]){{0, 10}}, NULL);
		// set data value to 1 for summation to 100 in check_shared()
		shared_data[i]=1;
	}
}

bool check_shared(){
	int total, i;
	for(i=0,total=0; i<DATALEN; i++){
		total+= shared_data[i];
	}
	return total==DATALEN?true:false;
}

static void *thread_op1(void *arg){
	int i;
	for(i=0; i<ITERATIONS; i++){
		cb_waitpoint_lock(&wp[0]);
		nonatomic_operation();
		if(!check_shared()){
			// increment error count when check_shared returns false
			errors++;
		}
		cb_waitpoint_unlock(&wp[0]);
	}
	return NULL;
}

static void *thread_corrupt1(void *arg){
	int i;
	for(i=0; i<ITERATIONS; i++){
		cb_waitpoint_lock(&wp[0]);
		memset(&shared_data, 0xFF, sizeof(shared_data));
		cb_waitpoint_unlock(&wp[0]);
	}
	return NULL;
}

static void *thread_wait_pingpong(void *arg){
	int i, id=*(int *)arg;
	int dest = 1-id;

	for(i=0; i<ITERATIONS; i++){
		// act as the sender of signal
		cb_waitpoint_lock(&wp[dest]);
		shared_data[dest]=id;
		UB_LOG(UBL_INFO, "%s: [%d](%d/%d) sending wake-up signal...\n", __func__, id, i, ITERATIONS);
		cb_waitpoint_wakeup(&wp[dest]);
		cb_waitpoint_unlock(&wp[dest]);
		if(i<ITERATIONS-1){
			// act as the receiver of signal
			cb_waitpoint_lock(&wp[id]);
			while(shared_data[id]!=dest){
				UB_LOG(UBL_INFO, "%s: [%d](%d/%d) waiting for wake-up signal...\n", __func__, id, i, ITERATIONS);
				cb_waitpoint_wakeup_at(&wp[id], 0 /* can wake up anytime */, true);
				// blocks until cb_waitpoint_wakeup is called
				UB_LOG(UBL_INFO, "%s: [%d](%d/%d) received wake-up signal...\n", __func__, id, i, ITERATIONS);
			}
			shared_data[id]=id;
			cb_waitpoint_unlock(&wp[id]);
		}
	}
	return NULL;
}

static void *thread_timedwait_pingpong(void *arg){
	int i, id=*(int *)arg;
	int dest = 1-id;
	uint64_t t1, t2;

	for(i=0; i<ITERATIONS; i++){
		// act as the sender of signal
		cb_waitpoint_lock(&wp[dest]);
		shared_data[dest]=id;
		UB_LOG(UBL_INFO, "%s: [%d](%d/%d) sending wake-up signal...\n", __func__, id, i, ITERATIONS);
		cb_waitpoint_wakeup(&wp[dest]);

		cb_waitpoint_unlock(&wp[dest]);
		if(i<ITERATIONS-1){
			// act as the receiver of signal
			cb_waitpoint_lock(&wp[id]);
			while(shared_data[id]!=dest){
				UB_LOG(UBL_INFO, "%s: [%d](%d/%d) waiting for wake-up signal...\n", __func__, id, i, ITERATIONS);
				t1 = ub_rt_gettime64();
				cb_waitpoint_wakeup_at(&wp[id], t1+2*UB_MSEC_NS, true);
				// blocks until cb_waitpoint_wakeup is called
				while(!cb_waitpoint_check(&wp[id], ub_rt_gettime64())){
					nanosleep((const struct timespec[]){{0, 10}}, NULL);
				}
				UB_LOG(UBL_INFO, "%s: [%d](%d/%d) received wake-up signal...\n", __func__, id, i, ITERATIONS);
				t2 = ub_rt_gettime64();
				if(t1>=t2){
					UB_LOG(UBL_ERROR, "%s: [%d](%d/%d) t1 is greater...\n", __func__, id, i, ITERATIONS);
					errors++;
				}
				if(t2-t1<2*UB_MSEC_NS){
					errors++;
				}
			}
			shared_data[id]=id;
			cb_waitpoint_unlock(&wp[id]);
		}
	}
	return NULL;
}

static void *thread_wait_signal(void *arg){
	int id = *((int *)arg);
	cb_waitpoint_lock(&wp[0]);
	UB_LOG(UBL_INFO, "%s: [%d] waiting for wake-up signal...\n", __func__, id);
	errors++;
	cb_waitpoint_wakeup_at(&wp[0], ub_gptp_gettime64(), true);
	errors--;
	UB_LOG(UBL_INFO, "%s: [%d] received wake-up signal...\n", __func__, id);
	cb_waitpoint_unlock(&wp[0]);
	return NULL;
}

static void test_race_condition(void **state){
	CB_THREAD_T th[THREAD_COUNT];
	int i;

	pthread_attr_t tattr;
	int newprio = THREAD_COUNT;
	struct sched_param param;

	for(i=0; i<THREAD_COUNT; i++){
		pthread_attr_init (&tattr);
		pthread_attr_getschedparam (&tattr, &param);
		// priority of the next thread should be higher to increase
		// chance of race condition
		param.sched_priority = newprio--;
		pthread_attr_setschedparam (&tattr, &param);

		if(i%2==0){
			CB_THREAD_CREATE(&th[i], &tattr, thread_op1, NULL);
		}else{
			CB_THREAD_CREATE(&th[i], &tattr, thread_corrupt1, NULL);
		}
	}

	for(i=0; i<THREAD_COUNT; i++){
		CB_THREAD_JOIN(th[i], NULL);
	}
	assert_int_equal(errors, 0);
}

static void test_ping_pong(void **state){
	CB_THREAD_T ping;
	CB_THREAD_T pong;
	int id1=0, id2=1;
	CB_THREAD_CREATE(&ping, NULL, thread_wait_pingpong, &id1);
	CB_THREAD_CREATE(&pong, NULL, thread_wait_pingpong, &id2);

	CB_THREAD_JOIN(ping, NULL);
	CB_THREAD_JOIN(pong, NULL);
	assert_int_equal(errors, 0);
}

static void test_timed_ping_pong(void **state){
	CB_THREAD_T ping;
	CB_THREAD_T pong;
	int id1=0, id2=1;

	CB_THREAD_CREATE(&ping, NULL, thread_timedwait_pingpong, &id1);
	CB_THREAD_CREATE(&pong, NULL, thread_timedwait_pingpong, &id2);

	CB_THREAD_JOIN(ping, NULL);
	CB_THREAD_JOIN(pong, NULL);
	assert_int_equal(errors, 0);
}

static void test_waitpoint_broadcast(void **state){
	CB_THREAD_T th[THREAD_COUNT];
	int i;
	for(i=0; i<THREAD_COUNT; i++){
		CB_THREAD_CREATE(&th[i], NULL, thread_wait_signal, &errors);
	}
	// ensure sequence
	while(errors!=THREAD_COUNT) nanosleep((const struct timespec[]){{0, 10000}}, NULL);
	// broadcast to all threads to continue
	cb_waitpoint_broadcast(&wp[0]);
	for(i=0; i<THREAD_COUNT; i++){
		CB_THREAD_JOIN(th[i], NULL);
	}
	assert_int_equal(errors, 0);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_race_condition),
		cmocka_unit_test(test_ping_pong),
		cmocka_unit_test(test_timed_ping_pong),
		cmocka_unit_test(test_waitpoint_broadcast),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
