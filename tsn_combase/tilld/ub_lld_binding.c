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
 /*
 * ub_lld_binding.c
 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tsn_unibase/unibase.h"
#include "tsn_unibase/unibase_binding.h"
#include "combase.h"
#include "fs_stdio.h"

typedef struct memory_out_data {
	char *buffer;
	int size;
	bool own_buffer;
	uint32_t wp;
	CB_THREAD_MUTEX_T mutex;
} memory_out_data_t;
static memory_out_data_t memoutd;
static const char *endmark=UBB_MEMOUT_ENDMARK;
static const int endmlen=sizeof(UBB_MEMOUT_ENDMARK)-(size_t)1;
int ubb_memory_out_init(char *mem, int size)
{
	(void)ubb_memory_out_close();
	memoutd.size=size;
	if(!size) {
		memoutd.size=-1;
		return 0;
	}
	if(mem!=NULL){
		memoutd.buffer=mem;
		memoutd.own_buffer=false;
	}else{
		memoutd.buffer=(char*)malloc(size);
		if(!memoutd.buffer){abort();}
		memoutd.own_buffer=true;
	}
	(void)memset(memoutd.buffer, 0, memoutd.size);
	return CB_THREAD_MUTEX_INIT(&memoutd.mutex, NULL);
}

int ubb_memory_out_close(void)
{
	if(memoutd.size<=0){return 0;}
	if(!memoutd.buffer){return -1;}
	CB_THREAD_MUTEX_DESTROY(&memoutd.mutex);
	if(memoutd.own_buffer){free(memoutd.buffer);}
	(void)memset(&memoutd, 0, sizeof(memory_out_data_t));
	return 0;
}

char *ubb_memory_out_buffer(void)
{
	return memoutd.buffer;
}

static int memory_out_find_line(int *point, int *size)
{
	int lwp;
	int i;
	int c=0;
	int p1;
	// skip the bottom null characters
	for(lwp=*point;lwp>=0;lwp--) {
		if(memoutd.buffer[lwp]!=0){break;}
	}
	if(lwp<0){return -1;}
	p1=-1;
	for(i=lwp;i>=0;i--){
		if(memoutd.buffer[i]!='\n'){continue;}
		if(c==1){
			*point=i+1;
			*size=p1-i;
			return 0;
		}
		c++;
		p1=i;
	}
	if(p1>=0){
		*point=0;
		*size=p1+1;
		return 0;
	}
	return -1;
}

int ubb_memory_out_lastline(char **str, int *size)
{
	int lwp;
	int res=-1;
	*str=NULL;
	*size=0;
	if(memoutd.size<=0){return -1;}
	CB_THREAD_MUTEX_LOCK(&memoutd.mutex);
	if(memcmp(&memoutd.buffer[memoutd.wp], endmark, endmlen)!=0) {
		goto erexit;
	}
	lwp=memoutd.wp;
	if(lwp==0){lwp=memoutd.size-1;}
	if(!memory_out_find_line(&lwp, size)){
		*str=&memoutd.buffer[lwp];
		res=0;
	}else{
		if(memoutd.wp==(uint32_t)0) {
			goto erexit;
		}
		lwp=memoutd.size-1;
		if(!memory_out_find_line(&lwp, size)){
			*str=&memoutd.buffer[lwp];
			res=0;
		}
	}
erexit:
	CB_THREAD_MUTEX_UNLOCK(&memoutd.mutex);
	return res;
}

int ubb_memory_out_alldata(char **rstr, int *size)
{
	int lwp;
	int i;
	int res=-1;
	int size1=0;
	char *str1=NULL;

	if(memoutd.size<=0){return res;}
	*rstr=(char*)malloc(memoutd.size);
	if(ub_assert_fatal(*rstr, __func__, "malloc error")){return -1;}
	(void)memset(*rstr, 0, memoutd.size);

	CB_THREAD_MUTEX_LOCK(&memoutd.mutex);
	if(memcmp(&memoutd.buffer[memoutd.wp], endmark, endmlen)!=0) {
		free(*rstr);
		*rstr=NULL;
		*size=0;
		goto erexit;
	}
	lwp=memoutd.wp;
	str1=&memoutd.buffer[lwp+endmlen+1];
	for(i=lwp+endmlen+1;i<memoutd.size;i++){
		if(memoutd.buffer[i]==0){
			break;
		}
	}
	size1=i-(lwp+endmlen+1);
	if(size1!=0){memcpy(*rstr, str1, size1);}
	if(lwp!=0){memcpy(&(*rstr)[size1], memoutd.buffer, lwp);}
	*size=size1+lwp;
	res=0;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&memoutd.mutex);
	return res;
}

static int ubb_memory_out(bool flush, const char *str)
{
	uint32_t v;
	if(memoutd.size<0){return 0;}
	if(!memoutd.size){
		if(ubb_memory_out_init(NULL, UBB_DEFAULT_DEBUG_LOG_MEMORY)!=0){return -1;}
	}
	CB_THREAD_MUTEX_LOCK(&memoutd.mutex);
	v=strlen(str);
	if((memoutd.wp+v) >= ((uint32_t)memoutd.size-((uint32_t)endmlen+(uint32_t)1))){
		// if it is at the bottom, fill 0 in the remaining buffer
		(void)memset(&memoutd.buffer[memoutd.wp], 0, memoutd.size-(int)memoutd.wp);
		memoutd.wp=0;
	}
	(void)strcpy(&memoutd.buffer[memoutd.wp], str);
	(void)strcpy(&memoutd.buffer[memoutd.wp+v], endmark);
	memoutd.wp+=v;
	CB_THREAD_MUTEX_UNLOCK(&memoutd.mutex);
	return v;
}

static int ubb_stdout(bool flush, const char *str)
{
	int res;
	res=fwrite(str, 1, strlen(str), stdout);
	if(flush){(void)fflush(stdout);}
	return res;
}

#ifndef UBB_STATIC_MUTEX_MAX
#define UBB_STATIC_MUTEX_MAX 4
#endif

static CB_THREAD_MUTEX_T global_static_mutex[UBB_STATIC_MUTEX_MAX];
static bool global_static_flag[UBB_STATIC_MUTEX_MAX];
static void *ubb_get_static_mutex(void)
{
	int i;
	CB_THREAD_MUTEX_T *mt=NULL;

	for(i=0;i<UBB_STATIC_MUTEX_MAX;i++){
		if(!global_static_flag[i]){
			mt=&global_static_mutex[i];
			break;
		}
	}
	if(!mt){return NULL;}
	if(CB_THREAD_MUTEX_INIT(mt, NULL)!=0){return NULL;}
	global_static_flag[i]=true;
	return mt;
}

/**
 * @brief close the static mutex which is got by 'ubb_get_static_mutex'
 */
static int ubb_static_mutex_close(void *mt)
{
	int i;

	if(!mt){return -1;}
	for(i=0;i<UBB_STATIC_MUTEX_MAX;i++){
		if(global_static_flag[i] && mt==&global_static_mutex[i]){
			CB_THREAD_MUTEX_DESTROY((CB_THREAD_MUTEX_T *)mt);
			global_static_flag[i]=false;
			return 0;
		}
	}
	return -1;
}

static int ubb_mutex_lock(void *mt)
{
	return CB_THREAD_MUTEX_LOCK((CB_THREAD_MUTEX_T *)mt);
}

static int ubb_mutex_unlock(void *mt)
{
	return CB_THREAD_MUTEX_UNLOCK((CB_THREAD_MUTEX_T *)mt);
}

static get64ts_t gptp_gettime64=NULL;
static uint64_t ubb_gettime64(ub_clocktype_t ctype)
{
	struct timespec ts={0,0};
	switch(ctype)
	{
	case UB_CLOCK_DEFAULT:
		break;
	case UB_CLOCK_REALTIME:
	case UB_CLOCK_MONOTONIC:
		return cb_lld_gettime64(ctype);
	case UB_CLOCK_GPTP:
		if(!gptp_gettime64){return 0;}
		return gptp_gettime64();
	default:
		break;
	}
	return ts.tv_sec*(uint64_t)UB_SEC_NS+ts.tv_nsec;
}

static void* ubb_fioopen(const char *name, const char *mode)
{
	return (void*)UB_FOPEN(name, mode);
}

static int ubb_fioclose(void *fio)
{
	return UB_FCLOSE(fio);
}

static int ubb_fioread(void *fio, void *ptr, int size)
{
	return UB_FREAD(fio, ptr, size);
}

static int ubb_fiowrite(void *fio, const void *ptr, int size)
{
	return UB_FWRITE(fio, ptr, size);
}

static int ubb_fioseek(void *fio, int offset)
{
	return UB_FSEEK(fio, offset);
}

void ubb_default_initpara(unibase_init_para_t *init_para)
{
	static const char *default_log_initstr=UBB_DEFAULT_LOG_INITSTR;
	ub_fiocb_set_t fioset;
	(void)memset(init_para, 0, sizeof(unibase_init_para_t));
	init_para->cbset.console_out=ubb_stdout;
	init_para->cbset.debug_out=ubb_memory_out;
	init_para->cbset.get_static_mutex=ubb_get_static_mutex;
	init_para->cbset.static_mutex_close=ubb_static_mutex_close;
	init_para->cbset.mutex_lock=ubb_mutex_lock;
	init_para->cbset.mutex_unlock=ubb_mutex_unlock;
	init_para->cbset.gettime64=ubb_gettime64;
	init_para->ub_log_initstr=default_log_initstr;
	fioset.open=ubb_fioopen;
	fioset.close=ubb_fioclose;
	fioset.read=ubb_fioread;
	fioset.write=ubb_fiowrite;
	fioset.seek=ubb_fioseek;
	(void)ub_fioinit(&fioset);
}

void ubb_set_gptp_gettime64(get64ts_t func)
{
	gptp_gettime64=func;
}

void ubb_unibase_easyinit(void)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	(void)unibase_init(&init_para);
	(void)ubb_memory_out_init(NULL, 0);// no memory is allocated for the logging
}

int ubb_memory_file_out(const char *fname)
{
	void *outf;
	int lwp;
	int i;
	int res=-1;
	int size1=0;
	char *str1=NULL;

	if(!fname || !fname[0]){return -1;}
	outf=ubb_fioopen(fname, "w");
	if(outf==NULL){return -1;}
	CB_THREAD_MUTEX_LOCK(&memoutd.mutex);
	if(memcmp(&memoutd.buffer[memoutd.wp], endmark, endmlen)!=0){goto erexit;}
	lwp=memoutd.wp;
	str1=&memoutd.buffer[lwp+endmlen+1];
	for(i=lwp+endmlen+1;i<memoutd.size;i++){
		if(memoutd.buffer[i]==0){
			break;
		}
	}
	size1=i-(lwp+endmlen+1);
	i=0;
	if(size1!=0){i=ubb_fiowrite(outf, str1, size1);}
	if(lwp!=0){i+=ubb_fiowrite(outf, memoutd.buffer, lwp);}
	if(i==(size1+lwp)){res=0;}
erexit:
	CB_THREAD_MUTEX_UNLOCK(&memoutd.mutex);
	(void)ubb_fioclose(outf);
	return res;
}
