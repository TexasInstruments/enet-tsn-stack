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
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "combase_private.h"

void *cb_get_shared_mem(int *memfd, const char *shmname, size_t size, int flag)
{
	void *mem;
	int prot;
	*memfd=shm_open(shmname, flag, ((mode_t)6<<6)|((mode_t)6<<3)|(mode_t)6);
	if(*memfd<0) {
		UB_LOG(UBL_ERROR,"%s:can't %s shared memory, name=%s, error=%d:%s\n", __func__,
		       (flag & O_CREAT)?"create":"open", shmname, errno, strerror(errno));
		goto erexit;
	}
	UB_LOG(UBL_DEBUG,"%s:%s shared memory, name=%s, size=%zd\n",__func__,
	       (flag & O_CREAT)?"create":"open", shmname, size);

	if(((flag && O_WRONLY)||(flag && O_RDWR)) && ftruncate(*memfd, size)){
		UB_LOG(UBL_ERROR,"%s:ftruncate,%s\n", __func__, strerror(errno));
		goto erexit;
	}

	if(flag && O_RDWR){
		prot=PROT_READ|PROT_WRITE;
	}else if(flag && O_WRONLY){
		prot=PROT_WRITE;
	}else {
		prot=PROT_READ;
	}

	mem=mmap(NULL, size, prot, MAP_SHARED, *memfd, 0);
	if(mem==MAP_FAILED){
		UB_LOG(UBL_ERROR,"%s:mmap, %s\n", __func__, strerror(errno));
		close(*memfd);
		shm_unlink(shmname);
		goto erexit;
	}
	return mem;
erexit:
	*memfd=0;
	return NULL;
}

int cb_close_shared_mem(void *mem, int *memfd, const char *shmname, size_t size, bool unlink)
{
	if(mem && size){
		if(munmap(mem, size)!=0){
			UB_LOG(UBL_ERROR,"%s:munmap, %s\n", __func__,strerror(errno));
			return -1;
		}
	}
	if(memfd && *memfd){
		close(*memfd);
		*memfd=0;
	}
	if(shmname && *shmname && (unlink)){
		if(shm_unlink(shmname)!=0){
			UB_LOG(UBL_ERROR,"%s:shm_unlink, shmname=%s, %s\n",
			       __func__, shmname, strerror(errno));
			return -1;
		}
	}
	return 0;
}
