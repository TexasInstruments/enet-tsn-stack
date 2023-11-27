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
#include "getopt.h"
#include "gptpcommon.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpconf/xl4-extmod-xl4gptp.h"
#include "tsn_uniconf/uc_dbal.h"
#include "tsn_uniconf/hal/uc_hwal.h"
#include "tsn_uniconf/uc_notice.h"
#include "tsn_uniconf/ucman.h"
#include "tsn_uniconf/yangs/tsn_data.h"
#include "tsn_uniconf/yangs/ieee1588-ptp_access.h"
#include "tsn_uniconf/yangs/ietf-interfaces_access.h"

typedef struct gptpdpd {
	const char **netdevs;
} gptpdpd_t;

typedef struct gptpoptd {
	char *devlist;
	const char **conf_files;
	const char *db_file;
	int netdnum;
	char *inittm;
	int instnum;
	bool ucthread;
	int numconf;
	uint16_t ucmonport;
} gptpoptd_t;

static int print_usage(char *pname, gptpoptd_t *gpoptd)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL) s=pname;
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-d|--devs \"eth0,eth1,...\": comma separated network devices\n");
	UB_CONSOLE_PRINT("-p|--dbname: database file\n");
	UB_CONSOLE_PRINT("-c|--conf: config file\n");
	UB_CONSOLE_PRINT("\tuse multiple times for multiple configuration files\n");
	UB_CONSOLE_PRINT("  -c "UC_INIT_COPY_INSTANCE_PRE"N, "
			 "copy instance N,domain=0 data to this instance,domain=0.\n");
	UB_CONSOLE_PRINT("-t|--inittm year:month:date:hour:minute:second: set initial time\n");
	UB_CONSOLE_PRINT("-i|--instnum instance_number:set an instance number(default=0)\n");
	UB_CONSOLE_PRINT("-u|--ucthread: run uniconf in a thread\n");
	UB_CONSOLE_PRINT("--ucmon port_number: run uniconfmon in uniconf with this udp port\n");
	return -1;
}

static int set_options(gptpoptd_t *gpoptd, int argc, char *argv[])
{
	int oc;
	int res;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"devs", required_argument, 0, 'd'},
		{"conf", required_argument, 0, 'c'},
		{"dbname", required_argument, 0, 'p'},
		{"inittm", required_argument, 0, 't'},
		{"instnum", required_argument, 0, 'i'},
		{"ucthread", no_argument, 0, 'u'},
		{"ucmon", required_argument, 0, 0},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hd:c:t:i:p:u", long_options, NULL))!=-1){
		switch(oc){
		case 'd':
			gpoptd->devlist=optarg;
			break;
		case 'c':
			gpoptd->conf_files=
				UB_SD_REGETMEM(GPTP_SMALL_ALLOC, gpoptd->conf_files,
					      sizeof(char*)*(gpoptd->numconf+2));
			gpoptd->conf_files[gpoptd->numconf++]=optarg;
			gpoptd->conf_files[gpoptd->numconf]=NULL;
			break;
		case 'p':
			gpoptd->db_file=optarg;
			break;
		case 't':
			gpoptd->inittm=optarg;
			break;
		case 'i':
			gpoptd->instnum=strtol(optarg, NULL, 0);
			break;
		case 'u':
			gpoptd->ucthread=true;
			break;
		case 0:
			gpoptd->ucmonport=strtol(optarg, NULL, 0);
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

#ifndef GPTP2_IN_LIBRARY
#define GPTP2D_MAIN main
#endif

static bool stopgptp;
static void signal_handler(int sig)
{
	stopgptp=true;
}

static int get_netdevices(gptpdpd_t *gpdpd, gptpoptd_t *gpoptd)
{
	int i, k;
	const char *ndev;

	if(!gpoptd->devlist){
		// use domainIndex=0
		gpoptd->netdnum=ydbi_portdevices_pt(ydbi_access_handle(),
						    gpoptd->instnum, 0, &gpdpd->netdevs);
		if(gpoptd->netdnum<=0) return -1;
	}else{
		k=strlen(gpoptd->devlist);
		gpoptd->netdnum=1;
		for(i=0;i<k;i++){
			if(gpoptd->netdnum==XL4_DATA_ABS_MAX_NETDEVS){break;}
			if(gpoptd->devlist[i]!=','){continue;}
			gpoptd->devlist[i++]=0;
			gpoptd->netdnum++;
		}
		gpdpd->netdevs=UB_SD_GETMEM(GPTP_SMALL_ALLOC,
					    (gpoptd->netdnum) * sizeof(char *));
		ndev=gpoptd->devlist;
		for(i=0;i<gpoptd->netdnum;i++){
			gpdpd->netdevs[i]=ndev;
			ndev+=strlen(ndev)+1;
		}
	}
	for(i=0;i<gpoptd->netdnum;i++){
		uint8_t up=1;
		UB_LOG(UBL_DEBUG, "use network device:%s\n", gpdpd->netdevs[i]);
		YDBI_SET_ITEM(ifk1vk0, (char*)gpdpd->netdevs[i],
			      IETF_INTERFACES_ENABLED, YDBI_CONFIG,
			      &up, 1, YDBI_PUSH_NOTICE);
	}
	return 0;
}

static void release_netdevices(gptpdpd_t *gpdpd, gptpoptd_t *gpoptd)
{
	if(!gpoptd->devlist){
		ydbi_portdevices_pt_release(&gpdpd->netdevs, gpoptd->netdnum);
	}else{
		UB_SD_RELMEM(GPTP_SMALL_ALLOC, gpdpd->netdevs);
	}
}

int GPTP2D_MAIN(int argc, char *argv[])
{
	gptpdpd_t gpdpd;
	gptpoptd_t gpoptd;
	int res=-1;
	mode_t oumask;
	unibase_init_para_t init_para;
	CB_THREAD_T ucthreadt;
	ucman_data_t ucmd;
	struct sigaction sigact;
	bool stopuniconf=false;

	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46,gptp:56",
						   "UBL_GPTP");
	unibase_init(&init_para);
	ubb_memory_out_init(NULL, 0);// start with zero, so that the memory is not allocated
	memset(&ucmd, 0, sizeof(ucmd));
	oumask=umask(011);
	memset(&gpdpd, 0, sizeof(gpdpd));
	memset(&gpoptd, 0, sizeof(gpoptd));

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);

	if(set_options(&gpoptd, argc, argv)<0){return -1;}
	if(gpoptd.ucthread){
		ucmd.dbname=gpoptd.db_file;
		ucmd.stoprun=&stopuniconf;
		ucmd.ucmode=UC_CALLMODE_UNICONF|UC_CALLMODE_THREAD;
		ucmd.ucmanstart=UB_SD_GETMEM(GPTP_SMALL_ALLOC, sizeof(CB_SEM_T));
		if(ub_assert_fatal(ucmd.ucmanstart!=NULL, __func__, NULL)){return -1;}
		memset(ucmd.ucmanstart, 0, sizeof(CB_SEM_T));
		ucmd.hwmod="";
		if(gpoptd.ucmonport!=0){
			ucmd.ucmon_thread_port=gpoptd.ucmonport;
		}
		CB_SEM_INIT(ucmd.ucmanstart, 0, 0);
		CB_THREAD_CREATE(&ucthreadt, NULL, uniconf_main, &ucmd);
		CB_SEM_WAIT(ucmd.ucmanstart);
	}
	res=gptpgcfg_init(gpoptd.db_file, gpoptd.conf_files, gpoptd.instnum,
				gpoptd.ucthread, NULL);
	if(gpoptd.conf_files){
		UB_SD_RELMEM(GPTP_SMALL_ALLOC, gpoptd.conf_files);
		gpoptd.conf_files=NULL;
	}
	if(res){
		UB_LOG(UBL_ERROR, "tsn_gptpd: error in gptpgcfg_init\n");
		return -1;
	}
	res=gptpgcfg_get_intitem(gpoptd.instnum, XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_SIZE,
				 YDBI_CONFIG);
	if(res<0){
		UB_LOG(UBL_WARN, "tsn_gptpd:No DEBUGLOG_MEMORY_SIZE, use 0\n");
	}else{
		ubb_memory_out_init(NULL, res * 1024);
	}
	UB_LOG(UBL_INFO, "tsn_gptpd: tsn_gptp-"TSNPKGVERSION"\n");
	res=get_netdevices(&gpdpd, &gpoptd);
	if(res!=0){
		UB_LOG(UBL_ERROR, "no network device\n");
		goto erexit;
	}

	gptpman_run(gpoptd.instnum, gpdpd.netdevs, gpoptd.netdnum,
		    gpoptd.inittm, &stopgptp);
	UB_TLOG(UBL_INFO,"tsn_gptpd going to close\n");
	res=0;
erexit:
	if(gpdpd.netdevs){release_netdevices(&gpdpd, &gpoptd);}
	const char *dbfname;
	char *fname=NULL;
	dbfname=gptpgcfg_get_item(gpoptd.instnum, XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_FILE,
				  YDBI_CONFIG);
	if(dbfname){
		fname=UB_SD_GETMEM(GPTP_SMALL_ALLOC, strlen(dbfname)+1);
		if(fname){strcpy(fname, dbfname);}
	}
	gptpgcfg_get_item_release(gpoptd.instnum);
	gptpgcfg_close(gpoptd.instnum);
	if(fname && ubb_memory_file_out(fname)){
		UB_LOG(UBL_ERROR, "tsn_gptpd:can't write the memory log into a file\n");
	}
	if(fname){UB_SD_RELMEM(GPTP_SMALL_ALLOC, fname);}
	if(gpoptd.ucthread){
		stopuniconf=true;
		CB_THREAD_JOIN(ucthreadt, NULL);
		CB_SEM_DESTROY(ucmd.ucmanstart);
		UB_SD_RELMEM(GPTP_SMALL_ALLOC, ucmd.ucmanstart);
	}
	umask(oumask);
	ubb_memory_out_close();
	unibase_close();
	return res;
}
