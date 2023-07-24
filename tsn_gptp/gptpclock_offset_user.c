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
 * For the old kernel which doesn't supports 'ptpdev_clock_setoffset',
 * but 'ptpdev_clock_gettime/settime'.
 * To enable this code, use the next CFLAG
 * '-DGPTPCLOCK_OFFSET_USER=\"gptpclock_offset_user.c\"'
 *
 * This mode is deprecated.
 */

static int gptpclock_setts_od(uint8_t gptpInstanceIndex, gptpclock_data_t *gcd,
			      int64_t ts64, oneclock_data_t *od)
{
	GPTP_CLOCK_GETTIME(od->ptpfd, od->last_setts64);

	if(!od->clockIndex || (od->mode==PTPCLOCK_SLAVE_SUB)){
		gptpclock_mutex_trylock(&gcd->shm->head.mcmutex);
	}

	od->offset64=ts64-od->last_setts64;
	if(od->mode==PTPCLOCK_SLAVE_MAIN){
		od->offset64=0;
		GPTP_CLOCK_SETTIME(od->ptpfd, ts64);
	}else{
		gptpclock_setoffset_od(gcd, od);
	}

	if(!od->clockIndex || (od->mode==PTPCLOCK_SLAVE_SUB)){
		CB_THREAD_MUTEX_UNLOCK(&gcd->shm->head.mcmutex);
	}

	return 0;
}

/* returns latency time in this function, if it is too long this setting is not accurate */
static int64_t time_setoffset64(uint8_t gptpInstanceIndex, gptpclock_data_t *gcd,
				int64_t offset64, int clockIndex, uint8_t domainNumber)
{
	int64_t ats64=-1;
	int64_t mt1,mt2;
	oneclock_data_t *od;
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainNumber);
	mt1=ub_mt_gettime64();
	(void)gptpclock_getts_od(&ats64, od);
	ats64 += offset64;
	(void)gptpclock_setts_od(gptpInstanceIndex, gcd, ats64, od);
	mt2=ub_mt_gettime64();
	return mt2-mt1;
}

static int avarage_time_setoffset(gptpclock_data_t *gcd, int clockIndex,
				  uint8_t domainNumber)
{
	int64_t v;
	int64_t vmax=0;
	int av=0, avc;
	int count=0;
	int i;
	for(i=0;i<10;i++){
		v = time_setoffset64(gptpInstanceIndex, gcd, 0, clockIndex, domainNumber);
		if(v > gptpgcfg_get_intitem(
			   gptpInstanceIndex,
			   XL4_EXTMOD_XL4GPTP_MAX_CONSEC_TS_DIFF,
			   YDBI_CONFIG)){continue;}
		if(llabs(vmax)<llabs(v)){vmax=v;}
		av += v;
		count ++;
	}
	// remove max value
	av-=vmax;
	count--;
	if(count<=0){
		UB_LOG(UBL_ERROR, "%s:clockIndex=%d, domainNumber=%d,"
		       "can't calculate setoffset time\n",
		       __func__, clockIndex, domainNumber);
		return 0;
	}
	av = av/count;
	// it was measured in a short loop, and likely shorter value than real use case value.
	avc = av*gptpgcfg_get_intitem(
		gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_TS2DIFF_CACHE_FACTOR,
		YDBI_CONFIG)/100;
	UB_LOG(UBL_DEBUG, "%s:clockIndex=%d, domainNumber=%d,"
	       "calculate setoffset time av=%d, avc=%d, vmax=%"PRIi64"\n",
	       __func__, clockIndex, domainNumber, av, avc, vmax);
	return avc;
}
