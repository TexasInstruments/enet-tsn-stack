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
#ifndef EXCELFORE_TSN_REMOTE_H_
#define EXCELFORE_TSN_REMOTE_H_

#include "yang_db_access.h"

typedef enum {
	EXCELFORE_TSN_REMOTE_VALUEKEY, // 0(0x0)
	EXCELFORE_TSN_REMOTE_DUMMY, // 1(0x1)
	EXCELFORE_TSN_REMOTE_NSASC, // 2(0x2)
	EXCELFORE_TSN_REMOTE_TSN_REMOTE, // 3(0x3)
	EXCELFORE_TSN_REMOTE_INSTANCES, // 4(0x4)
	EXCELFORE_TSN_REMOTE_INSTANCE, // 5(0x5)
	EXCELFORE_TSN_REMOTE_INSTANCE_INDEX, // 6(0x6)
	EXCELFORE_TSN_REMOTE_UPDATE, // 7(0x7)
	EXCELFORE_TSN_REMOTE_PORT, // 8(0x8)
	EXCELFORE_TSN_REMOTE_PROTOCOL, // 9(0x9)
	EXCELFORE_TSN_REMOTE_REQUEST_STATUS, // 10(0xa)
	EXCELFORE_TSN_REMOTE_RESULT_STATUS, // 11(0xb)
	EXCELFORE_TSN_REMOTE_EXTERNAL_CONTROL_MAN, // 12(0xc)
	EXCELFORE_TSN_REMOTE_CONTROL_OBJECTS, // 13(0xd)
	EXCELFORE_TSN_REMOTE_EXTERNAL_CONTROL, // 14(0xe)
	EXCELFORE_TSN_REMOTE_INDICATION_CHANGE_COUNTER, // 15(0xf)
	EXCELFORE_TSN_REMOTE_MSRP_TA, // 16(0x10)
	EXCELFORE_TSN_REMOTE_INDICATION_LIST, // 17(0x11)
	EXCELFORE_TSN_REMOTE_STREAM_ID, // 18(0x12)
	EXCELFORE_TSN_REMOTE_DEST_ADDRESS, // 19(0x13)
	EXCELFORE_TSN_REMOTE_VLAN_ID, // 20(0x14)
	EXCELFORE_TSN_REMOTE_MAX_FRAME_SIZE, // 21(0x15)
	EXCELFORE_TSN_REMOTE_MAX_INTERVAL_FRAMES, // 22(0x16)
	EXCELFORE_TSN_REMOTE_PCP, // 23(0x17)
	EXCELFORE_TSN_REMOTE_RANK, // 24(0x18)
	EXCELFORE_TSN_REMOTE_ACCUMULATED_LATENCY, // 25(0x19)
	EXCELFORE_TSN_REMOTE_ADMIN_REQUEST_LIST, // 26(0x1a)
	EXCELFORE_TSN_REMOTE_OPER_REQUEST_LIST, // 27(0x1b)
	EXCELFORE_TSN_REMOTE_MSRP_TF, // 28(0x1c)
	EXCELFORE_TSN_REMOTE_SYSTEM_ID, // 29(0x1d)
	EXCELFORE_TSN_REMOTE_FAILUER_CODE, // 30(0x1e)
	EXCELFORE_TSN_REMOTE_MSRP_TE, // 31(0x1f)
	EXCELFORE_TSN_REMOTE_STREAM_RANK, // 32(0x20)
	EXCELFORE_TSN_REMOTE_END_STATION_INTERFACES, // 33(0x21)
	EXCELFORE_TSN_REMOTE_MAC_ADDRESS, // 34(0x22)
	EXCELFORE_TSN_REMOTE_INTERFACE_NAME, // 35(0x23)
	EXCELFORE_TSN_REMOTE_DATA_FRAME_SPECIFICATION, // 36(0x24)
	EXCELFORE_TSN_REMOTE_INDEX, // 37(0x25)
	EXCELFORE_TSN_REMOTE_IEEE802_MAC_ADDRESSES, // 38(0x26)
	EXCELFORE_TSN_REMOTE_DESTINATION_MAC_ADDRESS, // 39(0x27)
	EXCELFORE_TSN_REMOTE_SOURCE_MAC_ADDRESS, // 40(0x28)
	EXCELFORE_TSN_REMOTE_IEEE802_VLAN_TAG, // 41(0x29)
	EXCELFORE_TSN_REMOTE_PRIORITY_CODE_POINT, // 42(0x2a)
	EXCELFORE_TSN_REMOTE_IPV4_TUPLE, // 43(0x2b)
	EXCELFORE_TSN_REMOTE_SOURCE_IP_ADDRESS, // 44(0x2c)
	EXCELFORE_TSN_REMOTE_DESTINATION_IP_ADDRESS, // 45(0x2d)
	EXCELFORE_TSN_REMOTE_DSCP, // 46(0x2e)
	EXCELFORE_TSN_REMOTE_SOURCE_PORT, // 47(0x2f)
	EXCELFORE_TSN_REMOTE_DESTINATION_PORT, // 48(0x30)
	EXCELFORE_TSN_REMOTE_IPV6_TUPLE, // 49(0x31)
	EXCELFORE_TSN_REMOTE_TRAFFIC_SPECIFICATION, // 50(0x32)
	EXCELFORE_TSN_REMOTE_INTERVAL, // 51(0x33)
	EXCELFORE_TSN_REMOTE_NUMERATOR, // 52(0x34)
	EXCELFORE_TSN_REMOTE_DENOMINATOR, // 53(0x35)
	EXCELFORE_TSN_REMOTE_MAX_FRAMES_PER_INTERVAL, // 54(0x36)
	EXCELFORE_TSN_REMOTE_TRANSMISSION_SELECTION, // 55(0x37)
	EXCELFORE_TSN_REMOTE_TIME_AWARE, // 56(0x38)
	EXCELFORE_TSN_REMOTE_EARLIEST_TRANSMIT_OFFSET, // 57(0x39)
	EXCELFORE_TSN_REMOTE_LATEST_TRANSMIT_OFFSET, // 58(0x3a)
	EXCELFORE_TSN_REMOTE_JITTER, // 59(0x3b)
	EXCELFORE_TSN_REMOTE_USER_TO_NETWORK_REQUIREMENTS, // 60(0x3c)
	EXCELFORE_TSN_REMOTE_NUM_SEAMLESS_TREES, // 61(0x3d)
	EXCELFORE_TSN_REMOTE_MAX_LATENCY, // 62(0x3e)
	EXCELFORE_TSN_REMOTE_INTERFACE_CAPABILITIES, // 63(0x3f)
	EXCELFORE_TSN_REMOTE_VLAN_TAG_CAPABLE, // 64(0x40)
	EXCELFORE_TSN_REMOTE_CB_STREAM_IDEN_TYPE_LIST, // 65(0x41)
	EXCELFORE_TSN_REMOTE_CB_SEQUENCE_TYPE_LIST, // 66(0x42)
	EXCELFORE_TSN_REMOTE_INTERFACE_CONFIGURATION, // 67(0x43)
	EXCELFORE_TSN_REMOTE_INTERFACE_LIST, // 68(0x44)
	EXCELFORE_TSN_REMOTE_CONFIG_LIST, // 69(0x45)
	EXCELFORE_TSN_REMOTE_TIME_AWARE_OFFSET, // 70(0x46)
	EXCELFORE_TSN_REMOTE_STATUS_INFO, // 71(0x47)
	EXCELFORE_TSN_REMOTE_TALKER_STATUS, // 72(0x48)
	EXCELFORE_TSN_REMOTE_LISTENER_STATUS, // 73(0x49)
	EXCELFORE_TSN_REMOTE_FAILURE_CODE, // 74(0x4a)
	EXCELFORE_TSN_REMOTE_FAILED_INTERFACES, // 75(0x4b)
	EXCELFORE_TSN_REMOTE_MSRP_LS, // 76(0x4c)
	EXCELFORE_TSN_REMOTE_FOUR_PACKED_EVENT, // 77(0x4d)
	EXCELFORE_TSN_REMOTE_MSRP_LE, // 78(0x4e)
	EXCELFORE_TSN_REMOTE_MSRP_DM, // 79(0x4f)
	EXCELFORE_TSN_REMOTE_SRCLASS_ID, // 80(0x50)
	EXCELFORE_TSN_REMOTE_SRCLASS_PRIORITY, // 81(0x51)
	EXCELFORE_TSN_REMOTE_SRCLASS_VID, // 82(0x52)
	EXCELFORE_TSN_REMOTE_MVRP, // 83(0x53)
	EXCELFORE_TSN_REMOTE_ENUM_END,
} excelfore_tsn_remote_enum_t;

int excelfore_tsn_remote_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
