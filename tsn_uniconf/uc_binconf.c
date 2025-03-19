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
#include "yangs/yang_db_access.h"
#include "uc_binconf.h"
#include "hal/uc_hwal.h"

struct uc_bcdata{
	uint32_t chunk_pt;
	uint32_t chunk_size;
	uint8_t *chunkdata;
	uint8_t lkey[UC_MAX_KEYSIZE];
	uint32_t lksize;
	void *vdata;
	uint16_t vsize;
	uint16_t apdsize;
	uc_hwald *hwald;
};

#define VSIZE_SIZE(x) (x&0x7fff)
#define VSIZE_APPEND(x) ((x&0x8000)!=0)

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

/*
 *  key: _[match]_a_[no match]___c_[match]_
 * lkey: _[match]_a_[no match]_b_[match]_
 * ksize-c == lksize-b, c=ksize-lksize+b
 * return pa:10bits, pb-pa:11bits, pc-pa:11bits
 */
static uint32_t find_dif_points(uc_bcdata_t *ucbcd, uint32_t ksize, uint8_t *key)
{
	unsigned int i;
	uint16_t pa, pb, pc;
	for(i=0;i<ucbcd->lksize;i++){
		if((i>=ksize) || (key[i]!=ucbcd->lkey[i])){
			break;
		}
	}
	pa=i;
	for(i=0;ucbcd->lksize-i>pa;i++){
		if((ksize-i<=0) || (key[ksize-i-1]!=ucbcd->lkey[ucbcd->lksize-i-1])){
			break;
		}
	}
	pb=ucbcd->lksize-i;
	pc=ksize-ucbcd->lksize+pb;
	UB_LOG(UBL_DEBUGV, "%s:pa=%d, pb=%d, pc=%d\n", __func__, pa, pb, pc);
	return (pa<<22)| ((pb-pa)<<11) | (pc-pa);
}

uc_bcdata_t *uc_binconf_init(uint32_t chunk_size)
{
	uc_bcdata_t *ucbcd;
	ucbcd=(uc_bcdata_t *)UB_SD_GETMEM(YANGINIT_GEN_SMEM, sizeof(uc_bcdata_t));
	if(ub_assert_fatal(ucbcd!=NULL, __func__, NULL)){return NULL;}
	memset(ucbcd, 0, sizeof(uc_bcdata_t));
	ucbcd->chunk_size=chunk_size;
	ucbcd->chunkdata=(uint8_t*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, ucbcd->chunk_size);
	if(ub_assert_fatal(ucbcd->chunkdata!=NULL, __func__, NULL)){
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, ucbcd);
		return NULL;
	}
	return ucbcd;
}

void uc_binconf_close(uc_bcdata_t *ucbcd)
{
	if(ucbcd->chunkdata!=NULL){
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, ucbcd->chunkdata);
	}
	UB_SD_RELMEM(YANGINIT_GEN_SMEM, ucbcd);
	return;
}

void uc_binconf_set_hwald(uc_bcdata_t *ucbcd, uc_hwald *hwald)
{
	ucbcd->hwald=hwald;
}

static int set_dbitem(uc_bcdata_t *ucbcd, uint32_t *chunk_rpt)
{
	uint32_t res=0;
	uint16_t pa,pb,pc;
	void *key;
	bool attop=(*chunk_rpt==0);
	if(ucbcd->chunk_pt<*chunk_rpt+sizeof(uint32_t)){return 1;}
	memcpy(&res, &ucbcd->chunkdata[*chunk_rpt], sizeof(uint32_t));
	pa=res>>22;
	pb=((res>>11)&0x7ff)+pa;
	pc=(res&0x7ff)+pa;
	*chunk_rpt+=sizeof(uint32_t);

	if(ucbcd->chunk_pt<*chunk_rpt+pc-pa){return 1;}
	key=&ucbcd->chunkdata[*chunk_rpt];
	*chunk_rpt+=pc-pa;

	if(ucbcd->chunk_pt<*chunk_rpt+sizeof(uint16_t)){return 1;}
	memcpy(&ucbcd->vsize, &ucbcd->chunkdata[*chunk_rpt], sizeof(uint16_t));
	*chunk_rpt+=sizeof(uint16_t);

	ucbcd->vdata=&ucbcd->chunkdata[*chunk_rpt];
	*chunk_rpt+=VSIZE_SIZE(ucbcd->vsize);
	if(ucbcd->lksize<pb){
		UB_LOG(UBL_ERROR, "%s:invalid size, ucbcd->lksize=%d, pb=%d\n",
		       __func__, ucbcd->lksize, pb);
		return -1;
	}
	if(ucbcd->chunk_pt<*chunk_rpt){
		if(!attop){return 1;}
		ucbcd->apdsize=*chunk_rpt-ucbcd->chunk_pt;
		*chunk_rpt=ucbcd->chunk_pt;
		UB_LOG(UBL_DEBUG, "%s:one item can't fit, apdsize=%d, vsize=%d, %d\n",
		       __func__, ucbcd->apdsize, VSIZE_SIZE(ucbcd->vsize), ucbcd->chunk_pt);
	}
	memmove(&ucbcd->lkey[pc], &ucbcd->lkey[pb], ucbcd->lksize-pb);
	memcpy(&ucbcd->lkey[pa], key, pc-pa);
	ucbcd->lksize+=pc-pb;
	return 0;
}

int uc_binconf_write_chunk(uc_bcdata_t *ucbcd, uc_dbald *dbald)
{
	int res;
	uint32_t chunk_rpt=0;
	uint32_t last_chunk_rpt=0;
	int writeitems=0;
	unsigned int awsize;
	uint8_t *aps;
	void *kvs[UC_MAX_KV_DEPTH+1];
	uint8_t kss[UC_MAX_KV_DEPTH];
	if(ucbcd->apdsize>0){
		awsize=UB_MIN(ucbcd->apdsize, ucbcd->chunk_pt);
		if(uc_dbal_append(dbald, ucbcd->lkey, ucbcd->lksize,
				  ucbcd->chunkdata, awsize)!=0){
			return -1;
		}
		ucbcd->apdsize-=awsize;
		if(ucbcd->chunk_pt>awsize){
			memmove(ucbcd->chunkdata, &ucbcd->chunkdata[awsize],
				ucbcd->chunk_pt-awsize);
		}
		ucbcd->chunk_pt-=awsize;
		if(ucbcd->apdsize>0){return 0;}
	}
	while(ucbcd->chunk_pt>chunk_rpt){
		uint32_t vsize;
		res=set_dbitem(ucbcd, &chunk_rpt);
		if(res<0){return res;}
		if(res!=0){break;}
		vsize=VSIZE_SIZE(ucbcd->vsize)-ucbcd->apdsize;
		if(ucbcd->vsize==0){
			uc_dbal_del(dbald, ucbcd->lkey, ucbcd->lksize);
		}else if(VSIZE_APPEND(ucbcd->vsize)){
			if(uc_dbal_append(dbald, ucbcd->lkey, ucbcd->lksize,
					  ucbcd->vdata, vsize)!=0){
				return -1;
			}
		}else{
			if(uc_dbal_create(dbald, ucbcd->lkey, ucbcd->lksize,
					  ucbcd->vdata, vsize)!=0){
				return -1;
			}
		}
		if(ucbcd->hwald!=NULL){
			if(yang_db_extract_key(ucbcd->lkey, ucbcd->lksize,
					       &aps, kvs, kss)!=0){return -1;}
			uc_hwal_reghw(ucbcd->hwald, aps, kvs, kss,
				      ucbcd->vdata, vsize);
			yang_db_extract_key_free(aps, kvs, kss);
		}
		last_chunk_rpt=chunk_rpt;
		writeitems++;
	}
	memmove(ucbcd->chunkdata, &ucbcd->chunkdata[last_chunk_rpt],
		ucbcd->chunk_pt-last_chunk_rpt);
	ucbcd->chunk_pt-=last_chunk_rpt;
	return writeitems;
}

int uc_binconf_pop_item(uc_bcdata_t *ucbcd, uc_dbald *dbald,
			void **key, uint32_t *ksize, void **vdata, uint32_t *vsize,
			uint8_t *atype)
{
	uint32_t res;
	uint32_t chunk_rpt=0;
	res=set_dbitem(ucbcd, &chunk_rpt);
	if(res!=0){return -1;}
	*key=ucbcd->lkey;
	*ksize=ucbcd->lksize;
	*vsize=VSIZE_SIZE(ucbcd->vsize);
	if(*vsize>0){
		*vdata=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *vdata, *vsize);
		if(ub_assert_fatal(*vdata!=NULL, __func__, NULL)){return -1;}
		memcpy(*vdata, ucbcd->vdata, *vsize-ucbcd->apdsize);
	}
	memmove(ucbcd->chunkdata, &ucbcd->chunkdata[chunk_rpt],
		ucbcd->chunk_pt-chunk_rpt);
	ucbcd->chunk_pt-=chunk_rpt;
	res=ucbcd->apdsize;
	ucbcd->apdsize=0;
	if(ucbcd->vsize==0){
		*atype=YANG_DB_ACTION_DELETE;
	}else if(VSIZE_APPEND(ucbcd->vsize)){
		*atype=YANG_DB_ACTION_APPEND;
	}else{
		*atype=YANG_DB_ACTION_CREATE;
	}
	return res;
}

int uc_binconf_set_kvdata(uc_bcdata_t *ucbcd, uint8_t *key, uint32_t ksize,
			  void *vdata, uint16_t vsize)
{
	uint32_t res=0;
	uint16_t pa,pc;
	uint16_t svsize=0;
	if(vdata!=NULL){svsize=VSIZE_SIZE(vsize);}
	if(ksize>=sizeof(ucbcd->lkey)){
		UB_LOG(UBL_ERROR, "%s:too large ksize\n", __func__);
		return -1;
	}
	if(ucbcd->lksize!=0){
		res=find_dif_points(ucbcd, ksize, key);
		pa=(res>>22)&0x03ff;
		pc=(res&0x07ff)+pa;
	}else{
		pa=0;
		pc=ksize;
		res=ksize;
	}
	if((ucbcd->chunk_size-ucbcd->chunk_pt) <
		   pc-pa+svsize+sizeof(uint32_t)+sizeof(uint16_t)){
		if(ucbcd->chunk_pt==0){return 2;}
		return 1;
	}
	memcpy(&ucbcd->chunkdata[ucbcd->chunk_pt],
	       &res, sizeof(uint32_t));
	memcpy(&ucbcd->chunkdata[ucbcd->chunk_pt+sizeof(uint32_t)],
	       &key[pa], pc-pa);
	memcpy(&ucbcd->chunkdata[ucbcd->chunk_pt+sizeof(uint32_t)+pc-pa],
	       &vsize, sizeof(uint16_t));
	if(vdata!=NULL){
		memcpy(&ucbcd->chunkdata[ucbcd->chunk_pt+sizeof(uint32_t)+
					 pc-pa+sizeof(uint16_t)],
		       vdata, svsize);
	}
	ucbcd->chunk_pt+=sizeof(uint32_t)+pc-pa+sizeof(uint16_t)+svsize;
	memcpy(ucbcd->lkey, key, ksize);
	ucbcd->lksize=ksize;
	return 0;
}

int uc_binconf_set_bindata(uc_bcdata_t *ucbcd, const void *bdata, uint32_t bsize)
{
	int setsize=0;
	setsize=UB_MIN(ucbcd->chunk_size-ucbcd->chunk_pt, bsize);
	if(setsize<=0){return 0;}
	memcpy(&ucbcd->chunkdata[ucbcd->chunk_pt], bdata, setsize);
	ucbcd->chunk_pt+=setsize;
	return setsize;
}

int uc_binconf_read_range(uc_bcdata_t *ucbcd, uc_dbald *dbald, uc_range *range)
{
	uint8_t *key;
	uint32_t ksize;
	void *vdata;
	uint32_t vsize;
	int res;
	if(range==NULL){return -1;}
	while(true){
		if(uc_get_keyvalue_in_range(dbald, range, (void**)&key,
					    &ksize, &vdata, &vsize,
					    UC_DBAL_NOMOVE)!=0){
			break;
		}
		res=uc_binconf_set_kvdata(ucbcd, key, ksize, vdata, vsize);
		if(res!=0){
			UB_LOG(UBL_DEBUGV, "%s: can't set kvdata, ksize=%d, vsize=%d\n",
			       __func__, ksize, vsize);
			if(res==2){
				UB_LOG(UBL_DEBUG,
				       "%s:ksize=%d, vsize=%d, too big to fit in chunkdata, "
				       "return pointer\n",
				       __func__, ksize, vsize);
				uc_binconf_set_kvdata(ucbcd, key, ksize, NULL, vsize);
				memcpy(&ucbcd->chunkdata[ucbcd->chunk_pt], &vdata, sizeof(void*));
				ucbcd->chunk_pt+=sizeof(void*);
				uc_move_in_range(dbald, range, UC_DBAL_FORWARD);
			}
			return res;
		}
		if(uc_move_in_range(dbald, range, UC_DBAL_FORWARD)!=0) break;
	}
	return 0;
}

int uc_binconf_get_chunk(uc_bcdata_t *ucbcd, uint8_t **chunkdata)
{
	*chunkdata=ucbcd->chunkdata;
	return ucbcd->chunk_pt;
}

void uc_binconf_clean_chunk(uc_bcdata_t *ucbcd)
{
	ucbcd->chunk_pt=0;
	return;
}

void uc_binconf_clean_lkey(uc_bcdata_t *ucbcd)
{
	ucbcd->lksize=0;
	return;
}

static int read_bindata(uc_bcdata_t *ucbcd, uc_dbald *dbald,
			const void *bdata, int bsize)
{
	void *inf=NULL;
	uint8_t buf[1024];
	const uint8_t *bufp;
	int res;
	int rsize=0;
	int rp=0;
	if(bsize==0){
		const char *fname=(const char*)bdata;
		// this is a file
		inf=ub_fioopen(fname, "r");
		if(inf==NULL){
			UB_LOG(UBL_ERROR, "%s:can't open file=%s\n", __func__, fname);
			return -1;
		}
		bufp=buf;
	}else{
		rsize=bsize;
		bufp=(const uint8_t*)bdata;
	}
	uc_binconf_clean_lkey(ucbcd);
	uc_binconf_clean_chunk(ucbcd);
	while(true){
		rp=0;
		if(inf!=NULL){rsize=ub_fioread(inf, buf, sizeof(buf));}
		if(rsize<=0){break;}
		while(rsize>0){
			res=uc_binconf_set_bindata(ucbcd, &bufp[rp], rsize);
			if(res<=0){break;}
			if(uc_binconf_write_chunk(ucbcd, dbald)<0){
				UB_LOG(UBL_ERROR, "%s:can't set data\n", __func__);
				res=-1;
				goto erexit;
			}
			rp+=res;
			rsize-=res;
			uc_dbal_releasedb(dbald);
		}
	}
	res=0;
erexit:
	if(inf!=NULL){ub_fioclose(inf);}
	return res;
}

int uc_binconf_read_binfile(uc_bcdata_t *ucbcd, uc_dbald *dbald, const char *fname)
{
	return read_bindata(ucbcd, dbald, fname, 0);
}

int uc_binconf_read_bconffile(uc_dbald *dbald, const char *fname)
{
	int res;
	uc_bcdata_t *ucbcd=uc_binconf_init(1024);
	if(!ucbcd || strstr(fname, ".bconf")==NULL){return -1;}
	res=read_bindata(ucbcd, dbald, fname, 0);
	uc_binconf_close(ucbcd);
	return res;
}

int uc_binconf_read_bindata(uc_bcdata_t *ucbcd, uc_dbald *dbald,
			    const uint8_t *bdata, int bsize)
{
	if(bsize==0){return 0;}
	return read_bindata(ucbcd, dbald, bdata, bsize);
}
