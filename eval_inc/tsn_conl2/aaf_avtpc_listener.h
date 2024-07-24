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
/***********************************************************************************
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
***********************************************************************************/

/**
 * @addtogroup xl4avbconl2
 * @{
 * @file aaf_avtpc_listener.h
 * @brief AVB Transport Protocol Client, AAF listener connection, header file
 *
 */

#ifndef __AAF_AVTPC_LISTENER_H_
#define __AAF_AVTPC_LISTENER_H_

#include "tsn_l2/avtpc.h"
#include "conl2_avtpc_listener.h"

typedef struct aaf_avtpc_listener_data aaf_avtpc_listener_data_t;
/**
 * @brief pcm information parameters. this structure contains following members:
 *   1. srate -> Sample Rate. the value is one of the followings:
 *	8000, 16000, 32000, 44100, 48000, 88200, 96000, 176400, 192000, 24000
 *      \n
 *   2. format -> the format of the data.
 *	the value is one of values in the following Table:
 *        Value      |    Name        | Description           | AAF AVTPDU Format
 *   :--------------:|:--------------:|:---------------------:|:------------------:
 *      0x0          |   USER         | User Specified        |     PCM
 *      0x1          |  FLOAT_32BIT   | 32 bit floating point |     PCM
 *      0x2          |   INT_32BIT    | 32 bit integer        |     PCM
 *      0x3          |   INT_24BIT    | 24 bit integer        |     PCM
 *      0x4          |   INT_16BIT    | 16 bit integer        |     PCM
 *      0x5          |   AES3_32      | 32 bit AES3 format    |     AES3
 *      0x6 to 0xFF  |   ----------   | Reserved              |     N/A
 *      \n
 *   3. channels -> the number of audio channels.
 *      \n
 *   4. "bit_depth" -> bit depth of audio data sample.
 *   	If the format field is set to a FLOAT_32BIT then the "bit_depth" field is 32.\n
 *	All unused data bits in a sample are set to 0.
 *      \n
 * @see avbtp_aaf_format_t
 */
typedef struct aaf_avtpc_pcminfo{
	uint32_t srate;
	avbtp_aaf_format_t format;
	uint16_t channels;
	uint8_t bit_depth;
}aaf_avtpc_pcminfo_t;

typedef enum
{
	DT_UNSPECIFIED = 0,
	DT_PCM,
	DT_SMPTE338,
	DT_IEC61937,
	DT_VENDOR,
	DT_RESERVED1, // 5
	DT_RESERVED2, // 6
	DT_RESERVED3, // 7
} aes3_org_dt_ref_val_t;

/**
 * @brief pcm information parameters. this structure contains following members:
 *   1. frate -> Frame rate. regardless of the sample rate of any audio samples carried within the AES3 frames
 *	8000, 16000, 32000, 44100, 48000, 88200, 96000, 176400, 192000, 24000
 *      \n
 *   2. format -> refer aaf_avtpc_pcminfo_t, but AES3_32 must be used
 *      \n
 *   3. streams_per_frame -> the number of streams per frame
 *      \n
 *   4. "aes3_data_type_h, aes3_data_type_l" -> TODO
 *      \n
 *   5. "aes3_dt_ref" -> bit depth of audio data sample.
 *      \n
 *	All unused data bits in a sample are set to 0.
 *      \n
 * @see aaf_avtpc_aesinfo_t
 */
typedef struct aaf_avtpc_aesinfo{
	uint32_t frate;
	avbtp_aaf_format_t format;
	uint16_t streams_per_frame;
	uint8_t aes3_data_type_h; // in case DT_SMPTE338, this value is not used
	uint8_t aes3_data_type_l; // in case DT_SMPTE338, this value is not used
	uint8_t aes3_dt_ref; // must be DT_SMPTE338
}aaf_avtpc_aesinfo_t;

// MAX AES3 Frame number is calculated by eac3 frmsiz max 2048 * 2 = 4096 bytes length.
// 4096 bytes in case of AES3 bit per subframe is 24 bit (6/8 frame)
// => we need roundup(4096/6) = 683 AES3 frame
#define AES3_FRAMES_NUMBER		683

#define AES3_X_PREAMBLE_VALUE	0x05                 // 0101b
#define AES3_Y_PREAMBLE_VALUE	0x06                 // 0110b same w/ Z, but sub-frame1
#define AES3_Z_PREAMBLE_VALUE	AES3_Y_PREAMBLE_VALUE // same w/ Y, but sub-frame0 so no problem

// To diffirentiate AAF AES3 and AES3 Original structure, naming will be:
//  aes3_aaf: aes3 format but follow AAF avtp standard |reserved| BCUV | 24 bit audio sample word|
//  aes3_org: aes3 original format                     |preamble| 24 bit audio sample word| VUCP |

typedef struct aes3_aaf_subframe{
	// |reserved 4 bits| BCUV 4 bits | 24 bit audio sample word|
	uint8_t data[4];
} aes3_aaf_subframe_t;
UB_ABIT8_FIELD(aes3_aaf_b, 3, 0x01)
UB_ABIT8_FIELD(aes3_aaf_c, 2, 0x01)
UB_ABIT8_FIELD(aes3_aaf_u, 1, 0x01)
UB_ABIT8_FIELD(aes3_aaf_v, 0, 0x01)

typedef struct aes3_aaf_frame
{
	aes3_aaf_subframe_t subframe[2];
} aes3_aaf_frame_t;

typedef struct aes3_aaf_stream{
	uint8_t stream_id;
	aes3_aaf_frame_t frames[AES3_FRAMES_NUMBER];
} aes3_aaf_stream_t;

// User can input array of AES subframe
typedef struct aes3_org_subframe{
	//|preamble 4 bits| 24 bit audio sample word| VUCP 4 bits|
	uint8_t data[4];
} aes3_org_subframe_t;
UB_ABIT8_FIELD(aes3_org_v, 3, 0x01)
UB_ABIT8_FIELD(aes3_org_u, 2, 0x01)
UB_ABIT8_FIELD(aes3_org_c, 1, 0x01)
UB_ABIT8_FIELD(aes3_org_p, 0, 0x01)
UB_ABIT8_FIELD(aes3_org_preamble, 4, 0x0F)

typedef struct aes3_org_frame{
	aes3_org_subframe_t subframe[2];
} aes3_org_frame_t;

typedef struct aes3_org_stream{
	uint8_t stream_id;
	aes3_org_frame_t frames[AES3_FRAMES_NUMBER];
} aes3_org_stream_t;

// Util APIs to get aes3 original infos
void aes3_org_set_asw(aes3_org_subframe_t* subframe, uint8_t* in_data); // asw = audio sample word
void aes3_org_get_asw(aes3_org_subframe_t* subframe, uint8_t* out_data);
void aes3_aaf_set_asw(aes3_aaf_subframe_t* subframe, uint8_t* in_data); // asw = audio sample word
void aes3_aaf_get_asw(aes3_aaf_subframe_t* subframe, uint8_t* out_data);

int aes3_org_stream_to_aes3_aaf_stream(aes3_org_stream_t* aesstream, aes3_aaf_stream_t* aafstream);
int aes3_aaf_stream_to_aes3_org_stream(aes3_aaf_stream_t* aafstream, aes3_org_stream_t* aesstream);

/**
 * @brief initialize aaf_avtpc_listener
 * @param bcp	connection parameters
 * @param rec_cb	data receiver callback function
 * @param rec_cbdata	data handler to pass the callback function
 * @return the data handle of aaf_avtpc_listener
 * @see conl2_basic_conparas_t
 * @see avbtp_rcv_cb_t
 *
 */
aaf_avtpc_listener_data_t *aaf_avtpc_listener_init(conl2_basic_conparas_t *bcp,
						   avbtp_rcv_cb_t rec_cb,
						   void *rec_cbdata);

/**
 * @brief close aaf_avtpc_listener
 * @param aaflad	the data handle of aaf_avtpc_listener
 * @see aaf_avtpc_listener_data_t
 *
 */
void aaf_avtpc_listener_close(aaf_avtpc_listener_data_t *aaflad);

/**
 * @brief get pcm information from the header
 * @param rsdinfo	information in 1722 packet
 * @param pcminfo	return the difference between the schedule time and current avb time
 * @return 0 if pcminfo is set, -1 if not valid format
 * @see avbtp_sd_info_t
 * @see aaf_avtpc_pcminfo_t
 *
 */
int aaf_avtpc_get_pcm_info(avbtp_sd_info_t *rsdinfo, aaf_avtpc_pcminfo_t *pcminfo);

/**
 * @brief get aes information from the header
 * @param rsdinfo	information in 1722 packet
 * @param aesinfo	return the difference between the schedule time and current avb time
 * @return 0 if pcminfo is set, -1 if not valid format
 * @see avbtp_sd_info_t
 * @see aaf_avtpc_pcminfo_t
 *
 */
int aaf_avtpc_get_aes3_info(avbtp_sd_info_t *rsdinfo, aaf_avtpc_aesinfo_t *aesinfo);

/**
 * @brief Search for the frame index in tx aesorg stream that contain block started
 * @param aesstream	rx aafaesstream 
 * @return found index or -1 if not found
 *
 */
int find_start_block_index_aes3_org_stream(aes3_org_stream_t* aesstream);

/**
 * @brief Search for the frame index in rx aafaesstream that contain block started
 * @param aafaesstream	rx aafaesstream 
 * @return found index or -1 if not found
 *
 */
int find_start_block_index_aaf_stream(aes3_aaf_stream_t* aafaesstream);

/**
 * @brief check timestamp
 * @param rsdinfo	information in 1722 packet
 * @param dts	return the difference between the schedule time and current avb time
 * if timestamp is valid, set to NULL to ignore dts calculation.
 * @return returns 0 if timestamp is valid, -1 if not valid timestamp
 * @see avbtp_sd_info_t
 *
 */
int aaf_avtpc_listener_checkts(avbtp_sd_info_t *rsdinfo, int64_t *dts);

#endif

/** @}*/
