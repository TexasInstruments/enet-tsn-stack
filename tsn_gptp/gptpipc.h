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
/**
 * @addtogroup gptp
 * @{
 * @file gptpipc.h
 * @brief Defines and methods for gPTP2.
 *
 * */

#ifndef __GPTPIPC_H_
#define __GPTPIPC_H_

#include "gptpbasetypes.h"
#include <stdint.h>

/**
 * @brief IPC node for gptp2 communication
 * This node is used to communicate with gptp2 internally.
 *
 * */
#define GPTP2D_IPC_CB_SOCKET_NODE "/tmp/gptp2d_ipc"


/**
 * @brief command for gptp ipc.
 */
typedef enum {
	GPTPIPC_CMD_NOP=0,
	GPTPIPC_CMD_REQ_NDPORT_INFO,
	GPTPIPC_CMD_REQ_GPORT_INFO,
	GPTPIPC_CMD_REQ_CLOCK_INFO,
	GPTPIPC_CMD_ACTIVE_DOMAINT_SWITCH,
	GPTPIPC_CMD_RUN_EXT_SCRIPT,
	GPTPIPC_CMD_TSN_SCHEDULE_CONTROL,
	GPTPIPC_CMD_REQ_STAT_INFO,
	GPTPIPC_CMD_REQ_STAT_INFO_RESET,
	GPTPIPC_CMD_REG_ABNORMAL_EVENT,
	GPTPIPC_CMD_DISCONNECT,
} gptp_ipc_command_t;

#define GPTPIPC_EXT_SCRIPT "gptpipc_extscript"

/**
 * @brief Enumeration for GPTP IPC events.
 */
typedef enum {
	GPTPIPC_EVENT_CLOCK_NETDEV_DOWN = 0,
	GPTPIPC_EVENT_CLOCK_NETDEV_UP,
	GPTPIPC_EVENT_CLOCK_PHASE_UPDATE,
	GPTPIPC_EVENT_CLOCK_FREQ_UPDATE,
	GPTPIPC_EVENT_CLOCK_GM_SYNCED,
	GPTPIPC_EVENT_CLOCK_GM_UNSYNCED,
	GPTPIPC_EVENT_CLOCK_GM_CHANGE,
	GPTPIPC_EVENT_PORT_AS_CAPABLE_DOWN,
	GPTPIPC_EVENT_PORT_AS_CAPABLE_UP,
	GPTPIPC_EVENT_CLOCK_ACTIVE_DOMAIN,
	GPTPIPC_EVENT_CLOCK_TIMELEAP_FUTURE,
	GPTPIPC_EVENT_CLOCK_TIMELEAP_PAST,
} gptpipc_event_t;

/**
 * @brief Macro which is used to define event flag. it is set using bitwise operation.
 */
#define GPTPIPC_EVENT_CLOCK_FLAG_NETDEV_DOWN (1<<GPTPIPC_EVENT_CLOCK_NETDEV_DOWN)
#define GPTPIPC_EVENT_CLOCK_FLAG_NETDEV_UP (1<<GPTPIPC_EVENT_CLOCK_NETDEV_UP)
#define GPTPIPC_EVENT_CLOCK_FLAG_PHASE_UPDATE (1<<GPTPIPC_EVENT_CLOCK_PHASE_UPDATE)
#define GPTPIPC_EVENT_CLOCK_FLAG_FREQ_UPDATE (1<<GPTPIPC_EVENT_CLOCK_FREQ_UPDATE)
#define GPTPIPC_EVENT_CLOCK_FLAG_GM_SYNCED (1<<GPTPIPC_EVENT_CLOCK_GM_SYNCED)
#define GPTPIPC_EVENT_CLOCK_FLAG_GM_UNSYNCED (1<<GPTPIPC_EVENT_CLOCK_GM_UNSYNCED)
#define GPTPIPC_EVENT_CLOCK_FLAG_GM_CHANGE (1<<GPTPIPC_EVENT_CLOCK_GM_CHANGE)
#define GPTPIPC_EVENT_PORT_FLAG_AS_CAPABLE_DOWN (1<<GPTPIPC_EVENT_PORT_AS_CAPABLE_DOWN)
#define GPTPIPC_EVENT_PORT_FLAG_AS_CAPABLE_UP (1<<GPTPIPC_EVENT_PORT_AS_CAPABLE_UP)
#define GPTPIPC_EVENT_CLOCK_FLAG_ACTIVE_DOMAIN (1<<GPTPIPC_EVENT_CLOCK_ACTIVE_DOMAIN)
#define GPTPIPC_EVENT_CLOCK_FLAG_TIMELEAP_FUTURE (1<<GPTPIPC_EVENT_CLOCK_TIMELEAP_FUTURE)
#define GPTPIPC_EVENT_CLOCK_FLAG_TIMELEAP_PAST (1<<GPTPIPC_EVENT_CLOCK_TIMELEAP_PAST)

/**
 * @brief gptp notice data type
 */
typedef struct gptpipc_notice_data {
	uint32_t event_flags;
	int32_t domainNumber;
	int32_t domainIndex;
	int32_t portIndex;
	ClockIdentity gmIdentity;
	int64_t lastGmPhaseChange_nsec;
	uint16_t gmTimeBaseIndicator;
	uint8_t lastGmFreqChangePk[sizeof(double)];
} __attribute__((packed)) gptpipc_notice_data_t;

/**
 * @brief gptp ipc client request, register/deregister abnormal event.
 *  1. subcmd -> 0: register, 1:deregister
 *  2. msgtype -> use the same number as PTPMsgType, -1 means all types for deregister subcmd
 *  3. eventtype -> use the same number as md_abn_event_type
 *  4. eventrate -> rate of event happening, 0.0 to 1.0
 *  5. repeat -> repeat times of the event
 *  6. interval -> interval times whtn it has repeat number
 *  7. eventpara -> integer parameter for the event
 */
typedef struct gptpipc_client_req_abnormal {
	int32_t subcmd;
	int32_t msgtype;
	int32_t eventtype;
	float eventrate;
	int32_t repeat;
	int32_t interval;
	int32_t eventpara;
} __attribute__((packed)) gptpipc_client_req_abnormal_t;

/**
 * @brief gptp ipc client request data. this structure formed with following details:
 *  1. cmd -> it is used for give command to IPC.
 *  2. domainNumber -> integer used for domain number.
 *  3. domainIndex -> integer used for domain index.
 *  4. portNumber -> integer used for port index.
 * @see gptp_ipc_command_t
 */
typedef struct gptpipc_client_req_data {
	gptp_ipc_command_t cmd;
	// if domainNumber==-1, use domainIndex; if domainIndex==-1, use domainNumber
	// if domainNumber==-1 and domainIndex==-1, all domains info is requested
	int32_t domainNumber;
	int32_t domainIndex;
	// for ndprot, if portIndex=0, all ports info is requested. domain is always 0
	// for gprot, if portIndex=0, all ports info for a specific domain or all domains
	int32_t portIndex;
	union {
		gptpipc_client_req_abnormal_t abnd;
	}u;
} __attribute__((packed)) gptpipc_client_req_data_t;

/**
 * @brief gptp data for netlink. this structure is formed with following details:
 *  1. up -> true or false, used to know that device is up or not.
 *  2. devname -> character array is used to hold device name (interface name i.e eth0,... ).
 *  3. ptpdev -> array used to hold ptp device name.
 *  4. speed -> integer used to notify speed.
 *  5. duplex -> integer used to notify type of communication.
 *  6. portid -> ClockIdentity is used to hold port identity number.
 */
#define GPTPIPC_MAX_NETDEV_NAME 16
#define GPTPIPC_MAX_PTPDEV_NAME 32
typedef struct gptpipc_data_netlink {
	uint32_t speed;
	uint32_t duplex;
	ClockIdentity portid;
	uint8_t up;
	char devname[GPTPIPC_MAX_NETDEV_NAME];
	char ptpdev[GPTPIPC_MAX_PTPDEV_NAME];
} __attribute__((packed)) gptpipc_data_netlink_t;

/**
 * @brief to retreive netlink status. this structure has nlstatus type
 * of gptpipc_data_netlinnk_t. for more details read
 * @see gptpipc_data_netlink_t
 *
 */
typedef struct gptpipc_ndport_data {
	gptpipc_data_netlink_t nlstatus;
} __attribute__((packed)) gptpipc_ndport_data_t;

/**
 * @brief grand master port data. this structure formed with following details:
 *  1. domainNumber -> used to hold domain number of clock.
 *  2. portIndex -> integer used to hold port index of clock.
 *  3. asCapable -> this port in tis time-awaer system can
 *  interoperate to the other end of port via the IEEE 802.1AS
 *  protocol.
 *  4. portOper -> True if the port is up and able to send and receive messages.
 *  5. gmClockId -> If gmPresent is TRUE, gmClockId is the
 *  ClockIdentity of the current grandmaster. If gmPresent is FALSE,
 *  the value of gmIdentity is 0x0.
 *  6. annPathSequenceCount -> number of path sequence to grand master
 *  7. annPathSequence -> array of path sequence to grand master
 */
typedef struct gptpipc_gport_data {
	int32_t domainNumber;
	int32_t portIndex;
	ClockIdentity gmClockId;
	uint8_t asCapable;
	uint8_t portOper;
	uint8_t gmStable;
	uint8_t selectedState;
	uint64_t pDelay;
	double pDelayRateRatio;
	uint8_t annPathSequenceCount;
	ClockIdentity annPathSequence[MAX_PATH_TRACE_N];
} __attribute__((packed)) gptpipc_gport_data_t;

/**
 * @brief gptp clock data.
 *  1. domainNumber -> The domain number of a gPTP domain
 *  2. portIndex -> index number of the clock port
 *  3. lastGmPhaseChange_nsec -> the most recent change in timeBaseIndicator
 *  4. clockId -> the clock identiy of this clock port
 *  5. gmClockId -> the clock identiy of the current grand master clock
 *  6. lastSyncReceiptTime_nsec ->
 *  7. lastSyncReceiptLocalTime_nsec ->
 *  8. adjppb -> adjustment value
 *  9. gmTimeBaseIndicator -> timeBaseIndicator of the current grand master
 *  10. lastSyncSeqID -> sequence number of the last Sync/FollowUp received
 *  11. gmsync -> True if synchronized to grand master
 *  12. domainActive -> True if this domain is providing the gptp clock
 *  13. lastGmFreqChangePk -> the last
 */
typedef struct gptpipc_clock_data{
	int32_t domainNumber;
	int32_t portIndex;
	int64_t lastGmPhaseChange_nsec;
	ClockIdentity clockId;
	ClockIdentity gmClockId;
	uint64_t lastSyncReceiptTime_nsec;
	uint64_t lastSyncReceiptLocalTime_nsec;
	int32_t adjppb;
	uint16_t gmTimeBaseIndicator;
	uint16_t lastSyncSeqID;
	uint8_t gmsync;
	uint8_t domainActive;
	uint8_t lastGmFreqChangePk[sizeof(double)];
} __attribute__((packed)) gptpipc_clock_data_t;

typedef struct gptpipc_statistics_system{
	int32_t portIndex;
	uint32_t pdelay_req_send;
	uint32_t pdelay_resp_rec;
	uint32_t pdelay_resp_rec_valid;
	uint32_t pdelay_resp_fup_rec;
	uint32_t pdelay_resp_fup_rec_valid;
	uint32_t pdelay_req_rec;
	uint32_t pdelay_req_rec_valid;
	uint32_t pdelay_resp_send;
	uint32_t pdelay_resp_fup_send;
} __attribute__((packed)) gptpipc_statistics_system_t;

typedef struct gptpipc_statistics_tas{
	int32_t domainNumber;
	int32_t portIndex;
	uint32_t sync_send;
	uint32_t sync_fup_send;
	uint32_t sync_rec;
	uint32_t sync_rec_valid;
	uint32_t sync_fup_rec;
	uint32_t sync_fup_rec_valid;
	uint32_t announce_send;
	uint32_t announce_rec;
	uint32_t announce_rec_valid;
	uint32_t signal_msg_interval_send;
	uint32_t signal_gptp_capable_send;
	uint32_t signal_rec;
	uint32_t signal_msg_interval_rec;
	uint32_t signal_gptp_capable_rec;
} __attribute__((packed)) gptpipc_statistics_tas_t;

/**
 * @brief data from gptp2d to connected clients
 */
typedef enum {
	GPTPIPC_GPTPD_NOTICE = 0,
	GPTPIPC_GPTPD_NDPORTD,
	GPTPIPC_GPTPD_GPORTD,
	GPTPIPC_GPTPD_CLOCKD,
	GPTPIPC_GPTPD_STATSD,
	GPTPIPC_GPTPD_STATTD,
} gptpd_data_type_t;

/**
 * @brief data type to be used in IPC.
 */
typedef struct gptpipc_gptpd_data{
	gptpd_data_type_t dtype;
	union {
		gptpipc_notice_data_t notice;
		gptpipc_ndport_data_t ndportd;
		gptpipc_gport_data_t gportd;
		gptpipc_clock_data_t clockd;
		gptpipc_statistics_system_t statsd;
		gptpipc_statistics_tas_t stattd;
	}u;
} __attribute__((packed)) gptpipc_gptpd_data_t;

typedef int (*gptpipc_cb_t)(gptpipc_gptpd_data_t *ipcrd, void *cb_data);

/**
 * @brief variables to run gptp ipc thread
 *   1. ipcthread -> thread variable
 *   2. ipcstop -> used to stop ipc thread.
 *   3. pname -> ipc socket node suffix
 *   4. query_interval -> issue IPC requests in this interval time(msec). if 0, no request.
 *   5. printdata -> if true, print the IPC data on cosole
 *   6. ipcfd -> socket descriptor for IPC. if 0, open new socket.
 *   7. cb -> callback function
 *   8. cbdata -> callback data
 *   9. udpport -> udp port number to use UDP for IPC. if 0, use domain socket.
 *   10. udpaddr -> udp address to use local UDP for IPC. if NULL, use 127.0.0.1
 */
typedef struct gptpipc_thread_data{
	CB_THREAD_T ipcthread;
	int ipcstop;
	char *pname;
	int query_interval; //in msec
	bool printdata;
	int ipcfd;
	gptpipc_cb_t cb;
	void *cbdata;
	int udpport;
	char *udpaddr;
} gptpipc_thread_data_t;

/**
 * @brief request IPC data
 * @param ipcfd socket descriptor of ipc.
 * @param domainNumber domain number.
 * @param portIndex clockport index or network device port index.
 * @param cmd command for IPC
 * @see gptp_ipc_command_t
 * @return 0 on success, -1 on Failure.
 *
 */
int send_ipc_request(int ipcfd, int domainNumber, int portIndex, gptp_ipc_command_t cmd);

/**
 * @brief run gptp2d ipc thread
 * @result 0:success, -1:error
 * @param ipctd	parameters to run IPC thread, to get callback set 'cb' and 'cbdata'
 * @param wait_toutsec	0:immediately returns regardless connection status,
 *			-1:forever wait to get connetction
 *			positive:wait for this seconds, -1 is returned with timetout
 */
int gptpipc_init(gptpipc_thread_data_t *ipctd, int wait_toutsec);

/**
 * @brief close IPC thread.
 * @param *ipctd reference to gptpipc_thread_data_t.
 * @return 0 on success.
 * @see gptpipc_thread_data_t
 *
 */
int gptpipc_close(gptpipc_thread_data_t *ipctd);

#endif
/** @}*/
