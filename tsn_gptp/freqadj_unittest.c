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
#include <stdlib.h>
#include <tsn_unibase/unibase_binding.h>
#include <setjmp.h>
#include <cmocka.h>
#include "gptpman_private.h"
#include "gptpclock.h"
#include "gptpconf/gptpgcfg.h"
#include <tsn_uniconf/ucman.h>
#include "gptpcommon.h"

UB_SD_GETMEM_DEF(GPTP_SMALL_ALLOC, GPTP_SMALL_AFSIZE, GPTP_SMALL_AFNUM);
UB_SD_GETMEM_DEF(GPTP_MEDIUM_ALLOC, GPTP_MEDIUM_AFSIZE, GPTP_MEDIUM_AFNUM);

static void get_tsrp(int ptpfd, uint64_t *tsr, uint64_t *tsp)
{
	*tsr=ub_mt_gettime64();
	GPTP_CLOCK_GETTIME(ptpfd, *tsp)
}

static void test_one_adjust(int ptpfd, int adjppb)
{
	uint64_t tsr1, tsp1=0, tsr2, tsp2=0;
	int64_t dr,dp;
	int64_t rppm, dppb;
	gptp_clock_adjtime(ptpfd, adjppb);
	get_tsrp(ptpfd, &tsr1, &tsp1);
	sleep(1);
	get_tsrp(ptpfd, &tsr2, &tsp2);
	dr=tsr2-tsr1;
	dp=tsp2-tsp1;
	rppm=(dp-dr)*1000000/dr;
	printf("adj=%dppm, RC:%"PRIi64", PC:%"PRIi64", rate=%"PRIi64"ppm\n",
	       adjppb/1000, dr, dp, rppm);
	dppb=(adjppb==0)?2000:adjppb/25;
	assert_true(labs(rppm) <= labs((adjppb+dppb)/1000));
	if(adjppb){assert_true(labs(rppm) >= labs((adjppb-dppb)/1000));}
}

static void test_two_adjust(int ptpfd1, int ptpfd2, int adjppba, int adjppbb)
{
	uint64_t tsr1, tspa1=0, tsr2, tspa2=0, tspb1, tspb2;
	int64_t dr, dpa, dpb;
	int64_t rppma, dppba, rppmb, dppbb;
	gptp_clock_adjtime(ptpfd1, adjppba);
	gptp_clock_adjtime(ptpfd2, adjppbb);
	get_tsrp(ptpfd1, &tsr1, &tspa1);
	GPTP_CLOCK_GETTIME(ptpfd2, tspb1);
	sleep(1);
	get_tsrp(ptpfd1, &tsr2, &tspa2);
	GPTP_CLOCK_GETTIME(ptpfd2, tspb2);
	dr=tsr2-tsr1;
	dpa=tspa2-tspa1;
	dpb=tspb2-tspb1;
	rppma=(dpa-dr)*1000000/dr;
	rppmb=(dpb-dr)*1000000/dr;
	printf("adja=%dppm, RC:%"PRIi64", PC:%"PRIi64", ratea=%"PRIi64"ppm\n",
	       adjppba/1000, dr, dpa, rppma);
	printf("adjb=%dppm, RC:%"PRIi64", PC:%"PRIi64", rateb=%"PRIi64"ppm\n",
	       adjppbb/1000, dr, dpb, rppmb);
	dppba=(adjppba==0)?2000:adjppba/25;
	dppbb=(adjppbb==0)?2000:adjppbb/25;
	assert_true(labs(rppma) <= labs((adjppba+dppba)/1000));
	if(adjppba){assert_true(labs(rppma) >= labs((adjppba-dppba)/1000));}
	assert_true(labs(rppmb) <= labs((adjppbb+dppbb)/1000));
	if(adjppbb){assert_true(labs(rppmb) >= labs((adjppbb-dppbb)/1000));}
}

static void test_adjustment(void **state)
{
	int ptpfd;
	char *ptpdev=CB_VIRTUAL_PTPDEV_PREFIX"w0";
	ptpclock_state_t pds;

	pds=gptp_get_ptpfd(0, ptpdev, &ptpfd);
	assert_int_equal(pds, PTPCLOCK_RDWR);
	test_one_adjust(ptpfd, 0);
	test_one_adjust(ptpfd, 100000);
	test_one_adjust(ptpfd, -100000);
	test_one_adjust(ptpfd, 1000000);
	test_one_adjust(ptpfd, -1000000);
	test_one_adjust(ptpfd, 10000000);
	test_one_adjust(ptpfd, -10000000);
}

static void test_multi(void **state)
{
	int ptpfd1, ptpfd2;
	char *ptpdev1=CB_VIRTUAL_PTPDEV_PREFIX"w0";
	char *ptpdev2=CB_VIRTUAL_PTPDEV_PREFIX"1";
	ptpclock_state_t pds;
	pds=gptp_get_ptpfd(0, ptpdev1, &ptpfd1);
	assert_int_equal(pds, PTPCLOCK_RDWR);
	pds=gptp_get_ptpfd(0,ptpdev2, &ptpfd2);
	assert_int_equal(pds, PTPCLOCK_RDONLY);
	test_two_adjust(ptpfd1, ptpfd2, 100000, 0);
	test_two_adjust(ptpfd1, ptpfd2, 100000, 0);
	test_two_adjust(ptpfd1, ptpfd2, -100000, 0);
}

static CB_THREAD_T ucthreadt;
static const char *dbname=".freqqdj_unittestdb";
static bool stopuc;
static ucman_data_t ucmd;

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46,gptp:46",
						   "UBL_GPTP");
	unibase_init(&init_para);
	uniconf_remove_dbfile(dbname);
	ucmd.stoprun=&stopuc;
	ucmd.dbname=dbname;
	ucmd.ucmode=UC_CALLMODE_UNICONF|UC_CALLMODE_THREAD;
	ucmd.ucmanstart=malloc(sizeof(CB_SEM_T));
	if(ub_assert_fatal(ucmd.ucmanstart!=NULL, __func__, NULL)){return -1;}
	memset(ucmd.ucmanstart, 0, sizeof(CB_SEM_T));
	ucmd.hwmod="NONE";
	CB_SEM_INIT(ucmd.ucmanstart, 0, 0);
	CB_THREAD_CREATE(&ucthreadt, NULL, uniconf_main, &ucmd);
	CB_SEM_WAIT(ucmd.ucmanstart);
	if(gptpgcfg_init(dbname, NULL, 0, true, NULL)) return -1;

	return 0;
}

static int teardown(void **state)
{
	stopuc=true;
	CB_THREAD_JOIN(ucthreadt, NULL);
	CB_SEM_DESTROY(ucmd.ucmanstart);
	free(ucmd.ucmanstart);
	uniconf_remove_dbfile(dbname);
	unibase_close();
	return 0;
}

int main(int argc, char *argv[])
{
	if(getenv("SKIP_TIME_CRITICAL")){return 77;}
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_adjustment),
		cmocka_unit_test(test_multi),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
