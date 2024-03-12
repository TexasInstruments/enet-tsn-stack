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
#include <stdlib.h>
#include <getopt.h>
#include "avbtp.h"
#include <tsn_unibase/unibase_binding.h>

#define DEFAULT_NET_DEVICE "eth0"
#define DEFAULT_TIME 1000 //msec
#define MAX_NUM_SIDS 10

typedef struct stidf_finder_data {
	int time;
	ub_macaddr_t mcastmac;
	char *netdev;
	bool tagged;
	bool promisc;
	ub_streamid_t stream_ids[MAX_NUM_SIDS];
	int nnum_sids;
}stidf_finder_data_t;

static int print_usage(char *pname)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	printf("%s [options]\n", s);
	printf("-h|--help: this help\n");
	printf("-d|--netdev device_name: network device, default=%s\n", DEFAULT_NET_DEVICE);
	printf("-m|--mcastmac mcast_mac: filter by multicast mac address\n");
	printf("-t|--time time_in_msec: time duration to search, default=%dmsec\n", DEFAULT_TIME);
	printf("--tagged: use taggaed vlan data\n");
	printf("-p|--promisc: use promiscuous mode\n");
	return -1;
}


static int set_options(stidf_finder_data_t *stidf, int argc, char *argv[])
{
	int oc;
	int res;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"netdev", required_argument, 0, 'd'},
		{"mcast", required_argument, 0, 'm'},
		{"time", required_argument, 0, 't'},
		{"tagged", no_argument, 0, 1},
		{"promisc", no_argument, 0, 'p'},
		{0, 0, 0, 0}
	};

	while((oc=getopt_long(argc, argv, "hd:m:t:p", long_options, NULL))!=-1){
		switch(oc){
		case 'd':
			stidf->netdev=optarg;
			break;
		case 'm':
			ub_smac2bmac(optarg, stidf->mcastmac);
			break;
		case 't':
			stidf->time=strtol(optarg, NULL, 0);
			break;
		case 1:
			stidf->tagged=true;
			break;
		case 'p':
			stidf->promisc=true;
			break;
		case 'h':
		default:
			print_usage(argv[0]);
			exit(0);
		}
	}

	res=optind;
	optind=0;
	return res;
}

static int check_sid_exist(stidf_finder_data_t *stidf, ub_streamid_t sid)
{
	int i;
	for(i=0;i<stidf->nnum_sids;i++){
		if(!memcmp(stidf->stream_ids[i], sid, ETH_ALEN)){return 0;}
	}
	return 1;
}

static int stidf_rcv_cb(uint8_t *data, int size, struct ethhdr *ethdr, void *cbdata, uint16_t pcv)
{
	stidf_finder_data_t *stidf=(stidf_finder_data_t *)cbdata;
	avbtp_cm_stream_header_t *cmsh;

	if(UB_NON_ZERO_B6(stidf->mcastmac) && memcmp(stidf->mcastmac, ethdr->h_dest, ETH_ALEN)){return 0;}

	if(size < (int)sizeof(avbtp_cm_stream_header_t)){
		UB_LOG(UBL_ERROR,"%s: size = %d is too small\n", __func__, size);
		return -1;
	}

	cmsh=(avbtp_cm_stream_header_t *)data;
	if(!ACCEPT_AVTP_VERSION(cmsh_version_bit_field(cmsh->bf))){
		UB_LOG(UBL_INFO,"%s: not acceptable AVTP version:%d\n",__func__,
		       cmsh_version_bit_field(cmsh->bf));
		return -1;
	}
	if(get_header_type(cmsh->hh.subtype)!=AVBTP_STREAM_HEADER){return 0;}

	if(stidf->nnum_sids<MAX_NUM_SIDS){
		if(!check_sid_exist(stidf, cmsh->stream_id)){return 0;}
		memcpy(stidf->stream_ids[stidf->nnum_sids++], cmsh->stream_id,
		       sizeof(ub_streamid_t));
	}
	return 0;
}

int main(int argc, char *argv[])
{
	avtp_ethernet_handle_t reh;
	stidf_finder_data_t stidf;
	int i;
	avtp_eth_paras_t ethp;
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR(
		"4,ubase:45,cbase:45,uconf:46,gptp:46,l2:46", "UBL_L2");
	unibase_init(&init_para);
	memset(&stidf, 0, sizeof(stidf_finder_data_t));
	stidf.netdev=DEFAULT_NET_DEVICE;
	stidf.time=DEFAULT_TIME;
	set_options(&stidf, argc, argv);
	ethp.promisc=stidf.promisc;
	ethp.dev=stidf.netdev;
	ethp.mcastmac=stidf.mcastmac;
	ethp.rec_tagged=stidf.tagged;
	ethp.priority=2;
	reh=avtp_ethernet_open(&ethp, stidf_rcv_cb, &stidf);
	usleep(stidf.time*1000);
	avtp_ethernet_close(reh);
	for(i=0;i<stidf.nnum_sids;i++){
		printf(UB_PRIhexB8"\n",UB_ARRAY_B8(stidf.stream_ids[i]));
	}
	ubb_memory_out_close();
	unibase_close();
	return 0;
}
