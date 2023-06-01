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
 * cb_crc_unittest.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <cmocka.h>
#include "combase.h"
#include <tsn_unibase/unibase_binding.h>

typedef struct {
	char *input;
	uint32_t input_length;
	uint32_t result;
} crcdata_t;

#define DATA_ENTRY_NUM 7
/*
 * Refer to SWS_Crc_00055 of Specification of CRC Routines AUTOSAR CP R20-11.
*/
crcdata_t s_crcdata[DATA_ENTRY_NUM] = {
	{"\x00\x00\x00\x00", 4, 0x2144DF1C},
	{"\xF2\x01\x83", 3, 0x24AB9D77},
	{"\x0F\xAA\x00\x55", 4, 0xB6C9B287},
	{"\x00\xFF\x55\x11", 4, 0x32A06212},
	{"\x33\x22\x55\xAA\xBB\xCC\xDD\xEE\xFF", 9, 0xB0AE863D},
	{"\x92\x6B\x55", 3, 0x9CDEA29B},
	{"\xFF\xFF\xFF\xFF", 4, 0xFFFFFFFF},
};

static void test_crc32_calculate(void **state)
{
	int i;

	assert_int_equal(cb_crc32_calculate(NULL, 0), 0);
	assert_int_equal(cb_crc32_calculate((uint8_t*)0xabcde, 0), 0);
	
	for (i = 0; i < DATA_ENTRY_NUM; i++) {
		assert_int_equal(
			cb_crc32_calculate((uint8_t *)s_crcdata[i].input, s_crcdata[i].input_length),
			s_crcdata[i].result);
	}
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr="4,ubase:45,combase:45";
	unibase_init(&init_para);
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
		cmocka_unit_test(test_crc32_calculate),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
