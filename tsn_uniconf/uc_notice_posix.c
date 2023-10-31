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
#include <errno.h>
#include "uc_notice.h"

UB_SD_GETMEM_DEF_EXTERN(UC_NOTICE_PUT);

int uc_notice_sig_open(bool thread, UC_NOTICE_SIG_T **sigp, int *master, const char *name)
{
	if(!name){return -1;}
	if(thread){
		UB_TLOG(UBL_DEBUG,"%s:thread, master=%p\n", __func__, master);
		if(!master){
			UB_TLOG(UBL_DEBUG,"%s:only master can create in thread mode\n",
				__func__);
			return -1;
		}
		*sigp=UB_SD_GETMEM(UC_NOTICE_PUT, sizeof(CB_SEM_T));
		if(ub_assert_fatal(*sigp!=NULL, __func__, NULL)){return -1;}
		(void)memset(*sigp, 0, sizeof(CB_SEM_T));
		if(CB_SEM_INIT(*sigp, 0, *master)!=0){
			UB_SD_RELMEM(UC_NOTICE_PUT, *sigp);
			*sigp=NULL;
			return -1;
		}
		return 0;
	}else{
		UB_TLOG(UBL_DEBUG,"%s:process, master=%p\n", __func__, master);
		if(master!=NULL){
			CB_SEM_UNLINK(name);
			*sigp=CB_SEM_OPEN(name, O_CREAT|O_EXCL,
					  ((mode_t)6<<6)|((mode_t)6<<3)|(mode_t)6, *master);
		}else{
			*sigp=CB_SEM_OPEN(name, 0);
		}
		if((CB_SEM_T*)*sigp==CB_SEM_FAILED){
			UB_LOG(UBL_ERROR, "%s:can't open a named semaphore:%s\n",
			       __func__, name);
			*sigp=NULL;
			return -1;
		}
		return 0;
	}
}

void uc_notice_sig_close(bool thread, UC_NOTICE_SIG_T *sigp, bool master, const char *name)
{
	if(!sigp){return;}
	if(!name){return;}
	if(master){
		if(thread){
			CB_SEM_DESTROY((CB_SEM_T*)sigp);
			UB_SD_RELMEM(UC_NOTICE_PUT, sigp);
		}else{
			CB_SEM_CLOSE((CB_SEM_T*)sigp);
			CB_SEM_UNLINK(name);
		}
	}else{
		CB_SEM_CLOSE((CB_SEM_T*)sigp);
	}
}

int uc_notice_sig_getvalue(bool thread, UC_NOTICE_SIG_T *sigp, int *sval)
{
	return CB_SEM_GETVALUE((CB_SEM_T*)sigp, sval);
}

int uc_notice_sig_post(bool thread, UC_NOTICE_SIG_T *sigp)
{
	return CB_SEM_POST((CB_SEM_T*)sigp);
}

int uc_notice_sig_check(bool thread, UC_NOTICE_SIG_T *sigp, int tout_ms, const char *fname)
{
	if(!tout_ms){
		if(CB_SEM_TRYWAIT((CB_SEM_T*)sigp)!=0){
			if(errno==EAGAIN){return 1;}
			goto erexit;
		}
	}else if(tout_ms<0){
		if(CB_SEM_WAIT((CB_SEM_T*)sigp)!=0){
			goto erexit;
		}
	}else{
		int64_t ts64;
		struct timespec ts;
		ts64=(int64_t)ub_rt_gettime64()+(tout_ms*UB_MSEC_NS);
		UB_NSEC2TS(ts64, ts);
		if(CB_SEM_TIMEDWAIT((CB_SEM_T*)sigp, &ts)!=0){
			if(errno==ETIMEDOUT){return 1;}
			goto erexit;
		}
	}
	return 0;
erexit:
	UB_LOG(UBL_ERROR, "%s:%s\n", fname, strerror(errno));
	return -1;
}

UB_SD_GETMEM_DEF(UC_NOTICE_SIGM, sizeof(CB_SEM_T*), UC_NOTICE_SIGNUM);

UC_NOTICE_SIG_T *uc_notice_sig_reallocate(bool thread, void *d, int n)
{
	if(n==0){
		UB_SD_RELMEM(UC_NOTICE_SIGM, d);
		return NULL;
	}
	d=UB_SD_REGETMEM(UC_NOTICE_SIGM, d, (int)sizeof(CB_SEM_T)*n);
	if(ub_assert_fatal(d, __func__, "realloc")){return NULL;}
	return d;
}

int uc_notice_sig_trywait(bool thread, UC_NOTICE_SIG_T *sigp)
{
	return CB_SEM_TRYWAIT((CB_SEM_T*)sigp);
}
