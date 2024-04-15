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
#ifndef __TSN_TILLD_INCLUDE_H_
#define __TSN_TILLD_INCLUDE_H_

#define COMBASE_NO_INET
#define COMBASE_NO_CRC
#define COMBASE_NO_IPCSOCK
#define UB_SD_STATIC
#define UC_RUNCONF
#define UB_ESARRAY_DFNUM 256

#define CB_NOIPCSHMEM_DFSIZE 32

#define SIMPLEDB_DBDATANUM 1600

// Default values in case of unuse avtp
//#define CB_LLDTASK_INSTNUM 10
//#define CB_LLDSEM_INSTNUM 12
//#define CB_LLDTASK_STACK_INSTNUM 1

#define TSN_USE_LOG_BUFFER 0

#if TSN_USE_LOG_BUFFER == 1
#define LOG_TASK_NUM 1
#else
#define LOG_TASK_NUM 0
#endif
// To enable up to 2 avtp applications, use below configurations:
// Note: Increase the configuration values will cause memory size increased.
// Number of task(s) to be used:

// To configure CB_LLDTASK_INSTNUM
// Log task: LOG_TASK_NUM
// Per system: uniconf, gptp2, avtpd, uc_hwal_catch_events_thread (total 4)
// Avtp RX: alwas 1 (even if there are multiple interfaces tilld0,1,..)
// Per stream (+2 per app):
//      +1 per avtp apps (talker or listener)
//      +1 avtpc connection handle if the stream is listener
//      +1 avtpd connection handle if the stream is the talker
// => To enable 1 avtp app: we need 4 + 1 + 2 = 7 tasks
// => To enable 2 avtp app: we need 4 + 1 + 2*2 = 9 tasks
#define CB_LLDTASK_INSTNUM (9 + LOG_TASK_NUM)

// To configure CB_LLDTASK_STACK_INSTNUM for the tasks which its stack is created
// inside the combase.
// Per system: uc_hwal_catch_events_thread (total: 1)
// Avtp RX: alwas 1 (even if there are multiple interfaces tilld0,1,..)
// Per stream (+1 per app):
//      +1 avtpc connection handle if the stream is listener
//      +1 avtpd connection handle if the stream is the talker
// => To enable 1 avtp app: we need 1 + 1 + 1 = 3 stacks
// => To enable 2 avtp app: we need 1 + 1 + 1*2 = 4 stacks
#define CB_LLDTASK_STACK_INSTNUM 4

// To configure CB_LLDSEM_INSTNUM (CB_SEM_INIT)
// Per system:
//      tsnapp  (total 2)
//          ucReadySem
//          g_avtpd_ready_sem(in case avtp enabled) +1)
//      uniconf (total 3)
//          simpledb open
//          uc_notice_init
//          ydbi_access_init
//      gptp2   (total 3)
//          ydbi_access_init
//          ifup/down notice
//          rx sem
//      avtpd   (total 4)
//          ydbi_access_init
//          avtpd_init sem_request/sem_response + 2
//          rx sem + 1
// Per stream:
//      avtpd (+4 per stream tsem_set/tsem_rel/rsem_set/rsem_rel)
// => To enable 1 avtp app in tilld0: we need 2 + 3 + 3 + 4 + 4*1 = 16 SEMs
// => To enable 2 avtp app in tilld0: we need 2 + 3 + 3 + 4 + 4*2 = 20 SEMs
#define CB_LLDSEM_INSTNUM 20

#define AVTP_ETHPKT_NUM 1
#define DISABLE_FAT_FS
#define CB_ETHERNET_NON_POSIX_H "tsn_combase/tilld/cb_lld_ethernet.h"
#define CB_THREAD_NON_POSIX_H "tsn_combase/tilld/cb_lld_thread.h"
#define CB_IPCSHMEM_NON_POSIX_H "tsn_combase/tilld/cb_lld_ipcshmem.h"
#define CB_EVENT_NON_POSIX_H "tsn_combase/tilld/cb_lld_tmevent.h"
#define UB_GETMEM_OVERRIDE_H "tsn_combase/tilld/ub_getmem_override.h"
#define AVTP_PLATFORM_INCLUDE "tsn_l2/tilld/frtos_avtp_include.h"

/* Disable the DEBUG and DEBUGV log level at the compilation time */
#define UB_LOG_COMPILE_LEVEL UBL_INFOV

/* Interval timeout in nanoseconds used to generate timers in GPTP.
 * Supported values are 125, 62.5, 31.25, 15.625 and 7.8125 milliseconds. */
//#define GPTPNET_INTERVAL_TIMEOUT_NSEC 15625000u

/* These macros are used in gptpcommon.h to alloc the static memory for gptp2d */
#define GPTP_MAX_PORTS 2
#define GPTP_MAX_DOMAINS 1
#define GPTP_MEDIUM_EXTRA_SIZE 1642 /* Optimize to use minimal of memory */

/*LLDP Definition*/
// Each port can have 3 LLDP agents
// Nearest bridge agent. Dest MAC 0x0180-C200-000E
// Nearest customer bridge agent. Dest MAC 0x0180-C200-0000
// Nearest non-TPMR bridge agent. Dest MAC 0x0180-C200-0003
#define LLDP_CFG_PORT_INSTNUM (2 * 3)

// LLDP system has one timer to check db change
// Each agent need 5 timers (txinterval, txtick, txshutdownwhile, agedout_monitor and too many neighbor )
// MAX timers needed is 5 * LLDP_CFG_PORT_INSTNUM + 1 = 31
#define CB_XTIMER_TMNUM ((LLDP_CFG_PORT_INSTNUM * 5) + 1)

// The information below apply  for max length of
// - Local Chassis ID,
// - Local Port ID,
// - Local Port Description
// - Local System name
// - Local System Description
#define LLDP_LOCAL_INFO_STRING_MAX_LEN 20

// The information below apply  for max length of remote info
// - Chassis ID
// - Port ID
// - Port Description
// - System name
// - System Description
#define LLDP_REMOTE_INFO_STRING_MAX_LEN 256

// The information below apply  for max length of remote unknown TLV info
// - Remote unknown TLV
#define MAX_RM_UNKNOWN_TLV_INFO_LEN    64

// The information below apply  for max length of Remote organization info
// - Remote organization info TLV
#define MAX_RM_ORG_INFO_LEN  64

/* LLDP Definition End */

#endif /* __TSN_TILLD_INCLUDE_H_ */
