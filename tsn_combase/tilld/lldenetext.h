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
#ifndef LLDENETEXT_H_
#define LLDENETEXT_H_

#include "combase_private.h"
#include "lldtype.h"
#include "lldenet.h"
#include "combase_link.h"

/**
 * @brief Set parameters for schedule traffic
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param macPort port number of the MAC port to be configured with TAS parameters.
 * @param ctsp input argument to be configured.
 * @return LLDENET_E_OK: on success, an error code otherwise.
 */
int LLDEnetTasSetConfig(LLDEnet_t *hLLDEnet, uint8_t macPort,
			cbl_tas_sched_params_t *ctsp);

/**
 * @brief Set frame preemption parameters for the port indicated by `macPort`.
 *
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param macPort port number of the MAC port to be configured with TAS parameters.
 * @param cpemp IET input parameters to be configured.
 * @param nevent response parameters will be stored in this object.
 * @return LLDENET_E_OK: on success, an error code otherwise.
 */
int LLDEnetIETSetConfig(LLDEnet_t *hLLDEnet, uint8_t macPort,
			cbl_preempt_params_t *cpemp,
			cbl_cb_event_t *nevent);

/**
 *@brief Map Tx DMA channel to a specified HW priority queue
 *@param hLLDEnet pointer to the LLDEnet instance.
 *@param macPorts an array of mac ports to be configured
 *@param nPorts number of mac ports in the array.
 *@param priority HW queue priority to be mapped.
 */
void LLDEnetEnableQueueDMAChannelMapping(LLDEnet_t *hLLDEnet, uint8_t macPorts[],
					 int nPorts, uint8_t priority);

/**
 * @brief Enable EST timestamping
 * @param hLLDEnet pointer to the LLDEnet instance.
 * @param macPort port number of the MAC port to be enabled EST timestamping
 * @param qmapPrm input parameters which indicate priority of queue to be enabled timestamp.
 * @return LLDENET_E_OK: on success, an error code otherwise.
 **/
int LLDEnetTasEnableTimestamp(LLDEnet_t *hLLDEnet, uint8_t macPort,
                              cbl_qmap_params_t *qmapPrm);

/**
 * @brief Disable EST timestamping
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param macPort port number of the MAC port to be disabled EST timestamping
 * @return LLDENET_E_OK: on success, an error code otherwise.
 **/
int LLDEnetTasDisableTimestamp(LLDEnet_t *hLLDEnet, uint8_t macPort);

/**
 * @brief Set parameters for credit based shapping
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param port 0xFF: host port; less than 0xFF:  mac port.
 * @param cbsprm pointer to an object of cbl_cbs_params_t
 * @return LLDENET_E_OK: on success, an error code otherwise.
 **/
int LLDEnetSetCreditBasedShaping(LLDEnet_t *hLLDEnet, uint8_t port,
				 cbl_cbs_params_t *cbsprm);

/**
 * @brief Enable mapping between traffic class and HW queue.
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param macPort index of a mac port to be set.
 * @param cqp TC to queue mapping parameters to be configured.
 * @return LLDENET_E_OK: on success, an error code otherwise.
 **/
int LLDEnetEnableTCQueueMapping(LLDEnet_t *hLLDEnet, uint8_t macPort,
				cbl_qmap_params_t *cqp);

/**
 * @brief Get stats info from host or mac port.
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param port 0xFF: host port; less than 0xFF:  mac port.
 * @note valid index of mac port depends on HW platform
 * @return LLDENET_E_OK: on success, an error code otherwise.
 **/
int LLDEnetGetPortStats(LLDEnet_t *hLLDEnet, uint8_t port, void *prm, int prmSize);

/**
 * @brief Reset stats info from host or mac port.
 * @param hLLDEnet Pointer to the LLDEnet instance.
 * @param port 0xFF: host port; less than 0xFF:  mac port.
 * @note valid index of mac port depends on HW platform
 **/
void LLDEnetResetPortStats(LLDEnet_t *hLLDEnet, uint8_t port);


#endif //LLDENETEXT_H_
