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
#ifndef YANG_MODULES_H_
#define YANG_MODULES_H_
#include <stddef.h>
#include <inttypes.h>

typedef enum {
	YANG_VTYPE_BINARY,                      //00(0x00), Any binary data
	YANG_VTYPE_BITS,                        //01(0x01), A set of bits or flags
	YANG_VTYPE_BOOLEAN,                     //02(0x02), "true" or "false"
	YANG_VTYPE_DECIMAL64,                   //03(0x03), 64-bit signed decimal number
	YANG_VTYPE_EMPTY,                       //04(0x04), A leaf that does not have any value
	YANG_VTYPE_ENUMERATION,                 //05(0x05), One of an enumerated set of strings
	YANG_VTYPE_IDENTITYREF,                 //06(0x06), A reference to an abstract identity
	YANG_VTYPE_INSTANCE_IDENTIFIER,         //07(0x07), A reference to a data tree node
	YANG_VTYPE_INT8,                        //08(0x08), 8-bit signed integer
	YANG_VTYPE_INT16,                       //09(0x09), 16-bit signed integer
	YANG_VTYPE_INT32,                       //10(0x0A), 32-bit signed integer
	YANG_VTYPE_INT64,                       //11(0x0B), 64-bit signed integer
	YANG_VTYPE_LEAFREF,                     //12(0x0C), A reference to a leaf instance
	YANG_VTYPE_STRING,                      //13(0x0D), A character string
	YANG_VTYPE_UINT8,                       //14(0x0E), 8-bit unsigned integer
	YANG_VTYPE_UINT16,                      //15(0x0F), 16-bit unsigned integer
	YANG_VTYPE_UINT32,                      //16(0x10), 32-bit unsigned integer
	YANG_VTYPE_UINT64,                      //17(0x11), 64-bit unsigned integer
	YANG_VTYPE_UNION,                       //18(0x12), Choice of member types
	YANG_VTYPE_STREAMID,                    //19(0x13), 8-byte stream id, xl4 added
	YANG_VTYPE_YANG_COUNTER8,               //20(0x14), 8-bit counter
	YANG_VTYPE_YANG_COUNTER16,              //21(0x15), 16-bit counter
	YANG_VTYPE_YANG_COUNTER32,              //22(0x16), 32-bit counter
	YANG_VTYPE_YANG_COUNTER64,              //23(0x17), 64-bit counter
	YANG_VTYPE_PRIORITY_TYPE,               //24(0x18), 0..7
	YANG_VTYPE_TRAFFIC_CLASS_TYPE,          //25(0x19), 0..7
	YANG_VTYPE_DOT1QTYPES_PRIORITY_TYPE,    //26(0x1A)
	YANG_VTYPE_INTERFACE_STATE_REF,         //27(0x1B)
	YANG_VTYPE_YANG_DATE_AND_TIME,          //28(0x1C)
	YANG_VTYPE_DOT1QTYPES_VLANID,           //29(0x1D)
	YANG_VTYPE_DOT1QTYPES_PORT_NUMBER_TYPE, //30(0x1E)
	YANG_VTYPE_DOT1QTYPES_MSTID_TYPE,       //31(0x1F)
	YANG_VTYPE_DOT1Q_TYPES_TRAFFIC_CLASS_TYPE,		//32(0x20)
	YANG_VTYPE_DOT1QTYPES_TRAFFIC_CLASS_TYPE,		//33(0x21)
	YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM,		//34(0x22)
	YANG_VTYPE_DOT1QTYPES_VLAN_INDEX_TYPE,		//35(0x23)
	YANG_VTYPE_IEEE_MAC_ADDRESS,		//36(0x24)
	YANG_VTYPE_YANG_PHYS_ADDRESS,		//37(0x25)
	YANG_VTYPE_YANG_GAUGE64,		//38(0x26)
	YANG_VTYPE_DOT1QTYPES_NAME_TYPE,		//39(0x27)
	YANG_VTYPE_DOT1QTYPES_VID_RANGE_TYPE,		//40(0x28)
	YANG_VTYPE_PORT_NUMBER_TYPE,		//41(0x29)
	YANG_VTYPE_YANG_GAUGE32,		//42(0x2A)
	YANG_VTYPE_IF_INTERFACE_REF,		//43(0x2B)
	YANG_VTYPE_YANG_ZERO_BASED_COUNTER32,		//44(0x2C)
	YANG_VTYPE_YANG_TIMETICKS,		//45(0x2D)
	YANG_VTYPE_LLDP_TYPES_MAN_ADDR_TYPE,		//46(0x2E)
	YANG_VTYPE_YANG_TIMESTAMP,		//47(0x2F)
	YANG_VTYPE_IEEE_CHASSIS_ID_SUBTYPE_TYPE,		//48(0x30)
	YANG_VTYPE_IEEE_CHASSIS_ID_TYPE,		//49(0x31)
	YANG_VTYPE_LLDP_TYPES_SYSTEM_CAPABILITIES_MAP,		//50(0x32)
	YANG_VTYPE_LLDP_TYPES_MAN_ADDR_IF_SUBTYPE,		//51(0x33)
	YANG_VTYPE_IEEE_PORT_ID_SUBTYPE_TYPE,		//52(0x34)
	YANG_VTYPE_IEEE_PORT_ID_TYPE,		//53(0x35)
	YANG_VTYPE_CLOCK_IDENTITY,		//54(0x36)
	YANG_VTYPE_TIME_INTERVAL,		//55(0x37)
	YANG_VTYPE_SCALED_NS,		//56(0x38)
	YANG_VTYPE_FLOAT64,		//57(0x39)
	YANG_VTYPE_PORT_STATE,		//58(0x3A)
	YANG_VTYPE_DELAY_MECHANISM,		//59(0x3B)
	YANG_VTYPE_PTP_TIME_INTERVAL,		//60(0x3C)
	YANG_VTYPE_USCALED_NS,		//61(0x3D)
	YANG_VTYPE_HEXUINT16,		//62(0x3E)
	YANG_VTYPE_TSN_STREAM_ID_TYPE,		//63(0x3F)
	YANG_VTYPE_NETCONF_DATASTORE_TYPE,		//64(0x40)
	YANG_VTYPE_INET_URI,		//65(0x41)
	YANG_VTYPE_YANG_XPATH1_0,		//66(0x42)
	YANG_VTYPE_INET_HOST,		//67(0x43)
	YANG_VTYPE_YANG_YANG_IDENTIFIER,		//68(0x44)
	YANG_VTYPE_MRP_PROTOCOL,		//69(0x45)
	YANG_VTYPE_INET_IPV4_ADDRESS,		//70(0x46)
	YANG_VTYPE_INET_IPV6_ADDRESS,		//71(0x47)
	YANG_VTYPE_X509C2N_TLS_FINGERPRINT,		//72(0x48)
	YANG_VTYPE_INET_PORT_NUMBER,		//73(0x49)
	YANG_VTYPE_ENUM_END,
} yang_vtype_enum_t;

#endif
