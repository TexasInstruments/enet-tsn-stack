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
#ifndef YANG_DB_ACCESS_H_
#define YANG_DB_ACCESS_H_

#include <tsn_unibase/unibase.h>
#include <tsn_combase/cb_thread.h>
#include "../uc_dbal.h"
#include "../hal/uc_hwal.h"
#include "../uc_notice.h"

/* number of network devices never go beyond this number */
#define XL4_DATA_ABS_MAX_NETDEVS 16
#define XL4_DATA_PATH_STR_SHORTLEN 64

#define YANGINIT_GEN_SMEM yanginit_gen_smem
#ifndef YANGINIT_GEN_SNUM
#define YANGINIT_GEN_SNUM 500
#endif //YANGINIT_GEN_SNUM

#ifndef YANGINIT_GEN_SSIZE
#define YANGINIT_GEN_SSIZE 8
#endif //YANGINIT_GEN_SSIZE

// gcc can detect big endian by the next.
// the other big endian compiler needs to define __BID_ENDIAN__
#ifndef BIG_ENDIAN__
#if defined(__BYTE_ORDER__)&&(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define BIG_ENDIAN__ 1
#else
#define BIG_ENDIAN__ 0
#endif
#endif

#if BIG_ENDIAN__
#define ADJUST_ENDIAN(x, mindex, csize) memcpy(x, (x)+(mindex), size)
#else
#define ADJUST_ENDIAN(x, mindex, csize)
#endif

#ifdef UC_RUNCONF
#define UC_RUNCONF_DATA yang_db_runtime_dataq_t
#define UC_RUNCONF_INIT(dbald,hwald) yang_db_runtime_init(dbald,hwald)
#define UC_RUNCONF_CLOSE(ydrd) yang_db_runtime_close(ydrd)
#define UC_RUNCONF_READFILE(ydrd,fname) yang_db_runtime_readfile(ydrd, fname, NULL)
#else
#define UC_RUNCONF_DATA void
#define UC_RUNCONF_INIT(dbald,hwald) NULL
#define UC_RUNCONF_CLOSE(ydrd)
#define UC_RUNCONF_READFILE(ydrd,fname) 0
#endif // UC_RUNCONF

#define UC_INIT_COPY_INSTANCE_PRE "COPY_INSTANCE_"

#define YANG_DB_READ_SETUP(dbpara, _aps, _kvs, _kss)	\
	dbpara.aps=(_aps);				\
	dbpara.kvs=(_kvs);				\
	dbpara.kss=(_kss);				\
	dbpara.atype=YANG_DB_ACTION_READ;		\
	dbpara.onhw=YANG_DB_ONHW_NOACTION;

#define YANG_DB_WRITE_SETUP(dbpara, _aps, _kvs, _kss)	\
	dbpara.aps=(_aps);				\
	dbpara.kvs=(_kvs);				\
	dbpara.kss=(_kss);				\
	dbpara.atype=YANG_DB_ACTION_CREATE;		\
	dbpara.onhw=YANG_DB_ONHW_NOACTION;

enum {
	YANG_DB_ACTION_CREATE,
	YANG_DB_ACTION_APPEND,
	YANG_DB_ACTION_READ,
	YANG_DB_ACTION_READ_RELEASE,
	YANG_DB_ACTION_DELETE,
	YANG_DB_ACTION_NONE,
};

enum {
	YANG_DB_ONHW_NOACTION,
	YANG_DB_ONHW_ONUPDATE,
	YANG_DB_ONHW_ALWAYS,
};

#define YANG_VTYPE_ENUM_e2e 1
#define YANG_VTYPE_ENUM_p2p 2
#define YANG_VTYPE_ENUM_no_mechanism 254
#define YANG_VTYPE_ENUM_common_p2p 3
#define YANG_VTYPE_ENUM_special 4


typedef void* kvs_t;

typedef struct yang_db_access_para{
	uint8_t atype; // YANG_DB_ACTION_*
	uint8_t onhw; // YANG_DB_ONHW_*
	uint8_t *paps;
	uint8_t *aps;
	kvs_t *kvs;
	uint8_t *kss;
	void *value;
	uint32_t vsize;
} yang_db_access_para_t;

typedef struct yang_db_item_access {
	uc_dbald *dbald;
	uc_notice_data_t *ucntd;
	yang_db_access_para_t dbpara;
	CB_THREAD_MUTEX_T *mutex;
	CB_SEM_T readrelsem;
}yang_db_item_access_t;

/************************************
 * specific YANG data support
 * 'FTYPE' is like [2-character][kN][vkM]
 *
 * optional 2 prefix character 'ny' is used for non-yong type data
 * 2 character abbreviations
 *  if: ietf-interfaces, qb: ieee802-dot1q-bridge, pt: ieee1588-ptp-tt,
 *  mr: xmrpd, tu: ieee802-dot1q-tsn-config-uni, tr: excelfore-tsn-remote,
 *  nc: excelfore-netconf-server, yl: ietf-yang-library
 *  nm: ietf-netconf-monitoring
 *  [kN] k1:1-key access, k2:2-key access,,,
 *  [vkN] vk0:vk1=1-value_key access, vk2:2-value_key access,,,
 ************************************/
#define YDBI_CONFIG false
#define YDBI_STATUS true
#define YDBI_KEEP_LOCK true
#define YDBI_REL_LOCK false
#define YDBI_PUSH_NOTICE_IN_UNICONF 2
#define YDBI_PUSH_NOTICE 1
#define YDBI_NO_NOTICE 0

/**
 * @brief read an item data from the db which is set by 'ydbi_access_init'
 * @param FTYPE	the word to specify the function(look at the comment at the top)
 * @param RVAL	a pointer returned from the DB
 * @note
 *  After successful YDBI_GET_ITEM call, YDBI_REL_ITEM must be called ASAP.
 *  The other YDBI_* actions are all blocked before YDBI_REL_ITEM is called.
 *  When you are sure 'ydbia.dbpara.*' is untached,
 *  it is okay to call 'ydbi_get_item_release' instead of 'YDBI_REL_ITEM'.
 *  The returned value on RVAL pointer, points an address in the DB memory.
*/
#define YDBI_GET_ITEM(FTYPE, RVAL, ...)					\
	ydbi_get_item_##FTYPE(ydbi_access_handle(), &(RVAL), __VA_ARGS__)

/**
 * @brief read an item data from the db which is set by 'ydbi_access_init'
 * @param FTYPE	the word to specify the function(look at the comment at the top)
 * @note
 *  For 'simpledb' and 'lmdb',
 *  'ydbia->dbpara.value' and 'ydbia->dbpara.vsize' are not needed to release.
 *  All the other 'ydbia->dbpara.*' should have the parameters to release
 *  the item from the DB.
*/
#define YDBI_REL_ITEM(FTYPE, ...)					\
	ydbi_rel_item_##FTYPE(ydbi_access_handle(), __VA_ARGS__)

/**
 * @brief substitute RES data to the read data from the DB
 * @note
 *  The data is released in this macro call, no need of a separated release call.
 *  E.G. YDBI_GET_ITEM_VSUBST(uint8_t*, ifk3vk0, res, rval, "eth0",,,,)
*/
#define YDBI_GET_ITEM_VSUBST(CAST, FTYPE, RES, RVAL, ...)			\
	{								\
		if(ydbi_get_item_##FTYPE(ydbi_access_handle(), &(RVAL), __VA_ARGS__)>0){ \
			RES=*((CAST)(RVAL));				\
			ydbi_get_item_release(ydbi_access_handle(), YDBI_REL_LOCK); \
		}							\
	}

/**
 * @brief substitute RES poiter to the read data pointer from the DB
 * @note
 *  the static value is released in this macro call, but the DB data is not released.
 *  It means, another YDBI_GET_ITEM is available, but the data is blocked in the DB.
 *  In 'simpledb', the blocked data becomes READ ONLY.
 *  YDBI_REL_ITEM must be called when using the data finishes.
*/
#define YDBI_GET_ITEM_PSUBST(FTYPE, RES, VSIZE, RVAL, ...)		\
	{								\
		VSIZE=ydbi_get_item_##FTYPE(ydbi_access_handle(), &(RVAL), __VA_ARGS__); \
		if((VSIZE)>0){						\
			RES=(char*)RVAL;					\
			ydbi_get_item_release(ydbi_access_handle(), YDBI_KEEP_LOCK); \
		}							\
	}

/**
 * @brief set a value into an item in the DB.
 * @note
 *  Check each 'ydbi_set_item_*' function for the detail.
 */
#define YDBI_SET_ITEM(FTYPE, ...)\
	ydbi_set_item_##FTYPE(ydbi_access_handle(), __VA_ARGS__)

/**
 * @brief delete an item in the DB item.
 * @note
 *  Check each 'ydbi_del_item_*' function for the detail.
 */
#define YDBI_DEL_ITEM(FTYPE, ...)\
	ydbi_del_item_##FTYPE(ydbi_access_handle(), __VA_ARGS__)

/**
 * @brief Initialization to use ydbi_* functions.
 * @param dbald	General DB access pointer, this is always needed.
 * @param ucntd	notice data pointer, can be NULL not to use the notice.
 * @note
 *  ydbi_access uses internal static variables with mutex.
 *  No guarantee to keep the internal static values between calls.
 */
void ydbi_access_init(uc_dbald *dbald, uc_notice_data_t *ucntd);

/**
 * @brief close ydbi_access
 */
void ydbi_access_close(void);

/**
 * @brief return ydbi access pointer in the internal static variable.
 */
yang_db_item_access_t *ydbi_access_handle(void);

int ydbi_get_head(yang_db_item_access_t *ydbia, const char *fname);
int ydbi_set_head(yang_db_item_access_t *ydbia, const char *fname);
int ydbi_del_head(yang_db_item_access_t *ydbia, const char *fname);
int ydbi_get_foot(yang_db_item_access_t *ydbia, const char *fname,
		  void **rval, ub_dbgmsg_level_t emlevel);
int ydbi_rel_head(yang_db_item_access_t *ydbia, const char *fname);
int ydbi_rel_foot(yang_db_item_access_t *ydbia, const char *fname);
int ydbi_set_foot(yang_db_item_access_t *ydbia, const char *fname, ub_dbgmsg_level_t emlevel,
		  uint8_t notice);

/**
 * @note if keeplocking=true, the DB item keeps a locked status.
 *	 how the locking works depends on DB.
 *	 'simpledb' has 'refcounter' on each item, and it locks the item. READONLY when locked.
 *	 'lmdb' doesn't have locking.
 */
int ydbi_get_item_release(yang_db_item_access_t *ydbia, bool keeplocking);

/**
 * @brief create a key from pap, ap and kvs
 * @param pap	array of byte code prefix key, MUST be terminated with 255
 * @param ap	array of byte code key, MUST be terminated with 255
 * @param kvs	array of value key
 * @param kss	array of value key size
 * @param key	result is copied. must have UC_MAX_KEYSIZE bytes size
 * @return	key size
 */
uint32_t yang_db_create_key(uint8_t *pap, uint8_t *ap, kvs_t *kvs, uint8_t *kss, void *key);

/**
 * @brief dump the key data in 'ap' and 'kvs'.  This is for debugging.
 */
void yang_db_keydump_log(int llevel, uc_dbald *dbald, uint8_t *ap, kvs_t *kvs, uint8_t *kss);

/**
 * @brief dump the key data in 'ap' and 'kvs', and value data.  This is for debugging.
 */
void yang_db_keyvaluedump_log(int llevel, uc_dbald *dbald, uint8_t *ap, kvs_t *kvs,
			      uint8_t *kss, void *value, uint32_t vsize);

/**
 * @brief copy all items under (ap, kvs) to under (ap, nkvs)
 * @note
 *  This can be conveniently used to copy one instance to the otehr.
 *  E.G. {ap0,ap1,255}+{kvs0,kvs1,NULL} copy to {ap0,ap1,255}+{nkvs0,nkvs1,NULL},
 *  All of {ap0,ap1,*,255}+{kvs0,kvs1,*,NULL} keys data are copied,
 *    {ap0,ap1,ap2,255}+{kvs0,kvs1,NULL} -> {ap0,ap1,ap2,255}+{nkvs0,nkvs1,NULL}
 *    {ap0,ap1,255}+{kvs0,kvs1,kvs2,NULL} -> {ap0,ap1,255}+{nkvs0,nkvs1,kvs2NULL}
 *    {ap0,ap1,ap2,255}+{kvs0,kvs1,kvs2,NULL} -> {ap0,ap1,ap2,255}+{nkvs0,nkvs1,kvs2,NULL}
 *    ...
 */
int yang_db_listcopy(uc_dbald *dbald, uint8_t *ap, kvs_t *kvs, uint8_t *kss,
		     kvs_t *nkvs, uint8_t *nkss);

/**
 * @brief move items under (ap, kvs) to a new key which is modified by 'keymod_cb'
 * @note keymod_cb is called for each item found under the list.
 *       returns '0': no move happens,
 *       returns '1': no overwriting, move only when the modified key has no data.
 *       returns '2': overwriting, move anyway.
 *       returns '3': no move and delete the source item. this works like 'listdelete'
 *       ap and kvs are internally allocated area, so keymod_cb can change the contents
 *       but can't change the size.
 */
typedef enum {
	KEYMOD_ERROR=-1,
	KEYMOD_NOMOVE=0,
	KEYMOD_MOVE_NOOVERWRITE,
	KEYMOD_MOVE_OVERWRITE,
	KEYMOD_NOMOVE_DELETE,
} keymod_rcode_t;

typedef keymod_rcode_t (*key_modifier_t)(uint8_t *ap, kvs_t *kvs, uint8_t *kss);

#define LISTMOVE_FORWARD true
#define LISTMOVE_BACKWARD false
int yang_db_listmove(uc_dbald *dbald, uint8_t *ap, kvs_t *kvs, uint8_t *kss,
		     bool forward_direction, key_modifier_t keymod_cb);

/**
 * @brief convert string value to binary value
 * @param	*size:size of *d memory, if the data is bigger it is reallocated.
 *		to allocate a new memory or force to reallocate with a new data size, set *size=0.
 *		newly allocated size is returned in this value.
 * @param	hints:a constant string to identify the correct way for conversion.
 *		Used specifically for 'identityref' and 'enumeration' type values.
 *		Can be set to NULL for most types.
 * @return	data size, it is different from the allocated *size
 */
int yang_value_conv(uint8_t vtype, char *vstr, void **d, uint32_t *size, char *hints);

/**
 * @brief convert value to string
 * @param	index:leaf-list index, depending of the type handler, this parameter might be
 *		ignored specially on non leaf-list configs.
 * @param	hints:a constant string to identify the correct way for conversion.
 *		Used specifically for 'identityref' and 'enumeration'  type values.
 *		Can be set to NULL for most types.
 * @note	this is a utility function to convert value to its string equivalent.
 *		the return string is a static value, and not thread-safe.
 */
char *yang_value_string(uint8_t vtype, void *value, uint32_t vsize, uint8_t index, char *hints);

/**
 * @brief get the namespace of the given vtype and value combination
 * @param	vtype: yang vtype (yang_vtype_enum_t) that you want to check.
 * @param	index:leaf-list index, depending of the type handler, this parameter might be
 *		ignored specially on non leaf-list configs.
 * @param	hints:a constant string to identify the correct table to use
 *		during search.
 * @return	the namespace prefix associate with the vtype and value.
 * @note	the return string is a static value, and not thread-safe.
 */
char *yang_value_namespace(uint8_t vtype, void *value, uint8_t index, char *hints);

/**
 * @brief get sizeof vtype
 * @param	vtype: yang vtype (yang_vtype_enum_t) that you want to get the size of.
 * @return	fixed size of passed vtype, if vtypes that has no fixed size -1 will be return.
 * @note	this is a utility function that return fixed data size,
 		useful for parsing leaf-list.
 */
int yang_sizeof_vtype(uint8_t vtype);

/**
 * @brief check if vtype is a string base data type
 * @param	vtype: yang vtype (yang_vtype_enum_t) that you want to check.
 * @note	return true if vtype is a string base data type, else false
 */
bool yang_isstring_vtype(uint8_t vtype);

/**
 * @brief check if vtype is an enum or identitref base data type
 * @param	vtype: yang vtype (yang_vtype_enum_t) that you want to check.
 * @note	return true if vtype is an enum base data type, else false
 */
bool yang_isenum_vtype(uint8_t vtype);

/**
 * @brief Action(YANG_DB_ACTION_*) set in dbpara->atype
 * @param dbald	DB access pointer
 * @param hwald	HW access pointer, can be NULL not to access HW
 * @param dbpara	parameters of the action
 */
int yang_db_action(uc_dbald *dbald, uc_hwald *hwald, yang_db_access_para_t *dbpara);

/**
 * @brief update leaf-list string data
 * @param dbald	DB access pointer
 * @param hwald	HW access pointer, can be NULL not to access HW
 * @param dbpara	parameters of the action
 *			dbpara->value and dbpara->vsize have the old data
 *			if dbpara->value is NULL or not found, simply append 'nvalue'
 * @param nvalue	new data
 *			if nvalue==NULL, delete dbpara->value
 * @param nvsize	new data size
 */
int yang_db_leaflist_strupdate(uc_dbald *dbald, uc_hwald *hwald, yang_db_access_para_t *dbpara,
			       void *nvalue, uint32_t nvsize);

/**
 * @brief update capability leaf-list string data
 *		'features' and 'deviations' are convined with old and new data
 */
int yang_db_leaflist_capupdate(uc_dbald *dbald, uc_hwald *hwald, yang_db_access_para_t *dbpara,
			       void *nvalue, uint32_t nvsize);

/**
 * @brief Extract a binary key data into 'ap' and 'kvs'
 * @param key	binary key data
 * @param kzie	size of the binary key data
 * @param ap	node key return pointer, allocated inside the function
 * @param kvs	value key return pointer array, each pointer is allocated inside the function.
		the array area must be allocated by the caller
 * @param kss	value key size array.
		the array area must be allocated by the caller.
 * @note yang_db_extract_key_free must be called to release the allocated data.
 */
int yang_db_extract_key(void *key, uint32_t ksize, uint8_t **ap, kvs_t *kvs, uint8_t *kss);

/**
 * @brief Release the allocated data in 'yang_db_extract_key'
 */
void yang_db_extract_key_free(uint8_t *ap, kvs_t *kvs, uint8_t *kss);

/**
 * @brief get the last value key
 * @return the pointer of {last_value_key_size, last_value_key_data}
 *	   NULL if no kvs part exists.
 * @note
 * 	key consists of 'aps' part + 255 + len0 + kvs[0] + len1 + kvs[1] + ,,, lenN + kvs[N].
 * 	len0, len1,,,lenN are uint8_t value.
 * 	this function returns the last 'lenN + kvs[N]' part.
 */
void *yang_db_key_bottomp(void *key, uint32_t ksize);

/**
 * @brief request to save the DB into a file
 */
int ydbi_request_savedb(yang_db_item_access_t *ydbia);

#endif
