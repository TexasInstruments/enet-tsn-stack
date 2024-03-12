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
#ifndef BUFRING_H_
#define BUFRING_H_

#include <enet_appmemutils.h>
#include <enet_apputils.h>
#include <stdint.h>

#ifndef CB_LLDENET_RXPKT_MMEM
#define CB_LLDENET_RXPKT_MMEM lldenet_pktmem
#endif

#ifndef CB_LLDENET_SUB_RX_RING_SIZE
#define CB_LLDENET_SUB_RX_RING_SIZE 4
#endif

#ifndef ENET_MEM_LARGE_POOL_PKT_SIZE
#define ENET_MEM_LARGE_POOL_PKT_SIZE ENET_UTILS_ALIGN(1536U, ENET_UTILS_CACHELINE_SIZE)
#endif //ENET_MEM_LARGE_POOL_PKT_SIZE

#ifndef ENET_MEM_NUM_RX_PKTS
#define ENET_MEM_NUM_RX_PKTS 16
#endif //ENET_MEM_NUM_RX_PKTS

typedef struct {
	void *buf;
	int size;
	int port;
	uint64_t rxts;
} LLDEnetPktBuf_t;

typedef struct {
	uint32_t ci, pi;
	LLDEnetPktBuf_t pktBufs[CB_LLDENET_SUB_RX_RING_SIZE];
} LLDEnetBufRing_t;

void InitBufRing(LLDEnetBufRing_t *ring);
void DeInitBufRing(LLDEnetBufRing_t *ring);
bool BufRingPush(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf);
bool BufRingPop(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf);
bool BufRingIsEmpty(LLDEnetBufRing_t *ring);
bool BufRingRefer(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf);
void BufRingReferPop(LLDEnetBufRing_t *ring);

#endif
