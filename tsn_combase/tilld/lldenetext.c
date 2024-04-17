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
#include "lldtype.h"
#include "lldenet_priv.h"
#include "lldenet.h"
#include "lldenetext.h"

#if ENET_CFG_IS_ON(CPSW_EST)

static uint64_t getCurrentTime(Enet_Handle hEnet, uint32_t coreId)
{
	Enet_IoctlPrms prms;
	int32_t status;
	uint64_t tsVal = 0ULL;
	ENET_IOCTL_SET_OUT_ARGS(&prms, &tsVal);
	ENET_IOCTL(hEnet, coreId,
		   ENET_TIMESYNC_IOCTL_GET_CURRENT_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		tsVal = 0ULL;
	}

	return tsVal;
}

static EnetTas_TasState getEstState(Enet_Handle hEnet,
				 uint32_t coreId, Enet_MacPort macPort)
{
	Enet_IoctlPrms prms;
	EnetTas_GenericInArgs stateInArgs;
	EnetTas_TasState state = ENET_TAS_RESET;
	int32_t status;

	/* Get the current EST state */
	stateInArgs.macPort = (Enet_MacPort)macPort;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &stateInArgs, &state);

	ENET_IOCTL(hEnet, coreId, ENET_TAS_IOCTL_GET_STATE, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to get TAS state: %d\n", status);
	}

	return state;
}

static int32_t setEstState(Enet_Handle hEnet, uint32_t coreId,
			Enet_MacPort macPort, EnetTas_TasState state)
{
	Enet_IoctlPrms prms;
	EnetTas_SetStateInArgs setStateInArgs;
	int32_t status;

	setStateInArgs.macPort = (Enet_MacPort)macPort;
	setStateInArgs.state   = state;
	ENET_IOCTL_SET_IN_ARGS(&prms, &setStateInArgs);

	ENET_IOCTL(hEnet, coreId, ENET_TAS_IOCTL_SET_STATE, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to set TAS state %u: %d\n",
		   (uint32_t)state, status);
	} else {
		UB_LOG(UBL_INFO, "TAS state is set to %u\n", (uint32_t)state);
	}

	return status;
}

static void printEstList(EnetTas_ControlList *list)
{
	uint8_t gateMask = 0U;
	uint32_t start = 0U;
	uint32_t end;
	uint32_t dur;
	uint32_t i;

	for (i = 0U; i < list->listLength; i++) {
		gateMask = list->gateCmdList[i].gateStateMask;
		dur = list->gateCmdList[i].timeInterval;
		end = start + dur - 1U;

		// o = Gate open, C = Gate closed
		UB_LOG(UBL_INFO, "Gate mask=%s%s%s%s%s%s%s%s (0x%02x), start=%u ns, end=%u ns, dur=%u ns\n",
		   ENET_IS_BIT_SET(gateMask, 7U) ? "o" : "C",
		   ENET_IS_BIT_SET(gateMask, 6U) ? "o" : "C",
		   ENET_IS_BIT_SET(gateMask, 5U) ? "o" : "C",
		   ENET_IS_BIT_SET(gateMask, 4U) ? "o" : "C",
		   ENET_IS_BIT_SET(gateMask, 3U) ? "o" : "C",
		   ENET_IS_BIT_SET(gateMask, 2U) ? "o" : "C",
		   ENET_IS_BIT_SET(gateMask, 1U) ? "o" : "C",
		   ENET_IS_BIT_SET(gateMask, 0U) ? "o" : "C",
		   gateMask, start, end, dur);
		start += dur;
	}

	UB_LOG(UBL_INFO, "Base time=%llu, Cycle time=%llu \n", list->baseTime, list->cycleTime);
}

static int validateOperListParams(Enet_Handle hEnet, uint32_t coreId,
				  Enet_MacPort macPort,
				  EnetTas_ControlList *adminList)
{
	Enet_IoctlPrms prms;
	EnetTas_GenericInArgs inArgs;
	EnetTas_ControlList operList;
	int32_t status, res = -1, i;

	inArgs.macPort = (Enet_MacPort)macPort;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &operList);

	ENET_IOCTL(hEnet, coreId, ENET_TAS_IOCTL_GET_OPER_LIST, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to get TAS oper list: %d\n", status);
	} else {
		res = -1;
#ifdef TAS_SHOW_OPERLIST
		printEstList(&operList);
#endif
		for (i = 0; i < adminList->listLength; i++) {
			if (operList.gateCmdList[i].gateStateMask !=
			adminList->gateCmdList[i].gateStateMask) {
				return res;
			}
		}
		if (operList.cycleTime != adminList->cycleTime) {
			return res;
		}
		res = 0;
	}
	return res;
}

/* Domain used to identify EST timestamp events */
#define ENET_APP_EST_TIMESTAMP_DOMAIN             (100U)
int LLDEnetTasEnableTimestamp(LLDEnet_t *hLLDEnet, uint8_t macPort, cbl_qmap_params_t *qmapPrm)
{
	Enet_IoctlPrms prms;
	CpswMacPort_EstTimestampCfg timestampCfg;
	int32_t status;

	if (qmapPrm->num_pq == 0) {
		return LLDENET_E_PARAM;
	}
	timestampCfg.macPort = (Enet_MacPort)macPort;
	/*
	* If we set timestamping mode CPSW_MACPORT_EST_TIMESTAMP_ALL to
	* enable EST timestamping for all traffics, it causes missing Tx TS event
	* of the PTP packets.
	* In addition, since this IOCTL doesn't support to set EST timestamping for
	* multiple priorities, we only enable EST timestamping for traffic
	* which has highest priority (highest number of HW queue).
	*/
	timestampCfg.mode = CPSW_MACPORT_EST_TIMESTAMP_ONEPRI;
	timestampCfg.domain = ENET_APP_EST_TIMESTAMP_DOMAIN;
	timestampCfg.priority = qmapPrm->num_pq-1;
	ENET_IOCTL_SET_IN_ARGS(&prms, &timestampCfg);

	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   CPSW_MACPORT_IOCTL_EST_ENABLE_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "%s,Failed to enbale EST timestamp: %d\n",
		       __func__, status);
	}
	return (status == ENET_SOK ? LLDENET_E_OK: LLDENET_E_FAILURE);
}

int LLDEnetTasDisableTimestamp(LLDEnet_t *hLLDEnet, uint8_t macPort)
{
	Enet_IoctlPrms prms;
	EnetMacPort_GenericInArgs inArgs;
	int32_t status;

	inArgs.macPort = (Enet_MacPort)macPort;
	ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   CPSW_MACPORT_IOCTL_EST_DISABLE_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "%s, Failed to disable EST timestamp: %d\n",
		       __func__, status);
	}
	return (status == ENET_SOK ? LLDENET_E_OK: LLDENET_E_FAILURE);
}

#endif // ENET_CFG_IS_ON(CPSW_EST)

#define OPER_LIST_UPDATE_CHECK_RETRY_MAX (10U)
int LLDEnetTasSetConfig(LLDEnet_t *hLLDEnet, uint8_t macPort,
			cbl_tas_sched_params_t *ctsp)
{
	int32_t status = LLDENET_E_UNSUPPORT;

#if ENET_CFG_IS_ON(CPSW_EST)

	Enet_IoctlPrms prms;
	EnetTas_GenericInArgs inArgs;
	EnetTas_ControlList adminList = {};
	EnetTas_SetAdminListInArgs adminListInArgs = {};
	cbl_tas_gate_cmd_entry_t *entry;
	int i, noe;

	inArgs.macPort = (Enet_MacPort)macPort;
	if (ctsp->action == CBL_ACTION_DEL) {
		status = setEstState(hLLDEnet->hEnet, hLLDEnet->coreId,
				     (Enet_MacPort)macPort, ENET_TAS_DISABLE);
		return LLDENET_E_OK;
	}

	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &adminList);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_TAS_IOCTL_GET_ADMIN_LIST, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to get TAS admin list: %d\n", status);
		return LLDENET_E_IOCTL;
	}

	adminListInArgs.macPort = (Enet_MacPort)macPort;
	uint64_t cycle_time = ctsp->cycle_time_denominator > 0?
		((ctsp->cycle_time_numerator*UB_SEC_NS)/ctsp->cycle_time_denominator):0ULL;
	adminListInArgs.adminList.cycleTime = cycle_time;

	noe = (ctsp->entries != NULL)? ub_esarray_ele_nums(ctsp->entries) : 0;
	adminListInArgs.adminList.listLength = noe;
	for (i = 0; i < noe; i++) {
		if (i == ENET_TAS_MAX_CMD_LISTS) {
			adminListInArgs.adminList.listLength = ENET_TAS_MAX_CMD_LISTS;
			break;
		}
		entry = (cbl_tas_gate_cmd_entry_t *)ub_esarray_get_ele(ctsp->entries, i);
		if (entry) {
			adminListInArgs.adminList.gateCmdList[i].timeInterval = entry->interval;
			adminListInArgs.adminList.gateCmdList[i].gateStateMask = entry->gate;
		}
	}
	if (ctsp->extp) {
		memcpy(adminListInArgs.adminList.sduTable.maxSDU,
			   ctsp->extp->max_sdu,
			   sizeof(EnetTas_MaxSDUTable));
	} else {
		memcpy(&adminListInArgs.adminList.sduTable,
			   &adminList.sduTable,
			   sizeof(EnetTas_MaxSDUTable));
	}
	// Set TAS state to `RESET`
	status = setEstState(hLLDEnet->hEnet, hLLDEnet->coreId,
				 (Enet_MacPort)macPort, ENET_TAS_RESET);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to reset TAS state \n");
		return LLDENET_E_IOCTL;
	}
	adminListInArgs.adminList.baseTime = ctsp->base_time_sec*UB_SEC_NS + ctsp->base_time_nsec;

	ENET_IOCTL_SET_IN_ARGS(&prms, &adminListInArgs);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_TAS_IOCTL_SET_ADMIN_LIST, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to set TAS admin list: %d\n", status);
		return LLDENET_E_IOCTL;
	}
	// Wait until the operational list is updated.
	EnetTas_OperStatus operStatus = ENET_TAS_OPER_LIST_NOT_YET_UPDATED;
	for (i = 0U; i < OPER_LIST_UPDATE_CHECK_RETRY_MAX; i++) {
		ENET_IOCTL_SET_INOUT_ARGS(&prms, &adminListInArgs.macPort, &operStatus);

		ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
			   ENET_TAS_IOCTL_GET_OPER_LIST_STATUS, &prms, status);
		if (status != ENET_SOK) {
			UB_LOG(UBL_ERROR,"Failed to check TAS operational list update status: %d\n",
				   status);
			break;
		}
		if (operStatus == ENET_TAS_OPER_LIST_UPDATED) {
			UB_LOG(UBL_INFO,"TAS operational list status updated:\n");
			break;
		}
	}
	// Set TAS state to `ENABLE`
	status = setEstState(hLLDEnet->hEnet, hLLDEnet->coreId,
				 (Enet_MacPort)macPort, ENET_TAS_ENABLE);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to enable TAS \n");
		return LLDENET_E_IOCTL;
	}
	if (!validateOperListParams(hLLDEnet->hEnet, hLLDEnet->coreId,
					(Enet_MacPort)macPort,
					&adminListInArgs.adminList)) {
		UB_LOG(UBL_INFO, "Successfully configure TAS \n");
		status = LLDENET_E_OK;
	} else {
		status = LLDENET_E_IOCTL;
	}

#endif //ENET_CFG_IS_ON(CPSW_EST)

	return status;
}

int LLDEnetIETSetConfig(LLDEnet_t *hLLDEnet, uint8_t macPort,
			cbl_preempt_params_t *cpemp,
			cbl_cb_event_t *nevent)
{
	int32_t status = LLDENET_E_UNSUPPORT;

#if ENET_CFG_IS_ON(CPSW_IET_INCL)

	EnetMacPort_GenericInArgs egargs = {};
	Enet_IoctlPrms prms;
	int32_t i;

	egargs.macPort = (Enet_MacPort)macPort;
	ENET_IOCTL_SET_IN_ARGS(&prms, &egargs);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_ENABLE_PREEMPTION, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to enable frame premption: %d \n", status);
		return LLDENET_E_IOCTL;
	}

	EnetMacPort_SetPreemptMinFragSizeInArgs fragSizeInArgs = {
		.macPort = egargs.macPort,
		.preemptMinFragSize = 1,
	};
	ENET_IOCTL_SET_IN_ARGS(&prms, &fragSizeInArgs);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_SET_PREEMPT_MIN_FRAG_SIZE, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to set min frag size: %d \n", status);
		return LLDENET_E_IOCTL;
	}
	EnetMacPort_SetPreemptQueueInArgs queuePreemptInArgs = {
		.macPort = egargs.macPort,
	};
	for (i = 0; i < ENET_PRI_NUM; i++) {
		queuePreemptInArgs.queuePreemptCfg.preemptMode[i] = (cpemp->prioiry_preempt[i] == 1)?
			ENET_MAC_QUEUE_PREEMPT_MODE_EXPRESS: ENET_MAC_QUEUE_PREEMPT_MODE_PREEMPT;
	}
	ENET_IOCTL_SET_IN_ARGS(&prms, &queuePreemptInArgs);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_SET_PREEMPT_QUEUE, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to set preempt queue: %d \n", status);
		return LLDENET_E_IOCTL;
	}
	bool isIetEnabled = false;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &egargs, &isIetEnabled);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_GET_PREEMPTION_ENABLE_STATUS, &prms, status);
	if (status == ENET_SOK && isIetEnabled) {
		UB_LOG(UBL_INFO, "Framepreemption enabled! \n");
	}
	bool isIetActive = false;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &egargs, &isIetActive);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_GET_PREEMPTION_ACTIVE_STATUS, &prms, status);
	if (status == ENET_SOK && isIetActive) {
		UB_LOG(UBL_INFO, "FramePreemption actived \n");
	}

	EnetMacPort_QueuePreemptCfg queueCfg = {};
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &egargs, &queueCfg);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_GET_QUEUE_PREEMPT_STATUS, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "Failed to get preempt queue status: %d \n", status);
		return LLDENET_E_IOCTL;
	}
	for (i = 0; i < ENET_PRI_NUM; i++) {
		if (queuePreemptInArgs.queuePreemptCfg.preemptMode[i] != queueCfg.preemptMode[i]) {
			UB_LOG(UBL_ERROR, "Status of the priority %d is incorrect\n", i);
			return LLDENET_E_NOAVAIL;
		}
	}
	nevent->u.preempt.preempt_active = 1;// frame preemption active
	UB_LOG(UBL_INFO, "Frame preemption setup successfully!\n");
	status = LLDENET_E_OK;

#endif //ENET_CFG_IS_OFF(CPSW_IET_INCL)

	return status;
}

int LLDEnetEnableTCQueueMapping(LLDEnet_t *hLLDEnet, uint8_t macPort,
				cbl_qmap_params_t *cqp)
{
	/**
	* In MCU platform, since priority to queue mapping is configured
	* in the application when it opens CPSW driver, no need to invoke 
	* any driver api here.
	* */
	return LLDENET_E_OK;
}

int LLDEnetGetPortStats(LLDEnet_t *hLLDEnet, uint8_t port, void *prm, int prmSize)
{
	Enet_IoctlPrms prms;
	int32_t status;
	CpswStats_PortStats *portStatsPrm = (CpswStats_PortStats *)prm;

	if (prmSize < sizeof(CpswStats_PortStats)) {
		return LLDENET_E_PARAM;
	}
	if (port == 0xFFU) {
		/* Get host port stats info */
		ENET_IOCTL_SET_OUT_ARGS(&prms, portStatsPrm);
		ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
			   ENET_STATS_IOCTL_GET_HOSTPORT_STATS, &prms, status);
		if (status != ENET_SOK) {
			UB_LOG(UBL_ERROR, "%s, Failed to get stats of host port: %d \n",
			       __func__, status);
			return LLDENET_E_FAILURE;
		}
		return LLDENET_E_OK;

	}
	/* Get MAC port stats info */
	Enet_MacPort macPort = (Enet_MacPort)port;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &macPort, portStatsPrm);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_STATS_IOCTL_GET_MACPORT_STATS, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "%s, Failed to get stats of mac port: %d, status: %d \n",
		       __func__, port, status);
		return LLDENET_E_FAILURE;
	}

	return LLDENET_E_OK;
}

void LLDEnetResetPortStats(LLDEnet_t *hLLDEnet, uint8_t port)
{
	Enet_IoctlPrms prms;
	int32_t status;

	if (port == 0xFFU) {
		ENET_IOCTL_SET_NO_ARGS(&prms);
		ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
			   ENET_STATS_IOCTL_RESET_HOSTPORT_STATS, &prms, status);
		if (status != ENET_SOK) {
			UB_LOG(UBL_ERROR, "%s, Failed to reset host port, status: %d \n",
			       __func__, status);
			return;
		}
		return;
	}
	Enet_MacPort macPort = (Enet_MacPort)port;
	ENET_IOCTL_SET_IN_ARGS(&prms, &macPort);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_STATS_IOCTL_RESET_MACPORT_STATS, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "%s, Failed to reset mac port: %d, status: %d \n",
		       __func__, port, status);
	}
}

