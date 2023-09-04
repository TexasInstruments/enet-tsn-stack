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
/* Automatically generated file.  Don't edit this file.*/
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <tsn_unibase/unibase.h>
#include "../yang_modules_schema.h"

struct schema_map {
    const char *module;
    const char *schema;
} module_to_schema_map[] = {
    { "ietf-netconf", ietf_netconf_schema },
    { "ietf-interfaces", ietf_interfaces_schema },
    { "ieee802-dot1q-bridge", ieee802_dot1q_bridge_schema },
    { "ieee802-dot1ab-lldp", ieee802_dot1ab_lldp_schema },
    { "ieee1588-ptp", ieee1588_ptp_schema },
    { "ieee802-dot1q-tsn-config-uni", ieee802_dot1q_tsn_config_uni_schema },
    { "ietf-yang-library", ietf_yang_library_schema },
    { "ietf-netconf-server", ietf_netconf_server_schema },
    { "ietf-keychain", ietf_keychain_schema },
    { "excelfore-tsn-remote", excelfore_tsn_remote_schema },
    { "ieee802-dot1as-ptp", ieee802_dot1as_ptp_schema },
    { "ieee802-dot1q-sched-bridge", ieee802_dot1q_sched_bridge_schema },
    { "ieee802-dot1q-preemption-bridge", ieee802_dot1q_preemption_bridge_schema },
    { "xl4-ietf-interfaces", xl4_ietf_interfaces_schema },
    { "xl4-ieee1588-ptp", xl4_ieee1588_ptp_schema },
    { "xl4-ieee802-dot1q-bridge", xl4_ieee802_dot1q_bridge_schema },
    { "xl4-ieee802-dot1q-tsn-config-uni", xl4_ieee802_dot1q_tsn_config_uni_schema },
    { "xl4-netconf-server", xl4_netconf_server_schema },
    { "xl4-keychain", xl4_keychain_schema },
    { NULL, NULL }
};

const char *yang_db_get_schema(char *module)
{
    struct schema_map *map=&module_to_schema_map[0];
    char *ret=NULL;
    int i;
    for (i=0; map[i].module; i++) {
        if(strcmp((char*)map[i].module, module)==0) {
            ret=(char*)map[i].schema;
            break;
        }
    }
    return ret;
}

