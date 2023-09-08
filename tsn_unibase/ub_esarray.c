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
/*
 * ub_esarray.c
 *
 */
#include "unibase_private.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "ub_getmem.h"

/*
 * the array is expanded by 'expnd_unit'
 * 'expnd_unit' is also hysteresis number,
 * when it needs one more 'expnd_unit' it expands.
 * when it has unused area as 2 of 'expnd_unit', it shrinks one 'expand_unit'
 */
struct ub_esarray_cstd {
	int expnd_unit;
	int expand_hysteresis;
	int element_size;
	int uaelement_size;
	int allocate_nums;
	int element_nums;
	int max_element_nums;
	uint8_t *data;
	bool data_lock;
};

/*
 * static memory allocation case('UB_SD_STATIC' is defined)
 * the memory is statically allocated by the following definitions.
 * the numbers defined here should be overridden by your needs.
 * when 'UB_SD_STATIC' is not defined, the number doesn't have meaning.
 */
#define UB_ESARRAY_INSTMEM ub_esarray_inst
// how many this esarray instances are used as maximum
#ifndef UB_ESARRAY_INSTNUM
#define UB_ESARRAY_INSTNUM 8
#endif
#define UB_ESARRAY_DATAMEM ub_esarray_data
// how many memory fragments are used in all instances as maximum
#ifndef UB_ESARRAY_DFNUM
#define UB_ESARRAY_DFNUM 64
#endif
// memory fragment size
#ifndef UB_ESARRAY_DFSIZE
#define UB_ESARRAY_DFSIZE 16
#endif

UB_SD_GETMEM_DEF(UB_ESARRAY_INSTMEM, (int)sizeof(struct ub_esarray_cstd),
		 UB_ESARRAY_INSTNUM);
UB_SD_GETMEM_DEF(UB_ESARRAY_DATAMEM, UB_ESARRAY_DFSIZE, UB_ESARRAY_DFNUM);

static void *get_newele_nomutex(ub_esarray_cstd_t *eah)
{
	uint8_t *np;
	void *res;
	if(eah->element_nums >= eah->max_element_nums){
		UB_LOG(UBL_ERROR, "%s:can't expand more than max number of elements: %d\n",
		       __func__, eah->max_element_nums);
		return NULL;
	}
	if(eah->allocate_nums<=eah->element_nums){
		if(eah->data_lock){
			UB_LOG(UBL_ERROR, "%s:data is locked\n", __func__);
			return NULL;
		}
		/* expand allocated memory */
		np=(uint8_t *)UB_SD_REGETMEM(UB_ESARRAY_DATAMEM,
				 eah->data,
				 eah->element_size *
				 (eah->allocate_nums + eah->expnd_unit));
		if(np == NULL){
			UB_LOG(UBL_ERROR, "%s:realloc error, %s\n", __func__, strerror(errno));
			return NULL;
		}
		eah->allocate_nums+= eah->expnd_unit;
		eah->data = np;
	}
	res=&eah->data[eah->element_size * eah->element_nums];
	eah->element_nums++;
	return res;
}

static int del_index_nomutex(ub_esarray_cstd_t *eah, int index)
{
	if(((index+1) > eah->element_nums) || (index < 0)){
		UB_LOG(UBL_ERROR, "%s: index=%d doesn't exist\n", __func__, index);
		return -1;
	}

	/* move data after *ed */
	if(eah->element_nums - (index + 1) > 0){
		memmove(&eah->data[eah->element_size * index],
			&eah->data[eah->element_size * (index + 1)],
			eah->element_size * (eah->element_nums - (index + 1)));
	}
	eah->element_nums--;
	if(eah->element_nums <= (eah->allocate_nums-(2*eah->expnd_unit))){
		if(eah->data_lock){
			UB_LOG(UBL_WARN, "%s:data is locked\n", __func__);
			return 0;
		}
		/* shrink allocate */
		eah->data = (uint8_t *)UB_SD_REGETMEM(UB_ESARRAY_DATAMEM,
					  eah->data,
					  eah->element_size *
					  (eah->allocate_nums-eah->expnd_unit));
		if((eah->element_nums > 0) && (eah->data == NULL)){
			UB_LOG(UBL_ERROR, "%s:failed to shrink allocated memory, %s\n",
			       __func__, strerror(errno));
			return -1;
		}
		eah->allocate_nums-=eah->expnd_unit;
	}
	return 0;
}

ub_esarray_cstd_t *ub_esarray_init(int expnd_unit, int element_size, int max_element_nums)
{
	ub_esarray_cstd_t *eah;
	eah = (ub_esarray_cstd_t *)UB_SD_GETMEM(UB_ESARRAY_INSTMEM,
						sizeof(ub_esarray_cstd_t));
	if(ub_assert_fatal(eah!=NULL, __func__, "malloc error")){
		return NULL;
	}
	(void)memset(eah, 0, sizeof(ub_esarray_cstd_t));
	eah->expnd_unit = expnd_unit;
	eah->element_size = UB_SD_ALIGN(element_size);
	eah->uaelement_size = element_size;
	eah->max_element_nums = max_element_nums;
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	(void)get_newele_nomutex(eah); // start with allocting the first block
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	eah->element_nums=0;
	return eah;
}

void ub_esarray_close(ub_esarray_cstd_t *eah)
{
	if(eah->data!=NULL){UB_SD_RELMEM(UB_ESARRAY_DATAMEM, eah->data);}
	UB_SD_RELMEM(UB_ESARRAY_INSTMEM, eah);
}

int ub_esarray_add_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed)
{
	void *pt;
	int res=-1;
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	pt = get_newele_nomutex(eah);
	if(pt == NULL){goto erexit;}
	memcpy(pt, ed, eah->uaelement_size);
	res=0;
erexit:
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	return res;
}

int ub_esarray_pop_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed)
{
	ub_esarray_element_t *resd;
	int res=-1;
	if(eah->data_lock){
		UB_LOG(UBL_ERROR, "%s: data is locked\n", __func__);
		return -1;
	}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	resd = ub_esarray_get_ele(eah, 0);
	if(resd==NULL){goto erexit;}
	memcpy(ed, resd, eah->uaelement_size);
	(void)del_index_nomutex(eah, 0);
	res = 0;
erexit:
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	return res;
}

int ub_esarray_pop_last_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed)
{
	ub_esarray_element_t *resd;
	int res=-1;
	if(eah->data_lock){
		UB_LOG(UBL_ERROR, "%s: data is locked\n", __func__);
		return -1;
	}
	if(!eah->element_nums){return res;}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	resd = ub_esarray_get_ele(eah, eah->element_nums-1);
	if(resd==NULL){goto erexit;}
	memcpy(ed, resd, eah->uaelement_size);
	(void)del_index_nomutex(eah, eah->element_nums-1);
	res = 0;
erexit:
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	return res;
}

int ub_esarray_del_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed)
{
	int i;
	int res;
	if(eah->data_lock){
		UB_LOG(UBL_ERROR, "%s: data is locked\n", __func__);
		return -1;
	}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	for(i=0;i<eah->element_nums;i++){
		if(!memcmp(&eah->data[eah->element_size * i], ed, eah->uaelement_size)){break;}
	}
	if(i == eah->element_nums){
		UB_LOG(UBL_ERROR, "%s: this data is not in the array\n",__func__);
		res=-1;
	}else{
		res=del_index_nomutex(eah, i);
	}
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	return res;
}

/*
 * Once ub_esarray_data_lock is called, eah->data_lock is locked.
 * In the locked status, any elements of data can't be deleted,
 * also, expanding the allocated area can't happen.
 *
 * eah->data_lock is unlocked by the parameter on es_del_index
 * or es_del_pointer.
 *
 * In single thread env., data_lock doesn't have to be cared.
 */
int ub_esarray_data_lock(ub_esarray_cstd_t *eah)
{
	bool res;
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	res=!eah->data_lock;
	eah->data_lock=true;
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	if(!res){
		UB_LOG(UBL_WARN, "%s:already locked\n", __func__);
		return -1;
	}
	return 0;
}

int ub_esarray_data_unlock(ub_esarray_cstd_t *eah)
{
	bool res;
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	res=eah->data_lock; // if already unlocked, return false;
	eah->data_lock=false;
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	if(!res){
		UB_LOG(UBL_WARN, "%s:already unlocked\n", __func__);
		return -1;
	}
	return 0;
}

int ub_esarray_ele_nums(ub_esarray_cstd_t *eah)
{
	return eah->element_nums;
}

ub_esarray_element_t *ub_esarray_get_newele(ub_esarray_cstd_t *eah)
{
	void *ele;
	if(!eah){return NULL;}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	ele=get_newele_nomutex(eah);
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	return ele;
}

ub_esarray_element_t *ub_esarray_get_ele(ub_esarray_cstd_t *eah, int index)
{
	if(!eah){return NULL;}
	if((index >= eah->element_nums) || (index < 0)){return NULL;}
	return (ub_esarray_element_t *)(&eah->data[eah->element_size * index]);
}

int ub_esarray_del_index(ub_esarray_cstd_t *eah, int index)
{
	int res;
	if(eah->data_lock){
		UB_LOG(UBL_ERROR, "%s: data is locked\n", __func__);
		return -1;
	}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	res=del_index_nomutex(eah, index);
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	return res;
}


int ub_esarray_del_pointer(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed)
{
	int i;
	int res;
	if(eah->data_lock){
		UB_LOG(UBL_ERROR, "%s: data is locked\n", __func__);
		return -1;
	}
	if(ubcd.threadding){ubcd.cbset.mutex_lock(ubcd.esarray_mutex);}
	for(i=0;i<eah->element_nums;i++){
		if((&eah->data[eah->element_size * i]) == ed){break;}
	}
	if(i == eah->element_nums){
		UB_LOG(UBL_ERROR, "%s: this data is not in the array\n",__func__);
		res=-1;
	}else{
		res=del_index_nomutex(eah, i);
	}
	if(ubcd.threadding){ubcd.cbset.mutex_unlock(ubcd.esarray_mutex);}
	return res;
}
