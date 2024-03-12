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

// definitions to use the combase headers
#define COMBASE_NO_INET
#define COMBASE_NO_CRC
#define COMBASE_NO_IPCSOCK
#define UB_SD_STATIC
#define UC_RUNCONF

#define UB_ESARRAY_DFNUM 256
#define SIMPLEDB_DBDATANUM 1600

#define CB_ETHERNET_NON_POSIX_H "tsn_combase/tilld/cb_lld_ethernet.h"
#define CB_THREAD_NON_POSIX_H "tsn_combase/tilld/cb_lld_thread.h"
#define CB_IPCSHMEM_NON_POSIX_H "tsn_combase/tilld/cb_lld_ipcshmem.h"
#define CB_EVENT_NON_POSIX_H "tsn_combase/tilld/cb_lld_tmevent.h"
#define UB_GETMEM_OVERRIDE_H "tsn_combase/tilld/ub_getmem_override.h"

#define UB_LOG_COMPILE_LEVEL UBL_INFOV

/* These macros are used in gptpcommon.h to alloc the static memory for gptp2d */
#define GPTP_MAX_PORTS 4
#define GPTP_MAX_DOMAINS 2
#define GPTP_MEDIUM_EXTRA_SIZE 1642 /* Optimize to use minimal of memory */

/*LLDP Definition*/
// Each port can have 3 LLDP agents     
// Nearest bridge agent. Dest MAC 0x0180-C200-000E 
// Nearest customer bridge agent. Dest MAC 0x0180-C200-0000 
// Nearest non-TPMR bridge agent. Dest MAC 0x0180-C200-0003
#define LLDP_CFG_PORT_INSTNUM (4 * 3)

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
