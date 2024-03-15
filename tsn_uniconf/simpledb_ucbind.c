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
#include <string.h>
#include <stdlib.h>
#include "uc_dbal.h"
#include "simpledb.h"

static simpledb_data_t *sdbd_shared;
static int sdbd_refcounter;

static uc_dbald *opendb_protect(const char *pfname, const char *mode, uint8_t callmode)
{
	simpledb_data_t *sdbd;
	if(UC_CALL_THREADSLAVE(callmode)){
		if(!sdbd_shared){
			if((mode!=NULL) && (strchr(mode, 'm')!=NULL)) return NULL;
			UB_LOG(UBL_ERROR, "%s:'uniconf' side should call first\n",
			       __func__);
			return NULL;
		}
		sdbd_refcounter++;
		return (uc_dbald *)sdbd_shared;
	}
	sdbd=simpledb_open(pfname);
	if(!sdbd){
		if((mode==NULL) || (strchr(mode, 'm')==NULL)){
			UB_LOG(UBL_ERROR, "%s:simpledb_open failed, pfname=%s\n",
			       __func__, pfname);
		}
	}
	sdbd_shared=sdbd;
	sdbd_refcounter++;
	return (uc_dbald *)sdbd;
}

uc_dbald *uc_dbal_open(const char *pfname, const char *mode, uint8_t callmode)
{
	uc_dbald *sdbd;
	UB_PROTECTED_FUNC(opendb_protect, sdbd, pfname, mode, callmode);
	return sdbd;
}

static void closedb_protect(uc_dbald *dbald, uint8_t callmode)
{
	if(sdbd_refcounter==0){return;}
	sdbd_refcounter--;
	if(sdbd_refcounter==0){
		(void)uc_dbal_save(dbald);
		(void)simpledb_close((simpledb_data_t *)dbald);
		sdbd_shared=NULL;
	}
}

void uc_dbal_close(uc_dbald *dbald, uint8_t callmode)
{
	UB_PROTECTED_FUNC_VOID(closedb_protect, dbald, callmode);
}

int uc_dbal_save(uc_dbald *dbald)
{
	uint8_t pkend=0x7f;
	simpledb_keydata_t kd={1, &pkend};
	key_range_t kr={NULL, &kd}; // save key[0]>=0 to key[0]<=0x7f
	key_range_t *keyranges[2]={&kr, NULL};
	return simpledb_savedata((simpledb_data_t *)dbald, keyranges);
}

int uc_dbal_getdb(uc_dbald *dbald, int toutms, uint8_t *key, uint32_t ksize)
{
	return 0;
}

void uc_dbal_releasedb(uc_dbald *dbald)
{
	return;
}

int uc_dbal_create(uc_dbald *dbald, void *key, uint32_t ksize, void *value,
		   uint32_t vsize)
{
	simpledb_keydata_t kd={ksize, key};
	return simpledb_put((simpledb_data_t *)dbald, &kd, value, vsize,
			    SIMPLEDB_FLAG_CREATE);
}

int uc_dbal_append(uc_dbald *dbald, void *key, uint32_t ksize, void *value,
		   uint32_t vsize)
{
	simpledb_keydata_t kd={ksize, key};
	return simpledb_put((simpledb_data_t *)dbald, &kd, value, vsize,
			    SIMPLEDB_FLAG_CREATE | SIMPLEDB_FLAG_APPEND);
}

int uc_dbal_get(uc_dbald *dbald, void *key, uint32_t ksize, void **value,
		uint32_t *vsize)
{
	simpledb_keydata_t kd={ksize, key};
	return simpledb_get((simpledb_data_t *)dbald, &kd, value, vsize);
}

int uc_dbal_get_release(uc_dbald *dbald, void *key, uint32_t ksize,
			 void *value, uint32_t vsize)
{
	simpledb_keydata_t kd={ksize, key};
	return simpledb_get_release((simpledb_data_t *)dbald, &kd);
}

int uc_dbal_del(uc_dbald *dbald, void *key, uint32_t ksize)
{
	simpledb_keydata_t kd={ksize, key};
	return simpledb_del((simpledb_data_t *)dbald, &kd);
}

uc_range *uc_get_range(uc_dbald *dbald, void *key1, uint32_t ksize1,
		       void *key2, uint32_t ksize2)
{
	simpledb_keydata_t kd1={ksize1, key1};
	simpledb_keydata_t kd2={ksize2, key2};
	key_range_t krange={&kd1, &kd2};
	return (uc_range *)simpledb_get_range((simpledb_data_t *)dbald, &krange);
}

void uc_get_range_release(uc_dbald *dbald, uc_range *range)
{
	simpledb_get_range_release((simpledb_data_t *)dbald,
				   (simpledb_range_t *)range);
}

int uc_get_keyvalue_in_range(uc_dbald *dbald, uc_range *range, void **kdata, uint32_t *ksize,
			     void **vdata, uint32_t *vsize, int direction)
{
	return simpledb_get_from_range((simpledb_data_t *)dbald,
				       (simpledb_range_t *)range,
				       kdata, ksize, vdata, vsize, direction);
}

int uc_get_key_in_range(uc_dbald *dbald, uc_range *range, void **kdata, uint32_t *ksize,
			int direction)
{
	return simpledb_get_from_range((simpledb_data_t *)dbald,
				       (simpledb_range_t *)range,
				       kdata, ksize, NULL, NULL, direction);
}

int uc_get_value_in_range(uc_dbald *dbald, uc_range *range, void **vdata, uint32_t *vsize,
			  int direction)
{
	return simpledb_get_from_range((simpledb_data_t *)dbald,
				       (simpledb_range_t *)range,
				       NULL, NULL, vdata, vsize, direction);
}

void uc_move_top_in_range(uc_dbald *dbald, uc_range *range)
{
	return simpledb_move_top_range((simpledb_data_t *)dbald,
				       (simpledb_range_t *)range);
}

void uc_move_bottom_in_range(uc_dbald *dbald, uc_range *range)
{
	return simpledb_move_bottom_range((simpledb_data_t *)dbald,
					  (simpledb_range_t *)range);
}

int uc_move_in_range(uc_dbald *dbald, uc_range *range, int direction)
{
	return simpledb_move_in_range((simpledb_data_t *)dbald,
				      (simpledb_range_t *)range,
				      direction);
}

int uc_del_in_range(uc_dbald *dbald, uc_range *range, int direction)
{
	return simpledb_del_in_range((simpledb_data_t *)dbald,
				     (simpledb_range_t *)range,
				     direction);
}

int uc_wait_release(uc_dbald *dbald, int toutms)
{
	return simpledb_wait_release((simpledb_data_t *)dbald, toutms);
}
