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
/**
 * @addtogroup xl4avbl2
 * @{
 * @file ll_avtp_ethernet.h
 * @brief low layer of avtp ethernet support
 */

#ifndef __LL_AVTP_ETHERNET_H_
#define __LL_AVTP_ETHERNET_H_

#include "avtp_ethernet.h"

/**
 * @brief avtp receiver handler thread
 * @param ptr	reference to the data handle of avtp_ethernet object
 * callback function(rcv_cb) must be called from this thread function.\n
 * When the callback function is called from the other place, this thread
 * function is not needed.
 */
void *ll_avtpe_receive_data(void *ptr);

/**
 * @brief set the avtp_ethernet receive data callback
 * @param fd	file descriptor of the socket
 * @param rcv_cb	callback function
 * @param dbdata	data to be passed to the callback
 * @note Normally, the callback is set in avtp_ethernet_open and
 * ll_avtpe_receive_data thread calls it.\n
 * For non-threading env. or the receiver thread is different from
 * ll_avtpe_receive_data, the callback should be set by this function.
 * @return 0 on success, -1 on error
 */
int ll_ethernet_set_cb(CB_SOCKET_T fd, avtp_ethernet_rcv_cb_t rcv_cb, void *cbdata, uint16_t proto);

/**
 * @brief sends the avtp packet out
 * @param reh the data handle of avtp_ethernet object
 * @param data the avtp packet data
 * @param size the avtp packet data size
 * @return the number of bytes sent on success, -1 on error
 */
int ll_avtpe_send_data(avtp_ethernet_handle_t reh, uint8_t *data, int size);

#endif
/** @}*/
