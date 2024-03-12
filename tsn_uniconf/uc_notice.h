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
#ifndef UC_NOTICE_H_
#define UC_NOTICE_H_

#include <tsn_unibase/unibase.h>
#include <tsn_combase/cb_thread.h>
#include "yangs/yang_db_access.h"


#define UC_NOTICE_PUT uc_notice_put

#define UC_SEMNAME_MAX 48
#define UC_GETNOTICE_SEM "/xl4uc_getntc"

/* how many number of signals, when 'uc_client' needs to be signaled?
 * This is a list of semaphore pointers */
#define UC_NOTICE_SIGM uc_notice_sigm
#ifndef UC_NOTICE_SIGNUM
#define UC_NOTICE_SIGNUM 4
#endif

typedef CB_SEM_T UC_NOTICE_SIG_T;

typedef struct uc_notice_data uc_notice_data_t;

/* platform dependent fuctions */
// if name=NULL, the default name is used
int uc_notice_sig_open(bool thread, UC_NOTICE_SIG_T **sigp, int *master, const char *name);
void uc_notice_sig_close(bool thread, UC_NOTICE_SIG_T *sigp, bool master, const char *name);
int uc_notice_sig_getvalue(bool thread, UC_NOTICE_SIG_T *sigp, int *sval);
int uc_notice_sig_post(bool thread, UC_NOTICE_SIG_T *sigp);
int uc_notice_sig_trywait(bool thread, UC_NOTICE_SIG_T *sigp);
UC_NOTICE_SIG_T *uc_notice_sig_reallocate(bool thread, void *d, int n);
// tout_ms>0:wait signal for the time, toug_ms<=0:wait until the signal
// return 0:get the signal, 1:timed out, -1:error
int uc_notice_sig_check(bool thread, UC_NOTICE_SIG_T *sigp, int tout_ms, const char *fname);


/**
 * @brief initialize 'uc_notice'
 * @param callmode
 */
uc_notice_data_t *uc_notice_init(uint8_t callmode, const char *dbname);

/**
 * @brief start network HW events monitoring thread
 */
int uc_notice_start_events_thread(uc_notice_data_t *ucntd, uc_hwald *hwald);

/**
 * @brief close 'uc_notice'
 * @param callmode
 */
void uc_notice_close(uc_notice_data_t *ucntd, uint8_t callmode);

/**
 * @brief 'uc_client' made some update on DB, and want to be signaled to
 *        the other registered 'uc_client'
 */
int uc_nc_askaction_push(uc_notice_data_t *ucntd, uc_dbald *dbald,
			 uint8_t *aps, void **kvs, uint8_t *kss);

/**
 * @brief 'uniconf' made some update on DB, and want to be signaled to
 *        the the registered 'uc_client'
 */
int uc_nu_askaction_push(uc_notice_data_t *ucntd, uc_dbald *dbald,
			 uint8_t *aps, void **kvs, uint8_t *kss);

/**
 * @brief 'uniconf' checks 'getnotice_sem'. if signaled, read DB and process on HW
 * @param tout_ms -1:wait forever, 0:no timeout, >0:msec timeout
 * @note 'getnotice_sem' signals from 'uc_client' by 'uc_askaction_push'
 */
int uc_nu_proc_asked_actions(uc_notice_data_t *ucntd,
			     uc_dbald *dbald, uc_hwald *hwald, int tout_ms);

/**
 * @brief 'uc_client' registers key to get noticed from 'uniconf'
 * @param aps	node parts of key. must be terminated by 255.
 * @param kvs	additional key part, semaphore name:'semname' must be the last
 *		then terminated by NULL: kvs={,,,semname,NULL}
 * @param regval value select in notice_register_value_t
 * @note  by 'semaname', a newly created semaphore or an existing semaphore
 *	  is returned in *sem.
 *        'semname' must be a unique name.
 */
typedef enum {
	UC_NOTICE_DBVAL_ADD = 0, // add single item to notice
	UC_NOTICE_DBVAL_DEL, // decrement refcouter, if it is 0 then remove
} notice_register_value_t;

int uc_nc_notice_register(uc_notice_data_t *ucntd, uc_dbald *dbald,
			  uint8_t *aps, void **kvs, uint8_t *kss,
			  notice_register_value_t regval, UC_NOTICE_SIG_T **sem);

/**
 * @brief 'uc_client' deregister keys with semaname
 * @note  'semaname' may be shared with multilpe keys, this clean all of them
 */
int uc_nc_notice_deregister_all(uc_notice_data_t *ucntd, uc_dbald *dbald,
				const char *semname);

/**
 * @brief 'uniconf' pushes an notice to registered 'semname'
 * @note this sends semaphore signals, 'uc_dbal_releasedb' must be called after this.
 */
int uc_nu_putnotice_push(uc_notice_data_t *ucntd, uc_dbald *dbald,
			 uint8_t *aps, void **kvs, uint8_t *kss);

/**
 * @brief 'uc_client' check if there are any notice from 'uniconf' on registered items.
 * @return -1:no notice, 0:found a notice and *key is returned
 * @note the notice is removed by this call.
 *	 the caller needs to bring key with size of UC_MAX_KEYSIZE.
 */
int uc_nc_get_notice_act(uc_notice_data_t *ucntd, uc_dbald *dbald, const char *semname,
			 void *key, uint32_t *ksize);

#endif
