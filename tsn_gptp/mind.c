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
#include <tsn_unibase/unibase.h>
#include "mind.h"
#include "gptpconf/gptpgcfg.h"
#include <tsn_uniconf/yangs/ieee1588-ptp-tt.h>
#include "gptpnet.h"
#include "gptpclock.h"
#include "gptpcommon.h"
#include "gptp_perfmon.h"

void ptas_glb_init(PerTimeAwareSystemGlobal **tasglb,
		   uint8_t gptpInstanceIndex, uint8_t domainIndex)
{
	if(!*tasglb){
		*tasglb=(PerTimeAwareSystemGlobal *)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(PerTimeAwareSystemGlobal));
		if(ub_assert_fatal(*tasglb!=NULL, __func__, "malloc error")){return;}
	}
	(void)memset(*tasglb, 0, sizeof(PerTimeAwareSystemGlobal));
	(*tasglb)->BEGIN=false;
	(*tasglb)->clockMasterLogSyncInterval=gptpgcfg_get_yang_portds_intitem(
		gptpInstanceIndex,
		IEEE1588_PTP_TT_LOG_SYNC_INTERVAL,
		domainIndex, 0, YDBI_STATUS); // use the data of port=0
	(*tasglb)->clockMasterSyncInterval.nsec=LOG_TO_NSEC(
		(*tasglb)->clockMasterLogSyncInterval);
	(*tasglb)->instanceEnable=true;
	(*tasglb)->domainIndex=domainIndex;
	(*tasglb)->gmRateRatio = 1.0;
	(*tasglb)->conformToAvnu = gptpgcfg_get_intitem(
		gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_FOLLOW_AVNU,
		YDBI_CONFIG);

	(*tasglb)->perfmonEnable = gptpgcfg_get_yang_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_PERFORMANCE_MONITORING_DS,
			IEEE1588_PTP_TT_ENABLE, 255,
			domainIndex, YDBI_CONFIG);
	UB_LOG(UBL_INFO, "IEEE1588-2019 performance monitoring %s.\n",
		(*tasglb)->perfmonEnable?"enabled":"disabled");

	if((*tasglb)->perfmonEnable){
		if(!(*tasglb)->perfmonClockDS){
			(*tasglb)->perfmonClockDS=(PerfMonClockDS*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(PerfMonClockDS));
			if(ub_assert_fatal((*tasglb)->perfmonClockDS!=NULL, __func__, "malloc error")){return;}
			if(gptpgcfg_deleteall_clock_perfmonDS(gptpInstanceIndex, domainIndex)){
				// non-fatal, performance monitoring will not be reflected to db
				UB_LOG(UBL_WARN, "%s: domainIndex=%d cannot clear clock perfmon entries in db...\n",
					__func__, domainIndex);
			}
			// reset current records
			gptp_clock_perfmon_dr_reset((*tasglb)->perfmonClockDS, PERFMON_ALL_DR, ub_mt_gettime64());
		}
		(*tasglb)->perfmonLongPeriod_ms = gptpgcfg_get_intitem(
			gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_PERFMON_LONG_PERIOD, YDBI_CONFIG);
		(*tasglb)->perfmonShortPeriod_ms = gptpgcfg_get_intitem(
			gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_PERFMON_SHORT_PERIOD, YDBI_CONFIG);
		(*tasglb)->perfmonCurrentPeriod_ms = gptpgcfg_get_intitem(
			gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_PERFMON_CURRENT_PERIOD, YDBI_CONFIG);
	}
	(*tasglb)->gptpInstanceIndex = gptpInstanceIndex;
}

void ptas_glb_close(PerTimeAwareSystemGlobal **tasglb)
{
	if(!*tasglb){return;}
	if((*tasglb)->perfmonClockDS){
		UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, (*tasglb)->perfmonClockDS);
	}
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, *tasglb);
	*tasglb=NULL;
}

static bool get_pp_ptpPortEnabled(uint8_t gptpInstanceIndex, uint16_t portIndex,
				  uint8_t domainIndex)
{
	int res;
	res=gptpgcfg_get_yang_portds_intitem(
		gptpInstanceIndex, IEEE1588_PTP_TT_PORT_ENABLE,
		portIndex, domainIndex, YDBI_CONFIG);
	UB_LOG(UBL_DEBUG, "domainIndex=%d, ptpPortEnabled[%d]=%d\n",
	       domainIndex, portIndex, res);
	return (res!=0);
}

void pp_glb_init(uint8_t gptpInstanceIndex, PerPortGlobal **ppglb,
		 PerPortGlobalForAllDomain *forAllDomain,
		 PerTimeAwareSystemGlobal *tasglb,
		 uint8_t domainIndex, uint16_t portIndex)
{
	if(!*ppglb){
		*ppglb=(PerPortGlobal*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(PerPortGlobal));
		if(ub_assert_fatal(*ppglb!=NULL, __func__, "malloc error")){return;}
	}
	(void)memset(*ppglb, 0, sizeof(PerPortGlobal));
	if(forAllDomain!=NULL){
		// domainIndex != 0
		(*ppglb)->forAllDomain=forAllDomain;
		//(*ppglb)->useMgtSettableLogSyncInterval = true; // 14.8.19
		/* ??? by 14.8.19 the default should be true,
		   but sync_interval_setting_sm doesn't send out the signaling messages,
		   and we set false here */
		(*ppglb)->useMgtSettableLogSyncInterval = false;
	}else{
		// domainIndex == 0
		(*ppglb)->forAllDomain=
			(PerPortGlobalForAllDomain*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC,
								 sizeof(PerPortGlobalForAllDomain));
		if(ub_assert_fatal((*ppglb)->forAllDomain!=NULL,
				   __func__, "malloc error")){return;}
		(void)memset((*ppglb)->forAllDomain, 0, sizeof(PerPortGlobalForAllDomain));
		(*ppglb)->forAllDomain->asymmetryMeasurementMode = false;
		(*ppglb)->forAllDomain->portOper = false;
		/* 802.1AS-2020 11.6 Control of computation of neighborRateRatio
		 * When useMgtSettableComputeNeighborRateRatio is false,
		 * computeNeighborRateRatio is initialzied to the value of
		 * initialComputeNeighborRateRatio.
		 * ...The default value of initialComputeNeighborRateRatio is true.
		 */
		(*ppglb)->forAllDomain->computeNeighborRateRatio = true;
		(*ppglb)->forAllDomain->computeNeighborPropDelay = true;
		(*ppglb)->forAllDomain->useMgtSettableLogAnnounceInterval = false;
		(*ppglb)->forAllDomain->mgtSettableLogAnnounceInterval =
			gptpgcfg_get_yang_portds_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_LOG_ANNOUNCE_INTERVAL,
				portIndex, domainIndex, YDBI_CONFIG);
		(*ppglb)->forAllDomain->useMgtSettableLogPdelayReqInterval = false;
		(*ppglb)->forAllDomain->mgtSettableLogPdelayReqInterval =
			gptpgcfg_get_yang_portds_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_INITIAL_LOG_PDELAY_REQ_INTERVAL,
				portIndex, domainIndex, YDBI_CONFIG);
		(*ppglb)->useMgtSettableLogSyncInterval = false;
	}

	(*ppglb)->asCapable = false;
	(*ppglb)->currentLogSyncInterval =
		gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_CURRENT_LOG_SYNC_INTERVAL,
			portIndex, domainIndex, YDBI_STATUS);
	(*ppglb)->initialLogSyncInterval =
		gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_INITIAL_LOG_SYNC_INTERVAL,
			portIndex, domainIndex, YDBI_CONFIG);
	(*ppglb)->syncReceiptTimeout =
		gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_SYNC_RECEIPT_TIMEOUT,
			portIndex, domainIndex, YDBI_CONFIG);
	(*ppglb)->syncInterval.nsec =
		LOG_TO_NSEC(gptpgcfg_get_yang_portds_intitem(
				    gptpInstanceIndex,
				    IEEE1588_PTP_TT_LOG_SYNC_INTERVAL,
				    portIndex, domainIndex, YDBI_STATUS));
	// we don't use IEEE1588_PTP_TT_SYNC_RECEIPT_TIMEOUT_INTERVAL
	(*ppglb)->syncReceiptTimeoutTimeInterval.nsec =
		(uint8_t)(*ppglb)->syncReceiptTimeout * (*ppglb)->syncInterval.nsec;
	(*ppglb)->ptpPortEnabled = get_pp_ptpPortEnabled(gptpInstanceIndex,
							 portIndex, domainIndex);
	(*ppglb)->thisPort = portIndex;
	(*ppglb)->syncLocked = false;
	(*ppglb)->neighborGptpCapable = false;
	(*ppglb)->syncSlowdown = false;

	(*ppglb)->logGptpCapableMessageInterval =
		gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_CURRENT_LOG_GPTP_CAP_INTERVAL,
			portIndex, domainIndex, YDBI_STATUS);
	(*ppglb)->gPtpCapableReceiptTimeout =
		gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_GPTP_CAP_RECEIPT_TIMEOUT,
			portIndex, domainIndex, YDBI_CONFIG);
	(*ppglb)->useMgtSettableOneStepTxOper =
		(gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_USE_MGT_ONE_STEP_TX_OPER,
			portIndex, domainIndex, YDBI_STATUS)==1);
	(*ppglb)->mgtSettableOneStepTxOper =
		(gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_MGT_ONE_STEP_TX_OPER,
			portIndex, domainIndex, YDBI_STATUS)==1);
	(*ppglb)->initialOneStepTxOper =
		(gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_INITIAL_ONE_STEP_TX_OPER,
			portIndex, domainIndex, YDBI_STATUS)==1);
	(*ppglb)->currentOneStepTxOper =
		(gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_CURRENT_ONE_STEP_TX_OPER,
			portIndex, domainIndex, YDBI_STATUS)==1);

	if(tasglb->perfmonEnable){
		(*ppglb)->perfmonDS=
			(PerfMonPortDS*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(PerfMonPortDS));
		if(ub_assert_fatal((*ppglb)->perfmonDS!=NULL, __func__, "malloc error")){return;}
		if(gptpgcfg_deleteall_port_perfmonDS(gptpInstanceIndex, domainIndex, portIndex)){
				// non-fatal, performance monitoring will not be reflected to db
				UB_LOG(UBL_WARN, "%s: domainIndex=%d cannot clear port=%d perfmon entries in db...\n",
					__func__, domainIndex, portIndex);
		}
		gptp_port_perfmon_dr_reset((*ppglb)->perfmonDS, PERFMON_ALL_DR,
			  domainIndex, portIndex, ub_mt_gettime64());
	}
}

void pp_glb_close(PerPortGlobal **ppglb, int domainIndex)
{
	if(!*ppglb){return;}
	if(domainIndex==0){UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, (*ppglb)->forAllDomain);}
	if((*ppglb)->perfmonDS){
		UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, (*ppglb)->perfmonDS);
	}
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, *ppglb);
	*ppglb=NULL;
}

void bmcs_ptas_glb_init(uint8_t gptpInstanceIndex, BmcsPerTimeAwareSystemGlobal **btasglb,
                        PerTimeAwareSystemGlobal *ptasglb, uint8_t domainIndex)
{
	if(!*btasglb){
		*btasglb=(BmcsPerTimeAwareSystemGlobal*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC,
								     sizeof(BmcsPerTimeAwareSystemGlobal));
		if(ub_assert_fatal(*btasglb, __func__, "malloc error")){return;}
	}
	(void)memset(*btasglb, 0, sizeof(BmcsPerTimeAwareSystemGlobal));

	(*btasglb)->externalPortConfiguration =
		gptpgcfg_get_yang_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_DEFAULT_DS,
			IEEE1588_PTP_TT_EXTERNAL_PORT_CONFIG_ENABLE, 255,
			domainIndex, YDBI_CONFIG);
	(*btasglb)->bmcsQuickUpdate =
		gptpgcfg_get_intitem(
			ptasglb->gptpInstanceIndex,
			XL4_EXTMOD_XL4GPTP_BMCS_QUICK_UPDATE_MODE,
			YDBI_CONFIG);
}

void bmcs_ptas_glb_update(uint8_t gptpInstanceIndex,
			  BmcsPerTimeAwareSystemGlobal **btasglb,
                          PerTimeAwareSystemGlobal *ptasglb,
			  uint8_t domainIndex)
{
		(*btasglb)->sysLeap61 = false;
		(*btasglb)->sysLeap59 = false;
		(*btasglb)->sysCurrentUTCOffsetValid = false;
		(*btasglb)->sysPtpTimescale = gptpgcfg_get_yang_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_DEFAULT_DS,
			IEEE1588_PTP_TT_PTP_TIMESCALE, 255,
			domainIndex, YDBI_STATUS);
		(*btasglb)->sysTimeTraceable = false;
		(*btasglb)->sysFrequencyTraceable = false;
		if((*btasglb)->sysPtpTimescale ){
			/* 8.2.3 UTC Offset = TAI - UTC
			* As noted in the standards, UTC was behind TAI by 37s, thus default
			* value even though currentUTCOffsetValid is false should be 37.
			*/
			(*btasglb)->sysCurrentUtcOffset = 37;
		}
		(*btasglb)->sysTimeSource = gptpgcfg_get_yang_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_DEFAULT_DS,
			IEEE1588_PTP_TT_TIME_SOURCE, 255,
			domainIndex, YDBI_STATUS);
		/* 10.3.5 systemPriority vector */
		// systemPriority = {SS: 0: {CS: 0}: 0}
		(*btasglb)->systemPriority.rootSystemIdentity.priority1 =
			gptpgcfg_get_yang_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_DEFAULT_DS,
				IEEE1588_PTP_TT_PRIORITY1, 255,
				domainIndex, YDBI_CONFIG);
		(*btasglb)->systemPriority.rootSystemIdentity.clockClass =
			gptpgcfg_get_yang_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_DEFAULT_DS,
				IEEE1588_PTP_TT_CLOCK_QUALITY,
				IEEE1588_PTP_TT_CLOCK_CLASS,
				domainIndex, YDBI_CONFIG);
		(*btasglb)->systemPriority.rootSystemIdentity.clockAccuracy =
			gptpgcfg_get_yang_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_DEFAULT_DS,
				IEEE1588_PTP_TT_CLOCK_QUALITY,
				IEEE1588_PTP_TT_CLOCK_ACCURACY,
				domainIndex, YDBI_CONFIG);
		(*btasglb)->systemPriority.rootSystemIdentity.offsetScaledLogVariance =
			gptpgcfg_get_yang_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_DEFAULT_DS,
				IEEE1588_PTP_TT_CLOCK_QUALITY,
				IEEE1588_PTP_TT_OFFSET_SCALED_LOG_VARIANCE,
				domainIndex, YDBI_CONFIG);
		(*btasglb)->systemPriority.rootSystemIdentity.priority2 =
			gptpgcfg_get_yang_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_DEFAULT_DS,
				IEEE1588_PTP_TT_PRIORITY2, 255,
				domainIndex, YDBI_CONFIG);
		memcpy((*btasglb)->systemPriority.rootSystemIdentity.clockIdentity,
		       ptasglb->thisClock, sizeof(ClockIdentity));
		(*btasglb)->systemPriority.stepsRemoved = 0;
		memcpy((*btasglb)->systemPriority.sourcePortIdentity.clockIdentity,
		       ptasglb->thisClock, sizeof(ClockIdentity));
		(*btasglb)->systemPriority.sourcePortIdentity.portIndex = 0;
		(*btasglb)->systemPriority.portNumber = 0;
}

void bmcs_ptas_glb_close(BmcsPerTimeAwareSystemGlobal **btasglb)
{
	if(!*btasglb){return;}
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, *btasglb);
	*btasglb=NULL;
}

void bmcs_pp_glb_init(uint8_t gptpInstanceIndex, BmcsPerPortGlobal **bppglb,
		      uint8_t domainIndex, uint8_t portIndex)
{
	if(!*bppglb){
		*bppglb=(BmcsPerPortGlobal*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC,
							 sizeof(BmcsPerPortGlobal));
		if(ub_assert_fatal(*bppglb, __func__, "malloc error")){return;}
	}
	(void)memset(*bppglb, 0, sizeof(BmcsPerPortGlobal));

	(*bppglb)->infoIs = Disabled;
	(*bppglb)->initialLogAnnounceInterval =
		gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_INITIAL_LOG_ANNOUNCE_INTERVAL,
			portIndex, domainIndex, YDBI_CONFIG);
	(*bppglb)->announceReceiptTimeout =
		gptpgcfg_get_yang_portds_intitem(
			gptpInstanceIndex,
			IEEE1588_PTP_TT_ANNOUNCE_RECEIPT_TIMEOUT,
			portIndex, domainIndex, YDBI_CONFIG);
}

void bmcs_pp_glb_close(BmcsPerPortGlobal **bppglb)
{
	if(!*bppglb){return;}
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, *bppglb);
	*bppglb=NULL;
}
