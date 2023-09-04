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

#define LLDP_MAX_LOCAL_INTERFACES   (8)
#define LLDP_STRING_MAX_LEN         (256)
/**
 * @brief yang_lldp ieee:chassis_id_subtype_type
 */
typedef enum
{
    C_CHASSIS_COMPONENT,
    C_INTERFACE_ALIAS,
    C_PORT_COMPONENT,
    C_MAC_ADDRESS,
    C_NETWORK_ADDRESS,
    C_INTERFACE_NAME,
    C_LOCAL
} chassis_id_type_t;

/**
 * @brief yang_lldp ieee:port_id_subtype_type
 */
typedef enum
{
    P_INTERFACE_ALIAS,
    P_PORT_COMPONENT,
    P_MAC_ADDRESS,
    P_NETWORK_ADDRESS,
    P_INTERFACE_NAME,
    P_AGENT_CIRCUIT_ID,
    P_LOCAL
} port_id_type_t;

/**
 * @brief yang_lldp remote_statistics
 */
typedef uint32_t timestamp_t;
typedef struct
{
    timestamp_t last_change_time;
    uint32_t remote_inserts;
    uint32_t remote_deletes;
    uint32_t remote_drops;
    uint32_t remote_ageouts;
} remote_statistic_t;

/**
 * @brief yang_lldp local_system_data
 */
typedef struct
{
    chassis_id_type_t chassis_id_subtype;
    char chassis_id[LLDP_STRING_MAX_LEN];
    char system_name[LLDP_STRING_MAX_LEN];
    char system_description[LLDP_STRING_MAX_LEN];
    uint16_t system_capabilities_supported;
    uint16_t system_capabilities_enabled;
} local_system_data_t;


/**
 * @brief TX enable bit position. Once the position is set to 1, lldpd will send the corresponding tlv info
 */
typedef enum
{
    port_desc = 0,
    sys_name = 1,
    sys_desc,
    sys_cap
} tlvs_tx_enable_bits_t;

/**
 * @brief Address subtype
 */
typedef enum
{
    ipv4 = 1,
    ipv6,
    nsap
} address_subtype_t;

/**
 * @brief If subtype
 */
typedef enum
{
    unknown = 1,
    port_ref = 2,
    system_port_number = 3
} man_addr_if_subtype_t;

/**
 * @brief Local manament address of a port. 1 port can have IPV4, IPV6, ...
 */
#define MAX_ADDRESS_LEN             (64) // pattern: [0-9A-F]{2}([0-9A-F]{2}){0,30} <= max len = 2 + 31 *2 = 64
#define MAX_PORT_MANAGEMENT_ADDRESS (4)
typedef struct
{
    address_subtype_t address_subtype; //!<  address_subtype:ipv4, ipv6
    char man_address[MAX_ADDRESS_LEN];     //!< man_address:192.168.88.109
    bool tx_enable;
    uint32_t addr_len;
    man_addr_if_subtype_t     if_subtype;
    uint32_t if_id;
} management_address_tx_port_t;


/**
 * @brief We can enable/disable tx/rx of one interface
 */
typedef enum
{
    tx_only = 1,
    rx_only,
    tx_and_rx,
    disabled
} admin_status_t;

/**
 * @brief tx statistic base on 1 port
 */
typedef struct
{
    uint32_t total_frames;
    uint32_t total_length_errors;
} tx_statistic_t;

/**
 * @brief rx statistic base on 1 port
 */
typedef struct
{
    uint32_t total_ageouts;
    uint32_t total_discarded_frames;
    uint32_t error_frames;
    uint32_t total_frames;
    uint32_t total_discarded_tlvs;
    uint32_t total_unrecognized_tlvs;
} rx_statistic_t;

/**
 * @brief Remote management address info needed
 */
typedef struct
{
    address_subtype_t address_subtype; //!<  address_subtype:ipv4, ipv6
    char address[MAX_ADDRESS_LEN];     //!< man_address:192.168.88.109
    man_addr_if_subtype_t     if_subtype;
    uint32_t if_id;
} management_address_t;

#define MAX_UNKNOW_TLV_LEN  (8)
#define MAX_TLV_INFO_LEN    (512)
/**
 * @brief Unknown tlv info (the tlv type is not listed in spec)
 */
typedef struct
{
    uint32_t tlv_type;
    uint8_t tlv_info[MAX_TLV_INFO_LEN];
} remote_unknown_tlv_t;

#define MAX_REMOTE_ORG_DEFINE_LEN  (8)
#define MAX_REMOTE_INFO_LEN    (508)
/**
 * @brief Remote organization defined info
 */
typedef struct 
{
    uint32_t info_identifier;
    uint32_t info_subtype;
    uint32_t info_index;
    uint8_t remote_info[MAX_REMOTE_INFO_LEN];
} remote_org_defined_info_t;

typedef uint32_t timeticks;
#define MAX_REMOTE_SYSTEM_DATA_LEN  (8)
/**
 * @brief Remote system data per port
 */
typedef struct
{
    timeticks time_mark;
    uint32_t remote_index;
    bool remote_too_many_neighbors;
    bool remote_changes;
    chassis_id_type_t chassis_id_subtype;
    char chassis_id[LLDP_STRING_MAX_LEN];
    port_id_type_t port_id_subtype;
    char           port_id[LLDP_STRING_MAX_LEN];
    char           port_desc[LLDP_STRING_MAX_LEN];
    char system_name[LLDP_STRING_MAX_LEN];
    char system_description[LLDP_STRING_MAX_LEN];
    uint16_t system_capabilities_supported;
    uint16_t system_capabilities_enabled;
    management_address_t management_address[MAX_PORT_MANAGEMENT_ADDRESS];
    remote_unknown_tlv_t remote_unknown_tlv[MAX_UNKNOW_TLV_LEN];
    remote_org_defined_info_t remote_org_defined_info[MAX_REMOTE_ORG_DEFINE_LEN];
} remote_systems_data_t;

/**
 * @brief Represent an interface with interface name and MAC address as keys
 * 1 lldpd system consists of multiple ports
 */
#define PORT_MAX_NAME_LEN       (16)
typedef struct
{
    char name[PORT_MAX_NAME_LEN];
    ub_macaddr_t dest_mac_address;
    admin_status_t admin_status;
    bool notification_enable;
    uint8_t tlvs_tx_enable; //!< mask with tlvs_tx_enable_bits_t
    uint32_t message_fast_tx;
    uint32_t message_tx_hold_multiplier;
    uint32_t message_tx_interval;
    uint32_t reinit_delay;
    uint32_t tx_credit_max;
    uint32_t tx_fast_init;
    uint32_t notification_interval;
    management_address_tx_port_t management_address_tx_port[MAX_PORT_MANAGEMENT_ADDRESS];
    port_id_type_t port_id_subtype;
    char           port_id[LLDP_STRING_MAX_LEN];
    char           port_desc[LLDP_STRING_MAX_LEN];
    tx_statistic_t tx_statistic;
    rx_statistic_t rx_statistic;
    remote_systems_data_t remote_systems_data[MAX_REMOTE_SYSTEM_DATA_LEN];
} lldp_port_t;

/**
 * @brief yang_lldp_t corresponding to yangdb but in mem
 * At the init time of lldp, lldpd should load db in to this struct
 */
typedef struct
{
    uint32_t message_fast_tx;
    uint32_t message_tx_hold_multiplier;
    uint32_t message_tx_interval;
    uint32_t reinit_delay;
    uint32_t tx_credit_max;
    uint32_t tx_fast_init;
    uint32_t notification_interval;
    remote_statistic_t remote_statistic; 
    local_system_data_t local_system_data;
    lldp_port_t ports[LLDP_MAX_LOCAL_INTERFACES];
} yang_lldp_t;

int ydbi_get_item_abk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1,  bool status);

int ydbi_get_item_abk2vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1, uint8_t k2, bool status);

int ydbi_load_lldp_db(yang_db_item_access_t *ydbia, yang_lldp_t* lldp);

#endif /* IEEE802_DOT1AB_LLDP_ACCESS_H_ */