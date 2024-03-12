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
#ifndef IEEE802_DOT1AB_LLDP_H_
#define IEEE802_DOT1AB_LLDP_H_

#include "yang_db_access.h"

typedef enum {
	IEEE802_DOT1AB_LLDP_VALUEKEY, // 0(0x0)
	IEEE802_DOT1AB_LLDP_DUMMY, // 1(0x1)
	IEEE802_DOT1AB_LLDP_NSASC, // 2(0x2)
	IEEE802_DOT1AB_LLDP_LLDP, // 3(0x3)
	IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX, // 4(0x4)
	IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER, // 5(0x5)
	IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL, // 6(0x6)
	IEEE802_DOT1AB_LLDP_REINIT_DELAY, // 7(0x7)
	IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX, // 8(0x8)
	IEEE802_DOT1AB_LLDP_TX_FAST_INIT, // 9(0x9)
	IEEE802_DOT1AB_LLDP_REMOTE_STATISTICS, // 10(0xa)
	IEEE802_DOT1AB_LLDP_LAST_CHANGE_TIME, // 11(0xb)
	IEEE802_DOT1AB_LLDP_REMOTE_INSERTS, // 12(0xc)
	IEEE802_DOT1AB_LLDP_REMOTE_DELETES, // 13(0xd)
	IEEE802_DOT1AB_LLDP_REMOTE_DROPS, // 14(0xe)
	IEEE802_DOT1AB_LLDP_REMOTE_AGEOUTS, // 15(0xf)
	IEEE802_DOT1AB_LLDP_LOCAL_SYSTEM_DATA, // 16(0x10)
	IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE, // 17(0x11)
	IEEE802_DOT1AB_LLDP_CHASSIS_ID, // 18(0x12)
	IEEE802_DOT1AB_LLDP_SYSTEM_NAME, // 19(0x13)
	IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION, // 20(0x14)
	IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED, // 21(0x15)
	IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED, // 22(0x16)
	IEEE802_DOT1AB_LLDP_PORT, // 23(0x17)
	IEEE802_DOT1AB_LLDP_NAME, // 24(0x18)
	IEEE802_DOT1AB_LLDP_DEST_MAC_ADDRESS, // 25(0x19)
	IEEE802_DOT1AB_LLDP_ADMIN_STATUS, // 26(0x1a)
	IEEE802_DOT1AB_LLDP_TLVS_TX_ENABLE, // 27(0x1b)
	IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS_TX_PORT, // 28(0x1c)
	IEEE802_DOT1AB_LLDP_ADDRESS_SUBTYPE, // 29(0x1d)
	IEEE802_DOT1AB_LLDP_MAN_ADDRESS, // 30(0x1e)
	IEEE802_DOT1AB_LLDP_TX_ENABLE, // 31(0x1f)
	IEEE802_DOT1AB_LLDP_ADDR_LEN, // 32(0x20)
	IEEE802_DOT1AB_LLDP_IF_SUBTYPE, // 33(0x21)
	IEEE802_DOT1AB_LLDP_IF_ID, // 34(0x22)
	IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE, // 35(0x23)
	IEEE802_DOT1AB_LLDP_PORT_ID, // 36(0x24)
	IEEE802_DOT1AB_LLDP_PORT_DESC, // 37(0x25)
	IEEE802_DOT1AB_LLDP_TX_STATISTICS, // 38(0x26)
	IEEE802_DOT1AB_LLDP_TOTAL_FRAMES, // 39(0x27)
	IEEE802_DOT1AB_LLDP_TOTAL_LENGTH_ERRORS, // 40(0x28)
	IEEE802_DOT1AB_LLDP_RX_STATISTICS, // 41(0x29)
	IEEE802_DOT1AB_LLDP_TOTAL_AGEOUTS, // 42(0x2a)
	IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_FRAMES, // 43(0x2b)
	IEEE802_DOT1AB_LLDP_ERROR_FRAMES, // 44(0x2c)
	IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_TLVS, // 45(0x2d)
	IEEE802_DOT1AB_LLDP_TOTAL_UNRECOGNIZED_TLVS, // 46(0x2e)
	IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA, // 47(0x2f)
	IEEE802_DOT1AB_LLDP_TIME_MARK, // 48(0x30)
	IEEE802_DOT1AB_LLDP_REMOTE_INDEX, // 49(0x31)
	IEEE802_DOT1AB_LLDP_REMOTE_TOO_MANY_NEIGHBORS, // 50(0x32)
	IEEE802_DOT1AB_LLDP_REMOTE_CHANGES, // 51(0x33)
	IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS, // 52(0x34)
	IEEE802_DOT1AB_LLDP_ADDRESS, // 53(0x35)
	IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV, // 54(0x36)
	IEEE802_DOT1AB_LLDP_TLV_TYPE, // 55(0x37)
	IEEE802_DOT1AB_LLDP_TLV_INFO, // 56(0x38)
	IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO, // 57(0x39)
	IEEE802_DOT1AB_LLDP_INFO_IDENTIFIER, // 58(0x3a)
	IEEE802_DOT1AB_LLDP_INFO_SUBTYPE, // 59(0x3b)
	IEEE802_DOT1AB_LLDP_INFO_INDEX, // 60(0x3c)
	IEEE802_DOT1AB_LLDP_REMOTE_INFO, // 61(0x3d)
	IEEE802_DOT1AB_LLDP_ENUM_END,
} ieee802_dot1ab_lldp_enum_t;

int ieee802_dot1ab_lldp_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
