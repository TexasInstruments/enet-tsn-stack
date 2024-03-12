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
 * @addtogroup gptp
 * @{
 * @file gptpmasterclock.h
 * @brief file contains gptp master clock related functions.
 *
 */

#ifndef GPTPMASTERCLOCK_H_
#define GPTPMASTERCLOCK_H_

/**
 * @brief initialize to get gptp clock from tsn_gptp daemon.
 * if previously initialized, it will simply return 0.
 * @param shmem_name	shared memory node name. set NULL to use the default
 * @return -1 on error, 0 on Successful initialization.
 * @note   argument 'shmem_name' will not be used in platforms that recommends against
 * using shared memory (e.g GHS INTEGRITY). Pass NULL is such case.
 */
int gptpmasterclock_init(const char *shmem_name);

/**
 * @brief close gptpmasterclcock
 * @return -1: on error, 0:on successfull
 */
int gptpmasterclock_close(void);

/**
 * @brief return the domainIndex which is currently used as systeme wide gptp clock.
 * @return domainIndex, -1: error
 */
int gptpmasterclock_gm_domainIndex(void);

/**
 * @brief get 64-bit nsec unit ts of system wide gptp clock
 * @return 0 on success, -1 on error
 *
 */
int64_t gptpmasterclock_getts64(void);

/**
 * @brief Wait until tts comes.
 * @return -1: error, 1:already passed, 2:in vclose, 3:farther than toofar seconds,
 * 0:returns from nanosleep(has been waited to very close timing)
 * @param tts	target time in nano second unit
 * @param vclose        nano second unit; treated as very close, and stop waiting
 * even ttv is still in future
 * @param toofar        nano second unit; treated as too far, and stop waiting
 */
int gptpmasterclock_wait_until_ts64(int64_t tts, int64_t vclose, int64_t toofar);

/**
 * @brief expand 32-bit nsec time to 64 bit with aligning to gptp clock.
 * @param timestamp timestamp which we are going to convert into 32bit to 64 bit.
 * @return expanded time
 * @note        a range of  -2.147 to 2.147 secconds can be correctly aligned
 *
 */
uint64_t gptpmasterclock_expand_timestamp(uint32_t timestamp);

/**
 * @brief expand 32-bit nsec time to 64 bit with aligning to
 * gptp clock at the time `ts64`.
 * @param timestamp timestamp which we are going to convert into 32bit to 64 bit.
 * @param ts64 the gptp clock time that the `timestamp` will be aligned to.
 * @return expanded time
 * @note        a range of  -2.147 to 2.147 secconds can be correctly aligned
 *
 */
uint64_t gptpmasterclock_expand_timestamp_ts64(uint32_t timestamp, int64_t ts64);

/**
 * @brief get GM change indicator, the number is incremented whenever GM is changed
 * @return GM change indicator value, -1: on error
 */
int gptpmasterclock_gmchange_ind(void);

/**
 * @brief get maximum number of domains
 * @return returns available number of domains.
 */
int gptpmasterclock_get_max_domains(void);

/**
 * @brief get a synchronized clock value on specific domain
 * @param ts64	pointer to return clock value
 * @param domainIndex domain index number
 * @return 0 on success, -1 on error.
 */
int gptpmasterclock_get_domain_ts64(int64_t *ts64, int domainIndex);

/**
 * @brief print phase offset for all domains
 */
void gptpmasterclock_dump_offset(void);

#endif
/** @}*/
