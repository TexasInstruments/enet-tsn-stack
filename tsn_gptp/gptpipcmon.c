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
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include "gptpbasetypes.h"
#include "gptpipc.h"
#include "mdeth.h"
#include "md_abnormal_hooks.h"
#include "tsn_unibase/unibase_macros.h"
#include <tsn_unibase/unibase_binding.h>

static int main_terminate;

static int print_usage(char *pname)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-d|--domain domainIndex\n");
	UB_CONSOLE_PRINT("-p|--port portIndex\n");
	UB_CONSOLE_PRINT("-n|--node: print IPC node and exit\n");
	UB_CONSOLE_PRINT("-u|--udpport port_number: use UDP mode connection with port_number\n");
	UB_CONSOLE_PRINT("-a|--udpaddr IP address: use UDP mode connection with target IP\n");
	UB_CONSOLE_PRINT("-i|--interval query interval time(msec unit):\n");
	UB_CONSOLE_PRINT("-o|--oneshot: print one shot of messages and terminate program\n");
	return -1;
}

typedef struct gptpipcmon {
	int domainIndex;
	int portIndex;
	uint16_t udpport;
	char *udpaddr;
	int query_interval; //in msec
}gptpipcmon_t;

static int set_options(gptpipcmon_t *gipcmd, int argc, char *argv[])
{
	int oc;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"domain", required_argument, 0, 'd'},
		{"port", required_argument, 0, 'p'},
		{"node", no_argument, 0, 'n'},
		{"udpport", required_argument, 0, 'u'},
		{"udpaddr", required_argument, 0, 'a'},
		{"interval", required_argument, 0, 'i'},
		{"oneshot", no_argument, 0, 'o'},
	};
	while((oc=getopt_long(argc, argv, "hd:p:nu:a:i:o", long_options, NULL))!=-1){
		switch(oc){
		case 'd':
			gipcmd->domainIndex=strtol(optarg, NULL, 0);
			break;
		case 'p':
			gipcmd->portIndex=strtol(optarg, NULL, 0);
			break;
		case 'n':
			UB_CONSOLE_PRINT("%s\n", GPTP2D_IPC_CB_SOCKET_NODE);
			return 1;
		case 'u':
			gipcmd->udpport=strtol(optarg, NULL, 0);
			break;
		case 'a':
			gipcmd->udpaddr=optarg;
			break;
		case 'i':
			gipcmd->query_interval=strtol(optarg, NULL, 0);
			break;
		case 'o':
			main_terminate=1;
			break;
		case 'h':
		default:
			return print_usage(argv[0]);
		}
	}
	return 0;
}

static void signal_handler(int sig)
{
	main_terminate=1;
}

static void print_key_commands(char *rbuf)
{
	UB_CONSOLE_PRINT("bad request command:%s\n",rbuf);
	UB_CONSOLE_PRINT("    G domain,port : GM port info\n");
	UB_CONSOLE_PRINT("    N [domain],port : Network port info, "
			 "domain is not used\n");
	UB_CONSOLE_PRINT("    C domain,port : Clock info\n");
	UB_CONSOLE_PRINT("    T [domain,port] : Statistics info, "
			 "domain=-1:all domains, port=0:all ports\n");
	UB_CONSOLE_PRINT("    R [domain,port] : Reset Statistics info, "
			 "domain=-1:all domains, port=0:all ports\n");
	UB_CONSOLE_PRINT("    S: start TSN Scheduling\n");
	UB_CONSOLE_PRINT("    s: stop TSN Scheduling\n");
	UB_CONSOLE_PRINT("    A: domain,port,msgtype,eventtype,eventrate,"
			 "repeat,interval,eventpara: register an abnormal event\n");
	UB_CONSOLE_PRINT("       msgtype='none|sync|fup|pdreq|pdres|pdrfup|ann|sign'\n");
	UB_CONSOLE_PRINT("       eventtype='none|skip|dup|badsqn|nots|sender'\n");
	UB_CONSOLE_PRINT("       eventrate='0.0 to 1.0'\n");
	UB_CONSOLE_PRINT("       repeat: repeat times, 0 is infinite times\n");
	UB_CONSOLE_PRINT("       interval: interval when repeat > 1\n");
	UB_CONSOLE_PRINT("       eventpara: parameter for the event\n");
	UB_CONSOLE_PRINT("    a: deregister abnormal events\n");
}

static int ipc_register_abnormal_event(int ipcfd, char *rbuf)
{
	gptpipc_client_req_data_t cd;
	char *msgtype=NULL;
	char *eventtype=NULL;
	int res;
	int rval=-1;
	int i;

	const char *msgtstr[]={"none","sync","fup","pdreq","pdres",
			       "pdrfup","ann","sign",NULL};
	int32_t msgtint[]={-1,(int32_t)SYNC,(int32_t)FOLLOW_UP,(int32_t)PDELAY_REQ,
			   (int32_t)PDELAY_RESP,(int32_t)PDELAY_RESP_FOLLOW_UP,
			   (int32_t)ANNOUNCE, (int32_t)SIGNALING};
	const char *eventtstr[]={"none","skip","dup","badsqn","nots","sender"};
	int32_t eventtint[]={(int32_t)MD_ABN_EVENT_NONE,(int32_t)MD_ABN_EVENT_SKIP,
			     (int32_t)MD_ABN_EVENT_DUP,(int32_t)MD_ABN_EVENT_BADSEQN,
			     (int32_t)MD_ABN_EVENT_NOTS,(int32_t)MD_ABN_EVENT_SENDER};

	memset(&cd, 0, sizeof(cd));
	cd.cmd=GPTPIPC_CMD_REG_ABNORMAL_EVENT;
	if(rbuf[0]=='a'){cd.u.abnd.subcmd=1;}
	res=sscanf(rbuf+1, "%d,%d,%m[a-z],%m[a-z],%f,%d,%d,%d",
		   &cd.domainNumber, &cd.portIndex,
		   &msgtype, &eventtype,
		   &cd.u.abnd.eventrate, &cd.u.abnd.repeat, &cd.u.abnd.interval,
		   &cd.u.abnd.eventpara);
	if(res<4) {
		if(!cd.u.abnd.subcmd){
			UB_LOG(UBL_ERROR, "%s:number of parameters=%d is wrong\n",__func__,res);
			goto erexit;
		}
		cd.u.abnd.msgtype=-1;
	}
	if(res>=3){
		for(i=0;;i++){
			if(!msgtstr[i]){goto erexit;}
			if(strcmp(msgtype,msgtstr[i])==0){
				cd.u.abnd.msgtype=msgtint[i];
				break;
			}
		}
	}
	if(res>=4){
		for(i=0;;i++){
			if(!eventtstr[i]){goto erexit;}
			if(strcmp(eventtype,eventtstr[i])==0){
				cd.u.abnd.eventtype=eventtint[i];
				break;
			}
		}
	}
	if(res<5){cd.u.abnd.eventrate=1.0;}
	if(res<6){cd.u.abnd.repeat=0;}
	if(res<7){cd.u.abnd.interval=0;}
	if(res<8){cd.u.abnd.eventpara=0;}
	rval=0;
	res=write(ipcfd, &cd, sizeof(cd));
	if(res!=sizeof(cd)){
		UB_LOG(UBL_ERROR, "%s:can't send to the IPC socket\n",__func__);
	}
erexit:
	if(rval){print_key_commands(rbuf);}
	if(msgtype){free(msgtype);}
	if(eventtype){free(eventtype);}
	return 0;
}

static int read_stdin(gptpipc_thread_data_t *ipctd)
{
	char rbuf[64];
	char *pb;
	int rsize;
	int domain=0;
	int port=0;
	rsize=read(STDIN_FILENO, rbuf, sizeof(rbuf)-1);
	if(rsize<0){
		UB_LOG(UBL_ERROR,"%s:stdin read error %s\n", __func__,strerror(errno));
		return 1;
	}
	pb=strchr(rbuf,'\n');
	if(pb){*pb=0;}
	if(!strcmp(rbuf,"q")){return 1;}
	if(rbuf[0]=='A' || rbuf[0]=='a'){
		return ipc_register_abnormal_event(ipctd->ipcfd, rbuf);
	}
	if(strchr(rbuf,',')){
		if(sscanf(rbuf+1,"%d,%d", &domain, &port)!=2){
			UB_LOG(UBL_WARN,"%s:bad format in 'domain,port'\n",__func__);
			return 0;
		}
	}else{
		if(sscanf(rbuf+1,"%d", &port)!=1){port=-1;}
	}
	switch(rbuf[0]){
	case 'G':
		return send_ipc_request(ipctd->ipcfd, domain, port,
					GPTPIPC_CMD_REQ_GPORT_INFO);
	case 'N':
		return send_ipc_request(ipctd->ipcfd, domain, port,
					GPTPIPC_CMD_REQ_NDPORT_INFO);
	case 'C':
		return send_ipc_request(ipctd->ipcfd, domain, port,
					GPTPIPC_CMD_REQ_CLOCK_INFO);
	case 'T':
		return send_ipc_request(ipctd->ipcfd, domain, port,
					GPTPIPC_CMD_REQ_STAT_INFO);
	case 'R':
		return send_ipc_request(ipctd->ipcfd, domain, port,
					GPTPIPC_CMD_REQ_STAT_INFO_RESET);
	case 'S':
		return send_ipc_request(ipctd->ipcfd, 1, 0,
					GPTPIPC_CMD_TSN_SCHEDULE_CONTROL);
	case 's':
		return send_ipc_request(ipctd->ipcfd, 0, 0,
					GPTPIPC_CMD_TSN_SCHEDULE_CONTROL);
	default:
		print_key_commands(rbuf);
	}
	return 0;
}

static int ipcmon_notice_cb(gptpipc_gptpd_data_t *ipcrd, void *ptr)
{
	switch(ipcrd->dtype){
	case GPTPIPC_GPTPD_GPORTD:
		/* do nothing */
		break;
	case GPTPIPC_GPTPD_CLOCKD:
		/* The following displays received clock data contents for gptp
		 * `thisClock` (portIndex=0). */
		if(ipcrd->u.clockd.portIndex==0){
			printf("-----------------------------------------------------------\n");
			printf("%s: domainNumber=%d, portIndex=%d clockIdentity="UB_PRIhexB8"\n"
					"\t %s GM="UB_PRIhexB8"\n"
					"\t lastSyncSeqID=%d lastSyncReceiptTime_nsec=%"PRIu64" lastSyncReceiptLocalTime_nsec=%"PRIu64"\n"
					"\t adjppb=%d\n",
					__func__, ipcrd->u.clockd.domainNumber, ipcrd->u.clockd.portIndex,
					UB_ARRAY_B8(ipcrd->u.clockd.clockId),
					ipcrd->u.clockd.gmsync?"GM_SYNC":"GM_UNSYNC",
					UB_ARRAY_B8(ipcrd->u.clockd.gmClockId),
					ipcrd->u.clockd.lastSyncSeqID, ipcrd->u.clockd.lastSyncReceiptTime_nsec,
					ipcrd->u.clockd.lastSyncReceiptLocalTime_nsec,
					ipcrd->u.clockd.adjppb);
			printf("-----------------------------------------------------------\n");
		}
		break;
	default:
		/* do nothing */
		break;
	}

	return 0;
}

static int ipc_reconnect(gptpipc_thread_data_t *ipctd, bool *ipcrun)
{
	if(*ipcrun){
		gptpipc_close(ipctd);
		*ipcrun=false;
	}
	while(gptpipc_init(ipctd, 1)){
		UB_TLOG(UBL_ERROR, "gptpipc can't be connected in 1 seconds, try again\n");
		gptpipc_close(ipctd);
		if(main_terminate){return -1;}
	}
	*ipcrun=true;
	return 0;
}

int main(int argc, char *argv[])
{
	gptpipcmon_t gipcmd;
	int res;
	struct sigaction sigact;
	char suffix[64];
	uint64_t ts64;
	gptpipc_thread_data_t ipctd;
	bool ipcrun=false;
	unibase_init_para_t init_para;

	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,gptp:40", "UBL_GPTP");
	unibase_init(&init_para);
	memset(&gipcmd, 0, sizeof(gipcmd));
	memset(&ipctd, 0, sizeof(ipctd));
	gipcmd.domainIndex=-1;
	gipcmd.portIndex=-1;
	gipcmd.query_interval=10000;
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);

	res=set_options(&gipcmd, argc, argv);
	if(res<0){return -1;}
	if(res){return 0;}

	ts64=ub_rt_gettime64();
	sprintf(suffix, "%"PRIi64, ts64);
	ipctd.pname=suffix;
	ipctd.printdata=true;
	ipctd.query_interval=gipcmd.query_interval;
	ipctd.cb=ipcmon_notice_cb;
	ipctd.cbdata=&ipctd;
	ipctd.udpport=gipcmd.udpport;
	ipctd.udpaddr=gipcmd.udpaddr;
	if(ipc_reconnect(&ipctd, &ipcrun)){return -1;}
	if(gipcmd.domainIndex>0){
		send_ipc_request(ipctd.ipcfd, gipcmd.domainIndex, 1, GPTPIPC_CMD_REQ_GPORT_INFO);
		send_ipc_request(ipctd.ipcfd, gipcmd.domainIndex, 0, GPTPIPC_CMD_REQ_CLOCK_INFO);
		send_ipc_request(ipctd.ipcfd, gipcmd.domainIndex, 1, GPTPIPC_CMD_REQ_CLOCK_INFO);
		usleep(100000);
	}
	while(!main_terminate){
		res=read_stdin(&ipctd);
		if(res==1){break;}
		if(res==-1) {
			if(ipc_reconnect(&ipctd, &ipcrun)){return -1;}
		}
	}
	if(ipcrun) {
		send_ipc_request(ipctd.ipcfd, 0, 0, GPTPIPC_CMD_DISCONNECT);
		gptpipc_close(&ipctd);
	}
	return 0;
}
