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
#include <stdlib.h>
#include <tsn_unibase/unibase.h>
#include "yang_modules.h"
#include "../yang_db_access.h"
#include "../yang_node.h"
#include "ieee1588-ptp-tt.h"

extern uint8_t IEEE1588_PTP_TT_func(uc_dbald *dbald);
#define IEEE1588_PTP_TT_RW IEEE1588_PTP_TT_func(dbald)
#define IEEE1588_PTP_TT_RO (IEEE1588_PTP_TT_func(dbald)|0x80u)

const char *ieee1588_ptp_tt_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"nsasc", 		// 2(0x2)
	"ptp", 		// 3(0x3)
	"instances", 		// 4(0x4)
	"instance", 		// 5(0x5)
	"instance-index", 		// 6(0x6)
	"default-ds", 		// 7(0x7)
	"clock-identity", 		// 8(0x8)
	"number-ports", 		// 9(0x9)
	"clock-quality", 		// 10(0xa)
	"clock-class", 		// 11(0xb)
	"clock-accuracy", 		// 12(0xc)
	"offset-scaled-log-variance", 		// 13(0xd)
	"priority1", 		// 14(0xe)
	"priority2", 		// 15(0xf)
	"domain-number", 		// 16(0x10)
	"time-receiver-only", 		// 17(0x11)
	"sdo-id", 		// 18(0x12)
	"current-time", 		// 19(0x13)
	"seconds-field", 		// 20(0x14)
	"nanoseconds-field", 		// 21(0x15)
	"external-port-config-enable", 		// 22(0x16)
	"max-steps-removed", 		// 23(0x17)
	"current-ds", 		// 24(0x18)
	"steps-removed", 		// 25(0x19)
	"offset-from-time-transmitter", 		// 26(0x1a)
	"mean-delay", 		// 27(0x1b)
	"parent-ds", 		// 28(0x1c)
	"parent-port-identity", 		// 29(0x1d)
	"port-number", 		// 30(0x1e)
	"grandmaster-identity", 		// 31(0x1f)
	"grandmaster-clock-quality", 		// 32(0x20)
	"grandmaster-priority1", 		// 33(0x21)
	"grandmaster-priority2", 		// 34(0x22)
	"protocol-address", 		// 35(0x23)
	"network-protocol", 		// 36(0x24)
	"address-length", 		// 37(0x25)
	"address-field", 		// 38(0x26)
	"time-properties-ds", 		// 39(0x27)
	"current-utc-offset", 		// 40(0x28)
	"current-utc-offset-valid", 		// 41(0x29)
	"leap59", 		// 42(0x2a)
	"leap61", 		// 43(0x2b)
	"time-traceable", 		// 44(0x2c)
	"frequency-traceable", 		// 45(0x2d)
	"ptp-timescale", 		// 46(0x2e)
	"time-source", 		// 47(0x2f)
	"performance-monitoring-ds", 		// 48(0x30)
	"enable", 		// 49(0x31)
	"record-list", 		// 50(0x32)
	"index", 		// 51(0x33)
	"measurement-valid", 		// 52(0x34)
	"period-complete", 		// 53(0x35)
	"pm-time", 		// 54(0x36)
	"average-time-transmitter-time-receiver-delay", 		// 55(0x37)
	"minimum-time-transmitter-time-receiver-delay", 		// 56(0x38)
	"maximum-time-transmitter-time-receiver-delay", 		// 57(0x39)
	"stddev-time-transmitter-time-receiver-delay", 		// 58(0x3a)
	"average-time-receiver-time-transmitter-delay", 		// 59(0x3b)
	"minimum-time-receiver-time-transmitter-delay", 		// 60(0x3c)
	"maximum-time-receiver-time-transmitter-delay", 		// 61(0x3d)
	"stddev-time-receiver-time-transmitter-delay", 		// 62(0x3e)
	"average-mean-path-delay", 		// 63(0x3f)
	"minimum-mean-path-delay", 		// 64(0x40)
	"maximum-mean-path-delay", 		// 65(0x41)
	"stddev-mean-path-delay", 		// 66(0x42)
	"average-offset-from-time-transmitter", 		// 67(0x43)
	"minimum-offset-from-time-transmitter", 		// 68(0x44)
	"maximum-offset-from-time-transmitter", 		// 69(0x45)
	"stddev-offset-from-time-transmitter", 		// 70(0x46)
	"ports", 		// 71(0x47)
	"port", 		// 72(0x48)
	"port-index", 		// 73(0x49)
	"underlying-interface", 		// 74(0x4a)
	"port-ds", 		// 75(0x4b)
	"port-identity", 		// 76(0x4c)
	"port-state", 		// 77(0x4d)
	"log-min-delay-req-interval", 		// 78(0x4e)
	"mean-link-delay", 		// 79(0x4f)
	"log-announce-interval", 		// 80(0x50)
	"announce-receipt-timeout", 		// 81(0x51)
	"log-sync-interval", 		// 82(0x52)
	"delay-mechanism", 		// 83(0x53)
	"log-min-pdelay-req-interval", 		// 84(0x54)
	"version-number", 		// 85(0x55)
	"minor-version-number", 		// 86(0x56)
	"delay-asymmetry", 		// 87(0x57)
	"port-enable", 		// 88(0x58)
	"performance-monitoring-port-ds", 		// 89(0x59)
	"record-list-peer-delay", 		// 90(0x5a)
	"average-mean-link-delay", 		// 91(0x5b)
	"min-mean-link-delay", 		// 92(0x5c)
	"max-mean-link-delay", 		// 93(0x5d)
	"stddev-mean-link-delay", 		// 94(0x5e)
	"announce-tx", 		// 95(0x5f)
	"announce-rx", 		// 96(0x60)
	"announce-foreign-rx", 		// 97(0x61)
	"sync-tx", 		// 98(0x62)
	"sync-rx", 		// 99(0x63)
	"follow-up-tx", 		// 100(0x64)
	"follow-up-rx", 		// 101(0x65)
	"delay-req-tx", 		// 102(0x66)
	"delay-req-rx", 		// 103(0x67)
	"delay-resp-tx", 		// 104(0x68)
	"delay-resp-rx", 		// 105(0x69)
	"pdelay-req-tx", 		// 106(0x6a)
	"pdelay-req-rx", 		// 107(0x6b)
	"pdelay-resp-tx", 		// 108(0x6c)
	"pdelay-resp-rx", 		// 109(0x6d)
	"pdelay-resp-follow-up-tx", 		// 110(0x6e)
	"pdelay-resp-follow-up-rx", 		// 111(0x6f)
	"external-port-config-port-ds", 		// 112(0x70)
	"desired-state", 		// 113(0x71)
	// augmented by ieee802-dot1as-ptp
	"gm-capable",		// 114(0x72)
	"last-gm-phase-change",		// 115(0x73)
	"last-gm-freq-change",		// 116(0x74)
	"gm-timebase-indicator",		// 117(0x75)
	"time-of-last-freq-change",		// 118(0x76)
	"is-measuring-delay",		// 119(0x77)
	"as-capable",		// 120(0x78)
	"mean-link-delay-thresh",		// 121(0x79)
	"neighbor-rate-ratio",		// 122(0x7a)
	"initial-log-announce-interval",		// 123(0x7b)
	"current-log-announce-interval",		// 124(0x7c)
	"use-mgt-log-announce-interval",		// 125(0x7d)
	"initial-log-sync-interval",		// 126(0x7e)
	"current-log-sync-interval",		// 127(0x7f)
	"use-mgt-log-sync-interval",		// 128(0x80)
	"sync-receipt-timeout",		// 129(0x81)
	"sync-receipt-timeout-interval",		// 130(0x82)
	"initial-log-pdelay-req-interval",		// 131(0x83)
	"current-log-pdelay-req-interval",		// 132(0x84)
	"use-mgt-log-pdelay-req-interval",		// 133(0x85)
	"mgt-log-pdelay-req-interval",		// 134(0x86)
	"current-log-gptp-cap-interval",		// 135(0x87)
	"use-mgt-compute-rate-ratio",		// 136(0x88)
	"use-mgt-compute-mean-link-delay",		// 137(0x89)
	"allowed-lost-responses",		// 138(0x8a)
	"allowed-faults",		// 139(0x8b)
	"gptp-cap-receipt-timeout",		// 140(0x8c)
	"one-step-tx-oper",		// 141(0x8d)
	"one-step-receive",		// 142(0x8e)
	"one-step-transmit",		// 143(0x8f)
	"initial-one-step-tx-oper",		// 144(0x90)
	"current-one-step-tx-oper",		// 145(0x91)
	"use-mgt-one-step-tx-oper",		// 146(0x92)
	"mgt-one-step-tx-oper",		// 147(0x93)
	"pdelay-truncated-timestamps",		// 148(0x94)
	"port-statistics-ds",		// 149(0x95)
	"rx-sync-count",		// 150(0x96)
	"rx-one-step-sync-count",		// 151(0x97)
	"rx-follow-up-count",		// 152(0x98)
	"rx-pdelay-req-count",		// 153(0x99)
	"rx-pdelay-resp-count",		// 154(0x9a)
	"rx-pdelay-resp-follow-up-count",		// 155(0x9b)
	"rx-announce-count",		// 156(0x9c)
	"tx-sync-count",		// 157(0x9d)
	"tx-follow-up-count",		// 158(0x9e)
	"tx-pdelay-req-count",		// 159(0x9f)
	"tx-pdelay-resp-count",		// 160(0xa0)
	"tx-pdelay-resp-follow-up-count",		// 161(0xa1)
	"tx-announce-count",		// 162(0xa2)
	// augmented by xl4gptp
	"port-clock",		// 163(0xa3)
	"adjrate",		// 164(0xa4)
	"ptpdev-name",		// 165(0xa5)
	"clock-state",		// 166(0xa6)
	"gmstate",		// 167(0xa7)
	"instance-domain-map",		// 168(0xa8)
};
const uint8_t ieee1588_ptp_tt_enum_max=169;

static int prefix_namespace_init(uc_dbald *dbald)
{
	if(yang_node_set_prefix_namespace(dbald, "ptp-tt",
		"urn:ieee:std:1588:yang:ieee1588-ptp-tt")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "xl4gptp",
		"http://excelfore.com/ns/xl4gptp")!=0){
		return -1;
	}
	if(yang_node_set_prefix_namespace(dbald, "dot1as-ptp",
		"urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp")!=0){
		return -1;
	}
	return 0;
}

static int node_namespace_init(uc_dbald *dbald)
{
	uint8_t apsd[11];
	apsd[0]=IEEE1588_PTP_TT_RW;
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_NSASC;
	apsd[3]=255u;
	if(uc_dbal_create(dbald, apsd, 4, (void*)"ptp-tt", 7)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_GM_CAPABLE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_CURRENT_UTC_OFFSET;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_CURRENT_UTC_OFFSET_VALID;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_LEAP59;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_LEAP61;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_TIME_TRACEABLE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_FREQUENCY_TRACEABLE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_PTP_TIMESCALE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_DEFAULT_DS;
	apsd[5]=IEEE1588_PTP_TT_TIME_SOURCE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_CURRENT_DS;
	apsd[5]=IEEE1588_PTP_TT_LAST_GM_PHASE_CHANGE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_CURRENT_DS;
	apsd[5]=IEEE1588_PTP_TT_LAST_GM_FREQ_CHANGE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_CURRENT_DS;
	apsd[5]=IEEE1588_PTP_TT_GM_TIMEBASE_INDICATOR;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_CURRENT_DS;
	apsd[5]=IEEE1588_PTP_TT_TIME_OF_LAST_FREQ_CHANGE;
	apsd[6]=IEEE1588_PTP_TT_NSASC;
	apsd[7]=255u;
	if(uc_dbal_create(dbald, apsd, 8, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_IS_MEASURING_DELAY;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_AS_CAPABLE;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_MEAN_LINK_DELAY_THRESH;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_NEIGHBOR_RATE_RATIO;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_INITIAL_LOG_ANNOUNCE_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_CURRENT_LOG_ANNOUNCE_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_USE_MGT_LOG_ANNOUNCE_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_INITIAL_LOG_SYNC_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_CURRENT_LOG_SYNC_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_USE_MGT_LOG_SYNC_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_SYNC_RECEIPT_TIMEOUT;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_SYNC_RECEIPT_TIMEOUT_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_INITIAL_LOG_PDELAY_REQ_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_CURRENT_LOG_PDELAY_REQ_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_USE_MGT_LOG_PDELAY_REQ_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_MGT_LOG_PDELAY_REQ_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_CURRENT_LOG_GPTP_CAP_INTERVAL;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_USE_MGT_COMPUTE_RATE_RATIO;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_USE_MGT_COMPUTE_MEAN_LINK_DELAY;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_ALLOWED_LOST_RESPONSES;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_ALLOWED_FAULTS;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_GPTP_CAP_RECEIPT_TIMEOUT;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_ONE_STEP_TX_OPER;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_ONE_STEP_RECEIVE;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_ONE_STEP_TRANSMIT;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_INITIAL_ONE_STEP_TX_OPER;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_CURRENT_ONE_STEP_TX_OPER;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_USE_MGT_ONE_STEP_TX_OPER;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_MGT_ONE_STEP_TX_OPER;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_DS;
	apsd[7]=IEEE1588_PTP_TT_PDELAY_TRUNCATED_TIMESTAMPS;
	apsd[8]=IEEE1588_PTP_TT_NSASC;
	apsd[9]=255u;
	if(uc_dbal_create(dbald, apsd, 10, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_STATISTICS_DS;
	apsd[7]=IEEE1588_PTP_TT_NSASC;
	apsd[8]=255u;
	if(uc_dbal_create(dbald, apsd, 9, (void*)"dot1as-ptp", 11)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_PORTS;
	apsd[5]=IEEE1588_PTP_TT_PORT;
	apsd[6]=IEEE1588_PTP_TT_PORT_CLOCK;
	apsd[7]=IEEE1588_PTP_TT_NSASC;
	apsd[8]=255u;
	if(uc_dbal_create(dbald, apsd, 9, (void*)"xl4gptp", 8)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCES;
	apsd[3]=IEEE1588_PTP_TT_INSTANCE;
	apsd[4]=IEEE1588_PTP_TT_CLOCK_STATE;
	apsd[5]=IEEE1588_PTP_TT_NSASC;
	apsd[6]=255u;
	if(uc_dbal_create(dbald, apsd, 7, (void*)"xl4gptp", 8)!=0){
		return -1;
	}
	apsd[1]=IEEE1588_PTP_TT_PTP;
	apsd[2]=IEEE1588_PTP_TT_INSTANCE_DOMAIN_MAP;
	apsd[3]=IEEE1588_PTP_TT_NSASC;
	apsd[4]=255u;
	if(uc_dbal_create(dbald, apsd, 5, (void*)"xl4gptp", 8)!=0){
		return -1;
	}
	return 0;
}

static int enumstring_init(uc_dbald *dbald, uint8_t modid)
{
	return 0;
}

int ieee1588_ptp_tt_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[12]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	if(enumstring_init(dbald, 0)!=0){return -1;}
	if(prefix_namespace_init(dbald)!=0){return -1;}
	if(node_namespace_init(dbald)!=0){return -1;}
	//0000_ieee1588-ptp-tt/ptp/instances/instance/default-ds/clock-quality
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[1] = IEEE1588_PTP_TT_PTP;
	aps[2] = IEEE1588_PTP_TT_INSTANCES;
	aps[3] = IEEE1588_PTP_TT_INSTANCE;
	aps[4] = IEEE1588_PTP_TT_DEFAULT_DS;
	aps[5] = IEEE1588_PTP_TT_CLOCK_QUALITY;
	aps[6] = IEEE1588_PTP_TT_CLOCK_CLASS;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_CLOCK_ACCURACY;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_OFFSET_SCALED_LOG_VARIANCE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE1588_PTP_TT_VALUEKEY;
	aps[5] = IEEE1588_PTP_TT_INSTANCE_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0001_ieee1588-ptp-tt/ptp/instances/instance/default-ds/current-time
	aps[4] = IEEE1588_PTP_TT_DEFAULT_DS;
	aps[5] = IEEE1588_PTP_TT_CURRENT_TIME;
	aps[6] = IEEE1588_PTP_TT_SECONDS_FIELD;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_NANOSECONDS_FIELD;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0002_ieee1588-ptp-tt/ptp/instances/instance/default-ds
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[5] = IEEE1588_PTP_TT_CLOCK_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_NUMBER_PORTS;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[5] = IEEE1588_PTP_TT_PRIORITY1;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_PRIORITY2;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[5] = IEEE1588_PTP_TT_DOMAIN_NUMBER;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[5] = IEEE1588_PTP_TT_TIME_RECEIVER_ONLY;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[5] = IEEE1588_PTP_TT_SDO_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[5] = IEEE1588_PTP_TT_EXTERNAL_PORT_CONFIG_ENABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[5] = IEEE1588_PTP_TT_MAX_STEPS_REMOVED;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_GM_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_CURRENT_UTC_OFFSET;
	vtype=YANG_VTYPE_INT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_CURRENT_UTC_OFFSET_VALID;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_LEAP59;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_LEAP61;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_TIME_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_FREQUENCY_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_PTP_TIMESCALE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_TIME_SOURCE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0003_ieee1588-ptp-tt/ptp/instances/instance/current-ds
	aps[4] = IEEE1588_PTP_TT_CURRENT_DS;
	aps[5] = IEEE1588_PTP_TT_STEPS_REMOVED;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_OFFSET_FROM_TIME_TRANSMITTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_MEAN_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_LAST_GM_PHASE_CHANGE;
	vtype=YANG_VTYPE_SCALED_NS;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_LAST_GM_FREQ_CHANGE;
	vtype=YANG_VTYPE_FLOAT64;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_GM_TIMEBASE_INDICATOR;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_TIME_OF_LAST_FREQ_CHANGE;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0004_ieee1588-ptp-tt/ptp/instances/instance/parent-ds/parent-port-identity
	aps[4] = IEEE1588_PTP_TT_PARENT_DS;
	aps[5] = IEEE1588_PTP_TT_PARENT_PORT_IDENTITY;
	aps[6] = IEEE1588_PTP_TT_CLOCK_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_PORT_NUMBER;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0005_ieee1588-ptp-tt/ptp/instances/instance/parent-ds/grandmaster-clock-quality
	aps[5] = IEEE1588_PTP_TT_GRANDMASTER_CLOCK_QUALITY;
	aps[6] = IEEE1588_PTP_TT_CLOCK_CLASS;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_CLOCK_ACCURACY;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_OFFSET_SCALED_LOG_VARIANCE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0006_ieee1588-ptp-tt/ptp/instances/instance/parent-ds/protocol-address
	aps[5] = IEEE1588_PTP_TT_PROTOCOL_ADDRESS;
	aps[6] = IEEE1588_PTP_TT_NETWORK_PROTOCOL;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_ADDRESS_LENGTH;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_ADDRESS_FIELD;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0007_ieee1588-ptp-tt/ptp/instances/instance/parent-ds
	aps[5] = IEEE1588_PTP_TT_GRANDMASTER_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_GRANDMASTER_PRIORITY1;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_GRANDMASTER_PRIORITY2;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0008_ieee1588-ptp-tt/ptp/instances/instance/time-properties-ds
	aps[4] = IEEE1588_PTP_TT_TIME_PROPERTIES_DS;
	aps[5] = IEEE1588_PTP_TT_CURRENT_UTC_OFFSET;
	vtype=YANG_VTYPE_INT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_CURRENT_UTC_OFFSET_VALID;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_LEAP59;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_LEAP61;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_TIME_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_FREQUENCY_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_PTP_TIMESCALE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TT_TIME_SOURCE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0009_ieee1588-ptp-tt/ptp/instances/instance/performance-monitoring-ds/record-list
	aps[4] = IEEE1588_PTP_TT_PERFORMANCE_MONITORING_DS;
	aps[5] = IEEE1588_PTP_TT_RECORD_LIST;
	aps[6] = IEEE1588_PTP_TT_MEASUREMENT_VALID;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_PERIOD_COMPLETE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_PM_TIME;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_AVERAGE_TIME_TRANSMITTER_TIME_RECEIVER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MINIMUM_TIME_TRANSMITTER_TIME_RECEIVER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MAXIMUM_TIME_TRANSMITTER_TIME_RECEIVER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_STDDEV_TIME_TRANSMITTER_TIME_RECEIVER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_AVERAGE_TIME_RECEIVER_TIME_TRANSMITTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MINIMUM_TIME_RECEIVER_TIME_TRANSMITTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MAXIMUM_TIME_RECEIVER_TIME_TRANSMITTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_STDDEV_TIME_RECEIVER_TIME_TRANSMITTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_AVERAGE_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MINIMUM_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MAXIMUM_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_STDDEV_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_AVERAGE_OFFSET_FROM_TIME_TRANSMITTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MINIMUM_OFFSET_FROM_TIME_TRANSMITTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_MAXIMUM_OFFSET_FROM_TIME_TRANSMITTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_STDDEV_OFFSET_FROM_TIME_TRANSMITTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_TT_VALUEKEY;
	aps[7] = IEEE1588_PTP_TT_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0010_ieee1588-ptp-tt/ptp/instances/instance/performance-monitoring-ds
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[5] = IEEE1588_PTP_TT_ENABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0011_ieee1588-ptp-tt/ptp/instances/instance/ports/port/port-ds/port-identity
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[4] = IEEE1588_PTP_TT_PORTS;
	aps[5] = IEEE1588_PTP_TT_PORT;
	aps[6] = IEEE1588_PTP_TT_PORT_DS;
	aps[7] = IEEE1588_PTP_TT_PORT_IDENTITY;
	aps[8] = IEEE1588_PTP_TT_CLOCK_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_PORT_NUMBER;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[6] = IEEE1588_PTP_TT_VALUEKEY;
	aps[7] = IEEE1588_PTP_TT_PORT_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0012_ieee1588-ptp-tt/ptp/instances/instance/ports/port/port-ds
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[6] = IEEE1588_PTP_TT_PORT_DS;
	aps[7] = IEEE1588_PTP_TT_PORT_STATE;
	vtype=YANG_VTYPE_PORT_STATE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_LOG_MIN_DELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_ANNOUNCE_RECEIPT_TIMEOUT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_DELAY_MECHANISM;
	vtype=YANG_VTYPE_DELAY_MECHANISM;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_LOG_MIN_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_VERSION_NUMBER;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_MINOR_VERSION_NUMBER;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_DELAY_ASYMMETRY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_PORT_ENABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_IS_MEASURING_DELAY;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_AS_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_MEAN_LINK_DELAY_THRESH;
	vtype=YANG_VTYPE_PTP_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_NEIGHBOR_RATE_RATIO;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_INITIAL_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_CURRENT_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_USE_MGT_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_INITIAL_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_CURRENT_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_USE_MGT_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_SYNC_RECEIPT_TIMEOUT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_SYNC_RECEIPT_TIMEOUT_INTERVAL;
	vtype=YANG_VTYPE_USCALED_NS;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_INITIAL_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_CURRENT_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_USE_MGT_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_MGT_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_CURRENT_LOG_GPTP_CAP_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_USE_MGT_COMPUTE_RATE_RATIO;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_USE_MGT_COMPUTE_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_ALLOWED_LOST_RESPONSES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_ALLOWED_FAULTS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_GPTP_CAP_RECEIPT_TIMEOUT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[7] = IEEE1588_PTP_TT_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_ONE_STEP_RECEIVE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_ONE_STEP_TRANSMIT;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_INITIAL_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_CURRENT_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_USE_MGT_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_MGT_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_PDELAY_TRUNCATED_TIMESTAMPS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0013_ieee1588-ptp-tt/ptp/instances/instance/ports/port/performance-monitoring-port-ds/record-list-peer-delay
	aps[6] = IEEE1588_PTP_TT_PERFORMANCE_MONITORING_PORT_DS;
	aps[7] = IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY;
	aps[8] = IEEE1588_PTP_TT_PM_TIME;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_AVERAGE_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_MIN_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_MAX_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_STDDEV_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_VALUEKEY;
	aps[9] = IEEE1588_PTP_TT_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0014_ieee1588-ptp-tt/ptp/instances/instance/ports/port/performance-monitoring-port-ds/record-list
	aps[7] = IEEE1588_PTP_TT_RECORD_LIST;
	aps[8] = IEEE1588_PTP_TT_PM_TIME;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_ANNOUNCE_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_ANNOUNCE_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_ANNOUNCE_FOREIGN_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_SYNC_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_SYNC_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_FOLLOW_UP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_FOLLOW_UP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_DELAY_REQ_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_DELAY_REQ_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_DELAY_RESP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_DELAY_RESP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_PDELAY_REQ_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_PDELAY_REQ_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_PDELAY_RESP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_PDELAY_RESP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_PDELAY_RESP_FOLLOW_UP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_PDELAY_RESP_FOLLOW_UP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_TT_VALUEKEY;
	aps[9] = IEEE1588_PTP_TT_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0015_ieee1588-ptp-tt/ptp/instances/instance/ports/port/external-port-config-port-ds
	//0016_ieee1588-ptp-tt/ptp/instances/instance/ports/port/external-port-config-port-ds
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[6] = IEEE1588_PTP_TT_EXTERNAL_PORT_CONFIG_PORT_DS;
	aps[7] = IEEE1588_PTP_TT_DESIRED_STATE;
	vtype=YANG_VTYPE_PORT_STATE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0017_ieee1588-ptp-tt/ptp/instances/instance/ports/port/port-statistics-ds
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[6] = IEEE1588_PTP_TT_PORT_STATISTICS_DS;
	aps[7] = IEEE1588_PTP_TT_RX_SYNC_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_RX_ONE_STEP_SYNC_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_RX_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_RX_PDELAY_REQ_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_RX_PDELAY_RESP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_RX_PDELAY_RESP_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_RX_ANNOUNCE_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_TX_SYNC_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_TX_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_TX_PDELAY_REQ_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_TX_PDELAY_RESP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_TX_PDELAY_RESP_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TT_TX_ANNOUNCE_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0018_ieee1588-ptp-tt/ptp/instances/instance/ports/port/port-clock
	aps[6] = IEEE1588_PTP_TT_PORT_CLOCK;
	aps[7] = IEEE1588_PTP_TT_ADJRATE;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[7] = IEEE1588_PTP_TT_PTPDEV_NAME;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0019_ieee1588-ptp-tt/ptp/instances/instance/ports/port
	aps[6] = IEEE1588_PTP_TT_UNDERLYING_INTERFACE;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0020_ieee1588-ptp-tt/ptp/instances/instance/clock-state
	//0021_ieee1588-ptp-tt/ptp/instances/instance/clock-state
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[4] = IEEE1588_PTP_TT_CLOCK_STATE;
	aps[5] = IEEE1588_PTP_TT_GMSTATE;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0022_ieee1588-ptp-tt/ptp/instances/instance
	//0023_ieee1588-ptp-tt/ptp/instance-domain-map
	//0024_ieee1588-ptp-tt/ptp
	aps[0] = IEEE1588_PTP_TT_RW;
	aps[2] = IEEE1588_PTP_TT_INSTANCE_DOMAIN_MAP;
	vtype=YANG_VTYPE_HEXUINT16;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_TT_RO;
	aps[1] = IEEE1588_PTP_TT_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}

