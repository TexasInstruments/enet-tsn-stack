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
 * tx_state_machine.h Handle tx state machine
 *
 *  Created on: Jun 20, 2023
 *      Author: hoangloc
 */

#ifndef XL4LLDP_TX_STATE_MACHINE_H_
#define XL4LLDP_TX_STATE_MACHINE_H_

typedef enum
{
	TX_LLDP_INITIALIZE,
	TX_IDLE,
	TX_SHUTDOWN_FRAME,
	TX_INFO_FRAME,
	TX_LAST_STATE
} TXState;

typedef enum
{
	TX_PORT_DISABLE,            //!< Any state which recv this event, should come back to TX_LLDP_INITIALIZE
	TX_ENABLED,                 //!< adminStatus = rxtx or txOnly
	TX_DISABLED,                //!< adminStatus = disable or rxOnly
	TX_SHUTDOWN_WHILE_EXPIRED,     //!< txShutdownWhile=0
	TX_NOW,                     //!< recvFrame = True and rxInfoAge = FALSE
	TX_UCT,                        //!< tx done
	TX_LAST_EVENT
} TXEvent;

struct _hw_interface ;
//typedef of function pointer
typedef void (*lldp_tx_event_handle)(struct _hw_interface* interface, TXState pre_state);

//structure of state and event with event handler
typedef struct
{
	TXState txState; //!< current state
	TXEvent event;   //!< incomming event
	lldp_tx_event_handle pfStateMachineEvnentHandler; // corresponding handle
	TXState txNextState; //!< next state
} txStateMachine;

void interface_tx_sm_process(TXEvent ev, struct _hw_interface* interface);
#endif /* XL4LLDP_TX_STATE_MACHINE_H_ */
