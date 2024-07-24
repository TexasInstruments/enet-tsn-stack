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
 * @file aaf_avtpc_talker.h
 * @brief AVB Transport Protocol Client, aaf talker connection, header file
 */

#ifndef __AAF_AVTPC_TALKER_H_
#define __AAF_AVTPC_TALKER_H_

#include "conl2_avtpc_talker.h"
#include "aaf_avtpc_listener.h"

typedef struct aaf_avtpc_talker_data aaf_avtpc_talker_data_t;

/**
 * @brief initialize aaf_avtpc_talker
 * @param bcp	connection parameters
 * @return the data handle of aaf_avtpc_talker
 * @see conl2_basic_conparas_t
 * @see conl2_conparas_t
 */
aaf_avtpc_talker_data_t *aaf_avtpc_talker_init(conl2_basic_conparas_t *bcp);

/**
 * @brief close aaf_avtpc_talker
 * @param aaftad	the data handle of aaf_avtpc_talker
 * @return void
 * @see aaf_avtpc_talker_data_t
 */
void aaf_avtpc_talker_close(aaf_avtpc_talker_data_t *aaftad);

/**
 * @brief set pcm information into the header
 * @param aaftad	the data handle of aaf_avtpc_talker
 * @param pcminfo	set the header from this pcm information
 * @see aaf_avtpc_talker_data_t
 * @see aaf_avtpc_pcminfo_t
 */
void aaf_avtpc_set_pcm_info(aaf_avtpc_talker_data_t *aaftad, aaf_avtpc_pcminfo_t *pcminfo);

/**
 * @brief set pcm information into the header
 * @param aaftad	the data handle of aaf_avtpc_talker
 * @param pcminfo	set the header from this pcm information
 * @see aaf_avtpc_talker_data_t
 * @see aaf_avtpc_pcminfo_t
 */
void aaf_avtpc_set_aes_info(aaf_avtpc_talker_data_t *aaftad, aaf_avtpc_aesinfo_t *aesinfo);
/**
 * @brief send out talker data
 * @param aaftad	the data handle of aaf_avtpc_talker
 * @param pts	timestamp
 * @param data	data to send
 * @param size	size of the data
 * @see aaf_avtpc_talker_data_t
 * @return 0 on success, -1 on error
 */
int aaf_avtpc_talker_write(aaf_avtpc_talker_data_t *aaftad, uint64_t pts,
			   uint8_t *data, int size);

#endif

/** @}*/
