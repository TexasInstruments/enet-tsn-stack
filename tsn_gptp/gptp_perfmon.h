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
#ifndef GPTP_PERFMON_H_
#define GPTP_PERFMON_H_

#include <tsn_unibase/unibase.h>
#include "mind.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpcommon.h"
#include "gptpnet.h"

#define SHORTINTV_START_IDX 0
#define SHORTINTV_END_IDX 96
#define LONGINTV_START_IDX 97
#define LONGINTV_END_IDX 98

/* PerfmanceMonitoringDataRecord */

typedef enum {
	PPMPDDR_meanLinkDelay,
} PPMPDDR_params_t;

void gptp_port_perfmon(PerfMonPortDS *ds, int domainIndex,
					   int portIndex, uint64_t cts64,
					   PerTimeAwareSystemGlobal *tasglb);
void gptp_port_perfmon_dr_reset(PerfMonPortDS *ds, uint8_t type,
		uint8_t di, uint8_t pi, uint64_t cts);
void gptp_port_perfmon_dr_dump(PerfMonPortDS *ds, ub_dbgmsg_level_t lv,
		uint8_t id, uint8_t di, uint8_t pi);
void gptp_port_perfmon_ppmpddr_add(PerfMonPortDS *ds, PPMPDDR_params_t p, int64_t v);

#define PERFMON_PPMPDDR_INC(ds, m) {\
	if(ds){ \
		(ds)->pdelayDR[PERFMON_SHORTINTV_DR].m++; \
		(ds)->pdelayDR[PERFMON_LONGINTV_DR].m++; \
	} \
}

#define PERFMON_PPMDR_INC(ds, m) {\
	if(ds){ \
		(ds)->portDR[PERFMON_SHORTINTV_DR].m++; \
		(ds)->portDR[PERFMON_LONGINTV_DR].m++; \
	} \
}

#define PERFMON_PPMSDR_INC(ds, m) {\
	if(ds){ \
		(ds)->signalingDR[PERFMON_SHORTINTV_DR].m++; \
		(ds)->signalingDR[PERFMON_LONGINTV_DR].m++; \
	} \
}

#define PERFMON_PPMPDDR_RST(ds, m) {\
	if(ds){ \
		(ds)->pdelayDR[PERFMON_SHORTINTV_DR].m=0; \
		(ds)->pdelayDR[PERFMON_LONGINTV_DR].m=0; \
	} \
}

#define PERFMON_PPMDR_RST(ds, m) {\
	if(ds){ \
		(ds)->portDR[PERFMON_SHORTINTV_DR].m=0; \
		(ds)->portDR[PERFMON_LONGINTV_DR].m=0; \
	} \
}

#define PERFMON_PPMSDR_RST(ds, m) {\
	if(ds){ \
		(ds)->signalingDR[PERFMON_SHORTINTV_DR].m=0; \
		(ds)->signalingDR[PERFMON_LONGINTV_DR].m=0; \
	} \
}

#define PERFMON_PPMPDDR_ADD(ds, p, v) {\
	gptp_port_perfmon_ppmpddr_add(ds, p, v); \
}


/* ClockPerfmanceMonitoringDataRecord */

typedef enum {
	CPMDR_masterSlaveDelay,
	CPMDR_slaveMasterDelay,
	CPMDR_meanPathDelay,
	CPMDR_offsetFromMaster,
} CPMDR_params_t;

void gptp_clock_perfmon(PerfMonClockDS *ds, uint64_t cts64, PerTimeAwareSystemGlobal *tasglb, uint8_t di);
void gptp_clock_perfmon_dr_reset(PerfMonClockDS *ds, uint8_t type, uint64_t cts);
void gptp_clock_perfmon_dr_dump(PerfMonClockDS *ds, ub_dbgmsg_level_t lv, uint8_t id);
void gptp_clock_perfmon_cpmdr_add(PerfMonClockDS *ds, CPMDR_params_t p, int64_t v);

#define PERFMON_CPMDR_ADD(ds, p, v) {\
	gptp_clock_perfmon_cpmdr_add(ds, p, v); \
}

#endif // GPTP_PERFMON_H_
