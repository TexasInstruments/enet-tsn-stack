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
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include "uniconf_unittest_helper.h"

static const char *check_exec_file(const char *efiles[])
{
	struct stat sf;
	int i;
	for(i=0;efiles[i];i++){
		if(!stat(efiles[i], &sf) &&
		   (sf.st_mode & (S_IXUSR|S_IXGRP|S_IXOTH))){return efiles[i];}
	}
	return NULL;
}

int uniconf_unittest_setup(void **state)
{
	unibase_init_para_t init_para;
	uniconf_uthp_data_t *utd=(uniconf_uthp_data_t *)*state;
	const char *uniconf[]={"../tsn_uniconf/uniconf", "./uniconf", NULL};
	const char *runiconf;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:66",
						   "UBL_UNICONF");
	unibase_init(&init_para);
	uniconf_remove_dbfile(utd->ucmd.dbname);
	utd->ucmd.stoprun=&utd->stoprun;
	if(utd->thread_mode){
		CB_SEM_INIT(utd->ucmd.ucmanstart, 0, 0);
		if(CB_THREAD_CREATE(&utd->ucthreadt, NULL, uniconf_main, &utd->ucmd)){
			return -1;
		}
		CB_SEM_WAIT(utd->ucmd.ucmanstart);
		UB_LOG(UBL_INFO, "uniconf started in thread mode\n");
	}else{
		runiconf=check_exec_file(uniconf);
		if(!runiconf){
			// expect it is in the executable path
			runiconf="uniconf";
		}
		if((utd->uniconf_pid=fork())==0){
			if(execlp(runiconf, runiconf, "-p", utd->ucmd.dbname, NULL)<0){
				UB_LOG(UBL_ERROR,"can't run %s:%s\n",
				       runiconf, strerror(errno));
				return -1;
			}
			exit(0);
		}
		if(uniconf_ready(utd->ucmd.dbname, 0, 100)){return -1;}
	}
	return 0;
}

int uniconf_unittest_teardown(void **state)
{
	uniconf_uthp_data_t *utd=(uniconf_uthp_data_t *)*state;
	if(utd->thread_mode){
		utd->stoprun=true;
		CB_THREAD_JOIN(utd->ucthreadt, NULL);
	}else{
		kill(utd->uniconf_pid, SIGINT);
		waitpid(utd->uniconf_pid, NULL, 0);
	}
	uniconf_remove_dbfile(utd->ucmd.dbname);
	if(utd->debuglog_file){
		ubb_memory_file_out(utd->debuglog_file);
	}
	ubb_memory_out_close();
	unibase_close();
	return 0;
}
