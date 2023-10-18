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
#ifndef EXCELFORE_NETCONF_SERVER_H_
#define EXCELFORE_NETCONF_SERVER_H_

#include "yang_db_access.h"

typedef enum {
	EXCELFORE_NETCONF_SERVER_VALUEKEY, // 0(0x0)
	EXCELFORE_NETCONF_SERVER_DUMMY, // 1(0x1)
	EXCELFORE_NETCONF_SERVER_X4NCONF_SERVER, // 2(0x2)
	EXCELFORE_NETCONF_SERVER_TRANSPORT, // 3(0x3)
	EXCELFORE_NETCONF_SERVER_TLS, // 4(0x4)
	EXCELFORE_NETCONF_SERVER_PORT, // 5(0x5)
	EXCELFORE_NETCONF_SERVER_SERVER_CERT, // 6(0x6)
	EXCELFORE_NETCONF_SERVER_SERVER_KEY, // 7(0x7)
	EXCELFORE_NETCONF_SERVER_CAPATH, // 8(0x8)
	EXCELFORE_NETCONF_SERVER_CERT_TO_NAME, // 9(0x9)
	EXCELFORE_NETCONF_SERVER_ID, // 10(0xa)
	EXCELFORE_NETCONF_SERVER_FINGERPRINT, // 11(0xb)
	EXCELFORE_NETCONF_SERVER_MAP_TYPE, // 12(0xc)
	EXCELFORE_NETCONF_SERVER_NAME, // 13(0xd)
	EXCELFORE_NETCONF_SERVER_SSH, // 14(0xe)
	EXCELFORE_NETCONF_SERVER_USE_UNIX_DOMAIN_SOCKET, // 15(0xf)
	EXCELFORE_NETCONF_SERVER_SUB_IPC_PORT, // 16(0x10)
	EXCELFORE_NETCONF_SERVER_SUB_SOCKET_NAME, // 17(0x11)
	EXCELFORE_NETCONF_SERVER_ENUM_END,
} excelfore_netconf_server_enum_t;

int excelfore_netconf_server_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
