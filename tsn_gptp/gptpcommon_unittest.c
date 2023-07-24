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
#include <setjmp.h>
#include <cmocka.h>
#include <tsn_unibase/unibase_binding.h>
#include "gptpcommon.h"

static void test_eui48to64(void **state)
{
	const uint8_t d1[6]={0x11,0x22,0x33,0x44,0x55,0x66};
	const uint8_t r1[8]={0x11,0x22,0x33,0xff,0xfe,0x44,0x55,0x66};
	const uint8_t r2[8]={0x11,0x22,0x33,0xaa,0xbb,0x44,0x55,0x66};
	uint8_t r[8];
	uint8_t i[2]={0xaa,0xbb};
	eui48to64(d1, r, NULL);
	assert_memory_equal(r, r1, 8);
	eui48to64(d1, r, i);
	assert_memory_equal(r, r2, 8);
}

static void test_log_to_nsec(void **state)
{
	assert_int_equal(LOG_TO_NSEC(0), 1 * UB_SEC_NS);
	assert_int_equal(LOG_TO_NSEC(-1), 500000000);
	assert_int_equal(LOG_TO_NSEC(-2), 250000000);
	assert_int_equal(LOG_TO_NSEC(-3), 125000000);
	assert_int_equal(LOG_TO_NSEC(1), 2 * UB_SEC_NS);
	assert_int_equal(LOG_TO_NSEC(2), 4 * UB_SEC_NS);
	assert_int_equal(LOG_TO_NSEC(3), 8 * UB_SEC_NS);
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46,gptp:46",
						   "UBL_GPTP");
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
		cmocka_unit_test(test_eui48to64),
		cmocka_unit_test(test_log_to_nsec),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
