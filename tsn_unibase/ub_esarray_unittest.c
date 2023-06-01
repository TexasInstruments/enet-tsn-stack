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
#include "ub_unittest_helper.c"

static void test_esarray1(void **state)
{
	ub_esarray_cstd_t *eah;
	int *pi;
	int i;
	int max_pnum = 11;

	eah = ub_esarray_init(4,4,max_pnum);

	for (i=0;i<max_pnum;i++){
		(void)ub_esarray_add_ele(eah, (ub_esarray_element_t *)&i);
	}

	// a new element exceed max elements
	assert_null(ub_esarray_get_newele(eah));

	assert_int_equal(ub_esarray_ele_nums(eah), max_pnum);

	for (i=0;i<max_pnum;i++){
		pi=(int*)ub_esarray_get_ele(eah, i);
		assert_int_equal(i, *pi);
	}

	pi = (int*)ub_esarray_get_ele(eah, 2);
	(void)ub_esarray_del_ele(eah, (ub_esarray_element_t *)pi); // 3rd point is removed
	for (i=0;i<2;i++){
		// before 3rd pointer
		pi = (int*)ub_esarray_get_ele(eah, i);
		assert_int_equal(i, *pi);
	}
	for (i=2;i<(max_pnum-1);i++){
		// 3rd to (MAX_PNUM-1)th
		pi = (int*)ub_esarray_get_ele(eah, i);
		assert_int_equal(i+1, *pi);
	}

	assert_int_equal(max_pnum-1, ub_esarray_ele_nums(eah));

	for (i=0;i<(max_pnum-1);i++){
		pi = (int*)ub_esarray_get_ele(eah, 0);
		if(i<2){
			assert_int_equal(i, *pi);
		}
		if(i>=2){
			assert_int_equal(i+1, *pi);
		}
		(void)ub_esarray_del_ele(eah, (ub_esarray_element_t *)pi);
	}

	assert_int_equal(0, ub_esarray_ele_nums(eah));

	ub_esarray_close(eah);
}

struct test_data {
	char x[7];
	int a;
	char b;
	uint16_t c;
};
static void test_esarray2(void **state)
{
	int i;
	ub_esarray_cstd_t *eah;
	struct test_data d0={.x={'a','b','c',0}, .a=10, .b=20, .c=30};
	struct test_data d1={.x={'a','b','c',0}, .a=10, .b=20, .c=30};
	eah = ub_esarray_init(8,sizeof(struct test_data),32);
	for(i=0;i<17;i++){
		d1.a+=1;
		(void)ub_esarray_add_ele(eah, (ub_esarray_element_t *)&d1);
	}
	(void)ub_esarray_data_lock(eah);
	assert_int_equal(-1, ub_esarray_pop_ele(eah, (ub_esarray_element_t *)&d1));
	(void)ub_esarray_data_unlock(eah);
	assert_int_equal(0, ub_esarray_pop_ele(eah, (ub_esarray_element_t *)&d1));
	assert_string_equal(d0.x, d1.x);
	assert_int_equal(d0.a+1, d1.a);
	assert_int_equal(d0.b, d1.b);
	assert_int_equal(d0.c, d1.c);
	assert_int_equal(0, ub_esarray_pop_last_ele(eah, (ub_esarray_element_t *)&d1));
	assert_string_equal(d0.x, d1.x);
	assert_int_equal(d0.a+17, d1.a);
	assert_int_equal(d0.b, d1.b);
	assert_int_equal(d0.c, d1.c);

	// remove all
	ub_esarray_close(eah);
	eah = ub_esarray_init(8,sizeof(struct test_data),32);
	(void)ub_esarray_data_lock(eah);
	for(i=0;i<8;i++){
		// the first 8 should be okay
		assert_int_equal(0, ub_esarray_add_ele(eah, (ub_esarray_element_t *)&d0));
	}
	// data is locked, and can't reallocate the area
	assert_int_equal(-1, ub_esarray_add_ele(eah, (ub_esarray_element_t *)&d0));

	// can't pop in the locked status
	assert_int_equal(-1, ub_esarray_pop_ele(eah, (ub_esarray_element_t *)&d1));

	// pop one, then one can be added
	(void)ub_esarray_data_unlock(eah);
	assert_int_equal(0, ub_esarray_pop_ele(eah, (ub_esarray_element_t *)&d1));
	(void)ub_esarray_data_lock(eah);
	assert_int_equal(0, ub_esarray_add_ele(eah, (ub_esarray_element_t *)&d0));

	// the next one again fails
	assert_int_equal(-1, ub_esarray_add_ele(eah, (ub_esarray_element_t *)&d0));
	(void)ub_esarray_data_unlock(eah);
	// after unlock, it should be okay
	assert_int_equal(0, ub_esarray_add_ele(eah, (ub_esarray_element_t *)&d0));

	ub_esarray_close(eah);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_esarray1),
		cmocka_unit_test(test_esarray2),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
