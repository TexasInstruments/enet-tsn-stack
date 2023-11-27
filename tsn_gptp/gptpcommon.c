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
#include <stdio.h>
#include "gptpcommon.h"

void eui48to64(const uint8_t *eui48, uint8_t *eui64, const uint8_t *insert)
{
	eui64[0]=eui48[0];
	eui64[1]=eui48[1];
	eui64[2]=eui48[2];
	if(!insert){
		eui64[3]=0xff;
		eui64[4]=0xfe;
	}else{
		eui64[3]=insert[0];
		eui64[4]=insert[1];
	}
	eui64[5]=eui48[3];
	eui64[6]=eui48[4];
	eui64[7]=eui48[5];
}

void print_priority_vector(ub_dbgmsg_level_t level, const char *identifier,
			   UInteger224 *priorityVector)
{
	UB_VLOG(level, "%s:%s P1=%d CC=%d CA=%d LV=%d P2=%d CI=%02X%02X%02X%02X%02X%02X%02X%02X"
	       " SR=%d SI=%02X%02X%02X%02X%02X%02X%02X%02X\n",
               __func__, identifier,
               priorityVector->rootSystemIdentity.priority1,
               priorityVector->rootSystemIdentity.clockClass,
               priorityVector->rootSystemIdentity.clockAccuracy,
               priorityVector->rootSystemIdentity.offsetScaledLogVariance,
               priorityVector->rootSystemIdentity.priority2,
               priorityVector->rootSystemIdentity.clockIdentity[0],
               priorityVector->rootSystemIdentity.clockIdentity[1],
               priorityVector->rootSystemIdentity.clockIdentity[2],
               priorityVector->rootSystemIdentity.clockIdentity[3],
               priorityVector->rootSystemIdentity.clockIdentity[4],
               priorityVector->rootSystemIdentity.clockIdentity[5],
               priorityVector->rootSystemIdentity.clockIdentity[6],
               priorityVector->rootSystemIdentity.clockIdentity[7],
               priorityVector->stepsRemoved,
               priorityVector->sourcePortIdentity.clockIdentity[0],
               priorityVector->sourcePortIdentity.clockIdentity[1],
               priorityVector->sourcePortIdentity.clockIdentity[2],
               priorityVector->sourcePortIdentity.clockIdentity[3],
               priorityVector->sourcePortIdentity.clockIdentity[4],
               priorityVector->sourcePortIdentity.clockIdentity[5],
               priorityVector->sourcePortIdentity.clockIdentity[6],
               priorityVector->sourcePortIdentity.clockIdentity[7]
        );
}

uint8_t compare_priority_vectors(UInteger224 *priorityA, UInteger224 *priorityB)
{
        /* 10.3.5 priority vector comparison */
        uint8_t result = SAME_PRIORITY;

        /* 10.3.4 For all components, a lsser numerical value is better,
           and earlier components in the list are more significant */
        int compare = memcmp(priorityA, priorityB,sizeof(UInteger224));
        if (compare < 0){
                result = SUPERIOR_PRIORITY;
        }
        else if (compare > 0){
                result = INFERIOR_PRIORITY;
        }else{}
        return result;
}
