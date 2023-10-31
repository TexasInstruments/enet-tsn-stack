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
    { ""  , ""                 },
    { "if", "up"               },
    { "if", "down"             },
    { "if", "testing"          },
    { "if", "unknown"          },
    { "if", "dormant"          },
    { "if", "not-present"      },
    { "if", "lower-layer-down" }
};
const uint32_t interfaces_oper_status_num =
    sizeof(interfaces_oper_status)/sizeof(interfaces_oper_status[0]);

const yang_enumeration_map_t interfaces_duplex[] = {
    { ""     , ""        },
    { "xl4if", "full"    },
    { "xl4if", "half"    },
    { "xl4if", "unknown" }
};
const uint32_t interfaces_duplex_num = sizeof(interfaces_duplex)/sizeof(interfaces_duplex[0]);

const yang_enumeration_map_t port_state[] = {
    { ""   , ""             },
    { "ptp", "initializing" },
    { "ptp", "faulty"       },
    { "ptp", "disabled"     },
    { "ptp", "listening"    },
    { "ptp", "pre-master"   },
    { "ptp", "master"       },
    { "ptp", "passive"      },
    { "ptp", "uncalibrated" },
    { "ptp", "slave"        }
};
const uint32_t port_state_num = sizeof(port_state)/sizeof(port_state[0]);

const yang_enumeration_map_t tsn_uni_talker_status[] = {
    { "dot1q-tsn-types", "none"   },
    { "dot1q-tsn-types", "ready"  },
    { "dot1q-tsn-types", "failed" }
};
const uint32_t tsn_uni_talker_status_num =
    sizeof(tsn_uni_talker_status)/sizeof(tsn_uni_talker_status[0]);

const yang_enumeration_map_t tsn_uni_listener_status[] = {
    { "dot1q-tsn-types", "none"           },
    { "dot1q-tsn-types", "ready"          },
    { "dot1q-tsn-types", "partial-failed" },
    { "dot1q-tsn-types", "failed"         }
};
const uint32_t tsn_uni_listener_status_num =
    sizeof(tsn_uni_listener_status)/sizeof(tsn_uni_listener_status[0]);

const yang_enumeration_map_t tsn_uni_stream_status[] = {
    { "dot1q-tsn-config-uni", "planned"    },
    { "dot1q-tsn-config-uni", "configured" },
    { "dot1q-tsn-config-uni", "modified"   }
};
const uint32_t tsn_uni_stream_status_num =
    sizeof(tsn_uni_stream_status)/sizeof(tsn_uni_stream_status[0]);

const yang_enumeration_map_t ieee802dq_control_reg[] = {
    { "dot1q-types", "registered"     },
    { "dot1q-types", "not-registered" }
};
const uint32_t ieee802dq_control_reg_num =
    sizeof(ieee802dq_control_reg)/sizeof(ieee802dq_control_reg[0]);

const yang_enumeration_map_t ieee802dq_entry_stadyn[] = {
    { "dot1q", "static"  },
    { "dot1q", "dynamic" }
};
const uint32_t ieee802dq_entry_stadyn_num =
    sizeof(ieee802dq_entry_stadyn)/sizeof(ieee802dq_entry_stadyn[0]);

const yang_enumeration_map_t excelfore_mrp_protocol[] = {
    { ""         , ""        },
    { "xl4tsnrmt", "mmrp"    },
    { "xl4tsnrmt", "mvrp"    },
    { "xl4tsnrmt", "msrp"    },
    { "xl4tsnrmt", "dynamic" }
};
const uint32_t excelfore_mrp_protocol_num =
    sizeof(excelfore_mrp_protocol)/sizeof(excelfore_mrp_protocol[0]);

const yang_enumeration_map_t frame_preemption_status[] = {
    { ""       , ""            },
    { "preempt", "express"     },
    { "preempt", "preemptable" }
};
const uint32_t frame_preemption_status_num =
    sizeof(frame_preemption_status)/sizeof(frame_preemption_status[0]);

const yang_enumeration_map_t hold_request_status[]= {
    { ""       , ""        },
    { "preempt", "hold"    }, 	// hold=1
    { "preempt", "release" }  	// release=2
};
const uint32_t hold_request_status_num =
    sizeof(hold_request_status)/sizeof(hold_request_status[0]);

const yang_enumeration_map_t private_key_algorithm[] = {
    { "kc", "rsa"       },
    { "kc", "dsa"       },
    { "kc", "secp192r1" },
    { "kc", "sect163k1" },
    { "kc", "sect163r2" },
    { "kc", "secp224r1" },
    { "kc", "sect233k1" },
    { "kc", "sect233r1" },
    { "kc", "secp256r1" },
    { "kc", "sect283k1" },
    { "kc", "sect283r1" },
    { "kc", "secp384r1" },
    { "kc", "sect409k1" },
    { "kc", "sect409r1" },
    { "kc", "secp521r1" },
    { "kc", "sect571k1" },
    { "kc", "sect571r1" }
};
const uint32_t private_key_algorithm_num =
    sizeof(private_key_algorithm)/sizeof(private_key_algorithm[0]);

const yang_enumeration_map_t lldp_chassis_id_subtype[] = {
    { ""    , ""                  },   // chassis-component start from 1
    { "ieee", "chassis-component" },
    { "ieee", "interface-alias"   },
    { "ieee", "port-component"    },
    { "ieee", "mac-address"       },
    { "ieee", "network-address"   },
    { "ieee", "interface-name"    },
    { "ieee", "local"             }
};
const uint32_t lldp_chassis_id_subtype_num =
    sizeof(lldp_chassis_id_subtype)/sizeof(lldp_chassis_id_subtype[0]);

const yang_enumeration_map_t lldp_port_id_subtype[] = {
    { ""    , ""                 },   // interface-alias start from 1
    { "ieee", "interface-alias"  },
    { "ieee", "port-component"   },
    { "ieee", "mac-address"      },
    { "ieee", "network-address"  },
    { "ieee", "interface-name"   },
    { "ieee", "agent-circuit-id" },
    { "ieee", "local"            }
};
const uint32_t lldp_port_id_subtype_num =
    sizeof(lldp_port_id_subtype)/sizeof(lldp_port_id_subtype[0]);

const yang_enumeration_map_t lldp_admin_status[] = {
    { ""    , ""          },
    { "lldp", "tx-only"   },
    { "lldp", "rx-only"   },
    { "lldp", "tx-and-rx" },
    { "lldp", "disabled"  }
};
const uint32_t lldp_admin_status_num =
    sizeof(lldp_admin_status)/sizeof(lldp_admin_status[0]);

const yang_enumeration_map_t lldp_man_addr_if_subtype[] = {
    { ""          , ""                   },
    { "lldp-types", "unknown"            },
    { "lldp-types", "port-ref"           },
    { "lldp-types", "system-port-number" }
};
const uint32_t lldp_man_addr_if_subtype_num =
    sizeof(lldp_man_addr_if_subtype)/sizeof(lldp_man_addr_if_subtype[0]);

const yang_enumeration_map_t conformance_type[] = {
    { "yanglib", "implement" },
    { "yanglib", "import"    }
};
const uint32_t conformance_type_num =
    sizeof(conformance_type)/sizeof(conformance_type[0]);

const yang_enumeration_map_t netconf_datastore_type[] = {
    { "ncm", "running"   },
    { "ncm", "candidate" },
    { "ncm", "startup"   }
};
const uint32_t netconf_datastore_type_num =
    sizeof(netconf_datastore_type)/sizeof(netconf_datastore_type[0]);

const yang_enumeration_map_t gmstate[] = {
    { "xl4gptp", "unsync"   },
    { "xl4gptp", "unstable" },
    { "xl4gptp", "stable"   }
};
const uint32_t gmstate_num = sizeof(gmstate)/sizeof(gmstate[0]);

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
    { ieee802dq_control_reg,    ieee802dq_control_reg_num,   "control-reg"        },
    { ieee802dq_entry_stadyn,   ieee802dq_entry_stadyn_num,  "entry-stadyn"       },
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
    { NULL,                     0,                           NULL                 }
};

/*-----------------------------------------------------------------------------
 * Static APIs
 *----------------------------------------------------------------------------*/
static  int yang_enumeration_get_tblidx(char *hints)
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
uint32_t yang_enumeration_getval(char *enumeration_str, char *hints)
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
        if (strcmp(enumeration_str, enumtbl[sidx].str)==0) {
            enumeration_val=sidx;
            break;
        }
    }
    return enumeration_val;
}

char* yang_enumeration_getstr(uint32_t enumeration_val, char *hints)
{
    char* enumeration_str=NULL;
    int tblidx=yang_enumeration_get_tblidx(hints);
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown enumeration hints=%s\n", __func__, hints);
        return enumeration_str;
    }
    const yang_enumeration_map_t *enumtbl=yang_enumeration_str_map_list[tblidx].enumtbl;
    if(enumeration_val < yang_enumeration_str_map_list[tblidx].strnum) {
        enumeration_str=(char*)enumtbl[enumeration_val].str;
    }
    return enumeration_str;
}

char* yang_enumeration_getns(uint32_t enumeration_val, char *hints)
{
    char* enumeration_nsprefix=NULL;
    int tblidx=yang_enumeration_get_tblidx(hints);
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown enumeration hints=%s\n", __func__, hints);
        return enumeration_nsprefix;
    }
    const yang_enumeration_map_t *enumtbl=yang_enumeration_str_map_list[tblidx].enumtbl;
    if(enumeration_val < yang_enumeration_str_map_list[tblidx].strnum) {
        enumeration_nsprefix=(char*)enumtbl[enumeration_val].nsprefix;
    }
    return enumeration_nsprefix;
}

