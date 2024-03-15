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
#include "avbtp.h"
#include "avtpc_acf.h"

extern char *avtpc_acf_msgtype_str(avtpc_acf_message_type_t mt);
void avtpc_acf_print_msg_field(avtpc_acf_msg_field_t *msgf)
{
	UB_CONSOLE_PRINT("msg_type=%s\n", avtpc_acf_msgtype_str((avtpc_acf_message_type_t)msgf->msg_type));
	switch(msgf->msg_type){
	case AVTPC_ACF_FLEXRAY:
		UB_CONSOLE_PRINT("mtv=0x%x\n", msgf->u.flexray.mtv);
		UB_CONSOLE_PRINT("busid=0x%x\n", msgf->u.flexray.busid);
		UB_CONSOLE_PRINT("chan=0x%x\n", msgf->u.flexray.chan);
		UB_CONSOLE_PRINT("str=0x%x\n", msgf->u.flexray.str);
		UB_CONSOLE_PRINT("syn=0x%x\n", msgf->u.flexray.syn);
		UB_CONSOLE_PRINT("pre=0x%x\n", msgf->u.flexray.pre);
		UB_CONSOLE_PRINT("nfi=0x%x\n", msgf->u.flexray.nfi);
		UB_CONSOLE_PRINT("msg_ts="UB_PRIhexB8"\n", UB_ARRAY_B8(msgf->u.flexray.msg_ts));
		UB_CONSOLE_PRINT("frid=0x%x\n", msgf->u.flexray.frid);
		UB_CONSOLE_PRINT("cycle=0x%x\n", msgf->u.flexray.cycle);
		break;
	case AVTPC_ACF_CAN:
		UB_CONSOLE_PRINT("mtv=0x%x\n", msgf->u.can.mtv);
		UB_CONSOLE_PRINT("rtr=0x%x\n", msgf->u.can.rtr);
		UB_CONSOLE_PRINT("eff=0x%x\n", msgf->u.can.eff);
		UB_CONSOLE_PRINT("brs=0x%x\n", msgf->u.can.brs);
		UB_CONSOLE_PRINT("fdf=0x%x\n", msgf->u.can.fdf);
		UB_CONSOLE_PRINT("esi=0x%x\n", msgf->u.can.esi);
		UB_CONSOLE_PRINT("busid=0x%x\n", msgf->u.can.busid);
		UB_CONSOLE_PRINT("msg_ts="UB_PRIhexB8"\n", UB_ARRAY_B8(msgf->u.can.msg_ts));
		UB_CONSOLE_PRINT("canid=0x%x\n", (unsigned int)msgf->u.can.canid);
		break;
	case AVTPC_ACF_ABCAN:
		UB_CONSOLE_PRINT("mtv=0x%x\n", msgf->u.abcan.mtv);
		UB_CONSOLE_PRINT("rtr=0x%x\n", msgf->u.abcan.rtr);
		UB_CONSOLE_PRINT("eff=0x%x\n", msgf->u.abcan.eff);
		UB_CONSOLE_PRINT("brs=0x%x\n", msgf->u.abcan.brs);
		UB_CONSOLE_PRINT("fdf=0x%x\n", msgf->u.abcan.fdf);
		UB_CONSOLE_PRINT("esi=0x%x\n", msgf->u.abcan.esi);
		UB_CONSOLE_PRINT("busid=0x%x\n", msgf->u.abcan.busid);
		UB_CONSOLE_PRINT("canid=0x%x\n", (unsigned int)msgf->u.abcan.canid);
		break;
	case AVTPC_ACF_LIN:
		UB_CONSOLE_PRINT("mtv=0x%x\n", msgf->u.lin.mtv);
		UB_CONSOLE_PRINT("busid=0x%x\n", msgf->u.lin.busid);
		UB_CONSOLE_PRINT("linid=0x%x\n", msgf->u.lin.linid);
		UB_CONSOLE_PRINT("msg_ts="UB_PRIhexB8"\n", UB_ARRAY_B8(msgf->u.lin.msg_ts));
		break;
	case AVTPC_ACF_MOST:
		UB_CONSOLE_PRINT("mtv=0x%x\n", msgf->u.most.mtv);
		UB_CONSOLE_PRINT("mostid=0x%x\n", msgf->u.most.mostid);
		UB_CONSOLE_PRINT("msg_ts="UB_PRIhexB8"\n", UB_ARRAY_B8(msgf->u.most.msg_ts));
		UB_CONSOLE_PRINT("devid=0x%x\n", msgf->u.most.devid);
		UB_CONSOLE_PRINT("fbid=0x%x\n", msgf->u.most.fbid);
		UB_CONSOLE_PRINT("instid=0x%x\n", msgf->u.most.instid);
		UB_CONSOLE_PRINT("funcid=0x%x\n", msgf->u.most.funcid);
		UB_CONSOLE_PRINT("optype=0x%x\n", msgf->u.most.optype);
		break;
	case AVTPC_ACF_GPC:
		UB_CONSOLE_PRINT("msg_id="UB_PRIhexB6"\n", UB_ARRAY_B6(msgf->u.gpc.msg_id));
		break;
	case AVTPC_ACF_SERIAL:
		UB_CONSOLE_PRINT("dcd=0x%x\n", msgf->u.serial.dcd);
		UB_CONSOLE_PRINT("dtr=0x%x\n", msgf->u.serial.dtr);
		UB_CONSOLE_PRINT("dsr=0x%x\n", msgf->u.serial.dsr);
		UB_CONSOLE_PRINT("rts=0x%x\n", msgf->u.serial.rts);
		UB_CONSOLE_PRINT("cts=0x%x\n", msgf->u.serial.cts);
		UB_CONSOLE_PRINT("ri=0x%x\n", msgf->u.serial.ri);
		break;
	case AVTPC_ACF_PARALLEL:
		UB_CONSOLE_PRINT("bwidth=0x%x\n", msgf->u.parallel.bwidth);
		break;
	case AVTPC_ACF_SENSOR:
		UB_CONSOLE_PRINT("mtv=0x%x\n", msgf->u.sensor.mtv);
		UB_CONSOLE_PRINT("nums=0x%x\n", msgf->u.sensor.nums);
		UB_CONSOLE_PRINT("sz=0x%x\n", msgf->u.sensor.sz);
		UB_CONSOLE_PRINT("sgroup=0x%x\n", msgf->u.sensor.sgroup);
		UB_CONSOLE_PRINT("msg_ts="UB_PRIhexB8"\n", UB_ARRAY_B8(msgf->u.sensor.msg_ts));
		break;
	case AVTPC_ACF_ABSENSOR:
		UB_CONSOLE_PRINT("nums=0x%x\n", msgf->u.absensor.nums);
		UB_CONSOLE_PRINT("sz=0x%x\n", msgf->u.absensor.sz);
		UB_CONSOLE_PRINT("sgroup=0x%x\n", msgf->u.absensor.sgroup);
		break;
	case AVTPC_ACF_AECP:
		break;
	case AVTPC_ACF_ANCILLARY:
		UB_CONSOLE_PRINT("mode=0x%x\n", msgf->u.ancillary.mode);
		UB_CONSOLE_PRINT("fp=0x%x\n", msgf->u.ancillary.fp);
		UB_CONSOLE_PRINT("lp=0x%x\n", msgf->u.ancillary.lp);
		UB_CONSOLE_PRINT("lnum=0x%x\n", msgf->u.ancillary.lnum);
		UB_CONSOLE_PRINT("did=0x%x\n", msgf->u.ancillary.did);
		UB_CONSOLE_PRINT("sdid=0x%x\n", msgf->u.ancillary.sdid);
		break;
	case AVTPC_ACF_USER0:
	case AVTPC_ACF_USER1:
	case AVTPC_ACF_USER2:
	case AVTPC_ACF_USER3:
	case AVTPC_ACF_USER4:
	case AVTPC_ACF_USER5:
	case AVTPC_ACF_USER6:
	case AVTPC_ACF_USER7:
		UB_CONSOLE_PRINT("pl_length=%d\n", msgf->pl_length);
		ub_hexdump(true, true, (unsigned char *)msgf->payload, msgf->pl_length, 0);
		break;
	default:
		break;
	}
	return;
}
