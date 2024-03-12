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
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <tsn_combase/cb_thread.h>
#include <tsn_combase/cb_ethernet.h>
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include "lldpd.h"
#include "lldp_cfg.h"

// Global variables
char* lldp_db_name = "cfg/lldp_db";
int vlanid = -1;
uint8_t terminated = 0;

/* Signal Handler for SIGINT */

/// @brief Handle termination signal
/// @param sig_num Termination signal
void handle_signal(int sig_num)
{
	terminated = 1;
}

/// @brief Print this usage
/// @param pname  program name
/// @return 
static int print_usage(char *pname)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-p|--dbname: filename\n");
	UB_CONSOLE_PRINT("-v|--vid: Input vlan id\n");

	return 0;
}

/// @brief Set option to lldpd
/// @param argc 
/// @param argv 
/// @return 
static int set_options(int argc, char *argv[])
{
	int oc;
	int res=0;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		// reading cmd
		{"dbname", required_argument, 0, 'p'},
		{"vid", required_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	while((oc=getopt_long(argc, argv, "hp:v:", long_options, NULL))!=-1){
		switch(oc){
		case 'h':
			print_usage(argv[0]);
			res=1;
			break;
		case 'p':
			lldp_db_name = optarg;
			UB_TLOG(UBL_INFO, "lldp db: %s\n", lldp_db_name);
			res=0;
			break;
		case 'v':
			{
				vlanid = (int)strtol(optarg, NULL, 0);
				UB_TLOG(UBL_INFO, "TODO: vid %d\n", vlanid);
				res = 0;
			}
			break;
		default:
			return -1;
		}
	}
	return res;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	unibase_init_para_t init_para;
	/* initialized excelfore's unibase library */
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:66,ucclt:66,lldp:46", "UBL_LLDP");
	unibase_init(&init_para);

	netdevname_t ndevs[8];

	UB_TLOG(UBL_INFO, "UBL_LLDP Starting...\n");
	if (set_options(argc, argv) == 0)
	{
		lldpd_uniconf_access_mode(0); // Posix: Process Mode

		int ndev_no = cb_get_all_netdevs(8, ndevs);

		if (lldpd_init(lldp_db_name, &vlanid, ndevs, ndev_no) == 0)
		{
			UB_TLOG(UBL_INFO, "lldp initialized. ndev_no %d\n", ndev_no);
			lldpd_run(&terminated); // Blocking task

			lldpd_deinit();
		}
		else
		{
			UB_TLOG(UBL_ERROR, "lldp initialized failure\n");
		}
	}

	return 0;
}
