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
#ifndef IEEE802_DOT1Q_CNC_CONFIG_ACCESS_H_
#define IEEE802_DOT1Q_CNC_CONFIG_ACCESS_H_
#include "yang_db_access.h"
#include "ieee802-dot1q-cnc-config.h"

#define TSNUNI_DOMAIN_ID "domain00" // replace this '00' to instanceIndex

//  "cuc events, bit0:taker-event, bit1:listner-event, bit3:other-event";
#define CC_EVENTFLAG_INIT (0u)
#define CC_EVENTFLAG_TALKER (1u<<0u)
#define CC_EVENTFLAG_LISTENER (1u<<1u)
#define CC_EVENTFLAG_OTHER (1u<<2u)

enum {
	CC_ACCEPT_POLICY_CALLBACK=0,
	CC_ACCEPT_POLICY_CONNECTLIST,
	CC_ACCEPT_POLICY_CONNECT,
	CC_ACCEPT_POLICY_DISCONNECT,
};

enum {
	CC_FRER_POLICY_NOUSE=0,
	CC_FRER_POLICY_USELIST,
	CC_FRER_POLICY_CALLBACK,
};

enum {
	CC_CBS_POLICY_NOUSE=0,
	CC_CBS_POLICY_USELIST,
	CC_CBS_POLICY_CALLBACK,
	CC_CBS_POLICY_USEALWAYS,
};

enum {
	CC_TRM_SELECTION_STRICT_POLICY=0,
	CC_TRM_SELECTION_CBS,
	CC_TRM_SELECTION_ETS,
	CC_TRM_SELECTION_ATS,
};

typedef enum {
	CC_STATUS_INIT=0x00U,
	CC_STATUS_CONNECTED,
	CC_STATUS_DISCONNECTED,
	CC_STATUS_DELETED,
	CC_STATUS_OPERATION_FAILED
}cc_endst_status_t;

typedef enum {
	CC_STREAM_STATUS_PLANNED=0x00U,
	CC_STREAM_STATUS_CONFIGURED,
	CC_STREAM_STATUS_MODIFIED,
}cc_stream_status_t;

typedef enum {
	CC_TALKER_STATUS_NONE=0x00U,
	CC_TALKER_STATUS_READY,
	CC_TALKER_STATUS_FAILED,
}cc_talker_status_t;

typedef enum {
	CC_LISTENER_STATUS_NONE=0x00U,
	CC_LISTENER_STATUS_READY,
	CC_LISTENER_STATUS_PARTIAL_FAILED,
	CC_LISTENER_STATUS_FAILED,
}cc_listener_status_t;

typedef struct cc_eventflag_info{
	const char *cuc_id;
	uint8_t eventflag;
} cc_eventflag_info_t;

typedef struct cc_endstation_info{
	uint8_t instIndex;
	const char *cuc_id;
	uint8_t *streamid;
	uint32_t lindex;
	const char *mac_address;
	const char *interface_name;
	const char *station_name;
	cc_endst_status_t status;
	uint32_t accumulated_latency;
	uint8_t accept;
	bool update_accept;
} cc_endstation_info_t;

typedef struct cc_dfspec_info{
	uint8_t index;
	/* mac_address and vlan_tag are choice fields,
	   so basically it should be in 'union'.
	   As we need the both information to configure bridges,
	   not using 'union'. */
	// mac_address
	char dest_mac_address[18];
	char src_mac_address[18];
	// vlan_tag
	uint8_t pcp;
	uint16_t vlanid;
} cc_dfspec_info_t;

typedef struct cc_tspec_info{
	uint32_t interval_numer;
	uint32_t interval_denom;
	uint16_t max_frame_interval;
	uint16_t max_frame_size;
	uint8_t transmission_selection;
} cc_tspec_info_t;

typedef struct cc_stream_info{
	uint8_t rank;
	cc_dfspec_info_t dfinfo;
	cc_tspec_info_t tsinfo;
} cc_stream_info_t;

enum {
	CC_TAS_GCE_OPER_SET_GATE_STATES=0x00U,
	CC_TAS_GCE_OPER_SET_AND_HOLD_MAC,
	CC_TAS_GCE_OPER_SET_AND_RELEASE_MAC
};

typedef struct cc_gate_control_entry{
	uint32_t index;
	uint32_t opername;
	uint32_t time_interval;
	uint8_t gate_states_value;
} cc_gate_control_entry_t;

typedef struct cc_tas_sched_list{
	uint32_t index;
	uint8_t *streamid;
	const char *bridge_port;
	uint32_t brport_size;
} cc_tas_sched_list_t;

enum {
	CC_CONF_ACCEPT_INIT = 0,
	CC_CONF_ACCEPT_CONNECT,
	CC_CONF_ACCEPT_DISCONNECT,
	CC_CONF_ACCEPT_DELETING,
};

enum {
	CC_STRAMINFO_FLAGBIT_DESTMAC = 0,
	CC_STRAMINFO_FLAGBIT_SRCMAC,
	CC_STRAMINFO_FLAGBIT_PCP,
	CC_STRAMINFO_FLAGBIT_VLANID,
	CC_STRAMINFO_FLAGBIT_INTERVAL_NUMER,
	CC_STRAMINFO_FLAGBIT_INTERVAL_DENOM,
	CC_STRAMINFO_FLAGBIT_MAX_FRAME_INTERVAL,
	CC_STRAMINFO_FLAGBIT_MAX_FRAME_SIZE,
	CC_STRAMINFO_FLAGBIT_TRM_SELECTION,
	CC_STRAMINFO_FLAGBIT_STREAM_RANK,
};

enum {
	CC_TSPEC_TRANS_SEL_STRICT=0x00u,
	CC_TSPEC_TRANS_SEL_CBS,
	CC_TSPEC_TRANS_SEL_ETS,
	CC_TSPEC_TRANS_SEL_ATS,
	// 4-254: Reserved for future specs
	CC_TSPEC_TRANS_SEL_VENDOR=0xFF,
};

int ydbi_del_item_cck1st(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status);
int ydbi_set_item_cck1st(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status,
			 void *value, uint32_t vsize, uint8_t notice);
int ydbi_rel_item_cck1st(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status);
int ydbi_get_item_cck1st(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, bool status);

int ydbi_get_item_cck1ns(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status);
int ydbi_rel_item_cck1ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status);
int ydbi_set_item_cck1ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status,
			 void *value, uint32_t vsize, uint8_t notice);
int ydbi_del_item_cck1ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, bool status);

int ydbi_get_item_cck2ns(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status);
int ydbi_rel_item_cck2ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status);
int ydbi_set_item_cck2ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status,
			 void *value, uint32_t vsize, uint8_t notice);
int ydbi_del_item_cck2ns(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t k1, uint8_t k2, bool status);

int ydbi_del_item_cck2vk2(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status);
int ydbi_set_item_cck2vk2(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status, void *value, uint32_t vsize,
			 uint8_t notice);
int ydbi_rel_item_cck2vk2(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status);
int ydbi_get_item_cck2vk2(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id,
			 uint8_t *streamid, uint8_t k1, uint8_t k2,
			 void *kv1, uint32_t kvs1, void *kv2, uint32_t kvs2,
			 bool status);

int ydbi_get_item_cck2ic(yang_db_item_access_t *ydbia, void **rval,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2);
int ydbi_rel_item_cck2ic(yang_db_item_access_t *ydbia,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2);
int ydbi_set_item_cck2ic(yang_db_item_access_t *ydbia,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2, void *value, uint32_t vsize,
			 uint8_t notice);
int ydbi_del_item_cck2ic(yang_db_item_access_t *ydbia,
			 bool talker, cc_endstation_info_t *endst, uint8_t cindex,
			 uint8_t k1, uint8_t k2);

int ydbi_get_item_cck1tas(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status);
int ydbi_rel_item_cck1tas(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status);
int ydbi_set_item_cck1tas(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status,
			 void *value, uint32_t vsize, uint8_t notice);
int ydbi_del_item_cck1tas(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint8_t k1, uint8_t k2, uint8_t k3, bool status);

int ydbi_get_item_cck2acl(yang_db_item_access_t *ydbia, void **rval,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status);
int ydbi_rel_item_cck2acl(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status);
int ydbi_set_item_cck2acl(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status,
			 void *value, uint32_t vsize, uint8_t notice);
int ydbi_del_item_cck2acl(yang_db_item_access_t *ydbia,
			 uint8_t instIndex, const char *cuc_id, uint32_t *schIndex,
			 uint32_t *gceIndex, uint8_t k1, uint8_t k2, bool status);

/**
 * @brief set "needaction" on "gate-parameter-table/gate-enabled"
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param dereg	true:deregister, false:register
 * @return -1: error, 0:okay
*/
int ydbi_set_needaction_cc(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, const char *cuc_id, bool dereg);

/**
 * @brief set 'accept' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name', 'accept' must be defined.
 * @return -1: error, 0:okay
*/
int ydbi_set_talker_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_set_listener_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief get 'accept' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined,
 *	  'accept' is set as the result.
 * @return -1: error, 0:okay
*/
int ydbi_get_talker_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_get_listener_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief delete 'accept' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined,
 * @return -1: error, 0:okay
*/
int ydbi_del_talker_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_del_listener_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				const char *cuc_id, cc_endstation_info_t *endst);

int ydbi_set_talker_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				     const char *cuc_id, cc_endstation_info_t *endst);

int ydbi_set_listener_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				       const char *cuc_id, cc_endstation_info_t *endst);

int ydbi_get_talker_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				     const char *cuc_id, cc_endstation_info_t *endst);

int ydbi_get_listener_update_accept_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
				       const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief set end-station interface 'status' for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name',
 *	  'status' must be defined.
 *	  other end-station information is left unchanged.
 * @return -1: error, 0:okay
*/
int ydbi_set_talker_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_set_listener_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief get end-station interface 'status' for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined,
 *	  'status' is set as the result.
 *	  other end-station information is left unchanged.
 * @return -1: error, 0:okay
*/
int ydbi_get_talker_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_get_listener_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief delete end-station interface 'status' for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined.
 * @return -1: error, 0:okay
*/
int ydbi_del_talker_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_del_listener_status_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief set 'station-name' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name',
 *	  'station-name' must be defined.
 *	  other end-station information is left unchanged.
 * @return -1: error, 0:okay
*/
int ydbi_set_talker_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_set_listener_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief get 'station-name' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined,
 *	  'station-name' is set as the result.
 *	  other end-station information is left unchanged.
 * @return -1: error, 0:okay
*/
int ydbi_get_talker_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_get_listener_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief delete 'station-name' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined.
 * @return -1: error, 0:okay
*/
int ydbi_del_talker_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_del_listener_station_name_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief set 'accumulated-latency' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name',
 *	  'accumulated-latency' must be defined.
 *	  other end-station information is left unchanged.
 * @return -1: error, 0:okay
*/
int ydbi_set_talker_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_set_listener_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief get 'accumulated-latency' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined,
 *	  'accumulated-latency' will be updated as the result.
 *	  other end-station information is left unchanged.
 * @return -1: error, 0:okay
*/
int ydbi_get_talker_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_get_listener_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief delete 'accumulated-latency' data for a talker/listener device
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst	endstation data structure,
 *	  'streamid', 'mac_address', 'interface_name' must be defined.
 * @return -1: error, 0:okay
*/
int ydbi_del_talker_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);
int ydbi_del_listener_accumulated_latency_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      const char *cuc_id, cc_endstation_info_t *endst);

/**
 * @brief get 'eventflags' data
 * @param ydbia 	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param event 	event flag information structure, 'cuc-id' must be defined,
 *              	'eventflags' is set as the result.
 * @return -1: error, 0:okay
*/
int ydbi_get_eventflags_info(yang_db_item_access_t *ydbia, uint8_t instIndex,
                             cc_eventflag_info_t *event);

/**
 * @brief set 'eventflags' data
 * @param ydbia 	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param event 	event flag information structure, 'cuc-id' and 'eventflags'
 *              	must be defined.
 * @param notice 	YDBI_NO_NOTICE or YDBI_PUSH_NOTICE
 * @return -1: error, 0:okay
*/
int ydbi_set_eventflags_info(yang_db_item_access_t *ydbia, uint8_t instIndex,
                             cc_eventflag_info_t *event, uint8_t notice);

/**
 * @brief del 'eventflags' data
 * @param ydbia 	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param event 	event flag information structure, 'cuc-id' must be defined,
 *              	'eventflags' value is not important.
 * @param notice 	YDBI_NO_NOTICE or YDBI_PUSH_NOTICE
 * @return -1: error, 0:okay
*/
int ydbi_del_eventflags_info(yang_db_item_access_t *ydbia, uint8_t instIndex,
                             cc_eventflag_info_t *event, uint8_t notice);
/**
 * @brief register cuc event flag uniconf notification
 * @param ydbia         ydbia handle
 * @param instIndex     'domain00' the bottom 2digits of hex
 * @param sem           pointer to a notice signal handle
 * @param semname       unique name to identify the notice signal handle
 * @return -1: error, 0:okay
*/
int ydbi_reg_eventflags_notice(yang_db_item_access_t *ydbia, uint8_t instIndex,
                               UC_NOTICE_SIG_T **sem, const char *semname);

/**
 * @brief unregister cuc event flag uniconf notification
 * @param ydbia         ydbia handle
 * @param instIndex     'domain00' the bottom 2digits of hex
 * @param semname       unique name that identify the notice signal handle
 * @return none
*/
void ydbi_unreg_eventflags_notice(yang_db_item_access_t *ydbia, uint8_t instIndex,
                                  const char *semname);

/**
 * @brief get event flag notice domain-id's instance index
 * @param ydbia 	ydbia handle
 * @param instIndex	return pointer to instance index
 * @param semname	unique name to identify the notice signal handle
 * @return -1: error, 0:okay
*/
int ydbi_get_eventflags_notice_info(yang_db_item_access_t *ydbia, uint8_t *instIndex,
                                    char *semname);

/**
 * @brief iterate talker end-station-interfaces
 * @param ydbia 	ydbia handle
 * @param range 	if NULL crate a new range.  not NULL continue
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cuc-id string, cuc's bridge name is used
 * @param endst 	return data structure
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_iterate_talkers_cc(yang_db_item_access_t *ydbia, uc_range **range,
			    uint8_t instIndex, const char *cuc_id,
			    cc_endstation_info_t *endst);
int ydbi_iterate_talker_status_cc(yang_db_item_access_t *ydbia, uc_range **range,
				 uint8_t instIndex, const char *cuc_id,
				 cc_endstation_info_t *endst);
int ydbi_iterate_talker_names_cc(yang_db_item_access_t *ydbia, uc_range **range,
				 uint8_t instIndex, const char *cuc_id,
				 cc_endstation_info_t *endst);

/**
 * @brief iterate listener end-station-interfaces
 * @param ydbia 	ydbia handle
 * @param range 	if NULL crate a new range.  not NULL continue
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param endst 	return data structure
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_iterate_listeners_cc(yang_db_item_access_t *ydbia, uc_range **range,
			      uint8_t instIndex, const char *cuc_id,
			      cc_endstation_info_t *endst);
int ydbi_iterate_listener_status_cc(yang_db_item_access_t *ydbia, uc_range **range,
				   uint8_t instIndex, const char *cuc_id,
				   cc_endstation_info_t *endst);
int ydbi_iterate_listener_names_cc(yang_db_item_access_t *ydbia, uc_range **range,
				   uint8_t instIndex, const char *cuc_id,
				   cc_endstation_info_t *endst);

/**
 * @brief iterate cuc event flag notification
 * @param ydbia 	ydbia handle
 * @param range 	if NULL crate a new range.  not NULL continue
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param event 	return event data structure
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_iterate_event_notice(yang_db_item_access_t *ydbia, uc_range **range,
			    uint8_t instIndex, cc_eventflag_info_t *event);

/**
 * @brief get stream information
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cucid	cucid string, cuc's bridge name is used
 * @param streamid	streamid
 * @param stinfo	return data structure
 * @param dfindex	index fo 'data-frame-specification', normally '0'
 * @return -1: error, 0:okay
*/
int ydbi_get_streaminfo_cc(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, const char *cuc_id, uint8_t *streamid,
			   cc_stream_info_t *stinfo, uint8_t dfindex);

/**
 * @brief set stream information
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cucid	cucid string, cuc's bridge name is used
 * @param streamid	streamid
 * @param stinfo	set data structure
 * @param dfindex	index fo 'data-frame-specification', normally '0'
 * @param set_flags	select which items to set
 * @return -1: error, 0:okay
*/
int ydbi_set_streaminfo_cc(yang_db_item_access_t *ydbia,
			   uint8_t instIndex, const char *cuc_id, uint8_t *streamid,
			   cc_stream_info_t *stinfo, uint8_t dfindex, uint32_t set_flags);

/**
 * @brief delete stream information
 * @param ydbia	ydbia handle
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cucid	cucid string, cuc's bridge name is used
 * @param streamid	streamid
 * @param dfindex	index fo 'data-frame-specification', normally '0'
 * @return -1: error, 0:okay
*/
int ydbi_del_streaminfo_cc(yang_db_item_access_t *ydbia, uint8_t instIndex,
			   const char *cuc_id, uint8_t *streamid, uint8_t dfindex);

/**
 * @brief set default aps, key values and key size in ydbia db access parameters
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cucid	cucid string, cuc's bridge name is used
 * @param streamid	streamid
 * @return none
*/
void ydbi_set_dbpara_ccknvkn(yang_db_item_access_t *ydbia, uint8_t instIndex,
			   const char *cuc_id, uint8_t *streamid);

/**
 * @brief search streamid in the list, and retrun 1(true) if found
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cucid	cucid string, cuc's bridge name is used
 * @param streamid	streamid
 * @return 0:not found, 1:found
*/
uint8_t found_in_streamid_leaflist(yang_db_item_access_t *ydbia,
				   uint8_t instIndex, const char *cuc_id,
				   uint8_t *streamid, uint8_t k4);

/**
 * @brief iterate tas-sched-list apply-stream info
 * @param ydbia 	ydbia handle
 * @param range 	if NULL crate a new range.  not NULL continue
 * @param instIndex	'domain00' the bottom 2digits of hex
 * @param cuc_id	cucid string, cuc's bridge name is used
 * @param schlist 	return data structure ('index' and 'streamid' is set as
 *                  the result)
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_iterate_tas_apply_stream(yang_db_item_access_t *ydbia,
		uc_range **range, uint8_t instIndex, const char *cuc_id,
		cc_tas_sched_list_t *schlist);

/**
 * @brief iterate tas-sched-list apply-bridge-port info
 * @param ydbia     ydbia handle
 * @param range     if NULL crate a new range.  not NULL continue
 * @param instIndex 'domain00' the bottom 2digits of hex
 * @param cuc_id    cucid string, cuc's bridge name is used
 * @param schlist   return data structure ('index' and 'bridge_port' is set as
 *                  the result)
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_iterate_tas_apply_bridge_port(yang_db_item_access_t *ydbia,
		uc_range **range, uint8_t instIndex, const char *cuc_id,
		const char *bridge_name, cc_tas_sched_list_t *schlist);

/**
 * @brief iterate tas-sched-list gate-control-entry operation-name
 * @param ydbia     ydbia handle
 * @param range     if NULL crate a new range.  not NULL continue
 * @param instIndex 'domain00' the bottom 2digits of hex
 * @param cuc_id    cucid string, cuc's bridge name is used
 * @param schdIndex target tas-sched-list index
 * @param gce       return data structure ('index' and 'opername' is set as
 *                  the result)
 * @return -1: error, 0:okay, 1:no more data
 * @note the created range is released when it returns non-ZERO
*/
int ydbi_iterate_tas_gate_ctrl_oper_name(yang_db_item_access_t *ydbia,
                              uc_range **range, uint8_t instIndex,
                              const char *cuc_id, uint32_t schdIndex,
                              cc_gate_control_entry_t *gce);
#endif
