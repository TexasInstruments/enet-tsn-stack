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
 * cb_tmevent.c
 * timer and event functions
 *
 */

#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <tsn_unibase/ub_getmem.h>
#include "combase_private.h"

struct cb_timer_object{
	timer_t timerid;
	char name[16];
	int timer_expire;
};

/*
 * static memory allocation case('UB_SD_STATIC' is defined)
 * the memory is statically allocated by the following definitions.
 * the numbers defined here should be overridden by your needs.
 * when 'UB_SD_STATIC' is not defined, the number doesn't have meaning.
 */
#define CB_TMEVENT_TMEM cb_tmevent_tm
#ifndef CB_TMEVENT_TNUM
#define CB_TMEVENT_TNUM 2
#endif

UB_SD_GETMEM_DEF(CB_TMEVENT_TMEM, (int)sizeof(struct cb_timer_object),
		 CB_TMEVENT_TNUM);

static int alarm_signal;

static void catch_alarm(int sig, siginfo_t *si, void *uc)
{
	cb_timer_object_t *mtmo;
	mtmo=(cb_timer_object_t *)si->si_value.sival_ptr;
	if(mtmo->timer_expire>=0){mtmo->timer_expire++;}
	UB_LOG(UBL_DEBUG,"timer:%s expired\n",mtmo->name);
}

int cb_timer_init(int signal)
{
	struct sigaction sa;
	alarm_signal=signal?signal:SIGALRM;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = catch_alarm;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, alarm_signal);
	if(sigaction(alarm_signal, &sa, NULL) == -1){
		UB_LOG(UBL_ERROR,"%s:sigaction failed, %s\n",__func__, strerror(errno));
		return -1;
	}
	return 0;
}

cb_timer_object_t *cb_timer_create(char *tname)
{
	cb_timer_object_t *mtmo;
	struct sigevent sev;

	if(!alarm_signal){(void)cb_timer_init(0);}
	mtmo=UB_SD_GETMEM(CB_TMEVENT_TMEM, sizeof(cb_timer_object_t));
	if(ub_assert_fatal(mtmo!=NULL, __func__, NULL)){return NULL;}
	(void)memset(mtmo, 0, sizeof(cb_timer_object_t));
	if(tname!=NULL){(void)snprintf(mtmo->name, sizeof(mtmo->name), "%s", tname);}
	sev.sigev_notify=SIGEV_SIGNAL;
	sev.sigev_signo=alarm_signal;
	sev.sigev_value.sival_ptr=mtmo;
	if(timer_create(CLOCK_REALTIME, &sev, &mtmo->timerid)!=0){
		UB_LOG(UBL_ERROR,"%s:%s\n", __func__, strerror(errno));
		UB_SD_RELMEM(CB_TMEVENT_TMEM, mtmo);
		return NULL;
	}
	return mtmo;
}

void cb_timer_close(cb_timer_object_t *mtmo)
{
	timer_delete(mtmo->timerid);
	UB_SD_RELMEM(CB_TMEVENT_TMEM, mtmo);
	return;
}

int cb_timer_start_interval(cb_timer_object_t *mtmo,
			      uint32_t value_ms, uint32_t interval_ms)
{
	struct itimerspec tmts;

	tmts.it_interval.tv_sec=interval_ms/1000u;
	tmts.it_interval.tv_nsec=(interval_ms%1000u)*1000000u;
	tmts.it_value.tv_sec=value_ms/1000u;
	tmts.it_value.tv_nsec=(value_ms%1000u)*1000000u;
	mtmo->timer_expire=0;
	return timer_settime(mtmo->timerid, 0, &tmts, NULL);
}

int cb_timer_stop(cb_timer_object_t *mtmo)
{
	struct itimerspec tmts;

	(void)memset(&tmts, 0, sizeof(tmts));
	mtmo->timer_expire=-1;
	return timer_settime(mtmo->timerid, 0, &tmts, NULL);
}

bool cb_timer_expired(cb_timer_object_t *mtmo, cb_timer_clear_t clear)
{
	bool res=false;
	res=mtmo->timer_expire>0;
	if(mtmo->timer_expire>0){
		switch(clear){
		case CB_TIMER_NO_CLEAR:
			break;
		case CB_TIMER_DECREMENT:
			mtmo->timer_expire--;
			break;
		case CB_TIMER_ALL_CLEAR:
			mtmo->timer_expire=0;
			break;
		default:
			break;
		}
	}
	return res;
}
