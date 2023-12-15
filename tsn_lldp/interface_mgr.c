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
#include "interface_mgr.h"
#include <string.h>
#include <tsn_unibase/ub_llist.h>
#include "lldp_yangdb.h"
#include "lldp_utils.h"
#include "tlv_data.h"

#define LLDP_HW_INTERFACE_INSTMEM lldp_hw_interface_inst
#ifndef LLDP_HW_INTERFACE_INSTNUM
#define LLDP_HW_INTERFACE_INSTNUM LLDP_CFG_PORT_INSTNUM
#endif

UB_SD_GETMEM_DEF(LLDP_HW_INTERFACE_INSTMEM, (int)sizeof(hw_interface),
		 LLDP_HW_INTERFACE_INSTNUM);

static void init_default_hw_interface(hw_interface* hw_if);
#ifdef USE_AGEDOUT_MONITOR
static void monitor_agedout_neigbor(hw_interface* hw_if);
#endif
static void db_mon_timer_expired_cb(cb_xtimer_t *which_timer, void* arg);

// Local variables
hw_interface_list* g_hw_if_list = NULL;

cb_xtimer_man_t* txtimer_man = NULL;
cb_xtimer_t *db_mon_timer;						//!< To check periodically whether any db change
// End local variables

#define RECV_TIMEOUT        (10000)  //10ms
#define DB_MON_DURATION       (100000)  //100ms

void start_db_mon_timer()
{
	cb_xtimer_start(db_mon_timer, DB_MON_DURATION); // 100ms
}

void stop_db_mon_timer()
{
	cb_xtimer_stop(db_mon_timer);
}

static void db_mon_timer_expired_cb(cb_xtimer_t *which_timer, void* arg)
{
	monitor_db_change();
}

#ifdef USE_AGEDOUT_MONITOR
static void monitor_agedout_neigbor(hw_interface* hw_if)
{
	bool found_aged_out = false;
	if (hw_if->curr_rx_state == LLDP_WAIT_PORT_OPERATIONAL || hw_if->curr_rx_state == RX_WAIT_FOR_FRAME || hw_if->curr_rx_state == RX_LLDP_INITIALIZE)
	{
		uint32_t now_in_milisec = get_current_time_in_milisec();
		struct ub_list_node *tmp_node;
		for(UB_LIST_FOREACH_ITER(&hw_if->cfg_port->remote_systems_data, tmp_node))
		{
			remote_systems_data_t* r = (remote_systems_data_t*)tmp_node;
			if (r->rx_ttl < now_in_milisec)
			{
				UB_LOG(UBL_INFO, "Aged out for %s key (%d,%u) due to (%u < %u). rxttl(%u)\n", r->port_desc, r->remote_index, r->time_mark, r->rx_ttl, now_in_milisec, r->original_rxttl);
				found_aged_out = true;
				r->need_deleted = true;
				break;
			}
		}
	}

	if (found_aged_out)
	{
		PUSH_RX_EVENT(RX_INFO_AGE_TTL_EXPIRED, hw_if);
	}
	// we do not need to monitor in other states
}
#endif

static void timer_expired_cb(cb_xtimer_t *which_timer, void* arg)
{
	// UB_LOG(UBL_INFO, "%s: %p expired\n", __func__, which_timer);
	hw_interface* hw_if = (hw_interface*)arg;
	if (hw_if)
	{
		if (which_timer == hw_if->txtimer)
		{
			UB_LOG(UBL_DEBUG, "%s: %s txtimer expired\n", __func__, hw_if->cfg_port->name);
			if (hw_if->curr_txtimer_state == TX_TIMER_IDLE)
			{
				PUSH_TXTIMER_EVENT(TXTIMER_TX_EXPIRED, hw_if);
			}
		}
		else if (which_timer == hw_if->txtick_timer)
		{
			// UB_LOG(UBL_INFO, "%s: %s txtick_timer expired\n", __func__, hw_if->cfg_port->name);
			PUSH_TXTIMER_EVENT(TXTIMER_TXTICK_EXPIRED, hw_if);
		}
		else if (which_timer == hw_if->tx_shutdown_while_timer)
		{
			UB_LOG(UBL_INFO, "%s: %s shutdown while timer expired\n", __func__, hw_if->cfg_port->name);
			PUSH_TX_EVENT(TX_SHUTDOWN_WHILE_EXPIRED, hw_if);
			
			if (hw_if->cfg_port->admin_status == tx_and_rx || hw_if->cfg_port->admin_status == tx_only)
			{
				UB_LOG(UBL_INFO, "%s: %s TX is enabled before shutdown while expired. Need to start again now\n", __func__, hw_if->cfg_port->name);
				PUSH_TX_EVENT(TX_ENABLED, hw_if);
				PUSH_TXTIMER_EVENT(TXTIMER_TX_ENABLE, hw_if);
			}
		}
#ifdef USE_AGEDOUT_MONITOR
		else if (which_timer == hw_if->ageout_monitor_timer)
		{
			monitor_agedout_neigbor(hw_if);
		}
#endif
		else if (which_timer == hw_if->too_many_neighbors_timer)
		{
			UB_LOG(UBL_WARN, "%s: %s tooManyNeighbors expired\n", __func__, hw_if->cfg_port->name);
			hw_if->agent_info.tooManyNeighbors = false;
		}
	}
}

static void init_default_hw_interface(hw_interface* hw_if)
{
	// Default state machines state
	hw_if->curr_rx_state = LLDP_WAIT_PORT_OPERATIONAL;
	hw_if->curr_tx_state = TX_LLDP_INITIALIZE;
	hw_if->curr_txtimer_state = TX_TIMER_INITIALIZE;
	hw_if->actual_link_state = 2; // DOWN and will be up soon

	hw_if->txtimer = cb_xtimer_create (txtimer_man, timer_expired_cb, (void*) hw_if);
	hw_if->txtick_timer = cb_xtimer_create (txtimer_man, timer_expired_cb, (void*) hw_if);
	hw_if->tx_shutdown_while_timer = cb_xtimer_create (txtimer_man, timer_expired_cb, (void*) hw_if); // No periodic
	hw_if->ageout_monitor_timer = cb_xtimer_create (txtimer_man, timer_expired_cb, (void*) hw_if);
	hw_if->too_many_neighbors_timer = cb_xtimer_create (txtimer_man, timer_expired_cb, (void*) hw_if); // No periodic
	cb_xtimer_set_periodic(hw_if->txtick_timer);
	cb_xtimer_set_periodic(hw_if->ageout_monitor_timer);
	
	// Agent info init
	hw_if->agent_info.rxInfoAge = false;
	hw_if->agent_info.txShutdownWhile = 0;
	hw_if->agent_info.txTick = false;
	hw_if->agent_info.txNow = false;
	hw_if->agent_info.txFast = 0;
	hw_if->agent_info.newNeighbor = false;
	hw_if->agent_info.txCredit = hw_if->cfg_port->tx_credit_max;
}

#ifndef USE_AGEDOUT_MONITOR
cb_xtimer_t * create_new_rs_rxttl_timer(xtmer_expired_cb cb, void* arg)
{
	cb_xtimer_t * rxttl_timer = cb_xtimer_create (txtimer_man, cb, arg);
	cb_xtimer_set_periodic(rxttl_timer);
	return rxttl_timer;
}
#endif
int lldp_start_tx_ttr_timer(hw_interface* interface)
{
	if (cb_xtimer_is_running(interface->txtimer))
	{
		cb_xtimer_stop(interface->txtimer);
	}
	
	int ret = cb_xtimer_start(interface->txtimer, interface->agent_info.txTTR * 1000000);
	if (ret != 0)
	{
		UB_LOG(UBL_ERROR, "%s: Cannot start txttrtimer timer %s ret %d\n",__func__, interface->if_name, ret);
	}
	else
	{
		UB_LOG(UBL_DEBUG, "%s:%s: txttrtimer %d s\n",__func__, interface->if_name, interface->agent_info.txTTR);
		UB_LOG(UBL_INFO, "%s: Next TX cycle %d s\n", interface->if_name, interface->agent_info.txTTR);
	}

	return ret;
}

int lldp_start_rx_agedout_mon_timer(hw_interface* hw_if)
{
	cb_xtimer_set_periodic(hw_if->ageout_monitor_timer);
	int ret = cb_xtimer_start(hw_if->ageout_monitor_timer, 100000); // 100ms
	if (ret != 0)
	{
		UB_LOG(UBL_ERROR, "%s: Cannot start ageout_monitor_timer timer %s ret %d\n",__func__, hw_if->if_name, ret);
	}
	else
	{
		UB_LOG(UBL_DEBUG, "%s:%s: started ageout_monitor_timer timer\n",__func__, hw_if->if_name);
	}

	return 0;
}

/**
 * @brief This function should be called admin status is enable for tx.
 * called from tx_timer state machine
 */
int lldp_start_txticktimer(hw_interface* hw_if)
{
	int ret = -1;
	cb_xtimer_set_periodic(hw_if->txtick_timer); // Stop timer already reset this flag
	ret = cb_xtimer_start(hw_if->txtick_timer, 1 * 1000000); // 1 tick is 1s
	if (ret != 0)
	{
		UB_LOG(UBL_ERROR, "%s: Cannot start txtick timer %s ret %d\n",__func__, hw_if->if_name, ret);
	}
	else
	{
		UB_LOG(UBL_INFO, "%s:%s: started txtick timer\n",__func__, hw_if->if_name);
	}

	return ret;
}

/**
 * @brief This function should be called once tx is disable for cleanup everything (2s).
 * called from tx_timer state machine
 */
int lldp_start_shutdown_while_timer(hw_interface* hw_if)
{
	int ret = -1;
	/*Start tx timer statemachine*/
	ret = cb_xtimer_start(hw_if->tx_shutdown_while_timer, hw_if->yang_lldp_cfg->reinit_delay * 1000000); // default: 2s
	if (ret != 0)
	{
		UB_LOG(UBL_ERROR, "%s: Cannot start shutdown while timer %s ret %d\n",__func__, hw_if->if_name, ret);
	}
	else
	{
		UB_LOG(UBL_INFO, "%s:%s: started shutdown while timer\n",__func__, hw_if->if_name);
	}
	return ret;
}

int lldp_start_too_many_neighbors_timer(hw_interface* interface, int rxTTL)
{
	int ret = -1;
	if (!cb_xtimer_is_running(interface->too_many_neighbors_timer))
	{
		ret = cb_xtimer_start(interface->too_many_neighbors_timer, rxTTL * 1000000); // if 1st time: use rxTTL
		if (ret != 0)
		{
			UB_LOG(UBL_ERROR, "%s: Cannot start shutdown while timer %s ret %d\n",__func__, interface->if_name, ret);
		}
		else
		{
			UB_LOG(UBL_INFO, "%s:%s: started shutdown while timer\n",__func__, interface->if_name);
		}
	}
	else
	{
		int remain_tout = (int) (cb_xtimer_remain_timeout(interface->too_many_neighbors_timer) / 1000000);
		cb_xtimer_stop(interface->too_many_neighbors_timer);
		 
		// if it's running, tooManyNeighborsTimer = max (tooManyNeighborsTimer, rxTTL)
		int new_timer_duration = MAX(rxTTL, remain_tout);
		ret = cb_xtimer_start(interface->too_many_neighbors_timer, new_timer_duration * 1000000);
		if (ret != 0)
		{
			UB_LOG(UBL_ERROR, "%s: Cannot start shutdown while timer %s ret %d\n",__func__, interface->if_name, ret);
		}
		else
		{
			UB_LOG(UBL_INFO, "%s:%s: started shutdown while timer\n",__func__, interface->if_name);
		}
	}

	return ret;
}
/**
 * @brief This function should be called admin status is disable for tx.
 * called from tx_timer state machine
 */
int lldp_stop_txtimers(hw_interface* interface)
{
	cb_xtimer_stop(interface->txtimer);
	cb_xtimer_stop(interface->txtick_timer);
	return 0;
}

int lldp_stop_rx_agedout_mon_timer(hw_interface* interface)
{
	cb_xtimer_stop(interface->ageout_monitor_timer);
	return 0;
}

static void on_link_state_up(hw_interface* interface)
{
	UB_LOG(UBL_INFO, "%s:%s\n", __func__, interface->cfg_port->name);
	PUSH_RX_EVENT(PORT_ENABLE, interface);

	admin_status_t admin_status = interface->cfg_port->admin_status;
	if (admin_status == tx_and_rx)
	{
		PUSH_RX_EVENT(RX_ENABLED, interface);
		PUSH_TX_EVENT(TX_ENABLED, interface);
		PUSH_TXTIMER_EVENT(TXTIMER_TX_ENABLE, interface);
	}
	else if (admin_status == tx_only)
	{
		PUSH_TX_EVENT(TX_ENABLED, interface);
		PUSH_TXTIMER_EVENT(TXTIMER_TX_ENABLE, interface);
	}
	else if (admin_status == rx_only)
	{
		PUSH_RX_EVENT(RX_ENABLED, interface);
	}
	else // disabled
	{
		UB_LOG(UBL_INFO, "%s: txrx disabled for %s %d\n", __func__, interface->cfg_port->name, admin_status);
	}
}

static void on_link_state_down(hw_interface* interface)
{
	UB_LOG(UBL_INFO, "%s:%s\n", __func__, interface->cfg_port->name);
	PUSH_RX_EVENT(RX_PORT_DISABLE, interface);
	PUSH_TX_EVENT(TX_PORT_DISABLE, interface);
	PUSH_TXTIMER_EVENT(TXTIMER_PORT_DISABLE, interface);
}

static void poll_link_state(hw_interface_list* hw_if_list)
{
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface* interface = (hw_interface*)tmp_node;
		uint8_t link_state = ietf_get_oper_status(interface->if_name);
		if (link_state != interface->actual_link_state)
		{
			UB_LOG(UBL_INFO, "%s:%s Detected Link State changed from %d -> %d\n",__func__, interface->if_name, interface->actual_link_state, link_state);
			if (interface->actual_link_state != 1 && link_state == 1)
			{
				// recorrect Port ID in case of Port ID subtype is MAC
				ietf_get_actual_hw_port_info(interface->cfg_port);

				on_link_state_up(interface);
			}
			else
			{
				on_link_state_down(interface);
			}

			interface->actual_link_state = link_state;
		}
	}
}

int lldpd_start_process(hw_interface_list* hw_if_list, uint8_t* terminate)
{
	int res = 0;
	struct ub_list_node *tmp_node;

	while(!(*terminate))
	{
		poll_link_state(hw_if_list);

		int remain_tout = cb_xtimer_man_schedule(txtimer_man);
		if (remain_tout == -1)
		{
			UB_LOG(UBL_ERROR, "%s: cb_xtimer_man_schedule ERROR\n",__func__);
			break;
		}
		else if (remain_tout == -2) // No running timer
		{
			uc_dbal_releasedb(ydbi_access_handle()->dbald);
			res = try_recv_packet(hw_if_list, RECV_TIMEOUT);
		}
		else
		{
			uc_dbal_releasedb(ydbi_access_handle()->dbald);
			res = try_recv_packet(hw_if_list, remain_tout);
		}

		if (res == -1)
		{
			UB_LOG(UBL_ERROR, "%s: Receive packet error\n",__func__);
			break;
		}
	}

	// Once complete, we need to send shutdown
	UB_LOG(UBL_INFO, "%s Got terminate signal. Broadcast shutdown to all interfaces \n", __func__);
	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface *interface = (hw_interface *)tmp_node;
		// But we still try to start shutdown while timer. Anyway, we done. So it should not a big deal.
		PUSH_TX_EVENT(TX_DISABLED, interface);

	}
	return res;
}

void init_timer_manager()
{
	txtimer_man = cb_xtimer_man_create();
	if (txtimer_man == NULL)
		UB_LOG(UBL_ERROR, "%s: Failure\n", __func__);

	db_mon_timer = cb_xtimer_create (txtimer_man, db_mon_timer_expired_cb, NULL); // global timer, no need any arg
	cb_xtimer_set_periodic(db_mon_timer);
}

void init_local_interfaces(hw_interface_list* hw_if_list, yang_lldp_t* yang_lldp_cfg)
{
	struct ub_list_node *tmp_node;

	// We need a reference to have notice working
	g_hw_if_list = hw_if_list;

	for(UB_LIST_FOREACH_ITER(&yang_lldp_cfg->ports, tmp_node))
	{
		lldp_port_t* port = (lldp_port_t*)tmp_node;
		if (strcmp(port->name, "") != 0)
		{
			// Init hw interface list base on this config
			hw_interface* hw_if = (hw_interface*)UB_SD_GETMEM(LLDP_HW_INTERFACE_INSTMEM, sizeof(hw_interface));
			memset(hw_if, 0, sizeof(hw_interface));
			memcpy(hw_if->if_name, port->name, strlen(port->name));
			hw_if->cfg_port = port;
			hw_if->yang_lldp_cfg = yang_lldp_cfg;
			init_default_hw_interface(hw_if);

			hw_if->agent_info.txFast = port->message_fast_tx ;
			hw_if->agent_info.txCredit = port->tx_credit_max;

			#ifdef USE_AGEDOUT_MONITOR
			lldp_start_rx_agedout_mon_timer(hw_if); // The rx aged out mon can also run on WAIT FOR FUNCTIONNAL state
			#endif

			ub_list_append(hw_if_list, (struct ub_list_node *)hw_if);
		}
		else
		{
			UB_LOG(UBL_INFO, "%s invalid port name %s \n", __func__, port->name);
		}
	}
}

int init_raw_sock(hw_interface_list* hw_if_list)
{
	int ret=0;
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface *interface = (hw_interface *)tmp_node;
		assign_raw_socket_to_port(interface);
	}
	return ret;
}

static void clear_hw_if_cb(struct ub_list_node *node, void *arg)
{
	UB_SD_RELMEM(LLDP_HW_INTERFACE_INSTMEM, (hw_interface*)node);
}

int terminate_all(hw_interface_list* hw_if_list)
{
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface *node = (hw_interface *)tmp_node;

		cb_xtimer_stop(node->txtimer);
		cb_xtimer_delete(node->txtimer);
		cb_xtimer_stop(node->txtick_timer);
		cb_xtimer_delete(node->txtick_timer);
		#ifdef USE_AGEDOUT_MONITOR
		cb_xtimer_stop(node->ageout_monitor_timer);
		cb_xtimer_delete(node->ageout_monitor_timer);
		#endif
		cb_xtimer_stop(node->too_many_neighbors_timer);
		cb_xtimer_delete(node->too_many_neighbors_timer);
		lldp_raw_socket_close(node);

	}

	ub_list_clear(hw_if_list, clear_hw_if_cb, NULL);

	cb_xtimer_man_delete(txtimer_man);

	return 0;
}

void on_local_changed_on_port(const char* if_name, ub_macaddr_t dest_mac)
{
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(g_hw_if_list, tmp_node))
	{
		hw_interface *hw_if = (hw_interface *)tmp_node;

		if (strcmp(if_name, hw_if->cfg_port->name) == 0 && memcmp(dest_mac, hw_if->cfg_port->dest_mac_address, 6) == 0)
		{
			UB_LOG(UBL_INFO,"%s:[%s-]"UB_PRIhexB6"\n", __func__, hw_if->if_name, UB_ARRAY_B6(dest_mac));
			PUSH_TXTIMER_EVENT(TXTIMER_LOCAL_CHANGED, hw_if);
		}
	}
}

void broadcast_local_system_changed()
{
	if (g_hw_if_list != NULL)
	{
		struct ub_list_node *tmp_node;
		for(UB_LIST_FOREACH_ITER(g_hw_if_list, tmp_node))
		{
			hw_interface *hw_if = (hw_interface *)tmp_node;
			PUSH_TXTIMER_EVENT(TXTIMER_LOCAL_CHANGED, hw_if);
		}
	}
}

void on_new_neighbor(hw_interface_list* hw_if_list, char* if_name)
{
	struct ub_list_node *tmp_node;
	bool is_found = false;
	for(UB_LIST_FOREACH_ITER(hw_if_list, tmp_node))
	{
		hw_interface *hw_if = (hw_interface *)tmp_node;
		UB_LOG(UBL_WARN,"%s: compare [%s] -[%s]\n", __func__, if_name, hw_if->if_name);
		if (strcmp(hw_if->if_name, if_name) == 0)
		{
			UB_LOG(UBL_WARN,"%s: found %s \n", __func__, if_name);
			
			PUSH_TXTIMER_EVENT(TXTIMER_NEW_NEIGHBOR, hw_if);

			is_found = true; // just for debugging
			break;
		}
	}

	if (!is_found)
		UB_LOG(UBL_WARN,"%s: Not found [%s] in config\n", __func__, if_name);
}

void on_port_adminstatus_changed(const char* if_name, ub_macaddr_t dest_mac, admin_status_t admin_status)
{
	struct ub_list_node *tmp_node;
	for(UB_LIST_FOREACH_ITER(g_hw_if_list, tmp_node))
	{
		hw_interface *hw_if = (hw_interface *)tmp_node;

		if (strcmp(if_name, hw_if->cfg_port->name) == 0 && memcmp(dest_mac, hw_if->cfg_port->dest_mac_address, 6) == 0) 
		{
			if (admin_status != hw_if->cfg_port->admin_status)
			{
				UB_LOG(UBL_WARN,"%s: Port %s admin status changed from %d -> %d \n", __func__, if_name, hw_if->cfg_port->admin_status, admin_status);
				switch (admin_status)
				{
					case tx_only:
						if (hw_if->cfg_port->admin_status == rx_only || hw_if->cfg_port->admin_status == disabled)
						{
							if (hw_if->agent_info.txShutdownWhile == 0)
							{
								PUSH_TX_EVENT(TX_ENABLED, hw_if);
								PUSH_TXTIMER_EVENT(TXTIMER_TX_ENABLE, hw_if);
							}
							else
							{
								UB_LOG(UBL_WARN,"%s: shutdown is in progress\n", __func__);
							}
						}
						if (hw_if->cfg_port->admin_status == rx_only || hw_if->cfg_port->admin_status == tx_and_rx)
						{
							PUSH_RX_EVENT(RX_DISABLED, hw_if);
						}
						break;
					case rx_only:
						if (hw_if->cfg_port->admin_status == tx_only || hw_if->cfg_port->admin_status == tx_and_rx)
						{
							PUSH_TX_EVENT(TX_DISABLED, hw_if);
							PUSH_TXTIMER_EVENT(TXTIMER_TX_DISABLED, hw_if);
						}

						if (hw_if->cfg_port->admin_status == tx_only || hw_if->cfg_port->admin_status == disabled)
						{
							PUSH_RX_EVENT(RX_ENABLED, hw_if);
						}
						break;
					case disabled:
						if (hw_if->cfg_port->admin_status == tx_only || hw_if->cfg_port->admin_status == tx_and_rx)
						{
							PUSH_TX_EVENT(TX_DISABLED, hw_if);
							PUSH_TXTIMER_EVENT(TXTIMER_TX_DISABLED, hw_if);
						}
						
						if (hw_if->cfg_port->admin_status == rx_only || hw_if->cfg_port->admin_status == tx_and_rx)
						{
							PUSH_RX_EVENT(RX_DISABLED, hw_if);
						}
						break;
					case tx_and_rx:
						if (hw_if->cfg_port->admin_status == rx_only || hw_if->cfg_port->admin_status == disabled)
						{
							if (hw_if->agent_info.txShutdownWhile == 0)
							{
								PUSH_TX_EVENT(TX_ENABLED, hw_if);
								PUSH_TXTIMER_EVENT(TXTIMER_TX_ENABLE, hw_if);
							}
							else
							{
								UB_LOG(UBL_WARN,"%s: shutdown is in progress\n", __func__);
							}
						}
						
						if (hw_if->cfg_port->admin_status == tx_only || hw_if->cfg_port->admin_status == disabled)
						{
							PUSH_RX_EVENT(RX_ENABLED, hw_if);
						}
						break;

				}
				hw_if->cfg_port->admin_status = admin_status;
			}
		}
	}
}