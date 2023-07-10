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
 * @ingroup  TSN_COMBASE_MODULE
 * @defgroup IPC_shm Shared memory IPC utility functions
 * @{
 * @file cb_ipcshmem.h
 *
 * @brief Shared memory IPC utility functions
 */

#ifndef CB_IPCSHMEM_H_
#define CB_IPCSHMEM_H_

#ifdef CB_IPCSHMEM_NON_POSIX_H
#include CB_IPCSHMEM_NON_POSIX_H
#else
#include <fcntl.h>		   /* For O_* constants */
#include <unistd.h>
#endif

/**
 * @brief get new shared memory
 * @param memfd	shared memory file descriptor
 * @param shmname	shared memory name to be used for shm_open
 * @param size	size of shared memroy
 * @param flag flag to be used for shm_open i.e O_RDONLY, O_RDWR,
 * O_WRONLY, O_CREAT, O_EXCL,O_TRUNC.
 * @see @c man @c shm_open().
 * @return a reference to the new shared memory, NULL if error
 *
 */
void *cb_get_shared_mem(int *memfd, const char *shmname, size_t size, int flag);

/**
 * @brief close shared memory
 * @param mem	mapped shared memory
 * @param memfd	shared memory file descriptor
 * @param shmname	shared memory name
 * @param size	size of shared memory
 * @param unlink flag indicating if shared memory will be unlinked
 * @return 0 on success, -1 on error
 */
int cb_close_shared_mem(void *mem, int *memfd, const char *shmname, size_t size, bool unlink);

#endif
/** @}*/
