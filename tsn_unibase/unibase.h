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
 * @ingroup TSN_UNIBASE_MODULE
 * @defgroup unibase General types and functions
 * @{
 * @file unibase.h
 *
 * @brief unibase general global header
 *
 * This header is intended to be included by application programs which
 * use 'unibase' library.  This is the only one header which needs to be
 * included.
 *
 * The categorized part like 'ub_logging' has separated header, and it
 * is included in this header.
 */
#ifndef UNIBASE_H_
#define UNIBASE_H_

#ifdef UB_OVERRIDE_H
#include UB_OVERRIDE_H
#endif

#include <inttypes.h>
#include <stdbool.h>

#include "unibase_macros.h"
#include "ub_logging.h"
#include "ub_esarray.h"
#include "ub_strutils.h"
#include "ub_llist.h"
#include "ub_confutils.h"
#include "ub_fileio.h"
#include "ub_getmem.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC CLOCK_REALTIME
#endif

#ifdef NO_GETOPT_LONG
#include <getopt.h>
struct option
{
	char *name;
	int has_arg;
	int *flag;
	int val;
};
#define no_argument		0
#define required_argument	1
#define optional_argument	2
static inline int getopt_long(int argc, char **argv, char *optstr,
			      struct option *lo, int *li)
{
	return getopt(argc, argv, optstr);
}
#endif

#define UB_UNIBASE_LOGCAT 0
/************************************************************
 * type declarations
 ************************************************************/
/**
 * @brief clock type which provides timestamp
 */
typedef enum {
	UB_CLOCK_DEFAULT=0,
	UB_CLOCK_REALTIME,
	UB_CLOCK_MONOTONIC,
	UB_CLOCK_GPTP,
}ub_clocktype_t;

typedef int (*ub_console_out)(bool flush, const char *str); //!< console output function
typedef int (*ub_debug_out)(bool flush, const char *str); //!< debug output function
typedef void* (*ub_mutex_init)(void); //!< mutex initialization function
typedef int (*ub_mutex_close)(void *mutex); //!< mutex de-initialization function
typedef int (*ub_mutex_lock)(void *mutex); //!< mutex lock function
typedef int (*ub_mutex_unlock)(void *mutex); //!< mutex unlock function
typedef void (*ub_fatal_func)(void); //!< function to process a fatal error event
typedef uint64_t (*ub_gettime64)(ub_clocktype_t ctype); //!< 64-bit timestamp function

/**
 * @brief a set of callback functions to process platform specific tasks
 */
typedef struct unibase_cb_set {
	ub_console_out console_out; //!< console_out callback
	ub_debug_out debug_out; //!< debug_out callback
	ub_mutex_init get_static_mutex; //!< get_static_mutex callback
	ub_mutex_close static_mutex_close; //!< static_mutex_close callback
	ub_mutex_lock mutex_lock; //!< mutex_lock callback
	ub_mutex_unlock mutex_unlock; //!< mutex_unlock callback
	ub_gettime64 gettime64; //!< gettime64 callback
	ub_fatal_func fatal; //!< fatal callback
} unibase_cb_set_t;

/**
 * @brief initialization parameters
 */
typedef struct unibase_init_para {
	unibase_cb_set_t cbset; //!< a set of callback functions
	const char *ub_log_initstr; //!< look at 'ub_log_init' in 'ub_logging.h'
}unibase_init_para_t;

/************************************************************
 * functions
 ************************************************************/
/**
 * @brief initialize unibase
 * @param ub_init_para	a set of callback functions to support platfrom
 *	specific functions, and a string to initialize logging functions
 * @return 0:succes, -1:error
 * @note unibase_init MUST be called at the beginning.
 * 	Calling unibase_init again after the first call are all ignored.
 *	If mutex_init=Null, it runs in a single thread mode.
 */

int unibase_init(unibase_init_para_t *ub_init_para);

/**
 * @brief claose unibase
 */
void unibase_close(void);

/**
 * @brief process a fatal error
 * @param mes1	printing message 1
 * @param mes2	printing message 2
 * @note 'fatal' callback is called if it is registered.
 *	after this call 'ub_fatalerror()' returns true;
 */
void ub_fatal(const char *mes1, const char *mes2);

/**
 * @brief return the fatal error status
 */
bool ub_fatalerror(void);

/**
 * @brief conditinal ub_fatal
 */
static inline bool ub_assert_fatal(bool cond, const char *mes1, const char *mes2)
{
	if(cond) {return false;}
	ub_fatal(mes1, mes2);
	return ub_fatalerror();
}

/**
 * @brief get 64-bit REALTIME clock value
 * @return clock value
 */
uint64_t ub_rt_gettime64(void);

/**
 * @brief get 64-bit MONOTONIC clock value
 * @return clock value
 */
uint64_t ub_mt_gettime64(void);

/**
 * @brief get 64-bit PTP clock value
 * @return clock value
 */
uint64_t ub_gptp_gettime64(void);

#ifdef __cplusplus
}
#endif

#endif
/** @}*/
