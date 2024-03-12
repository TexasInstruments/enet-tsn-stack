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
#ifndef __AVTPC_ACF_H__
#define __AVTPC_ACF_H__

/**
 * @addtogroup xl4avbl2
 * @{
 * @file avtpc_acf.h
 * @brief IEEE1722 AVB Transport Protocol, Control Format client interface
 */

typedef enum {
	AVTPC_ACF_FLEXRAY = 0,
	AVTPC_ACF_CAN,
	AVTPC_ACF_ABCAN,
	AVTPC_ACF_LIN,
	AVTPC_ACF_MOST,
	AVTPC_ACF_GPC,
	AVTPC_ACF_SERIAL,
	AVTPC_ACF_PARALLEL,
	AVTPC_ACF_SENSOR,
	AVTPC_ACF_ABSENSOR,
	AVTPC_ACF_AECP,
	AVTPC_ACF_ANCILLARY,
	AVTPC_ACF_USER0 = 0x78,
	AVTPC_ACF_USER1,
	AVTPC_ACF_USER2,
	AVTPC_ACF_USER3,
	AVTPC_ACF_USER4,
	AVTPC_ACF_USER5,
	AVTPC_ACF_USER6,
	AVTPC_ACF_USER7,
} avtpc_acf_message_type_t;

typedef struct avtpc_acf_msg_flexray {
	uint8_t mtv;
	uint8_t busid;
	uint8_t chan;
	uint8_t str;
	uint8_t syn;
	uint8_t pre;
	uint8_t nfi;
	uint8_t msg_ts[8];
	uint16_t frid;
	uint8_t cycle;
} avtpc_acf_msg_flexray_t;

typedef struct avtpc_acf_msg_can {
	uint8_t mtv;
	uint8_t rtr;
	uint8_t eff;
	uint8_t brs;
	uint8_t fdf;
	uint8_t esi;
	uint8_t busid;
	uint8_t msg_ts[8];
	uint32_t canid;
} avtpc_acf_msg_can_t;

typedef struct avtpc_acf_msg_abcan {
	uint8_t mtv;
	uint8_t rtr;
	uint8_t eff;
	uint8_t brs;
	uint8_t fdf;
	uint8_t esi;
	uint8_t busid;
	uint32_t canid;
} avtpc_acf_msg_abcan_t;

typedef struct avtpc_acf_msg_lin {
	uint8_t mtv;
	uint8_t busid;
	uint8_t linid;
	uint8_t msg_ts[8];
} avtpc_acf_msg_lin_t;

typedef struct avtpc_acf_msg_most {
	uint8_t mtv;
	uint8_t mostid;
	uint8_t msg_ts[8];
	uint16_t devid;
	uint8_t fbid;
	uint8_t instid;
	uint16_t funcid;
	uint8_t optype;
} avtpc_acf_msg_most_t;

typedef struct avtpc_acf_msg_gpc {
	uint8_t msg_id[6];
} avtpc_acf_msg_gpc_t;

typedef struct avtpc_acf_msg_serial {
	uint8_t dcd;
	uint8_t dtr;
	uint8_t dsr;
	uint8_t rts;
	uint8_t cts;
	uint8_t ri;
} avtpc_acf_msg_serial_t;

typedef struct avtpc_acf_msg_parallel {
	uint8_t bwidth;
} avtpc_acf_msg_parallel_t;

typedef struct avtpc_acf_msg_sensor {
	uint8_t mtv;
	uint8_t nums;
	uint8_t sz;
	uint8_t sgroup;
	uint8_t msg_ts[8];
} avtpc_acf_msg_sensor_t;

typedef struct avtpc_acf_msg_absensor {
	uint8_t nums;
	uint8_t sz;
	uint8_t sgroup;
} avtpc_acf_msg_absensor_t;

#if 0
typedef struct avtpc_acf_msg_aecp {
} avtpc_acf_msg_aecp_t;
#endif

typedef struct avtpc_acf_msg_ancillary {
	uint8_t mode;
	uint8_t fp;
	uint8_t lp;
	uint16_t lnum;
	uint8_t did;
	uint8_t sdid;
} avtpc_acf_msg_ancillary_t;

typedef struct avtpc_acf_msg_field {
	uint8_t msg_type;
	uint16_t pl_length;
	void *payload;
	union {
		avtpc_acf_msg_flexray_t flexray;
		avtpc_acf_msg_can_t can;
		avtpc_acf_msg_abcan_t abcan;
		avtpc_acf_msg_lin_t lin;
		avtpc_acf_msg_most_t most;
		avtpc_acf_msg_gpc_t gpc;
		avtpc_acf_msg_serial_t serial;
		avtpc_acf_msg_parallel_t parallel;
		avtpc_acf_msg_sensor_t sensor;
		avtpc_acf_msg_absensor_t absensor;
		//avtpc_acf_msg_aecp_t aecp;
		avtpc_acf_msg_ancillary_t ancillary;
	}u;
} avtpc_acf_msg_field_t;

/**
 * @brief call back fuction for received ACF data
 * @return 0:success, -1:error
 * @param msgf ASC message data
 * @param stream_id	Stream ID, this is NULL if stream_id_valid flag is false
 * @param ts	timestamp, this is NULL for NTSCF data
 * @param cbdata	cbdata given with avtpc_acf_init
 */
typedef int (*avtpc_acf_rcv_cb_t)(avtpc_acf_msg_field_t *msgf, ub_streamid_t stream_id,
				  uint64_t *ts, void *cbdata);

typedef struct avtpc_acf_data avtpc_acf_data_t;

/**
 * @brief initialize avtpc_acf
 * @return the data handle of avtpc_acf
 * @param subtype	AVBTP_SUBTYPE_TSCF or AVBTP_SUBTYPE_NTSCF
 * @param acfrcv_cb	call back function with received ACF data
 * @param cbdata	data given for the call back function
 * @param shsuf	suffix of the shared memory node name
 */
avtpc_acf_data_t *avtpc_acf_init(avbtp_subtype_def_t subtype,
				 avtpc_acf_rcv_cb_t acfrcv_cb, void *cbdata, char *shsuf);

/**
 * @brief set receiver timeout callback function
 * @param avtpc_acf	the data handle of avtpc acf
 * @param rcvtimeout_cb	callback function
 * @param timeout_cbdata	the callback function uses this data pointer
 */
void avtpc_acf_set_rcvtimeout_cb(avtpc_acf_data_t *avtpc_acf,
			avbtp_rcvtimeout_cb_t rcvtimeout_cb, void *timeout_cbdata);

/**
 * @brief close avtpc
 * @return 0:success, -1:error
 * @param avtpc_acf	the data handle of avtpc_acf
 */
int avtpc_acf_close(avtpc_acf_data_t *avtpc_acf);

/**
 * @brief send multiple fields of acf data
 * @return 0:success, -1:error
 * @param avtpc_acf	the data handle of avtpc_acf
 * @param msgfls	an array of acf message fields
 * @param timestamp	timestamp for TSCF header, not used for NTSCF
 */
int avtpc_acf_write_packet_mf(avtpc_acf_data_t *avtpc_acf, avtpc_acf_msg_field_t *msgfls[],
			      uint64_t timestamp);

/**
 * @brief set CCR for TSCF/NTSCF data
 * @return 0 on success, -1 on error
 * @param avtpc_acf	the data handle of avtpc_acf
 * @param ccr	CCR data pointer
 * @param netdev	ethernet device name like 'eth0'
 * @param vid	vlan id, if zero don't use tagged data. this is almost always zero.
 */
int avtpc_acf_set_ccr(avtpc_acf_data_t *avtpc_acf, client_connect_request_t *ccr,
		      const char *netdev, uint16_t vid);

/**
 * @brief request connection to avtpd
 * @return 0:success, -1:error
 * @param avtpc_acf	the data handle of avtpc_acf
 * @param ccr	CCR data pointer
 */
int avtpc_acf_connection_request(avtpc_acf_data_t *avtpc_acf, client_connect_request_t *ccr);


/**
 * @brief print message fields
 * @param msgf	acf message field
 */
void avtpc_acf_print_msg_field(avtpc_acf_msg_field_t *msgf);

/** @}*/
#endif
