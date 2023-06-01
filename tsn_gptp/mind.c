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
#include "mind.h"
#include "gptp_config.h"
#include "gptpnet.h"
#include "gptpclock.h"

void ptas_glb_init(PerTimeAwareSystemGlobal **tasglb, uint8_t domainNumber)
{
	if(!*tasglb){
		*tasglb=(PerTimeAwareSystemGlobal *)malloc(sizeof(PerTimeAwareSystemGlobal));
		if(ub_assert_fatal(*tasglb!=NULL, __func__, "malloc error")){return;}
	}
	memset(*tasglb, 0, sizeof(PerTimeAwareSystemGlobal));
	(*tasglb)->BEGIN=false;
	(*tasglb)->clockMasterLogSyncInterval=gptpconf_get_intitem(CONF_LOG_SYNC_INTERVAL);
	(*tasglb)->clockMasterSyncInterval.nsec=LOG_TO_NSEC(
		gptpconf_get_intitem(CONF_LOG_SYNC_INTERVAL));
	(*tasglb)->instanceEnable=true;
	(*tasglb)->domainNumber=domainNumber;
	(*tasglb)->gmRateRatio = 1.0;
	(*tasglb)->conformToAvnu = gptpconf_get_intitem(CONF_FOLLOW_AVNU);
}

void ptas_glb_close(PerTimeAwareSystemGlobal **tasglb)
{
	if(!*tasglb){return;}
	free(*tasglb);
	*tasglb=NULL;
}

static bool get_pp_ptpPortEnabled(uint16_t portIndex)
{
	bool ptpPortEnabled = true;
	int i;
	char *p, *str = (char *)gptpconf_get_item(CONF_PTP_PORT_ENABLED);

	p=str;
	if(portIndex>0){
		// portIndex=0(IPC port) excluded in config
		// portindex=1 uses first token in string config
		for(i=2;i<=portIndex;i++){
			p=strstr(p, ",");
			if(!p){break;}
			p++;
		}
		if(p){
			ptpPortEnabled = strtol(p,&p,10)?true:false;
		}
	}
	UB_LOG(UBL_DEBUG, "ptpPortEnabled[%d]=%s\n", portIndex, ptpPortEnabled?"true":"false");
	return ptpPortEnabled;
}

void pp_glb_init(PerPortGlobal **ppglb, PerPortGlobalForAllDomain *forAllDomain, uint16_t portIndex)
{
	if(!*ppglb){
		*ppglb=(PerPortGlobal *)malloc(sizeof(PerPortGlobal));
		if(ub_assert_fatal(*ppglb!=NULL, __func__, "malloc error")){return;}
	}
	memset(*ppglb, 0, sizeof(PerPortGlobal));
	if(forAllDomain){
		// domainNumber != 0
		(*ppglb)->forAllDomain=forAllDomain;
		//(*ppglb)->useMgtSettableLogSyncInterval = true; // 14.8.19
		/* ??? by 14.8.19 the default should be true,
		   but sync_interval_setting_sm doesn't send out the signaling messages,
		   and we set false here */
		(*ppglb)->useMgtSettableLogSyncInterval = false;
	}else{
		// domainNumber == 0
		(*ppglb)->forAllDomain=
			(PerPortGlobalForAllDomain *)malloc(sizeof(PerPortGlobalForAllDomain));
		if(ub_assert_fatal((*ppglb)->forAllDomain!=NULL,
				   __func__, "malloc error")){return;}
		memset((*ppglb)->forAllDomain, 0, sizeof(PerPortGlobalForAllDomain));
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
			gptpconf_get_intitem(CONF_LOG_ANNOUNCE_INTERVAL);
		(*ppglb)->forAllDomain->useMgtSettableLogPdelayReqInterval = false;
		(*ppglb)->forAllDomain->mgtSettableLogPdelayReqInterval =
			gptpconf_get_intitem(CONF_LOG_PDELAYREQ_INTERVAL);
		(*ppglb)->useMgtSettableLogSyncInterval = false;
	}

	(*ppglb)->asCapable = false;
	(*ppglb)->currentLogSyncInterval = gptpconf_get_intitem(CONF_LOG_SYNC_INTERVAL);
	(*ppglb)->initialLogSyncInterval = gptpconf_get_intitem(CONF_LOG_SYNC_INTERVAL);
	(*ppglb)->syncReceiptTimeout = gptpconf_get_intitem(CONF_SYNC_RECEIPT_TIMEOUT);
	(*ppglb)->syncInterval.nsec = LOG_TO_NSEC(gptpconf_get_intitem(CONF_LOG_SYNC_INTERVAL));
	(*ppglb)->syncReceiptTimeoutTimeInterval.nsec =
		gptpconf_get_intitem(CONF_SYNC_RECEIPT_TIMEOUT) * (*ppglb)->syncInterval.nsec;
	(*ppglb)->ptpPortEnabled = get_pp_ptpPortEnabled(portIndex);
	// portIndex and portNumber is the same in our implementation,
	// but it is not true in the standard
	// in case these two numbers are different, we keep portIndex separately
	(*ppglb)->thisPort = portIndex;
	(*ppglb)->thisPortIndex = portIndex;
	(*ppglb)->syncLocked = false;
	(*ppglb)->neighborGptpCapable = false;
	(*ppglb)->syncSlowdown = false;

	(*ppglb)->logGptpCapableMessageInterval =
		gptpconf_get_intitem(CONF_LOG_GPTP_CAPABLE_MESSAGE_INTERVAL);
	(*ppglb)->gPtpCapableReceiptTimeout =
		gptpconf_get_intitem(CONF_GPTP_CAPABLE_RECEIPT_TIMEOUT);
	(*ppglb)->useMgtSettableOneStepTxOper = true;
	(*ppglb)->mgtSettableOneStepTxOper = false;
	(*ppglb)->initialOneStepTxOper = false;
	(*ppglb)->currentOneStepTxOper = false;
}

void pp_glb_close(PerPortGlobal **ppglb, int domainNumber)
{
	if(!*ppglb){return;}
	if(domainNumber==0){free((*ppglb)->forAllDomain);}
	free(*ppglb);
	*ppglb=NULL;
}

void bmcs_ptas_glb_init(BmcsPerTimeAwareSystemGlobal **btasglb,
		PerTimeAwareSystemGlobal *ptasglb)
{
	if(!*btasglb){
		*btasglb=
			(BmcsPerTimeAwareSystemGlobal *)malloc(sizeof(BmcsPerTimeAwareSystemGlobal));
		if(ub_assert_fatal(*btasglb, __func__, "malloc error")){return;}
	}
	memset(*btasglb, 0, sizeof(BmcsPerTimeAwareSystemGlobal));

	(*btasglb)->externalPortConfiguration =
		gptpconf_get_intitem(CONF_EXTERNAL_PORT_CONFIGURATION);
	(*btasglb)->bmcsQuickUpdate =
		gptpconf_get_intitem(CONF_BMCS_QUICK_UPDATE_MODE);
}

void bmcs_ptas_glb_update(BmcsPerTimeAwareSystemGlobal **btasglb,
                          PerTimeAwareSystemGlobal *ptasglb, bool primary)
{
		(*btasglb)->sysLeap61 = false;
		(*btasglb)->sysLeap59 = false;
		(*btasglb)->sysCurrentUTCOffsetValid = false;
		(*btasglb)->sysPtpTimescale = gptpconf_get_intitem(CONF_TIMESCALE_PTP);
		(*btasglb)->sysTimeTraceable = false;
		(*btasglb)->sysFrequencyTraceable = false;
		if((*btasglb)->sysPtpTimescale ){
			/* 8.2.3 UTC Offset = TAI - UTC
			* As noted in the standards, UTC was behind TAI by 37s, thus default
			* value even though currentUTCOffsetValid is false should be 37.
			*/
			(*btasglb)->sysCurrentUtcOffset = 37;
		}
		(*btasglb)->sysTimeSource = gptpconf_get_intitem(CONF_TIME_SOURCE);
		/* 10.3.5 systemPriority vector */
		// systemPriority = {SS: 0: {CS: 0}: 0}
		if(primary){
			(*btasglb)->systemPriority.rootSystemIdentity.priority1 =
			    gptpconf_get_intitem(CONF_PRIMARY_PRIORITY1);
			(*btasglb)->systemPriority.rootSystemIdentity.clockClass =
			    gptpconf_get_intitem(CONF_PRIMARY_CLOCK_CLASS);
			(*btasglb)->systemPriority.rootSystemIdentity.clockAccuracy =
			    gptpconf_get_intitem(CONF_PRIMARY_CLOCK_ACCURACY);
			(*btasglb)->systemPriority.rootSystemIdentity.offsetScaledLogVariance =
			    gptpconf_get_intitem(CONF_PRIMARY_OFFSET_SCALED_LOG_VARIANCE);
			(*btasglb)->systemPriority.rootSystemIdentity.priority2 =
			    gptpconf_get_intitem(CONF_PRIMARY_PRIORITY2);
		}else{
			(*btasglb)->systemPriority.rootSystemIdentity.priority1 =
			    gptpconf_get_intitem(CONF_SECONDARY_PRIORITY1);
			(*btasglb)->systemPriority.rootSystemIdentity.clockClass =
			    gptpconf_get_intitem(CONF_SECONDARY_CLOCK_CLASS);
			(*btasglb)->systemPriority.rootSystemIdentity.clockAccuracy =
			    gptpconf_get_intitem(CONF_SECONDARY_CLOCK_ACCURACY);
			(*btasglb)->systemPriority.rootSystemIdentity.offsetScaledLogVariance =
			    gptpconf_get_intitem(CONF_SECONDARY_OFFSET_SCALED_LOG_VARIANCE);
			(*btasglb)->systemPriority.rootSystemIdentity.priority2 =
			    gptpconf_get_intitem(CONF_SECONDARY_PRIORITY2);
		}
		memcpy((*btasglb)->systemPriority.rootSystemIdentity.clockIdentity,
		       ptasglb->thisClock, sizeof(ClockIdentity));
		(*btasglb)->systemPriority.stepsRemoved = 0;
		memcpy((*btasglb)->systemPriority.sourcePortIdentity.clockIdentity,
		       ptasglb->thisClock, sizeof(ClockIdentity));
		(*btasglb)->systemPriority.sourcePortIdentity.portNumber = 0;
		(*btasglb)->systemPriority.portNumber = 0;
}

void bmcs_ptas_glb_close(BmcsPerTimeAwareSystemGlobal **btasglb)
{
	if(!*btasglb){return;}
	free(*btasglb);
	*btasglb=NULL;
}

void bmcs_pp_glb_init(BmcsPerPortGlobal **bppglb)
{
	if(!*bppglb){
		*bppglb=(BmcsPerPortGlobal *)malloc(sizeof(BmcsPerPortGlobal));
		if(ub_assert_fatal(*bppglb, __func__, "malloc error")){return;}
	}
	memset(*bppglb, 0, sizeof(BmcsPerPortGlobal));

	(*bppglb)->infoIs = Disabled;
	(*bppglb)->initialLogAnnounceInterval =
		gptpconf_get_intitem(CONF_INITIAL_LOG_ANNOUNCE_INTERVAL);

	(*bppglb)->announceReceiptTimeout =
		gptpconf_get_intitem(CONF_ANNOUNCE_RECEIPT_TIMEOUT);
}

void bmcs_pp_glb_close(BmcsPerPortGlobal **bppglb)
{
	if(!*bppglb){return;}
	free(*bppglb);
	*bppglb=NULL;
}
