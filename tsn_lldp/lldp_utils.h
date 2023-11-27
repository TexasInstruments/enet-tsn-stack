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
/*
 * lldp_utils.h provide some common APIs
 *
 *  Created on: Jun 21, 2023
 *      Author: hoangloc
 */

#ifndef XL4LLDP_LLDP_ULTIL_H_
#define XL4LLDP_LLDP_ULTIL_H_
#include <tsn_unibase/unibase.h>
#include <tsn_uniconf/yangs/ieee802-dot1ab-lldp_access.h>

#define UB_LOGTSTYPE UB_CLOCK_REALTIME

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

/// @brief Extract Dest MAC Address from Eth Buf
/// @param buf 
/// @param pos 
/// @param mac 
void get_dest_mac_addr(const char* buf, int* pos, ub_macaddr_t mac);

/// @brief Extract SRC MAC Address from Eth Buf
/// @param buf 
/// @param pos 
/// @param mac 
void get_src_mac_addr(const char* buf, int* pos, ub_macaddr_t mac);

/// @brief Get Eth Protocol (LLDP)
/// @param buf 
/// @param pos 
/// @return 
uint16_t get_eth_proto(const char* buf, int* pos);

/// @brief Get Current time in millisec
/// @return 
uint32_t get_current_time_in_milisec();

/// @brief Get Current time in microsec
/// @return 
uint64_t get_current_time_in_usec();

/// @brief Convert IPV6 string to array of bytes
/// @param ipv6str 
/// @param buf 
void ipv6_str_to_hex(const char* ipv6str, uint8_t* buf);

/// @brief Convert IPV4 string to array of bytes
/// @param ipv4str 
/// @param buf 
void ipv4_str_to_hex(const char* ipv4str, uint8_t* buf);

/// @brief Convert ipv4 to ascii string
/// @param buf 
/// @param ipv4_str 
void str_to_ipv4(uint8_t* buf, char* ipv4_str);

/// @brief Split string into array by `delim`
/// @param buf 
/// @param delim 
/// @param out 
/// @param expected_no 
void split_str_into_arr(char* buf, char* delim, char** out, int expected_no);

/// @brief convert mac to smac by `delim` instead of fixing `:` like ub_bmac2smac
/// @param bmac 
/// @param smac 
/// @param delim 
char *lldp_bmac2smac(ub_macaddr_t bmac, char *smac, const char delim);
#endif