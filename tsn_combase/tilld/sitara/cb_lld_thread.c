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
/*
 * cb_lld_thread.c
*/
#include <stdlib.h>
#include <stdbool.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/SemaphoreP.h>
#include "combase_private.h"
#include "cb_thread.h"

#define CB_SEM_MAXCOUNT 0xFFU

struct cb_lld_sem {
	SemaphoreP_Object lldsem;
	/* -1: error, 0: timedout, 1: wait done */
	int wait_status;
};

struct cb_lld_task {
	TaskP_Object lldtask;
	void *(*func)(void*);
	void *arg;
	bool exit;
	uint8_t *stack_alloc;
	char name[CB_TSN_THREAD_NAME_SIZE];
};

#define CB_LLDSEM_MMEM cb_lldsem
#ifndef CB_LLDSEM_INSTNUM
#define CB_LLDSEM_INSTNUM 12
#endif
UB_SD_GETMEM_DEF(CB_LLDSEM_MMEM, (int)sizeof(cb_lld_sem_t),
		 CB_LLDSEM_INSTNUM);

#define CB_LLDTASK_MMEM cb_lldtask
#ifndef CB_LLDTASK_INSTNUM
#define CB_LLDTASK_INSTNUM 10
#endif
UB_SD_GETMEM_DEF(CB_LLDTASK_MMEM, (int)sizeof(cb_lld_task_t),
		 CB_LLDTASK_INSTNUM);

#define CB_LLDTASK_STACK_MMEM cb_lldtask_stack
#ifndef CB_LLDTASK_STACK_INSTNUM
#define CB_LLDTASK_STACK_INSTNUM 4
#endif
#if _DEBUG_ == 1
#define CB_LLDTASK_STACK_SIZE (16*1024)
#else
#define CB_LLDTASK_STACK_SIZE (8*1024)
#endif

UB_SD_GETMEM_DEF(CB_LLDTASK_STACK_MMEM, CB_LLDTASK_STACK_SIZE,
		CB_LLDTASK_STACK_INSTNUM)

int cb_lld_sem_init(CB_SEM_T *sem, int pshared, unsigned int value)
{
	cb_lld_sem_t *cbsem;

	cbsem = (cb_lld_sem_t*)UB_SD_GETMEM(CB_LLDSEM_MMEM, sizeof(cb_lld_sem_t));
	if (cbsem == NULL) {
		UB_LOG(UBL_ERROR,"%s:failed to get mem!\n", __func__);
		return -1;
	}
	if (SemaphoreP_constructCounting(&cbsem->lldsem, value,
				CB_SEM_MAXCOUNT) != SystemP_SUCCESS) {
		UB_LOG(UBL_ERROR,"%s:failed to create sem!\n", __func__);
		goto error;
	}
	*sem = cbsem;
	return 0;
error:
	cb_lld_sem_destroy(&cbsem);
	return -1;
}

static int lld_sem_trywait(cb_lld_sem_t *cbsem, uint32_t timeout_tick)
{
	int32_t status;

	status = SemaphoreP_pend(&cbsem->lldsem, timeout_tick);
	if (status != SystemP_SUCCESS) {
		cbsem->wait_status = (status == SystemP_TIMEOUT)?
			TILLD_TIMEDOUT: TILLD_FAILURE;
		if (status != SystemP_TIMEOUT) {
			UB_LOG(UBL_ERROR,"%s:failed %d!\n", __func__, status);
		}
		return -1;
	}
	cbsem->wait_status = TILLD_SUCCESS;

	return 0;
}

int cb_lld_sem_wait_status(CB_SEM_T *sem)
{
	cb_lld_sem_t *cbsem = *sem;
	return cbsem->wait_status;
}


int cb_lld_sem_wait(CB_SEM_T *sem)
{
	return lld_sem_trywait(*sem, SystemP_WAIT_FOREVER);
}

int cb_lld_sem_trywait(CB_SEM_T *sem)
{
	return lld_sem_trywait(*sem, SystemP_NO_WAIT);
}

static uint32_t abs_realtime_to_tick(struct timespec *abstime)
{
	uint32_t timeout_usec;
	uint64_t ts64;
	uint64_t now;

	now = ub_rt_gettime64();
	ts64 = abstime->tv_sec * UB_SEC_NS + abstime->tv_nsec;
	if (ts64 <= now) {
		return 0;
	}
	timeout_usec = (ts64 - now) / UB_USEC_NS;
	return ClockP_usecToTicks(timeout_usec);
}

int cb_lld_sem_timedwait(CB_SEM_T *sem, struct timespec *abstime)
{
	uint32_t timeout_tick= abs_realtime_to_tick(abstime);
	return lld_sem_trywait(*sem, timeout_tick);
}

int cb_lld_sem_getvalue(CB_SEM_T* sem, int* sval)
{
	cb_lld_sem_t *cbsem = *sem;
	*sval = SemaphoreP_getCount(&cbsem->lldsem);
	return 0; // OK
}

/* don't add log to this function since it can be called from an ISR */
int cb_lld_sem_post(CB_SEM_T *sem)
{
	cb_lld_sem_t *cbsem = *sem;
	SemaphoreP_post(&cbsem->lldsem);

	return 0;
}

int cb_lld_sem_destroy(CB_SEM_T *sem)
{
	cb_lld_sem_t *cbsem = *sem;
	SemaphoreP_destruct(&cbsem->lldsem);
	UB_SD_RELMEM(CB_LLDSEM_MMEM, cbsem);
	return 0;
}

int cb_lld_mutex_init(CB_THREAD_MUTEX_T *cbmutex, CB_THREAD_MUTEXATTR_T attr)
{
	memset(cbmutex, 0, sizeof(cb_lld_mutex_t));
	if(SemaphoreP_constructMutex(&cbmutex->mobj) != SystemP_SUCCESS) {
		UB_LOG(UBL_ERROR,"%s:failed to create mutex!\n", __func__);
		goto error;
	}
	cbmutex->lldmutex = &cbmutex->mobj;
	return 0;
error:
	cb_lld_mutex_destroy(cbmutex);
	return -1;
}

int cb_lld_mutex_destroy(CB_THREAD_MUTEX_T *cbmutex)
{
	if (cbmutex->lldmutex != NULL) {
		SemaphoreP_destruct(cbmutex->lldmutex);
		cbmutex->lldmutex = NULL;
	}
	return 0;
}

static int lld_mutex_trylock(CB_THREAD_MUTEX_T *mutex, uint32_t timeout_tick)
{
	int32_t status = SystemP_FAILURE;
	status = SemaphoreP_pend(mutex->lldmutex, timeout_tick);

	if (status != SystemP_SUCCESS) {
		if (status != SystemP_TIMEOUT) {
			UB_LOG(UBL_ERROR,"%s:failed %d!\n", __func__, status);
		}
		return -1;
	}
	return 0;
}

int cb_lld_mutex_lock(CB_THREAD_MUTEX_T *mutex)
{
	return lld_mutex_trylock(mutex, SystemP_WAIT_FOREVER);
}

int cb_lld_mutex_unlock(CB_THREAD_MUTEX_T *mutex)
{
	SemaphoreP_post(mutex->lldmutex);
	return 0;
}

int cb_lld_mutex_trylock(CB_THREAD_MUTEX_T *mutex)
{
	return lld_mutex_trylock(mutex, SystemP_NO_WAIT);
}

int cb_lld_mutex_timedlock(CB_THREAD_MUTEX_T *mutex, struct timespec *abstime)
{
	uint32_t timeout_tick = abs_realtime_to_tick(abstime);
	return lld_mutex_trylock(mutex, timeout_tick);
}

static void task_fxn(void* a0)
{
	cb_lld_task_t *cbtask = (cb_lld_task_t*)a0;
	cbtask->func(cbtask->arg);
	cbtask->exit = true;
	TaskP_exit(); /* This function is required, otherwise an assertion is raised  */
}

static int cb_lld_task_destroy(cb_lld_task_t *cbtask)
{
	TaskP_destruct(&cbtask->lldtask);
	if (cbtask->stack_alloc != NULL) {
		UB_SD_RELMEM(CB_LLDTASK_STACK_MMEM, cbtask->stack_alloc);
	}
	UB_SD_RELMEM(CB_LLDTASK_MMEM, cbtask);
	return 0;
}

int cb_lld_task_create(CB_THREAD_T *th, void *vattr, void *(*func)(void*), void *arg)
{
	TaskP_Params param;
	cb_lld_task_t *cbtask;
	cb_tsn_thread_attr_t *attr = (cb_tsn_thread_attr_t*)vattr;
	int32_t status;

	cbtask = (cb_lld_task_t*)UB_SD_GETMEM(CB_LLDTASK_MMEM, sizeof(cb_lld_task_t));
	if (cbtask == NULL) {
		UB_LOG(UBL_ERROR,"%s:failed to get mem!\n", __func__);
		return -1;
	}
	memset(cbtask, 0, sizeof(cb_lld_task_t));

	TaskP_Params_init(&param);
	if (attr != NULL) {
		param.name = attr->name;
		if (attr->pri > 0) {
			param.priority = attr->pri;
		}
		if (attr->stack_size > 0) {
			param.stackSize = attr->stack_size;
		}
		if (attr->stack_addr != NULL) {
			param.stack = (uint8_t*)attr->stack_addr;
		}
	}
	if (param.stack == NULL) {
		cbtask->stack_alloc = (uint8_t*)
			UB_SD_GETMEM(CB_LLDTASK_STACK_MMEM, CB_LLDTASK_STACK_SIZE);
		if (cbtask->stack_alloc == NULL) {
			UB_LOG(UBL_ERROR,"%s:failed to get stack mem!\n", __func__);
			goto error;
		}
		param.stack = cbtask->stack_alloc;
		param.stackSize = CB_LLDTASK_STACK_SIZE;
	}
	cbtask->func = func;
	param.taskMain = &task_fxn;
	cbtask->arg = arg;
	cbtask->exit = false;
	param.args = cbtask;
	if (param.name == NULL) {
		param.name = "default_thread";
	}
	snprintf(cbtask->name, CB_TSN_THREAD_NAME_SIZE, "%s", param.name);

	status = TaskP_construct(&cbtask->lldtask, &param);
	if (status != SystemP_SUCCESS) {
		UB_LOG(UBL_ERROR,"%s:failed to TaskP_construct()!\n", __func__);
		cbtask->exit = true;
		goto error;
	}
	UB_LOG(UBL_INFO,"%s: %s stack_size=%d\n", __func__,
		   param.name, param.stackSize);
	*th = cbtask;
	return 0;
error:
	cb_lld_task_destroy(cbtask);
	return -1;
}

int cb_lld_task_join(CB_THREAD_T th, void **retval)
{
	while (th->exit == false) {
		ClockP_usleep(500000);
		UB_LOG(UBL_INFO,"%s:wait for task (%s) exit\n",
			   __func__, th->name);
	}
	return cb_lld_task_destroy(th);
}

void cb_lld_task_exit(void *retval)
{
	UB_LOG(UBL_WARN,"%s:don't support\n", __func__);
}
