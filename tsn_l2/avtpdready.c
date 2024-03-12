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
#include <tsn_combase/cb_tmevent.h>
#include <getopt.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include "l2conf/avtpgcfg.h"
#include "l2conf/xl4-extmod-xl4avtp.h"

int main(int argc, char* argv[])
{
	int instance=0, tout_ms=0;
	avtpgcfgd_t *avtpgcd;
	uint8_t avtpready;
	const char *dbname=NULL;
	int opt;
	void *value;
	int res;
	int64_t ts;

	while ((opt = getopt(argc, argv, "hp:i:w:")) != -1) {
	        switch (opt) {
	        case 'p':
			dbname=optarg;
			break;
	        case 'i':
			instance=strtol(optarg, NULL, 0);
			break;
	        case 'w':
			tout_ms=strtol(optarg, NULL, 0);
			break;
		default:
			printf("avbtpready -p dbname -i instance -w tout_ms\n");
			printf("    return 0:ready, 1:timed out, -1:error\n");
			return 1;
	        }
	}
	if(!dbname){return -1;}
	ubb_unibase_easyinit();
	avtpgcd=avtpgcfg_init(0, dbname, NULL, 0);
	if(!avtpgcd){return -1;}
	avtpready=0xff;
	ts=ub_rt_gettime64();
	while(true){
		YDBI_GET_ITEM_VSUBST(uint8_t*, nyavtpk2vk1, avtpready, value, instance,
				     XL4_EXTMOD_XL4AVTP_AVTPD_READY, 255, NULL, 0, YDBI_STATUS);
		if(avtpready==1){
			res=0;
			break;
		}
		tout_ms-=10;
		if(tout_ms<0){
			res=1;
			break;
		}
		avtpgcfg_releasedb(avtpgcd);
		CB_USLEEP(10000);
	}
	ts=(ub_rt_gettime64()-ts)/UB_MSEC_NS;
	if(res==0){
		UB_LOG(UBL_DEBUG, "avtpdready:done in %dmsec\n", (int)ts);
	}else{
		UB_LOG(UBL_DEBUG, "avtpdready:timed out, %dmsec\n", (int)ts);
	}
	avtpgcfg_close(avtpgcd);
	unibase_close();
	return res;
}
