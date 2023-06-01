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
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "gptpbasetypes.h"
#include "ll_gptpsupport.h"
#include "gptpipc.h"

static char *selectedStateStr(uint8_t selectedState)
{
	switch(selectedState){
	case DisabledPort: return "Disabled";
	case MasterPort: return "Master";
	case PassivePort: return "Passive";
	case SlavePort: return "Slave";
	default: return "Unknonw";
	}
}

static void print_ipc_data(gptpipc_gptpd_data_t *rd)
{
	char *duplex="unknown";
	char astr[64];
	int i;
	double lastGmFreqChange;

	switch(rd->dtype){
	case GPTPIPC_GPTPD_NOTICE:
		printf("GPTPD_NOTICE domainNumber=%"PRIi32" portIndex=%"PRIi32" flags=0x%04X  ",
		       rd->u.notice.domainNumber, rd->u.notice.portIndex,
		       (unsigned int)rd->u.notice.event_flags);

		if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_NETDEV_DOWN){
			printf("NETDEV_DOWN ");
		}else if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_NETDEV_UP){
			printf("NETDEV_UP ");
		}else{}

		if(rd->u.notice.event_flags & GPTPIPC_EVENT_PORT_FLAG_AS_CAPABLE_DOWN){
			printf("AS_CAPABLE_DOWN ");
		}else if(rd->u.notice.event_flags & GPTPIPC_EVENT_PORT_FLAG_AS_CAPABLE_UP){
			printf("AS_CAPABLE_UP ");
		}else{}

		if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_PHASE_UPDATE){
			printf("PHASE_UPDATE ");
			printf("PhaseChange=%"PRIi64"sec %"PRIi64"nsec ",
			       rd->u.notice.lastGmPhaseChange_nsec/1000000000,
			       rd->u.notice.lastGmPhaseChange_nsec%1000000000);
		}

		if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_FREQ_UPDATE){
			memcpy(&lastGmFreqChange, &rd->u.notice.lastGmFreqChangePk, sizeof(double));
			printf("FREQ_UPDATE ");
			printf("FreqChange=%.9f ", lastGmFreqChange);
		}

		if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_GM_SYNCED){
			printf("GM_SYNC ");
		}else if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_GM_UNSYNCED){
			printf("GM_UNSYNC ");
		}else{}
		if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_GM_CHANGE){
			printf("GM_CHANGE new GM="UB_PRIhexB8,
			       UB_ARRAY_B8(rd->u.notice.gmIdentity));
		}

		if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_TIMELEAP_FUTURE){
			printf("TIMELEAP_FUTURE ");
		}else if(rd->u.notice.event_flags & GPTPIPC_EVENT_CLOCK_FLAG_TIMELEAP_PAST){
			printf("TIMELEAP_PAST ");
		}else{}

		printf("\n");
		break;
	case GPTPIPC_GPTPD_NDPORTD:
		printf("GPTPD_PORTD ");
		if(rd->u.ndportd.nlstatus.up){printf("UP ");}
		else
			printf("DOWN ");
		if(rd->u.ndportd.nlstatus.duplex==1){duplex="full";}
		else if(rd->u.ndportd.nlstatus.duplex==2){duplex="half";}

		printf("%s speed=%"PRIu32" duplex=%s portid=%s\n", rd->u.ndportd.nlstatus.devname,
		       rd->u.ndportd.nlstatus.speed, duplex,
		       ub_bsid2ssid(rd->u.ndportd.nlstatus.portid, astr));
		break;
	case GPTPIPC_GPTPD_CLOCKD:
		printf("GPTPD_CLOCKD ");
		printf("domainNumber=%"PRIi32" portIndex=%"PRIi32" ",
		       rd->u.clockd.domainNumber, rd->u.clockd.portIndex);
		if(rd->u.clockd.gmsync){
			printf("GM_SYNC\n");
		}else{
			printf("GM_UNSYNC\n");
		}
		memcpy(&lastGmFreqChange, &rd->u.clockd.lastGmFreqChangePk, sizeof(double));
		printf("  gmTimeBaseIndicator=%u lastGmPhaseChange=%"PRIi64"\n"
			   "  lastSyncSeqID=%d lastSyncReceiptTime_nsec=%"PRIu64" lastSyncReceiptLocalTime_nsec=%"PRIu64"\n"
		       ", lastGmFreqChange=%.9f adjfreq=%"PRIi32"ppb\n",
		       rd->u.clockd.gmTimeBaseIndicator, rd->u.clockd.lastGmPhaseChange_nsec,
			   rd->u.clockd.lastSyncSeqID, rd->u.clockd.lastSyncReceiptTime_nsec,
			   rd->u.clockd.lastSyncReceiptLocalTime_nsec,
			   lastGmFreqChange, rd->u.clockd.adjppb);
		break;
	case GPTPIPC_GPTPD_GPORTD:
		printf("GPTPD_GPORTD ");
		printf("domainNumber=%"PRIi32" portIndex=%"PRIi32" asCapable=%s gmStable=%s state=%s\n"
		       "  pDelay=%"PRIu64" pDelayRateRatio=%f\n",
		       rd->u.gportd.domainNumber, rd->u.gportd.portIndex,
		       rd->u.gportd.asCapable?"True":"False",
		       rd->u.gportd.gmStable?"True":"False",
		       selectedStateStr(rd->u.gportd.selectedState),
		       rd->u.gportd.pDelay,
		       rd->u.gportd.pDelayRateRatio);
		if(!rd->u.gportd.asCapable){
			printf("\n");
			break;
		}
		printf("  GM="UB_PRIhexB8"\n", UB_ARRAY_B8(rd->u.gportd.gmClockId));
		for(i=0;i<rd->u.gportd.annPathSequenceCount;i++){
			if(i>=MAX_PATH_TRACE_N){break;}
			printf("  path trace %d: "UB_PRIhexB8"\n",
			       i+1, UB_ARRAY_B8(rd->u.gportd.annPathSequence[i]));
		}
		break;
	case GPTPIPC_GPTPD_STATSD:
		printf("GPTPD_STATSD --- portIndex=%"PRIi32"\n", rd->u.statsd.portIndex);
		printf("pdelay_req_send=%"PRIu32"\n", rd->u.statsd.pdelay_req_send);
		printf("pdelay_resp_rec=%"PRIu32"\n", rd->u.statsd.pdelay_resp_rec);
		printf("pdelay_resp_rec_valid=%"PRIu32"\n", rd->u.statsd.pdelay_resp_rec_valid);
		printf("pdelay_resp_fup_rec=%"PRIu32"\n", rd->u.statsd.pdelay_resp_fup_rec);
		printf("pdelay_resp_fup_rec_valid=%"PRIu32"\n", rd->u.statsd.pdelay_resp_fup_rec_valid);
		printf("pdelay_req_rec=%"PRIu32"\n", rd->u.statsd.pdelay_req_rec);
		printf("pdelay_req_rec_valid=%"PRIu32"\n", rd->u.statsd.pdelay_req_rec_valid);
		printf("pdelay_resp_send=%"PRIu32"\n", rd->u.statsd.pdelay_resp_send);
		printf("pdelay_resp_fup_send=%"PRIu32"\n", rd->u.statsd.pdelay_resp_fup_send);
		break;
	case GPTPIPC_GPTPD_STATTD:
		printf("GPTPD_STATTD --- domainNumber=%"PRIi32" portIndex=%"PRIi32"\n",
		       rd->u.stattd.domainNumber, rd->u.stattd.portIndex);
		printf("sync_send=%"PRIu32"\n", rd->u.stattd.sync_send);
		printf("sync_fup_send=%"PRIu32"\n", rd->u.stattd.sync_fup_send);
		printf("sync_rec=%"PRIu32"\n", rd->u.stattd.sync_rec);
		printf("sync_rec_valid=%"PRIu32"\n", rd->u.stattd.sync_rec_valid);
		printf("sync_fup_rec=%"PRIu32"\n", rd->u.stattd.sync_fup_rec);
		printf("sync_fup_rec_valid=%"PRIu32"\n", rd->u.stattd.sync_fup_rec_valid);
		printf("signal_msg_interval_send=%"PRIu32"\n", rd->u.stattd.signal_msg_interval_send);
		printf("signal_gptp_capable_send=%"PRIu32"\n", rd->u.stattd.signal_gptp_capable_send);
		printf("signal_rec=%"PRIu32"\n", rd->u.stattd.signal_rec);
		printf("signal_msg_interval_rec=%"PRIu32"\n", rd->u.stattd.signal_msg_interval_rec);
		printf("signal_gptp_capable_rec=%"PRIu32"\n", rd->u.stattd.signal_gptp_capable_rec);
		break;
	default:
		printf("unknonw data\n");
		break;
	}
	fflush(stdout);
}

#define IPC_TOUT_MS 100
int send_ipc_request(int ipcfd, int domainNumber, int portIndex, gptp_ipc_command_t cmd)
{
	gptpipc_client_req_data_t cd;
	int res;
	memset(&cd, 0, sizeof(cd));
	cd.domainNumber=domainNumber;
	cd.domainIndex=-1;
	cd.portIndex=portIndex;
	cd.cmd=cmd;
	res=write(ipcfd, &cd, sizeof(cd));
	if(res!=sizeof(cd)){
		UB_LOG(UBL_ERROR, "%s:can't send to the IPC socket\n",__func__);
		return -1;
	}
	return 0;
}

static int gptpipc_socket_open(gptpipc_thread_data_t *ipcpd)
{
	if(!ipcpd->udpport){
		return cb_ipcsocket_init(&ipcpd->ipcfd, GPTP2D_IPC_CB_SOCKET_NODE,
					 ipcpd->pname, GPTP2D_IPC_CB_SOCKET_NODE);
	}else{
		if(!ipcpd->udpaddr){
			return cb_ipcsocket_udp_init(&ipcpd->ipcfd, "127.0.0.1",
						     "127.0.0.1", ipcpd->udpport);
		}
		return cb_ipcsocket_udp_init(&ipcpd->ipcfd, NULL,
					     ipcpd->udpaddr, ipcpd->udpport);
	}
}

static void *gptpipc_thread_proc(void *ptr)
{
	int res;
	gptpipc_thread_data_t *ipcpd=(gptpipc_thread_data_t *)ptr;
	gptpipc_gptpd_data_t rd;
	bool get_response = false;
	int timeout_count = 0;
	int suppress_msg = 0;

	while(!ipcpd->ipcstop){
		if(!CB_SOCKET_VALID(ipcpd->ipcfd)){
			if(suppress_msg){ub_log_change(CB_COMBASE_LOGCAT, UBL_NONE, UBL_NONE);}
			if(gptpipc_socket_open(ipcpd)) {
				ipcpd->ipcfd=CB_SOCKET_INVALID_VALUE;
				UB_LOG(UBL_DEBUG, "cann't connect to gptp2d, "
				       "wait 100msec and try again\n");
				usleep(100000);
				if(suppress_msg){ub_log_return(CB_COMBASE_LOGCAT);}
				suppress_msg=1;
				continue;
			}
			if(suppress_msg){ub_log_return(CB_COMBASE_LOGCAT);}
			suppress_msg=0;
		}
		if(!get_response){
			if(send_ipc_request(ipcpd->ipcfd, 0, 1,
					    GPTPIPC_CMD_REQ_NDPORT_INFO)){goto reinit;}
			if(send_ipc_request(ipcpd->ipcfd, 0, 1,
					    GPTPIPC_CMD_REQ_GPORT_INFO)){goto reinit;}
			if(send_ipc_request(ipcpd->ipcfd, 0, 0,
					    GPTPIPC_CMD_REQ_CLOCK_INFO)){goto reinit;}
			if(send_ipc_request(ipcpd->ipcfd, 0, 1,
					    GPTPIPC_CMD_REQ_CLOCK_INFO)){goto reinit;}
		}
		res=cb_fdread_timeout(ipcpd->ipcfd, &rd, sizeof(rd), IPC_TOUT_MS);
		if(ipcpd->ipcstop){break;}
		if(res<0){goto reinit;}
		if(res==0) {
			if(ipcpd->query_interval &&
			   ++timeout_count > ipcpd->query_interval/IPC_TOUT_MS){
				get_response=false;
			}
			continue;
		}
		if(res!=sizeof(rd)){
			UB_LOG(UBL_WARN, "%s:ipc received wrong size=%d\n", __func__, res);
			continue;
		}
		timeout_count=0;
		get_response=true;
		if(ipcpd->printdata){print_ipc_data(&rd);}
		switch(rd.dtype){
		case GPTPIPC_GPTPD_NOTICE:
			UB_LOG(UBL_DEBUG,"%s:gptp IPC NOTICE\n", __func__);
			if(ipcpd->cb){ipcpd->cb(&rd, ipcpd->cbdata);}
			break;
		case GPTPIPC_GPTPD_NDPORTD:
			UB_LOG(UBL_DEBUG,"%s:gptp IPC NDPORTD response\n", __func__);
			if(ipcpd->cb){ipcpd->cb(&rd, ipcpd->cbdata);}
			break;
		case GPTPIPC_GPTPD_GPORTD:
			UB_LOG(UBL_DEBUG,"%s:gptp IPC GPORTD response\n", __func__);
			if(ipcpd->cb){ipcpd->cb(&rd, ipcpd->cbdata);}
			break;
		case GPTPIPC_GPTPD_CLOCKD:
			UB_LOG(UBL_DEBUG,"%s:gptp IPC CLOCKD response\n", __func__);
			if(ipcpd->cb){ipcpd->cb(&rd, ipcpd->cbdata);}
			break;
		case GPTPIPC_GPTPD_STATSD:
			UB_LOG(UBL_DEBUG,"%s:gptp IPC STATSD response\n", __func__);
			if(ipcpd->cb){ipcpd->cb(&rd, ipcpd->cbdata);}
			break;
		case GPTPIPC_GPTPD_STATTD:
			UB_LOG(UBL_DEBUG,"%s:gptp IPC STATTD response\n", __func__);
			if(ipcpd->cb){ipcpd->cb(&rd, ipcpd->cbdata);}
			break;
		}
		continue;
	reinit:
		UB_LOG(UBL_ERROR, "%s:ipcfd error, try to reconnect\n",__func__);
		close(ipcpd->ipcfd);
		ipcpd->ipcfd=CB_SOCKET_INVALID_VALUE;
		continue;
	}
	if(CB_SOCKET_VALID(ipcpd->ipcfd)) {
		if(ipcpd->udpport){
			cb_ipcsocket_close(ipcpd->ipcfd, NULL, NULL);
		}else{
			cb_ipcsocket_close(ipcpd->ipcfd, GPTP2D_IPC_CB_SOCKET_NODE, ipcpd->pname);
		}
	}
	ipcpd->ipcfd=CB_SOCKET_INVALID_VALUE;
	UB_LOG(UBL_INFO, "closing gptpipc_thread_proc\n");
	return NULL;
}

#if defined(GHINTEGRITY)
#define THREAD_NORM_STACK 0
#define THREAD_SAME_PRI 190
#else
#define THREAD_NORM_STACK 0
#define THREAD_SAME_PRI 0
#endif
int gptpipc_init(gptpipc_thread_data_t *ipctd, int wait_toutsec)
{
	int tcount=0;
	const char *thread_name="avtpd_gptpd_ipc_thread";
	cb_tsn_thread_attr_t attr;

	ipctd->ipcfd=CB_SOCKET_INVALID_VALUE;
	cb_tsn_thread_attr_init(&attr, THREAD_SAME_PRI, THREAD_NORM_STACK, thread_name);
	if(CB_THREAD_CREATE(&ipctd->ipcthread, &attr, gptpipc_thread_proc, ipctd)){
		UB_LOG(UBL_ERROR,"%s:CB_THREAD_CREATE, %s\n", __func__, strerror(errno));
		return -1;
	}
	if(wait_toutsec){
		while(!CB_SOCKET_VALID(ipctd->ipcfd)) {
			usleep(100000);
			tcount+=100;
			if(wait_toutsec>0 && tcount>wait_toutsec*1000){return -1;}
		}
	}
	return 0;
}

int gptpipc_close(gptpipc_thread_data_t *ipctd)
{
	ipctd->ipcstop=true;
	CB_THREAD_JOIN(ipctd->ipcthread, NULL);
	ipctd->ipcstop=false; // set to 'false' for the next connection
	return 0;
}
