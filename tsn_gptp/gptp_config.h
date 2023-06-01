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
#ifndef __GPTP_CONFING_H_
#define __GPTP_CONFING_H_
#include <inttypes.h>
#include "gptpbasetypes.h"
typedef enum {
	CONF_TEST_SYNC_REC_PORT,
	CONF_TEST_SYNC_SEND_PORT,
	CONF_MAX_DOMAIN_NUMBER,
	CONF_MAX_PORT_NUMBER,
	CONF_PTP_PORT_ENABLED,
	CONF_MASTER_PTPDEV,
	CONF_TXTS_LOST_TIME,
	CONF_SOCKET_TXPRIORITY,
	CONF_FIRST_DOMAIN_THIS_CLOCK,
	CONF_SECOND_DOMAIN_THIS_CLOCK,
	CONF_SECOND_DOMAIN_NUMBER,
	CONF_SINGLE_CLOCK_MODE,
	CONF_AFTERSEND_GUARDTIME,
	CONF_GPTPNET_EXTRA_TOUTNS,
	CONF_MAX_ADJUST_RATE_ON_CLOCK,
	CONF_IPC_NOTICE_PHASE_UPDATE,
	CONF_IPC_NOTICE_FREQ_UPDATE,
	CONF_NEIGHBOR_PROP_DELAY,
	CONF_STATIC_PORT_STATE_SLAVE_PORT,
	CONF_TIMESCALE_PTP,
	CONF_FOLLOW_AVNU,
	CONF_PRIMARY_PRIORITY1,
	CONF_PRIMARY_CLOCK_CLASS,
	CONF_PRIMARY_CLOCK_ACCURACY,
	CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE,
	CONF_PRIMARY_PRIORITY2,
	CONF_SECONDARY_PRIORITY1,
	CONF_SECONDARY_CLOCK_CLASS,
	CONF_SECONDARY_CLOCK_ACCURACY,
	CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE,
	CONF_SECONDARY_PRIORITY2,
	CONF_TIME_SOURCE,
	CONF_BMCS_QUICK_UPDATE_MODE,
	CONF_EXTERNAL_PORT_CONFIGURATION,
	CONF_INITIAL_LOG_ANNOUNCE_INTERVAL,
	CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL,
	CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT,
	CONF_SYNC_RECEIPT_TIMEOUT,
	CONF_ANNOUNCE_RECEIPT_TIMEOUT,
	CONF_CMLDS_MODE,
	CONF_MINOR_VERSION_PTP,
	CONF_LOG_SYNC_INTERVAL,
	CONF_LOG_ANNOUNCE_INTERVAL,
	CONF_LOG_PDELAYREQ_INTERVAL,
	CONF_LOG_DELAYREQ_INTERVAL,
	CONF_ALLOWED_LOST_RESPONSE,
	CONF_ALLOWED_FAULTS,
	CONF_NEIGHBOR_PROPDELAY_THRESH,
	CONF_NEIGHBOR_PROPDELAY_MINLIMIT,
	CONF_IPC_UDP_PORT,
	CONF_INITIAL_SLAVE_TIME,
	CONF_INITIAL_GM_STABLE_TIME,
	CONF_NORMAL_GM_STABLE_TIME,
	CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE,
	CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE,
	CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE,
	CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE,
	CONF_CLOCK_COMPUTE_INTERVAL_MSEC,
	CONF_FREQ_OFFSET_UPDATE_MRATE_PPB,
	CONF_FREQ_OFFSET_STABLE_PPB,
	CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB,
	CONF_ACTIVE_DOMAIN_AUTO_SWITCH,
	CONF_TSN_SCHEDULE_ON,
	CONF_TSN_SCHEDULE_ALIGNTIME,
	CONF_TSN_SCHEDULE_CYCLETIME,
	CONF_DEBUGLOG_MEMORY_FILE,
	CONF_DEBUGLOG_MEMORY_SIZE,
	CONF_OVIP_MODE_STRT_PORTNO,
	CONF_MASTER_CLOCK_SHARED_MEM,
	CONF_PTPVFD_CLOCK_RATE,
	CONF_MAX_CONSEC_TS_DIFF,
	CONF_TS2DIFF_CACHE_FACTOR,
	CONF_USE_HW_PHASE_ADJUSTMENT,
	CONF_PHASE_ADJUSTMENT_BY_FREQ,
	CONF_ACTIVATE_ABNORMAL_HOOKS,
	CONF_RESET_FREQADJ_BECOMEGM,
	CONF_INTEGRITY_CLOCK_INFO,
	CONF_ENUM_LAST_ITEM
} gptp_config_item_t;

void *gptpconf_get_item(gptp_config_item_t item);
int32_t gptpconf_get_intitem(gptp_config_item_t item);
int64_t gptpconf_get_lintitem(gptp_config_item_t item);
int gptpconf_set_item(gptp_config_item_t item, void *v);
gptp_config_item_t gptpconf_get_item_num(char *istr);
int gptpconf_set_stritem(char *istr, void *v);

#endif
