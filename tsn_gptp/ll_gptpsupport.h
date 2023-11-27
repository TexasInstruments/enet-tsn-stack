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
#ifndef LL_GPTPSUPPORT_H_
#define LL_GPTPSUPPORT_H_

#ifndef PTPFD_TYPE
#define PTPFD_TYPE int
#define PTPFD_VALID(ptpfd) ((ptpfd)>=0)
#define PTPFD_INVALID -1
#define PRiFD "%d"
#endif

/*******************************************************
 * functions supported in the platform dependent layer
 *******************************************************/
PTPFD_TYPE ptpdev_clock_open(char *ptpdev, int permission);
int ptpdev_clock_close(PTPFD_TYPE fd);
int ptpdev_clock_gettime(PTPFD_TYPE fd, int64_t *ts64);
int ptpdev_clock_settime(PTPFD_TYPE fd, int64_t *ts64);
int ptpdev_clock_adjtime(PTPFD_TYPE ptpfd, int adjppb);
int ptpdev_clock_setoffset(PTPFD_TYPE fd, int64_t offset);
/*******************************************************/

/*
 * Virtual clock is a clock that has prefix name as #CB_VIRTUAL_PTPDEV_PREFIX
 * and fd range from #GPTP_VIRTUAL_PTPDEV_FDBASE to #GPTP_VIRTUAL_PTPDEV_FDMAX.
 */
#ifdef PTP_VIRTUAL_CLOCK_SUPPORT

#define VIRTUAL_CLOCKFD(fd) \
	(((fd)>=(PTPFD_TYPE)GPTP_VIRTUAL_PTPDEV_FDBASE) &&	\
	 ((fd)<=(PTPFD_TYPE)GPTP_VIRTUAL_PTPDEV_FDMAX))

#define VIRTUAL_CLOCKNAME(name) \
	(strstr((name), CB_VIRTUAL_PTPDEV_PREFIX)==(name))

/*----------------------------------------------------------*/
/* These macros are used by application that use libtsn_gptp */

static inline int _zero_return(void){return 0;}

#define PTPDEV_CLOCK_OPEN(name, perm) \
	VIRTUAL_CLOCKNAME(name)?(PTPFD_TYPE)GPTP_VIRTUAL_PTPDEV_FDBASE:ptpdev_clock_open(name,perm)

#define PTPDEV_CLOCK_CLOSE(fd) \
	((fd)==(PTPFD_TYPE)(GPTP_VIRTUAL_PTPDEV_FDBASE))?_zero_return():ptpdev_clock_close(fd)

#define PTPDEV_CLOCK_GETTIME(fd,ts64) \
{\
	if(!VIRTUAL_CLOCKFD(fd)){\
		(void)ptpdev_clock_gettime((fd), (int64_t *)&(ts64));	\
	}else{\
		(ts64)=ub_rt_gettime64();	\
	}\
}
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
/* These macros are used by tsn_gptpd */
uint64_t gptp_vclock_gettime(PTPFD_TYPE ptpfd);
int gptp_vclock_settime(PTPFD_TYPE ptpfd, uint64_t ts64);
int gptp_vclock_setoffset(PTPFD_TYPE ptpfd, int64_t offset64);

#define GPTP_CLOCK_GETTIME(fd,ts64)					\
	{								\
		if(!VIRTUAL_CLOCKFD(fd)){				\
			(void)ptpdev_clock_gettime((fd), (int64_t *)&(ts64)); \
		}else{							\
			(ts64)=gptp_vclock_gettime(fd);			\
		}							\
	}

#define GPTP_CLOCK_SETTIME(fd,ts64)					\
	{								\
		if(!VIRTUAL_CLOCKFD(fd)){				\
			(void)ptpdev_clock_settime((fd), (int64_t *)&(ts64)); \
		}else{							\
			(void)gptp_vclock_settime((fd), (ts64));	\
		}							\
	}

#define GPTP_CLOCK_SETOFFSET(fd,offset64)				\
	{								\
		if(!VIRTUAL_CLOCKFD(fd)){				\
			(void)ptpdev_clock_setoffset((fd), (offset64));	\
		}else{							\
			(void)gptp_vclock_setoffset((fd), (offset64));	\
		}							\
}
/*----------------------------------------------------------*/

#else //!PTP_VIRTUAL_CLOCK_SUPPORT

/*----------------------------------------------------------*/
/* These macros are used by application that use libtsn_gptp */
#define PTPDEV_CLOCK_OPEN(name, perm) ptpdev_clock_open(name,perm)
#define PTPDEV_CLOCK_CLOSE(fd) ptpdev_clock_close(fd)
#define PTPDEV_CLOCK_GETTIME(fd,ts64) ptpdev_clock_gettime(fd, (int64_t *)&(ts64))
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
/* These macros are used by tsn_gptpd */
#define GPTP_CLOCK_GETTIME(fd,ts64) ptpdev_clock_gettime(fd, (int64_t *)&(ts64))
#define GPTP_CLOCK_SETTIME(fd,ts64) ptpdev_clock_settime(fd, (int64_t *)&(ts64))
#define GPTP_CLOCK_SETOFFSET(fd,offset64) ptpdev_clock_setoffset(fd, offset64)
/*----------------------------------------------------------*/

#endif //!PTP_VIRTUAL_CLOCK_SUPPORT

/**
 * @brief enables H/W Timestamping for socket
 * @param dev	ethernet device name like 'eth0', 'eno1', 'enp2s0'.
 * @return 0 on success, -1 on error
 */
int ll_set_hw_timestamping(CB_SOCKET_T cfd, const char *dev);

/**
 * @brief disables hardware timestamping for socket
 * @param dev	ethernet device name like 'eth0'
 * @return 0 on success, -1 on error
 */
int ll_close_hw_timestamping(CB_SOCKET_T cfd, const char *dev);

/**
 * @brief get Tx timestamp from msg
 * @param tts	return timestamp
 * @return 0 on success, -1 on error
 */
int ll_txmsg_timestamp(void *p, int64_t *ts64);

/**
 * @brief get Rx timestamp from msg
 * @param ts	return timestamp
 * @return 0 on success, -1 on error
 */
int ll_recv_timestamp(void *p, int64_t *ts64);

#endif
