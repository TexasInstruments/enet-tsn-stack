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
 * @ingroup TSN_UNIBASE_MODULE
 * @defgroup unibase_binding Bind unibase to a specific platform
 * @{
 * @file unibase_binding.h
 *
 * @brief functions to bind unibase to a specific platform
 *
 * - This layer support binding to a specific platform.
 * - The implementation is platform specific.
 * - Using this binding is optional, and for the unibase library,
 * a diferent binding layer can be used.
 */

#ifndef UNIBASE_BINDING_H_
#define UNIBASE_BINDING_H_

/**
 * @brief ubb_memory_out_init uses this value to allocate internal buffer when
 *	'mem' parameter is NULL.
 */
#define UBB_DEFAULT_DEBUG_LOG_MEMORY (64*1024)

/**
 * @brief ubb_memory_out always add this end mark at the end of printing.
 */
#define UBB_MEMOUT_ENDMARK "---###---"

/**
 * @brief ubb_default_initpara uses this string as 'ub_log_initstr'
 */
#define UBB_DEFAULT_LOG_INITSTR "4,ubase:45"

/**
 * @brief get a string from an environment variable
 */
#ifndef UBB_GETENV
#define UBB_GETENV getenv
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize the internal memory_out function
 * @param mem	use this memory as a buffer for memory_out.
 *	if mem is NULL, allocate memory internally for 'size' bytes
 * @param size	size of memory to be used for memory_out
 * @return 0 on success, -1 on error
 * @note if this is not explicitly called, it is called internally with mem=NULL
 *	and size=UBB_DEFAULT_DEBUG_LOG_MEMORY
 */
int ubb_memory_out_init(char *mem, int size);

/**
 * @brief de-initialize memory_out function
 * @return 0 on success, -1 on error
 * @note this must be called whichever case of the explicit call or
 * 	the internal call of 'ubb_memory_out_init'
 */
int ubb_memory_out_close(void);

/**
 * @brief return the memory pointer of memory_out
 * @return memory pointer
 */
char *ubb_memory_out_buffer(void);

/**
 * @brief return the most recent output line in memory_out
 * @param str	pointer of string pointer to return result string
 * @param size	pointer of integer pointer to return result size
 * @return 0 on success, -1 on error
 */
int ubb_memory_out_lastline(char **str, int *size);

/**
 * @brief return the all written line data in the buffer
 * @param rstr	allocated pointer of the returned data
 * @param size	size of the returned data
 * @return 0 on success, -1 on error
 * @note returned pointer in *str must be freed by the caller.
 * 	the allocation size is the same as the size in ubb_memory_out_init
 */
int ubb_memory_out_alldata(char **rstr, int *size);

/**
 * @brief write the data in memory_out buffer into a file
 * @param fname	file name
 * @return 0 on success, -1 on error
 */
int ubb_memory_file_out(const char *fname);

/**
 * @brief return the default initialization parameters supported in the binding layer
 * @param init_para	pointer of unibase initialization parameter
 */
void ubb_default_initpara(unibase_init_para_t *init_para);

typedef uint64_t(*get64ts_t)(void);
/**
 * @brief set gptptime function for ub_gptp_gettime64
 * @param func	a function which returns 64-bit gptp time
 */
void ubb_set_gptp_gettime64(get64ts_t func);

/**
 * @brief initialize unibase with 'ubb_default_initpara'
 * @note ubb_memory_out is initialize with no memory. To use memory logging,
 *       'ubb_memory_out_init' needs to be called separately
 */
void ubb_unibase_easyinit(void);

#ifdef __cplusplus
}
#endif

#endif
/** @}*/
