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

#ifndef LLDENET_PRIV_H_
#define LLDENET_PRIV_H_
#include <enet_appmemutils.h>
#include <enet_apputils.h>
#include <enet_mcm.h>
#include "bufring.h"

#ifndef ENET_MEM_NUM_TX_PKTS
#define ENET_MEM_NUM_TX_PKTS 16
#endif //ENET_MEM_NUM_TX_PKTS

#ifndef CB_LLDENET_MMEM
#define CB_LLDENET_MMEM lldenet_mem
#endif

#ifndef CB_LLDENET_INSTNUM
#define CB_LLDENET_INSTNUM (10)
#endif

#ifndef LLDNET_TXDMA_CHANNEL_NUM
#define LLDNET_TXDMA_CHANNEL_NUM 5
#endif //LLDNET_TXDMA_CHANNEL_NUM

#ifndef LLDNET_RXDMA_CHANNEL_NUM
#define LLDNET_RXDMA_CHANNEL_NUM 5
#endif //LLDNET_RXDMA_CHANNEL_NUM

#ifndef LLDENET_SUB_RX_NUM
#define LLDENET_SUB_RX_NUM 4
#endif //LLDENET_SUB_RX_NUM

typedef struct {
	int vlanId;
	int ethType;
	uint8_t dstMacAddr[ENET_MAC_ADDR_LEN];
} LLDEnetRxFilter_t;

/* TX dma is not shared between multiple apps */
typedef struct {
	EnetDma_PktQ txFreePktInfoQ;
	EnetDma_TxChHandle hTxCh;
	int32_t txChNum; /* returned by dma channel alloc API */
	int32_t txChId; /* configured by the user */
	void (*txNotifyCb)(void *cbArg);
	void *txCbArg;
	uint32_t nTxPkts;
	uint32_t pktSize;
	bool hasOwner;
} LLDEnetTxDma_t;

typedef struct {
	LLDEnetRxFilter_t rxFilter;
	void (*rxNotifyCb)(void *cbArg);
	void *rxCbArg;
	LLDEnetBufRing_t bufRing;
	int subRxIndex;
} LLDEnetSubRx_t;

/* RX dma can be shared between multiple apps */
typedef struct {
	/* FreeQ dedicated to each channel */
	EnetDma_PktQ rxFreeQ;
	/* pointer to common ReadyQ to store all the received pkts on all the channels owned by the owner. */
	EnetDma_PktQ *pRxReadyQ;
	EnetDma_RxChHandle hRxCh;
	uint32_t rxFlowStartIdx;
	uint32_t rxFlowIdx;
	uint32_t rxChNum; /* returned by dma channel alloc API */
	int32_t rxChId; /* configured by the user */

	/* Packet filter for the owner */
	LLDEnetRxFilter_t ownerFilter;

	/* The owner notify callback is always called when RX has data */
	void (*rxOwnerNotifyCb)(void *cbArg);
	void *rxOwnerCbArg;

	/* When no data is matched, the default data callback will be called.
	 * We don't need a notify callback which is intended only for AVB apps.
	 * The default callback will be used for other apps, e.g. LwIP
	 */
	void (*rxDefaultDataCb)(void *data, int size, int port, void *cbArg);
	void *rxDefaultCbArg;

	/* Sub RX receiver.
	 * N.B. To be simple and avoid using too many mutexes,
	 * we don't support releasing subRx.
	 */
	LLDEnetSubRx_t subRxTable[LLDENET_SUB_RX_NUM];
	int nSubRx;

	uint32_t nRxPkts;
	uint32_t pktSize;
	bool hasOwner;
	bool dmaRxShared;
} LLDEnetRxDma_t;

struct LLDEnet {
	Enet_Handle hEnet;
	/* If this is the Rx Owner, dmaRxOwner = true, This will store the received pkts on all the dma channels */
	EnetDma_PktQ rxReadyQ;
	uint32_t coreId;
	uint32_t coreKey;
	uint32_t numRxChannels;
	LLDEnetTxDma_t *hLLDTxDma;
	LLDEnetRxDma_t *hLLDRxDma[MAX_NUM_RX_DMA_CH_PER_INSTANCE];
	Enet_Type enetType;
	uint32_t instId;
	/* only dmaRxOwner=true has the permission to call the SDK RX DMA APIs*/
	bool dmaRxOwner;
	/* valid only when dmaRxOwner = false, and numRxChannels = 1*/
	int subRxIndex;
	/* true when Rx timestamp is in the dmaPktInfo, should be set as true for ICSSG */
	bool isRxTsInPkt;
};

#endif //LLDENET_PRIV_H_
