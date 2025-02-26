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
 * @file        nconf_transmngr.c
 *
 * @brief       Netconf Transport Manager Implementation.
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <string.h>
#include <lwip/init.h>
#include <lwip/stats.h>
#include <lwip/apps/fs.h>
#include <lwip/def.h>
#include <lwip/altcp.h>
#include <lwip/pbuf.h>
#include <lwip/altcp_tcp.h>
#if (NCONF_ENABLE_TCP_PLAINTEXT == 0x00U)
#include <lwip/altcp_tls.h>
#endif /* (NCONF_ENABLE_TCP_PLAINTEXT == 0x00) */
#include "nconf_server.h"
#include "nconf_msghdlr.h"
#include "nconf_msgdef.h"
#include "nconf_netstack.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

/** \brief Maximum number of simulateaous new connection request */
#define NCONF_SRV_CONN_BACKLOG      (3U)

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct x4nconfsrv {
    nconf_uccl_t dbhdl;
    uint32_t sessidctr;
#if (NCONF_ENABLE_TCP_PLAINTEXT == 0x00U)
    mbedtls_x509_crt cacert;
#endif /* (NCONF_ENABLE_TCP_PLAINTEXT == 0x00) */
    struct altcp_pcb *tlslpcb;
} x4nconfsrv_t;

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static struct altcp_pcb* nconf_transport_create_tcp_pcb(nconf_tlssrvopt_t *opt);
static int nconf_transport_enabled_ssl_client_auth(nconf_tlssrvopt_t *opt, x4nconfsrv_t *priv);
static int nconf_transport_bind_tcp_pcb(struct altcp_pcb *lpcb, uint16_t port);
static int nconf_transport_ssl_transport_init(nconf_tlssrvopt_t *opt, x4nconfsrv_t *priv);
static err_t nconf_transport_accept_tls_connection(void *arg, struct altcp_pcb *pcb, err_t err);
static err_t nconf_transport_process_tls_client_msg(void *arg, struct altcp_pcb *pcb,
                                                    struct pbuf *p, err_t err);
static void nconf_transport_process_tls_client_error(void *arg, err_t err);
static bool nconf_transport_is_message_complete(uint8_t *msg, int msglen);

/*=============================================================================
 * Global Variables
 *============================================================================*/

static x4nconfsrv_t ncsrv_priv;

/*=============================================================================
 * Function Definitions
 *============================================================================*/

static struct altcp_pcb* nconf_transport_create_tcp_pcb(nconf_tlssrvopt_t *opt)
{
    struct altcp_pcb *retpcb=NULL;
#if (NCONF_ENABLE_TCP_PLAINTEXT == 0x01U)
    retpcb=altcp_tcp_new_ip_type(IPADDR_TYPE_V4);
    if(NULL == retpcb) {
        UB_LOG(UBL_ERROR, "%s:failed to create tcp pcb\n", __func__);
    }
#else /* (NCONF_ENABLE_TCP_PLAINTEXT == 0x01) */
    /* configure tls over tcp pcb */
    struct altcp_tls_config *conf=
        altcp_tls_create_config_server_privkey_cert(
            opt->privkey, opt->privkey_len,
            opt->privkey_pass, opt->privkey_pass_len,
            opt->cert, opt->cert_len);
    if(conf!=NULL) {
        retpcb=altcp_tls_new(conf, IPADDR_TYPE_V4);
        if(NULL == retpcb) {
            UB_LOG(UBL_ERROR, "%s:failed to create tls tcp pcb\n", __func__);
            altcp_tls_free_config(conf);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to create tls config\n", __func__);
    }
#endif /* (NCONF_ENABLE_TCP_PLAINTEXT == 0x01) */
    return retpcb;
}

#if (NCONF_ENABLE_TCP_PLAINTEXT == 0x00U)
static int nconf_transport_enabled_ssl_client_auth(nconf_tlssrvopt_t *opt, x4nconfsrv_t *priv)
{
    mbedtls_ssl_context *sslctx=
        (mbedtls_ssl_context*)altcp_tls_context(priv->tlslpcb);
    int ret=-1;
    if (NULL != sslctx) {
        mbedtls_ssl_config *conf=(mbedtls_ssl_config *)sslctx->conf;
        mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_x509_crt_init(&priv->cacert);
        if(mbedtls_x509_crt_parse(&priv->cacert, opt->ca, opt->ca_len) >= 0) {
            mbedtls_ssl_conf_ca_chain(conf, &priv->cacert, NULL);
            ret=0;
        } else {
            UB_LOG(UBL_ERROR, "%s:mbedtls_x509_crt_parse() failed\n", __func__);
            mbedtls_x509_crt_free(&priv->cacert);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:ssl context not yet initialized\n", __func__);
    }
    return ret;
}
#endif /* (NCONF_ENABLE_TCP_PLAINTEXT == 0x00) */

static int nconf_transport_bind_tcp_pcb(struct altcp_pcb *lpcb, uint16_t port)
{
    int ret=-1;
    err_t alerr=altcp_bind(lpcb, IP4_ADDR_ANY, port);
    if (ERR_OK==alerr) {
        struct altcp_pcb *retpcb=
            altcp_listen_with_backlog(lpcb, NCONF_SRV_CONN_BACKLOG);
        if (NULL!=retpcb) {
            ret=0;
        } else {
            UB_LOG(UBL_ERROR, "%s:altcp_listen() failed\n", __func__);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:altcp_bind() failed [ret=%d]\n", __func__, alerr);
    }
    return ret;
}

static int nconf_transport_ssl_transport_init(nconf_tlssrvopt_t *opt, x4nconfsrv_t *priv)
{
    int ret=nconf_tlscon_mngr_init(opt);
    if (0==ret) {
        LOCK_TCPIP_CORE();
        priv->tlslpcb=nconf_transport_create_tcp_pcb(opt);
        if(NULL != priv->tlslpcb) {
#if (NCONF_ENABLE_TCP_PLAINTEXT == 0x00U)
            ret=nconf_transport_enabled_ssl_client_auth(opt, priv);
#endif /* (NCONF_ENABLE_TCP_PLAINTEXT == 0x01) */
        } else {
            ret=-1;
        }
        /* bind pcb to an address and port */
        if (0==ret) {
#if (NCONF_ENABLE_TCP_PLAINTEXT == 0x01U)
            ret=nconf_transport_bind_tcp_pcb(priv->tlslpcb, NCONF_SUB_TRANSPORT_PORT);
#else
            ret=nconf_transport_bind_tcp_pcb(priv->tlslpcb, NCONF_TLS_TRANSPORT_PORT);
#endif /* (NCONF_ENABLE_TCP_PLAINTEXT == 0x01) */
            if (0 != ret) {
                UB_LOG(UBL_ERROR, "%s:failed to bind tcp pcb\n", __func__);
            }
        }
        /* if everything is ok set new connection callback */
        if (0==ret) {
            altcp_accept(priv->tlslpcb, nconf_transport_accept_tls_connection);
        } else {
            altcp_close(priv->tlslpcb);
        }
        UNLOCK_TCPIP_CORE();
    }
    return ret;
}

static err_t nconf_transport_accept_tls_connection(void *arg, struct altcp_pcb *pcb, err_t err)
{
    x4nconfsrv_t *priv=&ncsrv_priv;
    nconf_tlscon_hdl_t conhdl=NULL;
    int ret=-1;
    UB_LOG(UBL_INFO, "%s:New Connection\n", __func__);
    ret=nconf_tlscon_accept_new_client(pcb, ++priv->sessidctr, &conhdl);
    if (0==ret && NULL!=conhdl) {
        /* set user args for the suceeding callbacks */
        altcp_arg(pcb, conhdl);
        /* Set up the various callback functions */
        altcp_recv(pcb, nconf_transport_process_tls_client_msg);
        altcp_err(pcb, nconf_transport_process_tls_client_error);
    } else {
        UB_LOG(UBL_ERROR, "%s:can't accept new client\n", __func__);
        altcp_close(pcb);
    }
    UB_LOG(UBL_INFO, "%s:End\n", __func__);
    return ERR_OK;
}

static err_t nconf_transport_process_tls_client_msg(void *arg, struct altcp_pcb *pcb,
                                                    struct pbuf *p, err_t err)
{
    nconf_tlscon_hdl_t conhdl=(nconf_tlscon_hdl_t)arg;
    nconf_queue_msg_t qmsg;
    uint8_t *pRecvMsg=NULL;
    int ret=0;
    if (nconf_tlscon_is_newconn(conhdl)) {
        /* notify message handler of the new connection */
        memset(&qmsg, 0, sizeof(nconf_queue_msg_t));
        qmsg.type=NCONF_QMSG_TYPE_CLIENT_CONNECT;
        qmsg.args.transport=NCONF_TRANSPORT_TYPE_TLS;
        qmsg.args.ch.tls=conhdl;
        ret=nconf_msghdlr_enqueue_msg(&qmsg);
        if (0!=ret) {
            UB_LOG(UBL_ERROR, "%s: initiating msg proc failed\n", __func__);
        }
        nconf_tlscon_set_newconn(conhdl, false);
    }
    if (0==ret && NULL != p) {
        int msglen=nconf_tlscon_recv_msg(conhdl, p, &pRecvMsg);
        if (msglen > 0) {
            bool msg_complete=nconf_transport_is_message_complete(pRecvMsg, msglen);
            if (msg_complete) {
                /* when message is complete unlinked the buffer so connection
                 * handler can handle next message from client */
                (void)nconf_tlscon_unlink_readbuf(conhdl, pRecvMsg);

                /* send client message to message handler */
                memset(&qmsg, 0, sizeof(nconf_queue_msg_t));
                qmsg.type=NCONF_QMSG_TYPE_CLIENT_MESSAGE;
                qmsg.args.transport=NCONF_TRANSPORT_TYPE_TLS;
                qmsg.args.ch.tls=conhdl;
                qmsg.ncmsglen=msglen;
                qmsg.ncmsg=pRecvMsg;
                int ret=nconf_msghdlr_enqueue_msg(&qmsg);
                if (0!=ret) {
                    UB_LOG(UBL_ERROR, "%s: initiating msg proc failed\n", __func__);
                }
            }
        }

        /* Inform the TCP/IP stack that we've processed 'p->tot_len'
         * bytes of received data */
        altcp_recved(pcb, p->tot_len);

        /* Free the received pbuf */
        pbuf_free(p);
    } else {
        UB_LOG(UBL_WARN, "%s:cl%u: recv Error: Disconnected\n", __func__,
               nconf_tlscon_get_sessid(conhdl));
        nconf_tlscon_disconnect_client(conhdl);
    }
    return ERR_OK;
}

static void nconf_transport_process_tls_client_error(void *arg, err_t err)
{
    nconf_tlscon_hdl_t conhdl=(nconf_tlscon_hdl_t)arg;
    UB_LOG(UBL_WARN, "%s: %s\n", __func__, lwip_strerr(err));
    if (conhdl != NULL) {
        nconf_tlscon_disconnect_client(conhdl);
    }
}

static bool nconf_transport_is_message_complete(uint8_t *msg, int msglen)
{
    uint8_t *tmp;
    bool ret=false;

    /* just check if message is complete by checking for end frame markers
     * no full parsing is done here, full parsing is done in msghdlr */

    /* Check for end-of-chunks marker '\n##\n' */
    tmp=(uint8_t*)strstr((char*)msg, NCONF_END_OF_CHUNKS_MARKER);
    if (tmp != NULL) { ret=true; }

    /* Check for end-of-chunks marker ']]>]]>' */
    tmp=(uint8_t*)strstr((char*)msg, NCONF_END_OF_MESSAGE_MARKER);
    if (tmp != NULL) { ret=true; }

    return ret;
}

int nconf_transport_init(nconf_transopt_t *sopt, nconf_uccl_t dbhdl)
{
    int ret=0;
    memset(&ncsrv_priv, 0, sizeof(ncsrv_priv));
    ret=nconf_transport_ssl_transport_init(&sopt->tls, &ncsrv_priv);
    if (0==ret) {
        ncsrv_priv.dbhdl=dbhdl;
    } else {
        UB_LOG(UBL_ERROR, "%s:SSL Transport Init failed\n", __func__);
    }
    return ret;
}

int nconf_transport_send_msg(nconf_trans_args_t *args, uint8_t *msg, uint32_t msglen)
{
    int ret=-1;
    if (NULL != msg && 0 != msglen) {
        switch(args->transport)
        {
            case NCONF_TRANSPORT_TYPE_TLS:
                ret=nconf_tlscon_send_msg(args->ch.tls, msg, msglen);
                break;
            default:
                UB_LOG(UBL_ERROR, "%s:Unsupported Transport Type\n", __func__);
                break;
        }
    } else {
        UB_LOG(UBL_ERROR, "%s: invalid params msg=%p, msglen=%u\n", __func__,
               msg, msglen);
    }
    return ret;
}

int nconf_transport_kill_session(uint32_t sessiond_id)
{
    int ret=NCONF_RPC_ERROR_TAG_INVALID_VALUE;
    void *chdl=(void*)nconf_tlscon_get_connhdl_by_sessid(sessiond_id);
    if (NULL!=chdl) {
        nconf_tlscon_disconnect_client((nconf_tlscon_hdl_t)chdl);
        ret=NCONF_RPC_ERROR_TAG_NO_ERROR;
    }
    return ret;
}

bool nconf_transport_is_session_valid(nconf_trans_args_t *args)
{
    bool ret=false;
    switch(args->transport)
    {
        case NCONF_TRANSPORT_TYPE_TLS:
            ret=nconf_tlscon_is_connection_valid(args->ch.tls);
            break;
        default:
            UB_LOG(UBL_ERROR, "%s:Unsupported Transport Type\n", __func__);
            break;
    }
    return ret;
}

uint32_t nconf_transport_get_client_sessid(nconf_trans_args_t *args)
{
    uint32_t ret=0;
    switch(args->transport)
    {
        case NCONF_TRANSPORT_TYPE_TLS:
            ret=nconf_tlscon_get_sessid(args->ch.tls);
            break;
        default:
            UB_LOG(UBL_ERROR, "%s:Unsupported Transport Type\n", __func__);
            break;
    }
    return ret;
}

void nconf_transport_release_readbuf(nconf_trans_args_t *args, uint8_t *rbuf)
{
    switch(args->transport)
    {
        case NCONF_TRANSPORT_TYPE_TLS:
            nconf_tlscon_release_readbuf(rbuf);
            break;
        default:
            UB_LOG(UBL_ERROR, "%s:Unsupported Transport Type\n", __func__);
            break;
    }
}

void nconf_transport_disconnect_client(nconf_trans_args_t *args)
{
    switch(args->transport)
    {
        case NCONF_TRANSPORT_TYPE_TLS:
            nconf_tlscon_disconnect_client(args->ch.tls);
            args->ch.tls=NULL;
            break;
        default:
            UB_LOG(UBL_ERROR, "%s:Unsupported Transport Type\n", __func__);
            break;
    }
}

void nconf_transport_deinit(void)
{
    nconf_tlscon_mngr_deinit();
}

