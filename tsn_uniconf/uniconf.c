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
#include "getopt.h"
#include <string.h>
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include "yangs/yang_db_runtime.h"
#include "uc_private.h"
#include "ucman.h"

typedef struct uniconfopt{
	int dlogmem;
	char *dlogfile;
	ucman_data_t ucman;
	mode_t oumask;
} uniconfopt_t;

#define UC_CONFIGFILE_LIST uc_configfile_list
#ifndef UC_CONFIGFILE_NUM
#define UC_CONFIGFILE_NUM 8
#endif
UB_SD_GETMEM_DEF(UC_CONFIGFILE_LIST, sizeof(char*), UC_CONFIGFILE_NUM);

static int print_usage(char *pname, uniconfopt_t *optd)
{
	char *s;
	s=strrchr(pname,'/');
	if(!s++){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-m|--dlogmem memsize(in kbytes) default=%d\n", optd->dlogmem);
	UB_CONSOLE_PRINT("-f|--dlogfile filename(to save logmemory) default=%s\n",
			 optd->dlogfile);
	UB_CONSOLE_PRINT("-p|--dbname filename(to save dbdata data) default=%s\n",
			 optd->ucman.dbname);
	UB_CONSOLE_PRINT("-c|--config filename(run time config file)\n");
	UB_CONSOLE_PRINT("\tuse multiple times for multiple configuration files\n");
	UB_CONSOLE_PRINT("-l|--hwmod hardware_layer\n");
	UB_CONSOLE_PRINT("\t\"\"(null string) for the default, \"NONE\" for no HW\n");
	UB_CONSOLE_PRINT("-u|--umask change umask to 0\n");
	return -1;
}

static int set_options(uniconfopt_t *optd, int argc, char *argv[])
{
	int oc;
	int res=-1;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"dlogmem", required_argument, 0, 'm'},
		{"dlogfile", required_argument, 0, 'f'},
		{"dbname", required_argument, 0, 'p'},
		{"config", required_argument, 0, 'c'},
		{"hwmod", required_argument, 0, 'l'},
		{"umask", no_argument, 0, 'u'},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hm:f:p:c:l:u", long_options, NULL))!=-1){
		switch(oc){
		case 'm':
			optd->dlogmem=strtol(optarg, NULL, 0);
			break;
		case 'f':
			optd->dlogfile=optarg;
			break;
		case 'p':
			optd->ucman.dbname=optarg;
			break;
		case 'c':
			optd->ucman.configfiles=
				UB_SD_REGETMEM(UC_CONFIGFILE_LIST, optd->ucman.configfiles,
					      sizeof(char*)*(optd->ucman.numconfigfile+1));
			optd->ucman.configfiles[optd->ucman.numconfigfile++]=optarg;
			break;
		case 'l':
			optd->ucman.hwmod=optarg;
			break;
		case 'u':
			optd->oumask=umask(0);
			break;
		case 'h':
		default:
			return print_usage(argv[0], optd);
		}
	}
	res=optind;
	optind=0;
	return res;
}

static bool stoprun=false;
static void signal_handler(int sig)
{
	stoprun=true;
}

#define XSTRINGFY(x) STRINGFY(x)
#define STRINGFY(x) #x
int main(int argc, char *argv[])
{
	int res=-1;
	uniconfopt_t optd;
	unibase_init_para_t init_para;
	struct sigaction sigact;

	if((argc==2) && (strcmp(argv[1],"--dbtype")==0)){
		printf("%s\n", XSTRINGFY(DBNAME));
		return 0;
	}
	memset(&optd, 0, sizeof(optd));
	optd.ucman.dbname="uniconfdb";
	optd.ucman.hwmod="";
	optd.dlogfile="uniconf.log";
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:46,uconf:46",
						   "UBL_UNICONF");
	unibase_init(&init_para);
	ubb_memory_out_init(NULL, 0);// start with zero, so that the memory is not allocated
	if(set_options(&optd, argc, argv)<0){goto erexit;}
	ubb_memory_out_init(NULL, optd.dlogmem*1024);
	optd.ucman.stoprun=&stoprun;
	optd.ucman.ucmode=UC_CALLMODE_UNICONF;

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);

	uniconf_main(&optd.ucman);
	res=optd.ucman.rval;
erexit:
	if(optd.ucman.configfiles){UB_SD_RELMEM(UC_CONFIGFILE_LIST, optd.ucman.configfiles);}
	if(optd.dlogmem && optd.dlogfile && ubb_memory_file_out(optd.dlogfile)){
		UB_LOG(UBL_ERROR, "%s:can't write the memory log into a file\n", __func__);
	}
	ubb_memory_out_close();
	unibase_close();
	if(optd.oumask>0){umask(optd.oumask);}
	return res;
}
