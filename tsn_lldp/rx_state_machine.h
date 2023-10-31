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
 * rx_state_machine.h Handle rx state machine
 *
 *  Created on: Jun 20, 2023
 *      Author: hoangloc
 */

#ifndef XL4LLDP_RX_STATE_MACHINE_H_
#define XL4LLDP_RX_STATE_MACHINE_H_

typedef enum
{
	LLDP_WAIT_PORT_OPERATIONAL,
	RX_LLDP_INITIALIZE,
	DELETE_AGED_INFO,
	DELETE_AGED_INFO_IN_INITIALIZE,	//!< Section 9.2.9 missed one aged-out on RX_LLDP_INITIALIZE state
	RX_WAIT_FOR_FRAME,
	RX_FRAME,
	DELETE_INFO,
	UPDATE_INFO,
	REMOTE_CHANGES,
	// For Rx Extended 
	RX_EXTENDED,
	RX_XPDU_REQUEST,
	LAST_STATE
} RXState;

typedef enum
{
	PORT_ENABLE,                    //!< network interface enable
	RX_PORT_DISABLE,                    //!< network interface disabled
	RX_INFO_AGE_TTL_EXPIRED,        //!< Time to live timer expired but not received data from [portid-chassid]
	RX_ENABLED,         			//!< adminStatus = rxtx or rxOnly or sendManifest = TRUE
	RX_DISABLED,        			//!< adminStatus = disable or txOnly or sendManifest = FALSE
	FRAME_RECEIVED,     //!< recvFrame = True and rxInfoAge = FALSE
	TTL_EXPIRED,     //!< recvFrame = True and rxInfoAge = FALSE
	RX_TYPE_XREQ,     //!< receive request send LLDP PDU
	RX_TYPE_NORMAL,           //!< receive Normal remote TLVs
	RX_TYPE_XPDU,           //!< receive XPDU or MANIFEST
	RX_TYPE_MANF,           //!< receive XPDU or MANIFEST
	RX_TYPE_SHUTDOWN,               //!< receive SHUTDOWN
	RX_TYPE_MALFORM,               //!< receive incorrect format
	RX_MANIFEST_COMPLETED,             //!< rxExtended = FALSE and manifest completed = TRUE
	RX_MANIFEST_NOT_COMPLETED,             //!< rxExtended = FALSE and manifest completed = TRUE
	RX_SOMETHING_CHANGED,                     //!< Something changed on remote side
	RX_BAD_FRAME,                   //!< 
	RX_NOTHING_CHANGED,              //!< 
	RX_UCT,                             //!< Done process, comeback to waiting for frame
	LAST_EVENT
} RXEvent;

struct _hw_interface ;
struct event_queue ;
//typedef of function pointer
typedef void (*lldp_rx_event_handle)(struct _hw_interface* interface, RXState pre_state);

//structure of state and event with event handler
typedef struct
{
	RXState rxState; //!< current state
	RXEvent event;   //!< incomming event
	lldp_rx_event_handle pfStateMachineEvnentHandler; // corresponding handle
	RXState rxNextState; //!< current state
} rxStateMachine;

void interface_rx_sm_process(RXEvent ev, struct _hw_interface* interface);

#endif /* XL4LLDP_RX_STATE_MACHINE_H_ */
