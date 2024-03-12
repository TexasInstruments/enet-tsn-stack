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
#ifndef IEEE802_DOT1Q_BRIDGE_H_
#define IEEE802_DOT1Q_BRIDGE_H_

#include "yang_db_access.h"

typedef enum {
	IEEE802_DOT1Q_BRIDGE_VALUEKEY, // 0(0x0)
	IEEE802_DOT1Q_BRIDGE_DUMMY, // 1(0x1)
	IEEE802_DOT1Q_BRIDGE_NSASC, // 2(0x2)
	IEEE802_DOT1Q_BRIDGE_BRIDGES, // 3(0x3)
	IEEE802_DOT1Q_BRIDGE_BRIDGE, // 4(0x4)
	IEEE802_DOT1Q_BRIDGE_NAME, // 5(0x5)
	IEEE802_DOT1Q_BRIDGE_ADDRESS, // 6(0x6)
	IEEE802_DOT1Q_BRIDGE_PORTS, // 7(0x7)
	IEEE802_DOT1Q_BRIDGE_UP_TIME, // 8(0x8)
	IEEE802_DOT1Q_BRIDGE_COMPONENTS, // 9(0x9)
	IEEE802_DOT1Q_BRIDGE_COMPONENT, // 10(0xa)
	IEEE802_DOT1Q_BRIDGE_ID, // 11(0xb)
	IEEE802_DOT1Q_BRIDGE_TRAFFIC_CLASS_ENABLED, // 12(0xc)
	IEEE802_DOT1Q_BRIDGE_BRIDGE_PORT, // 13(0xd)
	IEEE802_DOT1Q_BRIDGE_FILTERING_DATABASE, // 14(0xe)
	IEEE802_DOT1Q_BRIDGE_SIZE, // 15(0xf)
	IEEE802_DOT1Q_BRIDGE_STATIC_ENTRIES, // 16(0x10)
	IEEE802_DOT1Q_BRIDGE_DYNAMIC_ENTRIES, // 17(0x11)
	IEEE802_DOT1Q_BRIDGE_STATIC_VLAN_REGISTRATION_ENTRIES, // 18(0x12)
	IEEE802_DOT1Q_BRIDGE_DYNAMIC_VLAN_REGISTRATION_ENTRIES, // 19(0x13)
	IEEE802_DOT1Q_BRIDGE_MAC_ADDRESS_REGISTRATION_ENTRIES, // 20(0x14)
	IEEE802_DOT1Q_BRIDGE_FILTERING_ENTRY, // 21(0x15)
	IEEE802_DOT1Q_BRIDGE_DATABASE_ID, // 22(0x16)
	IEEE802_DOT1Q_BRIDGE_VIDS, // 23(0x17)
	IEEE802_DOT1Q_BRIDGE_ENTRY_TYPE, // 24(0x18)
	IEEE802_DOT1Q_BRIDGE_PORT_MAP, // 25(0x19)
	IEEE802_DOT1Q_BRIDGE_PORT_REF, // 26(0x1a)
	IEEE802_DOT1Q_BRIDGE_STATIC_FILTERING_ENTRIES, // 27(0x1b)
	IEEE802_DOT1Q_BRIDGE_CONTROL_ELEMENT, // 28(0x1c)
	IEEE802_DOT1Q_BRIDGE_CONNECTION_IDENTIFIER, // 29(0x1d)
	IEEE802_DOT1Q_BRIDGE_REGISTRAR_ADMIN_CONTROL, // 30(0x1e)
	IEEE802_DOT1Q_BRIDGE_VLAN_TRANSMITTED, // 31(0x1f)
	IEEE802_DOT1Q_BRIDGE_DYNAMIC_RESERVATION_ENTRIES, // 32(0x20)
	IEEE802_DOT1Q_BRIDGE_DYNAMIC_FILTERING_ENTRIES, // 33(0x21)
	IEEE802_DOT1Q_BRIDGE_STATUS, // 34(0x22)
	IEEE802_DOT1Q_BRIDGE_VLAN_REGISTRATION_ENTRY, // 35(0x23)
	IEEE802_DOT1Q_BRIDGE_PERMANENT_DATABASE, // 36(0x24)
	IEEE802_DOT1Q_BRIDGE_BRIDGE_VLAN, // 37(0x25)
	IEEE802_DOT1Q_BRIDGE_MAX_VIDS, // 38(0x26)
	IEEE802_DOT1Q_BRIDGE_VLAN, // 39(0x27)
	IEEE802_DOT1Q_BRIDGE_VID, // 40(0x28)
	IEEE802_DOT1Q_BRIDGE_UNTAGGED_PORTS, // 41(0x29)
	IEEE802_DOT1Q_BRIDGE_EGRESS_PORTS, // 42(0x2a)
	IEEE802_DOT1Q_BRIDGE_ENUM_END,
} ieee802_dot1q_bridge_enum_t;

int ieee802_dot1q_bridge_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
