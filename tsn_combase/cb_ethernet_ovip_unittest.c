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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define UNIT_TESTING
#include <cmocka.h>
#include "combase.h"
#include <tsn_unibase/unibase_binding.h>

#define UB_LOGCAT 2

static CB_SOCKET_T open_ovlpip_socket(uint16_t lport, uint16_t dport, char *devsuffix)
{
	cb_rawsock_ovip_para_t ovipp;
	cb_rawsock_paras_t rsp;
	CB_SOCKET_T sfd;
	CB_SOCKADDR_LL_T addr;
	ub_macaddr_t bmac;
	ub_macaddr_t ebmac=CB_VIRTUAL_ETHDEV_MACU32;
	uint16_t ebl=0;
	netdevname_t netdevs[1];

	strcpy(netdevs[0], CB_VIRTUAL_ETHDEV_PREFIX);
	strcpy(netdevs[0]+strlen(CB_VIRTUAL_ETHDEV_PREFIX), devsuffix);
	memset(&rsp, 0, sizeof(rsp));
	ovipp.laddr=127<<24|0<<16|0<<8|1;
	ovipp.lport=lport;
	ovipp.daddr=127<<24|0<<16|0<<8|1;
	ovipp.dport=dport;

	rsp.dev=netdevs[0];
	rsp.proto=ETH_P_1588;
	rsp.priority=0;
	rsp.sock_mode=CB_SOCK_MODE_OVIP;
	rsp.ovipp=&ovipp;
	cb_rawsock_open(&rsp, &sfd, &addr, NULL, bmac);
	while(*devsuffix) ebl+=*devsuffix++;
	ebmac[4]=ebl>>8;
	ebmac[5]=ebl&0xff;
	assert_memory_equal(bmac, ebmac, 6);
	return sfd;
}

static int close_ovlpip_socket(CB_SOCKET_T sfd)
{
	return cb_rawsock_close(sfd);
}

#define TEST_PORT1 9454
#define TEST_PORT2 9455
static char *test_string1="cfo9(eb@jbf!t-+=@wi+x3pew7#e^xai*q6k_igi55t$%&1xwt";
static char *test_string2=")zj_(_brpo)kcb=&6r0@zpdrv42pe4#-754^s=-)jr#xm72u3_";
static void *test_ethovip_thread1(void *ptr)
{
	int rsize, ssize;
	CB_SOCKET_T sfd;
	uint8_t buf[1500];
	CB_SOCKADDR_IN_T saddr;
	socklen_t addrlen=sizeof(saddr);

	sfd=open_ovlpip_socket(TEST_PORT1, TEST_PORT2, "0");
	assert_int_not_equal(sfd, CB_SOCKET_INVALID_VALUE);

	rsize=recvfrom(sfd, buf, 1500, 0, (CB_SOCKADDR_T *)&saddr, &addrlen);
	assert_int_equal(rsize, strlen(test_string1));
	assert_memory_equal(buf, test_string1, rsize);


	ssize=sendto(sfd, test_string2, strlen(test_string2), 0, (CB_SOCKADDR_T *)&saddr, addrlen);
	assert_int_equal(ssize, strlen(test_string2));

	close_ovlpip_socket(sfd);
	return NULL;
}

static void *test_ethovip_thread2(void *ptr)
{
	int rsize, ssize;
	CB_SOCKET_T sfd;
	uint8_t buf[1500];

	sfd=open_ovlpip_socket(TEST_PORT2, TEST_PORT1, "1");
	assert_int_not_equal(sfd, CB_SOCKET_INVALID_VALUE);
	usleep(10000); // make sure the other thread to run

	ssize=write(sfd, test_string1, strlen(test_string1));
	assert_int_equal(ssize, strlen(test_string1));

	rsize=read(sfd, buf, 1500);
	assert_int_equal(rsize, strlen(test_string2));
	assert_memory_equal(buf, test_string2, rsize);

	close_ovlpip_socket(sfd);
	return NULL;
}

static void test_ethovip1(void **state)
{
	CB_THREAD_T th1, th2;
	CB_THREAD_CREATE(&th1, NULL, test_ethovip_thread1, NULL);
	CB_THREAD_CREATE(&th2, NULL, test_ethovip_thread2, NULL);

	CB_THREAD_JOIN(th1, NULL);
	CB_THREAD_JOIN(th2, NULL);
}

static void test_ethovip2(void **state)
{
	int sfd;
	netdevname_t netdev=CB_VIRTUAL_ETHDEV_PREFIX;
	netdevname_t netdevt;
	ptpdevname_t ptpdev=CB_VIRTUAL_PTPDEV_PREFIX;
	ptpdevname_t ptpdevt;
	CB_IN_ADDR_T inp;
	ub_macaddr_t bmac=CB_VIRTUAL_ETHDEV_MACU32;
	ub_macaddr_t bmact;

	strcat(netdev,"0");
	strcat(ptpdev,"0");
	sfd=open_ovlpip_socket(TEST_PORT1, TEST_PORT2, "0");
	assert_int_not_equal(sfd, CB_SOCKET_INVALID_VALUE);
	cb_get_ip_bydev(sfd, netdev, &inp);
	assert_int_equal(ntohl(inp.s_addr), 0x7f000001);
	cb_get_brdip_bydev(sfd, netdev, &inp);
	assert_int_equal(ntohl(inp.s_addr), 0x7fffffff);
	cb_get_ptpdev_from_netdev(netdev, ptpdevt);
	assert_int_equal(strcmp(ptpdev, ptpdevt), 0);
	cb_get_netdev_from_ptpdev(ptpdev, netdevt);
	assert_int_equal(strcmp(netdev, netdevt), 0);
	bmac[4]=0;
	bmac[5]='0';
	cb_get_mac_bydev(sfd, netdev, bmact);
	assert_memory_equal(bmac, bmact, 6);

	close_ovlpip_socket(sfd);
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr="4,ubase:45,combase:45,ovpi:55";
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
		cmocka_unit_test(test_ethovip1),
		cmocka_unit_test(test_ethovip2),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
