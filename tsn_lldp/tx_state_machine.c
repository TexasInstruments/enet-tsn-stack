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
 * tx_state_machine.c
 *
 *  Created on: Jun 20, 2023
 *      Author: hoangloc
 */

#include "interface_mgr.h"
#include "tx_state_machine.h"
#include "tlv_data.h"
#include "lldp_utils.h"
#include <stdio.h>

// #define MIN(x, y) (((x) < (y)) ? (x) : (y))
uint32_t txTTL = 0; // once port disable or adminstatus = disable, reset to 0
bool txNow = false;

typedef struct sendbuf {
	CB_ETHHDR_T ehd;
	uint8_t pdata[1500];
} __attribute__((packed)) sendbuf_t;
uint8_t pdata[1500] = {0};

/*Below functions will be used to handle RX statemachine */
static void on_port_disable(struct _hw_interface* interface, TXState pre_state);
static void on_port_tx_enable(struct _hw_interface* interface, TXState pre_state);
static void on_port_tx_disable(struct _hw_interface* interface, TXState pre_state);
static void on_tx_shutdown_while_expired(struct _hw_interface* interface, TXState pre_state);
static void on_tx_now(struct _hw_interface* interface, TXState pre_state);
static void on_finish_tx_cycle(struct _hw_interface* interface, TXState pre_state);

static const char* get_state_name (TXState state)
{
	switch(state)
	{
	case TX_LLDP_INITIALIZE: return "TX_LLDP_INITIALIZE";
	case TX_IDLE: return "TX_IDLE";
	case TX_SHUTDOWN_FRAME: return "TX_SHUTDOWN_FRAME";
	case TX_INFO_FRAME: return "TX_INFO_FRAME";
	case TX_LAST_STATE: return "TX_LAST_STATE";
	default: return "default??";
	}
}
static void txInitializeLLDP(struct _hw_interface* interface);

txStateMachine tx_state_machine[] =
{
	{TX_IDLE, TX_PORT_DISABLE, on_port_disable, TX_LLDP_INITIALIZE},
	{TX_LLDP_INITIALIZE, TX_ENABLED, on_port_tx_enable,TX_IDLE},
	{TX_IDLE, TX_DISABLED, on_port_tx_disable, TX_SHUTDOWN_FRAME},
	{TX_IDLE, TX_NOW, on_tx_now, TX_INFO_FRAME},
	{TX_SHUTDOWN_FRAME, TX_SHUTDOWN_WHILE_EXPIRED, on_tx_shutdown_while_expired, TX_LLDP_INITIALIZE},
	{TX_INFO_FRAME, TX_UCT, on_finish_tx_cycle, TX_IDLE},
};

static void txInitializeLLDP(struct _hw_interface* interface)
{
	txTTL = MIN(interface->cfg_port->message_tx_interval * interface->cfg_port->message_tx_hold_multiplier + 1, 65535);
	UB_LOG(UBL_INFO, "%s:%s\n", __func__, interface->cfg_port->name);
	UB_LOG(UBL_INFO, "reinitDelay: %d\n", interface->cfg_port->reinit_delay);
	UB_LOG(UBL_INFO, "msgTxHold: %d\n", interface->cfg_port->message_tx_hold_multiplier);
	UB_LOG(UBL_INFO, "msgTxInterval: %d\n", interface->cfg_port->message_tx_interval);
	UB_LOG(UBL_INFO, "msgFastTx: %d\n", interface->cfg_port->message_fast_tx);
	UB_LOG(UBL_INFO, "ttl: %d\n", txTTL);
	interface->agent_info.txShutdownWhile = 0;
}

static int mibConstrInfoLLDPDU(struct _hw_interface* interface)
{
	int len = lldpdu_builder(interface->yang_lldp_cfg, pdata, interface->cfg_port);
	UB_LOG(UBL_DEBUG, "%s - tlvlen %d\n", __func__, len);
	// ub_hexdump(true, true, pdata, len, 0);

	return len;
}

static int mibConstrShutdownLLDPDU(struct _hw_interface* interface)
{
	int len = tlv_shutdown_builder(interface->yang_lldp_cfg, pdata, interface->cfg_port);
	UB_LOG(UBL_DEBUG, "%s - tlvlen %d\n", __func__, len);
	// ub_hexdump(true, true, pdata, len, 0);

	return len;
}

static void txFrame(struct _hw_interface* interface, uint8_t* buf, int len)
{
	int ret = 0;
	ret = lldp_send_packet(interface, buf, len, false);
	if (ret == -1)
	{
		increase_tx_len_error(interface->cfg_port);
	}
	else
	{
		increase_tx_frame(interface->cfg_port);
	}
	UB_LOG(UBL_DEBUG, "[%s] %s ret %d\n", __func__, interface->if_name, ret);
}

/*Below functions will be used to handle TX statemachine */
static void on_port_disable(struct _hw_interface* interface, TXState pre_state)
{
	interface->agent_info.txShutdownWhile = 0;
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_tx_state));
}

static void on_port_tx_enable(struct _hw_interface* interface, TXState pre_state)
{
	txInitializeLLDP(interface);
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_tx_state));
}
static void on_port_tx_disable(struct _hw_interface* interface, TXState pre_state)
{
	int tlv_len = mibConstrShutdownLLDPDU(interface);
	txFrame(interface, pdata, tlv_len);
	interface->agent_info.txShutdownWhile = interface->cfg_port->reinit_delay; // if txShutdownWhile > 0, mean shutdown is in progress
	
	lldp_start_shutdown_while_timer(interface);

	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_tx_state));
}

static void on_tx_shutdown_while_expired(struct _hw_interface* interface, TXState pre_state)
{
	interface->agent_info.txShutdownWhile = 0;
	UB_LOG(UBL_INFO, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_tx_state));
}

static void on_tx_now(struct _hw_interface* interface, TXState pre_state)
{
	int tlv_len = mibConstrInfoLLDPDU(interface);
	txFrame(interface, pdata, tlv_len);
	interface->agent_info.txCredit--;
	interface->agent_info.txNow = false;
	
	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_tx_state));
	UB_LOG(UBL_INFO, "%s: TX %d bytes\n", interface->if_name, tlv_len);
	// ub_hexdump(true, true, pdata, tlv_len, 0);
	PUSH_TX_EVENT(TX_UCT, interface);
}

static void on_finish_tx_cycle(struct _hw_interface* interface, TXState pre_state)
{
	UB_LOG(UBL_DEBUG, "%s:%s %s->%s\n", __func__, interface->if_name, get_state_name(pre_state), get_state_name(interface->curr_tx_state));
}

void interface_tx_sm_process(TXEvent ev, struct _hw_interface* interface)
{
	UB_LOG(UBL_DEBUG, "[%s][%s]\n", interface->if_name, __func__);
	if ( (interface->curr_tx_state < TX_LAST_STATE) && (ev < TX_LAST_EVENT) )
	{
		for (int i=0; i<sizeof(tx_state_machine) / sizeof(txStateMachine); i++)
		{
			if (tx_state_machine[i].txState == interface->curr_tx_state && tx_state_machine[i].event== ev && tx_state_machine[i].pfStateMachineEvnentHandler != NULL)
			{
				interface->curr_tx_state = tx_state_machine[i].txNextState;
				tx_state_machine[i].pfStateMachineEvnentHandler(interface, tx_state_machine[i].txState);
				// UB_LOG(UBL_INFO, "[%s][%s] State changed %s -> %s(%s)\n", interface->if_name, __func__, get_state_name(tx_state_machine[i].txState), get_state_name(interface->curr_tx_state), get_state_name(ret));
			}
		}
	}
}
