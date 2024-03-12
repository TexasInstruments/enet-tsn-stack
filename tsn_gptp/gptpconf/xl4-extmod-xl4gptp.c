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
#include "xl4-extmod-xl4gptp.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);
YANG_NODE_CACHED_EXTMODULE_GET_ENUM_DEF(XL4GPTP, "xl4gptp");
#define XL4_EXTMOD_XL4GPTP XL4_EXTMOD_XL4GPTP_func(dbald)

int xl4_extmod_xl4gptp_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t aps[5];
	int i;
	int res=-1;
	uint32_t vsize=0;
	void *kvs[2]={NULL};
	uint8_t kss[2];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,YANG_DB_ONHW_ALWAYS,
	                              NULL,aps,kvs,kss,NULL,0};
	if(yang_node_extmod_set_enum(dbald, "xl4gptp")<0){return -1;};
	dbpara.atype=YANG_DB_ACTION_READ;
	aps[0] = XL4_EXTMOD_RO;
	aps[1] = XL4_EXTMOD_XL4GPTP;
	aps[2] = XL4_EXTMOD_XL4GPTP_VALUEKEY;
	aps[3] = 255;
	if(yang_db_action(dbald, hwald, &dbpara)==0){
		if(dbpara.vsize==1 && (*(uint8_t*)dbpara.value==1)){return 0;}
	}
	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	yang_db_action(dbald, hwald, &dbpara);
	dbpara.atype=YANG_DB_ACTION_CREATE;
	//0000_xl4-extmod/xl4gptp/GPTP_INSTANCE
	kss[0]=0; // INSTANCE_INDEX
	res=yang_value_conv(YANG_VTYPE_UINT8, "0",
		&kvs[0], &vsize, NULL);
	if(res<0){goto erexit;}
	kss[0]=res;
	if(kvs[1]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[1]);kvs[1]=NULL;}
	aps[0] = XL4_EXTMOD_RW;
	aps[2] = XL4_EXTMOD_XL4GPTP_GPTP_INSTANCE;
	aps[3] = XL4_EXTMOD_XL4GPTP_TEST_SYNC_REC_PORT;
	aps[4] = 255;
	res=yang_value_conv(YANG_VTYPE_INT8, "-1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TEST_SYNC_SEND_PORT;
	res=yang_value_conv(YANG_VTYPE_INT8, "-1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MAX_DOMAIN_NUMBER;
	res=yang_value_conv(YANG_VTYPE_INT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MASTER_PTPDEV;
	res=yang_value_conv(YANG_VTYPE_STRING, """",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TXTS_LOST_TIME;
	res=yang_value_conv(YANG_VTYPE_UINT32, "20000000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SOCKET_TXPRIORITY;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FIRST_DOMAIN_THIS_CLOCK;
	res=yang_value_conv(YANG_VTYPE_INT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_THIS_CLOCK;
	res=yang_value_conv(YANG_VTYPE_INT8, "-1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_NUMBER;
	res=yang_value_conv(YANG_VTYPE_INT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE;
	res=yang_value_conv(YANG_VTYPE_UINT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_AFTERSEND_GUARDTIME;
	res=yang_value_conv(YANG_VTYPE_UINT32, "300000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_GPTPNET_EXTRA_TOUTNS;
	res=yang_value_conv(YANG_VTYPE_UINT32, "1000000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MAX_ADJUST_RATE_ON_CLOCK;
	res=yang_value_conv(YANG_VTYPE_UINT32, "1000000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_NEIGHBOR_PROP_DELAY;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_STATIC_PORT_STATE_SLAVE_PORT;
	res=yang_value_conv(YANG_VTYPE_INT8, "-1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FOLLOW_AVNU;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_BMCS_QUICK_UPDATE_MODE;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CMLDS_MODE;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_NEIGHBOR_PROPDELAY_MINLIMIT;
	res=yang_value_conv(YANG_VTYPE_INT32, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_INITIAL_SLAVE_TIME;
	res=yang_value_conv(YANG_VTYPE_UINT32, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_INITIAL_GM_STABLE_TIME;
	res=yang_value_conv(YANG_VTYPE_UINT32, "1000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_NORMAL_GM_STABLE_TIME;
	res=yang_value_conv(YANG_VTYPE_UINT32, "10000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_START_VALUE;
	res=yang_value_conv(YANG_VTYPE_UINT8, "2",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE;
	res=yang_value_conv(YANG_VTYPE_UINT8, "10",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_START_VALUE;
	res=yang_value_conv(YANG_VTYPE_UINT8, "2",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE;
	res=yang_value_conv(YANG_VTYPE_UINT8, "10",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CLOCK_COMPUTE_INTERVAL_MSEC;
	res=yang_value_conv(YANG_VTYPE_UINT32, "1000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_UPDATE_MRATE_PPB;
	res=yang_value_conv(YANG_VTYPE_UINT32, "10",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_STABLE_PPB;
	res=yang_value_conv(YANG_VTYPE_UINT32, "100",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB;
	res=yang_value_conv(YANG_VTYPE_UINT32, "800",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_ACTIVE_DOMAIN_AUTO_SWITCH;
	res=yang_value_conv(YANG_VTYPE_UINT8, "2",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_ON;
	res=yang_value_conv(YANG_VTYPE_UINT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_ALIGNTIME;
	res=yang_value_conv(YANG_VTYPE_UINT32, "100000000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_CYCLETIME;
	res=yang_value_conv(YANG_VTYPE_UINT32, "10000000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_FILE;
	res=yang_value_conv(YANG_VTYPE_STRING, "/tmp/tsn_gptpd_debugmem.log",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_SIZE;
	res=yang_value_conv(YANG_VTYPE_UINT32, "64",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_OVIP_MODE_STRT_PORTNO;
	res=yang_value_conv(YANG_VTYPE_UINT16, "5018",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MASTER_CLOCK_SHARED_MEM;
	res=yang_value_conv(YANG_VTYPE_STRING, """",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PTPVFD_CLOCK_RATE;
	res=yang_value_conv(YANG_VTYPE_INT32, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_MAX_CONSEC_TS_DIFF;
	res=yang_value_conv(YANG_VTYPE_UINT32, "500000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_TS2DIFF_CACHE_FACTOR;
	res=yang_value_conv(YANG_VTYPE_UINT16, "150",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_USE_HW_PHASE_ADJUSTMENT;
	res=yang_value_conv(YANG_VTYPE_UINT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PHASE_ADJUSTMENT_BY_FREQ;
	res=yang_value_conv(YANG_VTYPE_UINT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_ACTIVATE_ABNORMAL_HOOKS;
	res=yang_value_conv(YANG_VTYPE_UINT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_RESET_FREQADJ_BECOMEGM;
	res=yang_value_conv(YANG_VTYPE_UINT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_REPRESENT_PORT_NUMBER;
	res=yang_value_conv(YANG_VTYPE_INT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PERFMON_SHORT_PERIOD;
	res=yang_value_conv(YANG_VTYPE_UINT32, "900000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PERFMON_LONG_PERIOD;
	res=yang_value_conv(YANG_VTYPE_UINT32, "86400000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_PERFMON_CURRENT_PERIOD;
	res=yang_value_conv(YANG_VTYPE_UINT32, "3000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CONF_TILLD_PPS_REFCLK_HZ;
	res=yang_value_conv(YANG_VTYPE_UINT32, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[3] = XL4_EXTMOD_XL4GPTP_CONF_TILLD_PPS_OUTIDX;
	res=yang_value_conv(YANG_VTYPE_UINT32, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	if(kvs[0]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[0]);}
	kvs[0] = NULL;
	dbpara.atype=YANG_DB_ACTION_CREATE;
	aps[0] = XL4_EXTMOD_RO;
	aps[2] = XL4_EXTMOD_XL4GPTP_VALUEKEY;
	aps[3] = 255;
	res=yang_value_conv(YANG_VTYPE_UINT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	res=0;
erexit:
	if(dbpara.value!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, dbpara.value);}
	for(i=0;i<1;i++){
		if(kvs[i]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[i]);}
	}
	return res;
}

