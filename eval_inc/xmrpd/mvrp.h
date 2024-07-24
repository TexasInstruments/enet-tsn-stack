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
#ifndef __MVRP_H_
#define __MVRP_H_

#include "mrp_datatypes.h"
#include "mrp_map.h"

typedef struct mvrp_data mvrp_data_t;

int mvrp_proc(mvrp_data_t *mvrpd, uint64_t ts, int periodic,
	      uint32_t mrp_events, int portofevent);
mvrp_data_t *mvrp_init(mrpnet_data_t *mrpnd, int portIndex, mrp_map_data_t *mapd);
void mvrp_close(mvrp_data_t *mvrpd);
void mvrp_add_mad_action(mvrp_data_t *mvrpd, mrp_mad_actions_t madaction,
			 attrstate_t *astate, uint64_t ts);
mvrp_firstvalue_data_t *mvrp_parse_firstvalue_data(void *fv, int fvlen, uint8_t atype);
mvrp_firstvalue_data_t *mvrp_get_first_value(attrstate_t *astate);
void mvrp_print_firstvalue(bool console, bool debug,
			   mvrp_firstvalue_data_t *fdata, uint8_t atype, int indent);
mvrp_firstvalue_data_t *mvrp_pack_firstvalue_data(mvrp_firstvalue_data_t *fvd, int fvlen,
						  uint8_t atype);
attrstate_t *mvrp_find_attr(mvrp_data_t *mvrpd, attrstate_t *astate, int cmpstart,
			    int cmpsize, int decreg);

void release_mvrp_fvd(mvrp_firstvalue_data_t *fvd);
#endif
