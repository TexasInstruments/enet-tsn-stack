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
#ifndef AES3_HELPER_H__
#define AES3_HELPER_H__
#include <tsn_combase/combase.h>

// MAX_AES32_FRAME_LENGTH is calculate base on MAX eac3 frmsiz (2048 bytes) * 2
#define MAX_AES32_FRAME_LENGTH (2048 * 2)
#define SMPTE_PREAMBLE_LENGTH (12)

/// @brief 
/// In talker mode, file -> ec3 sync-frame -> smpte337_frame -> aes3org -> aes3aaf -> uint8_t* buf -> send out fragments
/// In listener mode, rx uint8_t payload -> aes3aaf -> smpte337_frame -> ec3 sync-frame -> file
typedef struct smpte337_frame
{
	// first 12 bytes is smpte337_preamble and next is 1536 bytes
	uint8_t buffer[SMPTE_PREAMBLE_LENGTH + MAX_AES32_FRAME_LENGTH];
    uint32_t rx_index; // use to indicate current smpte buffer index

	// IMPORTANT NOTICE: even SMPTE337 data burst is trying to fit to AES3 frame/subframe format (with Preamble and VUCP bits)
    // However, the word length is not limitted to max AES data size (of 192 frames) (192*2*4). 
    // It means, the SMPTE337 decoder MUST take responsibility of collecting enough SMPTE337 data burst base on it's word length.
    // For ex:
    //      Dolby EC3 frmsiz in max case of 11bit is 2047 => sync frame size = (2047+1) * 2 = 4096 bytes.
    //      Together with 12 bytes of SMPTE337 preamble, total SMPTE data burst is 4096 + 12 = 4108 bytes.
    //      In simplest case (24 bits audio), we will ultilize 3 bytes per one sub-frame (another byte is for preamble and VUCP) or 6 bytes per frame
    //      ==> We need 4108 / 6 = 684.66 frames (we need at least 685 frames).
    //      Together with requirement, that (1) the number of frames should be constant and (2) MAX MTU is 1500 bytes
    //      ==> We can separate 684.66 to 8 fragments, each fragment contain 96 AES3 frames.
    //         Fragment #0: | SMPTE Preamble 2 frames| 94 AES3 frames audio|
    //         Fragment #1: |           96 AES3 frames audio               |
    //         Fragment #2: |           96 AES3 frames audio               |
    //         Fragment #3: |           96 AES3 frames audio               |
    //         Fragment #4: |           96 AES3 frames audio               |
    //         Fragment #5: |           96 AES3 frames audio               |
    //         Fragment #6: |           96 AES3 frames audio               | <- reach 672 frames
    //         Fragment #7: | Remain 12.66 AES3 frames|.....unused data....| <- After reach enough data, SMPTE337 should take responsibility of combine 337frame
    //
    //  This example is applying very simple data of frmsiz is (383+1)*2=768 bytes + 12 SMPTE337 preamble header = 780 bytes
    //         => We need 780 / 6 = 130 frames and can be send within one packet without fragmentation.
    //                      | SMPTE Preamble 2 frames| 128 AES3 frames audio|
	uint32_t word_length; // in bit
} smpte337_frame_t;

/// @brief Set Dolby EC3 word length to dummy smpte337 preamble infos
/// @param frmsiz Frame size is extract from sync-frame 'bsi' info
///   data length is frmsiz * 2 (bytes)
void set_smpte_preamble_word_length(uint32_t frmsiz);

/// @brief Reset TXRX local info for next TXRX
void reset_aes3_txrx_info();

/// @brief Handle AES3 RX
///   Note: This is very simple logic which expecting SMPTE337 data burst data size should <= MAX MTU size
///   As mentioned, SMPTE337 decoder MUST take responsibility of collecting enough SMPTE337 data burst base on it's word length.
/// @param payload 
/// @param plsize 
/// @return 
int handle_aes3_rx(uint8_t *payload, int plsize);

typedef enum
{
    RX_INVALID,
    RX_WAIT_ANOTHER_PAYLOAD,
    RX_DONE
} AES3_RX_RET;
AES3_RX_RET fill_aes3_rx(uint8_t *payload, int plsize);

/// @brief Convert from Dolby sync-frame -> SMPTE337 -> AES3 ORG -> AES3 AAF frame
/// @param audio_buf 
/// @param size 
void syncframe_to_aes3aaf(uint8_t* audio_buf, int size);

/// @brief Get output frame337 from handle_aes3_rx APIs
/// @return 
smpte337_frame_t* get_frame337();

/// @brief Get output AES3 AAF frame from syncframe_to_aes3aaf
/// @return 
uint8_t* get_frameaaf();
#endif