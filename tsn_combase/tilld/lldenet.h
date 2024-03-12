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
/**
 * @addtogroup EnetLLD_network
 * @{
 * @file lldenet.h
 * @brief TI Ethernet Low Level Driver abstract APIs
 */
#ifndef LLDENET_H_
#define LLDENET_H_

#include "lldtype.h"

/**
 * @brief Structure representing the LLDEnet instance.
 */
typedef struct LLDEnet LLDEnet_t;

/**
 * @brief Configuration structure for LLDEnet.
 */
typedef struct {
	/**
	 * Ethernet type.
	 */
	uint32_t enetType;
	/**
	 * Instance ID.
	 */
	uint32_t instId;
	/**
	 * Transmit notification callback function pointer.
	 */
	void (*txNotifyCb)(void *cbArg);
	/**
	 * Receive notification callback function pointer.
	 */
	void (*rxNotifyCb)(void *cbArg);
	/**
	 * Argument to be passed to the transmit notification callback function.
	 */
	void *txCbArg;
	/**
	 * Argument to be passed to the receive notification callback function.
	 */
	void *rxCbArg;
	/**
	 * Destination MAC address to filter RX packets.
	 * If all bytes of MAC addres are 0, no filter is applied.
	 */
	uint8_t dstMacAddr[6];
	/**
	 * VLAN ID to filter RX packets.
	 * If all bytes of dstMacAddr are 0, no filter is applied.
	 */
	uint32_t vlanId;
	/**
	 * Ethernet Type to filter RX packets.
	 */
	uint32_t ethType;
	/**
	 * Number of buffers allocated for transmit packets
	 * Set to 0 to use the default value assigned by the implementation.
	 */
	uint32_t nTxPkts;
	/**
	 * Number of buffers allocated for receive packets
	 * Set to 0 to use the default value assigned by the implementation.
	 */
	uint32_t nRxPkts[MAX_NUM_RX_DMA_CH_PER_INSTANCE];
	/**
	 * Transmit and receive maximum packet size.
	 * Set to 0 to use the default value assigned by the implementation.
	 */
	uint32_t pktSize;
	/**
	 * DMA transmit channel ID.
	 * Set to -1 for dynamic allocation, only supported by Jacinto.
	 */
	int dmaTxChId;
	/**
	 * DMA receive channel ID.
	 * Set to -1 for dynamic allocation, only supported by Jacinto.
	 */
	int dmaRxChId[MAX_NUM_RX_DMA_CH_PER_INSTANCE];
	/**
	 * true: won't use DMA RX; false: will use DMA RX
	 * This is used when there is no interest in receiving data.
	 */
	bool unusedDmaRx;
	/**
	 * true: won't use DMA TX; false: will use DMA TX
	 * This is used when there is no interest in sending data.
	 */
	bool unusedDmaTx;
	/**
	 * Only the Sitara AM273x supports this param.
	 * true: the dmaRxChId is shared between apps; false: not shared
	 */
	bool dmaRxShared;
	/**
	 * Only the Sitara AM273x supports this param.
	 * true: the LLDEnet_t owns this RX DMA channel, it has the full permission to access
	 * the SDK DMA API; false: the LLDEnet_t is unable to to call SDK RX DMA API
	 * This is used when there are multiple LLDEnet_t shares the same DMA RX channel.
	 * User has to make sure only one LLDEnet_t has dmaRxOwner is set to true if the
	 * DMA channel share function is used.
	 * Currently TX DMA channel share is not supported.
	 */
	bool dmaRxOwner;
	/**
	 * Number of Rx DMA channels, only ICSSG peripheral has more than 1 Rx DMA channels.
	 */
	uint32_t numRxChannels;
} LLDEnetCfg_t;

/**
 * @brief Structure representing an Ethernet frame.
 */
typedef struct {
	/**
	 * Pointer to the buffer containing the frame data.
	 */
	uint8_t *buf;
	/**
	 * Size of the frame.
	 */
	uint32_t size;
	/**
	 * Port number associated with the frame.
	 */
	int port;
	/**
	 * Traffic class associated with the frame.
	 * Set to -1 to not assign a traffic class.
	 */
	int tc;
	/**
	 * timestamp of rx packet which was captured by host port.
	 */
	uint64_t rxts;
} LLDEnetFrame_t;

/**
 * @brief Initializes the LLDEnetCfg_t structure.
 *
 * @param cfg Pointer to the LLDEnetCfg_t structure to be initialized.
 */
void LLDEnetCfgInit(LLDEnetCfg_t *cfg);

/**
 * @brief Opens an instance of LLDEnet.
 *
 * @param cfg Pointer to the LLDEnetCfg_t structure containing the configuration parameters.
 * @return Pointer to the LLDEnet instance (LLDEnet_t) if successful, NULL otherwise.
 */
LLDEnet_t *LLDEnetOpen(LLDEnetCfg_t *cfg);

/**
 * @brief Closes an instance of LLDEnet.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 */
void LLDEnetClose(LLDEnet_t *hLLDEnet);

/**
 * @brief Filter RX packets by using the destination MAC address and VLAN ID.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param dstMacAddr Destination MAC address to filter.
 * @param vlanId VLAN ID to filter.
 * @param ethType Ethernet type to filter (e.g. PTP ethernet type is 0x88F7).
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetFilter(LLDEnet_t *hLLDEnet, uint8_t *dstMacAddr,
				  uint32_t vlanId, uint32_t ethType);

/**
 * @brief Allocates a source MAC address.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param srcMacAddr Pointer to store the allocated MAC address.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetAllocMac(LLDEnet_t *hLLDEnet, uint8_t *srcMacAddr);

/**
 * @brief Frees a previously allocated MAC address.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param srcMacAddr MAC address to be freed.
 */
void LLDEnetFreeMac(LLDEnet_t *hLLDEnet, uint8_t *srcMacAddr);

/**
 * @brief Sends an Ethernet frame using LLDEnet.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param frame Pointer to the LLDEnetFrame_t structure representing the frame to be sent.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetSend(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frame);

/**
 * @brief Sends multiple Ethernet frames using LLDEnet.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param frames Array of LLDEnetFrame_t structures representing the frames to be sent.
 * @param nFrames Number of frames in the array.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetSendMulti(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frames, uint32_t nFrames);

/**
 * @brief Receives an Ethernet frame using LLDEnet.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param frame Pointer to the LLDEnetFrame_t structure to store the received frame.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 * @note The LLDENET_E_NOMATCH error when this function has data but does not match
 * the data filter. This can happens when the RX DMA is shared between multiple
 * data flows.
 */
int LLDEnetRecv(LLDEnet_t *hLLDEnet, LLDEnetFrame_t *frame);

/**
 * @brief Receives an Ethernet frame using LLDEnet in the Zero-Copy way.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param LLDEnetRecvCb Receive callback
 * @param cbArg Callback argument
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetRecvZeroCopy(LLDEnet_t *hLLDEnet,
		void (*LLDEnetRecvCb)(LLDEnetFrame_t *frame, void *cbArg),  void *cbArg);

/**
 * @brief Checks if the specified port is up.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param portNum Port number to check.
 * @return True if the port is up, false otherwise.
 */
bool LLDEnetIsPortUp(LLDEnet_t *hLLDEnet, uint8_t portNum);

/**
 * @brief Retrieves the link speed and duplex information for the specified port.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param portNum Port number to retrieve the link information.
 * @param speed Pointer to store the link speed.
 * @param duplex Pointer to store the link duplex mode.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetGetLinkInfo(LLDEnet_t *hLLDEnet, uint8_t portNum,
					   uint32_t *speed, uint32_t *duplex);

/**
 * @brief Sets the transmit notification callback function for LLDEnet.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param txNotifyCb Pointer to the transmit notification callback function.
 * @param arg Argument to be passed to the callback function.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetSetTxNotifyCb(LLDEnet_t *hLLDEnet, void (*txNotifyCb)(void *arg), void *arg);

/**
 * @brief Sets the receive notification callback function for LLDEnet.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param rxNotifyCb Pointer to the receive notification callback function.
 * @param arg Argument to be passed to the callback function.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetSetRxNotifyCb(LLDEnet_t *hLLDEnet, void (*rxNotifyCb)(void *arg), void *arg);

/**
 * @brief Sets the receive data callback function for LLDEnet when filter does not match.
 * This callback is supported only when using the RX DMA shared channel.
 * Currently, only Sitara AM273x is supported for this feature.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param rxDefaultDataCb Pointer to the receive data callback function.
 * @param arg Argument to be passed to the callback function.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDEnetSetDefaultRxDataCb(LLDEnet_t *hLLDEnet,
		void (*rxDefaultDataCb)(void *data, int size, int port, void *arg), void *arg);

/**
 * @brief Checks the receive packet timestamping mode.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @return true if isRxTsInPkt is true for the LLDEnet handle.
 */
bool LLDEnetIsRxTsInPkt(LLDEnet_t *hLLDEnet);

#endif //LLDENET_H_

/** @}*/
