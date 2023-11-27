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
#ifndef COMBASE_LINUX_H__
#define COMBASE_LINUX_H__

/**
 * Duplex values which are used by xl4 modules
 */
#define CB_DUPLEX_FULL		(1)
#define CB_DUPLEX_HALF		(2)
#define CB_DUPLEX_UNKNOWN	(3)

typedef struct cbl_linkstatus{
	ub_macaddr_t address;
	uint32_t speed;
	uint32_t duplex;
} cbl_linkstatus_t;

typedef struct cbl_preempt_status{
	uint32_t hold_advance; // holdAdvance parameter for the Port in nanoseconds
	uint32_t release_advance; // releaseAdvance parameter for the Port in nanoseconds
	uint8_t preempt_active; // 1:if supported and actve, 0:otherwise
	uint8_t hold_request; // 1:hold request 2:release reuquest has been issued
} cbl_preempt_status_t;

typedef struct cbl_cb_event{
	char ifname[CB_MAX_NETDEVNAME];
	uint32_t ifindex;
	uint32_t eventflags;
	bool notice; // true:notice, false:response to a query
	union{
		cbl_linkstatus_t linkst;
		cbl_preempt_status_t preempt;
	}u;
} cbl_cb_event_t;

/*
 42 is the sum of followings:
12 byte minimum inter-frame spacing
 8 byte preamble and SFD
 6 bytes of the DA
 6 bytes of the SA
 4 bytes of the VLAN tag
 2 bytes of the Ethertype
 4 bytes of the CRC
 * a bridge needs to add one byte
*/
#ifdef CBL_BRIDGE_MODE
#define CBL_L2_OVERHEAD 43
#else
#define CBL_L2_OVERHEAD 42
#endif

#define CBL_EVENT_DEVUP (1<<0)
#define CBL_EVENT_DEVDOWN (1<<1)
#define CBL_EVENT_CHECKENABLED (1<<2)
#define CBL_EVENT_TCHW_ENABLED (1<<3)
#define CBL_EVENT_TCHW_DISABLED (1<<4)
#define CBL_EVENT_CBS_ENABLED (1<<5)
#define CBL_EVENT_CBS_DISABLED (1<<6)
#define CBL_EVENT_TAS_ENABLED (1<<7)
#define CBL_EVENT_TAS_DISABLED (1<<8)
#define CBL_EVENT_PREEMPT_STATUS (1<<9)

enum {
	CBL_CAPABILITY_ETHSTATUS=0,
	CBL_CAPABILITY_CBS,
	CBL_CAPABILITY_TAS,
	CBL_CAPABILITY_PREEMPT,
};
#define CBL_CAPABILITY_FLAG(x) (1<<x)

/*
 * this callback function is called when a uchw message is received and it is
 * related to UCHW_EVENT_*
 * Both a response to a query and a notice from uchw event call this fuction.
 */
typedef int (*cbl_cb_t)(void *cbdata, cbl_cb_event_t *nevent);

typedef int (*cbl_extfdcb_t)(void *cbdata, int index, void *data, int size);

typedef struct combase_link_data combase_link_data_t;

/**
 * this structure is used to initialize traffic classes and queues
 */
typedef enum {
	CBL_ACTION_SET=0,
	CBL_ACTION_DEL,
	CBL_ACTION_DELSET,
} cbl_action_t;

typedef struct cbl_qmap_params{
	uint16_t handle;
	uint8_t num_tc; // number of traffic classes, num_lq==num_tc and not used.
	uint8_t pri_lq_map[8]; // priority to logical queue map
	int8_t pq_lq_map[8]; // physical queue to logical queue map, -1:no mapping
	uint8_t num_pq; // number of physical queues
	uint8_t hw; // 0:software, 1:hardware
}cbl_qmap_params_t;

typedef struct cbl_tcinit_params{
	cbl_action_t action;
	const char *ifname;
	cbl_qmap_params_t qmap;
} cbl_tcinit_params_t;

typedef struct cbl_cbs_params{
	cbl_action_t action;
	const char *ifname;
	uint16_t handle;
	int64_t idleslope; // idle slope, bps unit
	int64_t sendslope; // send slope, bps unit
	int64_t hicredit; // high credit, byte unit
	int64_t locredit; // low credit, byte unit
	uint8_t qindex; // physical queue index starts from 0
	uint8_t offload; // set 1 to enable HW offload
} cbl_cbs_params_t;

#define TAS_MAX_NUM_QUEUES (8)
typedef struct cbl_tas_sched_params_ext{
	//T.B.D.
	uint16_t max_sdu[TAS_MAX_NUM_QUEUES];
} cbl_tas_sched_params_ext_t;

typedef enum {
	CB_TAS_GATE_SET=0,
	CB_TAS_GATE_SET_HOLD,
	CB_TAS_GATE_SET_RELEASE,
} cbl_tas_gate_operation_t;

typedef struct cbl_tas_gate_cmd_entry{
	uint32_t operation; //cbl_tas_gate_operation_t
	uint32_t interval;
	uint8_t gate;
} cbl_tas_gate_cmd_entry_t;

typedef struct cbl_tas_sched_params{
	cbl_action_t action;
	const char *ifname;
	cbl_qmap_params_t qmap;
	uint64_t base_time_sec;
	uint32_t base_time_nsec;
	uint32_t cycle_time_numerator;
	uint32_t cycle_time_denominator;
	cbl_tas_sched_params_ext_t *extp;
	ub_esarray_cstd_t *entries;
} cbl_tas_sched_params_t;

// for preemption, there is no action,
// by prioiry_preempt, corresponding queue is set express or preemptable.
typedef struct cbl_preempt_params{
	const char *ifname;
	cbl_qmap_params_t qmap;
	//'frame-preemption-status-enum',1:express, 2:preemptable
	uint8_t prioiry_preempt[8];
} cbl_preempt_params_t;

typedef struct cbl_query_thread_data{
	combase_link_data_t *cbld;
	CB_SEM_T *sigp;
	bool running;
} cbl_query_thread_data_t;

/*
 * this layer's APIs are only for uc_hwal.c in uniconf.
 * The other layer should read the status in the uniconf DB.
 * When you are very sure this layer's APIs are needed without uniconf,
 * it is possible to use this layer's APIs.
 */

/**
 * @brief combase_link init
 * @param event_cb callback function
 * @param cb_arg callback argument
 * @return combase_link_data instance
 */
#define COMBASE_LINK_EXTFD_MAX 2
typedef struct combase_link_extfd{
	CB_SOCKET_T extfds[COMBASE_LINK_EXTFD_MAX];
	cbl_extfdcb_t extfdcb;
	void *extfdcb_arg;
} combase_link_extfd_t;
combase_link_data_t *combase_link_init(cbl_cb_t event_cb, void *cb_arg,
				       combase_link_extfd_t *extfdp);

/**
 * @brief combase_link close
 * @param cbld combase_link_data
 */
void combase_link_close(combase_link_data_t *cbld);

/**
 * @brief get cbl capability flags
 * @param cbld combase_link_data
 * @return capability flags
 */
uint32_t cbl_get_capabilities(combase_link_data_t *cbld);

/**
 * @brief catch HW events to process
 * @param cbl_query_thread_data_t pointer
 * @note don't process data inside the thread. just signal the semaphore by events.
 *	If polling actions are needed, make periodic siganl on the semaphore.
 *	The main thread should do polling.
 */
void *cbl_query_thread(void *ptr);

/**
 * @brief get query response
 * @param cbld combase_link_data
 * @param tout_ms timeout in ms
 * @return -1:error, 1:timeout, 0:got response
 */
int cbl_query_response(combase_link_data_t *cbld, int tout_ms);

/**
 * @brief Query link status
 * @param cbld combase_link_data
 * @param ifname interface name
 * @return -1:error, 0:later, 1:nevent is updated in the call
 * @note when the result is immediately available, it returns 1
 *       when the return is 0,
 *       the result will be notified later by the callback function.
 */
int cbl_query_linkstatus(combase_link_data_t *cbld, cbl_cb_event_t *nevent);

/**
 * @brief set up traffic class to queue mapping
 * @param cbld combase_link_data
 * @param tip traffic class mapping parameters
 * @return -1:error, 0:notice the completion later, 1:completed the setup
 * @note when the return is 0,
 *       the result will be notified later by the callback function.
 */
int cbl_tc_queue_map(combase_link_data_t *cbld, cbl_tcinit_params_t *tip);

/**
 * @brief set up credit based shaper
 * @param cbld combase_link_data
 * @param cbsp CBS parameters
 * @return -1:error, 0:success, 1:completed the setup
 */
int cbl_cbs_setup(combase_link_data_t *cbld, cbl_cbs_params_t *cbsp);

/**
 * @brief set up traffic aware shaper
 * @param cbld combase_link_data
 * @param ctsp TAS parameters
 * @return -1:error, 0:success, 1:completed the setup
 */
int cbl_tas_setup(combase_link_data_t *cbld, cbl_tas_sched_params_t *ctsp);

/**
 * @brief set up preemption
 * @param cbld combase_link_data
 * @param cpemp preemption parameters
 * @param nevent when the action is completed, return the result
 * @return -1:error, 0:success, 1:completed
 * @note the result will come to cbl_preempt_status_t preempt_active in the call back.
 */
int cbl_preempt_setup(combase_link_data_t *cbld, cbl_preempt_params_t *cpemp,
		      cbl_cb_event_t *nevent);

#endif
