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
#include <signal.h>
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/combase.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/uc_notice.h>
#include "ucpra1.h"
#include "xl4-extmod-ucpra1.h"

#define MAX_INTERVAL (3*UB_SEC_NS)

static bool stoprun;
static void signal_handler(int sig)
{
	stoprun=true;
}

static int update_loop(uc_dbald *dbald, xl4_data_data_t *xdd,
		       uc_notice_data_t *ucntd)
{
	uint64_t intvts;
	int32_t newval;
	struct timeval tv;
	int res=0;
	uint8_t aps[4]={XL4_EXTMOD_RW, 0, XL4_EXTMOD_UCPRA1_VARIABLEX, 255};

	aps[1]=xl4_data_get_modid(xdd, "ucpra1");
	if(aps[1]==0xff){
		UB_LOG(UBL_ERROR, "%s:ucpra1 is not in DB\n", __func__);
		return -1;
	}
	if(res) return -1;
	srand(ub_rt_gettime64()&0xffffffff);
	while(!stoprun){
		newval=rand();
		UB_CONSOLE_PRINT("new value=%d\n", newval);
		res=uc_dbal_create(dbald, aps, 4, &newval, 4);
		if(res) break;
		// ask 'uniconf' actions to this update,
		// this example doesn't need any HW actions,
		// but needs to send a notice to the other applications
		uc_nc_askaction_push(ucntd, dbald, aps, NULL, NULL);
		intvts=rand()%MAX_INTERVAL;
		tv.tv_sec=intvts/UB_SEC_NS;
		tv.tv_usec=(intvts-(UB_SEC_NS*tv.tv_sec))/UB_USEC_NS;
		uc_dbal_releasedb(dbald);
		select(1, NULL, NULL, NULL, &tv);
	}
	return res;
}

int main(int argc, char* argv[])
{
	uc_dbald *dbald=NULL;
	xl4_data_data_t *xdd=NULL;
	uc_notice_data_t *ucntd=NULL;
	int res=-1;
	struct sigaction sigact;
	unibase_init_para_t init_para;

	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46,ucpra:66",
						   "UBL_UCPRA");
	unibase_init(&init_para);
	ubb_memory_out_init(NULL, 512);
	dbald=uc_dbal_open(UCPRA_DBNAME, "w", UCPRACALLMODE);
	if(!dbald) goto erexit;
	xdd=xl4_data_init(dbald);
	if(!xdd) goto erexit;
	ucntd=uc_notice_init(UCPRACALLMODE);
	if(!ucntd) goto erexit;
	if(xl4_data_set_xl4ext(xdd, "ucpra1", NULL, NULL)) goto erexit;
	if(xl4_extmod_ucpra1_config_init(xdd, dbald, NULL)) goto erexit;
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	res=update_loop(dbald, xdd, ucntd);
erexit:
	UB_LOG(UBL_INFO, "ucpra1_x0 closing\n");
	if(ucntd) uc_notice_close(ucntd, UCPRACALLMODE);
	if(xdd) xl4_data_close(xdd);
	if(dbald) uc_dbal_close(dbald, UCPRACALLMODE);
	ubb_memory_file_out("ucpra1_x0_debug.log");
	ubb_memory_out_close();
	unibase_close();
	return res;
}