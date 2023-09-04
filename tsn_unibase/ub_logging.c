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
 * ub_logging.c
 *
 */
#include <stdlib.h>
#include <string.h>
#include "unibase_private.h"

static inline int check_cat_index(int cat_index, const char *func)
{
	if((cat_index >= MAX_LOGMSG_CATEGORIES) || (cat_index < 0)){
		UB_CONSOLE_PRINT("%s:cat_index=%d is out of the range\n", func, cat_index);
		return -1;
	}
	return 0;
}

void ub_console_debug_select_print(bool console, bool debug, const char *astr)
{
	if(console && ubcd.cbset.console_out){ubcd.cbset.console_out(false, astr);}
	if(debug && ubcd.cbset.debug_out){ubcd.cbset.debug_out(false, astr);}
}

static int log_one_category(const char *ns, ub_logmsg_data_t *logmsgd, char *category_name)
{
	int i;
	int v;
	int nnp=0;
	if(!category_name){
		v=strlen(&ns[nnp]);
		for(i=0;i<v;i++){
			if(ns[nnp+i]!=':'){continue;}
			break;
		}
		if(i>CATEGORY_NAME_CHAR_MAX){
			UB_CONSOLE_PRINT("%s:wrong init string:%s\n",__func__, &ns[nnp]);
			return -1;
		}
		memcpy(logmsgd->category_name, &ns[nnp], i);
		logmsgd->category_name[i]=0;
		nnp+=i+1;
	}else{
		(void)strncpy(logmsgd->category_name, category_name, CATEGORY_NAME_CHAR_MAX);
	}
	v=ns[nnp]-'0';
	if((v<0) || (v>(int)UBL_DEBUGV)){
		UB_CONSOLE_PRINT("%s:wrong console log level:%s\n",__func__, &ns[nnp]);
		return-1;
	}
	logmsgd->clevel=(ub_dbgmsg_level_t)v;
	nnp+=1;
	v=ns[nnp]-'0';
	if((v<0) || (v>(int)UBL_DEBUGV)){
		// it seems to be only 1 digit, use 'clevel' for 'dlevel'
		logmsgd->dlevel=logmsgd->clevel;
	}else{
		logmsgd->dlevel=(ub_dbgmsg_level_t)v;
		nnp+=1;
	}
	if(ns[nnp]=='m'){
		nnp+=1;
		logmsgd->flags=UB_CLOCK_MONOTONIC;
	}else if (ns[nnp]=='r'){
		nnp+=1;
		logmsgd->flags=UB_CLOCK_REALTIME;
	}else if (ns[nnp]=='g'){
		nnp+=1;
		logmsgd->flags=UB_CLOCK_GPTP;
	}else{
		logmsgd->flags=UB_CLOCK_DEFAULT;
	}
	return nnp;
}

const char *ub_log_initstr_override(const char *ns, const char *os)
{
	int v;

	(void)memset(&ubcd.logmsgd_ovrd, 0, sizeof(ub_logmsg_data_t));
	if(!os){return ns;}
	if(strchr(os, ',')!=NULL){return os;} // if 'os' has ',', replace 'ns' with 'os'
	if(strchr(os, ':')==NULL){return os;} // if 'os' doesn't have ':', replace 'ns' with 'os'
	v=log_one_category(os, &ubcd.logmsgd_ovrd, NULL);
	if(v<(int)strlen(os)){
		(void)printf("invalid format of override string=%s\n", os);
	}
	return ns;
}

void ub_log_init(const char *ns)
{
	int i;
	int v;
	int cat_index;
	int defv=UBL_INFO; // default is the info level only on the console
	char defcname[8];
	int nsp=0;

	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.gmutex);}
	v=-1;
	if(ns!=NULL){
		v=log_one_category(ns, &ubcd.logmsgd[0], "def00");
	}
	if(v<0){
		(void)memset(&ubcd.logmsgd[0], 0, sizeof(ub_logmsg_data_t));
		(void)strcpy(ubcd.logmsgd[0].category_name, "def00");
		ubcd.logmsgd[0].clevel=(ub_dbgmsg_level_t)defv;
		ubcd.logmsgd[0].dlevel=(ub_dbgmsg_level_t)defv;
		v=0;
	}
	for(i=1;i<MAX_LOGMSG_CATEGORIES;i++){
		memcpy(&ubcd.logmsgd[i], &ubcd.logmsgd[0], sizeof(ub_logmsg_data_t));
		(void)snprintf(defcname, sizeof(defcname), "def%02d", i);
		(void)strcpy(ubcd.logmsgd[i].category_name, defcname);
	}
	if(!ns){goto erexit;}
	nsp=v;
	cat_index=0;
	ubcd.log_categories=0;
	while(ns[nsp]!=0){
		if(ns[nsp]==','){nsp++;}
		if(cat_index>=MAX_LOGMSG_CATEGORIES){
			UB_CONSOLE_PRINT("%s:no more room to add a category\n",__func__);
			break;
		}
		v=log_one_category(&ns[nsp], &ubcd.logmsgd[cat_index], NULL);
		if(v<0){continue;}
		if(!strcmp(ubcd.logmsgd_ovrd.category_name,
			   ubcd.logmsgd[cat_index].category_name)){
			memcpy(&ubcd.logmsgd[cat_index], &ubcd.logmsgd_ovrd,
			       sizeof(ub_logmsg_data_t));
		}
		nsp+=v;
		cat_index++;
		ubcd.log_categories++;
	}
erexit:
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.gmutex);}
}

int ub_log_add_category(const char *catstr)
{
	int v;
	if(ubcd.log_categories>=MAX_LOGMSG_CATEGORIES){
		UB_CONSOLE_PRINT("%s:no more room to add a category\n",__func__);
		return -1;
	}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.gmutex);}
	v=log_one_category(catstr, &ubcd.logmsgd[ubcd.log_categories], NULL);
	if(v>=0){ubcd.log_categories++;}
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.gmutex);}
	if(v<0){return -1;}
	return 0;
}

int ub_log_print(int cat_index, int flags, ub_dbgmsg_level_t level, const char *astr)
{
	ub_clocktype_t addts;
	uint64_t ts64=0;
	uint32_t tsec;
	uint32_t tnsec;
	char *level_mark[]=DBGMSG_LEVEL_MARK;
	char tsstr[64];

	if(level>UBL_DEBUGV){return -1;}
	if(check_cat_index(cat_index, __func__)!=0){return -1;}
	if(((level > ubcd.logmsgd[cat_index].clevel) &&
	    (level > ubcd.logmsgd[cat_index].dlevel)) || (level==UBL_NONE)){return 0;}
	addts = (ub_clocktype_t)(ubcd.logmsgd[cat_index].flags & (uint32_t)UBL_TS_BIT_FIELDS);
	if(!addts){addts = (ub_clocktype_t)(flags & UBL_TS_BIT_FIELDS);}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.gmutex);}
	if(addts!=UB_CLOCK_DEFAULT){
		if(ubcd.cbset.gettime64!=NULL){ts64=ubcd.cbset.gettime64(addts);}
		tsec=(uint32_t)(ts64/(uint64_t)UB_SEC_NS);
		tnsec=(uint32_t)(ts64%(uint64_t)UB_SEC_NS);
		(void)snprintf(tsstr, 64, "%s:%s:%06u-%06u:", level_mark[level],
			 ubcd.logmsgd[cat_index].category_name,
			 (uint32_t)(tsec%1000000u), (uint32_t)(tnsec/1000u));
		ub_console_debug_select_print(level <= ubcd.logmsgd[cat_index].clevel,
					      level <= ubcd.logmsgd[cat_index].dlevel, tsstr);
	}else{
		(void)snprintf(tsstr, 64, "%s:%s:", level_mark[level],
			 ubcd.logmsgd[cat_index].category_name);
		ub_console_debug_select_print(level <= ubcd.logmsgd[cat_index].clevel,
					      level <= ubcd.logmsgd[cat_index].dlevel, tsstr);
	}
	ub_console_debug_select_print(level <= ubcd.logmsgd[cat_index].clevel,
				      level <= ubcd.logmsgd[cat_index].dlevel, astr);
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.gmutex);}
	return 0;
}

bool ub_clog_on(int cat_index, ub_dbgmsg_level_t level)
{
	if(level<=UBL_NONE){return false;}
	if(check_cat_index(cat_index, __func__)!=0){return false;}
	if(level > ubcd.logmsgd[cat_index].clevel){return false;}
	return true;
}

bool ub_dlog_on(int cat_index, ub_dbgmsg_level_t level)
{
	if(level<=UBL_NONE){return false;}
	if(check_cat_index(cat_index, __func__)!=0){return false;}
	if(level > ubcd.logmsgd[cat_index].dlevel){return false;}
	return true;
}

int ub_log_change(int cat_index, ub_dbgmsg_level_t clevel, ub_dbgmsg_level_t dlevel)
{
	if(check_cat_index(cat_index, __func__)!=0){return -1;}
	ubcd.logmsgd[cat_index].clevel_saved=ubcd.logmsgd[cat_index].clevel;
	ubcd.logmsgd[cat_index].clevel=clevel;
	ubcd.logmsgd[cat_index].dlevel_saved=ubcd.logmsgd[cat_index].dlevel;
	ubcd.logmsgd[cat_index].dlevel=dlevel;
	return 0;
}

int ub_log_return(int cat_index)
{
	if(check_cat_index(cat_index, __func__)!=0){return false;}
	ubcd.logmsgd[cat_index].clevel=ubcd.logmsgd[cat_index].clevel_saved;
	ubcd.logmsgd[cat_index].dlevel=ubcd.logmsgd[cat_index].dlevel_saved;
	return 0;
}

void ub_log_flush(void)
{
	if(ubcd.cbset.console_out!=NULL){ubcd.cbset.console_out(true, "");}
	if(ubcd.cbset.debug_out!=NULL){ubcd.cbset.debug_out(true, "");}
}
