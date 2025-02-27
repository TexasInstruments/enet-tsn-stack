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
#ifndef __CB_FRER_IPC_H__
#define __CB_FRER_IPC_H__
#include <tsn_unibase/ub_strutils.h>
#include "combase_link.h"

/* FRER daemon node */
#define FRERD_DEFAULT_DMNODE "/tmp/frerd"

/* Maximum number of port for each in/out direction */
#define FRER_PORT_MAX CBL_FRER_PORT_MAX

/* FRER mode: replicate/eliminate */
#define FRER_MODE_INVALID CBL_FRER_INVALID
#define FRER_MODE_REP CBL_FRER_REPLICATE
#define FRER_MODE_ELI CBL_FRER_ELIMINATE

/* Control: enable/disable/check/exit */
#define FRER_DISABLE (0)
#define FRER_ENABLE (1)
#define FRER_CHECK (2)
#define FRER_EXIT (0xFF)

typedef struct frer_req {
	uint8_t mode; /* 0: replicate, 1: eliminate */
	uint8_t control; /* 0: disable, 1: enable, 2: check, 0xff: exit */
	uint8_t in_port_num; /* number of input ports */
	uint8_t out_port_num; /* number of output ports */
	uint32_t in_ports[FRER_PORT_MAX]; /* input port index */
	uint32_t out_ports[FRER_PORT_MAX]; /* output port index */
	int32_t vlanid; /* vlan id, used for filtering stream */
	ub_macaddr_t dest_mac; /* destination mac address, used for filtering stream */
} frer_req_t;

#endif /* __CB_FRER_IPC_H__ */
