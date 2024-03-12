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
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <getopt.h>
#include "gptpconf/gptpgcfg.h"
#define MAX_CONF_FILES 5
int main(int argc, char* argv[])
{
	uint8_t sginst=0, sdomain=0;
	uint8_t dginst=1, ddomain=0;
	const char *dbname=NULL;
	int opt;
	int res;
	int numconf=0;
	const char *conf_files[MAX_CONF_FILES+1]={NULL};

	while ((opt = getopt(argc, argv, "hc:p:s:t:d:e:")) != -1) {
	        switch (opt) {
		case 'c':
			if(numconf>=MAX_CONF_FILES){break;}
			conf_files[numconf++]=optarg;
			conf_files[numconf]=NULL;
			break;
	        case 'p':
			dbname=optarg;
			break;
	        case 's':
			sginst=strtol(optarg, NULL, 0);
			break;
	        case 't':
			sdomain=strtol(optarg, NULL, 0);
			break;
	        case 'd':
			dginst=strtol(optarg, NULL, 0);
			break;
	        case 'e':
			ddomain=strtol(optarg, NULL, 0);
			break;
		default:
			printf("gptpinst_copy -p dbname "
			       "-c config_file -s source_instance -t source_domain "
			       "-d dest_instance -e dest_domain\n");
			return 1;
	        }
	}
	if(!dbname){return -1;}
	ubb_unibase_easyinit();
	if(gptpgcfg_init(dbname, conf_files, 0, 0, NULL)){
		UB_LOG(UBL_ERROR, "gptpinst_copy:error in gptpgcfg_init\n");
		return -1;
	}
	res=gptpcfg_copy_instance(sginst, sdomain, dginst, ddomain);
	gptpgcfg_close(0);
	unibase_close();
	return res;
}
