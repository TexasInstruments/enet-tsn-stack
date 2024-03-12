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
#ifndef IETF_NETCONF_MONITORING_H_
#define IETF_NETCONF_MONITORING_H_

#include "yang_db_access.h"

typedef enum {
	IETF_NETCONF_MONITORING_VALUEKEY, // 0(0x0)
	IETF_NETCONF_MONITORING_DUMMY, // 1(0x1)
	IETF_NETCONF_MONITORING_NSASC, // 2(0x2)
	IETF_NETCONF_MONITORING_NETCONF_STATE, // 3(0x3)
	IETF_NETCONF_MONITORING_CAPABILITIES, // 4(0x4)
	IETF_NETCONF_MONITORING_CAPABILITY, // 5(0x5)
	IETF_NETCONF_MONITORING_DATASTORES, // 6(0x6)
	IETF_NETCONF_MONITORING_DATASTORE, // 7(0x7)
	IETF_NETCONF_MONITORING_NAME, // 8(0x8)
	IETF_NETCONF_MONITORING_LOCKS, // 9(0x9)
	IETF_NETCONF_MONITORING_GLOBAL_LOCK, // 10(0xa)
	IETF_NETCONF_MONITORING_LOCKED_BY_SESSION, // 11(0xb)
	IETF_NETCONF_MONITORING_LOCKED_TIME, // 12(0xc)
	IETF_NETCONF_MONITORING_PARTIAL_LOCK, // 13(0xd)
	IETF_NETCONF_MONITORING_LOCK_ID, // 14(0xe)
	IETF_NETCONF_MONITORING_SELECT, // 15(0xf)
	IETF_NETCONF_MONITORING_LOCKED_NODE, // 16(0x10)
	IETF_NETCONF_MONITORING_SCHEMAS, // 17(0x11)
	IETF_NETCONF_MONITORING_SCHEMA, // 18(0x12)
	IETF_NETCONF_MONITORING_IDENTIFIER, // 19(0x13)
	IETF_NETCONF_MONITORING_VERSION, // 20(0x14)
	IETF_NETCONF_MONITORING_FORMAT, // 21(0x15)
	IETF_NETCONF_MONITORING_NAMESPACE, // 22(0x16)
	IETF_NETCONF_MONITORING_LOCATION, // 23(0x17)
	IETF_NETCONF_MONITORING_SESSIONS, // 24(0x18)
	IETF_NETCONF_MONITORING_SESSION, // 25(0x19)
	IETF_NETCONF_MONITORING_SESSION_ID, // 26(0x1a)
	IETF_NETCONF_MONITORING_TRANSPORT, // 27(0x1b)
	IETF_NETCONF_MONITORING_USERNAME, // 28(0x1c)
	IETF_NETCONF_MONITORING_SOURCE_HOST, // 29(0x1d)
	IETF_NETCONF_MONITORING_LOGIN_TIME, // 30(0x1e)
	IETF_NETCONF_MONITORING_IN_RPCS, // 31(0x1f)
	IETF_NETCONF_MONITORING_IN_BAD_RPCS, // 32(0x20)
	IETF_NETCONF_MONITORING_OUT_RPC_ERRORS, // 33(0x21)
	IETF_NETCONF_MONITORING_OUT_NOTIFICATIONS, // 34(0x22)
	IETF_NETCONF_MONITORING_STATISTICS, // 35(0x23)
	IETF_NETCONF_MONITORING_NETCONF_START_TIME, // 36(0x24)
	IETF_NETCONF_MONITORING_IN_BAD_HELLOS, // 37(0x25)
	IETF_NETCONF_MONITORING_IN_SESSIONS, // 38(0x26)
	IETF_NETCONF_MONITORING_DROPPED_SESSIONS, // 39(0x27)
	IETF_NETCONF_MONITORING_ENUM_END,
} ietf_netconf_monitoring_enum_t;

int ietf_netconf_monitoring_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
