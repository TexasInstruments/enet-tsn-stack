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
#include "combase_private.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <tsn_unibase/ub_getmem.h>
#include "cb_ipcshmem.h"

static ub_esarray_cstd_t *shmem_table=NULL;
typedef struct shared_mem_table{
	char name[32];
	void *mem;
}shared_mem_table_t;

/*
 * static memory allocation case('UB_SD_STATIC' is defined)
 * the memory is statically allocated by the following definitions.
 * the numbers defined here should be overridden by your needs.
 * when 'UB_SD_STATIC' is not defined, the number doesn't have meaning.
 */
#define CB_NOIPCSHMEM_DMEM cb_noipcshmem_data
#ifndef CB_NOIPCSHMEM_DFSIZE
#define CB_NOIPCSHMEM_DFSIZE 256
#endif
#ifndef CB_NOIPCSHMEM_DFNUM
#define  CB_NOIPCSHMEM_DFNUM 64
#endif

UB_SD_GETMEM_DEF(CB_NOIPCSHMEM_DMEM, CB_NOIPCSHMEM_DFSIZE,
		 CB_NOIPCSHMEM_DFNUM);

static shared_mem_table_t *find_shared_mem(const char *shmname)
{
	int i;
	shared_mem_table_t *memt;
	if(!shmname){return NULL;}
	if(!shmem_table){return NULL;}
	for(i=0;i<ub_esarray_ele_nums(shmem_table);i++){
		memt=(shared_mem_table_t *)ub_esarray_get_ele(shmem_table, i);
		if(memt && !strcmp(shmname, memt->name)){return memt;}
	}
	return NULL;
}

void *cb_get_shared_mem(int *memfd, const char *shmname, size_t size, int flag)
{
	shared_mem_table_t *memt;
	*memfd=0;

	if(!shmname) {
		UB_LOG(UBL_ERROR,"%s:shmname is NULL\n", __func__);
		return NULL;
	}

	if(!shmem_table){
		shmem_table=ub_esarray_init(8, sizeof(shared_mem_table_t), 32);
	}
	memt=find_shared_mem(shmname);
	if(flag & O_CREAT){
		if(memt){
			UB_LOG(UBL_ERROR,"%s:shared memory, name=%s is already opened\n", __func__,
				   shmname);
			return NULL;
		}
		memt=(shared_mem_table_t *)ub_esarray_get_newele(shmem_table);
		if(memt) {
			strncpy(memt->name, shmname, 31);
			memt->mem=UB_SD_GETMEM(CB_NOIPCSHMEM_DMEM, size);
		}
		if(!memt || !memt->mem) {
			UB_LOG(UBL_ERROR,"%s:malloc error for size=%zu, %s\n",__func__,
				   size, strerror(errno));
			cb_close_shared_mem(NULL, 0, shmname, 0, true);
			return NULL;
		}
	}else{
		if(!memt){
			UB_LOG(UBL_ERROR,"%s:shared memory, name=%s doesn't exist\n", __func__,
				   shmname);
			return NULL;
		}
	}
	*memfd=(int)((uintptr_t)memt & 0x7fffffff);
	return memt->mem;
}

int cb_close_shared_mem(void *mem, int *memfd, const char *shmname, size_t size, bool unlink)
{
	shared_mem_table_t *memt;

	if(!shmname) {
		UB_LOG(UBL_ERROR,"%s:shmname is NULL\n", __func__);
		return -1;
	}

	memt=find_shared_mem(shmname);
	if(!memt){
		UB_LOG(UBL_ERROR,"%s:shared memory, name=%s doesn't exist\n",__func__, shmname);
		return -1;
	}
	if(mem && unlink){
		if(mem!=memt->mem){
			UB_LOG(UBL_ERROR,"%s:pointer doesn't match\n",__func__);
			return -1;
		}
		UB_SD_RELMEM(CB_NOIPCSHMEM_DMEM, mem);
	}
	if(unlink){
		ub_esarray_del_pointer(shmem_table, (ub_esarray_element_t *)memt);
		if(!ub_esarray_ele_nums(shmem_table)){
			ub_esarray_close(shmem_table);
			shmem_table=NULL;
		}
	}
	if(memfd){
		*memfd=0;
	}
	return 0;
}
