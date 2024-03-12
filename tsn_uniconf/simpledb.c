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
#include <tsn_unibase/unibase.h>
#include <tsn_combase/cb_thread.h>
#include "uc_dbal.h"
#include "simpledb.h"

typedef struct dbdata{
	uint8_t refcounter; // when this locked(>0), it becomes read only
	simpledb_keydata_t kd;
	uint32_t vsize;
	void *vdata;
} dbdata_t;

typedef struct one_dblist{
	uint8_t lock;
	struct ub_list dblist;
} one_dblist_t;

typedef struct serialized_dbrange{
	key_range_t keyrange;
	uint32_t size;
} serialized_dbrange_t;

struct simpledb_data{
	const char *pfname;
	struct ub_list dblist;
	CB_THREAD_MUTEX_T dbmutex;
	CB_SEM_T relwait_sem;
};

struct simpledb_range{
	struct ub_list_node *n1;
	struct ub_list_node *n2;
	struct ub_list_node *np;
};

#define SIMPLEDB_INSTMEM simpledb_instmem
#ifndef SIMPLEDB_INSTNUM
// the instance is likely only 1, and no need to change this number
#define SIMPLEDB_INSTNUM 1
#endif
UB_SD_GETMEM_DEF(SIMPLEDB_INSTMEM, (int)sizeof(simpledb_data_t),
		 SIMPLEDB_INSTNUM);

#define SIMPLEDB_DBDATAINST simpledb_dbdatainst
// this number must be the number of configuration data items
// 1250 data items must be enough for most of applications.
// to save memory, the number can be smaller.
#ifndef SIMPLEDB_DBDATANUM
#define	SIMPLEDB_DBDATANUM 1250u
#endif
#define SIMPLEDB_LISTNODE simpledb_listnode
UB_SD_GETMEM_DEF(SIMPLEDB_DBDATAINST, sizeof(dbdata_t),
		 SIMPLEDB_DBDATANUM);
UB_SD_GETMEM_DEF(SIMPLEDB_LISTNODE, sizeof(struct ub_list_node),
		 SIMPLEDB_DBDATANUM);

#define SIMPLEDB_RANGEINST simpledb_rangeinst
// the number of handling data ranges should be small, 4 must be enough.
#ifndef SIMPLEDB_RANGENUM
#define SIMPLEDB_RANGENUM 4
#endif
UB_SD_GETMEM_DEF(SIMPLEDB_RANGEINST, sizeof(simpledb_range_t),
		 SIMPLEDB_RANGENUM);

#define SIMPLEDB_KDATAINST simpledb_kdatainst
// keydata size fragment
// in 'simpledb', keydata area is SIMPLEDB_KDATASIZE*(SIMPLEDB_DBDATANUM*2)
// 2 fragments are taken as average, which is a guess
#ifndef SIMPLEDB_KDATASIZE
#define SIMPLEDB_KDATASIZE 16
#endif
// mostly less than 8, '*2' must be appropriate
#define SIMPLEDB_KDATANUM (SIMPLEDB_DBDATANUM*2u)
UB_SD_GETMEM_DEF(SIMPLEDB_KDATAINST, SIMPLEDB_KDATASIZE,
		 SIMPLEDB_KDATANUM);

#define SIMPLEDB_VDATAINST simpledb_vdatainst
// value data size fragment
// in 'simpledb', keydata area is SIMPLEDB_VDATASIZE*(SIMPLEDB_DBDATANUM*2)
// 2 fragments are taken as average, which is a guess
#ifndef SIMPLEDB_VDATASIZE
#define SIMPLEDB_VDATASIZE 8
#endif
// mostly less than 8, '*2' must be appropriate
#define SIMPLEDB_VDATANUM (SIMPLEDB_DBDATANUM*2u)
UB_SD_GETMEM_DEF(SIMPLEDB_VDATAINST, SIMPLEDB_VDATASIZE,
		 SIMPLEDB_VDATANUM);


// if klen1>klen2, key1[:klen2]==key2[:klen2] -> key1>key2
// if klen1<klen2, key1[:klen1]==key2[:klen1] -> key1<key2
static int keycmp(simpledb_keydata_t *kd1, simpledb_keydata_t *kd2)
{
	int res;
	if(kd1 && !kd2){return 1;} // non-NULL > NULL
	if(!kd1 && kd2){return -1;} // NULL < non-NULL
	if(!kd1 && !kd2){return 0;} // NULL == NULL
	if(kd1->ksize>kd2->ksize){
		res=memcmp(kd1->kdata, kd2->kdata, kd2->ksize);
		if(res==0){return 1;}
		return res;
	}
	if(kd1->ksize<kd2->ksize){
		res=memcmp(kd1->kdata, kd2->kdata, kd1->ksize);
		if(res==0){return -1;}
		return res;
	}
	return memcmp(kd1->kdata, kd2->kdata, kd1->ksize);
}

int bin_search(struct ub_list *dblist, simpledb_keydata_t *kd,
	       struct ub_list_node **rnode)
{
	int i, res=0;
	struct ub_list_node *n1;
	dbdata_t *dbd;
	int pn=ub_list_count(dblist);
	int mn;
	*rnode=NULL;
	if(!pn){return -1;}
	n1=ub_list_head(dblist);
	while(pn!=0){
		mn=pn%2;
		pn/=2;
		*rnode=n1;
		for(i=0;i<pn;i++){*rnode=(*rnode)->next;}
		dbd=(dbdata_t *)(*rnode)->data;
		res=keycmp(kd, &dbd->kd);
		if(!res || !pn){break;}
		if(res>0){
			n1=(*rnode)->next;
			if(!mn){pn--;}
		}
	}
	return res;
}

static int serialize_one(void *outf, struct ub_list_node *node, void *arg)
{
	dbdata_t *dbd;
	serialized_dbrange_t *sod=(serialized_dbrange_t *)arg;
	dbd=(dbdata_t *)node->data;

	// write kdata
	sod->size+=sizeof(uint32_t)+dbd->kd.ksize;
	if(ub_fiowrite(outf, &dbd->kd.ksize, sizeof(uint32_t))!=(int)sizeof(uint32_t)){return -1;}
	if(ub_fiowrite(outf, dbd->kd.kdata, dbd->kd.ksize)!=(int)dbd->kd.ksize){return -1;}

	// write vdata
	sod->size+=sizeof(uint32_t)+dbd->vsize;
	if(ub_fiowrite(outf, &dbd->vsize, sizeof(uint32_t))!=(int)sizeof(uint32_t)){return -1;}
	if(dbd->vsize>0u){
		if(ub_fiowrite(outf, dbd->vdata, dbd->vsize)!=(int)dbd->vsize){return -1;}
	}
	return 0;
}

static int get_range(struct ub_list *dblist, key_range_t *keyrange,
		     struct ub_list_node **n1,
		     struct ub_list_node **n2)
{
	int res;
	dbdata_t *dbd1, *dbd2;
	if(!keyrange->kd1 || !keyrange->kd1->ksize){
		*n1=ub_list_head(dblist);
	}else{
		res=bin_search(dblist, keyrange->kd1, n1);
		if((res>0) && (*n1!=NULL)){*n1=(*n1)->next;} // n1 is in lower, start at the next
	}

	if(!keyrange->kd2 || !keyrange->kd2->ksize){
		*n2=ub_list_tail(dblist);
	}else{
		res=bin_search(dblist, keyrange->kd2, n2);
		if((res<0) && (*n2!=NULL)){*n2=(*n2)->prev;} // n2 is in higher, end at the prev
	}

	if(!*n1 || !*n2){return -1;}
	dbd1=(dbdata_t *)((*n1)->data);
	dbd2=(dbdata_t *)((*n2)->data);
	if(keycmp(&dbd2->kd, &dbd1->kd)<0){return -1;}
	return 0;
}

static int serialize_range(void *outf, struct ub_list *dblist, serialized_dbrange_t *sod)
{
	struct ub_list_node *n1,*n2;
	if(get_range(dblist, &sod->keyrange, &n1, &n2)!=0) {
		UB_LOG(UBL_WARN, "%s:no data in the range\n",__func__);
		return -1;;
	}
	while(n1!=NULL){
		if(serialize_one(outf, n1, sod)!=0){break;}
		if(n1==n2){break;}
		n1=n1->next;
	}
	return 0;
}

static int deserialize_one(void *inf, dbdata_t *dbd)
{
	uint32_t size;
	int p=0;

	// read kdata size
	if(ub_fioread(inf, &size, sizeof(uint32_t))!=(int)sizeof(uint32_t)){return -1;}
	if(size>(uint32_t)UC_MAX_KEYSIZE){
		UB_LOG(UBL_ERROR,"%s:ksize=%d\n", __func__, size);
		return -1;
	}
	dbd->kd.ksize=size;
	p+=(int)sizeof(uint32_t);
	// read kdata
	dbd->kd.kdata=UB_SD_GETMEM(SIMPLEDB_KDATAINST, dbd->kd.ksize);
	if(ub_assert_fatal(dbd->kd.kdata!=NULL, __func__, NULL)){return -1;}
	if(ub_fioread(inf, dbd->kd.kdata, dbd->kd.ksize)!=(int)dbd->kd.ksize){return -1;}
	p+=(int)dbd->kd.ksize;

	// read vdata size
	if(ub_fioread(inf, &size, sizeof(uint32_t))!=(int)sizeof(uint32_t)){return -1;}
	if(size>(uint32_t)UC_MAX_VALUESIZE){
		UB_LOG(UBL_ERROR,"%s:vsize=%d\n", __func__, size);
		return -1;
	}
	dbd->vsize=size;
	p+=(int)sizeof(uint32_t);
	if(dbd->vsize>0u){
		dbd->vdata=UB_SD_GETMEM(SIMPLEDB_VDATAINST, dbd->vsize);
		if(ub_assert_fatal(dbd->vdata!=NULL, __func__, NULL)){return -1;}
		if(ub_fioread(inf, dbd->vdata, dbd->vsize)!=(int)dbd->vsize){return -1;}
	}
	p+=(int)dbd->vsize;
	return p;
}

static void dbdata_clear(dbdata_t *dbd)
{
	if(dbd==NULL){ return;}
	if(dbd->kd.kdata!=NULL){UB_SD_RELMEM(SIMPLEDB_KDATAINST, dbd->kd.kdata);}
	if(dbd->vdata!=NULL){UB_SD_RELMEM(SIMPLEDB_VDATAINST, dbd->vdata);}
	UB_SD_RELMEM(SIMPLEDB_DBDATAINST, dbd);
}

static void dbnode_clear(struct ub_list_node *node, void *arg)
{
	dbdata_clear((dbdata_t *)node->data);
	UB_SD_RELMEM(SIMPLEDB_LISTNODE, node);
}

static int deserialize_all(void *inf, struct ub_list *dblist)
{
	struct ub_list_node *node;
	dbdata_t *dbd;
	struct ub_list_node *pn=NULL;
	int p=0;
	int res;

	while(true){
		dbd=(dbdata_t*)UB_SD_GETMEM(SIMPLEDB_DBDATAINST, sizeof(dbdata_t));
		if(ub_assert_fatal(dbd!=NULL, __func__, NULL)){return -1;}
		(void)memset(dbd, 0, sizeof(dbdata_t));
		res=deserialize_one(inf, dbd);
		if(res<=0){
			dbdata_clear(dbd);
			break;
		}
		p+=res;
		if((pn && keycmp(&dbd->kd, &((dbdata_t *)(pn->data))->kd)<0)){
			dbdata_clear(dbd);
			continue;
		}
		node=(struct ub_list_node*)UB_SD_GETMEM(SIMPLEDB_LISTNODE, sizeof(struct ub_list_node));
		if(ub_assert_fatal(node!=NULL, __func__, NULL)){return -1;}
		(void)memset(node, 0, sizeof(struct ub_list_node));
		node->data=dbd;
		if(pn!=NULL){
			ub_list_insert_after(dblist, pn, node);
		}else{
			ub_list_append(dblist, node);
		}
		pn=node;
	}
	return p;
}

static int onedb_get(struct ub_list *dblist, simpledb_keydata_t *kd,
		     void **value, uint32_t *vlen)
{
	dbdata_t *dbd;
	struct ub_list_node *node;
	if(bin_search(dblist, kd, &node)!=0){return -1;}
	dbd=(dbdata_t *)node->data;
	dbd->refcounter+=1u;
	*value=dbd->vdata;
	*vlen=dbd->vsize;
	return 0;
}

static int onedb_get_release(struct ub_list *dblist, simpledb_keydata_t *kd)
{
	dbdata_t *dbd;
	struct ub_list_node *node;
	if(bin_search(dblist, kd, &node)!=0){return -1;}
	dbd=(dbdata_t *)node->data;
	if(dbd->refcounter>0u){dbd->refcounter-=1u;}
	return 0;
}

static int onedb_del(struct ub_list *dblist, simpledb_keydata_t *kd)
{
	dbdata_t *dbd;
	struct ub_list_node *node;
	if(bin_search(dblist, kd, &node)!=0){return -1;}
	dbd=(dbdata_t *)node->data;
	if(dbd->refcounter>0u){
		UB_LOG(UBL_WARN,"%s:the node is locked\n", __func__);
		return -1;
	}
	ub_list_unlink(dblist, node);
	dbnode_clear(node, NULL);
	return 0;
}

static int onedb_put(struct ub_list *dblist, simpledb_keydata_t *kd,
		     void *value, uint32_t vlen, uint8_t flags)
{
	dbdata_t *dbd=NULL;
	struct ub_list_node *node;
	struct ub_list_node *cnode;
	int res;
	res=bin_search(dblist, kd, &node);
	if(res!=0){
		if(!(flags & SIMPLEDB_FLAG_CREATE)){
			UB_LOG(UBL_ERROR,"%s:the key doesn't exist\n", __func__);
			return -1;
		}
		dbd=(dbdata_t*)UB_SD_GETMEM(SIMPLEDB_DBDATAINST, sizeof(dbdata_t));
		if(ub_assert_fatal(dbd!=NULL, __func__, NULL)){return -1;}
		(void)memset(dbd, 0, sizeof(dbdata_t));
		dbd->kd.ksize=kd->ksize;
		dbd->kd.kdata=UB_SD_GETMEM(SIMPLEDB_KDATAINST, kd->ksize);
		if(ub_assert_fatal(dbd->kd.kdata!=NULL, __func__, NULL)){return -1;}
		memcpy(dbd->kd.kdata, kd->kdata, kd->ksize);
		cnode=(struct ub_list_node*)UB_SD_GETMEM(SIMPLEDB_LISTNODE, sizeof(struct ub_list_node));
		if(ub_assert_fatal(cnode!=NULL, __func__, NULL)){return -1;}
		(void)memset(cnode, 0, sizeof(struct ub_list_node));
		cnode->data=dbd;
		if(res>0){
			if(!node){
				ub_list_append(dblist, cnode);
			}else{
				ub_list_insert_after(dblist, node, cnode);
			}
		}else{
			if(!node){
				ub_list_prepend(dblist, cnode);
			}else{
				ub_list_insert_before(dblist, node, cnode);
			}
		}
	}else{
		dbd=(dbdata_t *)node->data;
		if(dbd->refcounter>0u){
			UB_LOG(UBL_WARN,"%s:the node is locked, refc=%d\n",
			       __func__, dbd->refcounter);
			return -1;
		}
		if((flags & SIMPLEDB_FLAG_APPEND)!=0u){
			if(!value){return 0;}
			dbd->vdata=UB_SD_REGETMEM(SIMPLEDB_VDATAINST, dbd->vdata, dbd->vsize+vlen);
			if(ub_assert_fatal(dbd->vdata!=NULL,
					   __func__, "can't realloc vdata")){return -1;}
			(void)memcpy(&((uint8_t*)dbd->vdata)[dbd->vsize], value, vlen);
			dbd->vsize+=vlen;
			return 0;
		}
	}
	if(dbd->vsize!=vlen){
		if(!value){
			dbd->vsize=0;
		}else{
			dbd->vdata=UB_SD_REGETMEM(SIMPLEDB_VDATAINST, dbd->vdata, vlen);
			if(ub_assert_fatal(dbd->vdata, __func__,
					   "can't realloc vdata")){return -1;}
			dbd->vsize=vlen;
		}
	}
	if(dbd->vdata != NULL && value && vlen){memcpy(dbd->vdata, value, vlen);}
	return 0;
}

simpledb_data_t *simpledb_open(const char *pfname)
{
	void *inf=NULL;
	simpledb_data_t *sdbd;
	int res;
	sdbd=(simpledb_data_t*)UB_SD_GETMEM(SIMPLEDB_INSTMEM, sizeof(simpledb_data_t));
	if(ub_assert_fatal(sdbd!=NULL, __func__, NULL)){return NULL;}
	(void)memset(sdbd, 0, sizeof(simpledb_data_t));
	sdbd->pfname=pfname;
	if(CB_THREAD_MUTEX_INIT(&sdbd->dbmutex, NULL)!=0){goto erexit;}
	if(CB_SEM_INIT(&sdbd->relwait_sem, 0, 0)!=0){goto erexit;}
	if(sdbd->pfname!=NULL){inf=ub_fioopen(sdbd->pfname, "r");}
	if(inf==NULL){
		UB_LOG(UBL_INFO, "%s:no data is imported\n", __func__);
		return sdbd;
	}
	res=deserialize_all(inf, &sdbd->dblist);
	UB_LOG(UBL_INFO, "%s:imported %d bytes from the saved data\n", __func__, res);
	(void)ub_fioclose(inf);
	return sdbd;
erexit:
	UB_LOG(UBL_ERROR, "%s:failed\n", __func__);
	simpledb_close(sdbd);
	return NULL;
}

/*
 * the format of the seriarized data:
 * [[ksize(uint32_t), kdata, vsize(uint32_t), vdata],,,]
 *
 * 'keyranges' can include separated non continuous key ranges,
 * all ranges in 'keyranges' is saved in this function
 * the order in 'keyranges' must be smll to big by 'keycmp'
 */
int simpledb_savedata(simpledb_data_t *sdbd, key_range_t **keyranges)
{
	key_range_t *kr;
	serialized_dbrange_t sod;
	void *outf;
	int res=0;
	if(!sdbd || !keyranges || !sdbd->pfname){return -1;}
	outf=ub_fioopen(sdbd->pfname, "w");
	if(outf==NULL){return -1;}
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	while(!res){
		kr=*keyranges;
		keyranges++;
		if(!kr){break;}
		if(((kr->kd1==NULL) || (kr->kd1->ksize==0u)) &&
		   ((kr->kd2==NULL) || (kr->kd2->ksize==0u))){
			break;
		}
		(void)memset(&sod, 0, sizeof(sod));
		sod.keyrange.kd1=kr->kd1;
		sod.keyrange.kd2=kr->kd2;
		res=serialize_range(outf, &sdbd->dblist, &sod);
	}
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	(void)ub_fioclose(outf);
	return 0;
}

void simpledb_close(simpledb_data_t *sdbd)
{
	if(!sdbd){return;}
	CB_THREAD_MUTEX_DESTROY(&sdbd->dbmutex);
	CB_SEM_DESTROY(&sdbd->relwait_sem);
	ub_list_clear(&sdbd->dblist, dbnode_clear, NULL);
	UB_SD_RELMEM(SIMPLEDB_INSTMEM, sdbd);
}

int simpledb_put(simpledb_data_t *sdbd, simpledb_keydata_t *kd, void *value,
		 uint32_t vlen, uint8_t flags)
{
	int res;
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	res=onedb_put(&sdbd->dblist, kd, value, vlen, flags);
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return res;
}

int simpledb_get(simpledb_data_t *sdbd, simpledb_keydata_t *kd,
		 void **value, uint32_t *vlen)
{
	int res;
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	res=onedb_get(&sdbd->dblist, kd, value, vlen);
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return res;
}

int simpledb_get_release(simpledb_data_t *sdbd, simpledb_keydata_t *kd)
{
	int res;
	int semval;
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	res=onedb_get_release(&sdbd->dblist, kd);
	CB_SEM_GETVALUE(&sdbd->relwait_sem, &semval);
	if(semval==0){CB_SEM_POST(&sdbd->relwait_sem);}
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return res;
}

int simpledb_del(simpledb_data_t *sdbd, simpledb_keydata_t *kd)
{
	int res;
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	res=onedb_del(&sdbd->dblist, kd);
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return res;
}

/*
 * 2023.10.9 stop locking by 'dbd->refcounter+=1u'
 * the locking is rarely needed.
 * If the caller want to lock, it should lock by the other operations.
 */
simpledb_range_t *simpledb_get_range(simpledb_data_t *sdbd,
				     key_range_t *krange)
{
	simpledb_range_t *rangedp;
	rangedp=(simpledb_range_t*)UB_SD_GETMEM(SIMPLEDB_RANGEINST, sizeof(simpledb_range_t));
	if(ub_assert_fatal(rangedp!=NULL, __func__, NULL)){return NULL;}
	(void)memset(rangedp, 0, sizeof(simpledb_range_t));
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	if(get_range(&sdbd->dblist, krange, &rangedp->n1, &rangedp->n2)!=0) {
		UB_SD_RELMEM(SIMPLEDB_RANGEINST, rangedp);
		rangedp=NULL;
		goto erexit;
	}
	rangedp->np=rangedp->n1;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return rangedp;
}

void simpledb_get_range_release(simpledb_data_t *sdbd, simpledb_range_t *rangedp)
{
	if(rangedp!=NULL){
		UB_SD_RELMEM(SIMPLEDB_RANGEINST, rangedp);
	}
	return;
}

int simpledb_get_from_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp,
			    void **kdata, uint32_t *ksize, void **vdata, uint32_t *vsize,
			    int direction)
{
	dbdata_t *dbd;
	int res=-1;
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	if(!rangedp || !rangedp->np){goto erexit;}
	dbd=(dbdata_t *)rangedp->np->data;
	if(kdata!=NULL){
		*kdata=dbd->kd.kdata;
		*ksize=dbd->kd.ksize;
	}
	if(vdata!=NULL){
		*vdata=dbd->vdata;
		*vsize=dbd->vsize;
	}
	if(direction==UC_DBAL_FORWARD){
		if(rangedp->np==rangedp->n2){
			rangedp->np=NULL;
		}else{
			rangedp->np=rangedp->np->next;
		}
	}else if(direction==UC_DBAL_BACKWARD){
		if(rangedp->np==rangedp->n1){
			rangedp->np=NULL;
		}else{
			rangedp->np=rangedp->np->prev;
		}
	}else if(direction==UC_DBAL_NOMOVE){
	}else{
		UB_LOG(UBL_ERROR, "%s:invlid direction\n", __func__);
		goto erexit;
	}
	res=0;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return res;
}

static int move_in_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp,
			 int direction)
{
	int res=0;
	if(direction==UC_DBAL_FORWARD){
		if(rangedp->np==rangedp->n2){
			res=-1;
		}else{
			rangedp->np=rangedp->np->next;
		}
	}else if(direction==UC_DBAL_BACKWARD){
		if(rangedp->np==rangedp->n1){
			res=-1;
		}else{
			rangedp->np=rangedp->np->prev;
		}
	}else if(direction==UC_DBAL_NOMOVE){
	}else{
		UB_LOG(UBL_ERROR, "%s:invlid direction\n", __func__);
		res=-1;
	}
	return res;
}

int simpledb_move_in_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp,
			   int direction)
{
	int res=-1;
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	if(rangedp && rangedp->np){
		if(move_in_range(sdbd, rangedp, direction)){
			rangedp->np=NULL;
		}
		res=0;
	}
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return res;
}

void simpledb_move_top_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp)
{
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	rangedp->np=rangedp->n1;
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
}

void simpledb_move_bottom_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp)
{
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	rangedp->np=rangedp->n2;
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
}

int simpledb_del_in_range(simpledb_data_t *sdbd, simpledb_range_t *rangedp,
			  int direction)
{
	dbdata_t *dbd;
	int res=-1;
	struct ub_list_node *dnp;

	if(direction==UC_DBAL_NOMOVE){
		UB_LOG(UBL_ERROR, "%s:directon must be forward or back\n", __func__);
		return -1;
	}
	if(!rangedp->np){return -1;}
	CB_THREAD_MUTEX_LOCK(&sdbd->dbmutex);
	dnp=rangedp->np;
	dbd=(dbdata_t *)(dnp->data);
	if(dbd->refcounter>0u){
		UB_LOG(UBL_ERROR, "%s:the other user is locking the data\n", __func__);
		goto erexit;
	}

	if((dnp==rangedp->n1) && (dnp==rangedp->n2)){
		rangedp->n1=NULL;
		rangedp->n2=NULL;
		rangedp->np=NULL;
	}else if(dnp==rangedp->n2){
		rangedp->n2=dnp->prev;
		if(direction==UC_DBAL_FORWARD){
			rangedp->np=NULL;
		}else if(direction==UC_DBAL_BACKWARD){
			rangedp->np=rangedp->n2;
		}else{
			goto erexit;
		}
	}else if(dnp==rangedp->n1){
		rangedp->n1=dnp->next;
		if(direction==UC_DBAL_FORWARD){
			rangedp->np=rangedp->n1;
		}else if(direction==UC_DBAL_BACKWARD){
			rangedp->np=NULL;
		}else{
			goto erexit;
		}
	}else{
		if(direction==UC_DBAL_FORWARD){
			rangedp->np=rangedp->np->next;
		}else if(direction==UC_DBAL_BACKWARD){
			rangedp->np=rangedp->np->prev;
		}else{
			goto erexit;
		}
	}
	ub_list_unlink(&sdbd->dblist, dnp);
	dbnode_clear(dnp, NULL);
	res=0;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&sdbd->dbmutex);
	return res;
}

int simpledb_wait_release(simpledb_data_t *sdbd, int toutms)
{
	int res=0;
	struct timespec mtots;
	int64_t mtout;
	mtout=(int64_t)ub_rt_gettime64()+(toutms*UB_MSEC_NS);
	UB_NSEC2TS(mtout, mtots);
	if(CB_SEM_TIMEDWAIT(&sdbd->relwait_sem, &mtots)!=0){
		UB_LOG(UBL_ERROR, "%s:can't get relwait_sem\n", __func__);
		res=-1;
	}
	return res;
}
