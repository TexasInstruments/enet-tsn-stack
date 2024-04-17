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
 * tlv_data.c
 *
 *  Created on: Jul 3, 2023
 *      Author: hoangloc
 */


#include "tlv_data.h"
#include <tsn_unibase/unibase_macros.h>
#include <tsn_unibase/ub_strutils.h>
#include <tsn_combase/combase.h>
#include "lldp_utils.h"

#define MAX_TLV_BUF (1500)
#define TLV_ID_HEADER_MASK			(0xFE00)
#define TLV_LEN_HEADER_MASK			(0x01FF)
#define CHASSIS_ID_SUBTYPE_SZ		(1)
#define PORT_ID_SUBTYPE_SZ			(1)
#define IPV4_SIZE					(4)
#define IPV6_SIZE					(16)
#define TLV_TTL_LENTH				(2)
#define END_OF_TLV					(0x0000)
#define MAX_CHASSIS_INFO_STRING_LEN	LLDP_REMOTE_INFO_STRING_MAX_LEN
#define MAX_PORT_INFO_STRING_LEN	LLDP_REMOTE_INFO_STRING_MAX_LEN

static bool is_tx_enabled_for_tlv(lldp_port_t* cfg_port, LLDP_TLV tlv);
static int get_port_info(uint8_t* buf, remote_systems_data_t* neighbor_info, int* read_index);
static int get_chassis_info(uint8_t* buf, remote_systems_data_t* neighbor_info, int *read_index);
static LLDP_RX_TYPE collect_other_tlv_infos(uint8_t* buf, int read_index, remote_systems_data_t* neighbor_info, int len);
static uint8_t get_tlv_type(uint8_t tlv_header);
static uint16_t get_tlv_length(uint8_t tlv_header1, uint8_t tlv_header2);
static uint16_t gen_tlv_header_info(LLDP_TLV tlv, uint16_t length);

/// @brief Fill chassis information by subtype, since for each subtype, the length and the data format can be change (MAC or string)
/// @param lldp_cfg 
/// @param buffer 
static void fill_chassis_by_type(yang_lldp_t* lldp_cfg, uint8_t* buffer)
{
	switch(lldp_cfg->local_system_data.chassis_id_subtype)
		{
			case C_CHASSIS_COMPONENT:   // EntPhysicalAlias: String type IETF RFC 6933
			case C_INTERFACE_ALIAS:     // ifAlias DisplayString IETF RFC 2863
			case C_PORT_COMPONENT:      // EntPhysicalAlias: String type IETF RFC 6933
			case C_INTERFACE_NAME:      // ifName DisplayString IETF RFC 2863
			case C_LOCAL:               // local is an alpha-numeric string and is locally assigned
			case C_NETWORK_ADDRESS:     // networkAddress is an octet string
				memcpy(buffer, lldp_cfg->local_system_data.chassis_id, strlen( (char*) lldp_cfg->local_system_data.chassis_id));
				break;
			case C_MAC_ADDRESS: // MAC: 6 bytes
				{
					ub_macaddr_t bmac = {0,0,0,0,0,0};
					// From amendment spec: MAC Addr pattern "[0-9a-fA-F]{2}(-[0-9a-fA-F]{2}){5}";
					int i = ub_hexstr2barray((const char*)lldp_cfg->local_system_data.chassis_id, bmac, '-', 6);
					if(i<6)
					{
						for(int j=i;j<6;j++){bmac[j]=0;}
					}
					memcpy(buffer, bmac, 6);
				}
				break;
			default:
				break;
		}
}

static uint32_t byteswap32(uint32_t x)
{
	uint32_t y = (x >> 24) & 0xff;
	y |= (x >> 8) & 0xff00;
	y |= (x << 8) & 0xff0000;
	y |= (x << 24) & 0xff00000u;

	return y;
}

static uint32_t byteswap24(uint32_t x)
{
	uint32_t y = byteswap32(x);
	y = ((y >> 8) & 0x00ffffff);

	return y;
}

/// @brief Fill port information by subtype, since for each subtype, the length and the data format can be change (MAC or string)
/// @param cfg_port 
/// @param buffer 
static void fill_port_id_by_type(lldp_port_t* cfg_port, uint8_t* buffer)
{
	switch(cfg_port->port_id_subtype)
	{
		case P_INTERFACE_ALIAS:     // ifAlias DisplayString IETF RFC 2863
		case P_PORT_COMPONENT:      // EntPhysicalAlias: String type IETF RFC 6933
		case P_INTERFACE_NAME:      // ifName DisplayString IETF RFC 2863
		case P_LOCAL:               // local is an alpha-numeric string and is locally assigned
		case P_NETWORK_ADDRESS:     // networkAddress is an octet string
			memcpy(buffer, cfg_port->port_id, strlen( (char*) cfg_port->port_id));
			break;
		case P_MAC_ADDRESS: // MAC: 6 bytes
			{
				ub_macaddr_t bmac = {0,0,0,0,0,0};
				// From amendment spec: MAC Addr pattern "[0-9a-fA-F]{2}(-[0-9a-fA-F]{2}){5}";
				int i = ub_hexstr2barray((const char*)cfg_port->port_id, bmac, '-', 6);
				if(i<6)
				{
					for(int j=i;j<6;j++){bmac[j]=0;}
				}

				memcpy(buffer, bmac, 6);
			}
			break;
		default:
			break;
	}
}

/// @brief Only local tlv which has corresponding bit of tlvs-tx-enable set to '1' can be sent out
/// @param cfg_port input which store tlvs-tx-enable
/// @param tlv accept port-desc, sys-name, sys-desc, sys-cap
/// @return true/false
static bool is_tx_enabled_for_tlv(lldp_port_t* cfg_port, LLDP_TLV tlv)
{
	bool ret = false;
	uint8_t tlvs_tx_enable = cfg_port->tlvs_tx_enable;
	uint8_t enabled = 0;
	switch (tlv)
	{
		case PORT_DESCRIPTION:              // bit port-desc position 0;
			enabled = 0x01 & tlvs_tx_enable;
			UB_LOG(UBL_DEBUG, "%s: PORT_DESCRIPTION[%s]\n", __func__, (enabled==1)?"enabled":"disabled");
			break;
		case SYSTEM_NAME:
			enabled = 0x01 & (tlvs_tx_enable >> 1); // bit sys-name position 1;
			UB_LOG(UBL_DEBUG, "%s: SYSTEM_NAME[%s]\n", __func__, (enabled==1)?"enabled":"disabled");
			break;
		case SYSTEM_DESCRIPTION:  // bit sys-desc position 2;
			enabled = 0x01 & (tlvs_tx_enable >> 2); // bit sys-name position 1;
			UB_LOG(UBL_DEBUG, "%s: SYSTEM_DESCRIPTION[%s]\n", __func__, (enabled==1)?"enabled":"disabled");
			break;
		case SYSTEM_CAPABILITIES: // bit sys-cap position 3;
			enabled = 0x01 & (tlvs_tx_enable >> 3);
			UB_LOG(UBL_DEBUG, "%s: SYSTEM_CAPABILITIES[%s]\n", __func__, (enabled==1)?"enabled":"disabled");
			break;
		default:
			// No need to check others
			enabled=1; // In case of mgmtaddr, there is tx-enable flag inside of mgmt-addr-tx.
			break;
	}
	if (enabled) ret = true;
	return ret;
}

/// @brief For each mgmt addr if a port -> call to this function
/// @param send_buf 
/// @param set_idx 
/// @param mgmt_addr_tx_info 
static void fill_local_mgmt_addr_info(uint8_t* send_buf, int* set_idx, management_address_tx_port_t* mgmt_addr_tx_info)
{
	uint16_t tlv_info = (((uint16_t)MANAGEMENT_ADDRESS) << 9) & TLV_ID_HEADER_MASK;
	uint16_t tlv_len = 0;
	uint8_t mgmt_addr_str_len = 0;
	uint8_t ipv4_hex [4]= {0,0,0,0};
	uint8_t ipv6_hex [16]= {0};

	switch(mgmt_addr_tx_info->address_subtype)
	{
		case ipv4:
		mgmt_addr_str_len = 1/*size of mgmt address subtype is 1*/+ IPV4_SIZE/*size of ipv4*/;
		ipv4_str_to_hex((const char*)mgmt_addr_tx_info->man_address, ipv4_hex);
		UB_LOG(UBL_DEBUG, "%s:%s \n", __func__, mgmt_addr_tx_info->man_address);
		// ub_hexdump(true, true, ipv4_hex, 4, 0);
		break;
		case ipv6:
		mgmt_addr_str_len = 1/*size of mgmt address subtype is 1*/+ IPV6_SIZE/*size of ipv6*/;
		ipv6_str_to_hex((const char*)mgmt_addr_tx_info->man_address, ipv6_hex);
		UB_LOG(UBL_DEBUG, "%s:%s \n", __func__, mgmt_addr_tx_info->man_address);
		// ub_hexdump(true, true, ipv6_hex, 16, 0);
		break;
		case nsap:
		mgmt_addr_str_len = 1/*size of mgmt address subtype is 1*/ + strlen((char*)mgmt_addr_tx_info->man_address);
		break;
	}
	tlv_len += sizeof(mgmt_addr_str_len)/*size of mgmt address string len is 1*/ ;
	tlv_len += mgmt_addr_str_len;
	tlv_len += 5; // interface number (4) and interface number subtype (1)
	tlv_len += 1; // OID len. TODO: Should we support OID?
	tlv_info |= (tlv_len & TLV_LEN_HEADER_MASK);
	tlv_info = htons(tlv_info);
	memcpy(&send_buf[*set_idx], (uint8_t*)&tlv_info, sizeof(tlv_info)); *set_idx+=sizeof(tlv_info);
	send_buf[*set_idx] = mgmt_addr_str_len; *set_idx+=sizeof(mgmt_addr_str_len);
	send_buf[*set_idx] = mgmt_addr_tx_info->address_subtype; *set_idx+=1;
	
	switch(mgmt_addr_tx_info->address_subtype)
	{
		case ipv4:
			memcpy(&send_buf[*set_idx], ipv4_hex, 4);*set_idx+=4;
			break;
		case ipv6:
			memcpy(&send_buf[*set_idx], ipv6_hex, 16);*set_idx+=16;
			break;
		case nsap:
			memcpy(&send_buf[*set_idx], mgmt_addr_tx_info->man_address, strlen((char*)mgmt_addr_tx_info->man_address));
			*set_idx+=strlen((char*)mgmt_addr_tx_info->man_address);
		break;
	}
	send_buf[*set_idx] = mgmt_addr_tx_info->if_subtype;*set_idx+=1;
	
	uint32_t if_id = htonl(mgmt_addr_tx_info->if_id);
	memcpy(&send_buf[*set_idx], (uint8_t*) &if_id, sizeof(mgmt_addr_tx_info->if_id)); *set_idx+=4;
	send_buf[*set_idx] = 0;*set_idx+=1; //TODO: OID len = 0.

	// ub_hexdump(true, true, &send_buf[init_index], tlv_len + 2, 0);
}
/// @brief Fill other info to buffer. Caller need to check is_tx_enabled_for_tlv before call to this function
/// @param lldp_cfg 
/// @param cfg_port 
/// @param tlv 
/// @param send_buf 
/// @param set_idx 
static void fill_other_info(yang_lldp_t* lldp_cfg, lldp_port_t* cfg_port, LLDP_TLV tlv, uint8_t* send_buf, int* set_idx)
{
	if (is_tx_enabled_for_tlv(cfg_port, tlv))
	{
		uint16_t tlv_len = 0;
		switch (tlv)
		{
			case PORT_DESCRIPTION:
				tlv_len = strlen(cfg_port->port_desc);
				memcpy(&send_buf[*set_idx + 2], cfg_port->port_desc, tlv_len);
				break;
			case SYSTEM_NAME:
				tlv_len = strlen(lldp_cfg->local_system_data.system_name);
				memcpy(&send_buf[*set_idx + 2], lldp_cfg->local_system_data.system_name, tlv_len);
				break;
			case SYSTEM_DESCRIPTION:
				tlv_len = strlen(lldp_cfg->local_system_data.system_description);
				memcpy(&send_buf[*set_idx + 2], lldp_cfg->local_system_data.system_description, tlv_len);
				break;
			case SYSTEM_CAPABILITIES:
				{
				tlv_len = 4;
				uint16_t system_cap = htons(lldp_cfg->local_system_data.system_capabilities_supported);
				uint16_t enabled_cap = htons(lldp_cfg->local_system_data.system_capabilities_enabled);
				memcpy(&send_buf[*set_idx + 2], (uint8_t*)&system_cap, 2);
				memcpy(&send_buf[*set_idx + 4], (uint8_t*)&enabled_cap, 2);
				}
				break;
			case MANAGEMENT_ADDRESS:
				{
					struct ub_list_node *tmp_node;
					for(UB_LIST_FOREACH_ITER(&cfg_port->management_address_tx_port, tmp_node))
					{
						management_address_tx_port_t* management_addr_tx_port = (management_address_tx_port_t*)tmp_node;
						if (management_addr_tx_port->tx_enable)
						{
							fill_local_mgmt_addr_info(send_buf, set_idx, management_addr_tx_port);
							UB_LOG(UBL_DEBUG, "%s: fill mgmt addr :%s\n", __func__, management_addr_tx_port->man_address);
						}
					}
				}
				break;
			default:
				// No need to check others
				break;
		}
		
		if (MANAGEMENT_ADDRESS != tlv) // MANAGEMENT_ADDRESS already filled in the loop
		{
			uint16_t tlv_info = gen_tlv_header_info(tlv, tlv_len);
			memcpy(&send_buf[*set_idx], (uint8_t*)&tlv_info, sizeof(tlv_info));

			*set_idx += sizeof(tlv_info);
			*set_idx += tlv_len;
		}
	}
	else { /*Do nothing*/}
}

/// @brief Gen TLV header base on tlv type and it's length
/// @param tlv 
/// @param length 
/// @return 
static uint16_t gen_tlv_header_info(LLDP_TLV tlv, uint16_t length)
{
	uint16_t tlv_info = 0x0000;
	tlv_info = (tlv << 9) & TLV_ID_HEADER_MASK;
	tlv_info |= (length & TLV_LEN_HEADER_MASK);
	tlv_info = htons(tlv_info);

	return tlv_info;
}

static int fill_mandatory_info(yang_lldp_t* lldp_cfg, lldp_port_t* cfg_port, uint8_t* send_buf, bool is_shutdown)
{
	int set_idx = 0;

	// lldp chassis
	uint16_t chassis_info_len = (C_MAC_ADDRESS != lldp_cfg->local_system_data.chassis_id_subtype) 
								? strlen((char*)lldp_cfg->local_system_data.chassis_id) + CHASSIS_ID_SUBTYPE_SZ 
								: sizeof(ub_macaddr_t) + CHASSIS_ID_SUBTYPE_SZ;
	uint16_t chasiss_info = gen_tlv_header_info(CHASSIS_ID, chassis_info_len);

	memcpy(&send_buf[set_idx], (uint8_t*)&chasiss_info, sizeof(chasiss_info)); set_idx +=sizeof(chasiss_info);
	send_buf[set_idx] = lldp_cfg->local_system_data.chassis_id_subtype; set_idx +=CHASSIS_ID_SUBTYPE_SZ;
	fill_chassis_by_type(lldp_cfg, &send_buf[set_idx]); set_idx+=chassis_info_len - CHASSIS_ID_SUBTYPE_SZ;

	// port id
	uint16_t portid_info_len = (P_MAC_ADDRESS != cfg_port->port_id_subtype) 
								? strlen((char*)cfg_port->port_id) + PORT_ID_SUBTYPE_SZ 
								: sizeof(ub_macaddr_t) + PORT_ID_SUBTYPE_SZ;
	uint16_t portid_info = gen_tlv_header_info(PORT_ID, portid_info_len);
	memcpy(&send_buf[set_idx], (uint8_t*)&portid_info, sizeof(portid_info)); set_idx +=sizeof(portid_info);
	send_buf[set_idx] = cfg_port->port_id_subtype; set_idx +=PORT_ID_SUBTYPE_SZ;
	fill_port_id_by_type(cfg_port, &send_buf[set_idx]); set_idx+=portid_info_len - PORT_ID_SUBTYPE_SZ;

	// time to live: Shutdown frame: ttl = 0
	uint16_t tx_ttl = (is_shutdown) ? 0 : UB_MIN(cfg_port->message_tx_interval * cfg_port->message_tx_hold_multiplier + 1, 65535);
	uint16_t ttl_info = gen_tlv_header_info(TIME_TO_LIVE, TLV_TTL_LENTH);

	memcpy(&send_buf[set_idx], (uint8_t*)&ttl_info, sizeof(ttl_info)); set_idx +=sizeof(ttl_info);
	tx_ttl = htons(tx_ttl);
	memcpy(&send_buf[set_idx], (uint8_t*)&tx_ttl, sizeof(tx_ttl)); set_idx +=sizeof(tx_ttl);

	return set_idx;
}

int lldpdu_builder(yang_lldp_t* lldp_cfg, uint8_t* send_buf, lldp_port_t* cfg_port)
{
	int set_idx = 0;

	set_idx = fill_mandatory_info(lldp_cfg, cfg_port, send_buf, false);

	//// Other info
	fill_other_info(lldp_cfg, cfg_port, PORT_DESCRIPTION, send_buf, &set_idx);
	fill_other_info(lldp_cfg, cfg_port, SYSTEM_NAME, send_buf, &set_idx);
	fill_other_info(lldp_cfg, cfg_port, SYSTEM_DESCRIPTION, send_buf, &set_idx);
	fill_other_info(lldp_cfg, cfg_port, SYSTEM_CAPABILITIES, send_buf, &set_idx);
	fill_other_info(lldp_cfg, cfg_port, MANAGEMENT_ADDRESS, send_buf, &set_idx);
	//// End

	// End-of-tlv
	uint16_t end_of_tlv = END_OF_TLV;
	memcpy(&send_buf[set_idx], (uint8_t*)&end_of_tlv, sizeof(end_of_tlv)); set_idx +=sizeof(end_of_tlv);
	
	UB_LOG(UBL_DEBUG, "%s:%s done tlv len %d \n",  __func__, cfg_port->name, set_idx);
	return set_idx;
};

int tlv_shutdown_builder(yang_lldp_t* lldp_cfg, uint8_t* send_buf, lldp_port_t* cfg_port)
{
	int set_idx = 0;
	
	set_idx = fill_mandatory_info(lldp_cfg, cfg_port, send_buf, true);

	// End-of-tlv
	uint16_t end_of_tlv = END_OF_TLV;
	memcpy(&send_buf[set_idx], (uint8_t*)&end_of_tlv, sizeof(end_of_tlv)); set_idx +=sizeof(end_of_tlv);
	
	UB_LOG(UBL_DEBUG, "%s:%s done tlv len %d \n",  __func__, cfg_port->name, set_idx);
	return set_idx;
};

static uint8_t get_tlv_type(uint8_t tlv_header)
{
	// tlv type is 1st 7bits    
	uint8_t tlv_type = (tlv_header >> 1) & 0x7F;
	return tlv_type;
}

static uint16_t get_tlv_length(uint8_t tlv_header1, uint8_t tlv_header2)
{
	uint16_t length = ((uint16_t)tlv_header2 & 0x00FF);
	length |= ((((uint16_t)tlv_header1) << 8) & 0x0100);
	return length;
}


int get_chassis_info(uint8_t* buf, remote_systems_data_t* neighbor_info, int* read_index)
{
	int ret = -1;
	uint8_t chassis_id_type = get_tlv_type(buf[*read_index]);
	uint16_t chassis_len = get_tlv_length(buf[*read_index], buf[*read_index + 1]);
	
	*read_index += sizeof(uint16_t); // shift read-index to 2 bytes since we finished reading header

	if (chassis_id_type == CHASSIS_ID)
	{
		if (chassis_len >= sizeof(chassis_len) && chassis_len <= MAX_CHASSIS_INFO_STRING_LEN)
		{
			neighbor_info->chassis_id_subtype = (chassis_id_type_t)buf[*read_index];

			memcpy(neighbor_info->chassis_id, &buf[*read_index + CHASSIS_ID_SUBTYPE_SZ], chassis_len - CHASSIS_ID_SUBTYPE_SZ);
			*read_index += chassis_len;

			ret = (int)chassis_len;
			UB_LOG(UBL_DEBUG, "%s: %d[%d]\n", __func__, chassis_len, neighbor_info->chassis_id_subtype);
			// ub_hexdump(true, true, neighbor_info->chassis_id, chassis_len - 1, 0);
		}
		else
		{
			UB_LOG(UBL_ERROR, "Invalid chassis id len %d\n", chassis_len);
		}
	}
	else
	{
		UB_LOG(UBL_ERROR, "First tlv must be chassis id, but got %d\n", chassis_id_type);
	}

	return ret;
}

int get_port_info(uint8_t* buf, remote_systems_data_t* neighbor_info, int* read_index)
{
	int ret = -1;
	uint8_t port_id_type = get_tlv_type(buf[*read_index]);
	uint16_t port_id_len = get_tlv_length(buf[*read_index], buf[*read_index+1]);

	*read_index += sizeof(uint16_t); // shift read-index to 2 bytes since we finished reading header

	if (port_id_type == PORT_ID)
	{
		if (port_id_len >= sizeof(port_id_len) && port_id_len <= MAX_PORT_INFO_STRING_LEN)
		{
			neighbor_info->port_id_subtype = (port_id_type_t)buf[*read_index];
			memcpy(neighbor_info->port_id, &buf[*read_index + PORT_ID_SUBTYPE_SZ], port_id_len - PORT_ID_SUBTYPE_SZ);

			*read_index += port_id_len;

			ret = (int)port_id_len;
			UB_LOG(UBL_DEBUG, "%s: %d[%d]\n", __func__, port_id_len, neighbor_info->port_id_subtype);
			// ub_hexdump(true, true, neighbor_info->port_id, port_id_len - 1, 0);
		}
		else
		{
			UB_LOG(UBL_ERROR, "Invalid port id len %d\n", port_id_len);
		}
	}
	else
	{
		UB_LOG(UBL_ERROR, "Second tlv must be chassis id, but got %d\n", port_id_type);
	}

	return ret;
}

/// @brief Collect other than Chassis, Port, TTL and XPDUREQ or XPDU
/// @param buf 
/// @param read_index started read index from Buffer
/// @param neighbor_info Out info to be filled
/// @param len 
/// @return 
static LLDP_RX_TYPE collect_other_tlv_infos(uint8_t* buf, int read_index, remote_systems_data_t* neighbor_info, int len)
{
	LLDP_RX_TYPE ret = RX_NORMAL; // Initial is rx-normal since we validate rx-type in caller function
	bool read_done = false;
	while(read_index < len-1 && !read_done)
	{
		uint8_t tlv_type = get_tlv_type(buf[read_index]);
		uint16_t tlv_length = get_tlv_length(buf[read_index], buf[read_index+1]);
		if (tlv_length < len - read_index -1)
		{
			if (tlv_length > LLDP_REMOTE_INFO_STRING_MAX_LEN)
			{
				UB_LOG(UBL_WARN, "TLV %d dropped due to tlv length %d > MAX(%d)\n", tlv_type, tlv_length, LLDP_REMOTE_INFO_STRING_MAX_LEN);
			}
			else
			{
				switch (tlv_type)
				{
					case END_OF_TLV:
						read_done = true;
						if (tlv_length > 0) 
						{
							UB_LOG(UBL_ERROR, "Malformed. END_OF_TLV length > 0 (%d)\n", tlv_length);
							ret = RX_INVALID;
						}
						else
						{
							UB_LOG(UBL_DEBUG, "end of tlv\n");
						}
						break;
					case CHASSIS_ID:
					case PORT_ID:
					case TIME_TO_LIVE:
						read_done = true;
						ret = RX_INVALID;
						UB_LOG(UBL_INFO, "Malformed. Dupplicate tlv field %d\n", tlv_type);
						break;
					case PORT_DESCRIPTION:
						memset(neighbor_info->port_desc, 0, sizeof(neighbor_info->port_desc));
						memcpy(neighbor_info->port_desc, &buf[read_index+2], tlv_length);
						UB_LOG(UBL_DEBUG, "%s: Port desc[%s]\n", __func__, neighbor_info->port_desc);
						break;
					case SYSTEM_NAME:
						memset(neighbor_info->system_name, 0, sizeof(neighbor_info->system_name));
						memcpy(neighbor_info->system_name, &buf[read_index+2], tlv_length);
						UB_LOG(UBL_DEBUG, "%s: system name[%s]\n", __func__, neighbor_info->system_name);
						break;
					case SYSTEM_DESCRIPTION:
						memset(neighbor_info->system_description, 0, sizeof(neighbor_info->system_description));
						memcpy(neighbor_info->system_description, &buf[read_index+2], tlv_length);
						UB_LOG(UBL_DEBUG, "%s: system desc[%s]\n", __func__, neighbor_info->system_description);
						break;
					case SYSTEM_CAPABILITIES: // Length must be 4
						if (tlv_length == 4)
						{
							uint16_t system_caps = ((uint16_t)buf[read_index+2] << 8) & 0xFF00;
							system_caps |= (((uint16_t)buf[read_index+3]) & 0x00FF);
							neighbor_info->system_capabilities_supported = system_caps;

							uint16_t enabled_caps = ((uint16_t)buf[read_index+4] << 8) & 0xFF00;
							enabled_caps |= (((uint16_t)buf[read_index+5]) & 0x00FF);
							neighbor_info->system_capabilities_enabled = enabled_caps;

							UB_LOG(UBL_DEBUG, "%s: system_caps [%04x] enabled [%04x]\n", __func__, system_caps, enabled_caps);
						}
						else
						{
							UB_LOG(UBL_WARN, "Invalid system caps length\n");
							read_done = true;
							ret = RX_INVALID;
						}
						break;
					case MANAGEMENT_ADDRESS:
					{
						uint8_t management_addr[32];
						memset(management_addr, 0, sizeof(management_addr));
						uint8_t management_address_len = buf[read_index+2];
						uint8_t management_address_subtype = buf[read_index+3];
						memcpy(management_addr, &buf[read_index+4], management_address_len - 1); // -1 is len of addr subtype
						uint8_t interface_number_subtype = buf[read_index + 4 + management_address_len - 1];
						uint32_t interface_number = *((uint32_t*)(&buf[read_index + 4 + management_address_len]));

						// TODO: where could we store OID in yang structure?
						management_address_t* mgmt_addr = init_mgmt_addr();
						memset(mgmt_addr, 0, sizeof(management_address_t));
						mgmt_addr->address_subtype = (address_subtype_t)management_address_subtype;
						mgmt_addr->if_subtype = (man_addr_if_subtype_t)interface_number_subtype;
						mgmt_addr->if_id = htonl(interface_number);
						memset(mgmt_addr->address, 0, sizeof(mgmt_addr->address));
						memcpy(mgmt_addr->address, management_addr, management_address_len - 1);
						ub_list_append(&neighbor_info->management_address, (struct ub_list_node*)mgmt_addr);
						
						UB_LOG(UBL_DEBUG, "%s: management_address [subtype: %d] addr-len[%d] if [subtype: %d id: %d]\n", 
											__func__, 
											mgmt_addr->address_subtype, 
											management_address_len,
											mgmt_addr->if_subtype,
											mgmt_addr->if_id);
						// ub_hexdump(true, true, mgmt_addr->address, management_address_len - 1, 0);
					}
					break;
					case ORGANIZATION_SPECIFIC_TLV:
					{
						if(ub_list_count(&neighbor_info->remote_org_defined_info) == 0) ub_list_init(&neighbor_info->remote_org_defined_info);
						remote_org_defined_info_t* rm_org = init_rm_org_def();
						memset(rm_org, 0, sizeof(remote_org_defined_info_t));

						uint32_t oui = 0;
						memcpy(&oui, &buf[read_index+2], 3); // sizeof(OUI) is 3 bytes
						rm_org->info_identifier = byteswap24(oui);
						rm_org->info_subtype = buf[read_index+5];
						memcpy(rm_org->remote_info, &buf[read_index+6], tlv_length - 3  - 1);
						// UB_LOG(UBL_DEBUG, "%s: Original OUI ID (%x %x %x) Subtype %d \n", __func__, buf[read_index+2], buf[read_index+3], buf[read_index+4], rm_org->info_subtype);
						// UB_LOG(UBL_DEBUG, "%s: Converted OUI ID %d(%x) -> %d (%x) Subtype %d \n", __func__, oui, oui, rm_org->info_identifier, rm_org->info_identifier, rm_org->info_subtype);
						// ub_hexdump(true, true,  &buf[read_index+6], tlv_length - 3  - 1, 0);
						ub_list_append(&neighbor_info->remote_org_defined_info,  (struct ub_list_node*)rm_org);
					}
					break;
					default:
					{
						UB_LOG(UBL_WARN, "%s: Got unknown TLV type %d \n", __func__, tlv_type);
						// ub_hexdump(true, true,  &buf[read_index+2], tlv_length, 0);
						remote_unknown_tlv_t* unknown_tlv = init_rm_unknown_tlv();
						memset(unknown_tlv, 0, sizeof(remote_unknown_tlv_t));
						unknown_tlv->tlv_type = tlv_type;
						if (tlv_length <= MAX_RM_UNKNOWN_TLV_INFO_LEN)
						{
							memcpy(unknown_tlv->tlv_info, &buf[read_index+2], tlv_length);
						}
						else
						{
							UB_LOG(UBL_WARN, "%s: Unknown TLV (%d) Length too big (%d > max %d byte), ignored content!\n", __func__, tlv_type, tlv_length, MAX_RM_UNKNOWN_TLV_INFO_LEN);
						}

						if(ub_list_count(&neighbor_info->remote_unknown_tlv) == 0) ub_list_init(&neighbor_info->remote_unknown_tlv);
						ub_list_append(&neighbor_info->remote_unknown_tlv,  (struct ub_list_node*)unknown_tlv);
					}
					break;

				}
			}
			read_index+= 2;
			read_index+= tlv_length;
		}
		else
		{
			UB_LOG(UBL_ERROR, "Invalid tlv_length len %d - remain len %d\n", tlv_length, len - read_index - 1);
			read_done = true;
			ret = RX_INVALID;
		}
	}

	return ret;
}

/// @brief Collect TTL tlv, validate + detect rxtype (can be normal or shutdown, and invalid if verificaiton failed)
/// @param neighbor_info fill in TTL
/// @return 
LLDP_RX_TYPE collect_ttl_info(uint8_t* buf, int* read_idx, timeticks *rx_ttl)
{
	LLDP_RX_TYPE rx_type = RX_INVALID;
	uint16_t ttl_length = get_tlv_length(buf[*read_idx], buf[*read_idx+1]);
	if (ttl_length == TLV_TTL_LENTH)
	{
		*read_idx += TLV_TTL_LENTH;
		
		uint16_t ttl = ((uint16_t)buf[*read_idx] < 8) & 0xFF00;
		ttl |= ( ((uint16_t)buf[*read_idx + 1]) & 0x00FF);
		*rx_ttl = ttl;

		if (ttl == 0)
		{
			UB_LOG(UBL_INFO, "%s: Recv SHUTDOWN LLDPDU TTL=[%d]\n", __func__, ttl);
			rx_type = RX_SHUTDOWN;
		}
		else
		{
			// UB_LOG(UBL_DEBUG, "%s: Recv NORMAL LLDPDU TTL=[%d]\n", __func__, ttl);
			rx_type = RX_NORMAL;
		}

		*read_idx += TLV_TTL_LENTH; // total TTL TLV length is 4
	}
	else
	{
		UB_LOG(UBL_ERROR, "Invalid ttl len %d\n", ttl_length);
	}

	return rx_type;
}

/**
 * @brief Validate TLV follow Section 9.2.7.7.1
 * @param[in] buf received buffer from socket (without ethernet header)
 * @param[in] len received buffer len from socket (without ethernet header)
 * @param[out] neighbor_info remote data to store tlv info
 * @return:  LLDP_RX_TYPE
 * 
 */
LLDP_RX_TYPE collect_tlv_info(uint8_t* buf, int len, remote_systems_data_t* neighbor_info, timeticks* rx_ttl)
{
	int read_idx = 0;
	LLDP_RX_TYPE rx_type = RX_INVALID;
	int ret = -1;

	// ub_hexdump(true, true, buf, len, 0);
	
	ret = get_chassis_info(buf, neighbor_info, &read_idx);
	if (ret != -1)
	{
		ret = get_port_info(buf, neighbor_info, &read_idx);
		if (ret != -1)
		{
			uint8_t tlv_type = get_tlv_type(buf[read_idx]);
			switch(tlv_type)
			{
				case TIME_TO_LIVE:
					rx_type = collect_ttl_info(buf, &read_idx, rx_ttl);
					if (rx_type == RX_NORMAL) // Continue to collect other info. And may be, its include Manifest TLV.
					{
						rx_type = collect_other_tlv_infos(buf, read_idx, neighbor_info, len);
					}
					else if (rx_type == RX_SHUTDOWN)
					{
						// UB_LOG(UBL_DEBUG, "%s Receive shutdown remote\n", __func__);
						// ub_hexdump(true, true, neighbor_info->chassis_id, 16, 0);
						// ub_hexdump(true, true, neighbor_info->port_id, 16, 0);
					}
					break;
				default:
					UB_LOG(UBL_ERROR, "invalid 3rd TLV type %d \n", tlv_type);
					rx_type = RX_INVALID;
				break;
			}
		}
	}
	
	// UB_LOG(UBL_DEBUG, "%s: ret %d \n", __func__, ret);
	return rx_type;
}