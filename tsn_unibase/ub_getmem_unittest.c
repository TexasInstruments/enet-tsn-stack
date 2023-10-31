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
#define UB_LOGCAT 0
#define UB_LOGTSTYPE UB_CLOCK_MONOTONIC
#include <string.h>
#include "unibase.h"
#include "ub_cmocka_unittest.h"
#include "unibase_binding.h"

#define FLAGSIZE 80
#define FLAGNUM 10
UB_SD_GETMEM_DEF(test1, FLAGSIZE, FLAGNUM);

static void alloc_fill(uint8_t *dmem[], int fnum, int fsize)
{
	int i;
	for(i=0;i<fnum;i++){
		dmem[i]=UB_SD_GETMEM(test1, fsize);
		assert_non_null(dmem[i]);
		memset(dmem[i], i, fsize);
	}
}

static void check_data(uint8_t *dmem[], int fnum, int fsize)
{
	int i;
	int j;
	for(i=0;i<fnum;i++){
		for(j=0;j<fsize;j++){
			assert_int_equal(dmem[i][j], i);
		}
	}
}

void test_ubgetmem1(void **state)
{
	int i;
	uint8_t *dmem[FLAGNUM];
	uint8_t *mem;
	alloc_fill(dmem, FLAGNUM, FLAGSIZE);
	assert_null(UB_SD_GETMEM(test1, FLAGSIZE)); // no more space
	check_data(dmem, FLAGNUM, FLAGSIZE);
	assert_int_equal(UB_SD_PRINT_USAGE(test1, UBL_NONE), 0);

	for(i=0;i<FLAGNUM;i++){
		UB_SD_RELMEM(test1, dmem[i]);
	}
	assert_int_equal(UB_SD_PRINT_USAGE(test1, UBL_NONE), FLAGNUM);
	alloc_fill(dmem, FLAGNUM, FLAGSIZE);
	check_data(dmem, FLAGNUM, FLAGSIZE);

	for(i=0;i<FLAGNUM;i++){
		UB_SD_RELMEM(test1, dmem[i]);
	}

	alloc_fill(dmem, FLAGNUM/2, 2*FLAGSIZE);
	assert_null(UB_SD_GETMEM(test1, 2*FLAGSIZE)); // no more space
	assert_int_equal(UB_SD_PRINT_USAGE(test1, UBL_NONE), 0);

	UB_SD_RELMEM(test1, dmem[0]);
	UB_SD_RELMEM(test1, dmem[2]);
	UB_SD_RELMEM(test1, dmem[3]);
	assert_int_equal(UB_SD_PRINT_USAGE(test1, UBL_NONE), 6);
	mem=UB_SD_REGETMEM(test1, dmem[4], 3*FLAGSIZE);
	assert_int_equal(UB_SD_PRINT_USAGE(test1, UBL_NONE), 5);
	assert_ptr_equal(mem, dmem[2]);
	for(i=0;i<2*FLAGSIZE;i++){
		// check dmem[4] moved to mem
		assert_int_equal(mem[i], 4);
	}
	dmem[4]=mem;

	// one more 300 must be available
	mem=UB_SD_GETMEM(test1, 3*FLAGSIZE+1);
	assert_null(mem); // no 3*FLAGSIZE+1 spce
	mem=UB_SD_GETMEM(test1, 3*FLAGSIZE);
	assert_non_null(mem); // 3*FLAGSIZE is okay
	UB_SD_RELMEM(test1, mem);

	mem=UB_SD_GETMEM(test1, 4*FLAGSIZE);
	assert_null(mem); // no 4*FLAGSIZE spce
	UB_SD_RELMEM(test1, dmem[0]);
	mem=UB_SD_GETMEM(test1, 4*FLAGSIZE);
	assert_null(mem); // no 4*FLAGSIZE spce
	UB_SD_RELMEM(test1, dmem[1]);
	// freeing 2 first fragments, there must be a space
	mem=UB_SD_GETMEM(test1, 4*FLAGSIZE);
	assert_ptr_equal(mem, dmem[0]);

	return;
}

UB_SD_GETMEM_DEF(test21, 1, 1);
UB_SD_GETMEM_DEF(test28, 8, 1);
UB_SD_GETMEM_DEF(test29, 9, 1);
void test_alignment(void **state)
{
	int i;
	uint8_t *mem;
	for(i=1;i<=(int)sizeof(void*);i++){
		mem=UB_SD_GETMEM(test21, i);
		assert_non_null(mem);
		UB_SD_RELMEM(test21, mem);
	}
	mem=UB_SD_GETMEM(test21, sizeof(void*)+1);
	assert_null(mem);

	for(i=1;i<=(int)sizeof(void*);i++){
		mem=UB_SD_GETMEM(test28, i);
		assert_non_null(mem);
		UB_SD_RELMEM(test28, mem);
	}
	mem=UB_SD_GETMEM(test28, sizeof(void*)+1);
	assert_null(mem);

	for(i=(int)sizeof(void*)+1;i<=(int)sizeof(void*)*2;i++){
		mem=UB_SD_GETMEM(test29, i);
		assert_non_null(mem);
		UB_SD_RELMEM(test29, mem);
	}
	mem=UB_SD_GETMEM(test29, sizeof(void*)*2+1);
	assert_null(mem);
}

static int setup(void **state)
{
	ubb_unibase_easyinit();
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
		cmocka_unit_test(test_ubgetmem1),
		cmocka_unit_test(test_alignment),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
