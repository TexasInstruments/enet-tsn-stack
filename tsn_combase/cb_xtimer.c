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
 * cb_xtimer.c
 */
#include "cb_xtimer.h"
#include "combase_private.h"
#include <tsn_unibase/ub_getmem.h>

struct cb_xtimer_man {
	struct ub_list running_timers;
};

struct cb_xtimer_ndata {
	cb_xtimer_man_t *xtimer_man;
	uint64_t abstime_nsec;
	uint32_t timeout_us;
	bool is_periodic;
	bool is_running;
	void *exparg;
	xtimer_expirecb_t expirecb;
	void *xtimer;
};

struct cb_xtimer {
	struct ub_list_node timer_node;
	struct cb_xtimer_ndata timer_ndata;
};

static int add_timer_abstime(cb_xtimer_man_t *xtimer_man,
			     struct ub_list_node *tnode,
			     uint64_t abstime_nsec);

/*
 * static memory allocation case('UB_SD_STATIC' is defined)
 * the memory is statically allocated by the following definitions.
 * the numbers defined here should be overridden by your needs.
 * when 'UB_SD_STATIC' is not defined, the number doesn't have meaning.
 */
#define CB_XTIMER_MMEM cb_xtimer_man
#ifndef CB_XTIMER_INSTNUM
// how many this timer instances are used as maximum
#define CB_XTIMER_INSTNUM 1
#endif

#define CB_XTIMER_NMEM cb_xtimer_node
#ifndef CB_XTIMER_TMNUM
// how many timers are used in all instances as maximum
#define CB_XTIMER_TMNUM 20
#endif

UB_SD_GETMEM_DEF(CB_XTIMER_MMEM, (int)sizeof(struct cb_xtimer_man),
		 CB_XTIMER_INSTNUM);
UB_SD_GETMEM_DEF(CB_XTIMER_NMEM, (int)sizeof(struct cb_xtimer),
		 CB_XTIMER_TMNUM);

int cb_xtimer_man_schedule(cb_xtimer_man_t *xtimer_man)
{
	struct ub_list expired_timers = UB_LIST_INIT;
	uint64_t cts64 = ub_mt_gettime64();
	struct ub_list_node *tnode;
	cb_xtimer_ndata_t *tdata;

	if(!xtimer_man){return -1;}

	while(!ub_list_isempty(&xtimer_man->running_timers)){
		tnode=ub_list_head(&xtimer_man->running_timers);
		tdata=(cb_xtimer_ndata_t *)tnode->data;
		if(tdata->abstime_nsec > cts64){break;}
		ub_list_unlink(&xtimer_man->running_timers, tnode);
		ub_list_append(&expired_timers, tnode);
	}

	while(!ub_list_isempty(&expired_timers)){
		tnode=ub_list_head(&expired_timers);
		tdata=(cb_xtimer_ndata_t *)tnode->data;
		ub_list_unlink(&expired_timers, tnode);
		tdata->is_running = false;
		if(tdata->expirecb!=NULL){
			tdata->expirecb((cb_xtimer_t *)tdata->xtimer, tdata->exparg);
		}
		if(tdata->is_periodic){
			(void)add_timer_abstime(xtimer_man, tnode,
						((uint64_t)tdata->timeout_us*(uint64_t)UB_USEC_NS +
						 tdata->abstime_nsec));
		}
	}

	return cb_xtimer_man_nearest_timeout(xtimer_man);
}

int cb_xtimer_man_nearest_timeout(cb_xtimer_man_t *xtimer_man)
{
	uint64_t cts64 = ub_mt_gettime64();
	struct ub_list_node *tnode;
	cb_xtimer_ndata_t *tdata;

	if(!xtimer_man){return -1;}
	tnode = ub_list_head(&xtimer_man->running_timers);
	if(!tnode){return -2;}
	tdata=(cb_xtimer_ndata_t *)tnode->data;
	if(tdata->abstime_nsec <= cts64){return 0;}
	return (tdata->abstime_nsec - cts64)/(uint64_t)UB_USEC_NS;
}

void cb_xtimer_clear_periodic(cb_xtimer_t *xtimer)
{
	if(!xtimer){return;}
	xtimer->timer_ndata.is_periodic = false;
}

void cb_xtimer_set_periodic(cb_xtimer_t *xtimer)
{
	if(!xtimer){return;}
	xtimer->timer_ndata.is_periodic = true;
}

bool cb_xtimer_is_periodic(cb_xtimer_t *xtimer)
{
	if(!xtimer){return false;}
	return xtimer->timer_ndata.is_periodic;
}

cb_xtimer_man_t *cb_xtimer_man_create(void)
{
	cb_xtimer_man_t *xtimer_man = (cb_xtimer_man_t *)UB_SD_GETMEM(CB_XTIMER_MMEM, sizeof(*xtimer_man));
	if(!xtimer_man){return NULL;}
	(void)memset(xtimer_man, 0, sizeof(*xtimer_man));
	ub_list_init(&xtimer_man->running_timers);
	return xtimer_man;
}

void cb_xtimer_man_delete(cb_xtimer_man_t *xtimer_man)
{
	struct ub_list_node *tnode;
	cb_xtimer_ndata_t *tdata;

	if(!xtimer_man){return;}
	while(!ub_list_isempty(&xtimer_man->running_timers)) {
		tnode=ub_list_head(&xtimer_man->running_timers);
		tdata=(cb_xtimer_ndata_t *)tnode->data;
		ub_list_unlink(&xtimer_man->running_timers, tnode);
		tdata->is_running = false;
		tdata->xtimer_man = NULL;
	}
	UB_SD_RELMEM(CB_XTIMER_MMEM, xtimer_man);
}

cb_xtimer_t *cb_xtimer_create(cb_xtimer_man_t *xtimer_man,
			      xtimer_expirecb_t expirecb, void *exparg)
{
	cb_xtimer_t *xtimer;

	if(!xtimer_man){return NULL;}
	xtimer = (cb_xtimer_t *)UB_SD_GETMEM(CB_XTIMER_NMEM, sizeof(*xtimer));
	if(!xtimer){return NULL;}
	(void)memset(xtimer, 0, sizeof(*xtimer));
	xtimer->timer_node.data=&xtimer->timer_ndata;
	xtimer->timer_ndata.xtimer_man = xtimer_man;
	xtimer->timer_ndata.expirecb = expirecb;
	xtimer->timer_ndata.exparg = exparg;
	xtimer->timer_ndata.xtimer = xtimer;

	return xtimer;
}

void cb_xtimer_delete(cb_xtimer_t *xtimer)
{
	struct ub_list_node *tnode;
	if(!xtimer){return;}
	tnode=&xtimer->timer_node;
	if(xtimer->timer_ndata.is_running && xtimer->timer_ndata.xtimer_man){
		ub_list_unlink(&xtimer->timer_ndata.xtimer_man->running_timers, tnode);
	}
	UB_SD_RELMEM(CB_XTIMER_NMEM, xtimer);
}

int cb_xtimer_remain_timeout(cb_xtimer_t *xtimer)
{
	uint64_t cts64 = ub_mt_gettime64();

	if(!xtimer){return -1;}
	if(!xtimer->timer_ndata.is_running || (cts64 >= xtimer->timer_ndata.abstime_nsec)){return 0;}
	return (xtimer->timer_ndata.abstime_nsec - cts64)/(uint64_t)UB_USEC_NS;
}

static int add_timer_abstime(cb_xtimer_man_t *xtimer_man,
			     struct ub_list_node *tnode,
			     uint64_t abstime_nsec)
{
	struct ub_list_node *tnode_tmp;
	cb_xtimer_ndata_t *tdata;
	cb_xtimer_ndata_t *tdata_tmp;

	tdata=(cb_xtimer_ndata_t *)tnode->data;
	if (tdata->is_running){return -2;}

	tdata->is_running = true;
	tdata->abstime_nsec = abstime_nsec;

	tnode_tmp = ub_list_tail(&xtimer_man->running_timers);

	/* xtimer has largest abstime, so add to the tail */
	if(!tnode_tmp){
		ub_list_append(&xtimer_man->running_timers, tnode);
		return 0;
	}
	tdata_tmp = (cb_xtimer_ndata_t *)tnode_tmp->data;
	if(tdata->abstime_nsec >= tdata_tmp->abstime_nsec){
		ub_list_append(&xtimer_man->running_timers, tnode);
		return 0;
	}

	/* add in increasing order of abstime_nsec */
	for(UB_LIST_FOREACH_ITER(&xtimer_man->running_timers, tnode_tmp)){
		tdata_tmp = (cb_xtimer_ndata_t *)tnode_tmp->data;
		if(tdata->abstime_nsec <= tdata_tmp->abstime_nsec){
			ub_list_insert_before(&xtimer_man->running_timers,
					      tnode_tmp, tnode);
			break;
		}
	}
	return 0;
}

int cb_xtimer_start(cb_xtimer_t *xtimer, uint32_t timeout_us)
{
	int res;
	uint64_t abstime_nsec;

	if(!xtimer){return -1;}

	/* user may call a periodic timer start inside a expirecb */
	if (xtimer->timer_ndata.is_periodic && xtimer->timer_ndata.abstime_nsec){
		abstime_nsec = xtimer->timer_ndata.abstime_nsec +
			(uint64_t)timeout_us * (uint64_t)UB_USEC_NS;
	}else{
		abstime_nsec = ub_mt_gettime64() +
			(uint64_t)timeout_us * (uint64_t)UB_USEC_NS;
	}

	res = add_timer_abstime(xtimer->timer_ndata.xtimer_man, &xtimer->timer_node, abstime_nsec);
	if (res != 0){return res;}

	xtimer->timer_ndata.timeout_us = timeout_us;

	return 0;
}

void cb_xtimer_stop(cb_xtimer_t *xtimer)
{
	if(!xtimer){return;}
	if(xtimer->timer_ndata.is_running){
		ub_list_unlink(&xtimer->timer_ndata.xtimer_man->running_timers,
			       &xtimer->timer_node);
		xtimer->timer_ndata.is_running = false;
	}
	xtimer->timer_ndata.is_periodic = false;
}

bool cb_xtimer_is_running(cb_xtimer_t *xtimer)
{
	if(!xtimer){return false;}
	return xtimer->timer_ndata.is_running;
}
