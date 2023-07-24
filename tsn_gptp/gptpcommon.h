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
#ifndef GPTPCOMMON_H_
#define GPTPCOMMON_H_
#include "gptpbasetypes.h"
#include "gptpconf/gptpgcfg.h"

void eui48to64(const uint8_t *eui48, uint8_t *eui64, const uint8_t *insert);

#define LOG_TO_NSEC(x) (((x)>=0)?(uint64_t)UB_SEC_NS*(1u<<(uint8_t)(x)):(uint64_t)UB_SEC_NS/(1u<<(uint8_t)(-(x))))

#define GET_FLAG_BIT(x,b) ((x) & (1u<<(b)))
#define SET_FLAG_BIT(x,b) (x) |= (1u<<(b))
#define RESET_FLAG_BIT(x,b) (x) &= ~(1u<<(b))
#define SET_RESET_FLAG_BIT(c,x,b) if(c){SET_FLAG_BIT((x),(b));}else{RESET_FLAG_BIT((x),(b));}

#define SM_CLOSE(f,x) if((x)!=NULL){(f)(&(x));}

#ifndef GPTP_MAX_INSTANCES
#define GPTP_MAX_INSTANCES 1u
#endif

#define SM_DATA_INST sm_data_INST
#define SM_DATA_FSIZE (sizeof(void*)+12u)
#ifndef SM_DATA_FNUM
// there are 25 state machines and each uses 2 UD_SD_MALLOC, 3 state machines with 1 UD_SD_MALLOC
// With SM_DATA_FSIZE above, 9 fragments each can work for a single domain and a single port.
// Increasing ports/domain, 7 fragments each port, 12 fragments for domain/2ports
// (25*2+3)*(9+7*[number of extra ports - 1]+12*[number of domains - 1])
// The default is set here is for 2 ports and 2 domains
#define SM_DATA_FNUM (GPTP_MAX_INSTANCES*(((25u*2u)+3u)*(9u+7u+12u)))
#endif
UB_SD_GETMEM_DEF_EXTERN(SM_DATA_INST);

#define GPTP_SMALL_ALLOC gptp_small_alloc
#define GPTP_SMALL_AFSIZE (sizeof(void*))
#ifndef GPTP_SMALL_AFNUM
// With GPTP_SMALL_AFAIZE above, 80 fragments can work for a single domain and a single port.
// Increasing ports/domain, 12 fragments each port, 12 fragments for domain/2ports
// The default is set here is for 2 ports and 2 domains
#define GPTP_SMALL_AFNUM (GPTP_MAX_INSTANCES*(80u+12u+12u))
#endif
UB_SD_GETMEM_DEF_EXTERN(GPTP_SMALL_ALLOC);

#define GPTP_MEDIUM_ALLOC gptp_medium_alloc
#define GPTP_MEDIUM_AFSIZE 64u
#ifndef GPTP_MEDIUM_AFNUM
// With GPTP_MEDIUM_AFSIZE above, 135 fragments can work for a single domain and a single port.
// Increasing ports/domain, 65 fragments each port, 125 fragment for domain/2ports
// The default is set here is for 2 ports and 2 domains
#define GPTP_MEDIUM_AFNUM (GPTP_MAX_INSTANCES*(135u+65u+125u))
#endif
UB_SD_GETMEM_DEF_EXTERN(GPTP_MEDIUM_ALLOC);

/* allocate typed in *sm, then allocate typesm in (*sm)->thisSM */
#define INIT_SM_DATA(typed, typesm, sm)					\
{									\
	if(!*sm){							\
		*sm=UB_SD_GETMEM(SM_DATA_INST, sizeof(typed));		\
		(void)ub_assert_fatal(*sm!=NULL, __func__, "malloc1");	\
	}								\
	if(*sm!=NULL){							\
		(void)memset(*sm, 0, sizeof(typed));			\
		(*sm)->thisSM=UB_SD_GETMEM(SM_DATA_INST, sizeof(typesm));\
		if(!ub_assert_fatal((*sm)->thisSM!=NULL, __func__, "malloc2")){ \
			(void)memset((*sm)->thisSM, 0, sizeof(typesm));	\
		}							\
	}								\
}


/* free the above allocations */
#define CLOSE_SM_DATA(sm) \
	if(!*(sm)){return 0;}				\
	UB_SD_RELMEM(SM_DATA_INST, (*(sm))->thisSM);	\
	UB_SD_RELMEM(SM_DATA_INST, *(sm));		\
	*(sm)=NULL;

/* check for portPriority vector, any non-zero value is sufficient */
#define HAS_PORT_PRIORITY(pp) \
        (((pp).rootSystemIdentity.priority1 !=0u) || \
         ((pp).rootSystemIdentity.clockClass != 0u) || \
         ((pp).rootSystemIdentity.clockAccuracy != 0u) || \
         ((pp).rootSystemIdentity.offsetScaledLogVariance != 0u) || \
         ((pp).rootSystemIdentity.priority2 != 0u) || \
         ((pp).stepsRemoved != 0u))

typedef enum {
        SAME_PRIORITY,
        SUPERIOR_PRIORITY,
        INFERIOR_PRIORITY,
} bmcs_priority_comparison_result;

void print_priority_vector(ub_dbgmsg_level_t level, const char *identifier, UInteger224 *priorityVector);
uint8_t compare_priority_vectors(UInteger224 *priorityA, UInteger224 *priorityB);

#endif
