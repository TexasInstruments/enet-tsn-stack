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
/*
 * lldp_yangdb.h provide wrapper interface to yangdb
 *
 *  Created on: Jun 21, 2023
 *      Author: hoangloc
 */

#ifndef XL4LLDP_LLDP_YANGDB_H_
#define XL4LLDP_LLDP_YANGDB_H_
#include <tsn_uniconf/yangs/ieee802-dot1ab-lldp_access.h>
#include "lldp_cfg.h"

/// @brief Depend on Posix or LLDP, the input Uniconf access mode is different
/// @param mode 
void ydb_set_uniconf_access_mode(uint8_t mode);

/// @brief Load yang db from dbfile to yang_lldp
/// @param dbfile input db file
/// @param yang_lldp output yang data
/// @return 
int initialize_cfg(const char* dbfile, yang_lldp_t* yang_lldp, bool notice);

/// @brief public func used for lldpcli
/// @param ydbia 
/// @param lldp 
/// @return 
int ydbi_load_lldp_db(yang_db_item_access_t *ydbia, yang_lldp_t* lldp);

/// @brief de-init db
void deinit_db();

/// @brief cleanup data before exit
/// @param yang_lldp 
void cleanup_cfg(yang_lldp_t* yang_lldp);

/// @brief below functions should be moved to lldp_cfg.h/c
/// @param port 
/// @param in 
/// @return 
remote_systems_data_t* find_existed_neighbor(lldp_port_t* port, remote_systems_data_t* in);

/// @brief Create new remote system data base on input system data (likely a copy)
/// @param port parent port which store a remote system data list
/// @param in to-be-copied remote system data
remote_systems_data_t * create_new_remote_system_data(lldp_port_t* port, remote_systems_data_t* in);

/// @brief Delete existed remote system data 
/// @param port parent port which store a remote system data list
/// @param rs_data_tobe_deleted to-be-deleted item
void delete_remote_system_data(lldp_port_t* port, remote_systems_data_t* rs_data_tobe_deleted);

/// @brief Compare b/w incoming remote system data with existed one, judge whether remote changed or not
/// @param in Incoming remote systme data
/// @param existed Existed one
bool is_remote_system_data_change(remote_systems_data_t* in, remote_systems_data_t* existed);

/// @brief Update info to db
/// @param port 
/// @param in 
/// @param existed 
/// @return 
void update_remote_system_data(lldp_port_t* port, remote_systems_data_t* updated_data, remote_systems_data_t* existed_data);

/// @brief cleanup all list inside in-mem rs_data
/// @param rs_data 
void cleanup_remote_system_data(remote_systems_data_t* rs_data);

/// @brief Load db for ietf interface, check operation status
/// @param interface_name 
/// @return 
uint8_t ietf_get_oper_status(char* interface_name);

/// @brief Ietf get network info
/// @param interface_name 
/// @return 
int ietf_get_actual_hw_port_info(lldp_port_t* port);

/// @brief Check whether any update on db by 3rd parties like lldpcli or netconf
void monitor_db_change();

/// @brief Increase tx statistic
/// @param port Input port
void increase_tx_frame(lldp_port_t* port);

/// @brief Increase tx len error statistic
/// @param port 
void increase_tx_len_error(lldp_port_t* port);

/// @brief Increase total aged-out statistic
/// @param port 
void increase_rx_total_aged_out(lldp_port_t* port);

/// @brief Increase total discarded frame statistic
/// @param port 
void increase_rx_total_discarded_frame(lldp_port_t* port);

/// @brief Increase total rx error frame statistic
/// @param port 
void increase_rx_error_frame(lldp_port_t* port);

/// @brief Increase total rx frame statistic
/// @param port 
void increase_rx_total_frame(lldp_port_t* port);

/// @brief Increase total discarded RX TLV statistic
/// @param port 
void increase_rx_total_discarded_tlvs(lldp_port_t* port);

/// @brief Increase total unknown TLVs statistic
/// @param port 
void increase_rx_total_unreg_tlvs(lldp_port_t* port);

/// @brief Increase total neighbor inserts time statistic
/// @param port 
void increase_remote_stat_rm_inserts(yang_lldp_t* lldpcfg);

/// @brief Increase total neighbor deletes time statistic
/// @param port 
void increase_remote_stat_rm_deletes(yang_lldp_t* lldpcfg);

/// @brief Increase total neighbor dropped time statistic
/// @param port 
void increase_remote_stat_rm_drops(yang_lldp_t* lldpcfg);

/// @brief Increase total neighbor aged-out time statistic
/// @param port 
void increase_remote_stat_rm_agedouts(yang_lldp_t* lldpcfg);

/// @brief Increase update last changed time statistic on DB
/// @param port 
void update_remote_stat_last_changed_time(yang_lldp_t* lldpcfg);
#endif /* XL4LLDP_LLDP_YANGDB_H_ */
