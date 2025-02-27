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
#ifndef EXCELFORE_CONFIG_UNIT_ACCESS_H_
#define EXCELFORE_CONFIG_UNIT_ACCESS_H_

#include "yang_db_access.h"

/* nconf-server roles macros */
#define ROLE_FLAG_POS_BR  (0x00U)
#define ROLE_FLAG_POS_CUC (0x01U)
#define ROLE_FLAG_POS_CNC (0x02U)
#define IS_ROLE_ENABLED(_val, _pos) ((_val) & (1U << (_pos)))
#define YDBI_IS_BRIDGE(_role) IS_ROLE_ENABLED((_role), ROLE_FLAG_POS_BR)
#define YDBI_IS_CUC(_role)    IS_ROLE_ENABLED((_role), ROLE_FLAG_POS_CUC)
#define YDBI_IS_CNC(_role)    IS_ROLE_ENABLED((_role), ROLE_FLAG_POS_CNC)
#define YDBI_IS_BRIDGE_NAME(_idx, _name) \
	ydbi_is_cuc_cnc_name(ydbi_access_handle(), (_idx), (_name), \
	        ROLE_FLAG_POS_BR)
#define YDBI_IS_CUC_NAME(_idx, _name) \
	ydbi_is_cuc_cnc_name(ydbi_access_handle(), (_idx), (_name), \
	        ROLE_FLAG_POS_CUC)
#define YDBI_IS_CNC_NAME(_idx, _name) \
	ydbi_is_cuc_cnc_name(ydbi_access_handle(), (_idx), (_name), \
	        ROLE_FLAG_POS_CNC)

int ydbi_get_item_cunbk1vk0(yang_db_item_access_t *ydbia, void **rval,
			    uint8_t instIndex, uint8_t k1, bool status);

int ydbi_rel_item_cunbk1vk0(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, uint8_t k1, bool status);

int ydbi_set_item_cunbk1vk0(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, uint8_t k1, bool status,
			    void *value, uint32_t vsize, uint8_t notice);

int ydbi_del_item_cunbk1vk0(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, uint8_t k1, bool status);

int ydbi_get_item_cuk1vk0(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

int ydbi_rel_item_cuk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

int ydbi_set_item_cuk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice);

int ydbi_del_item_cuck1vk0(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint8_t k1, bool status);

int ydbi_get_item_cuck1vk0(yang_db_item_access_t *ydbia, void **rval,
			   const char* bridgename, uint8_t instIndex,
			   uint8_t k1, bool status);

int ydbi_rel_item_cuck1vk0(yang_db_item_access_t *ydbia,
			   const char* bridgename, uint8_t instIndex,
			   uint8_t k1, bool status);

int ydbi_set_item_cuck1vk0(yang_db_item_access_t *ydbia,
			   const char* bridgename, uint8_t instIndex,
			   uint8_t k1, bool status,
			   void *value, uint32_t vsize, uint8_t notice);

int ydbi_del_item_cuck1vk0(yang_db_item_access_t *ydbia, const char* bridgename,
			   uint8_t instIndex, uint8_t k1, bool status);

/**
 * @brief iterate bridgename
 * @param ydbia	ydbia handle
 * @param range	if NULL crate a new range.  not NULL continue
 * @param brname	return bridge name, direct pointer in the DB
 * @param vdata	return data, direct pointer in the DB
 * @param vsize	size of vdata
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param k1	leaf node key parameter
 * @param status	status/config
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_get_iterate_item_cuk1vk0(yang_db_item_access_t *ydbia,
				  uc_range **range, const char **brname,
				  void **vdata, uint32_t *vsize,
				  uint8_t instIndex,
				  uint8_t k1, bool status);

/**
 * @brief Check if a given bridge-name belongs to either CUC or CNC
 * @param ydbia	ydbia handle
 * @param brname	bridge name to check if it belongs to cuc/cnc
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param k1	leaf node key parameter to check
 * @return true: <brname> belongs to CUC/CNC, false: <brname> doesn't belongs to CUC/CNC
 * @note the k1 parameter determines which leaf to check, i.e.
 *  EXCELFORE_CONFIG_UNI_CUC_ENABLED: checks if given brname belongs to CUC
 *  EXCELFORE_CONFIG_UNI_CNC_ENABLED: checks if given brname belongs to CNC
*/
bool ydbi_is_cuc_cnc_name(yang_db_item_access_t *ydbia, uint8_t instIndex,
                          const char* bridgename, uint8_t k1);

/**
 * @brief push event_bridges
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param bridgename	push this name
 * @return -1: error, 0:okay
*/
int ydbi_push_event_bridges(yang_db_item_access_t *ydbia,
			    uint8_t instIndex, const char* bridgename);

/**
 * @brief push event_bridges
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param bridgename	pop bridge name to this, need 33bytes as maximum
 * @return -1: error, 0:okay
*/
int ydbi_pop_event_bridges(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, char *bridgename);

/**
 * @brief initialize mac address pool with 'num' of addresses start from 'addr'
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param addr	start address. default={0x91,0xE0,0xF0,0x00,0xFE,0x00} with addr=NULL
 * @param num	this number of addresses are set in the pool.  maximum is 64
 * @return -1: error, 0:okay
 * @note there is no 'close' function for this. It is re-initialized with repeating calls.
*/
int ydbi_init_mac_pool(yang_db_item_access_t *ydbia,
		       uint8_t instIndex, uint8_t *addr, int num);

/**
 * @brief get one mac address from the pool
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param rmac	return the address in this argument
 * @return -1: error, 0:okay
*/
int ydbi_get_mac_in_pool(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, ub_macaddr_t rmac);

/**
 * @brief release one mac address from the pool
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param rmac	return the address in this argument
 * @return -1: error, 0:okay
*/
int ydbi_rel_mac_in_pool(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, ub_macaddr_t rmac);

#endif
