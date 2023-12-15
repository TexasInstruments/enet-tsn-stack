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
#ifndef GPTPCOMMON_H_
#define GPTPCOMMON_H_
#include "gptpbasetypes.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpconf/xl4-extmod-xl4gptp.h"

/* Minimal message interval gptp can generate */
#define GPTPNET_INTERVAL_GRANULARITY_NSEC 7812500u //Should not be changed

#ifndef GPTPNET_INTERVAL_TIMEOUT_NSEC
#define GPTPNET_INTERVAL_TIMEOUT_NSEC 125000000u
#endif //GPTPNET_INTERVAL_TIMEOUT_NSEC

#if (GPTPNET_INTERVAL_TIMEOUT_NSEC != GPTPNET_INTERVAL_GRANULARITY_NSEC) && \
	(GPTPNET_INTERVAL_TIMEOUT_NSEC != GPTPNET_INTERVAL_GRANULARITY_NSEC*2u) && \
	(GPTPNET_INTERVAL_TIMEOUT_NSEC != GPTPNET_INTERVAL_GRANULARITY_NSEC*4u) && \
	(GPTPNET_INTERVAL_TIMEOUT_NSEC != GPTPNET_INTERVAL_GRANULARITY_NSEC*8u) && \
	(GPTPNET_INTERVAL_TIMEOUT_NSEC != GPTPNET_INTERVAL_GRANULARITY_NSEC*16u)
#error "Unsupported this GPTPNET_INTERVAL_TIMEOUT_NSEC"
#endif

#define ALIGN_TIME(x, a) ((((x)+((a)/2))/(a))*(a))
#define GPTP_ALIGN_TIME(x) ALIGN_TIME(x, GPTPNET_INTERVAL_GRANULARITY_NSEC)

/*==============Static memory configuration==============*/
/* The memory size increase when number of instances/ports/domains are increased.
 * Since the memory for small and medium are hard to estimate exactly,
 * an extra size option can be used in case of insufficent memory.
 */
#ifndef GPTP_MAX_INSTANCES
#define GPTP_MAX_INSTANCES 1u
#endif

/* Max physical ports e.g. 2 ports: eth0 and eth1 */
#ifndef GPTP_MAX_PORTS
#define GPTP_MAX_PORTS 2
#endif

#ifndef GPTP_MAX_DOMAINS
#define GPTP_MAX_DOMAINS 2
#endif

/* Allow to extend the small pool if any specific platform need more */
#ifndef GPTP_SMALL_EXTRA_SIZE
#define GPTP_SMALL_EXTRA_SIZE 0
#endif //GPTP_SMALL_EXTRA_SIZE

/* Allow to extend the medium pool if any specific platform need more */
#ifndef GPTP_MEDIUM_EXTRA_SIZE
#define GPTP_MEDIUM_EXTRA_SIZE 0
#endif //GPTP_MEDIUM_EXTRA_SIZE
/*==============End of static memory configuration==============*/

void eui48to64(const uint8_t *eui48, uint8_t *eui64, const uint8_t *insert);

#define LOG_TO_NSEC(x) (((x)>=0)?(uint64_t)UB_SEC_NS*(1u<<(uint8_t)(x)):(uint64_t)UB_SEC_NS/(1u<<(uint8_t)(-(x))))

#define GET_FLAG_BIT(x,b) ((x) & (1u<<(b)))
#define SET_FLAG_BIT(x,b) (x) |= (1u<<(b))
#define RESET_FLAG_BIT(x,b) (x) &= ~(1u<<(b))
#define SET_RESET_FLAG_BIT(c,x,b) if(c){SET_FLAG_BIT((x),(b));}else{RESET_FLAG_BIT((x),(b));}

#define SM_CLOSE(f,x) if((x)!=NULL){(f)(&(x));}

#define GPTP_ALIGN(x,n) ((((x)+n-1)/n)*n)

#define SM_DATA_INST sm_data_INST
#define SM_DATA_FSIZE 8
#define SM_DATA_ALIGN(x) GPTP_ALIGN((x),SM_DATA_FSIZE)

/* -------For state machines size blocks---------- */
/* In some cases, (GPTP_MAX_PORTS + 1) is used to account for the
 * addition of virtual port 0 alongside the physical ports. */
#define SM_DATA_TOTAL_SIZE \
	(SM_DATA_ALIGN(sizeof(site_sync_sync_data_t))*GPTP_MAX_DOMAINS+	  \
	SM_DATA_ALIGN(sizeof(SiteSyncSyncSM))*GPTP_MAX_DOMAINS+					\
	SM_DATA_ALIGN(sizeof(clock_master_sync_send_data_t))*GPTP_MAX_DOMAINS+	\
	SM_DATA_ALIGN(sizeof(ClockMasterSyncSendSM))*GPTP_MAX_DOMAINS+			\
	SM_DATA_ALIGN(sizeof(clock_slave_sync_data_t))*GPTP_MAX_DOMAINS+			\
	SM_DATA_ALIGN(sizeof(ClockSlaveSyncSM))*GPTP_MAX_DOMAINS+				\
	SM_DATA_ALIGN(sizeof(clock_master_sync_receive_data_t))*GPTP_MAX_DOMAINS+	\
	SM_DATA_ALIGN(sizeof(ClockMasterSyncReceiveSM))*GPTP_MAX_DOMAINS+			\
	SM_DATA_ALIGN(sizeof(clock_master_sync_offset_data_t))*GPTP_MAX_DOMAINS+		\
	SM_DATA_ALIGN(sizeof(ClockMasterSyncOffsetSM))*GPTP_MAX_DOMAINS+				\
	SM_DATA_ALIGN(sizeof(md_pdelay_req_data_t))*(GPTP_MAX_PORTS+1)+	\
	SM_DATA_ALIGN(sizeof(MDPdelayReqSM))*(GPTP_MAX_PORTS+1)+			\
	SM_DATA_ALIGN(sizeof(md_pdelay_resp_data_t))*(GPTP_MAX_PORTS+1)+	\
	SM_DATA_ALIGN(sizeof(MDPdelayRespSM))*(GPTP_MAX_PORTS+1)+			\
	SM_DATA_ALIGN(sizeof(link_delay_interval_setting_data_t))*(GPTP_MAX_PORTS+1)+ \
	SM_DATA_ALIGN(sizeof(LinkDelayIntervalSettingSM))*(GPTP_MAX_PORTS+1)+ \
	SM_DATA_ALIGN(sizeof(port_announce_information_data_t))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(PortAnnounceInformationSM))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(port_announce_information_ext_data_t))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(PortAnnounceInformationExtSM))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(md_sync_receive_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(MDSyncReceiveSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(port_sync_sync_receive_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(PortSyncSyncReceiveSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(port_sync_sync_send_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(PortSyncSyncSendSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(md_sync_send_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(MDSyncSendSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(port_announce_receive_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(PortAnnounceReceiveSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(port_announce_transmit_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(PortAnnounceTransmitSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(gptp_capable_transmit_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(gPtpCapableTransmitSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(gptp_capable_receive_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(gPtpCapableReceiveSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(sync_interval_setting_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(SyncIntervalSettingSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(announce_interval_setting_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(AnnounceIntervalSettingSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(one_step_tx_oper_setting_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(OneStepTxOperSettingSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(md_announce_send_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(MDAnnounceSendSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(md_announce_receive_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(MDAnnounceReceiveSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(port_state_setting_ext_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(PortStateSettingExtSM))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(port_state_selection_data_t))*GPTP_MAX_DOMAINS+				\
	SM_DATA_ALIGN(sizeof(PortStateSelectionSM))*GPTP_MAX_DOMAINS+					\
	SM_DATA_ALIGN(sizeof(md_signaling_send_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+	\
	SM_DATA_ALIGN(sizeof(md_signaling_receive_data_t))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	SM_DATA_ALIGN(sizeof(gm_stable_data_t))*GPTP_MAX_DOMAINS)

#define SM_DATA_FNUM (GPTP_MAX_INSTANCES*(SM_DATA_TOTAL_SIZE/SM_DATA_FSIZE))
UB_SD_GETMEM_DEF_EXTERN(SM_DATA_INST);
/* -------End of state machines size blocks---------- */

/* -------For small size blocks---------- */
#define GPTP_SMALL_ALLOC gptp_small_alloc
#define GPTP_SMALL_AFSIZE 4u
#define GPTP_SMALL_ALIGN(x) GPTP_ALIGN((x),GPTP_SMALL_AFSIZE)
#define GPTP_SMALL_TOTAL_SIZE \
	(GPTP_SMALL_ALIGN(sizeof(PerPortGlobal*)*(GPTP_MAX_PORTS+1))*GPTP_MAX_DOMAINS+ \
	 GPTP_SMALL_ALIGN(sizeof(BmcsPerPortGlobal*)*(GPTP_MAX_PORTS+1))*GPTP_MAX_DOMAINS+ \
	 GPTP_SMALL_ALIGN(sizeof(UInteger224))*(GPTP_MAX_PORTS+1)+			\
	 GPTP_SMALL_ALIGN(sizeof(PortStateSelectionSMforAllDomain))+		\
	 GPTP_SMALL_ALIGN(sizeof(PortStateSettingExtSMforAllDomain))*(GPTP_MAX_PORTS*GPTP_MAX_DOMAINS)+ \
	 GPTP_SMALL_ALIGN(sizeof(MDEntityGlobal))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	 GPTP_SMALL_ALIGN(128)+/* reserved for hard counted places */ \
	 GPTP_SMALL_ALIGN(GPTP_SMALL_EXTRA_SIZE))

#define GPTP_SMALL_AFNUM (GPTP_MAX_INSTANCES*(GPTP_SMALL_TOTAL_SIZE/GPTP_SMALL_AFSIZE))
UB_SD_GETMEM_DEF_EXTERN(GPTP_SMALL_ALLOC);
/* -------End of small size blocks---------- */

/*-------- For medium size blocks ---------------*/
/* In some cases, (GPTP_MAX_PORTS + 1) is used to account for the
 * addition of virtual port 0 alongside the physical ports. */
#define GPTP_MEDIUM_ALLOC gptp_medium_alloc
#define GPTP_MEDIUM_AFSIZE 16u
#define GPTP_MEDIUM_ALIGN(x) GPTP_ALIGN((x),GPTP_MEDIUM_AFSIZE)

/* The gptpnet is a platform-dependent, it is hard to estimate an accurate number for each.
 * The gptpclock structure will not be exported to the header, an rough number will be used.
 * We will accept the rough estimation in these cases. */
#define GPTP_MEDIUM_TOTAL_SIZE \
	(GPTP_MEDIUM_ALIGN(sizeof(gptpman_data_t))+ \
	 GPTP_MEDIUM_ALIGN(sizeof(gptpsm_tasd_t)*GPTP_MAX_DOMAINS)/*intention*/+ \
	 GPTP_MEDIUM_ALIGN(sizeof(gptpsm_ptd_t)*(GPTP_MAX_PORTS+1))*GPTP_MAX_DOMAINS/*intention*/+ \
	 GPTP_MEDIUM_ALIGN(sizeof(PerTimeAwareSystemGlobal))*GPTP_MAX_DOMAINS+ \
	 GPTP_MEDIUM_ALIGN(sizeof(PerfMonClockDS))*GPTP_MAX_DOMAINS+		\
	 GPTP_MEDIUM_ALIGN(sizeof(PerPortGlobal))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	 GPTP_MEDIUM_ALIGN(sizeof(PerPortGlobalForAllDomain))*(GPTP_MAX_PORTS+1)+ \
	 GPTP_MEDIUM_ALIGN(sizeof(PerfMonPortDS))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	 GPTP_MEDIUM_ALIGN(sizeof(BmcsPerTimeAwareSystemGlobal))*GPTP_MAX_DOMAINS+ \
	 GPTP_MEDIUM_ALIGN(sizeof(BmcsPerPortGlobal))*((GPTP_MAX_PORTS+1)*GPTP_MAX_DOMAINS)+ \
	 GPTP_MEDIUM_ALIGN(sizeof(MDEntityGlobalForAllDomain))*(GPTP_MAX_PORTS+1)+ \
	 GPTP_MEDIUM_ALIGN(48)+ /*48: big rough of sizeof(gptpclock_data_t)*/	\
	 GPTP_MEDIUM_ALIGN(56*GPTP_MAX_DOMAINS)+ /*56: big rough of sizeof(per_domain_data_t)*/ \
	 GPTP_MEDIUM_ALIGN((10+sizeof(event_data_netlink_t)+GPTP_MAX_PACKET_SIZE+sizeof(CB_ETHHDR_T))*GPTP_MAX_PORTS)+ /*big parts of netdevices */ \
	 GPTP_MEDIUM_ALIGN(128)+ /* reserved for uncountable size in gptpnet */ \
	 GPTP_MEDIUM_ALIGN(GPTP_MEDIUM_EXTRA_SIZE))

#define GPTP_MEDIUM_AFNUM \
	(GPTP_MAX_INSTANCES*(GPTP_MEDIUM_TOTAL_SIZE/GPTP_MEDIUM_AFSIZE))

UB_SD_GETMEM_DEF_EXTERN(GPTP_MEDIUM_ALLOC);
/*--------End of medium size blocks ---------------*/

/* allocate typed in *sm, then allocate typesm in (*sm)->thisSM */
#define INIT_SM_DATA(typed, typesm, sm)					\
{									\
	if(!*sm){							\
		*sm=(typed*)UB_SD_GETMEM(SM_DATA_INST, sizeof(typed));	\
		(void)ub_assert_fatal(*sm!=NULL, __func__, "malloc1");	\
	}								\
	if(*sm!=NULL){							\
		(void)memset(*sm, 0, sizeof(typed));			\
		(*sm)->thisSM=(typesm*)UB_SD_GETMEM(SM_DATA_INST, sizeof(typesm)); \
		if(!ub_assert_fatal((*sm)->thisSM!=NULL, __func__, "malloc2")){ \
			(void)memset((*sm)->thisSM, 0, sizeof(typesm));	\
		}							\
	}								\
}

/* free the above allocations */
#define CLOSE_SM_DATA(sm) \
	if(!*(sm)){return 0;}				\
	UB_SD_RELMEM(SM_DATA_INST, (*(sm))->thisSM);	\
	UB_SD_RELMEM(SM_DATA_INST, *(sm));		\
	*(sm)=NULL;

/* check for portPriority vector, any non-zero value is sufficient */
#define HAS_PORT_PRIORITY(pp) \
        (((pp).rootSystemIdentity.priority1 !=0u) || \
         ((pp).rootSystemIdentity.clockClass != 0u) || \
         ((pp).rootSystemIdentity.clockAccuracy != 0u) || \
         ((pp).rootSystemIdentity.offsetScaledLogVariance != 0u) || \
         ((pp).rootSystemIdentity.priority2 != 0u) || \
         ((pp).stepsRemoved != 0u))

typedef enum {
        SAME_PRIORITY,
        SUPERIOR_PRIORITY,
        INFERIOR_PRIORITY,
} bmcs_priority_comparison_result;

void print_priority_vector(ub_dbgmsg_level_t level, const char *identifier, UInteger224 *priorityVector);
uint8_t compare_priority_vectors(UInteger224 *priorityA, UInteger224 *priorityB);

#endif
