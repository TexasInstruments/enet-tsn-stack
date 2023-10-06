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
#include "tsn_uniconf/yangs/yang_modules.h"
#include "xl4-extmod-xl4gptp.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *xl4_extmod_xl4gptp_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"xl4gptp", 		// 2(0x2)
	"GPTP_INSTANCE", 		// 3(0x3)
	"INSTANCE_INDEX", 		// 4(0x4)
	"TEST_SYNC_REC_PORT", 		// 5(0x5)
	"TEST_SYNC_SEND_PORT", 		// 6(0x6)
	"MAX_DOMAIN_NUMBER", 		// 7(0x7)
	"MASTER_PTPDEV", 		// 8(0x8)
	"TXTS_LOST_TIME", 		// 9(0x9)
	"SOCKET_TXPRIORITY", 		// 10(0xa)
	"FIRST_DOMAIN_THIS_CLOCK", 		// 11(0xb)
	"SECOND_DOMAIN_THIS_CLOCK", 		// 12(0xc)
	"SECOND_DOMAIN_NUMBER", 		// 13(0xd)
	"SINGLE_CLOCK_MODE", 		// 14(0xe)
	"AFTERSEND_GUARDTIME", 		// 15(0xf)
	"GPTPNET_EXTRA_TOUTNS", 		// 16(0x10)
	"MAX_ADJUST_RATE_ON_CLOCK", 		// 17(0x11)
	"NEIGHBOR_PROP_DELAY", 		// 18(0x12)
	"STATIC_PORT_STATE_SLAVE_PORT", 		// 19(0x13)
	"FOLLOW_AVNU", 		// 20(0x14)
	"BMCS_QUICK_UPDATE_MODE", 		// 21(0x15)
	"CMLDS_MODE", 		// 22(0x16)
	"NEIGHBOR_PROPDELAY_MINLIMIT", 		// 23(0x17)
	"INITIAL_SLAVE_TIME", 		// 24(0x18)
	"INITIAL_GM_STABLE_TIME", 		// 25(0x19)
	"NORMAL_GM_STABLE_TIME", 		// 26(0x1a)
	"FREQ_OFFSET_IIR_ALPHA_START_VALUE", 		// 27(0x1b)
	"FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE", 		// 28(0x1c)
	"PHASE_OFFSET_IIR_ALPHA_START_VALUE", 		// 29(0x1d)
	"PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE", 		// 30(0x1e)
	"CLOCK_COMPUTE_INTERVAL_MSEC", 		// 31(0x1f)
	"FREQ_OFFSET_UPDATE_MRATE_PPB", 		// 32(0x20)
	"FREQ_OFFSET_STABLE_PPB", 		// 33(0x21)
	"FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB", 		// 34(0x22)
	"ACTIVE_DOMAIN_AUTO_SWITCH", 		// 35(0x23)
	"TSN_SCHEDULE_ON", 		// 36(0x24)
	"TSN_SCHEDULE_ALIGNTIME", 		// 37(0x25)
	"TSN_SCHEDULE_CYCLETIME", 		// 38(0x26)
	"DEBUGLOG_MEMORY_FILE", 		// 39(0x27)
	"DEBUGLOG_MEMORY_SIZE", 		// 40(0x28)
	"OVIP_MODE_STRT_PORTNO", 		// 41(0x29)
	"MASTER_CLOCK_SHARED_MEM", 		// 42(0x2a)
	"PTPVFD_CLOCK_RATE", 		// 43(0x2b)
	"MAX_CONSEC_TS_DIFF", 		// 44(0x2c)
	"TS2DIFF_CACHE_FACTOR", 		// 45(0x2d)
	"USE_HW_PHASE_ADJUSTMENT", 		// 46(0x2e)
	"PHASE_ADJUSTMENT_BY_FREQ", 		// 47(0x2f)
	"ACTIVATE_ABNORMAL_HOOKS", 		// 48(0x30)
	"RESET_FREQADJ_BECOMEGM", 		// 49(0x31)
	"REPRESENT_PORT_NUMBER", 		// 50(0x32)
	"PERFMON_SHORT_PERIOD", 		// 51(0x33)
	"PERFMON_LONG_PERIOD", 		// 52(0x34)
	"PERFMON_CURRENT_PERIOD", 		// 53(0x35)
	"CONF_TILLD_PPS_REFCLK_HZ", 		// 54(0x36)
	"CONF_TILLD_PPS_OUTIDX", 		// 55(0x37)
};

uint8_t xl4_extmod_xl4gptp_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)XL4_EXTMOD_XL4GPTP_ENUM_END;i++){
		if(!strcmp(astr, xl4_extmod_xl4gptp_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *xl4_extmod_xl4gptp_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)XL4_EXTMOD_XL4GPTP_ENUM_END){return NULL;}
	return xl4_extmod_xl4gptp_enum_strings[anum];
}

int xl4_extmod_xl4gptp_runconf_config_init(xl4_data_data_t *xdd, uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[7]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	//0000_xl4-extmod/xl4gptp/GPTP_INSTANCE
	aps[0] = XL4_EXTMOD_RW;
	aps[1] = xl4_data_get_modid(xdd, "xl4gptp");
	aps[2] = XL4_EXTMOD_XL4GPTP_GPTP_INSTANCE;
	aps[3] = XL4_EXTMOD_XL4GPTP_TEST_SYNC_REC_PORT;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TEST_SYNC_SEND_PORT;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MAX_DOMAIN_NUMBER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MASTER_PTPDEV;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TXTS_LOST_TIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SOCKET_TXPRIORITY;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FIRST_DOMAIN_THIS_CLOCK;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_THIS_CLOCK;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_NUMBER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_AFTERSEND_GUARDTIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_GPTPNET_EXTRA_TOUTNS;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MAX_ADJUST_RATE_ON_CLOCK;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_NEIGHBOR_PROP_DELAY;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_STATIC_PORT_STATE_SLAVE_PORT;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FOLLOW_AVNU;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_BMCS_QUICK_UPDATE_MODE;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CMLDS_MODE;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_NEIGHBOR_PROPDELAY_MINLIMIT;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_INITIAL_SLAVE_TIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_INITIAL_GM_STABLE_TIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_NORMAL_GM_STABLE_TIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_START_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_START_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CLOCK_COMPUTE_INTERVAL_MSEC;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_UPDATE_MRATE_PPB;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_STABLE_PPB;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_ACTIVE_DOMAIN_AUTO_SWITCH;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_ON;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_ALIGNTIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_CYCLETIME;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_FILE;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_SIZE;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_OVIP_MODE_STRT_PORTNO;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MASTER_CLOCK_SHARED_MEM;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PTPVFD_CLOCK_RATE;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MAX_CONSEC_TS_DIFF;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TS2DIFF_CACHE_FACTOR;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_USE_HW_PHASE_ADJUSTMENT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PHASE_ADJUSTMENT_BY_FREQ;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_ACTIVATE_ABNORMAL_HOOKS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_RESET_FREQADJ_BECOMEGM;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_REPRESENT_PORT_NUMBER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PERFMON_SHORT_PERIOD;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PERFMON_LONG_PERIOD;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PERFMON_CURRENT_PERIOD;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CONF_TILLD_PPS_REFCLK_HZ;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CONF_TILLD_PPS_OUTIDX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 6, &vtype, 1)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_VALUEKEY;
	aps[4] = XL4_EXTMOD_XL4GPTP_INSTANCE_INDEX;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 7, &vtype, 1)!=0){goto erexit;}
	aps[0] = XL4_EXTMOD_RO;
	aps[2] = XL4_EXTMOD_XL4GPTP_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
