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
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include "unibase.h"

static void test_smac2bmac(void **state)
{
	uint8_t bdata[32];
	uint8_t *bdb;

	bdata[6]=0xff;
	bdb=ub_smac2bmac("a1:b2:c3:D4:e5:F6", bdata);
	assert_int_equal(bdb[0],0xa1);
	assert_int_equal(bdb[1],0xb2);
	assert_int_equal(bdb[2],0xc3);
	assert_int_equal(bdb[3],0xd4);
	assert_int_equal(bdb[4],0xe5);
	assert_int_equal(bdb[5],0xf6);
	assert_int_equal(bdb[6],0xff);
}

static void test_bmac2smac(void **state)
{
	uint8_t bdata[6]={0xa1,0xb2,0xc3,0xD4,0xe5,0xF6};
	char bstr[18];

	(void)ub_bmac2smac(bdata, bstr);
	assert_string_equal(bstr, "A1:B2:C3:D4:E5:F6");
}

static void test_ssid2bsid(void **state)
{
	uint8_t bdata[32];
	uint8_t *bdb;

	bdata[8]=0xff;
	bdb=ub_ssid2bsid("11:a1:b2:c3:D4:e5:F6:aA", bdata);
	assert_int_equal(bdb[0],0x11);
	assert_int_equal(bdb[1],0xa1);
	assert_int_equal(bdb[2],0xb2);
	assert_int_equal(bdb[3],0xc3);
	assert_int_equal(bdb[4],0xd4);
	assert_int_equal(bdb[5],0xe5);
	assert_int_equal(bdb[6],0xf6);
	assert_int_equal(bdb[7],0xaa);
	assert_int_equal(bdb[8],0xff);
}

static void test_bsid2ssid(void **state)
{
	uint8_t ssid[8]={0x11,0xa1,0xb2,0xc3,0xD4,0xe5,0xF6,0xaA};
	char bstr[24];
	(void)ub_bsid2ssid(ssid, bstr);
	assert_string_equal(bstr, "11:A1:B2:C3:D4:E5:F6:AA");
}

static void test_str2bytearray(void **state)
{
	int res;
	const char *astr="00:11:22";
	uint8_t rbuf[256];
	res=ub_str2bytearray(rbuf, astr, 16);
	assert_int_equal(res,3);
	assert_int_equal(rbuf[0],0);
	assert_int_equal(rbuf[1],0x11);
	assert_int_equal(rbuf[2],0x22);
	astr="10,1,2,20";
	res=ub_str2bytearray(rbuf, astr, 10);
	assert_int_equal(res,4);
	assert_int_equal(rbuf[0],10);
	assert_int_equal(rbuf[1],1);
	assert_int_equal(rbuf[2],2);
	assert_int_equal(rbuf[3],20);
	astr="1,2,300,5";
	res=ub_str2bytearray(rbuf, astr, 10);
	assert_int_equal(res,2);
	assert_int_equal(rbuf[0],1);
	assert_int_equal(rbuf[1],2);
	astr="1,,2";
	res=ub_str2bytearray(rbuf, astr, 10);
	assert_int_equal(res,1);
	assert_int_equal(rbuf[0],1);
	astr="";
	res=ub_str2bytearray(rbuf, astr, 0);
	assert_int_equal(res,0);
	astr="-1,0";
	res=ub_str2bytearray(rbuf, astr, 0);
	assert_int_equal(res,0);
	astr="a";
	res=ub_str2bytearray(rbuf, astr, 0);
	assert_int_equal(res,0);
	astr="a";
	res=ub_str2bytearray(rbuf, astr, 16);
	assert_int_equal(res,1);
	assert_int_equal(rbuf[0],0xa);
	astr="0,1,,,,";
	res=ub_str2bytearray(rbuf, astr, 0);
	assert_int_equal(res,2);
	assert_int_equal(rbuf[0],0);
	assert_int_equal(rbuf[1],1);
}

static void test_bytearray2str(void **state)
{
	char dest[100];
	uint8_t bytes[] = {0x01, 0x23, 0x45, 0x67};
	assert_null(ub_bytearray2str(NULL, bytes, sizeof(bytes)));
	assert_null(ub_bytearray2str(NULL, NULL, sizeof(bytes)));
	assert_null(ub_bytearray2str(dest, NULL, sizeof(bytes)));
	assert_null(ub_bytearray2str(dest, bytes, 0));
	assert_string_equal(ub_bytearray2str(dest, bytes, sizeof(bytes)), "01,23,45,67");
	assert_string_equal(ub_bytearray2str(dest, bytes, 1), "01");
	assert_string_equal(ub_bytearray2str(dest, bytes, 2), "01,23");
	assert_string_equal(ub_bytearray2str(dest, bytes, 3), "01,23,45");
	uint8_t zerobytes[] = {0x00, 0x00, 0x00, 0x00};
	assert_string_equal(ub_bytearray2str(dest, zerobytes, sizeof(zerobytes)), "00,00,00,00");

	uint8_t ffbytes[]   = {0xFF, 0xFF, 0xFF, 0xFF};
	assert_string_equal(ub_bytearray2str(dest, ffbytes, sizeof(ffbytes)), "ff,ff,ff,ff");
}

static void test_command_line_parser(void **state)
{
	char *argv[5];
	int argc=5;
	char line[100];
	(void)strcpy(line, "abcdef");
	assert_int_equal(ub_find_nospace(line, strlen(line)), 0);
	assert_int_equal(ub_find_space(line, strlen(line)), -1);
	(void)strcpy(line, "abc def");
	assert_int_equal(ub_find_nospace(line, strlen(line)), 0);
	assert_int_equal(ub_find_space(line, strlen(line)), 3);
	(void)strcpy(line, "    ");
	assert_int_equal(ub_find_nospace(line, strlen(line)), -1);
	assert_int_equal(ub_find_space(line, strlen(line)), 0);
	(void)strcpy(line, "  a  ");
	assert_int_equal(ub_find_nospace(line, strlen(line)), 2);
	assert_int_equal(ub_find_space(line, strlen(line)), 0);

	(void)strcpy(line, "abc def ghi");
	argc=ub_command_line_parser(line, argv, 5);
	assert_int_equal(argc, 3);
	assert_string_equal(argv[0], "abc");
	assert_string_equal(argv[1], "def");
	assert_string_equal(argv[2], "ghi");
	(void)memset(argv, 0, sizeof(argv));
	argc=ub_command_line_parser(line, argv, 1);
	assert_int_equal(argc, 1);
	assert_string_equal(argv[0], "abc");
	assert_null(argv[1]);

	(void)memset(argv, 0, sizeof(argv));
	(void)strcpy(line, "abc");
	argc=ub_command_line_parser(line, argv, 5);
	assert_int_equal(argc, 1);
	assert_string_equal(argv[0], "abc");
	assert_null(argv[1]);

	(void)memset(argv, 0, sizeof(argv));
	(void)strcpy(line, "");
	argc=ub_command_line_parser(line, argv, 5);
	assert_int_equal(argc, 0);
	assert_null(argv[0]);

	(void)memset(argv, 0, sizeof(argv));
	(void)strcpy(line, "   ");
	argc=ub_command_line_parser(line, argv, 5);
	assert_int_equal(argc, 0);
	assert_null(argv[0]);

	(void)memset(argv, 0, sizeof(argv));
	(void)strcpy(line, "   abc def   ");
	argc=ub_command_line_parser(line, argv, 5);
	assert_int_equal(argc, 2);
	assert_string_equal(argv[0], "abc");
	assert_string_equal(argv[1], "def");
	assert_null(argv[2]);
}

static void test_strncpy(void **state)
{
	char x[10];
	int res;
	res=ub_strncpy(x, "abc", 10);
	assert_int_equal(res, 0);
	assert_string_equal(x, "abc");

	res=ub_strncpy(x, "012345678", 10);
	assert_int_equal(res, 0);
	assert_string_equal(x, "012345678");

	res=ub_strncpy(x, "0123456789", 10);
	assert_int_equal(res, -1);

	res=ub_strncpy(x, "0123456789a", 10);
	assert_int_equal(res, -1);
}

static int setup(void **state)
{
	return 0;
}

static int teardown(void **state)
{
	return 0;
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_smac2bmac),
		cmocka_unit_test(test_bmac2smac),
		cmocka_unit_test(test_ssid2bsid),
		cmocka_unit_test(test_bsid2ssid),
		cmocka_unit_test(test_str2bytearray),
		cmocka_unit_test(test_bytearray2str),
		cmocka_unit_test(test_command_line_parser),
		cmocka_unit_test(test_strncpy),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
