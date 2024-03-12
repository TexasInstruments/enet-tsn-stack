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
#ifndef IEEE802_DOT1Q_TSN_CONFIG_UNI_H_
#define IEEE802_DOT1Q_TSN_CONFIG_UNI_H_

#include "yang_db_access.h"

typedef enum {
	IEEE802_DOT1Q_TSN_CONFIG_UNI_VALUEKEY, // 0(0x0)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_DUMMY, // 1(0x1)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_NSASC, // 2(0x2)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_TSN_UNI, // 3(0x3)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_DOMAIN, // 4(0x4)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_DOMAIN_ID, // 5(0x5)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_CNC_ENABLED, // 6(0x6)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_CUC, // 7(0x7)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_CUC_ID, // 8(0x8)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM, // 9(0x9)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM_ID, // 10(0xa)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM_STATUS, // 11(0xb)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER, // 12(0xc)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_STREAM_RANK, // 13(0xd)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_RANK, // 14(0xe)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_END_STATION_INTERFACES, // 15(0xf)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_MAC_ADDRESS, // 16(0x10)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_NAME, // 17(0x11)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_DATA_FRAME_SPECIFICATION, // 18(0x12)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_INDEX, // 19(0x13)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_MAC_ADDRESSES, // 20(0x14)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_DESTINATION_MAC_ADDRESS, // 21(0x15)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_SOURCE_MAC_ADDRESS, // 22(0x16)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_IEEE802_VLAN_TAG, // 23(0x17)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_PRIORITY_CODE_POINT, // 24(0x18)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_ID, // 25(0x19)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_TRAFFIC_SPECIFICATION, // 26(0x1a)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERVAL, // 27(0x1b)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_NUMERATOR, // 28(0x1c)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_DENOMINATOR, // 29(0x1d)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_MAX_FRAMES_PER_INTERVAL, // 30(0x1e)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_MAX_FRAME_SIZE, // 31(0x1f)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_TRANSMISSION_SELECTION, // 32(0x20)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_TIME_AWARE, // 33(0x21)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_EARLIEST_TRANSMIT_OFFSET, // 34(0x22)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_LATEST_TRANSMIT_OFFSET, // 35(0x23)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_JITTER, // 36(0x24)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_USER_TO_NETWORK_REQUIREMENTS, // 37(0x25)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_NUM_SEAMLESS_TREES, // 38(0x26)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_MAX_LATENCY, // 39(0x27)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CAPABILITIES, // 40(0x28)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_VLAN_TAG_CAPABLE, // 41(0x29)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_CB_STREAM_IDEN_TYPE_LIST, // 42(0x2a)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_CB_SEQUENCE_TYPE_LIST, // 43(0x2b)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_ACCUMULATED_LATENCY, // 44(0x2c)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_CONFIGURATION, // 45(0x2d)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_INTERFACE_LIST, // 46(0x2e)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_CONFIG_LIST, // 47(0x2f)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_TIME_AWARE_OFFSET, // 48(0x30)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_LISTENER, // 49(0x31)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_STATUS_INFO, // 50(0x32)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_TALKER_STATUS, // 51(0x33)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_LISTENER_STATUS, // 52(0x34)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_FAILURE_CODE, // 53(0x35)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_FAILED_INTERFACES, // 54(0x36)
	IEEE802_DOT1Q_TSN_CONFIG_UNI_ENUM_END,
} ieee802_dot1q_tsn_config_uni_enum_t;

int ieee802_dot1q_tsn_config_uni_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
