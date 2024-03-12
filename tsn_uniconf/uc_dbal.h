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
#ifndef UC_DBAL_H_
#define UC_DBAL_H_

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void uc_dbald;
typedef void uc_range;

/* the size shouldn't be very big */
#define UC_MAX_KEYSIZE 128
#define UC_MAX_VALUEKEYS 8 // max number of value keys
#define UC_MAX_VALUESIZE 320

#ifndef UC_MAX_AP_DEPTH
#define UC_MAX_AP_DEPTH 16
#endif
#ifndef UC_MAX_KV_DEPTH
#define UC_MAX_KV_DEPTH 10
#endif

#define UC_CALLMODE_UNICONF (1u<<0u)
#define UC_CALLMODE_THREAD (1u<<1u)
#define UC_CALL_UNICONF(x) ((x)&UC_CALLMODE_UNICONF)
#define UC_CALL_CLIENT(x) (!((x)&UC_CALLMODE_UNICONF))
#define UC_CALL_THREAD(x) ((x)&UC_CALLMODE_THREAD)
#define UC_CALL_THREADSLAVE(x) (UC_CALL_CLIENT(x) && UC_CALL_THREAD(x))

// comarison with a shorter size of key
// if matched, a longer key is bigger.
static inline int uc_dbal_keycmp(uint8_t *key1, uint32_t ksize1,
				 uint8_t *key2, uint32_t ksize2)
{
	int res;
	if(((key1!=NULL) && (ksize1>0u)) && ((key2==NULL) || (ksize2==0u))){return 1;} // non-NULL > NULL
	if(((key1==NULL) || (ksize1==0u)) && ((key2!=NULL) && (ksize2>0u))){return -1;} // NULL < non-NULL
	if(ksize1 > ksize2){
		res=memcmp(key1, key2, ksize2);
		if(res==0){return 1;} // existing data is the same, a has a longer key
		return res; // comparison with a shorter size
	}
	if(ksize1 < ksize2){
		res=memcmp(key1, key2, ksize1);
		if(res==0){return -1;} // existing data is the same, b has a longer key
		return res; // comparison with a shorter size
	}
	// the both keys have the same size
	return memcmp(key1, key2, ksize1);
}

// 'mode' including 'm' is a monitor mode, and no error message is printed
uc_dbald *uc_dbal_open(const char *pfname, const char *mode, uint8_t callmode);

void uc_dbal_close(uc_dbald *dbald, uint8_t callmode);

int uc_dbal_getdb(uc_dbald *dbald, int toutms, uint8_t *key, uint32_t ksize);

void uc_dbal_releasedb(uc_dbald *dbald);

int uc_dbal_save(uc_dbald *dbald);

// Update. if the key node doesn't exist, create a new node with the value.
int uc_dbal_create(uc_dbald *dbald, void *key, uint32_t ksize, void *value,
		   uint32_t vsize);

// create if the key doesn't exists, append if the key exists
int uc_dbal_append(uc_dbald *dbald, void *key, uint32_t ksize, void *value,
		   uint32_t vsize);

int uc_dbal_get(uc_dbald *dbald, void *key, uint32_t ksize, void **value,
		uint32_t *vsize);

int uc_dbal_get_release(uc_dbald *dbald, void *key, uint32_t ksize,
			 void *value, uint32_t vsize);

int uc_dbal_del(uc_dbald *dbald, void *key, uint32_t ksize);

// get the range as 'key1 <= range <= key2'
uc_range *uc_get_range(uc_dbald *dbald, void *key1, uint32_t ksize1,
		       void *key2, uint32_t ksize2);

// release the range object
void uc_get_range_release(uc_dbald *dbald, uc_range *range);

#define UC_DBAL_FORWARD 1
#define UC_DBAL_BACKWARD -1
#define UC_DBAL_NOMOVE 0

/*
 * when 'move after operation' happens,
 * 1. if it is at the edge(top or bottom), the position will be lost.
 * 2. after the lost, it needs to recover position by 'uc_move_top/bottom_in_range'
 * 3. when the edge is deleted, the edge moves one inner position.
 * 4. in the case 3, if range has only 1 item, the both edges and current position are lost.
 */

// get a key and value data at the current position,
// then direction=0:no move, 1:next position, -1:previous position
// if kdata=NULL the same as 'uc_get_value_in_range'
// if vdata=NULL the same as 'uc_get_key_in_range'
int uc_get_keyvalue_in_range(uc_dbald *dbald, uc_range *range, void **kdata, uint32_t *ksize,
			     void **vdata, uint32_t *vsize, int direction);

// get a key data at the current position,
// then direction=0:no move, 1:next position, -1:previous position
int uc_get_key_in_range(uc_dbald *dbald, uc_range *range, void **kdata, uint32_t *ksize,
			int direction);

// get a value data at the current position,
// then direction=0:no move, 1:next position, -1:previous position
int uc_get_value_in_range(uc_dbald *dbald, uc_range *range, void **vdata, uint32_t *vsize,
			  int direction);

void uc_move_top_in_range(uc_dbald *dbald, uc_range *range);

void uc_move_bottom_in_range(uc_dbald *dbald, uc_range *range);

// move the position in the range
// direction=0:no move, 1:next position, -1:previous position
int uc_move_in_range(uc_dbald *dbald, uc_range *range, int direction);

// delete the position in the range. move to diction after the deletion
// direction=0:no move, 1:next position, -1:previous position
int uc_del_in_range(uc_dbald *dbald, uc_range *range, int direction);

// wait for relese from any locked status
int uc_wait_release(uc_dbald *dbald, int toutms);

#ifdef __cplusplus
}
#endif

#endif
