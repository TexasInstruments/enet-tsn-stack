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

// max ports sitara 2
#define MAX_TILLD_PORTS 2

/// These flags are used to calculate number of task to be reserved
/// User should self consider which modules stack should be built
#define GPTP_LIB_ENABLE 1
#define AVTP_LIB_ENABLE 1
#define MRP_LIB_ENABLE 1
#define LLDP_LIB_ENABLE 1

#if (GPTP_LIB_ENABLE == 1)
#include "gptp_buildconf.h"
#else // (GPTP_LIB_ENABLE == 1)
#define GPTP_TASK_NUM   0
#define GPTP_SEM_NUM    0
#define GPTP_EASYARR_DFNUM 0
#define GPTP_EASYARR_INSNUM 0
#define GPTP_SIMPLEDB_DBDATANUM 0
#endif // (GPTP_LIB_ENABLE == 1)

#if (AVTP_LIB_ENABLE == 1)
#include "avtp_buildconf.h"
#else //(AVTP_LIB_ENABLE == 1)
#define AVTP_TASK_NUM   0
#define AVTP_SEM_NUM    0
#define AVTP_SIMPLEDB_DBDATANUM 0
#define AVTP_RX_STACK_NUM 0
#endif //(AVTP_LIB_ENABLE == 1)

#if (MRP_LIB_ENABLE == 1)
#include "mrp_buildconf.h"
#else //(MRP_LIB_ENABLE == 1)
#define MRP_TASK_NUM   0
#define MRP_SEM_NUM    0
#define MRP_EASYARR_DFNUM 0
#define MRP_EASYARR_INSNUM 0
#define MRP_SIMPLEDB_DBDATANUM 0
#endif //(MRP_LIB_ENABLE == 1)

#if (LLDP_LIB_ENABLE == 1)
#include "lldp_buildconf.h"
#else // (LLDP_LIB_ENABLE == 1)
#define LLDP_TASK_NUM   0
#define LLDP_SEM_NUM    0 // rx sem
#define LLDP_EASYARR_DFNUM 0
#define LLDP_EASYARR_INSNUM 0
#define LLDP_SIMPLEDB_DBDATANUM 0
#endif // (LLDP_LIB_ENABLE == 1)

#define UNICONF_TASK_NUM   2 // uniconf main, uc_hwal_catch_events_thread
#define UNICONF_SEM_NUM   3 // simpledb open, uc_notice_init, ydbi_access_init
#define TSNAPP_SEM_NUM   1 // ucReadySem

// Currently the UB_ESARRAY is used by uniconf, mrp, gptp.
// Below estimation can cover all reservation for all applications's stack
// User can reduce these definitions, eg:
// - in case lldp running without needed of building gptp/mrp)
// - in case gptp running without needed of building mrp)
#define MAX_UB_ESARRAY_DFNUM 256
#define MAX_UB_ESARRAY_INSTNUM 40

#define UB_ESARRAY_DFNUM MAX_UB_ESARRAY_DFNUM
#define UB_ESARRAY_INSTNUM MAX_UB_ESARRAY_INSTNUM

#define CB_LLDTASK_INSTNUM (UNICONF_TASK_NUM + \
                            GPTP_TASK_NUM + \
                            AVTP_TASK_NUM + \
                            MRP_TASK_NUM + \
                            LLDP_TASK_NUM)

#define CB_LLDSEM_INSTNUM (UNICONF_SEM_NUM + \
                            TSNAPP_SEM_NUM + \
                            GPTP_SEM_NUM + \
                            AVTP_SEM_NUM + \
                            MRP_SEM_NUM + \
                            LLDP_SEM_NUM)

// To configure CB_LLDTASK_STACK_INSTNUM for the tasks which its stack is created
// inside the combase.
#define CB_LLDTASK_STACK_INSTNUM (AVTP_RX_STACK_NUM)

/// Below params are for tsn-stack internal usage
#define COMBASE_NO_INET
#define COMBASE_NO_CRC
#define COMBASE_NO_IPCSOCK
#define UB_SD_STATIC
#define UC_RUNCONF
#define GENERATE_INITCONFIG
#define SIMPLEDB_DBDATANUM (GPTP_SIMPLEDB_DBDATANUM + \
                           AVTP_SIMPLEDB_DBDATANUM + \
                           MRP_SIMPLEDB_DBDATANUM + \
                           LLDP_SIMPLEDB_DBDATANUM)

#define DISABLE_FAT_FS
#define YANGINIT_GEN_SNUM 1000
#define YANGINIT_GEN_SSIZE 8
#define CB_ETHERNET_NON_POSIX_H "tsn_combase/tilld/cb_lld_ethernet.h"
#define CB_THREAD_NON_POSIX_H "tsn_combase/tilld/cb_lld_thread.h"
#define CB_IPCSHMEM_NON_POSIX_H "tsn_combase/tilld/cb_lld_ipcshmem.h"
#define CB_EVENT_NON_POSIX_H "tsn_combase/tilld/cb_lld_tmevent.h"
#define UB_GETMEM_OVERRIDE_H "tsn_combase/tilld/ub_getmem_override.h"
#define AVTP_PLATFORM_INCLUDE "tsn_l2/tilld/frtos_avtp_include.h"

/* Disable the DEBUG and DEBUGV log level at the compilation time */
#define UB_LOG_COMPILE_LEVEL UBL_INFOV

#endif /* __TSN_TILLD_INCLUDE_H_ */
