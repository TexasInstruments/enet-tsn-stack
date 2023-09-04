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
#ifndef MD_ABNORMAL_HOOKS_H_
#define MD_ABNORMAL_HOOKS_H_

#include "gptpnet.h"

typedef enum {
	MD_ABN_EVENT_NONE, // no event happens
	MD_ABN_EVENT_SKIP, // skip a message, sequenceID has no skip
	MD_ABN_EVENT_DUP, // send the same message twice
	MD_ABN_EVENT_BADSEQN, // add eventpara on sequenceID.
			      // if eventpara==0 invert lower 8 bits of SequenceID
	MD_ABN_EVENT_NOTS, // make No Timestamp error
	MD_ABN_EVENT_SENDER, // make send error
} md_abn_event_type;

typedef struct md_abn_event {
	int domainNumber;
	int ndevIndex;
	PTPMsgType msgtype;
	md_abn_event_type eventtype;
	float eventrate; // 0.0 to 1.0: possibility of happening of the event
	int repeat; // 0:repeat forever, 1:one time, 2:two times, ...
	int interval; // 0:every time, 1: ever other time, 2: ever two times, ...
	int eventpara; // integer parameter for the event
} md_abn_event_t;

typedef enum {
	MD_ABN_EVENTP_NONE,
	MD_ABN_EVENTP_SKIP,
	MD_ABN_EVENTP_DUPLICATE,
	MD_ABN_EVENTP_MANUPULATE,
	MD_ABN_EVENTP_SENDER,
} md_abn_eventp_t;

/**
 * @brief initialize and activate 'md_abnormal_gptpnet_send_hook' operation.
 */
void md_abnormal_init(void);

/**
 * @brief close and deactivate 'md_abnormal_gptpnet_send_hook' operation.
 */
void md_abnormal_close(void);

/**
 * @brief register an abnormal event
 */
int md_abnormal_register_event(md_abn_event_t *event);

/**
 * @brief deregister all of abnormal events
 */
int md_abnormal_deregister_all_events(void);

/**
 * @brief deregister all of abnormal events with 'msgtype'
 */
int md_abnormal_deregister_msgtype_events(PTPMsgType msgtype);

/**
 * @brief inject abnomal send events by calling this function just before 'gptpnet_send'
 * @param ndevIndex	index of a network device
 * @param domainNumber	domain Number
 * @result eventp type
 */
md_abn_eventp_t md_abnormal_gptpnet_send_hook(gptpnet_data_t *gpnet, int ndevIndex,
					      uint16_t length);

static inline int gptpnet_send_whook(gptpnet_data_t *gpnet, int ndevIndex, uint16_t length)
{
	switch(md_abnormal_gptpnet_send_hook(gpnet, ndevIndex, length)){
	case MD_ABN_EVENTP_NONE:
		break;
	case MD_ABN_EVENTP_SKIP:
		return -(length+sizeof(CB_ETHHDR_T));
	case MD_ABN_EVENTP_DUPLICATE:
		(void)gptpnet_send(gpnet, ndevIndex, length);
		return gptpnet_send(gpnet, ndevIndex, length);
	case MD_ABN_EVENTP_MANUPULATE:
		break;
	case MD_ABN_EVENTP_SENDER:
	default:
		return -1;
	}
	return gptpnet_send(gpnet, ndevIndex, length);
}

/**
 * @brief check if the timestamp should be abandoned by an abnormal event
 * @param ndevIndex	index of a network device
 * @param domainNumber	domain Number
 * @result 0:no abnormal event, 1:hit with a registered abnormal event
 */
int md_abnormal_timestamp(PTPMsgType msgtype, int ndevIndex, int domainNumber);

#endif
