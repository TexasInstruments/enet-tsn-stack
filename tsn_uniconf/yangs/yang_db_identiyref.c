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
#include "yang_db_identiyref.h"

/*-----------------------------------------------------------------------------
 * Type Definitions
 *----------------------------------------------------------------------------*/

/* None */

/*-----------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------------*/

const yang_identity_map_t network_protocol[] = {
    { "udp-ipv4"  , 0x0001 }, /**< UDP on IPv4 */
    { "udp-ipv6"  , 0x0002 }, /**< UDP on IPv6 */
    { "ieee802-3" , 0x0003 }, /**< IEEE Std 802.3 (Ethernet). */
    { "devicenet" , 0x0004 }, /**< DeviceNet */
    { "controlnet", 0x0005 }, /**< ControlNet */
    { "profinet"  , 0x0006 }, /**< PROFINET */
    { "otn"       , 0x0007 }, /**< Optical Transport Network (OTN) */
    { "unknown"   , 0xFFFE }  /**< Unknown */
};
const uint32_t network_protocol_num = sizeof(network_protocol)/sizeof(network_protocol[0]);

const yang_identity_map_t clock_class[] = {
    { "cc-primary-sync"                      , 6U   },
    { "cc-primary-sync-lost"                 , 7U   },
    { "cc-application-specific-sync"         , 13U  },
    { "cc-application-specific-sync-lost"    , 14U  },
    { "cc-primary-sync-alternative-a"        , 52U  },
    { "cc-application-specific-alternative-a", 58U  },
    { "cc-primary-sync-alternative-b"        , 187U },
    { "cc-application-specific-alternative-b", 193U },
    { "cc-default"                           , 248U },
    { "cc-slave-only"                        , 255U }
};
const uint32_t clock_class_num = sizeof(clock_class)/sizeof(clock_class[0]);

const yang_identity_map_t clock_accuracy[] = {
    { "ca-time-accurate-to-1000-fs", 0x17 },
    { "ca-time-accurate-to-2500-fs", 0x18 },
    { "ca-time-accurate-to-10-ps"  , 0x19 },
    { "ca-time-accurate-to-25ps"   , 0x1A },
    { "ca-time-accurate-to-100-ps" , 0x1B },
    { "ca-time-accurate-to-250-ps" , 0x1C },
    { "ca-time-accurate-to-1000-ps", 0x1D },
    { "ca-time-accurate-to-2500-ps", 0x1E },
    { "ca-time-accurate-to-10-ns"  , 0x1F },
    { "ca-time-accurate-to-25-ns"  , 0x20 },
    { "ca-time-accurate-to-100-ns" , 0x21 },
    { "ca-time-accurate-to-250-ns" , 0x22 },
    { "ca-time-accurate-to-1000-ns", 0x23 },
    { "ca-time-accurate-to-2500-ns", 0x24 },
    { "ca-time-accurate-to-10-us"  , 0x25 },
    { "ca-time-accurate-to-25-us"  , 0x26 },
    { "ca-time-accurate-to-100-us" , 0x27 },
    { "ca-time-accurate-to-250-us" , 0x28 },
    { "ca-time-accurate-to-1000-us", 0x29 },
    { "ca-time-accurate-to-2500-us", 0x2A },
    { "ca-time-accurate-to-10-ms"  , 0x2B },
    { "ca-time-accurate-to-25-ms"  , 0x2C },
    { "ca-time-accurate-to-100-ms" , 0x2D },
    { "ca-time-accurate-to-250-ms" , 0x2E },
    { "ca-time-accurate-to-1-s"    , 0x2F },
    { "ca-time-accurate-to-10-s"   , 0x30 },
    { "ca-time-accurate-to-gt-10-s", 0x31 }
};
const uint32_t clock_accuracy_num = sizeof(clock_accuracy)/sizeof(clock_accuracy[0]);

const yang_identity_map_t time_source[] = {
    { "atomic-clock"       , 0x10 },
    { "gnss"               , 0x20 },
    { "terrestrial-radio"  , 0x30 },
    { "serial-time-code"   , 0x39 },
    { "ptp"                , 0x40 },
    { "ntp"                , 0x50 },
    { "hand-set"           , 0x60 },
    { "other"              , 0x90 },
    { "internal-oscillator", 0xA0 }
};
const uint32_t time_source_num = sizeof(time_source)/sizeof(time_source[0]);

const yang_identity_map_t gate_operation[] = {
    { "set-gate-states"       , 0x0 },
    { "set-and-hold-mac"      , 0x1 },
    { "set-and-release-mac"   , 0x2 },
};
const uint32_t gate_operation_num = sizeof(gate_operation)/sizeof(gate_operation[0]);

const yang_identity_map_t map_type[] = {
    { "specified"       , 0x01 },
    { "san-rfc822-name" , 0x02 },
    { "san-dns-name"    , 0x03 },
    { "san-ip-address"  , 0x04 },
    { "san-any"         , 0x05 },
    { "common-name"     , 0x06 }
};
const uint32_t map_type_num = sizeof(map_type)/sizeof(map_type[0]);

const yang_identity_map_t addr_family_map[] = {
    { "ipv4"       , 1 },
    { "ipv6"       , 2 },
    { "nsap"       , 3 }
};
const uint32_t addr_family_map_num = sizeof(addr_family_map)/sizeof(addr_family_map[0]);
/*-----------------------------------------------------------------------------
 * List of Identity Reference Tables
 *----------------------------------------------------------------------------*/
const yang_identityref_t yang_identityref_list[] = {
    { network_protocol, network_protocol_num, "network-protocol" },
    { clock_class,      clock_class_num,      "clock-class"      },
    { clock_accuracy,   clock_accuracy_num,   "clock-accuracy"   },
    { time_source,      time_source_num,      "time-source"      },
    { gate_operation,   gate_operation_num,   "gate-operation"   },
    { map_type,         map_type_num,         "map-type"         },
    { addr_family_map,  addr_family_map_num,  "address-family"         },
    { NULL,             0,                    NULL               }
};

/*-----------------------------------------------------------------------------
 * Static APIs
 *----------------------------------------------------------------------------*/
static  int yang_identityref_get_tblidx(char *hints)
{
    int ret=-1;
    int i;
    if (hints != NULL){
        for(i=0; yang_identityref_list[i].reftbl != NULL; i++){
            if(strstr(hints, yang_identityref_list[i].keyword) != NULL){
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

uint32_t yang_identityref_getval(char *identity_str, char *hints)
{
    uint32_t identity_val = INVALID_IDENTIY_VALUE;
    int tblidx=yang_identityref_get_tblidx(hints);
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown identityref hints=%s\n", __func__, hints);
        goto end_operation;
    }
    const yang_identity_map_t *reftbl = yang_identityref_list[tblidx].reftbl;
    uint32_t i;
    for (i = 0; i < yang_identityref_list[tblidx].refnum; i++) {
        if (!strcmp(identity_str, reftbl[i].name)){
            identity_val=reftbl[i].value;
            break;
        }
    }
end_operation:
    return identity_val;
}

char* yang_identityref_getstr(uint32_t identity_val, char *hints)
{
    char* identity_str=NULL;
    int tblidx=yang_identityref_get_tblidx(hints);
    if(tblidx < 0) {
        UB_LOG(UBL_ERROR, "%s:unknown identityref hints=%s\n", __func__, hints);
        goto end_operation;
    }
    const yang_identity_map_t *reftbl = yang_identityref_list[tblidx].reftbl;
    uint32_t i;
    for (i = 0; i < yang_identityref_list[tblidx].refnum; i++) {
        if (reftbl[i].value==identity_val) {
            identity_str=(char*)reftbl[i].name;
            break;
        }
    }
end_operation:
    return identity_str;
}
