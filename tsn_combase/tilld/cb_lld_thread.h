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
/**
 * @defgroup lld_thread TI ENET LLD thread specific
 * @{
 * @file cb_lld_thread.h
 *
 * @brief bindings to TI ENET LLD specific thread, mutex and semaphore functions
 */
#ifndef CB_LLD_THREAD_H
#define CB_LLD_THREAD_H

#include <stdint.h>
#include <string.h>
#include <time.h>
#include "lld_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cb_lld_sem cb_lld_sem_t;
typedef struct cb_lld_task cb_lld_task_t;

#define TILLD_SUCCESS       (0)
#define TILLD_FAILURE       (-1)
#define TILLD_TIMEDOUT      (-2)

#define CB_THREAD_T cb_lld_task_t*
#define CB_THREAD_CREATE cb_lld_task_create
#define CB_THREAD_JOIN cb_lld_task_join
#define CB_THREAD_EXIT cb_lld_task_exit
#define CB_THREAD_MUTEX_T cb_lld_mutex_t
#define CB_THREAD_MUTEX_LOCK cb_lld_mutex_lock
#define CB_THREAD_MUTEX_TRYLOCK cb_lld_mutex_trylock
#define CB_THREAD_MUTEX_TIMEDLOCK cb_lld_mutex_timedlock
#define CB_THREAD_MUTEX_UNLOCK cb_lld_mutex_unlock
#define CB_THREAD_MUTEX_INIT cb_lld_mutex_init
#define CB_THREAD_MUTEX_DESTROY cb_lld_mutex_destroy
#define CB_THREAD_IS_MUTEX_INITIALIZED(x) ((x).lldmutex!=NULL)
#define CB_STATIC_MUTEX_INITIALIZER(x) x=TILLD_MUTEX_INITIALIZER
#define CB_STATIC_MUTEX_CONSTRUCTOR(x) \
	UB_PROTECTED_FUNC_VOID(cb_lld_mutex_init_protect,&(x))
#define CB_STATIC_MUTEX_DESTRUCTOR(x) \
	UB_PROTECTED_FUNC_VOID(cb_lld_mutex_destroy_protect, &(x))

/* does not need to support these macros */
#define CB_THREAD_MUTEXATTR_T void*
#define CB_THREAD_MUTEXATTR_INIT(a)
#define CB_THREAD_MUTEXATTR_SETPSHARED(a,b)
#define CB_THREAD_PROCESS_SHARED 0 //PTHREAD_PROCESS_SHARED

#define CB_SEM_T cb_lld_sem_t*
#define CB_SEM_INIT cb_lld_sem_init
#define CB_SEM_GETVALUE cb_lld_sem_getvalue
#define CB_SEM_WAIT cb_lld_sem_wait
#define CB_SEM_TRYWAIT cb_lld_sem_trywait
#define CB_SEM_TIMEDWAIT cb_lld_sem_timedwait
#define CB_SEM_POST cb_lld_sem_post
#define CB_SEM_DESTROY cb_lld_sem_destroy

/* does not support condition variable */
#define CB_THREAD_COND_T void*
#define CB_THREAD_COND_INIT(a, b)
#define CB_THREAD_COND_DESTROY(a)
#define CB_THREAD_COND_WAIT(a, b)
#define CB_THREAD_COND_SIGNAL(a)
#define CB_THREAD_COND_BROADCAST(a)

/* Implement the same prototype as POSIX APIs */
/**
 * @brief Initializes a semaphore.
 *
 * @param sem Pointer to the semaphore.
 * @param pshared Indicates whether the semaphore is shared among processes.
 * @param value The initial value of the semaphore.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_sem_init(CB_SEM_T *sem, int pshared, unsigned int value);

/**
 * @brief Waits on a semaphore until it becomes available.
 *
 * @param sem Pointer to the semaphore.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_sem_wait(CB_SEM_T *sem);

/**
 * @brief Attempts to wait on a semaphore without blocking.
 * To know if the semaphore was acquired, get the return value from
 * the @ref cb_lld_sem_wait_status().
 * If the return value is 1, then the semaphore was acquired.
 * Otherwise, the semaphore was not acquired.
 *
 * @param sem Pointer to the semaphore.
 * @return 0 if no error, or -1 on error.
 */
int cb_lld_sem_trywait(CB_SEM_T *sem);

/**
 * @brief Waits on a semaphore until it becomes available or until a timeout occurs.
 * To know if the semaphore was acquired, refer to the @ref cb_lld_sem_wait_status().
 *
 * @param sem Pointer to the semaphore.
 * @param abstime Pointer to the absolute timeout value.
 * @return 0 if no error, or -1 on error.
 */
int cb_lld_sem_timedwait(CB_SEM_T *sem, struct timespec *abstime);

/**
 * @brief Posts (signals) a semaphore, releasing a waiting thread.
 *
 * @param sem Pointer to the semaphore.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_sem_post(CB_SEM_T *sem);

/**
 * @brief return the count of a semaphore
 *
 * @param sem Pointer to the semaphore.
 * @param sval return value of semaphore counter
 * @return 0 on success, or -1 on error.
 */
int cb_lld_sem_getvalue(CB_SEM_T* sem, int* sval);

/**
 * @brief Destroys a semaphore.
 *
 * @param sem Pointer to the semaphore.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_sem_destroy(CB_SEM_T *sem);

/**
 * @brief Get the semaphore wait status.
 * The return value of this function determines the wait status of
 * @ref cb_lld_sem_wait(), @ref cb_lld_sem_trywait() and @ref cb_lld_sem_timedwait()
 *
 * @param sem Pointer to the semaphore.
 * @return TILLD_SUCCESS: semaphore was acquired,
 * TILLD_FAILURE: on failure,
 * TILLD_TIMEDOUT: on timeout.
 */
int cb_lld_sem_wait_status(CB_SEM_T *sem);

/**
 * @brief Initializes a mutex.
 *
 * @param mutex Pointer to the mutex.
 * @param attr Pointer to the mutex attributes.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_mutex_init(CB_THREAD_MUTEX_T *mutex, CB_THREAD_MUTEXATTR_T attr);

/**
 * @brief Initializes a mutex that will be protected by the @ref UB_PROTECTED_FUNC_VOID().
 *
 * This function is primarily designed for initializing a mutex used in conjunction
 * with the @ref UB_PROTECTED_FUNC_VOID(). For general use cases, consider utilizing
 * the @ref cb_lld_mutex_init() function instead.
 *
 * @param mutex Pointer to a mutex of type CB_THREAD_MUTEX_T.
 * @return 0 on success, or -1 on error.
 */
static inline int cb_lld_mutex_init_protect(void *mutex)
{
	if(((CB_THREAD_MUTEX_T *)mutex)->lldmutex) {return 0;}
	return cb_lld_mutex_init((CB_THREAD_MUTEX_T *)mutex, NULL);
}

/**
 * @brief Destroys a mutex.
 *
 * @param mutex Pointer to the mutex.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_mutex_destroy(CB_THREAD_MUTEX_T *mutex);

/**
 * @brief Destroy a mutex that will be protected by the @ref UB_PROTECTED_FUNC_VOID().
 *
 * This function is primarily designed for destroying a mutex used in conjunction
 * with the @ref UB_PROTECTED_FUNC_VOID(). For general use cases, consider utilizing
 * the @ref cb_lld_mutex_destroy() function instead.
 *
 * @param mutex Pointer to a mutex of type CB_THREAD_MUTEX_T.
 */
static inline void cb_lld_mutex_destroy_protect(void *mutex)
{
	CB_THREAD_MUTEX_T *cbmutex = (CB_THREAD_MUTEX_T *)mutex;
	if(cbmutex->lldmutex == NULL) {return;}
	cb_lld_mutex_destroy(cbmutex);
	cbmutex->lldmutex = NULL;
}

/**
 * @brief Locks a mutex.
 *
 * @param mutex Pointer to the mutex.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_mutex_lock(CB_THREAD_MUTEX_T *mutex);

/**
 * @brief Unlocks a mutex.
 *
 * @param mutex Pointer to the mutex.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_mutex_unlock(CB_THREAD_MUTEX_T *mutex);

/**
 * @brief Attempts to lock a mutex without blocking.
 *
 * @param mutex Pointer to the mutex.
 * @return 0 on success, or -1 on error.
 */
int cb_lld_mutex_trylock(CB_THREAD_MUTEX_T *mutex);
/**
 * @brief Locks a mutex and waits until a timeout occurs.
 *
 * @param mutex Pointer to the mutex.
 * @param abstime Pointer to the absolute timeout value.
 * @return 0 on success, -1 on error
 */
int cb_lld_mutex_timedlock(CB_THREAD_MUTEX_T *mutex, struct timespec *abstime);

/**
 * @brief Creates a new thread.
 * This function creates a new thread with the specified attributes and starts executing
 * the specified function with the provided argument.
 * @param th Pointer to a thread object that will be initialized with the created thread.
 * @param attr Thread attributes.
 * @param func Pointer to the function that will be executed by the thread.
 * @param arg Argument to be passed to the thread function.
 * @return 0 on success, -1 on error
 */
int cb_lld_task_create(CB_THREAD_T *th, void *attr, void *(*func)(void*), void *arg);

/**
 * @brief Waits for a thread to terminate.
 * This function waits for the specified thread to terminate.
 * @param th Thread to be joined.
 * @param retval Pointer to a pointer that will be set to the exit status of
 * the joined thread (unsupported).
 * @return 0 on success, -1 on error
 */
int cb_lld_task_join(CB_THREAD_T th, void **retval);

/**
 * @brief Terminates the calling thread.
 * @param retval Exit value of the thread (unsupported).
 */
void cb_lld_task_exit(void *retval);

#ifdef __cplusplus
}
#endif

#endif //CB_LLD_THREAD_H

/** @}*/
