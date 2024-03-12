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
 * rx_state_machine.c
 *
 *  Created on: Jun 20, 2023
 *      Author: hoangloc
 */
#include "interface_mgr.h"
#include "rx_state_machine.h"
#include "lldp_utils.h"
#include "lldp_yangdb.h"
#include "tlv_data.h"
#include <stdio.h>
#include <time.h>

/*Below functions will be used to handle RX statemachine */
static void rxsm_on_port_enable(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_port_disable(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_admin_status_rx_enable(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_admin_status_rx_disable(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_receive_data(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_delete_rx_info_age(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_receive_shutdown(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_receive_normal(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_something_changed_remote(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_finish_one_rx_cycle(struct _hw_interface* interface, RXState pre_state);
static void rxsm_on_bad_frame(struct _hw_interface* interface, RXState pre_state);

static void dump_rs_data(remote_systems_data_t* rs_data);
static int rx_process_frame(hw_interface* interface);
#ifndef USE_AGEDOUT_MONITOR
static void rxttl_timer_expired_cb(cb_xtimer_t *which_timer, void* arg);
#endif
static const char* get_state_name (RXState state)
{
	switch(state)
	{
	case LLDP_WAIT_PORT_OPERATIONAL: return "LLDP_WAIT_PORT_OPERATIONAL";
	case RX_LLDP_INITIALIZE: return "RX_LLDP_INITIALIZE";
	case DELETE_AGED_INFO: return "DELETE_AGED_INFO";
	case RX_WAIT_FOR_FRAME: return "RX_WAIT_FOR_FRAME";
	case RX_FRAME: return "RX_FRAME";
	case DELETE_INFO: return "DELETE_INFO";
	case RX_EXTENDED: return "RX_EXTENDED";
	case UPDATE_INFO: return "UPDATE_INFO";
	case RX_XPDU_REQUEST: return "RX_XPDU_REQUEST";
	case REMOTE_CHANGES: return "REMOTE_CHANGES";
	case LAST_STATE: return "LAST_STATE";
	default: return "default??";
	}
}

// Since there is only one thread, and the process is sequential, 
// so the rx validation will store info into this value (RX_FRAME state), and process (update/delete on another state)
// After finish next state, the variable should be cleanup
remote_systems_data_t potential_remote_data;

rxStateMachine rx_state_machine[] =
{
	{RX_LLDP_INITIALIZE, RX_PORT_DISABLE, rxsm_on_port_disable, LLDP_WAIT_PORT_OPERATIONAL},
	{RX_WAIT_FOR_FRAME, RX_PORT_DISABLE, rxsm_on_port_disable, LLDP_WAIT_PORT_OPERATIONAL},

	{LLDP_WAIT_PORT_OPERATIONAL, PORT_ENABLE, rxsm_on_port_enable, RX_LLDP_INITIALIZE},
	{LLDP_WAIT_PORT_OPERATIONAL, RX_INFO_AGE_TTL_EXPIRED, rxsm_on_delete_rx_info_age, DELETE_AGED_INFO},

	{DELETE_AGED_INFO, RX_UCT, rxsm_on_finish_one_rx_cycle, LLDP_WAIT_PORT_OPERATIONAL},
	{DELETE_AGED_INFO_IN_INITIALIZE, RX_UCT, rxsm_on_finish_one_rx_cycle, RX_LLDP_INITIALIZE},

	{RX_LLDP_INITIALIZE, RX_ENABLED, rxsm_on_admin_status_rx_enable, RX_WAIT_FOR_FRAME},
	{RX_LLDP_INITIALIZE, RX_INFO_AGE_TTL_EXPIRED, rxsm_on_delete_rx_info_age, DELETE_AGED_INFO_IN_INITIALIZE},

	{RX_WAIT_FOR_FRAME, RX_DISABLED, rxsm_on_admin_status_rx_disable, RX_LLDP_INITIALIZE},
	{RX_WAIT_FOR_FRAME, FRAME_RECEIVED, rxsm_on_receive_data, RX_FRAME},
	{RX_WAIT_FOR_FRAME, RX_INFO_AGE_TTL_EXPIRED, rxsm_on_delete_rx_info_age, DELETE_INFO},

	{RX_FRAME, RX_BAD_FRAME, rxsm_on_bad_frame, RX_WAIT_FOR_FRAME},
	{RX_FRAME, RX_TYPE_NORMAL, rxsm_on_receive_normal, UPDATE_INFO},
	{RX_FRAME, RX_TYPE_SHUTDOWN, rxsm_on_receive_shutdown, DELETE_INFO},

	{RX_XPDU_REQUEST, RX_UCT, rxsm_on_finish_one_rx_cycle, RX_WAIT_FOR_FRAME},
	{RX_EXTENDED, RX_MANIFEST_NOT_COMPLETED, rxsm_on_finish_one_rx_cycle, RX_WAIT_FOR_FRAME},

	{UPDATE_INFO, RX_NOTHING_CHANGED, rxsm_on_finish_one_rx_cycle, RX_WAIT_FOR_FRAME},
	{UPDATE_INFO, RX_SOMETHING_CHANGED, rxsm_on_something_changed_remote, REMOTE_CHANGES},

	{DELETE_INFO, RX_UCT, rxsm_on_finish_one_rx_cycle, RX_WAIT_FOR_FRAME},
	{REMOTE_CHANGES, RX_UCT, rxsm_on_finish_one_rx_cycle, RX_WAIT_FOR_FRAME},

};
#ifndef USE_AGEDOUT_MONITOR
typedef struct
{
	hw_interface* parent_interface;
	remote_systems_data_t* rs_data;
} RxTickTimerArg_t;

/// @brief RXTick timer callback.
/// @param which_timer 
/// @param arg 
static void rxttl_timer_expired_cb(cb_xtimer_t *which_timer, void* arg)
{
	RxTickTimerArg_t* timerArg = (RxTickTimerArg_t*) arg;
	if (timerArg->rs_data && which_timer == timerArg->rs_data->rxtick_timer)
	{
		if (timerArg->rs_data->rx_ttl > 0)
		{
			// rx_ttl is used for normal rsdata.
			// UB_LOG(UBL_INFO, "%s:%s - neighbor %s rx_ttl %d -> %d ",__func__, timerArg->parent_interface->if_name, timerArg->rs_data->port_desc, timerArg->rs_data->rx_ttl, timerArg->rs_data->rx_ttl-1);
			// ub_hexdump(true, true, (unsigned char*)&timerArg->rs_data->chassis_id[0], 6, 0);
			timerArg->rs_data->rx_ttl--;
		}
		
		if (timerArg->rs_data->rx_ttl == 0)
		{
			UB_LOG(UBL_INFO, "%s:%s RXTTL Expired\n",__func__, timerArg->parent_interface->if_name);
			cb_xtimer_stop(which_timer); // Need to stop because rs_data will be deleted
			cb_xtimer_delete(which_timer);

			// Delete info
			timerArg->rs_data->need_deleted = true; // Mark this flag to delete it on state machine
			PUSH_RX_EVENT(RX_INFO_AGE_TTL_EXPIRED, timerArg->parent_interface);
		}
	}
}

static void start_rxtick_tmer(hw_interface* interface, remote_systems_data_t* rs_data)
{
	cb_xtimer_set_periodic(rs_data->rxtick_timer); // Stop timer already reset this flag
	int ret = cb_xtimer_start(rs_data->rxtick_timer, 1 * 1000000); // 1 tick is 1s
	if (ret != 0)
	{
		UB_LOG(UBL_INFO, "%s: Cannot start txtick timer %s ret %d\n",__func__, interface->if_name, ret);
	}
	else
	{
		UB_LOG(UBL_INFO, "%s:%s: started txtick timer\n",__func__, interface->if_name);
	}
}
#endif

static void rxInitializeLLDP(hw_interface* interface)
{
	interface->agent_info.tooManyNeighbors = false;
}

static void rxsm_on_bad_frame(struct _hw_interface* interface, RXState pre_state)
{
	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
	increase_rx_error_frame(interface->cfg_port);
	increase_rx_total_discarded_frame(interface->cfg_port);
	update_remote_stat_last_changed_time(interface->yang_lldp_cfg);
	increase_remote_stat_rm_drops(interface->yang_lldp_cfg);
	interface->agent_info.badFrame = true;
	UB_LOG(UBL_DEBUG, "%s %s RX Error [%d]\n", __func__, interface->if_name, interface->cfg_port->rx_statistic.error_frames);
}

static void init_potential_remote_data()
{
	memset(potential_remote_data.chassis_id, 0, sizeof(potential_remote_data.chassis_id));
	memset(potential_remote_data.port_desc, 0, sizeof(potential_remote_data.port_desc));
	memset(potential_remote_data.port_id, 0, sizeof(potential_remote_data.port_id));
	memset(potential_remote_data.system_description, 0, sizeof(potential_remote_data.system_description));
	memset(potential_remote_data.system_name, 0, sizeof(potential_remote_data.system_name));
	ub_list_init(&potential_remote_data.management_address);
	ub_list_init(&potential_remote_data.remote_unknown_tlv);
	ub_list_init(&potential_remote_data.remote_org_defined_info);
}
static void cleanup_potential_rs_data()
{
	// cleanup potential info to avoid mem-leak
	cleanup_remote_system_data(&potential_remote_data);
}

static int rx_process_frame(hw_interface* interface)
{
	int ret = -1;
	if (interface->lldpsock->recv_len > 0)
	{
		int read_idx = 0;
		uint8_t* buf_ptr = &interface->lldpsock->recv_buf[0];
		ub_macaddr_t dst_mac = {0, 0, 0, 0, 0, 0};
		get_dest_mac_addr((const char*)buf_ptr, &read_idx, dst_mac);
		ub_macaddr_t src_mac = {0, 0, 0, 0, 0, 0};
		get_src_mac_addr((const char*)buf_ptr, &read_idx, src_mac);
		uint16_t protocol = get_eth_proto((const char*)buf_ptr, &read_idx);
		// ub_hexdump(true, true, dst_mac, 6, 0);
		// ub_hexdump(true, true, src_mac, 6, 0);
		// UB_LOG(UBL_INFO, "%s: protocol %x \n", __func__, protocol);

		if (protocol == ETH_P_8021Q)
		{
			read_idx += 2;  // skip 4 bytes of VLAN Tagging (includes ETH_P_8021Q) and confirm again the protocol
			protocol = get_eth_proto((const char*)buf_ptr, &read_idx);
		}

		if (protocol == LLDP_PROTO)
		{
			init_potential_remote_data();
			LLDP_RX_TYPE ret = collect_tlv_info(&buf_ptr[read_idx], interface->lldpsock->recv_len - read_idx, &potential_remote_data, &potential_remote_data.rx_ttl);
			switch(ret)
			{
				case RX_INVALID:
					PUSH_RX_EVENT(RX_BAD_FRAME, interface);
					break;
				case RX_SHUTDOWN:
					PUSH_RX_EVENT(RX_TYPE_SHUTDOWN, interface);
					break;
				case RX_NORMAL:
					{
						UB_LOG(UBL_INFO, "%s: rx normal \n", __func__);
						potential_remote_data.original_rxttl = potential_remote_data.rx_ttl;
						PUSH_RX_EVENT(RX_TYPE_NORMAL, interface);
					}
					break;
				default:
				break;
			}

			// Reset this for next receive. The recv buffer is reflected to potential_remote_data
			interface->lldpsock->recv_len = 0;
			memset(interface->lldpsock->recv_buf, 0, sizeof(interface->lldpsock->recv_buf));
		}
		else
		{
			UB_LOG(UBL_INFO, "%s: Protocol %x is not handled in LLDP\n", __func__, protocol);
			PUSH_RX_EVENT(RX_BAD_FRAME, interface);
		}
	}

	return ret;
}

static void something_change_remote(struct _hw_interface* interface)
{
	// TODO: mibUpdateObjects logic should be moved from rx_process_frame into this function.
	// Actually we should do nothing with somethingChangedRemote since yangdb uniconf can take that role to inform another yang's client
	UB_LOG(UBL_DEBUG, "%s: %s \n", __func__, interface->if_name);
}

static void dump_rs_data(remote_systems_data_t* rs_data)
{
	switch(rs_data->chassis_id_subtype)
	{
		case C_MAC_ADDRESS:
			UB_LOG(UBL_INFO, "chassis id: %02x-%02x-%02x-%02x-%02x-%02x \n", 
			                   rs_data->chassis_id[0],
							   rs_data->chassis_id[1],
							   rs_data->chassis_id[2],
							   rs_data->chassis_id[3],
							   rs_data->chassis_id[4],
							   rs_data->chassis_id[5]);
			break;
		default:
			UB_LOG(UBL_INFO, "chassis id: %s \n", rs_data->chassis_id);
			break;
	}

	switch(rs_data->port_id_subtype)
	{
		case P_MAC_ADDRESS:
			UB_LOG(UBL_INFO, "port id:    %02x-%02x-%02x-%02x-%02x-%02x \n", 
			                   rs_data->port_id[0],
			                   rs_data->port_id[1],
							   rs_data->port_id[2],
							   rs_data->port_id[3],
							   rs_data->port_id[4],
							   rs_data->port_id[5]);
			break;
		default:
			UB_LOG(UBL_INFO, "port id:    %s \n", rs_data->port_id);
			break;
	}
}

static void rxsm_on_port_enable(hw_interface* interface, RXState pre_state)
{
	UB_LOG(UBL_INFO, "%s %s\n", __func__, interface->if_name);
	rxInitializeLLDP(interface);
	interface->agent_info.rxFrame = false;
}

static void rxsm_on_port_disable(hw_interface* interface, RXState pre_state)
{
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
}

static void rxsm_on_admin_status_rx_enable(hw_interface* interface, RXState pre_state)
{
	UB_LOG(UBL_DEBUG, "%s %s\n", __func__, interface->if_name);
	interface->agent_info.badFrame = false;
	interface->agent_info.rxInfoAge = false;

	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
}

static void rxsm_on_admin_status_rx_disable(hw_interface* interface, RXState pre_state)
{
	UB_LOG(UBL_INFO, "%s %s\n", __func__, interface->if_name);
	rxInitializeLLDP(interface);
	interface->agent_info.rxFrame = false;
	
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
}

static void rxsm_on_receive_data(hw_interface* interface, RXState pre_state)
{
	if (interface->agent_info.rxFrame)
	{
		interface->agent_info.rxFrame = false;
	}
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
	increase_rx_total_frame(interface->cfg_port);
	rx_process_frame(interface);
}

static void rxsm_on_receive_normal(struct _hw_interface* interface, RXState pre_state)
{
	remote_systems_data_t* existed_neighbor = find_existed_neighbor(interface->cfg_port, &potential_remote_data);
	uint32_t recv_time_in_mili = get_current_time_in_milisec();
	if (existed_neighbor == NULL)
	{
		if (interface->agent_info.tooManyNeighbors == false 
			&& ub_list_count(&interface->cfg_port->remote_systems_data) < MAX_REMOTE_SYSTEM_DATA_LEN)
		{
			// Create new remote system data + write it to DB
			UB_LOG(UBL_INFO, "%s: creating new neighbor [TTL=%d]:\n", interface->if_name, potential_remote_data.original_rxttl);
			remote_systems_data_t* rs_data = create_new_remote_system_data(interface->cfg_port, &potential_remote_data);
			UB_LOG(UBL_INFO, "%s: Got new neighbor [TTL=%d]:\n", interface->if_name, potential_remote_data.original_rxttl);
			dump_rs_data(rs_data);

#ifdef USE_AGEDOUT_MONITOR
			rs_data->rx_ttl = recv_time_in_mili + potential_remote_data.original_rxttl * 1000;
#else
			rs_data->rx_ttl = potential_remote_data.rx_ttl;
			// Create + set periodic
			RxTickTimerArg_t* arg = (RxTickTimerArg_t*)malloc(sizeof(RxTickTimerArg_t)); // TODO
			arg->rs_data = rs_data;
			arg->parent_interface = interface;
			rs_data->rxtick_timer = create_new_rs_rxttl_timer(rxttl_timer_expired_cb,(void*) arg);
			start_rxtick_tmer(interface, rs_data); // every 1s
#endif
			interface->agent_info.newNeighbor = true;

			PUSH_RX_EVENT(RX_SOMETHING_CHANGED, interface);
		}
		else
		{
			// no more space to add remote system data of a new neighbor.
			// 1. discard this neighbor
			interface->agent_info.tooManyNeighbors = true;
			interface->cfg_port->rx_statistic.total_discarded_frames++;
			increase_rx_total_discarded_frame(interface->cfg_port);
			
			// 2. Start tooManyNeighborsTimer = max (tooManyNeighborsTimer, rxTTL)
			lldp_start_too_many_neighbors_timer(interface, potential_remote_data.rx_ttl);
			
			// 3. Nothing changed to come back to wait for frame
			PUSH_RX_EVENT(RX_NOTHING_CHANGED, interface);
		}
	}
	else
	{
		#ifdef USE_AGEDOUT_MONITOR
			existed_neighbor->rx_ttl = recv_time_in_mili + potential_remote_data.original_rxttl * 1000;
		#else
			// Extend the aged-out timing
			existed_neighbor->rx_ttl = potential_remote_data.rx_ttl;
		#endif

		// Check if data changed?
		interface->agent_info.rxChanges = is_remote_system_data_change(&potential_remote_data, existed_neighbor);
		if (!interface->agent_info.rxChanges)
		{
			PUSH_RX_EVENT(RX_NOTHING_CHANGED, interface);
		}
		else
		{
			// Reflect the change to DB
			UB_LOG(UBL_INFO, "%s: Got updated for neighbor [TTL=%d]:\n", interface->if_name, potential_remote_data.original_rxttl);
			dump_rs_data(existed_neighbor);
			update_remote_system_data(interface->cfg_port, &potential_remote_data, existed_neighbor);
			PUSH_RX_EVENT(RX_SOMETHING_CHANGED, interface);
		}
	}
}

static void rxsm_on_receive_shutdown(hw_interface* interface, RXState pre_state)
{
	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
	remote_systems_data_t* existed_neighbor = find_existed_neighbor(interface->cfg_port, &potential_remote_data);
	if (existed_neighbor)
	{
		// ub_hexdump(true, true, &existed_neighbor->chassis_id[0], 16, 0);
		// ub_hexdump(true, true, &existed_neighbor->port_id[0], 16, 0);

		UB_LOG(UBL_INFO, "%s: Process shutdown for existed neighbor:\n", interface->if_name);
		dump_rs_data(existed_neighbor);

		// Delete from DB
		delete_remote_system_data(interface->cfg_port, existed_neighbor);

		// Update some statistic
		update_remote_stat_last_changed_time(interface->yang_lldp_cfg);
		increase_remote_stat_rm_deletes(interface->yang_lldp_cfg);

		// cleanup from ublist
		ub_list_unlink(&interface->cfg_port->remote_systems_data, (struct ub_list_node*)existed_neighbor);
		cleanup_remote_system_data(existed_neighbor);
		
		// Return space to static mem
		deinit_rm_system_data(existed_neighbor);

		something_change_remote(interface);
	}
	else
	{
		UB_LOG(UBL_WARN, "%s %s Recv LLDPDU SHUTDOWN even Neighbor not existed in DB??? Do nothing\n", __func__, interface->if_name);
		ub_hexdump(true, true, &existed_neighbor->chassis_id[0], 16, 0);
		ub_hexdump(true, true, &existed_neighbor->port_id[0], 16, 0);
		PUSH_RX_EVENT(RX_NOTHING_CHANGED, interface);
	}

	// Done, to come back to WAIT FOR FRAME
	PUSH_RX_EVENT(RX_UCT, interface);
}


static void rxsm_on_something_changed_remote(hw_interface* interface, RXState pre_state)
{
	something_change_remote(interface);

	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
	if (interface->agent_info.newNeighbor)
	{
		UB_LOG(UBL_INFO, "%s: Triggering FAST TX\n", interface->if_name);
		PUSH_TXTIMER_EVENT(TXTIMER_NEW_NEIGHBOR, interface);
		update_remote_stat_last_changed_time(interface->yang_lldp_cfg);
		increase_remote_stat_rm_inserts(interface->yang_lldp_cfg);
	}
	PUSH_RX_EVENT(RX_UCT, interface);
}

static void rxsm_on_finish_one_rx_cycle(hw_interface* interface, RXState pre_state)
{
	// Reset buffer for next recv
	cleanup_potential_rs_data();

	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
}

static void rxsm_on_delete_rx_info_age(hw_interface* interface, RXState pre_state)
{
	struct ub_list_node *tmp_node;
	bool found_aged_out = false;
	for(UB_LIST_FOREACH_ITER(&interface->cfg_port->remote_systems_data, tmp_node))
	{
		remote_systems_data_t* r = (remote_systems_data_t*)tmp_node;
		if (r->need_deleted == true)
		{
			// delete info in db
			UB_LOG(UBL_INFO, "%s %s deleting remote %s \n", __func__, interface->if_name, r->port_desc);
			delete_remote_system_data(interface->cfg_port, r);

			update_remote_stat_last_changed_time(interface->yang_lldp_cfg);
			increase_remote_stat_rm_deletes(interface->yang_lldp_cfg);
			increase_remote_stat_rm_agedouts(interface->yang_lldp_cfg);

			increase_rx_total_aged_out(interface->cfg_port);

			found_aged_out = true;
			break;
		}
	}

	if (found_aged_out && tmp_node != NULL)
	{
		UB_LOG(UBL_DEBUG, "%s %s Now delete item \n", __func__, interface->if_name);

		// remove data in cfg port
		ub_list_unlink(&interface->cfg_port->remote_systems_data, tmp_node);
		UB_LOG(UBL_DEBUG, "%s %s unlink done. Try release db \n", __func__, interface->if_name);
		cleanup_remote_system_data((remote_systems_data_t*) tmp_node);
		deinit_rm_system_data((remote_systems_data_t*) tmp_node);
		UB_LOG(UBL_DEBUG, "%s %s Release db done \n", __func__, interface->if_name);
	}


	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_rx_state));
	PUSH_RX_EVENT(RX_UCT, interface);
}

void interface_rx_sm_process(RXEvent ev, hw_interface* interface)
{
	if ( (interface->curr_rx_state < LAST_STATE) && (ev < LAST_EVENT) )
	{
		for (int i=0; i<sizeof(rx_state_machine) / sizeof(rxStateMachine); i++)
		{
			if (rx_state_machine[i].rxState == interface->curr_rx_state && rx_state_machine[i].event== ev && rx_state_machine[i].pfStateMachineEvnentHandler != NULL)
			{
				interface->curr_rx_state = rx_state_machine[i].rxNextState;
				rx_state_machine[i].pfStateMachineEvnentHandler(interface, rx_state_machine[i].rxState);
				// UB_LOG(UBL_INFO, "[%s][%s] State changed %s -> %s(%s)\n",interface->if_name, __func__, get_state_name(rx_state_machine[i].rxState), get_state_name(interface->curr_rx_state), get_state_name(ret));
			}
		}
	}
}
