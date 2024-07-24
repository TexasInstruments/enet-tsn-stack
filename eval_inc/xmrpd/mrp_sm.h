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
#ifndef __MRP_SM_H_
#define __MRP_SM_H_

#include "mrp_datatypes.h"

typedef enum {
	EVENT_NoEvent = -1,
	EVENT_Begin = 0, // Initialize state machine (10.7.5.1)
	EVENT_New, // A new declaration (10.7.5.4)
	EVENT_Join, // Declaration without signaling new registration (10.7.5.5)
	EVENT_Lv, // Withdraw a declaration (10.7.5.6)
	EVENT_tx, // Transmission opportunity without a LeaveAll (10.7.5.7)
	EVENT_txLA, // Transmission opportunity with a LeaveAll (10.7.5.8)
	EVENT_txLAF, // Transmission opportunity with a LeaveAll, and with no room (Full) (10.7.5.9)
	// rNew to rLv, the order must match to mrpdu_attribute_event_t
	EVENT_rNew, // receive New message (10.7.5.14)
	EVENT_rJoinIn, // receive JoinIn message (10.7.5.15)
	EVENT_rIn, // receive In message (10.7.5.18)
	EVENT_rJoinMt, // receive JoinEmpty message (10.7.5.16)
	EVENT_rMt, // receive Empty message (10.7.5.19)
	EVENT_rLv, // receive Leave message (10.7.5.17)
	EVENT_rLA, // receive a LeaveAll message (10.7.5.20)
	EVENT_Flush, // Port role changes from Root Port or Alternate Port to Designated Port (10.7.5.2)
	EVENT_ReDeclare, // Port role changes from Designated to Root Port orAlternate Port (10.7.5.3)
	EVENT_periodic, // A periodic transmission event occurs (10.7.5.10)
	EVENT_leavetimer, // leavetimer has expired (10.7.5.21)
	EVENT_leavealltimer, // leavealltimer has expired (10.7.5.22)
	EVENT_periodictimer, // periodictimer has expired (10.7.5.23)
	EVENT_periodicEnabled, // periodictimer enabled (10.7.5.11)
	EVENT_periodicDisabled, // periodictimer disabled (10.7.5.12)
	EVENT_LAST_NUM,
} protocol_event_t;

typedef enum {
	ACTION_NoAction, // No action
	ACTION_New, // send a New indication to MAP and the MRP application (10.7.6.12)
	ACTION_Join, // send a Join indication to MAP and the MRP application (10.7.6.13)
	ACTION_Lv, // send a Lv indication to MAP and the MRP application (10.7.6.14)
	ACTION_sN, // send a New message (10.7.6.2)
	ACTION_sJ, // send a JoinIn or JoinMT message (10.7.6.3)
	ACTION_sL, // send a Lv message (10.7.6.4)
	ACTION_s, // send an In or an Empty message (10.7.6.5)
	ACTION_s_, // send an In or an Empty message, if required for optimization of the encoding (10.7.6.5)
	ACTION_sL_, // send a Lv message, if required for optimization of the encoding (10.7.6.4)
	ACTION_sJ_, // send a Join message, if required for optimization of the encoding (10.7.6.3)
	ACTION_sLA, // send a Leave All message (10.7.6.6)
	ACTION_periodic, // Periodic transmission event (10.7.6.7).
	ACTION_leavetimer, // Leave period timer (10.7.4.2)
	ACTION_leavealltimer, // Leave All period timer (10.7.4.3)
	ACTION_periodictimer, // Periodic Transmission timer (10.7.4.4)
	ACTION_x, //Inapplicable. No action or state transition occurs in this case.
	ACTION_del, // delete the attribute. this is not in the standrd.
	ACTION_LAST_NUM,
} protocol_action_t;


// LeaveAll state (see 10.7.9)
typedef enum {
	LASTATE_Active = 0,
	LASTATE_Passive,
	LASTATE_LAST_NUM,
} leaveall_state_t;

// Periodic Transmission state (see 10.7.10)
typedef enum {
	PTSTATE_Active = 0,
	PTSTATE_Passive,
	PTSTATE_LAST_NUM,
} periodic_state_t;

// Leave Timer Actions
typedef enum {
	LVT_NoAction = 0,
	LVT_Start,
	LVT_Stop,
} lvt_action_t;

// Leave All Timer Actions
typedef enum {
	LVAT_NoAction = 0,
	LVAT_Start,
	LVAT_Stop,
} lvat_action_t;

// Periodic Timer Actions
typedef enum {
	PRT_NoAction = 0,
	PRT_Start,
	PRT_Stop,
} prt_action_t;

// Applicant State transition
typedef struct applicant_sm_trans{
	applicant_state_t new_state;
	protocol_action_t action;
} applicant_sm_trans_t;

// Registrar State transition
typedef struct registrar_sm_trans{
	registrar_state_t new_state;
	protocol_action_t action;
	lvt_action_t lvt_action;
} registrar_sm_trans_t;

// LeaveAll State transition
typedef struct leaveall_sm_trans{
	leaveall_state_t new_state;
	protocol_action_t action;
	lvat_action_t lvat_action;
} leaveall_sm_trans_t;

// Periodic Transmission State transition
typedef struct periodic_sm_trans{
	periodic_state_t new_state;
	protocol_action_t action;
	prt_action_t prt_action;
} periodic_sm_trans_t;

void sm_table_init(void);
const applicant_sm_trans_t *applicant_sm(protocol_event_t event, applicant_state_t state);
const registrar_sm_trans_t *registrar_sm(protocol_event_t event, registrar_state_t state);
const leaveall_sm_trans_t *leaveall_sm(protocol_event_t event, leaveall_state_t state);
const periodic_sm_trans_t *periodic_sm(protocol_event_t event, periodic_state_t state);

#endif
