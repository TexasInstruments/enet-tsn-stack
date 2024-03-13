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
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/combase.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/yangs/yang_db_runtime.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include <tsn_uniconf/yangs/ieee802-dot1ab-lldp.h>
#include <tsn_uniconf/ucman.h>
#include "lldp_yangdb.h"
#include "lldp_utils.h"
#include <tsn_uniconf/yangs/ieee802-dot1ab-lldp_access.h>
#include <tsn_uniconf/yangs/ietf-interfaces_access.h>
#include <tsn_uniconf/yangs/generated/ieee802-dot1ab-lldp_runconf.h>

#include "interface_mgr.h"

extern int yang_config_init(uc_dbald *dbald, uc_hwald *hwald);

extern uint8_t IEEE802_DOT1AB_LLDP_func(uc_dbald *dbald);
#define IEEE802_DOT1AB_LLDP_RW IEEE802_DOT1AB_LLDP_func(dbald)
#define IEEE802_DOT1AB_LLDP_RO (IEEE802_DOT1AB_LLDP_func(dbald)|0x80u)

#define MAX_KEY_SIZE    8
typedef char semname_t[35]; // strlen("tmp-utlldp_monsem") + 16 + 1 = 34
typedef struct 
{
	struct ub_list_node node; // better to store it as a list
	uint8_t item_size;
	uint8_t keys[MAX_KEY_SIZE]; // All of items wont have kv
	void* kvs[8]; // store value key like port|name=eth0|mac=01:02:03:04:05:06, last item must be semname then NULL
	uint8_t kss[8]; // Size of each value key in kvs
	UC_NOTICE_SIG_T *nsem;	//!< Each item monitor will have one nsem <- key to search in the list
	semname_t semname;
} monitor_item_t; // dynamic create

/// @brief 
// db_mon_item[0] Contains local configuration changed, include: 
//     message-fast-tx
//     message-tx-hold-multiplier 
//     message-tx-interval 
//     reinit-delay
//     tx-credit-max
//     tx-fast-init
//  And local system data changed, include:
//     chassis-id-subtype
//     chassis-id
//     system-name
//     system-description
//     system-capabilities-supported
//     system-capabilities-enabled
//  Once changed, all ports need to follow tx-fast flow
// db_mon_item[1] Contains local port data changed, include: 
//   port/admin-status
//   port/tlvs-tx-enable 
//   port/management-address-tx-port (added/delete/update)
//   port/port-id-subtype
//   port/port-id
//   port/port-desc
//   Once changed, corresponding port need to follow tx-fast flow
monitor_item_t db_mon_item[2];

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

//
UB_SD_GETMEM_DEF(LLDP_CFG_PORT_INSTMEM, (int)sizeof(lldp_port_t),
		 LLDP_CFG_PORT_INSTNUM);
UB_SD_GETMEM_DEF(LLDP_CFG_MGMT_ADDR_TX_PORT_INSTMEM, (int)sizeof(management_address_tx_port_t),
		 LLDP_CFG_MGMT_ADDR_TX_PORT_INSTNUM);
UB_SD_GETMEM_DEF(LLDP_CFG_MGMT_ADDR_INSTMEM, (int)sizeof(management_address_t),
		 LLDP_CFG_MGMT_ADDR_INSTMEM_INSTNUM);
UB_SD_GETMEM_DEF(LLDP_CFG_RM_SYSTEM_DATA_INSTMEM, (int)sizeof(remote_systems_data_t),
		 LLDP_CFG_RM_SYSTEM_DATA_INSTMEM_INSTNUM);
UB_SD_GETMEM_DEF(LLDP_CFG_RM_ORG_DEF_INSTMEM, (int)sizeof(remote_org_defined_info_t),
		 LLDP_CFG_RM_ORG_DEF_INSTMEM_INSTNUM);
UB_SD_GETMEM_DEF(LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM, (int)sizeof(remote_unknown_tlv_t),
		 LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM_INSTNUM);
//

// use to input to clear_cb to avoid many clearcb function
#define CLEAR_CMD_MON_ITEM			1
#define CLEAR_CMD_RMT_SYSTEM_DATA	2
#define CLEAR_CMD_RMT_UNKNOWN_TLV	3
#define CLEAR_CMD_RMT_ORG_TLV		4
#define CLEAR_CMD_MGMT_ADDR			5
#define CLEAR_CMD_MGMT_ADDR_TX		6
#define CLEAR_CMD_PORT				7

yang_lldp_t* g_yang_lldp = NULL; //!< For reference use ONLY
uint8_t g_uniconf_access_mode = 0;

/**
 * @brief Refered from uniconfmon. Private struct to store data queried 
 * to shortern params number in functions
 */
typedef struct lldp_cfg_param {
	uint8_t kss[UC_MAX_VALUEKEYS];
	kvs_t kvs[UC_MAX_VALUEKEYS+1]; // +1 for NULL termination
	uint8_t *aps;
	void *value;
	uint32_t vsize;
	char *rstr;
	char *uccfgstr;
	char *uccfgnomod;
	int vtype;
	uint8_t kvi;
} lldp_cfg_param_t;

static bool is_mac_valid (ub_macaddr_t mac);
static bool is_manangement_addr_tx_port_existed(lldp_port_t* port, address_subtype_t subtype, char* man_address);
static management_address_tx_port_t* get_existed_manament_addr_tx(lldp_port_t* port, address_subtype_t subtype, char* man_address);
static bool is_remote_unknown_tlv_existed(remote_systems_data_t* rs_data, uint32_t tlv_type);
static remote_unknown_tlv_t* get_existed_remote_unknown_tlv(remote_systems_data_t* rs_data, uint32_t tlv_type);
static bool is_remote_org_defined_existed(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index);
static remote_org_defined_info_t* get_existed_remote_org(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index);
static bool is_manangement_addr_existed(remote_systems_data_t* rs_data, address_subtype_t subtype, char* addr);
static management_address_t* get_existed_manament_addr(remote_systems_data_t* rs_data, address_subtype_t subtype, char* address);
static bool is_remote_system_data_existed(lldp_port_t* port, timeticks time_mark, uint32_t remote_index);
static bool is_port_existed(yang_lldp_t* lldp, const char * if_name, uint8_t* mac);
static lldp_port_t* get_existed_port(yang_lldp_t* lldp, const char * if_name, uint8_t* mac);
static void fill_remote_statistic_info(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp);
static void fill_local_system_data(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp);
static void fill_management_info(uint8_t* k, address_subtype_t addr_subtype, char* man_addr, lldp_cfg_param_t* prm, lldp_port_t* port);
static void fill_manament_addr(uint8_t* k, address_subtype_t addr_subtype, char* addr, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data);
static void fill_remote_unknown_tlv(uint8_t* k, uint32_t tlv_type, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data);
static void fill_remote_org_info(uint8_t* k, uint32_t info_id, uint32_t info_subtype, uint32_t info_index, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data);
static void fill_port_info(uint8_t* k, char* if_name, uint8_t* mac, lldp_cfg_param_t* prm, yang_lldp_t* lldp);
static void fill_lldp_config_info(void *key, yang_lldp_t* lldp, lldp_cfg_param_t* prm);
static bool is_same_mgmt_addr_list(management_address_list_t* mgmt_addr1, management_address_list_t* mgmt_addr2);
// APIs for update db
static int write_remote_system_data_to_db(lldp_port_t* port, remote_systems_data_t* new_rs_data);
static void update_rs_data_1st_level(lldp_port_t* port, remote_systems_data_t* new_rs_data, ieee802_dot1ab_lldp_enum_t k);
static void update_rs_data_2nd_level(lldp_port_t* port, remote_systems_data_t* new_rs_data, ieee802_dot1ab_lldp_enum_t k1, ieee802_dot1ab_lldp_enum_t k2);
static void delete_rs_data_1st_level(lldp_port_t* port, remote_systems_data_t* existed_rs_data, ieee802_dot1ab_lldp_enum_t k);
static void delete_rs_data_2nd_level(lldp_port_t* port, remote_systems_data_t* existed_rs_data, ieee802_dot1ab_lldp_enum_t k1, ieee802_dot1ab_lldp_enum_t k2);
static void update_port_statistic(lldp_port_t* port, uint8_t tx_or_rx_key, ieee802_dot1ab_lldp_enum_t k);
static void on_port_db_change();
static void on_local_system_change();

static int deregister_notice(uc_dbald *dbald, uc_notice_data_t *ucntd);
static void clear_cb(struct ub_list_node *node, void *arg);

static void fill_mgmt_addr_attr(management_address_t* mgmt_addr, attribute_pair_t* attr_pairs);
static void fill_rm_org_info_attr(remote_org_defined_info_t* remote_org, attribute_pair_t* attr_pairs);
static void fill_rm_unknown_tlv_attr(remote_unknown_tlv_t* unknown_tlv, attribute_pair_t* attr_pairs);

/// @brief Handle mgmt addr list changed. Can add new or update
/// @param existed_port 
/// @param updated_port 
static void update_port_mgmt_addr_tx(lldp_port_t* existed_port, lldp_port_t* updated_port)
{
	struct ub_list_node* tmp_node;
	for(UB_LIST_FOREACH_ITER(&updated_port->management_address_tx_port, tmp_node))
	{
		management_address_tx_port_t* updated_addr_tx = (management_address_tx_port_t*)tmp_node;
		management_address_tx_port_t* existed_addr_tx = get_existed_manament_addr_tx(existed_port, updated_addr_tx->address_subtype, (char*)updated_addr_tx->man_address);
		if (existed_addr_tx == NULL)
		{
			existed_addr_tx = init_mgmt_tx_port();
			existed_addr_tx->address_subtype = updated_addr_tx->address_subtype;
			existed_addr_tx->if_subtype = updated_addr_tx->if_subtype;
			existed_addr_tx->if_id = updated_addr_tx->if_id;
			memcpy(existed_addr_tx->man_address, updated_addr_tx->man_address, sizeof(updated_addr_tx->man_address));
			existed_addr_tx->tx_enable = updated_addr_tx->tx_enable;

			ub_list_append(&existed_port->management_address_tx_port, (struct ub_list_node*) existed_addr_tx);
		}
		else
		{
			existed_addr_tx->address_subtype = updated_addr_tx->address_subtype;
			existed_addr_tx->if_subtype = updated_addr_tx->if_subtype;
			existed_addr_tx->if_id = updated_addr_tx->if_id;
			memcpy(existed_addr_tx->man_address, updated_addr_tx->man_address, sizeof(updated_addr_tx->man_address));
			existed_addr_tx->tx_enable = updated_addr_tx->tx_enable;
		}
	}
}

/// @brief Handle port DB change
/// @param found_updated_item 
static void on_port_db_change()
{
	yang_lldp_t tmp_lldp_cfg;
	memset(&tmp_lldp_cfg, 0, sizeof(yang_lldp_t));
	ub_list_init(&tmp_lldp_cfg.ports);

	int ret = ydbi_load_lldp_db(ydbi_access_handle(), &tmp_lldp_cfg);
	if (ret == 0)
	{
		struct ub_list_node* tmp_node;
		for(UB_LIST_FOREACH_ITER(&tmp_lldp_cfg.ports, tmp_node))
		{
			lldp_port_t* updated_port = (lldp_port_t*)tmp_node;

			// Compare each pair of updated port and existed one
			lldp_port_t* existed_port = get_existed_port(g_yang_lldp, updated_port->name, updated_port->dest_mac_address);
			if (existed_port == NULL)
			{
				UB_LOG(UBL_WARN, "%s %s[%x-%x-%x-%x-%x-%x] Not existed in current DB\n", __func__,  (char*) updated_port->name,
						updated_port->dest_mac_address[0], updated_port->dest_mac_address[1], updated_port->dest_mac_address[2], 
						updated_port->dest_mac_address[3], updated_port->dest_mac_address[4], updated_port->dest_mac_address[5]);
				break;	
			}
			else
			{
				// Consider admin-status changed
				if (updated_port->admin_status != existed_port->admin_status)
				{
					UB_LOG(UBL_INFO, "%s:%s Detect admin status changed %d->%d\n", __func__,  existed_port->name , existed_port->admin_status, updated_port->admin_status);
					on_port_adminstatus_changed(updated_port->name, updated_port->dest_mac_address, updated_port->admin_status);
				}
				else
				{
					UB_LOG(UBL_INFO, "%s:%s scanned tlvs_tx_enable: %x -> %x\n", __func__, updated_port->name,  existed_port->tlvs_tx_enable, updated_port->tlvs_tx_enable);
					existed_port->tlvs_tx_enable = updated_port->tlvs_tx_enable;

					UB_LOG(UBL_INFO, "%s:%s scanned port_desc: %s -> %s\n", __func__, updated_port->name,  existed_port->port_desc, updated_port->port_desc);
					memset(existed_port->port_desc, 0, sizeof(existed_port->port_desc));
					memcpy(existed_port->port_desc, updated_port->port_desc, strlen(updated_port->port_desc));

					UB_LOG(UBL_INFO, "%s:%s scanned port_id_subtype: %d -> %d\n", __func__, updated_port->name,  existed_port->port_id_subtype, updated_port->port_id_subtype);
					existed_port->port_id_subtype = updated_port->port_id_subtype;

					UB_LOG(UBL_INFO, "%s:%s scanned port_id: %s -> %s\n", __func__, updated_port->name,  existed_port->port_id, updated_port->port_id);
					memset(existed_port->port_id, 0, sizeof(existed_port->port_id));
					memcpy(existed_port->port_id, updated_port->port_id, strlen((char*)updated_port->port_id));

					UB_LOG(UBL_INFO, "%s:%s scanned message_fast_tx: %d -> %d\n", __func__, updated_port->name, existed_port->message_fast_tx, updated_port->message_fast_tx);
					existed_port->message_fast_tx = updated_port->message_fast_tx;

					UB_LOG(UBL_INFO, "%s:%s scanned message_tx_hold_multiplier: %d -> %d\n", __func__, updated_port->name,  existed_port->message_tx_hold_multiplier, updated_port->message_tx_hold_multiplier);
					existed_port->message_tx_hold_multiplier = updated_port->message_tx_hold_multiplier;

					UB_LOG(UBL_INFO, "%s:%s scanned message_tx_interval: %d -> %d\n", __func__, updated_port->name,  existed_port->message_tx_interval, updated_port->message_tx_interval);
					existed_port->message_tx_interval = updated_port->message_tx_interval;

					UB_LOG(UBL_INFO, "%s:%s scanned reinit_delay: %d -> %d\n", __func__, updated_port->name,  existed_port->reinit_delay, updated_port->reinit_delay);
					existed_port->reinit_delay = updated_port->reinit_delay;

					UB_LOG(UBL_INFO, "%s:%s scanned tx_credit_max: %d -> %d\n", __func__, updated_port->name,  existed_port->tx_credit_max, updated_port->tx_credit_max);
					existed_port->tx_credit_max = updated_port->tx_credit_max;

					UB_LOG(UBL_INFO, "%s:%s scanned tx_fast_init: %d -> %d\n", __func__, updated_port->name,  existed_port->tx_fast_init, updated_port->tx_fast_init);
					existed_port->tx_fast_init = updated_port->tx_fast_init;

					update_port_mgmt_addr_tx(existed_port, updated_port);

					on_local_changed_on_port(existed_port->name, existed_port->dest_mac_address);
				}
			}
		}

		cleanup_cfg(&tmp_lldp_cfg);
	}
	else
	{
		UB_LOG(UBL_ERROR, "%s:Cannot load db\n", __func__);
	}
}

static void chk_if_change_then_apply_all_ports(uint8_t k, uint32_t curr_val, uint32_t new_val)
{
	if (curr_val != new_val)
	{
		struct ub_list_node* tmp_node;
		for(UB_LIST_FOREACH_ITER(&g_yang_lldp->ports, tmp_node))
		{
			lldp_port_t* port = (lldp_port_t*)tmp_node;
			switch(k)
			{
				case IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX:
					port->message_fast_tx = new_val;
					break;
				case IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER:
					port->message_tx_hold_multiplier = new_val;
					break;
				case IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL:
					port->message_tx_interval = new_val;
					break;
				case IEEE802_DOT1AB_LLDP_REINIT_DELAY:
					port->reinit_delay = new_val;
					break;
				case IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX:
					port->tx_credit_max = new_val;
					break;
				case IEEE802_DOT1AB_LLDP_TX_FAST_INIT:
					port->tx_fast_init = new_val;
					break;
				default:
					// not support
					break;
			}
		}
	}
}

/// @brief Handle local system changed
/// @param found_updated_item 
static void on_local_system_change()
{
	yang_lldp_t tmp_lldp_cfg;
	memset(&tmp_lldp_cfg, 0, sizeof(yang_lldp_t));
	ub_list_init(&tmp_lldp_cfg.ports);
	int ret = ydbi_load_lldp_db(ydbi_access_handle(), &tmp_lldp_cfg);
	if (ret == 0)
	{
		UB_LOG(UBL_INFO, "%s: scanned local_system_data: %s -> %s\n", __func__,  g_yang_lldp->local_system_data.system_name, tmp_lldp_cfg.local_system_data.system_name);
		memset(g_yang_lldp->local_system_data.system_name, 0, sizeof(g_yang_lldp->local_system_data.system_name));
		memcpy(g_yang_lldp->local_system_data.system_name, tmp_lldp_cfg.local_system_data.system_name, strlen(tmp_lldp_cfg.local_system_data.system_name));
		
		UB_LOG(UBL_INFO, "%s: scanned chassis_id_subtype: %d -> %d\n", __func__,  g_yang_lldp->local_system_data.chassis_id_subtype, tmp_lldp_cfg.local_system_data.chassis_id_subtype);
		g_yang_lldp->local_system_data.chassis_id_subtype = tmp_lldp_cfg.local_system_data.chassis_id_subtype;

		UB_LOG(UBL_INFO, "%s: scanned chassis_id: %s -> %s\n", __func__, g_yang_lldp->local_system_data.chassis_id, tmp_lldp_cfg.local_system_data.chassis_id);
		memset(g_yang_lldp->local_system_data.chassis_id, 0, sizeof(g_yang_lldp->local_system_data.chassis_id));
		memcpy(g_yang_lldp->local_system_data.chassis_id, tmp_lldp_cfg.local_system_data.chassis_id, sizeof(tmp_lldp_cfg.local_system_data.chassis_id));
		
		UB_LOG(UBL_INFO, "%s: scanned system_description: %s -> %s\n", __func__,  g_yang_lldp->local_system_data.system_description, tmp_lldp_cfg.local_system_data.system_description);
		memset(g_yang_lldp->local_system_data.system_description, 0, sizeof(g_yang_lldp->local_system_data.system_description));
		memcpy(g_yang_lldp->local_system_data.system_description, tmp_lldp_cfg.local_system_data.system_description, sizeof(tmp_lldp_cfg.local_system_data.system_description));
		
		UB_LOG(UBL_INFO, "%s: scanned system_capabilities_enabled: %x -> %x\n", __func__,  g_yang_lldp->local_system_data.system_capabilities_enabled, tmp_lldp_cfg.local_system_data.system_capabilities_enabled);
		g_yang_lldp->local_system_data.system_capabilities_enabled = tmp_lldp_cfg.local_system_data.system_capabilities_enabled;

		UB_LOG(UBL_INFO, "%s: scanned system_capabilities_supported: %x -> %x\n", __func__,  g_yang_lldp->local_system_data.system_capabilities_supported, tmp_lldp_cfg.local_system_data.system_capabilities_supported);
		g_yang_lldp->local_system_data.system_capabilities_supported = tmp_lldp_cfg.local_system_data.system_capabilities_supported;

		UB_LOG(UBL_INFO, "%s: scanned message_fast_tx: %d -> %d\n", __func__,  g_yang_lldp->message_fast_tx, tmp_lldp_cfg.message_fast_tx);
		chk_if_change_then_apply_all_ports(IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX,  g_yang_lldp->message_fast_tx, tmp_lldp_cfg.message_fast_tx);
		g_yang_lldp->message_fast_tx = tmp_lldp_cfg.message_fast_tx;

		UB_LOG(UBL_INFO, "%s: scanned message_tx_hold_multiplier: %d -> %d\n", __func__,  g_yang_lldp->message_tx_hold_multiplier, tmp_lldp_cfg.message_tx_hold_multiplier);
		chk_if_change_then_apply_all_ports(IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER,  g_yang_lldp->message_tx_hold_multiplier, tmp_lldp_cfg.message_tx_hold_multiplier);
		g_yang_lldp->message_tx_hold_multiplier = tmp_lldp_cfg.message_tx_hold_multiplier;

		UB_LOG(UBL_INFO, "%s: scanned message_tx_interval: %d -> %d\n", __func__,  g_yang_lldp->message_tx_interval, tmp_lldp_cfg.message_tx_interval);
		chk_if_change_then_apply_all_ports(IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL,  g_yang_lldp->message_tx_interval, tmp_lldp_cfg.message_tx_interval);
		g_yang_lldp->message_tx_interval = tmp_lldp_cfg.message_tx_interval;

		UB_LOG(UBL_INFO, "%s: scanned reinit_delay: %d -> %d\n", __func__,  g_yang_lldp->reinit_delay, tmp_lldp_cfg.reinit_delay);
		chk_if_change_then_apply_all_ports(IEEE802_DOT1AB_LLDP_REINIT_DELAY,  g_yang_lldp->reinit_delay, tmp_lldp_cfg.reinit_delay);
		g_yang_lldp->reinit_delay = tmp_lldp_cfg.reinit_delay;

		UB_LOG(UBL_INFO, "%s: scanned tx_credit_max: %d -> %d\n", __func__,  g_yang_lldp->tx_credit_max, tmp_lldp_cfg.tx_credit_max);
		chk_if_change_then_apply_all_ports(IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX,  g_yang_lldp->tx_credit_max, tmp_lldp_cfg.tx_credit_max);
		g_yang_lldp->tx_credit_max = tmp_lldp_cfg.tx_credit_max;

		UB_LOG(UBL_INFO, "%s: scanned tx_fast_init: %d -> %d\n", __func__,  g_yang_lldp->tx_fast_init, tmp_lldp_cfg.tx_fast_init);
		chk_if_change_then_apply_all_ports(IEEE802_DOT1AB_LLDP_TX_FAST_INIT,  g_yang_lldp->tx_fast_init, tmp_lldp_cfg.tx_fast_init);
		g_yang_lldp->tx_fast_init = tmp_lldp_cfg.tx_fast_init;

		broadcast_local_system_changed();

		cleanup_cfg(&tmp_lldp_cfg);
	}
	else
	{
		UB_LOG(UBL_ERROR, "%s:Cannot load db\n", __func__);
	}
}

static void on_db_changed(int mon_index)
{
	switch(mon_index)
	{
		case 0:
			on_local_system_change();
			break;
		case 1:
			on_port_db_change();
			break;
		default:
			break;
	}
	
	uc_dbal_releasedb(ydbi_access_handle()->dbald);
}

void monitor_db_change()
{
	int res;
	uint8_t key[UC_MAX_KEYSIZE];
	uint32_t ksize;

	monitor_item_t* found_updated_item = NULL;
	int i;
	for(i=0; i<2; i++)
	{
		monitor_item_t* m = (monitor_item_t*)&db_mon_item[i];
		if (m != NULL)
		{
			if(!uc_notice_sig_check(false, m->nsem, 0, __func__))
			{
				found_updated_item = m;
				break;
			}
		}
	}

	if(found_updated_item == NULL) {return; /*no notice. Do nthing*/}
	// received a notice
	if (i==0) {UB_LOG(UBL_INFO, "%s:a notice from local system\n", __func__);}
	else {UB_LOG(UBL_INFO, "%s:a notice from local port\n", __func__);}

	// get a key of signaled data
	res=uc_nc_get_notice_act(ydbi_access_handle()->ucntd, ydbi_access_handle()->dbald, found_updated_item->semname, key, &ksize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:get a signal, but no data in uc_nc_get_notice_act, "
			"res=%d\n", __func__, res);
		return;
	}

	on_db_changed(i);
}

static void init_local_system_notice(uc_dbald *dbald, uc_notice_data_t *ucntd, yang_db_runtime_dataq_t *ydrd, yang_lldp_t* yang_lldp)
{
	int i=0;
	monitor_item_t* local_system_mon_item = &db_mon_item[0];
	local_system_mon_item->nsem = NULL;	
	sprintf(local_system_mon_item->semname, "tmp-utlldp_monsem%16lx", (unsigned long)ub_rt_gettime64());

	// If we got notice in these field. All of relate port will be applied the same value.
	uint8_t local_mon_key[] =
	{
		IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX,
		IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER,
		IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL,
		IEEE802_DOT1AB_LLDP_REINIT_DELAY,
		IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX,
		IEEE802_DOT1AB_LLDP_TX_FAST_INIT,
	};

	for (i=0; i<sizeof(local_mon_key); i++)
	{
		local_system_mon_item->kvs[0] = (char*)local_system_mon_item->semname;
		local_system_mon_item->kss[0] = strlen(local_system_mon_item->semname)+1;
		local_system_mon_item->kvs[1] = NULL;
		local_system_mon_item->keys[0] = IEEE802_DOT1AB_LLDP_RW;
		local_system_mon_item->keys[1] = IEEE802_DOT1AB_LLDP_LLDP;
		local_system_mon_item->keys[2] = local_mon_key[i];
		local_system_mon_item->keys[3] = 255;

		if (local_system_mon_item->nsem == NULL)
		{
			if(uc_nc_notice_register(ucntd, 
								dbald, 
								local_system_mon_item->keys, 
								local_system_mon_item->kvs, 
								local_system_mon_item->kss,
								UC_NOTICE_DBVAL_ADD, 
								&local_system_mon_item->nsem))
			{
				UB_LOG(UBL_ERROR, "%s: Register notice for LOCAL_SYSTEM_CHANGED failed \n", __func__);
				return;
			}
		}
		else
		{
			// After having the nsem, we just need to input current sem-name
			if(uc_nc_notice_register(ucntd, 
								dbald, 
								local_system_mon_item->keys, 
								local_system_mon_item->kvs, 
								local_system_mon_item->kss,
								UC_NOTICE_DBVAL_ADD, 
								NULL))
			{
			UB_LOG(UBL_ERROR, "%s: Register notice for LOCAL_SYSTEM_CHANGED failed \n", __func__);
				return;
			}
		}
	}
}

static void init_per_port_notice(uc_dbald *dbald, uc_notice_data_t *ucntd, yang_db_runtime_dataq_t *ydrd, yang_lldp_t* yang_lldp, lldp_port_t* port)
{
	int i=0;
	monitor_item_t* local_port_mon_item = &db_mon_item[1];

	uint8_t lldp_port_mon_item[] =
	{
		IEEE802_DOT1AB_LLDP_ADMIN_STATUS,
		IEEE802_DOT1AB_LLDP_TLVS_TX_ENABLE,
		IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX,
		IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER,
		IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL,
		IEEE802_DOT1AB_LLDP_REINIT_DELAY,
		IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX,
		IEEE802_DOT1AB_LLDP_TX_FAST_INIT,
	};

	// default vals
	local_port_mon_item->keys[0] = IEEE802_DOT1AB_LLDP_RW;
	local_port_mon_item->keys[1] = IEEE802_DOT1AB_LLDP_LLDP;
	local_port_mon_item->keys[2] = IEEE802_DOT1AB_LLDP_PORT;

	// key 0, 1 must be port name/mac addr
	local_port_mon_item->kvs[0] = port->name;
	local_port_mon_item->kss[0] = strlen(port->name)+1;

	local_port_mon_item->kvs[1] = &port->dest_mac_address[0];
	local_port_mon_item->kss[1] = 6;

	for (i=0; i<sizeof(lldp_port_mon_item); i++)
	{
		local_port_mon_item->keys[3] = lldp_port_mon_item[i];
		local_port_mon_item->keys[4] = 255;

		local_port_mon_item->kvs[2] = (char*)local_port_mon_item->semname;
		local_port_mon_item->kss[2] = strlen(local_port_mon_item->semname)+1;
		local_port_mon_item->kvs[3] = NULL;

		if (local_port_mon_item->nsem == NULL)
		{
			if(uc_nc_notice_register(ucntd, dbald, local_port_mon_item->keys, local_port_mon_item->kvs, local_port_mon_item->kss, UC_NOTICE_DBVAL_ADD, &local_port_mon_item->nsem))
			{
				UB_LOG(UBL_ERROR, "%s: Register notice for LOCAL_SYSTEM_CHANGED failed \n", __func__);
				return;
			}
		}
		else
		{
			// After having the nsem, we just need to input current sem-name
			if(uc_nc_notice_register(ucntd, dbald, local_port_mon_item->keys, local_port_mon_item->kvs, local_port_mon_item->kss, UC_NOTICE_DBVAL_ADD, NULL))
			{
				UB_LOG(UBL_ERROR, "%s: Register notice for local port change failed \n", __func__);
				return;
			}
		}
	}

	struct ub_list_node* tmp_node;
	for(UB_LIST_FOREACH_ITER(&port->management_address_tx_port, tmp_node))
	{
		management_address_tx_port_t* mgmt_addr_tx = (management_address_tx_port_t*)tmp_node;
		local_port_mon_item->keys[3] = IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS_TX_PORT;
		local_port_mon_item->keys[4] = IEEE802_DOT1AB_LLDP_TX_ENABLE;
		local_port_mon_item->keys[5] = 255;

		// key 1, 2 must be mgmt subtype/addr
		local_port_mon_item->kvs[2] = &mgmt_addr_tx->address_subtype;
		local_port_mon_item->kss[2] = sizeof(mgmt_addr_tx->address_subtype);

		local_port_mon_item->kvs[3] = &mgmt_addr_tx->man_address;
		local_port_mon_item->kss[3] = strlen((char*)mgmt_addr_tx->man_address)+1;

		local_port_mon_item->kvs[4] = (char*)local_port_mon_item->semname;
		local_port_mon_item->kss[4] = strlen((char*)local_port_mon_item->semname)+1;
		local_port_mon_item->kvs[5] = NULL;

		if(uc_nc_notice_register(ucntd, dbald, local_port_mon_item->keys, local_port_mon_item->kvs, local_port_mon_item->kss, UC_NOTICE_DBVAL_ADD, NULL))
		{
			UB_LOG(UBL_ERROR, "%s: Register notice for IEEE802_DOT1AB_LLDP_TX_ENABLE failed \n", __func__);
			return;
		}
	}
}

static void init_notice(uc_dbald *dbald, uc_notice_data_t *ucntd, yang_db_runtime_dataq_t *ydrd, yang_lldp_t* yang_lldp)
{
	struct ub_list_node* tmp_node;

	init_local_system_notice(dbald, ucntd, ydrd, yang_lldp);

	db_mon_item[1].nsem = NULL;
	sprintf(db_mon_item[1].semname, "tmp-utlldp_monsem%16lx", (unsigned long)ub_rt_gettime64());
	for(UB_LIST_FOREACH_ITER(&yang_lldp->ports, tmp_node))
	{
		lldp_port_t* port = (lldp_port_t*)tmp_node;
		init_per_port_notice(dbald, ucntd, ydrd, yang_lldp, port);
	}
}

static int deregister_notice(uc_dbald *dbald, uc_notice_data_t *ucntd)
{
	for(int i=0; i<2; i++)
	{
		monitor_item_t* m = &db_mon_item[i];
		if (m != NULL)
		{
			if(uc_nc_notice_deregister_all(ucntd, dbald, m->semname))
			{
				return -1;
			}
		}
	}

	return 0;
}

uint8_t ietf_get_oper_status(char* interface_name)
{
	uint8_t up_down = 2; // down
	void *value;

	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk1vk0, up_down, value,
			     interface_name, IETF_INTERFACES_OPER_STATUS, YDBI_STATUS);
	return up_down;
}

/// @brief Ietf get network info
/// @param The port id, port id subtype and mac addr should be get from actual hw instead of lldp yang config
/// @return 
int ietf_get_actual_hw_port_info(lldp_port_t* port)
{
	int ret = 0;

	void *rval;
	int vsize;

	vsize=YDBI_GET_ITEM(ifk1vk0, rval, port->name,
			IETF_INTERFACES_PHYS_ADDRESS, YDBI_STATUS);
	ydbi_get_item_release(ydbi_access_handle(), YDBI_REL_LOCK);

	if (vsize == 6)
	{
		if (port->port_id_subtype == P_MAC_ADDRESS)
		{
			ub_macaddr_t bmac;
			char smac[18];
			memcpy(bmac, rval, sizeof(bmac));
			lldp_bmac2smac(bmac, smac, '-');

			memcpy(port->port_id, smac, strlen(smac));
			UB_LOG(UBL_INFO, "%s:%s MAC: %s\n", __func__, port->name, smac);
		}

		memcpy(port->src_mac_address, rval, sizeof(port->src_mac_address));

	}
	else
	{
		UB_LOG(UBL_ERROR, "%s:%s Cannot Get MAC Address \n", __func__, port->name);
	}


	return ret;
}

int initialize_cfg(const char* dbfile, yang_lldp_t* yang_lldp, bool notice)
{
	int ret = -1;
	uc_dbald *dbald = NULL;
	uc_notice_data_t *ucntd=NULL;

	ret=uniconf_ready(dbfile, g_uniconf_access_mode, 100);
	if (ret == 0)
	{
		// Init
		dbald=uc_dbal_open(dbfile, "w", g_uniconf_access_mode);
		ucntd=uc_notice_init(g_uniconf_access_mode, dbfile);
		if (!ucntd) { return -1; }

		ydbi_access_init(dbald, ucntd);

		ret = ydbi_load_lldp_db(ydbi_access_handle(), yang_lldp);
		if (ret == 0)
		{
			UB_LOG(UBL_INFO, "%s: loaded yang db\n", __func__);
			g_yang_lldp = yang_lldp;
		}

		if (notice)
		{
			// init_notice(dbald, ucntd, NULL, yang_lldp);
			init_notice(dbald, ucntd, NULL, yang_lldp);
		}
	}
	else
	{
		UB_LOG(UBL_ERROR, "%s: uniconf[%s] not ready yet\n", __func__, dbfile);
	}

	return ret;
}

void deinit_db()
{
	UB_LOG(UBL_INFO, "%s:\n", __func__);
	yang_db_item_access_t *ydbia = ydbi_access_handle();
	deregister_notice(ydbia->dbald, ydbia->ucntd);
	uc_notice_close(ydbia->ucntd, g_uniconf_access_mode);
	uc_dbal_close(ydbia->dbald, g_uniconf_access_mode);

	UB_LOG(UBL_INFO, "%s: done\n", __func__);
}

static void clear_cb(struct ub_list_node *node, void *arg)
{
	uint8_t clear_cmd = 0xFF;
	if (arg)
		clear_cmd = *((uint8_t*)arg);

	switch(clear_cmd)
	{
		case CLEAR_CMD_MGMT_ADDR_TX:
			UB_LOG(UBL_DEBUG, "%s: clear mgmt addr tx port items\n", __func__);
			deinit_mgmt_tx_port((management_address_tx_port_t*)node);
			break;
		case CLEAR_CMD_MGMT_ADDR:
			UB_LOG(UBL_DEBUG, "%s: clear mgmt addr items\n", __func__);
			deinit_mgmt_addr((management_address_t*)node);
			break;
		case CLEAR_CMD_RMT_UNKNOWN_TLV:
			UB_LOG(UBL_DEBUG, "%s: clear remote_unknown_tlv_t items\n", __func__);
			deinit_rm_unknown_tlv((remote_unknown_tlv_t*)node);
			break;
		case CLEAR_CMD_RMT_ORG_TLV:
			UB_LOG(UBL_DEBUG, "%s: clear remote_org_defined_info_t items\n", __func__);
			deinit_rm_org_def((remote_org_defined_info_t*)node);
			break;
		case CLEAR_CMD_PORT:
			UB_LOG(UBL_DEBUG, "%s: clear lldp_port_t items\n", __func__);
			deinit_cfg_port((lldp_port_t*)node);
			break;
		default:
			free(node);
	}
}

void cleanup_local_mgmt_addr_tx(lldp_port_t* p)
{
	uint8_t cmd = CLEAR_CMD_MGMT_ADDR_TX;
	ub_list_clear(&p->management_address_tx_port, clear_cb, &cmd);
}

/// @brief cleanup all list inside rs_data
/// @param rs_data 
void cleanup_remote_system_data(remote_systems_data_t* rs_data)
{
	uint8_t cmd = CLEAR_CMD_MGMT_ADDR;
	ub_list_clear(&rs_data->management_address, clear_cb, &cmd);

	cmd = CLEAR_CMD_RMT_UNKNOWN_TLV;
	ub_list_clear(&rs_data->remote_unknown_tlv, clear_cb, &cmd);

	cmd = CLEAR_CMD_RMT_ORG_TLV;
	ub_list_clear(&rs_data->remote_org_defined_info, clear_cb, &cmd);
}

void cleanup_cfg(yang_lldp_t* yang_lldp)
{
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&yang_lldp->ports, tmp_node))
	{
		lldp_port_t* p = (lldp_port_t*)tmp_node;
		cleanup_local_mgmt_addr_tx(p);

		struct ub_list_node *rs_data_node;
		for(UB_LIST_FOREACH_ITER(&p->remote_systems_data, rs_data_node))
		{
			cleanup_remote_system_data((remote_systems_data_t*)rs_data_node);
		}
	}

	uint8_t cmd = CLEAR_CMD_PORT;
	ub_list_clear(&yang_lldp->ports, clear_cb, &cmd);
}

/// @brief update data with only one lower level of IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA
/// @param new_rs_data 
/// @param k 
static void update_rs_data_1st_level(lldp_port_t* port, remote_systems_data_t* new_rs_data, ieee802_dot1ab_lldp_enum_t k)
{
	uint8_t kn_if_id[3] = {
		[0] = IEEE802_DOT1AB_LLDP_PORT,
		[1] = IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA,
		[2] = k,
	};

	attribute_pair_t attr_if_id[4] =
	{
		[0] = {port->name, strlen(port->name) +1},
		[1] = {port->dest_mac_address, 6},
		[2] = {(void*)&new_rs_data->time_mark, 4},
		[3] = {(void*)&new_rs_data->remote_index, 4},
	};

	// UB_LOG(UBL_DEBUG, "%s: %s \n", __func__, port->name);
	// ub_hexdump(true, true, port->dest_mac_address, 6, 0);

	void* new_val = NULL;
	size_t data_size = 0;

	switch (k)
	{
		case IEEE802_DOT1AB_LLDP_REMOTE_TOO_MANY_NEIGHBORS:
			new_val = (void*)&new_rs_data->remote_too_many_neighbors;
			data_size = sizeof(new_rs_data->remote_too_many_neighbors);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_CHANGES:
			new_val = (void*)&new_rs_data->remote_changes;
			data_size = sizeof(new_rs_data->remote_changes);
			break;
		case IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE:
			new_val = (void*)&new_rs_data->chassis_id_subtype;
			data_size = sizeof(new_rs_data->chassis_id_subtype);
			break;
		case IEEE802_DOT1AB_LLDP_CHASSIS_ID:
			new_val = (void*)&new_rs_data->chassis_id;
			data_size = strlen((char*) &new_rs_data->chassis_id[0]) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_PORT_ID:
			new_val = (void*)&new_rs_data->port_id;
			data_size = strlen((char*) &new_rs_data->port_id[0]) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE:
			new_val = (void*)&new_rs_data->port_id_subtype;
			data_size = sizeof(new_rs_data->port_id_subtype);
			break;
		case IEEE802_DOT1AB_LLDP_PORT_DESC:
			new_val = (void*)&new_rs_data->port_desc;
			data_size = strlen(new_rs_data->port_desc) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_NAME:
			new_val = (void*)&new_rs_data->system_name;
			data_size = strlen(new_rs_data->system_name) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION:
			new_val = (void*)&new_rs_data->system_description;
			data_size = strlen(new_rs_data->system_description) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED:
			new_val = (void*)&new_rs_data->system_capabilities_supported;
			data_size = sizeof(new_rs_data->system_capabilities_supported);
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED:
			new_val = (void*)&new_rs_data->system_capabilities_enabled;
			data_size = sizeof(new_rs_data->system_capabilities_enabled);
			break;

		default:
			break;
	}

	// UB_LOG(UBL_DEBUG, "%s:%s updating k %s\n", __func__, port->name, ieee802_dot1ab_lldp_get_string(k));

	if (new_val != NULL && data_size > 0)
	{

		YDBI_SET_ITEM(abknvkn, 
			kn_if_id, 
			sizeof(kn_if_id)/sizeof(uint8_t), 
			attr_if_id,
			sizeof(attr_if_id)/sizeof(attribute_pair_t), 
			YDBI_STATUS,
			new_val,
			data_size,
			YDBI_NO_NOTICE);
	}

	uc_dbal_releasedb(ydbi_access_handle()->dbald);
}

static void fill_mgmt_addr_attr(management_address_t* mgmt_addr, attribute_pair_t* attr_pairs)
{
	attr_pairs[4].vk = (void*)&mgmt_addr->address_subtype;
	attr_pairs[4].vk_sz = sizeof(mgmt_addr->address_subtype);

	switch(mgmt_addr->address_subtype)
	{
		case ipv4:
			attr_pairs[5].vk = (void*)mgmt_addr->address;
			attr_pairs[5].vk_sz = 4;
			// ub_hexdump(true, true, mgmt_addr->address, strlen((char*)mgmt_addr->address), 0);
		break;
		case ipv6:
			attr_pairs[5].vk = (void*)mgmt_addr->address;
			attr_pairs[5].vk_sz = 16;
			// ub_hexdump(true, true, mgmt_addr->address, 16, 0);
		break;
		case nsap:
			attr_pairs[5].vk = (void*)mgmt_addr->address;
			attr_pairs[5].vk_sz = strlen((char*)mgmt_addr->address) + 1;
		break;
		default:
		break;
	}
}

static void fill_rm_org_info_attr(remote_org_defined_info_t* remote_org, attribute_pair_t* attr_pairs)
{
	attr_pairs[4].vk = (void*)&remote_org->info_identifier;
	attr_pairs[4].vk_sz = sizeof(remote_org->info_identifier);
	attr_pairs[5].vk = (void*)&remote_org->info_subtype;
	attr_pairs[5].vk_sz = sizeof(remote_org->info_subtype);
	attr_pairs[6].vk = (void*)&remote_org->info_index;
	attr_pairs[6].vk_sz = sizeof(remote_org->info_index);
}

static void fill_rm_unknown_tlv_attr(remote_unknown_tlv_t* unknown_tlv, attribute_pair_t* attr_pairs)
{
	attr_pairs[4].vk = (void*)&unknown_tlv->tlv_type;
	attr_pairs[4].vk_sz = sizeof(unknown_tlv->tlv_type);
}

static void update_rs_data_2nd_level(lldp_port_t* port, remote_systems_data_t* new_rs_data, ieee802_dot1ab_lldp_enum_t k1, ieee802_dot1ab_lldp_enum_t k2)
{
	uint8_t kn_if_id[4] = {
		[0] = IEEE802_DOT1AB_LLDP_PORT,
		[1] = IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA,
		[2] = k1,
		[3] = k2,
	};

	attribute_pair_t attr_if_id[7] = /*max is 7*/
	{
		[0] = {port->name, strlen(port->name) +1},
		[1] = {port->dest_mac_address, 6},
		[2] = {(void*)&new_rs_data->time_mark, 4},
		[3] = {(void*)&new_rs_data->remote_index, 4},
		[4] = {NULL, 0},
		[5] = {NULL, 0},
		[6] = {NULL, 0},
	};

	void* new_val = NULL;
	size_t data_size = 0;

	// UB_LOG(UBL_DEBUG, "%s:%s updating k1 %s k2 %s\n", __func__, port->name, ieee802_dot1ab_lldp_get_string(k1), ieee802_dot1ab_lldp_get_string(k2));

	switch(k1)
	{
		case IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS:
		{
			struct ub_list_node* tmp_mgmt_addr_node;
			for(UB_LIST_FOREACH_ITER(&new_rs_data->management_address, tmp_mgmt_addr_node))
			{
				management_address_t* mgmt_addr = (management_address_t*)tmp_mgmt_addr_node;
				fill_mgmt_addr_attr(mgmt_addr, &attr_if_id[0]);

				if (k2 == IEEE802_DOT1AB_LLDP_IF_SUBTYPE || k2 == IEEE802_DOT1AB_LLDP_IF_ID)
				{
					new_val = (k2 == IEEE802_DOT1AB_LLDP_IF_SUBTYPE) ? (void*)&mgmt_addr->if_subtype : (void*)&mgmt_addr->if_id;
					data_size = (k2 == IEEE802_DOT1AB_LLDP_IF_SUBTYPE) ? sizeof(mgmt_addr->if_subtype) : sizeof(mgmt_addr->if_id);

					YDBI_SET_ITEM(abknvkn, 
						kn_if_id, 
						sizeof(kn_if_id)/sizeof(uint8_t),
						attr_if_id,
						6,
						YDBI_STATUS,
						new_val,
						data_size,
						YDBI_NO_NOTICE);
				}
			}
		}
		break;
		case IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV:
		{
			struct ub_list_node* tmp_unknown_tlv_node;
			for(UB_LIST_FOREACH_ITER(&new_rs_data->remote_unknown_tlv, tmp_unknown_tlv_node))
			{
				remote_unknown_tlv_t* unknown_tlv = (remote_unknown_tlv_t*)tmp_unknown_tlv_node;
				fill_rm_unknown_tlv_attr(unknown_tlv, &attr_if_id[0]);

				if (k2 == IEEE802_DOT1AB_LLDP_TLV_INFO)
				{
					new_val = (void*)&unknown_tlv->tlv_info;
					data_size = sizeof(unknown_tlv->tlv_info);

					YDBI_SET_ITEM( abknvkn, 
									kn_if_id, 
									sizeof(kn_if_id)/sizeof(uint8_t),
									attr_if_id,
									5,
									YDBI_STATUS,
									new_val,
									data_size,
									YDBI_NO_NOTICE);
				}
			}
		}
		break;
		case IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO:
		{
			struct ub_list_node* tmp_remote_org_node;
			for(UB_LIST_FOREACH_ITER(&new_rs_data->remote_org_defined_info, tmp_remote_org_node))
			{
				remote_org_defined_info_t* remote_org = (remote_org_defined_info_t*)tmp_remote_org_node;
				fill_rm_org_info_attr(remote_org, &attr_if_id[0]);

				if(k2 == IEEE802_DOT1AB_LLDP_REMOTE_INFO)
				{
					new_val = (void*)&remote_org->remote_info;
					data_size = sizeof(remote_org->remote_info);

					YDBI_SET_ITEM(abknvkn, 
						kn_if_id, 
						sizeof(kn_if_id)/sizeof(uint8_t),
						attr_if_id,
						7,
						YDBI_STATUS,
						new_val,
						data_size,
						YDBI_NO_NOTICE);

				}
			}
		}
		break;

		default:
			break;
	}

	uc_dbal_releasedb(ydbi_access_handle()->dbald);
}
/// @brief write new rs data to db
/// @param new_rs_data 
/// @return int remote index
static int write_remote_system_data_to_db(lldp_port_t* port, remote_systems_data_t* new_rs_data)
{
	// 1st level
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_REMOTE_TOO_MANY_NEIGHBORS);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_REMOTE_CHANGES);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_CHASSIS_ID);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_PORT_ID);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_PORT_DESC);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_SYSTEM_NAME);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED);
	update_rs_data_1st_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED);
	// mgmt addr
	update_rs_data_2nd_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_SUBTYPE);
	update_rs_data_2nd_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_ID);
	// tlv unknown
	update_rs_data_2nd_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV, IEEE802_DOT1AB_LLDP_TLV_INFO);
	// organization id
	update_rs_data_2nd_level(port, new_rs_data, IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO, IEEE802_DOT1AB_LLDP_REMOTE_INFO);

	// UB_LOG(UBL_DEBUG, "%s:%s k(%u, %d)\n", __func__, port->name, new_rs_data->time_mark, new_rs_data->remote_index);

	return 0;
}

/// @brief Below function should be moved to lldp_cfg
/// @param port 
/// @param in 
remote_systems_data_t *create_new_remote_system_data(lldp_port_t* port, remote_systems_data_t* in)
{
	uint32_t rs_index = ub_list_count(&port->remote_systems_data); /// Index will base on count. Start from 0
	timeticks creation_time = get_current_time_in_milisec();

	// Create new remote system data
	remote_systems_data_t *remote_system_data = init_rm_system_data();
	memset(remote_system_data, 0, sizeof(remote_systems_data_t));
	remote_system_data->remote_index = rs_index;
	remote_system_data->time_mark = creation_time;
	
	memcpy(remote_system_data->chassis_id, in->chassis_id, sizeof(in->chassis_id));
	memcpy(remote_system_data->port_id, in->port_id, sizeof(in->port_id));

	remote_system_data->chassis_id_subtype = in->chassis_id_subtype;
	remote_system_data->port_id_subtype = in->port_id_subtype;
	memcpy(remote_system_data->port_desc, in->port_desc, strlen(in->port_desc));
	memcpy(remote_system_data->system_name, in->system_name, strlen(in->system_name));
	memcpy(remote_system_data->system_description, in->system_description, strlen(in->system_description));
	
	// copy mgm addr
	ub_list_init(&remote_system_data->management_address);
	struct ub_list_node* tmp_mgmt_addr_node;
	for(UB_LIST_FOREACH_ITER(&in->management_address, tmp_mgmt_addr_node))
	{
		management_address_t* mgmt_addr = (management_address_t*)tmp_mgmt_addr_node; // <-- this pointer will be deleted outside of this function
		management_address_t* mgmt_addr_copy = init_mgmt_addr();
		memset(mgmt_addr_copy, 0, sizeof(management_address_t));
		mgmt_addr_copy->address_subtype = mgmt_addr->address_subtype;
		mgmt_addr_copy->if_id = mgmt_addr->if_id;
		mgmt_addr_copy->if_subtype = mgmt_addr->if_subtype;
		memcpy(mgmt_addr_copy->address, mgmt_addr->address, sizeof(mgmt_addr_copy->address));

		ub_list_append(&remote_system_data->management_address, (struct ub_list_node*)mgmt_addr_copy);
	}

	remote_system_data->system_capabilities_enabled = in->system_capabilities_enabled;
	remote_system_data->system_capabilities_supported = in->system_capabilities_supported;
	
	// init unknown tlv only
	ub_list_init(&remote_system_data->remote_unknown_tlv);
	struct ub_list_node* tmp_rm_unknown_tlv;
	for(UB_LIST_FOREACH_ITER(&in->remote_unknown_tlv, tmp_rm_unknown_tlv))
	{
		remote_unknown_tlv_t* in_rm_unknown_tlv = (remote_unknown_tlv_t*) tmp_rm_unknown_tlv;
		remote_unknown_tlv_t* rm_unknown_tlv = init_rm_unknown_tlv();
		memset(rm_unknown_tlv, 0, sizeof(remote_unknown_tlv_t));
		rm_unknown_tlv->tlv_type = in_rm_unknown_tlv->tlv_type;
		memcpy(rm_unknown_tlv->tlv_info, in_rm_unknown_tlv->tlv_info, sizeof(in_rm_unknown_tlv->tlv_info));

		ub_list_append(&remote_system_data->remote_unknown_tlv, (struct ub_list_node*)rm_unknown_tlv);
	}
	ub_list_init(&remote_system_data->remote_org_defined_info);
	struct ub_list_node* tmp_rm_org_defined;
	uint32_t rm_org_counter = 0;
	for(UB_LIST_FOREACH_ITER(&in->remote_org_defined_info, tmp_rm_org_defined))
	{
		remote_org_defined_info_t* in_rm_org = (remote_org_defined_info_t*) tmp_rm_org_defined;
		remote_org_defined_info_t* rm_org = init_rm_org_def();
		memset(rm_org, 0, sizeof(remote_org_defined_info_t));
		rm_org->info_identifier = in_rm_org->info_identifier;
		rm_org->info_index = rm_org_counter;
		rm_org->info_subtype = in_rm_org->info_subtype;
		memcpy(rm_org->remote_info, in_rm_org->remote_info, sizeof(in_rm_org->remote_info));

		ub_list_append(&remote_system_data->remote_org_defined_info, (struct ub_list_node*)rm_org);
		rm_org_counter ++;
	}

	ub_list_append(&port->remote_systems_data, (struct ub_list_node*)remote_system_data);
	// UB_LOG(UBL_DEBUG, "%s:%s k(%u, %d) rs counter %d\n", __func__, port->name, remote_system_data->time_mark, remote_system_data->remote_index, ub_list_count(&port->remote_systems_data));
	
	write_remote_system_data_to_db(port, remote_system_data);
	
	UB_TLOG(UBL_DEBUG, "%s:%s created k(%u, %d) \n", __func__, port->name, remote_system_data->time_mark, remote_system_data->remote_index);

	return remote_system_data;
}

/// @brief 
/// @param port 
/// @param is_tx tx or else is rx
/// @param k updated val
static void update_port_statistic(lldp_port_t* port, uint8_t tx_or_rx_key, ieee802_dot1ab_lldp_enum_t k)
{
	uint8_t kn_if_id[3] = {
		[0] = IEEE802_DOT1AB_LLDP_PORT,
		[1] = tx_or_rx_key,
		[2] = k,
	};

	attribute_pair_t attr_if_id[2] =
	{
		[0] = {port->name, strlen(port->name) +1},
		[1] = {port->dest_mac_address, 6}
	};

	// UB_LOG(UBL_DEBUG, "%s: %s \n", __func__, port->name);
	// ub_hexdump(true, true, port->dest_mac_address, 6, 0);

	void* new_val = NULL;
	size_t data_size = 0;

	switch (k)
	{
		case IEEE802_DOT1AB_LLDP_TOTAL_FRAMES:
			if (tx_or_rx_key == IEEE802_DOT1AB_LLDP_RX_STATISTICS)
			{
				new_val = (void*)&port->rx_statistic.total_frames;
				data_size = sizeof(port->rx_statistic.total_frames);
			} 
			else
			{
				new_val = (void*)&port->tx_statistic.total_frames;
				data_size = sizeof(port->tx_statistic.total_frames);
			}
			break;
		case IEEE802_DOT1AB_LLDP_TOTAL_LENGTH_ERRORS:
			new_val = (void*)&port->tx_statistic.total_length_errors;
			data_size = sizeof(port->tx_statistic.total_length_errors);
			break;
		case IEEE802_DOT1AB_LLDP_TOTAL_AGEOUTS:
			new_val = (void*)&port->rx_statistic.total_ageouts;
			data_size = sizeof(port->rx_statistic.total_ageouts);
			break;
		case IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_FRAMES:
			new_val = (void*)&port->rx_statistic.total_discarded_frames;
			data_size = sizeof(port->rx_statistic.total_discarded_frames);
			break;
		case IEEE802_DOT1AB_LLDP_ERROR_FRAMES:
			new_val = (void*)&port->rx_statistic.error_frames;
			data_size = sizeof(port->rx_statistic.error_frames);
			break;
		case IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_TLVS:
			new_val = (void*)&port->rx_statistic.total_discarded_tlvs;
			data_size = sizeof(port->rx_statistic.total_discarded_tlvs);
			break;
		case IEEE802_DOT1AB_LLDP_TOTAL_UNRECOGNIZED_TLVS:
			new_val = (void*)&port->rx_statistic.total_unrecognized_tlvs;
			data_size = sizeof(port->rx_statistic.total_unrecognized_tlvs);
			break;
		default:
			break;
	}

	// UB_LOG(UBL_DEBUG, "%s:%s updating k %s/%s to %d\n", __func__, port->name, ieee802_dot1ab_lldp_get_string(tx_or_rx_key), ieee802_dot1ab_lldp_get_string(k), *((int*)new_val));

	if (new_val != NULL && data_size > 0)
	{

		YDBI_SET_ITEM(abknvkn, 
			kn_if_id, 
			sizeof(kn_if_id)/sizeof(uint8_t), 
			attr_if_id,
			sizeof(attr_if_id)/sizeof(attribute_pair_t), 
			YDBI_STATUS,
			new_val,
			data_size,
			YDBI_NO_NOTICE);
	}
}
void increase_tx_frame(lldp_port_t* port)
{
	port->tx_statistic.total_frames++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_TX_STATISTICS, IEEE802_DOT1AB_LLDP_TOTAL_FRAMES);
}
void increase_tx_len_error(lldp_port_t* port)
{
	port->tx_statistic.total_length_errors++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_TX_STATISTICS, IEEE802_DOT1AB_LLDP_TOTAL_LENGTH_ERRORS);
}
void increase_rx_total_aged_out(lldp_port_t* port)
{
	port->rx_statistic.total_ageouts++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_RX_STATISTICS, IEEE802_DOT1AB_LLDP_TOTAL_AGEOUTS);
}
void increase_rx_total_discarded_frame(lldp_port_t* port)
{
	port->rx_statistic.total_discarded_frames++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_RX_STATISTICS, IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_FRAMES);
}
void increase_rx_error_frame(lldp_port_t* port)
{
	port->rx_statistic.error_frames++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_RX_STATISTICS, IEEE802_DOT1AB_LLDP_ERROR_FRAMES);
}
void increase_rx_total_frame(lldp_port_t* port)
{
	port->rx_statistic.total_frames++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_RX_STATISTICS, IEEE802_DOT1AB_LLDP_TOTAL_FRAMES);
	// UB_LOG(UBL_DEBUG, "%s:%s rx %d\n",__func__, port->name, port->rx_statistic.total_frames);
}
void increase_rx_total_discarded_tlvs(lldp_port_t* port)
{
	port->rx_statistic.total_discarded_tlvs++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_RX_STATISTICS, IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_TLVS);
}
void increase_rx_total_unreg_tlvs(lldp_port_t* port)
{
	port->rx_statistic.total_unrecognized_tlvs++;
	update_port_statistic(port, IEEE802_DOT1AB_LLDP_RX_STATISTICS, IEEE802_DOT1AB_LLDP_TOTAL_UNRECOGNIZED_TLVS);
}

/// @brief 
/// @param port 
/// @param is_tx tx or else is rx
/// @param k updated val
static void update_remote_statistic(yang_lldp_t* lldpcfg, ieee802_dot1ab_lldp_enum_t k)
{
	uint8_t kn_if_id[2] = {
		[0] = IEEE802_DOT1AB_LLDP_REMOTE_STATISTICS,
		[1] = k
	};

	attribute_pair_t attr_if_id;

	void* new_val = NULL;
	size_t data_size = 0;

	switch (k)
	{
		case IEEE802_DOT1AB_LLDP_LAST_CHANGE_TIME:
			new_val = (void*)&lldpcfg->remote_statistic.last_change_time;
			data_size = sizeof(lldpcfg->remote_statistic.last_change_time);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_INSERTS:
			new_val = (void*)&lldpcfg->remote_statistic.remote_inserts;
			data_size = sizeof(lldpcfg->remote_statistic.remote_inserts);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_DELETES:
			new_val = (void*)&lldpcfg->remote_statistic.remote_deletes;
			data_size = sizeof(lldpcfg->remote_statistic.remote_deletes);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_DROPS:
			new_val = (void*)&lldpcfg->remote_statistic.remote_drops;
			data_size = sizeof(lldpcfg->remote_statistic.remote_drops);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_AGEOUTS:
			new_val = (void*)&lldpcfg->remote_statistic.remote_ageouts;
			data_size = sizeof(lldpcfg->remote_statistic.remote_ageouts);
			break;
		default:
			break;
	}

	// UB_LOG(UBL_DEBUG, "%s: updating k %s\n", __func__, ieee802_dot1ab_lldp_get_string(k));

	if (new_val != NULL && data_size > 0)
	{

		YDBI_SET_ITEM(abknvkn, 
			kn_if_id, 
			sizeof(kn_if_id)/sizeof(uint8_t), 
			&attr_if_id,
			0,  // No attribute
			YDBI_STATUS,
			new_val,
			data_size,
			YDBI_NO_NOTICE);
	}
}
void increase_remote_stat_rm_inserts(yang_lldp_t* lldpcfg)
{
	lldpcfg->remote_statistic.remote_inserts++;
	update_remote_statistic(lldpcfg, IEEE802_DOT1AB_LLDP_REMOTE_INSERTS);
}

void increase_remote_stat_rm_deletes(yang_lldp_t* lldpcfg)
{
	lldpcfg->remote_statistic.remote_deletes++;
	update_remote_statistic(lldpcfg, IEEE802_DOT1AB_LLDP_REMOTE_DELETES);
}
void increase_remote_stat_rm_drops(yang_lldp_t* lldpcfg)
{
	lldpcfg->remote_statistic.remote_drops++;
	update_remote_statistic(lldpcfg, IEEE802_DOT1AB_LLDP_REMOTE_DROPS);
}
void increase_remote_stat_rm_agedouts(yang_lldp_t* lldpcfg)
{
	lldpcfg->remote_statistic.remote_ageouts++;
	update_remote_statistic(lldpcfg, IEEE802_DOT1AB_LLDP_REMOTE_AGEOUTS);
}

void update_remote_stat_last_changed_time(yang_lldp_t* lldpcfg)
{
	uint32_t now = get_current_time_in_milisec();
	lldpcfg->remote_statistic.last_change_time = now;
	update_remote_statistic(lldpcfg, IEEE802_DOT1AB_LLDP_LAST_CHANGE_TIME);
}
/// @brief delete data with only one lower level of IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA
/// @param new_rs_data 
/// @param k 
static void delete_rs_data_1st_level(lldp_port_t* port, remote_systems_data_t* existed_rs_data, ieee802_dot1ab_lldp_enum_t k)
{
	uint8_t kn_if_id[3] = {
		[0] = IEEE802_DOT1AB_LLDP_PORT,
		[1] = IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA,
		[2] = k,
	};

	attribute_pair_t attr_if_id[4] =
	{
		[0] = {port->name, strlen(port->name) +1},
		[1] = {port->dest_mac_address, 6},
		[2] = {(void*)&existed_rs_data->time_mark, 4},
		[3] = {(void*)&existed_rs_data->remote_index, 4},
	};

	// UB_LOG(UBL_DEBUG, "%s:%s deleting k %s\n", __func__, port->name, ieee802_dot1ab_lldp_get_string(k));
	YDBI_DEL_ITEM(abknvkn, 
				kn_if_id, 
				sizeof(kn_if_id) / sizeof(uint8_t), 
				attr_if_id,
				sizeof(attr_if_id)/sizeof(attribute_pair_t), 
				YDBI_STATUS,
				YDBI_NO_NOTICE);
	
	uc_dbal_releasedb(ydbi_access_handle()->dbald);
}

/// @brief delete data with two lower levels of IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA
/// @param port 
/// @param new_rs_data 
/// @param k1 
/// @param k2 
static void delete_rs_data_2nd_level(lldp_port_t* port, remote_systems_data_t* existed_rs_data, ieee802_dot1ab_lldp_enum_t k1, ieee802_dot1ab_lldp_enum_t k2)
{
	uint8_t kn_if_id[4] = {
		[0] = IEEE802_DOT1AB_LLDP_PORT,
		[1] = IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA,
		[2] = k1,
		[3] = k2,
	};

	attribute_pair_t attr_if_id[7] = /*max is 7*/
	{
		[0] = {port->name, strlen(port->name) +1},
		[1] = {port->dest_mac_address, 6},
		[2] = {(void*)&existed_rs_data->time_mark, 4},
		[3] = {(void*)&existed_rs_data->remote_index, 4},
		[4] = {NULL, 0},
		[5] = {NULL, 0},
		[6] = {NULL, 0},
	};

	// UB_LOG(UBL_DEBUG, "%s:deleting port(%s) rs(%u, %d) k1 %s k2 %s\n", __func__, port->name, existed_rs_data->time_mark, existed_rs_data->remote_index, ieee802_dot1ab_lldp_get_string(k1), ieee802_dot1ab_lldp_get_string(k2));

	switch(k1)
	{
		case IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS:
		{
			struct ub_list_node* tmp_mgmt_addr_node;
			for(UB_LIST_FOREACH_ITER(&existed_rs_data->management_address, tmp_mgmt_addr_node))
			{
				management_address_t* mgmt_addr = (management_address_t*)tmp_mgmt_addr_node;
				fill_mgmt_addr_attr(mgmt_addr, &attr_if_id[0]);

				YDBI_DEL_ITEM(abknvkn, 
					kn_if_id, 
					sizeof(kn_if_id) / sizeof(uint8_t), 
					attr_if_id,
					6, 
					YDBI_STATUS,
					YDBI_NO_NOTICE);
			}
			// There are two items in LLDP_MANAGEMENT_ADDRESS IEEE802_DOT1AB_LLDP_IF_SUBTYPE IEEE802_DOT1AB_LLDP_IF_ID
			// So we need to do delete all of thing, and then clear the ub_list at the caller of this function
		}
		break;
		case IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV:
		{
			struct ub_list_node* tmp_unknown_tlv_node;
			for(UB_LIST_FOREACH_ITER(&existed_rs_data->remote_unknown_tlv, tmp_unknown_tlv_node))
			{
				remote_unknown_tlv_t* unknown_tlv = (remote_unknown_tlv_t*)tmp_unknown_tlv_node;
				fill_rm_unknown_tlv_attr(unknown_tlv, &attr_if_id[0]);

				YDBI_DEL_ITEM(abknvkn, 
					kn_if_id, 
					sizeof(kn_if_id) / sizeof(uint8_t), 
					attr_if_id,
					5, 
					YDBI_STATUS,
					YDBI_NO_NOTICE);
			}

			// The remote unknown tlv only have IEEE802_DOT1AB_LLDP_TLV_INFO as k2, so we can cleanup the list here
			uint8_t cmd = CLEAR_CMD_RMT_UNKNOWN_TLV;
			ub_list_clear(&existed_rs_data->remote_unknown_tlv, clear_cb, &cmd);
		}
		break;
		case IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO:
		{
			struct ub_list_node* tmp_remote_org_node;
			for(UB_LIST_FOREACH_ITER(&existed_rs_data->remote_org_defined_info, tmp_remote_org_node))
			{
				remote_org_defined_info_t* remote_org = (remote_org_defined_info_t*)tmp_remote_org_node;
				fill_rm_org_info_attr(remote_org, &attr_if_id[0]);

				YDBI_DEL_ITEM(abknvkn, 
					kn_if_id, 
					sizeof(kn_if_id) / sizeof(uint8_t), 
					attr_if_id,
					7, 
					YDBI_STATUS,
					YDBI_NO_NOTICE);
			}
			// The remote org tlv only have IEEE802_DOT1AB_LLDP_REMOTE_INFO as k2, so we can cleanup the list here
			uint8_t cmd = CLEAR_CMD_RMT_ORG_TLV;
			ub_list_clear(&existed_rs_data->remote_org_defined_info, clear_cb, &cmd);
		}
		break;
		default: break;
	}

	uc_dbal_releasedb(ydbi_access_handle()->dbald);
}

/// @brief Delete existed remote system data 
/// @param port parent port which store a remote system data list
/// @param rs_data_tobe_deleted to-be-deleted item
void delete_remote_system_data(lldp_port_t* port, remote_systems_data_t* rs_data_tobe_deleted)
{
	// 1st level
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_REMOTE_TOO_MANY_NEIGHBORS);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_REMOTE_CHANGES);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_CHASSIS_ID);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_PORT_ID);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_PORT_DESC);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_SYSTEM_NAME);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED);
	delete_rs_data_1st_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED);
	// mgmt addr
	if (ub_list_count(&rs_data_tobe_deleted->management_address) > 0)
	{
		delete_rs_data_2nd_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_SUBTYPE);
		delete_rs_data_2nd_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_ID);

		uint8_t cmd = CLEAR_CMD_MGMT_ADDR;
		ub_list_clear(&rs_data_tobe_deleted->management_address, clear_cb, &cmd);
	}
	// tlv unknown
	if (ub_list_count(&rs_data_tobe_deleted->remote_unknown_tlv) > 0)
	{
		delete_rs_data_2nd_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV, IEEE802_DOT1AB_LLDP_TLV_INFO);
	}
	// organization id
	if (ub_list_count(&rs_data_tobe_deleted->remote_org_defined_info) > 0)
	{
		delete_rs_data_2nd_level(port, rs_data_tobe_deleted, IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO, IEEE802_DOT1AB_LLDP_REMOTE_INFO);
	}

	UB_TLOG(UBL_DEBUG, "%s:%s Deleted %s k(%u, %d)\n", __func__, port->name, rs_data_tobe_deleted->port_desc, rs_data_tobe_deleted->remote_index, rs_data_tobe_deleted->time_mark);
}

static bool is_same_chassis(remote_systems_data_t* cmp1, remote_systems_data_t* cmp2)
{
	bool ret = false;
	ret = (cmp1->chassis_id_subtype == cmp2->chassis_id_subtype); // If the subtype is different, we don't need to compare the content
	if (ret) 
	{
		switch(cmp1->chassis_id_subtype)
		{
			case C_CHASSIS_COMPONENT:   // EntPhysicalAlias: String type IETF RFC 6933
			case C_INTERFACE_ALIAS:     // ifAlias DisplayString IETF RFC 2863
			case C_PORT_COMPONENT:      // EntPhysicalAlias: String type IETF RFC 6933
			case C_INTERFACE_NAME:      // ifName DisplayString IETF RFC 2863
			case C_LOCAL:               // local is an alpha-numeric string and is locally assigned
			case C_NETWORK_ADDRESS:     // networkAddress is an octet string
				ret = (strcmp((char*)cmp1->chassis_id, (char*)cmp2->chassis_id) == 0);
				// UB_LOG(UBL_DEBUG, "%s %d: ret %d\n", __func__, cmp1->chassis_id_subtype, ret);
				// ub_hexdump(true, true, cmp1->chassis_id, strlen(cmp1->chassis_id), 0);
				// ub_hexdump(true, true, cmp2->chassis_id, strlen(cmp2->chassis_id), 0);
				break;
			case C_MAC_ADDRESS: // MAC: 6 bytes
				ret = (memcmp(cmp1->chassis_id, cmp2->chassis_id, 6) == 0);
				// UB_LOG(UBL_DEBUG, "%s C_MAC_ADDRESS: ret %d\n", __func__, ret);
				// ub_hexdump(true, true, cmp1->chassis_id, 6, 0);
				// ub_hexdump(true, true, cmp2->chassis_id, 6, 0);
				break;
			default:
				UB_LOG(UBL_DEBUG, "%s Got reserved type %d-%d\n", __func__, cmp1->chassis_id_subtype, cmp2->chassis_id_subtype);
				ret = true; // okay if same subtype but got reserved => ret = true always
				break;
		}
	}
	else
	{
		UB_LOG(UBL_INFO, "%s diff chassis id subtype %d - %d\n", __func__, cmp1->chassis_id_subtype, cmp2->chassis_id_subtype);
	}

	return ret;
}

static bool is_same_port(remote_systems_data_t* cmp1, remote_systems_data_t* cmp2)
{
	bool ret = false;
	ret = (cmp1->port_id_subtype == cmp2->port_id_subtype); // If the subtype is different, we don't need to compare the content
	if (ret) 
	{
		switch(cmp1->port_id_subtype)
		{
			case P_INTERFACE_ALIAS:     // ifAlias DisplayString IETF RFC 2863
			case P_PORT_COMPONENT:      // EntPhysicalAlias: String type IETF RFC 6933
			case P_INTERFACE_NAME:      // ifName DisplayString IETF RFC 2863
			case P_LOCAL:               // local is an alpha-numeric string and is locally assigned
			case P_NETWORK_ADDRESS:     // networkAddress is an octet string
				ret = (strcmp((char*)cmp1->port_id, (char*)cmp2->port_id) == 0);
				// UB_LOG(UBL_DEBUG, "%s %d: ret %d\n", __func__, cmp1->port_id_subtype, ret);
				// ub_hexdump(true, true, cmp1->port_id, strlen(cmp1->port_id), 0);
				// ub_hexdump(true, true, cmp2->port_id, strlen(cmp2->port_id), 0);
				break;
			case P_MAC_ADDRESS: // MAC: 6 bytes
				ret = (memcmp(cmp1->port_id, cmp2->port_id, 6) == 0);
				// UB_LOG(UBL_DEBUG, "%s P_MAC_ADDRESS: ret %d\n", __func__, ret);
				// ub_hexdump(true, true, cmp1->port_id, 6, 0);
				// ub_hexdump(true, true, cmp2->port_id, 6, 0);
				break;
			default:
				UB_LOG(UBL_DEBUG, "%s Got reserved type %d-%d\n", __func__, cmp1->port_id_subtype, cmp2->port_id_subtype);
				ret = true; // okay if same subtype but got reserved => ret = true always
				break;
		}
	}

	return ret;
}

/// @brief Below function should be moved to lldp_cfg
/// @param port 
/// @param in 
/// @return 
remote_systems_data_t* find_existed_neighbor(lldp_port_t* port, remote_systems_data_t* in)
{
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&port->remote_systems_data, tmp_node))
	{
		remote_systems_data_t* remote_system_data = (remote_systems_data_t*)tmp_node;
		if (is_same_chassis(in, remote_system_data) &&  
			is_same_port(in, remote_system_data) )
			{
				// UB_LOG(UBL_DEBUG, "%s: found existed neighbor\n", __func__);
				return remote_system_data;
			}
	}
	// UB_LOG(UBL_DEBUG, "%s: not found existed neighbor\n", __func__);
	return NULL;
}

static void dump_mgmt_addr_list(char* tag, management_address_list_t* mgmt_addr)
{
	struct ub_list_node *tmp_compare_node;
	for(UB_LIST_FOREACH_ITER(mgmt_addr, tmp_compare_node))
	{
		management_address_t* compare_mgmt_addr = (management_address_t*)tmp_compare_node;
		UB_LOG(UBL_INFO, "%s: address_subtype %d - if_id %d if_subtype %d \t", tag, compare_mgmt_addr->address_subtype, compare_mgmt_addr->if_id, compare_mgmt_addr->if_subtype);
		ub_hexdump(true, true, compare_mgmt_addr->address, sizeof(compare_mgmt_addr->address), 0);
	}
}
/// @brief Compare b/w two remote mgmt addr list. The order may be changed but the content can be same.
/// @param mgmt_addr1 
/// @param mgmt_addr2 
/// @return 
static bool is_same_mgmt_addr_list(management_address_list_t* mgmt_addr1, management_address_list_t* mgmt_addr2)
{
	bool ret = true;
	if (ub_list_count(mgmt_addr1) == ub_list_count(mgmt_addr2))
	{
		struct ub_list_node *tmp_node;
		for(UB_LIST_FOREACH_ITER(mgmt_addr1, tmp_node))
		{
			bool is_found_mgmt_addr = false;
			management_address_t* mgmt_addr = (management_address_t*)tmp_node;
			
			struct ub_list_node *tmp_compare_node;
			for(UB_LIST_FOREACH_ITER(mgmt_addr2, tmp_compare_node))
			{
				management_address_t* compare_mgmt_addr = (management_address_t*)tmp_compare_node;
				if (mgmt_addr->address_subtype == compare_mgmt_addr->address_subtype &&
					mgmt_addr->if_id == compare_mgmt_addr->if_id &&
					mgmt_addr->if_subtype == compare_mgmt_addr->if_subtype && 
					memcmp(mgmt_addr->address, compare_mgmt_addr->address, sizeof(compare_mgmt_addr->address)) == 0)
				{
						is_found_mgmt_addr = true;
						break;
				}
			}

			if (!is_found_mgmt_addr)
			{
				UB_LOG(UBL_INFO, "%s: mgmt addr is not same\n", __func__);
				dump_mgmt_addr_list("mgmt_addr1: ", mgmt_addr1);
				dump_mgmt_addr_list("mgmt_addr2: ", mgmt_addr2);
				ret = false;
				break; // no more needed to search again
			}
		}
	}
	else
	{
		ret = false;
	}

	return ret;
}

/// @brief Below function should be moved to lldp_cfg
/// @param in 
/// @param existed 
/// @return true: remote changed. false: not changed
bool is_remote_system_data_change(remote_systems_data_t* in, remote_systems_data_t* existed)
{
	bool ret = false;

	if (strcmp(in->port_desc, existed->port_desc) != 0)
	{
		UB_LOG(UBL_INFO, "%s: Detected port desc changed \n", __func__);
		ret = true;
	}

	if (!ret && strcmp(in->system_name, existed->system_name) != 0)
	{
		UB_LOG(UBL_INFO, "%s: Detected system name changed \n", __func__);
		ret = true;
	}

	if (!ret && strcmp(in->system_description, existed->system_description) != 0)
	{
		UB_LOG(UBL_INFO, "%s: Detected system description changed \n", __func__);
		ret = true;
	}

	if (!ret && in->system_capabilities_enabled != existed->system_capabilities_enabled)
	{
		UB_LOG(UBL_INFO, "%s: Detected system_capabilities_enabled changed\n", __func__);
		ret = true;
	}

	if (!ret && in->system_capabilities_supported != existed->system_capabilities_supported)
	{
		UB_LOG(UBL_INFO, "%s: Detected system_capabilities_supported changed\n", __func__);
		ret = true;
	}

	if (!is_same_mgmt_addr_list(&in->management_address, &existed->management_address))
	{
		UB_LOG(UBL_INFO, "%s: Detected management_address changed \n", __func__);
		ret = true;
	}

	UB_LOG(UBL_DEBUG, "%s: %d\n", __func__, ret);
	return ret;
}

void update_remote_system_data(lldp_port_t* port, remote_systems_data_t* in, remote_systems_data_t* existed)
{
	if (strcmp(in->port_desc, existed->port_desc) != 0)
	{
		UB_LOG(UBL_INFO, "%s: changed port_desc [%s-> %s]\n", __func__, (char*)existed->port_desc, (char*)in->port_desc);
		memset(existed->port_desc, 0, sizeof(existed->port_desc));
		memcpy(existed->port_desc, in->port_desc, strlen(in->port_desc));
		update_rs_data_1st_level(port, existed, IEEE802_DOT1AB_LLDP_PORT_DESC);
	}

	if (strcmp(in->system_name, existed->system_name) != 0)
	{
		UB_LOG(UBL_INFO, "%s: changed system_name [%s -> %s]\n", __func__, (char*)existed->system_name, (char*)in->system_name);
		memset(existed->system_name, 0, sizeof(existed->system_name));
		memcpy(existed->system_name, in->system_name, strlen(in->system_name));
		update_rs_data_1st_level(port, existed, IEEE802_DOT1AB_LLDP_SYSTEM_NAME);
	}

	if (strcmp(in->system_description, existed->system_description) != 0)
	{
		UB_LOG(UBL_INFO, "%s: changed system_description [%s %s]\n", __func__, (char*)existed->system_description, (char*)in->system_description);
		memset(existed->system_description, 0, sizeof(existed->system_description));
		memcpy(existed->system_description, in->system_description, strlen(in->system_description));
		update_rs_data_1st_level(port, existed, IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION);
	}

	if (in->system_capabilities_enabled != existed->system_capabilities_enabled)
	{
		UB_LOG(UBL_INFO, "%s: changed system_capabilities_enabled [%x -> %x]\n", __func__, existed->system_capabilities_enabled, in->system_capabilities_enabled);
		existed->system_capabilities_enabled = in->system_capabilities_enabled;
		update_rs_data_1st_level(port, existed, IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED);
	}

	if (in->system_capabilities_supported != existed->system_capabilities_supported)
	{
		UB_LOG(UBL_INFO, "%s: changed system_capabilities_supported [%x -> %x]\n", __func__, existed->system_capabilities_supported, in->system_capabilities_supported);
		existed->system_capabilities_supported = in->system_capabilities_supported;
		update_rs_data_1st_level(port, existed, IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED);
	}

	// because the list is dynamic, so better if we found any different, we will remove all existed item, and create new
	if (!is_same_mgmt_addr_list(&in->management_address, &existed->management_address))
	{
		
		UB_LOG(UBL_DEBUG, "%s: management_address diff. delete old mgmt list -> create new \n", __func__);
		delete_rs_data_2nd_level(port, existed, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_SUBTYPE);
		delete_rs_data_2nd_level(port, existed, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_ID);
		uint8_t cmd = CLEAR_CMD_MGMT_ADDR;
		ub_list_clear(&existed->management_address, clear_cb, &cmd);
		
		if (ub_list_count(&in->management_address) > 0)
		{
			struct ub_list_node* tmp_node;
			for(UB_LIST_FOREACH_ITER(&in->management_address, tmp_node))
			{
				management_address_t* mgmt_addr = (management_address_t*)tmp_node;
				management_address_t* copied_mgmt_addr = init_mgmt_addr();
				memset(copied_mgmt_addr, 0, sizeof(management_address_t));

				// strcpy(copied_mgmt_addr->address, mgmt_addr->address);
				memcpy(copied_mgmt_addr->address, mgmt_addr->address, strlen((char*)mgmt_addr->address));
				copied_mgmt_addr->address_subtype = mgmt_addr->address_subtype;
				copied_mgmt_addr->if_id = mgmt_addr->if_id;
				copied_mgmt_addr->if_subtype = mgmt_addr->if_subtype;

				UB_LOG(UBL_INFO, "%s:%s added new management_address %s \n", __func__, port->name, mgmt_addr->address);
				ub_list_append(&existed->management_address, (struct ub_list_node*)copied_mgmt_addr);
			}

			// mgmt addr
			update_rs_data_2nd_level(port, existed, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_SUBTYPE);
			update_rs_data_2nd_level(port, existed, IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, IEEE802_DOT1AB_LLDP_IF_ID);
		}
		else
		{
			UB_LOG(UBL_INFO, "%s: Received LLDPDU doesnt have Management Addr TLVs \n", __func__);
		}
	}
	
	UB_TLOG(UBL_DEBUG, "%s:%s Update %s k(%u, %d) \n", __func__, port->name, existed->port_desc, existed->remote_index, existed->time_mark);
}

static bool is_manangement_addr_tx_port_existed(lldp_port_t* port, address_subtype_t subtype, char* man_address)
{
	bool ret = false;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&port->management_address_tx_port, tmp_node))
	{
		management_address_tx_port_t* management_addr_tx_port = (management_address_tx_port_t*)tmp_node;
		if (strcmp(man_address, (char*)management_addr_tx_port->man_address) == 0 && management_addr_tx_port->address_subtype == subtype)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

static management_address_tx_port_t* get_existed_manament_addr_tx(lldp_port_t* port, address_subtype_t subtype, char* man_address)
{
	management_address_tx_port_t* m = NULL;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&port->management_address_tx_port, tmp_node))
	{
		management_address_tx_port_t* management_addr_tx_port = (management_address_tx_port_t*)tmp_node;
		if (strcmp(man_address, (char*)management_addr_tx_port->man_address) == 0 && management_addr_tx_port->address_subtype == subtype)
		{
			m = management_addr_tx_port;
			break;
		}
	}

	return m;
}

static bool is_remote_unknown_tlv_existed(remote_systems_data_t* rs_data, uint32_t tlv_type)
{
	bool ret = false;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&rs_data->remote_unknown_tlv, tmp_node))
	{
		remote_unknown_tlv_t* remote_unknow_tlv = (remote_unknown_tlv_t*)tmp_node;
		if (remote_unknow_tlv->tlv_type == tlv_type)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

static remote_unknown_tlv_t* get_existed_remote_unknown_tlv(remote_systems_data_t* rs_data, uint32_t tlv_type)
{
	remote_unknown_tlv_t* t = NULL;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&rs_data->remote_unknown_tlv, tmp_node))
	{
		remote_unknown_tlv_t* remote_unknow_tlv = (remote_unknown_tlv_t*)tmp_node;
		if (remote_unknow_tlv->tlv_type == tlv_type)
		{
			t = remote_unknow_tlv;
			break;
		}
	}

	return t;
}

static bool is_remote_org_defined_existed(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index)
{
	bool ret = false;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&rs_data->remote_org_defined_info, tmp_node))
	{
		remote_org_defined_info_t* remote_org_defined_info = (remote_org_defined_info_t*)tmp_node;
		if (remote_org_defined_info->info_identifier == info_id 
		&& remote_org_defined_info->info_identifier == info_index
		&& remote_org_defined_info->info_subtype == info_sybtype)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

static remote_org_defined_info_t* get_existed_remote_org(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index)
{
	remote_org_defined_info_t* t = NULL;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&rs_data->remote_org_defined_info, tmp_node))
	{
		remote_org_defined_info_t* remote_org_defined_info = (remote_org_defined_info_t*)tmp_node;
		if (remote_org_defined_info->info_identifier == info_id 
		&& remote_org_defined_info->info_identifier == info_index
		&& remote_org_defined_info->info_subtype == info_sybtype)
		{
			t = remote_org_defined_info;
			break;
		}
	}

	return t;
}


static bool is_manangement_addr_existed(remote_systems_data_t* rs_data, address_subtype_t subtype, char* addr)
{
	bool ret = false;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&rs_data->management_address, tmp_node))
	{
		management_address_t* management_addr = (management_address_t*)tmp_node;

		if (management_addr->address_subtype == subtype )
		{
			switch(subtype)
			{
				case ipv4:
					if (memcmp(management_addr->address, addr, 4) == 0) ret = true;
					break;
				case ipv6:
					if (memcmp(management_addr->address, addr, 16) == 0) ret = true;
					break;
				default:
					if (strcmp((char*)management_addr->address, addr) == 0) ret = true;
				break;
			}
			if (ret)
				break;
		}
	}

	return ret;
}

static management_address_t* get_existed_manament_addr(remote_systems_data_t* rs_data, address_subtype_t subtype, char* addr)
{
	management_address_t* m = NULL;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&rs_data->management_address, tmp_node))
	{
		management_address_t* management_addr = (management_address_t*)tmp_node;

		if (management_addr->address_subtype == subtype )
		{
			switch(subtype)
			{
				case ipv4:
					if (memcmp(management_addr->address, addr, 4) == 0) return management_addr;
					break;
				case ipv6:
					if (memcmp(management_addr->address, addr, 16) == 0) return management_addr;
					break;
				default:
					if (strcmp((char*)management_addr->address, addr) == 0) return management_addr;
				break;
			}
		}
	}

	return m;
}

static bool is_remote_system_data_existed(lldp_port_t* port, timeticks time_mark, uint32_t remote_index)
{
	bool ret = false;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&port->remote_systems_data, tmp_node))
	{
		remote_systems_data_t* remote_system_data = (remote_systems_data_t*)tmp_node;
		if (time_mark == remote_system_data->time_mark && remote_system_data->remote_index == remote_index)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

static remote_systems_data_t* get_existed_remote_system_data(lldp_port_t* port, timeticks time_mark, uint32_t remote_index)
{
	remote_systems_data_t* r = NULL;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&port->remote_systems_data, tmp_node))
	{
		remote_systems_data_t* remote_system_data = (remote_systems_data_t*)tmp_node;
		if (time_mark == remote_system_data->time_mark && remote_system_data->remote_index == remote_index)
		{
			r = remote_system_data;
			break;
		}
	}

	return r;
}

static bool is_port_existed(yang_lldp_t* lldp, const char * if_name, uint8_t* mac)
{
	bool ret = false;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&lldp->ports, tmp_node))
	{
		lldp_port_t* p = (lldp_port_t*)tmp_node;
	
		if ( (strcmp(if_name, p->name)  == 0) && (memcmp(p->dest_mac_address, mac, 6) == 0 ))
		{
			ret = true;
			break;
		}
	}

	return ret;
}

static lldp_port_t* get_existed_port(yang_lldp_t* lldp, const char * if_name, uint8_t* mac)
{
	lldp_port_t* p = NULL;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(&lldp->ports, tmp_node))
	{
		p = (lldp_port_t*)tmp_node;
		if ( (strcmp(if_name, p->name)  == 0) && (memcmp(p->dest_mac_address, mac, 6) == 0 ))
		{
			break;
		}
	}

	return p;
}

static void fill_remote_statistic_info(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp)
{
	// UB_LOG(UBL_DEBUG, "%s: %s\n", __func__, ieee802_dot1ab_lldp_get_string(k3));
	switch(k3)
	{
		case IEEE802_DOT1AB_LLDP_LAST_CHANGE_TIME:
			lldp->remote_statistic.last_change_time = *((uint32_t*)prm->value);
			// UB_LOG(UBL_DEBUG, "%s: last_change_time %u\n", __func__, lldp->remote_statistic.last_change_time);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_INSERTS:
			lldp->remote_statistic.remote_inserts = *((uint32_t*)prm->value);
			// UB_LOG(UBL_DEBUG, "%s: remote_inserts %u\n", __func__, lldp->remote_statistic.remote_inserts);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_DELETES:
			lldp->remote_statistic.remote_deletes= *((uint32_t*)prm->value);
			// UB_LOG(UBL_DEBUG, "%s: remote_deletes %u\n", __func__, lldp->remote_statistic.remote_deletes);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_DROPS:
			lldp->remote_statistic.remote_drops = *((uint32_t*)prm->value);
			// UB_LOG(UBL_DEBUG, "%s: remote_drops %u\n", __func__, lldp->remote_statistic.remote_drops);
			break;
		case IEEE802_DOT1AB_LLDP_REMOTE_AGEOUTS:
			lldp->remote_statistic.remote_ageouts = *((uint32_t*)prm->value);
			// UB_LOG(UBL_DEBUG, "%s: remote_ageouts %u\n", __func__, lldp->remote_statistic.remote_ageouts);
			break;
	}
}

static void fill_local_system_data(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp)
{
	switch(k3)
	{
		case IEEE802_DOT1AB_LLDP_CHASSIS_ID:
			memcpy(lldp->local_system_data.chassis_id, (char*)prm->value, prm->vsize);
			// UB_LOG(UBL_INFO, "%s: chassis id %s \n", __func__, lldp->local_system_data.chassis_id);
			break;
		case IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE:
			lldp->local_system_data.chassis_id_subtype = *((chassis_id_type_t*)prm->value);
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_NAME:
			memcpy(lldp->local_system_data.system_name, (char*)prm->value, prm->vsize);
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION:
			memcpy(lldp->local_system_data.system_description, (char*)prm->value, prm->vsize);
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED:
			lldp->local_system_data.system_capabilities_enabled = *((uint16_t*)prm->value);
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED:
			lldp->local_system_data.system_capabilities_supported = *((uint16_t*)prm->value);
			break;
	}
}

static void fill_management_info(uint8_t* k, address_subtype_t addr_subtype, char* man_addr, lldp_cfg_param_t* prm, lldp_port_t* port)
{
	management_address_tx_port_t* man_addr_tx_port = NULL;
	if (!is_manangement_addr_tx_port_existed(port, addr_subtype, man_addr))
	{
		// UB_LOG(UBL_DEBUG, "%s addr not existed. Added new man addr tx port \n", man_addr);
		man_addr_tx_port  = init_mgmt_tx_port();
		memset(man_addr_tx_port, 0, sizeof(management_address_tx_port_t));
		memcpy(man_addr_tx_port->man_address, man_addr, strlen(man_addr));
		man_addr_tx_port->address_subtype =  addr_subtype;
		ub_list_append(&port->management_address_tx_port, (struct ub_list_node*)man_addr_tx_port);
	}
	else
	{
		man_addr_tx_port = get_existed_manament_addr_tx(port, addr_subtype, man_addr);
	}
	
	if (man_addr_tx_port != NULL)
	{
        switch (k[4])
        {
        case IEEE802_DOT1AB_LLDP_TX_ENABLE:
            man_addr_tx_port->tx_enable = *((bool*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_ADDR_LEN:
            man_addr_tx_port->addr_len = *((uint32_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_IF_SUBTYPE:
            man_addr_tx_port->if_subtype = *((man_addr_if_subtype_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_IF_ID:
            man_addr_tx_port->if_id = *((uint32_t*)prm->value);
            break;
        default:
            break;
        }
	}
}

static void fill_manament_addr(uint8_t* k, address_subtype_t addr_subtype, char* addr, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data)
{
	management_address_t* mgr_addr = NULL;
	if (!is_manangement_addr_existed(rs_data, addr_subtype, addr))
	{
		// UB_LOG(UBL_DEBUG, "%d addr subtype not existed. Added new man addr \n", addr_subtype);
		mgr_addr = init_mgmt_addr();
		memset(mgr_addr, 0, sizeof(management_address_t));
		mgr_addr->address_subtype = addr_subtype;
		// memcpy(mgr_addr->address,  addr, strlen(addr));
		if (addr_subtype == ipv4)
			memcpy(mgr_addr->address, addr, 4);
		else if (addr_subtype == ipv6)
			memcpy(mgr_addr->address, addr, 16);
		else memcpy(mgr_addr->address, addr, strlen(addr));
		ub_list_append(&rs_data->management_address, (struct ub_list_node*) mgr_addr);
	}
	else
	{
		// UB_LOG(UBL_DEBUG, "man addr %d existed\n", addr_subtype);
		mgr_addr = get_existed_manament_addr(rs_data, addr_subtype, addr);
	}

	if (mgr_addr != NULL)
	{
        switch(k[5])
        {
        case IEEE802_DOT1AB_LLDP_IF_SUBTYPE:
            mgr_addr->if_subtype = *((man_addr_if_subtype_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_IF_ID:
            mgr_addr->if_id = *((uint32_t*)prm->value);
            break;
        }
	}
}

static void fill_remote_unknown_tlv(uint8_t* k, uint32_t tlv_type, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data)
{
	remote_unknown_tlv_t* remote_unknow_tlv = NULL;
	if (!is_remote_unknown_tlv_existed(rs_data, tlv_type))
	{
		// UB_LOG(UBL_INFO, "%d addr subtype not existed. Added new tlv \n", tlv_type);
		remote_unknow_tlv = init_rm_unknown_tlv();
		memset(remote_unknow_tlv, 0, sizeof(remote_unknown_tlv_t));
		remote_unknow_tlv->tlv_type = tlv_type;
		ub_list_append(&rs_data->remote_unknown_tlv, (struct ub_list_node*)remote_unknow_tlv);
	}
	else
	{
		// UB_LOG(UBL_INFO, "tlv %d existed\n", tlv_type);
		remote_unknow_tlv = get_existed_remote_unknown_tlv(rs_data, tlv_type);
	}

	switch(k[5])
	{
	case IEEE802_DOT1AB_LLDP_TLV_INFO:
		if (remote_unknow_tlv)
	        {
			memcpy(remote_unknow_tlv->tlv_info, (uint8_t*)prm->value, prm->vsize);
		}
		break;
	}
}

static void fill_remote_org_info(uint8_t* k, uint32_t info_id, uint32_t info_subtype, uint32_t info_index, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data)
{
	remote_org_defined_info_t* remote_org_info = NULL;
	if (!is_remote_org_defined_existed(rs_data, info_id, info_subtype, info_index))
	{
		// UB_LOG(UBL_DEBUG, "%d - %d org info not existed. Added new  \n", info_id, info_index);
		remote_org_info = init_rm_org_def();
		memset(remote_org_info, 0, sizeof(remote_org_defined_info_t));
		remote_org_info->info_identifier = info_id;
		remote_org_info->info_index = info_index;
		remote_org_info->info_subtype = info_subtype;

		ub_list_append(&rs_data->remote_org_defined_info, (struct ub_list_node*) remote_org_info);
	}
	else
	{
		// UB_LOG(UBL_DEBUG, "%d - %d org info existed  \n", info_id, info_index);
		remote_org_info = get_existed_remote_org(rs_data, info_id, info_subtype, info_index);
	}

	switch(k[5])
	{
	case IEEE802_DOT1AB_LLDP_REMOTE_INFO:
		// UB_LOG(UBL_INFO, "copy oui info [%d] \n",  prm->vsize);
		// ub_hexdump(true, true,  (uint8_t*)prm->value, prm->vsize, 0);
		if (remote_org_info)
		{
			memcpy(remote_org_info->remote_info, (uint8_t*)prm->value, prm->vsize);
		}
		// UB_LOG(UBL_INFO, "copy oui info [%d] done\n",  prm->vsize);
		break;
	}

}

static void fill_remote_system_data(uint8_t* k, timeticks time_mark, uint32_t remote_index, lldp_cfg_param_t* prm, lldp_port_t* port)
{
	remote_systems_data_t* remote_system_data;
	
	if ( !is_remote_system_data_existed(port, time_mark, remote_index) )
	{
		// UB_LOG(UBL_DEBUG, "%d rs data not existed. Add new \n", remote_index);
		remote_system_data = init_rm_system_data();
		memset(remote_system_data, 0, sizeof(remote_systems_data_t));
		remote_system_data->remote_index = remote_index;
		remote_system_data->time_mark =  time_mark;
		
		ub_list_init(&remote_system_data->management_address);
		ub_list_init(&remote_system_data->remote_org_defined_info);
		ub_list_init(&remote_system_data->remote_unknown_tlv);
		
		ub_list_append(&port->remote_systems_data, (struct ub_list_node*)remote_system_data);
	}
	else
	{
		remote_system_data = get_existed_remote_system_data(port, time_mark, remote_index);
	}
	
	// UB_LOG(UBL_DEBUG, "%s filling key %s \n", __func__, ieee802_dot1ab_lldp_get_string(k[4]));
	if (remote_system_data != NULL)
	{
        switch (k[4])
        {
        case IEEE802_DOT1AB_LLDP_REMOTE_TOO_MANY_NEIGHBORS:
            remote_system_data->remote_too_many_neighbors = *((bool*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_CHANGES:
            remote_system_data->remote_changes = *((bool*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE:
            remote_system_data->port_id_subtype = *((port_id_type_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_PORT_ID:
            memcpy(remote_system_data->port_id, (char*)prm->value, strlen((char*)prm->value));
            break;
        case IEEE802_DOT1AB_LLDP_PORT_DESC:
            memcpy(remote_system_data->port_desc, (char*)prm->value, strlen((char*)prm->value));
            break;
        case IEEE802_DOT1AB_LLDP_CHASSIS_ID:
            memcpy(remote_system_data->chassis_id, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE:
            remote_system_data->chassis_id_subtype = *((chassis_id_type_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_NAME:
            memcpy(remote_system_data->system_name, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION:
            memcpy(remote_system_data->system_description, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED:
            remote_system_data->system_capabilities_enabled = *((uint16_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED:
            remote_system_data->system_capabilities_supported = *((uint16_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS:
            {
                address_subtype_t addr_subtype = *((address_subtype_t*)prm->kvs[4]);
                uint8_t* addr = (uint8_t*)prm->kvs[5];
                if (addr_subtype == ipv4)
                {
                    ub_hexdump(true, true, addr, 4, 0);
                }
                else if (addr_subtype == ipv6)
                {
                    ub_hexdump(true, true, addr, 16, 0);
                }
                fill_manament_addr(k, addr_subtype, (char*)addr, prm, remote_system_data);
            }
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV:
            {
                uint32_t tlv_type = *((uint32_t*)prm->kvs[4]);
                fill_remote_unknown_tlv(k, tlv_type, prm, remote_system_data);
            }
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO:
            {
                uint32_t info_id = *((uint32_t*)prm->kvs[4]);
                uint32_t info_subtype = *((uint32_t*)prm->kvs[5]);
                uint32_t info_index = *((uint32_t*)prm->kvs[6]);
                fill_remote_org_info(k, info_id, info_subtype, info_index, prm, remote_system_data);
            }
            break;
        default:
            break;
        }
	}
}

static void fill_port_info(uint8_t* k, char* if_name, uint8_t* mac, lldp_cfg_param_t* prm, yang_lldp_t* lldp)
{
	lldp_port_t* port;
	if (!is_port_existed(lldp, if_name, mac))
	{
		// UB_LOG(UBL_DEBUG, "%s not existed. Added new lldp port \n", if_name);
		port = init_cfg_port();
		memset(port, 0, sizeof(lldp_port_t));
		ub_list_init(&port->remote_systems_data);
		ub_list_init(&port->management_address_tx_port);
		memcpy(port->name, if_name, strlen(if_name));
		memcpy(port->dest_mac_address, mac, 6);
		ub_list_append(&lldp->ports, (struct ub_list_node*) port);
	}
	else
	{
		port = get_existed_port(lldp, if_name, mac);
	}
	// Port cannot NULL
	if (port != NULL)
	{
        switch(k[3])
        {
            case IEEE802_DOT1AB_LLDP_ADMIN_STATUS:
                port->admin_status = *((admin_status_t*)prm->value);
                break;
    #if 0
            case IEEE802_DOT1AB_LLDP_NOTIFICATION_ENABLE:
                port->notification_enable = *((bool*)prm->value);
                break;
    #endif
            case IEEE802_DOT1AB_LLDP_TLVS_TX_ENABLE:
                port->tlvs_tx_enable = *((uint8_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX:
                port->message_fast_tx = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER:
                port->message_tx_hold_multiplier = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL:
                port->message_tx_interval = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_REINIT_DELAY:
                port->reinit_delay = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX:
                port->tx_credit_max = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_TX_FAST_INIT:
                port->tx_fast_init = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS_TX_PORT:
                {
                    char* man_addr = (char*)prm->kvs[3];
                    address_subtype_t addr_subtype = *((address_subtype_t*) prm->kvs[2]);
                    // UB_LOG(UBL_INFO, "%s: IP [%s] subtype: %d\n",port->name, man_addr, addr_subtype);
                    fill_management_info(k, addr_subtype, man_addr, prm, port);
                }
                break;
            case IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE:
                port->port_id_subtype = *((port_id_type_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_PORT_ID:
                memcpy(port->port_id, (char*)prm->value, strlen((char*)prm->value));
                break;
            case IEEE802_DOT1AB_LLDP_PORT_DESC:
                memcpy(port->port_desc, (char*)prm->value, strlen((char*)prm->value));
                break;
            case IEEE802_DOT1AB_LLDP_TX_STATISTICS:
                switch(k[4])
                {
                    case IEEE802_DOT1AB_LLDP_TOTAL_FRAMES:
                        port->tx_statistic.total_frames = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_LENGTH_ERRORS:
                        port->tx_statistic.total_length_errors = *((uint32_t*)prm->value);
                        break;
                }
                break;
            case IEEE802_DOT1AB_LLDP_RX_STATISTICS:
                switch(k[4])
                {
                    case IEEE802_DOT1AB_LLDP_TOTAL_AGEOUTS:
                        port->rx_statistic.total_ageouts = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_FRAMES:
                        port->rx_statistic.total_discarded_frames = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_ERROR_FRAMES:
                        port->rx_statistic.error_frames = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_TLVS:
                        port->rx_statistic.total_discarded_tlvs = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_UNRECOGNIZED_TLVS:
                        port->rx_statistic.total_unrecognized_tlvs = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_FRAMES:
                        port->rx_statistic.total_frames = *((uint32_t*)prm->value);
                        break;
                }
                break;
            case IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA:
                {
                    timeticks time_mark = *((timeticks*)prm->kvs[2]);
                    uint32_t remote_index = *((uint32_t*)prm->kvs[3]);
                    fill_remote_system_data(k, time_mark, remote_index, prm, port);
                }
                break;
        }
	}
	
}

static void fill_lldp_config_info(void *key, yang_lldp_t* lldp, lldp_cfg_param_t* prm)
{
	if (key)
	{
		if( ((uint8_t*)key)[1] == IEEE802_DOT1AB_LLDP_LLDP )
		{
			// UB_LOG(UBL_INFO, "%s: RW key[0] %d\n", __func__, ((uint8_t*)key)[0]);
			switch (((uint8_t*)key)[2])
			{
				case IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX:
					lldp->message_fast_tx = *((uint32_t*)prm->value);
					break;
				case IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER:
					lldp->message_tx_hold_multiplier = *((uint32_t*)prm->value);
					break;
				case IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL:
					lldp->message_tx_interval = *((uint32_t*)prm->value);
					break;
				case IEEE802_DOT1AB_LLDP_REINIT_DELAY:
					lldp->reinit_delay = *((uint32_t*)prm->value);
					break;
				case IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX:
					lldp->tx_credit_max = *((uint32_t*)prm->value);
					break;
				case IEEE802_DOT1AB_LLDP_TX_FAST_INIT:
					lldp->tx_fast_init = *((uint32_t*)prm->value);
					break;
#if 0
				case IEEE802_DOT1AB_LLDP_NOTIFICATION_INTERVAL:
					lldp->notification_interval = *((uint32_t*)prm->value);
					break;
#endif
				case IEEE802_DOT1AB_LLDP_REMOTE_STATISTICS:
					fill_remote_statistic_info(((uint8_t*)key)[3], prm, lldp);
					break;
				case IEEE802_DOT1AB_LLDP_LOCAL_SYSTEM_DATA:
					fill_local_system_data(((uint8_t*)key)[3], prm, lldp);
					break;
				case IEEE802_DOT1AB_LLDP_PORT:
				{
					ub_macaddr_t mac = {0};
					char* if_name = (char*)prm->kvs[0];
					memcpy(mac, (uint8_t*)prm->kvs[1], prm->kss[1]);
					if (is_mac_valid(mac))
					{
						fill_port_info((uint8_t*)key, if_name, mac, prm, lldp);
					}
					else
					{
						UB_LOG(UBL_INFO, "%s Invalid entry due to MAC invalid \n", if_name);
						#if 0
						UB_LOG(UBL_INFO, "%s %s/%s/%s/%s \n", if_name, ieee802_dot1ab_lldp_get_string(((uint8_t*)key)[3]),
						 ieee802_dot1ab_lldp_get_string(((uint8_t*)key)[4]),
						 ieee802_dot1ab_lldp_get_string(((uint8_t*)key)[5]),
						 ieee802_dot1ab_lldp_get_string(((uint8_t*)key)[6]));
						#endif
					}
				}
				break;
			}
		}
	}
}

static bool is_mac_valid (ub_macaddr_t mac)
{
	int i=0;
	for (i=0; i<6; i++)
	{
		if (mac[i] != 0) return true; // Only invalid if all mac are zero
	}

	return false;
}

static void load_specific_module(yang_db_item_access_t *ydbia, yang_lldp_t* lldp, uc_range *range)
{
	lldp_cfg_param_t prm;
	void *key=NULL;
	uint32_t ksize;
	uc_dbald *dbald = ydbia->dbald;
	
	while(true){
		memset(&prm, 0, sizeof(prm));
		if(uc_get_key_in_range(ydbia->dbald, range, &key, &ksize, UC_DBAL_NOMOVE)){
			break;
		}
		// UB_LOG(UBL_INFO, "%s ksize %d \n", (char*)key, ksize);
		if(uc_get_value_in_range(ydbia->dbald, range, &prm.value, &prm.vsize, UC_DBAL_FORWARD)){
			break;
		}

		if(yang_db_extract_key(key, ksize, &prm.aps, prm.kvs, prm.kss)!=0){
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, prm.rstr);
			continue;
		}

		if( ((uint8_t*)key)[0] == IEEE802_DOT1AB_LLDP_RW || ((uint8_t*)key)[0] == IEEE802_DOT1AB_LLDP_RO ) 
		{
			fill_lldp_config_info(key, lldp, &prm);
		}

		yang_db_extract_key_free(prm.aps, prm.kvs, prm.kss);
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, prm.rstr);
	}
}

static void refill_port_interval_info(yang_lldp_t* lldp)
{
	struct ub_list_node* tmp_node;
	for(UB_LIST_FOREACH_ITER(&lldp->ports, tmp_node))
	{
		lldp_port_t* port = (lldp_port_t*) tmp_node;
		port->message_tx_interval = (port->message_tx_interval == 0) ? lldp->message_tx_interval : port->message_tx_interval;
		port->message_fast_tx = (port->message_fast_tx == 0) ? lldp->message_fast_tx : port->message_fast_tx;
		port->message_tx_hold_multiplier = (port->message_tx_hold_multiplier == 0) ? lldp->message_tx_hold_multiplier : port->message_tx_hold_multiplier;
		port->reinit_delay = (port->reinit_delay == 0) ? lldp->reinit_delay : port->reinit_delay;
		port->tx_credit_max = (port->tx_credit_max == 0) ? lldp->tx_credit_max : port->tx_credit_max;
		port->tx_fast_init = (port->tx_fast_init == 0) ? lldp->tx_fast_init : port->tx_fast_init;
	}
}
int ydbi_load_lldp_db(yang_db_item_access_t *ydbia, yang_lldp_t* lldp)
{
	uc_range *range=NULL;
	uc_dbald *dbald = ydbia->dbald;
	int ret=0;

	// Get IEEE802_DOT1AB_LLDP_RO/key*
	uint8_t key1[1]={IEEE802_DOT1AB_LLDP_RO};
	uint8_t key2[2]={IEEE802_DOT1AB_LLDP_RO, 255};
	range=uc_get_range(ydbia->dbald, key1, 1, key2, 2);
	load_specific_module(ydbia, lldp, range);
	uc_get_range_release(ydbia->dbald, range);
	uc_dbal_releasedb(ydbia->dbald);

	// Get IEEE802_DOT1AB_LLDP_RW/key*
	key1[0]=IEEE802_DOT1AB_LLDP_RW;
	key2[0]=IEEE802_DOT1AB_LLDP_RW;
	key2[1]=255;
	range=uc_get_range(ydbia->dbald, key1, 1, key2, 2);
	load_specific_module(ydbia, lldp, range);
	uc_get_range_release(ydbia->dbald, range);
	uc_dbal_releasedb(ydbia->dbald);
	// Done

	// Fill port->interval info by global info if it's not existed
	refill_port_interval_info(lldp);
	return ret;
}

/// @brief Depend on Posix or LLDP, the input Uniconf access mode is different
/// @param mode 
void ydb_set_uniconf_access_mode(uint8_t mode)
{
	g_uniconf_access_mode = mode;
}
