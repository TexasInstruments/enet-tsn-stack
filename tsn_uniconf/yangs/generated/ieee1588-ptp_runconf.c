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
#include "ietf-netconf-server.h"
#include "ietf-keychain.h"
#include "excelfore-tsn-remote.h"

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
	"ports", 		// 47(0x2f)
	"port", 		// 48(0x30)
	"port-index", 		// 49(0x31)
	"underlying-interface", 		// 50(0x32)
	"port-ds", 		// 51(0x33)
	"port-identity", 		// 52(0x34)
	"port-state", 		// 53(0x35)
	"log-min-delay-req-interval", 		// 54(0x36)
	"mean-link-delay", 		// 55(0x37)
	"log-announce-interval", 		// 56(0x38)
	"announce-receipt-timeout", 		// 57(0x39)
	"log-sync-interval", 		// 58(0x3a)
	"delay-mechanism", 		// 59(0x3b)
	"log-min-pdelay-req-interval", 		// 60(0x3c)
	"version-number", 		// 61(0x3d)
	"minor-version-number", 		// 62(0x3e)
	"delay-asymmetry", 		// 63(0x3f)
	"port-enable", 		// 64(0x40)
	"external-port-config-port-ds", 		// 65(0x41)
	"desired-state", 		// 66(0x42)
	// augmented by ieee802-dot1as-ptp
	"gm-capable",		// 67(0x43)
	"last-gm-phase-change",		// 68(0x44)
	"last-gm-freq-change",		// 69(0x45)
	"gm-timebase-indicator",		// 70(0x46)
	"time-of-last-freq-change",		// 71(0x47)
	"is-measuring-delay",		// 72(0x48)
	"as-capable",		// 73(0x49)
	"mean-link-delay-thresh",		// 74(0x4a)
	"neighbor-rate-ratio",		// 75(0x4b)
	"initial-log-announce-interval",		// 76(0x4c)
	"current-log-announce-interval",		// 77(0x4d)
	"use-mgt-log-announce-interval",		// 78(0x4e)
	"initial-log-sync-interval",		// 79(0x4f)
	"current-log-sync-interval",		// 80(0x50)
	"use-mgt-log-sync-interval",		// 81(0x51)
	"sync-receipt-timeout",		// 82(0x52)
	"sync-receipt-timeout-interval",		// 83(0x53)
	"initial-log-pdelay-req-interval",		// 84(0x54)
	"current-log-pdelay-req-interval",		// 85(0x55)
	"use-mgt-log-pdelay-req-interval",		// 86(0x56)
	"mgt-log-pdelay-req-interval",		// 87(0x57)
	"current-log-gptp-cap-interval",		// 88(0x58)
	"use-mgt-compute-rate-ratio",		// 89(0x59)
	"use-mgt-compute-mean-link-delay",		// 90(0x5a)
	"allowed-lost-responses",		// 91(0x5b)
	"allowed-faults",		// 92(0x5c)
	"gptp-cap-receipt-timeout",		// 93(0x5d)
	"one-step-tx-oper",		// 94(0x5e)
	"one-step-receive",		// 95(0x5f)
	"one-step-transmit",		// 96(0x60)
	"initial-one-step-tx-oper",		// 97(0x61)
	"current-one-step-tx-oper",		// 98(0x62)
	"use-mgt-one-step-tx-oper",		// 99(0x63)
	"mgt-one-step-tx-oper",		// 100(0x64)
	"pdelay-truncated-timestamps",		// 101(0x65)
	"port-statistics-ds",		// 102(0x66)
	"rx-sync-count",		// 103(0x67)
	"rx-one-step-sync-count",		// 104(0x68)
	"rx-follow-up-count",		// 105(0x69)
	"rx-pdelay-req-count",		// 106(0x6a)
	"rx-pdelay-resp-count",		// 107(0x6b)
	"rx-pdelay-resp-follow-up-count",		// 108(0x6c)
	"rx-announce-count",		// 109(0x6d)
	"tx-sync-count",		// 110(0x6e)
	"tx-follow-up-count",		// 111(0x6f)
	"tx-pdelay-req-count",		// 112(0x70)
	"tx-pdelay-resp-count",		// 113(0x71)
	"tx-pdelay-resp-follow-up-count",		// 114(0x72)
	"tx-announce-count",		// 115(0x73)
	// augmented by xl4gptp
	"port-clock",		// 116(0x74)
	"adjrate",		// 117(0x75)
	"clock-state",		// 118(0x76)
	"gmstate",		// 119(0x77)
	"instance-domain-map",		// 120(0x78)
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
	//0009_ieee1588-ptp/ptp/instances/instance/ports/port/port-ds/port-identity
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
	//0010_ieee1588-ptp/ptp/instances/instance/ports/port/port-ds
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
	//0011_ieee1588-ptp/ptp/instances/instance/ports/port/external-port-config-port-ds
	aps[0] = IEEE1588_PTP_RW;
	aps[6] = IEEE1588_PTP_EXTERNAL_PORT_CONFIG_PORT_DS;
	aps[7] = IEEE1588_PTP_DESIRED_STATE;
	vtype=YANG_VTYPE_PORT_STATE;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0012_ieee1588-ptp/ptp/instances/instance/ports/port/port-statistics-ds
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
	//0013_ieee1588-ptp/ptp/instances/instance/ports/port/port-clock
	aps[6] = IEEE1588_PTP_PORT_CLOCK;
	aps[7] = IEEE1588_PTP_ADJRATE;
	vtype=YANG_VTYPE_INT32;
	if(uc_dbal_create(dbald, apsd, 10, &vtype, 1)!=0){goto erexit;}
	//0014_ieee1588-ptp/ptp/instances/instance/ports/port
	aps[0] = IEEE1588_PTP_RW;
	aps[6] = IEEE1588_PTP_UNDERLYING_INTERFACE;
	vtype=YANG_VTYPE_IF_INTERFACE_REF;
	if(uc_dbal_create(dbald, apsd, 9, &vtype, 1)!=0){goto erexit;}
	//0015_ieee1588-ptp/ptp/instances/instance/clock-state
	//0016_ieee1588-ptp/ptp/instances/instance/clock-state
	aps[0] = IEEE1588_PTP_RO;
	aps[4] = IEEE1588_PTP_CLOCK_STATE;
	aps[5] = IEEE1588_PTP_GMSTATE;
	vtype=YANG_VTYPE_ENUMERATION;
	if(uc_dbal_create(dbald, apsd, 8, &vtype, 1)!=0){goto erexit;}
	//0017_ieee1588-ptp/ptp/instances/instance
	//0018_ieee1588-ptp/ptp/instance-domain-map
	//0019_ieee1588-ptp/ptp
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
