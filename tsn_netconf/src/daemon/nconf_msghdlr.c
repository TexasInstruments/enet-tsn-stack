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
 * @file        nconf_msghdlr.c
 *
 * @brief       Netconf Message Handler Implementation.
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <string.h>
#include <stdlib.h>
#include "nconf_server.h"
#include "nconf_msghdlr.h"
#include "nconf_msgqueue.h"
#include "nconf_msgdef.h"
#include "nconf_msgdec.h"
#include "nconf_msgenc.h"
#include "nconf_memutils.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_MAX_BUF_SIZE          (1500U)
#define NCONF_MAX_CHUNK_HDR_LEN     (10U)
#define NCONF_INITIAL_SUBTREE_COUNT (2U)

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct nconf_msghdlr_priv {
    nconf_msgq_t mqhdl;
    nconf_uccl_t dbhdl;
} nconf_msghdlr_priv_t;

/*=============================================================================
 * Global Variables
 *============================================================================*/

static nconf_msghdlr_priv_t g_msghdlr_priv = { .mqhdl=NULL, .dbhdl=NULL };

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static void nconf_msghdlr_proc(nconf_queue_msg_t *qmsg, bool *isShutdownMsg);
static void nconf_msghdlr_client_connect_proc(nconf_queue_msg_t *qmsg);
static void nconf_msghdlr_client_msg_proc(nconf_queue_msg_t *qmsg);
static void nconf_msghdlr_hello_msg_proc(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static void nconf_msghdlr_rpc_msg_proc(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static void nconf_msghdlr_rpc_msg_proc_get(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static void nconf_msghdlr_rpc_msg_proc_get_config(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static void nconf_msghdlr_rpc_msg_proc_edit_config(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static void nconf_msghdlr_rpc_msg_proc_get_schema(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static void nconf_msghdlr_rpc_msg_proc_close_session(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static void nconf_msghdlr_rpc_msg_proc_kill_session(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl, uint8_t didx);
static nconf_nodeptr_t nconf_msghdlr_get_finfo(nconf_msgdec_t dechdl, uint8_t didx,
                                               nconf_filter_info_t **rfinfo);
static nconf_filter_info_t* nconf_msghdlr_get_default_finfo(nconf_uccl_t dbhdl);
static nconf_filter_info_t* nconf_msghdlr_get_rpcreq_finfo(nconf_nodeptr_t filter);
static nconf_treeinfo_t* nconf_msghdlr_gen_module_subtree(char *modname);
static int nconf_msghdlr_evaluate_filter(nconf_filter_info_t *finfo, nconf_nodeptr_t filter);
static int nconf_msghdlr_get_subtree_filters(nconf_filter_info_t *finfo, nconf_nodeptr_t fchild);
static int nconf_msghdlr_send_db_data(nconf_trans_args_t *args, nconf_filter_info_t *finfo,
                                      nconf_ucclcfg_type_t type);
static bool nconf_msghdlr_cfgnode_cmp(struct ub_list_node *node1, struct ub_list_node *node2,
                                      void *uargs);
static int nconf_msghdlr_send_nodeptr(nconf_nodeptr_t nodeptr, void *uargs);
static int nconf_msghdlr_send_nodeptr_header(nconf_nodeptr_t nodeptr, void *uargs);
static int nconf_msghdlr_send_nodeptr_footer(char *nodename, void *uargs);
static nconf_docptr_t nconf_msghdlr_generate_dummy_dbtree(void);
static bool nconf_msghdlr_is_value_key(const char *nodepath, uint8_t *kvidx);
static void nconf_msghdlr_release_filter_resource(nconf_uccl_t dbhdl, nconf_filter_info_t *finfo);
static int nconf_msghdlr_exec_config_update(nconf_nodeptr_t config);
static nconf_update_info_t* nconf_msghdlr_get_update_list(nconf_nodeptr_t config);
static int nconf_msghdlr_validate_update_list(nconf_treeinfo_t *configs);
static void nconf_msghdlr_release_update_resource(nconf_update_info_t *uinfo);
static void nconf_msghdlr_gen_missing_element_err_info(nconf_rpcerror_args_t *errargs,
                                                       char *missing_element);
static int nconf_msghdlr_send_ok_rpc_reply(nconf_queue_msg_t *qmsg, nconf_msgdec_t dechdl,
                                           uint8_t didx);
static int nconf_msghdlr_send_rpc_error(nconf_queue_msg_t *qmsg, nconf_rpcerror_args_t *errargs,
                                        nconf_msgdec_t dechdl, uint8_t didx);
static int nconf_msghdlr_send_eom_frame(nconf_trans_args_t *args, uint8_t *msg,
                                        uint32_t msglen, bool iscomplete);
static int nconf_msghdlr_send_chunked_frame(nconf_trans_args_t *args, uint8_t *msg,
                                            uint32_t msglen, bool iscomplete);
static int nconf_msghdlr_send_get_schema_resp(nconf_trans_args_t *args,  uint8_t *msg,
                                              uint32_t msglen, nconf_fshdl_t hdl);
static void nconf_msghdlr_display_filter_info(nconf_filter_info_t *finfo);
static void nconf_msghdlr_display_update_info(nconf_update_info_t *uinfo);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

int nconf_msghdlr_init(nconf_uccl_t dbhdl)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    int ret=nconf_msgq_init(&priv->mqhdl, 0, sizeof(nconf_queue_msg_t));
    if (0==ret) {
        nconf_xmlutils_lib_init();
        priv->dbhdl=dbhdl;
    }
    return ret;
}

int nconf_msghdlr_enqueue_msg(nconf_queue_msg_t *qmsg)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    int ret=0;
    if (priv->mqhdl != NULL) {
        ret=nconf_msgq_send(priv->mqhdl, qmsg, sizeof(nconf_queue_msg_t));
        if(0 != ret) {
            UB_LOG(UBL_ERROR, "%s:failed to send message to queue\n", __func__);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:message handler not yet initialized\n", __func__);
    }
    return ret;
}

int nconf_msghdlr_stop(void)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    nconf_queue_msg_t qmsg;
    int ret=0;
    if (priv->mqhdl != NULL) {
        memset(&qmsg, 0, sizeof(nconf_queue_msg_t));
        qmsg.type=NCONF_QMSG_TYPE_SYSTEM_SHUTDOWN;
        ret=nconf_msgq_send(priv->mqhdl, &qmsg, sizeof(nconf_queue_msg_t));
        if(0 != ret) {
            UB_LOG(UBL_ERROR, "%s:failed to send shutdown message\n", __func__);
        }
    } else {
        UB_LOG(UBL_WARN, "%s:invalid message queue handle\n", __func__);
    }
    return ret;
}

void nconf_msghdlr_deinit(void)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    if (priv->mqhdl != NULL) {
        nconf_msgq_deinit(&priv->mqhdl);
    } else {
        UB_LOG(UBL_DEBUG, "%s:msg handler is not yet initialized\n", __func__);
    }
}

void *nconf_msghdlr_main_loop(void *args)
{
    struct _msgq_hdl *mqhdl=g_msghdlr_priv.mqhdl;
    nconf_queue_msg_t rcvbuf;
    bool isShutdownMsg= false;
    int ret=0;
    UB_LOG(UBL_INFO, "%s:Started\n", __func__);
    while (!isShutdownMsg) {
        memset(&rcvbuf, 0, sizeof(rcvbuf));
        ret=nconf_msgq_recv(mqhdl, &rcvbuf, sizeof(rcvbuf));
        if (0==ret) {
            nconf_msghdlr_proc(&rcvbuf, &isShutdownMsg);
        } else {
            UB_LOG(UBL_ERROR, "%s:nconf_msgq_recv() failed\n", __func__);
        }
    }
    UB_LOG(UBL_WARN, "%s:Ended\n", __func__);
    NCONF_UNUSED(args);
    return NULL;
}

/* Static Functions */

static void nconf_msghdlr_proc(nconf_queue_msg_t *qmsg, bool *isShutdownMsg)
{
    if (NCONF_QMSG_TYPE_SYSTEM_SHUTDOWN==qmsg->type) {
        *isShutdownMsg=true;
    } else if (nconf_transport_is_session_valid(&qmsg->args)) {
        switch(qmsg->type)
        {
            case NCONF_QMSG_TYPE_CLIENT_CONNECT:
                nconf_msghdlr_client_connect_proc(qmsg);
                break;
            case NCONF_QMSG_TYPE_CLIENT_MESSAGE:
                nconf_msghdlr_client_msg_proc(qmsg);
                break;
            default:
                UB_LOG(UBL_WARN, "%s:ignoring unknown qmsg type\n", __func__);
                break;
        }
    } else {
        nconf_transport_release_readbuf(&qmsg->args, qmsg->ncmsg);
    }
}

static void nconf_msghdlr_client_connect_proc(nconf_queue_msg_t *qmsg)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    uint8_t *server_hello=NULL;
    uint32_t msglen=0U, caplen=0U;
    uint32_t session_id=0U;
    int ret;

    /* construct and encode server hello message */
    char *capstr=nconf_ucclient_get_capabilities(priv->dbhdl, &caplen);
    if(NULL != capstr) {
        session_id=nconf_transport_get_client_sessid(&qmsg->args);
        server_hello=nconf_msgenc_server_hello(capstr, caplen, session_id, &msglen);
    }
    nconf_ucclient_release_caps_resource(priv->dbhdl, (void*)capstr);
    if (NULL != server_hello) {
        /* send the <hello> message using end-of-message framing */
        ret=nconf_msghdlr_send_eom_frame(&qmsg->args, server_hello, msglen, true);
        if (ret <= 0) {
            UB_LOG(UBL_ERROR, "%s:sending hello message failed\n", __func__);
        }
        nconf_msgenc_release_buffer(server_hello);
    } else {
        nconf_transport_disconnect_client(&qmsg->args);
    }
}

static void nconf_msghdlr_client_msg_proc(nconf_queue_msg_t *qmsg)
{
    nconf_msgdec_t dechdl=NULL;
    uint8_t msgnum=0;
    bool disconnect=false;
    dechdl=nconf_msgdec_parse_client_msg(qmsg->ncmsg, qmsg->ncmsglen, &msgnum);
    if (NULL != dechdl) {
        uint8_t i;
        for (i=0; i<msgnum; i++) {
            switch(nconf_msgdec_get_msg_type(dechdl, i)) {
                case NCONF_MSG_TYPE_HELLO:
                    nconf_msghdlr_hello_msg_proc(qmsg, dechdl, i);
                    break;
                case NCONF_MSG_TYPE_RPC:
                    nconf_msghdlr_rpc_msg_proc(qmsg, dechdl, i);
                    break;
                case NCONF_MSG_TYPE_CLOSE_SESSION:
                    UB_LOG(UBL_WARN, "%s:received a non-rpc close-session\n", __func__);
                    break;
                default:
                    UB_LOG(UBL_ERROR, "%s:unknown message type\n", __func__);
                    disconnect=true;
                    break;
            }
        }
        nconf_transport_release_readbuf(&qmsg->args, qmsg->ncmsg);
        if (disconnect) {
            nconf_transport_disconnect_client(&qmsg->args);
        }
        nconf_msgdec_release_handle(dechdl);
    } else {
        UB_LOG(UBL_ERROR, "%s:client message parse failed\n", __func__);
        nconf_transport_disconnect_client(&qmsg->args);
    }
}

static void nconf_msghdlr_hello_msg_proc(nconf_queue_msg_t *qmsg,
                                         nconf_msgdec_t dechdl, uint8_t didx)
{
    nconf_xpath_args_t *search_prm=nconf_xmlutils_get_search_handle();
    if (NULL != search_prm) {
        nconf_nodeptr_t *retnode=NULL;
        memset(search_prm, 0, sizeof(nconf_xpath_args_t));
        search_prm->nodes[0].name="capability";
        search_prm->nodes[0].value="urn:ietf:params:netconf:base:1.1";
        search_prm->nodesnum++;
        retnode=nconf_msgdec_search_node(dechdl, didx, search_prm);
        if (NULL==retnode) {
            UB_LOG(UBL_ERROR, "%s:required capability not found\n", __func__);
            nconf_transport_disconnect_client(&qmsg->args);
            nconf_msgdec_search_node_release(search_prm);
        } else {
            nconf_msgdec_search_node_release(search_prm);
            search_prm->nodes[0].name="session-id";
            search_prm->nodes[0].value=NULL;
            retnode=nconf_msgdec_search_node(dechdl, didx, search_prm);
            if (NULL!=retnode) {
                UB_LOG(UBL_ERROR, "%s:<session-id> is not expected\n", __func__);
                nconf_transport_disconnect_client(&qmsg->args);
            }
            nconf_msgdec_search_node_release(search_prm);
        }
        nconf_xmlutils_release_search_handle(search_prm);
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to get search handle\n", __func__);
    }
}

static void nconf_msghdlr_rpc_msg_proc(nconf_queue_msg_t *qmsg,
                                       nconf_msgdec_t dechdl, uint8_t didx)
{
    switch(nconf_msgdec_get_rpc_msg_type(dechdl, didx)) {
        case NCONF_RPC_MSG_TYPE_GET:
            nconf_msghdlr_rpc_msg_proc_get(qmsg, dechdl, didx);
            break;
        case NCONF_RPC_MSG_TYPE_GET_CONFIG:
            nconf_msghdlr_rpc_msg_proc_get_config(qmsg, dechdl, didx);
            break;
        case NCONF_RPC_MSG_TYPE_EDIT_CONFIG:
            nconf_msghdlr_rpc_msg_proc_edit_config(qmsg, dechdl, didx);
            break;
        case NCONF_RPC_MSG_TYPE_DELETE_CONFIG:
            UB_LOG(UBL_INFO, "%s: RPC MSG: <delete-config>\n", __func__);
            break;
        case NCONF_RPC_MSG_TYPE_LOCK:
            UB_LOG(UBL_INFO, "%s: RPC MSG: <lock>\n", __func__);
            break;
        case NCONF_RPC_MSG_TYPE_UNLOCK:
            UB_LOG(UBL_INFO, "%s: RPC MSG: <unlock>\n", __func__);
            break;
        case NCONF_RPC_MSG_TYPE_CLOSE_SESSION:
            nconf_msghdlr_rpc_msg_proc_close_session(qmsg, dechdl, didx);
            break;
        case NCONF_RPC_MSG_TYPE_KILL_SESSION:
            nconf_msghdlr_rpc_msg_proc_kill_session(qmsg, dechdl, didx);
            break;
        case NCONF_RPC_MSG_TYPE_GET_SCHEMA:
            nconf_msghdlr_rpc_msg_proc_get_schema(qmsg, dechdl, didx);
            break;
        default:
            break;
    }
}

static void nconf_msghdlr_rpc_msg_proc_get(nconf_queue_msg_t *qmsg,
                                           nconf_msgdec_t dechdl, uint8_t didx)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    nconf_filter_info_t *finfo=NULL;
    nconf_nodeptr_t rpcreq=NULL;
    uint8_t *rpc_reply=NULL;
    uint32_t msglen=0U;
    int ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;

    UB_LOG(UBL_DEBUG, "%s: RPC MSG: <get>\n", __func__);
    rpcreq=nconf_msghdlr_get_finfo(dechdl, didx, &finfo);
    if (NULL != rpcreq && NULL != finfo) {
        nconf_docptr_t dbtree=nconf_msghdlr_generate_dummy_dbtree();
        if (NULL != dbtree) {
            nconf_nodeptr_t data=nconf_xmlutils_get_root_node(dbtree);
            if (NULL != data) {
                rpc_reply=nconf_msgenc_rpc_reply(rpcreq, data, &msglen);
                if (NULL != rpc_reply) { ret=NCONF_RPC_ERROR_TAG_NO_ERROR; }
                nconf_xmlutils_release_node(data);
            }
            nconf_xmlutils_free_doc(dbtree);
        }
        nconf_msgdec_release_rpcreq_doc(dechdl, didx);
    } else {
        ret=NCONF_RPC_ERROR_TAG_MALFORMED_MESSAGE;
    }

    if (NCONF_RPC_ERROR_TAG_NO_ERROR==ret) {
        nconf_ucclcfg_type_t type=NCONF_UCCLIENT_CFG_TYPE_ALL;
        uint32_t partlen=0;
        uint8_t *temp=(uint8_t*)strstr((const char *)rpc_reply, "dummy");
        if (NULL != temp) { temp[0]='\0'; partlen=strlen((char*)rpc_reply); }

        ret=nconf_msghdlr_send_chunked_frame(&qmsg->args, rpc_reply, partlen, false);
        if (ret > 0) {
            ret=nconf_msghdlr_send_db_data(&qmsg->args, finfo, type);
            if (ret < 0) {
                UB_LOG(UBL_ERROR, "%s:sending db data failed\n", __func__);
            }

            /* send end of <data> tag regardless of above status */
            temp[0]='d';
            temp=(uint8_t*)strstr((const char *)temp, "</data>");
            if (NULL != temp) {
                partlen=strlen((char*)temp);
                ret=nconf_msghdlr_send_chunked_frame(&qmsg->args, temp, partlen, true);
                if (ret <= 0) {
                    UB_LOG(UBL_ERROR, "%s:sending get rpc footer failed\n", __func__);
                }
            } else {
                UB_LOG(UBL_WARN, "%s:<data> end tag not found\n", __func__);
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:sending get rpc hdr failed\n", __func__);
            nconf_transport_disconnect_client(&qmsg->args);
        }
        XMLUTILS_NODE_RELEASE_ALL_INST();
        nconf_msgenc_release_buffer(rpc_reply);
    } else {
        nconf_rpcerror_args_t errargs;
        UB_LOG(UBL_ERROR, "%s:RPC MSG: processing failed\n", __func__);
        memset(&errargs, 0, sizeof(errargs));
        errargs.tag=(nconf_rpcerror_tag_t)ret;
        errargs.type=NCONF_RPC_ERROR_TYPE_RPC;
        errargs.severity=NCONF_RPC_ERROR_SEV_ERROR;
        ret=nconf_msghdlr_send_rpc_error(qmsg, &errargs, dechdl, didx);
    }
    nconf_msghdlr_release_filter_resource(priv->dbhdl, finfo);
    nconf_ucclient_dbtree_release(priv->dbhdl);
}

static void nconf_msghdlr_rpc_msg_proc_get_config(nconf_queue_msg_t *qmsg,
                                                  nconf_msgdec_t dechdl, uint8_t didx)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    nconf_filter_info_t *finfo=NULL;
    nconf_nodeptr_t rpcreq=NULL;
    uint8_t *rpc_reply=NULL;
    uint32_t msglen=0U;
    int ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;

    UB_LOG(UBL_DEBUG, "%s: RPC MSG: <get-config>\n", __func__);
    rpcreq=nconf_msghdlr_get_finfo(dechdl, didx, &finfo);
    if (NULL != rpcreq && NULL != finfo) {
        nconf_docptr_t dbtree=nconf_msghdlr_generate_dummy_dbtree();
        if (NULL != dbtree) {
            nconf_nodeptr_t data=nconf_xmlutils_get_root_node(dbtree);
            if (NULL != data) {
                rpc_reply=nconf_msgenc_rpc_reply(rpcreq, data, &msglen);
                if (NULL != rpc_reply) { ret=NCONF_RPC_ERROR_TAG_NO_ERROR; }
            }
            nconf_xmlutils_free_doc(dbtree);
        }
        nconf_msgdec_release_rpcreq_doc(dechdl, didx);
    } else {
        ret=NCONF_RPC_ERROR_TAG_MALFORMED_MESSAGE;
    }

    if (NCONF_RPC_ERROR_TAG_NO_ERROR==ret) {
        nconf_ucclcfg_type_t type=NCONF_UCCLIENT_CFG_TYPE_RW;
        uint32_t partlen=0;
        uint8_t *temp=(uint8_t*)strstr((const char *)rpc_reply, "dummy");
        if (NULL != temp) { temp[0]='\0'; partlen=strlen((char*)rpc_reply); }

        ret=nconf_msghdlr_send_chunked_frame(&qmsg->args, rpc_reply, partlen, false);
        if (ret > 0) {
            ret=nconf_msghdlr_send_db_data(&qmsg->args, finfo, type);
            if (ret < 0) {
                UB_LOG(UBL_ERROR, "%s:sending db data failed\n", __func__);
            }

            /* send end of <data> tag regardless of above status */
            temp[0]='d';
            temp=(uint8_t*)strstr((const char *)temp, "</data>");
            if (NULL != temp) {
                partlen=strlen((char*)temp);
                ret=nconf_msghdlr_send_chunked_frame(&qmsg->args, temp, partlen, true);
                if (ret <= 0) {
                    UB_LOG(UBL_ERROR, "%s:sending get rpc footer failed\n", __func__);
                }
            } else {
                UB_LOG(UBL_WARN, "%s:<data> end tag not found\n", __func__);
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:sending get rpc hdr failed\n", __func__);
            nconf_transport_disconnect_client(&qmsg->args);
        }
        XMLUTILS_NODE_RELEASE_ALL_INST();
        nconf_msgenc_release_buffer(rpc_reply);
    } else {
        nconf_rpcerror_args_t errargs;
        UB_LOG(UBL_ERROR, "%s:RPC MSG: processing failed\n", __func__);
        memset(&errargs, 0, sizeof(errargs));
        errargs.tag=(nconf_rpcerror_tag_t)ret;
        errargs.type=NCONF_RPC_ERROR_TYPE_RPC;
        errargs.severity=NCONF_RPC_ERROR_SEV_ERROR;
        ret=nconf_msghdlr_send_rpc_error(qmsg, &errargs, dechdl, didx);
    }
    nconf_msghdlr_release_filter_resource(priv->dbhdl, finfo);
    nconf_ucclient_dbtree_release(priv->dbhdl);
}

static void nconf_msghdlr_rpc_msg_proc_edit_config(nconf_queue_msg_t *qmsg,
                                                   nconf_msgdec_t dechdl,
                                                   uint8_t didx)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    nconf_nodeptr_t config=NULL;
    nconf_nodeptr_t rpcnode=NULL;
    nconf_nodeptr_t child=NULL;
    int ret=NCONF_RPC_ERROR_TAG_NO_ERROR;

    UB_LOG(UBL_INFO, "%s: RPC MSG: <edit-config>\n", __func__);

    rpcnode=nconf_msgdec_get_rpcnode(dechdl, didx);
    if (NULL != rpcnode) {
        /* get <edit-config> parameters */
        child=XMLUTILS_NODE_GET_CHILD(rpcnode);
        while(child) {
            nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(child);
            if (strcmp(XMLUTILS_NODE_GET_NAME(child), "target")==0) {
                nconf_nodeptr_t target=XMLUTILS_NODE_GET_CHILD(child);
                if (NULL != target) {
                    if (strcmp(XMLUTILS_NODE_GET_NAME(target), "running")!=0) {
                        UB_LOG(UBL_ERROR, "%s: target=[%s] datastore is not supported\n",
                               __func__, XMLUTILS_NODE_GET_NAME(target));
                        ret=NCONF_RPC_ERROR_TAG_INVALID_VALUE;
                    }
                    nconf_xmlutils_release_node(target);
                } else {
                    ret=NCONF_RPC_ERROR_TAG_MALFORMED_MESSAGE;
                }
                nconf_xmlutils_release_node(child);
            } else if (strcmp(XMLUTILS_NODE_GET_NAME(child), "config")==0) {
                config=child;
            } else {
                nconf_xmlutils_release_node(child);
            }
            /* move to the next sibling */
            child=sibling;
        }
        if (NULL==config) { ret=NCONF_RPC_ERROR_TAG_MISSING_ELEMENT; }
        if (NCONF_RPC_ERROR_TAG_NO_ERROR==ret) {
            ret=nconf_msghdlr_exec_config_update(config);
        }
        if (NCONF_RPC_ERROR_TAG_NO_ERROR==ret) {
            ret=nconf_ucclient_exec_config_save(priv->dbhdl);
        }
        if (NCONF_RPC_ERROR_TAG_NO_ERROR==ret) {
            ret=nconf_msghdlr_send_ok_rpc_reply(qmsg, dechdl, didx);
            if (ret <= 0) {
                UB_LOG(UBL_ERROR, "%s:RPC MSG: failed sending rpc reply\n", __func__);
            }
        } else {
            nconf_rpcerror_args_t errargs;
            memset(&errargs, 0, sizeof(errargs));
            errargs.tag=(nconf_rpcerror_tag_t)ret;
            if (NCONF_RPC_ERROR_TAG_DATA_MISSING==ret ||
                NCONF_RPC_ERROR_TAG_DATA_EXISTS==ret) {
                errargs.type=NCONF_RPC_ERROR_TYPE_APPLICATION;
            } else if (NCONF_RPC_ERROR_TAG_MISSING_ELEMENT==ret) {
                errargs.type=NCONF_RPC_ERROR_TYPE_PROTOCOL;
                nconf_msghdlr_gen_missing_element_err_info(&errargs, "config");
            } else {
                errargs.type=NCONF_RPC_ERROR_TYPE_RPC;
            }
            errargs.severity=NCONF_RPC_ERROR_SEV_ERROR;
            ret=nconf_msghdlr_send_rpc_error(qmsg, &errargs, dechdl, didx);
        }
        XMLUTILS_NODE_RELEASE_ALL_INST();
    } else {
        UB_LOG(UBL_ERROR, "%s:Failed to get request root node\n", __func__);
    }
}

static void nconf_msghdlr_get_field_values(nconf_nodeptr_t node,
                                           nconf_kval_t *prms, int count)
{
    nconf_nodeptr_t child=XMLUTILS_NODE_GET_CHILD(node);
    int i;
    while(child) {
        for (i=0; i<count; i++) {
            if (strcmp(XMLUTILS_NODE_GET_NAME(child), prms[i].key)==0) {
                if (NULL==prms[i].val) {
                    prms[i].val=nconf_xmlutils_get_node_content(child);
                } else {
                    UB_LOG(UBL_WARN, "%s:same child node name found\n", __func__);
                }
            }
        }
        /* get the next sibling */
        nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(child);
        nconf_xmlutils_release_node(child);
        child=sibling;
    }
}

static void nconf_msghdlr_free_field_values(nconf_kval_t *prms, int count)
{
    int i;
    for (i=0; i<count; i++) {
        if (NULL != prms[i].val) {
            nconf_xmlutils_free((uint8_t*)prms[i].val);
        }
    }
}

static void nconf_msghdlr_rpc_msg_proc_get_schema(nconf_queue_msg_t *qmsg,
                                                  nconf_msgdec_t dechdl,
                                                  uint8_t didx)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    nconf_nodeptr_t rpcreq=NULL;
    nconf_nodeptr_t rpcreq_oper=NULL;
    nconf_nodeptr_t schema_data=NULL;
    nconf_kval_t req_prms[3] = {
        { "identifier"  , NULL },
        { "version"     , NULL },
        { "format"      , NULL },
    };
    uint8_t *rpc_reply=NULL;
    uint32_t msglen=0U;
    nconf_fshdl_t schemahdl=NULL;
    char *module=NULL;
    char *version=NULL;
    char *format=NULL;
    int ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;

    rpcreq=nconf_msgdec_get_rootnode(dechdl, didx);
    if (NULL != rpcreq) {
        rpcreq_oper=nconf_msgdec_get_rpcnode(dechdl, didx);
        nconf_msghdlr_get_field_values(rpcreq_oper, req_prms, 3);
    } else {
        UB_LOG(UBL_ERROR, "%s:Failed to get request root node\n", __func__);
    }

    if (req_prms[0].val != NULL) {
        module=req_prms[0].val;
        version=req_prms[1].val;
        format=req_prms[2].val;
        if (NULL != format) {
            format=strstr(req_prms[2].val, ":") ?
                strstr(req_prms[2].val, ":")+1 : req_prms[2].val;
        }
        schemahdl=nconf_ucclient_get_schema_hdl(priv->dbhdl, module, version, format);
    }

    if(NULL != schemahdl) {
        schema_data=nconf_xmlutils_new_node(NULL, "data", "dummy");
        if (schema_data != NULL) {
            nconf_xmlutils_copy_node_nsdefs(schema_data, rpcreq_oper);
            rpc_reply=nconf_msgenc_rpc_reply(rpcreq, schema_data, &msglen);
            if (NULL != rpc_reply) { ret=NCONF_RPC_ERROR_TAG_NO_ERROR; }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to create new node\n", __func__);
        }
        XMLUTILS_NODE_RELEASE_ALL_INST();
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to get module schema [m=%s, v=%s, f=%s]\n",
               __func__, module != NULL ? module : "", version != NULL ? version : "",
               format!= NULL ? format : "");
        ret=NCONF_RPC_ERROR_TAG_INVALID_VALUE;
    }

    if (NCONF_RPC_ERROR_TAG_NO_ERROR==ret) {
        ret=nconf_msghdlr_send_get_schema_resp(&qmsg->args, rpc_reply, msglen, schemahdl);
        if (ret <= 0) {
            UB_LOG(UBL_ERROR, "%s:sending get-schema resp failed\n", __func__);
        }
        nconf_msgenc_release_buffer(rpc_reply);
        nconf_fclose(schemahdl);
    } else {
        nconf_rpcerror_args_t errargs;
        UB_LOG(UBL_ERROR, "%s:RPC MSG: processing failed\n", __func__);
        memset(&errargs, 0, sizeof(errargs));
        errargs.tag=(nconf_rpcerror_tag_t)ret;
        if (NCONF_RPC_ERROR_TAG_INVALID_VALUE==ret) {
            errargs.type=NCONF_RPC_ERROR_TYPE_APPLICATION;
        } else {
            errargs.type=NCONF_RPC_ERROR_TYPE_RPC;
        }
        errargs.severity=NCONF_RPC_ERROR_SEV_ERROR;
        ret=nconf_msghdlr_send_rpc_error(qmsg, &errargs, dechdl, didx);
    }
    nconf_msghdlr_free_field_values(req_prms, 3);
}

static void nconf_msghdlr_rpc_msg_proc_close_session(nconf_queue_msg_t *qmsg,
                                                     nconf_msgdec_t dechdl,
                                                     uint8_t didx)
{
    UB_LOG(UBL_DEBUG, "%s: RPC MSG: <close-session>\n", __func__);
    int ret=nconf_msghdlr_send_ok_rpc_reply(qmsg, dechdl, didx);
    if (ret > 0) {
        nconf_transport_disconnect_client(&qmsg->args);
    } else {
        UB_LOG(UBL_ERROR, "%s:RPC MSG: failed sending rpc reply\n", __func__);
    }
    XMLUTILS_NODE_RELEASE_ALL_INST();
}

static void nconf_msghdlr_rpc_msg_proc_kill_session(nconf_queue_msg_t *qmsg,
                                                    nconf_msgdec_t dechdl,
                                                    uint8_t didx)
{
    nconf_xpath_args_t *search_prm=NULL;
    nconf_nodeptr_t *retnode=NULL;
    uint32_t target_sess_id=0U;
    char *sessid_str=NULL;
    int ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;

    UB_LOG(UBL_DEBUG, "%s: RPC MSG: <kill-session>\n", __func__);
    search_prm=nconf_xmlutils_get_search_handle();
    if (NULL!=search_prm) {
        memset(search_prm, 0, sizeof(nconf_xpath_args_t));
        search_prm->nodes[0].name="session-id";
        search_prm->nodes[0].value=NULL;
        search_prm->nodesnum++;
        retnode=nconf_msgdec_search_node(dechdl, didx, search_prm);
        if (NULL!=retnode) {
            for(uint32_t i=0; i<search_prm->foundnodes.count; i++) {
                sessid_str=nconf_xmlutils_get_node_content(retnode[i]);
                break;
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:<session-id> not found\n", __func__);
            ret=NCONF_RPC_ERROR_TAG_MALFORMED_MESSAGE;
        }
        nconf_msgdec_search_node_release(search_prm);
        nconf_xmlutils_release_search_handle(search_prm);
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to get search handle\n", __func__);
    }

    if (NULL != sessid_str) {
        char *endptr=NULL;
        long int value=strtol(sessid_str, &endptr, 0);
        if (endptr != sessid_str) { target_sess_id=(uint32_t)value; }
        nconf_xmlutils_free((uint8_t*)sessid_str);
    }

    if (target_sess_id > 0) {
        uint32_t session_id=nconf_transport_get_client_sessid(&qmsg->args);
        if (session_id != target_sess_id) {
            UB_LOG(UBL_INFO, "%s:client#%u: killing sessid=%u\n", __func__,
                   session_id, target_sess_id);
            ret=nconf_transport_kill_session(target_sess_id);
        } else {
            UB_LOG(UBL_ERROR, "%s:killing own sessid=%u is not permitted\n",
                   __func__, target_sess_id);
            ret=NCONF_RPC_ERROR_TAG_INVALID_VALUE;
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:target sessid=%u value is out of range\n",
               __func__, target_sess_id);
        ret=NCONF_RPC_ERROR_TAG_INVALID_VALUE;
    }

    if (NCONF_RPC_ERROR_TAG_NO_ERROR==ret) {
        ret=nconf_msghdlr_send_ok_rpc_reply(qmsg, dechdl, didx);
    } else {
        nconf_rpcerror_args_t errargs;
        UB_LOG(UBL_ERROR, "%s:RPC MSG: processing failed\n", __func__);
        memset(&errargs, 0, sizeof(errargs));
        errargs.tag=(nconf_rpcerror_tag_t)ret;
        if (NCONF_RPC_ERROR_TAG_INVALID_VALUE==ret) {
            errargs.type=NCONF_RPC_ERROR_TYPE_APPLICATION;
        } else if (NCONF_RPC_ERROR_TAG_MALFORMED_MESSAGE==ret) {
            errargs.type=NCONF_RPC_ERROR_TYPE_RPC;
        } else {
            errargs.type=NCONF_RPC_ERROR_TYPE_PROTOCOL;
        }
        errargs.severity=NCONF_RPC_ERROR_SEV_ERROR;
        ret=nconf_msghdlr_send_rpc_error(qmsg, &errargs, dechdl, didx);
    }
    XMLUTILS_NODE_RELEASE_ALL_INST();
}

static nconf_nodeptr_t nconf_msghdlr_get_finfo(nconf_msgdec_t dechdl, uint8_t didx,
                                               nconf_filter_info_t **rfinfo)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    nconf_xpath_args_t *search_prm=NULL;
    nconf_nodeptr_t *filters=NULL;
    nconf_nodeptr_t rpcreq=NULL;
    search_prm=nconf_xmlutils_get_search_handle();
    if (NULL==search_prm) {
        UB_LOG(UBL_ERROR, "%s:failed to get search handle\n", __func__);
        goto end_oper;
    }
    memset(search_prm, 0, sizeof(nconf_xpath_args_t));
    rpcreq=nconf_msgdec_get_rootnode(dechdl, didx);
    if (NULL != rpcreq) {
        search_prm->nodes[0].name="filter";
        search_prm->nodes[0].value=NULL;
        search_prm->nodesnum++;
        filters=nconf_msgdec_search_node(dechdl, didx, search_prm);
        if (NULL != filters) {
            if(search_prm->foundnodes.count==1) {
                *rfinfo=nconf_msghdlr_get_rpcreq_finfo(filters[0]);
            } else {
                UB_LOG(UBL_ERROR, "%s:unexpected number of filter=%d\n",
                       __func__, search_prm->foundnodes.count);
            }
        } else {
            *rfinfo=nconf_msghdlr_get_default_finfo(priv->dbhdl);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:Failed to get request root node\n", __func__);
    }
end_oper:
    nconf_msgdec_search_node_release(search_prm);
    nconf_xmlutils_release_search_handle(search_prm);
    return rpcreq;
}

static nconf_filter_info_t* nconf_msghdlr_get_default_finfo(nconf_uccl_t dbhdl)
{
    nconf_filter_info_t *finfo=NULL;
    uint8_t yang_modkey_start=0;
    int modnum=0;

    finfo=(nconf_filter_info_t*)nconf_memalloc(sizeof(nconf_filter_info_t));
    if (NULL == finfo) {
        UB_LOG(UBL_ERROR, "%s:allocating filter info failed\n", __func__);
        goto end_oper;
    }
    memset(finfo, 0, sizeof(nconf_filter_info_t));
    modnum=nconf_ucclient_get_yang_module_num(dbhdl, &yang_modkey_start);
    if (modnum <= 0) {
        UB_LOG(UBL_ERROR, "%s:invalid yang module num=%d\n", __func__, modnum);
        goto end_oper;
    }
    finfo->subtrees=(nconf_treeinfo_t**)
        nconf_memalloc(modnum*sizeof(nconf_treeinfo_t*));
    if (NULL==finfo->subtrees) {
        UB_LOG(UBL_ERROR, "%s:allocating subtree array failed\n", __func__);
        goto end_oper;
    }

    for (uint8_t i=yang_modkey_start; i<(uint8_t)modnum; i++) {
        char modname[64]={0};
        memset(modname, 0, sizeof(modname));
        if (nconf_ucclient_get_yang_module_name(dbhdl, i, modname,
                                                sizeof(modname)-1)==0) {
            uint8_t si=finfo->count;
            finfo->subtrees[si]=nconf_msghdlr_gen_module_subtree(modname);
            if (NULL==finfo->subtrees[si]) {
                UB_LOG(UBL_ERROR, "%s:failed to generate module subtree\n",
                       __func__);
                break;
            }
            finfo->count++;
        }
    }
    nconf_msghdlr_display_filter_info(finfo);

end_oper:
    return finfo;
}

static nconf_filter_info_t* nconf_msghdlr_get_rpcreq_finfo(nconf_nodeptr_t filter)
{
    nconf_filter_info_t *finfo=(nconf_filter_info_t*)
            nconf_memalloc(sizeof(nconf_filter_info_t));
    if (NULL != finfo) {
        memset(finfo, 0, sizeof(nconf_filter_info_t));
        finfo->subtrees=(nconf_treeinfo_t **)
            nconf_memalloc(NCONF_INITIAL_SUBTREE_COUNT*sizeof(nconf_treeinfo_t*));
        if (NULL!=finfo->subtrees) {
            int ret=nconf_msghdlr_evaluate_filter(finfo, filter);
            if (0==ret) {
                nconf_msghdlr_display_filter_info(finfo);
            } else {
                UB_LOG(UBL_ERROR, "%s:evaluate filter failed\n", __func__);
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to allocate initial subtrees\n", __func__);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to allocate filter info\n", __func__);
    }
    return finfo;
}

static nconf_treeinfo_t *nconf_msghdlr_gen_module_subtree(char *modname)
{
    nconf_treeinfo_t *subtree=(nconf_treeinfo_t*)
        nconf_memalloc(sizeof(nconf_treeinfo_t));
    if (NULL!=subtree) {
        memset(subtree, 0, sizeof(nconf_treeinfo_t));
        subtree->nodepaths=(char**)nconf_memalloc(sizeof(char*));
        if (NULL != subtree->nodepaths) {
            uint32_t pathsize=strlen(modname)+2; //+2 for opening and closing '/'
            memset(subtree->nodepaths, 0, sizeof(char*));
            subtree->nodepaths[0]=(char*)nconf_memalloc(pathsize+1);
            if (NULL!=subtree->nodepaths[0]) {
                memset(subtree->nodepaths[0], 0, pathsize+1);
                sprintf(subtree->nodepaths[0], "/%s/", modname);
                subtree->count++;
            } else {
                UB_LOG(UBL_ERROR, "%s:failed to allocate nodepath string\n",
                       __func__);
                nconf_memfree(subtree->nodepaths);
                nconf_memfree(subtree);
                subtree=NULL;
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to allocate nodepath array\n", __func__);
            nconf_memfree(subtree);
            subtree=NULL;
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to allocate subtree info\n", __func__);
    }
    return subtree;
}

static int nconf_msghdlr_evaluate_filter(nconf_filter_info_t *finfo, nconf_nodeptr_t filter)
{
    int ret=-1;
    char *filtertype=nconf_xmlutils_get_ns_node_prop(filter, "type", NCONF_BASE_XML_NAMESPACE);
    if (NULL != filtertype) {
        if (strcmp(filtertype, "subtree")==0) {
            finfo->ftype=NCONF_FILTER_TYPE_SUBTREE;
        } else {
            finfo->ftype=NCONF_FILTER_TYPE_MAX;
            UB_LOG(UBL_ERROR, "%s:unsupported filter type: %s\n",
                   __func__, filtertype);
        }
        nconf_xmlutils_free((uint8_t*)filtertype);
    } else {
        /* rfc6241#section-7.7: default netconfig filter is subtree */
        finfo->ftype=NCONF_FILTER_TYPE_SUBTREE;
    }
    if (NCONF_FILTER_TYPE_SUBTREE==finfo->ftype) {
        nconf_nodeptr_t children=XMLUTILS_NODE_GET_CHILD(filter);
        if (NULL != children) {
            ret=nconf_msghdlr_get_subtree_filters(finfo, children);
        } else {
            /* when empty filter is received return success to process it
             * normally (i.e. server will return empty data tree) */
            UB_LOG(UBL_WARN, "%s:receive empty filter\n", __func__);
            ret=0;
        }
    }
    return ret;
}

static int nconf_msghdlr_get_subtree_filters(nconf_filter_info_t *finfo, nconf_nodeptr_t module)
{
    uint8_t max_index=NCONF_INITIAL_SUBTREE_COUNT;
    int ret=-1;
    while (NULL != module) {
        nconf_nodeptr_t modcopy=nconf_xmlutils_copy_node(module, 1);
        uint8_t si=finfo->count;
        if (si >= max_index) {
            uint8_t new_max_index=max_index*2;
            nconf_treeinfo_t **tmp=(nconf_treeinfo_t**)
                nconf_memrealloc(finfo->subtrees, new_max_index*sizeof(nconf_treeinfo_t *));
            if (NULL==tmp) {
                UB_LOG(UBL_ERROR, "%s:failed to reallocate subtree info\n",
                       __func__);
                ret=-1;
                break;
            }
            finfo->subtrees=tmp;
            max_index=new_max_index;
        }
        finfo->subtrees[si]=nconf_xmlutils_generate_tree_info(modcopy,
                    nconf_msghdlr_is_value_key, NCONF_TREEINFO_TYPE_FILTER);
        if (NULL==finfo->subtrees[si]) {
            UB_LOG(UBL_ERROR, "%s:failed to generate subtree filter\n", __func__);
            ret=-1;
            break;
        }
        finfo->count++;
        ret=0;

        /* get the next sibling */
        nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(module);
        nconf_xmlutils_release_node(modcopy);
        nconf_xmlutils_release_node(module);
        module=sibling;
    }
    return ret;
}

static int nconf_msghdlr_send_db_data(nconf_trans_args_t *args, nconf_filter_info_t *finfo,
                                      nconf_ucclcfg_type_t type)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    nconf_uccl_txfunc_t txfunc={
        .txnode=nconf_msghdlr_send_nodeptr,
        .txhdr=nconf_msghdlr_send_nodeptr_header,
        .txftr=nconf_msghdlr_send_nodeptr_footer,
        .uargs=(void*)args
    };
    int ret=nconf_ucclient_reg_txfunc(priv->dbhdl, &txfunc);
    if (NCONF_RPC_ERROR_TAG_NO_ERROR != ret) { goto end_oper; }

    for (uint8_t sidx=0; sidx<finfo->count; sidx++) {
        nconf_treeinfo_t *subtree=finfo->subtrees[sidx];
        struct ub_list cfglist;
        ub_list_init(&cfglist);
        ret=nconf_ucclient_dbtree_init(priv->dbhdl);
        if (NCONF_RPC_ERROR_TAG_NO_ERROR != ret) {
            UB_LOG(UBL_ERROR, "%s:dbtree init failed\n", __func__);
            break;
        }
        for (uint8_t i=0; i<subtree->count; i++) {
            ret=nconf_ucclient_get_config_from_db(priv->dbhdl, subtree->nodepaths[i],
                                                  &cfglist, type);
            if (NCONF_RPC_ERROR_TAG_NO_ERROR!=ret) {
                UB_LOG(UBL_ERROR, "%s:retrieving dbdata failed [nodepath=%s]\n",
                       __func__, subtree->nodepaths[i]);
                ub_list_clear(&cfglist,nconf_ucclient_config_info_release,NULL);
                nconf_ucclient_dbtree_release(priv->dbhdl);
                break;
            }
        }
        if (ub_list_count(&cfglist) > 0) {
            ub_list_sort(&cfglist, nconf_msghdlr_cfgnode_cmp, NULL);
            ret=nconf_ucclient_encode_config(priv->dbhdl, &cfglist);
            if (0 != ret) {
                UB_LOG(UBL_ERROR, "%s:endcoding dbtree failed\n", __func__);
            }
        }
        ub_list_clear(&cfglist, nconf_ucclient_config_info_release, NULL);
        nconf_ucclient_dbtree_release(priv->dbhdl);
    }

end_oper:
    nconf_ucclient_unreg_txfunc(priv->dbhdl);
    return ret;
}

static bool nconf_msghdlr_cfgnode_cmp(struct ub_list_node *node1, struct ub_list_node *node2,
                                      void *uargs)
{
    bool ret=false;
    nconf_cfginfo_t *cfginfo1=(nconf_cfginfo_t*)ub_list_nodedata(node1);
    nconf_cfginfo_t *cfginfo2=(nconf_cfginfo_t*)ub_list_nodedata(node2);
    if (NULL != cfginfo1 && NULL != cfginfo2) {
        if (strcmp(cfginfo1->path, cfginfo2->path)<=0){ret=true;}
    }
    NCONF_UNUSED(uargs);
    return ret;
}

static int nconf_msghdlr_send_nodeptr(nconf_nodeptr_t nodeptr, void *uargs)
{
    int ret=-1;
    nconf_trans_args_t *args=(nconf_trans_args_t *)uargs;
    uint8_t *xmlstr=NULL;
    uint32_t xmllen=0;
    nconf_xmlutils_get_nodeptr_str(nodeptr, (char**)&xmlstr, &xmllen);
    if (NULL != xmlstr && 0 != xmllen) {
        ret=nconf_msghdlr_send_chunked_frame(args, xmlstr, xmllen, false);
        if (ret <= 0) {
            UB_LOG(UBL_ERROR, "%s:sending nodeptr data failed\n", __func__);
        }
        nconf_xmlutils_free_nptr_encode_buf(xmlstr);
    }
    return ret;
}

static int nconf_msghdlr_send_nodeptr_header(nconf_nodeptr_t nodeptr, void *uargs)
{
    int ret=-1;
    nconf_trans_args_t *args=(nconf_trans_args_t *)uargs;
    uint8_t *xmlstr=(uint8_t*)nconf_xmlutils_get_node_header(nodeptr);
    if (NULL != xmlstr) {
        uint32_t xmllen=strlen((char*)xmlstr);
        ret=nconf_msghdlr_send_chunked_frame(args, xmlstr, xmllen, false);
        if (ret <= 0) {
            UB_LOG(UBL_ERROR, "%s:sending node header failed\n", __func__);
        }
        nconf_xmlutils_free(xmlstr);
    }
    return ret;
}

static int nconf_msghdlr_send_nodeptr_footer(char *nodename, void *uargs)
{
    int ret=-1;
    nconf_trans_args_t *args=(nconf_trans_args_t *)uargs;
    uint32_t xmllen=strlen(nodename)+3; //+3 for </>
    uint8_t *xmlstr=(uint8_t*)nconf_memalloc(xmllen+1); //+1 for null terminator
    if (NULL != xmlstr && 0 != xmllen) {
        memset(xmlstr, 0, xmllen+1);
        sprintf((char*)xmlstr, "</%s>", nodename);
        ret=nconf_msghdlr_send_chunked_frame(args, xmlstr, xmllen, false);
        if (ret <= 0) {
            UB_LOG(UBL_ERROR, "%s:sending node footer failed\n", __func__);
        }
        nconf_memfree(xmlstr);
    }
    return ret;
}

static nconf_docptr_t nconf_msghdlr_generate_dummy_dbtree(void)
{
    nconf_docptr_t dbtree=nconf_xmlutils_new_doc_w_root("1.0", "data");
    if (NULL!=dbtree) {
        nconf_nodeptr_t rootnode=nconf_xmlutils_get_root_node(dbtree);
        if (NULL != rootnode) {
            nconf_xmlutils_set_node_content(rootnode, "dummy");
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to get dbtree rootnode\n", __func__);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:creating new doc failed\n", __func__);
    }
    return dbtree;
}

static bool nconf_msghdlr_is_value_key(const char *nodepath, uint8_t *kvidx)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    char *parent_npath=NULL, *tmp=NULL;
    bool bret=false;
    uint8_t vknum=0;

    parent_npath=nconf_strdup((char*)nodepath);
    if (NULL==parent_npath) {
        UB_LOG(UBL_ERROR, "%s: nconf_strdup() failed\n", __func__);
        goto end_oper;
    }
    tmp=strrchr(parent_npath, '/');
    if (tmp != NULL) { *tmp='\0'; } /* parent's nodepath */

    vknum=nconf_ucclient_get_nodepath_vknum(priv->dbhdl, (char*)parent_npath);
    for (uint8_t i=0; i<vknum; i++) {
        char *vkstr=nconf_ucclient_get_nodepath_vkstr(priv->dbhdl, (char*)parent_npath, i);
        if (NULL != vkstr) {
            if (strcmp(nodepath, vkstr)==0) {
                nconf_ucclient_vkstr_release(vkstr);
                if (NULL!=kvidx) { *kvidx=i; }
                bret=true;
                break;
            }
            nconf_ucclient_vkstr_release(vkstr);
        } else {
            UB_LOG(UBL_ERROR, "%s:getting vkstr failed [npath=%s]\n", __func__,
                   parent_npath);
        }
    }
    nconf_memfree(parent_npath);

end_oper:
    return bret;
}

static void nconf_msghdlr_release_filter_resource(nconf_uccl_t dbhdl,
                                                  nconf_filter_info_t *finfo)
{
    if (NULL != finfo) {
        if (NULL != finfo->subtrees) {
            for (uint8_t i=0; i<finfo->count; i++) {
                nconf_xmlutils_release_tree_info(finfo->subtrees[i]);
            }
            nconf_memfree(finfo->subtrees);
        }
        nconf_memfree(finfo);
        NCONF_UNUSED(dbhdl);
    }
}

static int nconf_msghdlr_exec_config_update(nconf_nodeptr_t config)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    int ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;

    nconf_update_info_t *uinfo=nconf_msghdlr_get_update_list(config);
    if (NULL==uinfo) {
        UB_LOG(UBL_ERROR, "%s:failed to get filter info\n", __func__);
        goto end_oper;
    }

    /* validate all before executing db update */
    for (uint8_t i=0; i<uinfo->count; i++) {
        ret=nconf_msghdlr_validate_update_list(uinfo->configs[i]);
        if (NCONF_RPC_ERROR_TAG_NO_ERROR!=ret) {
            UB_LOG(UBL_ERROR, "%s:config validation failed i=%u\n",__func__,i);
            goto end_oper;
        }
    }

    nconf_msghdlr_display_update_info(uinfo);
    for (uint8_t i=0; i<uinfo->count; i++) {
        ret=nconf_ucclient_exec_config_update(priv->dbhdl, uinfo->configs[i]);
        if (0!=ret) {
            UB_LOG(UBL_ERROR, "%s:config update exec failed\n", __func__);
            ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;
        } else {
            ret=NCONF_RPC_ERROR_TAG_NO_ERROR;
        }
    }

end_oper:
    nconf_msghdlr_release_update_resource(uinfo);
    return ret;
}

static nconf_update_info_t* nconf_msghdlr_get_update_list(nconf_nodeptr_t config)
{
    uint8_t max_index=NCONF_INITIAL_SUBTREE_COUNT;
    nconf_nodeptr_t module=NULL;

    nconf_update_info_t *uinfo=(nconf_update_info_t*)
            nconf_memalloc(sizeof(nconf_update_info_t));
    if (NULL==uinfo) {
        UB_LOG(UBL_ERROR, "%s:failed to allocate filter info\n", __func__);
        goto end_oper;
    }
    memset(uinfo, 0, sizeof(nconf_update_info_t));
    uinfo->configs=(nconf_treeinfo_t**)
        nconf_memalloc(max_index*sizeof(nconf_treeinfo_t*));
    if (NULL==uinfo->configs) {
        UB_LOG(UBL_ERROR, "%s:failed to allocate initial configs\n", __func__);
        nconf_memfree(uinfo);
        uinfo=NULL;
        goto end_oper;
    }
    module=XMLUTILS_NODE_GET_CHILD(config);
    while (NULL != module) {
        nconf_nodeptr_t modcopy=nconf_xmlutils_copy_node(module, 1);
        uint8_t si=uinfo->count;
        if (si >= max_index) {
            uint8_t new_max_index=max_index*2;
            nconf_treeinfo_t **tmp=(nconf_treeinfo_t**)
                nconf_memrealloc(uinfo->configs, new_max_index*sizeof(nconf_treeinfo_t *));
            if (NULL==tmp) {
                UB_LOG(UBL_ERROR, "%s:failed to reallocate subtree info\n",
                       __func__);
                break;
            }
            uinfo->configs=tmp;
            max_index=new_max_index;
        }
        uinfo->configs[si]=nconf_xmlutils_generate_tree_info(modcopy,
                    nconf_msghdlr_is_value_key, NCONF_TREEINFO_TYPE_CONFIG);
        if (NULL==uinfo->configs[si]) {
            UB_LOG(UBL_ERROR, "%s:failed to generate subtree filter\n", __func__);
            goto end_oper;
        }
        uinfo->count++;

        /* get the next sibling */
        nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(module);
        nconf_xmlutils_release_node(modcopy);
        nconf_xmlutils_release_node(module);
        module=sibling;
    }
end_oper:
    return uinfo;
}

static int nconf_msghdlr_validate_update_list(nconf_treeinfo_t *configs)
{
    nconf_msghdlr_priv_t *priv=&g_msghdlr_priv;
    int ret=NCONF_RPC_ERROR_TAG_NO_ERROR, readret;
    for (uint8_t i=0; i<configs->count; i++) {
        char *operation=NULL;
        char *endpos=strchr(configs->nodepaths[i], '&');
        if (endpos != NULL) { endpos[0]='\0'; operation=&endpos[1]; }
        if (operation==NULL) { continue; }
        if ((strcmp(operation,"delete")==0)||(strcmp(operation,"create")==0)) {
            uint32_t vsize=0;
            void *value=NULL;
            readret=nconf_ucclient_exec_config_read(priv->dbhdl,
                    configs->nodepaths[i], &value, &vsize);
            if (readret < 0) {
                if (strcmp(operation, "delete")==0) {
                    UB_LOG(UBL_ERROR, "%s:cannot delete non-existing data: <%s>\n",
                           __func__, configs->nodepaths[i]);
                    ret=NCONF_RPC_ERROR_TAG_DATA_MISSING;
                }
            } else {
                if (strcmp(operation, "create")==0) {
                    UB_LOG(UBL_ERROR, "%s:data already exist: <%s>\n",
                           __func__, configs->nodepaths[i]);
                    ret=NCONF_RPC_ERROR_TAG_DATA_EXISTS;
                } else {
                    /* operation is delete, add delete mark for uniconf */
                    endpos[0]='-';
                    endpos[1]='\0';
                }
            }
            nconf_ucclient_config_read_release(priv->dbhdl, value);
        } else if (strcmp(operation,"remove")==0){
            /* operation is delete, add delete mark for uniconf */
            endpos[0]='-';
            endpos[1]='\0';
        }
        if (NCONF_RPC_ERROR_TAG_NO_ERROR != ret) { break; }
    }
    return ret;
}

static void nconf_msghdlr_release_update_resource(nconf_update_info_t *uinfo)
{
    if (NULL != uinfo) {
        if (NULL != uinfo->configs) {
            for (uint8_t i=0; i<uinfo->count; i++) {
                nconf_xmlutils_release_tree_info(uinfo->configs[i]);
            }
            nconf_memfree(uinfo->configs);
        }
        nconf_memfree(uinfo);
    }
}

static void nconf_msghdlr_gen_missing_element_err_info(nconf_rpcerror_args_t *errargs,
                                                       char *missing_element)
{
    nconf_nodeptr_t tmpnode=NULL;
    errargs->info = nconf_xmlutils_new_node(NULL, "error-info", NULL);
    if (NULL != errargs->info) {
        tmpnode=nconf_xmlutils_new_node(errargs->info, missing_element, NULL);
        nconf_xmlutils_release_node(tmpnode);
    }
}

static int nconf_msghdlr_send_ok_rpc_reply(nconf_queue_msg_t *qmsg,
                                           nconf_msgdec_t dechdl,
                                           uint8_t didx)
{
    nconf_nodeptr_t rpcreq=NULL;
    uint8_t *rpc_reply=NULL;
    uint32_t msglen=0U;
    int ret=-1;
    rpcreq=nconf_msgdec_get_rootnode(dechdl, didx);
    if (NULL != rpcreq) {
        rpc_reply=nconf_msgenc_rpc_reply_ok(rpcreq, &msglen);
        if (NULL != rpc_reply) {
            ret=nconf_msghdlr_send_chunked_frame(&qmsg->args, rpc_reply, msglen, true);
            if (ret <= 0) {
                UB_LOG(UBL_ERROR, "%s:sending rpc reply failed\n", __func__);
            }
            nconf_msgenc_release_buffer(rpc_reply);
        } else {
            UB_LOG(UBL_ERROR, "%s:RPC MSG: processing failed\n", __func__);
        }
    }
    return ret;
}

static int nconf_msghdlr_send_rpc_error(nconf_queue_msg_t *qmsg,
                                        nconf_rpcerror_args_t *errargs,
                                        nconf_msgdec_t dechdl,
                                        uint8_t didx)
{
    nconf_nodeptr_t rpcreq=NULL;
    uint8_t *rpc_reply=NULL;
    uint32_t msglen=0U;
    int ret=-1;
    rpcreq=nconf_msgdec_get_rootnode(dechdl, didx);
    if (NULL != rpcreq) {
        rpc_reply=nconf_msgenc_rpc_reply_error(rpcreq, errargs, &msglen);
        if (NULL != rpc_reply) {
            ret=nconf_msghdlr_send_chunked_frame(&qmsg->args, rpc_reply, msglen, true);
            if (ret <= 0) {
                UB_LOG(UBL_ERROR, "%s:sending rpc reply failed\n", __func__);
            }
            nconf_msgenc_release_buffer(rpc_reply);
        } else {
            UB_LOG(UBL_ERROR, "%s:RPC MSG: processing failed\n", __func__);
        }
    }
    return ret;
}

static int nconf_msghdlr_send_eom_frame(nconf_trans_args_t *args,
                                        uint8_t *msg, uint32_t msglen,
                                        bool iscomplete)
{
    int ret=-1;
    /* send the <hello> message securely */
    ret=nconf_transport_send_msg(args, msg, msglen);
    if (ret <= 0) {
        UB_LOG(UBL_ERROR, "%s:sending netconf message failed\n", __func__);
        goto end_oper;
    }
    if (!iscomplete) { goto end_oper; }
    ret=nconf_transport_send_msg(args, (uint8_t*)NCONF_END_OF_MESSAGE_MARKER,
            strlen(NCONF_END_OF_MESSAGE_MARKER));
    if (ret <= 0) {
        UB_LOG(UBL_ERROR, "%s:sending end-of-message marker failed\n", __func__);
        goto end_oper;
    }
end_oper:
    return ret;
}

static int nconf_msghdlr_send_chunked_frame(nconf_trans_args_t *args,
                                            uint8_t *msg, uint32_t msglen,
                                            bool iscomplete)
{
    char chunkhdr[NCONF_MAX_CHUNK_HDR_LEN+1]={0};
    uint32_t sendlen=0U, remaininglen=msglen;
    int ret=-1;

    while (remaininglen > 0) {
        uint32_t chunkSize=remaininglen > NCONF_MAX_BUF_SIZE ?
            NCONF_MAX_BUF_SIZE : remaininglen;

        memset(chunkhdr, 0, sizeof(chunkhdr));
        snprintf(chunkhdr, NCONF_MAX_CHUNK_HDR_LEN, "%s%u\n",
                 NCONF_CHUNK_SIZE_MSG_PREFIX, chunkSize);

        ret=nconf_transport_send_msg(args, (uint8_t*)chunkhdr, strlen(chunkhdr));
        if (ret <= 0) {
            UB_LOG(UBL_ERROR, "%s:sending chunk frame prefix failed\n", __func__);
            break;
        }

        ret=nconf_transport_send_msg(args, msg+sendlen, chunkSize);
        if (ret <= 0) {
            UB_LOG(UBL_ERROR, "%s:sending netconf message failed\n", __func__);
            break;
        }

        sendlen += chunkSize;
        remaininglen -= chunkSize;
    }
    if (ret <= 0 || !iscomplete) { goto end_oper; }
    /* send end of chunks marker after all chunks are sent */
    ret=nconf_transport_send_msg(args, (uint8_t*)NCONF_END_OF_CHUNKS_MARKER,
            strlen(NCONF_END_OF_CHUNKS_MARKER));
    if (ret <= 0) {
        UB_LOG(UBL_ERROR, "%s:sending end-of-chunk marker failed\n", __func__);
    }
end_oper:
    return ret;
}

static int nconf_msghdlr_send_get_schema_resp(nconf_trans_args_t *args,  uint8_t *msg,
                                              uint32_t msglen, nconf_fshdl_t hdl)
{
    uint8_t *endtag=NULL, *temp=NULL;
    uint32_t partlen=0;
    int ret=-1;

    temp=(uint8_t*)strstr((const char *)msg, "dummy</data>");
    if (NULL != temp) {
        temp[0]='\0';
        partlen=strlen((char*)msg);
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid msg [%u] %s\n", __func__, msglen, msg);
        goto end_oper;
    }

    ret=nconf_msghdlr_send_chunked_frame(args, msg, partlen, false);
    if (ret > 0) {
        size_t readlen;
        char *sndbuf=(char*)nconf_memalloc(NCONF_MAX_BUF_SIZE+1);
        if(ub_assert_fatal((NULL!=sndbuf), __func__, "alloc error")){ return -1; }
        memset(sndbuf, 0, NCONF_MAX_BUF_SIZE+1);

        while ((readlen=nconf_fread(sndbuf, 1, NCONF_MAX_BUF_SIZE, hdl))>0) {
            ret=nconf_msghdlr_send_chunked_frame(args, (uint8_t*)sndbuf,
                    readlen, false);
            if (ret <= 0) {
                UB_LOG(UBL_ERROR, "%s:sending file chunked failed\n", __func__);
                break;
            }
            memset(sndbuf, 0, NCONF_MAX_BUF_SIZE);
        }
        nconf_memfree(sndbuf);

        if (ret > 0) {
            temp[0]='d';
            endtag=(uint8_t*)strstr((const char *)temp, "</data>");
            if(NULL != endtag) {
                partlen=strlen((char*)endtag);
                ret=nconf_msghdlr_send_chunked_frame(args, endtag, partlen, true);
                if (ret <= 0) {
                    UB_LOG(UBL_ERROR, "%s:sending get-schema footer failed\n", __func__);
                }
            } else {
                UB_LOG(UBL_WARN, "%s:<data> end tag not found\n", __func__);
            }
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:sending get-schema hdr failed\n", __func__);
    }

end_oper:
    return ret;
}

static void nconf_msghdlr_display_filter_info(nconf_filter_info_t *finfo)
{
#ifdef NCONF_DEBUG_LOG_ENABLED
    UB_LOG(UBL_DEBUG, "# Filter Info\n");
    UB_LOG(UBL_DEBUG, "## type: %s\n",
           (finfo->ftype==NCONF_FILTER_TYPE_SUBTREE) ? "subtree" :
           (finfo->ftype==NCONF_FILTER_TYPE_XPATH) ? "xpath" : "invalid");
    UB_LOG(UBL_DEBUG, "## subtrees: [count: %u]\n", finfo->count);
    for (uint8_t i=0; i<finfo->count; i++) {
        UB_LOG(UBL_DEBUG, "### subtree[%u]:\n", i);
        UB_LOG(UBL_DEBUG, "### ------------\n");
        UB_LOG(UBL_DEBUG, "#### nodepaths: [count: %u]\n", finfo->subtrees[i]->count);
        for (uint8_t j=0; j<finfo->subtrees[i]->count; j++) {
            UB_LOG(UBL_DEBUG, "##### nodepath: %s\n", finfo->subtrees[i]->nodepaths[j]);
        }
    }
#else
    NCONF_UNUSED(finfo);
#endif
}

static void nconf_msghdlr_display_update_info(nconf_update_info_t *uinfo)
{
#ifdef NCONF_DEBUG_LOG_ENABLED
    UB_LOG(UBL_DEBUG, "# Update Info\n");
    UB_LOG(UBL_DEBUG, "## Configs: [count: %u]\n", uinfo->count);
    for (uint8_t i=0; i<uinfo->count; i++) {
        UB_LOG(UBL_DEBUG, "### Config[%u]:\n", i);
        UB_LOG(UBL_DEBUG, "### ------------\n");
        UB_LOG(UBL_DEBUG, "#### nodepaths: [count: %u]\n", uinfo->configs[i]->count);
        for (uint8_t j=0; j<uinfo->configs[i]->count; j++) {
            UB_LOG(UBL_DEBUG, "##### nodepath  : %s\n",
                   uinfo->configs[i]->nodepaths[j]);
            if (NULL != uinfo->configs[i]->nodevalues &&
                NULL != uinfo->configs[i]->nodevalues[j]) {
                UB_LOG(UBL_DEBUG, "      nodevalue : %s\n",
                       uinfo->configs[i]->nodevalues[j]);
            } else {
                UB_LOG(UBL_DEBUG, "      nodevalue : <null>\n");
            }
        }
    }
#else
    NCONF_UNUSED(uinfo);
#endif
}

