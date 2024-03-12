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
/**
 * @addtogroup xl4avbl2
 * @{
 * @file avtpc_crf.h
 * @brief IEEE1722 AVB Transport Protocol, client program interface
 */
#ifndef __AVTPC_CRF_H__
#define __AVTPC_CRF_H__

#include "avbtp.h"

typedef struct avtpc_crf_data avtpc_crf_data_t;

/**
 * @brief Minimum number of AVTPDUS that 'mr' bit remains in its new state 
 * once it is toggled.
 */
#define AVTPC_MR_PDUS_TOGGLE_MIN 8

/**
 * @brief AVTPC CRF configuration parameter
 */
typedef struct avtpc_crf_config {
	/**
	 * Ethernet device name.
	 */
	char *netdev;
	/**
	 * VLAN identifier. vlan_id is unused if it is negative.
	 */
	int16_t vlan_id;
	/**
	 * VLAN priority PCP field. 
	 */
	uint8_t vlan_pcp;
	/**
	 * Mode indicater, true: CRF listener; otherwise: CRF talker 
	 */
	bool listener;
	/**
	 * Destination L2 multicast address.
	 */
	char *dest_mcast;
	/**
	 * Join L2 multicast address to receive 1722 packets.
	 */
	char *join_mcast;
	/**
	 * CRF stream identifier. 
	 */
	char *stream_id;
	/**
	 * CRF type field in the CRF header packet. Only talker use this config.
	 */
	avbtp_crf_type_t type;
	/**
	 * CRF pull field in the CRF header packet. Only talker use this config.
	 */
	avbtp_crf_pull_t pull;
	/**
	 * Base frequency in the CRF header packet. Only talker use this config. 
	 */
	uint32_t base_frequency;
	/**
	 * Timestamp interval in the CRF header packet. Only talker use this config. 
	 */
	uint16_t timestamp_interval;
	/**
	 * Number of AVTPDUS that 'mr' bit remains in its new state once it is toggled.
	 * Minimum value is @ref AVTPC_MR_PDUS_TOGGLE_MIN. Only talker use this config.
	 */
	uint16_t mr_pdus_toggle; 
} avtpc_crf_config_t;

/**
 * @brief Control CRF dynamic header fields
 */
typedef struct avtpc_crf_control {
	/**
	 * Enable toggling the 'mr' bit when this field is true, else do nothing.
	 * The 'mr' bit is actually set in the stream depends on @ref mr_pdus_toggle config.
	 */
	bool media_restart;
	/**
	 * When this field is true, set 'fs' bit, else clear 'fs' bit.
	 */
	bool frame_sync;
} avtpc_crf_control_t;

/**
 * @brief initialize avtpc_crf
 * @return the data handle of avtpc_crf
 * @param shsuf	suffix of the shared memory node name
 */
avtpc_crf_data_t *avtpc_crf_init(char *shsuf);

/**
 * @brief request connection to avtpd
 * @return 0:success, -1:error
 * @param avtpc_crf	the data handle of avtpc_crf
 * @param ccr	CCR data pointer
 */
int avtpc_crf_connection_request(avtpc_crf_data_t *avtpc_crf,
		client_connect_request_t *ccr);

/**
 * @brief send a packet of crf data
 * @return 0:success, -1:error
 * @param avtpc_crf	the data handle of avtpc_crf
 * @param control control CRF header fields, set to NULL if not used.
 * @param crf_data crf data pointer
 * @param crf_data_length length of crf data
 */
int avtpc_crf_write_packet(avtpc_crf_data_t *avtpc_crf,
		avtpc_crf_control_t *control, uint8_t *crf_data, uint32_t crf_data_length);

/**
 * @brief set receiver callback function
 * @param avtpc_crf	the data handle of avtpc_crf
 * @param rcv_cb	callback function
 * @param rcv_cbdata	the callback function uses this data pointer
 */
void avtpc_crf_set_rcv_cb(avtpc_crf_data_t *avtpc_crf,
		avbtp_rcv_cb_t rcv_cb, void *rcv_cbdata);

/**
 * @brief set CCR for CRF data.
 * This function set buffer parameters as following:
 * ccr->max_frame_size=IEEE1722_DEFAULT_MAX_FRAME_SIZE;
 * ccr->max_intv_frames=1;
 * ccr->time_intv=1000;
 * ccr->lsem_post_time=0;
 * User can overwrite these CCR parametters after calling this API.
 *
 * @return 0 on success, -1 on error
 * @param avtpc_crf	the data handle of avtpc_crf
 * @param crfcfg crf config information	
 * @param ccr CCR data pointer
 */
int avtpc_crf_set_ccr(avtpc_crf_data_t *avtpc_crf,
		avtpc_crf_config_t *crfcfg, client_connect_request_t *ccr);

/**
 * @brief request disconnection to avtpd
 * @return 0:success, -1:error
 * @param avtpc_crf	the data handle of avtpc crf
 */
int avtpc_crf_disconnection_request(avtpc_crf_data_t *avtpc_crf);

/**
 * @brief close avtpc_crf
 * @return 0:success, -1:error
 * @param avtpc_crf	the data handle of avtpc_crf
 */

int avtpc_crf_close(avtpc_crf_data_t *avtpc_crf);

#endif
/** @}*/
