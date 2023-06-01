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
#include <signal.h>
#include <sys/stat.h>
#include <tsn_unibase/unibase_binding.h>
#include <fcntl.h>
#include <stdio.h>
#include "gptpclock.h"
#include "gptpnet.h"
#include "mdeth.h"
#include "gptpman.h"
#include <getopt.h>
#include "gptp_config.h"

extern int gptpconf_values_test(void);

typedef struct gptpdpd {
	char **netdevs;
} gptpdpd_t;

typedef struct gptpoptd {
	char *devlist;
	char *conf_file;
	int netdnum;
	int domain_num;
	char *inittm;
} gptpoptd_t;

static int print_usage(char *pname, gptpoptd_t *gpoptd)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-d|--devs \"eth0,eth1,...\": comma separated network devices\n");
	UB_CONSOLE_PRINT("-n|--dnum number: max number of network devices\n");
	UB_CONSOLE_PRINT("-m|--domain number: max number of domain\n");
	UB_CONSOLE_PRINT("-c|--conf: config file\n");
	UB_CONSOLE_PRINT("-t|--inittm year:month:date:hour:minute:second: set initial time\n");
	return -1;
}

static int set_options(gptpoptd_t *gpoptd, int argc, char *argv[])
{
	int oc;
	int res;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"devs", required_argument, 0, 'd'},
		{"dnum", required_argument, 0, 'n'},
		{"domain", required_argument, 0, 'm'},
		{"conf", required_argument, 0, 'c'},
		{"inittm", required_argument, 0, 't'},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hd:n:c:m:t:", long_options, NULL))!=-1){
		switch(oc){
		case 'd':
			gpoptd->devlist=optarg;
			break;
		case 'c':
			gpoptd->conf_file=optarg;
			break;
		case 'n':
			gpoptd->netdnum=strtol(optarg, NULL, 0);
			break;
		case 'm':
			gpoptd->domain_num=strtol(optarg, NULL, 0);
			break;
		case 't':
			gpoptd->inittm=optarg;
			break;
		case 'h':
		default:
			return print_usage(argv[0], gpoptd);
		}
	}
	res=optind;
	optind=0;
	return res;
}

static void signal_handler(int sig)
{
	gptpman_stop();
}
#ifndef GPTP2_IN_LIBRARY
#define GPTP2D_MAIN main
#endif

int GPTP2D_MAIN(int argc, char *argv[])
{
	gptpdpd_t gpdpd;
	gptpoptd_t gpoptd;
	int i, j, k;
	netdevname_t *netdevs;
	ptpdevname_t *ptpdevs;
	int res=-1;
	mode_t oumask;
	struct sigaction sigact;
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,gptp:46", "UBL_GPTP");
	unibase_init(&init_para);
	ubb_memory_out_init(NULL, 0);// start with zero, so that the memory is not allocated

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);

	if(gptpconf_values_test()){return -1;}
	oumask=umask(011);
	memset(&gpdpd, 0, sizeof(gpdpd));
	memset(&gpoptd, 0, sizeof(gpoptd));
	if(set_options(&gpoptd, argc, argv)<0){return -1;}
	if(gpoptd.conf_file){ub_read_config_file(gpoptd.conf_file, gptpconf_set_stritem);}
	ubb_memory_out_init(NULL, gptpconf_get_intitem(CONF_DEBUGLOG_MEMORY_SIZE)*1024);
	UB_LOG(UBL_INFO, "gptp2d: gptp2-"TSNPKGVERSION"\n");
	if(!gpoptd.netdnum){gpoptd.netdnum=gptpconf_get_intitem(CONF_MAX_PORT_NUMBER);}
	if(!gpoptd.domain_num){gpoptd.domain_num=gptpconf_get_intitem(CONF_MAX_DOMAIN_NUMBER);}
	netdevs=malloc(gpoptd.netdnum * sizeof(netdevname_t));
	ptpdevs=malloc(gpoptd.netdnum * sizeof(ptpdevname_t));
	gpdpd.netdevs=malloc((gpoptd.netdnum+1) * sizeof(char *));

	if(!gpoptd.devlist){
		gpoptd.netdnum = cb_get_all_netdevs(gpoptd.netdnum, netdevs);
		for(i=0,j=0;i<gpoptd.netdnum;i++){
			if(!cb_get_ptpdev_from_netdev(netdevs[i], ptpdevs[i])){
				gpdpd.netdevs[j++]=netdevs[i];
			}
		}
		gpdpd.netdevs[j]=0;
	}else{
		j=0;
		gpdpd.netdevs[j++]=gpoptd.devlist;
		k=strlen(gpoptd.devlist);
		for(i=0;i<k;i++){
			if(gpoptd.devlist[i]!=','){continue;}
			gpoptd.devlist[i++]=0;
			if(j>=gpoptd.netdnum){break;}
			gpdpd.netdevs[j++]=gpoptd.devlist+i;
			continue;
		}
		gpdpd.netdevs[j]=0;
		gpoptd.netdnum=j;
	}
	for(i=0;i<gpoptd.netdnum;i++){
		if(!gpdpd.netdevs[i]){break;}
		UB_LOG(UBL_DEBUG, "use network device:%s\n", gpdpd.netdevs[i]);
	}
	gptpconf_values_test();

	gptpman_run(gpdpd.netdevs, gpoptd.netdnum, gpoptd.domain_num, gpoptd.inittm);
	UB_LOG(UBL_INFO,"gptp2d going to close\n");
	res=0;
	free(gpdpd.netdevs);
	free(ptpdevs);
	free(netdevs);
	if(ubb_memory_file_out(gptpconf_get_item(CONF_DEBUGLOG_MEMORY_FILE))){
		UB_LOG(UBL_ERROR, "%s:can't write the memory log into a file\n", __func__);
	}
	umask(oumask);
	ubb_memory_out_close();
	unibase_close();
	return res;
}
