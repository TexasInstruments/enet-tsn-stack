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
#ifndef UB_GETMEM_H_
#define UB_GETMEM_H_

#define UB_CONCAT2(A, B) UB_CONCAT2_(A, B)
#define UB_CONCAT2_(A, B) A##B

/*
 * The default is not thread safe.
 * To be thread safe,
 *   to use pthread mutex, define UB_SD_PTHREAD_MUTEX before including ub_getmem.h
 *   to use other mutex, define the following 3 macros.
*/
#ifdef UB_SD_PTHREAD_MUTEX
#include <pthread.h>
#define UB_SD_MUTEX_INIT(x) static pthread_mutex_t (x)=PTHREAD_MUTEX_INITIALIZER
#define UB_SD_MUTEX_LOCK(x) pthread_mutex_lock(x)
#define UB_SD_MUTEX_UNLOCK(x) pthread_mutex_unlock(x)
#else
#ifndef UB_SD_MUTEX_INIT
#define UB_SD_MUTEX_INIT(x)
#define UB_SD_MUTEX_LOCK(x)
#define UB_SD_MUTEX_UNLOCK(x)
#endif
#endif

/*
 * to be safe to cast any type of data, the fragmet is aligned to 'sizeof(void*)' size
 */
#define UB_SD_ALIGN(x) ((((uint32_t)(x))+(sizeof(void*)-1u))&(~(sizeof(void*)-1u)))

#ifdef UB_SD_STATIC
void *ub_static_getmem(size_t size, void *mem, uint16_t* busysizes,
		       int fragnum, uint16_t fragsize, const char *mname);
void *ub_static_regetmem(void *p, size_t nsize, void *mem, uint16_t *busysizes,
			 int fragnum, uint16_t fragsize, const char *mname);
void ub_static_relmem(void *p, void *mem, uint16_t *busysizes,
		      int fragnum, uint16_t fragsize);

#define UB_SD_GETMEM_DEF(NAME, FRAGSIZE, FRAGNUM)			\
	static uint8_t UB_CONCAT2(NAME, mem)[UB_SD_ALIGN(FRAGSIZE)*(uint32_t)FRAGNUM] \
		__attribute__ ((aligned (sizeof(void*))));		\
	static uint16_t UB_CONCAT2(NAME, busysizes)[FRAGNUM]		\
		__attribute__ ((aligned (sizeof(void*))));		\
UB_SD_MUTEX_INIT(UB_CONCAT2(NAME,mutex));				\
void *UB_CONCAT2(static_getmem, NAME)(size_t size)			\
{									\
	void *m;							\
	UB_SD_MUTEX_LOCK(&UB_CONCAT2(NAME,mutex));			\
	m=ub_static_getmem(size, UB_CONCAT2(NAME, mem), UB_CONCAT2(NAME, busysizes),\
			   FRAGNUM, (uint16_t)UB_SD_ALIGN(FRAGSIZE), #NAME); \
	UB_SD_MUTEX_UNLOCK(&UB_CONCAT2(NAME,mutex));			\
	return m;							\
}									\
void *UB_CONCAT2(static_regetmem, NAME)(void *p, size_t nsize)		\
{									\
	void *m;							\
	UB_SD_MUTEX_LOCK(&UB_CONCAT2(NAME,mutex));			\
	m=ub_static_regetmem(p, nsize, UB_CONCAT2(NAME, mem), UB_CONCAT2(NAME, busysizes), \
			    FRAGNUM, (uint16_t)UB_SD_ALIGN(FRAGSIZE), #NAME); \
	UB_SD_MUTEX_UNLOCK(&UB_CONCAT2(NAME,mutex));			\
	return m;							\
}									\
void UB_CONCAT2(static_relmem, NAME)(void *p)				\
{									\
	return ub_static_relmem(p, UB_CONCAT2(NAME, mem), UB_CONCAT2(NAME, busysizes), \
			      FRAGNUM, (uint16_t)UB_SD_ALIGN(FRAGSIZE)); \
}

#define UB_SD_GETMEM_DEF_EXTERN(NAME)				\
extern void *UB_CONCAT2(static_getmem, NAME)(size_t size);	\
extern void *UB_CONCAT2(static_regetmem, NAME)(void *p, size_t nsize);	\
extern void UB_CONCAT2(static_relmem, NAME)(void *p)

#define UB_SD_GETMEM(name, size) UB_CONCAT2(static_getmem, name)(size)
#define UB_SD_REGETMEM(name, p, size) UB_CONCAT2(static_regetmem, name)(p, size)
#define UB_SD_RELMEM(name, p) UB_CONCAT2(static_relmem, name)(p)
#else
#define UB_SD_GETMEM_DEF(NAME, FRAGSIZE, FRAGNUM)
#define UB_SD_GETMEM_DEF_EXTERN(NAME)
#define UB_SD_GETMEM(name, size) malloc(size)
#define UB_SD_REGETMEM(name, p, size) realloc(p, size)
#define UB_SD_RELMEM(name, p) free(p)
#endif

#endif
