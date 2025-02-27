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
#include "yang_db_bits.h"

/*-----------------------------------------------------------------------------
 * Type Definitions
 *----------------------------------------------------------------------------*/

typedef struct yang_bit_map {
    const char *nsprefix;
    const char *str;
} yang_bit_map_t;

typedef struct yang_bit_str_map {
    const yang_bit_map_t *bittbl;
    const uint8_t bitnum;
    const char *keyword;
} yang_bit_str_map_t;

/*-----------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------------*/

const yang_bit_map_t tls_tx_enable[] = {
	{ "lldp", "port-desc" },
	{ "lldp", "sys-name"  },
	{ "lldp", "sys-desc"  },
	{ "lldp", "sys-cap"   }
};
#define tls_tx_enable_num \
	((uint8_t)(sizeof(tls_tx_enable)/sizeof(tls_tx_enable[0])))

const yang_bit_map_t system_capabilities_map[] = {
	{ "lldp-types", "other"               },
	{ "lldp-types", "repeater"            },
	{ "lldp-types", "bridge"              },
	{ "lldp-types", "wlan-access-point"   },
	{ "lldp-types", "router"              },
	{ "lldp-types", "telephone"           },
	{ "lldp-types", "docsis-cable-device" },
	{ "lldp-types", "station-only"        },
	{ "lldp-types", "cvlan-component"     },
	{ "lldp-types", "svlan-component"     },
	{ "lldp-types", "two-port-mac-relay"  }
};
#define system_capabilities_map_num \
	((uint8_t)(sizeof(system_capabilities_map)/sizeof(system_capabilities_map[0])))

/*-----------------------------------------------------------------------------
 * List of Identity Reference Tables
 *----------------------------------------------------------------------------*/
const yang_bit_str_map_t yang_bits_str_map_list[] = {
    { tls_tx_enable,            tls_tx_enable_num,           "tlvs-tx-enable"      },
    { system_capabilities_map,  system_capabilities_map_num, "system-capabilities" },
    { NULL,                     0,                           NULL                  }
};

/*-----------------------------------------------------------------------------
 * Static APIs
 *----------------------------------------------------------------------------*/
static  int yang_bits_get_tblidx(const char *hints)
{
    int ret=-1;
    int i;
    if (hints != NULL){
        for(i=0; yang_bits_str_map_list[i].bittbl != NULL; i++){
            if(strstr(hints, yang_bits_str_map_list[i].keyword) != NULL){
                ret=i;
                break;
            }
        }
    }
    return ret;
}

static char* yang_bits_getstrns(uint8_t bitpos, const char *hints, bool getstr)
{
    int tblidx=yang_bits_get_tblidx(hints);
    if(tblidx<0){
        UB_LOG(UBL_ERROR, "%s:unknown bit table hints=%s\n", __func__, hints);
        return NULL;
    }
    const yang_bit_map_t *bittbl=yang_bits_str_map_list[tblidx].bittbl;
    for (uint8_t sidx=0; sidx<yang_bits_str_map_list[tblidx].bitnum; sidx++){
        if(bitpos==sidx){
            if(getstr){
                return (char*)bittbl[sidx].str;
            }else{
                return (char*)bittbl[sidx].nsprefix;
            }
        }
    }
    return NULL;
}

/*-----------------------------------------------------------------------------
 * External APIs
 *----------------------------------------------------------------------------*/
char* yang_bits_getstr(uint8_t bitpos, const char *hints)
{
    return yang_bits_getstrns(bitpos, hints, true);
}

char* yang_bits_getns(uint8_t bitpos, const char *hints)
{
    return yang_bits_getstrns(bitpos, hints, false);
}

uint8_t yang_bits_getpos(const char *bit_name, int nlen, const char *hints)
{
    uint32_t bitpos=INVALID_BITPOS_VALUE;
    int tblidx=yang_bits_get_tblidx(hints);
    if(tblidx<0){
        UB_LOG(UBL_ERROR, "%s:unknown bit table hints=%s\n", __func__, hints);
        return bitpos;
    }
    const yang_bit_map_t *bittbl=yang_bits_str_map_list[tblidx].bittbl;
    for(uint8_t sidx=0; sidx<yang_bits_str_map_list[tblidx].bitnum; sidx++){
        if(memcmp(bit_name, bittbl[sidx].str, nlen)==0){
            bitpos=sidx;
            break;
        }
    }
    return bitpos;
}

#define MAX_BITNUM (32U)
uint8_t yang_bits_get_bitnum(const char *hints)
{
    int tblidx=yang_bits_get_tblidx(hints);
    if(tblidx<0){
        UB_LOG(UBL_ERROR, "%s:unknown bit table hints=%s\n", __func__, hints);
        return MAX_BITNUM;
    }
    return yang_bits_str_map_list[tblidx].bitnum;
}

