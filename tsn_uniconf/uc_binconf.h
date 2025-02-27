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
#ifndef UC_BINCONF_H_
#define UC_BINCONF_H_

#include "uc_dbal.h"
#include "hal/uc_hwal.h"

typedef struct uc_bcdata uc_bcdata_t;

/**
 * @brief initialize 'uc_binconf'
 * @param chunk_size internal chunkdata memory size to handle one chunk of data
 * @note when chunk_size is smaller than maximum one yang item size,
 *       the append mode of the DB access is used.
 */
uc_bcdata_t *uc_binconf_init(uint32_t chunk_size);

/**
 * @brief close 'uc_binconf'
 */
void uc_binconf_close(uc_bcdata_t *ucbcd);

/**
 * @brief set/reset hwald, to reset use hwald=NULL
 * @note setting hwald, hw action is invoked with uc_binconf_write_chunk.
 */
void uc_binconf_set_hwald(uc_bcdata_t *ucbcd, uc_hwald *hwald);

/**
 * @brief convert chunkdata buffer into YANG (key,data), and write into the DB
 * @return >=0:number of converted YANG items , -1:error
 * @note this call deletes already converted data in chunkdata, and make a space.
 * @note when a whole value data is not in chunkdata, it writes a part of data,
 *       following consecutive calls write the rest as append mode.
 *       the caller needs to set chunkdata before each call.
 *       the first call returns 1, the following calls return 0.
 */
int uc_binconf_write_chunk(uc_bcdata_t *ucbcd, uc_dbald *dbald);

/**
 * @brief convert chunkdata buffer into YANG (key,data), and pop out to parameters
 * @param key	keydata, internal buffer pointer is returned
 * @param ksize	keydata size
 * @param vdata	value data, internally allocated pointer
 * @param vsize	value data size
 * @param atype	YANG_DB_ACTION_DELETE/APPEND/CREATE is returned
 * @return 0:okay, -1:no more data in chunkdata, >0:non buffered data size
 * @note vdata is allocate by UB_SD_REGETMEM.
 *       vdata must be NULL or UB_SD_GETMEM area.
 *       the caller needs to release vdata memory as UB_SD_RELMEM(YANGINIT_GEN_SMEM, vdata)
 * @note this call deletes already converted data in chunkdata, and make a space.
 * @note when positive number is returned, vdata is allcated for the whole data,
 *       but the number of bytes at the bottom are not filled in vdata.
 *       the caller needs to fill that area before processing the data.
 *       it happens when a value data has a big size.
 */
int uc_binconf_pop_item(uc_bcdata_t *ucbcd, uc_dbald *dbald,
			void **key, uint32_t *ksize, void **vdata, uint32_t *vsize,
			uint8_t *atype);

/**
 * @brief convert (key,value) data to binary, and save into chunkdata buffer
 * @param key	keydata
 * @param ksize	keydata size
 * @param vdata	value data
 * @param vsize	value data size
 * @return 0:okay, 1:no more space in chunkdata buffer, -1:error
 *         2:data can't fit into the chunkdata buffer, set pointer as data
 * @note the binary data has compression based on previous data.
 *       to start initial condition, call uc_binconf_clean_lkey before this call
 */
int uc_binconf_set_kvdata(uc_bcdata_t *ucbcd, uint8_t *key, uint32_t ksize,
			  void *vdata, uint16_t vsize);

/**
 * @brief copy binary data into chunkdata buffer
 * @param bdata	binary data
 * @param bsize	binary data size
 * @return 0:okay, 1:no more space in chunkdata buffer, -1:error
 */
int uc_binconf_set_bindata(uc_bcdata_t *ucbcd, const void *bdata, uint32_t bsize);

/**
 * @brief convert range data to binary, and save into chunkdata buffer
 * @param range	a ranage object
 * @return 0:okay(completed the range), 1:no more space in chunkdata buffer,
 *         2:data can't fit into the chunkdata buffer, set pointer as data
 *         -1:error
 */
int uc_binconf_read_range(uc_bcdata_t *ucbcd, uc_dbald *dbald, uc_range *range);

/**
 * @brief get the interanl chunkdata buffer pointer
 * @param chunkdata	the internal chunkdata pointer is returned
 * @return the size of data in chunkdata
 */
int uc_binconf_get_chunk(uc_bcdata_t *ucbcd, uint8_t **chunkdata);

/**
 * @brief clean up chunkdata
 */
void uc_binconf_clean_chunk(uc_bcdata_t *ucbcd);

/**
 * @brief clean up the internal key buffer.
 */
void uc_binconf_clean_lkey(uc_bcdata_t *ucbcd);

/**
 * @brief read a binary file and set the data into the DB
 * @return 0:okay, -1:errror
 */
int uc_binconf_read_binfile(uc_bcdata_t *ucbcd, uc_dbald *dbald, const char *fname);

/**
 * @brief read a binary data and set the data into the DB
 * @return 0:okay, -1:errror
 */
int uc_binconf_read_bindata(uc_bcdata_t *ucbcd, uc_dbald *dbald,
			    const uint8_t *bdata, int bsize);

/**
 * @brief internally create ucbcd, then call uc_binconf_read_bindata
 * @return 0:okay, -1:errror
 */
int uc_binconf_read_bconffile(uc_dbald *dbald, const char *fname);

#endif
