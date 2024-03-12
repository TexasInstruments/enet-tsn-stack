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
 * unibase.c
 * unibase initialization
 *
 */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "unibase_private.h"

/*
 * this exceptionally uses a static global variable.
 * In the initialized process,
 * all the functions are universally available through this static variable.
 * In normal usage, to be safe, 'ubcd.cbset' is initialized only once
 * and never changed in the entire life of the process.
 * The other part of 'ubcd' can be re-configured, in threadding env. gmutex is locked.
 */
unibase_cstd_t ubcd = {false,};

int unibase_init(unibase_init_para_t *ub_init_para)
{
	if(!ub_init_para) {
		(void)printf("need 'ub_init_para'\n");
		return -1;
	}
	if(ubcd.locked!=0){
		ubcd.locked++;
		return 0;
	}
	ubcd.locked++;
	memcpy(&ubcd.cbset, &ub_init_para->cbset, sizeof(unibase_cb_set_t));
	if(ubcd.cbset.get_static_mutex && ubcd.cbset.static_mutex_close &&
	   ubcd.cbset.mutex_lock && ubcd.cbset.mutex_unlock) {
		ubcd.threadding=true;
		ubcd.ub_sd_mutex=ubcd.cbset.get_static_mutex();
		if(!ubcd.ub_sd_mutex){goto erexit;}
		ubcd.gmutex=ubcd.cbset.get_static_mutex();
		if(!ubcd.gmutex){goto erexit;}
		ubcd.func_mutex=ubcd.cbset.get_static_mutex();
		if(!ubcd.func_mutex){goto erexit;}
		ubcd.esarray_mutex=ubcd.cbset.get_static_mutex();
		if(!ubcd.esarray_mutex){goto erexit;}
	}
	ub_log_init(ub_init_para->ub_log_initstr);
	if(ubcd.cbset.console_out!=NULL){
		ubcd.cbset.console_out(true, "unibase-"TSNPKGVERSION"\n");
	}
	return 0;
erexit:
	unibase_close();
	return -1;
}

void unibase_close(void)
{
	if(!ubcd.locked){return;}
	ubcd.locked--;
	if(ubcd.locked>0){return;}
	if(ubcd.threadding){
		if(ubcd.gmutex){ubcd.cbset.static_mutex_close(ubcd.gmutex);}
		if(ubcd.ub_sd_mutex){ubcd.cbset.static_mutex_close(ubcd.ub_sd_mutex);}
		if(ubcd.func_mutex){ubcd.cbset.static_mutex_close(ubcd.func_mutex);}
		if(ubcd.esarray_mutex){ubcd.cbset.static_mutex_close(ubcd.esarray_mutex);}
	}
	(void)memset(&ubcd, 0, sizeof(ubcd));
}

void ub_fatal(const char *mes1, const char *mes2)
{
	const char *ames1=mes1;
	const char *ames2=mes2;
	if(!ames1){ames1="";}
	if(!ames2){ames2="";}
	UB_CONSOLE_DEBUGMEM_PRINT("fatal error:%s - %s\n", ames1, ames2);
	ubcd.fatalerror=true;
	if(ubcd.cbset.fatal!=NULL){ubcd.cbset.fatal();}
	return;
}

bool ub_fatalerror(void)
{
	return ubcd.fatalerror;
}

uint64_t ub_rt_gettime64(void)
{
	return ubcd.cbset.gettime64(UB_CLOCK_REALTIME);
}

uint64_t ub_mt_gettime64(void)
{
	return ubcd.cbset.gettime64(UB_CLOCK_MONOTONIC);
}

uint64_t ub_gptp_gettime64(void)
{
	return ubcd.cbset.gettime64(UB_CLOCK_GPTP);
}

int ub_func_private_mutex_lock(void)
{
	if(ubcd.threadding){return ubcd.cbset.mutex_lock(ubcd.func_mutex);}
	return -1;
}

int ub_func_private_mutex_unlock(void)
{
	if(ubcd.threadding){return ubcd.cbset.mutex_unlock(ubcd.func_mutex);}
	return -1;
}
