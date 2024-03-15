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
#include <tsn_uniconf/yangs/yang_modules.h>
#include "tsn_uniconf/yangs/yang_node.h"
#include <tsn_uniconf/yangs/yang_db_runtime.h>
#include <tsn_uniconf/yangs/ieee1588-ptp-tt.h>
#include <tsn_uniconf/yangs/ieee1588-ptp-tt_access.h>
#include <tsn_uniconf/yangs/ietf-interfaces.h>
#include <tsn_uniconf/yangs/excelfore-tsn-remote_access.h>
#include <tsn_uniconf/uc_notice.h>
#include "avtpgcfg.h"
#include "xl4-extmod-xl4avtp.h"
#include "xl4-extmod-xl4avtp_runconf.h"

extern uint8_t XL4_EXTMOD_XL4AVTP_func(uc_dbald *dbald);
#define XL4_EXTMOD_XL4AVTP XL4_EXTMOD_XL4AVTP_func(dbald)
#define XL4_EXTMOD_XL4AVTP_Y XL4_EXTMOD_XL4AVTP_func(ydbia->dbald)

struct avtpgcfg_data{
	uint8_t instanceIndex;
	uc_dbald *dbald;
	uc_notice_data_t *ucntd;
	uint8_t callmode;
};

#ifdef UC_RUNCONF
#define AVTPD_RUNCONF_INIT(dbald,hwald) xl4_extmod_xl4avtp_runconf_config_init(dbald,hwald)
#else
#define AVTPD_RUNCONF_INIT(dbald,hwald) 0
#endif

#define AVTPD_CONFIG_INST avtpd_config_inst
#ifndef AVTPD_MAX_INSTANCES
#define AVTPD_MAX_INSTANCES 2
#endif
UB_SD_GETMEM_DEF(AVTPD_CONFIG_INST, sizeof(avtpgcfgd_t), AVTPD_MAX_INSTANCES);

static int avtpd_nonyang_init(uc_dbald *dbald)
{
	int res=0;

	if(xl4_extmod_xl4avtp_config_init(dbald, NULL)!=0){
		UB_LOG(UBL_ERROR, "%s: error in xl4_extmod_xl4avtp_config_init\n", __func__);
		res=-1;
	}
	if(AVTPD_RUNCONF_INIT(dbald, NULL)!=0){
		UB_LOG(UBL_ERROR, "%s: error in AVTPD_RUNCONF_INIT\n", __func__);
		res=-1;
	}
	return res;
}

avtpgcfgd_t *avtpgcfg_init(uint8_t instanceIndex, const char *dbname,
			   const char **confnames, bool ucthread)
{
	int res=-1;
	yang_db_runtime_dataq_t *ydrd;
	avtpgcfgd_t *avtpgcd;
	uint8_t sinst;

	avtpgcd=(avtpgcfgd_t *)UB_SD_GETMEM(AVTPD_CONFIG_INST, sizeof(avtpgcfgd_t));
	if(ub_assert_fatal(avtpgcd!=NULL, __func__, NULL)){return NULL;}
	memset(avtpgcd, 0, sizeof(avtpgcfgd_t));
	avtpgcd->instanceIndex=instanceIndex;
	if(ucthread){avtpgcd->callmode=UC_CALLMODE_THREAD;}

	avtpgcd->dbald=uc_dbal_open(dbname, "w", avtpgcd->callmode);
	if(avtpgcd->dbald==NULL){goto erexit;}
	avtpgcd->ucntd=uc_notice_init(avtpgcd->callmode, dbname);
	if(avtpgcd->ucntd==NULL){goto erexit;}
	if(avtpd_nonyang_init(avtpgcd->dbald)!=0){goto erexit;}
	ydbi_access_init(avtpgcd->dbald, avtpgcd->ucntd);
	while(confnames && *confnames){
		if(strstr(*confnames, UC_INIT_COPY_INSTANCE_PRE)==*confnames){
			sinst=strtol(&(*confnames)[strlen(UC_INIT_COPY_INSTANCE_PRE)],
				     NULL, 0);
			if(sinst<instanceIndex){
				res=avtpgcfg_copy_instance(sinst, instanceIndex);
			}else{
				res=0;
			}
		}else{
			ydrd=UC_RUNCONF_INIT(avtpgcd->dbald, NULL);
			res=UC_RUNCONF_READFILE(ydrd, *confnames);
			UC_RUNCONF_CLOSE(ydrd);
		}
		if(res!=0){goto erexit;}
		confnames++;
	}
	res=0;
erexit:
	if(res!=0){
		avtpgcfg_close(avtpgcd);
		UB_LOG(UBL_ERROR, "%s:failed\n", __func__);
		return NULL;
	}else{
		UB_LOG(UBL_DEBUG, "%s:initialized\n", __func__);
	}
	return avtpgcd;
}

void avtpgcfg_close(avtpgcfgd_t *avtpgcd)
{
	if(!avtpgcd){return;}
	if(avtpgcd->ucntd!=NULL){
		uc_notice_close(avtpgcd->ucntd, avtpgcd->callmode);
	}
	if(avtpgcd->dbald!=NULL){
		uc_dbal_close(avtpgcd->dbald, avtpgcd->callmode);
	}
	ydbi_access_close();
	UB_SD_RELMEM(AVTPD_CONFIG_INST, avtpgcd);
	UB_LOG(UBL_DEBUG, "%s:closed\n", __func__);
	return;
}

int avtpgcfg_copy_instance(uint8_t sinst, uint8_t dinst)
{
	yang_db_item_access_t *ydbia=ydbi_access_handle();
	uint8_t aps[5]={XL4_EXTMOD_RW, 0, XL4_EXTMOD_XL4AVTP_AVTP_INSTANCE, 255};
	void *kvs[2]={&sinst, NULL};
	void *nkvs[2]={&dinst, NULL};
	uint8_t kss[1]={sizeof(uint8_t)};
	aps[1] = XL4_EXTMOD_XL4AVTP_Y;
	if(yang_db_listcopy(ydbia->dbald, aps, kvs, kss, nkvs, kss)){
		UB_LOG(UBL_ERROR, "%s:can't copy _RW\n", __func__);
		return -1;
	}
	aps[0]=XL4_EXTMOD_RO;
	if(yang_db_listcopy(ydbia->dbald, aps, kvs, kss, nkvs, kss)){
		UB_LOG(UBL_ERROR, "%s:can't copy _RO\n", __func__);
		return -1;
	}
	return 0;
}

void avtpgcfg_releasedb(avtpgcfgd_t *avtpgcd)
{
	uc_dbal_releasedb(avtpgcd->dbald);
}

static uint8_t ydbi_instIndex;
static void set_dpara_k2vk1(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t instIndex, uint8_t k1, uint8_t k2,
			    void *kv1, uint32_t kvs1, bool status)
{
	ydbi_instIndex=instIndex;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?XL4_EXTMOD_RO:XL4_EXTMOD_RW;
	dbpara->aps[1] = XL4_EXTMOD_XL4AVTP;
	dbpara->aps[2] = XL4_EXTMOD_XL4AVTP_AVTP_INSTANCE;
	dbpara->aps[3] = k1;
	dbpara->aps[4] = k2;
	dbpara->aps[5] = 255;
	dbpara->kvs[0] = &ydbi_instIndex;
	dbpara->kss[0] = sizeof(uint8_t);
	dbpara->kvs[1] = kv1;
	dbpara->kss[1] = kvs1;
	dbpara->kvs[2] = NULL;
}

int ydbi_get_item_nyavtpk2vk1(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			      uint8_t k1, uint8_t k2, void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k2vk1(ydbia->dbald, &ydbia->dbpara, instIndex, k1, k2, kv1, kvs1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_nyavtpk2vk1(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      uint8_t k1, uint8_t k2, void *kv1, uint32_t kvs1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k2vk1(ydbia->dbald, &ydbia->dbpara, instIndex, k1, k2, kv1, kvs1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_nyavtpk2vk1(yang_db_item_access_t *ydbia, uint8_t instIndex,
			      uint8_t k1, uint8_t k2, void *kv1, uint32_t kvs1, bool status,
			      void *value, uint32_t vsize, bool notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k2vk1(ydbia->dbald, &ydbia->dbpara, instIndex, k1, k2, kv1, kvs1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}
