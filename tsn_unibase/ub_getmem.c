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
#include "unibase_private.h"

/*
 |----------------------- FRAGSIZE*FRAGNUM bytes ------------------------|
 |-frag0-|-frag1-|-frag2-|-frag3-|-frag4-|-frag5-|-frag6-|-frag7-|-frag8-|

 | busy=0| busy=2| busy=0| busy=0| busy=1| busy=0| busy=0| busy=0| busy=0|

 the all fragments are statically allocated.
 the number in 'busy' means the following fragments for the number are used.
 the above case, 'frag1,frag2' and 'frag4' are used.

 'ub_static_getmem' look for consecutive not-busy fragments for 'size'.
 'ub_static_regetmem' for smaller size, it changes the number of 'busy'
 'ub_static_regetmem' for bigger size, it look for a new consecutive not-busy fragments for 'size'.
                     When it changes the location the content is copied by 'memmove'.

 the following parameters must be  defined at the build time.

 #define FRAGSIZE 64
 #define FRAGNUM 8
 static uint8_t mem[FRAGSIZE*FRAGNUM];
 static uint16_t busysizes[FRAGNUM];

 For a simple object instance allocation,
   set FRAGSIZE as the instance size.
   set FRAGNUM as the maximum number of instances.

 */

void ub_static_relmem(void *p, void *mem, uint16_t *busysizes,
		      int fragnum, uint16_t fragsize, const char *mname);

void *ub_static_getmem(size_t size, void *mem, uint16_t* busysizes,
		      int fragnum, uint16_t fragsize, const char *mname, bool nolock)
{
	int i;
	int pi=-1;
	int np=0;
	int max_np=0;
	void *res=NULL;
	if(size==0u){return NULL;}
	if(!nolock){
		if(ubcd.cbset.mutex_lock && ubcd.ub_sd_mutex){
			ubcd.cbset.mutex_lock(ubcd.ub_sd_mutex);
		}
	}
	for(i=0;i<fragnum;){
		if(!busysizes[i]) {
			if(pi==-1) {pi=i;}
			np++;
			if(np>max_np){max_np=np;}
			i++;
		}else{
			i+=(int)busysizes[i];
			np=0;
			pi=-1;
		}
		if((np*(int)fragsize)>=(int)size){
			busysizes[pi]=i-pi;
			res=&((uint8_t*)mem)[pi*(int)fragsize];
			break;
		}
	}
	if(!nolock){
		if(ubcd.cbset.mutex_unlock && ubcd.ub_sd_mutex){
			ubcd.cbset.mutex_unlock(ubcd.ub_sd_mutex);
		}
	}
	if(res==NULL){
		UB_LOG(UBL_ERROR, "%s:%s memory shortage: %d requested, %d max available\n",
			   __func__, mname, (int)size, max_np*(int)fragsize);
	}
	return res;
}

void *ub_static_regetmem(void *p, size_t nsize, void *mem, uint16_t *busysizes,
			int fragnum, uint16_t fragsize, const char *mname)
{
	uint16_t cbusy; // current busy size
	uint16_t nbusy; // new busy size
	int cpi; // current position index
	void *np=NULL; // new pointer
	int dp;
	const char *ermes="";
	if(p==NULL) {
		return ub_static_getmem(nsize, mem, busysizes, fragnum, fragsize, mname, false);
	}
	if(nsize==0u){
		ub_static_relmem(p, mem, busysizes, fragnum, fragsize, mname);
		return NULL;
	}
	// the next operation is checked to be in the same array, no violation of Misra-C Rule 18.2
	if(ubcd.cbset.mutex_lock && ubcd.ub_sd_mutex){
		ubcd.cbset.mutex_lock(ubcd.ub_sd_mutex);
	}
	dp=(uint8_t*)p-(uint8_t*)mem;
	cpi=dp/(int)fragsize;
	if((dp<0) || (cpi>=fragnum) || (cpi*(int)fragsize)!=dp) {
		ermes="invalid pointer";
		goto erexit;
	}
	nbusy=((int)nsize+(int)fragsize-1)/(int)fragsize;
	if(nbusy<=busysizes[cpi]){
		busysizes[cpi]=nbusy;
		np=p;
		goto erexit;
	}
	cbusy=busysizes[cpi];
	busysizes[cpi]=0u;
	np=ub_static_getmem(nsize, mem, busysizes, fragnum,
			    fragsize, mname, true);
	if(np==NULL) {
		// failed case, keep it busy with the original size
		// this is the same behavior as the 'realloc'
		busysizes[cpi]=cbusy;
		ermes="can't expand, pointer";
		goto erexit;
	}
	if(np!=p){
		memmove(np, p, cbusy*fragsize);
	}
erexit:
	if(ubcd.cbset.mutex_unlock && ubcd.ub_sd_mutex){
		ubcd.cbset.mutex_unlock(ubcd.ub_sd_mutex);
	}
	if(np==NULL){
		UB_LOG(UBL_ERROR, "%s:%s\n", __func__, ermes);
	}
	return np;
}

void ub_static_relmem(void *p, void *mem, uint16_t *busysizes,
			int fragnum, uint16_t fragsize, const char *mname)
{
	int dp;
	int cpi;
	if(p==NULL) {return;}
	// the next operation is checked to be in the same array, no violation of Misra-C Rule 18.2
	dp=(uint8_t*)p-(uint8_t*)mem;
	cpi=dp/(int)fragsize;
	if((dp<0) || (cpi>=fragnum) || ((int)cpi*(int)fragsize)!=dp) {
		UB_LOG(UBL_ERROR, "%s:%s:invalid pointer=%p\n", __func__, mname, p);
		return;
	}
	if(busysizes[cpi]==0u){
		UB_LOG(UBL_ERROR, "%s:%s: double relmem, pointer=%p\n", __func__, mname, p);
		return;
	}
	busysizes[cpi]=0u;
	return;
}

int ub_static_print_usage(uint16_t* busysizes, int fragnum,
		uint16_t fragsize, const char *mname, bool nolock, ub_dbgmsg_level_t level)
{
	int i;
	int fragused=0;

	if(!nolock){
		if(ubcd.cbset.mutex_lock && ubcd.ub_sd_mutex){
			ubcd.cbset.mutex_lock(ubcd.ub_sd_mutex);
		}
	}
	for(i=0;i<fragnum;i++){
		if(busysizes[i]!=0u){fragused+=busysizes[i];}
	}
	if(!nolock){
		if(ubcd.cbset.mutex_unlock && ubcd.ub_sd_mutex){
			ubcd.cbset.mutex_unlock(ubcd.ub_sd_mutex);
		}
	}
	if(ub_clog_on(UB_LOGCAT, level)){
		UB_LOG(UBL_INFO, "%s: fragsize=%d fragused/fragnum=%d/%d (%d%%)\n",
			   mname, (int)fragsize, fragused, fragnum, (fragused*100)/fragnum);
	}
	return (fragnum-fragused);
}
