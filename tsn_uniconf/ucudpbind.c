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
#include <tsn_combase/cb_ethernet.h>
#include <tsn_combase/cb_ipcsock.h>
#include <signal.h>
#include <sys/stat.h>
#include <getopt.h>
#include "ucman.h"
#include "uc_binconf.h"
#include "yangs/yang_db_access.h"
#include "ucudpbind.h"

typedef struct ucudpbindopt{
	int dlogmem;
	char *dlogfile;
	ucudbopt_t udbopt;
} ucudpbindopt_t;

typedef struct ucudpbin_data{
	cb_ipcserverd_t *svsockd;
	uc_dbald *dbald;
	uc_bcdata_t *ucbcd;
	uc_notice_data_t *ucntd;
}ucudpbin_data_t;

UB_SD_GETMEM_DEF(UCUDPBIN_MEM, 16, (UC_MAX_KEYSIZE+UC_MAX_VALUESIZE)/16);

static void debug_log_key(uc_dbald *dbald, uint8_t *key, uint32_t ksize,
			  const char *fname, const char *dmsg)
{
	if(!ub_dlog_on(UB_LOGCAT, UBL_DEBUG)){return;}
#ifdef UC_RUNCONF
	char *rstr;
	if(yang_db_runtime_getkeyvkstr(dbald, key, ksize, &rstr)==0){
		UB_TLOG(UBL_DEBUG, "%s:%s %s\n", fname, dmsg, rstr);
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr);
		return;
	}
#endif
	UB_TLOG(UBL_DEBUG, "%s:%s\n", fname, dmsg);
	ub_hexdump(false, true, key, ksize, 0);
}

static int get_wildcard_data(ucudpbin_data_t *ucudpd, uint8_t *rdata, int dsize,
			     uint32_t ksize, struct sockaddr *addr)
{
	int res=-1;
	int i;
	uint8_t *key1=NULL, *key2=NULL;
	uc_range *range=NULL;
	void *ekdata, *vdata;
	uint32_t eksize, vsize;
	uint16_t svsize;
	key1=UB_SD_GETMEM(UCUDPBIN_MEM, ksize);
	if(ub_assert_fatal(key1!=NULL, __func__, NULL)){goto erexit;}
	key2=UB_SD_GETMEM(UCUDPBIN_MEM, ksize);
	if(ub_assert_fatal(key2!=NULL, __func__, NULL)){goto erexit;}
	memcpy(key1, &rdata[5], ksize);
	memcpy(key2, &rdata[5], ksize);
	for(i=ksize-1;i>=0;i--){
		if(key2[i]!=0xff){
			key2[i]+=1;
			break;
		}
	}
	range=uc_get_range(ucudpd->dbald, key1, ksize, key2, ksize);
	if(range==NULL){
		goto erexit;
	}
	// get the first data in the range
	if(uc_get_keyvalue_in_range(ucudpd->dbald, range, &ekdata,
				    &eksize, &vdata, &vsize, UC_DBAL_NOMOVE)){
		// shouldn't happen
		goto erexit;
	}
	UB_SD_RELMEM(UCUDPBIN_MEM, key2);
	key2=NULL;
	key1=UB_SD_REGETMEM(UCUDPBIN_MEM, key1, eksize+vsize+7);
	key1[0]=UC_BINCONFUDP_PULLOKRES;
	memcpy(&key1[1], &eksize, 4);
	memcpy(&key1[5], ekdata, eksize);
	svsize=(uint16_t)vsize;
	memcpy(&key1[eksize+5], &svsize, 2);
	memcpy(&key1[eksize+7], vdata, vsize);
	res=cb_ipcsocket_server_write(ucudpd->svsockd, key1, eksize+vsize+7, addr);
erexit:
	if(key1!=NULL){UB_SD_RELMEM(UCUDPBIN_MEM, key1);}
	if(key2!=NULL){UB_SD_RELMEM(UCUDPBIN_MEM, key2);}
	if(range!=NULL){uc_get_range_release(ucudpd->dbald, range);}
	return res;
}

static int ucudpbin_receive_cb(void *cbdata, uint8_t *rdata, int dsize, struct sockaddr *addr)
{
	ucudpbin_data_t *ucudpd=(ucudpbin_data_t *)cbdata;
	uint32_t ksize;
	char *emsg=NULL;
	void *value;
	uint32_t vsize;
	uint16_t svsize;
	int res=-1;

	if(dsize<6){
		emsg="too small data";
		goto erexit;
	}
	ub_non_aligned_intsubst(&rdata[1], &ksize, 4);
	if(dsize<(int)ksize+5){
		emsg="dsize < key size";
		goto erexit;
	}
	switch(rdata[0]){
	case UC_BINCONFUDP_PULL:
		res=uc_dbal_get(ucudpd->dbald, &rdata[5], ksize, &value, &vsize);
		debug_log_key(ucudpd->dbald, &rdata[5], ksize, __func__, "PULL");
		if((res==0) && (vsize>0)){
			if(vsize+ksize+9>UCUDPBIN_PLSIZE){
				rdata[0]=UC_BINCONFUDP_PULLNGRES;
				emsg="vsize too big";
			}else{
				rdata[0]=UC_BINCONFUDP_PULLOKRES;
				svsize=(uint16_t)vsize;
				memcpy(&rdata[ksize+5], &svsize, 2);
				memcpy(&rdata[ksize+7], value, vsize);
				dsize=vsize+ksize+7;
			}
			uc_dbal_get_release(ucudpd->dbald, &rdata[5], ksize, value, vsize);
		}else{
			rdata[0]=UC_BINCONFUDP_PULLNGRES;
			emsg="no data";
		}
		//send data
		break;
	case UC_BINCONFUDP_PULL_WCARD:
		res=get_wildcard_data(ucudpd, rdata, dsize, ksize, addr);
		debug_log_key(ucudpd->dbald, &rdata[5], ksize, __func__, "PULL_WCARD");
		if(res==0){
			dsize=0;
		}else{
			rdata[0]=UC_BINCONFUDP_PULLNGRES;
			emsg="can't get a range data";
		}
		break;
	case UC_BINCONFUDP_PUSHC:
	case UC_BINCONFUDP_PUSHE:
		uc_binconf_clean_lkey(ucudpd->ucbcd);
		res=uc_binconf_set_bindata(ucudpd->ucbcd, &rdata[1], dsize-1);
		debug_log_key(ucudpd->dbald, &rdata[5], ksize, __func__, "PUSH");
		if(res==dsize-1){
			res=uc_binconf_write_chunk(ucudpd->ucbcd, ucudpd->dbald);
		}else{
			UB_LOG(UBL_ERROR, "%s:res=%d, expected=%d\n",
			       __func__, res, dsize-3);
			res=-1;
		}
		if(rdata[0]==UC_BINCONFUDP_PUSHE){
			if(res>0){
				rdata[0]=UC_BINCONFUDP_PUSHOKRES;
				if(yang_db_key_needaction(ucudpd->dbald, &rdata[5], ksize)){
					UB_LOG(UBL_DEBUG, "%s:askaction\n", __func__);
					uc_nc_askaction_key_push(
						ucudpd->ucntd, ucudpd->dbald, &rdata[5], ksize);
				}
			}else{
				rdata[0]=UC_BINCONFUDP_PUSHNGRES;
			}
			dsize=1;
			//send data
		}else{
			dsize=0;
			res=0;
		}
		break;
	default:
		emsg="invalid message\n";
		goto erexit;
	}
	if(dsize>0){
		res=cb_ipcsocket_server_write(ucudpd->svsockd, rdata, dsize, addr);
		if(res!=0){
			emsg="can't write pull res\n";
		}
	}
erexit:
	if(emsg!=NULL){UB_TLOG(UBL_WARN, "%s:%s\n", __func__, emsg);}
	return res;
}

static int ucudpbin_main(ucudbopt_t *udbopt, bool *ucudb_stoprun)
{
	int res=-1;
	uint8_t callmode=0;
	ucudpbin_data_t ucudpd;
	memset(&ucudpd, 0, sizeof(ucudpd));
	ucudpd.svsockd=cb_ipcsocket_server_init(NULL, NULL, udbopt->udpport);
	if(ucudpd.svsockd==NULL){return -1;}
	if(udbopt->thread_mode){callmode=UC_CALLMODE_THREAD;}
	ucudpd.dbald=uc_dbal_open(udbopt->dbname, "w", callmode);
	if(ucudpd.dbald==NULL){goto erexit;}
	ucudpd.ucbcd=uc_binconf_init(UCUDPBIN_PLSIZE);
	if(ucudpd.ucbcd==NULL){goto erexit;}
	ucudpd.ucntd=uc_notice_init(callmode, udbopt->dbname);
	if(ucudpd.ucntd==NULL){goto erexit;}
	while(!*ucudb_stoprun){
		uc_dbal_releasedb(ucudpd.dbald);
		cb_ipcsocket_server_read(ucudpd.svsockd, ucudpbin_receive_cb, &ucudpd);
	}
	res=0;
erexit:
	if(ucudpd.ucntd!=NULL){uc_notice_close(ucudpd.ucntd, callmode);}
	if(ucudpd.ucbcd!=NULL){uc_binconf_close(ucudpd.ucbcd);}
	if(ucudpd.dbald){uc_dbal_close(ucudpd.dbald, callmode);}
	if(ucudpd.svsockd!=NULL){
		cb_ipcsocket_server_close(ucudpd.svsockd);
	}
	return res;
}

#ifndef UCUDPBIN_IN_LIBRARY
static int print_usage(char *pname, ucudpbindopt_t *optd)
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
			 optd->udbopt.dbname);
	UB_CONSOLE_PRINT("-u|--udpport udp port number default=%d\n",
			 optd->udbopt.udpport);

	return -1;
}

static int set_options(ucudpbindopt_t *optd, int argc, char *argv[])
{
	int oc;
	int res=-1;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"dlogmem", required_argument, 0, 'm'},
		{"dlogfile", required_argument, 0, 'f'},
		{"dbname", required_argument, 0, 'p'},
		{"udpport", required_argument, 0, 'u'},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hm:f:p:u:", long_options, NULL))!=-1){
		switch(oc){
		case 'm':
			optd->dlogmem=strtol(optarg, NULL, 0);
			break;
		case 'f':
			optd->dlogfile=optarg;
			break;
		case 'p':
			optd->udbopt.dbname=optarg;
			break;
		case 'u':
			optd->udbopt.udpport=strtol(optarg, NULL, 0);
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

int main(int argc, char *argv[])
{
	int res=-1;
	ucudpbindopt_t optd;
	unibase_init_para_t init_para;
	struct sigaction sigact;

	memset(&optd, 0, sizeof(optd));
	optd.udbopt.dbname="uniconfdb";
	optd.udbopt.udpport=UCUDPBIND_PORT;
	optd.dlogfile="ucudpbind.log";
	optd.dlogmem=64;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:46,uconf:46",
						   "UBL_UNICONF");
	unibase_init(&init_para);
	if(set_options(&optd, argc, argv)<0){goto erexit;}
	ubb_memory_out_init(NULL, optd.dlogmem*1024);

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);

	res=ucudpbin_main(&optd.udbopt, &stoprun);
erexit:
	if(optd.dlogmem && optd.dlogfile && ubb_memory_file_out(optd.dlogfile)){
		UB_LOG(UBL_ERROR, "%s:can't write the memory log into a file\n", __func__);
	}
	ubb_memory_out_close();
	unibase_close();
	return res;
}

#endif // #ifndef UCUDPBIN_IN_LIBRARY
