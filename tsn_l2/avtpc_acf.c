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
#include "avtpc.h"
#include "avtpc_private.h"
#include "avtpc_acf.h"

/*
9.4.2 flexray
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 pad (padding length): 2 bits
 mtv (message_timestamp valid): 1 bit
 fr_bus_id: 5 bits
 rsv (reserved): 2 bits
 chan (source channel): 2 bits
 str (startup): 1 bit
 syn (sync): 1 bit
 pre (payload preamble): 1 bit
 nfi (null frame indicator): 1 bit
 message_timestamp: 8 octets
 fr_frame_id: 11 bits
 reserved: 15 bits
 cycle: 6 bits
 flexray_msg_payload: 0 to 64 quadlets

9.4.3 CAN/CAN FD message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 pad (padding length): 2 bits
 mtv (message_timestamp valid): 1 bit
 rtr (remote transmission request): 1 bit
 eff (extended frame format): 1 bit
 brs (bit rate switch): 1 bit
 fdf (CAN Flexible Data-rate (FD) format): 1 bit
 esi (error state indicator): 1 bit
 rsv (reserved): 3 bits
 can_bus_id: 5 bits
 message_timestamp: 8 octets
 rsv (reserved): 3 bits
 can_identifier: 29 bits
 can_msg_payload: 0 to 16 quadlets

9.4.4 Abbreviated CAN/CAN FD message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 pad (padding length): 2 bits
 mtv (message_timestamp valid): 1 bit
 rtr (remote transmission request): 1 bit
 eff (extended frame format): 1 bit
 brs (bit rate switch): 1 bit
 fdf (CAN Flexible Data-rate Format): 1 bit
 esi (error state indicator): 1 bit
 rsv (reserved): 3 bits
 can_bus_id: 5 bits
 can_identifier: 29 bits
 rsv: 3 bits
 can_msg_payload: 0 to 16 quadlets

9.4.5 LIN message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 pad (padding length): 2 bits
 mtv (message_timestamp valid): 1 bit
 lin_bus_id: 5 bits
 lin_identifier: 1 octet
 message_timestamp: 8 octets
 lin_msg_payload: 0 to 2 quadlets

9.4.6 MOST message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 pad (padding length): 2 bits
 mtv (message_timestamp valid): 1 bit
 most_net_id: 5 bits
 reserved: 1 octet
 message_timestamp: 8 octets
 device_id: 2 octets
 fblock_id: 1 octet
 inst_id: 1 octet
 func_id: 12 bits
 op_type: 4 bits
 reserved: 2 octets
 most_msg_payload: 0 to n quadlets

9.4.7 General Purpose Control (GPC) message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 gpc_msg_id: 6-octets
 gpc_msg_payload: 0 to n quadlets

9.4.8 Serial (SERIAL) message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 pad (padding length): 2 bits
 reserved: 1 octet
 dcd (data carrier detect): 1 bit
 dtr (data terminal ready): 1 bit
 dsr (data set ready): 1 bit
 rts (request to send): 1 bit
 cts (clear to send): 1 bit
 ri (ring indicator): 1 bit
 serial_msg_payload: 0 to n quadlets

9.4.9 Parallel (PARALLEL) message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 reserved: 1 octet
 bit_width: 1 octet
 parallel_msg_payload: 0 to n quadlets

9.4.10 Sensor (SENSOR) message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 mtv (message_timestamp valid): 1 bit
 num_sensors: 7 bits
 sz (sensor value size): 2 bits
 sensor_group: 6 bits
 message_timestamp: 8 octets
 sensor_msg_payload: 0 to n quadlets

9.4.11 Abbreviated Sensor message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 r (reserved): 1 bit
 num_sensors: 1 octet
 sz (sensor value size): 2 bits
 sensor_group: 6 bits
 sensor_msg_payload: 0 to n quadlets

9.4.12 AECP message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 reserved: 2 octets
 aecp_msg_payload: 0 to n quadlets

9.4.13 Ancillary Data message
 acf_msg_type: 7 bits
 acf_msg_length: 9 bits
 pad (padding length): 2 bits
 reserved: 10 bits
 mode: 2 bits
 fp (first packet): 1 bit
 lp (last packet): 1 bit
 line_number: 2 octets
 DID: 1 octet
 SDID/DBN: 1 octet
 ancillary_data_payload: 0 to n octets
 */

// this number of fieds can be sent in a packet
#define AVTPC_ACF_MAX_FIELDS 32

extern char *avtpc_acf_msgtype_str(avtpc_acf_message_type_t mt);

typedef struct acf_msg_flexray {
	uint8_t mtv_busid;
	uint8_t chan_str_syn_pre_nfi;
	uint8_t msg_ts[8];
	uint16_t frid;
	uint16_t cycle;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_flexray_t;
UB_ABIT16_FIELD(flexray_frid, 5, 0x3ff)
UB_ABIT16_FIELD(flexray_cycle, 0, 0x3f)
UB_ABIT8_FIELD(flexray_chan, 4, 0x3)
UB_ABIT8_FIELD(flexray_str, 3, 0x1)
UB_ABIT8_FIELD(flexray_syn, 2, 0x1)
UB_ABIT8_FIELD(flexray_pre, 1, 0x1)
UB_ABIT8_FIELD(flexray_nfi, 0, 0x1)
UB_ABIT8_FIELD(flexray_mtv, 5, 0x1)
UB_ABIT8_FIELD(flexray_busid, 0, 0x1f)

typedef struct acf_msg_can {
	uint8_t mtv_rtr_eff_brs_fdf_esi;
	uint8_t busid;
	uint8_t msg_ts[8];
	uint32_t canid;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_can_t;
UB_ABIT32_FIELD(can_canid, 0, 0x1fffffff)
UB_ABIT8_FIELD(can_busid, 0, 0x1f)
UB_ABIT8_FIELD(can_mtv, 5, 0x1)
UB_ABIT8_FIELD(can_rtr, 4, 0x1)
UB_ABIT8_FIELD(can_eff, 3, 0x1)
UB_ABIT8_FIELD(can_brs, 2, 0x1)
UB_ABIT8_FIELD(can_fdf, 1, 0x1)
UB_ABIT8_FIELD(can_esi, 0, 0x1)

typedef struct acf_msg_abcan {
	uint8_t mtv_rtr_eff_brs_fdf_esi;
	uint8_t busid;
	uint32_t canid;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_abcan_t;
UB_ABIT32_FIELD(abcan_canid, 0, 0x1fffffff)
UB_ABIT8_FIELD(abcan_busid, 0, 0x1f)
UB_ABIT8_FIELD(abcan_mtv, 5, 0x1)
UB_ABIT8_FIELD(abcan_rtr, 4, 0x1)
UB_ABIT8_FIELD(abcan_eff, 3, 0x1)
UB_ABIT8_FIELD(abcan_brs, 2, 0x1)
UB_ABIT8_FIELD(abcan_fdf, 1, 0x1)
UB_ABIT8_FIELD(abcan_esi, 0, 0x1)

typedef struct acf_msg_lin {
	uint8_t mtv_busid;
	uint8_t linid;
	uint8_t msg_ts[8];
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_lin_t;
UB_ABIT8_FIELD(lin_mtv, 5, 0x1)
UB_ABIT8_FIELD(lin_busid, 0, 0x1f)

typedef struct acf_msg_most {
	uint8_t mtv_mostid;
	uint8_t reserved0;
	uint8_t msg_ts[8];
	uint16_t devid;
	uint8_t fbid;
	uint8_t instid;
	uint16_t funcid_optype;
	uint16_t reserved1;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_most_t;
UB_ABIT16_FIELD(most_funcid, 4, 0xfff)
UB_ABIT16_FIELD(most_optype, 0, 0xf)
UB_ABIT8_FIELD(most_mtv, 5, 0x1)
UB_ABIT8_FIELD(most_mostid, 0, 0x1f)

typedef struct acf_msg_gpc {
	uint8_t msg_id[6];
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_gpc_t;

typedef struct acf_msg_serial {
	uint8_t reserved;
	uint8_t dcd_dtr_dsr_rts_cts_ri;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_serial_t;
UB_ABIT8_FIELD(serial_dcd, 5, 0x1)
UB_ABIT8_FIELD(serial_dtr, 4, 0x1)
UB_ABIT8_FIELD(serial_dsr, 3, 0x1)
UB_ABIT8_FIELD(serial_rts, 2, 0x1)
UB_ABIT8_FIELD(serial_cts, 1, 0x1)
UB_ABIT8_FIELD(serial_ri, 0, 0x1)

typedef struct acf_msg_parallel {
	uint8_t reserved;
	uint8_t bwidth;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_parallel_t;

typedef struct acf_msg_sensor {
	uint8_t mtv_nums;
	uint8_t sz_sgroup;
	uint8_t msg_ts[8];
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_sensor_t;
UB_ABIT8_FIELD(sensor_sz, 6, 0x3)
UB_ABIT8_FIELD(sensor_sgroup, 0, 0x3f)
UB_ABIT8_FIELD(sensor_mtv, 7, 0x1)
UB_ABIT8_FIELD(sensor_nums, 0, 0x7f)

typedef struct acf_msg_absensor {
	uint8_t nums;
	uint8_t sz_sgroup;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_absensor_t;
UB_ABIT8_FIELD(absensor_sz, 6, 0x3)
UB_ABIT8_FIELD(absensor_sgroup, 0, 0x3f)
UB_ABIT8_FIELD(absensor_nums, 0, 0x7f)

typedef struct acf_msg_aecp {
	uint16_t reserved;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_aecp_t;

typedef struct acf_msg_ancillary {
	uint16_t mode_fp_lp;
	uint16_t lnum;
	uint8_t did;
	uint8_t sdid;
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_ancillary_t;
UB_ABIT16_FIELD(ancillary_mode, 2, 0x3)
UB_ABIT16_FIELD(ancillary_fp, 1, 0x1)
UB_ABIT16_FIELD(ancillary_lp, 0, 0x1)

typedef struct acf_msg_user {
	uint8_t *payload0;
}__attribute__ ((packed)) acf_msg_user_t;

typedef struct acf_msg_field {
	uint16_t acf_msg_type_length;
	union {
		acf_msg_flexray_t flexray;
		acf_msg_can_t can;
		acf_msg_abcan_t abcan;
		acf_msg_lin_t lin;
		acf_msg_most_t most;
		acf_msg_gpc_t gpc;
		acf_msg_serial_t serial;
		acf_msg_parallel_t parallel;
		acf_msg_sensor_t sensor;
		acf_msg_absensor_t absensor;
		acf_msg_aecp_t aecp;
		acf_msg_ancillary_t ancillary;
		acf_msg_user_t user;
	}u;
}__attribute__ ((packed)) acf_msg_field_t;
UB_ABIT16_FIELD(acf_msg_type, 9, 0x7f)
UB_ABIT16_FIELD(acf_msg_length, 0, 0x1ff)


struct avtpc_acf_data{
	avtpc_data_t *avtpc;
	avbtp_subtype_def_t subtype;
	avtpc_acf_rcv_cb_t acfrcv_cb;
	void *cbdata;
};

static int acf_filed_header_size(avtpc_acf_message_type_t msgtype)
{
	switch(msgtype){
	case AVTPC_ACF_FLEXRAY:
		return sizeof(acf_msg_flexray_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_CAN:
		return sizeof(acf_msg_can_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_ABCAN:
		return sizeof(acf_msg_abcan_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_LIN:
		return sizeof(acf_msg_lin_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_MOST:
		return sizeof(acf_msg_most_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_GPC:
		return sizeof(acf_msg_gpc_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_SERIAL:
		return sizeof(acf_msg_serial_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_PARALLEL:
		return sizeof(acf_msg_parallel_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_SENSOR:
		return sizeof(acf_msg_sensor_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_ABSENSOR:
		return sizeof(acf_msg_absensor_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_AECP:
		return sizeof(acf_msg_aecp_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_ANCILLARY:
		return sizeof(acf_msg_ancillary_t)-sizeof(uint8_t*)+2;
	case AVTPC_ACF_USER0:
	case AVTPC_ACF_USER1:
	case AVTPC_ACF_USER2:
	case AVTPC_ACF_USER3:
	case AVTPC_ACF_USER4:
	case AVTPC_ACF_USER5:
	case AVTPC_ACF_USER6:
	case AVTPC_ACF_USER7:
		return sizeof(acf_msg_user_t)-sizeof(uint8_t*)+2;
	default:
		UB_LOG(UBL_ERROR, "%s:unknown msg_type=%d\n", __func__, msgtype);
	}
	return 0;
}

static uint8_t *acf_field_payload(avtpc_acf_message_type_t msgtype, acf_msg_field_t *acfmsg)
{
	switch(msgtype){
	case AVTPC_ACF_FLEXRAY:
		return acfmsg->u.flexray.payload0;
	case AVTPC_ACF_CAN:
		return acfmsg->u.can.payload0;
	case AVTPC_ACF_ABCAN:
		return acfmsg->u.abcan.payload0;
	case AVTPC_ACF_LIN:
		return acfmsg->u.lin.payload0;
	case AVTPC_ACF_MOST:
		return acfmsg->u.most.payload0;
	case AVTPC_ACF_GPC:
		return acfmsg->u.gpc.payload0;
	case AVTPC_ACF_SERIAL:
		return acfmsg->u.serial.payload0;
	case AVTPC_ACF_PARALLEL:
		return acfmsg->u.parallel.payload0;
	case AVTPC_ACF_SENSOR:
		return acfmsg->u.sensor.payload0;
	case AVTPC_ACF_ABSENSOR:
		return acfmsg->u.absensor.payload0;
	case AVTPC_ACF_AECP:
		return acfmsg->u.aecp.payload0;
	case AVTPC_ACF_ANCILLARY:
		return acfmsg->u.ancillary.payload0;
	case AVTPC_ACF_USER0:
	case AVTPC_ACF_USER1:
	case AVTPC_ACF_USER2:
	case AVTPC_ACF_USER3:
	case AVTPC_ACF_USER4:
	case AVTPC_ACF_USER5:
	case AVTPC_ACF_USER6:
	case AVTPC_ACF_USER7:
		return acfmsg->u.user.payload0;
	default:
		UB_LOG(UBL_ERROR, "%s:unknown msg_type=%d\n", __func__, msgtype);
	}
	return NULL;
}


static int acf_compose_one_field(avtpc_acf_msg_field_t *msgfl, acf_msg_field_t *acfmsg)
{
	acfmsg->acf_msg_type_length=
		acf_msg_type_set_bit_field(acfmsg->acf_msg_type_length, msgfl->msg_type);
	acfmsg->acf_msg_type_length=
		acf_msg_length_set_bit_field(acfmsg->acf_msg_type_length,
				     (msgfl->pl_length+acf_filed_header_size(
					     (avtpc_acf_message_type_t)msgfl->msg_type))/4);
	switch(msgfl->msg_type){
	case AVTPC_ACF_FLEXRAY:
		acfmsg->u.flexray.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.flexray.mtv_busid=
			flexray_mtv_set_bit_field(acfmsg->u.flexray.mtv_busid,
						  msgfl->u.flexray.mtv);
		acfmsg->u.flexray.mtv_busid=
			flexray_busid_set_bit_field(acfmsg->u.flexray.mtv_busid,
						    msgfl->u.flexray.busid);
		acfmsg->u.flexray.chan_str_syn_pre_nfi=
			flexray_chan_set_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi,
						   msgfl->u.flexray.chan);
		acfmsg->u.flexray.chan_str_syn_pre_nfi=
			flexray_str_set_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi,
						  msgfl->u.flexray.str);
		acfmsg->u.flexray.chan_str_syn_pre_nfi=
			flexray_syn_set_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi,
						  msgfl->u.flexray.syn);
		acfmsg->u.flexray.chan_str_syn_pre_nfi=
			flexray_pre_set_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi,
						  msgfl->u.flexray.pre);
		acfmsg->u.flexray.chan_str_syn_pre_nfi=
			flexray_nfi_set_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi,
						  msgfl->u.flexray.nfi);
		memcpy(acfmsg->u.flexray.msg_ts, msgfl->u.flexray.msg_ts, 8);
		acfmsg->u.flexray.frid=flexray_frid_set_bit_field(
			acfmsg->u.flexray.frid, msgfl->u.flexray.frid);
		acfmsg->u.flexray.cycle=flexray_cycle_set_bit_field(
			acfmsg->u.flexray.cycle, msgfl->u.flexray.cycle);
		break;
	case AVTPC_ACF_CAN:
		acfmsg->u.can.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi=
			can_mtv_set_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi,
					      msgfl->u.can.mtv);
		acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi=
			can_rtr_set_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi,
					      msgfl->u.can.rtr);
		acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi=
			can_eff_set_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi,
					      msgfl->u.can.eff);
		acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi=
			can_brs_set_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi,
					      msgfl->u.can.brs);
		acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi=
			can_fdf_set_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi,
					      msgfl->u.can.fdf);
		acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi=
			can_esi_set_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi,
					      msgfl->u.can.esi);
		acfmsg->u.can.busid=
			can_busid_set_bit_field(acfmsg->u.can.busid, msgfl->u.can.busid);
		memcpy(acfmsg->u.can.msg_ts, msgfl->u.can.msg_ts, 8);
		acfmsg->u.can.canid=
			can_canid_set_bit_field(acfmsg->u.can.canid, msgfl->u.can.canid);
		break;
	case AVTPC_ACF_ABCAN:
		acfmsg->u.abcan.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi=
			abcan_mtv_set_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi,
						msgfl->u.abcan.mtv);
		acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi=
			abcan_rtr_set_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi,
						msgfl->u.abcan.rtr);
		acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi=
			abcan_eff_set_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi,
						msgfl->u.abcan.eff);
		acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi=
			abcan_brs_set_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi,
						msgfl->u.abcan.brs);
		acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi=
			abcan_fdf_set_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi,
						msgfl->u.abcan.fdf);
		acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi=
			abcan_esi_set_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi,
						msgfl->u.abcan.esi);
		acfmsg->u.abcan.busid=
			abcan_busid_set_bit_field(acfmsg->u.abcan.busid, msgfl->u.abcan.busid);
		acfmsg->u.abcan.canid=
			abcan_canid_set_bit_field(acfmsg->u.abcan.canid, msgfl->u.abcan.canid);
		break;
	case AVTPC_ACF_LIN:
		acfmsg->u.lin.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.lin.mtv_busid=
			lin_mtv_set_bit_field(acfmsg->u.lin.mtv_busid, msgfl->u.lin.mtv);
		acfmsg->u.lin.mtv_busid=
			lin_busid_set_bit_field(acfmsg->u.lin.mtv_busid, msgfl->u.lin.busid);
		acfmsg->u.lin.linid=msgfl->u.lin.linid;
		memcpy(acfmsg->u.lin.msg_ts, msgfl->u.lin.msg_ts, 8);
		break;
	case AVTPC_ACF_MOST:
		acfmsg->u.most.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.most.mtv_mostid=
			most_mtv_set_bit_field(acfmsg->u.most.mtv_mostid, msgfl->u.most.mtv);
		acfmsg->u.most.mtv_mostid=
			most_mostid_set_bit_field(acfmsg->u.most.mtv_mostid, msgfl->u.most.mostid);
		memcpy(acfmsg->u.most.msg_ts, msgfl->u.most.msg_ts, 8);
		acfmsg->u.most.devid=htons(msgfl->u.most.devid);
		acfmsg->u.most.fbid=msgfl->u.most.fbid;
		acfmsg->u.most.instid=msgfl->u.most.instid;
		acfmsg->u.most.funcid_optype=
			most_funcid_set_bit_field(acfmsg->u.most.funcid_optype,
						  msgfl->u.most.funcid);
		acfmsg->u.most.funcid_optype=
			most_optype_set_bit_field(acfmsg->u.most.funcid_optype,
						  msgfl->u.most.optype);
		break;
	case AVTPC_ACF_GPC:
		acfmsg->u.gpc.payload0=(uint8_t*)msgfl->payload;
		memcpy(acfmsg->u.gpc.msg_id, msgfl->u.gpc.msg_id, 6);
		break;
	case AVTPC_ACF_SERIAL:
		acfmsg->u.serial.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri=
			serial_dcd_set_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri,
						 msgfl->u.serial.dcd);
		acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri=
			serial_dtr_set_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri,
						 msgfl->u.serial.dtr);
		acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri=
			serial_dsr_set_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri,
						 msgfl->u.serial.dsr);
		acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri=
			serial_rts_set_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri,
						 msgfl->u.serial.rts);
		acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri=
			serial_cts_set_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri,
						 msgfl->u.serial.cts);
		acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri=
			serial_ri_set_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri,
						msgfl->u.serial.ri);
		break;
	case AVTPC_ACF_PARALLEL:
		acfmsg->u.parallel.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.parallel.bwidth=msgfl->u.parallel.bwidth;
		break;
	case AVTPC_ACF_SENSOR:
		acfmsg->u.sensor.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.sensor.mtv_nums=
			sensor_mtv_set_bit_field(acfmsg->u.sensor.mtv_nums, msgfl->u.sensor.mtv);
		acfmsg->u.sensor.mtv_nums=
			sensor_nums_set_bit_field(acfmsg->u.sensor.mtv_nums, msgfl->u.sensor.nums);
		acfmsg->u.sensor.sz_sgroup=
			sensor_sz_set_bit_field(acfmsg->u.sensor.sz_sgroup, msgfl->u.sensor.sz);
		acfmsg->u.sensor.sz_sgroup=
			sensor_sgroup_set_bit_field(acfmsg->u.sensor.sz_sgroup,
						    msgfl->u.sensor.sgroup);
		memcpy(acfmsg->u.sensor.msg_ts, msgfl->u.sensor.msg_ts, 8);
		break;
	case AVTPC_ACF_ABSENSOR:
		acfmsg->u.absensor.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.absensor.nums=
			absensor_nums_set_bit_field(acfmsg->u.absensor.nums,
						    msgfl->u.absensor.nums);
		acfmsg->u.absensor.sz_sgroup=
			absensor_sz_set_bit_field(acfmsg->u.absensor.sz_sgroup,
						  msgfl->u.absensor.sz);
		acfmsg->u.absensor.sz_sgroup=
			absensor_sgroup_set_bit_field(acfmsg->u.absensor.sz_sgroup,
						      msgfl->u.absensor.sgroup);
		break;
	case AVTPC_ACF_AECP:
		acfmsg->u.aecp.payload0=(uint8_t*)msgfl->payload;
		break;
	case AVTPC_ACF_ANCILLARY:
		acfmsg->u.ancillary.payload0=(uint8_t*)msgfl->payload;
		acfmsg->u.ancillary.mode_fp_lp=
			ancillary_mode_set_bit_field(acfmsg->u.ancillary.mode_fp_lp,
						     msgfl->u.ancillary.mode);
		acfmsg->u.ancillary.mode_fp_lp=
			ancillary_fp_set_bit_field(acfmsg->u.ancillary.mode_fp_lp,
						   msgfl->u.ancillary.fp);
		acfmsg->u.ancillary.mode_fp_lp=
			ancillary_lp_set_bit_field(acfmsg->u.ancillary.mode_fp_lp,
						   msgfl->u.ancillary.lp);
		acfmsg->u.ancillary.lnum=htons(msgfl->u.ancillary.lnum);
		acfmsg->u.ancillary.did=msgfl->u.ancillary.did;
		acfmsg->u.ancillary.sdid=msgfl->u.ancillary.sdid;
		break;
	case AVTPC_ACF_USER0:
	case AVTPC_ACF_USER1:
	case AVTPC_ACF_USER2:
	case AVTPC_ACF_USER3:
	case AVTPC_ACF_USER4:
	case AVTPC_ACF_USER5:
	case AVTPC_ACF_USER6:
	case AVTPC_ACF_USER7:
		acfmsg->u.user.payload0=(uint8_t*)msgfl->payload;
		break;
	default:
		UB_LOG(UBL_ERROR, "%s:unknown msg_type=%d\n", __func__, msgfl->msg_type);
		return -1;
	}
	return 0;
}

static int acf_compose_fields(avtpc_acf_msg_field_t *msgfls[], acf_msg_field_t acfmsgs[])
{
	int fcount=0;
	avtpc_acf_msg_field_t *msgfl;
	int i;

	for(i=0;i<AVTPC_ACF_MAX_FIELDS;i++){
		msgfl=msgfls[i];
		if(!msgfl){break;}
		memset(&acfmsgs[fcount], 0, sizeof(acf_msg_field_t));
		if(!acf_compose_one_field(msgfl, &acfmsgs[fcount])){fcount++;}
		if(fcount==AVTPC_ACF_MAX_FIELDS){break;}
	}
	return fcount;
}

static int acf_read_one_field(avtpc_acf_message_type_t msg_type, int msg_length,
			      acf_msg_field_t *acfmsg, avtpc_acf_msg_field_t *msgfl)
{
	memset(msgfl, 0, sizeof(avtpc_acf_msg_field_t));
	msgfl->msg_type=msg_type;
	switch(msg_type){
	case AVTPC_ACF_FLEXRAY:
		msgfl->payload=&acfmsg->u.flexray.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_flexray_t)+sizeof(uint8_t*)-2;
		msgfl->u.flexray.mtv=flexray_mtv_bit_field(acfmsg->u.flexray.mtv_busid);
		msgfl->u.flexray.busid=flexray_busid_bit_field(acfmsg->u.flexray.mtv_busid);
		msgfl->u.flexray.chan=
			flexray_chan_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi);
		msgfl->u.flexray.str=
			flexray_str_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi);
		msgfl->u.flexray.syn=
			flexray_syn_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi);
		msgfl->u.flexray.pre=
			flexray_pre_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi);
		msgfl->u.flexray.nfi=
			flexray_nfi_bit_field(acfmsg->u.flexray.chan_str_syn_pre_nfi);
		memcpy(msgfl->u.flexray.msg_ts, acfmsg->u.flexray.msg_ts, 8);
		msgfl->u.flexray.frid=flexray_frid_bit_field(acfmsg->u.flexray.frid);
		msgfl->u.flexray.cycle=flexray_cycle_bit_field(acfmsg->u.flexray.cycle);
		break;
	case AVTPC_ACF_CAN:
		msgfl->payload=&acfmsg->u.can.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_can_t)+sizeof(uint8_t*)-2;
		msgfl->u.can.mtv=can_mtv_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.can.rtr=can_rtr_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.can.eff=can_eff_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.can.brs=can_brs_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.can.fdf=can_fdf_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.can.esi=can_esi_bit_field(acfmsg->u.can.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.can.busid=can_busid_bit_field(acfmsg->u.can.busid);
		memcpy(msgfl->u.can.msg_ts, acfmsg->u.can.msg_ts, 8);
		msgfl->u.can.canid=can_canid_bit_field(acfmsg->u.can.canid);
		break;
	case AVTPC_ACF_ABCAN:
		msgfl->payload=&acfmsg->u.abcan.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_abcan_t)+sizeof(uint8_t*)-2;
		msgfl->u.abcan.mtv=abcan_mtv_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.abcan.rtr=abcan_rtr_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.abcan.eff=abcan_eff_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.abcan.brs=abcan_brs_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.abcan.fdf=abcan_fdf_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.abcan.esi=abcan_esi_bit_field(acfmsg->u.abcan.mtv_rtr_eff_brs_fdf_esi);
		msgfl->u.abcan.busid=abcan_busid_bit_field(acfmsg->u.abcan.busid);
		msgfl->u.abcan.canid=abcan_canid_bit_field(acfmsg->u.abcan.canid);
		break;
	case AVTPC_ACF_LIN:
		msgfl->payload=&acfmsg->u.lin.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_lin_t)+sizeof(uint8_t*)-2;
		msgfl->u.lin.mtv=lin_mtv_bit_field(acfmsg->u.lin.mtv_busid);
		msgfl->u.lin.busid=lin_busid_bit_field(acfmsg->u.lin.mtv_busid);
		msgfl->u.lin.linid=acfmsg->u.lin.linid;
		memcpy(msgfl->u.lin.msg_ts, acfmsg->u.lin.msg_ts, 8);
		break;
	case AVTPC_ACF_MOST:
		msgfl->payload=&acfmsg->u.most.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_most_t)+sizeof(uint8_t*)-2;
		msgfl->u.most.mtv=most_mtv_bit_field(acfmsg->u.most.mtv_mostid);
		msgfl->u.most.mostid=most_mostid_bit_field(acfmsg->u.most.mtv_mostid);
		memcpy(msgfl->u.most.msg_ts, acfmsg->u.most.msg_ts, 8);
		msgfl->u.most.devid=ntohs(acfmsg->u.most.devid);
		msgfl->u.most.fbid=acfmsg->u.most.fbid;
		msgfl->u.most.instid=acfmsg->u.most.instid;
		msgfl->u.most.funcid=most_funcid_bit_field(acfmsg->u.most.funcid_optype);
		msgfl->u.most.optype=most_optype_bit_field(acfmsg->u.most.funcid_optype);
		break;
	case AVTPC_ACF_GPC:
		msgfl->payload=&acfmsg->u.gpc.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_gpc_t)+sizeof(uint8_t*)-2;
		memcpy(msgfl->u.gpc.msg_id, acfmsg->u.gpc.msg_id, 6);
		break;
	case AVTPC_ACF_SERIAL:
		msgfl->payload=&acfmsg->u.serial.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_serial_t)+sizeof(uint8_t*)-2;
		msgfl->u.serial.dcd=serial_dcd_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri);
		msgfl->u.serial.dtr=serial_dtr_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri);
		msgfl->u.serial.dsr=serial_dsr_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri);
		msgfl->u.serial.rts=serial_rts_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri);
		msgfl->u.serial.cts=serial_cts_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri);
		msgfl->u.serial.ri=serial_ri_bit_field(acfmsg->u.serial.dcd_dtr_dsr_rts_cts_ri);
		break;
	case AVTPC_ACF_PARALLEL:
		msgfl->payload=&acfmsg->u.parallel.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_parallel_t)+sizeof(uint8_t*)-2;
		msgfl->u.parallel.bwidth=acfmsg->u.parallel.bwidth;
		break;
	case AVTPC_ACF_SENSOR:
		msgfl->payload=&acfmsg->u.sensor.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_sensor_t)+sizeof(uint8_t*)-2;
		msgfl->u.sensor.mtv=sensor_mtv_bit_field(acfmsg->u.sensor.mtv_nums);
		msgfl->u.sensor.nums=sensor_nums_bit_field(acfmsg->u.sensor.mtv_nums);
		msgfl->u.sensor.sz=sensor_sz_bit_field(acfmsg->u.sensor.sz_sgroup);
		msgfl->u.sensor.sgroup=sensor_sgroup_bit_field(acfmsg->u.sensor.sz_sgroup);
		memcpy(msgfl->u.sensor.msg_ts, acfmsg->u.sensor.msg_ts, 8);
		break;
	case AVTPC_ACF_ABSENSOR:
		msgfl->payload=&acfmsg->u.absensor.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_absensor_t)+sizeof(uint8_t*)-2;
		msgfl->u.absensor.nums=absensor_nums_bit_field(acfmsg->u.absensor.nums);
		msgfl->u.absensor.sz=absensor_sz_bit_field(acfmsg->u.absensor.sz_sgroup);
		msgfl->u.absensor.sgroup=absensor_sgroup_bit_field(acfmsg->u.absensor.sz_sgroup);
		break;
	case AVTPC_ACF_AECP:
		msgfl->payload=&acfmsg->u.aecp.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_aecp_t)+sizeof(uint8_t*)-2;
		break;
	case AVTPC_ACF_ANCILLARY:
		msgfl->payload=&acfmsg->u.ancillary.payload0;
		msgfl->pl_length=msg_length*4-sizeof(acf_msg_ancillary_t)+sizeof(uint8_t*)-2;
		msgfl->u.ancillary.mode=ancillary_mode_bit_field(acfmsg->u.ancillary.mode_fp_lp);
		msgfl->u.ancillary.fp=ancillary_fp_bit_field(acfmsg->u.ancillary.mode_fp_lp);
		msgfl->u.ancillary.lp=ancillary_lp_bit_field(acfmsg->u.ancillary.mode_fp_lp);
		msgfl->u.ancillary.lnum=ntohs(acfmsg->u.ancillary.lnum);
		msgfl->u.ancillary.did=acfmsg->u.ancillary.did;
		msgfl->u.ancillary.sdid=acfmsg->u.ancillary.sdid;
		break;
	case AVTPC_ACF_USER0:
	case AVTPC_ACF_USER1:
	case AVTPC_ACF_USER2:
	case AVTPC_ACF_USER3:
	case AVTPC_ACF_USER4:
	case AVTPC_ACF_USER5:
	case AVTPC_ACF_USER6:
	case AVTPC_ACF_USER7:
		msgfl->payload=&acfmsg->u.user.payload0;
		msgfl->pl_length=msg_length*4-2;
		break;
	default:
		UB_LOG(UBL_ERROR, "%s:unknown msg_type=%d\n", __func__, msg_type);
		return -1;
	}
	return 0;
}

static int acf_receive_data(uint8_t *payload, int payload_size,
			    avbtp_rcv_cb_info_t *cbinfo, void *cbdata)
{
	avtpc_acf_data_t *avtpc_acf=(avtpc_acf_data_t *)cbdata;
	int seqn_diff;
	uint64_t *ts=NULL;
	uint8_t *stream_id=NULL;
	int pcount=0;

	UB_LOG(UBL_DEBUGV, "%s:\n",__func__);
	if(avtpc_acf->subtype==AVBTP_SUBTYPE_TSCF){
		avbtp_sd_info_t *rsdinfo=&cbinfo->u.rsdinfo;
		seqn_diff=rsdinfo->seqn_diff;
		if(rsdinfo->stream_id_valid){stream_id=rsdinfo->stream_id;}
		if(rsdinfo->timestamp_valid){ts=&rsdinfo->timestamp;}
	}else if(avtpc_acf->subtype==AVBTP_SUBTYPE_NTSCF){
		avbtp_cd_info_t *rcsinfo=&cbinfo->u.rcsinfo;
		seqn_diff=(cbinfo->u.rcsinfo.fsd0 - avtpc_acf->avtpc->seqn)&0xff;
		avtpc_acf->avtpc->seqn=cbinfo->u.rcsinfo.fsd0;
		if(rcsinfo->stream_id_valid){stream_id=rcsinfo->stream_id;}
	}else{
		UB_LOG(UBL_ERROR, "%s:subtype=%d is not TSCF or NTSCF\n",
		       __func__, avtpc_acf->subtype);
		return -1;
	}

	if(seqn_diff!=1){UB_LOG(UBL_INFO, "%s:seqn_diff=%d\n", __func__, seqn_diff);}
	while(pcount+4<payload_size){
		avtpc_acf_message_type_t msg_type;
		int msg_length;
		acf_msg_field_t *acfmsg;
		avtpc_acf_msg_field_t msgfl;
		acfmsg=(acf_msg_field_t *)payload;
		msg_type=(avtpc_acf_message_type_t)acf_msg_type_bit_field(
			acfmsg->acf_msg_type_length);
		msg_length=acf_msg_length_bit_field(acfmsg->acf_msg_type_length);
		if(!acf_read_one_field(msg_type, msg_length, acfmsg, &msgfl)){
			avtpc_acf->acfrcv_cb(&msgfl, stream_id, ts, avtpc_acf->cbdata);
		}
		pcount+=4*msg_length;
		payload+=4*msg_length;
	}
	return 0;
}

int avtpc_acf_set_ccr(avtpc_acf_data_t *avtpc_acf, client_connect_request_t *ccr,
		      const char *netdev, uint16_t vid)
{
	memset(ccr,0,sizeof(client_connect_request_t));
	if(avtpc_acf->subtype==AVBTP_SUBTYPE_TSCF){
		ccr->con_mode=AVTP_TSCF_CONNECTION;
		ccr->ht.preset_cmsh.hh.subtype=AVBTP_SUBTYPE_TSCF;
	}else{
		ccr->con_mode=AVTP_NTSCF_CONNECTION;
		ccr->ht.preset_cmch.hh.subtype=AVBTP_SUBTYPE_NTSCF;
	}
	strcpy(ccr->netdev, netdev);

	/* 1 packet in 1msec, one for a write buffer */
	ccr->max_frame_size=IEEE1722_DEFAULT_MAX_FRAME_SIZE;
	ccr->max_intv_frames=1;
	ccr->time_intv=1000;
	ccr->priority=IEEE1722_DEFAULT_PCP;
	ccr->rank=1;
	if(vid){ccr->tagged=true;}
	ccr->rec_tagged=VLAN_REC_TAGGED;
	ccr->vlanid=vid;
	ccr->lsem_post_time=0; //receive immediately with a single packet
	return 0;
}

int avtpc_acf_write_packet_mf(avtpc_acf_data_t *avtpc_acf, avtpc_acf_msg_field_t *msgfls[],
			      uint64_t timestamp)
{
	client_connection_data_t *ccd=avtpc_acf->avtpc->ccds;
	acf_msg_field_t acfmsgs[AVTPC_ACF_MAX_FIELDS];
	ccdbuf_data_t *cdata;
	avbtp_cm_control_header_t *cmch;
	avbtp_cm_stream_header_t *cmsh;
	struct timespec ts;
	uint64_t ts64;
	int fcount;
	int tsize;
	int hsize;
	int i=0;

	if(ccd->ccr.con_mode==AVTP_TSCF_CONNECTION){
		hsize=sizeof(avbtp_cm_stream_header_t);
	}else if(ccd->ccr.con_mode==AVTP_NTSCF_CONNECTION){
		hsize=sizeof(avbtp_cm_control_header_t);
	}else{
		UB_LOG(UBL_ERROR,"%s:this is not a TSCF/NTSCF connection\n", __func__);
		return -1;
	}

	if(!UB_NON_ZERO_B8(ccd->ccr.ht.preset_cmsh.stream_id)){
		UB_LOG(UBL_ERROR,"%s:steram_id is all zero\n",__func__);
		return -1;
	}

	fcount=acf_compose_fields(msgfls, acfmsgs);
	tsize=0;
	for(i=0;i<fcount;i++){
		int32_t msg_length;
		msg_length=acf_msg_length_bit_field(acfmsgs[i].acf_msg_type_length);
		if(tsize+msg_length*4 > ccd->ccr.max_frame_size - hsize){
			UB_LOG(UBL_WARN,"%s:can't send all of fields in msgfls\n", __func__);
			break;
		}
		tsize+=msg_length*4;
	}

	ts64=ub_rt_gettime64();
	UB_NSEC2TS(ts64+WRITE_PACKET_TIMEOUT*1000, ts);
	if(CB_SEM_TIMEDWAIT(&ccd->tsem_rel, &ts)) {
		UB_LOG(UBL_ERROR,"%s:timedout, streamid=0x%02hhX%02hhX\n", __func__,
				ccd->ccr.ht.preset_cmsh.stream_id[6],
				ccd->ccr.ht.preset_cmsh.stream_id[7]);
		return -1;
	}

	CB_THREAD_MUTEX_LOCK(&ccd->tmutex);
	cdata = (ccdbuf_data_t *)(ccd->buffer +
				  (CCDBUFF_PAYLOAD_OFFSET + ccd->ccr.max_frame_size +
				   sizeof(ccdbuf_data_info_t)) *
				  ccd->ctrlbufp);
	cdata->info.size=tsize+hsize;

	if(ccd->ccr.con_mode==AVTP_TSCF_CONNECTION){
		cmsh=(avbtp_cm_stream_header_t *)cdata->payload;
		cdata->info.timestamp=timestamp;
		avbtp_generate_stream_cm_header(cmsh, &ccd->ccr.ht.preset_cmsh,
						timestamp, tsize);
	}else{
		cmch=(avbtp_cm_control_header_t *)cdata->payload;
		avbtp_generate_ntscf_header(cmch, &ccd->ccr.ht.preset_cmch,
					    tsize);
	}

	tsize=0;
	for(i=0;i<fcount;i++){
		int msg_length;
		avtpc_acf_message_type_t msg_type;
		int fhsize;
		uint8_t *pd;
		msg_type=(avtpc_acf_message_type_t)acf_msg_type_bit_field(
			acfmsgs[i].acf_msg_type_length);
		msg_length=acf_msg_length_bit_field(acfmsgs[i].acf_msg_type_length);
		fhsize=acf_filed_header_size(msg_type);
		memcpy(cdata->payload+hsize+tsize, &acfmsgs[i], fhsize);
		pd=acf_field_payload(msg_type, &acfmsgs[i]);
		if(pd){memcpy(cdata->payload+hsize+tsize+fhsize, pd, msg_length*4-fhsize);}
		tsize+=msg_length*4;
	}
	CB_THREAD_MUTEX_UNLOCK(&ccd->tmutex);
	CB_SEM_POST(&ccd->tsem_set);
	UB_LOG(UBL_DEBUGV, "%s: sent one packet hsize=%d, tsize=%d\n",
	       __func__, hsize, tsize);
	return 0;
}

int avtpc_acf_close(avtpc_acf_data_t *avtpc_acf)
{
	int res;
	UB_LOG(UBL_DEBUGV, "%s:\n",__func__);
	res=avtpc_close(avtpc_acf->avtpc);
	free(avtpc_acf);
	return res;
}

avtpc_acf_data_t *avtpc_acf_init(avbtp_subtype_def_t subtype,
				 avtpc_acf_rcv_cb_t acfrcv_cb, void *cbdata, char *shsuf)
{
	avtpc_acf_data_t *avtpc_acf=NULL;
	UB_LOG(UBL_DEBUGV, "%s:\n",__func__);
	if(subtype!=AVBTP_SUBTYPE_TSCF && subtype!=AVBTP_SUBTYPE_NTSCF){
		UB_LOG(UBL_ERROR, "%s:subtype must be TSCF or NTSCF, subtype=0x%X\n",
		       __func__, subtype);
		return NULL;
	}
	avtpc_acf=(avtpc_acf_data_t *)malloc(sizeof(avtpc_acf_data_t));
	if(ub_assert_fatal(avtpc_acf!=NULL, __func__, NULL)) return NULL;
	memset(avtpc_acf, 0, sizeof(avtpc_acf_data_t));
	avtpc_acf->subtype=subtype;
	avtpc_acf->avtpc=avtpc_init(shsuf);
	if(!avtpc_acf->avtpc){goto erexit;};
	avtpc_acf->acfrcv_cb=acfrcv_cb;
	avtpc_acf->cbdata=cbdata;
	avtpc_set_rcv_cb(avtpc_acf->avtpc, acf_receive_data, avtpc_acf);
	return avtpc_acf;
erexit:
	avtpc_acf_close(avtpc_acf);
	return NULL;
}

void avtpc_acf_set_rcvtimeout_cb(avtpc_acf_data_t *avtpc_acf,
			avbtp_rcvtimeout_cb_t rcvtimeout_cb, void *timeout_cbdata)
{
	avtpc_set_rcvtimeout_cb(avtpc_acf->avtpc, rcvtimeout_cb, timeout_cbdata);
}

int avtpc_acf_connection_request(avtpc_acf_data_t *avtpc_acf, client_connect_request_t *ccr)
{
	return avtpc_connection_request(avtpc_acf->avtpc, ccr);
}
