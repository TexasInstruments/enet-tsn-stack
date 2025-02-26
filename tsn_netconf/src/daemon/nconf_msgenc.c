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
 * @file        nconf_msgdec.c
 *
 * @brief       Netconf Message Decoder Implementation
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <string.h>
#include "nconf_msgenc.h"
#include "nconf_xmlutils.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

/* None */

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

/* None */

/*=============================================================================
 * Global Variables
 *============================================================================*/

static const char *g_rpcerr_type[] = {
    "transport",
    "rpc",
    "protocol",
    "application",
    NULL
};

static const char *g_rpcerr_tag[] = {
    "",
    "in-use",
    "invalid-value",
    "too-big",
    "missing-attribute",
    "bad-attribute",
    "unknown-attribute",
    "missing-element",
    "bad-element",
    "unkown-element",
    "unkown-namespace",
    "access-denied",
    "lock-denied",
    "resource-denied",
    "rollback-failed",
    "data-exists",
    "data-missing",
    "operation-not-supported",
    "operation-failed",
    "partial-failed",
    "malformed-message",
    NULL
};

static const char *g_rpcerr_sev[] = {
    "error",
    "warning",
    NULL
};

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static uint8_t *nconf_msgenc_get_xmlmsg(nconf_docptr_t msgdoc, uint32_t *rlen);
static void nconf_msgenc_copy_rpcreq_attr(nconf_nodeptr_t rpcreq,
                                          nconf_nodeptr_t rpcreply);
static void nconf_msgenc_set_capabilities(nconf_nodeptr_t capabilities,
                                          char *capliststr, uint32_t caplistlen);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

uint8_t *nconf_msgenc_server_hello(char *capstr, uint32_t caplen, uint32_t sessid, uint32_t *rlen)
{
    nconf_docptr_t hello_message=nconf_xmlutils_new_doc_w_root(NULL, "hello");
    nconf_nodeptr_t root_node=NULL;
    nconf_nodeptr_t capabilities=NULL;
    nconf_nodeptr_t sessionid=NULL;
    char sidstr[11] = {0};

    /* FIXME: Server Hello encoding comsumes a lot of memory */
    root_node=nconf_xmlutils_get_root_node(hello_message);
    if (NULL != root_node) {
        nconf_xmlutils_new_node_namespace(root_node, NCONF_BASE_XML_NAMESPACE, NULL);
        capabilities=nconf_xmlutils_new_node(root_node, "capabilities", NULL);
        if (NULL != capabilities) {
            nconf_msgenc_set_capabilities(capabilities, capstr, caplen);
            nconf_xmlutils_release_node(capabilities);
        }
        memset(sidstr, 0, sizeof(sidstr));
        snprintf(sidstr, sizeof(sidstr)-1, "%u", sessid);
        sessionid=nconf_xmlutils_new_node(root_node, "session-id", sidstr);
        nconf_xmlutils_release_node(sessionid);
        nconf_xmlutils_release_node(root_node);
    }
    return nconf_msgenc_get_xmlmsg(hello_message, rlen);
}

uint8_t *nconf_msgenc_rpc_reply(nconf_nodeptr_t rpcreq,
                                nconf_nodeptr_t content, uint32_t *rlen)
{
    nconf_docptr_t rpc_reply=nconf_xmlutils_new_doc_w_root(NULL, "rpc-reply");
    nconf_nodeptr_t root_node=nconf_xmlutils_get_root_node(rpc_reply);
    if (NULL != root_node) {
        nconf_nodeptr_t tmpnode=NULL;
        nconf_msgenc_copy_rpcreq_attr(rpcreq, root_node);
        tmpnode=nconf_xmlutils_add_child_node(root_node, content);
        nconf_xmlutils_release_node(tmpnode);
        nconf_xmlutils_release_node(root_node);
    }
    return nconf_msgenc_get_xmlmsg(rpc_reply, rlen);
}

uint8_t *nconf_msgenc_rpc_reply_ok(nconf_nodeptr_t rpcreq, uint32_t *rlen)
{
    nconf_nodeptr_t ok_response=
        nconf_xmlutils_new_node(NULL, "ok", NULL);
    uint8_t *encmsg=nconf_msgenc_rpc_reply(rpcreq, ok_response, rlen);
    if (NULL==encmsg) {
        UB_LOG(UBL_ERROR, "%s:rpc_reply encoding failed\n", __func__);
    }
    nconf_xmlutils_release_node(ok_response);
    return encmsg;
}

uint8_t *nconf_msgenc_rpc_reply_error(nconf_nodeptr_t rpcreq,
                                      nconf_rpcerror_args_t *args, uint32_t *rlen)
{
    nconf_nodeptr_t rpc_error=NULL;
    nconf_nodeptr_t tmpnode=NULL;
    uint8_t *encmsg=NULL;

    rpc_error=nconf_xmlutils_new_node(NULL, "rpc-error", NULL);
    if (NULL==rpc_error) {
        UB_LOG(UBL_ERROR, "%s:failed to create new node\n", __func__);
        return NULL;
    }

    //error-type
    if (args->type < NCONF_RPC_ERROR_TYPE_MAX && g_rpcerr_type[args->type] != NULL) {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-type",
                                        (char*)g_rpcerr_type[args->type]);
    } else {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-type", "application");
    }
    nconf_xmlutils_release_node(tmpnode);

    //error-tag
    if (args->tag < NCONF_RPC_ERROR_TAG_MAX && g_rpcerr_tag[args->tag] != NULL) {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-tag",
                                        (char*)g_rpcerr_tag[args->tag]);
    } else {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-tag", "operation-failed");
    }
    nconf_xmlutils_release_node(tmpnode);

    //error-severity
    if (args->severity < NCONF_RPC_ERROR_SEV_MAX && g_rpcerr_sev[args->severity] != NULL) {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-severity",
                                        (char*)g_rpcerr_sev[args->severity]);
    } else {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-severity", "error");
    }
    nconf_xmlutils_release_node(tmpnode);

    //error-app-tag (Optional)
    if (NULL != args->app_tag) {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-app-tag", args->app_tag);
        nconf_xmlutils_release_node(tmpnode);
    }

    //error-path (Optional)
    if (NULL != args->xpath) {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-path", args->xpath);
        nconf_xmlutils_release_node(tmpnode);
    }

    //error-message (Optional)
    if (NULL != args->message) {
        tmpnode=nconf_xmlutils_new_node(rpc_error, "error-message", args->message);
        nconf_xmlutils_release_node(tmpnode);
    }

    //error-message (Optional)
    if (args->info) {
        tmpnode=nconf_xmlutils_add_child_node(rpc_error, args->info);
        nconf_xmlutils_release_node(tmpnode);
    }

    encmsg=nconf_msgenc_rpc_reply(rpcreq, rpc_error, rlen);
    if (NULL==encmsg) {
        UB_LOG(UBL_ERROR, "%s:rpc_reply encoding failed\n", __func__);
    }
    nconf_xmlutils_release_node(rpc_error);
    return encmsg;
}

void nconf_msgenc_release_buffer(uint8_t *buf)
{
    nconf_xmlutils_free_encode_buf(buf);
}

static uint8_t *nconf_msgenc_get_xmlmsg(nconf_docptr_t msgdoc, uint32_t *rlen)
{
    uint32_t msglen;
    char *xmlmsg=NULL;
    if (NULL != msgdoc) {
        nconf_xmlutils_get_docptr_str(msgdoc, &xmlmsg, &msglen);
        if (xmlmsg[msglen-1]=='\n') {
            xmlmsg[msglen-1]='\0';
            msglen--;
        }
        *rlen=msglen;
        nconf_xmlutils_free_doc(msgdoc);
    }
    return (uint8_t*)xmlmsg;
}

static void nconf_msgenc_copy_rpcreq_attr(nconf_nodeptr_t rpcreq,
                                          nconf_nodeptr_t rpcreply)
{
    nconf_xmlutils_copy_node_prop(rpcreply, rpcreq);
    nconf_xmlutils_copy_node_nsdefs(rpcreply, rpcreq);
}

static void nconf_msgenc_set_capabilities(nconf_nodeptr_t capabilities,
                                          char *capliststr, uint32_t caplistlen)
{
    nconf_nodeptr_t capability=NULL;
    char *caplist=capliststr;
    uint32_t remaining_len=caplistlen;
    uint32_t curidx=0;

    while(remaining_len > 0) {
        uint32_t curlen=strlen(&caplist[curidx])+1;
        char *vstr=NULL;
        int isUpdated=0;

        vstr=nconf_xmlutils_sanitize_value_string(&caplist[curidx], &isUpdated);
        capability=nconf_xmlutils_new_node(capabilities, "capability", vstr);
        nconf_xmlutils_release_node(capability);

        if (isUpdated) { nconf_xmlutils_release_sanitize_value(vstr); }
        if (curlen > remaining_len) {
            UB_LOG(UBL_WARN, "%s:curlen greater than remaining length\n", __func__);
            break;
        }

        /* update remaining length and current index */
        remaining_len-=curlen;
        curidx+=curlen;
    }
}

