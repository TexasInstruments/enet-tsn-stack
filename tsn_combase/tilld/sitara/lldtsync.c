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

typedef struct
{
	int (*getRxTime)(const LLDTSync_t *hTSync, uint8_t rxPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);

	int (*getTxTime)(const LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);

	int (*adjTsSourceFreq)(const LLDTSync_t *hTSync, int ppb);

	int (*setTsSourceTime)(const LLDTSync_t *hTSync, uint64_t ts);

	int (*getTsSourceTime)(const LLDTSync_t *hTSync, uint64_t *ts);

	int (*enableTsEvent)(const LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts);

	int (*shiftTsSourceTime)(const LLDTSync_t *hTSync, int64_t offset);

	int (*phyWaitTxTs)(LLDTSync_t *hTSync, uint8_t txPort, int msgType,
						uint16_t seqId, uint8_t domain);
}LLDTsync_driver;

struct LLDTSync {
	Enet_Type enetType;
	uint32_t instId;
	Enet_Handle hEnet;
	uint32_t coreId;
	LLDTsyncTsSource tsSource;
	LLDTsync_driver *tsyncDrv;
};

static int LLDTsyncGetCptsRxTime(const LLDTSync_t *hTSync, uint8_t rxPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);
static int LLDTsyncGetCptsTxTime(const LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);
static int LLDTSyncAdjCptsFreq(const LLDTSync_t *hTSync, int ppb);
static int LLDTSyncSetCptsTime(const LLDTSync_t *hTSync, uint64_t ts);
static int LLDTSyncGetCptsTime(const LLDTSync_t *hTSync, uint64_t *ts);
static int LLDTSyncEnableCptsTsEvent(const LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts);
static int LLDTSyncShiftCptsTime(const LLDTSync_t *hTSync, int64_t offset);


#if !ENET_ENABLE_PER_ICSSG
static int LLDTsyncGetPhyRxTime(const LLDTSync_t *hTSync, uint8_t rxPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);
static int LLDTsyncGetPhyTxTime(const LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);
static int LLDTSyncAdjPhyFreq(const LLDTSync_t *hTSync, int ppb);
static int LLDTSyncSetPhyTime(const LLDTSync_t *hTSync, uint64_t ts);
static int LLDTSyncGetPhyTime(const LLDTSync_t *hTSync, uint64_t *ts);
static int LLDTSyncEnablePhyTsEvent(const LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts);
static int LLDTSyncShiftPhyTime(const LLDTSync_t *hTSync, int64_t offset);
static int LLDTsyncPhyWaitTxTsDrv(LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain);
#endif

LLDTsync_driver gLLDTSyncCptsDriver = {
	.getRxTime = LLDTsyncGetCptsRxTime,
	.getTxTime = LLDTsyncGetCptsTxTime,
	.adjTsSourceFreq = LLDTSyncAdjCptsFreq,
	.setTsSourceTime = LLDTSyncSetCptsTime,
	.getTsSourceTime = LLDTSyncGetCptsTime,
	.enableTsEvent = LLDTSyncEnableCptsTsEvent,
	.shiftTsSourceTime = LLDTSyncShiftCptsTime,
	.phyWaitTxTs = NULL,
};

#if !ENET_ENABLE_PER_ICSSG
LLDTsync_driver gLLDTSyncPhyDriver = {
	.getRxTime = LLDTsyncGetPhyRxTime,
	.getTxTime = LLDTsyncGetPhyTxTime,
	.adjTsSourceFreq = LLDTSyncAdjPhyFreq,
	.setTsSourceTime = LLDTSyncSetPhyTime,
	.getTsSourceTime = LLDTSyncGetPhyTime,
	.enableTsEvent = LLDTSyncEnablePhyTsEvent,
	.shiftTsSourceTime = LLDTSyncShiftPhyTime,
	.phyWaitTxTs = LLDTsyncPhyWaitTxTsDrv,
};
#endif

#ifndef CB_LLDTSYNC_MMEM
#define CB_LLDTSYNC_MMEM lldtsync_mem
#endif

#ifndef CB_LLDTSYNC_INSTNUM
#define CB_LLDTSYNC_INSTNUM 16
#endif

UB_SD_GETMEM_DEF(CB_LLDTSYNC_MMEM, (int)sizeof(LLDTSync_t), CB_LLDTSYNC_INSTNUM);

LLDTSync_t *LLDTSyncOpen(LLDTSyncCfg_t *cfg, LLDTsyncTsSource tsSource)
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

	hTSync->tsSource = tsSource;

	#if !ENET_ENABLE_PER_ICSSG
	if (hTSync->tsSource == LLDTSYNC_TS_SOURCE_CPTS) {
		hTSync->tsyncDrv = &gLLDTSyncCptsDriver;
	}
	else if (hTSync->tsSource == LLDTSYNC_TS_SOURCE_PHY) {
		hTSync->tsyncDrv = &gLLDTSyncPhyDriver;
	}
	else {
		UB_LOG(UBL_ERROR,"Invalid Timestamp Source");
		hTSync->tsyncDrv = NULL;
		LLDTSyncClose(hTSync);
		hTSync = NULL;
	}
	#else /* ICSSG Peripheral */
	hTSync->tsyncDrv = &gLLDTSyncCptsDriver;
	#endif



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
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if ((hTSync == NULL) || (ts == NULL)) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->getRxTime != NULL)
	{
		retval = drv->getRxTime(hTSync, rxPort, msgType, seqId, domain, ts);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

int LLDTSyncGetTxTime(LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts)
{
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if ((hTSync == NULL) || (ts == NULL)) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->getTxTime != NULL)
	{
		retval = drv->getTxTime(hTSync, txPort, msgType, seqId, domain, ts);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

int LLDTSyncAdjFreq(LLDTSync_t *hTSync, int ppb)
{
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->adjTsSourceFreq != NULL)
	{
		retval = drv->adjTsSourceFreq(hTSync, ppb);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

int LLDTSyncSetTime(LLDTSync_t *hTSync, uint64_t ts)
{
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->setTsSourceTime != NULL)
	{
		retval = drv->setTsSourceTime(hTSync, ts);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

int LLDTSyncGetTime(LLDTSync_t *hTSync, uint64_t *ts)
{
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if ((hTSync == NULL) || (ts == NULL)) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->getTsSourceTime != NULL)
	{
		retval = drv->getTsSourceTime(hTSync, ts);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

int LLDTSyncEnableTsEvent(LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts)
{
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->enableTsEvent != NULL)
	{
		retval = drv->enableTsEvent(hTSync, ports, numPorts);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

int LLDTSyncShiftTime(LLDTSync_t *hTSync, int64_t offset)
{
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->shiftTsSourceTime != NULL)
	{
		retval = drv->shiftTsSourceTime(hTSync, offset);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

static int LLDTsyncGetCptsRxTime(const LLDTSync_t *hTSync, uint8_t rxPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts)
{
	int32_t status = ENET_SOK;
	int retval = LLDENET_E_IOCTL;
	/* For ENET-ICSSG the dmaPktInfo has the timestamp, this API is not valid */
	if (Enet_isCpswFamily(hTSync->enetType) == false) {
		return LLDENET_E_UNSUPPORT;
	}
	EnetTimeSync_GetEthTimestampInArgs inArgs;
	inArgs.domain  = (uint32_t)domain;
	inArgs.msgType = (EnetTimeSync_MsgType)msgType;
	inArgs.portNum = (uint32_t)rxPort;
	inArgs.seqId   = (uint32_t)seqId;
	status = EnetApp_getRxTimeStamp(hTSync->hEnet, hTSync->coreId, &inArgs, ts);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl GET_ETH_RX_TIMESTAMP failed %d\n", status);
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTsyncGetCptsTxTime(const LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetTimeSync_GetEthTimestampInArgs inArgs;
	inArgs.msgType = (EnetTimeSync_MsgType)msgType;
	inArgs.seqId   = (uint32_t)seqId;
	inArgs.portNum = (uint32_t)txPort;
	inArgs.domain  = (uint32_t)domain;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, ts);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			ENET_TIMESYNC_IOCTL_GET_ETH_TX_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl GET_ETH_TX_TIMESTAMP failed %d\n", status);
		retval = LLDENET_E_IOCTL;
	}
	retval = LLDENET_E_OK;
	return retval;
}


static int LLDTSyncAdjCptsFreq(const LLDTSync_t *hTSync, int ppb)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetTimeSync_TimestampAdj adjTsInArgs = {
		.adjValInNsecs   = ppb,
		.intervalInNsecs = 1000000000U,
	};
	ENET_IOCTL_SET_IN_ARGS(&prms, &adjTsInArgs);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			ENET_TIMESYNC_IOCTL_ADJUST_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl ADJUST_TIMESTAMP failed %d\n", status);
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTSyncSetCptsTime(const LLDTSync_t *hTSync, uint64_t ts)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;

	EnetTimeSync_setTimestamp timestamp = {
		.tsLoadVal = ts,
		.clkMode = 0U,
		.clkSign = 0U
	};
	ENET_IOCTL_SET_IN_ARGS(&prms, &timestamp);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			ENET_TIMESYNC_IOCTL_SET_TIMESTAMP, &prms, status);

	if (status == ENET_SOK) {
		if(Enet_isIcssFamily(hTSync->enetType)) {
			EnetApp_setTimeStampComplete(hTSync->hEnet, hTSync->coreId);
		}
		retval = LLDENET_E_OK;
	}
	else {
		UB_LOG(UBL_ERROR,"Enet_ioctl SET_TIMESTAMP failed %d\n", status);
		retval = LLDENET_E_IOCTL;
	}
	return retval;
}

static int LLDTSyncGetCptsTime(const LLDTSync_t *hTSync, uint64_t *ts)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	/* Software Time stamp Push event */
	ENET_IOCTL_SET_OUT_ARGS(&prms, ts);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
			ENET_TIMESYNC_IOCTL_GET_CURRENT_TIMESTAMP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl GET_CURRENT_TIMESTAMP failed %d\n", status);
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}

	return retval;
}


static int LLDTSyncEnableCptsTsEvent(const LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts)
{
	uint32_t maxMacPorts = 0U;
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

#define MAX_TS_NUDGE_VAL 1000000
static int LLDTSyncShiftCptsTime(const LLDTSync_t *hTSync, int64_t offset)
{
	int retval = LLDENET_E_FAILURE;
	#if ENET_ENABLE_PER_CPSW
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	int32_t adjValInNsecs;
	if (hTSync==NULL) {
		return LLDENET_E_PARAM;
	}

	/* Limit the offset to avoid long time loop */
	if(llabs(offset) > MAX_TS_NUDGE_VAL) {
		if(offset>0){
			offset = MAX_TS_NUDGE_VAL;
		}else{
			offset = -MAX_TS_NUDGE_VAL;
		}
	}

	while(offset != 0){
		if(offset >= CPSW_CPTS_NUDGE_MAX_VAL) {
			adjValInNsecs = CPSW_CPTS_NUDGE_MAX_VAL;
		} else if (offset <= CPSW_CPTS_NUDGE_MIN_VAL) {
			adjValInNsecs = CPSW_CPTS_NUDGE_MIN_VAL;
		} else {
			adjValInNsecs = (int32_t)offset;
		}
		ENET_IOCTL_SET_IN_ARGS(&prms, &adjValInNsecs);
		ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
				CPSW_CPTS_IOCTL_SET_TS_NUDGE, &prms, status);
		if (status != ENET_SOK) {
			UB_LOG(UBL_ERROR,"Enet_ioctl SET_TS_NUDGE failed %d\n", status);
			retval = LLDENET_E_IOCTL;
			break;
		}
		offset -= (int64_t)adjValInNsecs;
	}
	#else
	/* ICSSG Case, just return ok! */
	retval = LLDENET_E_OK;
	#endif
	return retval;
}

#if !ENET_ENABLE_PER_ICSSG
int LLDTsyncPhyWaitTxTs(LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain)
{
	LLDTsync_driver* drv;
	int retval = LLDENET_E_FAILURE;

	if (hTSync == NULL) {
		return LLDENET_E_PARAM;
	}

	drv = hTSync->tsyncDrv;
	if (drv == NULL) {
		return LLDENET_E_PARAM;
	}

	if (drv->phyWaitTxTs != NULL)
	{
		retval = drv->phyWaitTxTs(hTSync, txPort, msgType, seqId, domain);
	}
	else
	{
		retval = LLDENET_E_UNSUPPORT;
	}

	return retval;
}

int LLDTSyncProcPhyStatusFrame(void* hEnet, uint8_t coreId , uint8_t txPort,
										 uint8_t* frame, uint32_t size)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status;
	Enet_IoctlPrms prms;
	EnetPhy_ProcStatusFrameInArgs inArgs = {
		.frame = frame,
	};
	inArgs.macPort = (Enet_MacPort)txPort;
	inArgs.size = (uint32_t)size;

	EnetPhy_ProcStatusFrameOutArgs outArgs = {0};
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &outArgs);
	ENET_IOCTL((Enet_Handle)hEnet, coreId,
					ENET_PHY_IOCTL_PROC_STATUS_FRAME, &prms, status);
	if (status == ENET_SOK) {
		retval = LLDENET_E_OK;
	}
	else {
		retval = LLDENET_E_IOCTL;
	}
	return retval;
}


static int LLDTsyncGetPhyRxTime(const LLDTSync_t *hTSync, uint8_t rxPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts)
{
	int32_t status = ENET_SOK;
	int retval = LLDENET_E_IOCTL;
	Enet_IoctlPrms prms;
	EnetPhy_PtpPktTimestampInArgs inArgs;
	inArgs.domain = domain;
	inArgs.msgType = msgType;
	inArgs.macPort = (Enet_MacPort)rxPort;
	inArgs.seqId = seqId;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, ts);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_GET_PTP_RXTS, &prms, status);

	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl ENET_PHY_IOCTL_GET_PTP_RXTS failed %d\n", status);
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTsyncGetPhyTxTime(const LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetPhy_PtpPktTimestampInArgs inArgs;
	inArgs.macPort = (Enet_MacPort)txPort;
	inArgs.domain  = (uint32_t)domain;
	inArgs.seqId   = (uint32_t)seqId;
	inArgs.msgType = (uint32_t)msgType;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, ts);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_GET_PTP_TXTS, &prms, status);

	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl ENET_PHY_IOCTL_GET_PTP_TXTS failed %d\n", status);
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTSyncAdjPhyFreq(const LLDTSync_t *hTSync, int ppb)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetPhy_AdjPtpFreqInArgs inArgs = {
		.macPort = ENET_MAC_PORT_1
	};
	inArgs.ppb = (int64_t)ppb;
	ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_ADJ_PTP_FREQ, &prms, status);
	if (status != ENET_SOK) {
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTSyncSetPhyTime(const LLDTSync_t *hTSync, uint64_t ts)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;

	EnetPhy_SetPtpTimeInArgs inArgs = {
		.ts64 = ts,
		.macPort = ENET_MAC_PORT_1,
	};
	ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_SET_PTP_TIME, &prms, status);
	if (status != ENET_SOK) {
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTSyncGetPhyTime(const LLDTSync_t *hTSync, uint64_t *ts)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetPhy_GenericInArgs inArgs = {
		.macPort = ENET_MAC_PORT_1,
	};
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, ts);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_GET_PTP_TIME, &prms, status);

	if (status != ENET_SOK) {
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTSyncEnablePhyTsEvent(const LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetPhy_EnablePtpInArgs inArgs = {
		.macPort = ENET_MAC_PORT_1,
		.on = 1,
		.srcMacStatusFrameType = 0
	};
	ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_ENABLE_PTP, &prms, status);
	if (status != ENET_SOK) {
		retval = LLDENET_E_IOCTL;
	}
	else {
		retval = LLDENET_E_OK;
	}
	return retval;
}

static int LLDTSyncShiftPhyTime(const LLDTSync_t *hTSync, int64_t offset)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status = ENET_SOK;
	Enet_IoctlPrms prms;
	EnetPhy_AdjPtpPhaseInArgs inArgs = {
		.macPort = ENET_MAC_PORT_1,
		.offset = offset
	};
	ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_ADJ_PTP_PHASE, &prms, status);
	if (status == ENET_SOK) {
		retval = LLDENET_E_OK;
	}
	else {
		retval = LLDENET_E_IOCTL;
	}
	return retval;
}

static int LLDTsyncPhyWaitTxTsDrv(LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain)
{
	int retval = LLDENET_E_FAILURE;
	int32_t status;
	Enet_IoctlPrms prms;
	EnetPhy_PtpPktTimestampInArgs inArgs;
	inArgs.macPort = (Enet_MacPort)txPort;
	inArgs.domain  = (uint32_t)domain;
	inArgs.seqId   = (uint32_t)seqId;
	inArgs.msgType = (uint32_t)msgType;
	ENET_IOCTL_SET_IN_ARGS(&prms, &inArgs);
	ENET_IOCTL(hTSync->hEnet, hTSync->coreId,
					ENET_PHY_IOCTL_WAIT_PTP_TXTS, &prms, status);
	if (status == ENET_SOK) {
		retval = LLDENET_E_OK;
	}
	else {
		retval = LLDENET_E_IOCTL;
	}
	return retval;
}
#endif
