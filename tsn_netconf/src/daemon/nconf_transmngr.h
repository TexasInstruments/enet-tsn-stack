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
 * @file        nconf_transmngr.h
 *
 * @brief       Netconf Transport Manager Interface Header.
 */
#ifndef __NCONF_TRANSPORT_MANAGER_H__
#define __NCONF_TRANSPORT_MANAGER_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include "nconf_types.h"
#include "nconf_ucclient.h"
#include "nconf_transport_priv.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct nconf_transopt_priv nconf_transopt_t;

/*=============================================================================
 * Transport APIs
 *============================================================================*/

int nconf_transport_init(nconf_transopt_t *optd, nconf_uccl_t dbhdl);
int nconf_transport_send_msg(nconf_trans_args_t *args, uint8_t *msg, uint32_t msglen);
int nconf_transport_kill_session(uint32_t sessiond_id);
bool nconf_transport_is_session_valid(nconf_trans_args_t *args);
uint32_t nconf_transport_get_client_sessid(nconf_trans_args_t *args);
void nconf_transport_release_readbuf(nconf_trans_args_t *args, uint8_t *rbuf);
void nconf_transport_disconnect_client(nconf_trans_args_t *args);
void nconf_transport_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_TRANSPORT_MANAGER_H__ */
