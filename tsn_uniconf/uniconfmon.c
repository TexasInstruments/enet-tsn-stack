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
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <tsn_combase/combase.h>
#include <tsn_unibase/unibase_binding.h>
#include "yangs/tsn_data.h"
#include "yangs/yang_db_runtime.h"
#include "yangs/yang_modules.h"
#include "ucman.h"

typedef char semname_t[26]; // strlen("/ucmonsem") + 16 + 1 = 26
typedef struct ucmonopt{
	int intervalms;
	int nummonitem;
	const char **monitems;
	const char *dbname;
	bool semnotice;
	UC_NOTICE_SIG_T **nsems;
	semname_t *semnames;
	int waitdb;
	int64_t waitnum;
	int waitnsize;
	bool hwaction;
	char *writedata;
	char *hints;
} ucmonopt_t;

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

/*
 * this monitor program won't be embedded, and static memory allocation is rarely needed.
 * Just in case, define here to monitor 10 items.
 * UC_NOTICE_SIGNUM is defined in 'uc_notice.h'
 */
#define UNICONFMON_MONITEMS uniconfmon_monitems
UB_SD_GETMEM_DEF(UNICONFMON_MONITEMS, sizeof(char*), UC_NOTICE_SIGNUM);
#define UNICONFMON_SEMNAMES uniconfmon_semnames
UB_SD_GETMEM_DEF(UNICONFMON_SEMNAMES, sizeof(semname_t), UC_NOTICE_SIGNUM);
#define UNICONFMON_STRINGS uniconfmon_strings
UB_SD_GETMEM_DEF(UNICONFMON_STRINGS, 16, 16); // 16*16=256bytes likely enoufh

static int print_usage(char *pname, ucmonopt_t *optd)
{
	char *s;
	s=strrchr(pname,'/');
	if(!s++){s=pname;}
	UB_CONSOLE_PRINT("%s [options] [write_data]\n", s);
	UB_CONSOLE_PRINT("if write_data exits, the data is written to the first '-n' item\n");
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-p|--dbname database_filename, default=%s\n",
			 optd->dbname);
	UB_CONSOLE_PRINT("-w|--waitdb wait time for uniconf in msec, default=0\n");
	UB_CONSOLE_PRINT("-n|--name monitor_item_name, default=None\n");
	UB_CONSOLE_PRINT("    to monitor multiple items, set this option multiple times\n");
	UB_CONSOLE_PRINT("-i|--interval read_interval(msec unit), default=0\n");
	UB_CONSOLE_PRINT("    set 0 for interactive mode, set -1 for oneshot mode\n");
	UB_CONSOLE_PRINT("-s|--semnotice get a notice by a named semaphore from uniconf.\n");
	UB_CONSOLE_PRINT("    with this option, the 'interval' is not used.\n");
	UB_CONSOLE_PRINT("-u|--waitnum  with '-w' and '-n', "
			 "wait iteger item become this number.\n");
	UB_CONSOLE_PRINT("-t|--waitnsize 8|16|32|64 with '-u', "
			 "wait iteger size, default=8\n");
	UB_CONSOLE_PRINT("    no this option and '-w','-n', "
			 "wait appearance of the item in the DB\n");
	UB_CONSOLE_PRINT("-a|--hwaction:ask an action to uniconf\n");
	UB_CONSOLE_PRINT("--hints:use this hints to print value\n");
	return -1;
}

static int set_options(ucmonopt_t *optd, int argc, char *argv[])
{
	int oc;
	int res=-1;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"dbname", required_argument, 0, 'p'},
		{"name", required_argument, 0, 'n'},
		{"interval", required_argument, 0, 'i'},
		{"semnotice", no_argument, 0, 's'},
		{"waitdb", required_argument, 0, 'w'},
		{"waitnum", required_argument, 0, 'u'},
		{"waitnsize", required_argument, 0, 't'},
		{"hwaction", no_argument, 0, 'a'},
		{"hints", required_argument, 0, 0},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hi:n:p:sw:u:at:", long_options, NULL))!=-1){
		switch(oc){
		case 'i':
			optd->intervalms=strtol(optarg, NULL, 0);
			break;
		case 'n':
			optd->monitems=
				UB_SD_REGETMEM(UNICONFMON_MONITEMS, optd->monitems,
					      sizeof(char*)*(optd->nummonitem+1));
			optd->nsems=
				uc_notice_sig_reallocate(false, optd->nsems,
							 optd->nummonitem+1);
			optd->semnames=
				UB_SD_REGETMEM(UNICONFMON_SEMNAMES, optd->semnames,
					      sizeof(semname_t)*(optd->nummonitem+1));
			sprintf(optd->semnames[optd->nummonitem], "/ucmonsem%16lx",
				ub_rt_gettime64());
			optd->monitems[optd->nummonitem++]=optarg;
			break;
		case 'p':
			optd->dbname=optarg;
			break;
		case 's':
			optd->semnotice=true;
			break;
		case 'w':
			optd->waitdb=strtol(optarg, NULL, 0);
			break;
		case 'u':
			optd->waitnum=strtol(optarg, NULL, 0);
			break;
		case 't':
			optd->waitnsize=strtol(optarg, NULL, 0);
			break;
		case 'a':
			optd->hwaction=true;
			break;
		case 0:
			optd->hints=optarg;
			break;
		case 'h':
		default:
			return print_usage(argv[0], optd);
		}
	}
	res=optind;
	if(argc>res){optd->writedata=argv[res];}
	optind=0;
	return res;
}

static char *read_line(int tout_ms)
{
	static char rbuf[256];
	char *pb;
	int rsize;
	rbuf[0]=0;
	if(tout_ms<=0){rsize=read(STDIN_FILENO, rbuf, sizeof(rbuf)-1);}
	else{rsize=cb_fdread_timeout(STDIN_FILENO, rbuf, sizeof(rbuf)-1, tout_ms);}
	if(rsize<0){
		UB_LOG(UBL_ERROR,"%s:stdin read error %s\n", __func__,strerror(errno));
		return NULL;
	}
	pb=strchr(rbuf,'\n');
	if(pb){*pb=0;}
	return rbuf;
}

static void print_value(int vtype, void *value, uint32_t vsize, char *hints)
{
	char *vstr="";
	if(vtype>=0){vstr=yang_value_string(vtype, value, vsize, 0, hints);}
	UB_CONSOLE_PRINT("uniconfmon ");
	if(!vstr[0]){
		ub_hexdump(true, true, value, vsize, 0);
	}else{
		UB_CONSOLE_PRINT("%s\n", vstr);
	}
}

static void check_allitems(ucmonopt_t *optd, uc_dbald *dbald,
			   xl4_data_data_t *xdd, yang_db_runtime_dataq_t *ydrd)
{
	uint8_t key1[1]={XL4_EXTMOD_RW};
	uint8_t key2[1]={XL4_DATA_RO-1};
	void *key, *value;
	uint32_t ksize, vsize;
	char *rstr;
	int i, vtype;
	uc_range *range;
	for(i=0;i<2;i++){
		range=uc_get_range(dbald, key1, 1, key2, 1);
		if(!range){continue;}
		if(i==0){UB_CONSOLE_PRINT("config items\n");}
		else{UB_CONSOLE_PRINT("status items\n");}
		while(true){
			if(uc_get_key_in_range(dbald, range, &key, &ksize,
					       UC_DBAL_NOMOVE)){break;}
			if(uc_get_value_in_range(dbald, range, &value, &vsize,
						 UC_DBAL_FORWARD)){break;}
			if(yang_modules_get_node_string(xdd, &rstr, key)){continue;}
			vtype=yang_db_runtime_get_vtype(dbald, key);
			UB_CONSOLE_PRINT("%s: ", rstr);
			print_value(vtype, value, vsize, NULL);
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr);
		}
		UB_CONSOLE_PRINT("----------\n");
		key1[0]+=0x80;
		key2[0]+=0x80;
	}
}

static void check_monitems(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd,
			   xl4_data_data_t *xdd, yang_db_runtime_dataq_t *ydrd)
{
	int i;
	int res;
	void *value=NULL;
	uint32_t vsize=0;
	char *kstr;

	if(optd->nummonitem==0){return check_allitems(optd, dbald, xdd, ydrd);}
	for(i=0;i<optd->nummonitem;i++){
		if(optd->writedata && i==0){
			kstr=UB_SD_GETMEM(UNICONFMON_STRINGS, strlen(optd->monitems[i])+1);
			if(!kstr){continue;}
			strcpy(kstr, optd->monitems[i]);
			(void)yang_db_runtime_put_oneline(ydrd, kstr,
							  optd->writedata,
							  YANG_DB_ONHW_NOACTION);
			if(optd->hwaction){
				(void)yang_db_runtime_askaction(ydrd, ucntd);
			}
			UB_SD_RELMEM(UNICONFMON_STRINGS, kstr);
		}
		res=yang_db_runtime_get_oneline(ydrd, optd->monitems[i], &value,
						&vsize);
		if(res<0){
			UB_CONSOLE_PRINT("no such item:%s\n", optd->monitems[i]);
		}else{
			print_value(res, value, vsize, optd->hints);
		}
	}
	return;
}

static int register_notice(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd,
			   yang_db_runtime_dataq_t *ydrd)
{
	int i;
	for(i=0;i<optd->nummonitem;i++){
		if(yang_db_runtime_notice_register(ydrd, ucntd, optd->monitems[i],
						   optd->semnames[i], &optd->nsems[i])){
			return -1;
		}
	}
	return 0;
}

static int deregister_notice(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd)
{
	int i;
	for(i=0;i<optd->nummonitem;i++){
		if(uc_nc_notice_deregister_all(ucntd, dbald, optd->semnames[i])){
			return -1;
		}
	}
	return 0;
}

static int check_notice(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd)
{
	int i;
	int res;
	int ni=-1;
	uint8_t key[UC_MAX_KEYSIZE];
	void *value=NULL;
	uint32_t ksize, vsize;
	uint8_t vtype;
	for(i=0;i<optd->nummonitem;i++){
		if(!uc_notice_sig_check(false, optd->nsems[i], 0, __func__)){
			ni=i;
			break;
		}
	}
	if(ni<0){return 0;}
	// received a notice
	UB_LOG(UBL_DEBUG, "%s:a noitce from %s\n", __func__, optd->monitems[ni]);
	// get a key of signaled data
	res=uc_nc_get_notice_act(ucntd, dbald, optd->semnames[ni], key, &ksize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:get a signal, but no data in uc_nc_get_notice_act, "
		       "res=%d\n", __func__, res);
		return 0;
	}
	// get value type
	if(ksize+2>UC_MAX_KEYSIZE){return -1;}
	memmove(&key[2], key, ksize);
	key[0]=XL4_DATA_RW;
	key[1]=YANG_VALUE_TYPES;
	for(i=0;i<(int)ksize+2;i++){
		if(key[i]==255){break;}
	}
	res=uc_dbal_get(dbald, key, i, &value, &vsize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:failed to get vtype, res=%d\n", __func__, res);
		goto erexit;
	}
	vtype=*((uint8_t*)value);
	uc_dbal_get_release(dbald, key, ksize+2, value, vsize);
	// get a value of the key
	res=uc_dbal_get(dbald, &key[2], ksize, &value, &vsize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:failed in uc_dbal_get, res=%d\n", __func__, res);
		goto erexit;
	}
	print_value(vtype, value, vsize, NULL);
	uc_dbal_get_release(dbald, key, ksize, value, vsize);
	res=0;
erexit:
	return res;
}

// return 1: no wait item, 0: got the wait item, -1: can't get the wait item
int main_loop(ucmonopt_t *optd)
{
	xl4_data_data_t *xdd;
	uc_dbald *dbald;
	uc_notice_data_t *ucntd=NULL;
	yang_db_runtime_dataq_t *ydrd=NULL;
	int res=-1;
	char *line;
	if(uniconf_ready(optd->dbname, 0, optd->waitdb)){
		return -1;
	}
	if((optd->intervalms<0) && (optd->nummonitem==1) &&
	   (strcmp(optd->monitems[0],"/")==0))
	{
		// this checks if uniconf is running
		return 0;
	}
	dbald=uc_dbal_open(optd->dbname, "w", 0);
	if(!dbald){
		UB_LOG(UBL_ERROR, "%s:can't open the db:%s\n", __func__, optd->dbname);
		return -1;
	}
	xdd=xl4_data_init(dbald);
	if(!xdd){goto erexit;}
	ydrd=yang_db_runtime_init(xdd, dbald, NULL);
	if(!ydrd){goto erexit;}
	if((optd->intervalms<0) && (optd->nummonitem==1) && (optd->waitdb>0)){
		if(optd->waitnum!=INT64_MAX){
			res=yang_db_runtime_waititem(ydrd, optd->monitems[0],
						     &optd->waitnum,
						     optd->waitnsize/8, optd->waitdb);
			goto erexit;
		}
		res=yang_db_runtime_waititem(ydrd, optd->monitems[0], NULL, 0, optd->waitdb);
		goto erexit;
	}

	if(optd->semnotice || optd->hwaction){
		ucntd=uc_notice_init(0);
		if(!ucntd){goto erexit;}
		if(optd->semnotice){
			if(register_notice(optd, dbald, ucntd, ydrd)){goto erexit;}
			optd->intervalms=10;
		}
	}
	while(true){
		if(optd->semnotice){
			if(check_notice(optd, dbald, ucntd)){break;}
		}else{
			check_monitems(optd, dbald, ucntd, xdd, ydrd);
		}
		uc_dbal_releasedb(dbald);
		if(optd->intervalms<0){break;}
		line=read_line(optd->intervalms);
		if(!line){goto erexit;}
		if(!strcmp(line, "q")){break;}
		if(!optd->semnotice && *line){
			optd->nummonitem=1;
			optd->monitems[0]=line;
			continue;
		}
	}
	res=0;
erexit:
	if(ucntd) {
		deregister_notice(optd, dbald, ucntd);
		uc_notice_close(ucntd, 0);
	}
	if(ydrd){yang_db_runtime_close(ydrd);}
	if(xdd){xl4_data_close(xdd);}
	uc_dbal_close(dbald, 0);
	return res;
}

int main(int argc, char *argv[])
{
	int res=-1;
	ucmonopt_t optd;
	unibase_init_para_t init_para;

	memset(&optd, 0, sizeof(optd));
	optd.waitnum=INT64_MAX;
	optd.waitnsize=8;
	optd.monitems=UB_SD_GETMEM(UNICONFMON_MONITEMS, sizeof(char*));
	optd.dbname="uniconfdb";
	optd.intervalms=-1; // one shot mode is more commonly used
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46",
						   "UBL_UNICONF");
	unibase_init(&init_para);
	ubb_memory_out_init(NULL, 0);
	if(set_options(&optd, argc, argv)<0) goto erexit;
	res=main_loop(&optd);
erexit:
	if(optd.monitems){UB_SD_RELMEM(UNICONFMON_MONITEMS, optd.monitems);}
	if(optd.semnames){UB_SD_RELMEM(UNICONFMON_SEMNAMES, optd.semnames);}
	if(optd.nsems){uc_notice_sig_reallocate(false, optd.nsems, 0);}
	unibase_close();
	return res;
}
