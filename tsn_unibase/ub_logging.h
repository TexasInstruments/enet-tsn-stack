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
 * @defgroup logging Control logging
 * @{
 * @file ub_logging.h
 *
 * @brief Control logging by levels and categories.
 *
 * The logging is controlled by 8 levels which is from UBL_NONE to UBL_DEBUGV.
 * The levels are used, one is for regular console output,
 * and the other is for debug output.
 *
 * The two levels are set for each category.
 * The categories are defined by user applications, and need to be registered
 * by the initialization string.
 * Registered categories must be enumerated and managed by user.
 * Because category number 0 is used for 'unibase', in user applications
 * starting from category number 1 is recommended.
 */

#ifndef UB_LOGGING_H_
#define UB_LOGGING_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This defines different logging levels
 * - UBL_NONE No message will print on console
 * - UBL_FATAL to print FATAL message on console
 * - UBL_ERROR to print FATAL to ERROR message on console
 * - UBL_WARN to print FATAL to WARNING message on console
 * - UBL_INFO to print FATAL to INFO message on console
 * - UBL_INFOV to print FATAL to INFOV message on console
 * - UBL_DEBUG to print FATAL to DEBUG message on console
 * - UBL_DEBUGV to print FATAL to DEBUGV message on console
 */
typedef int ub_dbgmsg_level_t;

#define	UBL_NONE   0
#define	UBL_FATAL  1
#define	UBL_ERROR  2
#define	UBL_WARN   3
#define	UBL_INFO   4
#define	UBL_INFOV  5
#define	UBL_DEBUG  6
#define	UBL_DEBUGV 7

#ifndef MAX_LOGMSG_CATEGORIES
/** @brief maximum number of categories */
#define MAX_LOGMSG_CATEGORIES 16
#endif

/**
 * @brief 3-charcter string to represent each logging level
 */
#define DBGMSG_LEVEL_MARK {"NON", "FTL", "ERR", "WRN", "INF", "IFV", "DBG", "DBV"}

/**
 * @brief lowest 2 bits are used for TS Clock type, use ub_clocktype_t
 * 	UB_CLOCK_DEFAULT means no-timestamp
 */
#define UBL_TS_BIT_FIELDS 3 // bit0 and bit1

#ifndef UB_LOG_COMPILE_LEVEL
#define UB_LOG_COMPILE_LEVEL UBL_DEBUGV
#endif

#define UB_LOG_IS_COMPILED(level) (UB_LOG_COMPILE_LEVEL >= level)

#define UB_LOG_HELPER(level, tstype, ...)			\
	{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		(void)ub_log_print(UB_LOGCAT, tstype, level, coutstr);	\
	}

#if UB_LOG_IS_COMPILED(UBL_DEBUGV)
#define UB_LOG_UBL_DEBUGV(...) UB_LOG_HELPER(UBL_DEBUGV, 0, __VA_ARGS__)
#define UB_TLOG_UBL_DEBUGV(...) UB_LOG_HELPER(UBL_DEBUGV, UB_LOGTSTYPE, __VA_ARGS__)
#else
#define UB_LOG_UBL_DEBUGV(...)
#define UB_TLOG_UBL_DEBUGV(...)
#endif

#if UB_LOG_IS_COMPILED(UBL_DEBUG)
#define UB_LOG_UBL_DEBUG(...) UB_LOG_HELPER(UBL_DEBUG, 0, __VA_ARGS__)
#define UB_TLOG_UBL_DEBUG(...) UB_LOG_HELPER(UBL_DEBUG, UB_LOGTSTYPE, __VA_ARGS__)
#else
#define UB_LOG_UBL_DEBUG(...)
#define UB_TLOG_UBL_DEBUG(...)
#endif

#if UB_LOG_IS_COMPILED(UBL_INFOV)
#define UB_LOG_UBL_INFOV(...) UB_LOG_HELPER(UBL_INFOV, 0, __VA_ARGS__)
#define UB_TLOG_UBL_INFOV(...) UB_LOG_HELPER(UBL_INFOV, UB_LOGTSTYPE, __VA_ARGS__)
#else
#define UB_LOG_UBL_INFOV(...)
#define UB_TLOG_UBL_INFOV(...)
#endif

#if UB_LOG_IS_COMPILED(UBL_INFO)
#define UB_LOG_UBL_INFO(...) UB_LOG_HELPER(UBL_INFO, 0, __VA_ARGS__)
#define UB_TLOG_UBL_INFO(...) UB_LOG_HELPER(UBL_INFO, UB_LOGTSTYPE, __VA_ARGS__)
#else
#define UB_LOG_UBL_INFO(...)
#define UB_TLOG_UBL_INFO(...)
#endif

#if UB_LOG_IS_COMPILED(UBL_WARN)
#define UB_LOG_UBL_WARN(...) UB_LOG_HELPER(UBL_WARN, 0, __VA_ARGS__)
#define UB_TLOG_UBL_WARN(...) UB_LOG_HELPER(UBL_WARN, UB_LOGTSTYPE, __VA_ARGS__)
#else
#define UB_LOG_UBL_WARN(...)
#define UB_TLOG_UBL_WARN(...)
#endif

#if UB_LOG_IS_COMPILED(UBL_ERROR)
#define UB_LOG_UBL_ERROR(...) UB_LOG_HELPER(UBL_ERROR, 0, __VA_ARGS__)
#define UB_TLOG_UBL_ERROR(...) UB_LOG_HELPER(UBL_ERROR, UB_LOGTSTYPE, __VA_ARGS__)
#else
#define UB_LOG_UBL_ERROR(...)
#define UB_TLOG_UBL_ERROR(...)
#endif

#if UB_LOG_IS_COMPILED(UBL_FATAL)
#define UB_LOG_UBL_FATAL(...) UB_LOG_HELPER(UBL_FATAL, 0, __VA_ARGS__)
#define UB_TLOG_UBL_FATAL(...) UB_LOG_HELPER(UBL_FATAL, UB_LOGTSTYPE, __VA_ARGS__)
#else
#define UB_LOG_UBL_FATAL(...)
#define UB_TLOG_UBL_FATAL(...)
#endif

#define UB_LOG_UBL_NONE(...)
#define UB_TLOG_UBL_NONE(...)

/**
 * @brief override valuse 'x' to the value of the environment variable 'y'
 * @note UBB_GETENV must be defined before this macro is called.
 */
#define UBL_OVERRIDE_ISTR(x,y) ub_log_initstr_override(x,UBB_GETENV(y))

#ifdef PRINT_FORMAT_NO_WARNING
/** @brief some compilers don't have this type of attribute */
#define PRINT_FORMAT_ATTRIBUTE1
#define PRINT_FORMAT_ATTRIBUTE4
#else
/** @brief let the compiler show warning when printf type format is wrong */
#define PRINT_FORMAT_ATTRIBUTE1 __attribute__((format (printf, 1, 2))) //!< format starts at 1st argument
#define PRINT_FORMAT_ATTRIBUTE4 __attribute__((format (printf, 4, 5))) //!< format starts at 4th argument
#endif

/**
 * @brief output to console out and debug out by selection
 * @param console	select output to console
 * @param debug	select output to debug memory
 * @param astr	a string to print
 */
void ub_console_debug_select_print(bool console, bool debug, const char *astr);

/**
 * @brief override 'istr' of ub_log_inig
 * @param ns	original 'istr'
 * @param os	overriding string
 * @return 'ns' or 'os'
 * @note if 'os' includes ',', the entire 'os' is returned and it overrides the entire 'istr'
 * 	if 'os' one module of 'ns' like 'mod1:55m', 'mod1' part is overridden by 'os'
 *	e.g. when ns="4,mod0:44,mod1:44", os="mod1:55m"
 *	ub_log_init(ub_log_initstr_override(ns, os)) is initialized by "4,mod0:44,mod1:55m"
 */
const char *ub_log_initstr_override(const char *ns, const char *os);

/**
 * @brief initialize logging by a string
 * @param istr	initialization string
 * @note an example of string is like "ubase:34r,mod1:45m,mod2:56g,mod3:23"
 * 	each category string is separated by a comma, and the category
 *	initialization is a way in 'ub_log_add_category'
 */
void ub_log_init(const char *istr);

/**
 * @brief add a category of logging at the bottom of the index
 * @param catstr	category initialization string
 * @return 0 on success, -1 on error
 * @note the string is like "catname:45m", 'catname' must be less than 7 characters,
 * 	4 is UBL_INFO levlel, 5 is UBL_INFOV level, 'm' is option to add monotonic
 *	clock timestamp.
 */
int ub_log_add_category(const char *catstr);

/**
 * @brief print log message
 * @param cat_index index of categories, the index is defined by the initialization
 *	string in unibase_init function
 * @param flags timestamp option is defined in lower 2 bits.
 * @param level log level, see ub_dbgmsg_level_t
 * @param astr	a string to print
 * @return 0 on success, -1 on error
 */
int ub_log_print(int cat_index, int flags, ub_dbgmsg_level_t level, const char *astr);

/**
 * @brief check if console log is enabled or not for the indicated cat_index and level
 * @return true if enabled, otherwise false.
 */
bool ub_clog_on(int cat_index, ub_dbgmsg_level_t level);

/**
 * @brief check if debug log is enabled or not for the indicated cat_index and level
 * @return true if enabled, otherwise false.
 */
bool ub_dlog_on(int cat_index, ub_dbgmsg_level_t level);

/**
 * @brief change console log level to clevel, and debug log level to delevl
 * @return 0 on success, -1 on error
 */
int ub_log_change(int cat_index, ub_dbgmsg_level_t clevel, ub_dbgmsg_level_t dlevel);

/**
 * @brief return console log level and console log level to the previous status
 * @return 0 on success, -1 on error
 */
int ub_log_return(int cat_index);

/**
 * @brief flush out messages on the both of cosole log and debug log
 * @note the function depends of the callback function
 */
void ub_log_flush(void);

#ifdef __cplusplus
}
#endif

#endif
/** @}*/
