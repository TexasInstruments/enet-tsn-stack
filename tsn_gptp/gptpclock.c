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
#include <limits.h>
#include <tsn_unibase/unibase.h>
#include "gptpclock.h"
#include "gptpcommon.h"
#include "tsn_uniconf/yangs/ieee1588-ptp-tt_access.h"
#include "gptpconf/gptpgcfg.h"

/*
|------------------------+-------------+------------+-------------+------------|
|                        | HW PhaseAdj | HW FreqAdj | SW PhaseAdj | SW FreqAdj |
|------------------------+-------------+------------+-------------+------------|
| M:PTPCLOCK_MASTER      | No          | No         | Yes         | No         |
| SM:PTPCLOCK_SLAVE_MAIN | Yes         | Yes        | No          | No         |
| SS:PTPCLOCK_SLAVE_SUB  | No          | No         | Yes         | Yes        |
|------------------------+-------------+------------+-------------+------------|
HW PhaseAdj, FreqAdj: applied on ptp device(like /dev/ptp0) clock directly
SW PhaseAdj, FreqAdj: applied on software conversion

When there are 3 Network device and each one has independent PTP Clock device,
the clocks become as follows:
|-----+----------+--------------+---------------+--------------+--------------|
|     |          | masterClock  | thisClock     |              |              |
| TAS |          | Clk0(M,ptp0) | Clk1(SM,ptp0) | Clk2(M,ptp1) | Clk3(M,ptp3) |
|-----+----------+--------------+---------------+--------------+--------------|
| D0  | PhaseAdj | SW           |               |              |              |
|     | FreqAdj  | by Clk1      | HW            |              |              |
|-----+----------+--------------+---------------+--------------+--------------|
masterClock and thisClock work together to provide the ptp clock of 'D0 TAS'.
Because applying HW phase adjustment upset time-stamping, we never do it
other than the initialization time.

HW frequency adjustment is applied on 'thisClock',
the same adjustment is also applied on 'masterClock',
because they are based on the same 'ptp0' clock device.

'masterClock' is exported as 'ptp clock of D0', and by the SW PhaseAdj and
the HW FreqAdj, it shynchronize to 'GM clock'.

timestamps are always stamped by direct ptp device HW clock value.
In a single clock system, there is nothing complicated, and no conversions are needed.

Although it is easy to understand that PdelayReq and PdelayResp timestamps are used only
for one port and that timestamp value conversions are not needed
(since PdelayReq reception time and PdelayResp transmit time are always based on the same clock),
conversions are actually needed in the case of a switch with dedicated clocks for each port.
For example:

GM ---> SW:(clk1, port1) and (clk2, port2) ---> SLAVE(clk_slave)
The SW propagates information to calculate its rateRatio(clk1) expressed in clk1 to the SLAVE.
The SLAVE calculates neighborRateRatio(clk2) with the SW expressed in clk2 time.
The SLAVE now have rateRatio(clk1) and neighborRateRatio(clk2), then can calculate its rateRatio:

rateRatio(clk_slave) = rateRatio(clk1) * neighborRateRatio(clk2)

However, since clk1 and clk2 are two different clocks, this calculation will be incorrect.
To correct this, we must convert all timestamps related to PdelayReq and PdelayResp to clk1:

rateRatio(clk_slave) = rateRatio(clk1) * neighborRateRatio(clk1)

For Sync, which is received on one port and transmitted on the other port.
Two different clocks are possibly involved, and timestamp value conversions are required.

For the conversions, we use 'thisClock' as a common clock, and convert all timestamps to
'thisClock' based value.

E.G. when a Sync message comes on port2, which is based on Clk2: SyncRecTS(clk2)
this Sync message propagates to the other ports.
When it is sent on port3, get a timestamp based on Clk3: SyncSendTS(clk3)
We convert those values:
  SyncRecTS(clk2) --> SyncRecTS(thisClock)
  SyncSendTS(clk3) --> SyncSendTS(thisClock)
Then, 'SyncSendTS(thisClock) - SyncRecTS(thisClock)' can be calculated for the correction field.


The things are more complicated for multiple domains.

case 1: 'thisClock of D0' and 'thisClock of D1' are based on different clocks.
|-----+----------+--------------+---------------+---------------+--------------|
|     |          | masterClock  | thisClock     |               |              |
| TAS |          | Clk0(M,ptp0) | Clk1(SM,ptp0) | Clk2(M,ptp1)  | Clk3(M,ptp3) |
|-----+----------+--------------+---------------+---------------+--------------|
| D0  | PhaseAdj | SW           |               |               |              |
|     | FreqAdj  | by Clk1      | HW            | HW(D1)        |              |
|-----+----------+--------------+---------------+---------------+--------------|
|     |          | masterClock  |               | thisClock     |              |
| TAS |          | Clk0(M,ptp1) | Clk1(M,ptp0)  | Clk2(SM,ptp1) | Clk3(M,ptp3) |
|-----+----------+--------------+---------------+---------------+--------------|
| D1  | PhaseAdj | SW           |               |               |              |
|     | FreqAdj  | by Clk2      | HW(D0)        | HW            |              |
|-----+----------+--------------+---------------+---------------+--------------|

case 2: 'thisClock of D0' and 'thisClock of D1' are based on the same clock.
|-----+----------+--------------+---------------+--------------+--------------|
|     |          | masterClock  | thisClock     |              |              |
| TAS |          | Clk0(M,ptp0) | Clk1(SM,ptp0) | Clk2(M,ptp1) | Clk3(M,ptp3) |
|-----+----------+--------------+---------------+--------------+--------------|
| D0  | PhaseAdj | SW           |               |              |              |
|     | FreqAdj  | by Clk1      | HW            |              |              |
|-----+----------+--------------+---------------+--------------+--------------|
|     |          | masterClock  | thisClock     |              |              |
| TAS |          | Clk0(M,ptp0) | Clk1(SS,ptp0) | Clk2(M,ptp1) | Clk3(M,ptp3) |
|-----+----------+--------------+---------------+--------------+--------------|
| D1  | PhaseAdj | SW           |               |              |              |
|     | FreqAdj  | by Clk1      | SW+HW(D0)     |              |              |
|-----+----------+--------------+---------------+--------------+--------------|

HW adjustment are always applied on the same clock port for all domains.

For the same example,
  D0 Sync propagation from port2 to port3 becomes as follows:
    SyncRecTS(clk2D0) --> SyncRecTS(thisClockD0)
    SyncSendTS(clk3D0) --> SyncSendTS(thisClockD0)
  D1 Sync propagation from port2 to port3 becomes as follows:
    SyncRecTS(clk2D1) --> SyncRecTS(thisClockD1)
    SyncSendTS(clk3D1) --> SyncSendTS(thisClockD1)

domains other than D0, only 'masterClock' and 'thisClock' are needed,
and other clocks are not generated, and D0 clocks are used for them.
For the 'case 2' above, clk2D0 and clk3D0 are used for clk2D1 and clk3D1.
For the 'case 1' above, clk2D0 and clk2D1 are the same.
For D1, it becomes as follows:
  D1 Sync propagation from port2 to port3 becomes as follows:
    SyncRecTS(clk2D0) --> SyncRecTS(thisClockD1)
    SyncSendTS(clk3D0) --> SyncSendTS(thisClockD1)

For 'case 2', thisClock(Clk1) for D1 has additional SW frequency adjustment.
It is not applied for timestamps, and timestamps are based on D0 clock.
When conversions of 'TS(thisClockD0) --> TS(thisClockD1)' are done,
the SW frequency adjustment is applied.

After all, whatever Sync timestamps, use D0 clock as source, and convert to 'thisClock of TAS'
    TS(clkND0) --> TS(thisClock of TAS): 'thisClockD0 for D0', 'thisClockD1 for D1'
This works for all Sync TS conversions.

*/

/*
To sync between GM-D0 and GM-D1, use the case 2 above.
case 2: 'thisClock of D0' and 'thisClock of D1' are based on the same clock.

And when this device becomes GM in D1, set 0 for 'FreqAdj of thisClock of D1'.
Then Freq. of GM in D1 sync to Freq of GM in D0.
'phase of thisClock of D1' is synced to 'phase of thisClock of D0'

For this, 'adjrate' and 'last_setts64' must be shared by the shared memory,
and gptpmasterclock should be accesible for these values.
'adjrate' and 'last_setts64' for 'This Clock' is copied to shared memory.

Further more, when this device starts up and becomes GM of D0,
let this device become slave for D0 first and sync to existing GM,
after that this device takes over GM. With this way, there is no big jump in GM of D0.

When 'thisClock' is in PTPCLOCK_SLAVE_SUB mode, 'offset64' is used
on the clock. Even though it applies only 'adjrate' explicitly, SW frequency
adjustment updates the phase adjustment(offset64).
'offset64' of 'thisClock' and 'masterClock', the both of them must be maintained
separately, and the combined value must be copied into the shared memory.
*/

/*
  switching of the active domain
  Currently there is no rule for this in the standard.

  We introduce 'active domain' with which applications get the ptp clock.
  it is controled by GM status of each domain, and XL4_EXTMOD_XL4GPTP_ACTIVE_DOMAIN_AUTO_SWITCH
 */

#define LASTGM_OFFSET64_INVALID LLONG_MAX
#define LASTGM_ADJVPPB_INVALID INT_MAX

typedef enum {
	PTPCLOCK_MASTER = 0, // no adjustment
	PTPCLOCK_SLAVE_MAIN, // hw adjustment(use clock_adjtime)
	PTPCLOCK_SLAVE_SUB, // sw adjustment(use internal conversion)
} ptpclock_mode_t;

typedef struct oneclock_data {
	gptp_clock_ppara_t *pp;
	gptp_clock_ppara_t ppe;
	int clockIndex;
	ClockIdentity clockId;
	PTPFD_TYPE ptpfd;
	ptpclock_mode_t mode;
	int domainIndex;
	ptpclock_state_t state;
	int64_t offset64; // this is not the same as pp->offset64
	int64_t last_setts64; // this is not the same as pp->last_setts64
	double adjrate; // this is not the same as pp->adjrate
	int adjvppb;
	int ts2diff;
} oneclock_data_t;

typedef struct per_domain_data {
	struct timespec last_gmch_ts;
	bool waiting_d0_sync;
	bool we_are_gm;
	ClockIdentity gmClockId;
	int thisClockIndex;
	int thisClock_adjppb;
} per_domain_data_t;

struct gptpclock_data {
	ub_esarray_cstd_t *clds;
	int shmfd;
	int shmsize;
	gptp_master_clock_shm_t *shm;
	per_domain_data_t *pdd;
	int active_domain_switch;
};

static gptpclock_data_t** gcdl;
static uint8_t gcdl_num;

static int gptpclock_getts_od(int64_t *ts64, oneclock_data_t *od);
static int gptpclock_setoffset_od(gptpclock_data_t *gcd, oneclock_data_t *od);
static oneclock_data_t *get_clockod(gptpclock_data_t *gcd,
				    int clockIndex, uint8_t domainIndex);

#define GPTPCLOCK_FN_ENTRY(gcd,od,clockIndex,domainIndex) {		\
		if(!(gcd)->clds){return -1;}				\
		(od)=get_clockod(gcd, clockIndex, domainIndex);	\
		if((od)==NULL){return -1;}				\
		if(!PTPFD_VALID((od)->ptpfd)){return -1;}		\
	}

#ifdef HIGH_ACCURATE_CLOCKS_DIFF
/* the platform must support this function */
int gptp_clock_accurate_convert(PTPFD_TYPE ptpfd, PTPFD_TYPE ptpfd1, int64_t *ts64);

#define GPTPCLOCK_CONVERT(tp,od0,od1) \
	gptp_clock_accurate_convert((od0)->ptpfd,(od1)->ptpfd,tp)

#else //!HIGH_ACCURATE_CLOCKS_DIFF

static int diff_in_two_clocks(int64_t *tss64, oneclock_data_t *od,
			      oneclock_data_t *od1)
{
	int64_t ts1=-1, ts2=-1, ts3=-1;

	// get (ts2-ts1) - (ts3-ts1)/2
	if(gptpclock_getts_od(&ts1, od)!=0){
		UB_LOG(UBL_ERROR, "%s:can't get ts1=TS(clk=%d,D=%d)\n",
		       __func__, od->clockIndex, od->pp->domainIndex);
		return -2;
	}
	if(gptpclock_getts_od(&ts2, od1)!=0){
		UB_LOG(UBL_ERROR, "%s:can't get ts2=TS(clk=%d,D=%d)\n",
		       __func__, od1->clockIndex, od1->pp->domainIndex);
		return -2;
	}
	if(gptpclock_getts_od(&ts3, od)!=0){
		UB_LOG(UBL_ERROR, "%s:can't get ts3=TS(clk=%d,D=%d)\n",
		       __func__, od->clockIndex, od->pp->domainIndex);
		return -2;
	}

	ts3=(ts3-ts1)/2;
	if((od->ts2diff>0) && (ts3 > (od->ts2diff*10))){return -1;}
	*tss64=ts2-ts1-ts3;
	return 0;
}

static int gptpclock_convert(int64_t *ts64, oneclock_data_t *od,
			     oneclock_data_t *od1)
{
	int64_t dtss;

	if(diff_in_two_clocks(&dtss, od, od1)!=0){
		// in case of a fail by context switching, we'll try twice
		if(diff_in_two_clocks(&dtss, od, od1)!=0){
			UB_LOG(UBL_ERROR, "%s:can't convert ts, (ci=%d,di=%d)->(ci=%d,di=%d)\n",
			       __func__, od->clockIndex, od->domainIndex,
			       od1->clockIndex, od1->domainIndex);
			return -1;
		}
	}
	*ts64+=dtss;
	return 0;
}

#define GPTPCLOCK_CONVERT(tp,od0,od1) gptpclock_convert(tp,od0,od1)

#endif //!HIGH_ACCURATE_CLOCKS_DIFF


#ifdef GPTPCLOCK_OFFSET_USER
#include GPTPCLOCK_OFFSET_USER // deprecated mode
#else
/*
 * To set the ptp clock offset, use driver API
 * This is the default, and will be merged into 'gptpclock.c' in near future.
 */
static int gptpclock_setts_offset_od(uint8_t gptpInstanceIndex,
				     int64_t offset64, oneclock_data_t *od)
{
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTP_CLOCK_GETTIME(od->ptpfd, od->last_setts64);

	if(!od->clockIndex || (od->mode==PTPCLOCK_SLAVE_SUB)){
		(void)gptpclock_mutex_trylock(&gcd->shm->head.mcmutex);
	}

	od->offset64=offset64;
	if(od->mode==PTPCLOCK_SLAVE_MAIN){
		od->offset64=0;
		GPTP_CLOCK_SETOFFSET(od->ptpfd, offset64);
	}else{
		(void)gptpclock_setoffset_od(gcd, od);
	}

	if(!od->clockIndex || (od->mode==PTPCLOCK_SLAVE_SUB)){
		CB_THREAD_MUTEX_UNLOCK(&gcd->shm->head.mcmutex);
	}

	return 0;
}

static int gptpclock_setts_od(uint8_t gptpInstanceIndex, gptpclock_data_t *gcd,
			      int64_t ts64, oneclock_data_t *od)
{
	int64_t cts64;
	int64_t offset64;
	GPTP_CLOCK_GETTIME(od->ptpfd, cts64);
	offset64=ts64-cts64;
	(void)gptpclock_setts_offset_od(gptpInstanceIndex, offset64, od);
	return 0;
}

static int64_t time_setoffset64(uint8_t gptpInstanceIndex, gptpclock_data_t *gcd,
				int64_t offset64, int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	(void)gptpclock_setts_offset_od(gptpInstanceIndex, offset64, od);
	return 0;
}

static int avarage_time_setoffset(gptpclock_data_t *gcd, int clockIndex,
				  uint8_t domainIndex)
{
	return 0;
}
#endif // !GPTPCLOCK_OFFSET_USERSPACE

static oneclock_data_t *get_clockod(gptpclock_data_t *gcd,
				    int clockIndex, uint8_t domainIndex)
{
	int i;
	oneclock_data_t *od;
	for(i=0;i<ub_esarray_ele_nums(gcd->clds);i++){
		od = (oneclock_data_t *)ub_esarray_get_ele(gcd->clds, i);
		if((!od || od->clockIndex != clockIndex) ||
		   (od->pp->domainIndex != domainIndex)){continue;}
		return od;
	}
	return NULL;
}

static int gptpclock_getts_od(int64_t *ts64, oneclock_data_t *od)
{
	int64_t dts64=0;
	double adjrate;

	GPTP_CLOCK_GETTIME(od->ptpfd, *ts64);
	adjrate=od->adjrate;
	if((adjrate != 0.0) && (od->mode==PTPCLOCK_SLAVE_SUB)){
		// get dts, which is diff between now and last setts time
		dts64=*ts64-od->last_setts64;
		dts64=adjrate * (double)dts64;
		UB_LOG(UBL_DEBUGV, "%s:applied SW adjrate, dts=%"PRIi64"nsec\n",
		       __func__, dts64);
	}

	// add offset
	*ts64+=od->offset64;
	*ts64+=dts64;
	return 0;
}

static int gptpclock_setoffset_od(gptpclock_data_t *gcd, oneclock_data_t *od)
{
	oneclock_data_t *od0, *odt;

	if(od->mode==PTPCLOCK_MASTER){
		od0=get_clockod(gcd, 0, od->pp->domainIndex);
		odt=get_clockod(gcd, gcd->pdd[od->domainIndex].thisClockIndex,
				od->pp->domainIndex);
		if(odt && (od==od0)) {
			// offset64 in the shm must be updated with the one of 'thisClock'
			od->pp->offset64=od->offset64+odt->offset64;
		}
	}else if(od->mode==PTPCLOCK_SLAVE_SUB){
		od0=get_clockod(gcd, 0, od->pp->domainIndex);
		odt=get_clockod(gcd, gcd->pdd[od->domainIndex].thisClockIndex,
				od->pp->domainIndex);
		if(od0 && (odt==od)){
			od0->pp->last_setts64=od->last_setts64;
			od0->pp->offset64=od0->offset64+od->offset64;
		}
	}else{
		return -1;
	}
	return 0;
}

#define PTPCLOCK_OPEN_TOUT 100 // msec
/* It is okay to use ptpdev which doesn't belong to portIndex.
   In succh case, the mode shouldn't be SLAVE_MAIN  */
int gptpclock_add_clock(uint8_t gptpInstanceIndex, int clockIndex,
			char *ptpdev, int domainIndex,
			ClockIdentity id)
{
	int i;
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	uint32_t state=0;
	if(gcd==NULL){
		UB_LOG(UBL_ERROR, "%s:no gcd[%d]\n", __func__, gptpInstanceIndex);
		return -1;
	}
	if(!gcd->clds){return -1;}
	for(i=0;i<ub_esarray_ele_nums(gcd->clds);i++){
		od = (oneclock_data_t *)ub_esarray_get_ele(gcd->clds, i);
		if(od && (od->clockIndex == clockIndex) && (od->pp->domainIndex == domainIndex)){
			UB_LOG(UBL_ERROR,"%s:already exists, clockIndex=%d, domainIndex=%d\n",
			       __func__, clockIndex, domainIndex);
			return -1;
		}
	}
	gptpgcfg_set_clock_state_item(gptpInstanceIndex, IEEE1588_PTP_TT_GMSTATE,
				      domainIndex, YDBI_STATUS,
				      &state, sizeof(uint32_t), YDBI_NO_NOTICE);

	od = (oneclock_data_t *)ub_esarray_get_newele(gcd->clds);
	if(!od) {
		UB_LOG(UBL_ERROR, "%s:clockIndex=%d, ptpdev=%s failed to alloc memory\n",
		       __func__, clockIndex, ptpdev);
		return -1;
	}
	(void)memset(od, 0, sizeof(oneclock_data_t));
	if(clockIndex!=0){
		od->pp=&od->ppe;
	}else{
		//pp for id=0 must be shared with other processes
		od->pp=&gcd->shm->gcpp[domainIndex];
		(void)memset(od->pp, 0, sizeof(gptp_clock_ppara_t));
		od->pp->gmchange_ind=1; //start with 1
	}
	od->clockIndex=clockIndex;
	od->pp->domainIndex=domainIndex;
	od->domainIndex=domainIndex;
	memcpy(od->clockId, id, sizeof(ClockIdentity));
	od->state = gptp_get_ptpfd(gptpInstanceIndex, ptpdev, &od->ptpfd);
	if((od->state == PTPCLOCK_RDWR) || (od->state == PTPCLOCK_RDONLY)){
		(void)snprintf(od->pp->ptpdev, MAX_PTPDEV_NAME, "%s", ptpdev);
	}else{
		UB_LOG(UBL_ERROR, "%s:clockIndex=%d, ptpdev=%s is not accessible\n",
		       __func__, clockIndex, ptpdev);
		od->ptpfd=PTPFD_INVALID;
		(void)gptpclock_del_clock(gptpInstanceIndex, clockIndex, domainIndex);
		return -1;
	}
	od->ts2diff = avarage_time_setoffset(gcd, clockIndex, domainIndex);
	od->pp->offset64=0;
	od->offset64=0;
	UB_LOG(UBL_DEBUG, "%s:clockIndex=%d, ptpdev=%s, domainIndex=%d\n",
	       __func__, clockIndex, ptpdev, domainIndex);
	return 0;
}

int gptpclock_del_clock(uint8_t gptpInstanceIndex, int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	if(gcd==NULL){
		UB_LOG(UBL_ERROR, "%s:no gcd[%d]\n", __func__, gptpInstanceIndex);
		return -1;
	}
	if(!gcd->clds){return 0;}
	od=get_clockod(gcd, clockIndex, domainIndex);
	if(od!=NULL){
		if(PTPFD_VALID(od->ptpfd)){(void)gptp_close_ptpfd(od->ptpfd);}
		(void)ub_esarray_del_pointer(gcd->clds, (ub_esarray_element_t *)od);
		UB_LOG(UBL_DEBUG, "%s:clockIndex=%d, domainIndex=%d\n",
		       __func__, clockIndex, domainIndex);
		return 0;
	}
	UB_LOG(UBL_ERROR, "%s:doens't exist clockIndex=%d, domainIndex=%d\n",
	       __func__, clockIndex, domainIndex);
	return -1;
}

int gptpclock_init(uint8_t gptpInstanceIndex, int max_domains, int max_ports)
{
	int max_clocks = max_domains * max_ports;
	CB_THREAD_MUTEXATTR_T mattr;
	char *shmem_name, *shmem_namep;
	gptpclock_data_t *gcd;
	if(gptpInstanceIndex>=gcdl_num){
		gcdl=(gptpclock_data_t**)UB_SD_REGETMEM(GPTP_SMALL_ALLOC, gcdl,
				    sizeof(gptpclock_data_t*)*(gptpInstanceIndex+1u));
		if(ub_assert_fatal(gcdl, __func__, "realloc")){return -1;}
		(void)memset(&gcdl[gcdl_num], 0,
			     sizeof(gptpclock_data_t*)*(gptpInstanceIndex+1u-gcdl_num));
		gcdl_num=gptpInstanceIndex+1u;
	}else{
		if(gcdl[gptpInstanceIndex]!=NULL){return -1;}
	}
	gcd=(gptpclock_data_t*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(gptpclock_data_t));
	if(ub_assert_fatal(gcd!=NULL, __func__, NULL)){return -1;}
	(void)memset(gcd, 0, sizeof(gptpclock_data_t));
	gcdl[gptpInstanceIndex]=gcd;
	gcd->pdd=(per_domain_data_t*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC,
			      (unsigned int)max_domains*sizeof(per_domain_data_t));
	if(ub_assert_fatal(gcd->pdd, __func__, "malloc error")){return -1;}
	(void)memset(gcd->pdd, 0, (unsigned int)max_domains*sizeof(per_domain_data_t));
	gcd->active_domain_switch=-1; //default is automatic switch to a stable domain

	// clock data has pointer element, thus disallow realloc of container
	// set max elements and expansion units with the same values
	gcd->clds = ub_esarray_init(max_clocks, sizeof(oneclock_data_t), max_clocks);
	gcd->shmsize = ((int)sizeof(gptp_clock_ppara_t)*max_domains) +
		(int)sizeof(gptp_master_clock_shm_head_t);
	shmem_name=(char *)gptpgcfg_get_item(gptpInstanceIndex,
				     XL4_EXTMOD_XL4GPTP_MASTER_CLOCK_SHARED_MEM,
				     YDBI_CONFIG);
	if(shmem_name && shmem_name[0]){
		shmem_namep=shmem_name;
	}else{
		shmem_namep=GPTP_MASTER_CLOCK_SHARED_MEM;
	}
	gcd->shm=(gptp_master_clock_shm_t *)cb_get_shared_mem(
		&gcd->shmfd, shmem_namep, gcd->shmsize, O_CREAT | O_RDWR);
	if(shmem_name!=NULL){(void)gptpgcfg_get_item_release(gptpInstanceIndex);}
	if(!gcd->shm){
		gptpclock_close(gptpInstanceIndex);
		return -1;
	}
	(void)memset(gcd->shm, 0, gcd->shmsize);
	gcd->shm->head.max_domains = max_domains;
	UB_LOG(UBL_DEBUG, "%s:done, max_domains=%d, shmsize=%d\n",
	       __func__, max_domains, gcd->shmsize);
	CB_THREAD_MUTEXATTR_INIT(&mattr);
	CB_THREAD_MUTEXATTR_SETPSHARED(&mattr, CB_THREAD_PROCESS_SHARED);
	if(CB_THREAD_MUTEX_INIT(&gcd->shm->head.mcmutex, &mattr)){
		gptpclock_close(gptpInstanceIndex);
		return -1;
	}
	return 0;
}

void gptpclock_close(uint8_t gptpInstanceIndex)
{
	oneclock_data_t od;
	char *shmem_name, *shmem_namep;
	gptpclock_data_t *gcd;
	int i;
	if((gptpInstanceIndex>=gcdl_num) || !gcdl[gptpInstanceIndex]){return;}
	gcd=gcdl[gptpInstanceIndex];

	if(gcd->clds!=NULL){
		while(!ub_esarray_pop_ele(gcd->clds, (ub_esarray_element_t *)&od)){
			if(od.mode==PTPCLOCK_SLAVE_MAIN){
				// return HW adjustment rate to 0
				(void)gptp_clock_adjtime(od.ptpfd, 0);
			}
			if(PTPFD_VALID(od.ptpfd)){
				(void)gptp_close_ptpfd(od.ptpfd);
			}
		}
		ub_esarray_close(gcd->clds);
	}
	if(!gcd->shm){goto erexit;}
	gcd->shm->head.max_domains=0;
	CB_THREAD_MUTEX_DESTROY(&gcd->shm->head.mcmutex);
	shmem_name=(char *)gptpgcfg_get_item(
		gptpInstanceIndex,
		XL4_EXTMOD_XL4GPTP_MASTER_CLOCK_SHARED_MEM, YDBI_CONFIG);
	if(shmem_name && shmem_name[0]){
		shmem_namep=shmem_name;
	}else{
		shmem_namep=GPTP_MASTER_CLOCK_SHARED_MEM;
	}
	cb_close_shared_mem(gcd->shm, &gcd->shmfd, shmem_namep, gcd->shmsize, true);
	if(shmem_name!=NULL){(void)gptpgcfg_get_item_release(gptpInstanceIndex);}
erexit:
	if(gcd->pdd!=NULL){UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gcd->pdd);}
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gcd);
	gcdl[gptpInstanceIndex]=NULL;
	for(i=0;i<(int)gcdl_num;i++){
		if(gcdl[i]!=NULL){break;}
	}
	if(i==(int)gcdl_num){
		// all gcd in gcdl has been freed
		UB_SD_RELMEM(GPTP_SMALL_ALLOC, gcdl);
		gcdl=NULL;
		gcdl_num=0;
	}else{
		if(gptpInstanceIndex>=(gcdl_num-1u)){
			gcdl_num--;
			gcdl=(gptpclock_data_t**)UB_SD_REGETMEM(GPTP_SMALL_ALLOC, gcdl,
								sizeof(gptpgcfg_data_t*)*(gcdl_num));
		}
	}
	UB_LOG(UBL_DEBUG, "%s:closed\n", __func__);
}

int gptpclock_apply_offset(uint8_t gptpInstanceIndex,
			   int64_t *ts64, int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	*ts64+=od->offset64;
	return 0;
}

int64_t gptpclock_getts64(uint8_t gptpInstanceIndex,
			  int clockIndex, uint8_t domainIndex)
{
	int64_t ts64=-1;
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	(void)gptpclock_getts_od(&ts64, od);
	return ts64;
}

int gptpclock_active_domain(uint8_t gptpInstanceIndex)
{
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	return gcd->shm->head.active_domain;
}

int64_t gptpclock_gethwts64(uint8_t gptpInstanceIndex,
			    int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	int64_t ts64;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	GPTP_CLOCK_GETTIME(od->ptpfd, ts64);
	return ts64;
}

int gptpclock_setts64(uint8_t gptpInstanceIndex,
		      int64_t ts64, int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	return gptpclock_setts_od(gptpInstanceIndex, gcd, ts64, od);
}

int gptpclock_setoffset64(uint8_t gptpInstanceIndex,
			  int64_t ts64, int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);

	if(od->mode != PTPCLOCK_SLAVE_MAIN){
		if(!clockIndex){(void)gptpclock_mutex_trylock(&gcd->shm->head.mcmutex);}
		od->offset64=ts64;
		(void)gptpclock_setoffset_od(gcd, od);
		if(!clockIndex){CB_THREAD_MUTEX_UNLOCK(&gcd->shm->head.mcmutex);}
		return 0;
	}

	if(od->ts2diff>0){
		ts64 = (od->ts2diff/2) + ts64;
		if(time_setoffset64(gptpInstanceIndex, gcd, ts64,
				    clockIndex, domainIndex) > (od->ts2diff*10)){
			UB_LOG(UBL_WARN, "%s:clockIndex=%d, domainIndex=%d, "
			       "can't set in the time. the result must be inaccurate\n",
			       __func__, clockIndex, domainIndex);
			return -1;
		}
	}else{
		(void)time_setoffset64(gptpInstanceIndex, gcd, ts64,
				       clockIndex, domainIndex);
	}
	return 0;
}

int gptpclock_setadj(uint8_t gptpInstanceIndex,
		     int32_t adjvppb, int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	oneclock_data_t *od0;
	int64_t ts;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	if(gcd->pdd[od->domainIndex].thisClockIndex==clockIndex){
		gcd->pdd[od->domainIndex].thisClock_adjppb=adjvppb;
	}
	switch(od->mode){
	case PTPCLOCK_SLAVE_MAIN:
		if(gptp_clock_adjtime(od->ptpfd, adjvppb)<0){
			UB_LOG(UBL_ERROR, "%s:can't adjust freq, clockIndex=%d, domainIndex=%d\n",
			       __func__, clockIndex, domainIndex);
			return -1;
		}
		break;
	case PTPCLOCK_MASTER:
		UB_LOG(UBL_ERROR,"%s:MASTER can't adjust freq.\n",__func__);
		return -1;
	case PTPCLOCK_SLAVE_SUB:
		// to apply new adjrate, update offset value. it updates 'last_setts64'.
		if(od->ts2diff>0){
			ts=time_setoffset64(gptpInstanceIndex, gcd, od->ts2diff/2,
					    clockIndex, domainIndex);
			if(ts > (od->ts2diff*10)){
				UB_LOG(UBL_WARN,
				       "%s:clockIndex=%d, domainIndex=%d, time_setoffset64 "
				       "took too long, %"PRIi64"/%d\n",
				       __func__, clockIndex, domainIndex, ts, od->ts2diff);
			}
		}else{
			/* even though setoffset is 0, this function will adjust the clock phase */
			(void)gptpclock_getts_od(&ts, od);
			(void)gptpclock_setts_od(gptpInstanceIndex, gcd, ts, od);
		}
		od->adjrate = (double)adjvppb/1.0E9;
		od0=get_clockod(gcd, 0, domainIndex);
		// od0->pp->adjrate is in the shared memory
		// it is different from od0->adjrate,
		if(!od0){
			UB_LOG(UBL_ERROR, "%s:failed to get clock data, clockIndex=%d, domainIndex=%d\n",
			       __func__, clockIndex, domainIndex);
			return -1;
		}
		od0->pp->adjrate = od->adjrate;
		break;
	default:
		break;
	}
	od->adjvppb=adjvppb;
	if(gptpgcfg_set_yang_port_item(gptpInstanceIndex, IEEE1588_PTP_TT_PORT_CLOCK,
				       IEEE1588_PTP_TT_ADJRATE,
				       clockIndex, od->domainIndex, YDBI_STATUS,
				       &adjvppb, sizeof(int32_t), YDBI_NO_NOTICE)){
		UB_LOG(UBL_ERROR, "%s:can't update the DB, adjrate=%d ppb\n",
		       __func__, adjvppb);
	}
	return 0;
}

// this function is for debug purpose
void gptpclock_print_clkpara(uint8_t gptpInstanceIndex, ub_dbgmsg_level_t level)
{
	int i;
	gptp_clock_ppara_t *pp;
	oneclock_data_t *odt;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];

	if(!ub_clog_on(UB_LOGCAT, level)){return;}
	for(i=0;i<gcd->shm->head.max_domains;i++){
		pp=&gcd->shm->gcpp[i];
		odt=get_clockod(gcd, gcd->pdd[i].thisClockIndex, i);
		if(odt==NULL){
			UB_LOG(UBL_WARN, "domain=%d thisClockIndex=%d doesn't exists\n",
			       i,gcd->pdd[i].thisClockIndex);
			return;
		}
		UB_CONSOLE_PRINT("domain=%d, offset=%"PRIi64"nsec, ",
				 i, odt->offset64);
		switch(odt->mode){
		case PTPCLOCK_SLAVE_MAIN:
			UB_CONSOLE_PRINT("hw-adjrate=%dppb\n", odt->adjvppb);
			break;
		case PTPCLOCK_MASTER:
			UB_CONSOLE_PRINT("adjrate=0(master)\n");
			break;
		case PTPCLOCK_SLAVE_SUB:
			UB_CONSOLE_PRINT("sw-adjrate=%e\n", pp->adjrate);
			break;
		default:
			break;
		}
		UB_CONSOLE_PRINT("        gmsync=%s, last_setts64=%"PRIi64"nsec\n",
				 pp->gmsync?"true":"false", pp->last_setts64);
	}
}

int gptpclock_mode_master(uint8_t gptpInstanceIndex,
			  int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	if(!od) {
		UB_LOG(UBL_ERROR,"%s:failed to get clock data, clockIndex=%d, domainIndex=%d\n",
		       __func__, clockIndex, domainIndex);
		return -1;
	}
	od->mode=PTPCLOCK_MASTER;
	return 0;
}

int gptpclock_mode_slave_main(uint8_t gptpInstanceIndex,
			      int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od, *od1;
	int i;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);

	for(i=0;i<ub_esarray_ele_nums(gcd->clds);i++){
		od1 = (oneclock_data_t *)ub_esarray_get_ele(gcd->clds, i);
		if(!od1 || (od1->clockIndex != od->clockIndex) ||
		   (od1->pp->domainIndex == od->pp->domainIndex)){continue;}
		if(od1->mode==PTPCLOCK_SLAVE_MAIN){
			UB_LOG(UBL_INFO, "%s:domainIndex=%d already in SLAVE_MAIN\n",
			       __func__, od1->pp->domainIndex);
			return -1;
		}
	}
	od->mode=PTPCLOCK_SLAVE_MAIN;
	od->adjrate=0.0;
	// if SLAVE_MAIN is used, SLAVE_SUB is not used in the same domain.
	// When SLAVE_SUB is not used, adjrate in the shared mem. must be 0.0,
	// and offset64 is not needed to combine with the one of 'thisClock'
	GPTPCLOCK_FN_ENTRY(gcd, od, 0, domainIndex);
	od->pp->adjrate=0.0;
	od->pp->offset64=od->offset64;
	return 0;
}

int gptpclock_mode_slave_sub(uint8_t gptpInstanceIndex,
			     int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	od->mode=PTPCLOCK_SLAVE_SUB;
	return 0;
}

int gptpclock_tsconv(uint8_t gptpInstanceIndex,
		     int64_t *ts64, int clockIndex, uint8_t domainIndex,
		     int clockIndex1, uint8_t domainIndex1)
{
	oneclock_data_t *od, *od1;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];

	if((clockIndex==clockIndex1) && (domainIndex==domainIndex1)) {return 0;}
	GPTPCLOCK_FN_ENTRY(gcd, od1, clockIndex1, domainIndex1);
	if((clockIndex==clockIndex1) && (od1->mode!=PTPCLOCK_SLAVE_SUB)) {return 0;}
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, domainIndex);
	return GPTPCLOCK_CONVERT(ts64, od, od1);
}

uint8_t *gptpclock_clockid(uint8_t gptpInstanceIndex,
			   int clockIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	if(gcd==NULL){
		UB_LOG(UBL_ERROR, "%s:no gcdl[%d]\n", __func__, gptpInstanceIndex);
		return NULL;
	}
	if(!gcd->clds){return NULL;}
	od=get_clockod(gcd, clockIndex, domainIndex);
	if(od==NULL){return NULL;}
	return od->clockId;
}

int gptpclock_rate_same(uint8_t gptpInstanceIndex,
			int clockIndex, uint8_t domainIndex,
			int clockIndex1, uint8_t domainIndex1)
{
	oneclock_data_t *od, *od1;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	if(gcd==NULL){
		UB_LOG(UBL_ERROR, "%s:no gcdl[%d]\n", __func__, gptpInstanceIndex);
		return -1;
	}
	if(!gcd->clds){return -1;}
	od=get_clockod(gcd, clockIndex, domainIndex);
	if(od==NULL){return -1;}
	od1=get_clockod(gcd, clockIndex1, domainIndex1);
	if(od1==NULL){return -1;}
	if(!strcmp(od->pp->ptpdev, od1->pp->ptpdev)){
		if((od->mode != PTPCLOCK_SLAVE_SUB) && (od1->mode != PTPCLOCK_SLAVE_SUB)){return 0;}
		if((od->adjrate == 0.0) && (od1->adjrate == 0.0)){return 0;}
		if((od->adjrate == 0.0) && (od1->mode != PTPCLOCK_SLAVE_SUB)){return 0;}
		if((od1->adjrate == 0.0) && (od->mode != PTPCLOCK_SLAVE_SUB)){return 0;}
	}
	return 1;
}

static int switch_active_domain(uint8_t gptpInstanceIndex, int di)
{
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	if(gcd->shm->head.active_domain==di){return 1;}
	UB_TLOG(UBL_INFO, "active domain switched from %d to %d\n",
		 gcd->shm->head.active_domain, di);
	gcd->shm->head.active_domain=di;
	return 0;
}

static int gptpclock_update_active_domain(uint8_t gptpInstanceIndex)
{
	int i;
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];

	UB_LOG(UBL_DEBUGV, "%s:current active domain=%d\n",__func__,gcd->shm->head.active_domain);
	if(gcd->active_domain_switch>=0){
		return switch_active_domain(gptpInstanceIndex, gcd->active_domain_switch);
	}

	if(gptpgcfg_get_intitem(
		   gptpInstanceIndex,
		   XL4_EXTMOD_XL4GPTP_ACTIVE_DOMAIN_AUTO_SWITCH,
		   YDBI_CONFIG)==0){return 0;}

	GPTPCLOCK_FN_ENTRY(gcd, od, 0, gcd->shm->head.active_domain);

	if(od->pp->gmsync==GMSYNC_SYNC_STABLE &&
	   gptpgcfg_get_intitem(
		   gptpInstanceIndex,
		   XL4_EXTMOD_XL4GPTP_ACTIVE_DOMAIN_AUTO_SWITCH,
		   YDBI_CONFIG)==1){
		UB_LOG(UBL_DEBUG, "%s:current active domain=%d is stable, don't switch\n",
		       __func__,gcd->shm->head.active_domain);
		return 0;
	}

	for(i=0;i<gcd->shm->head.max_domains;i++){
		GPTPCLOCK_FN_ENTRY(gcd, od, 0, i);
		if(od->pp->gmsync==GMSYNC_SYNC_STABLE){
			return switch_active_domain(gptpInstanceIndex, i);
		}
	}

	UB_LOG(UBL_DEBUG, "%s:no stable GM in all domains, current active domain=%d\n",
	       __func__,gcd->shm->head.active_domain);
	return -1;
}

static int adjust_GM_btw_domains(uint8_t gptpInstanceIndex, int domainIndex)
{
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	UB_TLOG(UBL_INFO, "%s:domainIndex=%d\n",__func__, domainIndex);
	oneclock_data_t *od, *od1;
	// this is called after gm_stable of D0 becomes true
	GPTPCLOCK_FN_ENTRY(gcd, od, 0, 0);
	GPTPCLOCK_FN_ENTRY(gcd, od1, 0, domainIndex);

	if(gcd->pdd[0].thisClockIndex != gcd->pdd[od1->domainIndex].thisClockIndex){
		int64_t ts64=-1;
		(void)gptpclock_getts_od(&ts64, od);
		(void)gptpclock_setts_od(gptpInstanceIndex, gcd, ts64, od1);
		return 0;
	}
	// 'thisClock of D0' and 'thisClock of Di' is based on the same clock.
	od1->adjrate=0.0; // GM Freq. sync to Domain0
	od1->pp->adjrate=0.0;
	od1->offset64=od->offset64;
	od1->pp->offset64=od->pp->offset64;
	return 0;
}

int gptpclock_active_domain_switch(uint8_t gptpInstanceIndex, int domainIndex)
{
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	//domainIndex=-1:auto, domainIndex>=0:fix to the domain
	gcd->active_domain_switch=domainIndex;
	if((domainIndex<0) || (domainIndex>=gcd->shm->head.max_domains)){return 0;}
	return switch_active_domain(gptpInstanceIndex, gcd->active_domain_switch);
}

int gptpclock_active_domain_status(uint8_t gptpInstanceIndex)
{
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	return gcd->shm->head.active_domain;
}

bool gptpclock_we_are_gm(uint8_t gptpInstanceIndex, int domainIndex)
{
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	return gcd->pdd[domainIndex].we_are_gm;
}

int gptpclock_set_gmchange(uint8_t gptpInstanceIndex, uint8_t domainIndex,
			   ClockIdentity gmIdentity, bool becomeGM)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, 0, domainIndex);
	if(!memcmp(gcd->pdd[od->domainIndex].gmClockId, gmIdentity, sizeof(ClockIdentity))){
		// no update
		return 0;
	}
	od->pp->gmchange_ind++;
	memcpy(gcd->pdd[od->domainIndex].gmClockId, gmIdentity, sizeof(ClockIdentity));
	UB_TLOG(UBL_DEBUG, "%s:gptpInstance=%d, domainIndex=%d, becomeGM=%d, newgm="
		UB_PRIhexB8"\n", __func__,
		gptpInstanceIndex, domainIndex, becomeGM, UB_ARRAY_B8(gmIdentity));
	if(becomeGM){
		gcd->pdd[od->domainIndex].we_are_gm=true;
		if(domainIndex!=0u){
			(void)adjust_GM_btw_domains(gptpInstanceIndex, domainIndex);
		}
		if(gptpgcfg_get_intitem(
			   gptpInstanceIndex,
			   XL4_EXTMOD_XL4GPTP_RESET_FREQADJ_BECOMEGM, YDBI_CONFIG)){
			(void)gptpclock_setadj(gptpInstanceIndex, 0,
					       gcd->pdd[od->domainIndex].thisClockIndex,
					       domainIndex);
		}
	}else{
		gcd->pdd[od->domainIndex].we_are_gm=false;
	}
	return 0;
}

int gptpclock_set_gmsync(uint8_t gptpInstanceIndex, uint8_t domainIndex, uint32_t gmstate)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, 0, domainIndex); // gmsync is only on clockIndex=0
	if(od->pp->gmsync==gmstate){
		// no change
		return 0;
	}
	UB_LOG(UBL_INFO, "%s:gptpInstanceIndex=%d, domainIndex=%d, gmstate=%d\n", __func__,
	       gptpInstanceIndex, domainIndex, gmstate);
	od->pp->gmsync=gmstate;
	(void)gptpclock_update_active_domain(gptpInstanceIndex);
	gptpgcfg_set_clock_state_item(gptpInstanceIndex, IEEE1588_PTP_TT_GMSTATE,
				      od->domainIndex, YDBI_STATUS,
				      &gmstate, sizeof(uint32_t), YDBI_NO_NOTICE);
	return 0;
}

static int get_gmsync(uint8_t gptpInstanceIndex, uint8_t domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, 0, domainIndex); // gmsync is only on clockIndex=0
	return od->pp->gmsync;
}

gmsync_status_t gptpclock_get_gmsync(uint8_t gptpInstanceIndex, uint8_t domainIndex)
{
	int status=get_gmsync(gptpInstanceIndex, domainIndex);
	if(status<0){return GMSYNC_UNSYNC;}
	return (gmsync_status_t)status;
}

int gptpclock_get_gmchange_ind(uint8_t gptpInstanceIndex, int domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, 0, domainIndex);
	return od->pp->gmchange_ind;
}

int gptpclock_set_thisClock(uint8_t gptpInstanceIndex,
			    int clockIndex, uint8_t domainIndex, bool set_clock_para)
{
	oneclock_data_t *od, *mod;
	double adjrate;
	int64_t ts64;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	if(gcd==NULL){
		UB_LOG(UBL_ERROR, "%s:no gcdl[%d]\n", __func__, gptpInstanceIndex);
		return -1;
	}
	if(clockIndex==0){
		UB_LOG(UBL_ERROR,"%s:clockIndex=0 can't be thisClock\n", __func__);
		return -1;
	}
	if(!gcd->clds){return -1;}
	od=get_clockod(gcd, clockIndex, domainIndex);
	if(od==NULL){return -1;}
	mod=get_clockod(gcd, 0, domainIndex);
	if(mod==NULL){return -1;}
	if(strcmp(od->pp->ptpdev, mod->pp->ptpdev)!=0){
		UB_LOG(UBL_ERROR,
		       "%s:master clock and thisClock must be based on the same ptp clock\n",
			__func__);
		return -1;
	}

	gcd->pdd[od->domainIndex].thisClockIndex=clockIndex;
	// make sure the master clock(clockIndex=0) is PTPCLOCK_MASTER
	mod->mode=PTPCLOCK_MASTER;

	/* During the offset and adjrate are moved into thisClock from the master clock,
	   the master clock can't be read. So it must be locked by mutex.
	   clockIndex is never '0' in this section, and gcd->shm->head.mcmutex is
	   locked only when clockIndex==0 in other parts.
	   Make sure to keep the condition not to have deadlock.
	 */
	(void)gptpclock_mutex_trylock(&gcd->shm->head.mcmutex);
	adjrate=od->adjrate;
	if((od->state == PTPCLOCK_RDWR) &&
	   !gptpclock_mode_slave_main(gptpInstanceIndex, clockIndex, domainIndex)){
		//PTPCLOCK_SLAVE_MAIN
		if(!set_clock_para){goto mutexout;}
		/* when thisClock was SLAVE_SUB before, it may have SW offset and adjrate
		   move them to HW offset and adjrate */
		if(od->offset64!=0){
			(void)gptpclock_setoffset64(gptpInstanceIndex,
					      od->offset64, clockIndex, domainIndex);
		}
		if(adjrate!=0.0){
			(void)gptpclock_setadj(gptpInstanceIndex,
					       adjrate*(double)UB_SEC_NS, clockIndex, domainIndex);
		}

		/* move the offset in the master clock to thisClock */
		(void)gptpclock_setoffset64(gptpInstanceIndex,
				      mod->offset64, clockIndex, domainIndex);
		UB_LOG(UBL_INFO, "%s:thisClock is clockIndex=%d, SLAVE_MAIN\n",
		       __func__, clockIndex);
	}else{
		//PTPCLOCK_SLAVE_SUB
		(void)gptpclock_mode_slave_sub(gptpInstanceIndex, clockIndex, domainIndex);

		if(!set_clock_para){goto mutexout;}
		ts64 = od->offset64 + mod->offset64;
		od->offset64=ts64;
		UB_LOG(UBL_INFO, "%s:thisClock is clockIndex=%d, SLAVE_SUB\n",
		       __func__, clockIndex);
	}
	/* when set_clock_para==true, offset in the master clock has been moved to thisClock,
	   and it needs to be cleared */
	mod->offset64=0;
	mod->pp->offset64=mod->offset64+od->offset64;
mutexout:
	CB_THREAD_MUTEX_UNLOCK(&gcd->shm->head.mcmutex);
	return 0;
}

int64_t gptpclock_d0ClockfromRT(uint8_t gptpInstanceIndex, int clockIndex)
{
	oneclock_data_t *od;
	int64_t ts1=-1, ts2=-1, ts3=-1;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	GPTPCLOCK_FN_ENTRY(gcd, od, clockIndex, 0);
	if(gptpclock_getts_od(&ts1, od)!=0){
		UB_LOG(UBL_ERROR, "%s:can't get ts1=TS(clk=%d,D=%d)\n",
		       __func__, od->clockIndex, od->pp->domainIndex);
		return 0;
	}
	ts2=ub_rt_gettime64();
	if(gptpclock_getts_od(&ts3, od)!=0){
		UB_LOG(UBL_ERROR, "%s:can't get ts3=TS(clk=%d,D=%d)\n",
		       __func__, od->clockIndex, od->pp->domainIndex);
		return 0;
	}
	ts3=(ts3-ts1)/2;
	if((od->ts2diff>0) && (ts3 > (od->ts2diff*10))) {
		UB_LOG(UBL_WARN,"%s:gap of 2 ts is too big:clockIndex=%d, domainIndex=%d, %"
		       PRIi64"\n",__func__, od->clockIndex, od->pp->domainIndex, ts3);
	}
	UB_LOG(UBL_DEBUGV,"%s:%d, %d, %"PRIi64"\n",
	       __func__, od->clockIndex, od->pp->domainIndex, ts1-ts2+ts3);
	return ts1-ts2+ts3;
}

int gptpclock_get_adjppb(uint8_t gptpInstanceIndex, int clockIndex, int domainIndex)
{
	oneclock_data_t *od;
	gptpclock_data_t *gcd=gcdl[gptpInstanceIndex];
	od=get_clockod(gcd, clockIndex, domainIndex);
	if(od==NULL){return 0;}
	if(clockIndex==0){
		return gcd->pdd[od->domainIndex].thisClock_adjppb;
	}
	return od->adjvppb;
}
