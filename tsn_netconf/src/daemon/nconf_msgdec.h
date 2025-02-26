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
 * @file        nconf_msgdec.h
 *
 * @brief       Netconf Message Decoder Interface Header file
 */
#ifndef __NCONF_MSGDEC_H__
#define __NCONF_MSGDEC_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include "nconf_types.h"
#include "nconf_msgdef.h"
#include "nconf_xmlutils.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_MAX_NSMAP_NUM     (5U)
#define NCONF_MAX_ATKV_NUM      (5U)

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

struct _msgdec_hdl;
typedef struct _msgdec_hdl * nconf_msgdec_t;

/*=============================================================================
 * Message Decoder APIs
 *============================================================================*/

nconf_msgdec_t nconf_msgdec_parse_client_msg(uint8_t *ncmsg, uint32_t ncmsglen,
                                             uint8_t *msgnum);
nconf_msg_type_t nconf_msgdec_get_msg_type(nconf_msgdec_t dechdl, uint8_t idx);
nconf_rpc_msg_type_t nconf_msgdec_get_rpc_msg_type(nconf_msgdec_t dechdl, uint8_t idx);
nconf_nodeptr_t nconf_msgdec_get_rootnode(nconf_msgdec_t dechdl, uint8_t idx);
nconf_nodeptr_t nconf_msgdec_get_rpcnode(nconf_msgdec_t dechdl, uint8_t idx);
nconf_nodeptr_t *nconf_msgdec_search_node(nconf_msgdec_t dechdl, uint8_t idx,
                                          nconf_xpath_args_t *sargs);
void nconf_msgdec_search_node_release(nconf_xpath_args_t *sargs);
void nconf_msgdec_release_rpcreq_doc(nconf_msgdec_t dechdl, uint8_t idx);
void nconf_msgdec_release_handle(nconf_msgdec_t dechdl);

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_MSGDEC_H__ */
