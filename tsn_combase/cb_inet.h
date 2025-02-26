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
 * @defgroup ip_network IP network functions binding
 * @{
 * @file cb_inet.h
 *
 * @brief Bindings to POSIX network IP layer functions
 */

#ifndef CB_INET_H_
#define CB_INET_H_

#ifdef CB_INET_NON_POSIX_H
/* non-posix platforms need to support necessary POSIX compatible
 * functions and types which are defined as CB_* macros below.
 * And provide them in a header file defined as CB_SOCKET_NON_POSIX_H */
#include CB_INET_NON_POSIX_H
#else
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define CB_ADDRINFO_T struct addrinfo
#define CB_GETADDRINFO getaddrinfo
#define CB_FREEADDRINFO freeaddrinfo

#define CB_ADDRINFO_AI_FAMILY(x) ((x)->ai_family)
#define CB_ADDRINFO_AI_SOCKTYPE(x) ((x)->ai_socktype)
#define CB_ADDRINFO_AI_ADDR(x) ((x)->ai_addr)
#define CB_ADDRINFO_AI_ADDRLEN(x) ((x)->ai_addrlen)
#define CB_ADDRINFO_SA_FAMILY(x) ((x)->ai_addr->sa_family)

#define CB_SOCKADDR_SA_FAMILY(x) ((x)->sa_family)

#define CB_SOCKADDR_IN_ADDR(x)(((CB_SOCKADDR_IN_T *)(x))->sin_addr.s_addr)
#define CB_SOCKADDR_IN6_ADDR(x)(((CB_SOCKADDR_IN6_T *)(x))->sin6_addr)

#define CB_SOCK_BIND bind
#define CB_GETSOCKNAME getsockname
#define CB_GETNAMEINFO getnameinfo

/**
 * @brief Traverse CB_ADDRINFO_T
 */
#define CB_ADDRINFO_FOREACH_ITER(list, node) \
        (node) = (list); (node); (node) = (node)->ai_next

#endif // CB_INET_NON_POSIX_H

/**
 * @brief Network address encapsulation structure for IPv4 or IPv6 address
 * This structure is used to encapsulation of address type such that the implementation does not need
 * to have separate containers for IPv4 and IPv6 address. It now supports either AF_INET or AF_INET6
 * network addresses.
 * This structure may not hold both address type at a given time.
 * Dual-stack implementation must assume that IPv4 can be effective in parallel with IPv6 address, thus
 * it is expected that the the implementation uses two instances of this structure/
 */
typedef struct cb_inetaddr {
        int family; /**< address family type */
        union {
                CB_IN_ADDR_T ip_inaddr;
                CB_IN6_ADDR_T ip6_inaddr;
        } addr; /**< network address, hold either IPv4 or IPv6 address */
        unsigned short port; /**< port number */
} cb_inetaddr_t;

/**
 * @brief Network address to socket address mapping
 * This structure is used to store mapping between network address and socket address.
 */
typedef struct cb_inetaddr_map {
        CB_SOCKADDR_STORAGE_T ss; /**< socket storage */
        cb_inetaddr_t addr; /**< network address */
        uint64_t ts; /**< timestamp */
} cb_inetaddr_map_t;

/**
 * @brief Converts a name or ip address and port into CB_ADDRINFO_T
 * @param family    address protocol family type
 * @param socktype  preferred socket type
 * @param name      name of the service
 * @param port      port number of the service
 * @param numeric_only filter result to include numeric hosts only
 *
 * Pasing CB_AF_INET6 ensures that the list of CB_ADDRINFO_T returned are IPv6 address.
 * Otherwise passing CB_AF_UNSPEC leads to unspecified results, caller need to confirm the results.
 *
 * Note that this function allocates CB_ADDRINFO_T, the caller needs to perform CB_FREEADDRIINFO afterwards.
 */
CB_ADDRINFO_T * cb_name_to_addrinfo(int family, int socktype, const char *name, int port, bool numeric_only);

/**
 * @brief Translates socket address structure to IP address
 * @param sa        socket address structure
 * @param salen     socket address structure length
 * @param ip        buffer for the resulting IP address
 * @param ip_size   size of the buffer
 * @param port      resulting port number
 * @return 0 for success, otherwise error
 */
int cb_sockaddr_to_ipaddr(CB_SOCKADDR_T *sa, CB_SOCKLEN_T salen, char *ip,
			  size_t ip_size, int *port);

/**
 * @brief Translate socket information to IP address
 * @param ai        a ponter to an address information
 * @param ip        buffer for the resulting IP address
 * @param ip_size   size of the buffer
 * @param port      resulting port number
 * @return 0 for success, otherwise error
 */
int cb_addrinfo_to_ipaddr(const CB_ADDRINFO_T *ai, char *ip, size_t ip_size, int *port);


/**
 * @brief Checks if the proveded socket address is a multicast address
 * @param addr      socket address container
 * @return true if address is multicast, otherwise false
 */
bool cb_is_multicast_addr(CB_SOCKADDR_T *addr);

#endif
/** @}*/
