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
#include <unistd.h>
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/yangs/yang_db_runtime.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include <tsn_uniconf/yangs/ieee802-dot1ab-lldp.h>
#include <tsn_uniconf/yangs/ieee802-dot1ab-lldp_access.h>
#include <tsn_uniconf/yangs/ietf-interfaces_access.h>
#include "../yangs/generated/ieee802-dot1ab-lldp_runconf.h"
#include "../lldp_yangdb.h"
#include "../lldp_utils.h"
#include <signal.h>
#include <getopt.h>

bool terminated = false;
char* lldp_db_name = "cfg/lldp_db";
yang_lldp_t yang_lldp_cfg;

typedef struct 
{
	uint8_t index;
	char* cap;
} capability_pair;
capability_pair caps_pair[] = 
{
	{0, "Other"},
	{1, "Repeater"},
	{2, "Bridge"},
	{3, "WLAN"},
	{4, "Router"} ,
	{5, "Telephone"},
	{6, "DOCSIS cable device"},
	{7, "Station Only"},
	{8, "C-VLAN"},
	{9, "S-VLAN"} ,
	{10,"Two-port MAC Relay component"},
};
yang_lldp_t changed_yang_lldp_cfg;
lldp_port_t changed_port_info;
management_address_tx_port_t changed_mgmt_tx_port;

typedef struct lldpcli_cmd
{
	bool is_showing_neighbor;
	bool is_showing_statistic;
	char interface_name[16];

	bool is_configure_local_system;
	// local system
	bool configure_chassis_subtype;
	bool configure_chassis_id;
	bool configure_system_name;
	bool configure_system_desc;
	bool configure_system_cap_enabled;
	bool configure_system_cap_supported;
	bool configure_message_fast_tx;
	bool configure_message_tx_hold_multiplier;
	bool configure_message_tx_interval;
	bool configure_reinit_delay;
	bool configure_tx_credit_max;
	bool configure_tx_fast_init;

	bool is_configure_port;
	bool is_configure_dest_mac;
	// port
	bool configure_admin_status;
	bool configure_tlvs_tx_enable;
	bool configure_port_id_subtype;
	bool configure_port_id;
	bool configure_port_desc;

	//mgmt addr
	bool configure_addr_subtype;
	bool configure_man_addr;
	bool configure_tx_enable;
	bool configure_if_subtype;
	bool configure_if_id;

	bool status; // true: IEEE802_DOT1AB_LLDP_RO false: IEEE802_DOT1AB_LLDP_RW
} lldpcli_cmd_t;
lldpcli_cmd_t input_cmd;

/* Signal Handler for SIGINT */
void handle_signal(int sig_num)
{
	terminated = 1;
}

char* get_bit_on_off(uint16_t caps, int index)
{
	uint16_t val = (caps >> index) & 0x0001;
	if (val) return "capable";
	else return "no capabale";
}
static void decode_cap(uint16_t caps)
{
	for (int i=0; i<sizeof(caps_pair) / sizeof(capability_pair); i++)
	{
		UB_LOG(UBL_INFO, "\t\t%s: \t\t\t%s\n",get_bit_on_off(caps, caps_pair[i].index), caps_pair[i].cap);
	}
}

/// @brief 
/// @param interface_name Can be specific interface name or 'all'
void show_stats(char* interface_name)
{
	if (strcmp(interface_name, "all") == 0)
	{
		UB_LOG(UBL_INFO, "Remote Statistic:\n");
		UB_LOG(UBL_INFO, "\tLast changed time: %u\n", yang_lldp_cfg.remote_statistic.last_change_time);
		UB_LOG(UBL_INFO, "\tRemote Inserts: %d\n", yang_lldp_cfg.remote_statistic.remote_inserts);
		UB_LOG(UBL_INFO, "\tRemote Deletes: %d\n", yang_lldp_cfg.remote_statistic.remote_deletes);
		UB_LOG(UBL_INFO, "\tRemote Drops: %d\n", yang_lldp_cfg.remote_statistic.remote_drops);
		UB_LOG(UBL_INFO, "\tRemote Ageouts: %d\n", yang_lldp_cfg.remote_statistic.remote_ageouts);
	}

	struct ub_list_node *tmp_port_node;
	for(UB_LIST_FOREACH_ITER(&yang_lldp_cfg.ports, tmp_port_node))
	{
		lldp_port_t* port = (lldp_port_t*)tmp_port_node;
		if (strcmp(port->name, interface_name) == 0 || strcmp(interface_name, "all") == 0) 
		{
			UB_LOG(UBL_INFO, "+++++++++++++++Port[%s]+++++++++++++++++++++\n", port->name);
			UB_LOG(UBL_INFO, " Port %s Tx-Statistic:\n", port->name);
			UB_LOG(UBL_INFO, "\tTotal Frames: %d\n", port->tx_statistic.total_frames);
			UB_LOG(UBL_INFO, "\tTotal Length Error: %d\n", port->tx_statistic.total_length_errors);
			UB_LOG(UBL_INFO, " Port %s Rx-Statistic:\n", port->name);
			UB_LOG(UBL_INFO, "\tTotal Agedout: %d\n", port->rx_statistic.total_ageouts);
			UB_LOG(UBL_INFO, "\tTotal Discarded frames: %d\n", port->rx_statistic.total_discarded_frames);
			UB_LOG(UBL_INFO, "\tTotal Error frames: %d\n", port->rx_statistic.error_frames);
			UB_LOG(UBL_INFO, "\tTotal Frames: %d\n", port->rx_statistic.total_frames);
			UB_LOG(UBL_INFO, "\tTotal Discarded TLVs: %d\n", port->rx_statistic.total_discarded_tlvs);
			UB_LOG(UBL_INFO, "\tTotal Unrecognized TLVs: %d\n", port->rx_statistic.total_unrecognized_tlvs);
		}
	}
}
void show_neighbors(char* interface_name)
{
	UB_LOG(UBL_INFO, "%s\n", __func__);
	struct ub_list_node *tmp_port_node;
	for(UB_LIST_FOREACH_ITER(&yang_lldp_cfg.ports, tmp_port_node))
	{
		lldp_port_t* port = (lldp_port_t*)tmp_port_node;
		if (strcmp(port->name, interface_name) == 0 || strcmp(interface_name, "all") == 0)
		{
			UB_LOG(UBL_INFO, "Neighbors for interface %s:\n", port->name);
			
			struct ub_list_node *tmp_rs_data_node;
			for(UB_LIST_FOREACH_ITER(&port->remote_systems_data, tmp_rs_data_node))
			{
				remote_systems_data_t* rs_data = (remote_systems_data_t*)tmp_rs_data_node;
				UB_LOG(UBL_INFO, "\tChassis (Subtype: %d):\n", rs_data->chassis_id_subtype);
				switch(rs_data->chassis_id_subtype)
				{
					case C_MAC_ADDRESS:
						UB_LOG(UBL_INFO, "\t\tChassis ID:\t\t mac %02x:%02x:%02x:%02x:%02x:%02x\n", 
																(uint8_t)rs_data->chassis_id[0], 
																(uint8_t)rs_data->chassis_id[1], 
																(uint8_t)rs_data->chassis_id[2], 
																(uint8_t)rs_data->chassis_id[3], 
																(uint8_t)rs_data->chassis_id[4], 
																(uint8_t)rs_data->chassis_id[5] );
						break;
					default:
						UB_LOG(UBL_INFO, "\t\tChassis ID: \t\t%s\n", rs_data->chassis_id);
						break;
				}

				UB_LOG(UBL_INFO, "\tSystem name: %s\n", rs_data->system_name);
				UB_LOG(UBL_INFO, "\tSystem description: %s\n", rs_data->system_description);

				// Port info
				UB_LOG(UBL_INFO, "\tPort (Subtype: %d):\n", rs_data->port_id_subtype);
				switch(rs_data->port_id_subtype)
				{
					case P_MAC_ADDRESS:
					UB_LOG(UBL_INFO, "\t\tPort ID:\t\t mac %02x:%02x:%02x:%02x:%02x:%02x\n", 
																(uint8_t)rs_data->port_id[0], 
																(uint8_t)rs_data->port_id[1], 
																(uint8_t)rs_data->port_id[2], 
																(uint8_t)rs_data->port_id[3], 
																(uint8_t)rs_data->port_id[4], 
																(uint8_t)rs_data->port_id[5] );
						break;
					default:
						UB_LOG(UBL_INFO, "\t\tPort ID: \t\t%s\n", rs_data->port_id);
						break;
				}
				UB_LOG(UBL_INFO, "\t\tPort desc: %s\n", rs_data->port_desc);
				UB_LOG(UBL_INFO, "\tSystem Capabilities: 0x%04x \n", rs_data->system_capabilities_supported);
				decode_cap(rs_data->system_capabilities_supported);
				UB_LOG(UBL_INFO, "\tSystem Capabilities Enabled: 0x%04x\n", rs_data->system_capabilities_enabled);
				decode_cap(rs_data->system_capabilities_enabled);

				uint8_t mgmt_counter = 1;
				struct ub_list_node *tmp_mgmt_node;
				for(UB_LIST_FOREACH_ITER(&rs_data->management_address, tmp_mgmt_node))
				{
					management_address_t* mgmt_addr = (management_address_t*)tmp_mgmt_node;
					UB_LOG(UBL_INFO, "\tManagement Addr #%d:\n", mgmt_counter);
					switch(mgmt_addr->address_subtype)
					{
						case ipv4:
							UB_LOG(UBL_INFO, "\t\tAddress Subtype: IPv4\n");
							char ipv4_str[16];
							str_to_ipv4(mgmt_addr->address, ipv4_str);
							UB_LOG(UBL_INFO, "\t\tAddress: %s\n", ipv4_str);
							UB_LOG(UBL_INFO, "\t\tInterface Subtype: %d\n", mgmt_addr->if_subtype);
							UB_LOG(UBL_INFO, "\t\tInterface Number: %d\n", mgmt_addr->if_id);
							break;
						case ipv6:
							UB_LOG(UBL_INFO, "\t\tAddress Subtype: IPv6\n");
							UB_LOG(UBL_INFO, "\t\tAddress: ");
							ub_hexdump(true, true, mgmt_addr->address, 16, 0);
							UB_LOG(UBL_INFO, "\t\tInterface Subtype: %d\n", mgmt_addr->if_subtype);
							UB_LOG(UBL_INFO, "\t\tInterface Number: %d\n", mgmt_addr->if_id);
							break;
						default:
						break;
					}
					mgmt_counter++;
				}

				if (ub_list_count(&rs_data->remote_org_defined_info) > 0)
				{
					struct ub_list_node *tmp_node;
					uint32_t oui_counter = 1;
					for(UB_LIST_FOREACH_ITER(&rs_data->remote_org_defined_info, tmp_node))
					{
						UB_LOG(UBL_INFO, "\tOrganizationally Specific #%d\n", oui_counter);
						remote_org_defined_info_t* rm_org = (remote_org_defined_info_t*) tmp_node;
						UB_LOG(UBL_INFO, "\t\t OUI ID: %06x\n", rm_org->info_identifier);
						UB_LOG(UBL_INFO, "\t\t OUI Subtype: %d\n", rm_org->info_subtype);
						UB_LOG(UBL_INFO, "\t\t OUI Info: ");
						ub_hexdump(true, true, rm_org->remote_info, 16/*sizeof(rm_org->remote_info)*/, 0);
						oui_counter++;
					}
				}

				UB_LOG(UBL_INFO, "\tUnknown TLVs: %d records\n", ub_list_count(&rs_data->remote_unknown_tlv));
				if (ub_list_count(&rs_data->remote_unknown_tlv) > 0)
				{
					struct ub_list_node *tmp_node;
					uint32_t unknown_counter = 1;
					for(UB_LIST_FOREACH_ITER(&rs_data->remote_unknown_tlv, tmp_node))
					{
						UB_LOG(UBL_INFO, "\tUnknown TLV #%d\n", unknown_counter);
						remote_unknown_tlv_t* rm_unknown = (remote_unknown_tlv_t*) tmp_node;
						UB_LOG(UBL_INFO, "\t\t TLV Type: %d\n", rm_unknown->tlv_type);
						UB_LOG(UBL_INFO, "\t\t TLV Info: ");
						ub_hexdump(true, true, rm_unknown->tlv_info, 16/*sizeof(rm_org->remote_info)*/, 0);
						unknown_counter++;
					}
				}
			}
		}
	}
}

static lldp_port_t* find_port_by_name(char* name, ub_macaddr_t dest_mac)
{
	struct ub_list_node *tmp_port_node;
	for(UB_LIST_FOREACH_ITER(&yang_lldp_cfg.ports, tmp_port_node))
	{
		lldp_port_t* port = (lldp_port_t*)tmp_port_node;

		if (strcmp(port->name, changed_port_info.name) == 0 && memcmp(port->dest_mac_address, dest_mac, 6) == 0)
		{
			UB_LOG(UBL_INFO, "Found port to be update %s:\n", port->name);
			return port;
		}
	}

	return NULL;
}
static management_address_tx_port_t* find_mgmt_addr(lldp_port_t* port)
{
	struct ub_list_node *tmp_node;

	for(UB_LIST_FOREACH_ITER(&port->management_address_tx_port, tmp_node))
	{
		management_address_tx_port_t* mgmt_addr = (management_address_tx_port_t*)tmp_node;
		if (changed_mgmt_tx_port.address_subtype == mgmt_addr->address_subtype && strcmp((char *)changed_mgmt_tx_port.man_address, (char *)mgmt_addr->man_address) == 0)
		{
			return mgmt_addr;
		}
	}

	return NULL;
}

static void update_port_1st_level(uint8_t k)
{
	uint8_t kn_if_id[2] = {
		[0] = IEEE802_DOT1AB_LLDP_PORT,
		[1] = k,
	};
	
	attribute_pair_t attr[2] = // Local system data has no value-key
	{
		[0] = {changed_port_info.name, strlen(changed_port_info.name) +1},
		[1] = {changed_port_info.dest_mac_address, 6},
	};

	void* new_val = NULL;
	size_t data_size = 0;
	switch(k)
	{
		case IEEE802_DOT1AB_LLDP_ADMIN_STATUS:
			new_val = (void*)&changed_port_info.admin_status;
			data_size = sizeof(changed_port_info.admin_status);
			break;
		case IEEE802_DOT1AB_LLDP_TLVS_TX_ENABLE:
			new_val = (void*)&changed_port_info.tlvs_tx_enable;
			data_size = sizeof(changed_port_info.tlvs_tx_enable);
			break;
		case IEEE802_DOT1AB_LLDP_PORT_DESC:
			new_val = (void*)&changed_port_info.port_desc;
			data_size = strlen(changed_port_info.port_desc) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE:
			new_val = (void*)&changed_port_info.port_id_subtype;
			data_size = sizeof(changed_port_info.port_id_subtype);
			break;
		case IEEE802_DOT1AB_LLDP_PORT_ID:
			new_val = (void*)&changed_port_info.port_id;
			data_size=strlen((char *)changed_port_info.port_id) + 1;
			break;
		default:
			UB_LOG(UBL_INFO, "To be update %s\n", ieee802_dot1ab_lldp_get_string(k));
			break;
	}

	if (new_val != NULL && data_size > 0)
	{
		UB_LOG(UBL_INFO, "%s Updating %s done\n", __func__, ieee802_dot1ab_lldp_get_string(k));
		int ret = YDBI_SET_ITEM(abknvkn, 
					kn_if_id, 
					2, 
					attr,
					2, 
					input_cmd.status,
					new_val,
					data_size,
					YDBI_PUSH_NOTICE);
		UB_LOG(UBL_INFO, "%s Updating %s done Ret %d\n", __func__, ieee802_dot1ab_lldp_get_string(k), ret);
	}
}

static void update_port_mgmt_addr(uint8_t k)
{
	uint8_t kn_if_id[3] = {
		[0] = IEEE802_DOT1AB_LLDP_PORT,
		[1] = IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS_TX_PORT,
		[2] = k
	};
	
	attribute_pair_t attr[4] = // Local system data has no value-key
	{
		[0] = {changed_port_info.name, strlen(changed_port_info.name) +1},
		[1] = {changed_port_info.dest_mac_address, 6},
		[2] = {&changed_mgmt_tx_port.address_subtype, sizeof(address_subtype_t)},
		[3] = {changed_mgmt_tx_port.man_address, strlen((char*)changed_mgmt_tx_port.man_address) + 1}, // default: ipv4
	};

	// TODO: Should we add one item for length (IEEE802_DOT1AB_LLDP_ADDR_LEN)?

	void* new_val = NULL;
	size_t data_size = 0;
	switch(k)
	{
		case IEEE802_DOT1AB_LLDP_TX_ENABLE:
			new_val = (void*)&changed_mgmt_tx_port.tx_enable;
			data_size = sizeof(changed_mgmt_tx_port.tx_enable);
			break;
		case IEEE802_DOT1AB_LLDP_IF_SUBTYPE:
			new_val = (void*)&changed_mgmt_tx_port.if_subtype;
			data_size = sizeof(changed_mgmt_tx_port.if_subtype);
			break;
		case IEEE802_DOT1AB_LLDP_IF_ID:
			new_val = (void*)&changed_mgmt_tx_port.if_id;
			data_size = sizeof(changed_mgmt_tx_port.if_id);
			break;
		default:
			UB_LOG(UBL_INFO, "To be update %s\n", ieee802_dot1ab_lldp_get_string(k));
			break;
	}

	if (new_val != NULL && data_size > 0)
	{
		UB_LOG(UBL_INFO, "%s Updating %s\n", __func__, ieee802_dot1ab_lldp_get_string(k));
		int ret = YDBI_SET_ITEM(abknvkn, 
					kn_if_id, 
					sizeof(kn_if_id)/sizeof(uint8_t), 
					attr,
					4, 
					input_cmd.status,
					new_val,
					data_size,
					YDBI_PUSH_NOTICE);
		UB_LOG(UBL_INFO, "%s Updating %s done. Ret %d\n", __func__, ieee802_dot1ab_lldp_get_string(k), ret);

		void* value;
		int vsize=YDBI_GET_ITEM(abknvkn, value, 
					kn_if_id, 
					sizeof(kn_if_id)/sizeof(uint8_t), 
					attr,
					4, 
					input_cmd.status);
		ydbi_get_item_release(ydbi_access_handle(), YDBI_REL_LOCK);
		if (vsize > 0)
		{
			UB_LOG(UBL_INFO, "%s Get back %s: %d\n", __func__, ieee802_dot1ab_lldp_get_string(k), *((bool*)value));
		}
	}
}

static void check_for_port_id_changed(lldp_port_t* found_port)
{
	switch(changed_port_info.port_id_subtype)
	{
		default:
			if (input_cmd.configure_port_id && strcmp((char*)found_port->port_id, (char*)changed_port_info.port_id) != 0)
			{
				UB_LOG(UBL_INFO, "port id %s changed from %s -> %s \n", changed_port_info.name, found_port->port_id, changed_port_info.port_id);
				update_port_1st_level(IEEE802_DOT1AB_LLDP_PORT_ID);
			}
		break;
	}
}

static void check_and_update_port()
{
	lldp_port_t* found_port = find_port_by_name(changed_port_info.name, changed_port_info.dest_mac_address);

	if (found_port != NULL)
	{
		memcpy(changed_port_info.dest_mac_address, found_port->dest_mac_address, 6);

		if (input_cmd.configure_admin_status && changed_port_info.admin_status != found_port->admin_status)
		{
			UB_LOG(UBL_INFO, "port %s admin status changed from %d -> %d\n", changed_port_info.name, found_port->admin_status, changed_port_info.admin_status);
			update_port_1st_level(IEEE802_DOT1AB_LLDP_ADMIN_STATUS);
			UB_LOG(UBL_INFO, "port %s admin status changed\n", changed_port_info.name);

		}

		if (input_cmd.configure_tlvs_tx_enable && changed_port_info.tlvs_tx_enable != found_port->tlvs_tx_enable)
		{
			UB_LOG(UBL_INFO, "port %s tlvs_tx_enable changed from %d -> %d\n", changed_port_info.name, found_port->tlvs_tx_enable, changed_port_info.tlvs_tx_enable);
			update_port_1st_level(IEEE802_DOT1AB_LLDP_TLVS_TX_ENABLE);
		}

		if (input_cmd.configure_port_desc && strcmp(changed_port_info.port_desc, found_port->port_desc) != 0)
		{
			UB_LOG(UBL_INFO, "port %s port desc changed from %s -> %s\n", changed_port_info.name, found_port->port_desc, changed_port_info.port_desc);
			update_port_1st_level(IEEE802_DOT1AB_LLDP_PORT_DESC);
		}

		if (input_cmd.configure_port_id_subtype && changed_port_info.port_id_subtype != found_port->port_id_subtype) // we cannot compare string of port id due to various subtype
		{
			UB_LOG(UBL_INFO, "port %s port_id_subtype changed from %d -> %d\n", changed_port_info.name, found_port->port_id_subtype, changed_port_info.port_id_subtype);
			update_port_1st_level(IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE);
		}

		check_for_port_id_changed(found_port);

		if (input_cmd.configure_addr_subtype && input_cmd.configure_man_addr)
		{
			management_address_tx_port_t* found_mgmt_addr = find_mgmt_addr(found_port);
			if (found_mgmt_addr)
			{
				if (input_cmd.configure_tx_enable && found_mgmt_addr->tx_enable != changed_mgmt_tx_port.tx_enable)
				{
					UB_LOG(UBL_INFO, "port %s: addr subtype[%d] tx enable changed from %d -> %d\n", changed_port_info.name, found_mgmt_addr->address_subtype, found_mgmt_addr->tx_enable, changed_mgmt_tx_port.tx_enable);
					update_port_mgmt_addr(IEEE802_DOT1AB_LLDP_TX_ENABLE);
				}

				if (input_cmd.configure_if_subtype && found_mgmt_addr->if_subtype != changed_mgmt_tx_port.if_subtype)
				{
					UB_LOG(UBL_INFO, "port %s: addr subtype[%d] if_subtype changed from %d -> %d\n", changed_port_info.name, found_mgmt_addr->address_subtype, found_mgmt_addr->if_subtype, changed_mgmt_tx_port.if_subtype);
					update_port_mgmt_addr(IEEE802_DOT1AB_LLDP_IF_SUBTYPE);
				}

				if (input_cmd.configure_if_id && found_mgmt_addr->if_id != changed_mgmt_tx_port.if_id)
				{
					UB_LOG(UBL_INFO, "port %s: addr subtype[%d] if_id changed from %d -> %d\n", changed_port_info.name, found_mgmt_addr->address_subtype, found_mgmt_addr->if_id, changed_mgmt_tx_port.if_id);
					update_port_mgmt_addr(IEEE802_DOT1AB_LLDP_IF_ID);
				}
			}
			else
			{
				UB_LOG(UBL_INFO, "Cannot find existed mgmt addr. Considering add new?\n");
				update_port_mgmt_addr(IEEE802_DOT1AB_LLDP_TX_ENABLE);
				update_port_mgmt_addr(IEEE802_DOT1AB_LLDP_IF_SUBTYPE);
				update_port_mgmt_addr(IEEE802_DOT1AB_LLDP_IF_ID);
			}
		}

		uc_dbal_releasedb(ydbi_access_handle()->dbald);
	}
	else
	{
		UB_LOG(UBL_INFO, "Cannot find port %s in db\n", changed_port_info.name);
	}

}

static void update_global_config(uint8_t k)
{
	uint8_t kn_if_id[1] = {
		[0] = k,
	};
	attribute_pair_t dummy_attr; // Local system data has no value-key

	void* new_val = NULL;
	size_t data_size = 0;
	switch(k)
	{
		case IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX:
			new_val = (void*)&changed_yang_lldp_cfg.message_fast_tx;
			data_size = sizeof(changed_yang_lldp_cfg.message_fast_tx);
			break;
		case IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER:
			new_val = (void*)&changed_yang_lldp_cfg.message_tx_hold_multiplier;
			data_size = sizeof(changed_yang_lldp_cfg.message_tx_hold_multiplier);
			break;
		case IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL:
			new_val = (void*)&changed_yang_lldp_cfg.message_tx_interval;
			data_size = sizeof(changed_yang_lldp_cfg.message_tx_interval);
			break;
		case IEEE802_DOT1AB_LLDP_REINIT_DELAY:
			new_val = (void*)&changed_yang_lldp_cfg.reinit_delay;
			data_size = sizeof(changed_yang_lldp_cfg.reinit_delay);
			break;
		case IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX:
			new_val = (void*)&changed_yang_lldp_cfg.tx_credit_max;
			data_size = sizeof(changed_yang_lldp_cfg.tx_credit_max);
			break;
		case IEEE802_DOT1AB_LLDP_TX_FAST_INIT:
			new_val = (void*)&changed_yang_lldp_cfg.tx_fast_init;
			data_size = sizeof(changed_yang_lldp_cfg.tx_fast_init);
		default: break;
	}

	if (new_val != NULL && data_size > 0)
	{
		UB_LOG(UBL_INFO, "%s Updating %s\n", __func__, ieee802_dot1ab_lldp_get_string(k));
		int ret = YDBI_SET_ITEM(abknvkn, 
					kn_if_id, 
					sizeof(kn_if_id)/sizeof(uint8_t), 
					&dummy_attr,
					0, 
					input_cmd.status, // YDBI_CONFIG: false
					new_val,
					data_size,
					YDBI_PUSH_NOTICE);
		UB_LOG(UBL_INFO, "%s Updating %s done %d \n", __func__, ieee802_dot1ab_lldp_get_string(k), ret);
	}
}

static void update_local_system_data(uint8_t k)
{
	uint8_t kn_if_id[2] = {
		[0] = IEEE802_DOT1AB_LLDP_LOCAL_SYSTEM_DATA,
		[1] = k,
	};
	attribute_pair_t dummy_attr; // Local system data has no value-key

	void* new_val = NULL;
	size_t data_size = 0;
	switch(k)
	{
		case IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE:
			new_val = (void*)&changed_yang_lldp_cfg.local_system_data.chassis_id_subtype;
			data_size = sizeof(changed_yang_lldp_cfg.local_system_data.chassis_id_subtype);
			break;
		case IEEE802_DOT1AB_LLDP_CHASSIS_ID:
			new_val = (void*)&changed_yang_lldp_cfg.local_system_data.chassis_id;
			data_size = strlen((char*)changed_yang_lldp_cfg.local_system_data.chassis_id) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_NAME:
			new_val = (void*)&changed_yang_lldp_cfg.local_system_data.system_name;
			data_size = strlen(changed_yang_lldp_cfg.local_system_data.system_name) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION:
			new_val = (void*)&changed_yang_lldp_cfg.local_system_data.system_description;
			data_size = strlen(changed_yang_lldp_cfg.local_system_data.system_description) + 1;
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED:
			new_val = (void*)&changed_yang_lldp_cfg.local_system_data.system_capabilities_enabled;
			data_size = 2;
			break;
		case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED:
			new_val = (void*)&changed_yang_lldp_cfg.local_system_data.system_capabilities_supported;
			data_size = 2;
		default: break;
	}

	if (new_val != NULL && data_size > 0)
	{
		UB_LOG(UBL_INFO, "%s Updating %s\n", __func__, ieee802_dot1ab_lldp_get_string(k));
		int ret = YDBI_SET_ITEM(abknvkn, 
					kn_if_id, 
					sizeof(kn_if_id)/sizeof(uint8_t), 
					&dummy_attr,
					0, 
					input_cmd.status, //YDBI_STATUS,
					new_val,
					data_size,
					YDBI_PUSH_NOTICE);
		UB_LOG(UBL_INFO, "%s Updating %s done %d \n", __func__, ieee802_dot1ab_lldp_get_string(k), ret);
	}
}

static void check_and_update_local_system_data()
{
	if (input_cmd.configure_chassis_subtype && changed_yang_lldp_cfg.local_system_data.chassis_id_subtype != yang_lldp_cfg.local_system_data.chassis_id_subtype)
	{
		UB_LOG(UBL_INFO, "chassis_id_subtype changed from %d -> %d\n",yang_lldp_cfg.local_system_data.chassis_id_subtype, changed_yang_lldp_cfg.local_system_data.chassis_id_subtype);
		update_local_system_data(IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE);
	}

	if (input_cmd.configure_chassis_id && memcmp (changed_yang_lldp_cfg.local_system_data.chassis_id, yang_lldp_cfg.local_system_data.chassis_id, sizeof(yang_lldp_cfg.local_system_data.chassis_id)) != 0 )
	{
		UB_LOG(UBL_INFO, "updating chassis_id %s -> %s\n", yang_lldp_cfg.local_system_data.chassis_id, changed_yang_lldp_cfg.local_system_data.chassis_id);
		update_local_system_data(IEEE802_DOT1AB_LLDP_CHASSIS_ID);
	}

	if (input_cmd.configure_system_name &&  memcmp (changed_yang_lldp_cfg.local_system_data.system_name, yang_lldp_cfg.local_system_data.system_name, sizeof(yang_lldp_cfg.local_system_data.system_name)) != 0 )
	{
		UB_LOG(UBL_INFO, "updating system_name %s -> %s\n", yang_lldp_cfg.local_system_data.system_name, changed_yang_lldp_cfg.local_system_data.system_name);
		update_local_system_data(IEEE802_DOT1AB_LLDP_SYSTEM_NAME);
		UB_LOG(UBL_INFO, "updating system_name done\n");
	}

	if (input_cmd.configure_system_desc &&  memcmp (changed_yang_lldp_cfg.local_system_data.system_description, yang_lldp_cfg.local_system_data.system_description, sizeof(yang_lldp_cfg.local_system_data.system_description)) != 0 )
	{
		UB_LOG(UBL_INFO, "updating system_description %s -> %s\n", yang_lldp_cfg.local_system_data.system_description, changed_yang_lldp_cfg.local_system_data.system_description);
		update_local_system_data(IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION);
	}

	if (input_cmd.configure_system_cap_enabled &&  changed_yang_lldp_cfg.local_system_data.system_capabilities_enabled != yang_lldp_cfg.local_system_data.system_capabilities_enabled)
	{
		UB_LOG(UBL_INFO, "updating system_capabilities_enabled %x -> %x\n", yang_lldp_cfg.local_system_data.system_capabilities_enabled, changed_yang_lldp_cfg.local_system_data.system_capabilities_enabled);
		update_local_system_data(IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED);
	}

	if (input_cmd.configure_system_cap_supported &&  changed_yang_lldp_cfg.local_system_data.system_capabilities_supported != yang_lldp_cfg.local_system_data.system_capabilities_supported)
	{
		UB_LOG(UBL_INFO, "updating system_capabilities_supported %x -> %x\n", yang_lldp_cfg.local_system_data.system_capabilities_supported, changed_yang_lldp_cfg.local_system_data.system_capabilities_supported);
		update_local_system_data(IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED);
	}

	if (input_cmd.configure_message_fast_tx &&  changed_yang_lldp_cfg.message_fast_tx != yang_lldp_cfg.message_fast_tx)
	{
		UB_LOG(UBL_INFO, "updating message_fast_tx %d -> %d\n", yang_lldp_cfg.message_fast_tx, changed_yang_lldp_cfg.message_fast_tx);
		update_global_config(IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX);
	}

	if (input_cmd.configure_message_tx_hold_multiplier &&  changed_yang_lldp_cfg.message_tx_hold_multiplier != yang_lldp_cfg.message_tx_hold_multiplier)
	{
		UB_LOG(UBL_INFO, "updating message_tx_hold_multiplier %d -> %d\n", yang_lldp_cfg.message_tx_hold_multiplier, changed_yang_lldp_cfg.message_tx_hold_multiplier);
		update_global_config(IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER);
	}

	if (input_cmd.configure_message_tx_interval &&  changed_yang_lldp_cfg.message_tx_interval != yang_lldp_cfg.message_tx_interval)
	{
		UB_LOG(UBL_INFO, "updating message_tx_interval %d -> %d\n", yang_lldp_cfg.message_tx_interval, changed_yang_lldp_cfg.message_tx_interval);
		update_global_config(IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL);
	}

	if (input_cmd.configure_reinit_delay &&  changed_yang_lldp_cfg.reinit_delay != yang_lldp_cfg.reinit_delay)
	{
		UB_LOG(UBL_INFO, "updating reinit_delay %d -> %d\n", yang_lldp_cfg.reinit_delay, changed_yang_lldp_cfg.reinit_delay);
		update_global_config(IEEE802_DOT1AB_LLDP_REINIT_DELAY);
	}

	if (input_cmd.configure_tx_credit_max && changed_yang_lldp_cfg.tx_credit_max != yang_lldp_cfg.tx_credit_max)
	{
		UB_LOG(UBL_INFO, "updating tx_credit_max %d -> %d\n", yang_lldp_cfg.tx_credit_max, changed_yang_lldp_cfg.tx_credit_max);
		update_global_config(IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX);
	}

	if (input_cmd.configure_tx_fast_init && changed_yang_lldp_cfg.tx_fast_init != yang_lldp_cfg.tx_fast_init)
	{
		UB_LOG(UBL_INFO, "updating tx_fast_init %d -> %d\n", yang_lldp_cfg.tx_fast_init, changed_yang_lldp_cfg.tx_fast_init);
		update_global_config(IEEE802_DOT1AB_LLDP_TX_FAST_INIT);
	}

	uc_dbal_releasedb(ydbi_access_handle()->dbald);
}

static int print_usage(char *pname)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-P|--dbname: db file\n");
	
	UB_CONSOLE_PRINT("Reading information:\n");
	UB_CONSOLE_PRINT("-a|--show-neighbors: 'port-name' or 'all'\n");
	UB_CONSOLE_PRINT("-l|--show-statistics: 'port-name' or 'all'\n");

	// Per system
	UB_CONSOLE_PRINT("Per System Configuration:\n");
	UB_CONSOLE_PRINT("-c|--configure-chassis-subtype: subtype. \n");
	UB_CONSOLE_PRINT("-d|--configure-chassis-id: chassis id. \n");
	UB_CONSOLE_PRINT("-e|--configure-system-name: name\n");
	UB_CONSOLE_PRINT("-f|--configure-system-desc: desc \n");
	UB_CONSOLE_PRINT("-g|--configure-system-cap-enabled: caps \n");
	UB_CONSOLE_PRINT("-i|--configure-system-cap-supported: caps \n");
	UB_CONSOLE_PRINT("-m|--configure-message-fast-tx: val \n");
	UB_CONSOLE_PRINT("-o|--configure-message-tx-hold-multiplier: val \n");
	UB_CONSOLE_PRINT("-p|--configure-message-tx-interval: val \n");
	UB_CONSOLE_PRINT("-q|--configure-reinit-delay: val \n");
	UB_CONSOLE_PRINT("-r|--configure-tx-credit-max: val \n");
	UB_CONSOLE_PRINT("-s|--configure-tx-fast-init: val \n");

	// Per port configure
	UB_CONSOLE_PRINT("Per Port Configuration:\n");
	UB_CONSOLE_PRINT("-n|--configure-port: 'port-name' Set this flag to enable port change\n");
	UB_CONSOLE_PRINT("-D|--configure-dest-mac: 'dest-mac' together with port-name as pair of keys \n");
	UB_CONSOLE_PRINT("-k|--configure-admin-status: status. Need to have --configure-port 'name' to enable the change\n");
	UB_CONSOLE_PRINT("-b|--configure-tlvs-tx-enable: value. Need to have --configure-port 'name' to enable the change\n");
	UB_CONSOLE_PRINT("-t|--configure-port-id-subtype: subtype-id. Need to have --configure-port 'name' to enable the change\n");
	UB_CONSOLE_PRINT("-u|--configure-port-id: id. Need to have --configure-port 'name' to enable the change\n");
	UB_CONSOLE_PRINT("-v|--configure-port-desc: desc. Need to have --configure-port 'name' to enable the change\n");
	
	UB_CONSOLE_PRINT("\tPer port/management-address-tx-port configuration:\n");
	UB_CONSOLE_PRINT("\t(Need to have --configure-port 'name' to enable the change)\n");
	UB_CONSOLE_PRINT("-x|--configure-addr-subtype: ipv4, ipv6. Need to have --configure-port 'name' to enable the change\n");
	UB_CONSOLE_PRINT("-y|--configure-man-addr: ip. Need to have --configure-port 'name' to enable the change\n");
	UB_CONSOLE_PRINT("-z|--configure-tx-enable: yes|no. Need to have --configure-addr-subtype and --configure-man-addr to enable the change\n");
	UB_CONSOLE_PRINT("-w|--configure-if-subtype: val. Need to have --configure-addr-subtype and --configure-man-addr to enable the change\n");
	UB_CONSOLE_PRINT("-j|--configure-if-id: id. Need to have --configure-addr-subtype and --configure-man-addr to enable the change\n");
	
	return -1;
}

uint8_t mgmt_addr_key_counter = 0; // Should be '2' to start updating
static int set_options(int argc, char *argv[])
{
	int oc;
	int res=0;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"dbname", required_argument, 0, 'P'},
		// reading cmd
		{"show-neighbors", required_argument, 0, 'a'},
		{"show-statistics", required_argument, 0, 'l'},
		// Configure per-system variables
		{"configure-chassis-subtype", required_argument, 0, 'c'},
		{"configure-chassis-id", required_argument, 0, 'd'},
		{"configure-system-name", required_argument, 0, 'e'},
		{"configure-system-desc", required_argument, 0, 'f'},
		{"configure-system-cap-enabled", required_argument, 0, 'g'},
		{"configure-system-cap-supported", required_argument, 0, 'i'},
		{"configure-message-fast-tx",  required_argument, 0, 'm'},
		{"configure-message-tx-hold-multiplier",  required_argument, 0, 'o'},
		{"configure-message-tx-interval",  required_argument, 0, 'p'},
		{"configure-reinit-delay",  required_argument, 0, 'q'},
		{"configure-tx-credit-max",  required_argument, 0, 'r'},
		{"configure-tx-fast-init",  required_argument, 0, 's'},
		// Configure per-port variables
		// Notes: message-fast-tx,.., tx-fast-init per port are not used. Instead, global variables will be used.
		{"configure-port", required_argument, 0, 'n'}, // require interface name: eth0, enp2s0,..
		{"configure-dest-mac", required_argument, 0, 'D'}, // dest mac XX-XX-XX-XX-XX-XX
		{"configure-admin-status", required_argument, 0, 'k'},
		{"configure-tlvs-tx-enable", required_argument, 0, 'b'},
		{"configure-port-id-subtype", required_argument, 0, 't'},
		{"configure-port-id", required_argument, 0, 'u'},
		{"configure-port-desc", required_argument, 0, 'v'},
		// Per port[name]/man-addr-tx-port
		{"configure-addr-subtype", required_argument, 0, 'x'},		// work with configure-man-addr to configure man-addr-tx-port info
		{"configure-man-addr", required_argument, 0, 'y'},			// work with configure-addr-subtype to configure man-addr-tx-port info
		{"configure-tx-enable", required_argument, 0, 'z'},			// child of management-address-tx-port
		{"configure-if-subtype", required_argument, 0, 'w'},		// child of management-address-tx-port
		{"configure-if-id", required_argument, 0, 'j'},				// child of management-address-tx-port
		{0, 0, 0, 0}
	};

	while((oc=getopt_long(argc, argv, "hla:b:c:d:e:f:g:i:j:k:n:m:o:p:q:r:s:t:u:v:x:y:z:w:P:D:", long_options, NULL))!=-1){
		switch(oc){
		case 'a':
			// show_neighbors(optarg);
			input_cmd.is_showing_neighbor = true;
			strcpy(input_cmd.interface_name, optarg);
			res = 1; // done. Do nothing after this function
			break;
		case 'P':
			lldp_db_name = optarg;
			UB_LOG(UBL_INFO, "Read DB name from %s \n", lldp_db_name);
			break;
		case 'c':
			changed_yang_lldp_cfg.local_system_data.chassis_id_subtype = strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_chassis_subtype = true; 
			break;
		case 'd':
			{
				char* new_cs_id = optarg;
				memcpy(changed_yang_lldp_cfg.local_system_data.chassis_id, new_cs_id, strlen(new_cs_id));
				input_cmd.is_configure_local_system = true;
				input_cmd.configure_chassis_id = true;
			}
			break;
		case 'e':
			{
				char* s_name = optarg;
				memset (changed_yang_lldp_cfg.local_system_data.system_name, 0, sizeof(changed_yang_lldp_cfg.local_system_data.system_name));
				memcpy(changed_yang_lldp_cfg.local_system_data.system_name, s_name, strlen(s_name));
				input_cmd.is_configure_local_system = true;
				input_cmd.configure_system_name = true;
				UB_LOG(UBL_INFO, "update system name from %s to %s \n",yang_lldp_cfg.local_system_data.system_name, changed_yang_lldp_cfg.local_system_data.system_name);
			}
			break;
		case 'f':
			{
				char* s_desc = optarg;
				memcpy(changed_yang_lldp_cfg.local_system_data.system_description, s_desc, strlen(s_desc));
				input_cmd.is_configure_local_system = true;
				input_cmd.configure_system_desc = true;
			}
			break;
		case 'g':
			changed_yang_lldp_cfg.local_system_data.system_capabilities_enabled = (uint16_t) strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_system_cap_enabled = true;
			break;
		case 'h':
			print_usage(argv[0]);
			res=2; // make it different with read statistic and read neighbor
			break;
		case 'i':
			changed_yang_lldp_cfg.local_system_data.system_capabilities_supported = (uint16_t) strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_system_cap_supported = true;
			break;
		case 'n':
			UB_LOG(UBL_INFO, "configure port %s \n", optarg);
			input_cmd.is_configure_port = true;
			memcpy(changed_port_info.name, optarg, strlen(optarg));
			break;
		case 'D':
			{
				ub_macaddr_t dmac = {0,0,0,0,0,0};
				// From amendment spec: MAC Addr pattern "[0-9a-fA-F]{2}(-[0-9a-fA-F]{2}){5}";
				ub_hexstr2barray((const char*)optarg, dmac, '-', 6);
				input_cmd.is_configure_dest_mac = true;
				memcpy(changed_port_info.dest_mac_address, dmac, 6);
				UB_LOG(UBL_INFO, "configure dest mac %s \n", optarg);
			}
			break;
		case 'k':
			UB_LOG(UBL_INFO, "admin status %d \n", (admin_status_t) strtol(optarg, NULL, 0));
			changed_port_info.admin_status = (admin_status_t) strtol(optarg, NULL, 0);
			input_cmd.configure_admin_status = true;
			input_cmd.status = false; // RW
			break;
		case 'b':
			changed_port_info.tlvs_tx_enable = (uint8_t) strtol(optarg, NULL, 0);
			input_cmd.configure_tlvs_tx_enable = true;
			input_cmd.status = false; // RW
			break;
		case 't':
			changed_port_info.port_id_subtype = (port_id_type_t) strtol(optarg, NULL, 0);
			input_cmd.configure_port_id_subtype = true;
			break;
		case 'u':
			{
				char* port_id = optarg;
				memset (changed_port_info.port_id, 0, sizeof(changed_port_info.port_id));
				memcpy(changed_port_info.port_id, port_id, strlen(port_id));
				input_cmd.configure_port_id = true;
			}
			break;
		case 'v':
			{
				char* port_desc = optarg;
				memset (changed_port_info.port_desc, 0, sizeof(changed_port_info.port_desc));
				memcpy(changed_port_info.port_desc, port_desc, strlen(port_desc));
				input_cmd.configure_port_desc = true;
			}
			break;
		case 'x':
			{
				char* addr_subtype = optarg;
				if (strcmp(addr_subtype, "ipv4") == 0) changed_mgmt_tx_port.address_subtype = ipv4;
				else if (strcmp(addr_subtype, "ipv6") == 0) changed_mgmt_tx_port.address_subtype = ipv6;
				else changed_mgmt_tx_port.address_subtype = nsap;

				input_cmd.configure_addr_subtype = true;
			}
			break;
		case 'y':
			{
				char* addr = optarg;
				memset (changed_mgmt_tx_port.man_address, 0, sizeof(changed_mgmt_tx_port.man_address));
				memcpy(changed_mgmt_tx_port.man_address, addr, strlen(addr));
				input_cmd.configure_man_addr = true;
			}
			break;
		case 'z':
			changed_mgmt_tx_port.tx_enable = (strcmp("yes", optarg) == 0) ? true : false;
			input_cmd.configure_tx_enable = true;
			input_cmd.status = false; // RW
			break;
		case 'w':
			changed_mgmt_tx_port.if_subtype = strtol(optarg, NULL, 0);
			input_cmd.configure_if_subtype = true;
			break;
		case 'j':
			changed_mgmt_tx_port.if_id = strtol(optarg, NULL, 0);
			input_cmd.configure_if_id = true;
			break;
		case 'l':
			// show_stats(optarg);
			input_cmd.is_showing_statistic = true;
			strcpy(input_cmd.interface_name, optarg);
			res = 1;
			break;
		case 'm':
			changed_yang_lldp_cfg.message_fast_tx = strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_message_fast_tx = true;
			input_cmd.status = false; // RW
			break;
		case 'o':
			changed_yang_lldp_cfg.message_tx_hold_multiplier = strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_message_tx_hold_multiplier = true;
			input_cmd.status = false; // RW
			break;
		case 'p':
			changed_yang_lldp_cfg.message_tx_interval = strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_message_tx_interval = true;
			input_cmd.status = false; // RW
			break;
		case 'q':
			changed_yang_lldp_cfg.reinit_delay = strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_reinit_delay = true;
			input_cmd.status = false; // RW
			break;
		case 'r':
			changed_yang_lldp_cfg.tx_credit_max = strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_tx_credit_max = true;
			input_cmd.status = false; // RW
			break;
		case 's':
			changed_yang_lldp_cfg.tx_fast_init = strtol(optarg, NULL, 0);
			input_cmd.is_configure_local_system = true;
			input_cmd.configure_tx_fast_init = true;
			input_cmd.status = false; // RW
			break;
		default:
			return -1;
		}
	}
	return res;
}

void init_default_cfg()
{
	memset(&yang_lldp_cfg, 0, sizeof(yang_lldp_t));
	ub_list_init(&yang_lldp_cfg.ports);
}

void init_db(const char* dbfile)
{
	uc_dbald *dbald = NULL;
	uc_notice_data_t *ucntd=NULL;
	xl4_data_data_t *xdd = NULL;
	UB_LOG(UBL_INFO, "%s 1\n", __func__);

	// Init
	dbald=uc_dbal_open(dbfile, "w", 0);
	ucntd=uc_notice_init(0, dbfile);
	xdd =xl4_data_init(dbald);

	ydbi_access_init(dbald, xdd, ucntd);
	UB_LOG(UBL_INFO, "%s 4\n", __func__);

	int   ret = ydbi_load_lldp_db(ydbi_access_handle(), &yang_lldp_cfg);
	if (ret == 0)
	{
		UB_LOG(UBL_INFO, "%s: loaded yang db\n", __func__);
	}

}

void deinitdb()
{
	yang_db_item_access_t *ydbia = ydbi_access_handle();
	// deregister_notice(ydbia->dbald, ydbia->ucntd);
	uc_notice_close(ydbia->ucntd, 0);
	xl4_data_close(ydbia->xdd);
	uc_dbal_close(ydbia->dbald, (0));
}

int main(int argc, char *argv[]) 
{
	signal(SIGINT, handle_signal);

	unibase_init_para_t init_para;
	/* initialized excelfore's unibase library */
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:66,ucclt:66,lldpcli:46", "UBL_LLDPCLI");
	unibase_init(&init_para);

	UB_LOG(UBL_INFO, "LLDP Client started\n");
	memset(&input_cmd, 0, sizeof(input_cmd));
	input_cmd.status = true; // DEFAULT is RO.
	int res = set_options(argc, argv);
	if ( res == 0)
	{
		init_default_cfg();
		initialize_cfg(lldp_db_name, &yang_lldp_cfg, false);

		if (input_cmd.is_configure_local_system == true && input_cmd.is_configure_port == false && input_cmd.is_configure_dest_mac == false)
		{
			UB_LOG(UBL_INFO, "Got update local system data\n");
			check_and_update_local_system_data();
		}

		if (input_cmd.is_configure_port == true && input_cmd.is_configure_dest_mac == true)
		{
			UB_LOG(UBL_INFO, "Got update port data\n");
			check_and_update_port();
		}
	}
	else if (res == 1) // reading info
	{
		init_default_cfg();
		initialize_cfg(lldp_db_name, &yang_lldp_cfg, false);

		if (input_cmd.is_showing_neighbor)
		{
			show_neighbors(input_cmd.interface_name);
		}
		if (input_cmd.is_showing_statistic)
		{
			show_stats(input_cmd.interface_name);
		}
	}
	else{/*Do nothing. Print help or error*/}

	deinitdb();
	cleanup_cfg(&yang_lldp_cfg);
}