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
 * tx_timer_state_machine.c
 *
 *  Created on: Jul 3, 2023
 *      Author: hoangloc
 */
#include "interface_mgr.h"
#include "tx_timer_state_machine.h"
#include <stdio.h>
#include "lldp_utils.h"

static void txtimer_on_port_tx_enable(struct _hw_interface* interface, TXTimerState pre_state);
static void txtimer_on_port_tx_disable(struct _hw_interface* interface, TXTimerState pre_state);
static void txtimer_on_timer_expired(struct _hw_interface* interface, TXTimerState pre_state);
static void txtimer_on_new_neighbor(struct _hw_interface* interface, TXTimerState pre_state);
static void txtimer_on_txtick(struct _hw_interface* interface, TXTimerState pre_state);
static void txtimer_on_local_changed(struct _hw_interface* interface, TXTimerState pre_state);
static void txtimer_tx_uct(struct _hw_interface* interface, TXTimerState pre_state);

static const char* get_state_name (TXTimerState state)
{
	switch(state)
	{
	case TX_TIMER_INITIALIZE: return "TX_TIMER_INITIALIZE";
	case TX_TIMER_IDLE: return "TX_TIMER_IDLE";
	case TX_TIMER_EXPIRES: return "TX_TIMER_EXPIRES";
	case TX_FAST_START: return "TX_FAST_START";
	case SIGNAL_TX: return "SIGNAL_TX";
	case TX_TICK: return "TX_TICK";
	case TXTIMER_LAST_STATE: return "TXTIMER_LAST_STATE";
	default: return "default??";
	}
}

static const char* get_ev_name(TXTimerEvent ev)
{
	switch(ev)
	{
		case TXTIMER_PORT_DISABLE: return "TXTIMER_PORT_DISABLE";
		case TXTIMER_TX_DISABLED: return "TXTIMER_TX_DISABLED";
		case TXTIMER_TX_ENABLE: return "TXTIMER_TX_ENABLE";
		case TXTIMER_TX_EXPIRED: return "TXTIMER_TX_EXPIRED";
		case TXTIMER_NEW_NEIGHBOR: return "TXTIMER_NEW_NEIGHBOR";
		case TXTIMER_TXTICK_EXPIRED: return "TXTIMER_TXTICK_EXPIRED";
		case TXTIMER_TX_UCT: return "TXTIMER_TX_UCT";
		case TXTIMER_LOCAL_CHANGED: return "TXTIMER_LOCAL_CHANGED";
		case TXTIMER_LAST_EVENT: return "TXTIMER_LAST_EVENT";
		default: return "default??";
	}
}
txTimerStateMachine tx_timer_state_machine[] =
{
	{TX_TIMER_INITIALIZE, TXTIMER_TX_ENABLE, txtimer_on_port_tx_enable, TX_TIMER_IDLE},

	{TX_TIMER_IDLE, TXTIMER_PORT_DISABLE, txtimer_on_port_tx_disable, TX_TIMER_INITIALIZE},
	{TX_TIMER_IDLE, TXTIMER_TX_EXPIRED, txtimer_on_timer_expired, TX_TIMER_EXPIRES},
	{TX_TIMER_IDLE, TXTIMER_NEW_NEIGHBOR, txtimer_on_new_neighbor, TX_FAST_START},
	{TX_TIMER_IDLE, TXTIMER_TXTICK_EXPIRED, txtimer_on_txtick, TX_TICK},
	{TX_TIMER_IDLE, TXTIMER_LOCAL_CHANGED, txtimer_on_local_changed, SIGNAL_TX},

	{TX_TIMER_EXPIRES, TXTIMER_TX_UCT, txtimer_tx_uct, SIGNAL_TX},
	{TX_FAST_START, TXTIMER_TX_UCT, txtimer_tx_uct, TX_TIMER_EXPIRES},
	{SIGNAL_TX, TXTIMER_TX_UCT, txtimer_tx_uct,TX_TIMER_IDLE},
	{TX_TICK, TXTIMER_TX_UCT, txtimer_tx_uct, TX_TIMER_IDLE},

	{TX_TIMER_IDLE, TXTIMER_TX_DISABLED, txtimer_on_port_tx_disable, TX_TIMER_INITIALIZE},
	{TX_TIMER_EXPIRES, TXTIMER_TX_DISABLED, txtimer_on_port_tx_disable, TX_TIMER_INITIALIZE},
	{SIGNAL_TX, TXTIMER_TX_DISABLED, txtimer_on_port_tx_disable, TX_TIMER_INITIALIZE},
	{TX_TICK, TXTIMER_TX_DISABLED, txtimer_on_port_tx_disable, TX_TIMER_INITIALIZE},
	{TX_FAST_START, TXTIMER_TX_DISABLED, txtimer_on_port_tx_disable, TX_TIMER_INITIALIZE},
};

static void processTxSignal(struct _hw_interface* interface)
{
	UB_LOG(UBL_DEBUG, "[%s][%s] trigger signal tx\n", __func__, interface->if_name);
	interface->agent_info.txNow = true;
	if (interface->agent_info.txFast > 0)
		interface->agent_info.txTTR = interface->cfg_port->message_fast_tx;
	else
		interface->agent_info.txTTR = interface->cfg_port->message_tx_interval;

	// txnow -> send msg
	if (interface->agent_info.txNow && interface->agent_info.txCredit > 0)
	{
		UB_LOG(UBL_DEBUG, "[%s] triggerred signal tx with txCredit %d\n", __func__, interface->agent_info.txCredit);
		PUSH_TX_EVENT(TX_NOW, interface);
	}
	else
	{
		UB_LOG(UBL_WARN, "[%s] Cannot process tx txCredit %d\n", __func__, interface->agent_info.txCredit);
	}

	// restart txtimer (msg interval or txFast)
	lldp_start_tx_ttr_timer(interface);
}

static void txtimer_on_port_tx_enable(struct _hw_interface* interface, TXTimerState pre_state)
{
	UB_LOG(UBL_INFO, "[%s] Starting TX Timer and TxTick Timer\n", __func__);
	lldp_start_txticktimer(interface);
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_txtimer_state));
	interface->agent_info.txTTR = 1;
	if (interface->agent_info.txTTR == 0)
	{
		PUSH_TXTIMER_EVENT(TXTIMER_TX_EXPIRED, interface); // send at the first time
	}
	else
	{
		// restart txtimer (msg interval or txFast)
		lldp_start_tx_ttr_timer(interface);
	}
}

static void txtimer_on_port_tx_disable(struct _hw_interface* interface, TXTimerState pre_state)
{
	lldp_stop_txtimers(interface);
	interface->agent_info.txTick = false;
	interface->agent_info.txNow = false;
	interface->agent_info.txTTR = 0;
	interface->agent_info.txFast = 0;
	interface->agent_info.newNeighbor = false;
	interface->agent_info.txCredit = interface->cfg_port->tx_credit_max;
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_txtimer_state));
}

static void txtimer_on_timer_expired(struct _hw_interface* interface, TXTimerState pre_state)
{
	UB_LOG(UBL_DEBUG, "[%s] dec(txFast) %d -> %d\n", __func__, interface->agent_info.txFast, interface->agent_info.txFast -1);
	if (interface->agent_info.txFast > 0) interface->agent_info.txFast--;

	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_txtimer_state));
	PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);
}

static void txtimer_on_new_neighbor(struct _hw_interface* interface, TXTimerState pre_state)
{
	UB_LOG(UBL_DEBUG, "[%s][%s] \n", interface->if_name, __func__);
	interface->agent_info.newNeighbor = false;
	if (interface->agent_info.txFast == 0)
		interface->agent_info.txFast = interface->cfg_port->tx_fast_init;

	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_txtimer_state));
	PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);
}

static void txtimer_on_txtick(struct _hw_interface* interface, TXTimerState pre_state)
{
	interface->agent_info.txTick = false;
	if (interface->agent_info.txCredit < interface->cfg_port->tx_credit_max)
	{
		interface->agent_info.txCredit++; // txAddCredit
	}
	else
	{
		interface->agent_info.txCredit=interface->cfg_port->tx_credit_max;
	}
	PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);

	UB_LOG(UBL_DEBUG, "%s:%s %s->TX_TICK->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_txtimer_state));
}

static void txtimer_on_local_changed(struct _hw_interface* interface, TXTimerState pre_state)
{
	processTxSignal(interface);

	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_txtimer_state));
	PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);
}

static void txtimer_tx_uct(struct _hw_interface* interface, TXTimerState pre_state)
{
	if (TX_TICK != pre_state) // Print too much
		UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_txtimer_state));
	switch(pre_state)
	{
		case SIGNAL_TX: 
			UB_LOG(UBL_DEBUG, "[%s][%s] one TX interval finished\n", interface->if_name, __func__);
			PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);
			break;
		case TX_TIMER_EXPIRES:
			UB_LOG(UBL_DEBUG, "[%s][%s] Fast start -> expired\n", interface->if_name, __func__);
			processTxSignal(interface);
			PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);
			break;
		case TX_TICK:
			PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);
			break;
		case TX_FAST_START:
			PUSH_TXTIMER_EVENT(TXTIMER_TX_UCT, interface);
			break;
		default:
			UB_LOG(UBL_INFO, "[%s] unhandled state(%s)\n", __func__, get_state_name(interface->curr_txtimer_state));
			break;
	}
}

void interface_tx_timer_sm_process(TXTimerEvent ev, struct _hw_interface* interface)
{
	UB_LOG(UBL_DEBUG, "[%s][%s] Incoming event %s with current state %s\n",interface->if_name, __func__, get_ev_name(ev), get_state_name(interface->curr_txtimer_state));

	if ( (interface->curr_txtimer_state < TXTIMER_LAST_STATE) && (ev < TXTIMER_LAST_EVENT) )
	{
		for (int i=0; i<sizeof(tx_timer_state_machine) / sizeof(txTimerStateMachine); i++)
		{
			if (tx_timer_state_machine[i].txTimerState == interface->curr_txtimer_state
					&& tx_timer_state_machine[i].event== ev
					&& tx_timer_state_machine[i].pfStateMachineEvnentHandler != NULL)
			{
				// Change state first
				interface->curr_txtimer_state = tx_timer_state_machine[i].txTimerNextState;
				tx_timer_state_machine[i].pfStateMachineEvnentHandler(interface, tx_timer_state_machine[i].txTimerState);
				// ret and interface->curr_txtimer_state should be same
			}
		}
	}
}

