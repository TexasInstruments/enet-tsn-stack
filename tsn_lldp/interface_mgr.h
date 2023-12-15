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
 * interface.h
 *
 *  Created on: Jun 12, 2023
 *      Author: hoangloc
 */

#ifndef XL4LLDP_INTERFACE_MGR_H_
#define XL4LLDP_INTERFACE_MGR_H_

#include <tsn_unibase/unibase.h>
#include <tsn_combase/cb_ethernet.h>
#include <tsn_combase/cb_thread.h>
#include <tsn_combase/cb_xtimer.h>

#include "rx_state_machine.h"
#include "tx_state_machine.h"
#include "tx_timer_state_machine.h"
#include "lldp_yangdb.h"
#include "socket_util.h"

#define PUSH_TXTIMER_EVENT(ev, hw_if) interface_tx_timer_sm_process(ev, hw_if)
#define PUSH_RX_EVENT(ev, hw_if) interface_rx_sm_process(ev, hw_if)
#define PUSH_TX_EVENT(ev, hw_if) interface_tx_sm_process(ev, hw_if)

struct _hw_interface ;
typedef void (*process_rx_sm)(RXEvent ev, struct _hw_interface* interface);
typedef void (*process_tx_sm)(TXEvent ev, struct _hw_interface* interface);
typedef void (*process_txtimer_sm)(TXTimerEvent ev, struct _hw_interface* interface);
typedef void (*xtmer_expired_cb)(cb_xtimer_t *which_timer, void* arg);

/// @brief Store per-agent information
typedef struct
{
	bool txNow;
	bool txTick;
	bool newNeighbor;
	uint32_t txFast;
	uint32_t txCredit;
	uint32_t txTTL;
	uint32_t txTTR;
	uint32_t rxTTL;
	uint32_t txShutdownWhile;
	bool rxFrame;
	bool rxInfoAge;
	bool tooManyNeighbors;
	bool badFrame;
	bool rxChanges;
} agent_info_t;

/// @brief Main data structure, each hw_interface represents a LLDP port
typedef struct _hw_interface
{
	struct ub_list_node     node;            //!< To make it list-able
	char if_name[CB_MAX_NETDEVNAME];         //!< Interface name eth0,enpxs0,.... if_name is copied from yang-lldp
	uint8_t actual_link_state;               //!< 1: up 2: down

	lldp_socket_t* lldpsock;					 //!< Corresponding raw socket

	// State machine processing
	RXState curr_rx_state;                  //!< Current state of RX state machine
	TXState curr_tx_state;                  //!< Current state of TX state machine
	TXTimerState curr_txtimer_state;        //!< Current state of TXTimer state machine
	
	cb_xtimer_t *txtimer;                   //!< 9.2.2.3 txTTR Timer. Once timer expired, a local LLDPDU will be sent out (default duration: 30s)
	cb_xtimer_t *txtick_timer;              //!< 9.2.5.21 txTick Timer. this timer periodically run every 1s
	cb_xtimer_t *tx_shutdown_while_timer;   //!< 9.2.2.4 txShutdownWhile Timer. Once tx is disable, start this timer (default 2s, within 2s, we cannot start again tx)
#ifdef USE_AGEDOUT_MONITOR
	cb_xtimer_t *ageout_monitor_timer;      //!< 9.2.5.13 rxInfoAge Timer. Decide whether remote data is still valid or not (monitor base on rxTTL)
#endif
	cb_xtimer_t *too_many_neighbors_timer;  //!< 9.2.7.7.5 Too many neighbors

	agent_info_t agent_info;                //!< Store internal statistic

	lldp_port_t *cfg_port;                  //!< hw-interface <-> config interface reference (access to read local config (port name, system name,...))
	yang_lldp_t* yang_lldp_cfg;             //!< hw-interface <-> global config (access to read local config (chasis info ...))
} hw_interface;
typedef struct ub_list hw_interface_list;

/// @brief Init unibase xtimer manager
void init_timer_manager();

/// @brief Monitor any update from DB
void start_db_mon_timer();

/// @brief Stop Monitor any update from DB
void stop_db_mon_timer();

/// @brief Init local hwinterface base on yang db config
/// @param hw_if_list [out] All of hw_interface init will be push to this list
/// @param yang_lldp_cfg [in] Store number of interface to be init
void init_local_interfaces(hw_interface_list* hw_if_list,yang_lldp_t* yang_lldp_cfg);

/// @brief Init raw socket of each item in hw_if_list
/// @param hw_if_list 
/// @return 
int init_raw_sock(hw_interface_list* hw_if_list);

/// @brief Terminate all sockets/threads of each item in hw_if_list
/// @param hw_if_list 
/// @return 
int terminate_all(hw_interface_list* hw_if_list);

/// @brief lldpd main function
/// @param hw_if_list 
/// @return 
int lldpd_start_process(hw_interface_list* hw_if_list, uint8_t* terminate);

/// @brief This function should be called once admin status is enable for tx.
/// called from tx_timer state machine
int lldp_start_txticktimer(hw_interface* interface);

int lldp_start_rx_agedout_mon_timer(hw_interface* interface);
int lldp_stop_rx_agedout_mon_timer(hw_interface* interface);

/// @brief This function should be called once admin status is enable for tx.
/// called from tx_timer state machine
int lldp_start_tx_ttr_timer(hw_interface* interface);

/// @brief This function should be called once admin status is disable for tx.
/// called from tx_timer state machine
int lldp_stop_txtimers(hw_interface* interface);

/// @brief This function should be called once tx is disable for cleanup everything (2s).
/// called from tx_timer state machine
int lldp_start_shutdown_while_timer(hw_interface* interface);

/// @brief Start/Restart tooManyNeighborsTimer
/// called if no more space to store new neighbor
/// @param interface 
/// @param rxTTL rxTTL from LLDPDU to be discarded due to no more space
/// @return 
int lldp_start_too_many_neighbors_timer(hw_interface* interface, int rxTTL);

/// @brief Broadcast local changed event to all of hw_interface
void broadcast_local_system_changed();

/// @brief Local changed but for only specific port
/// @param if_name 
/// @param dest_mac 
void on_local_changed_on_port(const char* if_name, ub_macaddr_t dest_mac);

/// @brief This function should be called once lldpd receiving detected new neighbor
/// @param hw_if_list list of active ports (interface) to be searched
/// @param if_name name of affected interface from which new neighbor occur
void on_new_neighbor(hw_interface_list* hw_if_list, char* if_name);

/// @brief Admin status changed per port
/// @param if_name 
/// @param dest_mac 
/// @param admin_status 
void on_port_adminstatus_changed(const char* if_name, ub_macaddr_t dest_mac, admin_status_t admin_status);

#ifndef USE_AGEDOUT_MONITOR
/// @brief Create new per-neighbor rxttl timer. Using same xtimermanager with other xtimer in interface-mgr. But the callback is set from rxstatemachine
/// @param  cb Timer expired callback
/// @param arg Timer Arguments
/// @return newly created timer
cb_xtimer_t * create_new_rs_rxttl_timer(xtmer_expired_cb cb, void* arg);
#endif
#endif /* XL4LLDP_INTERFACE_MGR_H_ */
