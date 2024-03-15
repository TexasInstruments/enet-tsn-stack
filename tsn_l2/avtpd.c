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
 * avtpd.c
 * AVB Transport Protocol Daemon
 *
 */

#include "avbtp.h"
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <tsn_gptp/gptpbasetypes.h>
#include <tsn_gptp/gptpmasterclock.h>
#include <tsn_gptp/gptpconf/gptpgcfg.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/cb_tmevent.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include <tsn_uniconf/yangs/ieee1588-ptp-tt_access.h>
#include "l2conf/avtpgcfg.h"
#include "l2conf/xl4-extmod-xl4avtp.h"

#define DEFAULT_DFILE_PREFIX "/tmp/avtpd_"
#define DEFAULT_DEBUGMEM 4096
#define AVTP_ETHERNET_WRITE_TOUT 100000 // 100msec
#define MAX_CONF_FILES 3

static int stop_mainloop;

typedef struct avtpd_gptp_info{
	bool uncertain;
	bool ovr_uncertain;
}avtpd_gptp_info_t;

typedef struct avtpd_data {
	avtpd_gptp_info_t gptpinfo;
	uint8_t instanceIndex;
	uint8_t gptpInstance;
	avtpgcfgd_t *avtpgcd;
	int mcd_shmem;
	master_connection_data_t *mcd;
	int tccdl[AVBTP_MAX_CLIENTS];
	client_connection_data_t *tccds[AVBTP_MAX_CLIENTS];
	int lccdl[AVBTP_MAX_CLIENTS];
	client_connection_data_t *lccds[AVBTP_MAX_CLIENTS];
	CB_THREAD_MUTEX_T delccd_mutex;
	char *promisc_netdevs;
	int mcd_may_deadlock;
	uint16_t tsport;
	uint16_t tdport;
	char *shsuf;
	char *dfilep;
	char *gshmem;
	int debugmem;
	const char *configfiles[MAX_CONF_FILES+1];
	char *dbname;
} avtpd_data_t;

static avtpd_data_t *avtpdp;
static int avtpd_rcv_cb(uint8_t *data, int size, CB_ETHHDR_T *ethdr, void *cbdata, uint16_t pcv);

static void avtp_tcshaper_init(avtpd_data_t *avtpd,
			       avtp_ethernet_handle_t raweth)
{
	uint8_t i;
	void *value;
	avtp_traffic_shaper_prams_t tsp[8];
	uint8_t numtc=0;
	uint32_t prate=0;
	uint8_t pritcmap[8];
	YDBI_GET_ITEM_VSUBST(uint8_t*, nyavtpk2vk1, numtc, value, avtpd->instanceIndex,
			     XL4_EXTMOD_XL4AVTP_AVTPD_TRAFFIC_CLASS_NUM, 255,
			     NULL, 0, YDBI_CONFIG);
	YDBI_GET_ITEM_VSUBST(uint32_t*, nyavtpk2vk1, prate, value, avtpd->instanceIndex,
			     XL4_EXTMOD_XL4AVTP_AVTPD_PORT_BITRATE, 255,
			     NULL, 0, YDBI_CONFIG);
	memset(tsp, 0, sizeof(avtp_traffic_shaper_prams_t)*8);
	for(i=0;i<numtc;i++){
		YDBI_GET_ITEM_VSUBST(uint8_t*, nyavtpk2vk1, tsp[i].delta_bw, value,
				     avtpd->instanceIndex, XL4_EXTMOD_XL4AVTP_AVTPD_DELTABW,
				     XL4_EXTMOD_XL4AVTP_VALUE,
				     &i, sizeof(uint8_t), YDBI_CONFIG);
		YDBI_GET_ITEM_VSUBST(uint32_t*, nyavtpk2vk1, tsp[i].idleslope, value,
				     avtpd->instanceIndex, XL4_EXTMOD_XL4AVTP_AVTPD_IDLESLOPE,
				     XL4_EXTMOD_XL4AVTP_VALUE,
				     &i, sizeof(uint8_t), YDBI_CONFIG);
		YDBI_GET_ITEM_VSUBST(uint32_t*, nyavtpk2vk1, tsp[i].traffic_interval, value,
				     avtpd->instanceIndex, XL4_EXTMOD_XL4AVTP_AVTPD_TC_INTERVAL,
				     XL4_EXTMOD_XL4AVTP_VALUE,
				     &i, sizeof(uint8_t), YDBI_CONFIG);
		YDBI_GET_ITEM_VSUBST(uint32_t*, nyavtpk2vk1, tsp[i].max_num_packets, value,
				     avtpd->instanceIndex,
				     XL4_EXTMOD_XL4AVTP_AVTPD_MAX_NUM_PACKETS,
				     XL4_EXTMOD_XL4AVTP_VALUE,
				     &i, sizeof(uint8_t), YDBI_CONFIG);
	}
	for(i=0;i<8;i++){
		YDBI_GET_ITEM_VSUBST(uint8_t*, nyavtpk2vk1, pritcmap[i], value,
				     avtpd->instanceIndex, XL4_EXTMOD_XL4AVTP_AVTPD_PRI_TC_MAP,
				     XL4_EXTMOD_XL4AVTP_VALUE,
				     &i, sizeof(uint8_t), YDBI_CONFIG);
	}
	avtp_set_traffic_shaper(raweth, numtc, prate, pritcmap, tsp);
	avtpgcfg_releasedb(avtpd->avtpgcd);
}

#define TCSHPER_ADD false
#define TCSHPER_DELETE true
static int avtp_tcshaper_update(avtpd_data_t *avtpd,
				client_connection_data_t *ccd, bool isdelete)
{
	uint8_t numtc=0;
	void *value;
	uint8_t pritcmap[8];
	int i;
	YDBI_GET_ITEM_VSUBST(uint8_t*, nyavtpk2vk1, numtc, value, avtpd->instanceIndex,
			     XL4_EXTMOD_XL4AVTP_AVTPD_TRAFFIC_CLASS_NUM, 255,
			     NULL, 0, YDBI_CONFIG);
	if(numtc==0){
		avtpgcfg_releasedb(avtpd->avtpgcd);
		return 0;
	}
	for(i=0;i<8;i++){
		YDBI_GET_ITEM_VSUBST(uint8_t*, nyavtpk2vk1, pritcmap[i], value,
				     avtpd->instanceIndex, XL4_EXTMOD_XL4AVTP_AVTPD_PRI_TC_MAP,
				     XL4_EXTMOD_XL4AVTP_VALUE,
				     &i, sizeof(uint8_t), YDBI_CONFIG);
	}
	avtpgcfg_releasedb(avtpd->avtpgcd);
	uint8_t tc=pritcmap[ccd->ccr.priority];
	int res;
	/*
	  12 byte minimum inter-frame spacing
	  8 byte preamble and SFD
	  6 bytes of the DA
	  6 bytes of the SA
	  4 bytes of the VLAN tag
	  2 bytes of the Ethertype
	  4 bytes of the CRC
	  -------
	  total 42
	*/
	int fsize=UB_MAX(ccd->ccr.max_frame_size, 46);// 46 is the minimum payload size
	int32_t add_slope=((int64_t)ccd->ccr.max_intv_frames*(fsize+42)*1000*8 /
			   ccd->ccr.time_intv);
	if(isdelete) {
		if(!ccd->tcs_reserved) return 0;
		add_slope*=-1;
	}
	res=avtp_update_traffic(ccd->raweth, tc, add_slope);
	if(!res){
		if(isdelete)
			ccd->tcs_reserved=false;
		else
			ccd->tcs_reserved=true;
	}
	return res;
}

static int print_usage(char *pname)
{
	char *s;
	if((s=strchr(pname,'/'))==NULL){s=pname;}
	UB_CONSOLE_PRINT("%s [options]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-i|--instnum instance_number:set an instance number(default=0)\n");
	UB_CONSOLE_PRINT("-j|--ginstnum instance_number:set gptpd instance number\n");
	UB_CONSOLE_PRINT("	default='instnum', this gptp instance number.\n");
	UB_CONSOLE_PRINT("	the DB instance is got by this and the domainIndex\n");
	UB_CONSOLE_PRINT("-m|--pmiscdevs: devices to be opened with promisc mode"
			 "(comma separated list)\n");
	UB_CONSOLE_PRINT("-u|--uncertain: don't override timestamp uncertain flag\n");
	UB_CONSOLE_PRINT("-p|--dbname filename(mandatory needed option)\n");
	UB_CONSOLE_PRINT("-s|--tsport: port number: source port number for the test mode\n");
	UB_CONSOLE_PRINT("-d|--tdport: port number: destination port number for the test mode\n");
	UB_CONSOLE_PRINT("-H|--shsuf: suffix: shared memory suffix\n");
	UB_CONSOLE_PRINT("-g|--gshmem: shared_memory_node: gptp shared memory node\n");
	UB_CONSOLE_PRINT("-f|--dfilep: prefix: prefix of a file name to save debug messages,\n"
	       "          default=%s, 'no' for not to save\n", DEFAULT_DFILE_PREFIX);
	UB_CONSOLE_PRINT("-S|--debugmem: debugmemory_size: debug message memory size, default=%d\n",
			 DEFAULT_DEBUGMEM);
	UB_CONSOLE_PRINT("-c|--conf: config file\n");
	UB_CONSOLE_PRINT("  -c "UC_INIT_COPY_INSTANCE_PRE"N, "
			 "copy instance N data to this instance.\n");
	return -1;
}

static int set_options(avtpd_data_t *avtpd, int argc, char *argv[])
{
	int oc;
	int res;
	int cfindex=0;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"instnum", required_argument, 0, 'i'},
		{"ginstnum", required_argument, 0, 'j'},
		{"dbname", required_argument, 0, 'p'},
		{"pmiscdevs", required_argument, 0, 'm'},
		{"uncertain", no_argument, 0, 'u'},
		{"tsport", required_argument, 0, 's'},
		{"tdport", required_argument, 0, 'd'},
		{"shsuf", required_argument, 0, 'H'},
		{"dfilep", required_argument, 0, 'f'},
		{"gshmem", required_argument, 0, 'g'},
		{"debugmem", required_argument, 0, 'S'},
		{"conf", required_argument, 0, 'c'},
		{"noubinit", no_argument, 0, 'n'},
		{0, 0, 0, 0}
	};
	while((oc=getopt_long(argc, argv, "hp:m:ui:j:p:s:d:H:f:g:S:c:n", long_options, NULL))!=-1){
		switch(oc){
		case 'm':
			avtpd->promisc_netdevs=optarg;
			break;
		case 'u':
			avtpd->gptpinfo.ovr_uncertain=false;
			break;
		case 'p':
			avtpd->dbname=optarg;
			break;
		case 'i':
			avtpd->instanceIndex=strtol(optarg, NULL, 0);
			break;
		case 'j':
			avtpd->gptpInstance=strtol(optarg, NULL, 0);
			break;
		case 's':
			avtpd->tsport=strtol(optarg, NULL, 0);
			break;
		case 'd':
			avtpd->tdport=strtol(optarg, NULL, 0);
			break;
		case 'H':
			avtpd->shsuf=optarg;
			break;
		case 'f':
			avtpd->dfilep=optarg;
			break;
		case 'g':
			avtpd->gshmem=optarg;
			break;
		case 'S':
			avtpd->debugmem=strtol(optarg, NULL, 0);
			break;
		case 'c':
			if(cfindex>=MAX_CONF_FILES){return print_usage(argv[0]);}
			avtpd->configfiles[cfindex++]=optarg;
			break;
		case 'n':
			//noubinit must be the last argument, and handled before set_options
			break;
		case 'h':
		default:
			return print_usage(argv[0]);
		}
	}

	res=optind;
	optind=0;
	return res;
}

static int avtpd_close(avtpd_data_t *avtpd)
{
	int res=0;
	char shmname[32];
	CB_THREAD_MUTEX_DESTROY(&avtpd->delccd_mutex);
	CB_THREAD_MUTEX_DESTROY(&avtpd->mcd->mutex);
	snprintf(shmname, 32, "%s%s", AVBTP_MCD_SHMEM_NAME, avtpd->shsuf);
	res=cb_close_shared_mem(avtpd->mcd, &avtpd->mcd_shmem, shmname,
				sizeof(master_connection_data_t), true);
	return res;
}


static int avtpd_init(avtpd_data_t *avtpd)
{
	CB_THREAD_MUTEXATTR_T mattr;
	char shmname[32];

	CB_THREAD_MUTEXATTR_INIT(&mattr);
	CB_THREAD_MUTEXATTR_SETPSHARED(&mattr, CB_THREAD_PROCESS_SHARED);

	snprintf(shmname, 32, "%s%s", AVBTP_MCD_SHMEM_NAME, avtpd->shsuf);
	avtpd->mcd=(master_connection_data_t*)cb_get_shared_mem(&avtpd->mcd_shmem, shmname,
				     sizeof(master_connection_data_t), O_CREAT | O_RDWR);
	if(!avtpd->mcd){
		UB_LOG(UBL_ERROR,"%s:avtpd->mcd is null\n", __func__);
		return -1;
	}
	memset(avtpd->mcd, 0, sizeof(master_connection_data_t));
	IF_GOTO_EREXIT(CB_THREAD_MUTEX_INIT(&avtpd->mcd->mutex, &mattr));
	IF_GOTO_EREXIT(CB_SEM_INIT(&avtpd->mcd->sem_request, 1, 0));
	IF_GOTO_EREXIT(CB_SEM_INIT(&avtpd->mcd->sem_response, 1, 0));
	IF_GOTO_EREXIT(CB_THREAD_MUTEX_INIT(&avtpd->delccd_mutex, NULL));

	return 0;
erexit:
	avtpd_close(avtpd);
	return -1;

}

/*
 * look for stream_id and subtype match in ccds[]
 * subtype=AVBTP_SUBTYPE_ANYSTREAM is a wildcard subtype in stream types
 * stream_id=null(all zero) is a wildcard stream_id
 */
static int check_streamid_in_ccdl(avtpd_data_t *avtpd, int ccdl[],
				  client_connection_data_t *ccds[], ub_streamid_t stream_id,
				  avbtp_subtype_def_t subtype, bool replace_nullid)
{
	int i;
	int nullid=-1;
	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(!ccdl[i]){continue;}

		/* subtype=AVBTP_SUBTYPE_ANYSTREAM can't co-exist with the other stream subtype */
		if(subtype==AVBTP_SUBTYPE_ANYSTREAM){
			if(get_header_type(ccds[i]->ccr.ht.preset_cmsh.hh.subtype)!=
			   AVBTP_STREAM_HEADER){continue;}
		}else if(ccds[i]->ccr.ht.preset_cmsh.hh.subtype==AVBTP_SUBTYPE_ANYSTREAM){
			if(get_header_type(subtype)!=AVBTP_STREAM_HEADER){continue;}
		}else if(ccds[i]->ccr.ht.preset_cmsh.hh.subtype!=subtype){continue;}

		if(!memcmp(ccds[i]->ccr.ht.preset_cmsh.stream_id, stream_id, sizeof(ub_streamid_t))){return i;} // matched

		if(!replace_nullid){continue;}
		/* nullid(stream_id==all_zero) in ccds[] will be replaced the stream_id */
		if((nullid==-1) && !UB_NON_ZERO_B8(ccds[i]->ccr.ht.preset_cmsh.stream_id)){
			nullid=i;
		}
	}
	if(nullid>=0){
		memcpy(ccds[nullid]->ccr.ht.preset_cmsh.stream_id,
		       stream_id, sizeof(ub_streamid_t));
	}
	return nullid;
}

static int check_subtype_in_ccdl(avtpd_data_t *avtpd, int ccdl[],
				 client_connection_data_t *ccds[],
				 bool vsid,
				 avbtp_cm_control_header_t *cmch,
				 char *netdev)
{
	int i;
	int res=-1;
	uint8_t *stream_id=NULL;

	if(vsid && cmch_sv_bit_field(cmch->bf)){stream_id=cmch->stream_id;}

	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(!ccdl[i]){continue;}
		if(ccds[i]->ccr.ht.preset_cmch.hh.subtype!=cmch->hh.subtype){continue;}

		if(stream_id && UB_NON_ZERO_B8(stream_id)){
			if(!memcmp(ccds[i]->ccr.ht.preset_cmch.stream_id,
				   stream_id, sizeof(ub_streamid_t))){res=i;}
		}else{
			res=i;
		}
		if(res==-1){continue;}
		return res;
	}
	return -1;
}

static int get_newindex_in_ccdl(avtpd_data_t *avtpd, int ccdl[])
{
	int i;
	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(!ccdl[i]){return i;}
	}
	UB_LOG(UBL_WARN,"%s: No more ccd, max number %d\n", __func__, i);
	return -1;
}

static int send_one_control_buffer(client_connection_data_t *ccd)
{
	uint16_t pcv=0;
	avbtp_cm_stream_header_t *cmsh;
	ccdbuf_data_t *ccdbuf;
	int res=0;
	int tstime=0;

	// control packet uses only one buffer
	ccdbuf=(ccdbuf_data_t *)(
		ccd->buffer +
		(CCDBUFF_PAYLOAD_OFFSET + ccd->ccr.max_frame_size +
		 sizeof(ccdbuf_data_info_t)) * ccd->ctrlbufp);

	if(ccd->ccr.con_mode==AVTP_TSCF_CONNECTION){
		cmsh=(avbtp_cm_stream_header_t *)ccdbuf->payload;
		if(cmsh_tv_bit_field(cmsh->bf) && (ccd->ccr.send_ahead_ts!=-1) &&
		   ccdbuf->info.timestamp && !(cmsh->hh.bf1&1)){
			gptpmasterclock_wait_until_ts64(
				ccdbuf->info.timestamp - ccd->ccr.send_ahead_ts,
				50000, 2000000000);
		}
	}

	CB_THREAD_MUTEX_LOCK(&ccd->tmutex);
	avtp_ethernet_mutex_lock(ccd->raweth);
	avtp_ethernet_set_dmac(ccd->raweth, ccd->ccr.mcast_addr, NULL);
	ccd->ccr.mcast_addr_update=false;
	if(ccd->ccr.tagged){pcv=(ccd->ccr.priority<<13)|ccd->ccr.vlanid;}
	while(true){
		res=avtp_ethernet_write_nc(ccd->raweth, ccdbuf->header,
					   ccdbuf->info.size, pcv);
		if(res==0 || res<0){break;}
		UB_LOG(UBL_DEBUG, "%s:by tc shaper, sleep %d usec\n",__func__, res);
		if(tstime>AVTP_ETHERNET_WRITE_TOUT){
			UB_LOG(UBL_ERROR, "%s:sleep %d usec, still can't write\n",
			       __func__, tstime);
			res=-1;
			break;
		}
		CB_USLEEP(res);
		tstime+=res;
	}
	if(ccd->ccr.echoback) {
		CB_ETHHDR_T ethdr;
		memset(&ethdr, 0, sizeof(CB_ETHHDR_T));
		memcpy(ethdr.H_SOURCE, avtp_ethernet_get_smac(ccd->raweth), ETH_ALEN);
		avtpd_rcv_cb(ccdbuf->payload, ccdbuf->info.size, &ethdr, avtpdp,
			     (ccd->ccr.priority<<13)|ccd->ccr.vlanid);
	}
	avtp_ethernet_mutex_unlock(ccd->raweth);
	CB_THREAD_MUTEX_UNLOCK(&ccd->tmutex);
	if(res==0){CB_SEM_POST(&ccd->tsem_rel);}
	return 0;
}

static int send_crf_buffer(client_connection_data_t *ccd)
{
	int i;
	ccdbuf_data_t *ccdbuf;
	avbtp_crf_header_t *crfh;
	uint16_t pcv=0;
	int bufused;

	if(ccd->bufused==0){return 0;}

	CB_THREAD_MUTEX_LOCK(&ccd->tmutex);
	avtp_ethernet_mutex_lock(ccd->raweth);
	avtp_ethernet_set_dmac(ccd->raweth, ccd->ccr.mcast_addr, NULL);
	ccd->ccr.mcast_addr_update=false;
	bufused=ccd->bufused;
	CB_THREAD_MUTEX_UNLOCK(&ccd->tmutex);

	/* handle data from the client */
	for(i=0;i<bufused;i++){
		ccdbuf=(ccdbuf_data_t *)(ccd->buffer +
				(ccd->ccr.max_frame_size + sizeof(ccdbuf_data_info_t)) *
				ccd->readp);
		crfh=(avbtp_crf_header_t *)ccdbuf->payload;
		if(avtpdp->gptpinfo.ovr_uncertain && avtpdp->gptpinfo.uncertain){crfh->hh.bf0|=1;}

		if(ccd->ccr.tagged){pcv=(ccd->ccr.priority<<13)|ccd->ccr.vlanid;}

		avtp_ethernet_write_nc(ccd->raweth, ccdbuf->header, ccdbuf->info.size, pcv);

		if(ccd->ccr.echoback){
			CB_ETHHDR_T ethdr;
			memset(&ethdr, 0, sizeof(CB_ETHHDR_T));
			memcpy(ethdr.H_SOURCE, avtp_ethernet_get_smac(ccd->raweth), ETH_ALEN);
			avtpd_rcv_cb(ccdbuf->payload, ccdbuf->info.size, &ethdr, avtpdp,
				     (ccd->ccr.priority<<13)|ccd->ccr.vlanid);
		}
		if(++ccd->readp==ccd->bufnum){ccd->readp=0;}
	}

	avtp_ethernet_mutex_unlock(ccd->raweth);
	CB_THREAD_MUTEX_LOCK(&ccd->tmutex);
	ccd->bufused-=i;
	if(ccd->bufused==0){while(CB_SEM_TRYWAIT(&ccd->tsem_set)==0) ;} //set semaphore to zero
	else
		CB_SEM_POST(&ccd->tsem_set); // need another time
	CB_THREAD_MUTEX_UNLOCK(&ccd->tmutex);

	CB_SEM_POST(&ccd->tsem_rel);
	return 0;
}

#define MIN_SHAPER_SLEEP_US 20 // let is spin for less than this time
static void talker_thread_sleep(client_connection_data_t *ccd, int sleepus)
{
	if(sleepus<MIN_SHAPER_SLEEP_US){return;}
	avtp_ethernet_mutex_unlock(ccd->raweth);
	CB_USLEEP(sleepus);
	avtp_ethernet_mutex_lock(ccd->raweth);
	avtp_ethernet_set_dmac(ccd->raweth, ccd->ccr.mcast_addr, NULL);
}

static int cc_thread_talker(client_connection_data_t *ccd, tsshaper_t *tshd)
{
	struct timespec ts;
	uint64_t ts64;
	int i;
	ccdbuf_data_t *ccdbuf;
	avbtp_cm_stream_header_t *cmsh;
	uint64_t waitedts;
	uint16_t pcv=0;
	int bufused;
	int released=0;

	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+CLIENT_CONNECTION_CHECK_INTERVAL, ts);
	if(CB_SEM_TIMEDWAIT(&ccd->tsem_set, &ts)){return 1;}
	if(ccd->stop_thread==CC_THREAD_IMMEDIATE_STOP){return -1;}

	if(ccd->ccr.con_mode==AVTP_CONTROL_CONNECTION ||
	   ccd->ccr.con_mode==AVTP_TSCF_CONNECTION ||
	   ccd->ccr.con_mode==AVTP_NTSCF_CONNECTION){
		return send_one_control_buffer(ccd);
	}else if(ccd->ccr.con_mode==AVTP_CRF_TALKER_CONNECTION){
		return send_crf_buffer(ccd);
	}

	if(ccd->bufused==0){return 0;}
	// send stream packets
	ccdbuf=(ccdbuf_data_t *)(ccd->buffer +
				 (CCDBUFF_PAYLOAD_OFFSET + ccd->ccr.max_frame_size +
				  sizeof(ccdbuf_data_info_t)) * ccd->readp);

	cmsh=(avbtp_cm_stream_header_t *)ccdbuf->payload;

	if(ccd->ccr.con_mode!=AVTP_TALKONLY_CONTROL_CONNECTION &&
	   avtpdp->gptpinfo.ovr_uncertain && avtpdp->gptpinfo.uncertain){cmsh->hh.bf1|=1;}

	if(ccd->ccr.con_mode!=AVTP_TALKONLY_CONTROL_CONNECTION && (ccd->ccr.send_ahead_ts!=-1) &&
	   cmsh_tv_bit_field(cmsh->bf) && ccdbuf->info.timestamp && !(cmsh->hh.bf1&1)){
		gptpmasterclock_wait_until_ts64(ccdbuf->info.timestamp - ccd->ccr.send_ahead_ts,
						50000, 2000000000);
		waitedts=ccdbuf->info.timestamp;
	}else{
		waitedts=0;
	}

	CB_THREAD_MUTEX_LOCK(&ccd->tmutex);
	avtp_ethernet_mutex_lock(ccd->raweth);
	avtp_ethernet_set_dmac(ccd->raweth, ccd->ccr.mcast_addr, NULL);
	ccd->ccr.mcast_addr_update=false;
	bufused=ccd->bufused;
	CB_THREAD_MUTEX_UNLOCK(&ccd->tmutex);

	/* handle data from the client */
	for(i=0;i<bufused;i++){
		int res=0;
		int tstime=0;
		int sleepts;
		if(tshd->tshape_rate){
			sleepts=avtp_stream_tshaper_sleep(tshd, ccdbuf->info.size);
			if(sleepts>0){talker_thread_sleep(ccd, sleepts/1000);}
		}
		cmsh=(avbtp_cm_stream_header_t *)ccdbuf->payload;
		// set lower 32 bits of timestamp with network order
		if(ccd->ccr.con_mode!=AVTP_TALKONLY_CONTROL_CONNECTION &&
		   !cmsh->header_timestamp){set_header_timestamp(cmsh, ccdbuf->info.timestamp);}
		if(ccd->ccr.tagged){pcv=(ccd->ccr.priority<<13)|ccd->ccr.vlanid;}
		while(true){
			sleepts=avtp_ethernet_write_nc(ccd->raweth, ccdbuf->header,
						       ccdbuf->info.size, pcv);
			if(ccd->stop_thread==CC_THREAD_IMMEDIATE_STOP){
				res=-1;
				break;
			}
			if(sleepts==0){
				released++;
				break;
			}else if(sleepts<0){
				released++;
				UB_TLOG(UBL_DEBUGV, "%s:BE overflow, sid=%02X:%02X\n",
					__func__,
					ccd->ccr.ht.preset_cmsh.stream_id[6],
					ccd->ccr.ht.preset_cmsh.stream_id[7]);
				break;
			}
			UB_TLOG(UBL_DEBUG, "%s:ci=%d, by tc shaper, sleep %d usec\n",
			       __func__, ccd->ccr.ci, res);
			if(tstime>AVTP_ETHERNET_WRITE_TOUT){
				UB_LOG(UBL_ERROR, "%s:ci=%d, sleep %d usec, still can't write\n",
				       __func__, ccd->ccr.ci, tstime);
				res=-1;
				break;
			}
			talker_thread_sleep(ccd, sleepts);
			tstime+=sleepts;
		}
		if(res<0){break;}
		if(ccd->ccr.echoback){
			CB_ETHHDR_T ethdr;
			memset(&ethdr, 0, sizeof(CB_ETHHDR_T));
			memcpy(ethdr.H_SOURCE, avtp_ethernet_get_smac(ccd->raweth), ETH_ALEN);
			avtpd_rcv_cb(ccdbuf->payload, ccdbuf->info.size, &ethdr, avtpdp,
				     (ccd->ccr.priority<<13)|ccd->ccr.vlanid);
		}
		if(++ccd->readp==ccd->bufnum){ccd->readp=0;}
		if(ccd->ccr.con_mode!=AVTP_TALKONLY_CONTROL_CONNECTION &&
		   cmsh_tv_bit_field(cmsh->bf)){
			if((waitedts==0) || (waitedts!=ccdbuf->info.timestamp)){
				break;
			}
		}
		ccdbuf=(ccdbuf_data_t *)(ccd->buffer +
					 (CCDBUFF_PAYLOAD_OFFSET + ccd->ccr.max_frame_size +
					  sizeof(ccdbuf_data_info_t)) * ccd->readp);
	}

	avtp_ethernet_mutex_unlock(ccd->raweth);
	CB_THREAD_MUTEX_LOCK(&ccd->tmutex);
	ccd->bufused-=released;
	if(ccd->bufused==0){
		while(CB_SEM_TRYWAIT(&ccd->tsem_set)==0); //set semaphore to zero
	}else{
		CB_SEM_POST(&ccd->tsem_set); // need another time
	}
	CB_THREAD_MUTEX_UNLOCK(&ccd->tmutex);

	if(released>0){
		CB_SEM_POST(&ccd->tsem_rel);
	}else{
		UB_LOG(UBL_DEBUG, "%s:bufused=%d, can't release any\n",__func__, bufused);
	}
	return 0;
}

static int cc_thread_listener(client_connection_data_t *ccd)
{
	struct timespec ts;
	uint64_t ts64;

	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+CLIENT_CONNECTION_CHECK_INTERVAL, ts);
	if(CB_SEM_TIMEDWAIT(&ccd->rsem_rel, &ts)){return 1;}
	if(ccd->stop_thread==CC_THREAD_IMMEDIATE_STOP){return -1;}
	return 0;
}

static void *client_connection_thread(void *ptr)
{
	int res;
	client_connection_data_t *ccd=(client_connection_data_t*)ptr;
	tsshaper_t tshd;

	memset(&tshd, 0, sizeof(tsshaper_t));
	tshd.tshape_rate=ccd->ccr.tshape_rate;
	tshd.tshape_minsleep=ccd->ccr.tshape_minsleep;

	while(ccd->stop_thread<CC_THREAD_IMMEDIATE_STOP){
		switch(ccd->ccr.con_mode){
		case AVTP_LISTENER_CONNECTION:
		case AVTP_CRF_LISTENER_CONNECTION:
			res=cc_thread_listener(ccd);
			break;
		case AVTP_TALKER_CONNECTION:
		case AVTP_CONTROL_CONNECTION:
		case AVTP_TALKONLY_CONTROL_CONNECTION:
		case AVTP_NTSCF_CONNECTION:
		case AVTP_TSCF_CONNECTION:
		case AVTP_CRF_TALKER_CONNECTION:
			res=cc_thread_talker(ccd, &tshd);
			break;
		default:
			UB_LOG(UBL_ERROR,"%s:not supported connection mode: %d\n",
			       __func__, ccd->ccr.con_mode);
			res=-1;
			break;
		}
		if(res<0){
			ccd->stop_thread=CC_THREAD_IMMEDIATE_STOP;
		}else if(res==1 && ccd->stop_thread==CC_THREAD_BUFFLASH_STOP){
			ccd->stop_thread=CC_THREAD_IMMEDIATE_STOP;
		}
	}
	ccd->stop_thread=CC_THREAD_STOPPED;
	UB_TLOG(UBL_INFO, "%s:ci=%d, closing\n", __func__, ccd->ccr.ci);
	return NULL;
}

static int delete_last_mcast(avtpd_data_t *avtpd, int ci, client_connection_data_t *ccd, int *ccdl)
{
	int i;
	if(ccdl!=avtpd->lccdl){return 1;}
	if(!UB_NON_ZERO_B6(ccd->ccr.join_mcast)){return 1;}
	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(ci==i){continue;}
		if(!avtpd->lccdl[i] || avtpd->lccds[i]->raweth!=ccd->raweth){continue;}
		if(memcmp(ccd->ccr.join_mcast, avtpd->lccds[i]->ccr.join_mcast, 6)){continue;}
		// ccd->ccr->join_mcast is used by the other connections, don't delete now.
		return 1;
	}
	return avtp_ethernet_del_mcast(ccd->raweth, ccd->ccr.netdev, ccd->ccr.join_mcast);
}

/* raweth may be shared, close if it is used by only one ccd */
static int close_last_avtp_ethernet(avtpd_data_t *avtpd, int ci, client_connection_data_t *ccd, int *ccdl)
{
	int i;
	int count=0;
	delete_last_mcast(avtpd, ci, ccd, ccdl);
	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(avtpd->tccdl[i] && avtpd->tccds[i]->raweth==ccd->raweth){
			if(ci!=i || ccdl!=avtpd->tccdl){return 1;}
			count++;
		}
		if(avtpd->lccdl[i] && avtpd->lccds[i]->raweth==ccd->raweth){
			if(ci!=i || ccdl!=avtpd->lccdl){return 1;}
			count++;
		}
	}
	if(count>1){return 1;}
	avtp_ethernet_close(ccd->raweth);
	return 0;
}

static int delete_ccd(avtpd_data_t *avtpd, int ccdl[],
		      client_connection_data_t *ccds[], int shmsuffix, int ci)
{
	char shmname[32];
	int kcount=0;
	int ccdl_ci;
	if(ccdl[ci]<=0) {
		UB_LOG(UBL_ERROR,"%s:ccd of index=%d doesn't exist\n", __func__,ci);
		return -1;
	}
	CB_THREAD_MUTEX_LOCK(&avtpd->delccd_mutex);
	ccdl_ci=ccdl[ci];
	ccdl[ci]=0;
	CB_THREAD_MUTEX_UNLOCK(&avtpd->delccd_mutex);

	snprintf(shmname, 32, AVBTP_CCD_SHMEM_PREFIX"%s%d-%d", avtpd->shsuf, shmsuffix, ci);
	if(ccds[ci]){
		avtp_tcshaper_update(avtpd, ccds[ci], TCSHPER_DELETE);
		if(ccds[ci]->raweth){close_last_avtp_ethernet(avtpd, ci, ccds[ci], ccdl);}
		if(ccds[ci]->stop_thread>=CC_THREAD_RUN &&
		   ccds[ci]->stop_thread<CC_THREAD_STOPPED){
			ccds[ci]->stop_thread=CC_THREAD_IMMEDIATE_STOP;
			CB_SEM_POST(&ccds[ci]->tsem_set);
			CB_SEM_POST(&ccds[ci]->rsem_rel);
			while(ccds[ci]->stop_thread!=CC_THREAD_STOPPED){
				if(kcount++==10){
					UB_LOG(UBL_ERROR,
					       "%s:ccdd[%d], thread can't stop\n",__func__, ci);
					break;
				}
				CB_USLEEP(10000);
			}
		}
		if(ccds[ci]->thread){CB_THREAD_JOIN(ccds[ci]->thread, NULL);}
		UB_LOG(UBL_DEBUG,"%s:ccd thread was joined, ci=%d\n", __func__, ci);
		CB_THREAD_MUTEX_DESTROY(&ccds[ci]->rmutex);
		CB_SEM_DESTROY(&ccds[ci]->rsem_rel);
		CB_SEM_DESTROY(&ccds[ci]->rsem_set);
		CB_THREAD_MUTEX_DESTROY(&ccds[ci]->tmutex);
		CB_SEM_DESTROY(&ccds[ci]->tsem_rel);
		CB_SEM_DESTROY(&ccds[ci]->tsem_set);

		cb_close_shared_mem(ccds[ci], &ccdl_ci, shmname,
				 get_ccd_size(&ccds[ci]->ccr, NULL), true);
		ccds[ci]=NULL;
	}else{
		cb_close_shared_mem(NULL, &ccdl_ci, shmname, 0, true);
	}
	return 0;
}

/* BE CAREFUL, this runs in a different thread */
#define RMUTEX_BUSY_SLEEPUSEC 125
#define MAX_RMUTEX_BUSY 800 // 125usec*800 = 100 msec
static int avtpd_rcv_cb(uint8_t *data, int size, CB_ETHHDR_T *ethdr, void *cbdata, uint16_t pcv)
{
	avtpd_data_t *avtpd=(avtpd_data_t *)cbdata;
	avbtp_cm_stream_header_t *cmsh;
	char astr[32];
	int ci=-1;
	ccdbuf_data_t *ccdbuf;
	uint64_t timestamp=0;
	int res=-1;
	int rmutex_busy=0;
	bool vsid;

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
	CB_THREAD_MUTEX_LOCK(&avtpd->delccd_mutex);
	switch(get_header_type(cmsh->hh.subtype)){
	case AVBTP_UNKNOWN_HEADER:
		UB_LOG(UBL_INFO,"%s:unknown subtype=%d\n",__func__, cmsh->hh.subtype);
		goto erexit;
	case AVBTP_STREAM_HEADER:
		if((ci=check_streamid_in_ccdl(avtpd, avtpd->lccdl, avtpd->lccds,
					      cmsh->stream_id,
					      (avbtp_subtype_def_t)cmsh->hh.subtype, true))<0){
			UB_LOG(UBL_DEBUG,"%s: stream_id="UB_PRIhexB8
			       " subtype=%d is not in the listeners\n",
			       __func__, UB_ARRAY_B8(cmsh->stream_id), cmsh->hh.subtype);
			goto erexit;
		}
		if(!avtpd->lccds[ci]->ccr.avtpc_expandts32){
			timestamp = gptpmasterclock_expand_timestamp(get_header_timestamp(cmsh));
		}else{
			timestamp = (uint64_t)get_header_timestamp(cmsh);
		}
		if(avtpd->gptpinfo.ovr_uncertain && avtpd->gptpinfo.uncertain){cmsh->hh.bf1|=1;}
		break;
	case AVBTP_CONTROL_HEADER:
		sprintf(astr, UB_PRIhexB6, UB_ARRAY_B6(ethdr->H_SOURCE));
		if((ci=check_subtype_in_ccdl(avtpd, avtpd->lccdl, avtpd->lccds, true,
					     (avbtp_cm_control_header_t *)cmsh, astr))<0){
			UB_LOG(UBL_DEBUG,"%s: subtype=%d is not in the listeners\n", __func__,
			       cmsh->hh.subtype);
			goto erexit;
		}
		break;
	case AVBTP_ALTERNATIVE_HEADER:
		// use the common part of avbtp_cm_control_header_t
		sprintf(astr, UB_PRIhexB6, UB_ARRAY_B6(ethdr->H_SOURCE));
		vsid=(cmsh->hh.subtype==AVBTP_SUBTYPE_CRF)||
			(cmsh->hh.subtype==AVBTP_SUBTYPE_NTSCF);
		if((ci=check_subtype_in_ccdl(avtpd, avtpd->lccdl, avtpd->lccds, vsid,
					     (avbtp_cm_control_header_t *)cmsh, astr))<0){
			UB_LOG(UBL_DEBUG,"%s: subtype=%d is not in the listeners\n", __func__,
			       cmsh->hh.subtype);
			goto erexit;
		}
		if (cmsh->hh.subtype == AVBTP_SUBTYPE_CRF) {
			if(avtpd->gptpinfo.ovr_uncertain && avtpd->gptpinfo.uncertain) {
				avbtp_crf_header_t *crfh = (avbtp_crf_header_t *)data;
				crfh->hh.bf0|=1;
			}
		}
		break;
	}

	if(avtpd->lccds[ci]->stop_thread==CC_THREAD_BUFFLASH_STOP){
		UB_LOG(UBL_DEBUG,"%s:In BUFFLASH_STOP, throw away the data\n",__func__);
		goto erexit;
	}

	// if ccr.vlanid is not set, don't check vlanid even with tagged data
	if(pcv && avtpd->lccds[ci]->ccr.vlanid &&
	   avtpd->lccds[ci]->ccr.vlanid != (pcv&0x0fff)){
		UB_LOG(UBL_INFO,"%s: vlanid=0x%x doesn't match\n",__func__, pcv&0x0fff);
		goto erexit;
	}
	if(avtpd->lccds[ci]->ccr.filter_destmac &&
	   UB_NON_ZERO_B6(avtpd->lccds[ci]->ccr.join_mcast) &&
	   memcmp(avtpd->lccds[ci]->ccr.join_mcast, ethdr->h_dest, ETH_ALEN)){
		goto erexit;
	}
	if(size > CCDBUFF_PAYLOAD_OFFSET + avtpd->lccds[ci]->ccr.max_frame_size){
		UB_LOG(UBL_INFO,"%s: size=%d is too big\n",__func__, size);
		goto erexit;
	}
	if(avtpd->lccds[ci]->bufused==avtpd->lccds[ci]->bufnum) {
		UB_LOG(UBL_WARN,"%s: buffer full\n",__func__);
		/* In an unlikely case where previous rsem_set is missed or encompassed
		 * (absorbed) into current client processing, then buffer full occurs
		 * and client release the semaphore, it is needed to notify client again.
		 * Otherwise, client waits the full RECV_DATA_CHECK_INTERVAL and there
		 * is no chance for buffer to be consumed.
		 * At any rate, re-notifying client when bufused>0 posses no harm.
		 */
		CB_SEM_POST(&avtpd->lccds[ci]->rsem_set);
		// drop current frame
		goto erexit;
	}
	ccdbuf = (ccdbuf_data_t *)
		(avtpd->lccds[ci]->buffer +
		 (CCDBUFF_PAYLOAD_OFFSET + avtpd->lccds[ci]->ccr.max_frame_size +
		  sizeof(ccdbuf_data_info_t)) * avtpd->lccds[ci]->writep);
	ccdbuf->info.size = size;
	ccdbuf->info.timestamp = timestamp;

	memcpy(ccdbuf->header+ETH_ALEN, ethdr->H_SOURCE, ETH_ALEN); // copy source address
	memcpy(ccdbuf->payload, data, size); // copy payload
	if(++avtpd->lccds[ci]->writep==avtpd->lccds[ci]->bufnum){avtpd->lccds[ci]->writep=0;}
	while(CB_THREAD_MUTEX_TRYLOCK(&avtpd->lccds[ci]->rmutex)){
		if(++rmutex_busy > MAX_RMUTEX_BUSY){
			UB_LOG(UBL_INFO,"%s:ci=%d, rmutex is busy. client may be died already?\n",
			       __func__, ci);
			break;
		}
		CB_USLEEP(RMUTEX_BUSY_SLEEPUSEC);
	}
	avtpd->lccds[ci]->bufused++;
	if(rmutex_busy <= MAX_RMUTEX_BUSY){CB_THREAD_MUTEX_UNLOCK(&avtpd->lccds[ci]->rmutex);}
	if(avtpd->lccds[ci]->bufused > avtpd->lccds[ci]->rcv_sempost_bufnum){
		CB_SEM_POST(&avtpd->lccds[ci]->rsem_set);
	}
	res=0;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&avtpd->delccd_mutex);
	return res;
}


static void find_avtp_ethernet(int ccdl[], client_connection_data_t *ccds[],
			      client_connect_request_t *ccr,
			      avtp_ethernet_handle_t *raweth, bool *jmdone)
{
	int i;
	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(ccdl[i] && !strcmp(ccds[i]->ccr.netdev, ccr->netdev)){
			if(!*raweth){*raweth=ccds[i]->raweth;}
			if(!UB_NON_ZERO_B6(ccr->join_mcast)){
				*jmdone=true;
				break;
			}
			if(!memcmp(ccds[i]->ccr.join_mcast, ccr->join_mcast, ETH_ALEN)){
				*jmdone=true;
				break;
			}
		}
	}
}

static avtp_ethernet_handle_t get_avtp_ethernet(avtpd_data_t *avtpd,
						client_connect_request_t *ccr)
{
	avtp_ethernet_handle_t raweth=NULL;
	avtp_eth_paras_t ethp;
	bool jmdone=false;
	ethp.promisc=ccr->promisc;
	char *fdev;
	find_avtp_ethernet(avtpd->tccdl, avtpd->tccds, ccr, &raweth, &jmdone);
	if(jmdone){return raweth;}
	find_avtp_ethernet(avtpd->lccdl, avtpd->lccds, ccr, &raweth, &jmdone);
	if(jmdone){return raweth;}
	if(raweth){
		avtp_ethernet_add_mcast(raweth, ccr->netdev, ccr->join_mcast);
		return raweth;
	}
	if(avtpd->promisc_netdevs && (fdev=strstr(avtpd->promisc_netdevs, ccr->netdev))){
		if(fdev[strlen(ccr->netdev)]==0 || fdev[strlen(ccr->netdev)]==','){ethp.promisc=true;}
	}
	ethp.dev=ccr->netdev;
	ethp.mcastmac=ccr->join_mcast;
	ethp.rec_tagged=ccr->rec_tagged;
	ethp.priority=ccr->priority;
	ethp.tm_sport=avtpd->tsport;
	ethp.tm_dport=avtpd->tdport;
	ethp.vlanid=ccr->vlanid;
	switch(ccr->con_mode){
	case AVTP_TALKER_CONNECTION:
	case AVTP_TALKONLY_CONTROL_CONNECTION:
	case AVTP_CRF_TALKER_CONNECTION:
		ethp.rw_type=CB_RAWSOCK_WRONLY;
		break;
	case AVTP_LISTENER_CONNECTION:
	case AVTP_CRF_LISTENER_CONNECTION:
		ethp.rw_type=CB_RAWSOCK_RDONLY;
		break;
	case AVTP_CONTROL_CONNECTION:
	case AVTP_NTSCF_CONNECTION:
	case AVTP_TSCF_CONNECTION:
		ethp.rw_type=CB_RAWSOCK_RDWR;
		break;
	}
	raweth=avtp_ethernet_open(&ethp, avtpd_rcv_cb, avtpd);
	if(!raweth){return NULL;}
	avtp_tcshaper_init(avtpd, raweth);
	return raweth;
}

static int create_connection(avtpd_data_t *avtpd, int ccdl[],
			     client_connection_data_t *ccds[], int shmsuffix)
{
	int ci = 0;
	char shmname[32];
	unsigned int ccdbufnum = 0;
	unsigned int ccdsize = 0;
	CB_THREAD_MUTEXATTR_T mattr;
	int res=-1; // -1:fatal error, -2:error by invalid requests
	int ccdl_ci=0;
	bool create_thread=true;
	memset(shmname,0x00,sizeof(shmname));

	CB_THREAD_MUTEXATTR_INIT(&mattr);
	CB_THREAD_MUTEXATTR_SETPSHARED(&mattr, CB_THREAD_PROCESS_SHARED);

	if((ci=get_newindex_in_ccdl(avtpd, ccdl))<0){return -1;}
	ccdsize=get_ccd_size(&avtpd->mcd->ccr, &ccdbufnum);
	if(ccdbufnum<2){
		UB_LOG(UBL_ERROR,"the number of buffers:%d must be larger than 1\n",ccdbufnum);
		return -1;
	}
	snprintf(shmname, 32, AVBTP_CCD_SHMEM_PREFIX"%s%d-%d", avtpd->shsuf, shmsuffix, ci);
	ccds[ci]=(client_connection_data_t *)cb_get_shared_mem(&ccdl_ci, shmname, ccdsize, O_CREAT | O_RDWR);
	if(!ccds[ci]){return -1;}
	memset(ccds[ci], 0, ccdsize);
	IF_GOTO_EREXIT(CB_SEM_INIT(&ccds[ci]->tsem_set, 1, 0));
	IF_GOTO_EREXIT(CB_SEM_INIT(&ccds[ci]->tsem_rel, 1, 1));
	IF_GOTO_EREXIT(CB_THREAD_MUTEX_INIT(&ccds[ci]->tmutex, &mattr));
	IF_GOTO_EREXIT(CB_SEM_INIT(&ccds[ci]->rsem_set, 1, 0));
	IF_GOTO_EREXIT(CB_SEM_INIT(&ccds[ci]->rsem_rel, 1, 1));
	IF_GOTO_EREXIT(CB_THREAD_MUTEX_INIT(&ccds[ci]->rmutex, &mattr));

	memcpy(&ccds[ci]->ccr, &avtpd->mcd->ccr, sizeof(client_connect_request_t));
	ccds[ci]->ccr.ci=ci;
	UB_LOG(UBL_INFO,"%s: ci=%d frame size=%d bufnum=%d ccdsize=%d\n",__func__,
	       ci, CCDBUFF_PAYLOAD_OFFSET + ccds[ci]->ccr.max_frame_size,
		   ccdbufnum, ccdsize);
	if(ccds[ci]->ccr.con_mode==AVTP_CONTROL_CONNECTION ||
	   ccds[ci]->ccr.con_mode==AVTP_NTSCF_CONNECTION ||
	   ccds[ci]->ccr.con_mode==AVTP_TSCF_CONNECTION){
		// the last buffer is used to send control packet
		ccds[ci]->bufnum=ccdbufnum-1;
		ccds[ci]->ctrlbufp=ccdbufnum-1;
	}else{
		ccds[ci]->bufnum=ccdbufnum;
		UB_LOG(UBL_INFO,"%s:stream_id="UB_PRIhexB8", subtype=%d\n",__func__,
		       UB_ARRAY_B8(ccds[ci]->ccr.ht.preset_cmsh.stream_id),
		       ccds[ci]->ccr.ht.preset_cmch.hh.subtype);
	}
	ccds[ci]->ccr.netdev[0]=0;// to prevent of finding itself by find_avtp_ethernet
	if((ccds[ci]->raweth=get_avtp_ethernet(avtpd, &avtpd->mcd->ccr))==NULL) {
		UB_LOG(UBL_ERROR,"can't open raw ethernet, netdev=%s\n", avtpd->mcd->ccr.netdev);
		res=-2;
		goto erexit;
	}
	if(avtp_tcshaper_update(avtpd, ccds[ci], TCSHPER_ADD)) {
		res=-2;
		goto erexit;
	}
	ccds[ci]->ccr.netdev[0]=avtpd->mcd->ccr.netdev[0];
	avtp_ethernet_set_smac_bydev(ccds[ci]->raweth, ccds[ci]->ccr.netdev);
	avtp_ethernet_set_dmac(ccds[ci]->raweth, ccds[ci]->ccr.mcast_addr, NULL);

	if(shmsuffix==LISTENER_SHM_SUFFIX){
		/* to post the semaphore,
		   when the number of receive buffer reaches to rcv_sempost_bufnum */
		ccds[ci]->rcv_sempost_bufnum = avtpd->lccds[ci]->ccr.lsem_post_time *
			avtpd->lccds[ci]->ccr.max_intv_frames / avtpd->lccds[ci]->ccr.time_intv;
		ccds[ci]->rcv_sempost_bufnum = UB_MIN(avtpd->lccds[ci]->bufnum/2,
						   ccds[ci]->rcv_sempost_bufnum);
	}

	/* we don't create the thread for the listener when there is no health check */
	if (ccds[ci]->ccr.no_healthcheck) {
		 if (ccds[ci]->ccr.con_mode==AVTP_LISTENER_CONNECTION ||
			 ccds[ci]->ccr.con_mode==AVTP_CRF_LISTENER_CONNECTION) {
			 create_thread=false;
		 }
	}
	if(create_thread){
		cb_tsn_thread_attr_t attr;
		char thread_name[CB_TSN_THREAD_NAME_SIZE]={0};
		snprintf(thread_name, CB_TSN_THREAD_NAME_SIZE, "avtpd_%d", ci);
		cb_tsn_thread_attr_init(&attr, THREAD_NORM_PRI, THREAD_NORM_STACK, thread_name);
		if(CB_THREAD_CREATE(&ccds[ci]->thread, &attr,
							client_connection_thread, ccds[ci])){
			UB_LOG(UBL_ERROR,"%s:CB_THREAD_CREATE, %s\n", __func__, strerror(errno));
			goto erexit;
		}
		ccds[ci]->stop_thread=CC_THREAD_RUN;
	}
	ccdl[ci]=ccdl_ci;
	return ci;
erexit:
	// when an error happens, a new thread is not run.
	// setting ccdl[ci] won't make any accesses to the shared memory
	ccdl[ci]=ccdl_ci;
	delete_ccd(avtpd, ccdl, ccds, shmsuffix, ci);
	return res;
}

static int create_ccd(avtpd_data_t *avtpd)
{
	switch(avtpd->mcd->ccr.con_mode){
	case AVTP_TALKER_CONNECTION:
	case AVTP_TALKONLY_CONTROL_CONNECTION:
	case AVTP_CRF_TALKER_CONNECTION:
		return create_connection(avtpd, avtpd->tccdl, avtpd->tccds, TALKER_SHM_SUFFIX);
	case AVTP_LISTENER_CONNECTION:
	case AVTP_CONTROL_CONNECTION:
	case AVTP_NTSCF_CONNECTION:
	case AVTP_TSCF_CONNECTION:
	case AVTP_CRF_LISTENER_CONNECTION:
		return create_connection(avtpd, avtpd->lccdl, avtpd->lccds, LISTENER_SHM_SUFFIX);
	default:
		UB_LOG(UBL_ERROR,"%s:not supported connection mode: %d\n",
		       __func__, avtpd->mcd->ccr.con_mode);
		return -1;
	}
}

static int health_check_cc(avtpd_data_t *avtpd, client_connection_data_t *ccd)
{
	if(ccd->health_check>=HEALTH_STATE_BAD) {
		UB_LOG(UBL_INFO,"%s:health_check=%d\n",__func__, ccd->health_check);
		return -1;
	}
	ccd->health_check++;
	return 0;
}

/* return the number of runnng talkers and listeners */
static int check_ccd_thread(avtpd_data_t *avtpd, bool quit)
{
	int i;
	int count=0;
	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(!avtpd->tccdl[i]){continue;}
		if(avtpd->tccds[i]->ccr.no_healthcheck){continue;}
		if((avtpd->tccds[i]->stop_thread==CC_THREAD_RUN ||
		    avtpd->tccds[i]->stop_thread==CC_THREAD_BUFFLASH_STOP)  && !quit)
			if(!health_check_cc(avtpd, avtpd->tccds[i])) {
				count++;
				continue;
			}
		UB_TLOG(UBL_INFO,"%s:a talker thread is not running, "
			"delete the connection(ci=%d)\n", __func__, i);
		delete_ccd(avtpd, avtpd->tccdl, avtpd->tccds, TALKER_SHM_SUFFIX, i);
	}
	for(i=0;i<AVBTP_MAX_CLIENTS;i++){
		if(!avtpd->lccdl[i]){continue;}
		if(avtpd->lccds[i]->ccr.no_healthcheck){continue;}
		if((avtpd->lccds[i]->stop_thread == CC_THREAD_RUN ||
		    avtpd->lccds[i]->stop_thread==CC_THREAD_BUFFLASH_STOP) && !quit)
			if(!health_check_cc(avtpd, avtpd->lccds[i])) {
				count++;
				continue;
			}
		UB_LOG(UBL_INFO,"%s:a listener thread is not running, delete the connection(ci=%d)\n",
		       __func__, i);
		delete_ccd(avtpd, avtpd->lccdl, avtpd->lccds, LISTENER_SHM_SUFFIX, i);
	}
	return count;
}


static int check_connection_exist(avtpd_data_t *avtpd)
{
	avtp_conection_mode_t con_mode=avtpd->mcd->ccr.con_mode;

	switch(con_mode){
	case AVTP_TALKER_CONNECTION:
	case AVTP_CRF_TALKER_CONNECTION:
		if(check_streamid_in_ccdl(
			   avtpd, avtpd->tccdl, avtpd->tccds,
			   avtpd->mcd->ccr.ht.preset_cmsh.stream_id,
			   (avbtp_subtype_def_t)avtpd->mcd->ccr.ht.preset_cmsh.hh.subtype,
			   false)>=0){
			UB_LOG(UBL_INFO,"requested stream id("UB_PRIhexB8
			       ") for talker connection is already used\n",
			       UB_ARRAY_B8(avtpd->mcd->ccr.ht.preset_cmsh.stream_id));
			return 0;
		}
		return -1;
	case AVTP_LISTENER_CONNECTION:
	case AVTP_CRF_LISTENER_CONNECTION:
		if(check_streamid_in_ccdl(
			   avtpd, avtpd->lccdl, avtpd->lccds,
			   avtpd->mcd->ccr.ht.preset_cmsh.stream_id,
			   (avbtp_subtype_def_t)avtpd->mcd->ccr.ht.preset_cmsh.hh.subtype,
			   false)>=0){
			UB_LOG(UBL_INFO,"requested stream id("UB_PRIhexB8
			       ") for listener connection is already used\n",
			       UB_ARRAY_B8(avtpd->mcd->ccr.ht.preset_cmsh.stream_id));
			return 0;
		}
		return -1;
	case AVTP_CONTROL_CONNECTION:
	case AVTP_TSCF_CONNECTION:
	case AVTP_NTSCF_CONNECTION:
		if(check_subtype_in_ccdl(avtpd, avtpd->lccdl, avtpd->lccds,
					 true,
					 &avtpd->mcd->ccr.ht.preset_cmch,
					 avtpd->mcd->ccr.netdev)>=0){
			UB_LOG(UBL_INFO,"requested subtype(%d) for controller connection is "
			       "already used\n", avtpd->mcd->ccr.ht.preset_cmch.hh.subtype);
			return 0;
		}
		return -1;
	case AVTP_TALKONLY_CONTROL_CONNECTION:
		// multiple connections are allowed for this type
		UB_LOG(UBL_DEBUG,"requested subtype(%d) for talkonly controller connection\n",
		       avtpd->mcd->ccr.ht.preset_cmch.hh.subtype);
		return -1;
	}
	return -1;
}

#define CLIENT_ZERO_THRESH 5
#define CLIENT_NONZERO_THRESH 20
static int dead_client_check(avtpd_data_t *avtpd)
{
	int thresh=CLIENT_ZERO_THRESH;
	int runcls=check_ccd_thread(avtpd, false);
	if(runcls>0){thresh=CLIENT_NONZERO_THRESH;}
	if(CB_THREAD_MUTEX_TRYLOCK(&avtpd->mcd->mutex)){
		if(++avtpd->mcd_may_deadlock>=thresh){
			UB_LOG(UBL_INFO,"%s:mcd->mutex is locked consecutive %d times, "
			       "client num=%d\n",
			       __func__, avtpd->mcd_may_deadlock, runcls);
			UB_LOG(UBL_INFO,"force to unlock it\n");
			CB_THREAD_MUTEX_UNLOCK(&avtpd->mcd->mutex);
			avtpd->mcd_may_deadlock=0;
			while(CB_SEM_TRYWAIT(&avtpd->mcd->sem_response)==0){;}
			return 1;
		}
	}else{
		CB_THREAD_MUTEX_UNLOCK(&avtpd->mcd->mutex);
		avtpd->mcd_may_deadlock=0;
	}
	return 0;
}

static void update_gptpsysc_status(avtpd_data_t *avtpd, uint8_t domainIndex)
{
	int gdi;
	uint32_t clock_state=0;
	void *value;
	gdi=ydbi_gptpinstdomain2dbinst_pt(ydbi_access_handle(), avtpd->gptpInstance, domainIndex);
	YDBI_GET_ITEM_VSUBST(uint32_t*, ptk3vk0, clock_state, value, gdi,
			     IEEE1588_PTP_TT_CLOCK_STATE, IEEE1588_PTP_TT_GMSTATE, 255,
			     YDBI_STATUS);
	if(clock_state==GMSYNC_SYNC || clock_state==GMSYNC_SYNC_STABLE){
		avtpdp->gptpinfo.uncertain=false;
	}else{
		avtpdp->gptpinfo.uncertain=true;
	}
	avtpgcfg_releasedb(avtpd->avtpgcd);
	return;
}

static int avtpd_mainloop(avtpd_data_t *avtpd)
{
	struct timespec ts;
	uint64_t ts64;
	int ci;

	update_gptpsysc_status(avtpd, 0);
	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+CLIENT_REQUEST_TIMEOUT, ts);
	if(CB_SEM_TIMEDWAIT(&avtpd->mcd->sem_request, &ts)){
		// avtpd->mcd->sem_request timed out
		dead_client_check(avtpd);
		return 0;
	}
	switch(avtpd->mcd->reqtype){
	case CCR_CONNECTION_REQUEST:
		if(!check_connection_exist(avtpd)){
			avtpd->mcd->ccres.result=AVTP_CCR_ARLEADY_EXIST;
			avtpd->mcd->ccres.ccd=-1;
		}else{
			if((ci=create_ccd(avtpd))<0) {
				if(ci==-1){return -1;}
				avtpd->mcd->ccres.result=AVTP_CCR_INVALID_REQUEST;
				avtpd->mcd->ccres.ccd=-1;
			}else{
				avtpd->mcd->ccres.result=AVTP_CCR_ACCEPTED;
				avtpd->mcd->ccres.ccd=ci;
			}
		}

		CB_SEM_POST(&avtpd->mcd->sem_response);
		return 0;
	case CCR_DISCONNECTION_REQUEST:
		ci=avtpd->mcd->ccr.ci;
		/*
		  check_ccd_thread is likely to delte the connection before this request.
		  return AVTP_CCR_ACCEPTED instead of AVTP_CCR_NOT_INTHELIST;
		*/
		avtpd->mcd->ccres.result=AVTP_CCR_ACCEPTED;
		switch(avtpd->mcd->ccr.con_mode){
		case AVTP_TALKER_CONNECTION:
		case AVTP_TALKONLY_CONTROL_CONNECTION:
		case AVTP_CRF_TALKER_CONNECTION:
			if(avtpd->tccds[ci]){
				delete_ccd(avtpd, avtpd->tccdl, avtpd->tccds,
					   TALKER_SHM_SUFFIX, ci);
			}else{
				UB_LOG(UBL_INFO,"tconnecion index = %d doesn't exist\n", ci);
			}
			CB_SEM_POST(&avtpd->mcd->sem_response);
			return 0;
		case AVTP_LISTENER_CONNECTION:
		case AVTP_CONTROL_CONNECTION:
		case AVTP_NTSCF_CONNECTION:
		case AVTP_TSCF_CONNECTION:
		case AVTP_CRF_LISTENER_CONNECTION:
			if(avtpd->lccds[ci]){
				delete_ccd(avtpd, avtpd->lccdl, avtpd->lccds,
					   LISTENER_SHM_SUFFIX, ci);
			}else{
				UB_LOG(UBL_INFO,"lconnecion index = %d doesn't exist\n", ci);
			}
			CB_SEM_POST(&avtpd->mcd->sem_response);
			return 0;
		default:
			UB_LOG(UBL_ERROR,"%s:unknown connection mode %d, ignore this\n",
			       __func__, avtpd->mcd->ccr.con_mode);
			return 0;
		}
	default:
		UB_LOG(UBL_ERROR,"%s:bad reqtype %d, ignore this\n",__func__,
		       avtpd->mcd->reqtype);
		return 0;
	}
}

#ifndef AVTPD_HAVE_NO_SIGNAL
#include <signal.h>
static void signal_handler(int sig)
{
	UB_LOG(UBL_INFO,"avtpd:%s:sig=%d\n",__func__,sig);
	stop_mainloop=1;
	return;
}

static void register_signal(void)
{
	struct sigaction sigact;
	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
}
#define AVTPD_REGISTER_SIGNAL register_signal()
#else
#define AVTPD_REGISTER_SIGNAL
#endif

#ifdef AVTPD_IN_LIBRARY
void avtpd_stop(void)
{
	stop_mainloop=1;
}
CB_SEM_T g_avtpd_ready_sem;
#define AVTPD_READY_NOTICE {\
	if(!stop_mainloop){\
		CB_SEM_POST(&g_avtpd_ready_sem);\
	}\
}
#define INTHREAD true
#else //!AVTPD_IN_LIBRARY
#define AVTPD_MAIN main
#define AVTPD_READY_NOTICE
#define INTHREAD false
#endif //!AVTPD_IN_LIBRARY

#ifdef AVTPC_WRAPPER_ENABLE
#include "avtpc_server.h"
#define AVTPC_SERVER_START {\
	if(avtpc_server_start(avtpdp->shsuf) < 0){\
		stop_mainloop = 1;}\
}
#define AVTPC_SERVER_STOP avtpc_server_stop()
#else //!AVTPC_WRAPPER_ENABLE
#define AVTPC_SERVER_START
#define AVTPC_SERVER_STOP
#endif //!AVTPC_WRAPPER_ENABLE

static void avtpd_debug_memory_fileout(char *prefix, uint16_t tsport)
{
	char *buf;
	int size;
	int fd;
	char fname[80];

	if(!prefix){return;}
	snprintf(fname, 80, "%s%d.log", prefix, tsport);
	fd=CB_OPEN(fname, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if(fd<0) {
		UB_LOG(UBL_ERROR, "%s:can't open file:%s\n", __func__, fname);
		return;
	}
	if(ubb_memory_out_alldata(&buf, &size)){return;}
	if(write(fd, buf, size)!=size){
		UB_LOG(UBL_ERROR, "%s:can't write into the file\n", __func__);
	}
	CB_CLOSE(fd);
	free(buf);
	return;
}

int AVTPD_MAIN(int argc, char *argv[])
{
	avtpd_data_t avtpd;
	unibase_init_para_t init_para;
	bool noubinit=false;
	int res=-1;
	uint8_t ready=1;
	if(argc>0 && (!strcmp(argv[argc-1],"--noubinit") || !strcmp(argv[argc-1],"-n"))){
		noubinit=true;
	}
	if(!noubinit){
		ubb_default_initpara(&init_para);
		init_para.ub_log_initstr=UBL_OVERRIDE_ISTR(
			"4,ubase:45,cbase:45,uconf:46,gptp:46,l2:46", "UBL_L2");
		unibase_init(&init_para);
		// start with zero, so that the memory is not allocated
		ubb_memory_out_init(NULL, 0);
	}
	UB_LOG(UBL_INFO, "avtpd: l2-"TSNPKGVERSION"\n");
	AVTPD_REGISTER_SIGNAL;
	memset(&avtpd, 0, sizeof(avtpd));
	avtpd.gptpinfo.ovr_uncertain=true;
	avtpd.gptpinfo.uncertain=true;
	avtpd.debugmem=DEFAULT_DEBUGMEM;
	avtpd.gptpInstance=0xff;
	if(set_options(&avtpd, argc, argv)<0){return 1;}
	if(avtpd.gptpInstance==0xff){avtpd.gptpInstance=avtpd.instanceIndex;}
	avtpd.avtpgcd=avtpgcfg_init(avtpd.instanceIndex, avtpd.dbname,
				    avtpd.configfiles, INTHREAD);
	if(avtpd.avtpgcd==NULL){return -1;}
	if(!noubinit){ubb_memory_out_init(NULL, avtpd.debugmem);}
	if(!avtpd.shsuf){avtpd.shsuf="";}
	// wait for gptp, read port_state of port=0, domain=0
	if(gptpgcfg_wait_gptpready(ydbi_access_handle(), avtpd.gptpInstance, 3000)){
		UB_LOG(UBL_ERROR, "%s:gptp is not ready in 3000ms\n", __func__);
		goto erexit;
	}
	avtpgcfg_releasedb(avtpd.avtpgcd);
	//wait gptpmaster clock becomes ready
	while(!stop_mainloop && gptpmasterclock_init(avtpd.gshmem)){CB_USLEEP(100000);}
	if(avtpd_init(&avtpd)){goto erexit;}
	UB_LOG(UBL_INFO, "avtpd actually started\n");
	avtpdp=&avtpd;
	AVTPC_SERVER_START;
	AVTPD_READY_NOTICE;
	YDBI_SET_ITEM(nyavtpk2vk1, avtpd.instanceIndex,
		      XL4_EXTMOD_XL4AVTP_AVTPD_READY, 255,
		      NULL, 0, YDBI_STATUS, &ready, 1, YDBI_NO_NOTICE);
	while(!stop_mainloop){
		ub_log_flush();
		if(avtpd_mainloop(&avtpd)) {
			UB_LOG(UBL_ERROR,"%s:avtpd_mainloop returned with an error\n",__func__);
			CB_USLEEP(100000); // sleep to avoid a busy loop
		}
		//UB_LOG(UBL_DEBUG,"%s:avtpd_mainloop is running\n",__func__);
	}
	ready=0;
	YDBI_SET_ITEM(nyavtpk2vk1, avtpd.instanceIndex,
		      XL4_EXTMOD_XL4AVTP_AVTPD_READY, 255,
		      NULL, 0, YDBI_STATUS, &ready, 1, YDBI_NO_NOTICE);
	UB_LOG(UBL_INFO,"avtpd going to close\n");
	AVTPC_SERVER_STOP;
	CB_THREAD_MUTEX_UNLOCK(&avtpd.mcd->mutex);
	check_ccd_thread(&avtpd, true);
	avtpd_close(&avtpd);
	res=0;
erexit:
	avtpgcfg_close(avtpd.avtpgcd);
	gptpmasterclock_close();
	if(!avtpd.dfilep){
		avtpd.dfilep=DEFAULT_DFILE_PREFIX;
	}else{
		if(!strcmp(avtpd.dfilep, "no")){avtpd.dfilep=NULL;}
	}
	if(avtpd.dfilep){avtpd_debug_memory_fileout(avtpd.dfilep, avtpd.tsport);}
	if(!noubinit){
		ubb_memory_out_close();
		unibase_close();
	}
	return res;
}
