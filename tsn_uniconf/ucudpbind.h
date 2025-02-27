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
#ifndef UCUDPBIND_H_
#define UCUDPBIND_H_

#include <inttypes.h>
#include <tsn_combase/combase.h>
#include "yangs/yang_node.h"
#include "uc_binconf.h"

#ifndef UCUDPBIND_PORT
#define UCUDPBIND_PORT 8190
#endif

#define UCUDPBIN_PLSIZE 1472
typedef uint8_t ucudpbuf_t[UCUDPBIN_PLSIZE];

typedef struct ucudbopt{
	char *dbname;
	uint16_t udpport;
	bool thread_mode;
}ucudbopt_t;

enum {
	UC_BINCONFUDP_NOP,
	UC_BINCONFUDP_PULL,
	UC_BINCONFUDP_PULL_WCARD, // support only prefix match wildcard
	UC_BINCONFUDP_PULLOKRES,
	UC_BINCONFUDP_PULLNGRES,
	UC_BINCONFUDP_PUSHC, // push with following more pushes
	UC_BINCONFUDP_PUSHE, // push without following pushes
	UC_BINCONFUDP_PUSHOKRES,
	UC_BINCONFUDP_PUSHNGRES,
};

static inline bool ucudpbin_wait_uniconf(const char *destaddr, uint16_t dest_port, int toutms)
{
	int res, fd, i;
	bool rval=false;
	uint32_t ksize=3;
	uint8_t sbuf[12]={UC_BINCONFUDP_PULL,0,0,0,0,XL4_DATA_RO,UC_READY,255,};
	uint8_t rbuf[12];
	memcpy(&sbuf[1], &ksize, 4);
	res=cb_ipcsocket_udp_init(&fd, NULL, destaddr, dest_port);
	if(res!=0){return -1;}
	for(i=0;i<toutms;i+=10){
		res=CB_SOCK_WRITE(fd, sbuf, 8);
		if(res!=8){
			CB_USLEEP(10000);
			continue;
		}
		res=cb_fdread_timeout(fd, rbuf, sizeof(rbuf), 10);
		if(res!=11){
			CB_USLEEP(10000);
			continue;
		}
		rval=true;
		break;
	}
	CB_SOCK_CLOSE(fd);
	return rval;
}
#endif
