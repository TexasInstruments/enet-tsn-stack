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
#ifndef __MRP_DATATYPES_H_
#define __MRP_DATATYPES_H_

#include <limits.h>
#include <string.h>
#include <tsn_combase/combase.h>

typedef enum {
 	MRP_MMRP = 0,
 	MRP_MVRP,
 	MRP_MSRP,
} protocol_index_t;
#define NUM_PROTOCOLS 3

/* Attribute Data type */
#define MAX_ATTRIBUTE_NUM 4096

#ifndef XMRPD_MAX_PORT_NUM
#define XMRPD_MAX_PORT_NUM 8
#endif

//Registrar states (see 10.6):
typedef enum {
	RSTATE_IN, // In
	RSTATE_LV, // Leaving
	RSTATE_MT, // Empty
	RSTATE_LAST_NUM,
} registrar_state_t;

//Applicant and Simple-Applicant states (see 10.6):
typedef enum {
	ASTATE_VO = 0, // Very anxious Observer
	ASTATE_VP, // Very anxious Passive
	ASTATE_VN, // Very anxious New
	ASTATE_AN, // Anxious New
	ASTATE_AA, // Anxious Active
	ASTATE_QA, // Quiet Active
	ASTATE_LA, // Leaving Active
	ASTATE_AO, // Anxious Observer
	ASTATE_QO, // Quiet Observer
	ASTATE_AP, // Anxious Passive
	ASTATE_QP, // Quiet Passive
	ASTATE_LO, // Leaving Observer
	ASTATE_LAST_NUM,
} applicant_state_t;

typedef struct attrvalue {
	uint8_t attrtype;
	uint8_t apstate; // applicant_state_t
	uint8_t regstate; // registrar_state_t
	int8_t tpe; // next event, attribute event is mapped on protocol event
	int8_t fpe; // four packed event for the next transmit
	uint16_t nofv; // NumberOfValues info to compose a pcked packet
	// when multiple 'attrstate' creates one packed pcaket with NumberOfValues,
	// it has the same 'create_ts' and different 'fvindex'
	uint8_t fvindex;
	uint64_t create_ts; // the time of creation of 'attrstate', used as ID
	uint8_t fvd_size;
	void* fvd;
} attrvalue_t;

typedef enum {
	AEV_NoEvent = -1,
	AEV_New = 0,
	AEV_JoinIn,
	AEV_In,
	AEV_JoinMt,
	AEV_Mt,
	AEV_Lv,
	AEV_LAST_NUM,
} mrpdu_attribute_event_t;

/* MMRP Data types */
typedef enum {
	MMRP_None,
	MMRP_MacVector,
	MMRP_ServiceReq,
} mmrp_attributetype_t;

typedef union mmrp_firstvalue_data{
	ub_bytearray6_t mac;
	uint8_t srvreq;
} mmrp_firstvalue_data_t;

/* MVRP Data types */
typedef enum {
	MVRP_None,
	MVRP_AttrTypeValue = 1,
} mvrp_attributetype_t;

typedef union mvrp_firstvalue_data{
	uint16_t vid;
} __attribute__((packed)) mvrp_firstvalue_data_t;

/* MSRP Data types */
typedef enum {
	MSRP_None,
	MSRP_TalkerAdvertise = 1,
	MSRP_TalkerFailed,
	MSRP_Listener,
	MSRP_Domain,
} msrp_attributetype_t;

typedef enum {
	MSRP_FP_Ignore,
	MSRP_FP_AskingFailed,
	MSRP_FP_Ready,
	MSRP_FP_ReadyFailed,
} msrp_fpevent_t;

#define MSRP_SRClassID_A 6
#define MSRP_SRClassID_B 5
#define MSRP_Priority_ClassA 3
#define MSRP_Priority_ClassB 2
#define MSRP_Rank_Emergency 0
#define MSRP_Rank_NonEmergency 1

typedef struct msrp_stream_data {
	ub_bytearray8_t stream_id;
	ub_bytearray6_t destmac;
	uint16_t vlan_id;
	uint16_t max_frame_size;
	uint16_t max_intv_frames;
	uint8_t priority_rank_reserved;
	uint32_t accum_latency;
	ub_bytearray8_t bridge_id;
	uint8_t failure_code;
} __attribute__((packed)) msrp_stream_data_t;

typedef struct msrp_domain_data {
	uint8_t srclass_id;
	uint8_t srclass_priority;
	uint16_t srclass_vid;
} __attribute__((packed)) msrp_domain_data_t;

typedef union msrp_firstvalue_data{
	msrp_stream_data_t stream;
	msrp_domain_data_t domain;
} msrp_firstvalue_data_t;

static inline int mrp_get_firstvalue_size(protocol_index_t protoIndex, uint8_t atype)
{
	switch(protoIndex){
	case MRP_MMRP:
		switch(atype){
		case MMRP_MacVector:
			return 6;
		case MMRP_ServiceReq:
			return 1;
		default:
			return 0;
		}
	case MRP_MVRP:
		return 2;
 	case MRP_MSRP:
		switch(atype){
		case MSRP_TalkerFailed:
			return 34;
		case MSRP_TalkerAdvertise:
			return 25;
		case MSRP_Listener:
			return 8;
		case MSRP_Domain:
			return 4;
		default:
			return 0;
		}
	}
	return 0;
}

static inline int mrp_get_numberof_attrytypes(protocol_index_t protoIndex)
{
	switch(protoIndex){
	case MRP_MMRP: return 2;
	case MRP_MVRP: return 1;
 	case MRP_MSRP: return 4;
	}
	return 0;
}

static inline int mrp_next_firstvalue(protocol_index_t protoIndex, uint8_t atype,
				      uint8_t *fvd)
{
	switch(protoIndex){
	case MRP_MMRP:
			return -1;
	case MRP_MVRP:
	{
		mvrp_firstvalue_data_t *v1=(mvrp_firstvalue_data_t *)fvd;
		v1->vid+=1;
		return 0;
	}
 	case MRP_MSRP:
		switch(atype){
		case MSRP_TalkerFailed:
		case MSRP_TalkerAdvertise:
		case MSRP_Listener:
		{
			int i;
			msrp_stream_data_t *v1=(msrp_stream_data_t *)fvd;
			for(i=7;i>=0;i--){
				v1->stream_id[i]+=1;
				if(v1->stream_id[i]!=0) break;
			}
			if(atype==MSRP_Listener) return 0;
			// MSRP_TalkerFailed, MSRP_TalkerAdvertise
			for(i=5;i>=0;i--){
				v1->destmac[i]+=1;
				if(v1->destmac[i]!=0) break;
			}
			return 0;
		}
		case MSRP_Domain:
		{
			msrp_domain_data_t *v1=(msrp_domain_data_t *)fvd;
			v1->srclass_id+=1;
			v1->srclass_priority+=1;
			return 0;
		}
		default:
			return -1;
		}
	}
	return -1;
}

// return 0:match, -2:smaller(fvd1>fvd2), -1:match to the previous(fvd1=fvd2+1),
// 1:match to the next(fvd1=fvd2-1) 2:bigger(fvd1<fvd2)
static inline int mrp_next_firstvalue_comp(protocol_index_t protoIndex, uint8_t atype,
					   uint8_t *fvd1, uint8_t *fvd2)
{
	switch(protoIndex){
	case MRP_MMRP:
		return INT_MAX;
	case MRP_MVRP:
	{
		mvrp_firstvalue_data_t *v1, *v2;
		v1=(mvrp_firstvalue_data_t *)fvd1;
		v2=(mvrp_firstvalue_data_t *)fvd2;
		if(v1->vid==v2->vid) return 0;
		if(v1->vid<v2->vid){
			if(v1->vid+1==v2->vid) return 1;
			return 2;
		}
		if(v1->vid-1==v2->vid) return -1;
		return -2;
	}
 	case MRP_MSRP:
		switch(atype){
		case MSRP_TalkerFailed:
		case MSRP_TalkerAdvertise:
		case MSRP_Listener:
		{
			msrp_stream_data_t *v1, *v2;
			uint64_t vs1,vs2;
			uint64_t vd1=0,vd2=0;
			v1=(msrp_stream_data_t *)fvd1;
			v2=(msrp_stream_data_t *)fvd2;
			memcpy(&vs1, &(v1->stream_id), 8);
			memcpy(&vs2, &(v2->stream_id), 8);
			vs1=UB_NTOHLL(vs1);
			vs2=UB_NTOHLL(vs2);
			if(atype==MSRP_Listener){
				if(vs1==vs2) return 0;
				if(vs1<vs2){
					if(vs1+1==vs2) return 1;
					return 2;
				}
				if(vs1-1==vs2) return -1;
				return -2;
			}
			// MSRP_TalkerFailed, MSRP_TalkerAdvertise
			memcpy(((uint8_t*)&vd1)+2, &(v1->destmac), 6);
			memcpy(((uint8_t*)&vd2)+2, &(v2->destmac), 6);
			vd1=UB_NTOHLL(vd1);
			vd2=UB_NTOHLL(vd2);
			if(vs1==vs2 && vd1==vd2) return 0;
			if(vs1<vs2){
				if(vs1+1==vs2 && vd1+1==vd2) return 1;
				return 2;
			}
			if(vs1-1==vs2 && vd1-1==vd2) return -1;
			return -2;
		}
		case MSRP_Domain:
		{
			msrp_domain_data_t *v1, *v2;
			v1=(msrp_domain_data_t *)fvd1;
			v2=(msrp_domain_data_t *)fvd2;
			if(v1->srclass_id==v2->srclass_id &&
			   v1->srclass_priority==v2->srclass_priority)
				return 0;
			if(v1->srclass_id<v2->srclass_id){
				if(v1->srclass_id+1==v2->srclass_id &&
				   v1->srclass_priority+1==v2->srclass_priority)
					return 1;
				return 2;
			}
			if(v1->srclass_id-1==v2->srclass_id &&
			   v1->srclass_priority-1==v2->srclass_priority)
				return -1;
			return -1;
		default:
			return INT_MAX;
		}
		}
	}
	return INT_MAX;
}

static inline int mrp_get_firstvalue_cmpsize(protocol_index_t protoIndex, uint8_t atype)
{
	if(protoIndex==MRP_MSRP && (atype==MSRP_TalkerAdvertise || atype==MSRP_TalkerFailed))
		return 21; // for TA,TF before 'Accumulate Latency'
	return mrp_get_firstvalue_size(protoIndex, atype);
}

#define MRP_MSRP_FAILURE_CODE_STR { \
	"Unknown", \
	"Insufficient bandwidth", \
	"Insufficient Bridge resources", \
	"Insufficient bandwidth for traffic class.", \
	"StreamID in use by another Talker", \
	"Stream destination_address already in use", \
	"Stream preempted by higher rank", \
	"Reported latency has changed", \
	"Egress port is not AVB capable1", \
	"Use a different destination_address (i.e., MAC DA hash table full)", \
	"Out of MSRP resources", \
	"Out of MMRP resources", \
	"Cannot store destination_address (i.e., Bridge is out of MAC DA resources)", \
	"Requested priority is not an SR Class (3.231) priority", \
	"MaxFrameSize (35.2.2.8.4(a)) is too large for media", \
	"msrpMaxFanInPorts (35.2.1.4(f)) limit has been reached", \
	"Changes in FirstValue for a registered StreamID.", \
	"VLAN is blocked on this egress port (Registration Forbidden)2", \
	"VLAN tagging is disabled on this egress port (untagged set)", \
	"SR class priority mismatch", \
	}

typedef enum mrp_msrp_failure_code {
	MRP_MSRP_FAILURE_UNKNOWN = 0,
	MRP_MSRP_FAILURE_INSUFFICIENT_BW,
	MRP_MSRP_FAILURE_INSUFFICIENT_RESOURCE,
	MRP_MSRP_FAILURE_INSUFFICIENT_TCBW,
	MRP_MSRP_FAILURE_STREAMID_BUSY,
	MRP_MSRP_FAILURE_DESTADDR_BUSY, //5
	MRP_MSRP_FAILURE_STREAM_PREEMPTED,
	MRP_MSRP_FAILURE_LATENCY_CHANGED,
	MRP_MSRP_FAILURE_NOT_AVB_CAPABLE,
	MRP_MSRP_FAILURE_DIFFERENT_DA,
	MRP_MSRP_FAILURE_NO_RESOURCE_MSRP, //10
	MRP_MSRP_FAILURE_NO_RESOURCE_MMRP,
	MRP_MSRP_FAILURE_CANT_STORE_DA,
	MRP_MSRP_FAILURE_NO_SR_CLASS,
	MRP_MSRP_FAILURE_MAX_FSIZE_TOO_LARGE,
	MRP_MSRP_FAILURE_MSRP_MAX_FANIN, //15
	MRP_MSRP_FAILURE_CHANGE_FIRSTVALUE,
	MRP_MSRP_FAILURE_VLAN_BLOCKED,
	MRP_MSRP_FAILURE_VLAN_TAGGING,
	MRP_MSRP_FAILURE_SR_CLASS_MISMATCH,
} mrp_msrp_failure_code_t;

#endif
