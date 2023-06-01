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
#include <stdlib.h>
#include "combase_private.h"

#define MAX_SERVNAME_LEN 16

CB_ADDRINFO_T * cb_name_to_addrinfo(int family, int socktype, const char *name, int port, bool numeric_only){
        CB_ADDRINFO_T *res = NULL;
        CB_ADDRINFO_T hints;
        char serv[MAX_SERVNAME_LEN];
        int err;

        (void)memset(&hints, 0, sizeof(hints));
        (void)snprintf(serv, sizeof(serv), "%i", port);
        hints.ai_family = family;
        if(numeric_only){
                hints.ai_flags = AI_NUMERICSERV|AI_NUMERICHOST;
        }
        hints.ai_socktype = socktype;
        if(family==AF_INET6){
                hints.ai_flags |= AI_V4MAPPED;
                hints.ai_flags |= AI_ALL;
        }

        err = CB_GETADDRINFO(name,serv,&hints,&res);
        if(err!=0){
                if(!numeric_only || (err!=EAI_NONAME)){
                        return NULL;
                }
        }
        return res;
}

int cb_addrinfo_to_ipaddr(const CB_ADDRINFO_T *ai, char *ip, size_t ip_size, int *port){
        char serv[MAX_SERVNAME_LEN];
        int err = CB_GETNAMEINFO(CB_ADDRINFO_AI_ADDR(ai),
				 (CB_SOCKLEN_T)(CB_ADDRINFO_AI_ADDRLEN(ai)),
                                 ip, (CB_SOCKLEN_T)ip_size, serv, (CB_SOCKLEN_T)sizeof(serv),
                                 NI_NUMERICHOST|NI_NUMERICSERV);
        if(err!=0){return -1;}
        if(port!=NULL){*port=strtol(serv, NULL, 0);}
        return 0;
}

int cb_sockaddr_to_ipaddr(CB_SOCKADDR_T *sa, CB_SOCKLEN_T salen, char *ip,
			  size_t ip_size, int *port){
        CB_ADDRINFO_T ai;
        (void)memset(&ai, 0, sizeof(ai));
        CB_ADDRINFO_AI_ADDR(&ai) = sa;
        CB_ADDRINFO_AI_ADDRLEN(&ai) = salen;
        CB_ADDRINFO_AI_FAMILY(&ai) = CB_SOCKADDR_SA_FAMILY(sa);
        return cb_addrinfo_to_ipaddr(&ai, ip, ip_size, port);
}

bool cb_is_multicast_addr(CB_SOCKADDR_T *addr){
        switch(CB_SOCKADDR_SA_FAMILY(addr)){
        case AF_INET:
                return IN_MULTICAST(ntohl(CB_SOCKADDR_IN_ADDR(addr)));
        case AF_INET6:
                return IN6_IS_ADDR_MULTICAST(&(CB_SOCKADDR_IN6_ADDR(addr)));
        default:
                return false;
        }
}
