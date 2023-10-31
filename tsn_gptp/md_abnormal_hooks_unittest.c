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
#include <tsn_unibase/unibase_binding.h>
#include <setjmp.h>
#include <cmocka.h>
#include "mdeth.h"
#include "md_abnormal_hooks.h"
#include "gptpnet.h"
#include <tsn_uniconf/ucman.h>
#include "gptpconf/gptpgcfg.h"

static gptpnet_data_t *gpnet;

static int gptpnet_cb(void *cb_data, int portIndex, gptpnet_event_t event,
		      int64_t *event_ts, void *event_data)
{
	return 0;
}

static void test_abnormal_events1(void **state)
{
	md_abn_event_t event1={domainNumber:0, ndevIndex:0, msgtype:SYNC,
			       eventtype:MD_ABN_EVENT_SKIP,
			       eventrate:1.0, repeat:0, interval:0, eventpara:0};
	int length=sizeof(MDPTPMsgSync);
	ClockIdentity clockid={0,};

	md_header_compose(0, gpnet, 1, SYNC, length, clockid, 1, 100, -3);
	// without md_abnormal_init, no event happens
	assert_int_equal(md_abnormal_register_event(&event1),-1);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);

	md_abnormal_init();
	assert_int_equal(md_abnormal_register_event(&event1),0);

	// repeat every time forever
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// repeat=1, interval=0 : repeat one time
	event1.repeat=1;
	event1.interval=0;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// repeat=1, interval=1 : repeat one time
	event1.repeat=1;
	event1.interval=1;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// repeat=2, interval=0 : repeat 2 times
	event1.repeat=2;
	event1.interval=0;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// repeat=2, interval=1 : repeat 2 times with interval 1
	event1.repeat=2;
	event1.interval=1;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// repeat=2, interval=2 : repeat 2 times with interval 2
	event1.repeat=2;
	event1.interval=2;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// repeat=3, interval=2 : repeat 3 times with interval 2
	event1.repeat=3;
	event1.interval=2;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_SKIP);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	md_abnormal_close();
}

static void test_abnormal_events2(void **state)
{
	md_abn_event_t event1={domainNumber:0, ndevIndex:0, msgtype:PDELAY_REQ,
			       eventtype:MD_ABN_EVENT_DUP,
			       eventrate:1.0, repeat:0, interval:0, eventpara:0};
	int length=sizeof(MDPTPMsgPdelayReq);
	ClockIdentity clockid={0,};
	int i, count;

	md_header_compose(0, gpnet, 1, SYNC, length, clockid, 1, 100, -3);
	md_abnormal_init();

	// PDELAY_REQ != SYNC, the event doesn't happen
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	md_header_compose(0, gpnet, 1, PDELAY_REQ, length, clockid, 1, 100, -3);
	// repeat every time forever
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_DUPLICATE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_DUPLICATE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_DUPLICATE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// possibility=0.0, the event never happens
	event1.eventrate=0.0;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_gptpnet_send_hook(gpnet, 0, length), MD_ABN_EVENTP_NONE);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	// possibility=0.5, the event happens 50%
	event1.eventrate=0.5;
	assert_int_equal(md_abnormal_register_event(&event1),0);
	for(i=0,count=0;i<1000;i++){
		if(md_abnormal_gptpnet_send_hook(gpnet, 0, length) ==
		   MD_ABN_EVENTP_DUPLICATE){count++;}
	}
	assert_in_range(count, 400, 600);
	assert_int_equal(md_abnormal_deregister_all_events(),0);

	md_abnormal_close();
}

static CB_THREAD_T ucthreadt;
static const char *dbname=".ix_gptpclock_unittestdb";
static bool stopuc;
static ucman_data_t ucmd;

static int setup(void **state)
{
	unibase_init_para_t init_para;
	const char *netdevs[2]={"cbeth0",NULL};
	int np=1;

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
	gpnet=gptpnet_init(0, gptpnet_cb, NULL, netdevs, np, NULL);

	return 0;
}

static int teardown(void **state)
{
	gptpnet_close(gpnet);
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
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_abnormal_events1),
		cmocka_unit_test(test_abnormal_events2),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
