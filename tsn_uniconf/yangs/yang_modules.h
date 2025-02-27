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
	// VTYPE for 'leaf-list' in 0xC0 to 0xFE
	YANG_VTYPE_LEAF_LIST_START=0xC0,
	YANG_VTYPE_BINARY_LEAF_LIST=YANG_VTYPE_LEAF_LIST_START,	//192(0xC0)
	YANG_VTYPE_BITS_LEAF_LIST,                //193(0xc1), A set of bits or flags
	YANG_VTYPE_BOOLEAN_LEAF_LIST,             //194(0xc2), "true" or "false"
	YANG_VTYPE_DECIMAL64_LEAF_LIST,           //195(0xc3), 64-bit signed decimal number
	YANG_VTYPE_EMPTY_LEAF_LIST,               //196(0xc4), A leaf that does not have any value
	YANG_VTYPE_ENUMERATION_LEAF_LIST,         //197(0xc5), One of an enumerated set of strings
	YANG_VTYPE_IDENTITYREF_LEAF_LIST,         //198(0xc6), A reference to an abstract identity
	YANG_VTYPE_INSTANCE_IDENTIFIER_LEAF_LIST, //199(0xc7), A reference to a data tree node
	YANG_VTYPE_INT8_LEAF_LIST,                //200(0xc8), 8-bit signed integer
	YANG_VTYPE_INT16_LEAF_LIST,               //201(0xc9), 16-bit signed integer
	YANG_VTYPE_INT32_LEAF_LIST,               //202(0xcA), 32-bit signed integer
	YANG_VTYPE_INT64_LEAF_LIST,               //203(0xcB), 64-bit signed integer
	YANG_VTYPE_STRING_LEAF_LIST,              //204(0xcD), A character string
	YANG_VTYPE_UINT8_LEAF_LIST,               //205(0xcE), 8-bit unsigned integer
	YANG_VTYPE_UINT16_LEAF_LIST,              //206(0xcF), 16-bit unsigned integer
	YANG_VTYPE_UINT32_LEAF_LIST,              //207(0xd0), 32-bit unsigned integer
	YANG_VTYPE_UINT64_LEAF_LIST,              //208(0xd1), 64-bit unsigned integer
	YANG_VTYPE_UNION_LEAF_LIST,               //209(0xd2), Choice of member types
	YANG_VTYPE_STREAMID_LEAF_LIST,            //210(0xd3), 8-byte stream id, xl4 added
	YANG_VTYPE_HEXUINT16_LEAF_LIST,           //211(0xd4), string, 4-digit hex
	YANG_VTYPE_STREAM_ID_TYPE_LEAF_LIST,	  //212(0xd5), 8-byte stream id, ieee-tsn format

	// VTYPE for 'leaf' in 0x00 to 0xBF
	YANG_VTYPE_BINARY=0,                    //00(0x00), Any binary data
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
	YANG_VTYPE_STRING,                      //12(0x0C), A character string
	YANG_VTYPE_UINT8,                       //13(0x0D), 8-bit unsigned integer
	YANG_VTYPE_UINT16,                      //14(0x0E), 16-bit unsigned integer
	YANG_VTYPE_UINT32,                      //15(0x0F), 32-bit unsigned integer
	YANG_VTYPE_UINT64,                      //16(0x10), 64-bit unsigned integer
	YANG_VTYPE_UNION,                       //17(0x11), Choice of member types
	YANG_VTYPE_STREAMID,                    //18(0x12), 8-byte stream id, xl4 added
	YANG_VTYPE_HEXUINT16,                   //19(0x13), string, 4-digit hex
	YANG_VTYPE_STREAM_ID_TYPE,		//20(0x14), 8-byte stream id, ieee-tsn format
	YANG_VTYPE_MAC_ADDRESS,			//21(0x15), 6-byte mac address, '-' separated
	YANG_VTYPE_PHYS_ADDRESS,		//22(0x16), 6-byte mac address, ':' separated
	YANG_VTYPE_VID_RANGE_TYPE,		//23(0x17), list of 2 16-bit values
	YANG_VTYPE_TRANSPORT_STATUS,		//24(0x18), enum type
	YANG_VTYPE_TLS_FINGERPRINT,		//25(0x19), hex string, ':' separated
	YANG_VTYPE_NETCONF_DATASTORE_TYPE,	//26(0x1A), enum type
	YANG_VTYPE_END_STATION_STATUS,		//27(0x1B), enum type
	YANG_VTYPE_ENUM_END,
} yang_vtype_enum_t;

#endif
