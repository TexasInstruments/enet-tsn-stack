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
 * @file ub_getmem.h
 * @brief Header file for Static and Dynamic memory allocation functions
 */

#ifndef UB_GETMEM_H_
#define UB_GETMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Concatenates two tokens
 * @param A First token
 * @param B Second token
 * @return Concatenated tokens
 */
#define UB_CONCAT2(A, B) UB_CONCAT2_(A, B)

/**
 * @brief Helper macro for UB_CONCAT2
 * @param A First token
 * @param B Second token
 * @return Concatenated tokens
 */
#define UB_CONCAT2_(A, B) A##B

/**
 * @brief Aligns a memory address to the nearest multiple of sizeof(void*)
 * @param x Memory address to align
 * @return Aligned memory address
 */
#define UB_SD_ALIGN(x) ((((uint32_t)(x))+(sizeof(void*)-1u))&(~(sizeof(void*)-1u)))

#ifdef UB_SD_STATIC

/**
 * @brief Allocates memory from a static memory pool
 * @param size Size of memory to allocate
 * @param mem Pointer to the static memory pool
 * @param busysizes Array of busy sizes for each fragment in the memory pool
 * @param fragnum Number of fragments in the memory pool
 * @param fragsize Size of each fragment in the memory pool
 * @param mname Name of the memory pool
 * @param nolock Whether to use a lock to protect the memory pool
 * @return Pointer to the allocated memory
 */
void *ub_static_getmem(size_t size, void *mem, uint16_t* busysizes,
			 int fragnum, uint16_t fragsize, const char *mname, bool nolock);

/**
 * @brief Reallocates memory from a static memory pool
 * @param p Pointer to the memory to reallocate
 * @param nsize New size of the memory
 * @param mem Pointer to the static memory pool
 * @param busysizes Array of busy sizes for each fragment in the memory pool
 * @param fragnum Number of fragments in the memory pool
 * @param fragsize Size of each fragment in the memory pool
 * @param mname Name of the memory pool
 * @return Pointer to the reallocated memory
 */
void *ub_static_regetmem(void *p, size_t nsize, void *mem, uint16_t *busysizes,
			 int fragnum, uint16_t fragsize, const char *mname);

/**
 * @brief Frees memory from a static memory pool
 * @param p Pointer to the memory to free
 * @param mem Pointer to the static memory pool
 * @param busysizes Array of busy sizes for each fragment in the memory pool
 * @param fragnum Number of fragments in the memory pool
 * @param fragsize Size of each fragment in the memory pool
 * @param mname Name of the memory pool
 */
void ub_static_relmem(void *p, void *mem, uint16_t *busysizes,
		      int fragnum, uint16_t fragsize, const char *mname);

/**
 * @brief Prints memory usage if the log level is enabled.
 *
 * @param busysizes Array of busy sizes for each fragment in the memory pool
 * @param fragnum Number of fragments in the memory pool
 * @param fragsize Size of each fragment in the memory pool
 * @param mname Name of the memory pool
 * @param nolock Whether to use a lock to protect the memory pool
 * @param level The log level
 * @return Number of freed frags
 */
int ub_static_print_usage(uint16_t* busysizes, int fragnum,
		uint16_t fragsize, const char *mname, bool nolock, ub_dbgmsg_level_t level);

/**
 * @brief Defines a static memory pool and associated memory allocation functions
 * @param NAME Name of the memory pool
 * @param FRAGSIZE Size of each fragment in the memory pool
 * @param FRAGNUM Number of fragments in the memory pool
 */
#define UB_SD_GETMEM_DEF(NAME, FRAGSIZE, FRAGNUM)			\
	static uint8_t UB_CONCAT2(NAME, mem)[UB_SD_ALIGN(FRAGSIZE)*(uint32_t)(FRAGNUM)] \
		__attribute__ ((aligned (sizeof(void*))));		\
	static uint16_t UB_CONCAT2(NAME, busysizes)[FRAGNUM]		\
		__attribute__ ((aligned (sizeof(void*))));		\
void *UB_CONCAT2(static_getmem, NAME)(size_t size)			\
{									\
	void *m;							\
	m=ub_static_getmem(size, UB_CONCAT2(NAME, mem), UB_CONCAT2(NAME, busysizes),\
			   FRAGNUM, (uint16_t)UB_SD_ALIGN(FRAGSIZE), #NAME, false); \
	return m;							\
}									\
void *UB_CONCAT2(static_regetmem, NAME)(void *p, size_t nsize)		\
{									\
	void *m;							\
	m=ub_static_regetmem(p, nsize, UB_CONCAT2(NAME, mem), UB_CONCAT2(NAME, busysizes), \
			    FRAGNUM, (uint16_t)UB_SD_ALIGN(FRAGSIZE), #NAME); \
	return m;							\
}									\
void UB_CONCAT2(static_relmem, NAME)(void *p)				\
{									\
	return ub_static_relmem(p, UB_CONCAT2(NAME, mem), UB_CONCAT2(NAME, busysizes), \
			    FRAGNUM, (uint16_t)UB_SD_ALIGN(FRAGSIZE), #NAME); \
}															 \
int UB_CONCAT2(static_print_usage, NAME)(ub_dbgmsg_level_t level)		\
{																		\
	return ub_static_print_usage(UB_CONCAT2(NAME, busysizes),			\
				FRAGNUM, (uint16_t)UB_SD_ALIGN(FRAGSIZE), #NAME, false, level); \
}

/**
 * @brief Declares the memory allocation functions for a static memory pool
 * @param NAME Name of the memory pool
 */
#define UB_SD_GETMEM_DEF_EXTERN(NAME)				\
extern void *UB_CONCAT2(static_getmem, NAME)(size_t size);	\
extern void *UB_CONCAT2(static_regetmem, NAME)(void *p, size_t nsize);	\
extern void UB_CONCAT2(static_relmem, NAME)(void *p);					\
extern int UB_CONCAT2(static_print_usage, NAME)(ub_dbgmsg_level_t level)

/**
 * @brief Allocates memory from a static or dynamic memory pool
 * @param name Name of the memory pool
 * @param size Size of memory to allocate
 * @return Pointer to the allocated memory
 */
#define UB_SD_GETMEM(name, size) UB_CONCAT2(static_getmem, name)(size)

/**
 * @brief Reallocates memory from a static or dynamic memory pool
 * @param name Name of the memory pool
 * @param p Pointer to the memory to reallocate
 * @param size New size of the memory
 * @return Pointer to the reallocated memory
 */
#define UB_SD_REGETMEM(name, p, size) UB_CONCAT2(static_regetmem, name)(p, size)

/**
 * @brief Frees memory from a static or dynamic memory pool
 * @param name Name of the memory pool
 * @param p Pointer to the memory to free
 */
#define UB_SD_RELMEM(name, p) UB_CONCAT2(static_relmem, name)(p)

/**
 * @brief Prints memory usage if the log level is enabled.
 *
 * @param name Name of the memory pool
 * @param level The log level
 * @return Number of freed frags
 */
#define UB_SD_PRINT_USAGE(name, level) UB_CONCAT2(static_print_usage, name)(level)

#else
#define UB_SD_GETMEM_DEF(NAME, FRAGSIZE, FRAGNUM)
#define UB_SD_GETMEM_DEF_EXTERN(NAME)
#define UB_SD_PRINT_USAGE(name, level)

/**
 * @brief Allocates memory from the system heap
 * @param name Name of the memory pool (unused)
 * @param size Size of memory to allocate
 * @return Pointer to the allocated memory
 */
#define UB_SD_GETMEM(name, size) malloc(size)

/**
 * @brief Reallocates memory from the system heap
 * @param name Name of the memory pool (unused)
 * @param p Pointer to the memory to reallocate
 * @param size New size of the memory
 * @return Pointer to the reallocated memory
 */
#define UB_SD_REGETMEM(name, p, size) realloc(p, size)

/**
 * @brief Frees memory from the system heap
 * @param name Name of the memory pool (unused)
 * @param p Pointer to the memory to free
 */
#define UB_SD_RELMEM(name, p) free(p)

#endif

#ifdef __cplusplus
}
#endif

#endif
