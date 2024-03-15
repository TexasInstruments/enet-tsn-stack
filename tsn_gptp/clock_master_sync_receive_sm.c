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
#include "mind.h"
#include "mdeth.h"
#include "gptpnet.h"
#include "gptpclock.h"
#include "clock_master_sync_receive_sm.h"
#include "gptpcommon.h"

typedef enum {
	INIT,
	INITIALIZING,
	WAITING,
	RECEIVE_SOURCE_TIME,
	REACTION,
}clock_master_sync_receive_state_t;

typedef enum {
	OFFSET_NOT_ADJ=-1,
	OFFSET_START_ADJ,
	OFFSET_UNSTABLE_ADJ,
	OFFSET_STABLE_ADJ,
}offset_state_t;

#define RCVD_CLOCK_SOURCE_REQ sm->thisSM->rcvdClockSourceReq
#define RCVD_CLOCK_SOURCE_REQ_PTR sm->thisSM->rcvdClockSourceReqPtr
#define	RCVD_LOCAL_CLOCK_TICK sm->thisSM->rcvdLocalClockTick
#define GPTPINSTNUM sm->ptasg->gptpInstanceIndex

//if passing time between GM and thisClock, no way to calculate the freq offset
#define CMSR_TOO_BIG_PASSTIME_GAP (UB_SEC_NS/10)

/* the following FREQ_* and PHASE_* constants may be configurable.
 * But because the current values have been well tuned,
 * and changing is not so easy, we use hardcoded numbers at this time.
 */

// move to stable condition if the FREQ_OFFSET_STABLE_PPB passed this time consecutively
#define FREQ_OFFSET_STABLE_TRNS 3

#define PHASE_NEWGM_CRITERION 1000000 // 1msec
#define PHASE_STABLE_CRITERION 10000 // 10usec
#define PHASE_UNSTABLE_CRITERION 30000 // 30usec
// after stable, adjust phase when the detected gap between GM and thisClock exceeds this value
#define PHASE_OFFSET_ADJUST_TARGET 10000 // nsec
#define PHASE_OFFSET_ADJUST_BY_FREQ 100000 // 100usec
#define SET_PHASE_OFFSETGM_NOOP_RETURN -99999999 // not in +/-(PHASE_OFFSET_ADJUST_BY_FREQ*10)

static void debug_show_diff_to_GM(clock_master_sync_receive_data_t *sm,
				  int64_t lts, int64_t mts) __attribute__((unused));
static void debug_show_diff_to_GM(clock_master_sync_receive_data_t *sm,
				  int64_t lts, int64_t mts)
{
	(void)gptpclock_tsconv(GPTPINSTNUM, &lts, sm->ptasg->thisClockIndex,
			       sm->ptasg->domainIndex,
			       0, sm->ptasg->domainIndex);
	lts=mts-lts;
	UB_LOG(UBL_INFO,"domainIndex=%d, %"PRIi64"nsec, offset=%"PRIi64"\n",
	       sm->ptasg->domainIndex, lts, sm->offsetGM);
}

static int set_phase_offsetGM(clock_master_sync_receive_data_t *sm, int64_t dts, int64_t dlts)
{
	uint64_t dofg;
	int alpha=1;
	int64_t od;
	int64_t offsetGM=0;
	int padj_clockindex, poabf;

	dofg=llabs(dts-sm->offsetGM);
	if(dofg>=(unsigned int)PHASE_NEWGM_CRITERION){
		if(sm->gmchange_ind &&
		   (sm->gmchange_ind==gptpclock_get_gmchange_ind(GPTPINSTNUM,
								 sm->ptasg->domainIndex))){
			if(sm->offsetGM_stable>=OFFSET_START_ADJ) {
				UB_LOG(UBL_INFO, "%s:domainIndex=%d, big offset Jump=%u\n",
				       __func__, sm->ptasg->domainIndex, (unsigned int)dofg);
				sm->offsetGM_stable=OFFSET_NOT_ADJ;
				// don't update 'sm->offsetGM' by the first 'big jump'
				// if the value at the next time is in PHASE_NEWGM_CRITERION,
				// this is treated as 'OFFSET_START_ADJ'
				return 0;
			}
			UB_LOG(UBL_INFO, "%s:domainIndex=%d, big offset Jump=%u"
			       "second time, update with the big jump\n",
			       __func__, sm->ptasg->domainIndex, (unsigned int)dofg);
			// second 'big jump', start over from 'OFFSET_START_ADJ'
		}
		sm->offsetGM_stable=OFFSET_START_ADJ;
	}

	/*
	 * When XL4_EXTMOD_XL4GPTP_USE_HW_PHASE_ADJUSTMENT is enabled, negative
	 * dlts may occur if the master clock counter is less than that of the slave clock.
	 * Consequently, this can lead to extended waiting periods for the slave side
	 * to transition to the clock adjustment state.
	 * By checking (dlts > 0), we can avoid this issue.
	 */
	if((dlts > 0) && (dlts < gptpgcfg_get_intitem(
		    GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_CLOCK_COMPUTE_INTERVAL_MSEC,
		    YDBI_CONFIG)*UB_MSEC_NS)
	   && (sm->offsetGM_stable > OFFSET_START_ADJ)) {
		// once it reaches adjustment stage, use longer interval
		return SET_PHASE_OFFSETGM_NOOP_RETURN;
	}
	switch(sm->offsetGM_stable){
	case OFFSET_NOT_ADJ:
	case OFFSET_START_ADJ:
		offsetGM = dts;
		sm->offsetGM_stable=OFFSET_UNSTABLE_ADJ;
		UB_LOG(UBL_INFO, "%s:domainIndex=%d, New adjustment(New GM?)\n",
		       __func__, sm->ptasg->domainIndex);
		break;
	case OFFSET_UNSTABLE_ADJ:
		alpha=gptpgcfg_get_intitem(
			GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_START_VALUE,
			YDBI_CONFIG);
		offsetGM = (dts/alpha) + (alpha-1) * (sm->offsetGM / alpha);
		if(dofg<(unsigned int)PHASE_STABLE_CRITERION){
			UB_LOG(UBL_INFO, "%s:domainIndex=%d, stable\n",
			       __func__, sm->ptasg->domainIndex);
			sm->offsetGM_stable=OFFSET_STABLE_ADJ;
			sm->gmchange_ind=gptpclock_get_gmchange_ind(GPTPINSTNUM,
								    sm->ptasg->domainIndex);
			UB_LOG(UBL_DEBUG, "%s:gmchange_ind=%d\n",
			       __func__, sm->gmchange_ind);
		}
		break;
	case OFFSET_STABLE_ADJ:
		if(sm->gmchange_ind!=gptpclock_get_gmchange_ind(GPTPINSTNUM,
								sm->ptasg->domainIndex)){
			UB_LOG(UBL_INFO, "%s:domainIndex=%d, GM changed. start over.\n",
			       __func__, sm->ptasg->domainIndex);
			sm->offsetGM_stable=OFFSET_START_ADJ;
			return 0;
		}
		alpha=gptpgcfg_get_intitem(
			GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE,
			YDBI_CONFIG);
		offsetGM = (dts/alpha) + (alpha-1) * (sm->offsetGM / alpha);
		if(dofg>(unsigned int)PHASE_UNSTABLE_CRITERION){
			UB_LOG(UBL_INFO, "%s:domainIndex=%d, unstable\n",
			       __func__, sm->ptasg->domainIndex);
			sm->offsetGM_stable=OFFSET_UNSTABLE_ADJ;
		}
		break;
	default:
		break;
	}

	od=offsetGM-sm->offsetGM;
	if(gptpgcfg_get_intitem(
		   GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_USE_HW_PHASE_ADJUSTMENT,
		   YDBI_CONFIG) && (sm->ptasg->domainIndex==0u)){
		// the range to be adjusted by freq must be wider than normal,
		// then gptpclock_setoffset64 is called less frequently
		poabf=PHASE_OFFSET_ADJUST_BY_FREQ*10;
		padj_clockindex=sm->ptasg->thisClockIndex;
	}else{
		poabf=PHASE_OFFSET_ADJUST_BY_FREQ;
		padj_clockindex=0;
	}
	if((llabs(od)<poabf) && gptpgcfg_get_intitem(
		   GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_PHASE_ADJUSTMENT_BY_FREQ,
		   YDBI_CONFIG)){
		if(llabs(od)<PHASE_OFFSET_ADJUST_TARGET){return od;}
		UB_LOG(UBL_INFO, "%s:domainIndex=%d, offset adjustment by Freq., diff=%d\n",
		       __func__, sm->ptasg->domainIndex, (int)(od));
		return od;
	}
	UB_LOG(UBL_INFO, "%s:domainIndex=%d, offset adjustment, diff=%d\n",
	       __func__, sm->ptasg->domainIndex, (int)od);
	(void)gptpclock_setoffset64(GPTPINSTNUM, offsetGM, padj_clockindex, sm->ptasg->domainIndex);
	if(gptpgcfg_get_intitem(
		   GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_USE_HW_PHASE_ADJUSTMENT,
		   YDBI_CONFIG) && (sm->ptasg->domainIndex==0u)){
		sm->offsetGM=0;
	}else{
		sm->offsetGM=offsetGM;
	}
	return 0;
}

static int computeGmRateRatio(clock_master_sync_receive_data_t *sm,
			      uint64_t lts, uint64_t mts)
{
	int64_t dlts, dmts;
	int64_t	dts;
	double nrate;
	int ppb;
	int offset_comp;
	if(sm->unstable_ts64==0){
		sm->unstable_ts64=ub_mt_gettime64();
	}
	dlts = lts - sm->last_lts;
	dts=mts-lts;
	offset_comp=set_phase_offsetGM(sm, dts, dlts);
	if(offset_comp==SET_PHASE_OFFSETGM_NOOP_RETURN){return -1;}
	//debug_show_diff_to_GM(sm, lts, mts);

	dmts = mts - sm->last_mts;
	sm->last_lts = lts;
	sm->last_mts = mts;
	if(llabs(dmts-dlts) > CMSR_TOO_BIG_PASSTIME_GAP){return -1;}
	// IIR filter, M(n) = a*R(n) + (1-a)*M(n-1), a=CMSR_IIR_COEFF
	nrate = sm->alpha * ((double)dmts/(double)dlts) +
		(1-sm->alpha)*sm->mrate;
	ppb = ((nrate-1.0)*1.0E9);
	if((sm->rate_stable < FREQ_OFFSET_STABLE_TRNS) &&
	   (abs(ppb) <
	    gptpgcfg_get_intitem(
		    GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_STABLE_PPB,
		    YDBI_CONFIG))){
		sm->rate_stable++;
		if(sm->rate_stable >= FREQ_OFFSET_STABLE_TRNS){
			sm->alpha = 1.0/gptpgcfg_get_intitem(
				GPTPINSTNUM,
				XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE,
				YDBI_CONFIG);
			UB_LOG(UBL_INFO, "domainIndex=%d, clock_master_sync_receive:stable rate, %"PRIu64"msec from unstable\n",
				sm->ptasg->domainIndex, (ub_mt_gettime64()-sm->unstable_ts64)/UB_MSEC_NS);
		}
	}

	// Check against a configurable threshold in determining unstable delta rate.
	// Note that this threshold value does not restrict the actual adjustment -
	// refer to XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_UPDATE_MRATE_PPB - instead it performs the following:
	//  a) toggles the unstable status
	//  b) notifies timeleap event (TIMELEAP_FUTURE or TIMELEAP_PAST)
	if(abs(ppb) > gptpgcfg_get_intitem(
		   GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB,
		   YDBI_CONFIG)) {
		// Notify only when previous rate passed stable criteria
		if(sm->rate_stable!=0){
			UB_LOG(UBL_INFO,
			       "clock_master_sync_receive:%s:domainIndex=%d unstable rate=%dppb (%s)\n",
			       __func__, sm->ptasg->domainIndex, ppb,
			       (ppb>0)?"timeleap_future":"timeleap_past");
		}
		if(sm->rate_stable >= FREQ_OFFSET_STABLE_TRNS){
			sm->unstable_ts64=ub_mt_gettime64();
		}
		sm->rate_stable=0;
		sm->alpha = 1.0/gptpgcfg_get_intitem(
			GPTPINSTNUM,
			XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_START_VALUE,
			YDBI_CONFIG);
	}

	UB_LOG(UBL_DEBUG, "clock_master_sync_receive:%s:domainIndex=%d rate=%dppb\n",
			__func__, sm->ptasg->domainIndex, ppb);

	ppb+=offset_comp;
	if(abs(ppb) > gptpgcfg_get_intitem(
		   GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_UPDATE_MRATE_PPB,
		   YDBI_CONFIG)){
		int maxadj=gptpgcfg_get_intitem(
			GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_MAX_ADJUST_RATE_ON_CLOCK,
			YDBI_CONFIG);
		sm->gmadjppb += ppb;
		if(sm->gmadjppb > maxadj){
			sm->gmadjppb = maxadj;
		}else if(sm->gmadjppb < -maxadj){
			sm->gmadjppb = -maxadj;
		}else{}
		(void)gptpclock_setadj(GPTPINSTNUM, sm->gmadjppb,
				 sm->ptasg->thisClockIndex, sm->ptasg->domainIndex);
		UB_LOG(UBL_INFOV, "domainIndex=%d, clock_master_sync_receive:"
		       "the master clock rate to %dppb, GMdiff=%"PRIi64"nsec\n",
		       sm->ptasg->domainIndex, sm->gmadjppb, dts-sm->offsetGM);
		// the master must be synchronized and the rate becomes 1.0
		sm->mrate = 1.0;
		sm->ptasg->gmRateRatio = 1.0;
	}

	return 0;
}

static clock_master_sync_receive_state_t allstate_condition(clock_master_sync_receive_data_t *sm)
{
	if(sm->ptasg->BEGIN || !sm->ptasg->instanceEnable ) {
		return INITIALIZING;
	}
	return (clock_master_sync_receive_state_t)sm->state;
}

static void *initializing_proc(clock_master_sync_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "clock_master_sync_receive:%s:domainIndex=%d\n",
	       __func__, sm->domainIndex);
	sm->ptasg->clockSourceTimeBaseIndicatorOld = 0;
	sm->mrate = 1.0;
	sm->alpha = 1.0/gptpgcfg_get_intitem(
		GPTPINSTNUM, XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_START_VALUE,
		YDBI_CONFIG);
	sm->last_lts = 0;
	sm->last_mts = 0;
	sm->offsetGM = 0;
	sm->rate_stable = 0;
	sm->unstable_ts64 = 0;
	RCVD_CLOCK_SOURCE_REQ = false;
	RCVD_LOCAL_CLOCK_TICK = false;
	return NULL;
}

static clock_master_sync_receive_state_t initializing_condition(
	clock_master_sync_receive_data_t *sm)
{
	return WAITING;
}

static void *waiting_proc(clock_master_sync_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "clock_master_sync_receive:%s:domainIndex=%d\n",
	       __func__, sm->domainIndex);
	return NULL;
}

static clock_master_sync_receive_state_t waiting_condition(clock_master_sync_receive_data_t *sm)
{
	if(RCVD_CLOCK_SOURCE_REQ || RCVD_LOCAL_CLOCK_TICK){return RECEIVE_SOURCE_TIME;}
	return WAITING;
}

static void *receive_source_time_proc(clock_master_sync_receive_data_t *sm)
{
	UB_LOG(UBL_DEBUGV, "clock_master_sync_receive:%s:domainIndex=%d\n",
	       __func__, sm->domainIndex);


	//updateMasterTime(sm);
	//sm->ptasg->localTime = currentTime;
	if (RCVD_CLOCK_SOURCE_REQ) {
		uint64_t lts, mts;
		mts = (sm->ptasg->syncReceiptTime.seconds.lsb * (uint64_t)UB_SEC_NS) +
			sm->ptasg->syncReceiptTime.fractionalNanoseconds.msb;
		lts = sm->ptasg->syncReceiptLocalTime.nsec;
		(void)computeGmRateRatio(sm, lts, mts);
		sm->ptasg->clockSourceTimeBaseIndicatorOld =
			sm->ptasg->clockSourceTimeBaseIndicator;
		// As rcvd values have already saved into sm->ptasg->*, copy from there
		sm->ptasg->clockSourceTimeBaseIndicator = sm->ptasg->gmTimeBaseIndicator;
		sm->ptasg->clockSourceLastGmPhaseChange = sm->ptasg->lastGmPhaseChange;
		sm->ptasg->clockSourceLastGmFreqChange = sm->ptasg->lastGmFreqChange;

		if(gptpclock_get_gmsync(GPTPINSTNUM, sm->ptasg->domainIndex)==GMSYNC_UNSYNC){
			(void)gptpclock_set_gmsync(GPTPINSTNUM,
						   sm->ptasg->domainIndex, GMSYNC_SYNC);
		}
	}
	RCVD_CLOCK_SOURCE_REQ = false;
	RCVD_LOCAL_CLOCK_TICK = false;
	return NULL;
}

static clock_master_sync_receive_state_t receive_source_time_condition(
	clock_master_sync_receive_data_t *sm)
{
	return WAITING;
}


void *clock_master_sync_receive_sm(clock_master_sync_receive_data_t *sm, uint64_t cts64)
{
	bool state_change;
	void *retp=NULL;

	if(!sm){return NULL;}
	sm->state = allstate_condition(sm);

	while(true){
		state_change=(sm->last_state != sm->state);
		sm->last_state = sm->state;
		switch(sm->state){
		case INIT:
			sm->state = INITIALIZING;
			break;
		case INITIALIZING:
			if(state_change){retp=initializing_proc(sm);}
			sm->state = initializing_condition(sm);
			break;
		case WAITING:
			if(state_change){retp=waiting_proc(sm);}
			sm->state = waiting_condition(sm);
			break;
		case RECEIVE_SOURCE_TIME:
			if(state_change){retp=receive_source_time_proc(sm);}
			sm->state = receive_source_time_condition(sm);
			break;
		case REACTION:
		default:
			break;
		}
		if(retp!=NULL){return retp;}
		if(sm->last_state == sm->state){break;}
	}
	return retp;
}

void clock_master_sync_receive_sm_init(clock_master_sync_receive_data_t **sm,
	int domainIndex,
	PerTimeAwareSystemGlobal *ptasg)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, domainIndex);
	INIT_SM_DATA(clock_master_sync_receive_data_t, ClockMasterSyncReceiveSM, sm);
	if(ub_fatalerror()){return;}
	(*sm)->ptasg = ptasg;
	(*sm)->domainIndex = domainIndex;
	(void)clock_master_sync_receive_sm(*sm, 0);
}

int clock_master_sync_receive_sm_close(clock_master_sync_receive_data_t **sm)
{
	UB_LOG(UBL_DEBUGV, "%s:domainIndex=%d\n", __func__, (*sm)->domainIndex);
	CLOSE_SM_DATA(sm);
	return 0;
}

void *clock_master_sync_receive_sm_ClockSourceReq(clock_master_sync_receive_data_t *sm,
						  uint64_t cts64)
{
	RCVD_CLOCK_SOURCE_REQ = true;
	sm->last_state = REACTION;
	return clock_master_sync_receive_sm(sm, cts64);
}
