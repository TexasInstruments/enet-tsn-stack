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
#ifndef IETF_KEYCHAIN_H_
#define IETF_KEYCHAIN_H_

#include "yang_db_access.h"

typedef enum {
	IETF_KEYCHAIN_VALUEKEY, // 0(0x0)
	IETF_KEYCHAIN_DUMMY, // 1(0x1)
	IETF_KEYCHAIN_KEYCHAIN, // 2(0x2)
	IETF_KEYCHAIN_PRIVATE_KEYS, // 3(0x3)
	IETF_KEYCHAIN_PRIVATE_KEY, // 4(0x4)
	IETF_KEYCHAIN_NAME, // 5(0x5)
	IETF_KEYCHAIN_ALGORITHM, // 6(0x6)
	IETF_KEYCHAIN_KEY_LENGTH, // 7(0x7)
	IETF_KEYCHAIN_PUBLIC_KEY, // 8(0x8)
	IETF_KEYCHAIN_CERTIFICATES, // 9(0x9)
	IETF_KEYCHAIN_CERTIFICATE, // 10(0xa)
	IETF_KEYCHAIN_CHAIN, // 11(0xb)
	IETF_KEYCHAIN_TRUSTED_CERTIFICATES, // 12(0xc)
	IETF_KEYCHAIN_DESCRIPTION, // 13(0xd)
	IETF_KEYCHAIN_TRUSTED_CERTIFICATE, // 14(0xe)
	// augmented by xl4-keychain
	IETF_KEYCHAIN_LOCATION, // 15(0xf)
	IETF_KEYCHAIN_ENUM_END,
} ietf_keychain_enum_t;

int ietf_keychain_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
