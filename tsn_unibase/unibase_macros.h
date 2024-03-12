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
 * @defgroup unibase_macros Utility Macros
 * @{
 * @file unibase_macros.h
 * @brief Utility macros for convenience
 *
 */
#ifndef UNIBASE_MARCORS_H_
#define UNIBASE_MARCORS_H_

#include <stdio.h>

/************************************************************
 * Utility Macros
 ************************************************************/
#define UB_SEC_NS 1000000000LL //!< one second in unit of nano second
#define UB_MSEC_NS 1000000 //!< one mili second in unit of nano second
#define UB_USEC_NS 1000 //!< one micro second in unit of nano second
#define UB_SEC_US 1000000LL //!< one second in unit of microsecond
#define UB_MSEC_US 1000 //!< one mili second in unit of microsecond
#define UB_SEC_MS 1000 //!< one second in unit of milisecond

#define UB_CHARS_IN_LINE 384

#define UB_BIT(x) (1U<<(x)) //!< bit x
#define UB_MAX(x,y) ((x)>(y)?(x):(y)) //!< max(x,y)
#define UB_MIN(x,y) ((x)<(y)?(x):(y)) //!< min(x,y)

#define UB_CONSOLE_PRINT(...)						\
	{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		ub_console_debug_select_print(true, false, coutstr);	\
	}

#define UB_DEBUGMEM_PRINT(...)						\
	{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		ub_console_debug_select_print(false, true, coutstr);	\
	}

#define UB_CONSOLE_DEBUGMEM_PRINT(...)					\
	{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		ub_console_debug_select_print(true, true, coutstr);	\
	}

#define UB_SELECT_PRINT(console, debugmem, ...)				\
	{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		ub_console_debug_select_print(console, debugmem, coutstr); \
	}

/**
 * @brief UB_LOG(level, formt, ...), level is compared to the level in the category
 *	which is defined by UB_LOGCAT.
 *
 * 	UB_LOGCAT must be defined in .c file to indicate the log category index.
 *	UB_LOGCAT=0 is reserved as this unibase category.
 *	e.g. UB_LOG(UBL_DEBUG, "%s:x=%d\n", __func__, x);
 *	if UBL_DEBUG<="the level in UB_LOGCAT", it is printed
 */
#define UB_LOG(level, ...) UB_LOG_##level(__VA_ARGS__)

/**
 * @brief UB_TLOG add timestamp regardless the timestamp option in the category
 */
#define UB_TLOG(level, ...) UB_TLOG_##level(__VA_ARGS__)

/**
 * @brief UB_VLOG allows for flexible logging with a variable log level
 * specified as an argument in the function call.
 * Example Usage:
 * void func(int level, int abc)
 * {
 *     UB_VLOG(level, "hello world\n");
 * }
 * The func can be called as: func(UBL_DEBUGV, abc);
 * In this case UB_LOG and UB_TLOG can not work.
 * @note This macro may be inefficient and can impact performance.
 * Please use with the caution.
 */
#define UB_VLOG(var, ...) \
	if((var)==UBL_DEBUGV){UB_LOG(UBL_DEBUGV,__VA_ARGS__);}\
	else if((var)==UBL_DEBUG){UB_LOG(UBL_DEBUG,__VA_ARGS__);}\
	else if((var)==UBL_INFOV){UB_LOG(UBL_INFOV,__VA_ARGS__);}\
	else if((var)==UBL_INFO){UB_LOG(UBL_INFO,__VA_ARGS__);}\
	else if((var)==UBL_WARN){UB_LOG(UBL_WARN,__VA_ARGS__);}\
	else if((var)==UBL_ERROR){UB_LOG(UBL_ERROR,__VA_ARGS__);}\
	else if((var)==UBL_FATAL){UB_LOG(UBL_FATAL,__VA_ARGS__);}\
	else{;}

/** @brief use this to print ub_streamid_t */
#define UB_PRIhexB8 "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X"
//!< used with UB_PRIhexB8
#define UB_ARRAY_B8(x) (x)[0],(x)[1],(x)[2],(x)[3],(x)[4],(x)[5],(x)[6],(x)[7]
/** @brief use this to print ub_macaddr_t */
#define UB_PRIhexB6 "%02X:%02X:%02X:%02X:%02X:%02X"
#define UB_ARRAY_B6(x) (x)[0],(x)[1],(x)[2],(x)[3],(x)[4],(x)[5] //!< used with UB_PRIhexB6

/** @brief true if 6-byte binaries are not all zero */
#define UB_NON_ZERO_B6(x) (((x)[0]|(x)[1]|(x)[2]|(x)[3]|(x)[4]|(x)[5])!=0u)

/** @brief true if 8-byte binaries are not all zero */
#define UB_NON_ZERO_B8(x) (((x)[0]|(x)[1]|(x)[2]|(x)[3]|(x)[4]|(x)[5]|(x)[6]|(x)[7])!=0u)

/** @brief true if 6-byte binaries are all 0xff */
#define UB_ALLFF_B6(x) (((x)[0]&(x)[1]&(x)[2]&(x)[3]&(x)[4]&(x)[5])==0xffu)

/** @brief true if 8-byte binaries are all 0xff */
#define UB_ALLFF_B8(x) (((x)[0]&(x)[1]&(x)[2]&(x)[3]&(x)[4]&(x)[5]&(x)[6]&(x)[7])==0xffu)

/**
 * @brief two of inline functions: name_bit_field, name_set_bit_field
 *	are created for bit opperation on a network bit order endian variable.
 *
 *	e.g UB_ABIT8_FIELD(bs, 2, 0x1f),
 *	  - bs_bit_field(v): read 'bit6 to bit2 of 'v'
 *	  - v=bs_set_bit_field(v, 5): set 'bit6 to bit2' of 'v' to 5
 */
#define UB_ABIT8_FIELD(name, s, m)				      \
	static inline int name##_bit_field(uint8_t x)		      \
	{ return (x >> (s)) & (m); }				      \
	static inline int name##_set_bit_field(uint8_t x, uint8_t v) \
	{ return (x & ~((m) << (s))) | ((v & (m)) << (s)) ; }

/** @brief the same like UB_BIT8_FILED, works on 16-bit variable */
#define UB_ABIT16_FIELD(name, s, m)					\
	static inline int name##_bit_field(uint16_t x)			\
	{ return (htons(x) >> (s)) & (m); }				\
	static inline int name##_set_bit_field(uint16_t x, uint16_t v) \
	{return ntohs((htons(x) & ~((m) << (s)))			\
				    | ((v & (m)) << (s)));}

/** @brief the same like UB_BIT8_FILED, works on 32-bit variable */
#define UB_ABIT32_FIELD(name, s, m)					\
	static inline int name##_bit_field(uint32_t x)			\
	{ return (htonl(x) >> (s)) & (m); }				\
	static inline int name##_set_bit_field(uint32_t x, uint32_t v) \
	{return ntohl((htonl(x) & ~((m) << (s)))			\
				    | ((v & (m)) << (s)));}

/**
 * @brief inline function: name_toggle_bit_field
 *	is created for toggling bit opperation on a network bit order variable.
 *
 *	e.g UB_ABIT8_TOGGLE_FIELD(bs, 2, 1),
 *	  - v=bs_toggle_bit_field(v): toggle bit2 at each time of call
 */
#define UB_ABIT8_TOGGLE_FIELD(name, s, m)				\
	static inline int name##_toggle_bit_field(uint8_t x)		\
	{return (x ^ ((m) << (s)));}

/** @brief the same like UB_BIT8_TOGGLE_FIELD, works on 16-bit variable */
#define UB_ABIT16_TOGGLE_FIELD(name, s, m)				\
	static inline int name##_toggle_bit_field(uint16_t x)		\
	{return ntohs((htons(x) ^ ((m) << (s))));}

/** @brief the same like UB_BIT8_TOGGLE_FIELD, works on 32-bit variable */
#define UB_ABIT32_TOGGLE_FIELD(name, s, m)				\
	static inline int name##_toggle_bit_field(uint32_t x)		\
	{return ntohl((htonl(x) ^ ((m) << (s))));}

/** @brief convert 'struct timespec' vaule to nanosecond integer */
#define UB_TS2NSEC(ts) (((int64_t)(ts).tv_sec*1000000000)+(ts).tv_nsec)

/** @brief convert 'struct timespec' vaule to microsecond integer */
#define UB_TS2USEC(ts) (((int64_t)(ts).tv_sec*1000000)+(ts).tv_nsec/UB_USEC_NS)

/** @brief convert 'struct timespec' vaule to milisecond integer */
#define UB_TS2MSEC(ts) (((int64_t)(ts).tv_sec*UB_SEC_MS)+(ts).tv_nsec/UB_MSEC_NS)

/** @brief convert 'struct timeval' vaule to nanosecond integer */
#define UB_TV2NSEC(tv) (((int64_t)(tv).tv_sec*1000000000)+(int64_t)(tv).tv_usec*UB_USEC_NS)

/** @brief convert 'struct timeval' vaule to nanosecond integer */
#define UB_TV2USEC(tv) (((int64_t)(tv).tv_sec*1000000)+(tv).tv_usec)

/** @brief convert 'struct timeval' vaule to milisecond integer */
#define UB_TV2MSEC(tv) (((int64_t)(tv).tv_sec*UB_SEC_MS)+(tv).tv_usec/UB_MSEC_US)

/** @brief convert nanosec value to 'struct timespec' vaule */
#define UB_NSEC2TS(ns, ts) {(ts).tv_sec=(ns)/1000000000;(ts).tv_nsec=(ns)%1000000000;}

/** @brief convert microsec value to 'struct timespec' vaule */
#define UB_USEC2TS(us, ts) {(ts).tv_sec=(us)/1000000;(ts).tv_nsec=((us)%1000000)*UB_USEC_NS;}

/** @brief convert milisec value to 'struct timespec' vaule */
#define UB_MSEC2TS(ms, ts) {(ts).tv_sec=(ms)/UB_SEC_MS;(ts).tv_nsec=((ms)%UB_SEC_MS)*UB_MSEC_NS;}

/** @brief convert nanosec value to 'struct timeval' vaule */
#define UB_NSEC2TV(ns, tv) {(tv).tv_sec=(ns)/1000000000;(tv).tv_usec=((ns)%1000000000)/UB_USEC_NS;}

/** @brief convert microsec value to 'struct timeval' vaule */
#define UB_USEC2TV(us, tv) {(tv).tv_sec=(us)/1000000;(tv).tv_usec=(us)%1000000;}

/** @brief convert milisec value to 'struct timeval' vaule */
#define UB_MSEC2TV(ms, tv) {(tv).tv_sec=(ms)/UB_SEC_MS;(tv).tv_usec=((ms)%UB_SEC_MS)*UB_MSEC_US;}

/** @brief tv1-tv2 in 64-bit nanosecond unit */
#define UB_TV_DIFF64NS(tv1,tv2) (UB_TV2NSEC(tv1)-UB_TV2NSEC(tv2))

/** @brief tv1+tv2 in 64-bit nanosecond unit */
#define UB_TV_ADD64NS(tv1,tv2) (UB_TV2NSEC(tv1)+UB_TV2NSEC(tv2))

/** @brief ts1-ts2 in 64-bit nanosecond unit */
#define UB_TS_DIFF64NS(ts1,ts2) (UB_TS2NSEC(ts1)-UB_TS2NSEC(ts2))

/** @brief ts1+ts2 in 64-bit nanosecond unit */
#define UB_TS_ADD64NS(ts1,ts2) (UB_TS2NSEC(ts1)+UB_TS2NSEC(ts2))

/** @brief rtv=tv1-tv2 in 64-bit nanosecond unit */
#define UB_TV_DIFF_TV(rtv,tv1,tv2) {				\
		int64_t ns_ub_m_=(int64_t)UB_TV_DIFF64NS(tv1,tv2);	\
		UB_NSEC2TV(ns_ub_m_,rtv);			\
	}

/** @brief rts=ts1-ts2 in 64-bit nanosecond unit */
#define UB_TS_DIFF_TS(rts,ts1,ts2) {				\
		int64_t ns_ub_m_=(int64_t)UB_TS_DIFF64NS(ts1,ts2);	\
		UB_NSEC2TS(ns_ub_m_,rts);			\
	}

/** @brief rtv=tv1+tv2 in 64-bit nanosecond unit */
#define UB_TV_ADD_TV(rtv,tv1,tv2) {				\
		int64_t ns_ub_m_=(int64_t)UB_TV_ADD64NS(tv1,tv2);	\
		UB_NSEC2TV(ns_ub_m_,rtv);			\
	}

/** @brief rts=ts1+ts2 in 64-bit nanosecond unit */
#define UB_TS_ADD_TS(rts,ts1,ts2) {				\
		int64_t ns_ub_m_=(int64_t)UB_TS_ADD64NS(ts1,ts2);	\
		UB_NSEC2TS(ns_ub_m_,rts);			\
	}

/**
 * @brief convert values between host and network byte order.
 * which converts the unsigned integer host long long from
 * host byte order to network byte order.
 */
#define UB_HTONLL(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | \
		   htonl((x) >> 32))

/**
 * @brief convert values between host and network byte order.
 * converts the unsigned integer netlong from network byte
 * order to host byte order.
 */
#define UB_NTOHLL(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | \
		   ntohl((x) >> 32))

/**
 * @brief assert during compilation time.
 * When cond is equal to true, no error. Otherwise, compilation error is raised.
 * e.g. UB_STATIC_ASSERT(uint8_t == 1, Uint8InvalidSize)
 */
#define UB_STATIC_ASSERT(cond, error) typedef char type[(cond) ? 1 : -1]

/**
 * @brief call the cbfunc with an internal mutex protection.
 * The ub_func_private_mutex_lock() and ub_func_private_mutex_unlock()
 * are only used privately in this macro. Please do not use them freely elsewhere.
 */
#define UB_PROTECTED_FUNC(cbfunc, rval, ...) \
	int ub_func_private_mutex_lock(void); \
	int ub_func_private_mutex_unlock(void); \
	ub_func_private_mutex_lock(); \
	rval=cbfunc(__VA_ARGS__); \
	ub_func_private_mutex_unlock();

/**
 * @brief similar to the @ref UB_PROTECTED_FUNC without return value rval
 */
#define UB_PROTECTED_FUNC_VOID(cbfunc, ...) \
	int ub_func_private_mutex_lock(void); \
	int ub_func_private_mutex_unlock(void); \
	ub_func_private_mutex_lock(); \
	cbfunc(__VA_ARGS__); \
	ub_func_private_mutex_unlock();

#endif
/** @}*/
