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
 * socket_util.h
 */
#ifndef XL4LLDP_SOCKET_UTIL_H_
#define XL4LLDP_SOCKET_UTIL_H_
#include <tsn_unibase/unibase.h>
#include <tsn_combase/combase.h>
#include <tsn_combase/cb_ethernet.h>

#define MAX_LLDP_SIZE       (1500)

struct _hw_interface;

/// @brief Wrapper Socket structure uses for both TI_LLD and Posix
typedef struct lldp_socket
{
	char name[CB_MAX_NETDEVNAME];         //!< Interface name eth0,enpxs0,.... if_name is copied from yang-lldp
	int fd;                                  //!< For posix, is CB_SOCKET_T, but for TILLD, is MAC Port
	CB_SOCKADDR_LL_T addr;                   //!< Local socket address
	ub_macaddr_t bmac;                       //!< Local MAC address
	size_t mtu;                              //!< MTU size
	uint8_t     recv_buf[MAX_LLDP_SIZE];     //!< Recv buffer
	int    recv_len;                    	 //!< Recv len

	bool is_recv_available;					 //!< This flag is turn ON once rx available
} lldp_socket_t;

/// @brief 
/// @param lldpsock 
/// @param name 
/// @return 
int lldp_raw_socket_open(struct _hw_interface* interface);

/// @brief 
/// @param lldpsock 
/// @return 
int lldp_raw_socket_close(struct _hw_interface* interface);

/// @brief Try receive packet within timeout
/// @param hw_if_list 
/// @param timeout_usec 
/// @return 
int try_recv_packet(struct ub_list* hw_if_list, int timeout_usec);
/// @brief Once txNow is true. This function will be called from txStateMachine
/// @param  
/// @param buf 
/// @param len 
/// @param pcv 
/// @return 
int lldp_send_packet(struct _hw_interface* interface, uint8_t* buf, size_t len, uint16_t pcv);

/// @brief Init static variable inside socket_util (currently specific for TILLD)
/// @param  
int init_socket_utils(netdevname_t *netdevs, int ndev_size);

/// @brief DeInit static variable inside socket_util (currently specific for TILLD)
/// @param  
void deinit_socket_utils(void);

/// @brief Since one port can have multiple cfg port, so each hw_interface just keep the reference to opened socket fd.
void assign_raw_socket_to_port(struct _hw_interface* interface);
#endif