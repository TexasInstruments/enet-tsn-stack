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
 * @ingroup  TSN_COMBASE_MODULE
 * @defgroup thread Thread functions binding
 * @{
 * @file cb_thread.h
 *
 * @brief Bindings to POSIX thread, mutex and semaphore functions
 */

#ifndef CB_THREAD_H_
#define CB_THREAD_H_

#include <tsn_unibase/unibase.h>

/**
 * @brief data structure for thread attributes
 */
#define CB_TSN_THREAD_NAME_SIZE 128

/**
 * @brief parameters to initialize thread
 */
typedef struct cb_tsn_thread_attr{
	int pri;
        /**< thread priority */
	int stack_size;
        /**< stack size */
	char name[CB_TSN_THREAD_NAME_SIZE];
        /**< thread name */
	void *stack_addr;
        /**< stack address */
} cb_tsn_thread_attr_t;

#ifdef CB_THREAD_NON_POSIX_H
/* non-posix platforms need to support necessary POSIX compatible
 * functions and types which are defined as CB_* macros below.
 * And provide them in a header file defined as CB_THREAD_NON_POSIX_H */
#include CB_THREAD_NON_POSIX_H
#else
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define CB_THREAD_T pthread_t
#define CB_THREAD_CREATE(th, attr, func, arg) pthread_create(th, NULL, func, arg)
#define CB_THREAD_JOIN pthread_join
#define CB_THREAD_EXIT pthread_exit
#define CB_THREAD_MUTEX_T pthread_mutex_t
#define CB_THREAD_MUTEX_LOCK pthread_mutex_lock
#define CB_THREAD_MUTEX_TRYLOCK pthread_mutex_trylock
#define CB_THREAD_MUTEX_TIMEDLOCK pthread_mutex_timedlock
#define CB_THREAD_MUTEX_UNLOCK pthread_mutex_unlock
#define CB_THREAD_MUTEX_INIT pthread_mutex_init
#define CB_THREAD_MUTEX_DESTROY pthread_mutex_destroy
#define CB_THREAD_MUTEXATTR_T pthread_mutexattr_t
#define CB_THREAD_MUTEXATTR_INIT pthread_mutexattr_init
#define CB_THREAD_MUTEXATTR_SETPSHARED pthread_mutexattr_setpshared
#define CB_THREAD_PROCESS_SHARED PTHREAD_PROCESS_SHARED
#define CB_THREAD_PROCESS_PRIVATE PTHREAD_PROCESS_PRIVATE
#define CB_THREAD_COND_T pthread_cond_t
#define CB_THREAD_COND_INIT pthread_cond_init
#define CB_THREAD_COND_DESTROY pthread_cond_destroy
#define CB_THREAD_COND_WAIT pthread_cond_wait
#define CB_THREAD_COND_SIGNAL pthread_cond_signal
#define CB_THREAD_COND_BROADCAST pthread_cond_broadcast
#if !defined(CB_STATIC_MUTEX_INITIALIZER) && defined(PTHREAD_MUTEX_INITIALIZER)
#define CB_STATIC_MUTEX_INITIALIZER(x) x=PTHREAD_MUTEX_INITIALIZER
#define CB_STATIC_MUTEX_CONSTRUCTOR(x)
#define CB_STATIC_MUTEX_DESTRUCTOR(x)
#endif
#define CB_SEM_T sem_t
#define CB_SEM_INIT sem_init
#define CB_SEM_GETVALUE sem_getvalue
#define CB_SEM_WAIT sem_wait
#define CB_SEM_TRYWAIT sem_trywait
#define CB_SEM_TIMEDWAIT sem_timedwait
#define CB_SEM_POST sem_post
#define CB_SEM_DESTROY sem_destroy
#define CB_SEM_OPEN sem_open
#define CB_SEM_CLOSE sem_close
#define CB_SEM_UNLINK sem_unlink
#define CB_SEM_FAILED SEM_FAILED
#endif

static inline int cb_tsn_thread_attr_init(cb_tsn_thread_attr_t *attr, int pri,
					  int stack_size, const char* name)
{
	if(!attr) {return -1;}
	(void)memset(attr, 0, sizeof(cb_tsn_thread_attr_t));
	if(pri>0){attr->pri=pri;}
	if(stack_size>0){attr->stack_size=stack_size;}
	if(name!=NULL){
		(void)ub_strncpy(attr->name, name, CB_TSN_THREAD_NAME_SIZE);
	}
	return 0;
}

static inline int cb_tsn_thread_attr_set_stackaddr(cb_tsn_thread_attr_t *attr,
					void *stack_addr)
{
	attr->stack_addr = stack_addr;
	return 0;
}

/**
 * @brief Wait points that blocks the execution of a thread at a specific code point until an event occurs
 *
 * The cb_waitpoint mechanism bundles the pthread_cond with pthread_mutex to ensure thread safety and re-entrant
 * blocking mechanism. Ideally, when pthread_cond is used, it is always required to guard its access with
 * pthread_mutex. This waitpoint abstracts the use of pthread_cond and pthread_mutex into single waitpoint structure.
 */
typedef struct cb_waitpoint {
        CB_THREAD_MUTEX_T lock;
        /**< mutual exclusion lock */
        CB_THREAD_COND_T condition;
        /**< lock condition */
        uint64_t time;
        /**< lock time */
        bool wakeup;
        /**< boolean if waitpoint is unlocked */
} cb_waitpoint_t;

/**
 * @brief Wait point initialization
 * @param wp object reference to a waitpoint
 */
static inline void cb_waitpoint_init(cb_waitpoint_t *wp){
        CB_THREAD_MUTEX_INIT(&wp->lock, NULL);
        CB_THREAD_COND_INIT(&wp->condition, NULL);
        wp->time=0;
        wp->wakeup=false;
}

/**
 * @brief Wait point de-initialization
 * @param wp object reference to a waitpoint
 */
static inline void cb_waitpoint_deinit(cb_waitpoint_t *wp){
        CB_THREAD_COND_DESTROY(&wp->condition);
        CB_THREAD_MUTEX_DESTROY(&wp->lock);
}

/**
 * @brief Wait point check
 * @param wp object reference to a waitpoint
 * @param time reference time, usually the current time
 * @return true if wait time has elapsed and not yet awoke, otherwise false
 */
static inline bool cb_waitpoint_check(cb_waitpoint_t *wp, uint64_t time){
        if(wp->wakeup==true){
                // check time diff, consider wrap around to uint64_t max value
                // unsigned substract typecasted to unsigned stores the 2s complement
                if(((uint64_t)(time-wp->time)<((uint64_t)1<<63))){
                        wp->wakeup=false;
                        return true;
                }
        }
        return false;
}

/**
 * @brief Wait point blocks until wake point has been awoken and after specified time
 * @param wp object reference to a waitpoint
 * @param time reference time, usually the current time
 * @param dosleep whether to wait on waitpoint condition or not
 *
 * This interface should be called within and cb_waitpoint_lock, which is then automatically
 * unlocked before entering blocking state and then automatically locked again when exiting the blocked state.
 */
static inline void cb_waitpoint_wakeup_at(cb_waitpoint_t *wp, uint64_t time, bool dosleep){
        wp->time=time;
        wp->wakeup = true;
        if(dosleep){
                CB_THREAD_COND_WAIT(&wp->condition, &wp->lock);
        }
}

/**
 * @brief Wait point wakeup to signal any one thread currently at this wait point
 * @param wp object reference to a waitpoint
 *
 * This interface should be called within and cb_waitpoint_lock/cb_waitpoint_unlock.
 */
#define cb_waitpoint_wakeup(wp) CB_THREAD_COND_SIGNAL(&(wp)->condition)

/**
 * @brief Wait point wakeup to signal all threads currently at this wait point
 * @param wp object reference to a waitpoint
 *
 * This interface should be called within and cb_waitpoint_lock/cb_waitpoint_unlock.
 */
#define cb_waitpoint_broadcast(wp) CB_THREAD_COND_BROADCAST(&(wp)->condition)

/**
 * @brief Wait point lock
 * @param wp object reference to a waitpoint
 */
#define cb_waitpoint_lock(wp) CB_THREAD_MUTEX_LOCK(&(wp)->lock)

/**
 * @brief Wait point unlock
 * @param wp object reference to a waitpoint
 */
#define cb_waitpoint_unlock(wp) CB_THREAD_MUTEX_UNLOCK(&(wp)->lock)

#endif
/** @}*/
