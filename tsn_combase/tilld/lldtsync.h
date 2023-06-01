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
 * @file lldtsync.h
 * @brief TI Timesync Low Level Driver abstract APIs
 */
#ifndef LLDTSYNC_H_
#define LLDTSYNC_H_

#include "lldtype.h"

/**
 * @brief Configuration structure for LLDTSync.
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
} LLDTSyncCfg_t;

/**
 * @brief Structure representing the LLDTSync instance.
 */
typedef struct LLDTSync LLDTSync_t;

/**
 * @brief Initializes the LLDTSyncCfg_t configuration structure.
 * @param cfg Pointer to the LLDTSyncCfg_t configuration structure.
 */
void LLDTSyncCfgInit(LLDTSyncCfg_t *cfg);

/**
 * @brief Opens a new LLDTSync instance to access the PTP clock.
 * @param cfg Pointer to the LLDTSyncCfg_t configuration structure.
 * @return Pointer to the opened LLDTSync instance.
 */
LLDTSync_t *LLDTSyncOpen(LLDTSyncCfg_t *cfg);

/**
 * @brief Closes the LLDTSync instance.
 * @param hTSync Pointer to the LLDTSync instance.
 */
void LLDTSyncClose(LLDTSync_t *hTSync);

/**
 * @brief Retrieves the PTP RX timestamp for a specific message.
 * @param hTSync Pointer to the LLDTSync instance.
 * @param rxPort Receive port.
 * @param msgType Message type.
 * @param seqId Sequence ID.
 * @param domain Domain number.
 * @param ts Pointer to store the PTP RX timestamp.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDTSyncGetRxTime(LLDTSync_t *hTSync, uint8_t rxPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);

/**
 * @brief Retrieves the PTP TX timestamp for a specific message.
 * @param hTSync Pointer to the LLDTSync instance.
 * @param txPort Transmit port.
 * @param msgType Message type.
 * @param seqId Sequence ID.
 * @param domain Domain number.
 * @param ts Pointer to store the PTP TX timestamp.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDTSyncGetTxTime(LLDTSync_t *hTSync, uint8_t txPort, int msgType,
					  uint16_t seqId, uint8_t domain, uint64_t *ts);

/**
 * @brief Adjusts the frequency of the PTP clock.
 * @param hTSync Pointer to the LLDTSync instance.
 * @param ppb Parts per billion adjustment value.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDTSyncAdjFreq(LLDTSync_t *hTSync, int ppb);

/**
 * @brief Sets the time for the PTP clock.
 * @param hTSync Pointer to the LLDTSync instance.
 * @param ts Time to set.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDTSyncSetTime(LLDTSync_t *hTSync, uint64_t ts);

/**
 * @brief Retrieves the current time from the PTP clock.
 * @param hTSync Pointer to the LLDTSync instance.
 * @param ts Pointer to store the current time.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDTSyncGetTime(LLDTSync_t *hTSync, uint64_t *ts);

/**
 * @brief Enables PTP timestamp events for specified ports.
 * @param hTSync Pointer to the LLDTSync instance.
 * @param ports Array of ports to enable timestamp events for.
 * @param numPorts Number of ports in the array.
 * @return LLDENET_E_OK if successful, an error code otherwise.
 */
int LLDTSyncEnableTsEvent(LLDTSync_t *hTSync, uint32_t ports[], uint32_t numPorts);

#endif //LLDTSYNC_H_

/** @}*/
