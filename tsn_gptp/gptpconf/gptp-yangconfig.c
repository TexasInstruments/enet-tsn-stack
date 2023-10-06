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
/* Automatically generated file.  Don't edit this file.*/
#include <stdlib.h>
#include <tsn_unibase/unibase.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/yangs/ieee1588-ptp.h>

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

int gptp_yangconfig_config_add(uc_dbald *dbald, uc_hwald *hwald)
{
	uint8_t aps[9];
	int i;
	int res=-1;
	uint32_t vsize=0;
	void *kvs[3]={NULL};
	uint8_t kss[3];
	yang_db_access_para_t dbpara={YANG_DB_ACTION_CREATE,YANG_DB_ONHW_ALWAYS,
	                              NULL,aps,kvs,kss,NULL,0};
	//0000_ieee1588-ptp/ptp/instances/instance/default-ds/clock-quality
	kss[0]=0; // instance-index
	res=yang_value_conv(YANG_VTYPE_UINT32, "0",
		&kvs[0], &vsize, NULL);
	if(res<0){goto erexit;}
	kss[0]=res;
	if(kvs[1]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[1]);kvs[1]=NULL;}
	aps[0] = IEEE1588_PTP_RW;
	aps[1] = IEEE1588_PTP_PTP;
	aps[2] = IEEE1588_PTP_INSTANCES;
	aps[3] = IEEE1588_PTP_INSTANCE;
	aps[4] = IEEE1588_PTP_DEFAULT_DS;
	aps[5] = IEEE1588_PTP_CLOCK_QUALITY;
	aps[6] = IEEE1588_PTP_CLOCK_CLASS;
	aps[7] = 255;
	res=yang_value_conv(YANG_VTYPE_IDENTITYREF, "cc-default",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_CLOCK_ACCURACY;
	res=yang_value_conv(YANG_VTYPE_IDENTITYREF, "ca-time-accurate-to-250-ns",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[6] = IEEE1588_PTP_OFFSET_SCALED_LOG_VARIANCE;
	res=yang_value_conv(YANG_VTYPE_UINT16, "0x436a",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	//0001_ieee1588-ptp/ptp/instances/instance/default-ds
	aps[5] = IEEE1588_PTP_PRIORITY1;
	aps[6] = 255;
	res=yang_value_conv(YANG_VTYPE_UINT8, "248",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_PRIORITY2;
	res=yang_value_conv(YANG_VTYPE_UINT8, "248",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_EXTERNAL_PORT_CONFIG_ENABLE;
	res=yang_value_conv(YANG_VTYPE_BOOLEAN, "false",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[5] = IEEE1588_PTP_TIME_SOURCE;
	res=yang_value_conv(YANG_VTYPE_IDENTITYREF, "internal-oscillator",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[5] = IEEE1588_PTP_PTP_TIMESCALE;
	res=yang_value_conv(YANG_VTYPE_BOOLEAN, "true",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	//0002_ieee1588-ptp/ptp/instances/instance/ports/port/port-ds
	kss[1]=0; // port-index
	res=yang_value_conv(YANG_VTYPE_UINT16, "1",
		&kvs[1], &vsize, NULL);
	if(res<0){goto erexit;}
	kss[1]=res;
	if(kvs[2]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[2]);kvs[2]=NULL;}
	aps[0] = IEEE1588_PTP_RW;
	aps[4] = IEEE1588_PTP_PORTS;
	aps[5] = IEEE1588_PTP_PORT;
	aps[6] = IEEE1588_PTP_PORT_DS;
	aps[7] = IEEE1588_PTP_PORT_ENABLE;
	aps[8] = 255;
	res=yang_value_conv(YANG_VTYPE_BOOLEAN, "true",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_LOG_ANNOUNCE_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_GPTP_CAP_RECEIPT_TIMEOUT;
	res=yang_value_conv(YANG_VTYPE_UINT8, "3",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_ANNOUNCE_RECEIPT_TIMEOUT;
	res=yang_value_conv(YANG_VTYPE_UINT8, "3",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_LOG_SYNC_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "-3",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_MINOR_VERSION_NUMBER;
	res=yang_value_conv(YANG_VTYPE_UINT8, "1",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_INITIAL_LOG_ANNOUNCE_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_SYNC_RECEIPT_TIMEOUT;
	res=yang_value_conv(YANG_VTYPE_UINT8, "3",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_INITIAL_LOG_SYNC_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "-3",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_CURRENT_LOG_SYNC_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "-3",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_CURRENT_LOG_GPTP_CAP_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "3",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_INITIAL_LOG_PDELAY_REQ_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RO;
	aps[7] = IEEE1588_PTP_CURRENT_LOG_PDELAY_REQ_INTERVAL;
	res=yang_value_conv(YANG_VTYPE_INT8, "0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[0] = IEEE1588_PTP_RW;
	aps[7] = IEEE1588_PTP_ALLOWED_LOST_RESPONSES;
	res=yang_value_conv(YANG_VTYPE_UINT8, "9",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_ALLOWED_FAULTS;
	res=yang_value_conv(YANG_VTYPE_UINT8, "9",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	aps[7] = IEEE1588_PTP_MEAN_LINK_DELAY_THRESH;
	res=yang_value_conv(YANG_VTYPE_PTP_TIME_INTERVAL, "0x27100000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	//0003_ieee1588-ptp/ptp/instances/instance/ports/port
	aps[6] = IEEE1588_PTP_UNDERLYING_INTERFACE;
	aps[7] = 255;
	res=yang_value_conv(YANG_VTYPE_IF_INTERFACE_REF, "cbeth0",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	//0004_ieee1588-ptp/ptp/instances/instance
	//0005_ieee1588-ptp/ptp
	if(kvs[0]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[0]);kvs[0]=NULL;}
	aps[2] = IEEE1588_PTP_INSTANCE_DOMAIN_MAP;
	aps[3] = 255;
	res=yang_value_conv(YANG_VTYPE_HEXUINT16, "0x0000",
		&dbpara.value, &vsize, NULL);
	if(res<0){goto erexit;}
	dbpara.vsize=res;
	if(yang_db_action(dbald, hwald, &dbpara)!=0){goto erexit;}
	res=0;
erexit:
	if(dbpara.value!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, dbpara.value);}
	for(i=0;i<2;i++){
		if(kvs[i]!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[i]);}
	}
	return res;
}
