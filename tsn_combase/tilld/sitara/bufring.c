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
#include "bufring.h"
#include "combase_private.h"

#ifdef SOC_AM273X
/* When device is not SOC_AM273X, we will not allocate this memory */
UB_SD_GETMEM_DEF(CB_LLDENET_RXPKT_MMEM, ENET_MEM_LARGE_POOL_PKT_SIZE,
				 ENET_MEM_NUM_RX_PKTS);

void InitBufRing(LLDEnetBufRing_t *ring)
{
	int i;
	memset(ring, 0, sizeof(LLDEnetBufRing_t));
	for (i = 0; i < CB_LLDENET_SUB_RX_RING_SIZE; i++) {
		ring->pktBufs[i].buf = (LLDEnetPktBuf_t *)UB_SD_GETMEM(
			CB_LLDENET_RXPKT_MMEM, ENET_MEM_LARGE_POOL_PKT_SIZE);
		EnetAppUtils_assert(ring->pktBufs[i].buf != NULL);
	}
}

void DeInitBufRing(LLDEnetBufRing_t *ring)
{
	int i;
	for (i = 0; i < CB_LLDENET_SUB_RX_RING_SIZE; i++) {
		if (ring->pktBufs[i].buf) {
			UB_SD_RELMEM(CB_LLDENET_RXPKT_MMEM, ring->pktBufs);
			ring->pktBufs[i].buf = NULL;
		}
	}
	memset(ring, 0, sizeof(LLDEnetBufRing_t));
}

static bool RingIsEmpty(uint32_t pi, uint32_t ci)
{
	if (pi == ci) {
		return true;
	}
	return false;
}

static bool RingIsFull(uint32_t pi, uint32_t ci)
{
	if ((pi > ci && (pi - ci) == CB_LLDENET_SUB_RX_RING_SIZE - 1)
		|| (ci > pi && (ci - pi) == 1)) {
		return true;
	}
	return false;
}

bool BufRingIsEmpty(LLDEnetBufRing_t *ring)
{
	return RingIsEmpty(ring->pi, ring->ci);
}

bool BufRingPush(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf)
{
	uint32_t pi;
	uint32_t ci;

	pi = ring->pi;
	ci = ring->ci;

	if (RingIsFull(pi, ci)) {
		UB_LOG(UBL_ERROR,"ring is full\n");
		return false;
	}

	if (pktBuf->size > ENET_MEM_LARGE_POOL_PKT_SIZE) {
		UB_LOG(UBL_ERROR,"pkt size too large\n");
		return false;
	}

	memcpy(ring->pktBufs[pi].buf, pktBuf->buf, pktBuf->size);
	ring->pktBufs[pi].size = pktBuf->size;
	ring->pktBufs[pi].port = pktBuf->port;

	pi++;
	if (pi == CB_LLDENET_SUB_RX_RING_SIZE) {
		pi = 0;
	}
	ring->pi = pi;
	return true;
}

bool BufRingPop(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf)
{
	uint32_t pi;
	uint32_t ci;

	pi = ring->pi;
	ci = ring->ci;

	if (RingIsEmpty(pi, ci)) {
		UB_LOG(UBL_ERROR,"ring is empty\n");
		return false;
	}

	if (pktBuf->size < ring->pktBufs[ci].size) {
		UB_LOG(UBL_ERROR,"pkt size too small\n");
		return false;
	}

	memcpy(pktBuf->buf, ring->pktBufs[ci].buf, ring->pktBufs[ci].size);
	pktBuf->size = ring->pktBufs[ci].size;
	pktBuf->port = ring->pktBufs[ci].port;

	ci++;
	if (ci == CB_LLDENET_SUB_RX_RING_SIZE) {
		ci = 0;
	}
	ring->ci = ci;
	return true;
}

bool BufRingRefer(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf)
{
	uint32_t pi;
	uint32_t ci;

	pi = ring->pi;
	ci = ring->ci;

	if (RingIsEmpty(pi, ci)) {
		UB_LOG(UBL_ERROR,"ring is empty\n");
		return false;
	}

	pktBuf->buf = ring->pktBufs[ci].buf;
	pktBuf->size = ring->pktBufs[ci].size;
	pktBuf->port = ring->pktBufs[ci].port;

	return true;
}

void BufRingReferPop(LLDEnetBufRing_t *ring)
{
	uint32_t pi;
	uint32_t ci;

	pi = ring->pi;
	ci = ring->ci;

	if (RingIsEmpty(pi, ci)) {
		UB_LOG(UBL_ERROR,"ring is empty\n");
		return;
	}
	ci++;
	if (ci == CB_LLDENET_SUB_RX_RING_SIZE) {
		ci = 0;
	}
	ring->ci = ci;
}

#else //!SOC_AM273X
void InitBufRing(LLDEnetBufRing_t *ring)
{}

void DeInitBufRing(LLDEnetBufRing_t *ring)
{}

bool BufRingPush(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf)
{
	return false;
}

bool BufRingPop(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf)
{
	return false;
}

bool BufRingRefer(LLDEnetBufRing_t *ring, LLDEnetPktBuf_t *pktBuf)
{
	return true;
}

void BufRingReferPop(LLDEnetBufRing_t *ring)
{}

bool BufRingIsEmpty(LLDEnetBufRing_t *ring)
{
	return true;
}
#endif //!SOC_AM273X
