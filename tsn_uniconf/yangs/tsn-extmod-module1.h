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
#ifndef XL4_EXTMOD_MODULE1_H_
#define XL4_EXTMOD_MODULE1_H_

#include "yang_db_access.h"

typedef enum {
	XL4_EXTMOD_MODULE1_VALUEKEY, // 0(0x0)
	XL4_EXTMOD_MODULE1_DUMMY, // 1(0x1)
	XL4_EXTMOD_MODULE1_MODULE1, // 2(0x2)
	XL4_EXTMOD_MODULE1_INTEGERS, // 3(0x3)
	XL4_EXTMOD_MODULE1_VALUE_I8, // 4(0x4)
	XL4_EXTMOD_MODULE1_VALUE_I16, // 5(0x5)
	XL4_EXTMOD_MODULE1_VALUE_I32, // 6(0x6)
	XL4_EXTMOD_MODULE1_VALUE_I64, // 7(0x7)
	XL4_EXTMOD_MODULE1_READONLY, // 8(0x8)
	XL4_EXTMOD_MODULE1_ROVALUE_I8, // 9(0x9)
	XL4_EXTMOD_MODULE1_ROVALUE_I16, // 10(0xa)
	XL4_EXTMOD_MODULE1_ROVALUE_I32, // 11(0xb)
	XL4_EXTMOD_MODULE1_ROVALUE_I64, // 12(0xc)
	XL4_EXTMOD_MODULE1_UINTEGERS, // 13(0xd)
	XL4_EXTMOD_MODULE1_VALUE_U8, // 14(0xe)
	XL4_EXTMOD_MODULE1_VALUE_U16, // 15(0xf)
	XL4_EXTMOD_MODULE1_VALUE_U32, // 16(0x10)
	XL4_EXTMOD_MODULE1_VALUE_U64, // 17(0x11)
	XL4_EXTMOD_MODULE1_VALUE_BOOL, // 18(0x12)
	XL4_EXTMOD_MODULE1_VALUE_MAC_ADDRESS, // 19(0x13)
	XL4_EXTMOD_MODULE1_VALUE_STREAMID, // 20(0x14)
	XL4_EXTMOD_MODULE1_VALUE_STRING, // 21(0x15)
	XL4_EXTMOD_MODULE1_VALUE_LISTEMPTY1, // 22(0x16)
	XL4_EXTMOD_MODULE1_INDEX0, // 23(0x17)
	XL4_EXTMOD_MODULE1_VALUE, // 24(0x18)
	XL4_EXTMOD_MODULE1_VALUE_LISTEMPTY2, // 25(0x19)
	XL4_EXTMOD_MODULE1_INDEX1, // 26(0x1a)
	XL4_EXTMOD_MODULE1_VALUE_LIST1, // 27(0x1b)
	XL4_EXTMOD_MODULE1_VALUE_LIST2, // 28(0x1c)
	XL4_EXTMOD_MODULE1_ENUM_END,
} xl4_extmod_module1_enum_t;

int xl4_extmod_module1_config_init(xl4_data_data_t *xdd, uc_dbald *dbald, uc_hwald *hwald);

#endif
