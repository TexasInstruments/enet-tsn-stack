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
#define UCTEST_XML_CONF "../../tsn_uniconf/yangs/testdata/ietf-interfaces.xml"

/* uniconfmon result output is "uniconfmon result_string" format.
   find result_string part, and conver it to integer */
static char *get_result_num(char *pstr, int *num)
{
	char *rstr;
	rstr=strstr(pstr, "uniconfmon ");
	if(!rstr){return NULL;}
	memmove(pstr, rstr+11, strlen(rstr+11)+1);
	rstr=strchr(pstr, '\n');
	if(!rstr){return NULL;}
	*rstr=0;
	if(num!=NULL){
		*num=strtol(pstr, NULL, 0);
	}
	return pstr;
}

static int one_write_read(char *kstr, char *vstr)
{
	char pstr[1024];
	char kkstr[128];
	FILE *fo;
	int res;
	bool delmode=false;

	strcpy(kkstr, kstr);
	if(kkstr[strlen(kkstr)-1]=='-'){delmode=true;}
	// write a value
	if(vstr){
		sprintf(pstr, "./uniconfmon -p "UCTEST_DBNAME
			" -i -1 -n \"%s\" %s", kkstr, vstr);
	}else{
		sprintf(pstr, "./uniconfmon -p "UCTEST_DBNAME
			" -i -1 -n \"%s\"", kkstr);
	}
	fo=popen(pstr, "w");
	res=pclose(fo);
	assert_int_equal(res, 0);
	// read back the value
	if(delmode){
		kkstr[strlen(kkstr)-1]=0;
	}
	sprintf(pstr, "./uniconfmon -p "UCTEST_DBNAME
		" -i -1 -n \"%s\"", kkstr);
	fo=popen(pstr, "r");
	while(true){
		res=fread(pstr, 1, sizeof(pstr)-1, fo);
		if(res<=0){
			res=-1;
			break;
		}
		pstr[res]=0;
		if(get_result_num(pstr, &res)!=NULL) break;
	}
	pclose(fo);
	return res;
}

static char *one_read(char *kstr)
{
	char pstr[1024];
	FILE *fo;
	int res;
	char *rd;

	sprintf(pstr, "./uniconfmon -p "UCTEST_DBNAME
		" -i -1 -n \"%s\"", kstr);
	fo=popen(pstr, "r");
	while(true){
		res=fread(pstr, 1, sizeof(pstr)-1, fo);
		if(res<=0){
			res=-1;
			break;
		}
		pstr[res]=0;
		rd=get_result_num(pstr, NULL);
		if(rd!=NULL){break;}
	}
	pclose(fo);
	return rd;
}

static void test_write_read(void **state)
{
	char *pstr="./uniconfmon -p "UCTEST_DBNAME" -w 10 -i -1 -n /";
	FILE *fo;
	int res;
	// check if unicof is started
	fo=popen(pstr, "w");
	res=pclose(fo);
	assert_int_equal(res, 0);

	res=one_write_read("/ietf-interfaces/interfaces/interface|name:cbeth0|/speed", "500");
	assert_int_equal(res, 500);

	res=one_write_read("/ietf-interfaces/interfaces/interface|name:cbeth0|/speed-", NULL);
	assert_int_equal(res, -1);
}

static void test_conf_read(void **state)
{
	char *pstr="./uniconfmon -p "UCTEST_DBNAME" -c "UCTEST_XML_CONF;
	FILE *fo;
	char *res;
	// check if unicof is started
	fo=popen(pstr, "w");
	assert_int_equal(pclose(fo), 0);

	res=one_read("/ietf-interfaces/interfaces/interface|name:eth0|/enabled");
	assert_non_null(res);
	assert_string_equal(res, "true");
	res=one_read("/ietf-interfaces/interfaces/interface|name:eth0|/bridge-port/"
		     "gate-parameter-table/queue-max-sdu-table|traffic-class:0|/queue-max-sdu");
	assert_non_null(res);
	assert_string_equal(res, "750");
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
		cmocka_unit_test(test_conf_read),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
