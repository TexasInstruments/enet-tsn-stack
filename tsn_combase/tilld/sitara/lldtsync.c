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
/*
 * lldtsync.c
*/
#include <enet_mcm.h>
#include "tilld/lldtsync.h"
#include "combase_private.h"

extern int32_t EnetApp_setTimeStampComplete(Enet_Handle hEnet, uint32_t coreId);
extern int32_t EnetApp_enablePortTsEvent(Enet_Handle hEnet, uint32_t coreId,
				uint32_t macPort[], uint32_t numPorts);
extern int32_t EnetApp_getRxTimeStamp(Enet_Handle hEnet, uint32_t coreId,
				EnetTimeSync_GetEthTimestampInArgs* inArgs, uint64_t *ts);

struct LLDTSync {
	Enet_Type enetType;
	uint32_t instId;
	Enet_Handle hEnet;
	uint32_t coreId;
};

#ifndef CB_LLDTSYNC_MMEM
#define CB_LLDTSYNC_MMEM lldtsync_mem
#endif

#ifndef CB_LLDTSYNC_INSTNUM
#define CB_LLDTSYNC_INSTNUM 16
#endif

UB_SD_GETMEM_DEF(CB_LLDTSYNC_MMEM, (int)sizeof(LLDTSync_t), CB_LLDTSYNC_INSTNUM);

LLDTSync_t *LLDTSyncOpen(LLDTSyncCfg_t *cfg)
{
	LLDTSync_t *hTSync;
	EnetApp_HandleInfo handleInfo;

	if (cfg == NULL) {
		return NULL;
	}

	hTSync = (LLDTSync_t*)UB_SD_GETMEM(CB_LLDTSYNC_MMEM, sizeof(LLDTSync_t));
	EnetAppUtils_assert(hTSync != NULL);
	memset(hTSync, 0, sizeof(LLDTSync_t));

	hTSync->enetType = (Enet_Type)cfg->enetType;
	hTSync->instId = cfg->instId;
	hTSync->coreId = EnetSoc_getCoreId();

	EnetApp_acquireHandleInfo(hTSync->enetType, cfg->instId, &handleInfo);
	hTSync->hEnet = handleInfo.hEnet;
	EnetAppUtils_assert(hTSync->hEnet != NULL);

	return hTSync;
}

void LLDTSyncCfgInit(LLDTSyncCfg_t *cfg)
{
	if (cfg == NULL) {
		return;
	}
	memset(cfg, 0, sizeof(LLDTSyncCfg_t));
}

void LLDTSyncClose(LLDTSync_t *hTSync)
{
	if (hTSync == NULL) {
		return;
	}
	UB_SD_RELMEM(CB_LLDTSYNC_MMEM, hTSync);
}

int LLDTSyncGetRxTime(LLDTSync_t *hTSync, uint8_t rxPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts)
{
	int32_t status = ENET_SOK;
	EnetTimeSync_GetEthTimestampInArgs inArgs;

	if ((hTSync == NULL) || (ts == NULL)) {
		return LLDENET_E_PARAM;
	}
	/* For ENET-ICSSG the dmaPktInfo has the time stamp, this API is not valid */
	if (Enet_isCpswFamily(hTSync->enetType) == false) {
		return LLDENET_E_UNSUPPORT;
	}
	memset(&inArgs, 0, sizeof(inArgs));
	inArgs.msgType = (EnetTimeSync_MsgType)msgType;
	inArgs.seqId   = seqId;
	inArgs.portNum = rxPort;
	inArgs.domain  = domain;

	status = EnetApp_getRxTimeStamp(hTSync->hEnet, hTSync->coreId, &inArgs, ts);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl GET_ETH_RX_TIMESTAMP failed %d\n", status);
		return LLDENET_E_IOCTL;
	}

	return LLDENET_E_OK;
}

int LLDTSyncGetTxTime(LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts)
{
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetTimeSync_GetEthTimestampInArgs inArgs;

	if ((hTSync == NULL) || (ts == NULL)) {
		return LLDENET_E_PARAM;
	}

	inArgs.msgType = (EnetTimeSync_MsgType)msgType;
	inArgs.seqId = seqId;
	inArgs.portNum = txPort;
	inArgs.domain  = domain;

	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, ts);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			   ENET_TIMESYNC_IOCTL_GET_ETH_TX_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl GET_ETH_TX_TIMESTAMP failed %d\n", status);
		return LLDENET_E_IOCTL;
	}
	return LLDENET_E_OK;
}

int LLDTSyncAdjFreq(LLDTSync_t *hTSync, int ppb)
{
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetTimeSync_TimestampAdj adjTsInArgs;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}

	adjTsInArgs.adjValInNsecs   = ppb;
	adjTsInArgs.intervalInNsecs = 1000000000U;

	ENET_IOCTL_SET_IN_ARGS(&prms, &adjTsInArgs);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			   ENET_TIMESYNC_IOCTL_ADJUST_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl ADJUST_TIMESTAMP failed %d\n", status);
		return LLDENET_E_IOCTL;
	}

	return LLDENET_E_OK;
}

int LLDTSyncSetTime(LLDTSync_t *hTSync, uint64_t ts)
{
	int32_t status = ENET_SOK;
	EnetTimeSync_setTimestamp timestamp;
	Enet_IoctlPrms prms;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}

	memset((void *)(&timestamp), 0, sizeof(timestamp));
	timestamp.tsLoadVal = ts;
	timestamp.clkMode = 0U;
	timestamp.clkSign = 0U;
	ENET_IOCTL_SET_IN_ARGS(&prms, &timestamp);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			   ENET_TIMESYNC_IOCTL_SET_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl SET_TIMESTAMP failed %d\n", status);
		return LLDENET_E_IOCTL;
	}
	if(Enet_isIcssFamily(hTSync->enetType)) {
		EnetApp_setTimeStampComplete(hTSync->hEnet, hTSync->coreId);
	}
	return LLDENET_E_OK;
}

int LLDTSyncGetTime(LLDTSync_t *hTSync, uint64_t *ts)
{
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}
	/* Software Time stamp Push event */
	ENET_IOCTL_SET_OUT_ARGS(&prms, ts);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			   ENET_TIMESYNC_IOCTL_GET_CURRENT_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl GET_CURRENT_TIMESTAMP failed %d\n", status);
		return LLDENET_E_IOCTL;
	}

	return LLDENET_E_OK;
}

int LLDTSyncEnableTsEvent(LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts)
{
	uint32_t maxMacPorts = 0U;

	if ((hTSync == NULL) || (ports == NULL) || (numPorts == 0)) {
		return LLDENET_E_PARAM;
	}

	/* No MacPort TS Enable ioctl for ICSSG peripheral */
	if (Enet_isCpswFamily(hTSync->enetType)) {
		maxMacPorts = Enet_getMacPortMax(hTSync->enetType, hTSync->instId);
		if (numPorts > maxMacPorts) {
			numPorts = maxMacPorts;
		}
		EnetApp_enablePortTsEvent(hTSync->hEnet, hTSync->coreId, ports, numPorts);
	}

	return LLDENET_E_OK;
}
