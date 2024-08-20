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
/***********************************************************************************
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
***********************************************************************************/

/**
 * @addtogroup xl4avbconl2
 * @{
 * @file conl2_avtpc_listener.h
 * @brief AVB Transport Protocol Client, avtp listener connection, header file
 *
 */

#ifndef __CONL2_AVTPC_LISTENER_H_
#define __CONL2_AVTPC_LISTENER_H_

#include "tsn_l2/avtpc.h"

/**
 * @brief basic parameters for conl2 which contains following members:
 *  1. netdev		->	Ethernet Device name like 'eth0'
 *  2. mcastmac		->	multicast address.
 *  3. streamid		->	string representation of 8-byte stream ID
 *  4. vid		    ->	vlan ID
 *  5. echoback     ->  send data echoback to receive
 *  6. droplate     ->  enable dropping of avtp frames with ts in the past
 */
typedef struct conl2_basic_conparas{
	char netdev[CB_MAX_NETDEVNAME];
	ub_macaddr_t mcastmac;
	ub_streamid_t streamid;
	char shsuf[AVTP_SHSUF_MAX];
	uint16_t vid;
	uint8_t pcp;
	bool echoback;
	bool droplate;
	uint32_t max_frm_sz;
	uint32_t time_intv;
	uint32_t max_intv_frames;
	uint32_t avtpd_bufftime_us;
	int send_ahead_ts;
	bool is_direct;
	int tport; // Use for ovip mode (only use when is_direct is true)
}conl2_basic_conparas_t;

/**
 * @brief conl2 parameters.which contians following members:
 *   1. bc -> conl2_basic_conparas_t which contains basic conl2
 *   	parameters. see conl2_basic_conparas_t for more details.
 *   2. avtpd_bufftime	-> avtpd buffer time in usec unit.
 *   3. subtype		-> subtype defined in avbtp_subtype_def_t
 *   4. lsem_post_time -> avtpc gets event when the buffer is filled
 *   the amount of data correspond to this lsem_post_time in usec
 *   unit.  It gets better response with smaller value, but small
 *   number possibly makes bad impact on the performance.
 *   @see avbtp_subtype_def_t
 *
 */
typedef struct conl2_conparas{
	conl2_basic_conparas_t bc;
	avbtp_subtype_def_t subtype;
	int lsem_post_time;
}conl2_conparas_t;

typedef struct conl2_avtpc_listener_data conl2_avtpc_listener_data_t;

/**
 * @brief initialize conl2_avtpc_listener
 * @param conl2_rec_cb	callback function
 * @param cl2_cbdata	data to passed into the callback function
 * @return the data handle of conl2_avtpc_listener
 * @see conl2_conparas_t
 */
conl2_avtpc_listener_data_t *conl2_avtpc_listener_init(conl2_conparas_t *conl2conp,
						       avbtp_rcv_cb_t conl2_rec_cb,
						       void *cl2_cbdata);

/**
 * @brief close conl2_avtpc_listener
 * @param conl2lad the data handle of conl2_avtpc_listener
 * @return void
 *
 */
void conl2_avtpc_listener_close(conl2_avtpc_listener_data_t *conl2lad);

/**
 * @brief return the preset of common stream header
 * @param conl2lad the data handle of conl2_avtpc_listener
 * @return the preset of common stream header
 */
avbtp_cm_stream_header_t *conl2_avtpc_listener_cmsh(conl2_avtpc_listener_data_t *conl2lad);

#endif

/** @}*/
