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
#include <stdlib.h>
#include <tsn_unibase/unibase.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include <tsn_uniconf/yangs/yang_node.h>
#include "xl4-extmod-xl4avtp.h"

extern uint8_t XL4_EXTMOD_XL4AVTP_func(uc_dbald *dbald);
#define XL4_EXTMOD_XL4AVTP XL4_EXTMOD_XL4AVTP_func(dbald)

const char *xl4_extmod_xl4avtp_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"xl4avtp", 		// 3(0x3)
	"AVTP_INSTANCE", 		// 4(0x4)
	"INSTANCE_INDEX", 		// 5(0x5)
	"DEBUGLOG_MEMORY_SIZE", 		// 6(0x6)
	"DEBUGLOG_MEMORY_FILE_PREFIX", 		// 7(0x7)
	"AVTPD_TRAFFIC_CLASS_NUM", 		// 8(0x8)
	"AVTPD_PORT_BITRATE", 		// 9(0x9)
	"AVTPD_PRI_TC_MAP", 		// 10(0xa)
	"index0", 		// 11(0xb)
	"value", 		// 12(0xc)
	"AVTPD_TC_INTERVAL", 		// 13(0xd)
	"AVTPD_DELTABW", 		// 14(0xe)
	"AVTPD_IDLESLOPE", 		// 15(0xf)
	"AVTPD_MAX_NUM_PACKETS", 		// 16(0x10)
	"AVTPD_READY", 		// 17(0x11)
};
const uint8_t xl4_extmod_xl4avtp_enum_max=18;

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	uint8_t apsd[7]={XL4_DATA_RW, XL4_ENUM_TABLE,};
	int i;
	apsd[2] = XL4_EXTMOD_RW;
	apsd[3] = modid;
	apsd[4] = 255u;
	apsd[5] = 1u;
	for(i=0;i<xl4_extmod_xl4avtp_enum_max;i++){
		apsd[6] = (uint8_t)i;
		if(uc_dbal_create(dbald, apsd, 7,
			(void*)xl4_extmod_xl4avtp_enum_strings[i], 
				strlen(xl4_extmod_xl4avtp_enum_strings[i])+1)!=0){
		return -1;}
	}
	return 0;
}

int xl4_extmod_xl4avtp_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[8]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, XL4_EXTMOD_XL4AVTP)!=0){return -1;}
	//0000_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	aps[0] = XL4_EXTMOD_RW;
	aps[1] = XL4_EXTMOD_XL4AVTP;
	aps[2] = XL4_EXTMOD_XL4AVTP_AVTP_INSTANCE;
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_PRI_TC_MAP;
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUEKEY;
	aps[5] = XL4_EXTMOD_XL4AVTP_INDEX0;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4AVTP_VALUEKEY;
	aps[4] = XL4_EXTMOD_XL4AVTP_INSTANCE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0001_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_PRI_TC_MAP;
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0002_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0003_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0004_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0005_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0006_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0007_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_PRI_TC_MAP
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0008_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_TC_INTERVAL;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUEKEY;
	aps[5] = XL4_EXTMOD_XL4AVTP_INDEX0;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0009_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0010_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0011_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0012_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0013_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0014_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0015_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_TC_INTERVAL
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0016_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_DELTABW;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUEKEY;
	aps[5] = XL4_EXTMOD_XL4AVTP_INDEX0;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0017_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0018_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0019_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0020_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0021_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0022_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0023_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_DELTABW
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0024_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_IDLESLOPE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUEKEY;
	aps[5] = XL4_EXTMOD_XL4AVTP_INDEX0;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0025_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0026_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0027_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0028_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0029_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0030_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0031_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_IDLESLOPE
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0032_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_MAX_NUM_PACKETS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUEKEY;
	aps[5] = XL4_EXTMOD_XL4AVTP_INDEX0;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0033_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	aps[4] = XL4_EXTMOD_XL4AVTP_VALUE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0034_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0035_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0036_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0037_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0038_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0039_xl4-extmod/xl4avtp/AVTP_INSTANCE/AVTPD_MAX_NUM_PACKETS
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	//0040_xl4-extmod/xl4avtp/AVTP_INSTANCE
	aps[3] = XL4_EXTMOD_XL4AVTP_DEBUGLOG_MEMORY_SIZE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4AVTP_DEBUGLOG_MEMORY_FILE_PREFIX;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_TRAFFIC_CLASS_NUM;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_PORT_BITRATE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[0] = XL4_EXTMOD_RO;
	aps[3] = XL4_EXTMOD_XL4AVTP_AVTPD_READY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[2] = XL4_EXTMOD_XL4AVTP_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

