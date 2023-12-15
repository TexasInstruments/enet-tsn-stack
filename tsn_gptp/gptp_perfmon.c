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
#include "gptp_perfmon.h"
#include "gptpbasetypes.h"
#include "gptpconf/gptpgcfg.h"
#include "mind.h"
#include "tsn_unibase/unibase_macros.h"
#include <math.h>

// the next 2 functions are in gptpgcfg.c
// to make gptpgcfg.h public, they are excluded from gptpgcfg.h and need 'extern' here
extern int gptpgcfg_set_clock_perfmonDS(PerfMonClockDS *ds, uint8_t id, bool periodComplete,
					uint8_t gptpInstanceIndex, uint8_t domainIndex);

extern int gptpgcfg_set_port_perfmonDS(PerfMonPortDS *ds, uint8_t id, bool periodComplete,
				       uint8_t gptpInstanceIndex, uint8_t domainIndex,
				       uint8_t portIndex);

static void perfmon_running_stat_add(PerfMonAncillaryDataRecord *anc, int64_t x)
{
	/* IEEE1588-2019 PTP Variance
	 * The standards defines StdDev in section 7.6.3 specifying the the variance
	 * is based on Allan Variance and provides the formula (5) to calculate
	 * the unbiased estimate for the variance (StdDev is the square root of the
	 * variance).
	 *
	 * However, under the provision on section 7.6.3 (p.97) where implementation
	 * may opt to compute conservative estimates instead of following the standard
	 * formula, tsn_gptpd will use recurrence formulas to compute for running mean (M)
	 * and running variance (V) below:
	 *
	 *  Mk = Mk-1 + ( xk - Mk-1 ) / k
	 *  Vk = Vk-1 + ( xk - Mk-1) * ( xk - Mk )
	 *
	 *    For 2 <= k <= N, the kth estimate of variance is v^2 = Vk / ( k - 1 ).
	 *    Ref: https://www.johndcook.com/blog/standard_deviation/
	 *
	 *  Reason is we would like to prevent keeping large data sets and keeping
	 *  cummulative summation that may cause overflow.
	 *
	 *  Note: As a limitation on implementation, mean and variance are computed
	 *  in int64_t resulting to floating point portion is intentionally dropped.
	*/
	anc->N++;
	if(anc->N==1){
		anc->prev_M = anc->M=x;
		anc->prev_V = 0;
	}else{
		anc->M = anc->prev_M + (x - anc->prev_M) / anc->N;
		anc->V = anc->prev_V + (x - anc->prev_M) * (x - anc->M);
		anc->prev_M = anc->M;
		anc->prev_V = anc->V;
	}
}

/* PortPerfmanceMonitoringDataRecord */

/**
 * Submit the current PerfMonPortDS for collection (e.g. to uniconf).
 * Afterwards, the current record is reset.
 *
 * In the case of uniconf, it is expected that the current data record is submitted
 * as index=1 (98 for long interval), cascading old records.
 * Then a new data record is submitted for index=0 (or 97).
 */
static int gptp_port_perfmon_dr_submit(PerfMonPortDS *ds, uint8_t id,
                                       uint8_t gptpInstanceIndex,
                                       uint8_t di, uint8_t pi, uint64_t cts64)
{
	UB_LOG(UBL_DEBUG, "%s: submit PerformanceMonitoringPortDS [%s] domain=%d port=%d\n",
		__func__, id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval", di, pi);
	/* IEE1588-2019 J.5.2.2 performanceMonitoringPortDS.recordList
	 * - 15min (short) measurement record current record at index 0
	 *   thus, first historical records is at index 1
	 * - 24hour (long) measurement record current record at index 97
	 *   thus, first historical records is at index 98
	 */
	// cascade records
	if(gptpgcfg_cascade_port_perfmonDS(id, gptpInstanceIndex, di, pi)){
		UB_LOG(UBL_WARN, "%s: Unable to cascade historic records.\n", __func__);
		return -1;
	}
	ds->index[id]=(id==PERFMON_SHORTINTV_DR)?SHORTINTV_START_IDX+1:LONGINTV_START_IDX+1;
	gptp_port_perfmon_dr_dump(ds, UBL_DEBUG, id, di, pi);
	// store current record, with periodComplete=true
	if(gptpgcfg_set_port_perfmonDS(ds, id, true, gptpInstanceIndex, di, pi)){
		UB_LOG(UBL_ERROR, "%s: Unable to submit current record.\n", __func__);
		return -1;
	}
	gptp_port_perfmon_dr_reset(ds, id, di, pi, cts64);
	if(gptpgcfg_set_port_perfmonDS(ds, id, false, gptpInstanceIndex, di, pi)){
		UB_LOG(UBL_WARN, "%s: Unable to submit new empty current record.\n", __func__);
		return -1;
	}
	return 0;

}

/**
 * Update the current PerformanceMonitoringPortDS (presumably already written
 * into database as index=0 or index=97) with new value.
 * Change in values in the data records are not updated into the database in real-time,
 * in order to provide performance based tweaking.
 * This periodicity of this update is configurable by perfmonCurrentPeriod_ms.
 */
static int gptp_port_perfmon_dr_update(PerfMonPortDS *ds, uint8_t id,
			uint8_t gptpInstanceIndex, uint8_t domainIndex, uint8_t portIndex)
{
	UB_LOG(UBL_DEBUG, "%s: updating PerformanceMonitoringPortDS [%s] domain=%d port=%d\n",
		__func__, id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval",
		domainIndex, portIndex);
	gptp_port_perfmon_dr_dump(ds, UBL_DEBUGV, id, domainIndex, portIndex);
	return gptpgcfg_set_port_perfmonDS(ds, id, false, gptpInstanceIndex, domainIndex, portIndex);
}

void gptp_port_perfmon(PerfMonPortDS *ds, int di, int pi, uint64_t cts64,
					   PerTimeAwareSystemGlobal *tasglb)
{
	if(!ds) return;
	if(!tasglb) return;

	/*
	 * Check the following periodicity for collecting data (in order):
	 * - Short interval : collect then reset data record
	 * - Long interval : collect then reset data record
	 */
	PMTimestamp pts=cts64/(UB_MSEC_NS*10);

	#if 0
	UB_LOG(UBL_DEBUGV, "%s: di=%d pi=%d cts=%"PRIu64" PMTime=%d sPMTime=%d lPMTime=%d\n",
		__func__, di, pi, cts64, pts,
		ds->PMTime[PERFMON_SHORTINTV_DR], ds->PMTime[PERFMON_LONGINTV_DR]);
	UB_LOG(UBL_DEBUGV, "%s: sDiff=%d>=%d lastUpdate=%d>=%d\n", __func__,
		pts-ds->PMTime[PERFMON_SHORTINTV_DR], tasglb->perfmonShortPeriod_ms/10,
		pts-ds->lastUpdate[PERFMON_SHORTINTV_DR], tasglb->perfmonCurrentPeriod_ms/10);
	UB_LOG(UBL_DEBUGV, "%s: lDiff=%d>=%d lastUpdate=%d>=%d\n", __func__,
		pts-ds->PMTime[PERFMON_LONGINTV_DR], tasglb->perfmonLongPeriod_ms/10,
		pts-ds->lastUpdate[PERFMON_LONGINTV_DR], tasglb->perfmonCurrentPeriod_ms/10);
	#endif

	if(pts-ds->PMTime[PERFMON_SHORTINTV_DR]>=(tasglb->perfmonShortPeriod_ms/10)){
		if(gptp_port_perfmon_dr_submit(ds, PERFMON_SHORTINTV_DR, tasglb->gptpInstanceIndex, di, pi, cts64)){
			UB_LOG(UBL_ERROR, "%s: Error submitting short interval port=%d data record\n", __func__, pi);
		}
		ds->lastUpdate[PERFMON_SHORTINTV_DR]=pts;
	}else if(pts-ds->lastUpdate[PERFMON_SHORTINTV_DR]>=(tasglb->perfmonCurrentPeriod_ms/10)){
		if(gptp_port_perfmon_dr_update(ds, PERFMON_SHORTINTV_DR, tasglb->gptpInstanceIndex, di, pi)){
			UB_LOG(UBL_ERROR, "%s: Error updating short interval port=%d data record\n", __func__, pi);
		}
		ds->lastUpdate[PERFMON_SHORTINTV_DR]=pts;
	}

	if(pts-ds->PMTime[PERFMON_LONGINTV_DR]>=(tasglb->perfmonLongPeriod_ms/10)){
		if(gptp_port_perfmon_dr_submit(ds, PERFMON_LONGINTV_DR, tasglb->gptpInstanceIndex, di, pi, cts64)){
			UB_LOG(UBL_ERROR, "%s: Error submitting long interval port=%d data record\n", __func__, pi);
		}
		ds->lastUpdate[PERFMON_LONGINTV_DR]=pts;
	}else if(pts-ds->lastUpdate[PERFMON_LONGINTV_DR]>=(tasglb->perfmonCurrentPeriod_ms/10)){
		if(gptp_port_perfmon_dr_update(ds, PERFMON_LONGINTV_DR, tasglb->gptpInstanceIndex, di, pi)){
			UB_LOG(UBL_ERROR, "%s: Error submitting long interval port=%d data record\n", __func__, pi);
		}
		ds->lastUpdate[PERFMON_LONGINTV_DR]=pts;
	}
}

void gptp_port_perfmon_dr_dump(PerfMonPortDS *ds, ub_dbgmsg_level_t lv, uint8_t id, uint8_t di, uint8_t pi)
{
	if((!ds)||(id>=PERFMON_ALL_DR)) return;

	UB_VLOG(lv, "PortPeerDelayDataRecord [%s] domain=%d port=%d index=%d PMTime=%d\n",
		id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval", di, pi, ds->index[id], ds->PMTime[id]);
	UB_VLOG(lv, "\t maxMeanLinkDelay=0x%"PRIx64" minMeanLinkDelay=0x%"PRIx64"\n",
		ds->pdelayDR[id].maxMeanLinkDelay.scaledNanoseconds, ds->pdelayDR[id].minMeanLinkDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t aveMeanLinkDelay=0x%"PRIx64" stdDevMeanLinkDelay=0x%"PRIx64"\n",
		ds->pdelayDR[id].averageMeanLinkDelay.scaledNanoseconds, ds->pdelayDR[id].stdDevMeanLinkDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t meanLinkDelayCount=%d\n", ds->pdelayDR[id].meanLinkDelay.N);

	UB_VLOG(lv, "PortDataRecord [%s] domain=%d port=%d index=%d PMTime=%d\n",
		id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval", di, pi, ds->index[id], ds->PMTime[id]);
	UB_VLOG(lv, "\t announceTx=%d announceRx=%d announceForeignMasterRx=%d\n",
		ds->portDR[id].announceTx, ds->portDR[id].announceRx, ds->portDR[id].announceForeignMasterRx);
	UB_VLOG(lv, "\t syncTx=%d syncRx=%d\n", ds->portDR[id].syncTx, ds->portDR[id].syncRx);
	UB_VLOG(lv, "\t followUpTx=%d followUpRx=%d\n", ds->portDR[id].followUpTx, ds->portDR[id].followUpRx);
	UB_VLOG(lv, "\t delayReqTx=%d delayReqRx=%d\n", ds->portDR[id].delayReqRx, ds->portDR[id].delayReqTx);
	UB_VLOG(lv, "\t delayRespTx=%d delayRespRx=%d\n", ds->portDR[id].delayRespTx, ds->portDR[id].delayRespRx);
	UB_VLOG(lv, "\t pDelayReqTx=%d pDelayReqRx=%d\n", ds->portDR[id].pDelayReqTx, ds->portDR[id].pDelayReqRx);
	UB_VLOG(lv, "\t pDelayRespTx=%d pDelayRespRx=%d\n", ds->portDR[id].pDelayRespTx, ds->portDR[id].pDelayRespRx);
	UB_VLOG(lv, "\t pDelayRespFollowUpTx=%d pDelayRespFollowUpRx=%d\n",
		ds->portDR[id].pDelayRespFollowUpTx, ds->portDR[id].pDelayRespFollowUpRx);

	UB_VLOG(lv, "PortSignalingDataRecord [%s] domain=%d port=%d index=%d PMTime=%d\n",
		id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval", di, pi, ds->index[id], ds->PMTime[id]);
	UB_VLOG(lv, "\t msgIntervalTx=%d msgIntervalRx=%d\n",
		ds->signalingDR[id].msgIntervalTx, ds->signalingDR[id].msgIntervalRx);
	UB_VLOG(lv, "\t asCapableTx=%d asCapableRx=%d\n", ds->signalingDR[id].asCapableTx, ds->signalingDR[id].asCapableTx);
	UB_VLOG(lv, "\t signalingRx=%d\n", ds->signalingDR[id].signalingRx);
}

static void gptp_port_perfmon_ppmpddr_reset(PortPerfMonPeerDelayDataRecord *dr)
{
	memset(dr, 0, sizeof(PortPerfMonPeerDelayDataRecord));
	/* IEE1588-2019 J.5.2.1
	 * The following initialization values are specified for all records in the list...
	 * PortPerformanceMonitoringPeerDelayDataRecord.<parameter x>:
	 * one in all bits, except the most significant.
	 */
	dr->averageMeanLinkDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->minMeanLinkDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->maxMeanLinkDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->stdDevMeanLinkDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
}

static void gptp_port_perfmon_ppmdr_reset(PortPerfMonDataRecord *dr){
	memset(dr, 0, sizeof(PortPerfMonDataRecord));
}

static void gptp_port_perfmon_ppmsdr_reset(PortPerfMonSignalingDataRecord *dr){
	memset(dr, 0, sizeof(PortPerfMonSignalingDataRecord));
}

static void gptp_port_perfmon_dr_reset_byid(PerfMonPortDS *ds, uint8_t id, uint32_t pmtime, uint64_t cts64){
	gptp_port_perfmon_ppmpddr_reset(&(ds->pdelayDR[id]));
	gptp_port_perfmon_ppmdr_reset(&(ds->portDR[id]));
	gptp_port_perfmon_ppmsdr_reset(&(ds->signalingDR[id]));
	/* IEE1588-2019 J.5.2.2 performanceMonitoringPortDS.recordList
	 * - 15min (short) measurement record current record at index 0
	 * - 24hour (long) measurement record current record at index 97
	 */
	ds->index[id]=(id==PERFMON_SHORTINTV_DR)?SHORTINTV_START_IDX:LONGINTV_START_IDX;
	ds->PMTime[id]=pmtime;
	ds->lastUpdate[id]=cts64;
}

/**
 * Reset Port Performance Monitoring Data Record
 *
 * Operation is on a per port level where all records corresponding to the port are
 * updated at the same time. Operating on each record separately is not suported
 * as of this time. Records are seperated as to follow standards and provide
 * provision for future where per record operations is required separately.
*/
void gptp_port_perfmon_dr_reset(PerfMonPortDS *ds, uint8_t type, uint8_t di, uint8_t pi, uint64_t cts)
{
	uint32_t pmtime;
	if(!ds) return;
	// align announce time to 10ms and follow standards requiring ms unit
	// ieee1588-ptp.yang leaf pm-time
	// ...System time is an unsigned integer in units of 10milliseconds
	pmtime=((cts/10000000u)*10000000u)/(10*UB_MSEC_NS);
	switch(type){
		case PERFMON_ALL_DR:
			gptp_port_perfmon_dr_reset_byid(ds, PERFMON_SHORTINTV_DR, pmtime, cts);
			gptp_port_perfmon_dr_reset_byid(ds, PERFMON_LONGINTV_DR, pmtime, cts);

			gptp_port_perfmon_dr_dump(ds, UBL_DEBUGV, PERFMON_SHORTINTV_DR, di, pi);
			gptp_port_perfmon_dr_dump(ds, UBL_DEBUGV, PERFMON_LONGINTV_DR, di, pi);
			break;
		case PERFMON_SHORTINTV_DR:
			gptp_port_perfmon_dr_reset_byid(ds, PERFMON_SHORTINTV_DR, pmtime, cts);
			break;
		case PERFMON_LONGINTV_DR:
			gptp_port_perfmon_dr_reset_byid(ds, PERFMON_LONGINTV_DR, pmtime, cts);
			break;
	}
}

static void gptp_port_perfmon_ppmpddr_dr_add(PortPerfMonPeerDelayDataRecord *dr, PPMPDDR_params_t p, int64_t v)
{
	int64_t scaledns=v*SCALEDNS_FACTOR;
	int64_t max=0, *drmax=NULL, min=0, *drmin=NULL, *drave=NULL, *drstd=NULL;
	PerfMonAncillaryDataRecord *anc=NULL;
	switch(p){
		case PPMPDDR_meanLinkDelay:
			drmax=&dr->maxMeanLinkDelay.scaledNanoseconds;
			max=*drmax;
			drmin=&dr->minMeanLinkDelay.scaledNanoseconds;
			min=*drmin;
			drave=&dr->averageMeanLinkDelay.scaledNanoseconds;
			drstd=&dr->stdDevMeanLinkDelay.scaledNanoseconds;
			anc=&dr->meanLinkDelay;
			break;
		default:
			// do nothing
			return;
	}
	*drmax=((max==INVALID_TIMEINTERVAL_VALUE)||(max<scaledns))?scaledns:max;
	*drmin=((min==INVALID_TIMEINTERVAL_VALUE)||(min>scaledns))?scaledns:min;
	perfmon_running_stat_add(anc,v);
	*drave=anc->M*SCALEDNS_FACTOR;
	*drstd=sqrt(anc->V)*SCALEDNS_FACTOR;
}

void gptp_port_perfmon_ppmpddr_add(PerfMonPortDS *ds, PPMPDDR_params_t p, int64_t v)
{
	if(!ds) return;
	gptp_port_perfmon_ppmpddr_dr_add(&ds->pdelayDR[PERFMON_SHORTINTV_DR], p, v);
	gptp_port_perfmon_ppmpddr_dr_add(&ds->pdelayDR[PERFMON_LONGINTV_DR], p, v);
}


/* ClockPerfmanceMonitoringDataRecord */

/**
 * Submit the current PerfMonClockDS for collection (e.g. to uniconf).
 * Afterwards, the current record is reset.
 *
 * In the case of uniconf, it is expected that the current data record is submitted
 * as index=1 (98 for long interval), cascading old records.
 * Then a new data record is submitted for index=0 (or 97).
 */
static int gptp_clock_perfmon_dr_submit(PerfMonClockDS *ds, uint8_t id,
                                        uint8_t gptpInstanceIndex, uint8_t di, uint64_t cts64)
{
	UB_LOG(UBL_DEBUG, "%s: submit PerformanceMonitoringClockDS [%s]\n",
		__func__, id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval");
	/* IEE1588-2019 J.5.1.2 performanceMOnitoringDS.recordList
	 * - 15min (short) measurement record current record at index 0
	 *   thus, first historical records is at index 1
	 * - 24hour (long) measurement record current record at index 97
	 *   thus, first historical records is at index 98
	 */
	// cascade records
	if(gptpgcfg_cascade_clock_perfmonDS(id, gptpInstanceIndex, di)){
		UB_LOG(UBL_WARN, "%s: Unable to cascade historic records.\n", __func__);
		return -1;
	}
	ds->index[id]=(id==PERFMON_SHORTINTV_DR)?SHORTINTV_START_IDX+1:LONGINTV_START_IDX+1;
	gptp_clock_perfmon_dr_dump(ds, UBL_DEBUG, id);
	// store current record, with periodComplete=true
	if(gptpgcfg_set_clock_perfmonDS(ds, id, true, gptpInstanceIndex, di)){
		UB_LOG(UBL_ERROR, "%s: Unable to submit current record.\n", __func__);
		return -1;
	}
	gptp_clock_perfmon_dr_reset(ds, id, cts64);
	if(gptpgcfg_set_clock_perfmonDS(ds, id, false, gptpInstanceIndex, di)){
		UB_LOG(UBL_WARN, "%s: Unable to submit new empty current record.\n", __func__);
		return -1;
	}
	return 0;
}

/**
 * Update the current PerformanceMonitoringClockDS (presumably already written
 * into database as index=0 or index=97) with new value.
 * Change in values in the data records are not updated into the database in real-time,
 * in order to provide performance based tweaking.
 * This periodicity of this update is configurable by perfmonCurrentPeriod_ms.
 */
static int gptp_clock_perfmon_dr_update(PerfMonClockDS *ds, uint8_t id,
                                        uint8_t gptpInstanceIndex, uint8_t domainIndex)
{
	UB_LOG(UBL_DEBUG, "%s: updating PerformanceMonitoringPortDS [%s]\n",
		__func__, id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval");
	gptp_clock_perfmon_dr_dump(ds, UBL_DEBUG, id);
	return gptpgcfg_set_clock_perfmonDS(ds, id, false, gptpInstanceIndex, domainIndex);
}

void gptp_clock_perfmon(PerfMonClockDS *ds, uint64_t cts64, PerTimeAwareSystemGlobal *tasglb, uint8_t di)
{
	if(!ds) return;
	if(!tasglb) return;

	/*
	 * Check the following periodicity for collecting data (in order):
	 * - Short interval : collect then reset data record
	 * - Long interval : collect then reset data record
	 */
	PMTimestamp pts=cts64/(UB_MSEC_NS*10);

	if(pts-ds->PMTime[PERFMON_SHORTINTV_DR]>=(tasglb->perfmonShortPeriod_ms/10)){
		if(gptp_clock_perfmon_dr_submit(ds, PERFMON_SHORTINTV_DR, tasglb->gptpInstanceIndex, di, cts64)){
			UB_LOG(UBL_ERROR, "%s: Error submitting short interval clock data record\n", __func__);
		}
		ds->lastUpdate[PERFMON_SHORTINTV_DR]=pts;
	}else if(pts-ds->lastUpdate[PERFMON_SHORTINTV_DR]>=(tasglb->perfmonCurrentPeriod_ms/10)){
		if(gptp_clock_perfmon_dr_update(ds, PERFMON_SHORTINTV_DR,
		                                tasglb->gptpInstanceIndex, tasglb->domainIndex)){
			UB_LOG(UBL_ERROR, "%s: Error updating short interval clock data record\n", __func__);
		}
		ds->lastUpdate[PERFMON_SHORTINTV_DR]=pts;
	}

	if(pts-ds->PMTime[PERFMON_LONGINTV_DR]>=(tasglb->perfmonLongPeriod_ms/10)){
		if(gptp_clock_perfmon_dr_submit(ds, PERFMON_LONGINTV_DR, tasglb->gptpInstanceIndex, di, cts64)){
			UB_LOG(UBL_ERROR, "%s: Error submitting long interval clock data record\n", __func__);
		}
		ds->lastUpdate[PERFMON_LONGINTV_DR]=pts;
	}else if(pts-ds->lastUpdate[PERFMON_LONGINTV_DR]>=(tasglb->perfmonCurrentPeriod_ms/10)){
		if(gptp_clock_perfmon_dr_update(ds, PERFMON_LONGINTV_DR,
		                                tasglb->gptpInstanceIndex, tasglb->domainIndex)){
			UB_LOG(UBL_ERROR, "%s: Error submitting long interval clock data record\n", __func__);
		}
		ds->lastUpdate[PERFMON_LONGINTV_DR]=pts;
	}
}

void gptp_clock_perfmon_dr_dump(PerfMonClockDS *ds, ub_dbgmsg_level_t lv, uint8_t id)
{
	if((!ds)||(id>=PERFMON_ALL_DR)) return;

	UB_VLOG(lv, "ClockPerfMonDataRecord [%s] index=%d PMTime=%d\n",
		id==PERFMON_SHORTINTV_DR?"ShortInterval":"LongInterval", ds->index[id], ds->PMTime[id]);

	UB_VLOG(lv, "\t maxMasterSlaveDelay=0x%"PRIx64" minMasterSlaveDelay=0x%"PRIx64"\n",
		ds->clockDR[id].maxMasterSlaveDelay.scaledNanoseconds, ds->clockDR[id].minMasterSlaveDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t aveMasterSlaveDelay=0x%"PRIx64" stdDevMeanLinkDelay=0x%"PRIx64"\n",
		ds->clockDR[id].averageMasterSlaveDelay.scaledNanoseconds, ds->clockDR[id].stdDevMasterSlaveDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t masterSlaveDelayCount=%d\n", ds->clockDR[id].masterSlaveDelay.N);

	UB_VLOG(lv, "\t maxSlaveMasterDelay=0x%"PRIx64" minSlaveMasterDelay=0x%"PRIx64"\n",
		ds->clockDR[id].maxSlaveMasterDelay.scaledNanoseconds, ds->clockDR[id].minSlaveMasterDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t averageSlaveMasterDelay=0x%"PRIx64" stdDevSlaveMasterDelay=0x%"PRIx64"\n",
		ds->clockDR[id].averageSlaveMasterDelay.scaledNanoseconds, ds->clockDR[id].stdDevSlaveMasterDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t slaveMasterDelayCount=%d\n", ds->clockDR[id].slaveMasterDelay.N);

	UB_VLOG(lv, "\t maxMeanPathDelay=0x%"PRIx64" minMeanPathDelay=0x%"PRIx64"\n",
		ds->clockDR[id].maxMeanPathDelay.scaledNanoseconds, ds->clockDR[id].minMeanPathDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t averageMeanPathDelay=0x%"PRIx64" stdDevMeanLinkDelay=0x%"PRIx64"\n",
		ds->clockDR[id].averageMeanPathDelay.scaledNanoseconds, ds->clockDR[id].stdDevMeanPathDelay.scaledNanoseconds);
	UB_VLOG(lv, "\t meanPathDelayCount=%d\n", ds->clockDR[id].meanPathDelay.N);

	UB_VLOG(lv, "\t maxOffsetFromMaster=0x%"PRIx64" minOffsetFromMaster=0x%"PRIx64"\n",
		ds->clockDR[id].maxOffsetFromMaster.scaledNanoseconds, ds->clockDR[id].minOffsetFromMaster.scaledNanoseconds);
	UB_VLOG(lv, "\t averageOffsetFromMaster=0x%"PRIx64" stdDevOffsetFromMaster=0x%"PRIx64"\n",
		ds->clockDR[id].averageOffsetFromMaster.scaledNanoseconds, ds->clockDR[id].stdDevOffsetFromMaster.scaledNanoseconds);
	UB_VLOG(lv, "\t offsetFromMasterCount=%d\n", ds->clockDR[id].offsetFromMaster.N);
}

static void gptp_clock_perfmon_cpmdr_reset(ClockPerfMonDataRecord *dr)
{
	memset(dr, 0, sizeof(ClockPerfMonDataRecord));
	/* IEE1588-2019 J.5.1.2
	 * The following initialization values are specified for all records in the list...
	 * ClockPerfmanceMonitoringDataRecord.<parameter x>:
	 * one in all bits, except the most significant.
	 */
	dr->averageMasterSlaveDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->minMasterSlaveDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->maxMasterSlaveDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->stdDevMasterSlaveDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;

	dr->averageSlaveMasterDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->minSlaveMasterDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->maxSlaveMasterDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->stdDevSlaveMasterDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;

	dr->averageMeanPathDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->minMeanPathDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->maxMeanPathDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->stdDevMeanPathDelay.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;

	dr->averageOffsetFromMaster.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->minOffsetFromMaster.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->maxOffsetFromMaster.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
	dr->stdDevOffsetFromMaster.scaledNanoseconds=INVALID_TIMEINTERVAL_VALUE;
}

static void gptp_clock_perfmon_dr_reset_byid(PerfMonClockDS *ds, uint8_t id, uint32_t pmtime, uint64_t cts64){
	gptp_clock_perfmon_cpmdr_reset(&(ds->clockDR[id]));
	/* IEE1588-2019 J.5.2.2 performanceMonitoringPortDS.recordList
	 * - 15min (short) measurement record current record at index 0
	 * - 24hour (long) measurement record current record at index 97
	 */
	ds->index[id]=(id==PERFMON_SHORTINTV_DR)?SHORTINTV_START_IDX:LONGINTV_START_IDX;
	ds->PMTime[id]=pmtime;
	ds->lastUpdate[id]=cts64;
}

/**
 * Reset Clock Performance Monitoring Data Record
 */
void gptp_clock_perfmon_dr_reset(PerfMonClockDS *ds, uint8_t type, uint64_t cts)
{
	uint32_t pmtime;
	if(!ds) return;
	// align announce time to 10ms and follow standards requiring ms unit
	// ieee1588-ptp.yang leaf pm-time
	// ...System time is an unsigned integer in units of 10milliseconds
	pmtime=((cts/10000000u)*10000000u)/(10*UB_MSEC_NS);
	switch(type){
		case PERFMON_ALL_DR:
			gptp_clock_perfmon_dr_reset_byid(ds, PERFMON_SHORTINTV_DR, pmtime, cts);
			gptp_clock_perfmon_dr_reset_byid(ds, PERFMON_LONGINTV_DR, pmtime, cts);

			gptp_clock_perfmon_dr_dump(ds, UBL_DEBUGV, PERFMON_SHORTINTV_DR);
			gptp_clock_perfmon_dr_dump(ds, UBL_DEBUGV, PERFMON_LONGINTV_DR);
			break;
		case PERFMON_SHORTINTV_DR:
			gptp_clock_perfmon_dr_reset_byid(ds, PERFMON_SHORTINTV_DR, pmtime, cts);
			break;
		case PERFMON_LONGINTV_DR:
			gptp_clock_perfmon_dr_reset_byid(ds, PERFMON_LONGINTV_DR, pmtime, cts);
			break;
	}
}

static void gptp_clock_perfmon_cpmdr_dr_add(ClockPerfMonDataRecord *dr, CPMDR_params_t p, int64_t v)
{
	int64_t scaledns=v*SCALEDNS_FACTOR;
	int64_t max=0, *drmax=NULL, min=0, *drmin=NULL, *drave=NULL, *drstd=NULL;
	PerfMonAncillaryDataRecord *anc=NULL;
	switch(p){
		case CPMDR_masterSlaveDelay:
			drmax=&dr->maxMasterSlaveDelay.scaledNanoseconds;
			max=*drmax;
			drmin=&dr->minMasterSlaveDelay.scaledNanoseconds;
			min=*drmin;
			drave=&dr->averageMasterSlaveDelay.scaledNanoseconds;
			drstd=&dr->stdDevMasterSlaveDelay.scaledNanoseconds;
			anc=&dr->masterSlaveDelay;
			break;
		case CPMDR_slaveMasterDelay:
			drmax=&dr->maxSlaveMasterDelay.scaledNanoseconds;
			max=*drmax;
			drmin=&dr->minSlaveMasterDelay.scaledNanoseconds;
			min=*drmin;
			drave=&dr->averageSlaveMasterDelay.scaledNanoseconds;
			drstd=&dr->stdDevSlaveMasterDelay.scaledNanoseconds;
			anc=&dr->slaveMasterDelay;
			break;
		case CPMDR_meanPathDelay:
			// used only when Delay (not PDelay) mechanism
			drmax=&dr->maxMeanPathDelay.scaledNanoseconds;
			max=*drmax;
			drmin=&dr->minMeanPathDelay.scaledNanoseconds;
			min=*drmin;
			drave=&dr->averageMeanPathDelay.scaledNanoseconds;
			drstd=&dr->stdDevMeanPathDelay.scaledNanoseconds;
			anc=&dr->meanPathDelay;
			break;
		case CPMDR_offsetFromMaster:
			drmax=&dr->maxOffsetFromMaster.scaledNanoseconds;
			max=*drmax;
			drmin=&dr->minOffsetFromMaster.scaledNanoseconds;
			min=*drmin;
			drave=&dr->averageOffsetFromMaster.scaledNanoseconds;
			drstd=&dr->stdDevOffsetFromMaster.scaledNanoseconds;
			anc=&dr->offsetFromMaster;
			break;
		default:
			// do nothing
			return;
	}
	*drmax=((max==INVALID_TIMEINTERVAL_VALUE)||(max<scaledns))?scaledns:max;
	*drmin=((min==INVALID_TIMEINTERVAL_VALUE)||(min>scaledns))?scaledns:min;
	perfmon_running_stat_add(anc,v);
	*drave=anc->M*SCALEDNS_FACTOR;
	*drstd=sqrt(anc->V)*SCALEDNS_FACTOR;
}

void gptp_clock_perfmon_cpmdr_add(PerfMonClockDS *ds, CPMDR_params_t p, int64_t v)
{
	if(!ds) return;
	gptp_clock_perfmon_cpmdr_dr_add(&ds->clockDR[PERFMON_SHORTINTV_DR], p, v);
	gptp_clock_perfmon_cpmdr_dr_add(&ds->clockDR[PERFMON_LONGINTV_DR], p, v);
}
