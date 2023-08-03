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
#include "unibase.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>

#define RTS_SEC 10
#define RTS_MSEC 10
#define MTS_SEC 20
#define MTS_MSEC 20
#define GTS_SEC 30
#define GTS_MSEC 30

#define LINE_BUF_LEN 255
static char console_data[LINE_BUF_LEN+1];
static int cd_point;
static char debug_data[LINE_BUF_LEN+1];
static int dd_point;
enum {
	TEST_MOD0=0,
	TEST_MOD1,
	TEST_MOD2,
	TEST_MOD3,
	TEST_MOD4,
};

#define TEST_UB_LOG_PRINT(res, a, b, level, ...)			\
	{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		res=ub_log_print(a, b, level, coutstr);			\
	}

static int string_buf_out(char *sbuf, int *dp, const char *str)
{
	int res;
	res=strlen(str);
	if((cd_point+res) > LINE_BUF_LEN) {
		*dp=0;
		return 0;
	}
	(void)strcpy(&sbuf[*dp], str);
	if(str[res-1]=='\n'){
		*dp=0;
	}else{
		*dp+=res;
	}
	return res;
}

int test_console_out(bool flush, const char *str)
{
	return string_buf_out(console_data, &cd_point, str);
}
int test_debug_out(bool flush, const char *str)
{
	return string_buf_out(debug_data, &dd_point, str);
}

uint64_t test_gettime64(ub_clocktype_t ctype)
{
	struct timespec ts={0,0};
	switch(ctype){
	case UB_CLOCK_REALTIME:
		//clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec=RTS_SEC;
		ts.tv_nsec=RTS_MSEC*1000000;
		break;
	case UB_CLOCK_MONOTONIC:
		//clock_gettime(CLOCK_MONOTONIC, &ts);
		ts.tv_sec=MTS_SEC;
		ts.tv_nsec=MTS_MSEC*1000000;
		break;
	case UB_CLOCK_GPTP:
		ts.tv_sec=GTS_SEC;
		ts.tv_nsec=GTS_MSEC*1000000;
		break;
	default:
		break;
	}
	return ts.tv_sec*(uint64_t)UB_SEC_NS+ts.tv_nsec;
}

static const char *format1="%s:%s:%06u-%06u:int=%d, hex=0x%x, float=%f\n";
static const char *format2="%s:%s:int=%d, hex=0x%x, float=%f\n";
static const char *level_mark[]=DBGMSG_LEVEL_MARK;
static void test_ub_log_print(void **state)
{
	int i;
	float a;
	char pstr[256];
	int res;
	i=20;
	a=3.1415;

	// mod0 print REALTIME
	(void)sprintf(pstr, format1,level_mark[1],"mod0",
		RTS_SEC, RTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD0, 0, 1, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// mod1 print MONOTONICTIME
	(void)sprintf(pstr, format1,level_mark[1],"mod1",
		MTS_SEC, MTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD1, 0, 1, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// mod2 print GPTPTIME
	(void)sprintf(pstr, format1,level_mark[1],"mod2",
		GTS_SEC, GTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD2, 0, 1, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// mod0 print console level=3, debug level=4
	(void)sprintf(pstr, format1,level_mark[4],"mod0",
		RTS_SEC, RTS_MSEC*1000, i, i, a);
	console_data[0]=0;
	debug_data[0]=0;
	TEST_UB_LOG_PRINT(res, TEST_MOD0, 0, 4, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal("", console_data);
	assert_string_equal(pstr, debug_data);
	console_data[0]=0;
	debug_data[0]=0;
	TEST_UB_LOG_PRINT(res, TEST_MOD0, 0, 5, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal("", console_data);
	assert_string_equal("", debug_data);
	(void)sprintf(pstr, format1,level_mark[3],"mod0",
		RTS_SEC, RTS_MSEC*1000, i, i, a);
	console_data[0]=0;
	debug_data[0]=0;
	TEST_UB_LOG_PRINT(res, TEST_MOD0, 0, 3, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// mod3, no time stamp
	(void)sprintf(pstr, format2,level_mark[1],"mod3",i,i,a);
	console_data[0]=0;
	debug_data[0]=0;
	TEST_UB_LOG_PRINT(res, TEST_MOD3, 0, 1, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// mod3, force to print time stamp
	(void)sprintf(pstr, format1,level_mark[1],"mod3",
		GTS_SEC, GTS_MSEC*1000, i, i, a);
	console_data[0]=0;
	debug_data[0]=0;
	TEST_UB_LOG_PRINT(res, TEST_MOD3, UB_CLOCK_GPTP, 1, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// mod4 is not registered, print with default config
	(void)sprintf(pstr, format1,level_mark[1],"def04",
		GTS_SEC, GTS_MSEC*1000, i, i, a);
	console_data[0]=0;
	debug_data[0]=0;
	TEST_UB_LOG_PRINT(res, TEST_MOD4, UB_CLOCK_GPTP, 1, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// add mod4 config
	assert_int_equal(0, ub_log_add_category("mod4:33m"));
	(void)sprintf(pstr, format1,level_mark[1],"mod4",
		MTS_SEC, MTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD4, 0, 1, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_int_equal(0, res);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	// mod3:23
	assert_false(ub_clog_on(TEST_MOD3, 0));
	assert_true(ub_clog_on(TEST_MOD3, 1));
	assert_true(ub_clog_on(TEST_MOD3, 2));
	assert_false(ub_clog_on(TEST_MOD3, 3));
	assert_false(ub_clog_on(TEST_MOD3, 4));

	assert_false(ub_dlog_on(TEST_MOD3, 0));
	assert_true(ub_dlog_on(TEST_MOD3, 1));
	assert_true(ub_dlog_on(TEST_MOD3, 2));
	assert_true(ub_dlog_on(TEST_MOD3, 3));
	assert_false(ub_dlog_on(TEST_MOD3, 4));

	// change to mod3:34
	(void)ub_log_change(TEST_MOD3, 3, 4);
	assert_true(ub_clog_on(TEST_MOD3, 3));
	assert_false(ub_clog_on(TEST_MOD3, 4));
	assert_true(ub_dlog_on(TEST_MOD3, 4));
	assert_false(ub_dlog_on(TEST_MOD3, 5));

	// return to mod3:23
	(void)ub_log_return(TEST_MOD3);
	assert_true(ub_clog_on(TEST_MOD3, 2));
	assert_false(ub_clog_on(TEST_MOD3, 3));
	assert_true(ub_dlog_on(TEST_MOD3, 3));
	assert_false(ub_dlog_on(TEST_MOD3, 4));

}

static void test_ub_log_print_al1(void **state)
{
	int i;
	float a;
	char pstr[256];
	int res;
	i=20;
	a=3.1415;

	console_data[0]=0;
	debug_data[0]=0;
	// level=3, def01 print REALTIME
	(void)sprintf(pstr, format1,level_mark[3],"def01",
		RTS_SEC, RTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD1, 0, 3, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_int_equal(res, 0);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	console_data[0]=0;
	debug_data[0]=0;
	// level=3, def04 print REALTIME
	(void)sprintf(pstr, format1,level_mark[3],"def04",
		RTS_SEC, RTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD4, 0, 3, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

	console_data[0]=0;
	debug_data[0]=0;
	// level=4, def01 print REALTIME
	(void)sprintf(pstr, format1,level_mark[4],"def01",
		RTS_SEC, RTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD1, 0, 4, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal("", console_data);
	assert_string_equal(pstr, debug_data);

	console_data[0]=0;
	debug_data[0]=0;
	// level=4, def04 print REALTIME
	(void)sprintf(pstr, format1,level_mark[4],"def04",
		RTS_SEC, RTS_MSEC*1000, i, i, a);
	TEST_UB_LOG_PRINT(res, TEST_MOD4, 0, 4, "int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal("", console_data);
	assert_string_equal(pstr, debug_data);

	console_data[0]=0;
	debug_data[0]=0;
	for(i=TEST_MOD0;i<=TEST_MOD4;i++){
		// level=5
		TEST_UB_LOG_PRINT(res, i, 0, 5, "int=%d, hex=0x%x, float=%f\n",i,i,a);
		assert_string_equal("", console_data);
		assert_string_equal("", debug_data);
	}
}


#define UB_LOGCAT TEST_MOD0
#define UB_LOGTSTYPE UB_CLOCK_MONOTONIC
static void test_ub_log_print_macro(void **state)
{
	int i;
	float a;
	char pstr[256];
	i=20;
	a=3.1415;

	// mod0 print no TS
	console_data[0]=0;
	debug_data[0]=0;
	(void)sprintf(pstr, format2,level_mark[UBL_INFO],"mod0", i, i, a);
	UB_LOG(UBL_INFO,"int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal("", console_data);
	assert_string_equal(pstr, debug_data);

	// mod0 print add TS
	console_data[0]=0;
	debug_data[0]=0;
	(void)sprintf(pstr, format1,level_mark[UBL_WARN],"mod0",
		MTS_SEC, MTS_MSEC*1000, i, i, a);
	UB_TLOG(UBL_WARN,"int=%d, hex=0x%x, float=%f\n",i,i,a);
	assert_string_equal(pstr, console_data);
	assert_string_equal(pstr, debug_data);

}

static int setup1(void **state)
{
	unibase_init_para_t init_para={
		.cbset.console_out=test_console_out,
		.cbset.debug_out=test_debug_out,
		.cbset.get_static_mutex=NULL,
		.cbset.gettime64=test_gettime64,
		.ub_log_initstr="2,mod0:34r,mod1:45m,mod2:56g,mod3:23",
	};
	(void)unibase_init(&init_para);
	return 0;
}

static int setup2(void **state)
{
	unibase_init_para_t init_para={
		.cbset.console_out=test_console_out,
		.cbset.debug_out=test_debug_out,
		.cbset.get_static_mutex=NULL,
		.cbset.gettime64=test_gettime64,
		.ub_log_initstr="mod0:34,mod1:56m",//3:WARN, 4:INFO
	};
	(void)unibase_init(&init_para);
	return 0;
}

static int setup3(void **state)
{
	unibase_init_para_t init_para={
		.cbset.console_out=test_console_out,
		.cbset.debug_out=test_debug_out,
		.cbset.get_static_mutex=NULL,
		.cbset.gettime64=test_gettime64,
		//3:WARN, 4:INFO with RTS for all levels
		.ub_log_initstr=ub_log_initstr_override(
			"2,mod0:34r,mod1:45m,mod2:56g,mod3:23","34r"),
	};
	(void)unibase_init(&init_para);
	return 0;
}

static int teardown(void **state)
{
	unibase_close();
	return 0;
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_ub_log_print, setup1, teardown),
		cmocka_unit_test_setup_teardown(test_ub_log_print_macro, setup2, teardown),
		cmocka_unit_test_setup_teardown(test_ub_log_print_al1, setup3, teardown),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
