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
#ifndef __MRPDU_H_
#define __MRPDU_H_

#include "mrpnet.h"
#include "mrp_sm.h"

#define MRPDU_MAX_MESSAGES 32
#define MRPDU_MAX_ATTRIBUTES MAX_ATTRIBUTE_NUM

#define SUPPRESS_PRINT_ATTR_NUM 20

/// Datafragment is applied for mrpdu, msrp and mvrp firstvalue allocation
///                             mrpdu three/four packet events
#define MRPDU_DATAFRAGMENT_SIZE 8
#define MRPDU_DATAFRAGMENT_INSTMEM mrpdu_datafragment_inst
#ifndef MRPDU_DATAFRAGMENT_INSTNUM
#define MRPDU_DATAFRAGMENT_INSTNUM 1000
#endif

UB_SD_GETMEM_DEF_EXTERN(MRPDU_DATAFRAGMENT_INSTMEM);

typedef enum {
	LVAE_Null = 0,
	LVAE_LVA,
} mrpdu_leaveall_event_t;

static inline protocol_event_t attr_proto_event(mrpdu_attribute_event_t aevent)
{
	if(aevent<=AEV_NoEvent || aevent >= AEV_LAST_NUM) return EVENT_NoEvent;
	return (protocol_event_t)((int)EVENT_rNew+(int)aevent);
}

static inline mrpdu_attribute_event_t proto_attr_event(protocol_event_t pevent)
{
	if(pevent<EVENT_rNew || pevent>EVENT_rLv) return AEV_NoEvent;
	return (mrpdu_attribute_event_t)(pevent-EVENT_rNew);
}

typedef struct mrpdu_attribute_data mrpdu_attribute_data_t;
typedef struct mrpdu_message mrpdu_message_t;
typedef struct  mrpdu_vector_attribute  mrpdu_vector_attribute_t;

typedef struct packed_data{
	uint16_t NumberOfValues;
	uint8_t *AttributeEvents;
	uint8_t *FourPackedEvents;
} packed_data_t;

/**
 * @brief parse MRPDU network data, evaluate it as a protocol by protoIndex. 10.8.1.2
 * @return mrpdu_attribute_data
 * @param dp	MRPDU network data
 * @param protoIndex	protocol index
 * @note returned data must be discarded by calling 'mrpdu_discard_data'
 * 	 endian in the data including FirstValue is converted Network to Host
 */
mrpdu_attribute_data_t *mrpdu_parse_data(uint8_t *dp, int size, protocol_index_t protoIndex);

/**
 * @brief discard mrpdu_attribute_data
 * @param md	mrpdu_attribute_data pointer
 */
void mrpdu_discard_data(mrpdu_attribute_data_t *md);

/**
 * @brief generate MRPDU data from mrpdu_attribute_data
 * @param md	mrpdu_attribute_data pointer
 * @param hsize	header area size, normally ETH_HLEN is used
 * @return generated MRPDU data size not including 'hsize'. -1 for error.
 * @note returned data must be discarded by calling 'mrpdu_discard_pack_data'
 * 	 endian in the data including FirstValue is converte Host to Netowork
 */
int mrpdu_pack_data(uint8_t **data, mrpdu_attribute_data_t *md, int hsize);

/**
 * @brief discard data generated by 'mrpdu_pack_data'
 */
void mrpdu_discard_pack_data(uint8_t *data);

/**
 * @brief generate a new mrpdu_attribute_data
 * @param protoIndex	protocol index
 * @note returned data must be discarded by calling 'mrpdu_discard_data'
 */
mrpdu_attribute_data_t *mrpdu_gen_data(protocol_index_t protoIndex);

/**
 * @brief add a message on a mrpdu_attribute_data
 * @param AttributeType	attribute type. 10.8.2.2
 * @param AttributeLength	attribute length. 10.8.2.3
 * @return mrpdu_message pointer
 * @note calling 'mrpdu_discard_data' deletes this data.
 */
mrpdu_message_t *mrpdu_add_message(mrpdu_attribute_data_t *md, uint8_t AttributeType,
				   uint8_t AttributeLength);

/**
 * @brief delete a message from a mrpdu_attribute_data
 * @param md	mrpdu_attribute_data pointer
 * @param message	mrpdu_message pointer
 * @return 0:success, -1:error
 */
int mrpdu_delete_message(mrpdu_attribute_data_t *md, mrpdu_message_t *message);

/**
 * @brief add a vector attribute data on a mrpdu_attribute_data
 * @param md	mrpdu_attribute_data pointer
 * @param message	mrpdu_message pointer
 * @param LeaveAllEvent 10.8.2.6
 * @param FirstValue	10.8.2.7
 * @param pd	packed data: AttributeEvents and FourPackedEvents
 * @return mrpdu_vector_attribute pointer
 * @note calling 'mrpdu_discard_data' deletes this data.
 */
mrpdu_vector_attribute_t *mrpdu_add_vector_attribute(mrpdu_attribute_data_t *md,
						     mrpdu_message_t *message,
						     mrpdu_leaveall_event_t LeaveAllEvent,
						     uint8_t *FirstValue,
						     packed_data_t *pd);

/**
 * @brief delete a vector attribute data from a 'mrpdu_attribute_data/message'
 * @param md	mrpdu_attribute_data pointer
 * @param message	mrpdu_message pointer
 * @param attr	mrpdu_vector_attribute pointer
 * @return 0:success, -1:error
 */
int mrpdu_delete_vector_attribute(mrpdu_attribute_data_t *md,
				  mrpdu_message_t *message,
				  mrpdu_vector_attribute_t *attr);

void mrpdu_print_vector_attribute(bool console, bool debug,
				  protocol_index_t protoIndex, uint8_t attrtype,
				  mrpdu_vector_attribute_t *attr, int indent, int fvsize);

void mrpdu_print_message(bool console, bool debug,
			 protocol_index_t protoIndex, mrpdu_message_t *message, int indent);

void mrpdu_print_attribute(bool console, bool debug, mrpdu_attribute_data_t *md);

mrpdu_message_t *mrpdu_get_message(mrpdu_attribute_data_t *md, int index);

mrpdu_vector_attribute_t *mrpdu_get_vector_attribute(mrpdu_message_t *message, int index);

int mrpdu_get_num_vector_attribute(mrpdu_message_t *message);

uint8_t mrpdu_get_attribute_type(mrpdu_message_t *message);

uint8_t mrpdu_get_attribute_length(mrpdu_message_t *message);

int mrpdu_get_vector_values(mrpdu_vector_attribute_t *vattr,
			    mrpdu_leaveall_event_t *LeaveAllEvent,
			    uint8_t **FirstValue,
			    packed_data_t *pd);

#endif
