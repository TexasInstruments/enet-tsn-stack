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
#include <errno.h>
#include <tsn_unibase/unibase.h>
#include "gptpclock.h"
#include "gptpmasterclock.h"

#ifndef GPTPMCLCK_PLATFORM_INIT
#define GPTPMCLCK_PLATFORM_INIT
#endif //GPTPMCLCK_PLATFORM_INIT

#ifndef GPTPMCLCK_PLATFORM_CLOSE
#define GPTPMCLCK_PLATFORM_CLOSE
#endif //GPTPMCLCK_PLATFORM_CLOSE

typedef struct gptp_master_clock_data{
	int max_domains;
	int shmfd;
	int shmsize;
	gptp_master_clock_shm_t *shm;
	PTPFD_TYPE *ptpfds;
	int suppress_msg;
	int ref_counter;
	char shmem_name[GPTP_MAX_SIZE_SHARED_MEMNAME];
} gptp_master_clock_data_t;

static gptp_master_clock_data_t gmcd;

#define GMCD_INIT_CHECK ((gmcd.max_domains!=0)?0:gptpmasterclock_init(NULL))

static int ptpdev_open(void)
{
	int i;
	int res=-1;
	for(i=0;i<gmcd.max_domains;i++){
		if(gmcd.ptpfds[i]!=0){continue;} // already opened
		if(!gmcd.shm->gcpp[i].ptpdev[0]){continue;} // no ptpdev, it may come later
		gmcd.ptpfds[i]=PTPDEV_CLOCK_OPEN(gmcd.shm->gcpp[i].ptpdev, O_RDONLY);
		if(!PTPFD_VALID(gmcd.ptpfds[i])){
			UB_LOG(UBL_ERROR, "ptpdev=%s, can't open, %s\n",
			       gmcd.shm->gcpp[i].ptpdev, strerror(errno));
			return -1;
		}
		UB_LOG(UBL_DEBUG,"domainIndex=%d, ptpdev=%s\n",i,gmcd.shm->gcpp[i].ptpdev);
		res=0;
	}
	return res;
}

static int gptpmasterclock_health_check(int domainIndex)
{
	if(GMCD_INIT_CHECK!=0){return -1;}
	if(!gmcd.shm->head.max_domains){
		UB_LOG(UBL_ERROR, "%s:tsn_gptpd might be closed, re-initialize now\n",__func__);
		(void)gptpmasterclock_close();
		if(gptpmasterclock_init(gmcd.shmem_name)!=0){return -1;}
	}
	if(!PTPFD_VALID(gmcd.ptpfds[domainIndex])){
		// this must be rare case. when there are multiple ptp devices,
		// there might be latency to add ptp devices.
		if(!gmcd.suppress_msg){
			UB_LOG(UBL_INFO, "%s:domainIndex=%d, ptpdev is not yet opened\n",
			       __func__, domainIndex);
		}
		// when ptpdev is not yet opened, it may be opened this time
		if(ptpdev_open() || !PTPFD_VALID(gmcd.ptpfds[domainIndex])){
			gmcd.suppress_msg=1;
			return -1;
		}
		UB_LOG(UBL_INFO, "%s:domainIndex=%d, ptpdev=%s is opened\n",
		       __func__, domainIndex, gmcd.shm->gcpp[domainIndex].ptpdev);
		gmcd.suppress_msg=0;
	}
	return 0;
}

#define GPTPMASTER_PTPFD gptpmaster_ptpfd
#ifndef GPTPMASTER_PTPFDNUM
#define GPTPMASTER_PTPFDNUM 2
#endif
UB_SD_GETMEM_DEF(GPTPMASTER_PTPFD, sizeof(PTPFD_TYPE), GPTPMASTER_PTPFDNUM);

int gptpmasterclock_init(const char *shmem_name)
{
	int *dnum;

	GPTPMCLCK_PLATFORM_INIT;

	gmcd.ref_counter++;
	UB_LOG(UBL_INFO, "%s: tsn_gptp-"TSNPKGVERSION", ref_counter=%d\n",
	       __func__, gmcd.ref_counter);
	if(gmcd.max_domains!=0){
		UB_LOG(UBL_DEBUG, "%s: already initialized\n", __func__);
		return 0;
	}
	if(gmcd.suppress_msg!=0){(void)ub_log_change(CB_COMBASE_LOGCAT, UBL_NONE, UBL_NONE);}
	if(shmem_name && shmem_name[0]) {
		(void)snprintf(gmcd.shmem_name, GPTP_MAX_SIZE_SHARED_MEMNAME, "%s", shmem_name);
	}else{
		(void)strcpy(gmcd.shmem_name, GPTP_MASTER_CLOCK_SHARED_MEM);
	}
	dnum=(int*)cb_get_shared_mem(&gmcd.shmfd, gmcd.shmem_name, sizeof(int), O_RDONLY);
	if(gmcd.suppress_msg!=0){(void)ub_log_return(CB_COMBASE_LOGCAT);}
	if(!dnum){
		if(!gmcd.suppress_msg){
			UB_LOG(UBL_ERROR, "%s:master clock is not yet register by gptp\n",
			       __func__);
		}
		goto erexit;
	}
	if(*dnum==0){
		if(!gmcd.suppress_msg){
			UB_LOG(UBL_ERROR, "%s:master clock is not yet added\n", __func__);
		}
		goto erexit;
	}
	UB_LOG(UBL_DEBUG, "%s:*dnum=%d\n", __func__, *dnum);
	gmcd.max_domains=*dnum;
	cb_close_shared_mem(dnum, &gmcd.shmfd, gmcd.shmem_name, sizeof(int), false);
	gmcd.shmsize=((int)sizeof(gptp_clock_ppara_t)*gmcd.max_domains) +
		(int)sizeof(gptp_master_clock_shm_head_t);
	gmcd.shm=(gptp_master_clock_shm_t *)cb_get_shared_mem(
		&gmcd.shmfd, gmcd.shmem_name, gmcd.shmsize, O_RDWR);
	if(!gmcd.shm){
		UB_LOG(UBL_ERROR, "%s:can't get the shared memory\n", __func__);
		goto erexit;
	}
	gmcd.ptpfds=(PTPFD_TYPE *)UB_SD_GETMEM(GPTPMASTER_PTPFD, gmcd.max_domains*(int)sizeof(PTPFD_TYPE));
	if(ub_assert_fatal(gmcd.ptpfds!=NULL, __func__, "malloc error")){goto erexit;}
	(void)memset(gmcd.ptpfds,0,gmcd.max_domains*(int)sizeof(PTPFD_TYPE));
	if(ptpdev_open()!=0){goto erexit;}
	if(gmcd.suppress_msg!=0){
		UB_LOG(UBL_INFO, "%s:recovered\n",__func__);
		gmcd.suppress_msg=0;
	}
	UB_LOG(UBL_DEBUG, "%s:done\n",__func__);
	return 0;
erexit:
	if(!gmcd.suppress_msg){
		UB_LOG(UBL_INFO, "%s:failed\n",__func__);
		(void)gptpmasterclock_close();
		gmcd.suppress_msg=1;
	}else{
		(void)gptpmasterclock_close();
	}
	gmcd.ref_counter--;
	return -1;
}

int gptpmasterclock_close(void)
{
	int i;
	if(!gmcd.max_domains){return -1;}
	if(!gmcd.shm){return -1;}
	gmcd.ref_counter--;
	UB_LOG(UBL_INFO, "%s: ref_counter=%d\n", __func__, gmcd.ref_counter);
	if(gmcd.ref_counter>0){return 0;}
	if(gmcd.ptpfds!=NULL){
		for(i=0;i<gmcd.max_domains;i++){
			if(!gmcd.shm->gcpp[i].ptpdev[0]){continue;}
			PTPDEV_CLOCK_CLOSE(gmcd.ptpfds[i]);
		}
		UB_SD_RELMEM(GPTPMASTER_PTPFD, gmcd.ptpfds);
		gmcd.ptpfds=NULL;
	}
	cb_close_shared_mem(gmcd.shm, &gmcd.shmfd,
			    gmcd.shmem_name, gmcd.shmsize, false);
	(void)memset(&gmcd, 0, sizeof(gmcd));

	GPTPMCLCK_PLATFORM_CLOSE;

	return 0;
}

void gptpmasterclock_dump_offset(void)
{
	int i;
	if(GMCD_INIT_CHECK!=0){return;}
	UB_CONSOLE_PRINT("max_domains=%d, active_domain=%d\n", gmcd.shm->head.max_domains,
			 gmcd.shm->head.active_domain);
	for(i=0;i<gmcd.max_domains;i++){
		UB_CONSOLE_PRINT("index=%d, domainIndex=%d, gmsync=%d, gmchange_ind=%"PRIu32"\n",
				 i, gmcd.shm->gcpp[i].domainIndex,gmcd.shm->gcpp[i].gmsync,
				 gmcd.shm->gcpp[i].gmchange_ind);
		UB_CONSOLE_PRINT("offset %"PRIi64"nsec\n", gmcd.shm->gcpp[i].offset64);
	}
	UB_CONSOLE_PRINT("\n");
}

int gptpmasterclock_gm_domainIndex(void)
{
	if(GMCD_INIT_CHECK!=0){return -1;}
	return gmcd.shm->head.active_domain;
}

int gptpmasterclock_gmchange_ind(void)
{
	int i;
	i= gptpmasterclock_gm_domainIndex();
	if(i<0){return -1;}
	return gmcd.shm->gcpp[i].gmchange_ind;
}

int gptpmasterclock_get_max_domains(void)
{
	return gmcd.max_domains;
}

int gptpmasterclock_get_domain_ts64(int64_t *ts64, int domainIndex)
{
	int64_t dts;
	double adjrate;
	int rval=-1;
	if(gptpmasterclock_health_check(domainIndex)!=0){return -1;}
	if((domainIndex<0) || (domainIndex>=gmcd.max_domains)){return -1;}
	(void)gptpclock_mutex_trylock(&gmcd.shm->head.mcmutex);
	PTPDEV_CLOCK_GETTIME(gmcd.ptpfds[domainIndex], *ts64);

	adjrate=gmcd.shm->gcpp[domainIndex].adjrate;
	if(adjrate != 0.0){
		// get dts, which is diff between now and last setts time
		dts=*ts64-gmcd.shm->gcpp[domainIndex].last_setts64;
	}

	// add offset
	*ts64+=gmcd.shm->gcpp[domainIndex].offset64;
	if(adjrate != 0.0){*ts64+=(double)dts*adjrate;}
	rval=0;

	CB_THREAD_MUTEX_UNLOCK(&gmcd.shm->head.mcmutex);
	return rval;
}

int64_t gptpmasterclock_getts64(void)
{
	int64_t ts64;
	if(GMCD_INIT_CHECK!=0){return -1;}
	if(gptpmasterclock_get_domain_ts64(&ts64, gmcd.shm->head.active_domain)!=0){return -1;}
	return ts64;
}

/*
 * We need an accurate timer here to sleep
 * nanosleep relies on 'Linux kernel hrtimers'; check hrtimers.txt
 * in the kernel Documents.
 */
int gptpmasterclock_wait_until_ts64(int64_t tts, int64_t vclose, int64_t toofar)
{
	int64_t cts,dts;
	int64_t rem;

	cts=gptpmasterclock_getts64();
	dts=tts-cts;
	if(dts<0){return 1;}
	if (dts <= vclose){return 2;}
	if(toofar && (dts >= toofar)){
		UB_LOG(UBL_INFO,"%s: %"PRIi64"nsec is farther than %"PRIi64"\n",
		       __func__, dts, toofar);
		return 3;
	}
	UB_LOG(UBL_DEBUG,"%s: wait for %"PRIi64"nsec\n", __func__, dts);
	if(cb_nanosleep64(dts,&rem)!=0){
		if(errno==EINTR){
			cb_nanosleep64(rem, NULL);
		}else{
			UB_LOG(UBL_ERROR,"%s: error in nanosleep: %s:\n",
			       __func__, strerror(errno));
		}
		return -1;
	}
	return 0;
}

uint64_t gptpmasterclock_expand_timestamp(uint32_t timestamp)
{
	return gptpmasterclock_expand_timestamp_ts64(timestamp, gptpmasterclock_getts64());
}

uint64_t gptpmasterclock_expand_timestamp_ts64(uint32_t timestamp, int64_t ts64)
{
	int64_t dtime;
	// make dtime in a range of  -2.147 to 2.147 secconds
	dtime=((int64_t)timestamp - (ts64 & 0xffffffffLL));
	if(dtime>INT32_MAX){
		return ts64+(int64_t)INT32_MIN+dtime-((int64_t)INT32_MAX+1);
	}else if(dtime<INT32_MIN){
		return ts64+(int64_t)INT32_MAX+dtime-((int64_t)INT32_MIN-1);
	}else {
		return ts64+dtime;
	}
}
