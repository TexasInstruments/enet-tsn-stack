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
#include <tsn_unibase/unibase.h>
#include "mdeth.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpnet.h"
#include "gptpclock.h"
#include "gptpcommon.h"
#include <math.h>

/*
 * in the current implementation domainNumver=domainIndex
 * When a different number is used, we need a mapping funtion here
 */
uint8_t md_domain_index2number(uint8_t index)
{
	return index;
}
uint8_t md_domain_number2index(uint8_t number)
{
	return number;
}

/*
 * in the current implementation portNumver=portIndex
 * When a different number is used, we need a mapping funtion here
 */
uint16_t md_port_index2number(uint16_t index)
{
	return index;
}
uint16_t md_port_number2index(uint16_t number)
{
	return number;
}

void md_compose_head(PTPMsgHeader *head, uint8_t *phead)
{
	uint16_t sd;
	uint64_t ld;
	phead[MDPTPMSG_MAJORSDOID_MESSAGETYPE] = (head->majorSdoId << 4) | head->messageType;
	phead[MDPTPMSG_MINORVERSIONPTP_VERSIONPTP] = (head->minorVersionPTP << 4) |
		head->versionPTP;
	sd=htons(head->messageLength);
	(void)memcpy(&phead[MDPTPMSG_MESSAGELENGTH_NS], &sd, 2);
	phead[MDPTPMSG_DOMAINNUMBER] = md_domain_index2number(head->domainIndex);
	phead[MDPTPMSG_MINORSDOID] = head->minorSdoId;
	(void)memcpy(&phead[MDPTPMSG_FLAGS], head->flags, 2);
	ld=UB_HTONLL((uint64_t)head->correctionField);
	(void)memcpy(&phead[MDPTPMSG_CORRECTIONFIELD_NLL], &ld, 8);
	(void)memcpy(&phead[MDPTPMSG_MESSAGETYPESPECIFIC], head->messageTypeSpecific, 4);
	(void)memcpy(&phead[MDPTPMSG_SOURCEPORTIDENTITY], head->sourcePortIdentity.clockIdentity, 8);
	sd=htons(md_port_index2number(head->sourcePortIdentity.portIndex));
	(void)memcpy(&phead[MDPTPMSG_SOURCEPORTIDENTITY+8], &sd, 2);
	sd=htons(head->sequenceId);
	(void)memcpy(&phead[MDPTPMSG_SEQUENCEID_NS], &sd, 2);
	phead[MDPTPMSG_CONTROL] = head->control;
	phead[MDPTPMSG_LOGMESSAGEINTERVAL] = head->logMessageInterval;
}

void md_decompose_head(MDPTPMsgHeader *phead, PTPMsgHeader *head)
{
	head->majorSdoId = (phead->majorSdoId_messageType >> 4u) & 0xfu;
	head->messageType = phead->majorSdoId_messageType & 0xfu;
	head->minorVersionPTP = (phead->minorVersionPTP_versionPTP >> 4u) & 0xfu;
	head->versionPTP = phead->minorVersionPTP_versionPTP & 0xfu;
	head->messageLength = ntohs(phead->messageLength_ns);
	head->domainIndex = md_domain_number2index(phead->domainNumber);
	head->minorSdoId = phead->minorSdoId;
	(void)memcpy(head->flags, phead->flags, 2);
	head->correctionField = UB_NTOHLL((uint64_t)phead->correctionField_nll);
	(void)memcpy(head->messageTypeSpecific, phead->messageTypeSpecific, 4);
	(void)memcpy(head->sourcePortIdentity.clockIdentity,
		     phead->sourcePortIdentity.clockIdentity, sizeof(ClockIdentity));
	head->sourcePortIdentity.portIndex =
		md_port_number2index(ntohs(phead->sourcePortIdentity.portNumber_ns));
	head->sequenceId = ntohs(phead->sequenceId_ns);
	head->control = phead->control;
	head->logMessageInterval = phead->logMessageInterval;
}

void md_header_template(uint8_t gptpInstanceIndex, int portIndex, PTPMsgHeader *head,
			PTPMsgType msgtype, uint16_t len,
			PortIdentity *portId, uint16_t seqid, int8_t logMessageInterval)
{
	head->majorSdoId=1; // for CMLDS, this must be changed to '2'
	head->messageType=msgtype;
	head->minorVersionPTP=gptpgcfg_get_yang_portds_intitem(
		gptpInstanceIndex, IEEE1588_PTP_TT_MINOR_VERSION_NUMBER,
		portIndex, 0, YDBI_STATUS); // use the data of port=0,domain=0
	head->versionPTP=2;
	head->messageLength=len;
	head->domainIndex=0;
	head->minorSdoId=0;
	switch(msgtype){
		case SYNC: /* fall-through */
		case PDELAY_RESP:
			/* twoStepFlag bit is TRUE for Sync,Pdelay_Resp */
			head->flags[0]=0x2;
			break;
		default:
			head->flags[0]=0x0;
			break;
	}
	// IEEE1588_PTP_TT_PTP_TIMESCALE
	head->flags[1]=0x0u|(((uint32_t)gptpgcfg_get_yang_intitem(
				     gptpInstanceIndex, IEEE1588_PTP_TT_DEFAULT_DS,
				     IEEE1588_PTP_TT_PTP_TIMESCALE, 255,
				     0, YDBI_STATUS)&0x1u)<<3u);
	head->correctionField=0;
	(void)memset(head->messageTypeSpecific,0,4);
	(void)memcpy(&head->sourcePortIdentity, portId, sizeof(PortIdentity));
	head->sequenceId=seqid;
	switch(msgtype){
	case SYNC:
		head->control=0x0;
		break;
	case FOLLOW_UP:
		head->control=0x2;
		break;
	default:
		head->control=0x5;
		break;
	}
	head->logMessageInterval=logMessageInterval;
}

void *md_header_compose(uint8_t gptpInstanceIndex, gptpnet_data_t *gpnetd,
			int portIndex, PTPMsgType msgtype,
			uint16_t ssize, ClockIdentity thisClock, uint16_t thisPort,
			uint16_t seqid, int8_t logMessageInterval)
{
	PortIdentity portId;
	PTPMsgHeader head;
	uint8_t *sdata;

	if(!gpnetd) {
		UB_LOG(UBL_INFO, "%s:no gpnetd\n",__func__);
		return NULL;
	}
	sdata=gptpnet_get_sendbuf(gpnetd, portIndex-1);
	(void)memset(sdata, 0, ssize);

	(void)memcpy(portId.clockIdentity, thisClock, sizeof(ClockIdentity));
	portId.portIndex = thisPort;
	md_header_template(gptpInstanceIndex, thisPort, &head, msgtype,
			   ssize, &portId, seqid, logMessageInterval);
	md_compose_head(&head, sdata);
	return sdata;
}

void md_followup_information_tlv_compose(uint8_t *tlv,
					 double rateRatio, uint16_t gmTimeBaseIndicator,
					 ScaledNs lastGmPhaseChange, double lastGmFreqChange)
{
	uint16_t sd;
	uint32_t ud;
	uint64_t ld;

	// 11.4.4.3 Follow_Up information TLV
	sd=htons(0x3);
	(void)memcpy(&tlv[MDFOLLOWUPTLV_TLVTYPE_NS], &sd, 2);
	sd=htons(28);
	(void)memcpy(&tlv[MDFOLLOWUPTLV_LENGTHFIELD_NS], &sd, 2);
	tlv[MDFOLLOWUPTLV_ORGANIZATIONID+0] = 0x00;
	tlv[MDFOLLOWUPTLV_ORGANIZATIONID+1] = 0x80;
	tlv[MDFOLLOWUPTLV_ORGANIZATIONID+2] = 0xC2;

	tlv[MDFOLLOWUPTLV_ORGANIZATIONSUBTYPE_NB+0] = 0;
	tlv[MDFOLLOWUPTLV_ORGANIZATIONSUBTYPE_NB+1] = 0;
	tlv[MDFOLLOWUPTLV_ORGANIZATIONSUBTYPE_NB+2] = 1;

	ud=htonl((int32_t)ldexp((rateRatio - 1.0), 41));
	(void)memcpy(&tlv[MDFOLLOWUPTLV_CUMULATIVESCALEDRATEOFFSET_NL], &ud, 4);
	sd=htons(gmTimeBaseIndicator);
	(void)memcpy(&tlv[MDFOLLOWUPTLV_GMTIMEBASEINDICATOR_NS], &sd, 2);
	sd=htons(lastGmPhaseChange.nsec_msb);
	(void)memcpy(&tlv[MDFOLLOWUPTLV_LASTGMPHASECHANGE], &sd, 2);
	ld=UB_HTONLL((uint64_t)lastGmPhaseChange.nsec);
	(void)memcpy(&tlv[MDFOLLOWUPTLV_LASTGMPHASECHANGE+2], &ld, 4);
	sd=htons(lastGmPhaseChange.subns);
	(void)memcpy(&tlv[MDFOLLOWUPTLV_LASTGMPHASECHANGE+6], &sd, 2);
	ud=htonl((int32_t)ldexp(lastGmFreqChange, 41));
	(void)memcpy(&tlv[MDFOLLOWUPTLV_SCALEDLASTGMFREQCHANGE_NL], &ud, 4);
}

void md_entity_glb_init(uint8_t gptpInstanceIndex, MDEntityGlobal **mdeglb,
			MDEntityGlobalForAllDomain *forAllDomain, uint16_t portIndex)
{
	if(!*mdeglb){
		*mdeglb=(MDEntityGlobal *)UB_SD_GETMEM(GPTP_SMALL_ALLOC, sizeof(MDEntityGlobal));
		if(ub_assert_fatal(*mdeglb, __func__, "malloc error")){return;}
	}
	(void)memset(*mdeglb, 0, sizeof(MDEntityGlobal));
	if(forAllDomain!=NULL){
		(*mdeglb)->forAllDomain = forAllDomain;
	}else{
		(*mdeglb)->forAllDomain =
			(MDEntityGlobalForAllDomain*)UB_SD_GETMEM(GPTP_MEDIUM_ALLOC,
								  sizeof(MDEntityGlobalForAllDomain));
		if(ub_assert_fatal((*mdeglb)->forAllDomain, __func__, "malloc error")){return;}
		(void)memset((*mdeglb)->forAllDomain, 0, sizeof(MDEntityGlobalForAllDomain));
		(*mdeglb)->forAllDomain->currentLogPdelayReqInterval =
			gptpgcfg_get_yang_portds_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_CURRENT_LOG_PDELAY_REQ_INTERVAL,
				portIndex, 0, YDBI_STATUS);
		(*mdeglb)->forAllDomain->initialLogPdelayReqInterval =
			gptpgcfg_get_yang_portds_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_INITIAL_LOG_PDELAY_REQ_INTERVAL,
				portIndex, 0, YDBI_CONFIG);
		(*mdeglb)->forAllDomain->pdelayReqInterval.nsec =
			LOG_TO_NSEC((*mdeglb)->forAllDomain->initialLogPdelayReqInterval);
		(*mdeglb)->forAllDomain->allowedLostResponses =
			gptpgcfg_get_yang_portds_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_ALLOWED_LOST_RESPONSES,
				portIndex, 0, YDBI_CONFIG);
		(*mdeglb)->forAllDomain->allowedFaults =
			gptpgcfg_get_yang_portds_intitem(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_ALLOWED_FAULTS,
				portIndex, 0, YDBI_CONFIG);
		(*mdeglb)->forAllDomain->neighborPropDelayThresh.nsec =
			((uint64_t)gptpgcfg_get_yang_portds_int64item(
				gptpInstanceIndex,
				IEEE1588_PTP_TT_MEAN_LINK_DELAY_THRESH,
				portIndex, 0, YDBI_CONFIG)>>16u);
		(*mdeglb)->forAllDomain->neighborPropDelayMinLimit.nsec =
			gptpgcfg_get_intitem(
				gptpInstanceIndex,
				XL4_EXTMOD_XL4GPTP_NEIGHBOR_PROPDELAY_MINLIMIT,
				YDBI_CONFIG);
	}
	(*mdeglb)->syncSequenceId = (uint16_t)rand();
	(*mdeglb)->oneStepReceive = false;
	(*mdeglb)->oneStepTransmit = false;
	(*mdeglb)->oneStepTxOper = false;
}

void md_entity_glb_close(MDEntityGlobal **mdeglb, int domainIndex)
{
	if(!*mdeglb){return;}
	if(domainIndex==0){UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, (*mdeglb)->forAllDomain);}
	UB_SD_RELMEM(GPTP_SMALL_ALLOC, *mdeglb);
	*mdeglb=NULL;
}
