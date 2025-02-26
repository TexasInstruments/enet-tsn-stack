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
 * @file        nconf_memutils.c
 *
 * @brief       Netconf Memory Allocator Utility Implementation
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <string.h>
#include "nconf_types.h"
#include "nconf_memutils.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#ifdef NCONF_MEMUSTILS_DEBUG
#define NCONF_MEMUTILS_ASSERT(_c, _m, EXIT_FUNCTION) \
{\
    char __info__[257]={0}; \
    snprintf(__info__, 256, "[%s:%d]", file, line); \
    if(ub_assert_fatal(_c, __info__, _m)){ \
        UB_SD_PRINT_USAGE(NCONF_GEN_STATIC_MEM, UBL_ERROR); \
        EXIT_FUNCTION; \
    } \
}
#ifdef UB_SD_STATIC
#define NCONF_MEMUTILS_PTR_CHECK(_p, EXIT_FUNCTION) \
{\
    char __info__[257]={0}; \
    int dp=(uint8_t*)(_p)-(uint8_t*)(UB_CONCAT2(NCONF_GEN_STATIC_MEM, mem)); \
    uint16_t cpi=dp/(int)UB_SD_ALIGN(8u); \
    snprintf(__info__, 256, "[%s:%d]", file, line); \
    if((dp<0) || (cpi>=NCONF_GEN_ALLOC_FRAGMENTS) || ((int)cpi*(int)(8u))!=dp){\
        UB_LOG(UBL_ERROR, "%s:%s:invalid pointer=%p\n", __func__, __info__, (_p)); \
        EXIT_FUNCTION; \
    } \
}
#else
#define NCONF_MEMUTILS_PTR_CHECK(_p, EXIT_FUNCTION)
#endif
#else
#define NCONF_MEMUTILS_ASSERT(_c, _m, EXIT_FUNCTION) \
    if(ub_assert_fatal(_c, __func__, _m)){ \
        UB_SD_PRINT_USAGE(NCONF_GEN_STATIC_MEM, UBL_ERROR); \
        EXIT_FUNCTION; \
    }
#ifdef UB_SD_STATIC
#define NCONF_MEMUTILS_PTR_CHECK(_p, EXIT_FUNCTION) \
{\
    int dp=(uint8_t*)(_p)-(uint8_t*)(UB_CONCAT2(NCONF_GEN_STATIC_MEM, mem)); \
    uint16_t cpi=dp/(int)UB_SD_ALIGN(8u); \
    if((dp<0) || (cpi>=NCONF_GEN_ALLOC_FRAGMENTS) || ((int)cpi*(int)(8u))!=dp){\
        UB_LOG(UBL_ERROR, "%s:invalid pointer=%p\n", __func__, (_p)); \
        EXIT_FUNCTION; \
    } \
}
#else
#define NCONF_MEMUTILS_PTR_CHECK(_p, EXIT_FUNCTION)
#endif
#endif

/*=============================================================================
 * Global Variables
 *============================================================================*/

UB_SD_GETMEM_DEF(NCONF_GEN_STATIC_MEM, 8u, NCONF_GEN_ALLOC_FRAGMENTS);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

void* nconf_memalloc_def(ALLOC_PARAMS)
{
    void *nptr=UB_SD_GETMEM(NCONF_GEN_STATIC_MEM, size);
    NCONF_MEMUTILS_ASSERT((NULL!=nptr), "alloc error", return NULL);
    return nptr;
}

void* nconf_memrealloc_def(REALLOC_PARAMS)
{
    void *nptr=(uint8_t*)UB_SD_REGETMEM(NCONF_GEN_STATIC_MEM, optr, size);
    NCONF_MEMUTILS_ASSERT((NULL!=nptr), "realloc error", return NULL);
    return nptr;
}

char* nconf_strdup_def(STRDUP_PARAMS)
{
    size_t sz=strlen(src)+1;
    char *dst=(char*)nconf_memalloc(sz);
    NCONF_MEMUTILS_ASSERT((NULL!=dst), "strdup error", return NULL);
    memset(dst, 0, sz);
    memcpy(dst, src, sz);
    return dst;
}

void nconf_memfree_def(FREE_PARAMS)
{
    NCONF_MEMUTILS_ASSERT((NULL!=ptr), "invalid ptr", return);
    NCONF_MEMUTILS_PTR_CHECK(ptr, return);
    UB_SD_RELMEM(NCONF_GEN_STATIC_MEM, ptr);
}

void nconf_memusage(ub_dbgmsg_level_t level)
{
    UB_SD_PRINT_USAGE(NCONF_GEN_STATIC_MEM, level);
#ifndef UB_SD_STATIC
    NCONF_UNUSED(level);
#endif
}

