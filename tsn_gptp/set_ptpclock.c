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
#include <fcntl.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include "ll_gptpsupport.h"

#define PTPDEV_CLOCKFD 3
#define FD_TO_CLOCKID(ptpfd) ((~(clockid_t) (ptpfd) << 3) | PTPDEV_CLOCKFD)

static int print_usage(char *pname)
{
	char *s;
	if((s=strrchr(pname,'/'))==NULL){s=pname;}
	else
		s++;
	printf("%s [options] [+/-]yyyy:mm:dd-HH:MM:SS\n", s);
	printf("-h|--help: this help\n");
	printf("-d|--ptpdev ptp_device (default:/dev/ptp0)\n");
	printf("\ne.g.\n");
	printf("# set to 2021 Jan. 1st 00:00:00\n");
	printf("  %s -d /dev/ptp1 2021:01:01-00:00:00\n", s);
	printf("# set 5 seconds ahead from the current, clock=/dev/ptp0 by default\n");
	printf("  %s +5\n", s);
	printf("# set 1 min 5 seconds behind from the current, clock=/dev/ptp0 by default\n");
	printf("  %s -- -01:10\n", s);
	return -1;
}

static int set_options(int argc, char *argv[], char **ptpdev)
{
	int oc;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"ptpdev", required_argument, 0, 'd'},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hd:", long_options, NULL))!=-1){
		switch(oc){
		case 'd':
			*ptpdev=optarg;
			break;
		case 'h':
			print_usage(argv[0]);
			return 1;
		default:
			return print_usage(argv[0]);
		}
	}
	return 0;
}

static void print_tm(int ptpfd, struct timespec *nts)
{
	struct timespec ts;
	struct tm *ltm;
	char stime[64];

	if(!nts){
		clock_gettime(FD_TO_CLOCKID(ptpfd), &ts);
		nts=&ts;
	}
	ltm=localtime(&nts->tv_sec);
	strftime(stime, sizeof(stime), "%Y:%m:%d-%H:%M:%S", ltm);
	printf("%s\n", stime);
	return;
}

static int get_newts(int ptpfd, char *stv, struct timespec *nts)
{
	//struct timespec ts;
	int rv=0;
	int vv[6];
	int i,v,dn;
	char *nstv;
	int rt;
	struct tm *ltm;

	if(*stv=='+'){rv=1;}
	if(*stv=='-'){rv=-1;}
	if(rv){stv++;}
	for(i=0;i<6;i++){
		v=strtol(stv, &nstv, 10);
		if(stv==nstv){break;}
		vv[i]=v;
		stv=nstv+1;
	}
	dn=i;
	if(dn==0){
		printf("invalid string: %s\n", stv);
		return -1;
	}
	if(!rv){
		// set absolute time
		clock_gettime(FD_TO_CLOCKID(ptpfd), nts);
		ltm=localtime(&nts->tv_sec);
		if(dn>5){ltm->tm_year=vv[dn-6]-1900;}
		if(dn>4){ltm->tm_mon=vv[dn-5]-1;}
		if(dn>3){ltm->tm_mday=vv[dn-4];}
		if(dn>2){ltm->tm_hour=vv[dn-3];}
		if(dn>1){ltm->tm_min=vv[dn-2];}
		if(dn>0){ltm->tm_sec=vv[dn-1];}
		nts->tv_sec=mktime(ltm);
	}else{
		if(dn>3){
			printf("only HH:MM:SS is valide to set diff: %s\n", stv);
			return -1;
		}
		clock_gettime(FD_TO_CLOCKID(ptpfd), nts);
		rt=1;
		for(i=dn-1;i>=0;i--){
			nts->tv_sec+=rv*vv[i]*rt;
			rt*=60;
		}
	}
	return 0;
}


int main(int argc, char* argv[])
{
	char *ptpdev="/dev/ptp0";
	int ptpfd;

	if(set_options(argc, argv, &ptpdev)){return -1;}
	ptpfd = open(ptpdev, O_RDWR);
	if(ptpfd<0){
		printf("can't open ptpdev=%s\n", ptpdev);
		return -1;
	}
	print_tm(ptpfd, NULL);
	if(argc>optind){
		struct timespec nts;
		get_newts(ptpfd, argv[optind], &nts);
		print_tm(ptpfd, &nts);
		clock_settime(FD_TO_CLOCKID(ptpfd), &nts);
	}

	close(ptpfd);
	return 0;
}
