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
#ifndef IEEE802_DOT1Q_TSN_CONFIG_UNI_ACCESS_H_
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_ACCESS_H_
#include "yang_db_access.h"
#include "ieee802-dot1q-tsn-config-uni.h"

int ydbi_talker_tu(yang_db_item_access_t *ydbia, uint8_t instIndex, ub_bytearray8_t streamid,
		   const char *netdev, uint32_t cfindex,
		   ub_bytearray6_t smac, ub_bytearray6_t dmac, bool reg);

int ydbi_listener_tu(yang_db_item_access_t *ydbia, uint8_t instIndex, uint8_t *streamid,
		     uint32_t ltindex, const char *netdev, uint32_t cfindex,
		     uint8_t pcp, uint16_t vid, bool reg);

/**
 * @brief scan all streams to get an end station
 * @param ydbia ydbia handle
 * @param instIndex 'domain00' the bottom 2digits of hex string
 * @param count this 'count'th stream is searched
 * @param t_or_l, 't':search talker, 'l':search listener, other:search any
 * @return 0: found, -1:not found,
 * 	      the result comes to stream_id, mac_address, ifname
 */
int ydbi_find_end_station_tu(yang_db_item_access_t *ydbia, uint8_t instIndex,
			     uint8_t count, char t_or_l,
			     uint8_t **stream_id, uint8_t **mac_address, char **ifname);

/**
 * @brief get data in 'data-frame-specification' section
 * @param ydbia ydbia handle
 * @param instIndex 'domain00' the bottom 2digits of hex string
 * @param streamid stream_id
 * @param dfs_index index of 'data-frame-specification' list
 * @param k1
 * @return >0:data size, -1:error
 */
int ydbi_get_item_tufspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t dfs_index,
			  uint8_t k1, uint8_t k2);

/**
 * @brief release of data by ydbi_get_item_tufspec
 * @param ydbia ydbia handle
 * @param instIndex 'domain00' the bottom 2digits of hex string
 * @param streamid stream_id
 * @param dfs_index index of 'data-frame-specification' list
 * @param k1
 * @return 0:released, -1:error
 */
int ydbi_rel_item_tufspec(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  ub_bytearray8_t streamid, uint8_t dfs_index,
			  uint8_t k1, uint8_t k2);

/**
 * @brief set data in 'data-frame-specification' section
 * @param ydbia ydbia handle
 * @param instIndex 'domain00' the bottom 2digits of hex string
 * @param streamid stream_id
 * @param dfs_index index of 'data-frame-specification' list
 * @param k1
 * @param value
 * @param vsize
 * @param notice
 * @return 0:success, -1:error
 */
int ydbi_set_item_tufspec(yang_db_item_access_t *ydbia, uint32_t instIndex,
			  ub_bytearray8_t streamid, uint8_t dfs_index,
			  uint8_t k1, uint8_t k2,
			  void *value, uint32_t vsize, uint8_t notice);

/**
 * @brief get data in 'traffic-specification' section
 * @param ydbia ydbia handle
 * @param instIndex 'domain00' the bottom 2digits of hex string
 * @param streamid stream_id
 * @param k1
 * @param k2
 * @return >0:data size, -1:error
 */
int ydbi_get_item_tutspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t k1, uint8_t k2);

/**
 * @brief get data in 'traffic-specification' section
 * @param ydbia ydbia handle
 * @param instIndex 'domain00' the bottom 2digits of hex string
 * @param streamid stream_id
 * @param k1
 * @param k2
 * @return 0:released, -1:error
 */
int ydbi_rel_item_tutspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t k1, uint8_t k2);
/**
 * @brief get data in 'traffic-specification' section
 * @param ydbia ydbia handle
 * @param instIndex 'domain00' the bottom 2digits of hex string
 * @param streamid stream_id
 * @param k1
 * @param k2
 * @param value
 * @param vsize
 * @param notice
 * @return 0:success, -1:error
 */
int ydbi_set_item_tutspec(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			  ub_bytearray8_t streamid, uint8_t k1, uint8_t k2,
			  void *value, uint32_t vsize, uint8_t notice);

#endif
