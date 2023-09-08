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
#include "gptpclock_virtual.h"
#include "gptpclock.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpconf/xl4-extmod-xl4gptp.h"

#define PTPVDEV_MAX_NAME 16
#define MAX_VPTPD (GPTP_VIRTUAL_PTPDEV_FDMAX-GPTP_VIRTUAL_PTPDEV_FDBASE+1)

typedef struct ptpfd_virtual {
	char ptpdev[PTPVDEV_MAX_NAME];
	PTPFD_TYPE fd;
	int freq_adj;
	uint64_t lastts;
	uint64_t lastpts;
	bool rdwr_mode;
	int users;
	int vclkrate;
	uint8_t gptpInstanceIndex;
} ptpfd_virtual_t;

// all instances share the same set of ptpfd_virtual_t
// the data is per ptpdev(fd)
static ub_esarray_cstd_t *esvptpd = NULL;

static ptpfd_virtual_t *find_ptpfd_virtual(PTPFD_TYPE ptpfd)
{
	int i;
	ptpfd_virtual_t *pv;
	int en;
	if (!esvptpd) {
		UB_LOG(UBL_ERROR,"%s:esvptpd is NULL\n", __func__);
		return NULL;
	}
	en=ub_esarray_ele_nums(esvptpd);
	for(i=0;i<en;i++){
		pv=(ptpfd_virtual_t *)ub_esarray_get_ele(esvptpd, i);
		if(ptpfd==pv->fd){return pv;}
	}
	UB_LOG(UBL_ERROR,"%s:ptpfd="PRiFD" is not opened\n", __func__, ptpfd);
	return NULL;
}

PTPFD_TYPE gptp_vclock_alloc_fd(uint8_t gptpInstanceIndex, char *ptpdev)
{
	int i,en;
	ptpfd_virtual_t *pv, *rpv;
	PTPFD_TYPE fdmax=(PTPFD_TYPE)(GPTP_VIRTUAL_PTPDEV_FDBASE-1);
	if(!esvptpd){
		esvptpd=ub_esarray_init(4, sizeof(ptpfd_virtual_t), MAX_VPTPD);
	}
	en=ub_esarray_ele_nums(esvptpd);
	for(i=0;i<en;i++){
		pv=(ptpfd_virtual_t *)ub_esarray_get_ele(esvptpd, i);
		if(!strcmp(pv->ptpdev, ptpdev)) {
			(pv->users)++;
			return pv->fd;
		}
		fdmax=UB_MAX(pv->fd, fdmax);
	}
	rpv=(ptpfd_virtual_t *)ub_esarray_get_newele(esvptpd);
	(void)memset(rpv, 0, sizeof(ptpfd_virtual_t));
	(void)strncpy(rpv->ptpdev, ptpdev, PTPVDEV_MAX_NAME-1);
	rpv->fd=fdmax+1;
	rpv->users=1;
	rpv->gptpInstanceIndex=gptpInstanceIndex;
	if(ptpdev[strlen(CB_VIRTUAL_PTPDEV_PREFIX)]=='w'){rpv->rdwr_mode=true;}
	if(rpv->fd>(PTPFD_TYPE)GPTP_VIRTUAL_PTPDEV_FDMAX){
		ub_esarray_del_pointer(esvptpd, (ub_esarray_element_t *)rpv);
		return PTPFD_INVALID;
	}
	rpv->vclkrate=gptpgcfg_get_intitem(
		rpv->gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_PTPVFD_CLOCK_RATE,
		YDBI_CONFIG);
	return rpv->fd;
}

int gptp_vclock_free_fd(PTPFD_TYPE ptpfd)
{
	ptpfd_virtual_t *pv=find_ptpfd_virtual(ptpfd);
	if(!pv){
		UB_LOG(UBL_ERROR,"%s:ptpfd="PRiFD" is not opened\n", __func__, ptpfd);
		return -1;
	}
	if(--(pv->users)==0){
		ub_esarray_del_pointer(esvptpd, (ub_esarray_element_t *)pv);
		if(ub_esarray_ele_nums(esvptpd)==0){
			ub_esarray_close(esvptpd);
			esvptpd=NULL;
		}
	}
	return 0;
}

int gptp_vclock_settime(PTPFD_TYPE ptpfd, uint64_t ts64)
{
	ptpfd_virtual_t *pv=find_ptpfd_virtual(ptpfd);
	if(!pv){return -1;}
	if(!pv->rdwr_mode){return -1;}
	pv->lastpts=ts64;
	pv->lastts=ub_rt_gettime64();
	return 0;
}

uint64_t gptp_vclock_gettime(PTPFD_TYPE ptpfd)
{
	uint64_t ts64;
	int64_t dts64, dpts64;
	ptpfd_virtual_t *pv=find_ptpfd_virtual(ptpfd);
	if(!pv){return 0;}
	ts64=ub_rt_gettime64();
	if(!pv->lastts){
		pv->lastpts=ts64;
		pv->lastts=ts64;
		return ts64;
	}
	dts64=ts64-pv->lastts;
	if(pv->rdwr_mode){
		dpts64=dts64*(pv->freq_adj+pv->vclkrate)/UB_SEC_NS;
	}else{
		dpts64=dts64*pv->vclkrate/UB_SEC_NS;
	}
	pv->lastts=ts64;
	dts64=(int64_t)pv->lastpts+dts64+dpts64;
	pv->lastpts=(uint64_t)dts64;
	return (uint64_t)dts64;
}

int gptp_vclock_adjtime(PTPFD_TYPE ptpfd, int adjppb)
{
	ptpfd_virtual_t *pv=find_ptpfd_virtual(ptpfd);
	if(!pv){return -1;}
	if(!pv->rdwr_mode){return -1;}
	pv->freq_adj=adjppb;
	return 0;
}

int gptp_vclock_setoffset(PTPFD_TYPE ptpfd, int64_t offset64)
{
	ptpfd_virtual_t *pv=find_ptpfd_virtual(ptpfd);
	if(!pv){return -1;}
	if(!pv->rdwr_mode){return -1;}
	pv->lastpts=(int64_t)gptp_vclock_gettime(ptpfd)+offset64;
	pv->lastts=ub_rt_gettime64();
	return 0;
}
