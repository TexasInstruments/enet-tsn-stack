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
#define COMBASE_NO_XTIMER
#define COMBASE_NO_IPCSOCK
#define UB_SD_STATIC
#define UC_RUNCONF
// there are 25 state machines and each uses 2 UD_SD_MALLOC, 3 state machines with 1 UD_SD_MALLOC
// With SM_DATA_FSIZE above, 9 fragments each can work for a single domain and a single port.
// Increasing ports/domain, 7 fragments each port, 12 fragments for domain/2ports
// (25*2+3)*(9+7*[number of extra ports - 1]+12*[number of domains - 1])
// The default is set here is for 2 ports and 2 domains
#define SM_DATA_FNUM_DEFAULT (((25u*2u)+3u)*(9u+7u+12u))
#define SM_DATA_FNUM (2*SM_DATA_FNUM_DEFAULT) // 4 ports

// 80 fragments can work for a single domain and a single port.
// Increasing ports/domain, 12 fragments each port, 12 fragments for domain/2ports
// The default is set here is for 2 ports and 2 domains
#define GPTP_SMALL_AFNUM_DEFAULT (80u+12u+12u)
#define GPTP_SMALL_AFNUM (2*GPTP_SMALL_AFNUM_DEFAULT) // 4 ports


// 135 fragments can work for a single domain and a single port.
// Increasing ports/domain, 65 fragments each port, 125 fragment for domain/2ports
// The default is set here is for 2 ports and 2 domains
#define GPTP_MEDIUM_AFNUM_DEFAULT (135u+65u+125u)
#define GPTP_MEDIUM_AFNUM (2*GPTP_MEDIUM_AFNUM_DEFAULT) // 4 ports
#define UB_ESARRAY_DFNUM 256

#define CB_ETHERNET_NON_POSIX_H "tsn_combase/tilld/cb_lld_ethernet.h"
#define CB_THREAD_NON_POSIX_H "tsn_combase/tilld/cb_lld_thread.h"
#define CB_IPCSHMEM_NON_POSIX_H "tsn_combase/tilld/cb_lld_ipcshmem.h"
#define CB_EVENT_NON_POSIX_H "tsn_combase/tilld/cb_lld_tmevent.h"
#define UB_GETMEM_OVERRIDE_H "tsn_combase/tilld/ub_getmem_override.h"

#endif /* __TSN_TILLD_INCLUDE_H_ */
