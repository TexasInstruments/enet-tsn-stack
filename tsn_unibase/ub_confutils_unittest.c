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
#include "unibase_binding.h"
#include "ubconftest_conf.h"
extern int ubconftestconf_values_test(void);

static void create_conf_file(void)
{
	FILE *fp;
	char *astr;
	fp=fopen("ubconfutils_test.conf", "w");
	astr="CONF_TEST_INT1 12\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="CONF_TEST_INT2 12 # comment post value\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="\n"; // empty line
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="# this is a comment line\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="CONF_TEST_HEXINT1 0x1000000b\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="CONF_TEST_LINT1 10000000002\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="# this is another comment line\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="CONF_TEST_BYTE6 1a:2b:3c:4d:5e:70\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="CONF_TEST_STR2 \"hello world\"\n";
	(void)fwrite(astr, 1, strlen(astr), fp);
	astr="CONF_TEST_STR3 \"hello world\" #comment post value\n";
	(void)fwrite(astr, 1, strlen(astr), fp);

	(void)fclose(fp);
}

static void test_ubconfutils(void **state)
{
	const uint8_t d6[]={0x1a,0x2b,0x3c,0x4d,0x5e,0x70};
	create_conf_file();
	assert_false(ub_read_config_file("ubconfutils_test.conf", ubconftestconf_set_stritem));
	(void)ubconftestconf_values_test();
	assert_int_equal(ubconftestconf_get_intitem(CONF_TEST_INT1), 12);
	assert_int_equal(ubconftestconf_get_intitem(CONF_TEST_INT2), 12);
	assert_int_equal(ubconftestconf_get_intitem(CONF_TEST_HEXINT1), 0x1000000b);
	assert_int_equal(ubconftestconf_get_lintitem(CONF_TEST_LINT1), 10000000002);
	assert_memory_equal(ubconftestconf_get_item(CONF_TEST_BYTE6), d6, 6);
	assert_string_equal(ubconftestconf_get_item(CONF_TEST_STR2), "hello world");
	assert_string_equal(ubconftestconf_get_item(CONF_TEST_STR3), "hello world");
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
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
		cmocka_unit_test(test_ubconfutils),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
