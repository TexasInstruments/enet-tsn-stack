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
/* Automatically generated file.  Don't edit this file.*/

#include "../../uc_dbal.h"
extern uint8_t IETF_INTERFACES_func(uc_dbald *dbald);
#define IETF_INTERFACES_RW IETF_INTERFACES_func(dbald)
extern const char *ietf_interfaces_enum_strings[];
extern const uint8_t ietf_interfaces_enum_max;
extern uint8_t IEEE802_DOT1Q_BRIDGE_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_BRIDGE_RW IEEE802_DOT1Q_BRIDGE_func(dbald)
extern const char *ieee802_dot1q_bridge_enum_strings[];
extern const uint8_t ieee802_dot1q_bridge_enum_max;
extern uint8_t IEEE802_DOT1AB_LLDP_func(uc_dbald *dbald);
#define IEEE802_DOT1AB_LLDP_RW IEEE802_DOT1AB_LLDP_func(dbald)
extern const char *ieee802_dot1ab_lldp_enum_strings[];
extern const uint8_t ieee802_dot1ab_lldp_enum_max;
extern uint8_t IEEE1588_PTP_TT_func(uc_dbald *dbald);
#define IEEE1588_PTP_TT_RW IEEE1588_PTP_TT_func(dbald)
extern const char *ieee1588_ptp_tt_enum_strings[];
extern const uint8_t ieee1588_ptp_tt_enum_max;
extern uint8_t IEEE802_DOT1Q_TSN_CONFIG_UNI_func(uc_dbald *dbald);
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_RW IEEE802_DOT1Q_TSN_CONFIG_UNI_func(dbald)
extern const char *ieee802_dot1q_tsn_config_uni_enum_strings[];
extern const uint8_t ieee802_dot1q_tsn_config_uni_enum_max;
extern uint8_t IETF_NETCONF_MONITORING_func(uc_dbald *dbald);
#define IETF_NETCONF_MONITORING_RW IETF_NETCONF_MONITORING_func(dbald)
extern const char *ietf_netconf_monitoring_enum_strings[];
extern const uint8_t ietf_netconf_monitoring_enum_max;
extern uint8_t IETF_YANG_LIBRARY_func(uc_dbald *dbald);
#define IETF_YANG_LIBRARY_RW IETF_YANG_LIBRARY_func(dbald)
extern const char *ietf_yang_library_enum_strings[];
extern const uint8_t ietf_yang_library_enum_max;
extern uint8_t EXCELFORE_TSN_REMOTE_func(uc_dbald *dbald);
#define EXCELFORE_TSN_REMOTE_RW EXCELFORE_TSN_REMOTE_func(dbald)
extern const char *excelfore_tsn_remote_enum_strings[];
extern const uint8_t excelfore_tsn_remote_enum_max;
extern uint8_t EXCELFORE_NETCONF_SERVER_func(uc_dbald *dbald);
#define EXCELFORE_NETCONF_SERVER_RW EXCELFORE_NETCONF_SERVER_func(dbald)
extern const char *excelfore_netconf_server_enum_strings[];
extern const uint8_t excelfore_netconf_server_enum_max;
//HEADER_PART
const char *yang_static_get_name_by_id(uc_dbald *dbald, uint8_t nameid, uint8_t ap)
{
	if(ap==IETF_INTERFACES_RW){
		if(nameid>=ietf_interfaces_enum_max){return NULL;}
		return ietf_interfaces_enum_strings[nameid];
	}
	if(ap==IEEE802_DOT1Q_BRIDGE_RW){
		if(nameid>=ieee802_dot1q_bridge_enum_max){return NULL;}
		return ieee802_dot1q_bridge_enum_strings[nameid];
	}
	if(ap==IEEE802_DOT1AB_LLDP_RW){
		if(nameid>=ieee802_dot1ab_lldp_enum_max){return NULL;}
		return ieee802_dot1ab_lldp_enum_strings[nameid];
	}
	if(ap==IEEE1588_PTP_TT_RW){
		if(nameid>=ieee1588_ptp_tt_enum_max){return NULL;}
		return ieee1588_ptp_tt_enum_strings[nameid];
	}
	if(ap==IEEE802_DOT1Q_TSN_CONFIG_UNI_RW){
		if(nameid>=ieee802_dot1q_tsn_config_uni_enum_max){return NULL;}
		return ieee802_dot1q_tsn_config_uni_enum_strings[nameid];
	}
	if(ap==IETF_NETCONF_MONITORING_RW){
		if(nameid>=ietf_netconf_monitoring_enum_max){return NULL;}
		return ietf_netconf_monitoring_enum_strings[nameid];
	}
	if(ap==IETF_YANG_LIBRARY_RW){
		if(nameid>=ietf_yang_library_enum_max){return NULL;}
		return ietf_yang_library_enum_strings[nameid];
	}
	if(ap==EXCELFORE_TSN_REMOTE_RW){
		if(nameid>=excelfore_tsn_remote_enum_max){return NULL;}
		return excelfore_tsn_remote_enum_strings[nameid];
	}
	if(ap==EXCELFORE_NETCONF_SERVER_RW){
		if(nameid>=excelfore_netconf_server_enum_max){return NULL;}
		return excelfore_netconf_server_enum_strings[nameid];
	}
	//NAME_BY_ID
	return NULL;
}
int yang_static_get_id_by_name(uc_dbald *dbald, const char *name, uint8_t ap)
{
	int i;
	if(ap==IETF_INTERFACES_RW){
		for(i=0;i<ietf_interfaces_enum_max;i++){
			if(strcmp(name, ietf_interfaces_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==IEEE802_DOT1Q_BRIDGE_RW){
		for(i=0;i<ieee802_dot1q_bridge_enum_max;i++){
			if(strcmp(name, ieee802_dot1q_bridge_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==IEEE802_DOT1AB_LLDP_RW){
		for(i=0;i<ieee802_dot1ab_lldp_enum_max;i++){
			if(strcmp(name, ieee802_dot1ab_lldp_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==IEEE1588_PTP_TT_RW){
		for(i=0;i<ieee1588_ptp_tt_enum_max;i++){
			if(strcmp(name, ieee1588_ptp_tt_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==IEEE802_DOT1Q_TSN_CONFIG_UNI_RW){
		for(i=0;i<ieee802_dot1q_tsn_config_uni_enum_max;i++){
			if(strcmp(name, ieee802_dot1q_tsn_config_uni_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==IETF_NETCONF_MONITORING_RW){
		for(i=0;i<ietf_netconf_monitoring_enum_max;i++){
			if(strcmp(name, ietf_netconf_monitoring_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==IETF_YANG_LIBRARY_RW){
		for(i=0;i<ietf_yang_library_enum_max;i++){
			if(strcmp(name, ietf_yang_library_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==EXCELFORE_TSN_REMOTE_RW){
		for(i=0;i<excelfore_tsn_remote_enum_max;i++){
			if(strcmp(name, excelfore_tsn_remote_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	if(ap==EXCELFORE_NETCONF_SERVER_RW){
		for(i=0;i<excelfore_netconf_server_enum_max;i++){
			if(strcmp(name, excelfore_netconf_server_enum_strings[i])==0){return i;}
		}
		return -1;
	}
	//ID_BY_NAME
	return -1;
}
