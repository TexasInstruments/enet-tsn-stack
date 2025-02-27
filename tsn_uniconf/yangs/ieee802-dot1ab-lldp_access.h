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
#ifndef IEEE802_DOT1AB_LLDP_ACCESS_H_
#define IEEE802_DOT1AB_LLDP_ACCESS_H_
#include "yang_db_access.h"

typedef struct
{
    kvs_t vk;
    uint8_t vk_sz; //!< size of kv
} attribute_pair_t;

//ieee802-types.yang -> port-id-subtype-type
typedef enum {
    ePORT_ID_SUBTYPE_INTERFACE_ALIAS=0x01,
    ePORT_ID_SUBTYPE_PORT_COMPONENT,
    ePORT_ID_SUBTYPE_MAC_ADDRESS,
    ePORT_ID_SUBTYPE_NETWORK_ADDRESS,
    ePORT_ID_SUBTYPE_INTERFACE_NAME,
    ePORT_ID_SUBTYPE_AGENT_CIRCUIT_ID,
    ePORT_ID_SUBTYPE_LOCAL,
    ePORT_ID_SUBTYPE_MAX
} lldp_port_id_subtype_t;

typedef struct remote_portid_info {
    const char *brname;
    const char *ifname;
    uint8_t *dstmacaddr;
    uint32_t timemarks;
    uint32_t remoteidx;
    lldp_port_id_subtype_t subtype;
    const char *portid;
} lldp_remote_portid_info_t;

int ydbi_get_item_abk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1,  bool status);

int ydbi_get_item_abk2vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1, uint8_t k2, bool status);

int ydbi_set_item_abk1vk0(yang_db_item_access_t *ydbia, void *rval, uint32_t vsize, uint8_t k1,  bool status, uint8_t notice);

int ydbi_set_item_abk2vk0(yang_db_item_access_t *ydbia, void *rval, uint32_t vsize, uint8_t k1, uint8_t k2, bool status, uint8_t notice);

/**
 * @brief Get val base on n key from LLDP until end
 * Also provided vkn as attribute
 */
int ydbi_get_item_abknvkn(yang_db_item_access_t *ydbia,
                            void **rval,
                            uint8_t kn[],
                            uint8_t kn_sz,
                            attribute_pair_t attrs[],
                            uint8_t kvs_sz,
                            bool status);

/**
 * @brief Set val base on n key from LLDP until end
 * Also provided vkn as attribute
 */
int ydbi_set_item_abknvkn(yang_db_item_access_t *ydbia,
                            uint8_t kn[],
                            uint8_t kn_sz,
                            attribute_pair_t attrs[],
                            uint8_t kvs_sz,
                            bool status,
                            void* value,
                            uint32_t vsize,
                            uint8_t notice);

/**
 * @brief Set val base on n key from LLDP until end
 * Also provided vkn as attribute
 */
int ydbi_del_item_abknvkn(yang_db_item_access_t *ydbia,
                            uint8_t kn[],
                            uint8_t kn_sz,
                            attribute_pair_t attrs[],
                            uint8_t kvs_sz,
                            bool status,
                            uint8_t notice);
/**
 * @brief set ydbi db parameters for remote-systems-data leafs
 * @param ydbia      ydbia handle
 * @param brname     uc-mirror bridge name (used only if ismirror=true)
 * @param name       port's name key value
 * @param destmac    port's dest-mac-address key value
 * @param k1         key of the target leaf
 * @param ismirror   set to true to set uc-mirror prefix.
 * @param iswildcard set to true to set default kss value even if kvs is NULL.
*/
void ydbi_set_dbpara_abk1vk1(yang_db_item_access_t *ydbia, char* brname,
                             char *name, uint8_t *destmac, uint8_t k1,
                             bool ismirror, bool iswildcard);

/**
 * @brief iterate remote port-id data in db
 * @param ydbia 	ydbia handle
 * @param range 	if NULL crate a new range. not NULL continue
 * @param portid 	remote system data structure return.
 * @param ismirror	if set to true, mirrored(prefixed) db data will be search.
 *              	if false, local (non-prefixed) db data will be search.
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_iterate_portid(yang_db_item_access_t *ydbia, uc_range **range,
                        lldp_remote_portid_info_t *portid, bool ismirror);

/**
 * @brief get the port-id-subtype of the given port-id info
 * @param ydbia 	ydbia handle
 * @param portid 	remote system data structure return.
 *               	'ifname', 'dstmacaddr', 'timemarks', and 'remoteidx' must
 *               	be defined.
 * @param ismirror	if set to true, mirrored(prefixed) db data will return.
 *              	if false, local (non-prefixed) db data will be return.
 * @return -1: error, 0:sucess
*/
int ydbi_get_portid_subtype(yang_db_item_access_t *ydbia,
                            lldp_remote_portid_info_t *portid, bool ismirror);

/**
 * @brief deleted the given remote-system-data leaf (k1) with the given port-id info
 * @param ydbia 	ydbia handle
 * @param k1	    remote-system-data leaf to be deleted
 * @param portid 	remote system data structure return.
 *               	'ifname', 'dstmacaddr', 'timemarks', and 'remoteidx' must
 *               	be defined.
 * @param ismirror	if set to true, mirrored(prefixed) db data will be deleted.
 *              	if false, local (non-prefixed) db data will be deleted.
 * @return -1: error, 0:success
*/
int ydbi_del_remote_systems_data(yang_db_item_access_t *ydbia, uint8_t k1,
                                 lldp_remote_portid_info_t *portid, bool ismirror);

#endif /* IEEE802_DOT1AB_LLDP_ACCESS_H_ */
