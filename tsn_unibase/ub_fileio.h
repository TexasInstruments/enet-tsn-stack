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
 * @defgroup fileio File I/O
 * @{
 * @file ub_fileio.h
 *
 * @brief Header file for file I/O functions
 */

#ifndef UB_FILEIO_H_
#define UB_FILEIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function pointer type for opening a file
 * @param name Name of the file to open
 * @param mode Mode to open the file in
 * @return Pointer to the opened file
 */
typedef void*(*ub_fileio_open)(const char *name, const char *mode);

/**
 * @brief Function pointer type for closing a file
 * @param fio Pointer to the file to close
 * @return 0 on success, -1 on failure
 */
typedef int(*ub_fileio_close)(void *fio);

/**
 * @brief Function pointer type for reading from a file
 * @param fio Pointer to the file to read from
 * @param ptr Pointer to the buffer to read into
 * @param size Number of bytes to read
 * @return Number of bytes read on success, -1 on failure
 */
typedef int(*ub_fileio_read)(void *fio, void *ptr, int size);

/**
 * @brief Function pointer type for writing to a file
 * @param fio Pointer to the file to write to
 * @param ptr Pointer to the buffer to write from
 * @param size Number of bytes to write
 * @return Number of bytes written on success, -1 on failure
 */
typedef	int(*ub_fileio_write)(void *fio, const void *ptr, int size);

/**
 * @brief Function pointer type for seeking within a file
 * @param fio Pointer to the file to seek within
 * @param offset Number of bytes to seek
 * @return 0 on success, -1 on failure
 */
typedef	int(*ub_fileio_seek)(void *fio, int offset);

/**
 * @brief Struct containing function pointers for file I/O operations
 */
typedef struct ub_fiocb_set {
	ub_fileio_open open;	/**< Function pointer for opening a file */
	ub_fileio_close close;	/**< Function pointer for closing a file */
	ub_fileio_read read;	/**< Function pointer for reading from a file */
	ub_fileio_write write;	/**< Function pointer for writing to a file */
	ub_fileio_seek seek;	/**< Function pointer for seeking within a file */
} ub_fiocb_set_t;

/**
 * @brief Initializes the file I/O function pointers
 * @param fileio_set Pointer to the struct containing the function pointers
 * @return 0 on success, -1 on failure
 */
int ub_fioinit(ub_fiocb_set_t *fileio_set);

/**
 * @brief Opens a file
 * @param name Name of the file to open
 * @param mode Mode to open the file in ("r", "w", or "rw")
 * @return Pointer to the opened file
 */
void* ub_fioopen(const char *name, const char *mode);

/**
 * @brief Closes a file
 * @param fio Pointer to the file to close
 * @return 0 on success, -1 on failure
 */
int ub_fioclose(void *fio);

/**
 * @brief Reads from a file
 * @param fio Pointer to the file to read from
 * @param ptr Pointer to the buffer to read into
 * @param size Number of bytes to read
 * @return Number of bytes read on success, -1 on failure
 */
int ub_fioread(void *fio, void *ptr, int size);

/**
 * @brief Writes to a file
 * @param fio Pointer to the file to write to
 * @param ptr Pointer to the buffer to write from
 * @param size Number of bytes to write
 * @return Number of bytes written on success, -1 on failure
 */
int ub_fiowrite(void *fio, const void *ptr, int size);

/**
 * @brief Seeks within a file
 * @param fio Pointer to the file to seek within
 * @param offset Number of bytes to seek
 * @return 0 on success, -1 on failure
 */
int ub_fioseek(void *fio, int offset);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
