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
#ifndef XL4_EXTMOD_XL4AVTP_H_
#define XL4_EXTMOD_XL4AVTP_H_

#include <tsn_uniconf/yangs/yang_db_access.h>

typedef enum {
	XL4_EXTMOD_XL4AVTP_VALUEKEY, // 0(0x0)
	XL4_EXTMOD_XL4AVTP_DUMMY, // 1(0x1)
	XL4_EXTMOD_XL4AVTP_NSASC, // 2(0x2)
	XL4_EXTMOD_XL4AVTP_XL4AVTP, // 3(0x3)
	XL4_EXTMOD_XL4AVTP_AVTP_INSTANCE, // 4(0x4)
	XL4_EXTMOD_XL4AVTP_INSTANCE_INDEX, // 5(0x5)
	XL4_EXTMOD_XL4AVTP_DEBUGLOG_MEMORY_SIZE, // 6(0x6)
	XL4_EXTMOD_XL4AVTP_DEBUGLOG_MEMORY_FILE_PREFIX, // 7(0x7)
	XL4_EXTMOD_XL4AVTP_AVTPD_TRAFFIC_CLASS_NUM, // 8(0x8)
	XL4_EXTMOD_XL4AVTP_AVTPD_PORT_BITRATE, // 9(0x9)
	XL4_EXTMOD_XL4AVTP_AVTPD_PRI_TC_MAP, // 10(0xa)
	XL4_EXTMOD_XL4AVTP_INDEX0, // 11(0xb)
	XL4_EXTMOD_XL4AVTP_VALUE, // 12(0xc)
	XL4_EXTMOD_XL4AVTP_AVTPD_TC_INTERVAL, // 13(0xd)
	XL4_EXTMOD_XL4AVTP_AVTPD_DELTABW, // 14(0xe)
	XL4_EXTMOD_XL4AVTP_AVTPD_IDLESLOPE, // 15(0xf)
	XL4_EXTMOD_XL4AVTP_AVTPD_MAX_NUM_PACKETS, // 16(0x10)
	XL4_EXTMOD_XL4AVTP_AVTPD_READY, // 17(0x11)
	XL4_EXTMOD_XL4AVTP_ENUM_END,
} xl4_extmod_xl4avtp_enum_t;

int xl4_extmod_xl4avtp_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
