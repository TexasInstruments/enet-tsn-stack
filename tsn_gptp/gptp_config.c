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
/* Don't edit this file.  This is an automatically generated file. */
#include <string.h>
#include "gptp_config.h"
static char *config_item_strings[]={
	"CONF_TEST_SYNC_REC_PORT",
	"CONF_TEST_SYNC_SEND_PORT",
	"CONF_MAX_DOMAIN_NUMBER",
	"CONF_MAX_PORT_NUMBER",
	"CONF_PTP_PORT_ENABLED",
	"CONF_MASTER_PTPDEV",
	"CONF_TXTS_LOST_TIME",
	"CONF_SOCKET_TXPRIORITY",
	"CONF_FIRST_DOMAIN_THIS_CLOCK",
	"CONF_SECOND_DOMAIN_THIS_CLOCK",
	"CONF_SECOND_DOMAIN_NUMBER",
	"CONF_SINGLE_CLOCK_MODE",
	"CONF_AFTERSEND_GUARDTIME",
	"CONF_GPTPNET_EXTRA_TOUTNS",
	"CONF_MAX_ADJUST_RATE_ON_CLOCK",
	"CONF_IPC_NOTICE_PHASE_UPDATE",
	"CONF_IPC_NOTICE_FREQ_UPDATE",
	"CONF_NEIGHBOR_PROP_DELAY",
	"CONF_STATIC_PORT_STATE_SLAVE_PORT",
	"CONF_TIMESCALE_PTP",
	"CONF_FOLLOW_AVNU",
	"CONF_PRIMARY_PRIORITY1",
	"CONF_PRIMARY_CLOCK_CLASS",
	"CONF_PRIMARY_CLOCK_ACCURACY",
	"CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE",
	"CONF_PRIMARY_PRIORITY2",
	"CONF_SECONDARY_PRIORITY1",
	"CONF_SECONDARY_CLOCK_CLASS",
	"CONF_SECONDARY_CLOCK_ACCURACY",
	"CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE",
	"CONF_SECONDARY_PRIORITY2",
	"CONF_TIME_SOURCE",
	"CONF_BMCS_QUICK_UPDATE_MODE",
	"CONF_EXTERNAL_PORT_CONFIGURATION",
	"CONF_INITIAL_LOG_ANNOUNCE_INTERVAL",
	"CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL",
	"CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT",
	"CONF_SYNC_RECEIPT_TIMEOUT",
	"CONF_ANNOUNCE_RECEIPT_TIMEOUT",
	"CONF_CMLDS_MODE",
	"CONF_MINOR_VERSION_PTP",
	"CONF_LOG_SYNC_INTERVAL",
	"CONF_LOG_ANNOUNCE_INTERVAL",
	"CONF_LOG_PDELAYREQ_INTERVAL",
	"CONF_LOG_DELAYREQ_INTERVAL",
	"CONF_ALLOWED_LOST_RESPONSE",
	"CONF_ALLOWED_FAULTS",
	"CONF_NEIGHBOR_PROPDELAY_THRESH",
	"CONF_NEIGHBOR_PROPDELAY_MINLIMIT",
	"CONF_IPC_UDP_PORT",
	"CONF_INITIAL_SLAVE_TIME",
	"CONF_INITIAL_GM_STABLE_TIME",
	"CONF_NORMAL_GM_STABLE_TIME",
	"CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE",
	"CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE",
	"CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE",
	"CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE",
	"CONF_CLOCK_COMPUTE_INTERVAL_MSEC",
	"CONF_FREQ_OFFSET_UPDATE_MRATE_PPB",
	"CONF_FREQ_OFFSET_STABLE_PPB",
	"CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB",
	"CONF_ACTIVE_DOMAIN_AUTO_SWITCH",
	"CONF_TSN_SCHEDULE_ON",
	"CONF_TSN_SCHEDULE_ALIGNTIME",
	"CONF_TSN_SCHEDULE_CYCLETIME",
	"CONF_DEBUGLOG_MEMORY_FILE",
	"CONF_DEBUGLOG_MEMORY_SIZE",
	"CONF_OVIP_MODE_STRT_PORTNO",
	"CONF_MASTER_CLOCK_SHARED_MEM",
	"CONF_PTPVFD_CLOCK_RATE",
	"CONF_MAX_CONSEC_TS_DIFF",
	"CONF_TS2DIFF_CACHE_FACTOR",
	"CONF_USE_HW_PHASE_ADJUSTMENT",
	"CONF_PHASE_ADJUSTMENT_BY_FREQ",
	"CONF_ACTIVATE_ABNORMAL_HOOKS",
	"CONF_RESET_FREQADJ_BECOMEGM",
	"CONF_INTEGRITY_CLOCK_INFO",
};

static int32_t value_CONF_TEST_SYNC_REC_PORT=-1;
static int32_t value_CONF_TEST_SYNC_SEND_PORT=-1;
static int32_t value_CONF_MAX_DOMAIN_NUMBER=1;
static int32_t value_CONF_MAX_PORT_NUMBER=8;
static char value_CONF_PTP_PORT_ENABLED[32]={'1',',','1',',','1',',','1',',','1',',','1',',','1',',','1',0};
static char value_CONF_MASTER_PTPDEV[32]={0};
static int32_t value_CONF_TXTS_LOST_TIME=20000000;
static int32_t value_CONF_SOCKET_TXPRIORITY=0;
static int32_t value_CONF_FIRST_DOMAIN_THIS_CLOCK=1;
static int32_t value_CONF_SECOND_DOMAIN_THIS_CLOCK=-1;
static int32_t value_CONF_SECOND_DOMAIN_NUMBER=1;
static int32_t value_CONF_SINGLE_CLOCK_MODE=0;
static int32_t value_CONF_AFTERSEND_GUARDTIME=300000;
static int32_t value_CONF_GPTPNET_EXTRA_TOUTNS=1000000;
static int32_t value_CONF_MAX_ADJUST_RATE_ON_CLOCK=1000000;
static int32_t value_CONF_IPC_NOTICE_PHASE_UPDATE=1;
static int32_t value_CONF_IPC_NOTICE_FREQ_UPDATE=0;
static int32_t value_CONF_NEIGHBOR_PROP_DELAY=0;
static int32_t value_CONF_STATIC_PORT_STATE_SLAVE_PORT=-1;
static int32_t value_CONF_TIMESCALE_PTP=1;
static int32_t value_CONF_FOLLOW_AVNU=0;
static int32_t value_CONF_PRIMARY_PRIORITY1=248;
static int32_t value_CONF_PRIMARY_CLOCK_CLASS=248;
static int32_t value_CONF_PRIMARY_CLOCK_ACCURACY=0x22;
static int32_t value_CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE=0x436A;
static int32_t value_CONF_PRIMARY_PRIORITY2=248;
static int32_t value_CONF_SECONDARY_PRIORITY1=248;
static int32_t value_CONF_SECONDARY_CLOCK_CLASS=248;
static int32_t value_CONF_SECONDARY_CLOCK_ACCURACY=0x22;
static int32_t value_CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE=0x436A;
static int32_t value_CONF_SECONDARY_PRIORITY2=248;
static int32_t value_CONF_TIME_SOURCE=INTERNAL_OSCILLATOR;
static int32_t value_CONF_BMCS_QUICK_UPDATE_MODE=0;
static int32_t value_CONF_EXTERNAL_PORT_CONFIGURATION=VALUE_DISABLED;
static int32_t value_CONF_INITIAL_LOG_ANNOUNCE_INTERVAL=0;
static int32_t value_CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL=3;
static int32_t value_CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT=3;
static int32_t value_CONF_SYNC_RECEIPT_TIMEOUT=3;
static int32_t value_CONF_ANNOUNCE_RECEIPT_TIMEOUT=3;
static int32_t value_CONF_CMLDS_MODE=0;
static int32_t value_CONF_MINOR_VERSION_PTP=1;
static int32_t value_CONF_LOG_SYNC_INTERVAL=-3;
static int32_t value_CONF_LOG_ANNOUNCE_INTERVAL=0;
static int32_t value_CONF_LOG_PDELAYREQ_INTERVAL=0;
static int32_t value_CONF_LOG_DELAYREQ_INTERVAL=3;
static int32_t value_CONF_ALLOWED_LOST_RESPONSE=9;
static int32_t value_CONF_ALLOWED_FAULTS=9;
static int32_t value_CONF_NEIGHBOR_PROPDELAY_THRESH=40000;
static int32_t value_CONF_NEIGHBOR_PROPDELAY_MINLIMIT=0;
static int32_t value_CONF_IPC_UDP_PORT=0;
static int32_t value_CONF_INITIAL_SLAVE_TIME=0;
static int32_t value_CONF_INITIAL_GM_STABLE_TIME=1000;
static int32_t value_CONF_NORMAL_GM_STABLE_TIME=10000;
static int32_t value_CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE=2;
static int32_t value_CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE=10;
static int32_t value_CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE=2;
static int32_t value_CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE=10;
static int32_t value_CONF_CLOCK_COMPUTE_INTERVAL_MSEC=1000;
static int32_t value_CONF_FREQ_OFFSET_UPDATE_MRATE_PPB=10;
static int32_t value_CONF_FREQ_OFFSET_STABLE_PPB=100;
static int32_t value_CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB=800;
static int32_t value_CONF_ACTIVE_DOMAIN_AUTO_SWITCH=2;
static int32_t value_CONF_TSN_SCHEDULE_ON=0;
static int32_t value_CONF_TSN_SCHEDULE_ALIGNTIME=100000000;
static int32_t value_CONF_TSN_SCHEDULE_CYCLETIME=10000000;
static char value_CONF_DEBUGLOG_MEMORY_FILE[64]={'/','t','m','p','/','g','p','t','p','2','d','_','d','e','b','u','g','m','e','m','.','l','o','g',0};
static int32_t value_CONF_DEBUGLOG_MEMORY_SIZE=64;
static int32_t value_CONF_OVIP_MODE_STRT_PORTNO=5018;
static char value_CONF_MASTER_CLOCK_SHARED_MEM[32]={0};
static int32_t value_CONF_PTPVFD_CLOCK_RATE=0;
static int32_t value_CONF_MAX_CONSEC_TS_DIFF=500000;
static int32_t value_CONF_TS2DIFF_CACHE_FACTOR=150;
static int32_t value_CONF_USE_HW_PHASE_ADJUSTMENT=0;
static int32_t value_CONF_PHASE_ADJUSTMENT_BY_FREQ=1;
static int32_t value_CONF_ACTIVATE_ABNORMAL_HOOKS=0;
static int32_t value_CONF_RESET_FREQADJ_BECOMEGM=0;
static char value_CONF_INTEGRITY_CLOCK_INFO[128]={'I','O','D','_','A','V','B','_','P','T','P','_','C','l','o','c','k',':','7','8','7','6','9','2','3',0};
static void *gptp_item_table[CONF_ENUM_LAST_ITEM]={
	[CONF_TEST_SYNC_REC_PORT]=(void*)&value_CONF_TEST_SYNC_REC_PORT,
	[CONF_TEST_SYNC_SEND_PORT]=(void*)&value_CONF_TEST_SYNC_SEND_PORT,
	[CONF_MAX_DOMAIN_NUMBER]=(void*)&value_CONF_MAX_DOMAIN_NUMBER,
	[CONF_MAX_PORT_NUMBER]=(void*)&value_CONF_MAX_PORT_NUMBER,
	[CONF_PTP_PORT_ENABLED]=(void*)&value_CONF_PTP_PORT_ENABLED,
	[CONF_MASTER_PTPDEV]=(void*)&value_CONF_MASTER_PTPDEV,
	[CONF_TXTS_LOST_TIME]=(void*)&value_CONF_TXTS_LOST_TIME,
	[CONF_SOCKET_TXPRIORITY]=(void*)&value_CONF_SOCKET_TXPRIORITY,
	[CONF_FIRST_DOMAIN_THIS_CLOCK]=(void*)&value_CONF_FIRST_DOMAIN_THIS_CLOCK,
	[CONF_SECOND_DOMAIN_THIS_CLOCK]=(void*)&value_CONF_SECOND_DOMAIN_THIS_CLOCK,
	[CONF_SECOND_DOMAIN_NUMBER]=(void*)&value_CONF_SECOND_DOMAIN_NUMBER,
	[CONF_SINGLE_CLOCK_MODE]=(void*)&value_CONF_SINGLE_CLOCK_MODE,
	[CONF_AFTERSEND_GUARDTIME]=(void*)&value_CONF_AFTERSEND_GUARDTIME,
	[CONF_GPTPNET_EXTRA_TOUTNS]=(void*)&value_CONF_GPTPNET_EXTRA_TOUTNS,
	[CONF_MAX_ADJUST_RATE_ON_CLOCK]=(void*)&value_CONF_MAX_ADJUST_RATE_ON_CLOCK,
	[CONF_IPC_NOTICE_PHASE_UPDATE]=(void*)&value_CONF_IPC_NOTICE_PHASE_UPDATE,
	[CONF_IPC_NOTICE_FREQ_UPDATE]=(void*)&value_CONF_IPC_NOTICE_FREQ_UPDATE,
	[CONF_NEIGHBOR_PROP_DELAY]=(void*)&value_CONF_NEIGHBOR_PROP_DELAY,
	[CONF_STATIC_PORT_STATE_SLAVE_PORT]=(void*)&value_CONF_STATIC_PORT_STATE_SLAVE_PORT,
	[CONF_TIMESCALE_PTP]=(void*)&value_CONF_TIMESCALE_PTP,
	[CONF_FOLLOW_AVNU]=(void*)&value_CONF_FOLLOW_AVNU,
	[CONF_PRIMARY_PRIORITY1]=(void*)&value_CONF_PRIMARY_PRIORITY1,
	[CONF_PRIMARY_CLOCK_CLASS]=(void*)&value_CONF_PRIMARY_CLOCK_CLASS,
	[CONF_PRIMARY_CLOCK_ACCURACY]=(void*)&value_CONF_PRIMARY_CLOCK_ACCURACY,
	[CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE]=(void*)&value_CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE,
	[CONF_PRIMARY_PRIORITY2]=(void*)&value_CONF_PRIMARY_PRIORITY2,
	[CONF_SECONDARY_PRIORITY1]=(void*)&value_CONF_SECONDARY_PRIORITY1,
	[CONF_SECONDARY_CLOCK_CLASS]=(void*)&value_CONF_SECONDARY_CLOCK_CLASS,
	[CONF_SECONDARY_CLOCK_ACCURACY]=(void*)&value_CONF_SECONDARY_CLOCK_ACCURACY,
	[CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE]=(void*)&value_CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE,
	[CONF_SECONDARY_PRIORITY2]=(void*)&value_CONF_SECONDARY_PRIORITY2,
	[CONF_TIME_SOURCE]=(void*)&value_CONF_TIME_SOURCE,
	[CONF_BMCS_QUICK_UPDATE_MODE]=(void*)&value_CONF_BMCS_QUICK_UPDATE_MODE,
	[CONF_EXTERNAL_PORT_CONFIGURATION]=(void*)&value_CONF_EXTERNAL_PORT_CONFIGURATION,
	[CONF_INITIAL_LOG_ANNOUNCE_INTERVAL]=(void*)&value_CONF_INITIAL_LOG_ANNOUNCE_INTERVAL,
	[CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL]=(void*)&value_CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL,
	[CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT]=(void*)&value_CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT,
	[CONF_SYNC_RECEIPT_TIMEOUT]=(void*)&value_CONF_SYNC_RECEIPT_TIMEOUT,
	[CONF_ANNOUNCE_RECEIPT_TIMEOUT]=(void*)&value_CONF_ANNOUNCE_RECEIPT_TIMEOUT,
	[CONF_CMLDS_MODE]=(void*)&value_CONF_CMLDS_MODE,
	[CONF_MINOR_VERSION_PTP]=(void*)&value_CONF_MINOR_VERSION_PTP,
	[CONF_LOG_SYNC_INTERVAL]=(void*)&value_CONF_LOG_SYNC_INTERVAL,
	[CONF_LOG_ANNOUNCE_INTERVAL]=(void*)&value_CONF_LOG_ANNOUNCE_INTERVAL,
	[CONF_LOG_PDELAYREQ_INTERVAL]=(void*)&value_CONF_LOG_PDELAYREQ_INTERVAL,
	[CONF_LOG_DELAYREQ_INTERVAL]=(void*)&value_CONF_LOG_DELAYREQ_INTERVAL,
	[CONF_ALLOWED_LOST_RESPONSE]=(void*)&value_CONF_ALLOWED_LOST_RESPONSE,
	[CONF_ALLOWED_FAULTS]=(void*)&value_CONF_ALLOWED_FAULTS,
	[CONF_NEIGHBOR_PROPDELAY_THRESH]=(void*)&value_CONF_NEIGHBOR_PROPDELAY_THRESH,
	[CONF_NEIGHBOR_PROPDELAY_MINLIMIT]=(void*)&value_CONF_NEIGHBOR_PROPDELAY_MINLIMIT,
	[CONF_IPC_UDP_PORT]=(void*)&value_CONF_IPC_UDP_PORT,
	[CONF_INITIAL_SLAVE_TIME]=(void*)&value_CONF_INITIAL_SLAVE_TIME,
	[CONF_INITIAL_GM_STABLE_TIME]=(void*)&value_CONF_INITIAL_GM_STABLE_TIME,
	[CONF_NORMAL_GM_STABLE_TIME]=(void*)&value_CONF_NORMAL_GM_STABLE_TIME,
	[CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE]=(void*)&value_CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE,
	[CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE]=(void*)&value_CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE,
	[CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE]=(void*)&value_CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE,
	[CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE]=(void*)&value_CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE,
	[CONF_CLOCK_COMPUTE_INTERVAL_MSEC]=(void*)&value_CONF_CLOCK_COMPUTE_INTERVAL_MSEC,
	[CONF_FREQ_OFFSET_UPDATE_MRATE_PPB]=(void*)&value_CONF_FREQ_OFFSET_UPDATE_MRATE_PPB,
	[CONF_FREQ_OFFSET_STABLE_PPB]=(void*)&value_CONF_FREQ_OFFSET_STABLE_PPB,
	[CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB]=(void*)&value_CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB,
	[CONF_ACTIVE_DOMAIN_AUTO_SWITCH]=(void*)&value_CONF_ACTIVE_DOMAIN_AUTO_SWITCH,
	[CONF_TSN_SCHEDULE_ON]=(void*)&value_CONF_TSN_SCHEDULE_ON,
	[CONF_TSN_SCHEDULE_ALIGNTIME]=(void*)&value_CONF_TSN_SCHEDULE_ALIGNTIME,
	[CONF_TSN_SCHEDULE_CYCLETIME]=(void*)&value_CONF_TSN_SCHEDULE_CYCLETIME,
	[CONF_DEBUGLOG_MEMORY_FILE]=(void*)&value_CONF_DEBUGLOG_MEMORY_FILE,
	[CONF_DEBUGLOG_MEMORY_SIZE]=(void*)&value_CONF_DEBUGLOG_MEMORY_SIZE,
	[CONF_OVIP_MODE_STRT_PORTNO]=(void*)&value_CONF_OVIP_MODE_STRT_PORTNO,
	[CONF_MASTER_CLOCK_SHARED_MEM]=(void*)&value_CONF_MASTER_CLOCK_SHARED_MEM,
	[CONF_PTPVFD_CLOCK_RATE]=(void*)&value_CONF_PTPVFD_CLOCK_RATE,
	[CONF_MAX_CONSEC_TS_DIFF]=(void*)&value_CONF_MAX_CONSEC_TS_DIFF,
	[CONF_TS2DIFF_CACHE_FACTOR]=(void*)&value_CONF_TS2DIFF_CACHE_FACTOR,
	[CONF_USE_HW_PHASE_ADJUSTMENT]=(void*)&value_CONF_USE_HW_PHASE_ADJUSTMENT,
	[CONF_PHASE_ADJUSTMENT_BY_FREQ]=(void*)&value_CONF_PHASE_ADJUSTMENT_BY_FREQ,
	[CONF_ACTIVATE_ABNORMAL_HOOKS]=(void*)&value_CONF_ACTIVATE_ABNORMAL_HOOKS,
	[CONF_RESET_FREQADJ_BECOMEGM]=(void*)&value_CONF_RESET_FREQADJ_BECOMEGM,
	[CONF_INTEGRITY_CLOCK_INFO]=(void*)&value_CONF_INTEGRITY_CLOCK_INFO,
};

void *gptpconf_get_item(gptp_config_item_t item)
{
	if(item<0||item>=CONF_ENUM_LAST_ITEM) return NULL;
	return gptp_item_table[item];
}

int32_t gptpconf_get_intitem(gptp_config_item_t item)
{
	int32_t *ptr = (int32_t *)gptpconf_get_item(item);
	if (ptr){return *ptr;}else{return -1;}
}

int64_t gptpconf_get_lintitem(gptp_config_item_t item)
{
	int64_t *ptr = (int64_t *)gptpconf_get_item(item);
	if (ptr){return *ptr;} else {return -1;}
}

int gptpconf_set_item(gptp_config_item_t item, void *v)
{
	switch(item) {
	case CONF_TEST_SYNC_REC_PORT:
		value_CONF_TEST_SYNC_REC_PORT=*((int32_t*)v);
		break;
	case CONF_TEST_SYNC_SEND_PORT:
		value_CONF_TEST_SYNC_SEND_PORT=*((int32_t*)v);
		break;
	case CONF_MAX_DOMAIN_NUMBER:
		value_CONF_MAX_DOMAIN_NUMBER=*((int32_t*)v);
		break;
	case CONF_MAX_PORT_NUMBER:
		value_CONF_MAX_PORT_NUMBER=*((int32_t*)v);
		break;
	case CONF_PTP_PORT_ENABLED:
		if(sizeof(value_CONF_PTP_PORT_ENABLED)>0)
			strncpy(value_CONF_PTP_PORT_ENABLED, (char *)v, sizeof(value_CONF_PTP_PORT_ENABLED)-1);
		break;
	case CONF_MASTER_PTPDEV:
		if(sizeof(value_CONF_MASTER_PTPDEV)>0)
			strncpy(value_CONF_MASTER_PTPDEV, (char *)v, sizeof(value_CONF_MASTER_PTPDEV)-1);
		break;
	case CONF_TXTS_LOST_TIME:
		value_CONF_TXTS_LOST_TIME=*((int32_t*)v);
		break;
	case CONF_SOCKET_TXPRIORITY:
		value_CONF_SOCKET_TXPRIORITY=*((int32_t*)v);
		break;
	case CONF_FIRST_DOMAIN_THIS_CLOCK:
		value_CONF_FIRST_DOMAIN_THIS_CLOCK=*((int32_t*)v);
		break;
	case CONF_SECOND_DOMAIN_THIS_CLOCK:
		value_CONF_SECOND_DOMAIN_THIS_CLOCK=*((int32_t*)v);
		break;
	case CONF_SECOND_DOMAIN_NUMBER:
		value_CONF_SECOND_DOMAIN_NUMBER=*((int32_t*)v);
		break;
	case CONF_SINGLE_CLOCK_MODE:
		value_CONF_SINGLE_CLOCK_MODE=*((int32_t*)v);
		break;
	case CONF_AFTERSEND_GUARDTIME:
		value_CONF_AFTERSEND_GUARDTIME=*((int32_t*)v);
		break;
	case CONF_GPTPNET_EXTRA_TOUTNS:
		value_CONF_GPTPNET_EXTRA_TOUTNS=*((int32_t*)v);
		break;
	case CONF_MAX_ADJUST_RATE_ON_CLOCK:
		value_CONF_MAX_ADJUST_RATE_ON_CLOCK=*((int32_t*)v);
		break;
	case CONF_IPC_NOTICE_PHASE_UPDATE:
		value_CONF_IPC_NOTICE_PHASE_UPDATE=*((int32_t*)v);
		break;
	case CONF_IPC_NOTICE_FREQ_UPDATE:
		value_CONF_IPC_NOTICE_FREQ_UPDATE=*((int32_t*)v);
		break;
	case CONF_NEIGHBOR_PROP_DELAY:
		value_CONF_NEIGHBOR_PROP_DELAY=*((int32_t*)v);
		break;
	case CONF_STATIC_PORT_STATE_SLAVE_PORT:
		value_CONF_STATIC_PORT_STATE_SLAVE_PORT=*((int32_t*)v);
		break;
	case CONF_TIMESCALE_PTP:
		value_CONF_TIMESCALE_PTP=*((int32_t*)v);
		break;
	case CONF_FOLLOW_AVNU:
		value_CONF_FOLLOW_AVNU=*((int32_t*)v);
		break;
	case CONF_PRIMARY_PRIORITY1:
		value_CONF_PRIMARY_PRIORITY1=*((int32_t*)v);
		break;
	case CONF_PRIMARY_CLOCK_CLASS:
		value_CONF_PRIMARY_CLOCK_CLASS=*((int32_t*)v);
		break;
	case CONF_PRIMARY_CLOCK_ACCURACY:
		value_CONF_PRIMARY_CLOCK_ACCURACY=*((int32_t*)v);
		break;
	case CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE:
		value_CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE=*((int32_t*)v);
		break;
	case CONF_PRIMARY_PRIORITY2:
		value_CONF_PRIMARY_PRIORITY2=*((int32_t*)v);
		break;
	case CONF_SECONDARY_PRIORITY1:
		value_CONF_SECONDARY_PRIORITY1=*((int32_t*)v);
		break;
	case CONF_SECONDARY_CLOCK_CLASS:
		value_CONF_SECONDARY_CLOCK_CLASS=*((int32_t*)v);
		break;
	case CONF_SECONDARY_CLOCK_ACCURACY:
		value_CONF_SECONDARY_CLOCK_ACCURACY=*((int32_t*)v);
		break;
	case CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE:
		value_CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE=*((int32_t*)v);
		break;
	case CONF_SECONDARY_PRIORITY2:
		value_CONF_SECONDARY_PRIORITY2=*((int32_t*)v);
		break;
	case CONF_TIME_SOURCE:
		value_CONF_TIME_SOURCE=*((int32_t*)v);
		break;
	case CONF_BMCS_QUICK_UPDATE_MODE:
		value_CONF_BMCS_QUICK_UPDATE_MODE=*((int32_t*)v);
		break;
	case CONF_EXTERNAL_PORT_CONFIGURATION:
		value_CONF_EXTERNAL_PORT_CONFIGURATION=*((int32_t*)v);
		break;
	case CONF_INITIAL_LOG_ANNOUNCE_INTERVAL:
		value_CONF_INITIAL_LOG_ANNOUNCE_INTERVAL=*((int32_t*)v);
		break;
	case CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL:
		value_CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL=*((int32_t*)v);
		break;
	case CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT:
		value_CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT=*((int32_t*)v);
		break;
	case CONF_SYNC_RECEIPT_TIMEOUT:
		value_CONF_SYNC_RECEIPT_TIMEOUT=*((int32_t*)v);
		break;
	case CONF_ANNOUNCE_RECEIPT_TIMEOUT:
		value_CONF_ANNOUNCE_RECEIPT_TIMEOUT=*((int32_t*)v);
		break;
	case CONF_CMLDS_MODE:
		value_CONF_CMLDS_MODE=*((int32_t*)v);
		break;
	case CONF_MINOR_VERSION_PTP:
		value_CONF_MINOR_VERSION_PTP=*((int32_t*)v);
		break;
	case CONF_LOG_SYNC_INTERVAL:
		value_CONF_LOG_SYNC_INTERVAL=*((int32_t*)v);
		break;
	case CONF_LOG_ANNOUNCE_INTERVAL:
		value_CONF_LOG_ANNOUNCE_INTERVAL=*((int32_t*)v);
		break;
	case CONF_LOG_PDELAYREQ_INTERVAL:
		value_CONF_LOG_PDELAYREQ_INTERVAL=*((int32_t*)v);
		break;
	case CONF_LOG_DELAYREQ_INTERVAL:
		value_CONF_LOG_DELAYREQ_INTERVAL=*((int32_t*)v);
		break;
	case CONF_ALLOWED_LOST_RESPONSE:
		value_CONF_ALLOWED_LOST_RESPONSE=*((int32_t*)v);
		break;
	case CONF_ALLOWED_FAULTS:
		value_CONF_ALLOWED_FAULTS=*((int32_t*)v);
		break;
	case CONF_NEIGHBOR_PROPDELAY_THRESH:
		value_CONF_NEIGHBOR_PROPDELAY_THRESH=*((int32_t*)v);
		break;
	case CONF_NEIGHBOR_PROPDELAY_MINLIMIT:
		value_CONF_NEIGHBOR_PROPDELAY_MINLIMIT=*((int32_t*)v);
		break;
	case CONF_IPC_UDP_PORT:
		value_CONF_IPC_UDP_PORT=*((int32_t*)v);
		break;
	case CONF_INITIAL_SLAVE_TIME:
		value_CONF_INITIAL_SLAVE_TIME=*((int32_t*)v);
		break;
	case CONF_INITIAL_GM_STABLE_TIME:
		value_CONF_INITIAL_GM_STABLE_TIME=*((int32_t*)v);
		break;
	case CONF_NORMAL_GM_STABLE_TIME:
		value_CONF_NORMAL_GM_STABLE_TIME=*((int32_t*)v);
		break;
	case CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE:
		value_CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE=*((int32_t*)v);
		break;
	case CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE:
		value_CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE=*((int32_t*)v);
		break;
	case CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE:
		value_CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE=*((int32_t*)v);
		break;
	case CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE:
		value_CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE=*((int32_t*)v);
		break;
	case CONF_CLOCK_COMPUTE_INTERVAL_MSEC:
		value_CONF_CLOCK_COMPUTE_INTERVAL_MSEC=*((int32_t*)v);
		break;
	case CONF_FREQ_OFFSET_UPDATE_MRATE_PPB:
		value_CONF_FREQ_OFFSET_UPDATE_MRATE_PPB=*((int32_t*)v);
		break;
	case CONF_FREQ_OFFSET_STABLE_PPB:
		value_CONF_FREQ_OFFSET_STABLE_PPB=*((int32_t*)v);
		break;
	case CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB:
		value_CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB=*((int32_t*)v);
		break;
	case CONF_ACTIVE_DOMAIN_AUTO_SWITCH:
		value_CONF_ACTIVE_DOMAIN_AUTO_SWITCH=*((int32_t*)v);
		break;
	case CONF_TSN_SCHEDULE_ON:
		value_CONF_TSN_SCHEDULE_ON=*((int32_t*)v);
		break;
	case CONF_TSN_SCHEDULE_ALIGNTIME:
		value_CONF_TSN_SCHEDULE_ALIGNTIME=*((int32_t*)v);
		break;
	case CONF_TSN_SCHEDULE_CYCLETIME:
		value_CONF_TSN_SCHEDULE_CYCLETIME=*((int32_t*)v);
		break;
	case CONF_DEBUGLOG_MEMORY_FILE:
		if(sizeof(value_CONF_DEBUGLOG_MEMORY_FILE)>0)
			strncpy(value_CONF_DEBUGLOG_MEMORY_FILE, (char *)v, sizeof(value_CONF_DEBUGLOG_MEMORY_FILE)-1);
		break;
	case CONF_DEBUGLOG_MEMORY_SIZE:
		value_CONF_DEBUGLOG_MEMORY_SIZE=*((int32_t*)v);
		break;
	case CONF_OVIP_MODE_STRT_PORTNO:
		value_CONF_OVIP_MODE_STRT_PORTNO=*((int32_t*)v);
		break;
	case CONF_MASTER_CLOCK_SHARED_MEM:
		if(sizeof(value_CONF_MASTER_CLOCK_SHARED_MEM)>0)
			strncpy(value_CONF_MASTER_CLOCK_SHARED_MEM, (char *)v, sizeof(value_CONF_MASTER_CLOCK_SHARED_MEM)-1);
		break;
	case CONF_PTPVFD_CLOCK_RATE:
		value_CONF_PTPVFD_CLOCK_RATE=*((int32_t*)v);
		break;
	case CONF_MAX_CONSEC_TS_DIFF:
		value_CONF_MAX_CONSEC_TS_DIFF=*((int32_t*)v);
		break;
	case CONF_TS2DIFF_CACHE_FACTOR:
		value_CONF_TS2DIFF_CACHE_FACTOR=*((int32_t*)v);
		break;
	case CONF_USE_HW_PHASE_ADJUSTMENT:
		value_CONF_USE_HW_PHASE_ADJUSTMENT=*((int32_t*)v);
		break;
	case CONF_PHASE_ADJUSTMENT_BY_FREQ:
		value_CONF_PHASE_ADJUSTMENT_BY_FREQ=*((int32_t*)v);
		break;
	case CONF_ACTIVATE_ABNORMAL_HOOKS:
		value_CONF_ACTIVATE_ABNORMAL_HOOKS=*((int32_t*)v);
		break;
	case CONF_RESET_FREQADJ_BECOMEGM:
		value_CONF_RESET_FREQADJ_BECOMEGM=*((int32_t*)v);
		break;
	case CONF_INTEGRITY_CLOCK_INFO:
		if(sizeof(value_CONF_INTEGRITY_CLOCK_INFO)>0)
			strncpy(value_CONF_INTEGRITY_CLOCK_INFO, (char *)v, sizeof(value_CONF_INTEGRITY_CLOCK_INFO)-1);
		break;
	default:
		return -1;
	}
	return 0;
}

gptp_config_item_t gptpconf_get_item_num(char *istr)
{
	int i;
	for(i=0;i<CONF_ENUM_LAST_ITEM;i++)
		if(!strcmp(istr, config_item_strings[i])) return (gptp_config_item_t)i;
	return CONF_ENUM_LAST_ITEM;
}

int gptpconf_set_stritem(char *istr, void *v)
{
	gptp_config_item_t item;
	item=gptpconf_get_item_num(istr);
	return gptpconf_set_item(item, v);
}

