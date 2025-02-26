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
#include "nconf_msgdec.h"
#include "nconf_xmlutils.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_MAX_MESSAGE_DECODE    (3U)

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct _nconfmsg {
    nconf_docptr_t doc;
    nconf_nodeptr_t rootnode;
    nconf_nodeptr_t rpcnode;
    nconf_msg_type_t msgtype;
    nconf_rpc_msg_type_t rpctype;
} nconf_msginfo_t;

struct _msgdec_hdl {
    nconf_msginfo_t msg[NCONF_MAX_MESSAGE_DECODE];
    uint8_t msgnum;
};

/*=============================================================================
 * Global Variables
 *============================================================================*/

UB_SD_GETMEM_DEF(NCONF_MSGDEC_HANDLE, sizeof(struct _msgdec_hdl), 1);

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static nconf_msgdec_t nconf_msgdec_hdl_init(void);
static void nconf_msgdec_hdl_deinit(nconf_msgdec_t hdl);
static void nconf_msgdec_deinit_msginfo(nconf_msginfo_t *msginfo);
static int nconf_msgdec_parse_netconf_frame(nconf_msgdec_t hdl,
                                            uint8_t *frame, uint32_t len);
static x4netconf_frame_type_t nconf_msgdec_get_frame_type(uint8_t *msg, uint32_t len);
static uint8_t* nconf_msgdec_parse_chunked_frame(uint8_t *frame, uint32_t *len,
                                                 uint8_t **next);
static uint8_t* nconf_msgdec_parse_eom_frame(uint8_t *msg, uint32_t *len,
                                             uint8_t **next);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

nconf_msgdec_t nconf_msgdec_parse_client_msg(uint8_t *ncmsg, uint32_t ncmsglen,
                                             uint8_t *msgnum)
{
    nconf_msgdec_t rethdl=NULL;
    if (NULL!=ncmsg && 0!=ncmsglen) {
        nconf_msgdec_t hdl=nconf_msgdec_hdl_init();
        if (NULL != hdl) {
            int ret=nconf_msgdec_parse_netconf_frame(hdl, ncmsg, ncmsglen);
            if (ret > 0) {
                *msgnum=hdl->msgnum;
                rethdl=hdl;
            } else {
                /* release the handle if some error happens along the way */
                nconf_msgdec_hdl_deinit(hdl);
            }
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid parmeters\n", __func__);
    }
    return rethdl;
}

nconf_msg_type_t nconf_msgdec_get_msg_type(nconf_msgdec_t dechdl, uint8_t idx)
{
    nconf_nodeptr_t rootnode=NULL;
    nconf_msg_type_t rettype=NCONF_MSG_TYPE_MAX;
    if (NULL != dechdl) {
        rootnode=dechdl->msg[idx].rootnode;
        if (NCONF_MSG_TYPE_MAX != dechdl->msg[idx].msgtype) {
            rettype=dechdl->msg[idx].msgtype;
        } else if (NULL != XMLUTILS_NODE_GET_NAME(rootnode)) {
            char *msgname=XMLUTILS_NODE_GET_NAME(rootnode);
            if (strcmp(msgname, "hello")==0) {
                rettype=NCONF_MSG_TYPE_HELLO;
            } else if (strcmp(msgname, "rpc")==0) {
                rettype=NCONF_MSG_TYPE_RPC;
            } else if (strcmp(msgname, "rpc-reply")==0) {
                rettype=NCONF_MSG_TYPE_RPC_REPLY;
            } else if (strcmp(msgname, "close-session")==0) {
                rettype=NCONF_MSG_TYPE_CLOSE_SESSION;
            } else {
                UB_LOG(UBL_ERROR, "%s:unsupported netconf message type: <%s>\n",
                       __func__, (char*)msgname);
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to get xml root node\n", __func__);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid decoder handle\n", __func__);
    }
    return rettype;
}

nconf_rpc_msg_type_t nconf_msgdec_get_rpc_msg_type(nconf_msgdec_t dechdl, uint8_t idx)
{
    nconf_rpc_msg_type_t rettype=NCONF_RPC_MSG_TYPE_MAX;
    if (NULL != dechdl) {
        if (NCONF_RPC_MSG_TYPE_MAX != dechdl->msg[idx].rpctype) {
            rettype=dechdl->msg[idx].rpctype;
        } else {
            nconf_nodeptr_t rpcnode=
                XMLUTILS_NODE_GET_CHILD(dechdl->msg[idx].rootnode);
            char *rpcname=XMLUTILS_NODE_GET_NAME(rpcnode);
            dechdl->msg[idx].rpcnode=rpcnode;
            if (strcmp(rpcname, "get")==0) {
                rettype=NCONF_RPC_MSG_TYPE_GET;
            } else if (strcmp(rpcname, "get-config")==0) {
                rettype=NCONF_RPC_MSG_TYPE_GET_CONFIG;
            } else if (strcmp(rpcname, "edit-config")==0) {
                rettype=NCONF_RPC_MSG_TYPE_EDIT_CONFIG;
            } else if (strcmp(rpcname, "delete-config")==0) {
                rettype=NCONF_RPC_MSG_TYPE_DELETE_CONFIG;
            } else if (strcmp(rpcname, "lock")==0) {
                rettype=NCONF_RPC_MSG_TYPE_LOCK;
            } else if (strcmp(rpcname, "unlock")==0) {
                rettype=NCONF_RPC_MSG_TYPE_UNLOCK;
            } else if (strcmp(rpcname, "close-session")==0) {
                rettype=NCONF_RPC_MSG_TYPE_CLOSE_SESSION;
            } else if (strcmp(rpcname, "kill-session")==0) {
                rettype=NCONF_RPC_MSG_TYPE_KILL_SESSION;
            } else if (strcmp(rpcname, "get-schema")==0) {
                rettype=NCONF_RPC_MSG_TYPE_GET_SCHEMA;
            } else {
                UB_LOG(UBL_ERROR, "%s:unsupported rpc message: [%s]\n",
                       __func__, rpcname);
            }
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid decoder handle\n", __func__);
    }
    return rettype;
}

nconf_nodeptr_t nconf_msgdec_get_rootnode(nconf_msgdec_t dechdl, uint8_t idx)
{
    nconf_nodeptr_t retnode=NULL;
    if (NULL != dechdl) {
        retnode=dechdl->msg[idx].rootnode;
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid decoder handle\n", __func__);
    }
    return retnode;
}

nconf_nodeptr_t nconf_msgdec_get_rpcnode(nconf_msgdec_t dechdl, uint8_t idx)
{
    nconf_nodeptr_t retnode=NULL;
    if (NULL != dechdl) {
        retnode=dechdl->msg[idx].rpcnode;
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid decoder handle\n", __func__);
    }
    return retnode;
}

nconf_nodeptr_t* nconf_msgdec_search_node(nconf_msgdec_t dechdl, uint8_t idx,
                                          nconf_xpath_args_t *sargs)
{
    nconf_nodeptr_t *foundnode=NULL;
    if (NULL != dechdl && idx < dechdl->msgnum) {
        sargs->xmldoc=dechdl->msg[idx].doc;
        int ret=nconf_xmlutils_xpath_search(sargs);
        if (0==ret) {
            foundnode=sargs->foundnodes.node;
        } else {
            UB_LOG(UBL_DEBUGV, "%s:Node name: [%s], value: [%s] not found\n", __func__,
                   sargs->nodes[0].name, NULL==sargs->nodes[0].value ?
                   "null" : sargs->nodes[0].value);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid parameter [idx=%u]\n", __func__, idx);
    }
    return foundnode;
}

void nconf_msgdec_search_node_release(nconf_xpath_args_t *sargs)
{
    if (NULL != sargs) {
        sargs->xmldoc=NULL;
        nconf_xmlutils_xpath_release(sargs);
    }
}

void nconf_msgdec_release_rpcreq_doc(nconf_msgdec_t dechdl, uint8_t idx)
{
    if (NULL != dechdl && idx < dechdl->msgnum) {
        nconf_msgdec_deinit_msginfo(&dechdl->msg[idx]);
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid parameter [idx=%u]\n", __func__, idx);
    }
}

void nconf_msgdec_release_handle(nconf_msgdec_t dechdl)
{
    if (NULL != dechdl) {
        nconf_msgdec_hdl_deinit(dechdl);
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid decoder handle\n", __func__);
    }
}

static nconf_msgdec_t nconf_msgdec_hdl_init(void)
{
    struct _msgdec_hdl *hdl=(struct _msgdec_hdl*)
         UB_SD_GETMEM(NCONF_MSGDEC_HANDLE, sizeof(struct _msgdec_hdl));
    if(ub_assert_fatal((NULL!=hdl), __func__, "alloc error")){return NULL;}
    memset(hdl, 0, sizeof(struct _msgdec_hdl));
    for (uint8_t i=0; i<NCONF_MAX_MESSAGE_DECODE; i++) {
        hdl->msg[i].msgtype=NCONF_MSG_TYPE_MAX;
        hdl->msg[i].rpctype=NCONF_RPC_MSG_TYPE_MAX;
    }
    return hdl;
}

static void nconf_msgdec_hdl_deinit(nconf_msgdec_t hdl)
{
    uint8_t i;
    for (i=0; i<NCONF_MAX_MESSAGE_DECODE; i++) {
        nconf_msgdec_deinit_msginfo(&hdl->msg[i]);
    }
    UB_SD_RELMEM(NCONF_MSGDEC_HANDLE, hdl);
}

static void nconf_msgdec_deinit_msginfo(nconf_msginfo_t *msginfo)
{
    if (NULL != msginfo->rootnode) {
        nconf_xmlutils_release_node(msginfo->rootnode);
        msginfo->rootnode=NULL;
    }
    if (NULL != msginfo->rpcnode) {
        nconf_xmlutils_release_node(msginfo->rpcnode);
        msginfo->rpcnode=NULL;
    }
    if (NULL != msginfo->doc) {
        nconf_xmlutils_free_doc(msginfo->doc);
        msginfo->doc=NULL;
    }
}

static int nconf_msgdec_parse_netconf_frame(nconf_msgdec_t hdl, uint8_t *frame, uint32_t len)
{
    x4netconf_frame_type_t frametype=NCONF_FRAME_TYPE_INVALID;
    uint32_t minimumlen=strlen(NCONF_END_OF_MESSAGE_MARKER);
    uint32_t remaininglen=len;
    uint8_t *xmlMsg=NULL;
    uint8_t *next=NULL;
    uint8_t *cur=frame;
    int ret=0;

    while(remaininglen > minimumlen && hdl->msgnum < NCONF_MAX_MESSAGE_DECODE) {
        uint32_t xmllen=remaininglen;
        uint8_t idx=hdl->msgnum;
        xmlMsg=NULL;
        frametype=nconf_msgdec_get_frame_type(cur, remaininglen);
        switch(frametype) {
            case NCONF_FRAME_TYPE_CHUNKED_FRAMING:
                xmlMsg=nconf_msgdec_parse_chunked_frame(cur, &xmllen, &next);
                break;
            case NCONF_FRAME_TYPE_END_OF_MSG_FRAMING:
                xmlMsg=nconf_msgdec_parse_eom_frame(cur, &xmllen, &next);
                break;
            default:
                UB_LOG(UBL_ERROR, "%s:Invalid Message\n", __func__);
                break;
        }
        if (NULL==xmlMsg) {
            UB_LOG(UBL_ERROR, "%s:frame parsing failed\n", __func__);
            break;
        }
        hdl->msg[idx].doc=nconf_xmlutils_parse_xml(xmlMsg, xmllen);
        if (NULL==hdl->msg[idx].doc) {
            UB_LOG(UBL_ERROR, "%s:xml doc creation failed\n", __func__);
            break;
        }
        hdl->msg[idx].rootnode=nconf_xmlutils_get_root_node(hdl->msg[idx].doc);
        if (NULL==hdl->msg[idx].rootnode) {
            UB_LOG(UBL_ERROR, "%s:failed to get root node\n", __func__);
            break;
        }
        hdl->msg[idx].msgtype=nconf_msgdec_get_msg_type(hdl, idx);
        if (NCONF_MSG_TYPE_RPC==hdl->msg[idx].msgtype) {
            hdl->msg[hdl->msgnum].rpctype=nconf_msgdec_get_rpc_msg_type(hdl, idx);
        }
        hdl->msgnum++;
        ret++;
        if (NULL != next) {
            remaininglen=remaininglen-(next-cur);
            cur=next;
        } else {
            break;
        }
    }

    return ret;
}

static x4netconf_frame_type_t nconf_msgdec_get_frame_type(uint8_t *msg, uint32_t len)
{
    x4netconf_frame_type_t rettype=NCONF_FRAME_TYPE_INVALID;
    uint8_t *tmp=(uint8_t*)strstr((char*)msg, NCONF_END_OF_MESSAGE_MARKER);
    if (NULL != tmp) {
        rettype=NCONF_FRAME_TYPE_END_OF_MSG_FRAMING;
    } else {
        uint32_t chunkPrefixLen=strlen(NCONF_CHUNK_SIZE_MSG_PREFIX);
        if (chunkPrefixLen < len) {
            if(strncmp((char*)msg, NCONF_CHUNK_SIZE_MSG_PREFIX, chunkPrefixLen)==0) {
                rettype=NCONF_FRAME_TYPE_CHUNKED_FRAMING;
            }
        }
    }
    return rettype;
}

static uint8_t* nconf_msgdec_parse_chunked_frame(uint8_t *frame, uint32_t *len,
                                                 uint8_t **next)
{
    uint32_t endOfChunksLen=strlen(NCONF_END_OF_CHUNKS_MARKER);
    uint32_t assembledlen=0;
    uint32_t framelen=*len;
    uint32_t chunksize;
    uint8_t *xml=frame;
    uint8_t *ptr=frame;

    uint8_t *tmp=(uint8_t*)strstr((char*)frame, NCONF_END_OF_CHUNKS_MARKER);
    if (NULL==tmp) {
        UB_LOG(UBL_ERROR, "%s:missing end of chunks marker\n", __func__);
        return NULL;
    }

    if ((tmp-frame)+endOfChunksLen < framelen) {
        framelen=(tmp-frame)+endOfChunksLen;
        *next=tmp+endOfChunksLen;
    } else {
        *next=NULL;
    }

    while (*ptr!='\0') {
        char *chunkStart=(char*)ptr;
        char *temp=strstr((char*)ptr, "\n#");
        if (NULL==temp) {
            UB_LOG(UBL_ERROR, "%s:invalid chunk encoding\n", __func__);
            break;
        }
        temp += 2;  /* skip '\n#' */

        /* when we have at least 1 chunk, check if it's already end-of-chunks */
        if (assembledlen > 0) {
            if (strncmp(temp, "#\n", 2) == 0) { break; }
        }

        char *lf=strchr(temp, '\n');
        if (NULL==lf) {
            UB_LOG(UBL_ERROR, "%s:invalid chunk encoding: "
                              "missing LF after chunk size\n", __func__);
            assembledlen=0;
            break;
        }
        int sizeLen = lf - temp;
        if ((uint32_t)sizeLen >= framelen) {
            UB_LOG(UBL_ERROR, "%s:invalid chunk encoding: "
                              "missing chunk data\n", __func__);
            assembledlen=0;
            break;
        }
        *lf='\0';
        chunksize = strtol(temp, NULL, 10);
        if (chunksize <= 0) {
            UB_LOG(UBL_ERROR, "%s:invalid chunk encoding: "
                              "invalid chunk-size\n", __func__);
            assembledlen=0;
            break;
        }
        temp=lf+1;
        memmove(chunkStart, temp, chunksize);

        /* move to the next chunk */
        ptr+=chunksize;
        assembledlen+=chunksize;
    }

    /* clear the rest of the frame */
    if (assembledlen > 0 && framelen > assembledlen) {
        memset(frame+assembledlen, 0, framelen-assembledlen);
    }
    *len=assembledlen;
    return xml;
}

static uint8_t* nconf_msgdec_parse_eom_frame(uint8_t *msg, uint32_t *msglen,
                                             uint8_t **next)
{
    uint32_t xmllen=0U;
    uint8_t *xml=msg;

    /* Remove NETCONF EOD marker ']]>]]>' */
    uint8_t *eod=(uint8_t*)strstr((char*)xml, NCONF_END_OF_MESSAGE_MARKER);
    if (eod != NULL) {
        uint32_t marklen=strlen(NCONF_END_OF_MESSAGE_MARKER);
        xmllen=(uint32_t)(eod - xml);
        if ((xmllen+marklen) < *msglen) {
            *next=xml+(xmllen+marklen);
        } else {
            *next=NULL;
        }
        *msglen = xmllen;
        *eod = '\0';
    }
    return xml;
}

