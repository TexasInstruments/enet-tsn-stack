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
#ifndef IEEE802_DOT1CB_STREAM_IDENTIFICATION_H_
#define IEEE802_DOT1CB_STREAM_IDENTIFICATION_H_

#include "yang_db_access.h"

typedef enum {
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_VALUEKEY, // 0(0x0)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_DUMMY, // 1(0x1)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_NSASC, // 2(0x2)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_STREAM_IDENTITY, // 3(0x3)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_INDEX, // 4(0x4)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_HANDLE, // 5(0x5)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_IN_FACING, // 6(0x6)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_INPUT_PORT, // 7(0x7)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_OUTPUT_PORT, // 8(0x8)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_OUT_FACING, // 9(0x9)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_NULL_STREAM_IDENTIFICATION, // 10(0xa)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_DESTINATION_MAC, // 11(0xb)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_TAGGED, // 12(0xc)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_VLAN, // 13(0xd)
	// augmented by xl4-ieee802-dot1cb-stream-identification
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_CONFIG_STATUS, // 14(0xe)
	IEEE802_DOT1CB_STREAM_IDENTIFICATION_ENUM_END,
} ieee802_dot1cb_stream_identification_enum_t;

int ieee802_dot1cb_stream_identification_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
