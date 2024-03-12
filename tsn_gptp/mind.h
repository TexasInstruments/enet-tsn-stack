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
#ifndef MIND_H_
#define MIND_H_

#include <tsn_uniconf/yangs/yang_db_access.h>
#include "gptpbasetypes.h"
#include <stdint.h>

#define NUMBER_OF_PORTS 2

typedef struct PTPMsgAnnounce PTPMsgAnnounce;
typedef struct PTPMsgIntervalRequestTLV PTPMsgIntervalRequestTLV;
typedef struct PTPMsgGPTPCapableTLV PTPMsgGPTPCapableTLV;
typedef struct PerPortGlobal PerPortGlobal;

// 10.2 Time-synchronization state machines
// 10.2.2.1 MDSyncSend
typedef struct MDSyncSend {
	uint8_t domainIndex;
	ScaledNs followUpCorrectionField;
	PortIdentity sourcePortIdentity;
	int8_t logMessageInterval;
	Timestamp preciseOriginTimestamp;
	UScaledNs upstreamTxTime;
	double rateRatio;
	uint16_t gmTimeBaseIndicator;
	ScaledNs lastGmPhaseChange;
	double lastGmFreqChange;
} MDSyncSend;

// 10.2.2.2 MDSyncReceive
typedef struct MDSyncReceive {
	uint8_t domainIndex;
	ScaledNs followUpCorrectionField;
	PortIdentity sourcePortIdentity;
	int8_t logMessageInterval;
	Timestamp preciseOriginTimestamp;
	UScaledNs upstreamTxTime;
	double rateRatio;
	uint16_t gmTimeBaseIndicator;
	ScaledNs lastGmPhaseChange;
	double lastGmFreqChange;
	uint16_t seqid;
} MDSyncReceive;

// 10.2.2.3 PortSyncSync
typedef struct PortSyncSync {
	uint8_t domainIndex;
	int16_t localPortIndex;
	UScaledNs syncReceiptTimeoutTime;
	ScaledNs followUpCorrectionField;
	PortIdentity sourcePortIdentity;
	int8_t logMessageInterval;
	Timestamp preciseOriginTimestamp;
	UScaledNs upstreamTxTime;
	double rateRatio;
	uint16_t gmTimeBaseIndicator;
	ScaledNs lastGmPhaseChange;
	double lastGmFreqChange;
	// Next send time relative to current time
	UScaledNs syncNextSendTimeoutTime;
	uint16_t lastSyncSeqID;
	PerPortGlobal *local_ppg; // per-port-global for the localPort
} PortSyncSync;

// IEEE1588-2019 J.4.1 Performance Monitoring
typedef uint32_t PMTimestamp; // ms unit
#define PERFMON_SHORTINTV_DR 0 // short interval, typically 15m
#define PERFMON_LONGINTV_DR 1 // long interval, typically 24h
#define PERFMON_ALL_DR 2  // keep current (index=0) data records only

// IEEE1588-2019 Tbl. J.1 and J.2 invalid value
#define INVALID_TIMEINTERVAL_VALUE 0x7FFFFFFFFFFFFFFF

// Ancillary data required to compute parameters on standard Data Records
typedef struct PerfMonAncillaryDataRecord {
	uint32_t N; // number of entries
	int64_t prev_M; // previous mean
	int64_t M; // running mean
	int64_t prev_V; // previous variance
	int64_t V; //  running variance
} PerfMonAncillaryDataRecord;

// IEEE1588-2019 Tbl J.1 ClockPerfMonDataRecord
typedef struct ClockPerfMonDataRecord {
	// uint16_t index;
	// PMTimestamp PMTime;
	bool measurementValid;
	bool periodComplete;
	TimeInterval averageMasterSlaveDelay;
	TimeInterval minMasterSlaveDelay;
	TimeInterval maxMasterSlaveDelay;
	TimeInterval stdDevMasterSlaveDelay;
	TimeInterval averageSlaveMasterDelay;
	TimeInterval minSlaveMasterDelay;
	TimeInterval maxSlaveMasterDelay;
	TimeInterval stdDevSlaveMasterDelay;
	TimeInterval averageMeanPathDelay;
	TimeInterval minMeanPathDelay;
	TimeInterval maxMeanPathDelay;
	TimeInterval stdDevMeanPathDelay;
	TimeInterval averageOffsetFromMaster;
	TimeInterval minOffsetFromMaster;
	TimeInterval maxOffsetFromMaster;
	TimeInterval stdDevOffsetFromMaster;
	// ancillary structure members (not in standards)
	PerfMonAncillaryDataRecord masterSlaveDelay;
	PerfMonAncillaryDataRecord slaveMasterDelay;
	PerfMonAncillaryDataRecord meanPathDelay;
	PerfMonAncillaryDataRecord offsetFromMaster;
} ClockPerfMonDataRecord;

// IEEE1588-2019 Tbl J.2 PortPerformanceMonitoringPeerDelayDataRecord
typedef struct PortPerfMonPeerDelayDataRecord {
	// uint16_t index;
	// PMTimestamp PMTime;
	TimeInterval averageMeanLinkDelay;
	TimeInterval minMeanLinkDelay;
	TimeInterval maxMeanLinkDelay;
	TimeInterval stdDevMeanLinkDelay;
	// ancillary structure members (not in standards)
	PerfMonAncillaryDataRecord meanLinkDelay;
} PortPerfMonPeerDelayDataRecord;

// IEEE1588-2019 Tbl J.3 PortPerformanceMonitoringDataRecord
typedef struct PortPerfMonDataRecord {
	// uint16_t index;
	// PMTimestamp PMTime;
	uint32_t announceTx;
	uint32_t announceRx;
	uint32_t announceForeignMasterRx;
	uint32_t syncTx;
	uint32_t syncRx;
	uint32_t followUpTx;
	uint32_t followUpRx;
	uint32_t delayReqTx;
	uint32_t delayReqRx;
	uint32_t delayRespTx;
	uint32_t delayRespRx;
	uint32_t pDelayReqTx;
	uint32_t pDelayReqRx;
	uint32_t pDelayRespTx;
	uint32_t pDelayRespRx;
	uint32_t pDelayRespFollowUpTx;
	uint32_t pDelayRespFollowUpRx;
} PortPerfMonDataRecord;

// IEEE1588-2019 Excelfore Additions
typedef struct PortPerfMonSignalingDataRecord {
	// uint16_t index;
	// PMTimestamp PMTime;
	uint32_t msgIntervalTx;
	uint32_t msgIntervalRx;
	uint32_t asCapableTx;
	uint32_t asCapableRx;
	uint32_t signalingRx;
} PortPerfMonSignalingDataRecord;

// IEEE1588-2019 J.5 Data Sets for Performance Monitoring
// As per the standard each Data Record should have their own index and PMTime,
// for simplicity, we operate on per port where each per port data record shares
// the same index and same PMTime.
// Moreover, tsn_gptp keeps the current data record only (index=0). Historical data
// are not kept in application, they should be stored in database (e.g. uniconf)
// instead.
typedef struct PerfMonPortDS {
	PMTimestamp lastUpdate[PERFMON_ALL_DR]; // last update time of current
	uint16_t index[PERFMON_ALL_DR];
	PMTimestamp PMTime[PERFMON_ALL_DR];
	PortPerfMonPeerDelayDataRecord pdelayDR[PERFMON_ALL_DR];
	PortPerfMonDataRecord portDR[PERFMON_ALL_DR];
	PortPerfMonSignalingDataRecord signalingDR[PERFMON_ALL_DR];
} PerfMonPortDS;
typedef struct PerfMonClockDS {
	PMTimestamp lastUpdate[PERFMON_ALL_DR]; // last update time of current
	uint16_t index[PERFMON_ALL_DR];
	PMTimestamp PMTime[PERFMON_ALL_DR];
	ClockPerfMonDataRecord clockDR[PERFMON_ALL_DR];
} PerfMonClockDS;

// 10.2.3 Per-time-aware-system global variables
typedef struct PerTimeAwareSystemGlobal {
	bool BEGIN;
	UScaledNs clockMasterSyncInterval;
	ExtendedTimestamp clockSlaveTime;
	ExtendedTimestamp syncReceiptTime;
	UScaledNs syncReceiptLocalTime;
	double clockSourceFreqOffset;
	ScaledNs clockSourcePhaseOffset;
	uint16_t clockSourceTimeBaseIndicator;
	uint16_t clockSourceTimeBaseIndicatorOld;
	ScaledNs clockSourceLastGmPhaseChange;
	double clockSourceLastGmFreqChange;
	//UScaledNs currentTime; //we use gptpclock of (clockIndex,domainIndex)
	bool gmPresent;
	double gmRateRatio;
	uint16_t gmTimeBaseIndicator;
	ScaledNs lastGmPhaseChange;
	double lastGmFreqChange;
	TimeInterval localClockTickInterval;
	//UScaledNs localTime;  //we use gptpclock of (clockIndex,domainIndex)
	Enumeration2 selectedState[XL4_DATA_ABS_MAX_NETDEVS];
	//ExtendedTimestamp masterTime; //we use gptpclock of (clockIndex=0,domainIndex)
	ClockIdentity thisClock;
	int8_t parentLogSyncInterval;
	bool instanceEnable;
	uint8_t domainIndex;
	int thisClockIndex; // index of the gptpclock entity of 'thisClock'
	int8_t clockMasterLogSyncInterval;
	bool gm_stable_initdone;
	bool asCapableOrAll;
	uint16_t lastSyncSeqID;
	ClockIdentity gmIdentity;
	// Flag to determine if AVNU is followed over 802.1AS
	bool conformToAvnu;
	uint8_t gptpInstanceIndex; // this is the same data as the one in gptpman_data

	// IEEE1588-2019 J.5 Data set for performance monitoring, per PTP instance data set
	PerfMonClockDS *perfmonClockDS;
	// IEEE1588-2019 J.4 Data collection periodicity
	bool perfmonEnable; // perform performance-monitoring
	uint32_t perfmonShortPeriod_ms; // 15m as per standards
	uint32_t perfmonLongPeriod_ms; // 24h as per standards
	uint32_t perfmonCurrentPeriod_ms; // updates current short and long data records
} PerTimeAwareSystemGlobal;

// 10.2.4 Per-port global variables
typedef struct PerPortGlobalForAllDomain {
	bool asymmetryMeasurementMode;
	double neighborRateRatio;
	UScaledNs neighborPropDelay;
	UScaledNs delayAsymmetry;
	bool computeNeighborRateRatio;
	bool computeNeighborPropDelay;
	bool portOper;
	bool useMgtSettableLogAnnounceInterval;
	int8_t mgtSettableLogAnnounceInterval;
	//14.8.25 useMgtSettableLogPdelayReqInterval
	bool useMgtSettableLogPdelayReqInterval;
	int8_t mgtSettableLogPdelayReqInterval;
	//when PdelayReq comes in CMLDS mode(SdoId=0x200), set this
	int8_t receivedNonCMLDSPdelayReq;
}PerPortGlobalForAllDomain;
struct PerPortGlobal {
	bool asCapable; // for domain!=0, somebody needs to set this
	UScaledNs syncReceiptTimeoutTimeInterval;
	int8_t currentLogSyncInterval;
	int8_t initialLogSyncInterval;
	UScaledNs syncInterval;
	bool ptpPortEnabled;
	uint16_t thisPort;
	bool syncLocked;
	bool neighborGptpCapable;
	bool syncSlowdown;
	UScaledNs oldAnnounceInterval;
	PerPortGlobalForAllDomain *forAllDomain;
	// 10.7.3.1 syncReceiptTimeout
	int8_t syncReceiptTimeout;
	// 10.4.1 gPtpCapableTransmit state machine needs this value
	int8_t logGptpCapableMessageInterval;
	// 10.7.3.3 gPtpCapableReceiptTimeout
	int8_t gPtpCapableReceiptTimeout;
	// 14.8.19 useMgtSettableLogSyncInterval
	bool useMgtSettableLogSyncInterval;
	// 14.8.20 mgtSettableLogSyncInterval
	int8_t mgtSettableLogSyncInterval;
	// 14.8.38 initialOneStepTxOper
	bool initialOneStepTxOper;
	// 14.8.39 currentOneStepTxOper
	bool currentOneStepTxOper;
	// 14.8.40 useMgtSettableOneStepTxOper
	bool useMgtSettableOneStepTxOper;
	// 14.8.41 mgtSettableOneStepTxOper
	bool mgtSettableOneStepTxOper;

	// IEEE1588-2019 J.5 Data set for performance monitoring
	PerfMonPortDS *perfmonDS;
};

// 10.2.6 SiteSyncSync state machine
typedef struct SiteSyncSyncSM {
	bool rcvdPSSync;
	PortSyncSync *rcvdPSSyncPtr;
	// 802.1AS-2020 10.2.7.1.3 txPSSyncPtrSSS
	PortSyncSync *txPSSyncPtrSSS;
} SiteSyncSyncSM;

// 10.2.7 PortSyncSyncReceive state machine
typedef struct PortSyncSyncReceiveSM {
	bool rcvdMDSync;
	MDSyncReceive *rcvdMDSyncPtr;
	PortSyncSync *txPSSyncPtr;
	double rateRatio;
} PortSyncSyncReceiveSM;

// 10.2.8 ClockMasterSyncSend state machine
typedef struct ClockMasterSyncSendSM {
	UScaledNs syncSendTime;
	PortSyncSync *txPSSyncPtr;
} ClockMasterSyncSendSM;

// 10.2.9 ClockMasterSyncOffset state machine
typedef struct ClockMasterSyncOffsetSM {
	bool rcvdSyncReceiptTime;
} ClockMasterSyncOffsetSM;

// 10.2.10 ClockMasterSyncReceive state machine
typedef struct ClockMasterSyncReceiveSM {
	bool rcvdClockSourceReq;
	// a pointer to the received ClockSourceTime.invoke function parameters.
	void *rcvdClockSourceReqPtr;
	bool rcvdLocalClockTick;
} ClockMasterSyncReceiveSM;

// 10.2.11 PortSyncSyncSend state machine
typedef struct PortSyncSyncSendSM {
	bool rcvdPSSync;
	PortSyncSync *rcvdPSSyncPtr;
	Timestamp lastPreciseOriginTimestamp;
	ScaledNs lastFollowUpCorrectionField;
	double lastRateRatio;
	UScaledNs lastUpstreamTxTime;
	UScaledNs lastSyncSentTime;
	uint16_t lastRcvdPortNum;
	uint16_t lastGmTimeBaseIndicator;
	ScaledNs lastGmPhaseChange;
	double lastGmFreqChange;
	MDSyncSend *txMDSyncSendPtr;
	UScaledNs syncReceiptTimeoutTime;
	uint8_t numberSyncTransmissions;
	UScaledNs interval1;
} PortSyncSyncSendSM;

// 10.2.12 ClockSlaveSync state machine
typedef struct ClockSlaveSyncSM {
	bool rcvdPSSync;
	bool rcvdLocalClockTick;
	PortSyncSync *rcvdPSSyncPtr;
} ClockSlaveSyncSM;


// 10.3 Best master clock selection
// 10.3.8 Per-time-aware-system global variables
typedef struct BmcsPerTimeAwareSystemGlobal {
	bool reselect[XL4_DATA_ABS_MAX_NETDEVS];
	bool selected[XL4_DATA_ABS_MAX_NETDEVS];
	uint16_t masterStepsRemoved;
	bool leap61;
	bool leap59;
	bool currentUtcOffsetValid;
	bool ptpTimescale;
	bool timeTraceable;
	bool frequencyTraceable;
	int16_t currentUtcOffset;
	Enumeration8 timeSource;
	bool sysLeap61;
	bool sysLeap59;
	bool sysCurrentUTCOffsetValid;
	bool sysPtpTimescale;
	bool sysTimeTraceable;
	bool sysFrequencyTraceable;
	int16_t sysCurrentUtcOffset;
	Enumeration8 sysTimeSource;
	UInteger224 systemPriority;
	UInteger224 gmPriority;
	UInteger224 lastGmPriority;
	// Addition of pathTrace length
	uint8_t pathTraceCount;
	ClockIdentity pathTrace[MAX_PATH_TRACE_N];
	Enumeration8 externalPortConfiguration;
	uint32_t lastAnnouncePort;
	int bmcsQuickUpdate;
} BmcsPerTimeAwareSystemGlobal;

// 10.3.9 Per-port global variables
typedef struct BmcsPerPortGlobal {
	UScaledNs announceReceiptTimeoutTimeInterval;
	bool announceSlowdown;
	UScaledNs oldAnnounceInterval;
	Enumeration2 infoIs;
	UInteger224 masterPriority;
	int8_t currentLogAnnounceInterval;
	int8_t initialLogAnnounceInterval;
	UScaledNs announceInterval;
	uint16_t messageStepsRemoved;
	bool newInfo;
	UInteger224 portPriority;
	uint16_t portStepsRemoved;
	PTPMsgAnnounce *rcvdAnnouncePtr;
	bool rcvdMsg;
	bool updtInfo;
	bool annLeap61;
	bool annLeap59;
	bool annCurrentUtcOffsetValid;
	bool annPtpTimescale;
	bool annTimeTraceable;
	bool annFrequencyTraceable;
	int16_t annCurrentUtcOffset;
	Enumeration8 annTimeSource;
	// ??? global pathTrace is updated only when portState is known
	// to be SlavePort, in the case when system is grandmaster (no SlavePort)
	// and the Announce received may convey transition of portState to SlavePort
	// a copy of the announce pathSequence should be used for global pathTrace
	uint8_t annPathSequenceCount;
	ClockIdentity annPathSequence[MAX_PATH_TRACE_N];
	// Additional from 10.7.3 Timeouts
	int8_t announceReceiptTimeout;
} BmcsPerPortGlobal;

// 10.3.9.4 infoIs
typedef enum {
        Received,
        Mine,
        Aged,
        Disabled
} BmcsInfoIs;

// 10.3.11.2.1 rcvInfo
typedef enum {
        RepeatedMasterInfo,
        SuperiorMasterInfo,
        InferiorMasterInfo,
        OtherInfo
} BmcsRcvdInfo;

// 10.3.10 PortAnnounceReceive state machine
typedef struct PortAnnounceReceiveSM {
	bool rcvdAnnounce;
} PortAnnounceReceiveSM;

// 10.3.11 PortAnnounceInformation state machine
typedef struct PortAnnounceInformationSM {
	UScaledNs announceReceiptTimeoutTime;
	UInteger224 messagePriority;
	Enumeration2 rcvdInfo;
} PortAnnounceInformationSM;

// 10.3.12 PortStateSelection state machine
typedef struct PortStateSelectionSMforAllDomain {
	bool asymmetryMeasurementModeChange;
} PortStateSelectionSMforAllDomain;
typedef struct PortStateSelectionSM {
	bool  systemIdentityChange;
	PortStateSelectionSMforAllDomain *forAllDomain;
} PortStateSelectionSM;

// 10.3.13 PortAnnounceInformationExt state machine
typedef struct PortAnnounceInformationExtSM {
	bool rcvdAnnounce;
	UInteger224 messagePriority;
} PortAnnounceInformationExtSM;

// 10.3.14 PortStateSettingExt state machine
typedef struct PortStateSettingExtSMforAllDomain {
	bool asymmetryMeasurementModeChangeThisPort;
} PortStateSettingExtSMforAllDomain;
typedef struct PortStateSettingExtSM {
	bool disabledExt;
	bool rcvdPortStateInd;
	Enumeration2 portStateInd;
	PortStateSettingExtSMforAllDomain *forAllDomain;
} PortStateSettingExtSM;

// 10.3.15 PortAnnounceTransmit state machine
typedef struct PortAnnounceTransmitSM {
	UScaledNs announceSendTime;
	uint8_t numberAnnounceTransmissions;
	UScaledNs interval2;
} PortAnnounceTransmitSM;

// 10.3.16 AnnounceIntervalSetting state machine
typedef struct AnnounceIntervalSettingSM {
	bool rcvdSignalingMsg2;
	PTPMsgIntervalRequestTLV *rcvdSignalingPtr;
} AnnounceIntervalSettingSM;

// 10.4 State machines related to signaling gPTP protocol capability
// 10.4.1 gPtpCapableTransmit state machine
typedef struct gPtpCapableTransmitSM {
	UScaledNs signalingMsgTimeInterval;
	UScaledNs intervalTimer;
	PTPMsgGPTPCapableTLV *txSignalingMsgPtr;
} gPtpCapableTransmitSM;

// 10.4.2 gPtpCapableReceive state machine
typedef struct gPtpCapableReceiveSM {
	bool rcvdGptpCapableTlv;
	PTPMsgGPTPCapableTLV *rcvdSignalingMsgPtr;
	UScaledNs gPtpCapableReceiptTimeoutInterval;
	UScaledNs timeoutTime;
} gPtpCapableReceiveSM;

// 10.6.2 Message formats - Header
typedef struct PTPMsgHeader {
	Nibble majorSdoId;
	Enumeration4 messageType;
	UInteger4 minorVersionPTP;
	UInteger4 versionPTP;
	uint16_t messageLength;
	uint8_t domainIndex;
	uint8_t minorSdoId;
	Octet2 flags;
	int64_t correctionField;
	Octet4 messageTypeSpecific;
	PortIdentity sourcePortIdentity;
	uint16_t sequenceId;
	uint8_t control;
	int8_t logMessageInterval;
} PTPMsgHeader;

// 10.6.3 Announce message
struct PTPMsgAnnounce {
	PTPMsgHeader header;
	int16_t currentUtcOffset;
	uint8_t grandmasterPriority1;
	ClockQuality grandmasterClockQuality;
	uint8_t grandmasterPriority2;
	ClockIdentity grandmasterIdentity;
	uint16_t stepsRemoved;
	Enumeration8 timeSource;
	Enumeration16 tlvType;
	uint16_t tlvLength;
	ClockIdentity pathSequence[MAX_PATH_TRACE_N];
};

// 10.6.4.3 Message interval request TLV definition
struct PTPMsgIntervalRequestTLV {
	Enumeration16 tlvType;
	uint16_t lengthField;
	Octet3 organizationId;
	Enumeration24 organizationSubType;
	int8_t linkDelayInterval;
	int8_t timeSyncInterval;
	int8_t announceInterval;
	Octet flags;
};

// 10.6.4.4 gPTP capable TLV
struct PTPMsgGPTPCapableTLV {
	Enumeration16 tlvType;
	uint16_t lengthField;
	Octet3 organizationId;
	Enumeration24 organizationSubType;
	int8_t logGptpCapableMessageInterval;
	Octet flags;
};

// 10.6.4.3.9 flags (Octet)
#define COMPUTE_NEIGHBOR_RATE_RATIO_BIT 0
#define COMPUTE_NEIGHBOR_PROP_DELAY_BIT 1
#define ONE_STEP_RECEIVE_CAPABLE_BIT 2

void ptas_glb_init(PerTimeAwareSystemGlobal **tasglb,
		   uint8_t gptpInstanceIndex, uint8_t domainIndex);
void ptas_glb_close(PerTimeAwareSystemGlobal **tasglb);

void pp_glb_init(uint8_t gptpInstanceIndex, PerPortGlobal **ppglb,
		 PerPortGlobalForAllDomain *forAllDomain,
		 PerTimeAwareSystemGlobal *tasglb,
		 uint8_t domainIndex, uint16_t portIndex);
void pp_glb_close(PerPortGlobal **ppglb, int domainIndex);

void bmcs_ptas_glb_init(uint8_t gptpInstanceIndex, BmcsPerTimeAwareSystemGlobal **btasglb,
                        PerTimeAwareSystemGlobal *ptasglb, uint8_t domainIndex);
void bmcs_ptas_glb_update(uint8_t gptpInstanceIndex,
			  BmcsPerTimeAwareSystemGlobal **btasglb,
			  PerTimeAwareSystemGlobal *ptasglb,
			  uint8_t domainIndex);
void bmcs_ptas_glb_close(BmcsPerTimeAwareSystemGlobal **btasglb);

void bmcs_pp_glb_init(uint8_t gptpInstanceIndex, BmcsPerPortGlobal **bppglb,
		      uint8_t domainIndex, uint8_t portIndex);

void bmcs_pp_glb_close(BmcsPerPortGlobal **bppglb);

#endif
