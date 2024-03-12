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
#ifndef IEEE1588_PTP_TT_ACCESS_H_
#define IEEE1588_PTP_TT_ACCESS_H_

#include "yang_db_access.h"
#include "ieee1588-ptp-tt.h"

int ydbi_get_item_ptk4vk1(yang_db_item_access_t *ydbia, void **rval, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1, bool status);
int ydbi_rel_item_ptk4vk1(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1, bool status);
int ydbi_set_item_ptk4vk1(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1,
			  bool status, void *value, uint32_t vsize, uint8_t notice);
int ydbi_del_item_ptk4vk1(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4,
			  void *kv1, uint32_t kvs1,
			  bool status);

int ydbi_get_item_ptk3vk0(yang_db_item_access_t *ydbia, void **rval, uint32_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, bool status);

int ydbi_rel_item_ptk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3, bool status);

int ydbi_set_item_ptk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3,
			  bool status, void *value, uint32_t vsize, uint8_t notice);
int ydbi_del_item_ptk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			  uint8_t k1, uint8_t k2, uint8_t k3,
			  bool status);

/**
 * @brief get instance index from 'gptpInstance' and 'domainIndex'
 * @note check 'instance_domain_Init' in gptp_yangconfg.c for the detail
 */
int ydbi_gptpinstdomain2dbinst_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance, uint8_t domainIndex);

/**
 * @brief reverse of 'ydbi_gptpinstdomain2dbinst_pt'
 */
int ydbi_dbinst2gptpinstdomain_pt(yang_db_item_access_t *ydbia, uint16_t instIndex,
				  uint8_t *gptpInstance, uint8_t *domainIndex);

/**
 * @brief get network devices in the instance
 * @result -1:error, positive number: number of devices
 * @param instIndex	ieee1588-ptp-tt instance index
 * @param netdevs	all data is allocated inside.
 * 	  ydbi_portdevices_pt_release must be called to release the allocated data.
 */
typedef const char * ieee1588_netdev_t;

int ydbi_portdevices_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			uint8_t domainIndex, ieee1588_netdev_t **portdevs);

void ydbi_portdevices_pt_release(ieee1588_netdev_t **portdevs, int devnum);

/**
 * @brief convert portIndex -> netdev
 * @note each (gptpInstance, domainIndex) can have an instance specific set of devices.
 *       To get netdev, use this function.
 */
int ydbi_port_index2device_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			      uint8_t domainIndex, uint16_t portIndex, char *netdev);
/**
 * @brief convert netdev -> portIndex
 * @note reverse conversion of the above ydbi_port_index2device_pt
 */
int ydbi_port_device2index_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			      uint8_t domainIndex, const char *netdev, uint16_t *portIndex);

/**
 * @brief Retrieve configured ptpdev based on partIndex (portIndex -> ptpdev)
 * @note each (gptpInstance, domainIndex) can have an instance specific set of devices.
 *       In the case that portIndex is not known and netdev is known, the caller
 *       should covert netdev to portIndex first by calling `ydbi_port_device2index_pt`.
 */
int ydbi_port_index2ptpdev_pt(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			      uint8_t domainIndex, uint16_t portIndex, char *ptpdev);

/**
 * @brief read asCapable status
 * @param portIndex 0 is used for gptp internal logical port, regular port start with 1
 * @note portIndex in this module shouldn't be shared with the other modules.
 *       It is not a common index.
 */
bool ydbi_get_asCapable(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
			uint8_t domainIndex, uint16_t portIndex);

/**
 * @brief create a semaphore to get notices of asCapable change
 * @param portIndex 0 is used for gptp internal logical port, regular port start with 1
 */
int ydbi_set_asCapable_ucnotice(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
				uint8_t domainIndex, uint16_t portIndex,
				UC_NOTICE_SIG_T **sem, const char *semname);

/**
 * @brief remove all the semaphores registered with 'semname'
 */
int ydbi_clear_asCapable_ucnotice(yang_db_item_access_t *ydbia, const char *semname);

/**
 * @brief call this function to get asCapable value when a notice comes with the semaphore
 *        gptpInstance and domainIndex are also returned.
 *        If 'semname' is unique, the gptpInstance and domainIndex must be the same as
 *        the registered value.
 * @param portIndex 0 is used for gptp internal logical port, regular port start with 1
 */
int ydbi_get_asCapable_ucnotice(yang_db_item_access_t *ydbia, uint8_t *gptpInstance,
				uint8_t *domainIndex, uint8_t *portIndex,
				bool *asCapable, const char *semname);

/**
 * @brief Set value to ClockPerformanceMonitoringDataRecord confitem
 * @param index is the key for the data record
 */
int ydbi_set_perfmon_clock_item(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
                                uint8_t domainIndex, uint16_t index, uint8_t confitem,
                                void *value, uint32_t vsize);

/**
 * @brief Set value to PortPerformanceMonitoringDataRecord confitem
 * @param list is either IEEE1588_PTP_TT_RECORD_LIST or IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY
 * @param index is the key for the data record
 */
int ydbi_set_perfmon_port_item(yang_db_item_access_t *ydbia, uint8_t gptpInstance,
	uint8_t domainIndex, uint16_t portIndex, uint8_t list, uint16_t index,
	uint8_t confitem, void *value, uint32_t vsize);

/**
 * @brief delete all records under the clock performance-monitoring record-list
 * @return true for successful deletion, otherwise false
 */
int ydbi_clear_perfmon_clock_ds(yang_db_item_access_t *ydbia,
		uint8_t gptpInstance, uint8_t domainIndex);

/**
 * @brief cascades ClockPerformanceMonitoringDataRecord in the range of index 0 to 96.
 */
int ydbi_cascade_perfmon_clock_ds(yang_db_item_access_t *ydbia,
		uint8_t gptpInstance, uint8_t domainIndex);

/**
 * @brief delete all records under the port-peer-delay performance-monitoring record-list
 * @param list is either IEEE1588_PTP_TT_RECORD_LIST or IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY
 * @param portIndex 0 is used for gptp internal logical port, regular port start with 1
 * @return 0 for successful deletion, otherwise false
 */
int ydbi_clear_perfmon_port_ds(yang_db_item_access_t *ydbia, uint8_t list,
		uint8_t gptpInstance, uint8_t domainIndex, uint16_t portIndex);

/**
 * @brief cascades PortPerformanceMonitoringDataRecord in the range of index 0 to 96.
 */
int ydbi_cascade_perfmon_port_ds(yang_db_item_access_t *ydbia, uint8_t list,
		uint8_t gptpInstance, uint8_t domainIndex, uint16_t portIndex);

#endif
