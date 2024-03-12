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
#ifndef IETF_INTERFACES_H_
#define IETF_INTERFACES_H_

#include "yang_db_access.h"

typedef enum {
	IETF_INTERFACES_VALUEKEY, // 0(0x0)
	IETF_INTERFACES_DUMMY, // 1(0x1)
	IETF_INTERFACES_NSASC, // 2(0x2)
	IETF_INTERFACES_INTERFACES, // 3(0x3)
	IETF_INTERFACES_INTERFACE, // 4(0x4)
	IETF_INTERFACES_NAME, // 5(0x5)
	IETF_INTERFACES_ENABLED, // 6(0x6)
	IETF_INTERFACES_OPER_STATUS, // 7(0x7)
	IETF_INTERFACES_IF_INDEX, // 8(0x8)
	IETF_INTERFACES_PHYS_ADDRESS, // 9(0x9)
	IETF_INTERFACES_SPEED, // 10(0xa)
	// augmented by ieee802-dot1q-bridge
	IETF_INTERFACES_BRIDGE_PORT, // 11(0xb)
	IETF_INTERFACES_BRIDGE_NAME, // 12(0xc)
	IETF_INTERFACES_COMPONENT_NAME, // 13(0xd)
	IETF_INTERFACES_PVID, // 14(0xe)
	IETF_INTERFACES_DEFAULT_PRIORITY, // 15(0xf)
	IETF_INTERFACES_PRIORITY_REGENERATION, // 16(0x10)
	IETF_INTERFACES_PRIORITY0, // 17(0x11)
	IETF_INTERFACES_PRIORITY1, // 18(0x12)
	IETF_INTERFACES_PRIORITY2, // 19(0x13)
	IETF_INTERFACES_PRIORITY3, // 20(0x14)
	IETF_INTERFACES_PRIORITY4, // 21(0x15)
	IETF_INTERFACES_PRIORITY5, // 22(0x16)
	IETF_INTERFACES_PRIORITY6, // 23(0x17)
	IETF_INTERFACES_PRIORITY7, // 24(0x18)
	IETF_INTERFACES_TRAFFIC_CLASS, // 25(0x19)
	IETF_INTERFACES_TRAFFIC_CLASS_TABLE, // 26(0x1a)
	IETF_INTERFACES_NUMBER_OF_TRAFFIC_CLASSES, // 27(0x1b)
	IETF_INTERFACES_TRANSMISSION_SELECTION_ALGORITHM_TABLE, // 28(0x1c)
	IETF_INTERFACES_TRANSMISSION_SELECTION_ALGORITHM_MAP, // 29(0x1d)
	IETF_INTERFACES_TRANSMISSION_SELECTION_ALGORITHM, // 30(0x1e)
	IETF_INTERFACES_ADDRESS, // 31(0x1f)
	// augmented by xl4interfaces
	IETF_INTERFACES_TC_DATA, // 32(0x20)
	IETF_INTERFACES_TC, // 33(0x21)
	IETF_INTERFACES_LQUEUE, // 34(0x22)
	IETF_INTERFACES_ADMIN_IDLESLOPE, // 35(0x23)
	IETF_INTERFACES_OPER_IDLESLOPE, // 36(0x24)
	IETF_INTERFACES_MAX_INTERFERENCE_SIZE, // 37(0x25)
	IETF_INTERFACES_MAX_FRAME_SIZE, // 38(0x26)
	IETF_INTERFACES_CBS_ENABLED, // 39(0x27)
	IETF_INTERFACES_NUMBER_OF_PQUEUES, // 40(0x28)
	IETF_INTERFACES_PQUEUE_MAP, // 41(0x29)
	IETF_INTERFACES_PQUEUE, // 42(0x2a)
	IETF_INTERFACES_DUPLEX, // 43(0x2b)
	IETF_INTERFACES_DISCON_WORKAROUND_TIME, // 44(0x2c)
	// augmented by ieee802-dot1q-sched-bridge
	IETF_INTERFACES_GATE_PARAMETER_TABLE, // 45(0x2d)
	IETF_INTERFACES_QUEUE_MAX_SDU_TABLE, // 46(0x2e)
	IETF_INTERFACES_QUEUE_MAX_SDU, // 47(0x2f)
	IETF_INTERFACES_TRANSMISSION_OVERRUN, // 48(0x30)
	IETF_INTERFACES_GATE_ENABLED, // 49(0x31)
	IETF_INTERFACES_ADMIN_GATE_STATES, // 50(0x32)
	IETF_INTERFACES_OPER_GATE_STATES, // 51(0x33)
	IETF_INTERFACES_ADMIN_CONTROL_LIST, // 52(0x34)
	IETF_INTERFACES_GATE_CONTROL_ENTRY, // 53(0x35)
	IETF_INTERFACES_INDEX, // 54(0x36)
	IETF_INTERFACES_OPERATION_NAME, // 55(0x37)
	IETF_INTERFACES_TIME_INTERVAL_VALUE, // 56(0x38)
	IETF_INTERFACES_GATE_STATES_VALUE, // 57(0x39)
	IETF_INTERFACES_OPER_CONTROL_LIST, // 58(0x3a)
	IETF_INTERFACES_ADMIN_CYCLE_TIME, // 59(0x3b)
	IETF_INTERFACES_NUMERATOR, // 60(0x3c)
	IETF_INTERFACES_DENOMINATOR, // 61(0x3d)
	IETF_INTERFACES_OPER_CYCLE_TIME, // 62(0x3e)
	IETF_INTERFACES_ADMIN_CYCLE_TIME_EXTENSION, // 63(0x3f)
	IETF_INTERFACES_OPER_CYCLE_TIME_EXTENSION, // 64(0x40)
	IETF_INTERFACES_ADMIN_BASE_TIME, // 65(0x41)
	IETF_INTERFACES_SECONDS, // 66(0x42)
	IETF_INTERFACES_NANOSECONDS, // 67(0x43)
	IETF_INTERFACES_OPER_BASE_TIME, // 68(0x44)
	IETF_INTERFACES_CONFIG_CHANGE, // 69(0x45)
	IETF_INTERFACES_CONFIG_CHANGE_TIME, // 70(0x46)
	IETF_INTERFACES_TICK_GRANULARITY, // 71(0x47)
	IETF_INTERFACES_CURRENT_TIME, // 72(0x48)
	IETF_INTERFACES_CONFIG_PENDING, // 73(0x49)
	IETF_INTERFACES_CONFIG_CHANGE_ERROR, // 74(0x4a)
	IETF_INTERFACES_SUPPORTED_LIST_MAX, // 75(0x4b)
	IETF_INTERFACES_SUPPORTED_CYCLE_MAX, // 76(0x4c)
	IETF_INTERFACES_SUPPORTED_INTERVAL_MAX, // 77(0x4d)
	// augmented by ieee802-dot1q-preemption-bridge
	IETF_INTERFACES_FRAME_PREEMPTION_PARAMETERS, // 78(0x4e)
	IETF_INTERFACES_FRAME_PREEMPTION_STATUS_TABLE, // 79(0x4f)
	IETF_INTERFACES_HOLD_ADVANCE, // 80(0x50)
	IETF_INTERFACES_RELEASE_ADVANCE, // 81(0x51)
	IETF_INTERFACES_PREEMPTION_ACTIVE, // 82(0x52)
	IETF_INTERFACES_HOLD_REQUEST, // 83(0x53)
	IETF_INTERFACES_ENUM_END,
} ietf_interfaces_enum_t;

int ietf_interfaces_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
