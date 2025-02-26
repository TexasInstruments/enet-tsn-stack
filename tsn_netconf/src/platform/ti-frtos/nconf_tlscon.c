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
 * @file        nconf_tlscon.c
 *
 * @brief       Netconf TLS Connection Manager Implementation.
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
#include <lwip/altcp_tls.h>
#include "nconf_tlscon.h"
#include "nconf_memutils.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_TLSCON_CLIENT_INFO    nconf_tlscon_client_info

#define NCONF_TLSCON_INIT_CHECK(EXIT_FUNCTION) \
{\
    if (false==g_mngr_priv.is_initialized) {\
        UB_LOG(UBL_ERROR, "%s:TLS Connection Manager not yet initialized\n",\
               __func__);\
        EXIT_FUNCTION;\
    }\
}

#define NCONF_TLSCON_HANDLE_CHECK(EXIT_FUNCTION) \
{\
    uint32_t cidx; \
    if (NULL==conhdl) {\
        UB_LOG(UBL_ERROR, "%s:Invalid TLS connection Handle\n", __func__);\
        EXIT_FUNCTION;\
    }\
    for (cidx=0U; cidx < NCONF_TLS_MAX_CLIENTS; cidx++) { \
        if (conhdl==g_mngr_priv.conhdl[cidx]) { break; } \
    } \
    if (cidx >= NCONF_TLS_MAX_CLIENTS) { EXIT_FUNCTION; } \
}

#define NCONF_TLSCON_HANDLE_CHECK_AND_RELEASE(EXIT_FUNCTION) \
{\
    uint32_t cidx; \
    if (NULL==conhdl) {\
        UB_LOG(UBL_ERROR, "%s:Invalid TLS connection Handle\n", __func__);\
        EXIT_FUNCTION;\
    }\
    for (cidx=0U; cidx < NCONF_TLS_MAX_CLIENTS; cidx++) { \
        if (conhdl==g_mngr_priv.conhdl[cidx]) { \
            g_mngr_priv.conhdl[cidx]=NULL; \
            break; \
        } \
    } \
    if (cidx >= NCONF_TLS_MAX_CLIENTS) { EXIT_FUNCTION; } \
}

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

struct nconf_tlscon_hdl {
    struct altcp_pcb *conn_pcb;
    uint32_t sessid;
    uint32_t rbuflen;
    uint8_t *rbuf;
    bool newconn;
};

struct nconf_tlscon_mngr {
    nconf_tlscon_hdl_t conhdl[NCONF_TLS_MAX_CLIENTS];
    bool is_initialized;
};

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static struct nconf_tlscon_hdl* nconf_tlscon_conhdl_new(void);
static void nconf_tlscon_close_connection_pcb(struct altcp_pcb *conn_pcb);

/*=============================================================================
 * Global Variables
 *============================================================================*/

static struct nconf_tlscon_mngr g_mngr_priv = { .is_initialized=false };
UB_SD_GETMEM_DEF(NCONF_TLSCON_CLIENT_INFO,sizeof(struct nconf_tlscon_hdl),
                 NCONF_TLS_MAX_CLIENTS);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

int nconf_tlscon_mngr_init(nconf_tlssrvopt_t *sopt)
{
    int ret=-1;
    if (false==g_mngr_priv.is_initialized) {
        g_mngr_priv.is_initialized=true;
        ret=0;
    } else {
        UB_LOG(UBL_ERROR, "%s:TLS Connection Manager already initialized\n", __func__);
        NCONF_UNUSED(sopt);
    }
    return ret;
}

int nconf_tlscon_accept_new_client(nconf_nethdl_t ctx, uint32_t sessid,
                                   nconf_tlscon_hdl_t *rconhdl)
{
    struct nconf_tlscon_hdl *conhdl=NULL;
    int ret=-1;
    NCONF_TLSCON_INIT_CHECK(return -1);
    conhdl=nconf_tlscon_conhdl_new();
    if (NULL!=conhdl) {
        conhdl->sessid=sessid;
        conhdl->conn_pcb=(struct altcp_pcb *)ctx;
        conhdl->newconn=true;
        *rconhdl=conhdl;
        ret=0;
    } else {
        UB_LOG(UBL_ERROR, "%s:Unable to get new conhdl instance\n", __func__);
    }
    return ret;
}

int nconf_tlscon_recv_msg(nconf_tlscon_hdl_t conhdl, nconf_pbuf_t p, uint8_t **retbuf)
{
    NCONF_TLSCON_INIT_CHECK(return -1);
    NCONF_TLSCON_HANDLE_CHECK(return -1);
    struct pbuf *pbuf=(struct pbuf *)p;
    uint8_t *buf=conhdl->rbuf;
    uint32_t buflen=conhdl->rbuflen+pbuf->tot_len+1;
    int readlen=conhdl->rbuflen;
    buf=(uint8_t*)nconf_memrealloc(buf, buflen);
    if(ub_assert_fatal((NULL!=buf), __func__, "alloc error")){return -1;}
    memset(buf+readlen, 0, buflen-readlen);
    memcpy(buf+readlen, pbuf->payload, pbuf->tot_len);
    readlen+=pbuf->tot_len;
    conhdl->rbuf=buf;
    conhdl->rbuflen=readlen;
    *retbuf=conhdl->rbuf;
    return readlen;
}

int nconf_tlscon_send_msg(nconf_tlscon_hdl_t conhdl, uint8_t *msg, uint32_t msglen)
{
    uint16_t bytes_sent=0;
    int ret=-1;
    NCONF_TLSCON_INIT_CHECK(return -1);
    NCONF_TLSCON_HANDLE_CHECK(return -1);
    LOCK_TCPIP_CORE();
    do {
        uint16_t snd_len=msglen-bytes_sent;
        uint16_t snd_buf=altcp_sndbuf(conhdl->conn_pcb);
        if (snd_buf < snd_len) { snd_len=snd_buf; }
        err_t err=altcp_write(conhdl->conn_pcb, msg+bytes_sent, snd_len,
                              TCP_WRITE_FLAG_COPY | TCP_WRITE_FLAG_MORE);
        if (err != ERR_OK) {
            UB_LOG(UBL_ERROR, "%s:altcp_write() failed [ret=%d]\n", __func__, err);
            break;
        }
        bytes_sent += snd_len;
        ret=bytes_sent;
    } while(bytes_sent < msglen);
    altcp_output(conhdl->conn_pcb);
    UNLOCK_TCPIP_CORE();
    return ret;
}

uint8_t* nconf_tlscon_unlink_readbuf(nconf_tlscon_hdl_t conhdl, uint8_t *rbuf)
{
    uint8_t *unlinked_buf=NULL;
    NCONF_TLSCON_INIT_CHECK(return NULL);
    NCONF_TLSCON_HANDLE_CHECK(return NULL);
    if(conhdl->rbuf==rbuf) {
        unlinked_buf=conhdl->rbuf;
        conhdl->rbuf=NULL;
        conhdl->rbuflen=0U;
    } else if(conhdl->rbuf==NULL) {
        UB_LOG(UBL_DEBUGV, "%s:rbuf is already unlinked\n", __func__);
    } else {
        UB_LOG(UBL_WARN, "%s:rbuf is different from cached buffer\n", __func__);
    }
    return unlinked_buf;
}

nconf_tlscon_hdl_t nconf_tlscon_get_connhdl_by_sessid(uint32_t sessiond_id)
{
    nconf_tlscon_hdl_t conhdl=NULL;
    uint32_t cidx;
    NCONF_TLSCON_INIT_CHECK(return NULL);
    for (cidx=0U; cidx < NCONF_TLS_MAX_CLIENTS; cidx++) {
        if (g_mngr_priv.conhdl[cidx]->sessid==sessiond_id) {
            conhdl=g_mngr_priv.conhdl[cidx];
            break;
        }
    }
    if (NULL==conhdl) {
        UB_LOG(UBL_WARN, "%s:session-id: [%u] not found\n", __func__, sessiond_id);
    }
    return conhdl;
}

void nconf_tlscon_release_readbuf(uint8_t *rbuf)
{
    if (NULL != rbuf) {
        nconf_memfree(rbuf);
    }
}

void nconf_tlscon_disconnect_client(nconf_tlscon_hdl_t conhdl)
{
    NCONF_TLSCON_INIT_CHECK(return);
    NCONF_TLSCON_HANDLE_CHECK_AND_RELEASE(return);
    UB_LOG(UBL_WARN, "%s: disconnecting client=%u\n", __func__, conhdl->sessid);
    nconf_tlscon_close_connection_pcb(conhdl->conn_pcb);
    conhdl->conn_pcb=NULL;
    if (NULL!=conhdl->rbuf) {
        nconf_tlscon_release_readbuf(conhdl->rbuf);
        conhdl->rbuf=NULL;
        conhdl->rbuflen=0U;
    }
    UB_SD_RELMEM(NCONF_TLSCON_CLIENT_INFO, conhdl);
}

bool nconf_tlscon_is_connection_valid(nconf_tlscon_hdl_t conhdl)
{
    NCONF_TLSCON_INIT_CHECK(return false);
    NCONF_TLSCON_HANDLE_CHECK(return false);
    return true;
}

uint32_t nconf_tlscon_get_sessid(nconf_tlscon_hdl_t conhdl)
{
    NCONF_TLSCON_INIT_CHECK(return 0);
    NCONF_TLSCON_HANDLE_CHECK(return 0);
    return conhdl->sessid;
}

void nconf_tlscon_mngr_deinit(void)
{
    uint32_t cidx;
    NCONF_TLSCON_INIT_CHECK(return);
    for (cidx=0U; cidx < NCONF_TLS_MAX_CLIENTS; cidx++) {
        if (NULL!=g_mngr_priv.conhdl[cidx]) {
            nconf_tlscon_disconnect_client(g_mngr_priv.conhdl[cidx]);
        }
    }
    g_mngr_priv.is_initialized=false;
}

bool nconf_tlscon_is_newconn(nconf_tlscon_hdl_t conhdl)
{
    NCONF_TLSCON_INIT_CHECK(return false);
    NCONF_TLSCON_HANDLE_CHECK(return false);
    return conhdl->newconn;
}

void nconf_tlscon_set_newconn(nconf_tlscon_hdl_t conhdl, bool newflag)
{
    NCONF_TLSCON_INIT_CHECK(return);
    NCONF_TLSCON_HANDLE_CHECK(return);
    conhdl->newconn=newflag;
}

static struct nconf_tlscon_hdl* nconf_tlscon_conhdl_new(void)
{
    struct nconf_tlscon_hdl *conhdl=NULL;
    uint32_t cidx;
    for(cidx=0U; cidx < NCONF_TLS_MAX_CLIENTS; cidx++) {
        if (NULL==g_mngr_priv.conhdl[cidx]) { break; }
    }
    if (cidx >= NCONF_TLS_MAX_CLIENTS) {
        UB_LOG(UBL_WARN, "%s:Max TLS clients reached\n", __func__);
        goto end_oper;
    }
    g_mngr_priv.conhdl[cidx]=(struct nconf_tlscon_hdl*)
        UB_SD_GETMEM(NCONF_TLSCON_CLIENT_INFO,
        sizeof(struct nconf_tlscon_hdl));
    if (NULL!=g_mngr_priv.conhdl[cidx]) {
        conhdl=g_mngr_priv.conhdl[cidx];
        memset(conhdl, 0, sizeof(struct nconf_tlscon_hdl));
    } else {
        UB_LOG(UBL_ERROR, "%s:Failed to allocate connection handle\n", __func__);
    }
end_oper:
    return conhdl;
}

static void nconf_tlscon_close_connection_pcb(struct altcp_pcb *conn_pcb)
{
    if (NULL != conn_pcb) {
        LOCK_TCPIP_CORE();
        altcp_close(conn_pcb);
        UNLOCK_TCPIP_CORE();
    }
}

