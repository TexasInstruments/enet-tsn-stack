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
/* Automatically generated file.  Don't edit this file.*/
#ifndef XL4_EXTMOD_XL4GPTP_H_
#define XL4_EXTMOD_XL4GPTP_H_

#include <tsn_uniconf/yangs/yang_db_access.h>

typedef enum {
	XL4_EXTMOD_XL4GPTP_VALUEKEY, // 0(0x0)
	XL4_EXTMOD_XL4GPTP_DUMMY, // 1(0x1)
	XL4_EXTMOD_XL4GPTP_NSASC, // 2(0x2)
	XL4_EXTMOD_XL4GPTP_XL4GPTP, // 3(0x3)
	XL4_EXTMOD_XL4GPTP_GPTP_INSTANCE, // 4(0x4)
	XL4_EXTMOD_XL4GPTP_INSTANCE_INDEX, // 5(0x5)
	XL4_EXTMOD_XL4GPTP_TEST_SYNC_REC_PORT, // 6(0x6)
	XL4_EXTMOD_XL4GPTP_TEST_SYNC_SEND_PORT, // 7(0x7)
	XL4_EXTMOD_XL4GPTP_MAX_DOMAIN_NUMBER, // 8(0x8)
	XL4_EXTMOD_XL4GPTP_MASTER_PTPDEV, // 9(0x9)
	XL4_EXTMOD_XL4GPTP_TXTS_LOST_TIME, // 10(0xa)
	XL4_EXTMOD_XL4GPTP_SOCKET_TXPRIORITY, // 11(0xb)
	XL4_EXTMOD_XL4GPTP_FIRST_DOMAIN_THIS_CLOCK, // 12(0xc)
	XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_THIS_CLOCK, // 13(0xd)
	XL4_EXTMOD_XL4GPTP_SECOND_DOMAIN_NUMBER, // 14(0xe)
	XL4_EXTMOD_XL4GPTP_SINGLE_CLOCK_MODE, // 15(0xf)
	XL4_EXTMOD_XL4GPTP_AFTERSEND_GUARDTIME, // 16(0x10)
	XL4_EXTMOD_XL4GPTP_GPTPNET_EXTRA_TOUTNS, // 17(0x11)
	XL4_EXTMOD_XL4GPTP_MAX_ADJUST_RATE_ON_CLOCK, // 18(0x12)
	XL4_EXTMOD_XL4GPTP_NEIGHBOR_PROP_DELAY, // 19(0x13)
	XL4_EXTMOD_XL4GPTP_STATIC_PORT_STATE_SLAVE_PORT, // 20(0x14)
	XL4_EXTMOD_XL4GPTP_FOLLOW_AVNU, // 21(0x15)
	XL4_EXTMOD_XL4GPTP_BMCS_QUICK_UPDATE_MODE, // 22(0x16)
	XL4_EXTMOD_XL4GPTP_CMLDS_MODE, // 23(0x17)
	XL4_EXTMOD_XL4GPTP_NEIGHBOR_PROPDELAY_MINLIMIT, // 24(0x18)
	XL4_EXTMOD_XL4GPTP_INITIAL_SLAVE_TIME, // 25(0x19)
	XL4_EXTMOD_XL4GPTP_INITIAL_GM_STABLE_TIME, // 26(0x1a)
	XL4_EXTMOD_XL4GPTP_NORMAL_GM_STABLE_TIME, // 27(0x1b)
	XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_START_VALUE, // 28(0x1c)
	XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_IIR_ALPHA_STABLE_VALUE, // 29(0x1d)
	XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_START_VALUE, // 30(0x1e)
	XL4_EXTMOD_XL4GPTP_PHASE_OFFSET_IIR_ALPHA_STABLE_VALUE, // 31(0x1f)
	XL4_EXTMOD_XL4GPTP_CLOCK_COMPUTE_INTERVAL_MSEC, // 32(0x20)
	XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_UPDATE_MRATE_PPB, // 33(0x21)
	XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_STABLE_PPB, // 34(0x22)
	XL4_EXTMOD_XL4GPTP_FREQ_OFFSET_TIMELEAP_MAX_JUMP_PPB, // 35(0x23)
	XL4_EXTMOD_XL4GPTP_ACTIVE_DOMAIN_AUTO_SWITCH, // 36(0x24)
	XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_ON, // 37(0x25)
	XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_ALIGNTIME, // 38(0x26)
	XL4_EXTMOD_XL4GPTP_TSN_SCHEDULE_CYCLETIME, // 39(0x27)
	XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_FILE, // 40(0x28)
	XL4_EXTMOD_XL4GPTP_DEBUGLOG_MEMORY_SIZE, // 41(0x29)
	XL4_EXTMOD_XL4GPTP_OVIP_MODE_STRT_PORTNO, // 42(0x2a)
	XL4_EXTMOD_XL4GPTP_MASTER_CLOCK_SHARED_MEM, // 43(0x2b)
	XL4_EXTMOD_XL4GPTP_PTPVFD_CLOCK_RATE, // 44(0x2c)
	XL4_EXTMOD_XL4GPTP_MAX_CONSEC_TS_DIFF, // 45(0x2d)
	XL4_EXTMOD_XL4GPTP_TS2DIFF_CACHE_FACTOR, // 46(0x2e)
	XL4_EXTMOD_XL4GPTP_USE_HW_PHASE_ADJUSTMENT, // 47(0x2f)
	XL4_EXTMOD_XL4GPTP_PHASE_ADJUSTMENT_BY_FREQ, // 48(0x30)
	XL4_EXTMOD_XL4GPTP_ACTIVATE_ABNORMAL_HOOKS, // 49(0x31)
	XL4_EXTMOD_XL4GPTP_RESET_FREQADJ_BECOMEGM, // 50(0x32)
	XL4_EXTMOD_XL4GPTP_REPRESENT_PORT_NUMBER, // 51(0x33)
	XL4_EXTMOD_XL4GPTP_PERFMON_SHORT_PERIOD, // 52(0x34)
	XL4_EXTMOD_XL4GPTP_PERFMON_LONG_PERIOD, // 53(0x35)
	XL4_EXTMOD_XL4GPTP_PERFMON_CURRENT_PERIOD, // 54(0x36)
	XL4_EXTMOD_XL4GPTP_CONF_TILLD_PPS_REFCLK_HZ, // 55(0x37)
	XL4_EXTMOD_XL4GPTP_CONF_TILLD_PPS_OUTIDX, // 56(0x38)
	XL4_EXTMOD_XL4GPTP_ENUM_END,
} xl4_extmod_xl4gptp_enum_t;

int xl4_extmod_xl4gptp_config_init(uc_dbald *dbald, uc_hwald *hwald);

#endif
