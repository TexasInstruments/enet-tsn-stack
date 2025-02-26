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
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/combase.h>
#include <getopt.h>

static int print_usage(void)
{
	UB_CONSOLE_PRINT("udpuniconfmon [addr(default=127.0.0.1)] "
			 "port [uniconfmon parameters]\n");
	UB_CONSOLE_PRINT("E.G. udpuniconfmon 6666 -p .testdb -n\"/0/0/\"\n");
	return -1;
}

int main(int argc, char *argv[])
{
	char *addr="127.0.0.1";
	uint16_t port=0;
	int opti=1;
	int fd;
	char data[256]="uniconfmon ";
	int dp=11; // strlen("uniconfmon ")
	int rp;
	int res=-1;

	ubb_unibase_easyinit();

	if(argc<3){return print_usage();}
	if(strchr(argv[opti], '.')!=NULL){
		addr=argv[opti++];
	}
	port=strtol(argv[opti++], NULL, 0);
	if(port==0){return print_usage();}

	for(;opti<argc;opti++){
		if(ub_strncpy(&data[dp], argv[opti], sizeof(data)-dp-1)){goto erexit;}
		dp+=strlen(argv[opti]);
		if(opti==argc-1){
			data[dp]=0;
		}else{
			data[dp++]=' ';
		}
	}
	res=cb_ipcsocket_udp_init(&fd, NULL, addr, port);
	if(res!=0){goto erexit;}
	res=CB_SOCK_WRITE(fd, data, dp);
	if(res!=dp){goto erexit;}
	while(true){
		res=cb_fdread_timeout(fd, data, sizeof(data)-1, 100);
		if(res<=0){break;}
		data[res]=0;
		rp=0;
		if(res>=6){
			if(data[res-7]=='#'){
				rp=7;
			}else if(data[res-6]=='#'){
				rp=6;
			}
		}
		if((rp>0) && (strstr(&data[res-rp], "#res=")==&data[res-rp])){
			res=atoi(&data[res-rp+5]);
			break;
		}
		UB_CONSOLE_PRINT("%s", data);
	}
erexit:
	if(CB_SOCKET_VALID(fd)) {
		(void)CB_SOCK_CLOSE(fd);
	}
	unibase_close();
	return res;
}
