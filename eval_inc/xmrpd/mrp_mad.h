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
#ifndef __MRP_MAD_H_
#define __MRP_MAD_H_

#include "mrp_sm.h"
#include "mrpdu.h"
#include "mrp_datatypes.h"

struct mrp_map_data;
typedef struct mrp_map_data mrp_map_data_t;

typedef enum {
	MAD_NOACTION,
	MAD_JOIN_REQUEST_NEW,
	MAD_JOIN_REQUEST,
	MAD_LEAVE_REQUEST,
	MAD_JOIN_INDICATION_NEW,
	MAD_JOIN_INDICATION,
	MAD_LEAVE_INDICATION,
	MAD_ACTION_SEND,
} mrp_mad_actions_t;

// each Attribute Type has its own applicant and registrar state
// Attribute Type is application specific and it is unknown in this layer how many there are
typedef struct attrstate {
	attrvalue_t av;
	uint64_t access_ts;
	uint64_t leavetimer_ts;
	// if prop_port=0 propagate to all the other ports, or propagate to port='prop_port - 1'
	uint32_t prop_port;
	uint32_t action_flags;
	uint8_t transmit;
	bool repacked; // true when it is repacked to transmit.
} attrstate_t;

typedef enum {
	ATTR_ACTION_DELETE,
	ATTR_ACTION_UNCOND_DELETE,
	ATTR_ACTION_FLUSH,
	ATTR_ACTION_REGSIDE,
	ATTR_ACTION_NODELETE,
	ATTR_ACTION_PROP_OWNPORT, // enable propagation to its own port
	ATTR_ACTION_NOPROC_DEC, // no process on a propagated declaration port
	ATTR_ACTION_PROP_CHANGED,   // attr was changed when it was prpagated last time
	ATTR_ACTION_NEWLY_CREATED,
	ATTR_ACTION_NEWLY_REGSIDE,
	ATTR_ACTION_LAST_INMESSAGE,
	ATTR_ACTION_BULK_START,
	ATTR_ACTION_BY_DEFAULT,
	ATTR_ACTION_BY_EXTCTRL,
} mrp_mad_attr_action_flag_t;

#define MRP_BULK_PROC_MIN 100

static inline int attr_action_flag(attrstate_t *attr, mrp_mad_attr_action_flag_t f)
{
	return attr->action_flags & (1<<f);
}
static inline int set_attr_action_flag(attrstate_t *attr, mrp_mad_attr_action_flag_t f)
{
	return attr->action_flags |= (1<<f);
}
static inline int clear_attr_action_flag(attrstate_t *attr, mrp_mad_attr_action_flag_t f)
{
	return attr->action_flags &= ~(1<<f);
}

#define	MRP_MAD_ATTR_DEC (1<<0)
#define MRP_MAD_ATTR_REG (1<<1)
#define MRP_MAD_ATTR_NOT_BYDEFAULT (1<<2)
/*
  MRP_MAD_ATTR_Dec: all of D only attributes
  MRP_MAD_ATTR_Dec_Reg: most of R/D attributes
  MRP_MAD_ATTR_Reg: all of R only attributes and some R/D(in a case D gets Lv and goes to VO)
*/
static inline int attr_in_regside(attrstate_t *attr)
{
	bool astate_cond;
	int res=0;
	if(!attr_action_flag(attr, ATTR_ACTION_BY_DEFAULT)) res=MRP_MAD_ATTR_NOT_BYDEFAULT;
	astate_cond= (attr->av.apstate==ASTATE_VO) ||
		(attr->av.apstate==ASTATE_AO) ||
		(attr->av.apstate==ASTATE_QO) ||
		(attr->av.apstate==ASTATE_LO);
	if(attr_action_flag(attr, ATTR_ACTION_REGSIDE) && astate_cond)
		return res|MRP_MAD_ATTR_REG;
	if(attr_action_flag(attr, ATTR_ACTION_REGSIDE))
		return res | MRP_MAD_ATTR_REG | MRP_MAD_ATTR_DEC;
	return res | MRP_MAD_ATTR_DEC;
}

typedef struct mrp_mad_data mrp_mad_data_t;

typedef int(*mrp_mad_actioncb)(void *cbdata, mrp_mad_actions_t act, attrstate_t *astate,
			       uint64_t ts, bool onrec);

typedef int (*mrp_mad_rec_eventcb)(void *cbdata, uint8_t attrtype, void *fv, uint8_t fvlen,
				   int nofv, int fvindex, mrpdu_attribute_event_t ate,
				   int8_t fpe, uint64_t ts, bool lastinmes);

attrstate_t *mrp_mad_get_attr_state_type_fvd(mrp_mad_data_t *madd, uint8_t attrtype,
					     void *fvd, uint8_t fvd_cmpstart,
					     uint8_t fvd_cmpsize, unsigned int *startp);
attrstate_t *mrp_mad_get_attr_state_afterts(mrp_mad_data_t *madd, uint8_t *attrtype, uint64_t ts);

attrstate_t *mrp_mad_add_attr_state(mrp_mad_data_t *madd, uint8_t attrtype, void *fvd,
				    uint8_t fvd_size, int nofv, int fvindex,
				    bool delold, uint64_t ts);

attrstate_t *mrp_mad_get_add_attr_state(mrp_mad_data_t *madd, uint8_t attrtype, uint8_t *fvd,
					uint8_t fvd_size, int nofv, int fvindex,
					mrpdu_attribute_event_t ate, uint64_t ts);

attrstate_t *mrp_mad_astate_per_atype(mrp_mad_data_t *madd, uint8_t *atype_start,
				      int *index);

int mrp_mad_del_oldest_attr_state(mrp_mad_data_t *madd);
int mrp_mad_del_duplicated_attr(mrp_mad_data_t *madd, attrstate_t *astate, uint8_t attrtype,
				int checksize);

void mrp_mad_rec_event(mrp_mad_data_t *madd, attrstate_t *astate, uint64_t ts);

mrp_mad_data_t *mrp_mad_init(mrpnet_data_t *mrpnd, int portIndex, protocol_index_t protoIndex,
			     mrp_map_data_t *mapd, mrp_mad_rec_eventcb rec_eventcb,
			     mrp_mad_actioncb actioncb, void *mapp_cbdata);
void mrp_mad_close(mrp_mad_data_t *madd);

int mrp_mad_proc_events(mrp_mad_data_t *madd, uint64_t ts,
			uint32_t mrp_events, int portofevent);

int mrp_mad_proc(mrp_mad_data_t *madd, uint64_t ts, int periodic);

attrstate_t *mrp_mad_attr_inreg(mrp_mad_data_t *madd, attrstate_t *astate,
				int cmpstart, int cmpsize, int decreg);

void mrp_mad_add_mad_action(mrp_mad_data_t *madd, mrp_mad_actions_t madaction,
			    attrstate_t *astate, int cmpsize, uint64_t ts);

ub_esarray_cstd_t *mrp_mad_get_attr_states(mrp_mad_data_t *madd);

void mrp_mad_print_attr_state(bool console, bool debug,
			      protocol_index_t protoIndex, int portIndex,
			      int indent, attrstate_t *astate);

void mrp_mapd_state_bulk_copy(mrp_mad_data_t *madd, attrstate_t *astate);

int mrp_mad_attr_bulk_copy(mrp_mad_data_t *madd, attrstate_t **astate, int cnum);

#endif
