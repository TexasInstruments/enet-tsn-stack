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
#ifndef SIMPLEDB_H_
#define SIMPLEDB_H_

#include "tsn_unibase/unibase.h"

#define MAX_NUM_DBLIST 20

typedef struct simpledb_data simpledb_data_t;

typedef struct simpledb_range simpledb_range_t;

typedef struct keydata{
	uint32_t ksize;
	void *kdata;
} simpledb_keydata_t;

typedef struct key_range{
	simpledb_keydata_t *kd1;
	simpledb_keydata_t *kd2;
}key_range_t;

/**
 * @brief open the db. if pfname is set, the config(rw) data is restored from the file.
 */
simpledb_data_t *simpledb_open(const char *pfname);

/**
 * @brief close the db. if pfname is set at the open, the config(rw) data is saved.
 */
void simpledb_close(simpledb_data_t *sdbd);

/**
 * @brief save data to a file
 * @note keyranges must be terminated by NULL
 */
int simpledb_savedata(simpledb_data_t *sdbd, key_range_t **keyranges);

/**
 * @brief put data into the db.
 * @note  if it has CREATE flag, a new item is added.
 *	  if it has APPEND flag, data is added at the bottom when the key exist
 */
#define SIMPLEDB_FLAG_CREATE (1u<<0u)
#define SIMPLEDB_FLAG_APPEND (1u<<1u)
int simpledb_put(simpledb_data_t *sdbd, simpledb_keydata_t *kd, void *value,
		 uint32_t vlen, uint8_t flags);

/**
 * @brief get data from the db
 * @note untill simpledb_get_release is called, the item becomes readonly.
 */
int simpledb_get(simpledb_data_t *sdbd, simpledb_keydata_t *kd,
		 void **value, uint32_t *vlen);

int simpledb_get_release(simpledb_data_t *sdbd, simpledb_keydata_t *kd);

/**
 * @brief delete the key item. need to be unlocked status.
 */
int simpledb_del(simpledb_data_t *sdbd, simpledb_keydata_t *kd);

/**
 * @brief get datarange pointer which the keys are between (key1, key2)
 * @note untill 'simpledb_keylist_release' is called,
 *	 all items in the range are locked and become readonly
 */
simpledb_range_t *simpledb_get_range(simpledb_data_t *sdbd,
				     key_range_t *krange);

void simpledb_get_range_release(simpledb_data_t *sdbd, simpledb_range_t *rangedp);

/**
 * @brief get key or value in the range
 * @note directon set pointer move, 1:forward, 0:stay, -1:backward
 */
#define SIMPLEDB_FORWARD 1
#define SIMPLEDB_BACKWARD -1
#define SIMPLEDB_NOMOVE 0
int simpledb_get_from_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp,
			    void **kdata, uint32_t *ksize, void **vdata, uint32_t *vsize,
			    int direction);

void simpledb_move_top_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp);

void simpledb_move_bottom_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp);

int simpledb_move_in_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp,
			   int direction);

int simpledb_del_in_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp,
			  int direction);

int simpledb_wait_release(simpledb_data_t *sdbd, int toutms);

#endif
