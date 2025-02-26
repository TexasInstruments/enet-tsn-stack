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
 * @file        nconf_memutils.h
 *
 * @brief       Netconf Memory Allocator Utility Interface Header
 */
#ifndef __NCONF_MEMUTILS_H__
#define __NCONF_MEMUTILS_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <stddef.h>
#include <stdbool.h>
#include <ub_logging.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#ifdef NCONF_MEMUSTILS_DEBUG
#define FILE_LINE_PARAMS    file, line,
#define ALLOC_PARAMS        const char *file, int line, size_t size
#define REALLOC_PARAMS      const char *file, int line, void *optr, size_t size
#define STRDUP_PARAMS       const char *file, int line, char *src
#define FREE_PARAMS         const char *file, int line, void *ptr
#define nconf_memalloc(_sz)         nconf_memalloc_def(__FILE__, __LINE__, _sz)
#define nconf_memrealloc(_ptr, _sz) nconf_memrealloc_def(__FILE__, __LINE__, _ptr, _sz)
#define nconf_strdup(_src)          nconf_strdup_def(__FILE__, __LINE__, _src)
#define nconf_memfree(_ptr)         nconf_memfree_def(__FILE__, __LINE__, _ptr)
#else
#define FILE_LINE_PARAMS
#define ALLOC_PARAMS        size_t size
#define REALLOC_PARAMS      void *optr, size_t size
#define STRDUP_PARAMS       char *src
#define FREE_PARAMS         void *ptr
#define nconf_memalloc(_sz)         nconf_memalloc_def(_sz)
#define nconf_memrealloc(_ptr, _sz) nconf_memrealloc_def(_ptr, _sz)
#define nconf_strdup(_src)          nconf_strdup_def(_src)
#define nconf_memfree(_ptr)         nconf_memfree_def(_ptr)
#endif

/*=============================================================================
 * Allocation/Deallocation Utility APIs
 *============================================================================*/

void* nconf_memalloc_def(ALLOC_PARAMS);
void* nconf_memrealloc_def(REALLOC_PARAMS);
char* nconf_strdup_def(STRDUP_PARAMS);
void nconf_memfree_def(FREE_PARAMS);
void nconf_memusage(ub_dbgmsg_level_t level);

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_MEMUTILS_H__ */
