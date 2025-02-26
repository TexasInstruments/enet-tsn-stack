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
 * @file        nconf_types.h
 *
 * @brief       Netconf Common Data Types.
 */
#ifndef __NCONF_TYPES_H__
#define __NCONF_TYPES_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unibase.h>
#include <unibase_macros.h>
#include <unibase_binding.h>
#include "nconf_version.h"
#include "nconf_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

/**
 * \anchor Unibase Static Memory Names
 * \name Unibase Static Memory Names
 *  @{
*/
#define NCONF_GEN_STATIC_MEM        nconf_gen_static_mem
#define NCONF_UCCLIENT_HANDLE       nconf_ucclient_handle
#define NCONF_CFG_CACHE_DATAINST    nconf_cfg_cache_datainst
#define NCONF_CFG_CACHE_LISTNODE    nconf_cfg_cache_listnode
#define NCONF_XMLUTILS_SEARCH_ARGS  nconf_xmlutils_search_args
#define NCONF_MSGDEC_HANDLE         nconf_msgdec_handle
/* @} */

/** \brief general macro to resolve unused parameter warning */
#define NCONF_UNUSED(x)             (void)(x)

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

/** \brief Netconf Transport Type */
typedef enum {
    NCONF_TRANSPORT_TYPE_TLS=0x00,
    NCONF_TRANSPORT_TYPE_SUB
} nconf_trans_type_t;

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_TYPES_H__ */
