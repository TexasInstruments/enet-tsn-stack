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
 * @file        nconf_msgdef.h
 *
 * @brief       Netconf Messages Definitions Header File
 */
#ifndef __NCONF_MSGDEF_H__
#define __NCONF_MSGDEF_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include "nconf_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

/** \brief XML namespace where all netconf protocol are defined */
#define NCONF_BASE_XML_NAMESPACE       "urn:ietf:params:xml:ns:netconf:base:1.0"

/**
 * \anchor Chunked Framing Mechanism
 * \name Chunked Framing Mechanism
 *
 *  This mechanism encodes all NETCONF messages with a chunked framing.
 *  Specifically, the message follows the ABNF [RFC5234] rule Chunked-Message:
 *      Chunked-Message =   1*chunk
 *                          end-of-chunks
 *
 *      chunk           =   LF HASH chunk-size LF
 *                          chunk-data
 *      chunk-size      =   1*DIGIT1 0*DIGIT
 *      chunk-data      =   1*OCTET
 *
 *      end-of-chunks   =   LF HASH HASH LF
 *
 *      DIGIT1          =   %x31-39
 *      DIGIT           =   %x30-39
 *      HASH            =   %x23
 *      LF              =   %x0A
 *      OCTET           =   %x00-FF
 *  @{
*/

/** \brief chunk        =   LF HASH chunk-size LF   */
#define NCONF_CHUNK_SIZE_MSG_PREFIX     "\n#"

/** \brief end-of-chunks=   LF HASH HASH LF      */
#define NCONF_END_OF_CHUNKS_MARKER      "\n##\n"

/* @} */

/**
 * \anchor End-of-Message Framing Mechanism
 * \name End-of-Message Framing Mechanism
 *
 *  This mechanism exists for backwards compatibility with
 *  implementations that only advertise base:1.0.
 *
 *  When this mechanism is used, the special character sequence ]]>]]>,
 *  MUST be sent after each message (XML document) in the NETCONF exchange.
 *  @{
*/

/** \brief Marks the end of message frame */
#define NCONF_END_OF_MESSAGE_MARKER     "]]>]]>"

/* @} */

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

/** \brief Netconf Frame Types */
typedef enum {
    NCONF_FRAME_TYPE_CHUNKED_FRAMING=0x00U,
    NCONF_FRAME_TYPE_END_OF_MSG_FRAMING,
    NCONF_FRAME_TYPE_INVALID
} x4netconf_frame_type_t;

/** \brief Netconf Message Types */
typedef enum {
    NCONF_MSG_TYPE_HELLO=0x00U,
    NCONF_MSG_TYPE_RPC,
    NCONF_MSG_TYPE_RPC_REPLY,
    NCONF_MSG_TYPE_CLOSE_SESSION, /* Some client sends a non
                                   * RPC close-session request
                                   */
    NCONF_MSG_TYPE_MAX
} nconf_msg_type_t;

/** \brief Netconf RPC Message Types */
typedef enum {
    /* RFC 4741/6241 (Netconf Base) */
    NCONF_RPC_MSG_TYPE_GET=0x00U,
    NCONF_RPC_MSG_TYPE_GET_CONFIG,
    NCONF_RPC_MSG_TYPE_EDIT_CONFIG,
    NCONF_RPC_MSG_TYPE_COPY_CONFIG,
    NCONF_RPC_MSG_TYPE_DELETE_CONFIG,
    NCONF_RPC_MSG_TYPE_LOCK,
    NCONF_RPC_MSG_TYPE_UNLOCK,
    NCONF_RPC_MSG_TYPE_CLOSE_SESSION,
    NCONF_RPC_MSG_TYPE_KILL_SESSION,
    /* RFC 6022 (Netconf Monitoring) */
    NCONF_RPC_MSG_TYPE_GET_SCHEMA,
    NCONF_RPC_MSG_TYPE_MAX
} nconf_rpc_msg_type_t;

/** \brief Netconf RPC REPLY Message Types */
typedef enum {
    NCONF_RPC_REPLY_MSG_TYPE_OK=0x00U,
    NCONF_RPC_REPLY_MSG_TYPE_ERROR,
    NCONF_RPC_REPLY_MSG_TYPE_MAX
} nconf_rpcreply_msg_type_t;

typedef enum {
    NCONF_RPC_ERROR_TYPE_TRANSPORT=0x00U,
    NCONF_RPC_ERROR_TYPE_RPC,
    NCONF_RPC_ERROR_TYPE_PROTOCOL,
    NCONF_RPC_ERROR_TYPE_APPLICATION,
    NCONF_RPC_ERROR_TYPE_MAX
} nconf_rpcerror_type_t;

typedef enum {
    NCONF_RPC_ERROR_TAG_NO_ERROR=0x00U,
    NCONF_RPC_ERROR_TAG_IN_USE,
    NCONF_RPC_ERROR_TAG_INVALID_VALUE,
    NCONF_RPC_ERROR_TAG_TOO_BIG,
    NCONF_RPC_ERROR_TAG_MISSING_ATTRIBUTE,
    NCONF_RPC_ERROR_TAG_BAD_ATTRIBUTE,
    NCONF_RPC_ERROR_TAG_UNKNOWN_ATTRIBUTE,
    NCONF_RPC_ERROR_TAG_MISSING_ELEMENT,
    NCONF_RPC_ERROR_TAG_BAD_ELEMENT,
    NCONF_RPC_ERROR_TAG_UNKNOWN_ELEMENT,
    NCONF_RPC_ERROR_TAG_UNKNOWN_NAMESPACE,
    NCONF_RPC_ERROR_TAG_ACCESS_DENIED,
    NCONF_RPC_ERROR_TAG_LOCK_DENIED,
    NCONF_RPC_ERROR_TAG_RESOURCE_DENIED,
    NCONF_RPC_ERROR_TAG_ROLLBACK_FAILED,
    NCONF_RPC_ERROR_TAG_DATA_EXISTS,
    NCONF_RPC_ERROR_TAG_DATA_MISSING,
    NCONF_RPC_ERROR_TAG_OPERATION_NOT_SUPPORTED,
    NCONF_RPC_ERROR_TAG_OPERATION_FAILED,
    NCONF_RPC_ERROR_TAG_PARTIAL_OPERATION,
    NCONF_RPC_ERROR_TAG_MALFORMED_MESSAGE,
    NCONF_RPC_ERROR_TAG_MAX
} nconf_rpcerror_tag_t;

typedef enum {
    NCONF_RPC_ERROR_SEV_ERROR=0x00U,
    NCONF_RPC_ERROR_SEV_WARNING,
    NCONF_RPC_ERROR_SEV_MAX
} nconf_rpcerror_sev_t;

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_MSGDEF_H__ */
