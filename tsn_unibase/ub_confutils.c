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
#include "unibase_private.h"
#include <string.h>
#include "ub_strutils.h"
#include "ub_confutils.h"

#define MAX_CHARS_PER_LINE 256
static int conf_readline(FILE *inf, char *line)
{
	char a;
	int res;
	int rp=0;

	if(feof(inf)!=0) {return 0;}
	while(true){
		res=fread(&a, 1, 1, inf);
		if(!res) {break;}
		if(res<0){
			if(feof(inf)!=0) {return 0;}
			UB_LOG(UBL_WARN,"%s:error to read config file\n", __func__);
			return -1;
		}
		if(a=='\n') {break;}
		line[rp]=a;
		rp++;
		if(rp>=(MAX_CHARS_PER_LINE-1)){
			UB_LOG(UBL_WARN,"%s:too long line\n", __func__);
			return -1;
		}
	}
	line[rp]=0;
	rp++;
	return rp;
}

static int config_set_oneitem(char *itemp, char *sp, ub_set_item_cb_t set_item)
{
	int64_t v;
	uint8_t p[16];
	if(!strchr(sp,',') && strchr(sp,':')){
		if(ub_hexstr2barray(sp, p, ':', 16)==0) {return -1;}
		return set_item(itemp, p);
	}
	if(sp[0]=='"'){
		if(sp[strlen(&sp[1])]!='"') {return -1;}
		sp[strlen(&sp[1])]=0;
		return set_item(itemp, &sp[1]);
	}
	v=strtol(sp, NULL, 0);
	return set_item(itemp, &v);
}

int ub_read_config_file(char *fname, ub_set_item_cb_t set_item)
{
	FILE *inf;
	char line[MAX_CHARS_PER_LINE];
	int itemp;
	int res;
	int lp;
	int nlp;

	inf=fopen(fname, "r");
	if(!inf){
		UB_LOG(UBL_WARN,"%s:can't read config file:%s\n", __func__, fname);
		return -1;
	}
	while(true){
		res=conf_readline(inf, line);
		if(res<=0) {break;}
		lp=ub_find_nospace(line, MAX_CHARS_PER_LINE);
		if(lp<0) {continue;}
		if(line[lp]=='#') {continue;}
		itemp=lp;

		nlp=ub_find_space(&line[lp], MAX_CHARS_PER_LINE-lp);
		if(nlp<0) {continue;}
		lp+=nlp;
		line[lp]=0;

		nlp=ub_find_nospace(&line[lp+1], MAX_CHARS_PER_LINE-(lp+1));
		if(nlp<0) {continue;}
		lp+=nlp+1;

		nlp=ub_find_space(&line[lp], MAX_CHARS_PER_LINE-lp);
		if(nlp>=0){
			line[lp+nlp]=0;
		}

		if(!config_set_oneitem(&line[itemp], &line[lp], set_item)){
			UB_LOG(UBL_INFO,"%s:configured:%s\n", __func__, line);
		}else{
			UB_LOG(UBL_WARN,"%s:can't process this line:%s\n", __func__, line);
		}
	}
	(void)fclose(inf);
	return res;
}
