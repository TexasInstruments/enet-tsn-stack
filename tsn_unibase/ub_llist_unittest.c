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

static bool test_comp(struct ub_list_node *node1, struct ub_list_node *node2,
		       void *arg)
{
	return *((int*)node1->data) <= *((int*)node2->data);
}

static bool test_revcomp(struct ub_list_node *node1, struct ub_list_node *node2,
		       void *arg)
{
	return *((int*)node1->data) >= *((int*)node2->data);
}

static bool test_apply(struct ub_list_node *node, void *arg)
{
	int i=*(int*)arg;
	if((i==2) && (*((int*)node->data)==50)){return -1;}
	if((i==3) && (*((int*)node->data)==102)){return -1;}
	*((int*)node->data)*=i;
	return 0;
}

static void test_node_clear(struct ub_list_node *node, void *arg)
{
	*(int*)arg+=1;
}

static void test_llist1(void **state)
{
	const int nnum=100;
	const int hnnum=100/2;
	struct ub_list_node node[100];
	struct ub_list_node *nodep;
	int ndata[100];
	struct ub_list list;
	int i,j,a;
	int dp1,dp2;

	ub_list_init(&list);
	for(i=0;i<nnum;i++) {
		ndata[i]=i;
		node[i].data=&ndata[i];
	}
	for(i=0;i<hnnum;i++){
		ub_list_append(&list, &node[i]);
	}
	for(i=0;i<hnnum;i++){
		assert_ptr_equal(node[i].data, &ndata[i]);
	}
	// delete the bottom
	ub_list_unlink(&list, &node[hnnum-1]);
	assert_ptr_equal(node[hnnum-2].next, NULL);
	// delete the top
	ub_list_unlink(&list, &node[0]);
	assert_ptr_equal(node[1].prev, NULL);
	// delete the middle
	ub_list_unlink(&list, &node[10]);
	assert_ptr_equal(node[9].next, &node[11]);
	assert_ptr_equal(node[11].prev, &node[9]);
	// now the list is 1...9,11...hnnum-2 --- hnnum-3 elements
	// add node[50] at the bottom
	ub_list_append(&list, &node[50]);
	// add node[51] at the top
	ub_list_prepend(&list, &node[51]);
	// check head and tail
	assert_ptr_equal(list.head, &node[51]);
	assert_ptr_equal(list.tail, &node[50]);
	// now the list is 51,1...9,11...hnnum-2,50 --- hnnum-1 elements
	// insert 52 before 9
	ub_list_insert_before(&list, &node[9], &node[52]);
	assert_ptr_equal(node[8].next, &node[52]);
	assert_ptr_equal(node[9].prev, &node[52]);
	// insert 53 after 11
	ub_list_insert_after(&list, &node[11], &node[53]);
	assert_ptr_equal(node[11].next, &node[53]);
	assert_ptr_equal(node[12].prev, &node[53]);
	// now the list is 51,1...8,52,9,11,53,12...hnnum-2,50 --- hnnum+1 elements
	a=2;
	(void)ub_list_apply(&list, true, test_apply, &a);
	// now the list is 102,2,4...16,104,18,22,106,24...(hnnum-2)*2,50
	a=3;
	(void)ub_list_apply(&list, false, test_apply, &a);
	// now the list is 102,6,12...48,312,54,66,318,72...(hnnum-2)*2*3,150
	assert_int_equal(*((int*)list.head->data), 102);
	assert_int_equal(*((int*)list.tail->data), 150);

	// sort it
	ub_list_sort(&list, test_comp, NULL);
	// now the list is 6,12...,54,66,...,96,102,102,108,...,150,150,156,...,(hnnum-2)*2*3,312,318
	// hnnum-1 + 2 elements
	dp1=0;
	dp2=0;
	nodep=list.head;
	j=1;
	for(i=0;i<(hnnum-1);i++){
		if((j*6)==60){
			j++;
		}
		assert_int_equal(*((int *)nodep->data), j*6);
		if((j*6)==102){
			if(dp1==0){j--;}
			dp1++;
		}
		if((j*6)==150){
			if(dp2==0){j--;}
			dp2++;
		}
		nodep=nodep->next;
		j++;
	}
	assert_int_equal(*((int *)nodep->data), 312);
	nodep=nodep->next;
	assert_int_equal(*((int *)nodep->data), 318);
	assert_ptr_equal(nodep=nodep->next, NULL);

	// rev. sort it
	ub_list_sort(&list, test_revcomp, NULL);
	dp1=0;
	dp2=0;
	nodep=list.head;
	assert_int_equal(*((int *)nodep->data), 318);
	nodep=nodep->next;
	assert_int_equal(*((int *)nodep->data), 312);
	nodep=nodep->next;
	j=hnnum-2;
	for(i=hnnum-2;i>=0;i--){
		if((j*6)==60){j--;}
		assert_int_equal(*((int *)nodep->data), j*6);
		if((j*6)==102){
			if(dp1==0){j++;}
			dp1++;
		}
		if((j*6)==150){
			if(dp2==0){j++;}
			dp2++;
		}
		nodep=nodep->next;
		j--;
	}

	assert_int_equal(ub_list_count(&list), hnnum+1);
	a=0;
	ub_list_clear(&list, test_node_clear, &a);
	assert_int_equal(a, hnnum+1);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_llist1),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
