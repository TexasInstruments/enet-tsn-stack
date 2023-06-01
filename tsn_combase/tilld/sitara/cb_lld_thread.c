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
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/SemaphoreP.h>
#include <tsn_unibase/ub_getmem.h>
#include "combase_private.h"
#include "cb_thread.h"

#define CB_SEM_MAXCOUNT 0xFFU

struct cb_lld_mutex {
	SemaphoreP_Object lldmutex;
};

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
};

#define CB_LLDSEM_MMEM cb_lldsem
#ifndef CB_LLDSEM_INSTNUM
#define CB_LLDSEM_INSTNUM 10
#endif
UB_SD_GETMEM_DEF(CB_LLDSEM_MMEM, (int)sizeof(cb_lld_sem_t),
		 CB_LLDSEM_INSTNUM);

#define CB_LLDMUTEX_MMEM cb_lldmutex
#ifndef CB_LLDMUTEX_INSTNUM
#define CB_LLDMUTEX_INSTNUM 20
#endif
UB_SD_GETMEM_DEF(CB_LLDMUTEX_MMEM, (int)sizeof(cb_lld_mutex_t),
		 CB_LLDMUTEX_INSTNUM);

#define CB_LLDTASK_MMEM cb_lldtask
#ifndef CB_LLDTASK_INSTNUM
#define CB_LLDTASK_INSTNUM 10
#endif
UB_SD_GETMEM_DEF(CB_LLDTASK_MMEM, (int)sizeof(cb_lld_task_t),
		 CB_LLDTASK_INSTNUM);

int cb_lld_sem_init(CB_SEM_T *sem, int pshared, unsigned int value)
{
	cb_lld_sem_t *cbsem;

	cbsem = UB_SD_GETMEM(CB_LLDSEM_MMEM, sizeof(cb_lld_sem_t));
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

/* returns only 0 or -1 to make it compatible with the sem_timedwait() call.
 * to get wait_status please use #cb_lld_sem_wait_status()
 */
int lld_sem_trywait(cb_lld_sem_t *cbsem, uint32_t timeout_msec)
{
	int32_t status;

	status = SemaphoreP_pend(&cbsem->lldsem, timeout_msec);
	if ((status != SystemP_SUCCESS) && (status != SystemP_TIMEOUT)) {
		UB_LOG(UBL_ERROR,"%s:failed %d!\n", __func__, status);
		cbsem->wait_status = -1;
		return -1;
	}
	if (status == SystemP_TIMEOUT) {
		cbsem->wait_status = 0;
	} else {
		cbsem->wait_status = 1;
	}
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
	return lld_sem_trywait(*sem, 0);
}

static uint32_t abs_realtime_to_msec(struct timespec *abstime)
{
	uint32_t timeout_msec;
	uint64_t ts64;
	uint64_t now;

	now = ub_rt_gettime64();
	ts64 = abstime->tv_sec * UB_SEC_NS + abstime->tv_nsec;
	if (ts64 <= now) {
		timeout_msec = 0;
	} else {
		timeout_msec = (ts64 - now) / UB_MSEC_NS;
	}
	return timeout_msec;
}

int cb_lld_sem_timedwait(CB_SEM_T *sem, struct timespec *abstime)
{
	uint32_t timeout_msec = abs_realtime_to_msec(abstime);
	return lld_sem_trywait(*sem, timeout_msec);
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

int cb_lld_mutex_init(CB_THREAD_MUTEX_T *mutex, CB_THREAD_MUTEXATTR_T attr)
{
	cb_lld_mutex_t *cbmutex;

	cbmutex = UB_SD_GETMEM(CB_LLDMUTEX_MMEM, sizeof(cb_lld_mutex_t));
	if (cbmutex == NULL) {
		UB_LOG(UBL_ERROR,"%s:failed to get mem!\n", __func__);
		return -1;
	}
	memset(cbmutex, 0, sizeof(cb_lld_mutex_t));
	if(SemaphoreP_constructMutex(&cbmutex->lldmutex) != SystemP_SUCCESS) {
		UB_LOG(UBL_ERROR,"%s:failed to create mutex!\n", __func__);
		goto error;
	}
	*mutex = cbmutex;
	return 0;
error:
	cb_lld_mutex_destroy(&cbmutex);
	return -1;
}

int cb_lld_mutex_destroy(CB_THREAD_MUTEX_T *mutex)
{
	cb_lld_mutex_t *cbmutex = *mutex;

	SemaphoreP_destruct(&cbmutex->lldmutex);
	UB_SD_RELMEM(CB_LLDMUTEX_MMEM, cbmutex);

	return 0;
}

static int lld_mutex_trylock(cb_lld_mutex_t *cbmutex, uint32_t timeout_msec)
{
	int32_t status = SystemP_FAILURE;
	status = SemaphoreP_pend(&cbmutex->lldmutex, timeout_msec);

	if ((status != SystemP_SUCCESS) && (status != SystemP_FAILURE)) {
		UB_LOG(UBL_ERROR,"%s:failed %d!\n", __func__, status);
		return -1;
	}
	return 0;
}

int cb_lld_mutex_lock(CB_THREAD_MUTEX_T *mutex)
{
	return lld_mutex_trylock(*mutex, SystemP_WAIT_FOREVER);
}

int cb_lld_mutex_unlock(CB_THREAD_MUTEX_T *mutex)
{
	cb_lld_mutex_t *cbmutex = *mutex;
	SemaphoreP_post(&cbmutex->lldmutex);
	return 0;
}

int cb_lld_mutex_trylock(CB_THREAD_MUTEX_T *mutex)
{
	return lld_mutex_trylock(*mutex, 0);
}

int cb_lld_mutex_timedlock(CB_THREAD_MUTEX_T *mutex, struct timespec *abstime)
{
	uint32_t timeout_msec = abs_realtime_to_msec(abstime);
	return lld_mutex_trylock(*mutex, timeout_msec);
}

// WARNING! The following function will be called inside the API
// `ub_protected_func` which shares the same global mutex named `gmutex`
// with log module in the unibase library. Thus, calling any unibase
// log macro (ie. UB_LOG) inside this function will cause a DEADLOCK.
int cb_lld_global_mutex_init(void *mutex)
{
	cb_lld_mutex_t **mt = (cb_lld_mutex_t **)mutex;
	cb_lld_mutex_t *cbmutex;

	if(*mt){ return 0; }
	cbmutex = UB_SD_GETMEM(CB_LLDMUTEX_MMEM, sizeof(cb_lld_mutex_t));
	if (cbmutex == NULL) {
		return -1;
	}
	memset(cbmutex, 0, sizeof(cb_lld_mutex_t));
	if(SemaphoreP_constructMutex(&cbmutex->lldmutex) != SystemP_SUCCESS) {
		goto error;
	}
	*mt = cbmutex;
	return 0;
error:
	cb_lld_mutex_destroy(&cbmutex);
	return -1;
}

static void task_fxn(void* a0)
{
	cb_lld_task_t *cbtask = a0;
	cbtask->func(cbtask->arg);
	cbtask->exit = true;
}

static int cb_lld_task_destroy(cb_lld_task_t *cbtask)
{
	TaskP_destruct(&cbtask->lldtask);
	UB_SD_RELMEM(CB_LLDTASK_MMEM, cbtask);
	return 0;
}

int cb_lld_task_create(CB_THREAD_T *th, void *vattr, void *(*func)(void*), void *arg)
{
	TaskP_Params param;
	cb_lld_task_t *cbtask;
	cb_tsn_thread_attr_t *attr = vattr;
	int32_t status;

	cbtask = UB_SD_GETMEM(CB_LLDTASK_MMEM, sizeof(cb_lld_task_t));
	if (cbtask == NULL) {
		UB_LOG(UBL_ERROR,"%s:failed to get mem!\n", __func__);
		return -1;
	}

	TaskP_Params_init(&param);
	if (attr != NULL) {
		param.name = attr->name;
		if (attr->stack_size > 0) {
			param.stackSize = attr->stack_size;
		}
		if (attr->pri > 0) {
			param.priority = attr->pri;
		}
		if (attr->stack_addr != NULL) {
			param.stack = attr->stack_addr;
		}
	}
	cbtask->func = func;
	param.taskMain = &task_fxn;
	cbtask->arg = arg;
	cbtask->exit = false;
	param.args = cbtask;

	status = TaskP_construct(&cbtask->lldtask, &param);
	if (status != SystemP_SUCCESS) {
		UB_LOG(UBL_ERROR,"%s:failed to TaskP_construct()!\n", __func__);
		goto error;
	}
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
		UB_LOG(UBL_INFO,"%s:wait for task exit\n", __func__);
	}
	return cb_lld_task_destroy(th);
}

void cb_lld_task_exit(void *retval)
{
	UB_LOG(UBL_WARN,"%s:don't support\n", __func__);
}
