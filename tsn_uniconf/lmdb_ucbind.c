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
#include "uc_dbal.h"
#include <lmdb.h>
#include <sys/stat.h>
#include <errno.h>
#include "tsn_combase/combase.h"
#include "yangs/yang_modules.h"

typedef struct lmdb_ucbind_data {
	MDB_env *env;
	MDB_txn *txn;
	MDB_dbi dbi;
	CB_THREAD_MUTEX_T putlock;
	CB_SEM_T *relwait;
	bool threadmode;
} lmdb_ucbind_data_t;

#define MAXDBMAPSIZE (1*1024*1024)
#define RELWAITSEM "/lmdb_relwait"
#define DPERM_MODE 0777
#define FPERM_MODE 0666

#define LMDB_INTERNAL_TXN_WAIT 200
#define TXN_RECAPTURE(x, d, s)						\
	if(!lmubd->txn && uc_dbal_getdb(dbald, LMDB_INTERNAL_TXN_WAIT, d, s)){ \
	UB_LOG(UBL_ERROR, "%s:no access\n", __func__);			\
	return x;							\
	}


// comarison with a shorter size of key
// if matched, a longer key is bigger.
static int lmdbcmp(const MDB_val *a, const MDB_val *b)
{
	return uc_dbal_keycmp((uint8_t*)a->mv_data, (uint32_t)a->mv_size,
			      (uint8_t*)b->mv_data, (uint32_t)b->mv_size);
}

static int create_directory(const char *dname)
{
	struct stat stb;
	if(!stat(dname, &stb)){
		if((stb.st_mode & S_IFMT)==S_IFDIR){return 0;}
		UB_LOG(UBL_ERROR, "%s:not a directory:%s\n", __func__, dname);
		return -1;
	}
	if(errno!=ENOENT){
		UB_LOG(UBL_ERROR, "%s:stat error,%s,%s\n", __func__,
		       strerror(errno), dname);
		return -1;
	}
	if(mkdir(dname, DPERM_MODE)){
		UB_LOG(UBL_ERROR, "%s:mkdir error,%s,%s\n", __func__,
		       strerror(errno), dname);
		return -1;
	}
	return 0;
}

uc_dbald *uc_dbal_open(const char *dbal_options, const char *mode, uint8_t callmode)
{
	lmdb_ucbind_data_t *lmubd;
	static lmdb_ucbind_data_t *lmubd_shared=NULL;
	int res=-1;
	const char *ermes="";

	if(UC_CALL_THREADSLAVE(callmode)){
		if(!lmubd_shared){
			if((mode!=NULL) && (strchr(mode, 'm')!=NULL)) return NULL;
			UB_LOG(UBL_ERROR, "%s:'uniconf' side should call first\n",
			       __func__);
			return NULL;
		}
		return lmubd_shared;
	}
	lmubd=malloc(sizeof(lmdb_ucbind_data_t));
	if(ub_assert_fatal(lmubd!=NULL, __func__, NULL)){return NULL;}
	memset(lmubd, 0, sizeof(lmdb_ucbind_data_t));
	if(CB_THREAD_MUTEX_INIT(&lmubd->putlock, NULL)){goto erexit;}
	if(UC_CALL_UNICONF(callmode)){
		CB_SEM_UNLINK(RELWAITSEM);
		lmubd->relwait=CB_SEM_OPEN(RELWAITSEM, O_CREAT|O_EXCL, FPERM_MODE, 1);
	}else{
		lmubd->relwait=CB_SEM_OPEN(RELWAITSEM, 0);
	}
	if(lmubd->relwait==SEM_FAILED) {
		ermes="can't create semaphore";
		goto erexit;
	}

	if(mdb_env_create(&lmubd->env)){
		ermes="can't create env";
		goto erexit;
	}
	if(create_directory(dbal_options)){goto erexit;}
	mdb_env_set_maxdbs(lmubd->env, 10); // extra entries are 10
	mdb_env_set_mapsize(lmubd->env, (size_t)MAXDBMAPSIZE);

	res=mdb_env_open(lmubd->env, dbal_options, 0, FPERM_MODE);
	if(res){
		ermes="can't open env";
		goto erexit;
	}
	if(UC_CALL_THREAD(callmode)){
		lmubd->threadmode=true;
		if(uc_dbal_getdb((uc_dbald *)(lmubd), LMDB_INTERNAL_TXN_WAIT, NULL, 0)){
			ermes="can't create a db";
			goto erexit;
		}
	}
	lmubd_shared=lmubd;
	return (uc_dbald *)(lmubd);
erexit:
	if((mode==NULL) || (strchr(mode, 'm')==NULL)){
		UB_LOG(UBL_ERROR, "%s:%s, res=%d\n", __func__, ermes, res);
	}
	uc_dbal_close(lmubd, callmode);
	return NULL;
}

void uc_dbal_close(uc_dbald *dbald, uint8_t callmode)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	if(!lmubd){return;}
	if(UC_CALL_THREADSLAVE(callmode)){return;}
	CB_THREAD_MUTEX_DESTROY(&lmubd->putlock);
	// the same as 'uc_dbal_releasedb',
	// so 'uc_dbal_releasedb' is not needed when 'uc_dbal_close' is called
	if(lmubd->txn){
		mdb_txn_commit(lmubd->txn);
		CB_SEM_POST(lmubd->relwait);
	}
	CB_SEM_CLOSE(lmubd->relwait);
	if(UC_CALL_UNICONF(callmode)){CB_SEM_UNLINK(RELWAITSEM);}
	if(lmubd->env){mdb_env_close(lmubd->env);}
	free(lmubd);
}

int uc_dbal_save(uc_dbald *dbald)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	UB_TLOG(UBL_DEBUG, "%s:\n", __func__);
	TXN_RECAPTURE(-1, NULL, 0);
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	mdb_txn_commit(lmubd->txn);
	mdb_env_sync(lmubd->env, 1);
	lmubd->txn=NULL;
	CB_SEM_POST(lmubd->relwait);
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return 0;
}

/* to debug uc_dbal_releasedb interval, we need to know who captures it at the time
 * 'program_invocation_short_name' is not portable,
 * RELEASEDB_DEBUG is defined only for the debug.
 */
#ifdef RELEASEDB_DEBUG
enum {
	DB_CAPTREC_KEY=0,
	DB_CAPTREC_TS,
	DB_CAPTREC_MAXDTS,
	DB_CAPTREC_MAX_PNAME,
	DB_CAPTREC_MAX_KEY,
	DB_CAPTREC_MAX_TS,
};
extern char *program_invocation_short_name;
static void db_capture_rec(lmdb_ucbind_data_t *lmubd,
			   uint8_t *key, uint32_t ksize)
{
	uint8_t aps[]={XL4_DATA_RW, UC_LAST_DB_ACCESS, 0};
	MDB_val mkv={3, aps};
	MDB_val mvv;
	uint64_t ts=ub_rt_gettime64();
	aps[2]=DB_CAPTREC_KEY;
	mvv.mv_size=ksize;
	mvv.mv_data=key;
	mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
	aps[2]=DB_CAPTREC_TS;
	mvv.mv_size=sizeof(uint64_t);
	mvv.mv_data=&ts;
	mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
	return;
}

static int db_release_rec(lmdb_ucbind_data_t *lmubd)
{
	uint8_t aps[]={XL4_DATA_RW, UC_LAST_DB_ACCESS, 0};
	MDB_val mkv={3, aps};
	MDB_val mvv;
	uint64_t ts=ub_rt_gettime64();
	uint64_t dts, mdts=0;

	aps[2]=DB_CAPTREC_TS;
	if(mdb_get(lmubd->txn, lmubd->dbi, &mkv, &mvv)){return -1;}
	dts=ts-*((uint64_t*)mvv.mv_data);
	aps[2]=DB_CAPTREC_MAXDTS;
	if(!mdb_get(lmubd->txn, lmubd->dbi, &mkv, &mvv)){
		mdts=*((uint64_t*)mvv.mv_data);
	}
	if(dts>mdts){
		// update maximum holding time holder record
		mvv.mv_data=&dts;
		mvv.mv_size=sizeof(uint64_t);
		mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
		aps[2]=DB_CAPTREC_MAX_PNAME;
		mvv.mv_data=program_invocation_short_name;
		mvv.mv_size=strlen(program_invocation_short_name)+1;
		UB_LOG(UBL_INFO, "Max holding:%s, %d usec\n",
		       (char*)mvv.mv_data, (int)(dts/UB_USEC_NS));
		mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
		aps[2]=DB_CAPTREC_KEY;
		if(!mdb_get(lmubd->txn, lmubd->dbi, &mkv, &mvv)){
			aps[2]=DB_CAPTREC_MAX_KEY;
			mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
			ub_hexdump(true, true, mvv.mv_data, mvv.mv_size, 0);
		}
		aps[2]=DB_CAPTREC_TS;
		if(!mdb_get(lmubd->txn, lmubd->dbi, &mkv, &mvv)){
			aps[2]=DB_CAPTREC_MAX_TS;
			mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
			dts=*((uint64_t*)mvv.mv_data);
			UB_LOG(UBL_INFO, "  ts:%d-%d\n",
			       (int)(dts/UB_SEC_NS), (int)(dts%UB_SEC_NS));
		}
	}
	return 0;
}
#else // not RELEASEDB_DEBUG
#define db_release_rec(a) 0
#define db_capture_rec(a,b,c)
#endif

int uc_dbal_getdb(uc_dbald *dbald, int toutms, uint8_t *key, uint32_t ksize)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	int res=-1;
	struct timespec mtots;
	uint64_t ts, mtout;
	const char *ermes="";
	if(lmubd->txn){
		UB_LOG(UBL_ERROR, "%s:it is not released\n", __func__);
		return -1;
	}
	ts=ub_rt_gettime64();
	mtout=ts+(uint64_t)toutms*UB_MSEC_NS;
	UB_NSEC2TS(mtout, mtots);
	if(CB_SEM_TIMEDWAIT(lmubd->relwait, &mtots)){
		UB_TLOG(UBL_ERROR, "%s:can't get it in the timeout\n", __func__);
		if(key!=NULL){ub_hexdump(true, true, key, ksize, 0);}
		return -1;
	}
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=mdb_txn_begin(lmubd->env, NULL, 0, &lmubd->txn);
	if(res){
		ermes="can't begin transaction";
		goto erexit;
	}
	res=mdb_dbi_open(lmubd->txn, NULL, MDB_CREATE, &lmubd->dbi);
	if(res){
		ermes="can't open dbi";
		goto erexit;
	}
	res=mdb_set_compare(lmubd->txn, lmubd->dbi, lmdbcmp);
	if(res){
		ermes="can't set compare function";
		goto erexit;
	}
	res=0;
erexit:
	if((res==0) && (key!=NULL)){
		db_capture_rec(lmubd, key, ksize);
	}
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	if(res){
		UB_TLOG(UBL_ERROR, "%s:%s, res=%d\n", __func__, ermes, res);
		uc_dbal_releasedb(lmubd);
	}
	return res;
}

void uc_dbal_releasedb(uc_dbald *dbald)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	if(!lmubd->txn){return;}
	if(lmubd->threadmode){
		// in the thread mode, one txn is shared by threads.
		// no need to releasedb to create a different txn
		return;
	}
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	if(db_release_rec(lmubd)){
		UB_LOG(UBL_WARN, "%s:can't record\n", __func__);
	}
	mdb_txn_commit(lmubd->txn);
	lmubd->txn=NULL;
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	CB_SEM_POST(lmubd->relwait);
}

int uc_dbal_create(uc_dbald *dbald, void *key, uint32_t ksize,
		    void *value, uint32_t vsize)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	int res=-1;
	MDB_val mkv={ksize, key};
	MDB_val mvv={vsize, value};
	TXN_RECAPTURE(-1, key, ksize);
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return res;
}

int uc_dbal_append(uc_dbald *dbald, void *key, uint32_t ksize,
		    void *value, uint32_t vsize)
{
	int res=-1;
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	MDB_val mkv={ksize, key};
	void *nvalue=NULL;
	MDB_val mvv;
	TXN_RECAPTURE(-1, key, ksize);
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	if(!mdb_get(lmubd->txn, lmubd->dbi, &mkv, &mvv)){
		nvalue=malloc(mvv.mv_size+vsize);
		if(ub_assert_fatal(nvalue!=NULL, __func__, NULL)){return -1;}
		memset(nvalue, 0, mvv.mv_size+vsize);
		memcpy(nvalue, mvv.mv_data, mvv.mv_size);
		memcpy(nvalue+mvv.mv_size, value, vsize);
		mvv.mv_size+=vsize;
		mvv.mv_data=nvalue;
	}else{
		mvv.mv_size=vsize;
		mvv.mv_data=value;
	}
	res=mdb_put(lmubd->txn, lmubd->dbi, &mkv, &mvv, 0);
	if(nvalue){free(nvalue);}
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return res;
}

int uc_dbal_get(uc_dbald *dbald, void *key, uint32_t ksize,
		 void **value, uint32_t *vsize)
{
	int res;
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	MDB_val mkv={ksize, key};
	MDB_val mvv;
	TXN_RECAPTURE(-1, key, ksize);
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=mdb_get(lmubd->txn, lmubd->dbi, &mkv, &mvv);
	if(!res){
		*vsize=mvv.mv_size;
		*value=mvv.mv_data;
	}
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return res;
}

int uc_dbal_get_release(uc_dbald *dbald, void *key, uint32_t ksize,
			 void *value, uint32_t vsize)
{
	return 0;
}

int uc_dbal_del(uc_dbald *dbald, void *key, uint32_t ksize)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	MDB_val mkv={ksize, key};
	int res;
	TXN_RECAPTURE(-1, key, ksize);
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=mdb_del(lmubd->txn, lmubd->dbi, &mkv, NULL);
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return res;
}

typedef struct key_range{
	MDB_cursor *cursor;
	MDB_val mkv1;
	MDB_val mkv2;
	MDB_val data;
	int diredge; // -2:lower, -1:after down, 0:after no move, 1:after up, 2:upper
}key_range_t;

/*
 * With lmdb, creating multiple kranges is not a problem.
 * The cursor object belongs to krange, no interference between multiple cursors.
 */
uc_range *uc_get_range(uc_dbald *dbald, void *key1, uint32_t ksize1,
		       void *key2, uint32_t ksize2)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	key_range_t *krange;
	int res=-1;
	const char *emes="";
	void *kdata;
	TXN_RECAPTURE(NULL, key1, ksize1);
	krange=malloc(sizeof(key_range_t));
	if(ub_assert_fatal(krange!=NULL, __func__, NULL)){return NULL;}
	memset(krange, 0, sizeof(key_range_t));
	krange->mkv1.mv_data=key1;
	krange->mkv1.mv_size=ksize1;
	krange->mkv2.mv_data=key2;
	krange->mkv2.mv_size=ksize2;

	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=mdb_cursor_open(lmubd->txn, lmubd->dbi, &krange->cursor);
	if(res) {
		emes="mdb_cursor_open";
		goto erexit;
	}
	res=mdb_cursor_get(krange->cursor, &krange->mkv2, &krange->data, MDB_SET_RANGE);
	if(res){
		res=mdb_cursor_get(krange->cursor, &krange->mkv2, &krange->data, MDB_LAST);
		if(res){
			emes="mdb_cursor_get to get the bottom key";
			goto erexit;
		}
	}else{
		if(krange->mkv2.mv_size!=ksize2 ||
		   memcmp(krange->mkv2.mv_data, key2, ksize2)){
			// key2 doesn't exist, krange->mkv2 should be one previous one
			res=mdb_cursor_get(krange->cursor, &krange->mkv2, &krange->data, MDB_PREV);
			if(res){
				emes="mdb_cursor_get to get one from the bottom key";
				goto erexit;
			}
		}
	}
	if(lmdbcmp(&krange->mkv1, &krange->mkv2)>0){
		emes="no keys in the ranage";
		res=-1;
		goto erexit;
	}
	res=mdb_cursor_get(krange->cursor, &krange->mkv1, &krange->data, MDB_SET_RANGE);
	if(res) {
		emes="mdb_cursor_get to get the top key";
		goto erexit;
	}
	res=-1;
	kdata=malloc(krange->mkv1.mv_size);
	if(ub_assert_fatal(kdata!=NULL, __func__, NULL)){
		goto erexit;
	}
	memcpy(kdata, krange->mkv1.mv_data, krange->mkv1.mv_size);
	krange->mkv1.mv_data=kdata;
	kdata=malloc(krange->mkv2.mv_size);
	if(ub_assert_fatal(kdata!=NULL, __func__, NULL)){
		free(krange->mkv1.mv_data);
		goto erexit;
	}
	memcpy(kdata, krange->mkv2.mv_data, krange->mkv2.mv_size);
	krange->mkv2.mv_data=kdata;
	res=0;
#if !UB_LOG_IS_COMPILED(UBL_DEBUG)
	(void)emes;
#endif
erexit:
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	if(res){
		krange->mkv1.mv_data=NULL;
		krange->mkv2.mv_data=NULL;
		uc_get_range_release(dbald, krange);
		UB_LOG(UBL_DEBUG, "%s:no keys:%s, res=%d\n", __func__, emes, res);
		return NULL;
	}
	return (uc_range *)krange;
}

void uc_get_range_release(uc_dbald *dbald, uc_range *range)
{
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	key_range_t *krange=(key_range_t *)range;
	if(!lmubd->txn){
		UB_LOG(UBL_ERROR, "%s:txn is not available\n", __func__);
		return;
	}
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	if(krange->cursor){mdb_cursor_close(krange->cursor);}
	if(krange->mkv1.mv_data){
		free(krange->mkv1.mv_data);
		krange->mkv1.mv_data=NULL;
	}
	if(krange->mkv2.mv_data){
		free(krange->mkv2.mv_data);
		krange->mkv2.mv_data=NULL;
	}
	free(krange);
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
}

static int get_key_in_range(lmdb_ucbind_data_t *lmubd, uc_range *range,
			    MDB_val *key, MDB_val *data, int direction)
{
	int res=-1;
	key_range_t *krange=(key_range_t *)range;
	MDB_val ckey;
	MDB_val cdata;
	MDB_val dkey;
	MDB_val ddata;

	if(krange->diredge==-2 || krange->diredge==2){return -1;}
	res=mdb_cursor_get(krange->cursor, &ckey,
			   &cdata, MDB_GET_CURRENT);
	if(res){return -1;}
	if(key){*key=ckey;}
	if(data){*data=cdata;}
	switch(direction){
	case UC_DBAL_FORWARD:
		if(lmdbcmp(&ckey, &krange->mkv2)>=0){
			if(krange->diredge==2){
				res=1;
				break; // it is at the bottom
			}
			krange->diredge=2;
		}else{
			res=mdb_cursor_get(krange->cursor, &dkey, &ddata, MDB_NEXT);
			krange->diredge=1;
		}
		res=0;
		break;
	case UC_DBAL_BACKWARD:
		if(lmdbcmp(&ckey, &krange->mkv1)<=0){
			if(krange->diredge==-2){
				res=-1;
				break; // it is at the top
			}
			krange->diredge=-2;
		}else{
			mdb_cursor_get(krange->cursor, &dkey, &ddata, MDB_PREV);
			krange->diredge=-1;
		}
		res=0;
		break;
	case UC_DBAL_NOMOVE:
		if(krange->diredge>-2 && krange->diredge<2){
			res=0;
		}else{
			if(krange->diredge==-2){res=-1;}
			else{res=1;}
		}
		break;
	}
	return res;
}

int uc_get_keyvalue_in_range(uc_dbald *dbald, uc_range *range, void **kdata, uint32_t *ksize,
			     void **vdata, uint32_t *vsize, int direction)
{
	MDB_val key;
	MDB_val data;
	int res;
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	if(!lmubd->txn){
		UB_LOG(UBL_ERROR, "%s:txn is not available\n", __func__);
		return -1;
	}
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=get_key_in_range(lmubd, range, &key, &data, direction);
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	if(res){return res;}
	if(kdata!=NULL){
		*kdata=key.mv_data;
		*ksize=key.mv_size;
	}
	if(vdata!=NULL){
		*vdata=data.mv_data;
		*vsize=data.mv_size;
	}
	return 0;
}

int uc_get_key_in_range(uc_dbald *dbald, uc_range *range, void **kdata, uint32_t *ksize,
			int direction)
{
	return uc_get_keyvalue_in_range(dbald, range, kdata, ksize, NULL, NULL, direction);
}

int uc_get_value_in_range(uc_dbald *dbald, uc_range *range, void **vdata, uint32_t *vsize,
			  int direction)
{
	return uc_get_keyvalue_in_range(dbald, range, NULL, NULL, vdata, vsize, direction);
}

int uc_move_in_range(uc_dbald *dbald, uc_range *range, int direction)
{
	int res;
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	if(!lmubd->txn){
		UB_LOG(UBL_ERROR, "%s:txn is not available\n", __func__);
		return -1;
	}
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=get_key_in_range(lmubd, range, NULL, NULL, direction);
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return res;
}

int uc_del_in_range(uc_dbald *dbald, uc_range *range, int direction)
{
	int res=-1;
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	key_range_t *krange=(key_range_t *)range;
	MDB_val mkv;
	MDB_val tmkv;
	const char* emes;
	bool deltop, delbottom;
	if(!lmubd->txn){
		UB_LOG(UBL_ERROR, "%s:txn is not available\n", __func__);
		return -1;
	}
	if(direction==0){
		UB_LOG(UBL_ERROR, "%s:directon must be forward or back\n", __func__);
		return -1;
	}
	if(krange->diredge==-2 || krange->diredge==2){return -1;}

	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=get_key_in_range(lmubd, range, &mkv, NULL, direction);
	if(res!=0){goto erexit;}
	deltop=(lmdbcmp(&mkv, &krange->mkv1)==0);
	delbottom=(lmdbcmp(&mkv, &krange->mkv2)==0);
	if(deltop && delbottom){
		// all data in group were deleted
		krange->diredge=2;
	}else if(deltop){
		mdb_cursor_get(krange->cursor, &krange->mkv1,
			       &krange->data, MDB_SET);
		mdb_cursor_get(krange->cursor, &tmkv,
			       &krange->data, MDB_NEXT);
	}else if(delbottom){
		mdb_cursor_get(krange->cursor, &krange->mkv2,
			       &krange->data, MDB_SET);
		mdb_cursor_get(krange->cursor, &tmkv,
			       &krange->data, MDB_PREV);
	}
	if(mdb_del(lmubd->txn, lmubd->dbi, &mkv, NULL)){
		emes="failed to delete";
		goto erexit;
	}
	res=0;
erexit:
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	if(res){UB_LOG(UBL_ERROR, "%s:%s, res=%d\n", __func__, emes, res);}
	return res;
}

void uc_move_top_in_range(uc_dbald *dbald, uc_range *range)
{
	int res;
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	key_range_t *krange=(key_range_t *)range;
	MDB_val tmkv;
	if(!lmubd->txn){
		UB_LOG(UBL_ERROR, "%s:txn is not available\n", __func__);
		return;
	}
	if(!krange->mkv1.mv_data){return;}
	// in case the top was deleted, use MDB_SET_RANGE and need mutable key data
	tmkv.mv_data=krange->mkv1.mv_data;
	tmkv.mv_size=krange->mkv1.mv_size;
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=mdb_cursor_get(krange->cursor, &tmkv,
			   &krange->data, MDB_SET_RANGE);
	if(!res){
		krange->diredge=-1;
	}
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return;
}

void uc_move_bottom_in_range(uc_dbald *dbald, uc_range *range)
{
	int res;
	lmdb_ucbind_data_t *lmubd=(lmdb_ucbind_data_t *)dbald;
	key_range_t *krange=(key_range_t *)range;
	MDB_val tmkv;
	if(!lmubd->txn){
		UB_LOG(UBL_ERROR, "%s:txn is not available\n", __func__);
		return;
	}
	if(!krange->mkv2.mv_data){return;}
	// in case the bottom was deleted, use MDB_SET_RANGE and need mutable key data
	tmkv.mv_data=krange->mkv2.mv_data;
	tmkv.mv_size=krange->mkv2.mv_size;
	CB_THREAD_MUTEX_LOCK(&lmubd->putlock);
	res=mdb_cursor_get(krange->cursor, &tmkv,
			   &krange->data, MDB_SET_RANGE);
	if(res){
		// in case the deleted bottom was the DB's bottom
		res=mdb_cursor_get(krange->cursor, &tmkv,
				   &krange->data, MDB_LAST);
	}
	if(!res){
		krange->diredge=1;
	}
	CB_THREAD_MUTEX_UNLOCK(&lmubd->putlock);
	return;
}

int uc_wait_release(uc_dbald *dbald, int toutms)
{
	return 0;
}
