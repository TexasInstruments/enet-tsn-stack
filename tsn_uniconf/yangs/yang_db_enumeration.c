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

typedef struct yang_enumeration_map {
    const char *nsprefix;
    const char *str;
    uint32_t enval;
} yang_enumeration_map_t;

typedef struct yang_enumeration_str_map {
    const yang_enumeration_map_t *enumtbl;
    const uint32_t strnum;
    const char *keyword;
} yang_enumeration_str_map_t;

/*-----------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------------*/

const yang_enumeration_map_t interfaces_oper_status[] = {
	{ ""  , ""                 ,0},
	{ "if", "up"               ,0},
	{ "if", "down"             ,0},
	{ "if", "testing"          ,0},
	{ "if", "unknown"          ,0},
	{ "if", "dormant"          ,0},
	{ "if", "not-present"      ,0},
	{ "if", "lower-layer-down" ,0}
};
#define interfaces_oper_status_num \
	sizeof(interfaces_oper_status)/sizeof(interfaces_oper_status[0])

const yang_enumeration_map_t interfaces_duplex[] = {
	{ ""     , ""        ,0},
	{ "xl4if", "full"    ,0},
	{ "xl4if", "half"    ,0},
	{ "xl4if", "unknown" ,0}
};
#define interfaces_duplex_num sizeof(interfaces_duplex)/sizeof(interfaces_duplex[0])

const yang_enumeration_map_t port_state[] = {
	{ ""   , ""             ,0},
	{ "ptp-tt", "initializing" ,0},
	{ "ptp-tt", "faulty"       ,0},
	{ "ptp-tt", "disabled"     ,0},
	{ "ptp-tt", "listening"    ,0},
	{ "ptp-tt", "pre-master"   ,0},
	{ "ptp-tt", "master"       ,0},
	{ "ptp-tt", "passive"      ,0},
	{ "ptp-tt", "uncalibrated" ,0},
	{ "ptp-tt", "slave"        ,0}
};
#define port_state_num sizeof(port_state)/sizeof(port_state[0])

const yang_enumeration_map_t tsn_uni_talker_status[] = {
	{ "dot1q-tsn-types", "none"   ,0},
	{ "dot1q-tsn-types", "ready"  ,0},
	{ "dot1q-tsn-types", "failed" ,0}
};
#define tsn_uni_talker_status_num \
    sizeof(tsn_uni_talker_status)/sizeof(tsn_uni_talker_status[0])

const yang_enumeration_map_t tsn_uni_listener_status[] = {
	{ "dot1q-tsn-types", "none"           ,0},
	{ "dot1q-tsn-types", "ready"          ,0},
	{ "dot1q-tsn-types", "partial-failed" ,0},
	{ "dot1q-tsn-types", "failed"         ,0}
};
#define tsn_uni_listener_status_num \
    sizeof(tsn_uni_listener_status)/sizeof(tsn_uni_listener_status[0])

const yang_enumeration_map_t tsn_uni_stream_status[] = {
	{ "dot1q-tsn-config-uni", "planned"    ,0},
	{ "dot1q-tsn-config-uni", "configured" ,0},
	{ "dot1q-tsn-config-uni", "modified"   ,0}
};
#define tsn_uni_stream_status_num \
    sizeof(tsn_uni_stream_status)/sizeof(tsn_uni_stream_status[0])

const yang_enumeration_map_t ieee802dq_control_fwd[] = {
	{ "dot1q-types", "forward"        ,0},
	{ "dot1q-types", "filter"         ,0},
	{ "dot1q-types", "forward-filter" ,0}
};
#define ieee802dq_control_fwd_num \
    sizeof(ieee802dq_control_fwd)/sizeof(ieee802dq_control_fwd[0])

const yang_enumeration_map_t ieee802dq_control_reg[] = {
	{ "dot1q-types", "registered"     ,0},
	{ "dot1q-types", "not-registered" ,0}
};
#define ieee802dq_control_reg_num \
    sizeof(ieee802dq_control_reg)/sizeof(ieee802dq_control_reg[0])

const yang_enumeration_map_t ieee802dq_entry_stadyn[] = {
	{ "dot1q", "static"  ,0},
	{ "dot1q", "dynamic" ,0}
};
#define ieee802dq_entry_stadyn_num \
    sizeof(ieee802dq_entry_stadyn)/sizeof(ieee802dq_entry_stadyn[0])

const yang_enumeration_map_t excelfore_mrp_protocol[] = {
	{ ""         , ""        ,0},
	{ "xl4tsnrmt", "mmrp"    ,0},
	{ "xl4tsnrmt", "mvrp"    ,0},
	{ "xl4tsnrmt", "msrp"    ,0},
	{ "xl4tsnrmt", "dynamic" ,0}
};
#define excelfore_mrp_protocol_num \
    sizeof(excelfore_mrp_protocol)/sizeof(excelfore_mrp_protocol[0])

const yang_enumeration_map_t frame_preemption_status[] = {
	{ ""       , ""            ,0},
	{ "preempt", "express"     ,0},
	{ "preempt", "preemptable" ,0}
};
#define frame_preemption_status_num \
    sizeof(frame_preemption_status)/sizeof(frame_preemption_status[0])

const yang_enumeration_map_t hold_request_status[]= {
	{ ""       , ""        ,0},
	{ "preempt", "hold"    ,0}, 	// hold=1
	{ "preempt", "release" ,0}  	// release=2
};
#define hold_request_status_num \
    sizeof(hold_request_status)/sizeof(hold_request_status[0])

const yang_enumeration_map_t private_key_algorithm[] = {
	{ "kc", "rsa"       ,0},
	{ "kc", "dsa"       ,0},
	{ "kc", "secp192r1" ,0},
	{ "kc", "sect163k1" ,0},
	{ "kc", "sect163r2" ,0},
	{ "kc", "secp224r1" ,0},
	{ "kc", "sect233k1" ,0},
	{ "kc", "sect233r1" ,0},
	{ "kc", "secp256r1" ,0},
	{ "kc", "sect283k1" ,0},
	{ "kc", "sect283r1" ,0},
	{ "kc", "secp384r1" ,0},
	{ "kc", "sect409k1" ,0},
	{ "kc", "sect409r1" ,0},
	{ "kc", "secp521r1" ,0},
	{ "kc", "sect571k1" ,0},
	{ "kc", "sect571r1" ,0}
};
#define private_key_algorithm_num \
    sizeof(private_key_algorithm)/sizeof(private_key_algorithm[0])

const yang_enumeration_map_t lldp_chassis_id_subtype[] = {
	{ ""    , ""                  ,0},   // chassis-component start from 1
	{ "ieee", "chassis-component" ,0},
	{ "ieee", "interface-alias"   ,0},
	{ "ieee", "port-component"    ,0},
	{ "ieee", "mac-address"       ,0},
	{ "ieee", "network-address"   ,0},
	{ "ieee", "interface-name"    ,0},
	{ "ieee", "local"             ,0}
};
#define lldp_chassis_id_subtype_num \
    sizeof(lldp_chassis_id_subtype)/sizeof(lldp_chassis_id_subtype[0])

const yang_enumeration_map_t lldp_port_id_subtype[] = {
	{ ""    , ""                 ,0},   // interface-alias start from 1
	{ "ieee", "interface-alias"  ,0},
	{ "ieee", "port-component"   ,0},
	{ "ieee", "mac-address"      ,0},
	{ "ieee", "network-address"  ,0},
	{ "ieee", "interface-name"   ,0},
	{ "ieee", "agent-circuit-id" ,0},
	{ "ieee", "local"            ,0}
};
#define lldp_port_id_subtype_num \
    sizeof(lldp_port_id_subtype)/sizeof(lldp_port_id_subtype[0])

const yang_enumeration_map_t lldp_admin_status[] = {
	{ ""    , ""          ,0},
	{ "lldp", "tx-only"   ,0},
	{ "lldp", "rx-only"   ,0},
	{ "lldp", "tx-and-rx" ,0},
	{ "lldp", "disabled"  ,0}
};
#define lldp_admin_status_num \
    sizeof(lldp_admin_status)/sizeof(lldp_admin_status[0])

const yang_enumeration_map_t lldp_man_addr_if_subtype[] = {
	{ ""          , ""                   ,0},
	{ "lldp-types", "unknown"            ,0},
	{ "lldp-types", "port-ref"           ,0},
	{ "lldp-types", "system-port-number" ,0}
};
#define lldp_man_addr_if_subtype_num \
    sizeof(lldp_man_addr_if_subtype)/sizeof(lldp_man_addr_if_subtype[0])

const yang_enumeration_map_t conformance_type[] = {
	{ "yanglib", "implement" ,0},
	{ "yanglib", "import"    ,0}
};
#define conformance_type_num \
    sizeof(conformance_type)/sizeof(conformance_type[0])

const yang_enumeration_map_t netconf_datastore_type[] = {
	{ "ncm", "running"   ,0},
	{ "ncm", "candidate" ,0},
	{ "ncm", "startup"   ,0}
};
#define netconf_datastore_type_num \
    sizeof(netconf_datastore_type)/sizeof(netconf_datastore_type[0])

const yang_enumeration_map_t gmstate[] = {
	{ "xl4gptp", "unsync"   ,0},
	{ "xl4gptp", "unstable" ,0},
	{ "xl4gptp", "stable"   ,0}
};
#define gmstate_num sizeof(gmstate)/sizeof(gmstate[0])

const yang_enumeration_map_t transport_status[] = {
	{ "xl4nconf", "offline" ,0},
	{ "xl4nconf", "online"  ,0}
};
const uint32_t transport_status_num = sizeof(transport_status)/sizeof(transport_status[0]);

const yang_enumeration_map_t ipc_type[] = {
	{ "xl4nconf", "local-tcp"           ,0},
	{ "xl4nconf", "unix-domain-socket"  ,0}
};
const uint32_t ipc_type_num = sizeof(ipc_type)/sizeof(ipc_type[0]);

const yang_enumeration_map_t endst_iface_status[] = {
	{ "xl4cnc", "init"             ,0},
	{ "xl4cnc", "connected"        ,0},
	{ "xl4cnc", "disconnected"     ,0},
	{ "xl4cnc", "deleted"          ,0},
	{ "xl4cnc", "operation-failed" ,0}
};
const uint32_t endst_iface_status_num =
   sizeof(endst_iface_status)/sizeof(endst_iface_status[0]);

const yang_enumeration_map_t delay_mechanism[] = {
	{ "ptp-tt", "e2e"           ,1},
	{ "ptp-tt", "p2p"           ,2},
	{ "ptp-tt", "common-p2p"    ,3},
	{ "ptp-tt", "special"       ,4},
	{ "ptp-tt", "no-mechanism"  ,254}
};
const uint32_t delay_mechanism_num =
   sizeof(delay_mechanism)/sizeof(delay_mechanism[0]);

const yang_enumeration_map_t vlan_tag_id_type[] = {
	{ "dot1cb-sid", "tagged"    ,1},
	{ "dot1cb-sid", "priority"  ,2},
	{ "dot1cb-sid", "all"       ,3}
};
const uint32_t vlan_tag_id_type_num =
   sizeof(vlan_tag_id_type)/sizeof(vlan_tag_id_type[0]);

const yang_enumeration_map_t stream_type[] = {
	{ "xl4aed", "none"   ,0},
	{ "xl4aed", "not-ready" ,0},
	{ "xl4aed", "ready" ,0}
};
#define stream_type_num sizeof(stream_type)/sizeof(stream_type[0])

const yang_enumeration_map_t control_status[] = {
	{ "xl4dot1q", "init",    0},
	{ "xl4dot1q", "busy",    0},
	{ "xl4dot1q", "success", 0},
	{ "xl4dot1q", "failed",  0}
};
#define control_status_num sizeof(control_status)/sizeof(control_status[0])

/*-----------------------------------------------------------------------------
 * List of Identity Reference Tables
 *----------------------------------------------------------------------------*/
const yang_enumeration_str_map_t yang_enumeration_str_map_list[] = {
    { interfaces_oper_status,   interfaces_oper_status_num,  "oper-status"        },
    { interfaces_duplex,        interfaces_duplex_num,       "duplex"             },
    { port_state,               port_state_num,              "port-state"         },
    { tsn_uni_talker_status,    tsn_uni_talker_status_num,   "talker-status"      },
    { tsn_uni_listener_status,  tsn_uni_listener_status_num, "listener-status"    },
    { tsn_uni_stream_status,    tsn_uni_stream_status_num,   "stream-status"      },
    { ieee802dq_control_fwd,    ieee802dq_control_fwd_num,   "static-filtering-entries/control-element" },
    { ieee802dq_control_fwd,    ieee802dq_control_fwd_num,   "dynamic-reservation-entries/control-element" },
    { ieee802dq_control_fwd,    ieee802dq_control_fwd_num,   "dynamic-filtering-entries/control-element" },
    { ieee802dq_control_reg,    ieee802dq_control_reg_num,   "mac-address-registration-entries/control-element" },
    { ieee802dq_control_reg,    ieee802dq_control_reg_num,   "dynamic-vlan-registration-entries/control-element" },
    { ieee802dq_entry_stadyn,   ieee802dq_entry_stadyn_num,  "entry-type"         },
    { excelfore_mrp_protocol,   excelfore_mrp_protocol_num,  "protocol"           },
    { frame_preemption_status,  frame_preemption_status_num, "preemption-status"  },
    { hold_request_status,      hold_request_status_num,     "hold-request"       },
    { private_key_algorithm,    private_key_algorithm_num,   "algorithm"          },
    /*LLDP enums*/
    { lldp_chassis_id_subtype,  lldp_chassis_id_subtype_num, "chassis-id-subtype" },
    { lldp_port_id_subtype,     lldp_port_id_subtype_num,    "port-id-subtype"    },
    { lldp_admin_status,        lldp_admin_status_num,       "admin-status"       },
    { lldp_man_addr_if_subtype, lldp_man_addr_if_subtype_num,"if-subtype"         },
    /*LLDP enums end*/
    { conformance_type,         conformance_type_num,        "conformance-type"   },
    { netconf_datastore_type,   netconf_datastore_type_num,  "datastore"          },
    { gmstate,                  gmstate_num,                 "gmstate"            },
    { transport_status,         transport_status_num,        "transport-status"   },
    { ipc_type,                 ipc_type_num,                "ipc-type"           },
    { endst_iface_status,       endst_iface_status_num,      "end-station-status" },
    { delay_mechanism,          delay_mechanism_num,         "delay-mechanism"    },
    { vlan_tag_id_type,         vlan_tag_id_type_num,        "tagged"             },
    { stream_type,              stream_type_num,             "stream-type"        },
    { control_status,           control_status_num,          "control-status"     },
    { NULL,                     0,                           NULL                 }
};

/*-----------------------------------------------------------------------------
 * Static APIs
 *----------------------------------------------------------------------------*/
static  int yang_enumeration_get_tblidx(const char *hints)
{
    int ret=-1;
    int i;
    if (hints != NULL){
        for(i=0; yang_enumeration_str_map_list[i].enumtbl != NULL; i++){
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
uint32_t yang_enumeration_getval(const char *enumeration_str, int eslen,
				 const char *hints)
{
    uint32_t enumeration_val=INVALID_ENUMERATION_VALUE;
    int tblidx=yang_enumeration_get_tblidx(hints);
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown enumeration hints=%s\n", __func__, hints);
        return enumeration_val;
    }
    const yang_enumeration_map_t *enumtbl=yang_enumeration_str_map_list[tblidx].enumtbl;
    uint32_t sidx;
    for (sidx=0; sidx < yang_enumeration_str_map_list[tblidx].strnum; sidx++) {
	    if (memcmp(enumeration_str, enumtbl[sidx].str, eslen)==0) {
		    if(enumtbl[sidx].enval!=0){
			    enumeration_val=enumtbl[sidx].enval;
		    }else{
			    enumeration_val=sidx;
		    }
		    break;
	    }
    }
    return enumeration_val;
}

static char* yang_enumeration_getstrns(uint32_t enumeration_val, const char *hints,
				       bool getstr)
{
    int tblidx=yang_enumeration_get_tblidx(hints);
    uint32_t sidx;
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown enumeration hints=%s\n", __func__, hints);
        return NULL;
    }
    const yang_enumeration_map_t *enumtbl=yang_enumeration_str_map_list[tblidx].enumtbl;

    for (sidx=0; sidx < yang_enumeration_str_map_list[tblidx].strnum; sidx++) {
	    if(enumtbl[sidx].enval!=0){
		    if(enumeration_val==enumtbl[sidx].enval){
			    if(getstr){
				    return (char*)enumtbl[sidx].str;
			    }else{
				    return (char*)enumtbl[sidx].nsprefix;
			    }
		    }
	    }else{
		    if(enumeration_val==sidx){
			    if(getstr){
				    return (char*)enumtbl[sidx].str;
			    }else{
				    return (char*)enumtbl[sidx].nsprefix;
			    }
		    }
	    }
    }
    return NULL;
}

char* yang_enumeration_getstr(uint32_t enumeration_val, const char *hints)
{
	return yang_enumeration_getstrns(enumeration_val, hints, true);
}

char* yang_enumeration_getns(uint32_t enumeration_val, const char *hints)
{
	return yang_enumeration_getstrns(enumeration_val, hints, false);
}
