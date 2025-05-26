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
#ifndef __AVTP_BUILDCONF_H_
#define __AVTP_BUILDCONF_H_

// Each tx will take one task (thread)
#define AVB_TALKER_STREAMS_NUMBER 2

// There is only one task to receive multiple (interleaved) rx streams
#define AVB_LISTENER_STREAMS_NUMBER 1

/* When RX zero copy is used, clear this flag to reduce the lib size */
#define AVTP_USE_TILLD_RX_ZERO_COPY 1

#define AVTP_TASK_NUM   (AVB_TALKER_STREAMS_NUMBER + AVB_LISTENER_STREAMS_NUMBER)
#define AVTP_SEM_NUM    2 // ydbi_access_init, rx sem

// Defines the number of Ethernet buffers avtpd can allocate for RX packets when zero-copy mode is disabled
// Set this to match the maximum number of Ethernet interfaces.
// If zero-copy mode is enabled, set to 0 since no buffer is needed.
#define AVTP_ETHPKT_NUM 0

// Defines the number of Ethernet buffers avtpc direct mode can allocate for RX packets when zero-copy mode is disabled.
// Set this to match the maximum number of Ethernet interfaces.
// If zero-copy mode is enabled, set to 0 since no buffer is needed.
#define AVTPC_RXDIRECT_ETHPKT_NUM 0

// This define of maximum number of avtpc instance number, this flag is apply for client which
// use diag feature or access to uniconf db, in our case, always set this to zero
#define AVTPC_CONFIG_INST_NUM 0

// This is number of SIMPLEDB entries used by AVTP
// Increase this number if the entries are not enough for your application.
#define AVTP_SIMPLEDB_DBDATANUM (50)

// Avtp RX: alwas 1 (even if there are multiple interfaces tilld0,1,..)
#define AVTP_RX_STACK_NUM (1)

#endif //__AVTP_BUILDCONF_H_