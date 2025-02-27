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
#ifndef IEEE802_DOT1Q_BRIDGE_ACCESS_H_
#define IEEE802_DOT1Q_BRIDGE_ACCESS_H_

#include "yang_db_access.h"
#include "ieee802-dot1q-bridge.h"
#include <tsn_combase/combase.h>

#define DOT1Q_DEFAULT_BRIDGE_NAME "br0"

typedef struct dq_port_prir{
	uint32_t port;
	uint32_t prir;
} dq_port_prir_t;

enum {
	QB_CTRL_STATUS_INIT=0u,
	QB_CTRL_STATUS_BUSY,
	QB_CTRL_STATUS_SUCCESS,
	QB_CTRL_STATUS_FAILED,
};

void ydbi_set_dbpara_qbk1noinst(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t k1, bool status, bool ismirror);

void ydbi_set_dbpara_qbk1vk0(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint8_t k1, bool status, bool ismirror);

void ydbi_set_dbpara_vlan_reg(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint16_t vid1, uint16_t vid2, uint32_t port_ref,
			  bool ismirror);

void ydbi_set_dbpara_filtering_entry(yang_db_item_access_t *ydbia, bool istalker,
			  const char* bridgename, uint8_t instIndex, uint16_t vid1,
			  uint16_t vid2, ub_macaddr_t destmac, uint32_t port_ref,
			  bool ismirror);

int ydbi_get_item_qbk1vk0(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

int ydbi_set_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status, void *value, uint32_t vsize,
			  uint8_t notice);

int ydbi_del_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

int ydbi_rel_item_qbk1vk0(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

int ydbi_get_item_qbk1noinst(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t k1, bool status);

int ydbi_set_item_qbk1noinst(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice);

int ydbi_del_item_qbk1noinst(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t k1, bool status);

int ydbi_rel_item_qbk1noinst(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t k1, bool status);

int ydbi_get_item_qbk1vk0m(yang_db_item_access_t *ydbia, void **rval,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

int ydbi_set_item_qbk1vk0m(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status, void *value, uint32_t vsize,
			  uint8_t notice);

int ydbi_del_item_qbk1vk0m(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

int ydbi_rel_item_qbk1vk0m(yang_db_item_access_t *ydbia,
			  const char* bridgename, uint8_t instIndex,
			  uint8_t k1, bool status);

/**
 * @brief get registered bridge name
 * @param dbald	database handle
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @return NULL: not registered, pointer:this is direct pointer in DB memory
 * @note get name from /ieee802-dot1q-bridge/bridges/bridge|name:|/component|name:cmp00|/ports
*/
const char *qb_get_bridge_name(uc_dbald *dbald, uint8_t instIndex);


/**
 * @brief register vlans
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param vid1	starting 'vlan id'
 * @param vid2	ending 'vlan id'
 * @param port_ref	port-ref key value
 * @param reg	true-register, false-unregister
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return -1: error, 0:okay
 * @note vlans are group of VLAN IDs(vid1,vid2)
*/
int ydbi_vlan_regis_qb(yang_db_item_access_t *ydbia,
		       const char* bridgename, uint8_t instIndex,
		       uint16_t vid1, uint16_t vid2, uint32_t port_ref,
		       bool reg, bool ismirror);

/**
 * @brief register vlans using external variables
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param database_id	pointer to database-id key value
 * @param vids	range of vlan-id key.
 * @param port_ref	pointer to port-ref key value
 * @param value	pointer to control-element value to be set
 * @param reg	true-register, false-unregister
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return -1: error, 0:okay
 * @note vlans are group of VLAN IDs(vid1,vid2)
*/
int ydbi_vlan_regis_qb_ext(yang_db_item_access_t *ydbia, const char* bridgename,
		       uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
		       uint32_t *port_ref, uint32_t *value, bool reg, bool ismirror);

/**
 * @brief check if vlans are registered
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param vid1	starting 'vlan id'
 * @param vid2	ending 'vlan id'
 * @param port_ref	port-ref key value
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return true: regsitered, false: not registered
 * @note vlans are group of VLAN IDs(vid1,vid2)
*/
bool ydbi_vlan_check_qb(yang_db_item_access_t *ydbia,
			const char* bridgename, uint8_t instIndex,
			uint16_t vid1, uint16_t vid2, uint32_t port_ref, bool ismirror);

/**
 * @brief check if vlans are registered using external variables
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param database_id	pointer to database-id key value
 * @param vids	range of vlan-id key.
 * @param port_ref	pointer to port-ref key value
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return true: regsitered, false: not registered
 * @note vlans are group of VLAN IDs(vid1,vid2)
*/
bool ydbi_vlan_check_qb_ext(yang_db_item_access_t *ydbia, const char *bridgename,
			uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
			uint32_t *port_ref, bool ismirror);

/**
 * @brief return number of bridge ports
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return -1:error, >=0:number of ports
 * @note read "/ieee802-dot1q-bridge/bridges/bridge|name:|/component|name:|/ports"
 *	ports=1 means it is in the end station mode
*/
int ydbi_bridge_ports_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			 uint8_t instIndex, bool ismirror);

/**
 * @brief close uniconf builtin bridge harware functions
 * @param dbald	database handle
 * @param hwald	uniconf harware access handle
 */
void qb_builtin_close(uc_dbald *dbald, uc_hwald *hwald);

/**
 * @brief add/remove listener port
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param destmac	destination mac address
 * @param vid	vlan id
 * @param port_ref	listener port_ref
 * @param reg	true:set, false:remove
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return -1:error, 0:okay
 * @note
 *	add/remove '.../filtering-entry/.../dynamic-filtering-entries/control-element'
 *	control-element has only 'forward' enum, we use add/remove control-element=0
 *	uniconf gets a notice
*/
int ydbi_listener_qb(yang_db_item_access_t *ydbia,
		     const char* bridgename, uint8_t instIndex,
		     ub_macaddr_t destmac, uint16_t vid,
		     uint32_t port_ref, bool reg, bool ismirror);

/**
 * @brief add/remove listener port using external variables
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param database_id	pointer to database-id key value
 * @param vids	range of vlan-id key.
 * @param destmac	destination mac address
 * @param port_ref	listener port_ref
 * @param value	pointer to control-element value to be set
 * @param reg	true:set, false:remove
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return -1:error, 0:okay
 * @note
 *	See note in 'ydbi_listener_qb()'
*/
int ydbi_listener_qb_ext(yang_db_item_access_t *ydbia, const char* bridgename,
		     uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
		     ub_macaddr_t destmac, uint32_t *port_ref, uint32_t *value,
		     bool reg, bool ismirror);

/**
 * @brief add/remove talker port
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param destmac	destination mac address
 * @param vid	vlan id
 * @param port_ref	talker port_ref
 * @param reg	true:set, false:remove
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return -1:error, 0:okay
 * @note
 *	add/remove '.../filtering-entry/.../dynamic-reservation-entries/control-element'
 *	control-element has 'forward' and 'filter' enum,
 *	but we use to save 'priority_rank_reserved' field of TA.
 *	'forward' and 'filter' enum can be lower 4 bits when it is really needed.
 *	uniconf doesn't get a notice
*/
int ydbi_talker_qb(yang_db_item_access_t *ydbia,
		   const char* bridgename, uint8_t instIndex,
		   ub_macaddr_t destmac, uint16_t vid, uint8_t prir,
		   uint32_t port_ref, bool reg, bool ismirror);

/**
 * @brief add/remove talker port using external variables
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param database_id	pointer to database-id key value
 * @param vids	range of vlan-id key.
 * @param destmac	destination mac address
 * @param port_ref	talker port_ref
 * @param value	pointer to control-element value to be set
 * @param reg	true:set, false:remove
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return -1:error, 0:okay
 * @note
 *	See note in 'ydbi_talker_qb()'
*/
int ydbi_talker_qb_ext(yang_db_item_access_t *ydbia, const char* bridgename,
		   uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
		   ub_macaddr_t destmac, uint32_t *port_ref, uint32_t *value,
		   bool reg, bool ismirror);

/**
 * @brief check if talker/listener control-element configs are available or not
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param destmac	destination mac address
 * @param vid	vlan id
 * @param port_ref	talker port_ref
 * @param ismirror	true: with mirror prefix, false: normal db access
 * @return true: regsitered, false: not registered
*/
bool ydbi_listener_check_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			uint8_t instIndex, ub_macaddr_t destmac, uint16_t vid,
			uint32_t port_ref, bool ismirror);
bool ydbi_talker_check_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			uint8_t instIndex, ub_macaddr_t destmac, uint16_t vid,
			uint32_t port_ref, bool ismirror);

/**
 * @brief check if talker/listener control-element configs are available
 *        or not using external variables
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param database_id	pointer to database-id key value
 * @param vids	range of vlan-id key.
 * @param destmac	destination mac address
 * @param port_ref	talker port_ref
 * @return true: regsitered, false: not registered
*/
bool ydbi_listener_check_qb_ext(yang_db_item_access_t *ydbia, const char *bridgename,
			uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
			ub_macaddr_t destmac, uint32_t *port_ref, bool ismirror);
bool ydbi_talker_check_qb_ext(yang_db_item_access_t *ydbia, const char *bridgename,
			uint8_t instIndex, uint32_t *database_id, uint16_t vids[4],
			ub_macaddr_t destmac, uint32_t *port_ref, bool ismirror);

/**
 * @brief get talker port_ref from destmac and vid
 * @param dbald	database handle
 * @param bridgename	if NULL use the default:"br0"
 * @param compname	if NULL use the default:'cmp00'
 * @param destmac	destination mac address
 * @param vid	vlan id
 * @param port_prir	port:found talker port,
 *                      prir:priority_rank_reserved of the found talker
 * @param ppsize	allocated elements of port_prir
 * @return -1: not found, 0: found
 * @note the caller need to release *port_prir with UB_SD_RELMEM(YANGINIT_GEN_SMEM,)
 */
int qb_get_talker_port_prir(uc_dbald *dbald,
			    const char* bridgename, const char* compname,
			    ub_macaddr_t destmac, uint16_t vid,
			    dq_port_prir_t **port_prir, int *ppsize);

/**
 * @brief get port_ref from interface name
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL use the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param name	interface name
 * @param ismirror	set to true to search mirror/prefixed data
 * @return -1: error, 0: not found, >0:port_ref
 */
int ydbi_port_name2ref_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, const char* name, bool ismirror);

/**
 * @brief get interface name from port_ref
 * @param ydbia	ydbia handle
 * @param bridgename	if NULL usef the default:"br0"
 * @param instIndex	'cmp00' the bottom 2digits of hex
 * @param port_ref	port_ref
 * @param name	return interface name
 * @param ismirror	set to true to search mirror/prefixed data
 * @return -1: error, 0: found, the result is in name
 */
int ydbi_port_ref2name_qb(yang_db_item_access_t *ydbia, const char* bridgename,
			  uint8_t instIndex, uint16_t port_ref, char name[CB_MAX_NETDEVNAME],
			  bool ismirror);


#endif
