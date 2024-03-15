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
/*
 * avtp_testappli.c
 * test program to use l2 layer
 *
 * avtp_testclient.c
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "avtpc.h"
#include <tsn_unibase/unibase_binding.h>
#include <tsn_unibase/unibase.h>
#include <tsn_combase/cb_rate_reporter.h>
#include "tsn_gptp/gptpmasterclock.h"
#ifdef AVTP_PLATFORM_INCLUDE
#include AVTP_PLATFORM_INCLUDE
#endif
#ifndef WAIT_AVTPD_READY
#define WAIT_AVTPD_READY
#endif

#define DEFAULT_MCAST_ADDR "91:E0:F0:00:FE:00"
#define DEFAULT_JOIN_ADDR "91:E0:F0:00:FE:00"

#ifndef CONSOLE_RFD
#define CONSOLE_RFD 0
#define CONSOLE_READ(buf,len) read(CONSOLE_RFD, buf, len)
#endif

#define DECIMALMB 1000000.0
#define MEGANUM DECIMALMB

#define REPORT_HEADER_LEN 			(80)

/* Interval for report checking */
#define REPORT_CHECK_INTERVAL_MS	(10)

typedef enum {
	TALKER_MODE=0,
	LISTENER_MODE,
} mode_type;

typedef enum {
	FIXED_DATA=0,
	AVB_TEST_VIDEO,
} data_type;

typedef struct avtptc_data {
	avtpc_data_t *avtpc;
	mode_type mode;
	data_type dtype;
	char *netdev;
	uint16_t sidl2;
	uint16_t vid;
	bool promisc;
	uint16_t max_frame_size;
	uint16_t max_intv_frames;
	uint16_t time_intv;
	int lsem_post_time;
	uint16_t priority;
	double mbps;
	uint8_t data[1500];
#ifndef HAVE_NO_POSIX_CLOCK
	clock_t ct0;
	clock_t ct1;
#endif //HAVE_NO_POSIX_CLOCK
	bool rec_tagged;
	bool no_avtpd;
	char *mcast;
	char *mjoin;
	char *sid;
	bool nodc;
	bool prts;
	bool start_menu;
	int mpsize;
	int ssize;
	int tshape;
	int tsminsl;
	int subtype;
	int quitn;
	char *shsuf;
	char *gshmem;
	int send_interval;
	int tsoffset; //usec unit
	int send_ahead_ts;
	uint64_t recv_tv1;
	uint64_t recv_last_ts;
	int addber;
	int runber;
	int report_interval;           // report interval (in seconds)
	int wthres_latency;            // warning threshold for maximum latency (us unit)
	uint64_t total_err_packets;
	uint64_t total_rx_packets;
	uint32_t report_counter;       // number of shown report
	uint64_t high_latency_count;   // counter for latency values that higher than threshold
	uint64_t total_lost_packets;   // number of lost packets on runtime
	int test_duration;			   // the test will be run in `test_duration` seconds
	int avtpd_buftime; //in usec

	cb_rate_reporter_t rate_reporter;
	bool no_healthcheck; //no health check between the avtpd and avtpc
	bool avtpc_expandts32; //avtpc expands ts32 to ts64, the avtpd will not do
	bool filter_destmac;
} avtptc_data_t;

/* when we run multiple 'avtp_testclient' in threads,
 * because 'avtp_running' is a static variable, calling stop_testclient stop all of
 * them.  It is likely an intended operatin, should be okay.
 */
static bool avtp_running;
static void show_report_header_footer(avtptc_data_t *avtptcd, bool is_header);

static bool durnation_ended(uint64_t start_time, int test_duration);

static int print_usage(char *pname, avtptc_data_t *avtptcd)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-m|--mode: l(listener)|t(talker): default=talker\n");
	UB_CONSOLE_PRINT("-d|--netdev: network_device: defaut=%s\n", avtptcd->netdev);
	UB_CONSOLE_PRINT("-v|--vid: vlan id: defaut=%d\n", avtptcd->vid);
	UB_CONSOLE_PRINT("-s|--mfsize: max frame size(without L2 header): defaut=%d\n",
			 avtptcd->max_frame_size);
	UB_CONSOLE_PRINT("-f|--miframes: max number of frames: defaut=%d\n",
			 avtptcd->max_intv_frames);
	UB_CONSOLE_PRINT("-t|--interval: interval time: defaut=%d\n", avtptcd->time_intv);
	UB_CONSOLE_PRINT("-e|--lsempost: listener semaphore post time: defaut=%d\n",
			 avtptcd->lsem_post_time);
	UB_CONSOLE_PRINT("-r|--priority: priority: default=%d\n", avtptcd->priority);
	UB_CONSOLE_PRINT("-b|--mbps: bit rate: default(Mbps)=%f\n", avtptcd->mbps);
	UB_CONSOLE_PRINT("-p|--promisc: use promiscuous mode, default=%d\n", avtptcd->promisc);
	UB_CONSOLE_PRINT("-g|--rtag: receive tagged data from socket\n");
	UB_CONSOLE_PRINT("-n|--noavtpd: [BOOL]: optional_argument, '-nfalse' run avtpd "
			 "internally, default=%s\n", avtptcd->no_avtpd?"true":"false");
	UB_CONSOLE_PRINT("-q|--quitn: N(number of packets): quit after handling N packets\n");
	UB_CONSOLE_PRINT("-A|--mcast: multicast address: default=%s\n", DEFAULT_MCAST_ADDR);
	UB_CONSOLE_PRINT("-S|--sid: stream id: default=00:01:02:03:04:05:00:00\n");
	UB_CONSOLE_PRINT("-N|--nodc: don't check data contents\n");
	UB_CONSOLE_PRINT("-P|--prts: print timestamps\n");
	UB_CONSOLE_PRINT("-T|--tshape: bytes_per_msec: set software traffic shaper\n");
	UB_CONSOLE_PRINT("-M|--tsminsl: minimum_sleep_time: set minimum sleep time(usec unit)"
			 " in the shaper\n");
	UB_CONSOLE_PRINT("-E|--subtype: subtype: set 1722 subtype, default=CVF(3)\n");
	UB_CONSOLE_PRINT("-H|--shsuf: suffix: shared memory suffix\n");
	UB_CONSOLE_PRINT("-G|--gshmem: shared node: gptp shared memory node\n");
	UB_CONSOLE_PRINT("-D|--data: type of test data: v(avbvideo)|f(fixed): default=fixed\n");
	UB_CONSOLE_PRINT("-I|--interval: sending interval(usec): defaut=%d\n",
			 avtptcd->send_interval);
	UB_CONSOLE_PRINT("-o|--tsoffset: usec: this time is added on talker pts, default=%dusec\n",
			 avtptcd->tsoffset);
	UB_CONSOLE_PRINT("-w|--aheadts: usec: send this time ahead to timestamp in avtpd, "
			 "default=%dusec\n", avtptcd->send_ahead_ts);
	UB_CONSOLE_PRINT("           setting -1, no timing control by timestamp in the talker\n");
	UB_CONSOLE_PRINT("-x|--addber: error bit number: add bit error in a second, default=%d\n",
			 avtptcd->addber);
	UB_CONSOLE_PRINT("-y|--report-interval: seconds: interval for reporting information (bitrate, latency, ...), default=%d\n",
			 avtptcd->report_interval);
	UB_CONSOLE_PRINT("-Z|--wthres-latency: usec: warning threshold for maximum latency, default=%d\n",
			 avtptcd->wthres_latency);
	UB_CONSOLE_PRINT("-B|--avtpd-buftime: usec: avtpd buffer time in usec, default=%d\n",
			 avtptcd->avtpd_buftime);
	UB_CONSOLE_PRINT("-a|--test-duration: sec: duration time of the testing will be run (unset for running forever) , default=%d\n",
			 avtptcd->test_duration);
	UB_CONSOLE_PRINT("-c|--no-healthchk: no health check between avtpd and avtpc\n");
	UB_CONSOLE_PRINT("-C|--avtpc-expandts: the avtpc expands ts32 to ts64\n");
	UB_CONSOLE_PRINT("-F|--filter-dstmac: enable avtpd to filter the destination mac\n");
	return -1;
}

static int set_options(avtptc_data_t *avtptcd, int argc, char *argv[])
{
	int oc;
	int res;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"mode", required_argument, 0, 'm'},
		{"netdev", required_argument, 0, 'd'},
		{"vid", required_argument, 0, 'v'},
		{"mfsize", required_argument, 0, 's'},
		{"miframes", required_argument, 0, 'f'},
		{"tintv", required_argument, 0, 't'},
		{"priority", required_argument, 0, 'r'},
		{"mbps", required_argument, 0, 'b'},
		{"promisc", no_argument, 0, 'p'},
		{"rtag", no_argument, 0, 'g'},
		{"noavtpd", optional_argument, 0, 'n'},
		{"mcast", required_argument, 0, 'A'},
		{"sid", required_argument, 0, 'S'},
		{"nodc", no_argument, 0, 'N'},
		{"prts", no_argument, 0, 'P'},
		{"lsempost", required_argument, 0, 'e'},
		{"tshape", required_argument, 0, 'T'},
		{"tsminsl", required_argument, 0, 'M'},
		{"subtype", required_argument, 0, 'E'},
		{"shsuf", required_argument, 0, 'H'},
		{"gshmem", required_argument, 0, 'G'},
		{"data", required_argument, 0, 'D'},
		{"quitn", required_argument, 0, 'q'},
		{"interval", required_argument, 0, 'I'},
		{"tsoffset", required_argument, 0, 'o'},
		{"aheadts", required_argument, 0, 'w'},
		{"addber", required_argument, 0, 'x'},
		{"report-interval", required_argument, 0, 'y'},
		{"wthres-latency", required_argument, 0, 'Z'},
		{"avtpd-buftime", required_argument, 0, 'B'},
		{"test-duration", required_argument, 0, 'a'},
		{"no-healthchk", no_argument, 0, 'c'},
		{"avtpc-expandts", no_argument, 0, 'C'},
		{"filter-dstmac", no_argument, 0, 'F'},
		{"noubinit", no_argument, 0, 'u'},
		{0, 0, 0, 0}
	};

	while((oc=getopt_long(argc, argv,
		"hm:d:v:s:f:t:r:b:pgn::e:q:A:S:NPT:M:E:H:G:D:a:I:o:w:x:y:Z:B:cCFu",
		long_options, NULL))!=-1){
		switch(oc){
		case 'm':
			if(optarg[0]=='l'){avtptcd->mode=LISTENER_MODE;}
			break;
		case 'd':
			avtptcd->netdev=optarg;
			break;
		case 'v':
			avtptcd->vid=strtol(optarg, NULL, 0);
			break;
		case 's':
			avtptcd->max_frame_size=strtol(optarg, NULL, 0);
			break;
		case 'f':
			avtptcd->max_intv_frames=strtol(optarg, NULL, 0);
			break;
		case 't':
			avtptcd->time_intv=strtol(optarg, NULL, 0);
			break;
		case 'p':
			avtptcd->promisc=true;
			break;
		case 'r':
			avtptcd->priority=strtol(optarg, NULL, 0);
			break;
		case 'b':
			avtptcd->mbps=strtof(optarg, NULL);
			break;
		case 'g':
			avtptcd->rec_tagged=true;
			break;
		case 'n':
			if (optarg && !strcmp(optarg, "false")){avtptcd->no_avtpd=false;}
			break;
		case 'e':
			avtptcd->lsem_post_time=strtol(optarg, NULL, 0);
			break;
		case 'q':
			avtptcd->quitn=strtol(optarg, NULL, 0);
			break;
		case 'A':
			avtptcd->mcast=optarg;
			avtptcd->mjoin=optarg;
			break;
		case 'S':
			avtptcd->sid=optarg;
			break;
		case 'N':
			avtptcd->nodc=true;
			break;
		case 'P':
			avtptcd->prts=true;
			break;
		case 'T':
			avtptcd->tshape=strtol(optarg, NULL, 0);
			break;
		case 'M':
			avtptcd->tsminsl=strtol(optarg, NULL, 0);
			break;
		case 'E':
			avtptcd->subtype=strtol(optarg, NULL, 0);
			break;
		case 'H':
			avtptcd->shsuf=optarg;
			break;
		case 'G':
			avtptcd->gshmem=optarg;
			break;
		case 'D':
			if(optarg[0]=='v'){avtptcd->dtype=AVB_TEST_VIDEO;}
			break;
		case 'I':
			avtptcd->send_interval=strtol(optarg, NULL, 0);
			break;
		case 'a':
			avtptcd->test_duration=strtol(optarg, NULL, 0);
			break;
		case 'o':
			avtptcd->tsoffset=strtol(optarg, NULL, 0);
			break;
		case 'w':
			avtptcd->send_ahead_ts=strtol(optarg, NULL, 0);
			if(avtptcd->send_ahead_ts!=-1){avtptcd->send_ahead_ts*=1000;}
			break;
		case 'x':
			avtptcd->addber=strtol(optarg, NULL, 0);
			break;
		case 'y':
			avtptcd->report_interval=strtol(optarg, NULL, 0);
			break;
		case 'Z':
			avtptcd->wthres_latency=strtol(optarg, NULL, 0);
			break;
		case 'B':
			avtptcd->avtpd_buftime=strtol(optarg, NULL, 0);
			break;
		case 'c':
			avtptcd->no_healthcheck=true;
			break;
		case 'C':
			avtptcd->avtpc_expandts32=true;
			break;
		case 'F':
			avtptcd->filter_destmac=true;
			break;
		case 'u':
			//noubinit must be the last one
			break;
		case 'h':
		default:
			return print_usage(argv[0], avtptcd);
		}
	}

	res=optind;
	optind=0;
	return res;
}


static int set_ccr(client_connect_request_t *ccr, avtptc_data_t *avtptcd)
{
	ub_streamid_t stream_id;
	char astr[32];
	if(avtptcd->mode==LISTENER_MODE){ccr->con_mode=AVTP_LISTENER_CONNECTION;}
	else{ccr->con_mode=AVTP_TALKER_CONNECTION;}
	strcpy(ccr->netdev, avtptcd->netdev);
	ccr->promisc=avtptcd->promisc;
	if(avtptcd->vid){ccr->tagged=true;}
	ccr->rec_tagged=avtptcd->rec_tagged;
	ccr->max_frame_size=avtptcd->max_frame_size;
	ccr->max_intv_frames=avtptcd->max_intv_frames;
	ccr->time_intv=avtptcd->time_intv;
	ccr->lsem_post_time=avtptcd->lsem_post_time;
	ccr->priority=avtptcd->priority;
	ccr->rank=1;
	ccr->avtpd_bufftime=avtptcd->avtpd_buftime;
	if(!avtptcd->sid){sprintf(astr, "00:01:02:03:04:05:%02X:%02X",
			avtptcd->sidl2>>8, avtptcd->sidl2&0xff);}
	else{strcpy(astr, avtptcd->sid);}
	ub_ssid2bsid(astr, stream_id);
	avbtp_set_stream_id(&ccr->ht.preset_cmsh, stream_id);
	ccr->ht.preset_cmsh.hh.subtype=avtptcd->subtype;
	ccr->ht.preset_cmsh.fsd2=htonl(
		(AVBTP_CVF_FORMAT_RFC<<24)|(AVBTP_CVF_FORMAT_SUBTYPE_MJPEG<<16));
	ccr->ht.preset_cmsh.hh.bf0 |= 0x01;
	set_header_fsd3((&ccr->ht.preset_cmsh), 0x1000);
	ccr->vlanid=avtptcd->vid;
	ub_smac2bmac(avtptcd->mcast, ccr->mcast_addr);
	ub_smac2bmac(avtptcd->mjoin, ccr->join_mcast);
	ccr->send_ahead_ts=1000000; // 1msec
	ccr->no_healthcheck=avtptcd->no_healthcheck;
	ccr->avtpc_expandts32=avtptcd->avtpc_expandts32;
	ccr->filter_destmac=avtptcd->filter_destmac;
	return 0;
}

static void avtp_brater_period_print(avtptc_data_t *avtptcd, bool talker, int pkt_size)
{
	if(avtptcd->rate_reporter.total_size == 0){return;}

	if(avtptcd->rate_reporter.end_ts - avtptcd->rate_reporter.last_ts
	   >= avtptcd->rate_reporter.period_ns){
		double rate, prate;
		uint64_t latency_avg;

		cb_rate_reporter_calc(&avtptcd->rate_reporter, &rate, &prate, &latency_avg);

		if(talker){
			UB_CONSOLE_PRINT("[TX %s] Rp#%d Mbps(period=%f, total=%f)\n",
							 avtptcd->sid, ++avtptcd->report_counter, prate, rate);
		}else{
			if(avtptcd->report_counter==0){
				UB_CONSOLE_PRINT("[RX] ReportNumber# Mbps(period, total) "
								 "DelayUsec(min, average, max) "
								 "Packets(Received, Error, Lost)---------\n");
			}
			UB_CONSOLE_PRINT("[RX %s] Rp#%d Mbps(%f, %f) Delay(%d, %d, %d) "
							 "Pkt(%"PRIu64", %"PRIu64", %"PRIu64")\n",
							 avtptcd->sid,
							 ++avtptcd->report_counter,
							 prate, rate,
							 (int)(avtptcd->rate_reporter.latency_min/1000),
							 (int)(latency_avg/1000),
							 (int)(avtptcd->rate_reporter.latency_max/1000),
							 avtptcd->total_rx_packets,
							 avtptcd->total_err_packets,
							 avtptcd->total_lost_packets);
		}
	}
}

static void avtp_show_final_report(avtptc_data_t *avtptcd, bool talker, int pkt_size)
{
	if(avtptcd->rate_reporter.total_size == 0){return;}

	if (avtptcd->rate_reporter.last_ts > avtptcd->rate_reporter.start_ts){
		double rate, prate;
		uint64_t latency_avg;

		cb_rate_reporter_calc(&avtptcd->rate_reporter, &rate, &prate, &latency_avg);

		show_report_header_footer(avtptcd, true);
		if(talker){
			UB_CONSOLE_PRINT("+ talker: total rate %f Mbps\n", rate);
		}else{
			UB_CONSOLE_PRINT("+ listener: total rate %f Mbps\n", rate);
			UB_CONSOLE_PRINT("+ total received packets: %"PRIu64"\n",
							 (uint64_t)(avtptcd->rate_reporter.total_size/pkt_size));
			UB_CONSOLE_PRINT("+ total lost packets: %"PRIu64"\n",
							 avtptcd->total_lost_packets);
			if (avtptcd->dtype==FIXED_DATA){
				UB_CONSOLE_PRINT("+ total error packets: %"PRIu64"\n",
								 avtptcd->total_err_packets);
			}

			UB_CONSOLE_PRINT("+ ts latency: max=%"PRIu64" nsec, min=%"PRIu64
							 " nsec, average=%"PRIu64" nsec\n",
				 			 avtptcd->rate_reporter.latency_max,
							 avtptcd->rate_reporter.latency_min,
				 			 latency_avg);

			UB_CONSOLE_PRINT("+ High latency counter (threshold:%d usec): %"PRIu64"\n",
							avtptcd->wthres_latency, avtptcd->high_latency_count);
		}
		show_report_header_footer(avtptcd, false);
	}
}

static void show_report_header_footer(avtptc_data_t *avtptcd, bool is_header)
{
	char buf[REPORT_HEADER_LEN+1];
	int ret;

	ret = 0;
	if (is_header) {
		ret = snprintf(buf, sizeof (buf), "----- Report during %d seconds --",
					   avtptcd->test_duration);
	}

	while (ret <= REPORT_HEADER_LEN-1) {
		buf[ret++]='-';
	}
	buf[ret]='\0';

	if (is_header){UB_CONSOLE_PRINT("\n");}
	UB_CONSOLE_PRINT("%s\n", buf);
	if (!is_header){UB_CONSOLE_PRINT("\n");}
}


static int talker_one_loop(void *ptr)
{
	uint64_t byte = 0;
	int res;
	int64_t ts;

	avtptc_data_t *avtptcd=(avtptc_data_t *)ptr;

	byte = cb_rate_reporter_gen(&avtptcd->rate_reporter, avtptcd->mbps);

	avtptcd->ssize=(int)byte;
	while(avtptcd->ssize>=avtptcd->mpsize){

		ts=gptpmasterclock_getts64()+avtptcd->tsoffset*UB_USEC_NS;

		if (avtptcd->dtype==AVB_TEST_VIDEO) {
			// for avbvideo decoder to work, need to provide the timestamp
			// as the first bytes after the 8-byte mjpeg header.
			memcpy(&avtptcd->data[8], &ts, sizeof(ts));
		}

		if(avtptcd->runber){
			uint8_t bdata[1500];
			int i;
			memcpy(bdata, avtptcd->data, avtptcd->mpsize);
			for(i=0;i<avtptcd->mpsize && avtptcd->runber;i++){
				bdata[i]^=1; //reverse bit0
				avtptcd->runber--;
			}
			res=avtpc_write_sd_packet(avtptcd->avtpc,
						  avtptcd->mpsize, bdata, ts);
		}else{
			res=avtpc_write_sd_packet(avtptcd->avtpc,
						  avtptcd->mpsize, avtptcd->data, ts);
		}

		if(res<0){
			UB_LOG(UBL_ERROR,"%s:error in avtpc_write_sd_packet\n",__func__);
			avtp_running=false;
			break;
		}

		avtptcd->ssize-=avtptcd->mpsize;
		cb_rate_reporter_send_update_bytes(&avtptcd->rate_reporter, avtptcd->mpsize);

		if(avtptcd->quitn){
			if(--(avtptcd->quitn)==0){
				avtp_running=false;
				break;
			}
		}
	}

	if(avtptcd->test_duration == -1){
		avtp_brater_period_print(avtptcd, true, avtptcd->mpsize);
	}
	cb_rate_reporter_period_reset(&avtptcd->rate_reporter);
	ub_log_flush();

	return 0;
}

static int start_talker(avtptc_data_t *avtptcd)
{
	client_connect_request_t ccr;

	UB_LOG(UBL_INFO,"%s:\n",__func__);
	avtptcd->avtpc=avtpc_init(avtptcd->shsuf);
	if(!avtptcd->avtpc){return -1;}
	memset(&ccr, 0, sizeof(ccr));
	set_ccr(&ccr, avtptcd);
	ccr.send_ahead_ts=avtptcd->send_ahead_ts;
	avtptcd->mpsize=avtptcd->max_frame_size - sizeof(avbtp_cm_stream_header_t);

	if(avtptcd->tshape){avtpc_set_tshaper(&ccr, avtptcd->tshape, avtptcd->tsminsl*1000);}
	if(avtpc_connection_request(avtptcd->avtpc, &ccr)){return -1;}

	avtptcd->ssize=0;
	UB_LOG(UBL_INFO,"%s:start\n",__func__);
	if (avtptcd->runber>0){
		UB_LOG(UBL_INFO,"%s:Simulate error bits: %d bits/%d seconds...\n",
		       __func__, avtptcd->runber, avtptcd->report_interval);
	}
	UB_LOG(UBL_INFO,"%s:Report interval: %d seconds. Please wait...\n",
		   __func__, avtptcd->report_interval);

	cb_rate_reporter_init(&avtptcd->rate_reporter, avtptcd->report_interval * UB_SEC_NS);

	uint64_t start_ts = ub_mt_gettime64();
	while(avtp_running && !durnation_ended(start_ts, avtptcd->test_duration)){
#ifdef HAVE_NO_SELECT
		talker_one_loop(avtptcd);
		CB_USLEEP(avtptcd->send_interval);
#else //!HAVE_NO_SELECT
		int i;
		char c;
		fd_set rfds;
		struct timeval tv;
		uint64_t ts;

		talker_one_loop(avtptcd);
		FD_ZERO(&rfds);
		FD_SET(CONSOLE_RFD, &rfds);
		tv.tv_sec=0;
		tv.tv_usec=avtptcd->send_interval;
		if((i=select(CONSOLE_RFD+1, &rfds, NULL, NULL, &tv))){
			if(i<0){
				// select doesn't like CONSOLE_RFD
				UB_LOG(UBL_WARN,"%s:select() failed (ret=%d, errno=%d)\n",
					   __func__, i, errno);
				CB_USLEEP(1000);
				continue;
			}
			CONSOLE_READ(&c,1);
			if(c!='q'){break;}
			avtptcd->data[0]=0xff;
			ts=gptpmasterclock_getts64();
			avtpc_write_sd_packet(avtptcd->avtpc, avtptcd->mpsize, avtptcd->data, ts);
			break;
		}
#endif //!HAVE_NO_SELECT
	}
	avtptcd->rate_reporter.last_ts=ub_mt_gettime64();

	avtp_show_final_report(avtptcd, true, avtptcd->mpsize);
	UB_LOG(UBL_INFO,"%s:closing\n",__func__);
	CB_USLEEP(10000);
	avtpc_close(avtptcd->avtpc);
	return 0;
}

static int check_biterror(uint8_t *rdata, uint8_t *sdata, int size)
{
	int i,j;
	uint8_t bd;
	int count=0;
	for(i=0;i<size;i++){
		bd=rdata[i]^sdata[i];
		for(j=0;j<8;j++){
			if(bd & 1){count++;}
			bd=bd>>1;
		}
	}
	return count;
}

static int receive_data(uint8_t *payload, int payload_size,
			avbtp_rcv_cb_info_t *cbinfo, void *cbdata)
{
	avtptc_data_t *avtptcd=(avtptc_data_t *)cbdata;
	avbtp_sd_info_t *rsdinfo=&cbinfo->u.rsdinfo;
	uint64_t ts;
	int errbits = 0;

	// The packet size should not change per one avb session for the fix data test
	avtptcd->mpsize = payload_size;

	if(!rsdinfo->current_timestamp){
		ts=gptpmasterclock_getts64();
	}else{
		ts=rsdinfo->current_timestamp;
	}

	if(rsdinfo->seqn_diff!=1) {
		UB_LOG(UBL_INFO,"%s:seqn_diff=%d\n",__func__,rsdinfo->seqn_diff);
		if (rsdinfo->seqn_diff > 0 && avtptcd->total_rx_packets) {
			avtptcd->total_lost_packets += (rsdinfo->seqn_diff-1);
		}
	}
	avtptcd->total_rx_packets++;

	if(!avtptcd->nodc){
		if(payload[0]==0xff) {
			avtp_running=false;
			avtptcd->rate_reporter.last_ts=ub_mt_gettime64();
			return 0;
		}
		if (avtptcd->dtype==FIXED_DATA) {
			// checking of payload can only be performed for fixed data
			errbits=check_biterror(payload, avtptcd->data, payload_size);
			if (errbits > 0) {
				UB_LOG(UBL_INFO,"%s:packet payload doesn't match,"
					   " error bits/payload=%d/%d\n",
				       __func__, errbits, payload_size);
				avtptcd->total_err_packets++;
			}
		}
	}

	if(rsdinfo->timestamp_valid && !rsdinfo->timestamp_uncertain){
		int64_t ts_diff=ts-rsdinfo->timestamp;

		// Count number of high latency values
		if (ts_diff >= ((int64_t)avtptcd->wthres_latency*UB_USEC_NS)) {
			avtptcd->high_latency_count++;
		}

		if(avtptcd->prts && avtptcd->recv_last_ts!=rsdinfo->timestamp){
			UB_CONSOLE_PRINT("system time=%"PRIu64", timestamp=%"PRIu64
							 ", ts=%"PRIu64"\n",
							 ub_rt_gettime64(), rsdinfo->timestamp, ts);
			avtptcd->recv_last_ts=rsdinfo->timestamp;
		}
		cb_rate_reporter_update_latency(&avtptcd->rate_reporter, ts_diff);
	}

	cb_rate_reporter_recv_update_bytes(&avtptcd->rate_reporter, payload_size);
	if (avtptcd->test_duration == -1){
		avtp_brater_period_print(avtptcd, false, payload_size);
	}
	cb_rate_reporter_period_reset(&avtptcd->rate_reporter);

	ub_log_flush();
	return 0;
}

static bool durnation_ended(uint64_t start_time, int test_duration)
{
	if (test_duration == -1) {return false;}
	if (ub_mt_gettime64() - start_time >= (uint64_t)test_duration * UB_SEC_NS){
		UB_LOG(UBL_INFO,"%s:ended\n",__func__);
		return true;
	}
	return false;
}

static int start_listener(avtptc_data_t *avtptcd)
{
	client_connect_request_t ccr;
	uint64_t start_ts;

	UB_LOG(UBL_INFO,"%s:\n",__func__);
	avtptcd->avtpc=avtpc_init(avtptcd->shsuf);
	if(!avtptcd->avtpc){return -1;}
	memset(&ccr, 0, sizeof(ccr));
	set_ccr(&ccr, avtptcd);
	avtpc_set_rcv_cb(avtptcd->avtpc, receive_data, avtptcd);
	if(avtpc_connection_request(avtptcd->avtpc, &ccr)){return -1;}
	cb_rate_reporter_init(&avtptcd->rate_reporter, avtptcd->report_interval * UB_SEC_NS);
	UB_LOG(UBL_INFO,"%s:start\n",__func__);
	start_ts = ub_mt_gettime64();
	while(avtp_running && !durnation_ended(start_ts, avtptcd->test_duration)){
#ifdef HAVE_NO_SELECT
		CB_USLEEP(10000);
#else //!HAVE_NO_SELECT
		int i;
		fd_set rfds;
		struct timeval tv;

		FD_ZERO(&rfds);
		FD_SET(CONSOLE_RFD, &rfds);
		tv.tv_sec=0;
		tv.tv_usec=10000;
		if(!(i=select(1, &rfds, NULL, NULL, &tv))){continue;}
		if(i<0){
			// select doesn't like CONSOLE_RFD
			CB_USLEEP(10000);
			continue;
		}
		break;
#endif //!HAVE_NO_SELECT
	}
	UB_LOG(UBL_INFO,"%s:closing\n",__func__);
	avtp_show_final_report(avtptcd, false, avtptcd->mpsize);
	avtpc_close(avtptcd->avtpc);
	return 0;
}

#ifdef AVTPD_IN_LIBRARY
extern void avtpd_stop(void);
extern int AVTPD_MAIN(int argc, char *argv[]);

static void *run_avtpd(void *ptr)
{
	avtptc_data_t *avtptcd=(avtptc_data_t*)ptr;
	int argc=1;
	int i;
	char *argv[9]={"avtpd",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	if(avtptcd->gshmem){
		argv[argc++]="-g";
		argv[argc++]=avtptcd->gshmem;
	}
	if(avtptcd->shsuf){
		argv[argc++]="-H";
		argv[argc++]=avtptcd->shsuf;
	}
	argv[argc++]="-n";
	for(i=0;i<argc;i++){
		UB_CONSOLE_PRINT("%s ", argv[i]);
	}
	UB_CONSOLE_PRINT("\n");
	AVTPD_MAIN(argc,argv);
	return NULL;
}

static int run_avtpd_inthread(avtptc_data_t *avtptcd, void **avtpdth)
{
	cb_tsn_thread_attr_t attr;
	char thread_name[CB_TSN_THREAD_NAME_SIZE]={0};
	CB_THREAD_T *avtpd_thread;
	int i;

	*avtpdth=malloc(sizeof(CB_THREAD_T));
	if(ub_assert_fatal(*avtpdth!=NULL, __func__, "malloc")) return -1;
	memset(*avtpdth, 0, sizeof(CB_THREAD_T));
	avtpd_thread=(CB_THREAD_T *)*avtpdth;
	snprintf(thread_name, CB_TSN_THREAD_NAME_SIZE, "%s","avtpd_thead");
	cb_tsn_thread_attr_init(&attr, THREAD_NORM_PRI, THREAD_NORM_STACK, thread_name);
	if(CB_THREAD_CREATE(avtpd_thread, &attr, run_avtpd, avtptcd)){
		UB_LOG(UBL_ERROR,"can't run avtpd\n");
		return -1;
	}
	for(i=0;i<10;i++){
		avtptcd->avtpc=avtpc_init(avtptcd->shsuf);
		if(!avtptcd->avtpc){
			CB_USLEEP(100000);
			continue;
		}
		avtpc_close(avtptcd->avtpc);
		return 0;
	}
	UB_LOG(UBL_ERROR,"can't connect to avtpd\n");
	return -1;
}
static void stop_avtpd_inthread(void *avtpdth)
{
	CB_THREAD_T *avtpd_thread=(CB_THREAD_T *)avtpdth;
	if(!avtpdth){return;}
	avtpd_stop();
	CB_THREAD_JOIN(*avtpd_thread, NULL);
	free(avtpdth);
}

#else //!AVTPD_IN_LIBRARY
static int run_avtpd_inthread(avtptc_data_t *avtptcd, void **avtpdth)
{
	UB_LOG(UBL_INFO,"%s:Not support avtpd run internally!!!\n",__func__);
	return -1;
}
static void stop_avtpd_inthread(void *avtpdth)
{
	return;
}
#endif //!AVTPD_IN_LIBRARY

static int console_menu_setup(avtptc_data_t *avtptcd)
{
	char c;
	for(c=0;c!='1' && c!='2';){
		UB_CONSOLE_PRINT("1:Talker\n");
		UB_CONSOLE_PRINT("2:Listener\n");
		UB_CONSOLE_PRINT("1 or 2 ?) ");
		CONSOLE_READ(&c,1);
		UB_CONSOLE_PRINT("\n");
	}
	if(c=='1'){
		avtptcd->mode=TALKER_MODE;
	}else if(c=='2'){
		avtptcd->mode=LISTENER_MODE;
	}else{
		return -1;
	}
	return 0;
}

static void fill_test_data(uint8_t *data, size_t data_size)
{
	if(data_size < 8) {
		UB_LOG(UBL_WARN, "%s():test data buffer too small\n",__func__);
		return;
	}

	// Fill the first 8 bytes with a fixed valid mjpeg header data
	memset(data, 0, data_size);
	data[4] = 0x01; //MJPEG TYPE: 1 (1 byte)
	data[5] = 0xFF; //MJPEG Q: 255 (1 byte)
	data[6] = 0x50; //MJPEG Width: 80 (1 byte)
	data[7] = 0x3C; //MJPEG Height: 60 (1 byte)

	// fill the rest of the payload with fixed data
	for(int i=4;i<(int)(data_size/2);i++){
		data[2*i]=(i>>8)&0xff;
		data[2*i+1]=i&0xff;
	}
}

#ifdef AVTP_TESTCLIENT_MAIN
int stop_testclient()
{
	if( avtp_running ) {
		avtp_running = false;
		return 0;
	}else{
		return -1;
	}
}
#else //!AVTP_TESTCLIENT_MAIN
#define AVTP_TESTCLIENT_MAIN main
#endif //!AVTP_TESTCLIENT_MAIN

int AVTP_TESTCLIENT_MAIN(int argc, char *argv[])
{
	avtptc_data_t avtptcd;
	unibase_init_para_t init_para;
	bool noubinit=false;
	void *avtpdth=NULL;
	memset(&avtptcd, 0, sizeof(avtptcd));
	avtptcd.mode = TALKER_MODE;
	avtptcd.dtype = FIXED_DATA;
	avtptcd.netdev = DEFAULT_NETDEV;
	avtptcd.vid = 110;
	avtptcd.max_frame_size = 1500;
	avtptcd.max_intv_frames = 1;
	avtptcd.time_intv = 250;
	avtptcd.lsem_post_time = DEFAULT_LSEM_POST_TIME;
	avtptcd.priority = 2;
	avtptcd.mbps = 1.0;
	avtptcd.no_avtpd = true;
	avtptcd.mjoin = DEFAULT_JOIN_ADDR;
	avtptcd.mcast = DEFAULT_MCAST_ADDR;
	avtptcd.subtype = AVBTP_SUBTYPE_CVF;
	avtptcd.send_interval = 1000;
	avtptcd.send_ahead_ts = -1;
	avtptcd.test_duration = -1; // Running forever

	// By default, report every 1 seconds
	avtptcd.report_interval = 1;
	// By default, show warning if maximum latency
	// is larger than 20ms.
	avtptcd.wthres_latency = 20000;
	avtptcd.avtpd_buftime = 200000; //200msec should be big enough

	if(argc>0 && (!strcmp(argv[argc-1],"--noubinit") || !strcmp(argv[argc-1], "-u"))){
		noubinit=true;
	}
	if(!noubinit){
		ubb_default_initpara(&init_para);
		init_para.ub_log_initstr=UBL_OVERRIDE_ISTR(
			"4,ubase:45,cbase:45,uconf:46,gptp:46,l2:46", "UBL_L2");
		unibase_init(&init_para);
	}
	fill_test_data(avtptcd.data, sizeof(avtptcd.data));

	if(!avtp_running){avtp_running=true;}

	if(set_options(&avtptcd, argc, argv)<0){return -1;}
	avtptcd.addber=avtptcd.addber*avtptcd.report_interval;
	avtptcd.runber=avtptcd.addber;

	if(strstr(avtptcd.netdev, CB_VIRTUAL_ETHDEV_PREFIX)==avtptcd.netdev){
		// need the IP/UDP header space
		avtptcd.max_frame_size -= CCDBUFF_PAYLOAD_OFFSET + 20;
	}
	if(!avtptcd.no_avtpd && run_avtpd_inthread(&avtptcd, &avtpdth)){
		UB_LOG(UBL_ERROR, "can't run avtpd\n");
		return -1;
	}
	WAIT_AVTPD_READY;

	if(gptpmasterclock_init(avtptcd.gshmem)){return -1;}
#ifndef HAVE_NO_POSIX_CLOCK
	uint64_t tv;
	double rate,prate;
	avtptcd.ct0=clock();
#endif //HAVE_NO_POSIX_CLOCK
	if(avtptcd.start_menu){if(console_menu_setup(&avtptcd)) return -1;}
	UB_CONSOLE_PRINT("mode=%d, hit return to quit\n", avtptcd.mode);
	if(avtptcd.mode==LISTENER_MODE){start_listener(&avtptcd);}
	else{start_talker(&avtptcd);}
	if(!avtptcd.no_avtpd){stop_avtpd_inthread(avtpdth);}
#ifndef HAVE_NO_POSIX_CLOCK
	avtptcd.ct1=clock();
	tv=avtptcd.rate_reporter.last_ts-avtptcd.rate_reporter.start_ts;
	rate=(double)(avtptcd.ct1-avtptcd.ct0)/(double)CLOCKS_PER_SEC;
	prate=rate*100.0/((double)tv/1.0e9);
	UB_CONSOLE_PRINT("cpu time %.2f sec, real time %.2f sec, cpu usage %.2f%%\n",
			 rate, (double)tv*1.0e-9, prate);
#endif //HAVE_NO_POSIX_CLOCK
	if(!noubinit){
		ubb_memory_out_close();
		unibase_close();
	}
	return 0;
}
