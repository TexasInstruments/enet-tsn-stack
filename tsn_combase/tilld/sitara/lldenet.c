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
 * lldenet.c
*/
#include "lldtype.h"
#include "lldenet_priv.h"
#include "combase_private.h"
#include "lldenetext.h"

extern int32_t EnetApp_filterPriorityPacketsCfg(Enet_Handle hEnet, uint32_t coreId);
extern int32_t EnetApp_applyClassifier(Enet_Handle hEnet, uint32_t coreId, uint8_t *dstMacAddr, uint32_t vlanId,
										uint32_t ethType, uint32_t rxFlowIdx);

static LLDEnetTxDma_t s_lldTxDmaTable[LLDNET_TXDMA_CHANNEL_NUM];
static LLDEnetRxDma_t s_lldRxDmaTable[LLDNET_RXDMA_CHANNEL_NUM];

#define TXDMA_LOCK(hLLDTxDma) \
	{if(CB_THREAD_IS_MUTEX_INITIALIZED(hLLDTxDma->txMutex))\
		{CB_THREAD_MUTEX_LOCK(&hLLDTxDma->txMutex);}}
#define TXDMA_UNLOCK(hLLDTxDma) \
	{if(CB_THREAD_IS_MUTEX_INITIALIZED(hLLDTxDma->txMutex))\
		{CB_THREAD_MUTEX_UNLOCK(&hLLDTxDma->txMutex);}}

UB_SD_GETMEM_DEF(CB_LLDENET_MMEM, (int)sizeof(LLDEnet_t), CB_LLDENET_INSTNUM);

static LLDEnetTxDma_t *LLDEnetGetTxDma(int txChId)
{
	if ((txChId < 0) || (txChId >= LLDNET_TXDMA_CHANNEL_NUM)) {
		UB_LOG(UBL_ERROR,"Invalid TxChId: %d\n", txChId);
		return NULL;
	}
	return &s_lldTxDmaTable[txChId];
}

static LLDEnetRxDma_t *LLDEnetGetRxDma(int rxChId)
{
	if ((rxChId < 0) || (rxChId >= LLDNET_RXDMA_CHANNEL_NUM)) {
		UB_LOG(UBL_ERROR,"Invalid RxChId: %d\n", rxChId);
		return NULL;
	}
	return &s_lldRxDmaTable[rxChId];
}

/* This function is called inside the protected function to
 * protect hLLDRxDma->nSubRx from concurrent update */
static int LLDEnetSetRxDmaCfg(LLDEnet_t *hLLDEnet, LLDEnetCfg_t *cfg)
{
	uint32_t i = 0U;
	for(i = 0U; i < cfg->numRxChannels; i++) {
		LLDEnetRxDma_t *hLLDRxDma = hLLDEnet->hLLDRxDma[i];
		if (hLLDRxDma->nSubRx == LLDENET_SUB_RX_NUM) {
			UB_LOG(UBL_ERROR,"Reached sub Rx channel: %d\n", hLLDRxDma->nSubRx);
			return -1;
		}
		if (cfg->dmaRxOwner) {
			hLLDRxDma->nRxPkts = cfg->nRxPkts[i];
			if (hLLDRxDma->nRxPkts == 0) {
				hLLDRxDma->nRxPkts = ENET_MEM_NUM_RX_PKTS;
			}
			hLLDRxDma->pktSize = cfg->pktSize;
			if (hLLDRxDma->pktSize == 0) {
				hLLDRxDma->pktSize = ENET_MEM_LARGE_POOL_PKT_SIZE;
			}
			hLLDRxDma->rxOwnerNotifyCb = cfg->rxNotifyCb;
			hLLDRxDma->rxOwnerCbArg = cfg->rxCbArg;
			hLLDRxDma->rxChId = cfg->dmaRxChId[i];
			hLLDRxDma->nSubRx = 0;
			hLLDRxDma->dmaRxShared = cfg->dmaRxShared;
			UB_LOG(UBL_INFO,"rxChId %d has owner dmaRxShared %d\n",
				   hLLDRxDma->rxChId, hLLDRxDma->dmaRxShared);
		} else {
			if (!hLLDRxDma->dmaRxShared) {
				UB_LOG(UBL_ERROR,"rxChId %d is not shared by owner\n",
					   hLLDRxDma->rxChId);
				return -1;
			}
			/* If it is not the owner of DMA channel, only one DMA channel is supported */
			EnetAppUtils_assert(cfg->numRxChannels == 1);
			LLDEnetSubRx_t *subRx;
			hLLDEnet->subRxIndex = hLLDRxDma->nSubRx;
			subRx = &hLLDRxDma->subRxTable[hLLDEnet->subRxIndex];
			subRx->rxNotifyCb = cfg->rxNotifyCb;
			subRx->rxCbArg = cfg->rxCbArg;
			subRx->subRxIndex = hLLDRxDma->nSubRx;
			InitBufRing(&subRx->bufRing);

			UB_LOG(UBL_INFO,"subRxIndex %d reused rxChId %d dmaRxShared %d\n",
				   subRx->subRxIndex, hLLDRxDma->rxChId, hLLDRxDma->dmaRxShared);

			hLLDRxDma->nSubRx++;
		}
	}
	return 0;
}

static int LLDEnetSetTxDmaCfg(LLDEnet_t *hLLDEnet, LLDEnetCfg_t *cfg)
{
	LLDEnetTxDma_t *hLLDTxDma = hLLDEnet->hLLDTxDma;
	if (hLLDTxDma->refCount == 0) {

		hLLDTxDma->nTxPkts = cfg->nTxPkts;
		if (hLLDTxDma->nTxPkts == 0) {
			hLLDTxDma->nTxPkts = ENET_MEM_NUM_TX_PKTS;
		}
		hLLDTxDma->pktSize = cfg->pktSize;
		if (hLLDTxDma->pktSize == 0) {
			hLLDTxDma->pktSize = ENET_MEM_LARGE_POOL_PKT_SIZE;
		}
		hLLDTxDma->txNotifyCb = cfg->txNotifyCb;
		hLLDTxDma->txCbArg = cfg->txCbArg;
		hLLDTxDma->txChId = cfg->dmaTxChId;
		hLLDTxDma->dmaTxShared = cfg->dmaTxShared;
		hLLDEnet->dmaTxOwner = true;
	} else {
		if (!hLLDTxDma->dmaTxShared) {
			UB_LOG(UBL_ERROR,"Tx channel is not shared by owner\n");
			return -1;
		}
		hLLDEnet->dmaTxOwner = false;
	}
	hLLDTxDma->refCount++;
	return 0;
}

static void DmaBufQInit(EnetDma_PktQ *q, void *user, int nPkts, uint32_t pktSize)
{
	int i;
	EnetDma_Pkt *pktInfo;
	uint32_t scatterSegments[1];

	EnetQueue_initQ(q);

	scatterSegments[0] = pktSize;
	for (i = 0; i < nPkts; i++) {
		pktInfo = EnetMem_allocEthPkt(user, ENETDMA_CACHELINE_ALIGNMENT,
							ENET_ARRAYSIZE(scatterSegments),
							scatterSegments);
		EnetAppUtils_assert(pktInfo != NULL);
		ENET_UTILS_SET_PKT_APP_STATE(&pktInfo->pktState, ENET_PKTSTATE_APP_WITH_FREEQ);

		EnetQueue_enq(q, &pktInfo->node);
	}
}

static void DmaTxQInit(LLDEnetTxDma_t *hLLDTxDma)
{
	DmaBufQInit(&hLLDTxDma->txFreePktInfoQ, NULL,
				hLLDTxDma->nTxPkts, hLLDTxDma->pktSize);
}

static void DmaRxQInit(LLDEnetRxDma_t *hLLDRxDma)
{
	EnetDma_PktQ rxReadyQ;
	int32_t status;

	EnetQueue_initQ(&rxReadyQ);

	DmaBufQInit(&hLLDRxDma->rxFreeQ, (void *)hLLDRxDma,
				hLLDRxDma->nRxPkts, hLLDRxDma->pktSize);

	/* Retrieve any CPSW packets which are ready */
	status = EnetDma_retrieveRxPktQ(hLLDRxDma->hRxCh, &rxReadyQ);
	EnetAppUtils_assert(status == ENET_SOK);
	/* There should not be any packet with DMA during init */
	EnetAppUtils_assert(EnetQueue_getQCount(&rxReadyQ) == 0U);

	EnetAppUtils_validatePacketState(&hLLDRxDma->rxFreeQ,
			ENET_PKTSTATE_APP_WITH_FREEQ, ENET_PKTSTATE_APP_WITH_DRIVER);

	EnetDma_submitRxPktQ(hLLDRxDma->hRxCh, &hLLDRxDma->rxFreeQ);

	/* Assert here as during init no. of DMA descriptors should be equal to
	 * no. of free Ethernet buffers available with app */
	EnetAppUtils_assert(0U == EnetQueue_getQCount(&hLLDRxDma->rxFreeQ));
}

static void LLDEnetTxNotifyCb(void *cbArg);
static void LLDEnetRxNotifyCb(void *cbArg);

static int DmaRxOpen(LLDEnet_t *hLLDEnet)
{
	EnetApp_GetDmaHandleInArgs rxInArgs;
	EnetApp_GetRxDmaHandleOutArgs rxChInfo;
	uint32_t i = 0U;

	for (i = 0U; i < hLLDEnet->numRxChannels; i++) {
		LLDEnetRxDma_t *hLLDRxDma = hLLDEnet->hLLDRxDma[i];
		hLLDRxDma->pRxReadyQ = &hLLDEnet->rxReadyQ;
		/* Open the RX channel */
		rxInArgs.notifyCb = LLDEnetRxNotifyCb;
		rxInArgs.cbArg = hLLDRxDma;
		EnetApp_getRxDmaHandle(hLLDRxDma->rxChId, &rxInArgs, &rxChInfo);

#ifdef ENET_SOC_HOSTPORT_DMA_TYPE_UDMA
		hLLDRxDma->rxFlowStartIdx = rxChInfo.rxFlowStartIdx;
		hLLDRxDma->rxFlowIdx = rxChInfo.rxFlowIdx;
#else
		hLLDRxDma->rxFlowIdx = rxChInfo.rxChNum;
#endif
		hLLDRxDma->hRxCh = rxChInfo.hRxCh;
		EnetAppUtils_assert(hLLDRxDma->hRxCh != NULL);

		/* Submit all ready RX buffers to DMA.*/
		DmaRxQInit(hLLDRxDma);
	}
	/* init the rxReadyQ */
	EnetQueue_initQ(&hLLDEnet->rxReadyQ);
	return 0;
}

static int DmaTxOpen(LLDEnet_t *hLLDEnet)
{
	int32_t status = ENET_SOK;
	int res;
	EnetApp_GetDmaHandleInArgs txInArgs;
	EnetApp_GetTxDmaHandleOutArgs txChInfo;
	LLDEnetTxDma_t *hLLDTxDma = hLLDEnet->hLLDTxDma;

	/* Open the TX channel */
	txInArgs.cbArg = hLLDTxDma;
	txInArgs.notifyCb = LLDEnetTxNotifyCb;
	EnetApp_getTxDmaHandle(hLLDTxDma->txChId, &txInArgs, &txChInfo);

	hLLDTxDma->txChNum = txChInfo.txChNum;
	hLLDTxDma->hTxCh = txChInfo.hTxCh;
	EnetAppUtils_assert(hLLDTxDma->hTxCh != NULL);

	DmaTxQInit(hLLDTxDma);

	status = EnetDma_enableTxEvent(hLLDTxDma->hTxCh);
	EnetAppUtils_assert(status == ENET_SOK);

	if(hLLDTxDma->dmaTxShared) {
		/* For shared TX DMA, we need to initialize the mutex */
		res=CB_THREAD_MUTEX_INIT(&hLLDTxDma->txMutex, NULL);
		EnetAppUtils_assert(res == 0);
	}

	return 0;
}

static uint32_t LLDEnetRetrieveFreeTxPkts(EnetDma_TxChHandle hTxCh,
					EnetDma_PktQ *txPktInfoQ)
{
	EnetDma_PktQ txFreeQ;
	EnetDma_Pkt *pktInfo;
	uint32_t txFreeQCnt = 0U;
	int32_t status;

	EnetQueue_initQ(&txFreeQ);

	/* Retrieve any packets that may be free now */
	status = EnetDma_retrieveTxPktQ(hTxCh, &txFreeQ);
	if (status == ENET_SOK) {
		txFreeQCnt = EnetQueue_getQCount(&txFreeQ);

		pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&txFreeQ);
		while (NULL != pktInfo) {
			EnetDma_checkPktState(&pktInfo->pktState, ENET_PKTSTATE_MODULE_APP,
				ENET_PKTSTATE_APP_WITH_DRIVER, ENET_PKTSTATE_APP_WITH_FREEQ);

			EnetQueue_enq(txPktInfoQ, &pktInfo->node);
			pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&txFreeQ);
		}
	}

	return txFreeQCnt;
}

static void DmaRxClose(LLDEnet_t *hLLDEnet)
{
	EnetDma_PktQ fqPktInfoQ;
	EnetDma_PktQ cqPktInfoQ;
	uint32_t i = 0U;

	for (i = 0U; i < hLLDEnet->numRxChannels; i++) {
		LLDEnetRxDma_t *hLLDRxDma = hLLDEnet->hLLDRxDma[i];
		/* Close RX Flow */
		if ((hLLDRxDma != NULL) && (hLLDRxDma->hRxCh != NULL)) {
			EnetQueue_initQ(&fqPktInfoQ);
			EnetQueue_initQ(&cqPktInfoQ);

			EnetApp_closeRxDma(hLLDRxDma->rxChId, hLLDEnet->hEnet, hLLDEnet->coreKey,
							   hLLDEnet->coreId, &fqPktInfoQ, &cqPktInfoQ);
			EnetAppUtils_freePktInfoQ(&fqPktInfoQ);
			EnetAppUtils_freePktInfoQ(&cqPktInfoQ);
			EnetAppUtils_freePktInfoQ(&hLLDRxDma->rxFreeQ);
			/* The LLDEnetRxNotifyCb() can push the packets into the rxReadyQ.
			 * We need to release them here as well. */
			EnetAppUtils_freePktInfoQ(&hLLDEnet->rxReadyQ);
			EnetQueue_initQ(&hLLDEnet->rxReadyQ);
			memset(hLLDRxDma, 0, sizeof(LLDEnetRxDma_t));
		}
	}
}

static void DmaTxCloseRefcount(LLDEnet_t *hLLDEnet)
{
	EnetDma_PktQ fqPktInfoQ;
	EnetDma_PktQ cqPktInfoQ;
	LLDEnetTxDma_t *hLLDTxDma = hLLDEnet->hLLDTxDma;
	if(!hLLDTxDma || !hLLDTxDma->refCount){
		return;
	}
	hLLDTxDma->refCount--;
	if(hLLDTxDma->refCount){
		return;
	}
	/* Close TX channel */
	if ((hLLDTxDma != NULL) && (hLLDTxDma->hTxCh != NULL)) {
		EnetQueue_initQ(&fqPktInfoQ);
		EnetQueue_initQ(&cqPktInfoQ);

		/* Retrieve any pending TX packets from driver */
		LLDEnetRetrieveFreeTxPkts(hLLDTxDma->hTxCh, &hLLDTxDma->txFreePktInfoQ);

		EnetApp_closeTxDma(hLLDTxDma->txChId, hLLDEnet->hEnet, hLLDEnet->coreKey,
					   hLLDEnet->coreId, &fqPktInfoQ, &cqPktInfoQ);

		EnetAppUtils_freePktInfoQ(&fqPktInfoQ);
		EnetAppUtils_freePktInfoQ(&cqPktInfoQ);

		EnetAppUtils_freePktInfoQ(&hLLDTxDma->txFreePktInfoQ);
		if(CB_THREAD_IS_MUTEX_INITIALIZED(hLLDTxDma->txMutex)) {
			CB_THREAD_MUTEX_DESTROY(&hLLDTxDma->txMutex);
		}
		memset(hLLDTxDma, 0, sizeof(LLDEnetTxDma_t));
	}
}

static bool IsMacAddrSet(uint8_t *mac)
{
	return ((mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5]) != 0);
}

int LLDEnetFilter(LLDEnet_t *hLLDEnet, uint8_t *dstMacAddr,
				  uint32_t vlanId, uint32_t ethType)
{
	LLDEnetRxDma_t *hLLDRxDma;
	int32_t status = ENET_SOK;
	uint32_t i;

	if ((hLLDEnet == NULL) || (dstMacAddr == NULL)) {
		return LLDENET_E_PARAM;
	}
	for (i = 0U; i < hLLDEnet->numRxChannels; i++) {
		hLLDRxDma = hLLDEnet->hLLDRxDma[i];
		if (hLLDRxDma == NULL) {
			return LLDENET_E_DENY;
		}
		if ((IsMacAddrSet(dstMacAddr) == false) && (ethType == 0)) {
			return LLDENET_E_PARAM;
		}
		if (hLLDEnet->dmaRxOwner == false) {
			/* For LLDEnet with shared Rx DMA, only one DMA channel is supported */
			EnetAppUtils_assert(hLLDEnet->numRxChannels == 1);
			LLDEnetSubRx_t *subRx = &hLLDEnet->hLLDRxDma[i]->subRxTable[hLLDEnet->subRxIndex];
			if (vlanId > 0) {
				subRx->rxFilter.vlanId = vlanId;
			}
			if (ethType > 0) {
				subRx->rxFilter.ethType = ethType;
			}
			if (IsMacAddrSet(dstMacAddr)) {
				memcpy(subRx->rxFilter.dstMacAddr, dstMacAddr, ENET_MAC_ADDR_LEN);
			}
		} else {
			if (vlanId > 0) {
				hLLDRxDma->ownerFilter.vlanId = vlanId;
			}
			if (ethType > 0) {
				hLLDRxDma->ownerFilter.ethType = ethType;
			}
			if (IsMacAddrSet(dstMacAddr)) {
				memcpy(hLLDRxDma->ownerFilter.dstMacAddr, dstMacAddr, ENET_MAC_ADDR_LEN);
			}
		}
	}

	/* This is a work around for now, CPSW only has one dma channel, so [0] is fine.
	 * and the current ICSSG classifier is not exactly based on dma channel number
	 * but the first dma channel number matches the ICSSG queue number needed for classification.
	 * TODO: refine this logic */
	hLLDRxDma = hLLDEnet->hLLDRxDma[0];

	/* For the Sitara AM273x SoC, setting dmaRxShared to true is required.
	 * Policers/classifiers aren't effective on this SoC, and configuring them could
	 * lead to a shortage when enabling multiple TSN modules as it supports only 4.*/
	if(hLLDRxDma->dmaRxShared == false) {
		status = EnetApp_applyClassifier(hLLDEnet->hEnet, hLLDEnet->coreId, dstMacAddr,
										 vlanId, ethType, hLLDRxDma->rxFlowIdx);
	}
	else
	{
		status = EnetApp_filterPriorityPacketsCfg(hLLDEnet->hEnet, hLLDEnet->coreId);
	}

	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Enet_ioctl failed %d\n", status);
		return LLDENET_E_IOCTL;
	}
	return LLDENET_E_OK;
}

static bool LLDEnet_isStatusFrame(EthFrame *frame)
{
	bool isStatusFrame = false;
	if ((frame->hdr.srcMac[0] == 0x08) &&
		(frame->hdr.srcMac[1] == 0x00))
	{
		isStatusFrame = true;
	}

	return isStatusFrame;
}

static uint32_t LLDEnetReceiveRxReadyPkts(LLDEnetRxDma_t *hLLDRxDma)
{
	EnetDma_PktQ rxTempQ;
	int32_t status;
	uint32_t rxReadyCnt = 0U;

	EnetQueue_initQ(&rxTempQ);

	/* Retrieve any CPSW packets which are ready */
	status = EnetDma_retrieveRxPktQ(hLLDRxDma->hRxCh, &rxTempQ);
	rxReadyCnt = EnetQueue_getQCount(&rxTempQ);
	if (status == ENET_SOK) {
		EnetAppUtils_validatePacketState(&rxTempQ,
										 ENET_PKTSTATE_APP_WITH_DRIVER,
										 ENET_PKTSTATE_APP_WITH_READYQ);
		EnetQueue_append(hLLDRxDma->pRxReadyQ, &rxTempQ);
	}

	return rxReadyCnt;
}

static void LLDEnetRxNotifyCb(void *cbArg)
{
	uint32_t rxReadyCnt = 0U;
	LLDEnetRxDma_t *hLLDRxDma = (LLDEnetRxDma_t*)cbArg;

	if (hLLDRxDma == NULL) {
		return;
	}

	rxReadyCnt = LLDEnetReceiveRxReadyPkts(hLLDRxDma);
	if ((rxReadyCnt == 0U) || (hLLDRxDma->rxOwnerNotifyCb == NULL)) {
		return;
	}
	hLLDRxDma->rxOwnerNotifyCb(hLLDRxDma->rxOwnerCbArg);
}

static uint32_t LLDEnetRetrieveTxDonePkts(LLDEnetTxDma_t *hLLDTxDma)
{
	EnetDma_PktQ txFreeQ;
	int32_t status;
	uint32_t txFreeQCnt = 0U;

	EnetQueue_initQ(&txFreeQ);

	/* Retrieve any CPSW packets that may be free now */
	status = EnetDma_retrieveTxPktQ(hLLDTxDma->hTxCh, &txFreeQ);
	txFreeQCnt = EnetQueue_getQCount(&txFreeQ);
	if (status == ENET_SOK) {
		EnetAppUtils_validatePacketState(&txFreeQ,
										 ENET_PKTSTATE_APP_WITH_DRIVER,
										 ENET_PKTSTATE_APP_WITH_FREEQ);
		EnetQueue_append(&hLLDTxDma->txFreePktInfoQ, &txFreeQ);
	}

	return txFreeQCnt;
}

static void LLDEnetTxNotifyCb(void *cbArg)
{
	uint32_t pktCount;
	LLDEnetTxDma_t *hLLDTxDma = (LLDEnetTxDma_t*)cbArg;

	if (hLLDTxDma == NULL) {
		return;
	}

	pktCount = LLDEnetRetrieveTxDonePkts(hLLDTxDma);
	if ((pktCount == 0U) || (hLLDTxDma->txNotifyCb == NULL)) {
		return;
	}
	hLLDTxDma->txNotifyCb(hLLDTxDma->txCbArg);
}

LLDEnet_t *LLDEnetOpen(LLDEnetCfg_t *cfg)
{
	EnetApp_HandleInfo handleInfo;
	EnetPer_AttachCoreOutArgs attachInfo;
	LLDEnet_t *hLLDEnet;
	int res;

	if (cfg == NULL) {
		UB_LOG(UBL_ERROR,"%s:invalid param\n", __func__);
		return NULL;
	}

	hLLDEnet = (LLDEnet_t*)UB_SD_GETMEM(CB_LLDENET_MMEM, sizeof(LLDEnet_t));
	EnetAppUtils_assert(hLLDEnet != NULL);
	memset(hLLDEnet, 0, sizeof(LLDEnet_t));

	hLLDEnet->coreId = EnetSoc_getCoreId();
	EnetApp_acquireHandleInfo((Enet_Type)cfg->enetType, cfg->instId, &handleInfo);
	EnetApp_coreAttach((Enet_Type)cfg->enetType, cfg->instId,
			   hLLDEnet->coreId, &attachInfo);
	EnetAppUtils_assert(handleInfo.hEnet != NULL);

	hLLDEnet->hEnet = handleInfo.hEnet;
	hLLDEnet->coreKey = attachInfo.coreKey;
	hLLDEnet->enetType = (Enet_Type)cfg->enetType;
	hLLDEnet->instId = cfg->instId;
	hLLDEnet->numRxChannels = cfg->numRxChannels;
	if (cfg->unusedDmaRx == false) {
		uint32_t i = 0U;
		EnetAppUtils_assert(
			(cfg->numRxChannels > 0) &&
			(cfg->numRxChannels <= MAX_NUM_RX_DMA_CH_PER_INSTANCE));
		for (i = 0U; i < cfg->numRxChannels; i++) {
			/* get RX dma handle */
			hLLDEnet->hLLDRxDma[i] = LLDEnetGetRxDma(cfg->dmaRxChId[i]);
			EnetAppUtils_assert(hLLDEnet->hLLDRxDma[i] != NULL);
			hLLDEnet->hLLDRxDma[i]->rxChId = cfg->dmaRxChId[i];
			EnetAppUtils_assert(cfg->dmaRxChId[i]>=0);
			/* At first hLLDRxDma->hasOwner is false, user need to set the
			 * cfg->dmaRxOwner to true.
			 * When the hLLDRxDma->hasOwner is true, user can not set cfg->dmaRxOwner
			 * to true because only one can own the RX DMA channel */
			EnetAppUtils_assert(cfg->dmaRxOwner != hLLDEnet->hLLDRxDma[i]->hasOwner);
		}
		UB_PROTECTED_FUNC(LLDEnetSetRxDmaCfg, res, hLLDEnet, cfg);
		EnetAppUtils_assert(res == 0);
		if (cfg->dmaRxOwner) {
			DmaRxOpen(hLLDEnet);
		}
		hLLDEnet->dmaRxOwner = cfg->dmaRxOwner;
		if (hLLDEnet->dmaRxOwner) {
			for (i = 0U; i < cfg->numRxChannels; i++) {
				hLLDEnet->hLLDRxDma[i]->hasOwner = true;
			}
		}
		if (IsMacAddrSet(cfg->dstMacAddr) || (cfg->ethType > 0)) {
			res = LLDEnetFilter(hLLDEnet, cfg->dstMacAddr, cfg->vlanId,
					cfg->ethType);
			EnetAppUtils_assert(res == 0);
		}
	}
	if (cfg->unusedDmaTx == false) {
		/* get TX dma handle */
		hLLDEnet->hLLDTxDma = LLDEnetGetTxDma(cfg->dmaTxChId);
		EnetAppUtils_assert(hLLDEnet->hLLDTxDma != NULL);
		UB_PROTECTED_FUNC(LLDEnetSetTxDmaCfg, res, hLLDEnet, cfg);
		EnetAppUtils_assert(res == 0);
		if(hLLDEnet->dmaTxOwner) {
			DmaTxOpen(hLLDEnet);
		}
	}

	return hLLDEnet;
}

void LLDEnetClose(LLDEnet_t *hLLDEnet)
{
	if (hLLDEnet == NULL) {
		return;
	}
	if (hLLDEnet->dmaRxOwner) {
		DmaRxClose(hLLDEnet);
	}
	UB_PROTECTED_FUNC_VOID(DmaTxCloseRefcount, hLLDEnet);

	EnetApp_coreDetach(hLLDEnet->enetType, hLLDEnet->instId,
					   hLLDEnet->coreId, hLLDEnet->coreKey);
	memset(hLLDEnet, 0, sizeof(LLDEnet_t));
	UB_SD_RELMEM(CB_LLDENET_MMEM, hLLDEnet);
}

static int FlattenScatterFrame(LLDEnetFrameScatter_t *frame,
			uint8_t *buf, uint32_t bufsize, uint32_t *filledsize)
{
	int i;
	int frame_size=0;

	for (i = 0; i < frame->nBufs; i++) {
		if((frame->buf[i]==NULL) || (frame->size[i]==0)) {
			return LLDENET_E_PARAM;
		}
		/* check if enough buffer */
		if((frame_size + frame->size[i]) > bufsize) {
			return LLDENET_E_BUFSIZE;
		}
		memcpy(buf+frame_size, frame->buf[i], frame->size[i]);
		frame_size+=frame->size[i];
	}
	*filledsize = frame_size;
	return LLDENET_E_OK;
}

int LLDEnetSendMultiScatter(LLDEnet_t *hLLDEnet, LLDEnetFrameScatter_t *frames,
							uint32_t nFrames)
{
	EnetDma_PktQ txSubmitQ;
	EnetDma_Pkt *pktInfo;
	LLDEnetTxDma_t *hLLDTxDma;
	int status;
	int res=LLDENET_E_OK;
	int i = 0;

	if ((hLLDEnet == NULL) || (frames == NULL) || (nFrames == 0)) {
		return LLDENET_E_PARAM;
	}
	hLLDTxDma = hLLDEnet->hLLDTxDma;
	/* when TX DMA is not used */
	if (hLLDTxDma == NULL) {
		return LLDENET_E_DENY;
	}

	TXDMA_LOCK(hLLDTxDma);

	/* Don't return in the middle from here, use goto to avoid missing TXDMA_UNLOCK */
	if (EnetQueue_getQCount(&hLLDTxDma->txFreePktInfoQ) < nFrames) {
		res=LLDENET_E_NOBUF;
		goto exit;
	}

	EnetQueue_initQ(&txSubmitQ);

	for (i = 0; i < nFrames; i++) {
		pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&hLLDTxDma->txFreePktInfoQ);
		EnetAppUtils_assert(pktInfo != NULL);

		res = FlattenScatterFrame(&frames[i],
								  pktInfo->sgList.list[0].bufPtr,
								  pktInfo->sgList.list[0].segmentAllocLen,
								  &pktInfo->sgList.list[0].segmentFilledLen);
		if(res != LLDENET_E_OK) {
			goto exit;
		}
		if(Enet_isIcssFamily(hLLDEnet->enetType)) {
			//TODO: Other than PTP we will not handle this
			/* Time stamp only Event msg class packets.
			 * IEEE 802.1AS-2020 - clause 11.4.2.2 - only
			 * Sync, Pdelay_Req, Pdelay_Resp are Event class */
			if(frames[i].nBufs==1){ /* PTP should have single buf */
				EthFrame *txframe = (EthFrame *)frames[i].buf[0];
				uint8_t *payload = txframe->payload;
				if(PTP_HEAD_MSGTYPE(payload) < 8U) {
					pktInfo->tsInfo.enableHostTxTs = true;
					pktInfo->txTsId = PTP_HEAD_SEQID(payload);
				}
			}
		}
		pktInfo->appPriv = (void *)hLLDEnet;
		EnetAppUtils_assert(frames[i].port >= 0U);
		pktInfo->txPortNum = (Enet_MacPort)ENET_MACPORT_NORM(frames[i].port);
#ifdef ENET_SOC_HOSTPORT_DMA_TYPE_UDMA
		if (frames[i].tc < 0) {
			pktInfo->txPktTc = ENET_TRAFFIC_CLASS_INV;
		} else {
			pktInfo->txPktTc = frames[i].tc;
		}
#endif
		EnetDma_checkPktState(&pktInfo->pktState, ENET_PKTSTATE_MODULE_APP,
				ENET_PKTSTATE_APP_WITH_FREEQ, ENET_PKTSTATE_APP_WITH_DRIVER);

		EnetQueue_enq(&txSubmitQ, &pktInfo->node);
	}

	status = EnetDma_submitTxPktQ(hLLDTxDma->hTxCh, &txSubmitQ);
	if(status != ENET_SOK) {
		res=LLDENET_E_DMA;
		goto exit;
	}
	res=LLDENET_E_OK;
exit:
	TXDMA_UNLOCK(hLLDTxDma);
	return res;
}

int LLDEnetSendScatter(LLDEnet_t *hLLDEnet, LLDEnetFrameScatter_t *frame)
{
	return LLDEnetSendMultiScatter(hLLDEnet, frame, 1);
}

int LLDEnetSendMulti(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frames, uint32_t nFrames)
{
	if((frames==NULL) || (nFrames == 0) || (nFrames > LLDENET_MAX_SCATTER_FRAMES)) {
		return LLDENET_E_PARAM;
	}
	LLDEnetFrameScatter_t scatterFrames[LLDENET_MAX_SCATTER_FRAMES];
	int i;

	for (i = 0; i < nFrames; i++) {
		if (frames[i].buf == NULL) {
			return LLDENET_E_PARAM;
		}
		scatterFrames[i].nBufs = 1;
		scatterFrames[i].buf[0] = frames[i].buf;
		scatterFrames[i].size[0] = frames[i].size;
		scatterFrames[i].port = frames[i].port;
		scatterFrames[i].tc = frames[i].tc;
	}
	return LLDEnetSendMultiScatter(hLLDEnet, scatterFrames, nFrames);
}

int LLDEnetSend(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frame)
{
	return LLDEnetSendMulti(hLLDEnet, frame, 1);
}

static bool FilterIsMatched(LLDEnetRxFilter_t *filter, uint8_t *buf, int size)
{
	uint16_t ethType;

	/* dstMac: 6 bytes, srcMac: 6 bytes and ethernet_type: 2 bytes */
	if (size <= 14) {
		return false;
	}

	/* This filter should match the default callback data */
	if ((filter->vlanId <= 0) && (filter->ethType <= 0)
		&& !IsMacAddrSet(filter->dstMacAddr)) {
		return false;
	}

	ethType = ntohs(*(uint16_t *)(buf + 12));
	if ((filter->vlanId > 0) && (ethType != ETH_P_8021Q)) {
		return false;
	}
	if (filter->ethType > 0) {
		/* We support this case in case the ALE add the tag to the non-tag packet */
		if (ethType == ETH_P_8021Q) {
			uint16_t subEthType;
			/* dstMac: 6 bytes, srcMac: 6 bytes, Qtag: 4 bytes,
			 * and ethernet_type: 2 bytes */
			if (size <= 18) {
				return false;
			}
			subEthType = ntohs(*(uint16_t *)(buf + 16));
			if (subEthType != filter->ethType) {
				return false;
			}
		} else {
			if (ethType != filter->ethType) {
				return false;
			}
		}
	}
	if (IsMacAddrSet(filter->dstMacAddr) && memcmp(filter->dstMacAddr, buf, 6)) {
		return false;
	}
	return true;
}

static int LLDEnetPushFrameToMatchedSubRx(LLDEnetRxDma_t *hLLDRxDma, int portNum,
										  void *buf, int size, uint64_t rxts)
{
	int i;

	for (i = 0; i < hLLDRxDma->nSubRx; i++) {
		LLDEnetSubRx_t *subRx = &hLLDRxDma->subRxTable[i];
		if (FilterIsMatched(&subRx->rxFilter, (uint8_t *)buf, size)) {
			LLDEnetPktBuf_t pktBuf;
			memset(&pktBuf, 0, sizeof(pktBuf));
			pktBuf.buf = buf;
			pktBuf.size = size;
			pktBuf.port = portNum;
			pktBuf.rxts = rxts;
			if (BufRingPush(&subRx->bufRing, &pktBuf)) {
				if (subRx->rxNotifyCb) {
					subRx->rxNotifyCb(subRx->rxCbArg);
				}
			} else {
				UB_LOG(UBL_ERROR,"Unable to push subIdx: %d\n", subRx->subRxIndex);
			}
			break;
		}
	}

	/* Not matched any SubRx */
	if ((i == hLLDRxDma->nSubRx) && (hLLDRxDma->rxDefaultDataCb)) {
		hLLDRxDma->rxDefaultDataCb(buf, size, portNum, hLLDRxDma->rxDefaultCbArg);
	}

	return 0;
}

static bool IsRxFrameForDmaOwner(LLDEnetRxDma_t *hLLDRxDma, void *frame, int size)
{
	return FilterIsMatched(&hLLDRxDma->ownerFilter, (uint8_t*)frame, size);
}

static int LLDEnetRecvSubRxGen(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frame,
							   LLDEnetRecvCb_t LLDEnetRecvCb,  void *cbArg)
{
	/* Only one channel for subRx */
	EnetAppUtils_assert(hLLDEnet->numRxChannels == 1);
	LLDEnetSubRx_t *subRx = &hLLDEnet->hLLDRxDma[0]->subRxTable[hLLDEnet->subRxIndex];
	LLDEnetPktBuf_t pktBuf;

	memset(&pktBuf, 0, sizeof(pktBuf));

	if (BufRingIsEmpty(&subRx->bufRing)) {
		return LLDENET_E_NOAVAIL;
	}
	if (LLDEnetRecvCb != NULL) { /* zero-copy */
		LLDEnetFrame_t tmpFrame;
		memset(&tmpFrame, 0, sizeof(tmpFrame));
		if (!BufRingRefer(&subRx->bufRing, &pktBuf)) {
			UB_LOG(UBL_ERROR,"Unable to refer ring subIdx: %d\n", subRx->subRxIndex);
			return LLDENET_E_FAILURE;
		}
		tmpFrame.rxts = pktBuf.rxts;
		tmpFrame.size = pktBuf.size;
		tmpFrame.buf = (uint8_t*)pktBuf.buf;
		tmpFrame.port = pktBuf.port;

		LLDEnetRecvCb(&tmpFrame, cbArg);
	} else {
		pktBuf.buf = frame->buf;
		pktBuf.size = frame->size;
		if (!BufRingPop(&subRx->bufRing, &pktBuf)) {
			UB_LOG(UBL_ERROR,"Unable to pop subIdx: %d\n", subRx->subRxIndex);
			return LLDENET_E_FAILURE;
		}
		frame->size = pktBuf.size;
		frame->port = pktBuf.port;
	}
	return LLDENET_E_OK;
}

static int LLDEnetRecvGen(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frame,
			LLDEnetRecvCb_t LLDEnetRecvCb, void *cbArg)
{
	EnetDma_Pkt *pktInfo;
	EthFrame *rxFrame;
	uint32_t rxReadyCnt;
	uint32_t pktSize;
	LLDEnetRxDma_t *hLLDRxDma;
	int res = LLDENET_E_OK;

	if (hLLDEnet == NULL) {
		return LLDENET_E_PARAM;
	}
	if (LLDEnetRecvCb == NULL) {
		if ((frame == NULL) || (frame->buf == NULL) || (frame->size == 0)) {
			return LLDENET_E_PARAM;
		}
	}

	if (hLLDEnet->dmaRxOwner == false) {
		/* This code will not be called by dmaRxOwner */
		return LLDEnetRecvSubRxGen(hLLDEnet, frame, LLDEnetRecvCb, cbArg);
	}
	/* This code will be called by the dmaRxOwner */
	rxReadyCnt = EnetQueue_getQCount(&hLLDEnet->rxReadyQ);
	if (rxReadyCnt == 0) {
		return LLDENET_E_NOAVAIL;
	}
	do{
		/* Consume the received packets and release them */
		pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&hLLDEnet->rxReadyQ);
		EnetAppUtils_assert(pktInfo != NULL);
		rxReadyCnt--;

		EnetDma_checkPktState(&pktInfo->pktState,
							  ENET_PKTSTATE_MODULE_APP,
							  ENET_PKTSTATE_APP_WITH_READYQ,
							  ENET_PKTSTATE_APP_WITH_FREEQ);

		EnetAppUtils_assert(pktInfo->sgList.numScatterSegments == 1);
		/* Get back the packet's dma channel handle from appPriv */
		hLLDRxDma = (LLDEnetRxDma_t *)pktInfo->appPriv;
		EnetAppUtils_assert(hLLDRxDma != NULL);
		rxFrame = (EthFrame *)pktInfo->sgList.list[0].bufPtr;
		pktSize = pktInfo->sgList.list[0].segmentFilledLen;

		if ((hLLDRxDma->dmaRxShared == false) ||
			IsRxFrameForDmaOwner(hLLDRxDma, rxFrame, pktSize)) {
			if (LLDEnetRecvCb != NULL) { /* zero-copy */
				LLDEnetFrame_t tmpFrame;
				memset(&tmpFrame, 0, sizeof(tmpFrame));
				tmpFrame.rxts = pktInfo->tsInfo.rxPktTs;
				tmpFrame.size = pktSize;
				tmpFrame.buf = (uint8_t*)rxFrame;
				tmpFrame.port = (uint8_t)pktInfo->rxPortNum;
				LLDEnetRecvCb(&tmpFrame, cbArg);
			} else {
				if (frame->size >= pktSize) {
					frame->rxts = pktInfo->tsInfo.rxPktTs;
					frame->size = pktSize;
					memcpy(frame->buf, rxFrame, frame->size);
					frame->port = (uint8_t)pktInfo->rxPortNum;
				} else {
					res = LLDENET_E_BUFSIZE;
				}
			}
		} else {
			/* only happens when the dmaRxShared = true and this function
			 * is called by owner */
			LLDEnetPushFrameToMatchedSubRx(hLLDRxDma, pktInfo->rxPortNum, rxFrame,
										   pktInfo->sgList.list[0].segmentFilledLen,
										   pktInfo->tsInfo.rxPktTs);
			res = LLDENET_E_NOMATCH;
		}

		/* Release the received packet to the correct DMA channel */
		EnetQueue_enq(&hLLDRxDma->rxFreeQ, &pktInfo->node);
		/* We want to receive all the packets from the queue at one
		 * LLDEnetRecvGen() call when zero-copy is used (LLDEnetRecvCb != NULL) */
	} while ((rxReadyCnt > 0) && (LLDEnetRecvCb != NULL));

	EnetAppUtils_validatePacketState(&hLLDRxDma->rxFreeQ,
									 ENET_PKTSTATE_APP_WITH_FREEQ,
									 ENET_PKTSTATE_APP_WITH_DRIVER);
	EnetDma_submitRxPktQ(hLLDRxDma->hRxCh, &hLLDRxDma->rxFreeQ);

	return res;
}

int LLDEnetRecvZeroCopy(LLDEnet_t *hLLDEnet,
		LLDEnetRecvCb_t LLDEnetRecvCb, void *cbArg)
{
	return LLDEnetRecvGen(hLLDEnet, NULL, LLDEnetRecvCb, cbArg);
}

int LLDEnetRecv(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frame)
{
	return LLDEnetRecvGen(hLLDEnet, frame, NULL, NULL);
}

void LLDEnetCfgInit(LLDEnetCfg_t *cfg)
{
	uint32_t i;

	if (cfg == NULL) {
		return;
	}
	memset(cfg, 0, sizeof(LLDEnetCfg_t));
	cfg->dmaTxChId = -1;
	cfg->numRxChannels = 1;
	for (i = 0U; i < MAX_NUM_RX_DMA_CH_PER_INSTANCE ; i++) {
		cfg->dmaRxChId[i] = -1;
	}
	cfg->dmaTxShared = false;
	cfg->dmaRxOwner = true;
	cfg->dmaRxShared = false;
	cfg->unusedDmaTx = false;
	cfg->unusedDmaRx = false;
}

bool LLDEnetIsPortUp(LLDEnet_t *hLLDEnet, uint8_t portNum)
{
	if (hLLDEnet == NULL) {
		return false;
	}
	return EnetAppUtils_isPortLinkUp(hLLDEnet->hEnet, hLLDEnet->coreId,
				(Enet_MacPort)(ENET_MACPORT_NORM(portNum)));
}

int LLDEnetGetLinkInfo(LLDEnet_t *hLLDEnet, uint8_t portNum,
					   uint32_t *speed, uint32_t *duplex)
{

	EnetMacPort_LinkCfg phyOutArgs;
	Enet_IoctlPrms prms;
	int32_t status;
	Enet_MacPort phyInArgs;
	EnetPhy_GenericInArgs phyGenericInArgs;

	if ((hLLDEnet == NULL) || (speed == NULL) || (duplex == NULL)) {
		return LLDENET_E_PARAM;
	}

#if ENET_ENABLE_PER_ICSSG
	/* Get port link state (speed & duplexity) from PHY */
	phyInArgs = (Enet_MacPort)(ENET_MACPORT_NORM(portNum));
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &phyInArgs, &phyOutArgs);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
				ENET_PER_IOCTL_GET_PORT_LINK_CFG, &prms, status);
	(void)phyGenericInArgs;

#else
	/* For CPSW, ENET_PER_IOCTL_GET_PORT_LINK_CFG ioctl has limitation with RMII/MII mode
	 * as RgmiiStatus register is read to fetch the Link speed, duplexity.
	 * It is suggested to use ENET_PHY_IOCTL_GET_LINK_MODE ioctl which reads the phy registers
	 * to get the link speed, duplexity */
	phyGenericInArgs.macPort = (Enet_MacPort)(ENET_MACPORT_NORM(portNum));
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &phyGenericInArgs, &phyOutArgs);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
			   ENET_PHY_IOCTL_GET_LINK_STATUS, &prms, status);
	(void)phyInArgs;
#endif
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to get link info: %d\n", status);
		return LLDENET_E_IOCTL;
	}

	if (phyOutArgs.speed == ENET_SPEED_10MBIT) {
		*speed = 10;
	} else if (phyOutArgs.speed == ENET_SPEED_100MBIT) {
		*speed = 100;
	} else if (phyOutArgs.speed == ENET_SPEED_1GBIT) {
		*speed = 1000;
	} else {
		*speed = 0;
	}

	if (phyOutArgs.duplexity == ENET_DUPLEX_FULL) {
		*duplex = 1;
	} else {
		*duplex = 0;
	}

	return LLDENET_E_OK;
}

int LLDEnetAllocMac(LLDEnet_t *hLLDEnet, uint8_t *srcMacAddr)
{
	int32_t status;

	if ((hLLDEnet == NULL) || (srcMacAddr == NULL)) {
		return LLDENET_E_PARAM;
	}
	status = EnetAppUtils_allocMac(hLLDEnet->hEnet, hLLDEnet->coreKey,
								   hLLDEnet->coreId, srcMacAddr);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to alloc Mac: %d\n", status);
		return LLDENET_E_IOCTL;
	}
	return LLDENET_E_OK;
}

void LLDEnetFreeMac(LLDEnet_t *hLLDEnet, uint8_t *srcMacAddr)
{
	if ((hLLDEnet == NULL) || (srcMacAddr == NULL)) {
		return;
	}
	EnetAppUtils_freeMac(hLLDEnet->hEnet, hLLDEnet->coreKey,
						 hLLDEnet->coreId, srcMacAddr);
}

int LLDEnetSetTxNotifyCb(LLDEnet_t *hLLDEnet, void (*txNotifyCb)(void *arg), void *arg)
{
	if (hLLDEnet == NULL) {
		return LLDENET_E_PARAM;
	}
	if (hLLDEnet->hLLDTxDma == NULL) {
		return LLDENET_E_DENY;
	}
	if (hLLDEnet->dmaTxOwner) {
		/* only dmaTxOwner can set notify */
		hLLDEnet->hLLDTxDma->txNotifyCb = txNotifyCb;
		hLLDEnet->hLLDTxDma->txCbArg = arg;
	}
	return LLDENET_E_OK;
}

int LLDEnetSetRxNotifyCb(LLDEnet_t *hLLDEnet, void (*rxNotifyCb)(void *arg), void *arg)
{
	uint32_t i = 0U;
	if (hLLDEnet == NULL) {
		return LLDENET_E_PARAM;
	}
	for( i = 0U; i < hLLDEnet->numRxChannels; i++) {
		if (hLLDEnet->hLLDRxDma[i] == NULL) {
			return LLDENET_E_DENY;
		}
		if (hLLDEnet->dmaRxOwner) {
			hLLDEnet->hLLDRxDma[i]->rxOwnerNotifyCb = rxNotifyCb;
			hLLDEnet->hLLDRxDma[i]->rxOwnerCbArg = arg;
		} else {
			/* If it is not the owner of DMA channel, only one DMA channel is supported */
			EnetAppUtils_assert(hLLDEnet->numRxChannels == 1);
			LLDEnetSubRx_t *subRx;
			subRx = &hLLDEnet->hLLDRxDma[0]->subRxTable[hLLDEnet->subRxIndex];
			subRx->rxNotifyCb = rxNotifyCb;
			subRx->rxCbArg = arg;
		}
	}

	return LLDENET_E_OK;
}

int LLDEnetSetDefaultRxDataCb(LLDEnet_t *hLLDEnet,
		void (*rxDefaultDataCb)(void *data, int size, int port, void *arg), void *arg)
{
	LLDEnetRxDma_t *hLLDRxDma;
	uint32_t i = 0U;

	if (hLLDEnet == NULL) {
		return LLDENET_E_PARAM;
	}
	for (i = 0U; i < hLLDEnet->numRxChannels; i++) {
		hLLDRxDma = hLLDEnet->hLLDRxDma[i];
		if (hLLDRxDma == NULL) {
			return LLDENET_E_DENY;
		}
		hLLDRxDma->rxDefaultDataCb = rxDefaultDataCb;
		hLLDRxDma->rxDefaultCbArg = arg;
	}

	return LLDENET_E_OK;
}

void LLDEnetEnableQueueDMAChannelMapping(LLDEnet_t *hLLDEnet, uint8_t macPorts[],
                                        int nPorts, uint8_t priority)
{
#if ENET_ENABLE_PER_CPSW
#if ENET_CFG_IS_ON(CPSW_EST)

	Enet_IoctlPrms prms;
	EnetMacPort_GenericInArgs inArgs;
	EnetMacPort_SetEgressPriorityMapInArgs macPortPrioMap;
	EnetPort_PriorityMap prioMap;
	uint32_t i = 0U, j;
	int32_t status;

	if (priority > ENET_PRI_MAX) {
		UB_LOG(UBL_ERROR,"%s, Invalid priority!, input priority: %d\n",
		       __func__, priority);
		return;
	}

	/* Read the Current HostPort Tx Mapping */
	memset(&prioMap, 0, sizeof(EnetPort_PriorityMap));
	ENET_IOCTL_SET_OUT_ARGS(&prms, &prioMap);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_HOSTPORT_IOCTL_GET_PRI_REGEN_MAP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to get IOCTL_GET_PRI_REGEN_MAP: %d\n",
		       status);
		return;
	}

	/* Update the mapping for DMA channel and apply */
	EnetAppUtils_assert(hLLDEnet->hLLDTxDma->txChId < ENET_PRI_NUM);
	prioMap.priorityMap[hLLDEnet->hLLDTxDma->txChId] = priority;
	ENET_IOCTL_SET_IN_ARGS(&prms, &prioMap);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_HOSTPORT_IOCTL_SET_PRI_REGEN_MAP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to set IOCTL_GET_PRI_REGEN_MAP:%d\n",
		       status);
		return;
	}

	/* Read the Current MacPort Tx Mapping */
	memset(&prioMap, 0, sizeof(EnetPort_PriorityMap));
	memset(&inArgs, 0, sizeof(EnetMacPort_GenericInArgs));
	for (i = 0; i < nPorts; i++) {
		inArgs.macPort = (Enet_MacPort)macPorts[i];
		ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &prioMap);
		ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
			   ENET_MACPORT_IOCTL_GET_EGRESS_QOS_PRI_MAP, &prms, status);
		if (status != ENET_SOK) {
			UB_LOG(UBL_ERROR,"Failed to get IOCTL_GET_EGRESS_QOS_PRI_MAP:%d, port: %d\n",
			       status, macPorts[i]);
			return;
		}
		/*Set the mapping[priority] to priority and copy the rest*/
		for (j = 0; j < ENET_PRI_NUM; j++) {
			macPortPrioMap.priorityMap.priorityMap[j] = prioMap.priorityMap[j];
		}
		macPortPrioMap.priorityMap.priorityMap[priority] = priority;
		macPortPrioMap.macPort = (Enet_MacPort)macPorts[i];
		ENET_IOCTL_SET_IN_ARGS(&prms, &macPortPrioMap);
		ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
			   ENET_MACPORT_IOCTL_SET_EGRESS_QOS_PRI_MAP, &prms, status);
		if (status != ENET_SOK) {
			UB_LOG(UBL_ERROR,"Failed to set IOCTL_SET_EGRESS_QOS_PRI_MAP:%d, port: %d\n",
			       status, macPorts[i]);
			return;
		}
	}
#endif // ENET_CFG_IS_ON(CPSW_EST)
#endif // #if ENET_ENABLE_PER_CPSW
}

int LLDEnetEnablePriority2QueueMapping(LLDEnet_t *hLLDEnet, uint8_t macPort,
				       cbl_qmap_params_t *cqp)
{
	int32_t status = LLDENET_E_UNSUPPORT;
#if ENET_ENABLE_PER_CPSW
	int32_t i;
	int maxSuppotedPriorities;
	Enet_IoctlPrms prms;
	EnetMacPort_GenericInArgs inArgs;
	EnetMacPort_SetEgressPriorityMapInArgs macPortPrioMap;
	EnetPort_PriorityMap prioMap;

	memset(&inArgs, 0, sizeof(EnetMacPort_GenericInArgs));
	memset(&prioMap, 0, sizeof(EnetPort_PriorityMap));
	inArgs.macPort = (Enet_MacPort)macPort;
	ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &prioMap);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_GET_EGRESS_QOS_PRI_MAP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to get IOCTL_GET_EGRESS_QOS_PRI_MAP:%d, port: %d\n",
		       status, macPort);
		return -1;
	}
	memset(&macPortPrioMap, 0, sizeof(EnetMacPort_SetEgressPriorityMapInArgs));
	maxSuppotedPriorities = UB_MIN(sizeof(cqp->pri_lq_map), ENET_PRI_NUM);
	for (i = 0; i < ENET_PRI_NUM; i++) {
		// Preserve the existing mapping.
		macPortPrioMap.priorityMap.priorityMap[i] = prioMap.priorityMap[i];
	}
	for (i = 0; i < maxSuppotedPriorities; i++) {
		macPortPrioMap.priorityMap.priorityMap[i] = cqp->pri_lq_map[i];
	}
	macPortPrioMap.macPort = (Enet_MacPort)macPort;
	ENET_IOCTL_SET_IN_ARGS(&prms, &macPortPrioMap);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_SET_EGRESS_QOS_PRI_MAP, &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR,"Failed to set IOCTL_SET_EGRESS_QOS_PRI_MAP:%d, port: %d\n",
		       status, macPort);
	}
#endif // #if ENET_ENABLE_PER_CPSW
	return (status == ENET_SOK? 0: -1);
}

int LLDEnetSetCreditBasedShaping(LLDEnet_t *hLLDEnet, uint8_t port,
				 cbl_cbs_params_t *cbsprm)
{
	int32_t status = LLDENET_E_UNSUPPORT;
#if ENET_ENABLE_PER_CPSW
#if ENET_CFG_IS_ON(CPSW_MACPORT_TRAFFIC_SHAPING)
	Enet_IoctlPrms prms;
	EnetMacPort_SetCreditBasedShaperInArgs cbsInArg;

	/* There are only `ENET_PRI_MAX` HW queues in TI platform */
	if (cbsprm->qindex > ENET_PRI_MAX) {
		UB_LOG(UBL_ERROR, "%s, Invalid HW queue index: %d \n",
		       __func__, cbsprm->qindex);
		return LLDENET_E_PARAM;
	}

	memset(&cbsInArg, 0, sizeof(cbsInArg));
	cbsInArg.macPort = (Enet_MacPort)port;
	cbsInArg.cbsCfg.idleSlope = cbsprm->idleslope;
	cbsInArg.cbsCfg.queueNum = cbsprm->qindex;
	ENET_IOCTL_SET_IN_ARGS(&prms, &cbsInArg);
	ENET_IOCTL(hLLDEnet->hEnet, hLLDEnet->coreId,
		   ENET_MACPORT_IOCTL_SET_CREDIT_BASED_SHAPING,
		   &prms, status);
	if (status != ENET_SOK) {
		UB_LOG(UBL_ERROR, "%s, Failed to set CBS for mac port %d, queue: %d \n",
		       __func__, port, cbsprm->qindex);
		return LLDENET_E_FAILURE;
	}
        status = LLDENET_E_OK;
#endif // CPSW_MACPORT_TRAFFIC_SHAPING
#endif // #if ENET_ENABLE_PER_CPSW
	return status;
}

int LLDEnet_processStatusFrames(LLDEnet_t *hLLDEnet)
{
	#if !ENET_ENABLE_PER_ICSSG
	int status = LLDENET_E_OK;
	int numStatusFrames = 0;
	EnetDma_PktQ tempQ;
	LLDEnetRxDma_t *hLLDRxDma = NULL;

	/* Initialize the temp Queue. */
	EnetQueue_initQ(&tempQ);

	/* Get the size of the status frame Queue. */
	int qSize = EnetQueue_getQCount(&hLLDEnet->rxReadyQ);

	for (int i = 0; i < qSize; i++)
	{
		EnetDma_Pkt* packet = (EnetDma_Pkt *)EnetQueue_deq(&hLLDEnet->rxReadyQ);

		EthFrame* rxFrame = (EthFrame *)packet->sgList.list[0].bufPtr;
        uint32_t size = packet->sgList.list[0].segmentFilledLen;

		if (LLDEnet_isStatusFrame(rxFrame))
		{
			numStatusFrames += 1;

			/* Process the status frames. */
			status = LLDTSyncProcPhyStatusFrame(hLLDEnet->hEnet,
							hLLDEnet->coreId, 0, (uint8_t *)rxFrame, size);
			Enet_devAssert(status == LLDENET_E_OK, "process status frame failed");

			EnetDma_checkPktState(&packet->pktState, ENET_PKTSTATE_MODULE_APP,
													ENET_PKTSTATE_APP_WITH_READYQ,
													ENET_PKTSTATE_APP_WITH_FREEQ);

			EnetQueue_enq(&tempQ, &packet->node);

			hLLDRxDma = (LLDEnetRxDma_t *)packet->appPriv;
		}
		else
		{
			/* Put it back in the Queue. */
			EnetQueue_enq(&hLLDEnet->rxReadyQ, &packet->node);
		}
	}

	if (EnetQueue_getQCount(&tempQ) > 0)
	{
		EnetAppUtils_validatePacketState(&tempQ,
									 ENET_PKTSTATE_APP_WITH_FREEQ,
									 ENET_PKTSTATE_APP_WITH_DRIVER);

		status = EnetDma_submitRxPktQ(hLLDRxDma->hRxCh, &tempQ);
		Enet_devAssert(status == 0, "Submit rx packet failed");
	}

	return numStatusFrames;
	#else
	(void)hLLDEnet;
	return 0;
	#endif
}
