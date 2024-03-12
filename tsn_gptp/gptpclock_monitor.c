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
#include <stdio.h>
#include <getopt.h>
#include <tsn_unibase/unibase_binding.h>
#include "gptpcommon.h"
#include "gptpmasterclock.h"

static int verbose;
static int oneshot;
static int dcdiff;
static char *shmem_name;
static int sgap;

static int64_t get_two_ts_diff(int64_t *ts64, int64_t *rts64, int di)
{
	int64_t ts1, ts2, ts3;
	if(di<0){
		ts1=gptpmasterclock_getts64();
	}else{
		if(gptpmasterclock_get_domain_ts64(&ts1, di)) {
			UB_LOG(UBL_ERROR, "domain=%d is not used\n", di);
			return -1;
		}
	}
	ts2=ub_rt_gettime64();
	if(di<0){
		ts3=gptpmasterclock_getts64();
	}else{
		if(gptpmasterclock_get_domain_ts64(&ts3, di)) {
			UB_LOG(UBL_ERROR, "domain=%d is not used\n", di);
			return -1;
		}
	}
	if(ts3-ts1>200000){
		UB_LOG(UBL_ERROR, "%s:dts=%"PRIi64" nsec\n", __func__, ts3-ts1);
		return -1;
	}
	*rts64=ts2;
	*ts64=(ts1+ts3)/2;
	return *ts64-*rts64;
}

static int read_all_domains(int max_domains, int64_t *dts64, int *invalid_domain)
{
	int i;
	int64_t ts64, rts64;
	for(i=max_domains>1?-1:0; i<max_domains; i++){
		if(invalid_domain && invalid_domain[i+1]){continue;}
		if((dts64[i+1]=get_two_ts_diff(&ts64, &rts64, i))==-1) {
			if(!invalid_domain){return -1;}
			invalid_domain[i+1]=1;
			continue;
		}
		if(oneshot){
			printf("domain=%d %"PRIi64" %"PRIi64"\n", i, ts64, dts64[i+1]);
		}else{
			printf("domain=%d is %"PRIi64"sec %"PRIi64"nsec\n", i,
			       (int64_t)(ts64/UB_SEC_NS), (int64_t)(ts64%UB_SEC_NS));
			printf(", %"PRIi64"sec %"PRIi64"nsec ahead to CLOCK_REALTIME\n",
			       (int64_t)(dts64[i+1]/UB_SEC_NS), (int64_t)(dts64[i+1]%UB_SEC_NS));
		}
	}
	return 0;
}

static int get_dc_diff(int64_t *dts64)
{
	int64_t dts, tts;
	int64_t ts0, ts1;
	int i;
	uint64_t ts64;
	for(i=0;i<5;i++){
		gptpmasterclock_get_domain_ts64(&ts0, 0);
		if(gptpmasterclock_get_domain_ts64(&tts, dcdiff)) {
			UB_LOG(UBL_ERROR, "%s: domain=%d is not used\n", __func__, dcdiff);
			return -1;
		}
		gptpmasterclock_get_domain_ts64(&ts1, 0);
		dts=ts1-ts0;
		if(dts>1000000){continue;}
		break;
	}
	if(i==5) {
		UB_LOG(UBL_ERROR, "%s:two ts is further than 1msec, %"PRIi64"nsec\n",
		       __func__, dts);
		return -1;
	}
	ts64=(ts0+ts1)/2;
	*dts64=tts-ts64;
	return 0;
}

int main_loop(void)
{
	int64_t ts64=0, rts64=0;
	int64_t *dts64a, *dts64b;
	int *invalid_domain;
	int adi, last_adi;
	int max_domains;
	int i;
	int stoprun=0;
	bool jump;

	max_domains=gptpmasterclock_get_max_domains();
	if(dcdiff>max_domains){return -1;}
	dts64a=malloc((max_domains+1)*sizeof(int64_t));
	dts64b=malloc((max_domains+1)*sizeof(int64_t));
	invalid_domain=malloc((max_domains+1)*sizeof(int));
	memset(invalid_domain, 0, (max_domains+1)*sizeof(int));
	if(verbose){gptpmasterclock_dump_offset();}
	last_adi=gptpmasterclock_gm_domainIndex();
	if(last_adi<0){
		printf("gptp may not be running at the first time, reinitialize\n");
		return 2;
	}
	if(dcdiff){
		if(get_dc_diff(&dts64a[dcdiff])){return -1;}
		printf("%"PRIi64"\n", dts64a[dcdiff]);
	}else{
		if(max_domains>1){
			printf("active domain=%d, the value is shown as '-1' domain\n",
			       last_adi);
		}
		read_all_domains(max_domains, dts64a, invalid_domain);
	}
	if(oneshot){return 1;}
	while(!stoprun){
		adi=gptpmasterclock_gm_domainIndex();
		if(adi==-1){
			printf("gptp may not be running, reinitialize\n");
			return 2;
		}
		if(adi!=last_adi){
			printf("active domain changed from %d to %d\n", last_adi, adi);
			last_adi=adi;
		}
		jump=false;
		for(i=max_domains>1?-1:0; i<max_domains; i++){
			if(dcdiff && dcdiff != i){continue;}
			if(invalid_domain[i+1]){continue;}
			if(dcdiff){
				if(get_dc_diff(&dts64b[dcdiff])){return -1;}
			}else{
				if((dts64b[i+1]=get_two_ts_diff(&ts64, &rts64, i))==-1) {
					stoprun=1;
					break;
				}
			}
			if(llabs(dts64b[i+1]-dts64a[i+1])>(UB_SEC_NS/1000)){
				jump=true;
				printf("big jump(%"PRIi64"nsec) happned, domain=%d\n",
				       dts64b[i+1]-dts64a[i+1], i);
				printf("domain=%d is %"PRIi64"sec %"PRIi64
				       "nsec ahead to CLOCK_REALTIME\n",
				       i, (int64_t)(dts64b[i+1]/UB_SEC_NS),
				       (int64_t)(dts64b[i+1]%UB_SEC_NS));
				if(verbose){
					printf("system clock %"PRIi64"sec %"PRIi64"nsec\n",
					       (int64_t)(rts64/UB_SEC_NS),
					       (int64_t)(rts64%UB_SEC_NS));
					printf("ptp clock    %"PRIi64"sec %"PRIi64"nsec\n",
					       (int64_t)(ts64/UB_SEC_NS),
					       (int64_t)(ts64%UB_SEC_NS));
				}
			}
			dts64a[i+1]=dts64b[i+1];
		}
		usleep(10000); // 10mses sleep
		if(jump) {
			if(verbose){gptpmasterclock_dump_offset();}
			if(dcdiff){
				if(get_dc_diff(&dts64a[dcdiff])){return -1;}
				printf("%"PRIi64"\n", dts64a[dcdiff]);
			}else{
				read_all_domains(max_domains, dts64a, invalid_domain);
			}
		}
	}
	return 0;
}

static int print_usage(char *pname)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-v|--vorbose: print verbose messages\n");
	UB_CONSOLE_PRINT("-o|--oneshot: print one shot of messages and terminate program\n");
	UB_CONSOLE_PRINT("-d|--diff domain_N: show time diff between domain_N and domain_0\n");
	UB_CONSOLE_PRINT("-s|--shmem shmem_name: shared memory node name\n");
	return -1;
}

static int set_options(int argc, char *argv[])
{
	int oc;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"verbose", no_argument, 0, 'v'},
		{"oneshot", no_argument, 0, 'o'},
		{"diff", required_argument, 0, 'd'},
		{"shmem", required_argument, 0, 's'},
		{"sgap", no_argument, 0, 'g'},
	};
	while((oc=getopt_long(argc, argv, "hvod:s:g", long_options, NULL))!=-1){
		switch(oc){
		case 'v':
			verbose=1;
			break;
		case 'o':
			oneshot=1;
			break;
		case 'd':
			dcdiff=strtol(optarg, NULL, 0);
			break;
		case 's':
			shmem_name=optarg;
			break;
		case 'g':
			sgap=1;
			break;
		case 'h':
		default:
			return print_usage(argv[0]);
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int res;
	unibase_init_para_t init_para;

	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46,gptp:46",
						   "UBL_GPTP");
	unibase_init(&init_para);
	if(set_options(argc, argv)){return 1;}
	if(gptpmasterclock_init(shmem_name)){return 1;}
	if(sgap){
		printf("clockgap=%"PRIi64"\n", gptpmasterclock_getts64()-ub_rt_gettime64());
		goto mexit;
	}
	while(true){
		res=main_loop();
		if(res==2){
			sleep(1); // sleep one sec before re-initialization
			gptpmasterclock_close();
			gptpmasterclock_init(shmem_name);
			continue;
		}
		if(res){break;}
		if(oneshot){break;}
	}
mexit:
	gptpmasterclock_close();
	return 0;
}
