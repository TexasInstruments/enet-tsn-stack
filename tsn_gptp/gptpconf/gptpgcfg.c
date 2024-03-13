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
#include <limits.h>
#include <stdint.h>
#include <tsn_unibase/unibase.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/yangs/ietf-interfaces.h>
#include <tsn_uniconf/yangs/ietf-interfaces_access.h>
#include <tsn_uniconf/yangs/ieee1588-ptp-tt.h>
#include <tsn_uniconf/yangs/ieee1588-ptp-tt_access.h>
#include <tsn_uniconf/yangs/yang_db_runtime.h>
#include <tsn_uniconf/yangs/yang_node.h>
#include <tsn_uniconf/uc_notice.h>
#include "gptpgcfg.h"
#include "xl4-extmod-xl4gptp_runconf.h"
#include "../mind.h"
#include "../gptpnet.h"
#include "../gptpbasetypes.h"
#include "../gptpcommon.h"

extern uint8_t XL4_EXTMOD_XL4GPTP_func(uc_dbald *dbald);
#define XL4_EXTMOD_XL4GPTP XL4_EXTMOD_XL4GPTP_func(dbald)
#define XL4_EXTMOD_XL4GPTP_Y XL4_EXTMOD_XL4GPTP_func(ydbia->dbald)
extern uint8_t IEEE1588_PTP_TT_func(uc_dbald *dbald);
#define IEEE1588_PTP_TT_RW_Y IEEE1588_PTP_TT_func(ydbia->dbald)
#define IEEE1588_PTP_TT_RW_G IEEE1588_PTP_TT_func(gycd->dbald)
#define IEEE1588_PTP_TT_RO_Y (IEEE1588_PTP_TT_func(ydbia->dbald)|0x80u)

/****************************************
 * layer independent functions
 ****************************************/

static int get_int_from_value(void *value, int vsize, int64_t *rval)
{
	switch(vsize){
	case 1:
		*rval=*((int8_t*)value);
		break;
	case 2:
		*rval=*((int16_t*)value);
		break;
	case 4:
		*rval=*((int32_t*)value);
		break;
	case 8:
		*rval=*((int64_t*)value);
		break;
	default:
		UB_LOG(UBL_ERROR, "%s:not integer? size=%d\n",
		       __func__, vsize);
		return -1;
	}
	return 0;
}

static uint8_t ydbi_instIndex;
static void set_dpara_k1vk0(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			    uint8_t instIndex, uint8_t k1, bool status)
{
	ydbi_instIndex=instIndex;
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?XL4_EXTMOD_RO:XL4_EXTMOD_RW;
	dbpara->aps[1] = XL4_EXTMOD_XL4GPTP;
	dbpara->aps[2] = XL4_EXTMOD_XL4GPTP_GPTP_INSTANCE;
	dbpara->aps[3] = k1;
	dbpara->aps[4] = 255;
	dbpara->kvs[0]=&ydbi_instIndex;
	dbpara->kss[0]=sizeof(uint8_t);
	dbpara->kvs[1]=NULL;
}

int ydbi_get_item_nyptk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			    uint8_t k1, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_DEBUG);
}

int ydbi_rel_item_nyptk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status)
{
	if(ydbi_rel_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	return ydbi_rel_foot(ydbia, __func__);
}

int ydbi_set_item_nyptk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status, void *value, uint32_t vsize,
			    bool notice)
{
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(ydbia->dbald, &ydbia->dbpara, instIndex, k1, status);
	ydbia->dbpara.value=value;
	ydbia->dbpara.vsize=vsize;
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, notice);
}

/*
  this is called only for NON-YANG data.
  For NON-YANG data, gptpinstance-index = INSTANCE_INDEX, which is NOT per domain.
*/
void* gptpgcfg_get_item(uint8_t gptpInstanceIndex, uint8_t confitem, bool status)
{
	void *rval;
	int res;
	res=ydbi_get_item_nyptk1vk0(ydbi_access_handle(), &rval,
				    gptpInstanceIndex, confitem, status);
	if(res>0) return rval;
	return NULL;
}

int gptpgcfg_get_item_release(uint8_t gptpInstanceIndex)
{
	return ydbi_get_item_release(ydbi_access_handle(), YDBI_REL_LOCK);
}

/*
  this is called only for NON-YANG data.
  For NON-YANG data, gptpinstance-index = INSTANCE_INDEX, which is NOT per domain.
*/
int gptpgcfg_get_intitem(uint8_t gptpInstanceIndex, uint8_t confitem, bool status)
{
	void *value;
	int64_t res=-1;
	int vsize;
	vsize=ydbi_get_item_nyptk1vk0(ydbi_access_handle(), &value,
				      gptpInstanceIndex, confitem, status);
	if(vsize>0){
		(void)get_int_from_value(value, vsize, &res);
		ydbi_get_item_release(ydbi_access_handle(), YDBI_REL_LOCK);
	}
	return res;
}

/*
  this is called only for NON-YANG data.
  For NON-YANG data, gptpinstance-index = INSTANCE_INDEX, which is NOT per domain.
*/
int gptpgcfg_set_item(uint8_t gptpInstanceIndex, uint8_t confitem,
		      bool status, void *value, uint32_t vsize)
{
	return ydbi_set_item_nyptk1vk0(ydbi_access_handle(), gptpInstanceIndex,
				       confitem, status, value, vsize, YDBI_NO_NOTICE);
}

int gptpgcfg_wait_gptpready(yang_db_item_access_t *ydbia, uint8_t gptpInstanceIndex, int tout_ms)
{
	int gdi;
	uint8_t port_state=0;
	void *value;
	uint16_t pindex=0;
	// read port_state of port=0, domain=0
	int res=0;
	while(true){
		gdi=ydbi_gptpinstdomain2dbinst_pt(ydbia, gptpInstanceIndex, 0);
		if(gdi>=0){
			YDBI_GET_ITEM_VSUBST(uint8_t*, ptk4vk1, port_state, value, gdi,
					     IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
					     IEEE1588_PTP_TT_PORT_DS, IEEE1588_PTP_TT_PORT_STATE,
					     &pindex, sizeof(uint16_t), YDBI_STATUS);
			if(port_state==MasterPort||port_state==PassivePort||
			   port_state==SlavePort){
				res=0;
				break;
			}
		}
		tout_ms-=10;
		if(tout_ms<0){
			UB_LOG(UBL_INFO, "%s:no gptp in %dmsec, gdi=%d, port_state=%d\n",
			       __func__, tout_ms, gdi, port_state);
			res=1;
			break;
		}
		uc_dbal_releasedb(ydbia->dbald);
		CB_USLEEP(10000);// 10msec to wait for the update
	}
	uc_dbal_releasedb(ydbia->dbald);
	return res;
}

int gptpcfg_copy_instance(uint8_t sginst, uint8_t sdomain, uint8_t dginst, uint8_t ddomain)
{
	int in;
	uint8_t aps[UC_MAX_AP_DEPTH] = {0};
	yang_db_item_access_t *ydbia=ydbi_access_handle();
	uint8_t sinst, dinst;
	uint32_t sinst32, dinst32;
	void *kvs[2]={&sinst, NULL};
	void *nkvs[2]={&dinst, NULL};
	uint8_t kss[1]={sizeof(uint8_t)};

	in=ydbi_gptpinstdomain2dbinst_pt(ydbia, sginst, sdomain);
	if(in<0){
		UB_LOG(UBL_ERROR, "%s:no map for source(%d,%d)\n",
		       __func__, sginst, sdomain);
		return -1;
	}
	sinst=in;
	sinst32=in;
	in=ydbi_gptpinstdomain2dbinst_pt(ydbia, dginst, ddomain);
	if(in<0){
		uint16_t md;
		yang_db_access_para_t dbpara={YANG_DB_ACTION_APPEND, YANG_DB_ONHW_NOACTION,
					      NULL, aps, NULL, NULL, &md, sizeof(uint16_t)};
		UB_LOG(UBL_INFO, "%s:no map for dest(%d,%d), add it\n",
		       __func__, dginst, ddomain);
		aps[0]=IEEE1588_PTP_TT_RW_Y;
		aps[1]=IEEE1588_PTP_TT_PTP;
		aps[2]=IEEE1588_PTP_TT_INSTANCE_DOMAIN_MAP;
		aps[3]=255;
		md=dginst<<8|ddomain;
		in=-1;
		if(yang_db_action(ydbia->dbald, NULL, &dbpara)==0){
			dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
			(void)yang_db_action(ydbia->dbald, NULL, &dbpara);
			in=ydbi_gptpinstdomain2dbinst_pt(ydbia, dginst, ddomain);
		}
		if(in<0){
			UB_LOG(UBL_ERROR, "%s:can't add a new map\n", __func__);
			return -1;
		}
	}
	dinst=in;
	dinst32=in;
	if(sinst==dinst){return 0;}
	aps[0]=XL4_EXTMOD_RW;
	aps[1]=XL4_EXTMOD_XL4GPTP_Y;
	aps[2]=XL4_EXTMOD_XL4GPTP_GPTP_INSTANCE;
	aps[3]=255;
	if(aps[1]==0xff){
		UB_LOG(UBL_ERROR, "%s:no no xl4gptp module\n", __func__);
		return -1;
	}
	if(yang_db_listcopy(ydbia->dbald, aps, kvs, kss, nkvs, kss)){
		UB_LOG(UBL_ERROR, "%s:can't copy xl4gptp data\n", __func__);
		return -1;
	}
	aps[0]=XL4_EXTMOD_RO;
	(void)yang_db_listcopy(ydbia->dbald, aps, kvs, kss, nkvs, kss);
	aps[0]=IEEE1588_PTP_TT_RW_Y;
	aps[1]=IEEE1588_PTP_TT_PTP;
	aps[2]=IEEE1588_PTP_TT_INSTANCES;
	aps[3]=IEEE1588_PTP_TT_INSTANCE;
	aps[4]=255;
	kvs[0]=&sinst32;
	nkvs[0]=&dinst32;
	kss[0]=sizeof(uint32_t);
	if(yang_db_listcopy(ydbia->dbald, aps, kvs, kss, nkvs, kss)){
		UB_LOG(UBL_ERROR, "%s:can't copy ieee1588 data\n", __func__);
		return -1;
	}
	aps[0]=IEEE1588_PTP_TT_RO_Y;
	(void)yang_db_listcopy(ydbia->dbald, aps, kvs, kss, nkvs, kss);
	return 0;
}

/****************************************
 * layer private functions
 ****************************************/

#ifdef UC_RUNCONF
#define GPTP_RUNCONF_INIT(dbald,hwald) xl4_extmod_xl4gptp_runconf_config_init(dbald,hwald)
#else
#define GPTP_RUNCONF_INIT(dbald,hwald) 0
#endif

#define GPTP_LINKSEMNAME "/gptplinksem"

struct gptpgcfg_data{
	uc_dbald *dbald;
	uint8_t max_domain;
	uint8_t *instance_domain_map;
	uc_notice_data_t *ucntd;
	uint8_t callmode;
	int8_t rep_port;
	UC_NOTICE_SIG_T *linksem;
	char semname[12+16+1]; // strlen(GPTP_LINKSEMNAME)+64bitTS+NULL
};

#define GPTP_YANCONF_MEM gptp_yanconf_mem
#define GPTP_YANCONF_AFSIZE 4u
#define GPTP_YANCONF_ALIGN(x) GPTP_ALIGN(x, GPTP_YANCONF_AFSIZE)
#define GPTP_YANGCONF_TOTAL_SIZE \
	(GPTP_YANCONF_ALIGN(sizeof(gptpgcfg_data_t))+			\
	 GPTP_YANCONF_ALIGN(sizeof(gptpgcfg_data_t*))+			\
	 GPTP_YANCONF_ALIGN(sizeof(uint8_t)*GPTP_MAX_DOMAINS))

#define GPTP_YANCONF_AFNUM \
	(GPTP_MAX_INSTANCES*(GPTP_YANGCONF_TOTAL_SIZE/GPTP_YANCONF_AFSIZE))

UB_SD_GETMEM_DEF(GPTP_YANCONF_MEM, GPTP_YANCONF_AFSIZE, GPTP_YANCONF_AFNUM);

static gptpgcfg_data_t **gycdl;
static uint8_t gycdl_num;

/*
  "gycd" is created per "gptpinstance".
  gptpinstance-index starts from 0 and increment by crating a new gptp instance.
  "/ieee1588-ptp-tt/prp/intstances/instance", this instance is different from 'gptpinstance'.
  Each gptpinstance has "the number of domains" instances.
  E.G. 1st gptpinstance has 2 domains, and 2nd has 1 domain.
  "instance-domain-map" data becomes "0-0,0-1,1-0"
  the map element is "gptpinstance-index"-"domain-index".
  "0-0"(0x0000) is the 1st "/ieee1588-ptp-tt/prp/intstances/instance|instance-index:0|"
  "0-1"(0x0001) is the 2nd "/ieee1588-ptp-tt/prp/intstances/instance|instance-index:1|"
  "1-0"(0x0100) is the 3rd "/ieee1588-ptp-tt/prp/intstances/instance|instance-index:2|"
    gycd of gptpinstance-index=0, gycd->instance_domain_map={0,1}
    gycd of gptpinstance-index=1, gycd->instance_domain_map={2}
 */
static int instance_domain_Init(gptpgcfg_data_t *gycd, uint8_t gptpInstanceIndex)
{
	uint8_t aps[]={IEEE1588_PTP_TT_RW_G, IEEE1588_PTP_TT_PTP, IEEE1588_PTP_TT_INSTANCE_DOMAIN_MAP,
		       255};
	yang_db_access_para_t dbpara={YANG_DB_ACTION_READ, YANG_DB_ONHW_NOACTION,
				      NULL, aps, NULL, NULL, NULL, 0};
	uint16_t *mapv;
	int i,n;
	int num=0;
	gycd->max_domain=gptpgcfg_get_intitem(gptpInstanceIndex,
					      XL4_EXTMOD_XL4GPTP_MAX_DOMAIN_NUMBER,
					      false);
	if(gycd->max_domain==255u){
		UB_LOG(UBL_ERROR, "%s:failed to read MAX_DOMAIN_NUMBER, instIndex=%d\n",
		       __func__, gptpInstanceIndex);
		return -1;
	}

	if(gycd->max_domain<1u){gycd->max_domain=1;}
	gycd->instance_domain_map=(uint8_t*)UB_SD_GETMEM(GPTP_YANCONF_MEM,
							 gycd->max_domain*sizeof(uint8_t));
	if(ub_assert_fatal(gycd->instance_domain_map!=NULL, __func__, NULL)){return -1;}

	if(yang_db_action(gycd->dbald, NULL, &dbpara)==0){
		mapv=(uint16_t *)dbpara.value;
		n=dbpara.vsize/sizeof(uint16_t);
		for(i=0;i<n;i++){
			if((gptpInstanceIndex==(mapv[i]>>8u)) &&
			   ((mapv[i]&0xffu)<gycd->max_domain)){
				gycd->instance_domain_map[mapv[i]&0xffu]=i;
				num++;
			}
		}
		dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
		(void)yang_db_action(gycd->dbald, NULL, &dbpara);
	}
	if(num<gycd->max_domain){
		uint16_t dmap;
		UB_LOG(UBL_INFO, "%s:INSTANCE_DOMAIN_MAP doesn't exist in the DB.\n"
		       "append (%d,%d)-(%d,%d) start from %d\n", __func__,
		       gptpInstanceIndex, gycd->instance_domain_map[0],
		       gptpInstanceIndex, gycd->instance_domain_map[gycd->max_domain-1], num);
		dbpara.atype=YANG_DB_ACTION_CREATE;
		dbpara.value=&dmap;
		dbpara.vsize=sizeof(uint16_t);
		for(i=num;i<(int)gycd->max_domain;i++){
			gycd->instance_domain_map[i]=i;
			dmap=(gptpInstanceIndex<<8)|i;
			if(yang_db_action(gycd->dbald, NULL, &dbpara)!=0){
				return -1;
			}
		}
	}
	return 0;
}

static int gptp_nonyang_init(uc_dbald *dbald)
{
	int res=0;

	if(xl4_extmod_xl4gptp_config_init(dbald, NULL)!=0){
		UB_LOG(UBL_ERROR, "%s: error in xl4_extmod_xl4gptp_config_init\n", __func__);
		res=-1;
	}
	if(GPTP_RUNCONF_INIT(dbald, NULL)!=0){
		UB_LOG(UBL_ERROR, "%s: error in GPTP_RUNCONF_INIT\n", __func__);
		res=-1;
	}
	return res;
}


int gptpgcfg_init(const char *dbname, const char **confnames,
		uint8_t gptpInstanceIndex, bool ucthread,
		int (*nonconfile_cb)(uint8_t gptpInstanceIndex))
{
	gptpgcfg_data_t *gycd;
	yang_db_runtime_dataq_t *ydrd;
	int res;
	uint8_t sinst;
	if((gptpInstanceIndex<gycdl_num) && gycdl[gptpInstanceIndex]){
		UB_LOG(UBL_ERROR, "%s:already called for gptpInstanceIndex=%d\n",
		       __func__, gptpInstanceIndex);
		return -1;
	}

	if(gycdl_num<=gptpInstanceIndex){
		gycdl=(gptpgcfg_data_t**)UB_SD_REGETMEM(GPTP_YANCONF_MEM, gycdl,
							sizeof(gptpgcfg_data_t*)*(gptpInstanceIndex+1u));
		if(ub_assert_fatal(gycdl, __func__, "realloc")){return -1;}
		uint8_t i;
		for(i=gycdl_num;i<=gptpInstanceIndex;i++){
			gycdl[i]=NULL;
		}
		gycdl_num=gptpInstanceIndex+1u;
	}
	gycd=(gptpgcfg_data_t*)UB_SD_GETMEM(GPTP_YANCONF_MEM, sizeof(gptpgcfg_data_t));
	if(ub_assert_fatal(gycd!=NULL, __func__, NULL)){return -1;}
	(void)memset(gycd, 0, sizeof(gptpgcfg_data_t));
	// 'tsn_uniconf' must run before this, to connect to the DB
	if(ucthread){gycd->callmode=UC_CALLMODE_THREAD;}
	gycdl[gptpInstanceIndex]=gycd;
	gycd->dbald=uc_dbal_open(dbname, "w", gycd->callmode);
	if(!gycd->dbald){goto erexit;}
	if(gptp_nonyang_init(gycd->dbald)!=0){goto erexit;}
	gycd->ucntd=uc_notice_init(gycd->callmode, dbname);
	if(!gycd->ucntd){goto erexit;}
	ydbi_access_init(gycd->dbald, gycd->ucntd);
	while(confnames && *confnames){
		if(strstr(*confnames, UC_INIT_COPY_INSTANCE_PRE)==*confnames){
			sinst=strtol(&(*confnames)[strlen(UC_INIT_COPY_INSTANCE_PRE)],
				     NULL, 0);
			if(sinst<gptpInstanceIndex){
				res=gptpcfg_copy_instance(sinst, 0, gptpInstanceIndex, 0);
			}else{
				res=0;
			}
		}else{
			ydrd=UC_RUNCONF_INIT(gycd->dbald, NULL);
			if(!ydrd){goto erexit;}
			res=UC_RUNCONF_READFILE(ydrd, *confnames);
			UC_RUNCONF_CLOSE(ydrd);
		}
		if(res!=0){goto erexit;}
		confnames++;
	}
	if(nonconfile_cb){
		nonconfile_cb(gptpInstanceIndex);
	}
	if(instance_domain_Init(gycd, gptpInstanceIndex)){
		goto erexit;
	}
	gycd->rep_port=gptpgcfg_get_intitem(gptpInstanceIndex,
					    XL4_EXTMOD_XL4GPTP_REPRESENT_PORT_NUMBER,
					    YDBI_CONFIG);
	UB_LOG(UBL_DEBUG, "%s:a new instance is created for gptpInstanceIndex=%d\n",
	       __func__, gptpInstanceIndex);
	return 0;
erexit:
	gptpgcfg_close(gptpInstanceIndex);
	return -1;
}

void gptpgcfg_close(uint8_t gptpInstanceIndex)
{
	gptpgcfg_data_t *gycd;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return;}
	gycd=gycdl[gptpInstanceIndex];
	if(gycd->ucntd!=NULL){uc_notice_close(gycd->ucntd, gycd->callmode);}
	if(gycd->dbald!=NULL){uc_dbal_close(gycd->dbald, gycd->callmode);}
	ydbi_access_close();
	if(gycd->instance_domain_map!=NULL){
		UB_SD_RELMEM(GPTP_YANCONF_MEM, gycd->instance_domain_map);
	}
	UB_SD_RELMEM(GPTP_YANCONF_MEM, gycd);
	gycdl[gptpInstanceIndex]=NULL;
	if((gptpInstanceIndex+1u)==gycdl_num){
		gycdl_num--;
		gycdl=(gptpgcfg_data_t**)UB_SD_REGETMEM(GPTP_YANCONF_MEM, gycdl,
							sizeof(gptpgcfg_data_t*)*gycdl_num);
	}
	UB_LOG(UBL_DEBUG, "%s:the instance was closed for gptpInstanceIndex=%d\n",
	       __func__, gptpInstanceIndex);
	return;
}

static int port_intitem_one(uint32_t dbinstanceIndex,
			    uint8_t confitem0, uint8_t confitem1, int8_t rep_port,
			    uint16_t pindex, bool status, int64_t *rval)
{
	void *value;
	int vsize;
	int res=-1;
	uint16_t pirep=(uint16_t)rep_port;
	vsize=YDBI_GET_ITEM(ptk4vk1, value, dbinstanceIndex,
			    IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT, confitem0, confitem1,
			    &pindex, sizeof(uint16_t), status);
	if(vsize>0){
		res=get_int_from_value(value, vsize, rval);
		ydbi_get_item_release(ydbi_access_handle(), YDBI_REL_LOCK);
	}
	if(res==0) {return 0;}
	if((rep_port>=0) && (pindex!=(uint16_t)rep_port)){
		// replace pindex with REPRESENT_PORT_NUMBER
		vsize=YDBI_GET_ITEM(ptk4vk1, value, dbinstanceIndex,
				    IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT, confitem0, confitem1,
				    &pirep, sizeof(uint16_t), status);
		if(vsize>0){
			res=get_int_from_value(value, vsize, rval);
			ydbi_get_item_release(ydbi_access_handle(), YDBI_REL_LOCK);
		}
	}
	return res;
}

static int get_yang_port_intitem(uint8_t gptpInstanceIndex,
				 uint8_t confitem0, uint8_t confitem1,
				 uint16_t pindex, uint8_t domainIndex, bool status,
				 int64_t *rval)
{
	int res;
	uint32_t dbinstanceIndex;
	gptpgcfg_data_t *gycd;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	if(domainIndex>=gycd->max_domain){return -1;}
	dbinstanceIndex=gycd->instance_domain_map[domainIndex];
	res=port_intitem_one(dbinstanceIndex, confitem0, confitem1, gycd->rep_port,
			     pindex, status, rval);
	if(!res || (domainIndex==0u)){goto erexit;}
	// if domainIndex>0, try again with domainIndex=0
	dbinstanceIndex=gycd->instance_domain_map[0];
	res=port_intitem_one(dbinstanceIndex, confitem0, confitem1, gycd->rep_port,
			     pindex, status, rval);
erexit:
	if(res!=0){
		UB_LOG(UBL_ERROR, "%s:no data, status=%d, "
		       "/ieee1588-ptp-tt/ptp/instances/instance/ports/port/\n",
		       __func__, status);
		UB_LOG(UBL_ERROR, "    confitem0=%d, confitem1=%d, rep_port=%d, "
		       "pindex=%d, domainIndex=%d\n",
		       confitem0, confitem1, gycd->rep_port, pindex, domainIndex);
	}
	return res;
}

int gptpgcfg_get_yang_portds_intitem(uint8_t gptpInstanceIndex, uint8_t confitem,
				     uint16_t pindex, uint8_t domainIndex, bool status)
{
	int64_t rval;
	if(get_yang_port_intitem(gptpInstanceIndex,
				 IEEE1588_PTP_TT_PORT_DS,
				 confitem, pindex, domainIndex, status, &rval)!=0){
		return INT_MAX;
	}
	return (int)rval;
}

int64_t gptpgcfg_get_yang_portds_int64item(uint8_t gptpInstanceIndex, uint8_t confitem,
					   uint16_t pindex, uint8_t domainIndex, bool status)
{
	int64_t rval;
	if(get_yang_port_intitem(gptpInstanceIndex,
				 IEEE1588_PTP_TT_PORT_DS,
				 confitem, pindex, domainIndex, status, &rval)!=0){
		return LLONG_MAX;
	}
	return rval;
}

int gptpgcfg_set_yang_port_item(uint8_t gptpInstanceIndex, uint8_t confitem1, uint8_t confitem2,
				uint16_t pindex, uint8_t domainIndex, bool status,
				void *value, int vsize, bool notice)
{
	uint32_t dbinstanceIndex;
	gptpgcfg_data_t *gycd;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	if(domainIndex>=gycd->max_domain){return -1;}
	dbinstanceIndex=gycd->instance_domain_map[domainIndex];
	return YDBI_SET_ITEM(ptk4vk1, dbinstanceIndex,
			     IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			     confitem1, confitem2,
			     &pindex, sizeof(uint16_t), status,
			     value, vsize, notice);
}

int gptpgcfg_set_clock_state_item(uint8_t gptpInstanceIndex, uint8_t confitem,
				  uint8_t domainIndex, bool status,
				  void *value, int vsize, bool notice)
{
	uint32_t dbinstanceIndex;
	gptpgcfg_data_t *gycd;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	if(domainIndex>=gycd->max_domain){return -1;}
	dbinstanceIndex=gycd->instance_domain_map[domainIndex];
	return YDBI_SET_ITEM(ptk3vk0, dbinstanceIndex,
			     IEEE1588_PTP_TT_CLOCK_STATE, confitem, 255,
			     status, value, vsize, notice);
}

int gptpgcfg_get_yang_intitem(uint8_t gptpInstanceIndex,
			      uint8_t confitem0, uint8_t confitem1, uint8_t confitem2,
			      uint8_t domainIndex, bool status)
{
	int64_t rval=INT_MAX;
	yang_db_item_access_t *ydbia;
	void *value;
	int vsize;
	uint32_t dbinstanceIndex;
	gptpgcfg_data_t *gycd;

	ydbia=ydbi_access_handle();
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return INT_MAX;}
	gycd=gycdl[gptpInstanceIndex];
	if(domainIndex>=gycd->max_domain){return INT_MAX;}
	dbinstanceIndex=gycd->instance_domain_map[domainIndex];
	vsize=YDBI_GET_ITEM(ptk3vk0, value, dbinstanceIndex,
			    confitem0, confitem1, confitem2, status);
	if(vsize<=0){
		// if domainIndex>0, try again with domainIndex=0
		dbinstanceIndex=gycd->instance_domain_map[0];
		vsize=YDBI_GET_ITEM(ptk3vk0, value, dbinstanceIndex,
				    confitem0, confitem1, confitem2, status);
		if(vsize<=0){goto erexit;}
	}
	(void)get_int_from_value(value, vsize, &rval);
	ydbi_get_item_release(ydbia, YDBI_REL_LOCK);
erexit:
	return rval;
}

void gptpgcfg_releasedb(uint8_t gptpInstanceIndex)
{
	gptpgcfg_data_t *gycd;
	if(gptpInstanceIndex>=gycdl_num || !gycdl[gptpInstanceIndex]) return;
	gycd=gycdl[gptpInstanceIndex];
	uc_dbal_releasedb(gycd->dbald);
}

int gptpgcfg_set_netdevs(uint8_t gptpInstanceIndex, const char *netdevs[], int numdevs)
{
	gptpgcfg_data_t *gycd;
	UC_NOTICE_SIG_T *sem=NULL;
	int i, res;

	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	(void)sprintf(gycd->semname, "%s%16"PRIx64, GPTP_LINKSEMNAME, ub_rt_gettime64());
	for(i=0;i<numdevs;i++){
		if(sem==NULL){
			res=ydbi_set_ifupdown_ucnotice(ydbi_access_handle(), netdevs[i],
						       &sem, gycd->semname);
			if(!res && !gycd->linksem){gycd->linksem=sem;}
		}else{
			res=ydbi_set_ifupdown_ucnotice(ydbi_access_handle(), netdevs[i],
						       NULL, gycd->semname);
		}
		if(res!=0){
			UB_LOG(UBL_WARN, "%s:error in uc_nc_notice_register, %s\n",
			       __func__, netdevs[i]);
		}else{
			UB_LOG(UBL_DEBUG, "%s:uc_nc_notice_register, %s\n",
			       __func__, netdevs[i]);
		}
	}
	if(!gycd->linksem){
		UB_LOG(UBL_ERROR, "%s:no semaphore is registered\n", __func__);
		return -1;
	}
	return 0;
}

void gptpgcfg_remove_netdevs(uint8_t gptpInstanceIndex)
{
	gptpgcfg_data_t *gycd;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return;}
	gycd=gycdl[gptpInstanceIndex];
	(void)uc_nc_notice_deregister_all(gycd->ucntd, gycd->dbald, gycd->semname);
	gycd->linksem=NULL;
	return;
}

int gptpgcfg_link_check(uint8_t gptpInstanceIndex, gptpnet_data_netlink_t *edtnl)
{
	char key[UC_MAX_KEYSIZE];
	uint32_t ksize;
	gptpgcfg_data_t *gycd;
	void *value=NULL;
	uint32_t vsize;
	const char *emes="";
	int res;
	bool thread_mode;

	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	if(!gycd->linksem){return -1;}
	thread_mode=(gycd->callmode==UC_CALLMODE_THREAD);
	if(uc_notice_sig_trywait(thread_mode, gycd->linksem)!=0){return 1;}
	UB_TLOG(UBL_DEBUG, "%s:signaled by linksem\n", __func__);
	res=uc_nc_get_notice_act(gycd->ucntd, gycd->dbald, gycd->semname, key, &ksize);
	if(res!=0){
		UB_LOG(UBL_ERROR, "%s:failed in uc_nc_get_notice_act, res=%d\n",
		       __func__, res);
		return 0;
	}
	if((ksize<5u) || (key[3]!=IETF_INTERFACES_OPER_STATUS)){
		emes="not expected notice";
		goto erexit;
	}

	res=uc_dbal_get(gycd->dbald, key, ksize, &value, &vsize);
	if(res!=0){
		emes="can't read oper-status";
		goto erexit;
	}
	(void)memset(edtnl, 0, sizeof(event_data_netlink_t));
	// key+4:255, key+5:size, key+6:ifname
	// force to truncate ifname to XL4_DATA_ABS_MAX_NETDEVS
	key[6+sizeof(edtnl->devname)-1]=0;
	memcpy(edtnl->devname, &key[6], strlen(&key[6])+1);
	if(*((uint32_t*)value)==1){edtnl->up=true;}
	(void)uc_dbal_get_release(gycd->dbald, key, ksize, value, vsize);

	key[3]=IETF_INTERFACES_SPEED;
	res=uc_dbal_get(gycd->dbald, key, ksize, &value, &vsize);
	if(res!=0){
		emes="can't read speed";
		goto erexit;
	}
	edtnl->speed=(uint32_t)(*((uint64_t*)value)/1000000);
	(void)uc_dbal_get_release(gycd->dbald, key, ksize, value, vsize);

	key[3]=IETF_INTERFACES_DUPLEX;
	res=uc_dbal_get(gycd->dbald, key, ksize, &value, &vsize);
	if(res!=0){
		emes="can't read duplex";
		goto erexit;
	}
	edtnl->duplex=*((uint32_t*)value);
	(void)uc_dbal_get_release(gycd->dbald, key, ksize, value, vsize);
erexit:
	if(res!=0){
		UB_LOG(UBL_WARN, "%s:%s\n", __func__, emes);
		return 1;
	}
	return 0;
}

int gptpgcfg_get_ptpdev(uint8_t gptpInstanceIndex, uint8_t domainIndex,
			   char *netdev, char *value, uint32_t vsize)
{
	int res;
	uint16_t portIndex;
	gptpgcfg_data_t *gycd;
	if((!value) || (vsize<XL4_DATA_PATH_STR_SHORTLEN)) return -1;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	if(domainIndex>=gycd->max_domain){return -1;}

	res=ydbi_port_device2index_pt(ydbi_access_handle(), gptpInstanceIndex, domainIndex,
	                              netdev, &portIndex);
	if(res){
		UB_TLOG(UBL_WARN, "%s:domainIndex=%d, netdev=%s portIndex not found\n",
		        __func__, domainIndex, netdev);
		// ptpdev not found (return -1) when netdev is not found
		return -1;
	}
	res=ydbi_port_index2ptpdev_pt(ydbi_access_handle(), gptpInstanceIndex, domainIndex,
	                              portIndex, value);
	return res;
}

int gptpgcfg_set_asCapable(uint8_t gptpInstanceIndex, uint8_t domainIndex,
			   uint16_t portIndex, bool asCapable)
{
	gptpgcfg_data_t *gycd;
	uint8_t asc=asCapable?1:0;
	uint32_t dbinstanceIndex;
	int res;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	dbinstanceIndex=gycd->instance_domain_map[domainIndex];
	res=YDBI_SET_ITEM(ptk4vk1, dbinstanceIndex,
			  IEEE1588_PTP_TT_PORTS, IEEE1588_PTP_TT_PORT,
			  IEEE1588_PTP_TT_PORT_DS, IEEE1588_PTP_TT_AS_CAPABLE,
			  &portIndex, sizeof(uint16_t),
			  YDBI_STATUS, &asc, sizeof(uint8_t), YDBI_PUSH_NOTICE);
	UB_TLOG(UBL_INFO, "%s:domainInde=%d, portIndex=%d, ascapable=%d\n", __func__,
		domainIndex, portIndex, asc);
	return res;
}

int gptpgcfg_set_clock_perfmonDS(PerfMonClockDS *ds, uint8_t id, bool periodComplete,
                                 uint8_t gptpInstanceIndex, uint8_t domainIndex)
{
	ClockPerfMonDataRecord *dr;
	int res=0;
	gptpgcfg_data_t *gycd;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	if(domainIndex>=gycd->max_domain){return -1;}

	if(!ds){ return -1; }
	dr = &ds->clockDR[id];

	// ClockPerformanceMonitoringDataRecord.measurementValid
	dr->measurementValid=true;
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MEASUREMENT_VALID, &dr->measurementValid, sizeof(uint8_t));
	// ClockPerformanceMonitoringDataRecord.periodComplete
	dr->periodComplete=periodComplete;
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_PERIOD_COMPLETE, &dr->periodComplete, sizeof(uint8_t));
	// ClockPerformanceMonitoringDataRecord.PMTime
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_PM_TIME, &ds->PMTime[id], sizeof(uint32_t));
	// ClockPerformanceMonitoringDataRecord.parameter 1..16
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_AVERAGE_TIME_TRANSMITTER_TIME_RECEIVER_DELAY, &dr->averageMasterSlaveDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MINIMUM_TIME_TRANSMITTER_TIME_RECEIVER_DELAY, &dr->minMasterSlaveDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MAXIMUM_TIME_TRANSMITTER_TIME_RECEIVER_DELAY, &dr->maxMasterSlaveDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_STDDEV_TIME_TRANSMITTER_TIME_RECEIVER_DELAY, &dr->stdDevMasterSlaveDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_AVERAGE_TIME_RECEIVER_TIME_TRANSMITTER_DELAY, &dr->averageSlaveMasterDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MINIMUM_TIME_RECEIVER_TIME_TRANSMITTER_DELAY, &dr->minSlaveMasterDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MAXIMUM_TIME_RECEIVER_TIME_TRANSMITTER_DELAY, &dr->maxSlaveMasterDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_STDDEV_TIME_RECEIVER_TIME_TRANSMITTER_DELAY, &dr->stdDevSlaveMasterDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_AVERAGE_MEAN_PATH_DELAY, &dr->averageMeanPathDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MINIMUM_MEAN_PATH_DELAY, &dr->minMeanPathDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MAXIMUM_MEAN_PATH_DELAY, &dr->maxMeanPathDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_STDDEV_MEAN_PATH_DELAY, &dr->stdDevMeanPathDelay, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_AVERAGE_OFFSET_FROM_TIME_TRANSMITTER, &dr->averageOffsetFromMaster, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MINIMUM_OFFSET_FROM_TIME_TRANSMITTER, &dr->minOffsetFromMaster, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_MAXIMUM_OFFSET_FROM_TIME_TRANSMITTER, &dr->maxOffsetFromMaster, sizeof(uint64_t));
	res+=ydbi_set_perfmon_clock_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, ds->index[id],
			IEEE1588_PTP_TT_STDDEV_OFFSET_FROM_TIME_TRANSMITTER, &dr->stdDevOffsetFromMaster, sizeof(uint64_t));

	if(res){
		UB_LOG(UBL_WARN, "%s:can't update the DB, %d items not updated\n", __func__, res);
	}
	return res;
}

int gptpgcfg_deleteall_clock_perfmonDS(uint8_t gptpInstanceIndex, uint8_t domainIndex)
{
	int res=-1;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	res=ydbi_clear_perfmon_clock_ds(ydbi_access_handle(), gptpInstanceIndex, domainIndex);
	return res;
}

int gptpgcfg_cascade_clock_perfmonDS(uint8_t id, uint8_t gptpInstanceIndex, uint8_t domainIndex)
{
	int res=-1;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	if(id==PERFMON_SHORTINTV_DR){
		res=ydbi_cascade_perfmon_clock_ds(ydbi_access_handle(), gptpInstanceIndex, domainIndex);
	}else{
		res=0; // other record types does not need cascading
	}
	return res;
}

int gptpgcfg_set_port_perfmonDS(PerfMonPortDS *ds, uint8_t id, bool periodComplete,
                                 uint8_t gptpInstanceIndex, uint8_t domainIndex, uint8_t portIndex)
{
	PortPerfMonPeerDelayDataRecord *pddr;
	PortPerfMonDataRecord *dr;
	int res=0;
	gptpgcfg_data_t *gycd;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	gycd=gycdl[gptpInstanceIndex];
	if(domainIndex>=gycd->max_domain){return -1;}

	if(!ds){ return -1; }
	pddr = &ds->pdelayDR[id];
	dr = &ds->portDR[id];

	// PortPerformanceMonitoringPeerDelayDataRecord.PMTime
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY, ds->index[id],
			IEEE1588_PTP_TT_PM_TIME, &ds->PMTime[id], sizeof(uint32_t));
	// PortPerformanceMonitoringPeerDelayDataRecord.parameters 1..4
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY, ds->index[id],
			IEEE1588_PTP_TT_AVERAGE_MEAN_LINK_DELAY, &pddr->averageMeanLinkDelay, sizeof(int64_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY, ds->index[id],
			IEEE1588_PTP_TT_MIN_MEAN_LINK_DELAY, &pddr->minMeanLinkDelay, sizeof(int64_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY, ds->index[id],
			IEEE1588_PTP_TT_MAX_MEAN_LINK_DELAY, &pddr->maxMeanLinkDelay, sizeof(int64_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY, ds->index[id],
			IEEE1588_PTP_TT_STDDEV_MEAN_LINK_DELAY, &pddr->stdDevMeanLinkDelay, sizeof(int64_t));

	// PortPerformanceMonitoringDataRecord.PMTime
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_PM_TIME, &ds->PMTime[id], sizeof(uint32_t));
	// PortPerformanceMonitoringDataRecord.parameter 1..17
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_ANNOUNCE_TX, &dr->announceTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_ANNOUNCE_RX, &dr->announceRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_ANNOUNCE_FOREIGN_RX, &dr->announceForeignMasterRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_SYNC_TX, &dr->syncTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_SYNC_RX, &dr->syncRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_FOLLOW_UP_TX, &dr->followUpTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_FOLLOW_UP_RX, &dr->followUpRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_DELAY_REQ_TX, &dr->delayReqTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_DELAY_REQ_RX, &dr->delayReqRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_DELAY_RESP_TX, &dr->delayRespTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_DELAY_RESP_RX, &dr->delayRespRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_PDELAY_REQ_TX, &dr->pDelayReqTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_PDELAY_REQ_RX, &dr->pDelayReqRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_PDELAY_RESP_TX, &dr->pDelayRespTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_PDELAY_RESP_RX, &dr->pDelayRespRx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_PDELAY_RESP_FOLLOW_UP_TX, &dr->pDelayRespFollowUpTx, sizeof(uint32_t));
	res+=ydbi_set_perfmon_port_item(ydbi_access_handle(), gptpInstanceIndex, domainIndex, portIndex,
			IEEE1588_PTP_TT_RECORD_LIST, ds->index[id],
			IEEE1588_PTP_TT_PDELAY_RESP_FOLLOW_UP_RX, &dr->pDelayRespFollowUpRx, sizeof(uint32_t));

	if(res){
		UB_LOG(UBL_WARN, "%s:can't update the DB, %d items not updated\n", __func__, res);
	}
	return res;
}

int gptpgcfg_deleteall_port_perfmonDS(uint8_t gptpInstanceIndex, uint8_t domainIndex, uint16_t portIndex)
{
	int res=-1;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	// clear record-list-peer-delay and record-list
	res=ydbi_clear_perfmon_port_ds(ydbi_access_handle(), IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY,
			gptpInstanceIndex, domainIndex, portIndex);
	res+=ydbi_clear_perfmon_port_ds(ydbi_access_handle(), IEEE1588_PTP_TT_RECORD_LIST,
			gptpInstanceIndex, domainIndex, portIndex);
	return res;
}

int gptpgcfg_cascade_port_perfmonDS(uint8_t id, uint8_t gptpInstanceIndex, uint8_t domainIndex, uint16_t portIndex)
{
	int res=-1;
	if((gptpInstanceIndex>=gycdl_num) || !gycdl[gptpInstanceIndex]){return -1;}
	if(id==PERFMON_SHORTINTV_DR){
		res=ydbi_cascade_perfmon_port_ds(ydbi_access_handle(), IEEE1588_PTP_TT_RECORD_LIST,
				gptpInstanceIndex, domainIndex, portIndex);
		res+=ydbi_cascade_perfmon_port_ds(ydbi_access_handle(), IEEE1588_PTP_TT_RECORD_LIST_PEER_DELAY,
				gptpInstanceIndex, domainIndex, portIndex);
	}else{
		res=0; // other record types does not need cascading
	}
	return res;
}
