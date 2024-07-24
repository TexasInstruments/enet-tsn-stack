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
#ifndef XL4_EXTMOD_XL4MRP_H_
#define XL4_EXTMOD_XL4MRP_H_

#include <tsn_uniconf/yangs/yang_db_access.h>

typedef enum {
	XL4_EXTMOD_XL4MRP_VALUEKEY, // 0(0x0)
	XL4_EXTMOD_XL4MRP_DUMMY, // 1(0x1)
	XL4_EXTMOD_XL4MRP_NSASC, // 2(0x2)
	XL4_EXTMOD_XL4MRP_XL4MRP, // 3(0x3)
	XL4_EXTMOD_XL4MRP_XMRP_INSTANCE, // 4(0x4)
	XL4_EXTMOD_XL4MRP_XMRPD_READY, // 5(0x5)
	XL4_EXTMOD_XL4MRP_INSTANCE_INDEX, // 6(0x6)
	XL4_EXTMOD_XL4MRP_MRPNET_SELECT_RES_TIMEOUT, // 7(0x7)
	XL4_EXTMOD_XL4MRP_OVIP_LPORT, // 8(0x8)
	XL4_EXTMOD_XL4MRP_OVIP_DPORT, // 9(0x9)
	XL4_EXTMOD_XL4MRP_DEBUGLOG_MEMORY_SIZE, // 10(0xa)
	XL4_EXTMOD_XL4MRP_DEBUGLOG_MEMORY_FILE, // 11(0xb)
	XL4_EXTMOD_XL4MRP_LEAVE_ALL_TIME, // 12(0xc)
	XL4_EXTMOD_XL4MRP_LEAVE_TIME, // 13(0xd)
	XL4_EXTMOD_XL4MRP_JOIN_TIME, // 14(0xe)
	XL4_EXTMOD_XL4MRP_AVNU_SRPDOMAINBOUNDARY, // 15(0xf)
	XL4_EXTMOD_XL4MRP_LATENCY_INTERNAL_PROCESS, // 16(0x10)
	XL4_EXTMOD_XL4MRP_LATENCY_OUTPORT_WIRE, // 17(0x11)
	XL4_EXTMOD_XL4MRP_BRIDGE_CONFIG_ENABLE, // 18(0x12)
	XL4_EXTMOD_XL4MRP_BRIDGEID, // 19(0x13)
	XL4_EXTMOD_XL4MRP_BRIDGENAME, // 20(0x14)
	XL4_EXTMOD_XL4MRP_NPORT_DATA, // 21(0x15)
	XL4_EXTMOD_XL4MRP_NPINDEX, // 22(0x16)
	XL4_EXTMOD_XL4MRP_DELTABANDWIDTH, // 23(0x17)
	XL4_EXTMOD_XL4MRP_INDEX0, // 24(0x18)
	XL4_EXTMOD_XL4MRP_VALUE, // 25(0x19)
	XL4_EXTMOD_XL4MRP_SRPBOUNDARYPORT, // 26(0x1a)
	XL4_EXTMOD_XL4MRP_STP_UPDATE, // 27(0x1b)
	XL4_EXTMOD_XL4MRP_STP_PORTSTATE, // 28(0x1c)
	XL4_EXTMOD_XL4MRP_STP_PORTROLE, // 29(0x1d)
	XL4_EXTMOD_XL4MRP_STP_TOPOLOGY_CHANGED, // 30(0x1e)
	XL4_EXTMOD_XL4MRP_MMRP_ENABLE, // 31(0x1f)
	XL4_EXTMOD_XL4MRP_MVRP_ENABLE, // 32(0x20)
	XL4_EXTMOD_XL4MRP_MSRP_ENABLE, // 33(0x21)
	XL4_EXTMOD_XL4MRP_MMRP_PERIODIC_ENABLE, // 34(0x22)
	XL4_EXTMOD_XL4MRP_MVRP_PERIODIC_ENABLE, // 35(0x23)
	XL4_EXTMOD_XL4MRP_MSRP_PERIODIC_ENABLE, // 36(0x24)
	XL4_EXTMOD_XL4MRP_SRPDOMAINBOUNDARY_FALSE, // 37(0x25)
	XL4_EXTMOD_XL4MRP_CLASS_SRCLASID_MAP, // 38(0x26)
	XL4_EXTMOD_XL4MRP_TRAFFIC_CLASS_INTERVAL, // 39(0x27)
	XL4_EXTMOD_XL4MRP_ENUM_END,
} xl4_extmod_xl4mrp_enum_t;

int xl4_extmod_xl4mrp_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
