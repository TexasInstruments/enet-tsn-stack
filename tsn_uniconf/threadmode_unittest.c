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
#include "yangs/yang_modules.h"
#include "yangs/ietf-interfaces_access.h"
#define UCTEST_DBNAME ".uniconftestdb"
#define TEST_DEVICE "cbeth0"
#define NOTICE_SEMNAME "test_sem"

static int wait_semaphore(UC_NOTICE_SIG_T *sem, int tout_ms, const char *fname)
{
	struct timespec ts;
	uint64_t ts64;
	ts64=ub_rt_gettime64()+(uint64_t)tout_ms*UB_MSEC_NS;
	UB_NSEC2TS(ts64, ts);
	if(CB_SEM_TIMEDWAIT(sem, &ts)){
		UB_LOG(UBL_ERROR, "%s:timed out\n", fname);
		return -1;
	}
	return 0;
}

static void *db_one_write(void *ptr)
{
	uint32_t oper_status=1;
	UB_LOG(UBL_INFO, "%s:write OPER_STATUS in a thread\n", __func__);
	YDBI_SET_ITEM(ifk1vk0, TEST_DEVICE, IETF_INTERFACES_OPER_STATUS,
		      YDBI_STATUS, &oper_status, sizeof(oper_status),
		      YDBI_PUSH_NOTICE);
	return NULL;
}

/*
 * in this test, use 'yang_db_action' for readint/writing the DB
 * and use a notice supported by uc_notice
 */
static void test_db_access1(void **state)
{
	int res;
	CB_THREAD_T cthread;
	yang_db_item_access_t *ydbia;
	uint8_t aps[5]={IETF_INTERFACES_RO, IETF_INTERFACES_INTERFACES,
			IETF_INTERFACES_INTERFACE, IETF_INTERFACES_OPER_STATUS, 255};
	void *kvs[3]={(void*)TEST_DEVICE, (void*)NOTICE_SEMNAME, NULL};
	uint8_t kss[2]={strlen(TEST_DEVICE)+1, strlen(NOTICE_SEMNAME)+1};
	UC_NOTICE_SIG_T *sem;
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
		NULL,aps,kvs,kss,NULL,0};
	ydbia=ydbi_access_handle();
	res=uniconf_ready(UCTEST_DBNAME, UC_CALLMODE_THREAD, 100);
	assert_int_equal(res, 0);
	uc_nc_notice_register(ydbia->ucntd, ydbia->dbald, aps, kvs, kss,
			      UC_NOTICE_DBVAL_ADD, &sem);

	CB_THREAD_CREATE(&cthread, NULL, db_one_write, NULL);
	// the child thread write OPER_STATUS, and pus notice
	// the notice should come to this 'sem'
	res=wait_semaphore(sem, 100, __func__);
	assert_int_equal(res, 0);
	// read the OPER_STATUS
	kvs[1]=NULL;
	res=yang_db_action(ydbia->dbald, NULL, &dbpara);
	assert_int_equal(res, 0);
	assert_int_equal(dbpara.vsize, 4);
	assert_int_equal(*((uint32_t*)dbpara.value), 1);

	dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	res=yang_db_action(ydbia->dbald, NULL, &dbpara);
	assert_int_equal(res, 0);

	CB_THREAD_JOIN(cthread, NULL);
	res=uc_nc_notice_deregister_all(ydbia->ucntd, ydbia->dbald, NOTICE_SEMNAME);
	assert_int_equal(res, 0);
}

/*
 * in this test, use 'uc_dbal_get' adn 'uc_dbal_put' for readint/writing the DB
 */
static void test_db_access2(void **state)
{
	int res;
	CB_THREAD_T cthread;
	yang_db_item_access_t *ydbia;
	uint8_t key[16]={IETF_INTERFACES_RO, IETF_INTERFACES_INTERFACES,
		IETF_INTERFACES_INTERFACE, IETF_INTERFACES_OPER_STATUS, 255,0};
	uint32_t ksize;
	uint32_t oper_status;
	void *value;
	uint32_t vsize;
	ydbia=ydbi_access_handle();
	res=uniconf_ready(UCTEST_DBNAME, UC_CALLMODE_THREAD, 100);
	assert_int_equal(res, 0);
	key[5]=strlen(TEST_DEVICE)+1;
	strcpy((char*)&key[6], TEST_DEVICE);
	ksize=6+key[5];
	oper_status=0;
	res=uc_dbal_create(ydbia->dbald, key, ksize,
			   &oper_status, sizeof(uint32_t));
	// read back the written data
	uc_dbal_get(ydbia->dbald, key, ksize, &value, &vsize);
	assert_int_equal(vsize, sizeof(uint32_t));
	assert_int_equal(*((uint32_t*)value), 0);
	uc_dbal_get_release(ydbia->dbald, key, ksize, value, vsize);
	// write in a child thread
	CB_THREAD_CREATE(&cthread, NULL, db_one_write, NULL);
	CB_THREAD_JOIN(cthread, NULL);
	// read the data written in the child thread
	uc_dbal_get(ydbia->dbald, key, ksize, &value, &vsize);
	assert_int_equal(vsize, sizeof(uint32_t));
	assert_int_equal(*((uint32_t*)value), 1);
	uc_dbal_get_release(ydbia->dbald, key, ksize, value, vsize);
}

/*
 * in this test, use YDBI macros and ydbi functions.
 */
static void test_db_access3(void **state)
{
	uint32_t oper_status=0;
	void *value;
	CB_THREAD_T cthread;
	YDBI_SET_ITEM(ifk1vk0, TEST_DEVICE, IETF_INTERFACES_OPER_STATUS,
		      YDBI_STATUS, &oper_status, sizeof(oper_status),
		      YDBI_NO_NOTICE);
	// read back the written data
	oper_status=UINT32_MAX;
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk1vk0, oper_status, value,
			     TEST_DEVICE, IETF_INTERFACES_OPER_STATUS,
			     YDBI_STATUS);
	assert_int_equal(oper_status, 0);
	// write in a child thread
	CB_THREAD_CREATE(&cthread, NULL, db_one_write, NULL);
	CB_THREAD_JOIN(cthread, NULL);
	// read the data written in the child thread
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk1vk0, oper_status, value,
			     TEST_DEVICE, IETF_INTERFACES_OPER_STATUS,
			     YDBI_STATUS);
	assert_int_equal(oper_status, 1);
}

static int setup(void **state)
{
	uniconf_uthp_data_t *utd;
	uc_dbald *dbald;
	xl4_data_data_t *xdd;
	uc_notice_data_t *ucntd;

	utd=malloc(sizeof(uniconf_uthp_data_t));
	memset(utd, 0, sizeof(uniconf_uthp_data_t));
	utd->thread_mode=true;
	utd->ucmd.dbname=UCTEST_DBNAME;
	utd->ucmd.ucmode=UC_CALLMODE_UNICONF|UC_CALLMODE_THREAD;
	utd->ucmd.ucmanstart=malloc(sizeof(CB_SEM_T));
	utd->ucmd.hwmod="";
	*state=utd;
	if(uniconf_unittest_setup(state)){return -1;}

	dbald=uc_dbal_open(UCTEST_DBNAME, "w", UC_CALLMODE_THREAD);
	xdd=xl4_data_init(dbald);
	ucntd=uc_notice_init(UC_CALLMODE_THREAD, UCTEST_DBNAME);
	ydbi_access_init(dbald, xdd, ucntd);
	return 0;
}

static int teardown(void **state)
{
	yang_db_item_access_t *ydbia;
	uniconf_uthp_data_t *utd;

	ydbia=ydbi_access_handle();
	if(ydbia){
		uc_notice_close(ydbia->ucntd, UC_CALLMODE_THREAD);
		xl4_data_close(ydbia->xdd);
		uc_dbal_close(ydbia->dbald, UC_CALLMODE_THREAD);
	}
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
		cmocka_unit_test(test_db_access1),
		cmocka_unit_test(test_db_access2),
		cmocka_unit_test(test_db_access3),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
