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
 * @file avbtp.h
 * @brief IEEE1722 AVB Transport Protocol
 */

#ifndef __AVBTP_H_
#define __AVBTP_H_

#include <stdbool.h>
#include <tsn_unibase/unibase.h>
#include <tsn_combase/combase.h>
#include "tsn_gptp/gptpmasterclock.h"
#include "avtp_ethernet.h"

/**
 * @brief default buffering time, usec unit
 */
#define DEFAULT_AVTPD_BUFFTIME 10000 // 10msec

/**
 * @brief default listener semaphore post time, usec unit
 */
#define DEFAULT_LSEM_POST_TIME 2000 // usec unit

/**
 * @brief default subtype for IEEE1722 packets.
 */
#define IEEE1722_DEFAULT_SUBTYPE AVBTP_SUBTYPE_61883

/**
 * @brief default multicast address to send IEEE1722 packets.
 */
#define IEEE1722_DEFAULT_MCAST "91:E0:F0:00:FE:00"

/**
 * @brief Default Time Stamp, msec unit.
 */
#define IEEE1722_DEFAULT_TSCOMP 1000000 // 1msec

/**
 * @brief Default Network device if not provided by user. default='eth0'.
 */
#define IEEE1722_DEFAULT_NETDEV DEFAULT_NETDEV

/**
 * @brief Default PTP Device. defualt=/dev/ptp0.
 */
#define IEEE1722_DEFAULT_PTPDEV DEFAULT_PTPDEVICE

/**
 * @brief Default Talker Address if explicitly not provided by user.
 */
#define IEEE1722_DEFAULT_TALKER_ADDR "00:00:00:00:00:00"

/**
 * @brief Default multicast address
 */
#define IEEE1722_DEFAULT_MCASTADDR "FF:FF:FF:FF:FF:FF"

/**
 * @brief Default VLAN ID
 */
#define IEEE1722_DEFAULT_VID 100

/**
 * @brief Default PCP value in VLAN tag. 2 for class B, 3 for class A
 */
#define IEEE1722_DEFAULT_PCP 2 // Class B

/**
 * @brief default streamID
 */
#define IEEE1722_DEFAULT_STREAM_ID "00:00:00:00:00:00:00:00"

/**
 * @brief Default frame Size
 */
#define IEEE1722_DEFAULT_MAX_FRAME_SIZE 1500

/**
 * @brief max string size of the shared memory node suffix
 */
#define AVTP_SHSUF_MAX 16

/**
 * @brief suffix of talker's shared memory node
 */
#define TALKER_SHM_SUFFIX 0

/**
 * @brief suffix of listener's shared memory node
 */
#define LISTENER_SHM_SUFFIX 1

/**
 * @brief muliticast mac address for maap
 */
#define AVBTP_MCAST_MAC_MAAP "91:E0:F0:00:FF:00"

/**
 * @brief muliticast mac address for avdecc
 */
#define AVBTP_MCAST_MAC_AVDECC "91:E0:F0:01:00:00"

enum {
	AVBTP_REC_TS_NORMAL = 0,
	AVBTP_REC_TS_NOTVALID,
	AVBTP_REC_TS_NEGATIVE_MOVE,
	AVBTP_REC_TS_BIG_MOVE,
};

enum {
	AVBTP_REC_TS_IN_PAST = 0,
	AVBTP_REC_TS_IN_FUTURE,
};

/**
 * @brief this is defined in IEEE 1722, the whole packet size never exceeds 1500
 */
#define AVBTP_MAX_STREAM_PAYLOAD_SIZE 1476

/**
 * @brief max number of talkers and listeners
 */
#define AVBTP_MAX_TLKSLSTS 16

/**
 * @brief max number of clients to connect to this daemon
 */
#define AVBTP_MAX_CLIENTS 16

/**
 * @brief This Macro is used to check subtype.
 */
#define AVBTP_SUBTYPE_CHECK TRUE

/**
 * @brief AVBTP Tag Value
 */
#define AVBTP_TAG_VALUE	    1

/**
 * @brief AVBTP Channel Value.
 */
#define AVBTP_CHNL_VALUE    31

/**
 * @brief Type Code Value as per IEEE standard which must be 10.
 */
#define AVBTP_TCODE_VALUE   10  /*0x0a*/

/**
 * @brief sy field is application specific and beyond of this standard.
 */
#define AVBTP_SY_VALUE      0

/**
 * @brief time interval for Class A. unit usec.
 */
#define AVBTP_CLASS_A_TIME_INTV 125

/**
 * @brief time interval for class B, unit usec.
 */
#define AVBTP_CLASS_B_TIME_INTV 250

/**
 * @brief thread in avtpd check 'stop_thread' with this interval
 */
#define CLIENT_CONNECTION_CHECK_INTERVAL (200*UB_MSEC_NS)

/**
 * @brief thread in avtpc check 'stop_thread' with this interval
 */
#define RECV_DATA_CHECK_INTERVAL (200*UB_MSEC_NS)

/**
 * @brief requests of disconnect need to wait stopping the thread
 */
#define AVTPD_RESPONSE_TIMEOUT (CLIENT_CONNECTION_CHECK_INTERVAL*2)

/**
 * @brief avtpd check request from avtpc with this timeout
 */
#define CLIENT_REQUEST_TIMEOUT (200*UB_MSEC_NS)

#define HEALTH_STATE_BAD ((RECV_DATA_CHECK_INTERVAL/CLIENT_REQUEST_TIMEOUT)+2)

/**
 * @brief AVTP stream data subtype values
 */
typedef enum {
	AVBTP_SUBTYPE_ANYSTREAM=0x7F, //!< no filter applied for stream packets,
	// *Caution* AVBTP_SUBTYPE_EF_STREAM uses the same code
	AVBTP_SUBTYPE_61883=0, //!< 61883
	AVBTP_SUBTYPE_MMA_STREAM=1, //!< MIDI Audio
	AVBTP_SUBTYPE_AAF=2, //!< AVTP Audio Format
	AVBTP_SUBTYPE_CVF=3, //!< Compressed Video Format
	AVBTP_SUBTYPE_CRF=4, //!< Clock Reference Format
	AVBTP_SUBTYPE_TSCF=5, //!< Time Synchronous Control Format
	AVBTP_SUBTYPE_SVF=6, //!< SDI Video Format
	AVBTP_SUBTYPE_RVF=7, //!< Raw Video Format
	AVBTP_SUBTYPE_AEF_CONTINUOUS=0x6E, //!<  Encrypted Format Stream
	AVBTP_SUBTYPE_VSF_STREAM=0x6F, //!< Vendor Specific Format Stream
	AVBTP_SUBTYPE_EF_STREAM=0x7F, //!< Experimental Format Stream
	AVBTP_SUBTYPE_MMA_CONTROL=0x81, //!< MMA Control
	AVBTP_SUBTYPE_NTSCF=0x82, //!< Non Time Synchronous Control Format
	AVBTP_SUBTYPE_ESCF=0xEC, //!< ECC Signed Control Format
	AVBTP_SUBTYPE_EECF=0xED, //!< ECC Encrypted Control Format
	AVBTP_SUBTYPE_AEF_DISCRETE=0xEE, //!< AES Encrypted Format Control
	AVBTP_SUBTYPE_ADP=0xFA, //!< AVDECC Discovery Protocol
	AVBTP_SUBTYPE_AECP=0xFB, //!< AVDECC Enumeration and Control Protocol FC16
	AVBTP_SUBTYPE_ACMP=0xFC, //!< Connection Management Protocol
	AVBTP_SUBTYPE_MAAP=0xFE, //!< MAAP Protocol
	AVBTP_SUBTYPE_EF_CONTROL=0xFF, //!< Experimental Format Control
} avbtp_subtype_def_t;

/**
 * @brief AVTP AAF stream data fromat values, Std. 8.2.2
 */
typedef enum {
	AVBTP_AAF_FORMAT_USER=0,
	AVBTP_AAF_FORMAT_FLOAT_32BIT=1,
	AVBTP_AAF_FORMAT_INT_32BIT=2,
	AVBTP_AAF_FORMAT_INT_24BIT=3,
	AVBTP_AAF_FORMAT_INT_16BIT=4,
	AVBTP_AAF_FORMAT_AES3_32BIT=5,
} avbtp_aaf_format_t;

/**
 * @brief AVTP AAF NSR(Nominal Sample Rate) values, Std. 8.3.1
 */
typedef enum {
	AVBTP_AAF_NSR_USER_SPECIFIED = 0,
	AVBTP_AAF_NSR_8KHZ = 1,
	AVBTP_AAF_NSR_16KHZ = 2,
	AVBTP_AAF_NSR_32KHZ = 3,
	AVBTP_AAF_NSR_44_1KHZ = 4,
	AVBTP_AAF_NSR_48KHZ = 5,
	AVBTP_AAF_NSR_88_2KHZ = 6,
	AVBTP_AAF_NSR_96KHZ = 7,
	AVBTP_AAF_NSR_176_4KHZ = 8,
	AVBTP_AAF_NSR_192KHZ = 9,
	AVBTP_AAF_NSR_24KHZ = 0xA,
} avbtp_aaf_nsr_t;

/**
 * @brief AVTP CVF stream data fromat values, Std. 9.3.1
 */
typedef enum {
	AVBTP_CVF_FORMAT_RFC=2,
} avbtp_cvf_format_t;

/**
 * @brief AVTP CVF stream data fromat_subtype values, Std. 9.3.2
 */
typedef enum {
	AVBTP_CVF_FORMAT_SUBTYPE_MJPEG=0,
	AVBTP_CVF_FORMAT_SUBTYPE_H264=1,
	AVBTP_CVF_FORMAT_SUBTYPE_JPEG2000=2,
} avbtp_cvf_format_subtype_t;

/**
 * @brief AVTP IEC 61883 Format IDs, Std. I.2.2.3.1
 */
typedef enum {
	AVBTP_IEC_61883_4_FMT=0x20,
	AVBTP_IEC_61883_6_FMT=0x10,
	AVBTP_IEC_61883_7_FMT=0x21,
	AVBTP_IEC_61883_8_FMT=0x01,
} avbtp_iec_format_id_t;

/*
 * @brief AVTP IEC61883 Sampling rate values
 */
typedef enum {
	AVBTP_IEC61883_6_SR_32KHZ	= 32000,
	AVBTP_IEC61883_6_SR_44_1KHZ  = 44100,
	AVBTP_IEC61883_6_SR_48KHZ	= 48000,
	AVBTP_IEC61883_6_SR_88_2KHZ  = 88200,
	AVBTP_IEC61883_6_SR_96KHZ	= 96000,
	AVBTP_IEC61883_6_SR_176_4KHZ =176400,
	AVBTP_IEC61883_6_SR_192_KHZ  =192000,
} avbtp_iec61883_6_sr_t;

/**
 * @brief AVTP IEC61883 SFC(Nominal Sampling frequency code) values
 */
typedef enum {
	AVBTP_IEC61883_6_NSR_32KHZ	= 0,
	AVBTP_IEC61883_6_NSR_44_1KHZ  = 1,
	AVBTP_IEC61883_6_NSR_48KHZ	= 2,
	AVBTP_IEC61883_6_NSR_88_2KHZ  = 3,
	AVBTP_IEC61883_6_NSR_96KHZ	= 4,
	AVBTP_IEC61883_6_NSR_176_4KHZ = 5,
	AVBTP_IEC61883_6_NSR_192_KHZ  = 6,
	AVBTP_IEC61883_6_NSR_RESERVED = 7,
} avbtp_iec61883_6_nsr_t;

/**
 * @brief AVTP IEC61883 SYT_INTERVAL values
 */
typedef enum {
	AVBTP_IEC61883_6_SYTINT_32KHZ	= 8,
	AVBTP_IEC61883_6_SYTINT_44_1KHZ  = 8,
	AVBTP_IEC61883_6_SYTINT_48KHZ	= 8,
	AVBTP_IEC61883_6_SYTINT_88_2KHZ  = 16,
	AVBTP_IEC61883_6_SYTINT_96KHZ	= 16,
	AVBTP_IEC61883_6_SYTINT_176_4KHZ = 32,
	AVBTP_IEC61883_6_SYTINT_192_KHZ  = 32,
} avbtp_iec61883_6_syt_interval_t;


/* @brief AVTP stream data format dependent field control , Std I.2.2.3.3 */
typedef enum {
	AVTBP_FMT_IEC61883_6_32bit_AM824=0,
	AVTBP_FMT_IEC61883_6_32bit_FLOAT_PT=4,
	AVTBP_FMT_IEC61883_6_32bit_FIXED_PT=6
} avbtp_iec61883_6_format_t;

/* @brief AVTP FMT IEC-61883 values */
typedef enum {
	AVBTP_FMT_IEC61883_2=0x00,
	AVBTP_FMT_IEC61883_8=0x01,
	AVBTP_FMT_IEC61883_6=0x10,
	AVBTP_FMT_IEC61883_4=0x40,
	AVBTP_FMT_IEC61883_7=0x41
} avbtp_iec61883_media_format_t;

/* @brief AVTP FMT CRF values */
typedef enum {
	AVBTP_FMT_CRF_PULL_MULT_1=0x00,
	AVBTP_FMT_CRF_PULL_MULT_1_OVER_1_0001=0x01,
	AVBTP_FMT_CRF_PULL_MULT_1_001=0x02,
	AVBTP_FMT_CRF_PULL_MULT_24_OVER_25=0x03,
	AVBTP_FMT_CRF_PULL_MULT_25_OVER_24=0x04,
	AVBTP_FMT_CRF_PULL_MULT_1_OVER_8=0x05
} avbtp_crf_pull_field_t;

/**
 * @brief AVTP CRF pull values
 */
typedef enum {
	AVBTP_CRF_PULL_MULT_1=0x00,
	AVBTP_CRF_PULL_MULT_1_OVER_1_0001=0x01,
	AVBTP_CRF_PULL_MULT_1_001=0x02,
	AVBTP_CRF_PULL_MULT_24_OVER_25=0x03,
	AVBTP_CRF_PULL_MULT_25_OVER_24=0x04,
	AVBTP_CRF_PULL_MULT_1_OVER_8=0x05
} avbtp_crf_pull_t;

/**
 * @brief AVTP CRF types
 */
typedef enum {
	AVBTP_CRF_TYPE_USER=0x00,
	AVBTP_CRF_TYPE_AUDIO_SAMPLE=0x01,
	AVBTP_CRF_TYPE_VIDEO_FRAME=0x02,
	AVBTP_CRF_TYPE_VIDEO_LINE=0x03,
	AVBTP_CRF_TYPE_MACHINE_CYCLE=0x04
} avbtp_crf_type_t;

/**
 * @brief avbtp_subtype datatype
 */
typedef uint8_t avbtp_subtype_t;

/**
 * @brief shared memory name of the master connection data
 */
#define AVBTP_MCD_SHMEM_NAME "/avbtp_mcd_shmem"

/**
 * @brief shared memory name prefix of the client connection data
 */
#define AVBTP_CCD_SHMEM_PREFIX "/avbtp_ccd_shmem_"

/**
 * @brief used to exit from process when getting error.
 */
#define IF_GOTO_EREXIT(a) if(a){\
		UB_LOG(UBL_ERROR,"%s:%s,%s\n", __func__, #a, strerror(errno)); goto erexit;\
	}
#define IF_RETURN_MINUSONE(a) if(a){\
		UB_LOG(UBL_ERROR,"%s:error in %s\n", __func__, #a); return -1;\
	}

/**
 * @brief IEC61883 CIP header length (in bytes)
 */
#define AVBTP_IEC61883_CIP_HEADERLEN 8

/**
 * @brief IEC61883 CIP header
 * @note	1722 Std., 5.4.3 IEC 61883 CIP header encapsulation
 */
typedef struct avbtp_iec_cip_info {
	uint8_t qi_1;
	uint8_t sid;
	uint8_t dbs;
	uint8_t fn;
	uint8_t qpc;
	uint8_t sph;
	uint8_t dbc;
	uint8_t qi_2;
	avbtp_iec_format_id_t fmt;
	uint8_t fdf[3];
}avbtp_iec_cip_info_t;

/**
 * @brief IEC61883 stream data information
 * @note	1722 Std., 5.2 IEC 61883/IIDC stream data encapsulation
 */
typedef struct avbtp_sd_iec_info {
	bool gv;
	uint32_t gwinfo;
	uint8_t tag;
	uint8_t channel;
	uint8_t tcode;
	uint8_t sy;
	avbtp_iec_cip_info_t cip;
}avbtp_sd_iec_info_t;

/**
 * @brief AVTPDU common stream data information (generic)
 * Intended for non-specific subtype (AVBTP_SUBTYPE_ANYSTREAM) or for handling
 * vendor specific stream information (AVBTP_SUBTYPE_VSF_STREAM).
 * @note	1722 Std., 4.4.4 AVTPDU common stream header information
 */
typedef struct avbtp_sd_generic_info {
	uint32_t fsd2;
	uint16_t s_dlen; // stream_data_length
	uint16_t fsd3;
	uint8_t fsd;
	uint8_t fsd1;
}avbtp_sd_generic_info_t;

typedef struct avbtp_sd_iec61883_6_info{
	bool gv;
	uint32_t gwinfo;
	uint8_t tag;
	uint8_t channel;
	uint8_t tcode;
	uint8_t sy;
	struct{
		uint8_t qi_1;
		uint8_t sid;
		uint8_t dbs;
		uint8_t fn;
		uint8_t qpc;
		uint8_t sph;
		uint8_t dbc;
		uint8_t qi_2;
		uint8_t fmt;
		struct{
			uint8_t evt;
			uint8_t sfc;
		}fdf;
		uint16_t syt;
		uint8_t payload_am824_label;
		uint8_t *cip_header_start;
	}cip;
}avbtp_sd_iec61883_6_info_t;

/**
 * @brief AAF stream data information
 * @see avbtp_aaf_format_t
 * @note	1722 Std., 7. AVTP Audio Format
 */
typedef struct avbtp_sd_aaf_info {
	avbtp_aaf_format_t format;
	uint32_t afsd1;
	uint8_t afsd;
	uint8_t afsd2;
	uint8_t evt;
	bool sp; //sparse timestamp mode
}avbtp_sd_aaf_info_t;

/**
 * @brief CVF stream data information
 * @see avbtp_sd_aaf_info_t
 * @see avbtp_cvf_format_subtype_t
 * @note	1722 Std., 8. Compressed Video Format
 */
typedef struct avbtp_sd_cvf_info {
	avbtp_cvf_format_t format;
	avbtp_cvf_format_subtype_t format_subtype;
	uint8_t evt;
	bool m;
}avbtp_sd_cvf_info_t;

/**
 * @brief stream data information
 */
typedef struct avbtp_sd_info {
	/* information in the received header */
	avbtp_subtype_def_t subtype;
	uint8_t version;
	bool stream_id_valid;
	bool media_restart;
	bool timestamp_valid;
	bool timestamp_uncertain;
	ub_streamid_t stream_id;
	uint8_t seqn_diff;
	/* target time to play: calculated from the timestamp in the header */
	uint64_t timestamp;
	 /* current ptp timestamp, this is only valid when the ccr->avtpc_expandts32=true */
	uint64_t current_timestamp;
	union {
		avbtp_sd_generic_info_t gen;
		avbtp_sd_iec_info_t iec;
		avbtp_sd_aaf_info_t aaf;
		avbtp_sd_cvf_info_t cvf;
	}u;
} avbtp_sd_info_t;

typedef struct avtpc_payload_profile {
        avbtp_subtype_def_t subtype;
        unsigned int  srate;
        unsigned short channels;
        uint8_t bit_depth;
        unsigned int frames_per_packet;
        uint32_t fmt_ctrl;
        uint32_t syt_interval;
        //cppcheck-suppress misra_19.2
        union {
	        avbtp_sd_iec61883_6_info_t iec6;
        }u;
} avtpc_payload_profile_t;

/**
 * @brief control/NTSCF data information
 */
typedef struct avbtp_cd_info {
	/* information in the received header */
	bool stream_id_valid;
	uint8_t version;
	uint32_t fsd0; // seqnum for NTSCF
	ub_streamid_t stream_id;
	ub_macaddr_t s_addr;
} avbtp_cd_info_t;

/**
 * @brief CRF data information
 * @note 1722 Std., 10 Clock Reference Format
 */
typedef struct avbtp_crf_info {
	avbtp_subtype_def_t subtype;
	bool stream_id_valid;
	uint8_t version;
	bool media_restart;
	bool frame_sync;
	uint8_t timestamp_uncertain;
	uint8_t seqn_diff;
	avbtp_crf_type_t type;
	ub_streamid_t stream_id;
	avbtp_crf_pull_t pull;
	uint32_t base_frequency;
	uint16_t timestamp_interval;
} avbtp_crf_info_t;

/**
 * @brief callback information data
 */
typedef struct avbtp_rcv_cb_info {
	union {
		avbtp_sd_info_t rsdinfo;
		avbtp_cd_info_t rcsinfo;
		avbtp_crf_info_t rcrfinfo;
	}u;
} avbtp_rcv_cb_info_t;

/**
 * @brief struct for multiple stream data buffers, for avtpc_write_sd_packet_mb
 */
typedef struct payload_multi_bufs {
	int size;
	uint8_t *data;
} payload_multi_bufs_t;

/**
 * @brief callback function from the receiver thread
 */
typedef int (*avbtp_rcv_cb_t)(uint8_t *payload, int payload_size,
			      avbtp_rcv_cb_info_t *rsdinfo, void *cbdata);

/**
 * @brief callback function from the receiver thread when timeout happen and no payload to receive
 */
typedef void (*avbtp_rcvtimeout_cb_t)(void *cbdata);

/**
 * @brief converts stream id into string format.
 * @param sid stream id
 * @param mstr	reference to the string buffer, which is used to store converted stream id.
 * @return pointer to the converted string.
 */
static inline char *ub_streamid_to_string(ub_streamid_t sid, char *mstr)
{
	sprintf(mstr, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
		sid[0],sid[1],sid[2],sid[3],sid[4],sid[5],sid[6],sid[7]);
	return mstr;
}

/**
 * @brief converts string to stream id.
 * @param mstr	string which are going to be converted into stream id.
 * @param sid	stream id which is used to store converted streamid into that/
 * @return	pointer to the streamid.
 */
static inline uint8_t *string_to_stream_id(const char *mstr, ub_streamid_t sid)
{
	sscanf(mstr,"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
	       sid, sid+1, sid+2, sid+3, sid+4, sid+5, sid+6, sid+7);
	return sid;
}

/**
 * @brief avtp header type
 */
typedef enum {
	AVBTP_UNKNOWN_HEADER=0,
	AVBTP_STREAM_HEADER,
	AVBTP_CONTROL_HEADER,
	AVBTP_ALTERNATIVE_HEADER,
}avbtp_header_type_t;

/**
@brief AVTP common stream header (CMSH), IEEE1722-2016 4.4.4

the definition of avbtp_cm_stream_header is as follows,
but the assingnemnt of bit fields depends on compiler,
and the bit-field macro is used to access the bit fields.

    typedef struct avbtp_cm_stream_header {
            avbtp_subtype_t subtype:8;
            uint32_t sv:1; // stream_id valid
            uint32_t version:3;
            uint32_t mr:1; // media clock restart
            uint32_t fsd0:2; // format specific data 0
            uint32_t tv:1; // timestamp valid
            uint32_t seqn:8; // sequence number
            uint32_t fsd1:7; // format specific data 1
            uint32_t tu:1; // timestamp uncertain
            ub_streamid_t stream_id;
            uint32_t header_timestamp;
            uint32_t fsd2; // format specific data 2
            uint16_t pd_length; //  packet data length
            uint16_t fsd3; // format specific data 3
    } __attribute__ ((packed)) avbtp_cm_stream_header_t;
*/
struct avbtp_cm_stream_hh {
	avbtp_subtype_t subtype;
	uint8_t bf0;
	uint8_t seqn;
	uint8_t bf1;
} __attribute__ ((packed));
struct avbtp_cm_stream_header {
	union{
		struct avbtp_cm_stream_hh hh;
		uint32_t bf;
	};
	ub_streamid_t stream_id;
	uint32_t header_timestamp; // *** use macro to access ***
	uint32_t fsd2; // *** use macro to access *** , format specific data 2
	uint16_t pd_length; // *** use macro to access *** , packet data length
	uint16_t fsd3; // *** use macro to access *** , format specific data 3
} __attribute__ ((packed));
typedef struct avbtp_cm_stream_header avbtp_cm_stream_header_t;

/**
@brief AVTP common control header (CMCH)

the definition of avbtp_cm_control_header is as follows,
but the assingnemnt of bit fields depends on compiler,
and the bit-field macro is used to access the bit fields.

    typedef struct avbtp_cm_control_header {
            avbtp_subtype_t subtype:8;
            uint32_t sv:1; // stream_id valid
            uint32_t version:3;
            uint32_t fsd0:9; // format specific data 0
            uint32_t cd_length:11;
            ub_streamid_t stream_id;
    } __attribute__ ((packed)) avbtp_cm_control_header_t;
*/
struct avbtp_cm_control_hh {
	avbtp_subtype_t subtype;
	uint8_t dummy[3];
} __attribute__ ((packed));
struct avbtp_cm_control_header {
	union{
		struct avbtp_cm_control_hh hh;
		uint32_t bf;
	};
	ub_streamid_t stream_id;
} __attribute__ ((packed));
typedef struct avbtp_cm_control_header avbtp_cm_control_header_t;

/**
@brief NTSCF(Non-Time-Synchronous Control Format)

the definition of avbtp_cm_control_header is as follows,
but the assingnemnt of bit fields depends on compiler,
and the bit-field macro is used to access the bit fields.

    typedef struct avbtp_ntscf_header {
            avbtp_subtype_t subtype:8;
            uint32_t sv:1; // stream_id valid
            uint32_t version:3;
	    uint32_t r:1;
            uint32_t data_length:11; // format specific data 0
            uint32_t seqnum:8;
            ub_streamid_t stream_id;
    } __attribute__ ((packed)) avbtp_ntscf_header_t;

Use avbtp_cm_control_header_t for this avbtp_ntscf_header_t
*/

/**
@brief CRF(Clock Reference Format)

the definition of avbtp_crf_header is as follows,
but the assingnemnt of bit fields depends on compiler,
and the bit-field macro is used to access the bit fields.

	typedef struct avbtp_crf_header {
			avbtp_subtype_t subtype:8;
			uint32_t sv:1; // stream_id valid
			uint32_t version:3;
			uint32_t mr:1;
			uint32_t r:1;
			uint32_t fs:1;
			uint32_t tu:1;
			uint32_t sequence_num:8;
			uint32_t type:8;
			ub_streamid_t stream_id;
			uint32_t pull:3;
			uint32_t base_frequency:29;
			uint16_t crf_data_length;
			uint16_t timestamp_interval;
	} __attribute__ ((packed)) avbtp_crf_header_t;
*/
struct avbtp_crf_hh {
	avbtp_subtype_t subtype;
	uint8_t bf0;
	uint8_t seqn;
	uint8_t type;
} __attribute__ ((packed));
struct avbtp_crf_header {
	union{
		struct avbtp_crf_hh hh;
		uint32_t bf;
	};
	ub_streamid_t stream_id;
	uint32_t bf1;
	uint16_t pd_length;
	uint16_t timestamp_interval;
} __attribute__ ((packed));
typedef struct avbtp_crf_header avbtp_crf_header_t;

/**
 * @brief macro to get header timestamp.
 */
#define get_header_timestamp(cmsh) ntohl(cmsh->header_timestamp)

/**
 * @brief macro to set header timestamp.
 */
#define set_header_timestamp(cmsh,x) cmsh->header_timestamp=htonl(x)

/**
 * @brief macro to get header 'format specific data 2'.
 */
#define get_header_fsd2(cmsh) ntohl(cmsh->fsd2)

/**
 * @brief macro to set header specifc data 2 field.
 *
 */
#define set_header_fsd2(cmsh,x) cmsh->fsd2=htonl(x)

/**
 * @brief macro to get header payload length.
 */
#define get_header_pd_length(cmsh) ntohs(cmsh->pd_length)

/**
 * @brief macro to set header payload length.
 */
#define set_header_pd_length(cmsh,x) cmsh->pd_length=htons(x)

/**
 * @brief get header 'format specific data 3'.
 */
#define get_header_fsd3(cmsh) ntohs(cmsh->fsd3)

/**
 * @brief set header 'format specifc data 3'.
 */
#define set_header_fsd3(cmsh,x) cmsh->fsd3=htons(x)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// the following bit fields are defined in the first 4 bytes of the header
UB_ABIT32_FIELD(cmsh_sv, 23, 0x1) // cmsh_sv_bit_field, cmsh_sv_set_bit_field
UB_ABIT32_FIELD(cmsh_version, 20, 0x7) // cmsh_version_bit_field, cmsh_version_set_bit_field
UB_ABIT32_FIELD(cmsh_mr, 19, 0x1) // cmsh_mr_bit_field, cmsh_mr_set_bit_field
UB_ABIT32_TOGGLE_FIELD(cmsh_mr, 19, 0x1) // cmsh_mr_toggle_bit_field
UB_ABIT32_FIELD(cmsh_fsd0, 17, 0x3) // cmsh_fsd0_bit_field, cmsh_fsd0_set_bit_field
UB_ABIT32_FIELD(cmsh_tv, 16, 0x1) // cmsh_tv_bit_field, cmsh_tv_set_bit_field
UB_ABIT32_FIELD(cmsh_fsd1, 1, 0x7f) // cmsh_fsd1_bit_field, cmsh_fsd1_set_bit_field
UB_ABIT32_FIELD(cmsh_tu, 0, 0x1) // cmsh_tu_bit_field, cmsh_tu_set_bit_field

UB_ABIT32_FIELD(cmch_sv, 23, 0x1) // cmch_sv_bit_field, cmch_sv_set_bit_field
UB_ABIT32_FIELD(cmch_version, 20, 0x7) // cmch_version_bit_field, cmch_version_set_bit_field
UB_ABIT32_FIELD(cmch_fsd0, 11, 0x1ff) // cmch_fsd0_bit_field, cmch_fsd0_set_bit_field
UB_ABIT32_FIELD(cmch_cd_length, 0, 0x7ff) // cmch_cd_length_bit_field, cmch_cd_length_set_bit_field

UB_ABIT16_FIELD(cmsh_cvf_ptv, 13, 0x1) // cmsh_cvf_ptv_bit_field, cmsh_cvf_ptv_set_bit_field
UB_ABIT16_FIELD(cmsh_cvf_m, 12, 0x1) // cmsh_cvf_m_bit_field, cmsh_cvf_m_set_bit_field
UB_ABIT16_FIELD(cmsh_cvf_evt, 8, 0xf) // cmsh_cvf_evt_bit_field, cmsh_cvf_evt_set_bit_field

UB_ABIT32_FIELD(ntscf_sv, 23, 0x1) // sv_bit_field, ntscf_sv_set_bit_field
UB_ABIT32_FIELD(ntscf_version, 20, 0x7) // version_bit_field, ntscf_version_set_bit_field
UB_ABIT32_FIELD(ntscf_data_length, 8, 0x7ff) // version_bit_field, ntscf_data_length_set_bit_field
UB_ABIT32_FIELD(ntscf_seqnum, 0, 0xff) // version_bit_field, ntscf_seqnum_set_bit_field
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#define ACCEPT_AVTP_VERSION(v) (v==0)

/**
 * @brief avtp conncetion mode types.
 */
typedef enum {
	/**
	 * Data stream talker connection, multiple avtpd buffers are
	 * used for sending.
	 */
	AVTP_TALKER_CONNECTION = 0,
	/**
	 * Data stream listener connection, multiple avtpd buffers are
	 * used for receiving.
	 */
	AVTP_LISTENER_CONNECTION,
	/**
	 * Both control talker and listener connection.
	 * However talker uses only one avtpd buffer for sending,
	 * listener uses multiple avtpd buffers for receiving.
	 */
	AVTP_CONTROL_CONNECTION,
	/**
	 * Control talker connection, multiple avtpd buffers are
	 * used for sending.
	 */
	AVTP_TALKONLY_CONTROL_CONNECTION,
	/**
	 * Both NTSCF talker and listener connection.
	 * However talker uses only one avtpd buffer for sending,
	 * listener uses multiple avtpd buffers for receiving.
	 */
	AVTP_NTSCF_CONNECTION,
	/**
	 * Both TSCF talker and listener connection.
	 * However talker uses only one avtpd buffer for sending,
	 * listener uses multiple avtpd buffers for receiving.
	 */
	AVTP_TSCF_CONNECTION,
	/**
	 * CRF talker connection, multiple avtpd buffers are
	 * used for sending.
	 */
	AVTP_CRF_TALKER_CONNECTION,
	/**
	 * CRF listener connection, multiple avtpd buffers are
	 * used for receiving.
	 */
	AVTP_CRF_LISTENER_CONNECTION,
} avtp_conection_mode_t;

/**
 * @brief client connect request (CCR)
 */
typedef struct client_connect_request {
	union {
		avbtp_cm_stream_header_t preset_cmsh;
		avbtp_cm_control_header_t preset_cmch;
		avbtp_crf_header_t preset_crfh;
	}ht;
	int ci; //!< connection index
	avtp_conection_mode_t con_mode;
	char netdev[CB_MAX_NETDEVNAME+17]; // 17 is length of UB_PRIhexB6, added for QNX case
	bool promisc; //!< true to use promiscuous mode socket
	bool tagged; //!< if true, use tagged data for talker.
	bool rec_tagged; //!< if true, open socket with ETH_P_8021Q instead of ETH_P_TSN
	uint16_t vlanid;
	bool mcast_addr_update;
	ub_macaddr_t mcast_addr;	//!< multicast address(can be uni cast)
	// (not related to talker, this is a bit confusing name)
	ub_macaddr_t join_mcast; //!< all zero for single cast or talker-mode
	int max_frame_size; //!< MaxFrameSize 802.1Q-2014 35.2.8.4, don't include Eth Header
	int max_intv_frames;
	int time_intv; //!< Class A:125, Class B:250, or non-standard time (usec unit)
	int lsem_post_time; //!< set the listener semaphore with this time buffering(usec uint)
	int priority; //!< Class A:3, Class B:2
	int rank; //!< 0 for emergency, normally 1
	/**
	 * The talker sends packet ahead to ts for this (nano second) if this value >=0.
	 * If this value is set to -1, there is no timing control by timestamp in the talker.
	 * For other negative values, talker will send after the ts for this value.
	 */
	int send_ahead_ts;
	int avtpd_bufftime; //!< buffering time by the max number of buffers
	bool echoback;
	uint32_t tshape_rate; //!< software traffic shaper, bytes per msec, 0 to disable
	int tshape_minsleep; //!< minimum sleep time for the traffic shaper, nsec unit
	/**
	 * true if no health check is applied. This option is recommended to use when
	 * the avtpd and avtpc are located in the same address space.
	 */
	bool no_healthcheck;
	/**
	 * 32bit timestamp will be expanded in the avtpc side instead of in the avtpd.
	 * This option aims to minimize the frequency of calls to gptpmasterclock_getts64(),
	 * particularly on resource-constrained platforms where the function may introduce
	 * significant latency. The goal is to enhance performance by reducing the impact
	 * on the RX data rate in such scenarios.
	 */
	bool avtpc_expandts32;
	/**
	 * Discard packets with a different destination MAC address than ccr->join_mcast.
	 */
	bool filter_destmac;
} client_connect_request_t;

/**
 * @brief client conncetion result.
 */
typedef enum {
	AVTP_CCR_ACCEPTED = 0,
	AVTP_CCR_ARLEADY_EXIST,
	AVTP_CCR_NOT_INTHELIST,
	AVTP_CCR_INVALID_REQUEST,
} client_connect_result_t;

/**
 * @brief data type of response to client connect request
 * @see client_connect_result_t
 */
typedef struct client_connect_response {
	client_connect_result_t result;
	int ccd;
} client_connect_response_t;

/**
 * @brief client connection request type.
 */
typedef enum {
	CCR_CONNECTION_REQUEST = 0,
	CCR_DISCONNECTION_REQUEST,
} ccr_request_type_t;

/**
 * @brief master connection data (MCD)
 * @see client_connect_request_t
 * @see client_connect_response_t
 * @see ccr_request_type_t
 *
 */
typedef struct master_connection_data {
	CB_THREAD_MUTEX_T mutex;
	CB_SEM_T sem_request;
	CB_SEM_T sem_response;
	client_connect_request_t ccr;
	client_connect_response_t ccres;
	ccr_request_type_t reqtype;
} master_connection_data_t;

/**
 * @brief thread states.
 */
typedef enum {
	CC_THREAD_INIT=0,
	CC_THREAD_RUN,
	CC_THREAD_BUFFLASH_STOP,
	CC_THREAD_IMMEDIATE_STOP,
	CC_THREAD_STOPPED,
} cc_thread_stop_type_t;

/**
 * @brief buffer payload offset.
 */
#define CCDBUFF_PAYLOAD_OFFSET ((int)(sizeof(CB_ETHHDR_T)+4))

/**
 * @brief client connect data (CCD)
 * @see avtp_ethernet_handle_t
 * @see cc_thread_stop_type_t
 * @see client_connect_request_t
 *
 */
typedef struct client_connection_data {
	avtp_ethernet_handle_t raweth;
	bool tcs_reserved;
	int health_check;
	CB_THREAD_MUTEX_T tmutex;
	CB_SEM_T tsem_set;
	CB_SEM_T tsem_rel;
	CB_THREAD_MUTEX_T rmutex;
	CB_SEM_T rsem_set;
	CB_SEM_T rsem_rel;
	CB_THREAD_T thread;
	cc_thread_stop_type_t stop_thread;
	client_connect_request_t ccr;
	int readp;
	int writep;
	int bufused;
	int bufnum;
	int rcv_sempost_bufnum;
	int ctrlbufp;
	uint8_t buffer[];
} client_connection_data_t;

/**
 * @brief info part of client connection buffer data
 */
struct ccdbuf_data_info {
	int size; //payload size
	uint64_t timestamp;
} __attribute__ ((packed));
typedef struct ccdbuf_data_info ccdbuf_data_info_t;

/**
 * @brief client connection buffer data
 */
struct ccdbuf_data {
	ccdbuf_data_info_t info;
	uint8_t header[CCDBUFF_PAYLOAD_OFFSET];
	uint8_t payload[];
} __attribute__ ((packed));
typedef struct ccdbuf_data ccdbuf_data_t;


#define AVBTP_PAYLOAD_SIZE(ccr) ((ccr).max_frame_size -			\
				 (sizeof(avbtp_cm_stream_header_t)))

/**
 * @brief Serialize crf timestamps into standard types of crf data.
 * This is used for CRF AVTPDUs with a type value of CRF_AUDIO_SAMPLE, CRF_VIDEO_FRAME,
 * CRF_VIDEO_LINE, or CRF_MACHINE_CYCLE.
 *
 * @param array_ts an array of crf timestamps
 * @param num_ts number of ts in the array
 * @param buffer output buffer contains serialized crf timestamps
 * @param buffer_size output buffer size
 * @return -1: error, >0: serialized data length
 *
 */
int avbtp_serialize_stdtype_crf_data(uint64_t *array_ts, uint32_t num_ts,
				uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief Deserialize standard types of crf data into an array of crf timestamps
 * This is used for CRF AVTPDUs with a type value of CRF_AUDIO_SAMPLE, CRF_VIDEO_FRAME,
 * CRF_VIDEO_LINE, or CRF_MACHINE_CYCLE.
 *
 * @param array_ts an array to contains deserialized crf timestamps output
 * @param array_size size of array
 * @param crf_data intput buffer contains serialized crf timestamps
 * @param data_length length of crf_data
 * @return -1: error, >0: number of deserialized crf timestamps
 */
int avbtp_deserialize_stdtype_crf_data(uint64_t *array_ts, uint32_t array_size,
				uint8_t *crf_data, uint32_t data_length);

/**
 * @brief update avbtp_cm_stream_header
 * @param cmsh	target avbtp_cm_stream_header
 * @param cmsh_temp	new avbtp_cm_stream_header
 * @param timestamp	timestamp for new avbtp_cm_stream_header
 * @param payload_size	payload size
 * @note seqn field is incremented
 * @return void
 *
 */
void avbtp_generate_stream_cm_header(avbtp_cm_stream_header_t *cmsh,
				     avbtp_cm_stream_header_t *cmsh_temp,
				     uint32_t timestamp, uint16_t payload_size);

/**
 * @brief update avbtp_cm_control_header
 * @param cmch	target avbtp_cm_control_header
 * @param cmch_temp	new avbtp_cm_control_header
 * @param payload_size	payload size
 * @return void
 */
void avbtp_generate_control_cm_header(avbtp_cm_control_header_t *cmch,
				      avbtp_cm_control_header_t *cmch_temp,
				      uint16_t payload_size);

/**
 * @brief update avbtp_cm_control_header for ntscf
 * @param cmch	target avbtp_cm_control_header
 * @param cmch_temp	new avbtp_cm_control_header
 * @param payload_size	payload size
 * @return void
 */
void avbtp_generate_ntscf_header(avbtp_cm_control_header_t *cmch,
				 avbtp_cm_control_header_t *cmch_temp,
				 uint16_t payload_size);

/**
 * @brief update a crf header
 * @param crfh crf header needs to be updated based on crfh_temp
 * @param crfh_temp base crf header
 * @param payload_size	payload size
 * @return void
 */
void avbtp_generate_crf_header(avbtp_crf_header_t *crfh,
				 avbtp_crf_header_t *crfh_temp, uint16_t payload_size);

/**
 * @brief calculate nominal frequency from the base frequency
 * @param pull specifies the multiplier modifier of the base frequency
 * @param base_freq base frequency
 * @return nominal frequency
 */
float avbtp_crf_nominal_frequency(avbtp_crf_pull_t pull, uint32_t base_freq);

/**
 * @brief set stream id into avbtp_cm_control_header
 * @param cmsh	avbtp_cm_control_header
 * @param stream_id	stream ID
 * @note if stream_id is all 0, clear sv bit, otherwise set sv bit
 * @return void
 */
void avbtp_set_stream_id(avbtp_cm_stream_header_t *cmsh, ub_streamid_t stream_id);

/**
 * @brief set mac address and lower 2 bytes stream id into avbtp_cm_control_header
 * @param cmsh	avbtp_cm_control_header
 * @param dev	ethernet device name like 'eth0', whose mac address is used
 * @param sid	lower 2 bytes of stream id (host order)
 * @return	void
 */
void avbtp_set_stream_id_withmac(avbtp_cm_stream_header_t *cmsh, const char* dev,
				 uint16_t sid);

/**
 * @brief return avbtp header size
 */
int avbtp_header_size(void);

/**
 * @brief set stream data information from received payload data
 * @param sd_rinfo	stream data information
 * @param payload	received payload data
 * @param seqn	sequence number, sd_rinfo->seqn_diff is set and new receceived number is set
 * @param timestamp	sd_rinfo->timestamp is updated to this timestamp
 * @return packet data length
 */
int avbtp_set_sd_rinfo(avbtp_sd_info_t *sd_rinfo, uint8_t *payload,
		       int *seqn, int64_t timestamp);

/**
 * @brief set CRF header information from received payload data
 * @param crf_rinfo	CRF header information
 * @param payload	CRF payload data
 * @param seqn	sequence number, crf_rinfo->seqn_diff is set and new receceived number is set
 * @return CRF payload length
 */
int avbtp_set_crf_rinfo(avbtp_crf_info_t *crf_info, uint8_t *payload, int *seqn);

/**
 * @brief get client connection data(CCD) size
 * @param ccr	client connect request
 * @param ccdbufnum	return the number of packet buffers in ccd
 * @return CCD size
 */
size_t get_ccd_size(client_connect_request_t *ccr, unsigned int *ccdbufnum);

/**
 * @brief sanity check of ccr parameters to connect to avtpd
 * @param ccr	client connect request
 * @return -1 if bad, 0 if okay
 */
int ccr_sanity_check(client_connect_request_t *ccr);

/**
 * @brief get header type (stream, control, alternative) from subtype
 * @param subtype	subtype of avtp data
 * @return header type
 */
avbtp_header_type_t get_header_type(avbtp_subtype_t subtype);

/**
 * @brief set CCR for control data
 * @param ccr	CCR data pointer
 * @param netdev	ethernet device name like 'eth0'
 * @param subtype	control data subtype
 * @param fsd0	fsd0 field in the common control header
 * @param vid	vlan id, if zero don't use tagged data. this is almost always zero.
 * @return 0 on success, -1 on error
 */
int avbtp_set_control_ccr(client_connect_request_t *ccr, const char *netdev,
			  avbtp_subtype_t subtype, uint32_t fsd0, uint16_t vid);

/**
 * @brief set CCR for stream data
 * @param ccr	CCR data pointer
 * @param netdev	ethernet device name like 'eth0'
 * @param subtype	control data subtype
 * @param talker	true for talker, false for listener
 * @param vid	        vlan id, if zero don't use tagged data.
 * @param pcp           priority of the stream
 * @param max_frm_sz    max frame size without layer 2 header
 * @param time_intv     class A:125, class B:250, or non-standard time (usec unit)
 * @param max_intv_frames max frames per time_intv above
 * @return 0 on success, -1 on error
 */
int avbtp_set_stream_ccr(client_connect_request_t *ccr, const char *netdev,
                         avbtp_subtype_t subtype, bool talker, uint16_t vid,
                         uint8_t pcp, uint32_t max_frm_sz,
                         uint32_t time_intv, uint32_t max_intv_frm);

/**
 * @brief check the packet timestamp to avb time
 * @param last_pts	timestamp at the last call
 * @param rsdinfo	information from a stream packet
 * @return bit0-3:indicate 'normal', 'not valid', 'negative move' or 'big move'
 	   bit4-7:indicate 'in past' or 'in future'
 */
int avbtp_rec_ts_check(int64_t *last_pts, avbtp_sd_info_t *rsdinfo);

/**
 * @brief Set IEC header values from avbtp_sd_iec_info_t structure
 * @param cmshavbtp_cm_control_header
 * @param iec input value to be written in to the header
 * @return -1: error, 0: success
 */
int avbtp_set_cmsh_iec_info(avbtp_cm_stream_header_t *cmsh, avbtp_sd_iec_info_t *iec);

/**
 * @brief Set IEC CIP header values from avbtp_iec_cip_info_t structure
 * @param payload destination payload
 * @param payload_size destination size
 * @param cip input value to be written in to the CIP header
 * @return -1: error, 0: success
 */
int avbtp_set_iec_cip_header(uint8_t *payload, uint16_t payload_size, avbtp_iec_cip_info_t *cip);

/**
 * @brief Set IEC SPH header timestamp value
 * @param payload destination payload
 * @param payload_size destination size
 * @param sph input value to be written in to the SPH header
 * @return -1: error, 0: success
 */
int avbtp_set_iec_sph_header(uint8_t *payload, uint16_t payload_size, uint64_t sph);

#endif
/** @}*/
