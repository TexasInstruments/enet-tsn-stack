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
#ifndef YANG_DB_RUNTIME_H_
#define YANG_DB_RUNTIME_H_

#include "../uc_dbal.h"
#include "../hal/uc_hwal.h"
#include "../uc_notice.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct yang_db_runtime_data yang_db_runtime_dataq_t;

yang_db_runtime_dataq_t *yang_db_runtime_init(uc_dbald *dbald, uc_hwald *hwald);

void yang_db_runtime_close(yang_db_runtime_dataq_t *ydrd);


/**
 * @brief read from a config file, and set data.
 * @param ydrd yang_db_runtime_dataq_t
 * @param fname config file name
 * @param ucntd uc_notice handle to ask an action to uniconf.
 *              if NULL no action is pushed.
 * @return -1:error, 0:success
 */
int yang_db_runtime_readfile(yang_db_runtime_dataq_t *ydrd, const char* fname,
			     uc_notice_data_t *ucntd);

/**
 * @brief get value type of 'aps' leaf
 * @return >=0:vtype, -1:error
 */
int yang_db_runtime_get_vtype(uc_dbald *dbald, uint8_t *aps);

/**
 * @brief write one node db data by a string line
 * @param ydrd yang_db_runtime_dataq_t
 * @param kstr key string
 * @param vstr value string
 * @param onhw action on hardware
 * @return -1:error, 0:success
 */
int yang_db_runtime_put_oneline(yang_db_runtime_dataq_t *ydrd,
				char *kstr, char *vstr, uint8_t onhw);

/**
 * @brief ask to call 'uc_nc_askaction_push'
 * @param ydrd yang_db_runtime_dataq_t
 * @param ucntd uc_notice_data_t
 * @return -1:error, 0:success
 * @note ydrd->aps,kvs,kss must be set already to call this function.
 */
int yang_db_runtime_askaction(yang_db_runtime_dataq_t *ydrd,
			      uc_notice_data_t *ucntd);


/**
 * @brief get a range from line
 * @param ydrd yang_db_runtime_dataq_t
 * @param line key string
 * @param kvs return value keys, it must have MAX_KV_DEPTH+1 of void pointer space
 * @param kss return value key sizes, it must have MAX_KV_DEPTH of uint8_t space
 * @param status true:get range from status('ro') area, false: get from config('rw') area
 * @return ragne object:success, NULL:error
 * @note returned ranage object must be released by calling uc_get_range_release
 */
uc_range *yang_db_runtime_range_fromline(yang_db_runtime_dataq_t *ydrd,
					 const char* line, void **kvs, uint8_t *kss,
					 bool status);

/**
 * @brief get one node db data by a string line
 * @param ydrd yang_db_runtime_dataq_t
 * @param line key string
 * @param value value pointer to return read value. it is reallocated inside the function.
 *	  it must be NULL or allocated address.
 * @param vsize value size
 * @return -1:error, vtype>=0:success
 */
int yang_db_runtime_get_oneline(yang_db_runtime_dataq_t *ydrd,
				const char* line, void **value, uint32_t *vsize);

/*
 * 'uc_nc_notice_register' is called by creating key from 'line'
 * 'semname' must be a unique name.
 */
int yang_db_runtime_notice_register(yang_db_runtime_dataq_t *ydrd, uc_notice_data_t *ucntd,
				    const char* line, char *semname, UC_NOTICE_SIG_T **sem);

/**
 * @return number of value keys on aps node.
 */
uint8_t yang_db_runtime_getvknum(uc_dbald *dbald, uint8_t *aps);

/**
 * @return vtype of vkindex's key on aps node.
 * return -1: error
 * return 0: vkey, vtype(yang_vtype_enum_t) is set
 */
int yang_db_runtime_getvkvtype(uc_dbald *dbald, uint8_t *aps, uint8_t vkindex,
			       uint8_t *vkey, uint8_t *vtype);

/**
 * @return value key node string of vkindex's key on aps node.
 * return -1: error
 * return 0: the result str in '*rst', the caller must call UB_SD_RELMEM to release.
 */
int yang_db_runtime_getvkstr(uc_dbald *dbald,
			     uint8_t *aps, uint8_t vkindex, char **rstr);

/**
 * @brief wait witem,
 *	if waitv!=NULL wait unitl the value matches. if waitv==NULL wait appearance of witem.
 * @return return 0:got the waitv, 1:timed out, -1:error
 */
int yang_db_runtime_waititem(yang_db_runtime_dataq_t *ydrd, const char* witem,
			     void *waitv, uint32_t wvsize, int tout_ms);

/**
 * @brief 'key node string' and 'value key node string' of key
 * @return -1: error, 0: the result str in '*rst', the caller must call UB_SD_RELMEM to release.
 */
int yang_db_runtime_getkeyvkstr(uc_dbald *dbald,
				void *key, uint32_t ksize, char **rstr);

/**
 * @return N>=0:set node and need N value keys, -1:set one value key, -2:pass leaf, -3:error
 * @param ydrd yang_db_runtime_dataq_t
 * @param reset reset ydrd->api to start over
 * @param kstr key stting
 * @param vstr value string, if this is NULL, kstr should be node
 */
int yang_db_runtime_proc_nodestring(yang_db_runtime_dataq_t *ydrd, bool reset,
				    char *kstr, char *vstr);

/**
 * @brief call yang_db_runtime_getkeyvkstr with the internal ydrd state
 * @return 0: the result str in '*rst', the caller must call UB_SD_RELMEM to release.
 */
int yang_db_runtime_state_keyvkstr(yang_db_runtime_dataq_t *ydrd, char **rstr);

// functions in convxml2conf.c
/**
 * @brief convert xml format config file to Excelfore format config file
 * @param ydrd yang_db_runtime_dataq_t
 * @param ifname input xml format filename
 * @param ofname output Excelfore format filename. if NULL, use a default temp file
 * @return 'converted filename': success, NULL:fail
 */
const char *convxml2conf_getconf(yang_db_runtime_dataq_t *ydrd, const char *ifname,
				 const char *ofname);

#ifdef __cplusplus
}
#endif

#endif
