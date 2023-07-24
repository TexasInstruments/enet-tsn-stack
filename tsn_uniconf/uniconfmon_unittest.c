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
#include "uniconf_unittest_helper.h"

#define UCTEST_DBNAME ".uniconftestdb"

/* uniconfmon result output is "uniconfmon result_string" format.
   find result_string part, and conver it to integer */
static int get_result_num(char *pstr, int *num)
{
	char *rstr;
	rstr=strstr(pstr, "uniconfmon ");
	if(!rstr){return -1;}
	memmove(pstr, rstr+11, strlen(rstr+11)+1);
	rstr=strchr(pstr, '\n');
	if(!rstr){return -1;}
	*rstr=0;
	*num=strtol(pstr, NULL, 0);
	return 0;
}

static void test_write_read(void **state)
{
	char pstr[1024];
	FILE *fo;
	int res;
	// check if unicof is started
	sprintf(pstr, "./uniconfmon -p "UCTEST_DBNAME" -w 10 -i -1 -n /");
	fo=popen(pstr, "w");
	res=pclose(fo);
	assert_int_equal(res, 0);
	// write a value
	sprintf(pstr, "./uniconfmon -p "UCTEST_DBNAME
		" -i -1 -n \"/ietf-interfaces/interfaces/interface|name:cbeth0|/speed\" 500");
	fo=popen(pstr, "w");
	res=pclose(fo);
	assert_int_equal(res, 0);
	// read back the value
	sprintf(pstr, "./uniconfmon -p "UCTEST_DBNAME
		" -i -1 -n \"/ietf-interfaces/interfaces/interface|name:cbeth0|/speed\"");
	fo=popen(pstr, "r");
	while(true){
		res=fread(pstr, 1, sizeof(pstr)-1, fo);
		if(res<=0) break;
		pstr[res]=0;
		if(get_result_num(pstr, &res)==0) break;
	}
	pclose(fo);
	assert_int_equal(res, 500);
}

static int setup(void **state)
{
	uniconf_uthp_data_t *utd;
	utd=malloc(sizeof(uniconf_uthp_data_t));
	memset(utd, 0, sizeof(uniconf_uthp_data_t));
	utd->thread_mode=false;
	utd->ucmd.dbname=UCTEST_DBNAME;
	utd->ucmd.ucmode=UC_CALLMODE_UNICONF;
	utd->ucmd.ucmanstart=malloc(sizeof(CB_SEM_T));
	utd->ucmd.hwmod="";
	*state=utd;
	return uniconf_unittest_setup(state);
}

static int teardown(void **state)
{
	uniconf_uthp_data_t *utd;
	int res=uniconf_unittest_teardown(state);
	utd=(uniconf_uthp_data_t *)*state;
	if(utd){
		free(utd->ucmd.ucmanstart);
		free(utd);
	}
	return res;
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_write_read),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
