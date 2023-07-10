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
 * @ingroup  TSN_COMBASE_MODULE
 * @defgroup IPC_sock Socket IPC utility functions
 * @{
 * @file cb_ipcsock.h
 *
 * @brief Socket IPC utility functions
 */

#ifndef CB_IPCSOCK_H_
#define CB_IPCSOCK_H_

typedef enum {
	CB_IPCCLIENT_DEFAULT = 0,
	CB_IPCCLIENT_BINARY,
	CB_IPCCLIENT_TEXT,
	CB_IPCCLIENT_LAST, // layer private numbers after here
} cb_ipcclient_commode_t;

/**
 * @brief read from file descriptor with timeout
 * @return 0 on timeout, -1 on error, positive number on read size
 * @param fd	a file descriptor
 * @param data	buffer to read data, which must have more than size bytes
 * @param size	read size
 * @param tout_ms	timeout time in milliseconds
 */
int cb_fdread_timeout(int fd, void *data, int size, int tout_ms);

/**
 * @brief creats and initializes Unix Domain Socket for IPC
 * @return 0 on success, -1 on error
 * @param ipcfd	return opened file descriptor
 * @param node	main part of file node i.e /temp/node
 * @param suffix	suffix part of file node, set "" for non suffix
 * @param server_node	if NULL, create a socket without connection. \n
 * if not NULL, connect to this server_node(must be existing node).
 */
int cb_ipcsocket_init(CB_SOCKET_T *ipcfd, char *node, char *suffix, char *server_node);

/**
 * @brief open Unix Domain Socket in UDP mode for IPC
 * @return 0 on success, -1 on error.
 * @param ipcfd	return opened file descriptor for socket.
 * @param own_ip	NULL for any IF, '127.0.0.1' for local only
 * @param server_ip	NULL in the server mode, set IP in the client mode
 * @param server_port	port number on which the server mode listens
 */
int cb_ipcsocket_udp_init(CB_SOCKET_T *ipcfd, char *own_ip, char *server_ip, int server_port);

/**
 * @brief close Unix Domain Socket for IPC
 * @return 0 on success, -1 on error
 * @param ipcfd	ipc file descriptor
 * @param node	main part of file node
 * @param suffix	suffix part of file node, set "" for non suffix
 * @note this call unlink the node name of node+suffix
 */
int cb_ipcsocket_close(CB_SOCKET_T ipcfd, char *node, char *suffix);

/*
 * this number of IPC clients can be allowed to connect.
 * 2-connection memory data is expanded by increasing of connections,
 * and when the disconnection is detected the data is removed and freed.
 */
#define MAX_IPC_CLIENTS 16
typedef struct cb_ipcserverd cb_ipcserverd_t;

/**
 * @brief initialize the server mode ipc socket
 * @return the data handle
 * @param node_ip	unix domain socket file node name OR udp socket port IP address
 * @param suffix	suffix part of file node, set "" for non suffix
 * @param port	the local port number for udp mode connection. Set 0 for unix domain socket.
 */
cb_ipcserverd_t *cb_ipcsocket_server_init(char *node_ip, char *suffix, uint16_t port);

/**
 * @brief close the server mode ipc socket
 * @param ipcsd	the data handle
 */
void cb_ipcsocket_server_close(cb_ipcserverd_t *ipcsd);

/**
 * @brief send ipc data to a specific client_address or internally managed IPC clients
 * @return 0 on success, -1 on error
 * @param ipcsd	the data handle
 * @param data	send data
 * @param size	send data size
 * @param client_address	if set, the data is sent to this client_address,
 * 	if NULL, the data is sent to all IPC cients
 * 	depends on 'ipcsd->udpport', client_address is 'sockaddr_in' or 'sockaddr_un'
 */
int cb_ipcsocket_server_write(cb_ipcserverd_t *ipcsd, uint8_t *data, int size,
			      struct sockaddr *client_address);

/**
 * @brief callback function to get sending data
 * @note  *sdata must be allocated in the callback
 */
typedef int(* cb_ipcsocket_server_ddatacb)(void *cbdata, uint8_t **sdata,
					   int *size, struct sockaddr *addr);

/**
 * @brief send ipc data to all clients
 * @return 0 on success, -1 on error
 * @param ipcsd	the data handle
 * @param cbdata	data to be passed with the callback
 * @param ddatacb	callback function to get defered data from the caller
 * @note this is used when notice data is different for each client.
 *	the data is not provided to this call but got by the callback.
 */
int cb_ipcsocket_server_write_ddata(cb_ipcserverd_t *ipcsd, void *cbdata,
				    cb_ipcsocket_server_ddatacb ddatacb);


/**
 * @brief callback function to be called from 'cb_ipcsocket_server_read'
 * @note  if the callback returns non-zero, the connection is closed.
 */
typedef int(* cb_ipcsocket_server_rdcb)(void *cbdata, uint8_t *rdata,
					int size, struct sockaddr *addr);

/**
 * @brief receive data on the IPC socket.
 * @return 0 on success, -1 on error
 * @param ipcsd	the data handle
 * @param ipccb	a callback function to be called with the read data
 * @param cbdata	data to be passed with the callback
 * @note this may block the process.  the caller functin must check events not to be blocked
 */
int cb_ipcsocket_server_read(cb_ipcserverd_t *ipcsd,
			     cb_ipcsocket_server_rdcb ipccb, void *cbdata);

/**
 * @brief return ipc socket fd
 */
CB_SOCKET_T cb_ipcsocket_getfd(cb_ipcserverd_t *ipcsd);

/**
 * @brief remove IPC client from the managed list
 */
int cb_ipcsocket_remove_client(cb_ipcserverd_t *ipcsd, struct sockaddr *client_address);


/**
 * @brief get IPC ipc communication mode
 * @return -1 on error, ipc communication mode
 * @param ipcsd	the data handle
 * @param client_address	if set, the data is sent to this client_address,
 */
cb_ipcclient_commode_t cb_ipcsocket_get_commode(cb_ipcserverd_t *ipcsd,
						struct sockaddr *client_address);


/**
 * @brief get IPC ipc communication mode
 * @return 0 on success, -1 on error
 * @param ipcsd	the data handle
 * @param client_address	if set, the data is sent to this client_address,
 * @param commode ipc communication mode
 */
int cb_ipcsocket_set_commode(cb_ipcserverd_t *ipcsd, struct sockaddr *client_address,
			     cb_ipcclient_commode_t commode);

#endif
/** @}*/
