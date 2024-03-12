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
 * @addtogroup gptp
 * @{
 * @file gptpgcfg.h
 * @brief gPTP configuration related functions
 *
 */
#ifndef GPTPGCFG_H_
#define GPTPGCFG_H_

#include <tsn_uniconf/yangs/ieee1588-ptp-tt.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief gPTP configuration data
 *
 */
typedef struct gptpgcfg_data gptpgcfg_data_t;

#define XL4GPTP_MODULE "xl4gptp"


/****************************************
 * layer independent functions
 ****************************************/

/**
 * @brief Gets the non-yang 1588-ptp item from the database
 *
 * @param ydbia Yang db item access pointer
 * @param rval Pointer to the value to be retrieved
 * @param instIndex Instance index of the database to access
 * @param k1 Enum key 1 of the item to be retrieved
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return int 0 on success, -1 on failure
 */
int ydbi_get_item_nyptk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			    uint8_t k1, bool status);

/**
 * @brief Releases the blocking of the non-yang 1588-ptp item in the database
 *
 * @param ydbia Yang db item access pointer
 * @param instIndex Instance index of the database to access
 * @param k1 Enum key 1 of the item to be released
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return int 0 on success, -1 on failure
 */
int ydbi_rel_item_nyptk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status);

/**
 * @brief Sets the non-yang 1588-ptp item in the database
 *
 * @param ydbia Yang db item access pointer
 * @param instIndex Instance index of the database to access
 * @param k1 Enum key 1 of the item to be set
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @param value Value to be set
 * @param vsize Size of the value to be set
 * @param notice YDBI_PUSH_NOTICE: push notification, YDBI_NO_NOTICE: no notification
 * @return int
 */
int ydbi_set_item_nyptk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status, void *value, uint32_t vsize,
			    bool notice);

/**
 * @brief Gets the non-yang 1588-ptp item from the database
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem Configuration item to be retrieved
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return void* Pointer to the value retrieved
 */
void* gptpgcfg_get_item(uint8_t gptpInstanceIndex, uint8_t confitem, bool status);

/**
 * @brief Only for NON-YANG Data. Releases the blocking of the non-yang 1588-ptp item in the database
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @return int 0 on success, !0 on failure
 */
int gptpgcfg_get_item_release(uint8_t gptpInstanceIndex);

/**
 * @brief Only for NON-YANG Data. Gets the integer value of the non-yang 1588-ptp item from the database.
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem Configuration item to be retrieved
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return int value of the item retrieved
 */
int gptpgcfg_get_intitem(uint8_t gptpInstanceIndex, uint8_t confitem, bool status);

/**
 * @brief Only for NON-YANG Data. Sets the non-yang 1588-ptp item in the database
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem Configuration item to be set
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @param value Value to be set
 * @param vsize Size of the value to be set
 * @return int 0 on success, !0 on failure
 */
int gptpgcfg_set_item(uint8_t gptpInstanceIndex, uint8_t confitem,
		      bool status, void *value, uint32_t vsize);

/**
 * @brief wait until tsn_gptpd sets port0 PORT_STATE
 * @param ydbia Yang db item access pointer
 * @param gptpInstance Index of the gptp instance to be configured
 * @param tout_ms Time to wait in ms
 * @return 0:got PORT_SATE, 1:timed out
 */
int gptpgcfg_wait_gptpready(yang_db_item_access_t *ydbia, uint8_t gptpInstance, int tout_ms);

/**
 * @brief Copy instance data from one instance to another
 * @param sginst Source gptp instance
 * @param sdomain Source domain
 * @param dginst Destination gptp instance
 * @param ddomain Destination domain
 * @return 0:success, -1:failure
 */
int gptpcfg_copy_instance(uint8_t sginst, uint8_t sdomain, uint8_t dginst, uint8_t ddomain);


/****************************************
 * layer private functions
 ****************************************/
#if defined(GPTP_PRIVATE_H_) || defined(GPTP_STATIC_MEMORY_H_) || defined(LLD_GPTP_PRIVATE_H_)

/**
 * @brief Initialize gptp configuration
 *
 * @param dbname Database name to be used for gptp configuration
 * @param confnames Configuration file names to be used for gptp configuration
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param ucthread true: uniconf is in thread mode, false: uniconf is in process mode
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_init(const char *dbname, const char **confnames,
		uint8_t gptpInstanceIndex, bool ucthread,
		int (*nonconfile_cb)(uint8_t gptpInstanceIndex));

/**
 * @brief Close gptp configuration
 *
 * @param gptpInstanceIndex Index of the gptp to be configured
 */
void gptpgcfg_close(uint8_t gptpInstanceIndex);


/**
 * @brief Gets a yang integer item from gptp configuration
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem0 Index 0 of Enum key of the yang item to be retrieved
 * @param confitem1 Index 1 of Enum key of the yang item to be retrieved
 * @param confitem2 Index 2 of Enum key of the yang item to be retrieved
 * @param domainIndex Index of the gptp domain to be configured
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return int value of the yang item
 */
int gptpgcfg_get_yang_intitem(uint8_t gptpInstanceIndex,
			      uint8_t confitem0, uint8_t confitem1, uint8_t confitem2,
			      uint8_t domainIndex, bool status);

/**
 * @brief Get yang item within the port data set with int data type
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem Configuration item to be retrieved
 * @param pindex Port index to be configured within the gptp instance
 * @param domainIndex Domain index to be configured within the gptp instance
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return int value of the yang item
 */
int gptpgcfg_get_yang_portds_intitem(uint8_t gptpInstanceIndex, uint8_t confitem,
				     uint16_t pindex, uint8_t domainIndex, bool status);

/**
 * @brief Get yang item within the port data set with int64 data type
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem Configuration item to be retrieved
 * @param pindex Port index to be configured within the gptp instance
 * @param domainIndex Domain index to be configured within the gptp instance
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return int64_t value of the yang item
 */
int64_t gptpgcfg_get_yang_portds_int64item(uint8_t gptpInstanceIndex, uint8_t confitem,
					   uint16_t pindex, uint8_t domainIndex, bool status);

/**
 * @brief Sets port specific Yang item
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem1 Index 1 of Enum key of the yang item to be set
 * @param confitem2 Index 2 of Enum key of the yang item to be set
 * @param pindex Port index to be configured within the gptp instance
 * @param domainIndex Domain index to be configured within the gptp instance
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @param value Value to be set
 * @param vsize Data size of the value to be set
 * @param notice YDBI_PUSH_NOTICE: push notification, YDBI_NO_NOTICE: no notification
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_set_yang_port_item(uint8_t gptpInstanceIndex, uint8_t confitem1, uint8_t confitem2,
				uint16_t pindex, uint8_t domainIndex, bool status,
				void *value, int vsize, bool notice);

/**
 * @brief Sets clock state specific Yang item
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param confitem Configuration item to be set
 * @param domainIndex Domain index to be configured within the gptp instance
 * @param status YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @param value Value to be set
 * @param vsize Data size of the value to be set
 * @param notice YDBI_PUSH_NOTICE: push notification, YDBI_NO_NOTICE: no notification
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_set_clock_state_item(uint8_t gptpInstanceIndex, uint8_t confitem,
				  uint8_t domainIndex, bool status,
				  void *value, int vsize, bool notice);

/**
 * @brief Releases database from gptp configuration
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 */
void gptpgcfg_releasedb(uint8_t gptpInstanceIndex);

/**
 * @brief Sets the network devices to be used in gptp configuration
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param netdevs String array of network devices to be used in gptp configuration
 * @param numdevs Number of network devices to be used in gptp configuration
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_set_netdevs(uint8_t gptpInstanceIndex, const char *netdevs[], int numdevs);

/**
 * @brief Removes network devices item from the gptp configuration
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 */
void gptpgcfg_remove_netdevs(uint8_t gptpInstanceIndex);

/**
 * @brief Sets the current asCapable value of the gptp instance
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param domainIndex Domain index to be configured within the gptp instance
 * @param portIndex Port index to be configured within the gptp instance
 * @param asCapable asCapable value of the gptp instance to be set
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_set_asCapable(uint8_t gptpInstanceIndex, uint8_t domainIndex,
			   uint16_t portIndex, bool asCapable);

/**
 * @brief Deletes all clock related values in the performance monitoring data set of the gptp instance
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param domainIndex Domain index to be configured within the gptp instance
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_deleteall_clock_perfmonDS(uint8_t gptpInstanceIndex, uint8_t domainIndex);

/**
 * @brief Cascades the clock related performance monitoring records of the gptp instance
 *
 * @param id 0: Short interval, 1: Long interval
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param domainIndex Domain index to be configured within the gptp instance
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_cascade_clock_perfmonDS(uint8_t id, uint8_t gptpInstanceIndex, uint8_t domainIndex);

/**
 * @brief Deletes all port related values in the performance monitoring data set of the gptp instance
 *
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param domainIndex Domain index to be configured within the gptp instance
 * @param portIndex Port index to be configured within the gptp instance
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_deleteall_port_perfmonDS(uint8_t gptpInstanceIndex, uint8_t domainIndex,
				      uint16_t portIndex);

/**
 * @brief Cascades the specific port related performance monitoring records of the gptp instance
 *
 * @param id 0: Short interval, 1: Long interval
 * @param gptpInstanceIndex Index of the gptp instance to be configured
 * @param domainIndex Domain index to be configured within the gptp instance
 * @param portIndex Port index to be configured within the gptp instance
 * @return int 0 on success, -1 on failure
 */
int gptpgcfg_cascade_port_perfmonDS(uint8_t id, uint8_t gptpInstanceIndex, uint8_t domainIndex,
				    uint16_t portIndex);

#endif // defined(GPTP_PRIVATE_H_) || defined(GPTP_STATIC_MEMORY_H_) || defined(LLD_GPTP_PRIVATE_H_)

#ifdef __cplusplus
}
#endif

#endif

/** @}*/
