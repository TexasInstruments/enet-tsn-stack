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
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "gptp_defaults.h"
#include "gptp_config.h"

int gptpconf_values_test(void)
{
	int res=0;
	if(gptpconf_get_intitem(CONF_TEST_SYNC_REC_PORT) != DEFAULT_TEST_SYNC_REC_PORT){
		printf("TEST_SYNC_REC_PORT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TEST_SYNC_REC_PORT,
			gptpconf_get_intitem(CONF_TEST_SYNC_REC_PORT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_TEST_SYNC_SEND_PORT) != DEFAULT_TEST_SYNC_SEND_PORT){
		printf("TEST_SYNC_SEND_PORT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TEST_SYNC_SEND_PORT,
			gptpconf_get_intitem(CONF_TEST_SYNC_SEND_PORT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_MAX_DOMAIN_NUMBER) != DEFAULT_MAX_DOMAIN_NUMBER){
		printf("MAX_DOMAIN_NUMBER default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_MAX_DOMAIN_NUMBER,
			gptpconf_get_intitem(CONF_MAX_DOMAIN_NUMBER));
		res++;
	}
	if(gptpconf_get_intitem(CONF_MAX_PORT_NUMBER) != DEFAULT_MAX_PORT_NUMBER){
		printf("MAX_PORT_NUMBER default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_MAX_PORT_NUMBER,
			gptpconf_get_intitem(CONF_MAX_PORT_NUMBER));
		res++;
	}
	if(strcmp((char*)gptpconf_get_item(CONF_PTP_PORT_ENABLED), DEFAULT_PTP_PORT_ENABLED)){
		printf("PTP_PORT_ENABLED default=%s, configured=%s\n",
			DEFAULT_PTP_PORT_ENABLED,
			(char*)gptpconf_get_item(CONF_PTP_PORT_ENABLED));
		res++;
	}
	if(strcmp((char*)gptpconf_get_item(CONF_MASTER_PTPDEV), DEFAULT_MASTER_PTPDEV)){
		printf("MASTER_PTPDEV default=%s, configured=%s\n",
			DEFAULT_MASTER_PTPDEV,
			(char*)gptpconf_get_item(CONF_MASTER_PTPDEV));
		res++;
	}
	if(gptpconf_get_intitem(CONF_TXTS_LOST_TIME) != DEFAULT_TXTS_LOST_TIME){
		printf("TXTS_LOST_TIME default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TXTS_LOST_TIME,
			gptpconf_get_intitem(CONF_TXTS_LOST_TIME));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SOCKET_TXPRIORITY) != DEFAULT_SOCKET_TXPRIORITY){
		printf("SOCKET_TXPRIORITY default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SOCKET_TXPRIORITY,
			gptpconf_get_intitem(CONF_SOCKET_TXPRIORITY));
		res++;
	}
	if(gptpconf_get_intitem(CONF_FIRST_DOMAIN_THIS_CLOCK) != DEFAULT_FIRST_DOMAIN_THIS_CLOCK){
		printf("FIRST_DOMAIN_THIS_CLOCK default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_FIRST_DOMAIN_THIS_CLOCK,
			gptpconf_get_intitem(CONF_FIRST_DOMAIN_THIS_CLOCK));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SECOND_DOMAIN_THIS_CLOCK) != DEFAULT_SECOND_DOMAIN_THIS_CLOCK){
		printf("SECOND_DOMAIN_THIS_CLOCK default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SECOND_DOMAIN_THIS_CLOCK,
			gptpconf_get_intitem(CONF_SECOND_DOMAIN_THIS_CLOCK));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SECOND_DOMAIN_NUMBER) != DEFAULT_SECOND_DOMAIN_NUMBER){
		printf("SECOND_DOMAIN_NUMBER default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SECOND_DOMAIN_NUMBER,
			gptpconf_get_intitem(CONF_SECOND_DOMAIN_NUMBER));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SINGLE_CLOCK_MODE) != DEFAULT_SINGLE_CLOCK_MODE){
		printf("SINGLE_CLOCK_MODE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SINGLE_CLOCK_MODE,
			gptpconf_get_intitem(CONF_SINGLE_CLOCK_MODE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_AFTERSEND_GUARDTIME) != DEFAULT_AFTERSEND_GUARDTIME){
		printf("AFTERSEND_GUARDTIME default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_AFTERSEND_GUARDTIME,
			gptpconf_get_intitem(CONF_AFTERSEND_GUARDTIME));
		res++;
	}
	if(gptpconf_get_intitem(CONF_GPTPNET_EXTRA_TOUTNS) != DEFAULT_GPTPNET_EXTRA_TOUTNS){
		printf("GPTPNET_EXTRA_TOUTNS default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_GPTPNET_EXTRA_TOUTNS,
			gptpconf_get_intitem(CONF_GPTPNET_EXTRA_TOUTNS));
		res++;
	}
	if(gptpconf_get_intitem(CONF_MAX_ADJUST_RATE_ON_CLOCK) != DEFAULT_MAX_ADJUST_RATE_ON_CLOCK){
		printf("MAX_ADJUST_RATE_ON_CLOCK default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_MAX_ADJUST_RATE_ON_CLOCK,
			gptpconf_get_intitem(CONF_MAX_ADJUST_RATE_ON_CLOCK));
		res++;
	}
	if(gptpconf_get_intitem(CONF_IPC_NOTICE_PHASE_UPDATE) != DEFAULT_IPC_NOTICE_PHASE_UPDATE){
		printf("IPC_NOTICE_PHASE_UPDATE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_IPC_NOTICE_PHASE_UPDATE,
			gptpconf_get_intitem(CONF_IPC_NOTICE_PHASE_UPDATE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_IPC_NOTICE_FREQ_UPDATE) != DEFAULT_IPC_NOTICE_FREQ_UPDATE){
		printf("IPC_NOTICE_FREQ_UPDATE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_IPC_NOTICE_FREQ_UPDATE,
			gptpconf_get_intitem(CONF_IPC_NOTICE_FREQ_UPDATE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_NEIGHBOR_PROP_DELAY) != DEFAULT_NEIGHBOR_PROP_DELAY){
		printf("NEIGHBOR_PROP_DELAY default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_NEIGHBOR_PROP_DELAY,
			gptpconf_get_intitem(CONF_NEIGHBOR_PROP_DELAY));
		res++;
	}
	if(gptpconf_get_intitem(CONF_STATIC_PORT_STATE_SLAVE_PORT) != DEFAULT_STATIC_PORT_STATE_SLAVE_PORT){
		printf("STATIC_PORT_STATE_SLAVE_PORT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_STATIC_PORT_STATE_SLAVE_PORT,
			gptpconf_get_intitem(CONF_STATIC_PORT_STATE_SLAVE_PORT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_TIMESCALE_PTP) != DEFAULT_TIMESCALE_PTP){
		printf("TIMESCALE_PTP default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TIMESCALE_PTP,
			gptpconf_get_intitem(CONF_TIMESCALE_PTP));
		res++;
	}
	if(gptpconf_get_intitem(CONF_FOLLOW_AVNU) != DEFAULT_FOLLOW_AVNU){
		printf("FOLLOW_AVNU default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_FOLLOW_AVNU,
			gptpconf_get_intitem(CONF_FOLLOW_AVNU));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PRIMARY_PRIORITY1) != DEFAULT_PRIMARY_PRIORITY1){
		printf("PRIMARY_PRIORITY1 default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PRIMARY_PRIORITY1,
			gptpconf_get_intitem(CONF_PRIMARY_PRIORITY1));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PRIMARY_CLOCK_CLASS) != DEFAULT_PRIMARY_CLOCK_CLASS){
		printf("PRIMARY_CLOCK_CLASS default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PRIMARY_CLOCK_CLASS,
			gptpconf_get_intitem(CONF_PRIMARY_CLOCK_CLASS));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PRIMARY_CLOCK_ACCURACY) != DEFAULT_PRIMARY_CLOCK_ACCURACY){
		printf("PRIMARY_CLOCK_ACCURACY default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PRIMARY_CLOCK_ACCURACY,
			gptpconf_get_intitem(CONF_PRIMARY_CLOCK_ACCURACY));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE) != DEFAULT_PRIMARY_OFFSET_SCALED_LOG_VARIANCE){
		printf("PRIMARY_OFFSET_SCALED_LOG_VARIANCE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PRIMARY_OFFSET_SCALED_LOG_VARIANCE,
			gptpconf_get_intitem(CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PRIMARY_PRIORITY2) != DEFAULT_PRIMARY_PRIORITY2){
		printf("PRIMARY_PRIORITY2 default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PRIMARY_PRIORITY2,
			gptpconf_get_intitem(CONF_PRIMARY_PRIORITY2));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SECONDARY_PRIORITY1) != DEFAULT_SECONDARY_PRIORITY1){
		printf("SECONDARY_PRIORITY1 default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SECONDARY_PRIORITY1,
			gptpconf_get_intitem(CONF_SECONDARY_PRIORITY1));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SECONDARY_CLOCK_CLASS) != DEFAULT_SECONDARY_CLOCK_CLASS){
		printf("SECONDARY_CLOCK_CLASS default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SECONDARY_CLOCK_CLASS,
			gptpconf_get_intitem(CONF_SECONDARY_CLOCK_CLASS));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SECONDARY_CLOCK_ACCURACY) != DEFAULT_SECONDARY_CLOCK_ACCURACY){
		printf("SECONDARY_CLOCK_ACCURACY default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SECONDARY_CLOCK_ACCURACY,
			gptpconf_get_intitem(CONF_SECONDARY_CLOCK_ACCURACY));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE) != DEFAULT_SECONDARY_OFFSET_SCALED_LOG_VARIANCE){
		printf("SECONDARY_OFFSET_SCALED_LOG_VARIANCE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SECONDARY_OFFSET_SCALED_LOG_VARIANCE,
			gptpconf_get_intitem(CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SECONDARY_PRIORITY2) != DEFAULT_SECONDARY_PRIORITY2){
		printf("SECONDARY_PRIORITY2 default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SECONDARY_PRIORITY2,
			gptpconf_get_intitem(CONF_SECONDARY_PRIORITY2));
		res++;
	}

	if(gptpconf_get_intitem(CONF_BMCS_QUICK_UPDATE_MODE) != DEFAULT_BMCS_QUICK_UPDATE_MODE){
		printf("BMCS_QUICK_UPDATE_MODE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_BMCS_QUICK_UPDATE_MODE,
			gptpconf_get_intitem(CONF_BMCS_QUICK_UPDATE_MODE));
		res++;
	}

	if(gptpconf_get_intitem(CONF_INITIAL_LOG_ANNOUNCE_INTERVAL) != DEFAULT_INITIAL_LOG_ANNOUNCE_INTERVAL){
		printf("INITIAL_LOG_ANNOUNCE_INTERVAL default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_INITIAL_LOG_ANNOUNCE_INTERVAL,
			gptpconf_get_intitem(CONF_INITIAL_LOG_ANNOUNCE_INTERVAL));
		res++;
	}
	if(gptpconf_get_intitem(CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL) != DEFAULT_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL){
		printf("LOG_GPTP_CAPABLE_MESSAGE_INTERVAL default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL,
			gptpconf_get_intitem(CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL));
		res++;
	}
	if(gptpconf_get_intitem(CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT) != DEFAULT_GPTP_CAPABLE_RECEIPT_TIMEOUT){
		printf("GPTP_CAPABLE_RECEIPT_TIMEOUT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_GPTP_CAPABLE_RECEIPT_TIMEOUT,
			gptpconf_get_intitem(CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_SYNC_RECEIPT_TIMEOUT) != DEFAULT_SYNC_RECEIPT_TIMEOUT){
		printf("SYNC_RECEIPT_TIMEOUT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_SYNC_RECEIPT_TIMEOUT,
			gptpconf_get_intitem(CONF_SYNC_RECEIPT_TIMEOUT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_ANNOUNCE_RECEIPT_TIMEOUT) != DEFAULT_ANNOUNCE_RECEIPT_TIMEOUT){
		printf("ANNOUNCE_RECEIPT_TIMEOUT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_ANNOUNCE_RECEIPT_TIMEOUT,
			gptpconf_get_intitem(CONF_ANNOUNCE_RECEIPT_TIMEOUT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_CMLDS_MODE) != DEFAULT_CMLDS_MODE){
		printf("CMLDS_MODE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_CMLDS_MODE,
			gptpconf_get_intitem(CONF_CMLDS_MODE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_MINOR_VERSION_PTP) != DEFAULT_MINOR_VERSION_PTP){
		printf("MINOR_VERSION_PTP default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_MINOR_VERSION_PTP,
			gptpconf_get_intitem(CONF_MINOR_VERSION_PTP));
		res++;
	}
	if(gptpconf_get_intitem(CONF_LOG_SYNC_INTERVAL) != DEFAULT_LOG_SYNC_INTERVAL){
		printf("LOG_SYNC_INTERVAL default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_LOG_SYNC_INTERVAL,
			gptpconf_get_intitem(CONF_LOG_SYNC_INTERVAL));
		res++;
	}
	if(gptpconf_get_intitem(CONF_LOG_ANNOUNCE_INTERVAL) != DEFAULT_LOG_ANNOUNCE_INTERVAL){
		printf("LOG_ANNOUNCE_INTERVAL default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_LOG_ANNOUNCE_INTERVAL,
			gptpconf_get_intitem(CONF_LOG_ANNOUNCE_INTERVAL));
		res++;
	}
	if(gptpconf_get_intitem(CONF_LOG_PDELAYREQ_INTERVAL) != DEFAULT_LOG_PDELAYREQ_INTERVAL){
		printf("LOG_PDELAYREQ_INTERVAL default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_LOG_PDELAYREQ_INTERVAL,
			gptpconf_get_intitem(CONF_LOG_PDELAYREQ_INTERVAL));
		res++;
	}
	if(gptpconf_get_intitem(CONF_LOG_DELAYREQ_INTERVAL) != DEFAULT_LOG_DELAYREQ_INTERVAL){
		printf("LOG_DELAYREQ_INTERVAL default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_LOG_DELAYREQ_INTERVAL,
			gptpconf_get_intitem(CONF_LOG_DELAYREQ_INTERVAL));
		res++;
	}
	if(gptpconf_get_intitem(CONF_ALLOWED_LOST_RESPONSE) != DEFAULT_ALLOWED_LOST_RESPONSE){
		printf("ALLOWED_LOST_RESPONSE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_ALLOWED_LOST_RESPONSE,
			gptpconf_get_intitem(CONF_ALLOWED_LOST_RESPONSE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_ALLOWED_FAULTS) != DEFAULT_ALLOWED_FAULTS){
		printf("ALLOWED_FAULTS default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_ALLOWED_FAULTS,
			gptpconf_get_intitem(CONF_ALLOWED_FAULTS));
		res++;
	}
	if(gptpconf_get_intitem(CONF_NEIGHBOR_PROPDELAY_THRESH) != DEFAULT_NEIGHBOR_PROPDELAY_THRESH){
		printf("NEIGHBOR_PROPDELAY_THRESH default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_NEIGHBOR_PROPDELAY_THRESH,
			gptpconf_get_intitem(CONF_NEIGHBOR_PROPDELAY_THRESH));
		res++;
	}
	if(gptpconf_get_intitem(CONF_NEIGHBOR_PROPDELAY_MINLIMIT) != DEFAULT_NEIGHBOR_PROPDELAY_MINLIMIT){
		printf("NEIGHBOR_PROPDELAY_MINLIMIT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_NEIGHBOR_PROPDELAY_MINLIMIT,
			gptpconf_get_intitem(CONF_NEIGHBOR_PROPDELAY_MINLIMIT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_IPC_UDP_PORT) != DEFAULT_IPC_UDP_PORT){
		printf("IPC_UDP_PORT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_IPC_UDP_PORT,
			gptpconf_get_intitem(CONF_IPC_UDP_PORT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_INITIAL_SLAVE_TIME) != DEFAULT_INITIAL_SLAVE_TIME){
		printf("INITIAL_SLAVE_TIME default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_INITIAL_SLAVE_TIME,
			gptpconf_get_intitem(CONF_INITIAL_SLAVE_TIME));
		res++;
	}
	if(gptpconf_get_intitem(CONF_INITIAL_GM_STABLE_TIME) != DEFAULT_INITIAL_GM_STABLE_TIME){
		printf("INITIAL_GM_STABLE_TIME default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_INITIAL_GM_STABLE_TIME,
			gptpconf_get_intitem(CONF_INITIAL_GM_STABLE_TIME));
		res++;
	}
	if(gptpconf_get_intitem(CONF_NORMAL_GM_STABLE_TIME) != DEFAULT_NORMAL_GM_STABLE_TIME){
		printf("NORMAL_GM_STABLE_TIME default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_NORMAL_GM_STABLE_TIME,
			gptpconf_get_intitem(CONF_NORMAL_GM_STABLE_TIME));
		res++;
	}
	if(gptpconf_get_intitem(CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE) != DEFAULT_FREQ_OFFSET_IIR_ALPHA_START_VALUE){
		printf("FREQ_OFFSET_IIR_ALPHA_START_VALUE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_FREQ_OFFSET_IIR_ALPHA_START_VALUE,
			gptpconf_get_intitem(CONF_FREQ_OFFSET_IIR_ALPHA_START_VALUE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE) != DEFAULT_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE){
		printf("FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE,
			gptpconf_get_intitem(CONF_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE) != DEFAULT_PHASE_OFFSET_IIR_ALPHA_START_VALUE){
		printf("PHASE_OFFSET_IIR_ALPHA_START_VALUE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PHASE_OFFSET_IIR_ALPHA_START_VALUE,
			gptpconf_get_intitem(CONF_PHASE_OFFSET_IIR_ALPHA_START_VALUE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE) != DEFAULT_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE){
		printf("PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE,
			gptpconf_get_intitem(CONF_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_CLOCK_COMPUTE_INTERVAL_MSEC) != DEFAULT_CLOCK_COMPUTE_INTERVAL_MSEC){
		printf("CLOCK_COMPUTE_INTERVAL_MSEC default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_CLOCK_COMPUTE_INTERVAL_MSEC,
			gptpconf_get_intitem(CONF_CLOCK_COMPUTE_INTERVAL_MSEC));
		res++;
	}
	if(gptpconf_get_intitem(CONF_FREQ_OFFSET_UPDATE_MRATE_PPB) != DEFAULT_FREQ_OFFSET_UPDATE_MRATE_PPB){
		printf("FREQ_OFFSET_UPDATE_MRATE_PPB default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_FREQ_OFFSET_UPDATE_MRATE_PPB,
			gptpconf_get_intitem(CONF_FREQ_OFFSET_UPDATE_MRATE_PPB));
		res++;
	}
	if(gptpconf_get_intitem(CONF_FREQ_OFFSET_STABLE_PPB) != DEFAULT_FREQ_OFFSET_STABLE_PPB){
		printf("FREQ_OFFSET_STABLE_PPB default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_FREQ_OFFSET_STABLE_PPB,
			gptpconf_get_intitem(CONF_FREQ_OFFSET_STABLE_PPB));
		res++;
	}
	if(gptpconf_get_intitem(CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB) != DEFAULT_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB){
		printf("FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB,
			gptpconf_get_intitem(CONF_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB));
		res++;
	}
	if(gptpconf_get_intitem(CONF_ACTIVE_DOMAIN_AUTO_SWITCH) != DEFAULT_ACTIVE_DOMAIN_AUTO_SWITCH){
		printf("ACTIVE_DOMAIN_AUTO_SWITCH default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_ACTIVE_DOMAIN_AUTO_SWITCH,
			gptpconf_get_intitem(CONF_ACTIVE_DOMAIN_AUTO_SWITCH));
		res++;
	}
	if(gptpconf_get_intitem(CONF_TSN_SCHEDULE_ON) != DEFAULT_TSN_SCHEDULE_ON){
		printf("TSN_SCHEDULE_ON default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TSN_SCHEDULE_ON,
			gptpconf_get_intitem(CONF_TSN_SCHEDULE_ON));
		res++;
	}
	if(gptpconf_get_intitem(CONF_TSN_SCHEDULE_ALIGNTIME) != DEFAULT_TSN_SCHEDULE_ALIGNTIME){
		printf("TSN_SCHEDULE_ALIGNTIME default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TSN_SCHEDULE_ALIGNTIME,
			gptpconf_get_intitem(CONF_TSN_SCHEDULE_ALIGNTIME));
		res++;
	}
	if(gptpconf_get_intitem(CONF_TSN_SCHEDULE_CYCLETIME) != DEFAULT_TSN_SCHEDULE_CYCLETIME){
		printf("TSN_SCHEDULE_CYCLETIME default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TSN_SCHEDULE_CYCLETIME,
			gptpconf_get_intitem(CONF_TSN_SCHEDULE_CYCLETIME));
		res++;
	}
	if(strcmp((char*)gptpconf_get_item(CONF_DEBUGLOG_MEMORY_FILE), DEFAULT_DEBUGLOG_MEMORY_FILE)){
		printf("DEBUGLOG_MEMORY_FILE default=%s, configured=%s\n",
			DEFAULT_DEBUGLOG_MEMORY_FILE,
			(char*)gptpconf_get_item(CONF_DEBUGLOG_MEMORY_FILE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_DEBUGLOG_MEMORY_SIZE) != DEFAULT_DEBUGLOG_MEMORY_SIZE){
		printf("DEBUGLOG_MEMORY_SIZE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_DEBUGLOG_MEMORY_SIZE,
			gptpconf_get_intitem(CONF_DEBUGLOG_MEMORY_SIZE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_OVIP_MODE_STRT_PORTNO) != DEFAULT_OVIP_MODE_STRT_PORTNO){
		printf("OVIP_MODE_STRT_PORTNO default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_OVIP_MODE_STRT_PORTNO,
			gptpconf_get_intitem(CONF_OVIP_MODE_STRT_PORTNO));
		res++;
	}
	if(strcmp((char*)gptpconf_get_item(CONF_MASTER_CLOCK_SHARED_MEM), DEFAULT_MASTER_CLOCK_SHARED_MEM)){
		printf("MASTER_CLOCK_SHARED_MEM default=%s, configured=%s\n",
			DEFAULT_MASTER_CLOCK_SHARED_MEM,
			(char*)gptpconf_get_item(CONF_MASTER_CLOCK_SHARED_MEM));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PTPVFD_CLOCK_RATE) != DEFAULT_PTPVFD_CLOCK_RATE){
		printf("PTPVFD_CLOCK_RATE default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PTPVFD_CLOCK_RATE,
			gptpconf_get_intitem(CONF_PTPVFD_CLOCK_RATE));
		res++;
	}
	if(gptpconf_get_intitem(CONF_MAX_CONSEC_TS_DIFF) != DEFAULT_MAX_CONSEC_TS_DIFF){
		printf("MAX_CONSEC_TS_DIFF default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_MAX_CONSEC_TS_DIFF,
			gptpconf_get_intitem(CONF_MAX_CONSEC_TS_DIFF));
		res++;
	}
	if(gptpconf_get_intitem(CONF_TS2DIFF_CACHE_FACTOR) != DEFAULT_TS2DIFF_CACHE_FACTOR){
		printf("TS2DIFF_CACHE_FACTOR default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_TS2DIFF_CACHE_FACTOR,
			gptpconf_get_intitem(CONF_TS2DIFF_CACHE_FACTOR));
		res++;
	}
	if(gptpconf_get_intitem(CONF_USE_HW_PHASE_ADJUSTMENT) != DEFAULT_USE_HW_PHASE_ADJUSTMENT){
		printf("USE_HW_PHASE_ADJUSTMENT default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_USE_HW_PHASE_ADJUSTMENT,
			gptpconf_get_intitem(CONF_USE_HW_PHASE_ADJUSTMENT));
		res++;
	}
	if(gptpconf_get_intitem(CONF_PHASE_ADJUSTMENT_BY_FREQ) != DEFAULT_PHASE_ADJUSTMENT_BY_FREQ){
		printf("PHASE_ADJUSTMENT_BY_FREQ default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_PHASE_ADJUSTMENT_BY_FREQ,
			gptpconf_get_intitem(CONF_PHASE_ADJUSTMENT_BY_FREQ));
		res++;
	}
	if(gptpconf_get_intitem(CONF_ACTIVATE_ABNORMAL_HOOKS) != DEFAULT_ACTIVATE_ABNORMAL_HOOKS){
		printf("ACTIVATE_ABNORMAL_HOOKS default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_ACTIVATE_ABNORMAL_HOOKS,
			gptpconf_get_intitem(CONF_ACTIVATE_ABNORMAL_HOOKS));
		res++;
	}
	if(gptpconf_get_intitem(CONF_RESET_FREQADJ_BECOMEGM) != DEFAULT_RESET_FREQADJ_BECOMEGM){
		printf("RESET_FREQADJ_BECOMEGM default=%"PRIi32", configured=%"PRIi32"\n",
			(int32_t)DEFAULT_RESET_FREQADJ_BECOMEGM,
			gptpconf_get_intitem(CONF_RESET_FREQADJ_BECOMEGM));
		res++;
	}
	if(strcmp((char*)gptpconf_get_item(CONF_INTEGRITY_CLOCK_INFO), DEFAULT_INTEGRITY_CLOCK_INFO)){
		printf("INTEGRITY_CLOCK_INFO default=%s, configured=%s\n",
			DEFAULT_INTEGRITY_CLOCK_INFO,
			(char*)gptpconf_get_item(CONF_INTEGRITY_CLOCK_INFO));
		res++;
	}
	return res;
}
