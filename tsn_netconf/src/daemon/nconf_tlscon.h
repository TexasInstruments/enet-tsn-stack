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
 * @file        nconf_tlscon.h
 *
 * @brief       Netconf TLS Connection Manager Interface Header
 */
#ifndef __NCONF_TLSCON_MNGR_H__
#define __NCONF_TLSCON_MNGR_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include "nconf_tlscon_priv.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

/** \brief Maximum number of simultaneous tls client connection */
#define NCONF_TLS_MAX_CLIENTS       (5U)

/*=============================================================================
 * TLS Connection Manager Common APIs
 *============================================================================*/

int nconf_tlscon_mngr_init(nconf_tlssrvopt_t *sopt);
int nconf_tlscon_accept_new_client(nconf_nethdl_t ctx, uint32_t sessid,
                                   nconf_tlscon_hdl_t *rconhdl);
int nconf_tlscon_recv_msg(nconf_tlscon_hdl_t chdl, nconf_pbuf_t p, uint8_t **retbuf);
int nconf_tlscon_send_msg(nconf_tlscon_hdl_t chdl, uint8_t *msg, uint32_t msglen);
bool nconf_tlscon_is_connection_valid(nconf_tlscon_hdl_t chdl);
uint32_t nconf_tlscon_get_sessid(nconf_tlscon_hdl_t chdl);
uint8_t* nconf_tlscon_unlink_readbuf(nconf_tlscon_hdl_t chdl, uint8_t *rbuf);
nconf_tlscon_hdl_t nconf_tlscon_get_connhdl_by_sessid(uint32_t sessiond_id);
void nconf_tlscon_release_readbuf(uint8_t *rbuf);
void nconf_tlscon_disconnect_client(nconf_tlscon_hdl_t chdl);
void nconf_tlscon_mngr_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_TLSCON_MNGR_H__ */
