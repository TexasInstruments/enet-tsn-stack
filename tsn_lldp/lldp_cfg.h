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
 * lldp_cfg.h
 *
 *  Created on: Jun 12, 2023
 *      Author: hoangloc
 */

#ifndef XL4LLDP_LLDP_CFG_H_
#define XL4LLDP_LLDP_CFG_H_
#include <tsn_unibase/unibase.h>
#include <tsn_combase/cb_xtimer.h>

#ifndef LLDP_LOCAL_INFO_STRING_MAX_LEN
#define LLDP_LOCAL_INFO_STRING_MAX_LEN         (128)
#endif

#ifndef LLDP_REMOTE_INFO_STRING_MAX_LEN
#define LLDP_REMOTE_INFO_STRING_MAX_LEN			(256)
#endif

#define XLLDP_SUPPORTED 0           //!< 1: Extended mode, follow admenment 802.1ABcu 2021
#define DEBUG_ENABLE	1

#if 0
#if DEBUG_ENABLE == 1
#define UB_LOG(level, ...)						\
	{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		(void)ub_log_print(UB_LOGCAT, 0, level, coutstr);	\
	}
#else
#define UB_LOG(level, ...) { do{}while(0); }
#endif
#endif
// The rx-state machine target is to create one rx-ttl timer per neighbor (dynamic), however, due to limitation on memory register on TILLD.
// Better to use one AGEDOUT monitor timer (every 100ms and monitor all neighbor rx-ttl)
#define USE_AGEDOUT_MONITOR
/**
 * @brief yang_lldp ieee:chassis_id_subtype_type
 */
typedef enum
{
	C_CHASSIS_COMPONENT = 1,
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
	P_INTERFACE_ALIAS = 1,
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
	uint8_t chassis_id[LLDP_LOCAL_INFO_STRING_MAX_LEN];
	char system_name[LLDP_LOCAL_INFO_STRING_MAX_LEN];
	char system_description[LLDP_LOCAL_INFO_STRING_MAX_LEN];
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
	struct ub_list_node     node;            //!< To make it queueable
	address_subtype_t address_subtype; //!<  address_subtype:ipv4, ipv6
	uint8_t man_address[MAX_ADDRESS_LEN];     //!< man_address:192.168.88.109
	bool tx_enable;
	uint32_t addr_len;
	man_addr_if_subtype_t     if_subtype;
	uint32_t if_id;
} management_address_tx_port_t;
typedef struct ub_list management_address_tx_port_list_t;

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
	struct ub_list_node     node;            //!< To make it list-able
	address_subtype_t address_subtype; //!<  address_subtype:ipv4, ipv6
	uint8_t address[MAX_ADDRESS_LEN];     //!< man_address:192.168.88.109
	man_addr_if_subtype_t     if_subtype;
	uint32_t if_id;
} management_address_t;
typedef struct ub_list management_address_list_t;

#define MAX_UNKNOW_TLV_LEN  (8)
#ifndef MAX_RM_UNKNOWN_TLV_INFO_LEN
#define MAX_RM_UNKNOWN_TLV_INFO_LEN    (512)
#endif
/**
 * @brief Unknown tlv info (the tlv type is not listed in spec)
 */
typedef struct
{
	struct ub_list_node     node;            //!< To make it list-able
	uint32_t tlv_type;
	uint8_t tlv_info[MAX_RM_UNKNOWN_TLV_INFO_LEN];
} remote_unknown_tlv_t;
typedef struct ub_list remote_unknown_tlv_list_t;

#define MAX_REMOTE_ORG_DEFINE_LEN  (8)
#ifndef MAX_RM_ORG_INFO_LEN
#define MAX_RM_ORG_INFO_LEN    (508)
#endif
/**
 * @brief Remote organization defined info
 */
typedef struct 
{
	struct ub_list_node     node;            //!< To make it list-able
	uint32_t info_identifier;
	uint32_t info_subtype;
	uint32_t info_index;
	uint8_t remote_info[MAX_RM_ORG_INFO_LEN];
} remote_org_defined_info_t;
typedef struct ub_list remote_org_defined_info_list_t;

typedef uint32_t timeticks;
#define MAX_REMOTE_SYSTEM_DATA_LEN  (8)     //!< This define is for tooManyNeighbor checking (will increase later)
/**
 * @brief Remote system data per port
 */
typedef struct
{
	struct ub_list_node     node;            //!< To make it queueable
	timeticks time_mark;                     //!< creation time
	bool need_deleted;
	int original_rxttl;						//!< Original rxttl, use for extended rxttl to switch b/w t-out of x-req and t-out of manifest collecting
	timeticks rx_ttl;						//!< -- counter. Normally it will be reset if we have valid RX, but if no RX, once rx_ttl reach zero => aged-out
	uint32_t remote_index;
	bool remote_too_many_neighbors;
	bool remote_changes;
	chassis_id_type_t chassis_id_subtype;
	uint8_t chassis_id[LLDP_REMOTE_INFO_STRING_MAX_LEN];
	port_id_type_t port_id_subtype;
	uint8_t           port_id[LLDP_REMOTE_INFO_STRING_MAX_LEN];
	char           port_desc[LLDP_REMOTE_INFO_STRING_MAX_LEN];
	char system_name[LLDP_REMOTE_INFO_STRING_MAX_LEN];
	char system_description[LLDP_REMOTE_INFO_STRING_MAX_LEN];
	uint16_t system_capabilities_supported;
	uint16_t system_capabilities_enabled;
	management_address_list_t management_address;
	remote_unknown_tlv_list_t remote_unknown_tlv;
	remote_org_defined_info_list_t remote_org_defined_info;

#ifndef USE_AGEDOUT_MONITOR
	// Per-neighbor rxtick timer to monitor RXTTL every 1s
	cb_xtimer_t *rxtick_timer;
#endif
} remote_systems_data_t;
typedef struct ub_list remote_systems_data_list_t;
/**
 * @brief Represent an interface with interface name and MAC address as keys
 * 1 lldpd system consists of multiple ports
 */
#define PORT_MAX_NAME_LEN       (16)
typedef struct
{
	struct ub_list_node     node;            //!< To make it list-able
	char name[PORT_MAX_NAME_LEN];
	ub_macaddr_t src_mac_address;
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

	// management_address_tx_port_t management_address_tx_port[MAX_PORT_MANAGEMENT_ADDRESS];
	management_address_tx_port_list_t management_address_tx_port;
	port_id_type_t port_id_subtype;
	uint8_t           port_id[LLDP_LOCAL_INFO_STRING_MAX_LEN];
	char           port_desc[LLDP_LOCAL_INFO_STRING_MAX_LEN];
	tx_statistic_t tx_statistic;
	rx_statistic_t rx_statistic;
	remote_systems_data_list_t remote_systems_data; // More dynamic
} lldp_port_t;
typedef struct ub_list lldp_port_list_t;

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
	lldp_port_list_t ports;
} yang_lldp_t;


/////////// Static Memory Definition
#define LLDP_CFG_PORT_INSTMEM lldp_cfg_port_inst
#ifndef LLDP_CFG_PORT_INSTNUM
#define LLDP_CFG_PORT_INSTNUM 8
#endif
UB_SD_GETMEM_DEF_EXTERN(LLDP_CFG_PORT_INSTMEM);

// Assume each port has two IPs
#define LLDP_CFG_MGMT_ADDR_TX_PORT_INSTMEM lldp_cfg_mgmt_addr_tx_port_inst
#ifndef LLDP_CFG_MGMT_ADDR_TX_PORT_INSTNUM
#define LLDP_CFG_MGMT_ADDR_TX_PORT_INSTNUM (LLDP_CFG_PORT_INSTNUM * 2)
#endif
UB_SD_GETMEM_DEF_EXTERN(LLDP_CFG_MGMT_ADDR_TX_PORT_INSTMEM);

// Assume each neighbor has two IPs 10 *2 = 20
#define LLDP_CFG_MGMT_ADDR_INSTMEM lldp_cfg_mgmt_addr_inst
#ifndef LLDP_CFG_MGMT_ADDR_INSTMEM_INSTNUM
#define LLDP_CFG_MGMT_ADDR_INSTMEM_INSTNUM 20
#endif
UB_SD_GETMEM_DEF_EXTERN(LLDP_CFG_MGMT_ADDR_INSTMEM);

// Currently we allow 10 neighbors ONLY
#define LLDP_CFG_RM_SYSTEM_DATA_INSTMEM lldp_cfg_rm_system_data_inst
#ifndef LLDP_CFG_RM_SYSTEM_DATA_INSTMEM_INSTNUM
#define LLDP_CFG_RM_SYSTEM_DATA_INSTMEM_INSTNUM 10
#endif
UB_SD_GETMEM_DEF_EXTERN(LLDP_CFG_RM_SYSTEM_DATA_INSTMEM);

#define LLDP_CFG_RM_ORG_DEF_INSTMEM lldp_cfg_rm_org_def_inst
#ifndef LLDP_CFG_RM_ORG_DEF_INSTMEM_INSTNUM
#define LLDP_CFG_RM_ORG_DEF_INSTMEM_INSTNUM 10
#endif
UB_SD_GETMEM_DEF_EXTERN(LLDP_CFG_RM_ORG_DEF_INSTMEM);

#define LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM lldp_cfg_rm_unknown_tlv_inst
#ifndef LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM_INSTNUM
#define LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM_INSTNUM 10
#endif
UB_SD_GETMEM_DEF_EXTERN(LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM);

/////////// Static Memory Definition End


static inline lldp_port_t* init_cfg_port() {return (lldp_port_t*)UB_SD_GETMEM(LLDP_CFG_PORT_INSTMEM, sizeof(lldp_port_t));}
static inline void deinit_cfg_port(lldp_port_t* p) {UB_SD_RELMEM(LLDP_CFG_PORT_INSTMEM, p);}
static inline management_address_tx_port_t* init_mgmt_tx_port() {return (management_address_tx_port_t*)UB_SD_GETMEM(LLDP_CFG_MGMT_ADDR_TX_PORT_INSTMEM, sizeof(management_address_tx_port_t));}
static inline void deinit_mgmt_tx_port(management_address_tx_port_t* m) {UB_SD_RELMEM(LLDP_CFG_MGMT_ADDR_TX_PORT_INSTMEM, m);}
static inline management_address_t* init_mgmt_addr() {return (management_address_t*)UB_SD_GETMEM(LLDP_CFG_MGMT_ADDR_INSTMEM, sizeof(management_address_t));}
static inline void deinit_mgmt_addr(management_address_t* m) {UB_SD_RELMEM(LLDP_CFG_MGMT_ADDR_INSTMEM, m);}
static inline remote_systems_data_t* init_rm_system_data() {return (remote_systems_data_t*)UB_SD_GETMEM(LLDP_CFG_RM_SYSTEM_DATA_INSTMEM, sizeof(remote_systems_data_t));}
static inline void deinit_rm_system_data(remote_systems_data_t* m) {UB_SD_RELMEM(LLDP_CFG_RM_SYSTEM_DATA_INSTMEM, m);}
static inline remote_org_defined_info_t* init_rm_org_def() {return (remote_org_defined_info_t*)UB_SD_GETMEM(LLDP_CFG_RM_ORG_DEF_INSTMEM, sizeof(remote_org_defined_info_t));}
static inline void deinit_rm_org_def(remote_org_defined_info_t* m) {UB_SD_RELMEM(LLDP_CFG_RM_ORG_DEF_INSTMEM, m);}
static inline remote_unknown_tlv_t* init_rm_unknown_tlv() {return (remote_unknown_tlv_t*)UB_SD_GETMEM(LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM, sizeof(remote_unknown_tlv_t));}
static inline void deinit_rm_unknown_tlv(remote_unknown_tlv_t* m) {UB_SD_RELMEM(LLDP_CFG_RM_UNKNOWN_TLV_INSTMEM, m);}
#endif /* XL4LLDP_LLDP_CFG_H_ */
