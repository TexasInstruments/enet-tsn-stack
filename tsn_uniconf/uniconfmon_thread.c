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
#include <tsn_combase/combase.h>

extern int UCMON_MAIN(int argc, char *argv[]); // from uniconfmon.c

static CB_SOCKADDR_IN_T client_address;
static CB_THREAD_T ucmthread;
static uint16_t ucmport;
static CB_SOCKET_T ucmonfd=CB_SOCKET_INVALID_VALUE;

#define MAX_BADCOUNT 10
#define UCMON_MAX_ARGMENTS 10

static int ucmon_udp_proc(char *dbuf, int dsize)
{
	char *argv[UCMON_MAX_ARGMENTS];
	int argc;
	argc=ub_command_line_parser(dbuf, argv, UCMON_MAX_ARGMENTS);
	if((argc<1) || (strcmp(argv[0], "uniconfmon")!=0)){return -1;}
	argv[0]="uniconfmot";
	return UCMON_MAIN(argc, argv);
}

static void *uniconfmon_thread(void *ptr)
{
	char dbuf[256];
	int rsize;
	int badcount=0;
	int res=0;
	CB_SOCKLEN_T address_length=sizeof(CB_SOCKADDR_IN_T);

	if(cb_ipcsocket_udp_init(&ucmonfd, NULL, NULL, ucmport)){
		ucmport=0;
		return NULL;
	}
	while(ucmport>0){
		if(badcount>MAX_BADCOUNT){
			UB_LOG(UBL_ERROR, "%s:too many bad format data, close thread\n",
			       __func__);
			res=-1;
			break;
		}
		rsize=CB_SOCK_RECVFROM(ucmonfd, dbuf, sizeof(dbuf)-1, 0,
				       (CB_SOCKADDR_T*)&client_address, &address_length);
		if(rsize<=0){
			badcount++;
			continue;
		}
		dbuf[rsize]=0;// guarantee it is terminated with '\0'
		if(strcmp(dbuf, "quitmon")==0){break;}
		res=ucmon_udp_proc(dbuf, rsize);
		if(res){
			badcount++;
		}else{
			badcount=0;// reset the counter when it gets a good data
		}
		rsize=sprintf(dbuf, "#res=%d", res);
		CB_SOCK_SENDTO(ucmonfd, dbuf, rsize, 0,
			       (CB_SOCKADDR_T*)&client_address, address_length);
	}
	(void)cb_ipcsocket_close(ucmonfd, NULL, NULL);
	ucmonfd=CB_SOCKET_INVALID_VALUE;
	return NULL;
}

int uniconfmon_thread_start(uint16_t port)
{
	if(ucmport!=0){
		UB_LOG(UBL_ERROR, "%s:already running\n", __func__);
		return -1;
	}
	ucmport=port;
	if(CB_THREAD_CREATE(&ucmthread, NULL, uniconfmon_thread, NULL)){
		UB_LOG(UBL_ERROR, "%s:can't start\n", __func__);
		return -1;
	}
	return 0;
}

int uniconfmon_thread_stop(void)
{
	int fd;
	if(ucmport==0){
		UB_LOG(UBL_ERROR, "%s:not running\n", __func__);
		return -1;
	}
	if(cb_ipcsocket_udp_init(&fd, NULL, "127.0.0.1", ucmport)==0)
	{
		// send signale to close
		if(CB_SOCK_WRITE(fd, "quitmon", 8)==8){
			cb_ipcsocket_close(fd, NULL, NULL);
		}
	}
	ucmport=0;
	CB_THREAD_JOIN(ucmthread, NULL);
	return 0;
}

bool uniconfmon_thread_running(void)
{
	return ucmonfd!=CB_SOCKET_INVALID_VALUE;
}


int uniconfmon_thread_write(uint8_t *data, int size)
{
	int ssize;
	if(ucmonfd==CB_SOCKET_INVALID_VALUE){return -1;}
	ssize=sizeof(CB_SOCKADDR_IN_T);
	return CB_SOCK_SENDTO(ucmonfd, data, size, 0,
			      (CB_SOCKADDR_T*)&client_address, ssize);
}

int uniconfmon_thread_read(uint8_t *data, int size, int tout_ms)
{
	int rsize;
	if(ucmonfd==CB_SOCKET_INVALID_VALUE){return -1;}
	if(tout_ms<=0){
		rsize=read(ucmonfd, data, size);
	}else{
		rsize=cb_fdread_timeout(ucmonfd, data, size, tout_ms);
	}
	return rsize;
}
