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
#include "../yang_modules.h"
#include "ieee1588-ptp.h"
#include "ietf-interfaces.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "ietf-netconf-monitoring.h"
#include "ietf-yang-library.h"
#include "excelfore-tsn-remote.h"
#include "excelfore-netconf-server.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *ieee1588_ptp_enum_strings[]={
	"valuekey", 		// 0(0x0)
	"dummy", 		// 1(0x1)
	"ptp", 		// 2(0x2)
	"instances", 		// 3(0x3)
	"instance", 		// 4(0x4)
	"instance-index", 		// 5(0x5)
	"default-ds", 		// 6(0x6)
	"clock-identity", 		// 7(0x7)
	"number-ports", 		// 8(0x8)
	"clock-quality", 		// 9(0x9)
	"clock-class", 		// 10(0xa)
	"clock-accuracy", 		// 11(0xb)
	"offset-scaled-log-variance", 		// 12(0xc)
	"priority1", 		// 13(0xd)
	"priority2", 		// 14(0xe)
	"domain-number", 		// 15(0xf)
	"slave-only", 		// 16(0x10)
	"sdo-id", 		// 17(0x11)
	"current-time", 		// 18(0x12)
	"seconds-field", 		// 19(0x13)
	"nanoseconds-field", 		// 20(0x14)
	"external-port-config-enable", 		// 21(0x15)
	"max-steps-removed", 		// 22(0x16)
	"current-ds", 		// 23(0x17)
	"steps-removed", 		// 24(0x18)
	"offset-from-master", 		// 25(0x19)
	"mean-delay", 		// 26(0x1a)
	"parent-ds", 		// 27(0x1b)
	"parent-port-identity", 		// 28(0x1c)
	"port-number", 		// 29(0x1d)
	"grandmaster-identity", 		// 30(0x1e)
	"grandmaster-clock-quality", 		// 31(0x1f)
	"grandmaster-priority1", 		// 32(0x20)
	"grandmaster-priority2", 		// 33(0x21)
	"protocol-address", 		// 34(0x22)
	"network-protocol", 		// 35(0x23)
	"address-length", 		// 36(0x24)
	"address-field", 		// 37(0x25)
	"time-properties-ds", 		// 38(0x26)
	"current-utc-offset", 		// 39(0x27)
	"current-utc-offset-valid", 		// 40(0x28)
	"leap59", 		// 41(0x29)
	"leap61", 		// 42(0x2a)
	"time-traceable", 		// 43(0x2b)
	"frequency-traceable", 		// 44(0x2c)
	"ptp-timescale", 		// 45(0x2d)
	"time-source", 		// 46(0x2e)
	"performance-monitoring-ds", 		// 47(0x2f)
	"enable", 		// 48(0x30)
	"record-list", 		// 49(0x31)
	"index", 		// 50(0x32)
	"measurement-valid", 		// 51(0x33)
	"period-complete", 		// 52(0x34)
	"pm-time", 		// 53(0x35)
	"average-master-slave-delay", 		// 54(0x36)
	"minimum-master-slave-delay", 		// 55(0x37)
	"maximum-master-slave-delay", 		// 56(0x38)
	"stddev-master-slave-delay", 		// 57(0x39)
	"average-slave-master-delay", 		// 58(0x3a)
	"minimum-slave-master-delay", 		// 59(0x3b)
	"maximum-slave-master-delay", 		// 60(0x3c)
	"stddev-slave-master-delay", 		// 61(0x3d)
	"average-mean-path-delay", 		// 62(0x3e)
	"minimum-mean-path-delay", 		// 63(0x3f)
	"maximum-mean-path-delay", 		// 64(0x40)
	"stddev-mean-path-delay", 		// 65(0x41)
	"average-offset-from-master", 		// 66(0x42)
	"minimum-offset-from-master", 		// 67(0x43)
	"maximum-offset-from-master", 		// 68(0x44)
	"stddev-offset-from-master", 		// 69(0x45)
	"ports", 		// 70(0x46)
	"port", 		// 71(0x47)
	"port-index", 		// 72(0x48)
	"underlying-interface", 		// 73(0x49)
	"port-ds", 		// 74(0x4a)
	"port-identity", 		// 75(0x4b)
	"port-state", 		// 76(0x4c)
	"log-min-delay-req-interval", 		// 77(0x4d)
	"mean-link-delay", 		// 78(0x4e)
	"log-announce-interval", 		// 79(0x4f)
	"announce-receipt-timeout", 		// 80(0x50)
	"log-sync-interval", 		// 81(0x51)
	"delay-mechanism", 		// 82(0x52)
	"log-min-pdelay-req-interval", 		// 83(0x53)
	"version-number", 		// 84(0x54)
	"minor-version-number", 		// 85(0x55)
	"delay-asymmetry", 		// 86(0x56)
	"port-enable", 		// 87(0x57)
	"performance-monitoring-port-ds", 		// 88(0x58)
	"record-list-peer-delay", 		// 89(0x59)
	"average-mean-link-delay", 		// 90(0x5a)
	"min-mean-link-delay", 		// 91(0x5b)
	"max-mean-link-delay", 		// 92(0x5c)
	"stddev-mean-link-delay", 		// 93(0x5d)
	"announce-tx", 		// 94(0x5e)
	"announce-rx", 		// 95(0x5f)
	"announce-foreign-rx", 		// 96(0x60)
	"sync-tx", 		// 97(0x61)
	"sync-rx", 		// 98(0x62)
	"follow-up-tx", 		// 99(0x63)
	"follow-up-rx", 		// 100(0x64)
	"delay-req-tx", 		// 101(0x65)
	"delay-req-rx", 		// 102(0x66)
	"delay-resp-tx", 		// 103(0x67)
	"delay-resp-rx", 		// 104(0x68)
	"pdelay-req-tx", 		// 105(0x69)
	"pdelay-req-rx", 		// 106(0x6a)
	"pdelay-resp-tx", 		// 107(0x6b)
	"pdelay-resp-rx", 		// 108(0x6c)
	"pdelay-resp-follow-up-tx", 		// 109(0x6d)
	"pdelay-resp-follow-up-rx", 		// 110(0x6e)
	"external-port-config-port-ds", 		// 111(0x6f)
	"desired-state", 		// 112(0x70)
	// augmented by ieee802-dot1as-ptp
	"gm-capable",		// 113(0x71)
	"last-gm-phase-change",		// 114(0x72)
	"last-gm-freq-change",		// 115(0x73)
	"gm-timebase-indicator",		// 116(0x74)
	"time-of-last-freq-change",		// 117(0x75)
	"is-measuring-delay",		// 118(0x76)
	"as-capable",		// 119(0x77)
	"mean-link-delay-thresh",		// 120(0x78)
	"neighbor-rate-ratio",		// 121(0x79)
	"initial-log-announce-interval",		// 122(0x7a)
	"current-log-announce-interval",		// 123(0x7b)
	"use-mgt-log-announce-interval",		// 124(0x7c)
	"initial-log-sync-interval",		// 125(0x7d)
	"current-log-sync-interval",		// 126(0x7e)
	"use-mgt-log-sync-interval",		// 127(0x7f)
	"sync-receipt-timeout",		// 128(0x80)
	"sync-receipt-timeout-interval",		// 129(0x81)
	"initial-log-pdelay-req-interval",		// 130(0x82)
	"current-log-pdelay-req-interval",		// 131(0x83)
	"use-mgt-log-pdelay-req-interval",		// 132(0x84)
	"mgt-log-pdelay-req-interval",		// 133(0x85)
	"current-log-gptp-cap-interval",		// 134(0x86)
	"use-mgt-compute-rate-ratio",		// 135(0x87)
	"use-mgt-compute-mean-link-delay",		// 136(0x88)
	"allowed-lost-responses",		// 137(0x89)
	"allowed-faults",		// 138(0x8a)
	"gptp-cap-receipt-timeout",		// 139(0x8b)
	"one-step-tx-oper",		// 140(0x8c)
	"one-step-receive",		// 141(0x8d)
	"one-step-transmit",		// 142(0x8e)
	"initial-one-step-tx-oper",		// 143(0x8f)
	"current-one-step-tx-oper",		// 144(0x90)
	"use-mgt-one-step-tx-oper",		// 145(0x91)
	"mgt-one-step-tx-oper",		// 146(0x92)
	"pdelay-truncated-timestamps",		// 147(0x93)
	"port-statistics-ds",		// 148(0x94)
	"rx-sync-count",		// 149(0x95)
	"rx-one-step-sync-count",		// 150(0x96)
	"rx-follow-up-count",		// 151(0x97)
	"rx-pdelay-req-count",		// 152(0x98)
	"rx-pdelay-resp-count",		// 153(0x99)
	"rx-pdelay-resp-follow-up-count",		// 154(0x9a)
	"rx-announce-count",		// 155(0x9b)
	"tx-sync-count",		// 156(0x9c)
	"tx-follow-up-count",		// 157(0x9d)
	"tx-pdelay-req-count",		// 158(0x9e)
	"tx-pdelay-resp-count",		// 159(0x9f)
	"tx-pdelay-resp-follow-up-count",		// 160(0xa0)
	"tx-announce-count",		// 161(0xa1)
	// augmented by xl4gptp
	"port-clock",		// 162(0xa2)
	"adjrate",		// 163(0xa3)
	"clock-state",		// 164(0xa4)
	"gmstate",		// 165(0xa5)
	"instance-domain-map",		// 166(0xa6)
};

uint8_t ieee1588_ptp_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)IEEE1588_PTP_ENUM_END;i++){
		if(!strcmp(astr, ieee1588_ptp_enum_strings[i])){return i;}
	}
        return 0xffu;
}

const char *ieee1588_ptp_get_string(uint8_t anum)
{
        if(anum>=(uint8_t)IEEE1588_PTP_ENUM_END){return NULL;}
	return ieee1588_ptp_enum_strings[anum];
}

int ieee1588_ptp_runconf_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t apsd[12]={XL4_DATA_RW, YANG_VALUE_TYPES, 0,};
	uint8_t *aps=&apsd[2];
	int res=-1;
	uint8_t vtype;
	//0000_ieee1588-ptp/ptp/instances/instance/default-ds/clock-quality
	aps[0] = IEEE1588_PTP_RW;
	aps[1] = IEEE1588_PTP_PTP;
	aps[2] = IEEE1588_PTP_INSTANCES;
	aps[3] = IEEE1588_PTP_INSTANCE;
	aps[4] = IEEE1588_PTP_DEFAULT_DS;
	aps[5] = IEEE1588_PTP_CLOCK_QUALITY;
	aps[6] = IEEE1588_PTP_CLOCK_CLASS;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_CLOCK_ACCURACY;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_OFFSET_SCALED_LOG_VARIANCE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[4] = IEEE1588_PTP_VALUEKEY;
	aps[5] = IEEE1588_PTP_INSTANCE_INDEX;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0001_ieee1588-ptp/ptp/instances/instance/default-ds/current-time
	aps[4] = IEEE1588_PTP_DEFAULT_DS;
	aps[5] = IEEE1588_PTP_CURRENT_TIME;
	aps[6] = IEEE1588_PTP_SECONDS_FIELD;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_NANOSECONDS_FIELD;
	vtype=YANG_VTYPE_UINT32;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0002_ieee1588-ptp/ptp/instances/instance/default-ds
	aps[0] = IEEE1588_PTP_RO;
	aps[5] = IEEE1588_PTP_CLOCK_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_NUMBER_PORTS;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[5] = IEEE1588_PTP_PRIORITY1;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_PRIORITY2;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[5] = IEEE1588_PTP_DOMAIN_NUMBER;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[5] = IEEE1588_PTP_SLAVE_ONLY;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[5] = IEEE1588_PTP_SDO_ID;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[5] = IEEE1588_PTP_EXTERNAL_PORT_CONFIG_ENABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[5] = IEEE1588_PTP_MAX_STEPS_REMOVED;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_GM_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_CURRENT_UTC_OFFSET;
	vtype=YANG_VTYPE_INT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_CURRENT_UTC_OFFSET_VALID;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_LEAP59;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_LEAP61;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TIME_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_FREQUENCY_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_PTP_TIMESCALE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TIME_SOURCE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0003_ieee1588-ptp/ptp/instances/instance/current-ds
	aps[4] = IEEE1588_PTP_CURRENT_DS;
	aps[5] = IEEE1588_PTP_STEPS_REMOVED;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_OFFSET_FROM_MASTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_MEAN_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_LAST_GM_PHASE_CHANGE;
	vtype=YANG_VTYPE_SCALED_NS;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_LAST_GM_FREQ_CHANGE;
	vtype=YANG_VTYPE_FLOAT64;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_GM_TIMEBASE_INDICATOR;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TIME_OF_LAST_FREQ_CHANGE;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0004_ieee1588-ptp/ptp/instances/instance/parent-ds/parent-port-identity
	aps[4] = IEEE1588_PTP_PARENT_DS;
	aps[5] = IEEE1588_PTP_PARENT_PORT_IDENTITY;
	aps[6] = IEEE1588_PTP_CLOCK_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_PORT_NUMBER;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0005_ieee1588-ptp/ptp/instances/instance/parent-ds/grandmaster-clock-quality
	aps[5] = IEEE1588_PTP_GRANDMASTER_CLOCK_QUALITY;
	aps[6] = IEEE1588_PTP_CLOCK_CLASS;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_CLOCK_ACCURACY;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_OFFSET_SCALED_LOG_VARIANCE;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0006_ieee1588-ptp/ptp/instances/instance/parent-ds/protocol-address
	aps[5] = IEEE1588_PTP_PROTOCOL_ADDRESS;
	aps[6] = IEEE1588_PTP_NETWORK_PROTOCOL;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_ADDRESS_LENGTH;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_ADDRESS_FIELD;
	vtype=YANG_VTYPE_STRING;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0007_ieee1588-ptp/ptp/instances/instance/parent-ds
	aps[5] = IEEE1588_PTP_GRANDMASTER_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_GRANDMASTER_PRIORITY1;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_GRANDMASTER_PRIORITY2;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0008_ieee1588-ptp/ptp/instances/instance/time-properties-ds
	aps[4] = IEEE1588_PTP_TIME_PROPERTIES_DS;
	aps[5] = IEEE1588_PTP_CURRENT_UTC_OFFSET;
	vtype=YANG_VTYPE_INT16;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_CURRENT_UTC_OFFSET_VALID;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_LEAP59;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_LEAP61;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TIME_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_FREQUENCY_TRACEABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_PTP_TIMESCALE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_TIME_SOURCE;
	vtype=YANG_VTYPE_IDENTITYREF;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0009_ieee1588-ptp/ptp/instances/instance/performance-monitoring-ds/record-list
	aps[4] = IEEE1588_PTP_PERFORMANCE_MONITORING_DS;
	aps[5] = IEEE1588_PTP_RECORD_LIST;
	aps[6] = IEEE1588_PTP_MEASUREMENT_VALID;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_PERIOD_COMPLETE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_PM_TIME;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_AVERAGE_MASTER_SLAVE_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MINIMUM_MASTER_SLAVE_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MAXIMUM_MASTER_SLAVE_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_STDDEV_MASTER_SLAVE_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_AVERAGE_SLAVE_MASTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MINIMUM_SLAVE_MASTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MAXIMUM_SLAVE_MASTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_STDDEV_SLAVE_MASTER_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_AVERAGE_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MINIMUM_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MAXIMUM_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_STDDEV_MEAN_PATH_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_AVERAGE_OFFSET_FROM_MASTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MINIMUM_OFFSET_FROM_MASTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_MAXIMUM_OFFSET_FROM_MASTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_STDDEV_OFFSET_FROM_MASTER;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_VALUEKEY;
	aps[7] = IEEE1588_PTP_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0010_ieee1588-ptp/ptp/instances/instance/performance-monitoring-ds
	aps[0] = IEEE1588_PTP_RW;
	aps[5] = IEEE1588_PTP_ENABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0011_ieee1588-ptp/ptp/instances/instance/ports/port/port-ds/port-identity
	aps[0] = IEEE1588_PTP_RO;
	aps[4] = IEEE1588_PTP_PORTS;
	aps[5] = IEEE1588_PTP_PORT;
	aps[6] = IEEE1588_PTP_PORT_DS;
	aps[7] = IEEE1588_PTP_PORT_IDENTITY;
	aps[8] = IEEE1588_PTP_CLOCK_IDENTITY;
	vtype=YANG_VTYPE_CLOCK_IDENTITY;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_PORT_NUMBER;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[6] = IEEE1588_PTP_VALUEKEY;
	aps[7] = IEEE1588_PTP_PORT_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0012_ieee1588-ptp/ptp/instances/instance/ports/port/port-ds
	aps[0] = IEEE1588_PTP_RO;
	aps[6] = IEEE1588_PTP_PORT_DS;
	aps[7] = IEEE1588_PTP_PORT_STATE;
	vtype=YANG_VTYPE_PORT_STATE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_LOG_MIN_DELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_ANNOUNCE_RECEIPT_TIMEOUT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_DELAY_MECHANISM;
	vtype=YANG_VTYPE_DELAY_MECHANISM;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_LOG_MIN_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_VERSION_NUMBER;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_MINOR_VERSION_NUMBER;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_DELAY_ASYMMETRY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_PORT_ENABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_IS_MEASURING_DELAY;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_AS_CAPABLE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_MEAN_LINK_DELAY_THRESH;
	vtype=YANG_VTYPE_PTP_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_NEIGHBOR_RATE_RATIO;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_INITIAL_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_CURRENT_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_USE_MGT_LOG_ANNOUNCE_INTERVAL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_INITIAL_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_CURRENT_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_USE_MGT_LOG_SYNC_INTERVAL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_SYNC_RECEIPT_TIMEOUT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_SYNC_RECEIPT_TIMEOUT_INTERVAL;
	vtype=YANG_VTYPE_USCALED_NS;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_INITIAL_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_CURRENT_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_USE_MGT_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_MGT_LOG_PDELAY_REQ_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_CURRENT_LOG_GPTP_CAP_INTERVAL;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_USE_MGT_COMPUTE_RATE_RATIO;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_USE_MGT_COMPUTE_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_ALLOWED_LOST_RESPONSES;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_ALLOWED_FAULTS;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_GPTP_CAP_RECEIPT_TIMEOUT;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_ONE_STEP_RECEIVE;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_ONE_STEP_TRANSMIT;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_INITIAL_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_CURRENT_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_USE_MGT_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_BOOLEAN;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_MGT_ONE_STEP_TX_OPER;
	vtype=YANG_VTYPE_INT8;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_PDELAY_TRUNCATED_TIMESTAMPS;
	vtype=YANG_VTYPE_UINT64;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0013_ieee1588-ptp/ptp/instances/instance/ports/port/performance-monitoring-port-ds/record-list-peer-delay
	aps[6] = IEEE1588_PTP_PERFORMANCE_MONITORING_PORT_DS;
	aps[7] = IEEE1588_PTP_RECORD_LIST_PEER_DELAY;
	aps[8] = IEEE1588_PTP_PM_TIME;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_AVERAGE_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_MIN_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_MAX_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_STDDEV_MEAN_LINK_DELAY;
	vtype=YANG_VTYPE_TIME_INTERVAL;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_VALUEKEY;
	aps[9] = IEEE1588_PTP_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0014_ieee1588-ptp/ptp/instances/instance/ports/port/performance-monitoring-port-ds/record-list
	aps[7] = IEEE1588_PTP_RECORD_LIST;
	aps[8] = IEEE1588_PTP_PM_TIME;
	vtype=YANG_VTYPE_YANG_TIMESTAMP;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_ANNOUNCE_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_ANNOUNCE_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_ANNOUNCE_FOREIGN_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_SYNC_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_SYNC_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_FOLLOW_UP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_FOLLOW_UP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_DELAY_REQ_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_DELAY_REQ_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_DELAY_RESP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_DELAY_RESP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_PDELAY_REQ_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_PDELAY_REQ_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_PDELAY_RESP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_PDELAY_RESP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_PDELAY_RESP_FOLLOW_UP_TX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_PDELAY_RESP_FOLLOW_UP_RX;
	vtype=YANG_VTYPE_YANG_ZERO_BASED_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 11, &vtype, 1)!=0){goto erexit;}
	aps[8] = IEEE1588_PTP_VALUEKEY;
	aps[9] = IEEE1588_PTP_INDEX;
	vtype=YANG_VTYPE_UINT16;
	if(uc_dbal_create(dbald, apsd, 12, &vtype, 1)!=0){goto erexit;}
	//0015_ieee1588-ptp/ptp/instances/instance/ports/port/external-port-config-port-ds
	//0016_ieee1588-ptp/ptp/instances/instance/ports/port/external-port-config-port-ds
	aps[0] = IEEE1588_PTP_RW;
	aps[6] = IEEE1588_PTP_EXTERNAL_PORT_CONFIG_PORT_DS;
	aps[7] = IEEE1588_PTP_DESIRED_STATE;
	vtype=YANG_VTYPE_PORT_STATE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0017_ieee1588-ptp/ptp/instances/instance/ports/port/port-statistics-ds
	aps[0] = IEEE1588_PTP_RO;
	aps[6] = IEEE1588_PTP_PORT_STATISTICS_DS;
	aps[7] = IEEE1588_PTP_RX_SYNC_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_RX_ONE_STEP_SYNC_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_RX_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_RX_PDELAY_REQ_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_RX_PDELAY_RESP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_RX_PDELAY_RESP_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_RX_ANNOUNCE_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TX_SYNC_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TX_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TX_PDELAY_REQ_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TX_PDELAY_RESP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TX_PDELAY_RESP_FOLLOW_UP_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_TX_ANNOUNCE_COUNT;
	vtype=YANG_VTYPE_YANG_COUNTER32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0018_ieee1588-ptp/ptp/instances/instance/ports/port/port-clock
	aps[6] = IEEE1588_PTP_PORT_CLOCK;
	aps[7] = IEEE1588_PTP_ADJRATE;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0019_ieee1588-ptp/ptp/instances/instance/ports/port
	aps[0] = IEEE1588_PTP_RW;
	aps[6] = IEEE1588_PTP_UNDERLYING_INTERFACE;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0020_ieee1588-ptp/ptp/instances/instance/clock-state
	//0021_ieee1588-ptp/ptp/instances/instance/clock-state
	aps[0] = IEEE1588_PTP_RO;
	aps[4] = IEEE1588_PTP_CLOCK_STATE;
	aps[5] = IEEE1588_PTP_GMSTATE;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0022_ieee1588-ptp/ptp/instances/instance
	//0023_ieee1588-ptp/ptp/instance-domain-map
	//0024_ieee1588-ptp/ptp
	aps[0] = IEEE1588_PTP_RW;
	aps[2] = IEEE1588_PTP_INSTANCE_DOMAIN_MAP;
	vtype=YANG_VTYPE_HEXUINT16;
	if(uc_dbal_create(dbald, apsd, 5, &vtype, 1)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[1] = IEEE1588_PTP_VALUEKEY;
	vtype=YANG_VTYPE_UINT8;
	if(uc_dbal_create(dbald, apsd, 4, &vtype, 1)!=0){goto erexit;}
	res=0;
erexit:
	return res;
}
