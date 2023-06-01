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
#ifndef __GPTPCOMMON_H_
#define __GPTPCOMMON_H_
#include "gptpbasetypes.h"
#include "gptp_config.h"

void eui48to64(const uint8_t *eui48, uint8_t *eui64, const uint8_t *insert);

#define LOG_TO_NSEC(x) (((x)>=0)?UB_SEC_NS*(1<<(x)):UB_SEC_NS/(1<<-(x)))

#define GET_FLAG_BIT(x,b) ((x) & (1<<(b)))
#define SET_FLAG_BIT(x,b) x |= (1<<(b))
#define RESET_FLAG_BIT(x,b) x &= ~(1<<(b))
#define SET_RESET_FLAG_BIT(c,x,b) if(c) SET_FLAG_BIT(x,b); else RESET_FLAG_BIT(x,b)

#define SM_CLOSE(f,x) if(x) f(&x)

/* allocate typed in *sm, then allocate typesm in (*sm)->thisSM */
#define INIT_SM_DATA(typed, typesm, sm) \
	({if(!*sm){ \
			*sm=(typed *)malloc(sizeof(typed)); \
		ub_assert_fatal(*sm!=NULL, __func__, "malloc1"); \
	} \
	if(*sm!=NULL){ \
		memset(*sm, 0, sizeof(typed)); \
		(*sm)->thisSM=(typesm *)malloc(sizeof(typesm));	\
		if(!ub_assert_fatal((*sm)->thisSM!=NULL, __func__, "malloc2")){ \
			memset((*sm)->thisSM, 0, sizeof(typesm)); \
		} \
	} \
	ub_fatalerror();})

/* free the above allocations */
#define CLOSE_SM_DATA(sm) \
	if(!*sm) return 0; \
	free((*sm)->thisSM); \
	free(*sm); \
	*sm=NULL;

/* check for portPriority vector, any non-zero value is sufficient */
#define HAS_PORT_PRIORITY(pp) \
        (((pp).rootSystemIdentity.priority1 !=0) || \
         ((pp).rootSystemIdentity.clockClass != 0) || \
         ((pp).rootSystemIdentity.clockAccuracy != 0) || \
         ((pp).rootSystemIdentity.offsetScaledLogVariance != 0) || \
         ((pp).rootSystemIdentity.priority2 != 0) || \
         ((pp).stepsRemoved != 0))

typedef enum {
        SAME_PRIORITY,
        SUPERIOR_PRIORITY,
        INFERIOR_PRIORITY,
} bmcs_priority_comparison_result;

void print_priority_vector(ub_dbgmsg_level_t level, const char *identifier, UInteger224 *priorityVector);
uint8_t compare_priority_vectors(UInteger224 *priorityA, UInteger224 *priorityB);

#endif
