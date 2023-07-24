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
#include <string.h>
#include <tsn_unibase/unibase.h>
#include "yang_db_enumeration.h"

/*-----------------------------------------------------------------------------
 * Type Definitions
 *----------------------------------------------------------------------------*/

typedef struct yang_enumeration_str_map {
    const char **strlist;
    const uint32_t strnum;
    const char *keyword;
} yang_enumeration_str_map_t;

/*-----------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------------*/

const char* interfaces_oper_status[] = {
    "",
    "up",
    "down",
    "testing",
    "unknown",
    "dormant",
    "not-present",
    "lower-layer-down",
    NULL
};
const uint32_t interfaces_oper_status_num = sizeof(interfaces_oper_status)/sizeof(char*);

const char* interfaces_duplex[] = {
    "",
    "full",
    "half",
    "unknown",
    NULL
};
const uint32_t interfaces_duplex_num = sizeof(interfaces_duplex)/sizeof(char*);

const char* tsn_uni_talker_status[] = {
    "none",
    "ready",
    "failed",
    NULL
};
const uint32_t tsn_uni_talker_status_num = sizeof(tsn_uni_talker_status)/sizeof(char*);

const char* tsn_uni_listener_status[] = {
    "none",
    "ready",
    "partial-failed",
    "failed",
    NULL
};
const uint32_t tsn_uni_listener_status_num = sizeof(tsn_uni_listener_status)/sizeof(char*);

const char* tsn_uni_stream_status[] = {
    "planned",
    "configured",
    "modified",
    NULL
};
const uint32_t tsn_uni_stream_status_num = sizeof(tsn_uni_stream_status)/sizeof(char*);

const char* ieee802dq_control_reg[] = {
	"registered",
	"not-registered",
	NULL
};
const uint32_t ieee802dq_control_reg_num = sizeof(ieee802dq_control_reg)/sizeof(char*);

const char* ieee802dq_entry_stadyn[] = {
	"static",
	"dynamic",
	NULL
};
const uint32_t ieee802dq_entry_stadyn_num = sizeof(ieee802dq_entry_stadyn)/sizeof(char*);

const char* excelfore_mrp_protocol[] = {
	"",
	"mmrp",
	"mvrp",
	"msrp",
	"dynamic",
	NULL
};
const uint32_t excelfore_mrp_protocol_num = sizeof(excelfore_mrp_protocol)/sizeof(char*);

const char *frame_preemption_status[] = {
	"",
	"express",
	"preemptable",
	NULL
};
const uint32_t frame_preemption_status_num = sizeof(frame_preemption_status)/sizeof(char*);

const char *hold_request_status[]= {
	"",
	"hold", 	// hold=1
	"release",	// release=2
	NULL
};
const uint32_t hold_request_status_num = sizeof(hold_request_status)/sizeof(char*);

const char* private_key_algorithm[] = {
    "rsa",
    "dsa",
    "secp192r1",
    "sect163k1",
    "sect163r2",
    "secp224r1",
    "sect233k1",
    "sect233r1",
    "secp256r1",
    "sect283k1",
    "sect283r1",
    "secp384r1",
    "sect409k1",
    "sect409r1",
    "secp521r1",
    "sect571k1",
    "sect571r1",
    NULL
};
const uint32_t private_key_algorithm_num = sizeof(private_key_algorithm)/sizeof(char*);

const char* lldp_chassis_id_subtype[] = {
    "chassis-component",
    "interface-alias",
    "port-component",
    "mac-address",
    "network-address",
    "interface-name",
    "local",
    NULL
};
const uint32_t lldp_chassis_id_subtype_num = sizeof(lldp_chassis_id_subtype)/sizeof(char*);

/*-----------------------------------------------------------------------------
 * List of Identity Reference Tables
 *----------------------------------------------------------------------------*/
const yang_enumeration_str_map_t yang_enumeration_str_map_list[] = {
    { interfaces_oper_status,   interfaces_oper_status_num,  "oper-status"      },
    { interfaces_duplex,        interfaces_duplex_num,       "duplex"           },
    { tsn_uni_talker_status,    tsn_uni_talker_status_num,   "talker-status"    },
    { tsn_uni_listener_status,  tsn_uni_listener_status_num, "listener-status"  },
    { tsn_uni_stream_status,    tsn_uni_stream_status_num,   "stream-status"    },
    { ieee802dq_control_reg,    ieee802dq_control_reg_num,   "control-reg"      },
    { ieee802dq_entry_stadyn,   ieee802dq_entry_stadyn_num,  "entry-stadyn"     },
    { excelfore_mrp_protocol,   excelfore_mrp_protocol_num,  "mrp-protocol"     },
    { frame_preemption_status,  frame_preemption_status_num, "preemption-status"},
    { hold_request_status,      hold_request_status_num,     "hold-request"     },
    { private_key_algorithm,    private_key_algorithm_num,   "algorithm"        },
    { lldp_chassis_id_subtype,  lldp_chassis_id_subtype_num, "chassis-id-subtype"},
    { NULL,                     0,                           NULL               }
};

/*-----------------------------------------------------------------------------
 * Static APIs
 *----------------------------------------------------------------------------*/
static  int yang_enumeration_get_tblidx(char *hints)
{
    int ret=-1;
    int i;
    if (hints != NULL){
        for(i=0; yang_enumeration_str_map_list[i].strlist != NULL; i++){
            if(strstr(hints, yang_enumeration_str_map_list[i].keyword) != NULL){
                ret=i;
                break;
            }
        }
    }
    return ret;
}

/*-----------------------------------------------------------------------------
 * External APIs
 *----------------------------------------------------------------------------*/
uint32_t yang_enumeration_getval(char *enumeration_str, char *hints)
{
    uint32_t enumeration_val=INVALID_ENUMERATION_VALUE;
    int tblidx=yang_enumeration_get_tblidx(hints);
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown enumeration hints=%s\n", __func__, hints);
        goto end_operation;
    }
    const char **strlist=yang_enumeration_str_map_list[tblidx].strlist;
    uint32_t sidx;
    for (sidx=0; sidx < yang_enumeration_str_map_list[tblidx].strnum; sidx++) {
        if (strcmp(enumeration_str, strlist[sidx])==0) {
            enumeration_val=sidx;
            break;
        }
    }
end_operation:
    return enumeration_val;
}

char* yang_enumeration_getstr(uint32_t enumeration_val, char *hints)
{
    char* enumeration_str=NULL;
    int tblidx=yang_enumeration_get_tblidx(hints);
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown enumeration hints=%s\n", __func__, hints);
        goto end_operation;
    }
    const char **strlist=yang_enumeration_str_map_list[tblidx].strlist;
    if(enumeration_val < yang_enumeration_str_map_list[tblidx].strnum) {
        enumeration_str=(char*)strlist[enumeration_val];
    }
end_operation:
    return enumeration_str;
}
