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
#include <inttypes.h>
#include <string.h>
#include <tsn_unibase/unibase.h>
#include "../yang_modules.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

const char *yang_modules_enum_strings[]={
	"xl4-data",
	"xl4-extmod",
	"ietf-interfaces",
	"ieee802-dot1q-bridge",
	"ieee802-dot1ab-lldp",
	"ieee1588-ptp",
	"ieee802-dot1q-tsn-config-uni",
	"ietf-netconf-monitoring",
	"ietf-yang-library",
	"excelfore-tsn-remote",
	"excelfore-netconf-server",
};
extern uint8_t excelfore_netconf_server_get_enum(char *astr);
extern const char* excelfore_netconf_server_get_string(uint8_t anum);
extern uint8_t excelfore_tsn_remote_get_enum(char *astr);
extern const char* excelfore_tsn_remote_get_string(uint8_t anum);
extern uint8_t ietf_yang_library_get_enum(char *astr);
extern const char* ietf_yang_library_get_string(uint8_t anum);
extern uint8_t ietf_netconf_monitoring_get_enum(char *astr);
extern const char* ietf_netconf_monitoring_get_string(uint8_t anum);
extern uint8_t ieee802_dot1q_tsn_config_uni_get_enum(char *astr);
extern const char* ieee802_dot1q_tsn_config_uni_get_string(uint8_t anum);
extern uint8_t ieee1588_ptp_get_enum(char *astr);
extern const char* ieee1588_ptp_get_string(uint8_t anum);
extern uint8_t ieee802_dot1ab_lldp_get_enum(char *astr);
extern const char* ieee802_dot1ab_lldp_get_string(uint8_t anum);
extern uint8_t ieee802_dot1q_bridge_get_enum(char *astr);
extern const char* ieee802_dot1q_bridge_get_string(uint8_t anum);
extern uint8_t ietf_interfaces_get_enum(char *astr);
extern const char* ietf_interfaces_get_string(uint8_t anum);

uint8_t yang_modules_get_enum(char *astr)
{
	int i;
	for(i=0;i<(int)YANG_MODULES_ENUM_END-(int)XL4_DATA_RO;i++){
		if(!strcmp(astr, yang_modules_enum_strings[i])){return i;}
	}
		return 0xffu;
}

const char *yang_modules_get_string(uint8_t anum)
{
	if(anum>=(uint8_t)XL4_DATA_RO){anum-=(uint8_t)XL4_DATA_RO;}
		if(anum>=(uint8_t)YANG_MODULES_ENUM_END-(uint8_t)XL4_DATA_RO){return NULL;}
	return yang_modules_enum_strings[anum];
}

#ifndef UC_RUNCONF
uint8_t yang_modules_get_node_enums(xl4_data_data_t *xdd, char *astr, uint8_t *anums, int maxele)
{
		return 255;
}
#else
uint8_t yang_modules_get_node_enums(xl4_data_data_t *xdd, char *astr, uint8_t *anums, int maxele)
{
	if(astr == NULL) {return -1;}
	if(!*astr || !anums || !maxele){return -1;}

	char *q, *p=astr;
	get_enum_func get_enum;
	int ec=0;
	uint8_t ed, ef;

	if(*p=='/'){
			p++;
		q=strchr(p, '/');
		if(q!=NULL){
			*q=0;
			q++;
		}
		anums[0]=yang_modules_get_enum(p);
		p=q;
		ec++;
	}
	switch(anums[0]){
	case XL4_DATA_RW:
		get_enum=xl4_data_get_enum;
		break;
	case XL4_EXTMOD_RW:
		if(!xdd){return -1;}
		if (p != NULL)
		{
		    q=strchr(p, '/');
		}
		else
		{
		    q = NULL;
		}
		if(q!=NULL){
			*q=0;
			q++;
			ef=xl4_data_get_modid(xdd, p);
			anums[ec]=ef;
				ec++;
			p=q;
		}else{
			if(maxele==1){
				anums[0]=xl4_data_get_modid(xdd, p);
				if(anums[0]!=0xffu){return 1;}
				return -1;
			}
			ef=anums[1];
		}
		get_enum=xl4_extmod_get_enum_func(xdd, NULL, ef);
		if(!get_enum){return -1;}
		break;
	case IETF_INTERFACES_RW:
		get_enum=ietf_interfaces_get_enum;
		break;
	case IEEE802_DOT1Q_BRIDGE_RW:
		get_enum=ieee802_dot1q_bridge_get_enum;
		break;
	case IEEE802_DOT1AB_LLDP_RW:
		get_enum=ieee802_dot1ab_lldp_get_enum;
		break;
	case IEEE1588_PTP_RW:
		get_enum=ieee1588_ptp_get_enum;
		break;
	case IEEE802_DOT1Q_TSN_CONFIG_UNI_RW:
		get_enum=ieee802_dot1q_tsn_config_uni_get_enum;
		break;
	case IETF_NETCONF_MONITORING_RW:
		get_enum=ietf_netconf_monitoring_get_enum;
		break;
	case IETF_YANG_LIBRARY_RW:
		get_enum=ietf_yang_library_get_enum;
		break;
	case EXCELFORE_TSN_REMOTE_RW:
		get_enum=excelfore_tsn_remote_get_enum;
		break;
	case EXCELFORE_NETCONF_SERVER_RW:
		get_enum=excelfore_netconf_server_get_enum;
		break;
	// get_node_enums_ENUM_END
	default:
		UB_LOG(UBL_ERROR, "%s:the top node=%d doesn't exist\n",
			   __func__, anums[0]);
		return -1;
	}
	while((ec<maxele) && (p!=NULL) && (*p!=0)){
		q=strchr(p, '/');
		if(q!=NULL){
				*q=0;
			q++;
		}
		ed=get_enum(p);
		if(ed==0xffu) {
			UB_LOG(UBL_ERROR, "%s:no such node:%s\n", __func__, p);
			return -1;
		}
		anums[ec]=ed;
		ec++;
		p=q;
	}
	if(ec<maxele){anums[ec]=0xffu;}
	return ec;
}
#endif

#ifndef UC_RUNCONF
int yang_modules_get_node_string(xl4_data_data_t *xdd, char **rstr, uint8_t *anums)
{
	return -1;
}
#else
int yang_modules_get_node_string(xl4_data_data_t *xdd, char **rstr, uint8_t *anums)
{
		const char *astr;
		int rsize;
	int ec=1;
		get_string_func get_string = NULL;
		*rstr=NULL;
		astr=yang_modules_get_string(anums[0]);
		if(!astr){return -1;}
		rsize=(int)strlen(astr)+2;
		*rstr=(char*)UB_SD_GETMEM(YANGINIT_GEN_SMEM, rsize);
		if(ub_assert_fatal(*rstr!=NULL, __func__, NULL)){return -1;}
		(void)memset(*rstr, 0, rsize);
		*rstr[0]='/';
		(void)strcpy(&(*rstr)[1], astr);
		if(anums[1]==255u){return 0;}

	switch(anums[0]){
	case XL4_DATA_RW:
	case XL4_DATA_RO:
		get_string=xl4_data_get_string;
		break;
	case XL4_EXTMOD_RW:
	case XL4_EXTMOD_RO:
		if(!xdd){goto erexit;}
		astr=xl4_data_get_modname(xdd, anums[1]);
		if(!astr){goto erexit;}
		rsize+=(int)strlen(astr)+1;
		*rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *rstr, rsize);
		if(ub_assert_fatal(*rstr!=NULL, __func__, "realloc")){return -1;}
		(void)strcat(*rstr, "/");
		(void)strcat(*rstr, astr);
		get_string=xl4_extmod_get_string_func(xdd, NULL, anums[1]);
		ec=2;
		break;
	case IETF_INTERFACES_RW:
	case IETF_INTERFACES_RO:
		get_string=ietf_interfaces_get_string;
		break;
	case IEEE802_DOT1Q_BRIDGE_RW:
	case IEEE802_DOT1Q_BRIDGE_RO:
		get_string=ieee802_dot1q_bridge_get_string;
		break;
	case IEEE802_DOT1AB_LLDP_RW:
	case IEEE802_DOT1AB_LLDP_RO:
		get_string=ieee802_dot1ab_lldp_get_string;
		break;
	case IEEE1588_PTP_RW:
	case IEEE1588_PTP_RO:
		get_string=ieee1588_ptp_get_string;
		break;
	case IEEE802_DOT1Q_TSN_CONFIG_UNI_RW:
	case IEEE802_DOT1Q_TSN_CONFIG_UNI_RO:
		get_string=ieee802_dot1q_tsn_config_uni_get_string;
		break;
	case IETF_NETCONF_MONITORING_RW:
	case IETF_NETCONF_MONITORING_RO:
		get_string=ietf_netconf_monitoring_get_string;
		break;
	case IETF_YANG_LIBRARY_RW:
	case IETF_YANG_LIBRARY_RO:
		get_string=ietf_yang_library_get_string;
		break;
	case EXCELFORE_TSN_REMOTE_RW:
	case EXCELFORE_TSN_REMOTE_RO:
		get_string=excelfore_tsn_remote_get_string;
		break;
	case EXCELFORE_NETCONF_SERVER_RW:
	case EXCELFORE_NETCONF_SERVER_RO:
		get_string=excelfore_netconf_server_get_string;
		break;
	// get_node_string_ENUM_END
	default:
		UB_LOG(UBL_ERROR, "%s:the top node=%d doesn't exist\n",
			   __func__, anums[0]);
		goto erexit;
		break;
	}
	if (get_string != NULL)
	{
		while(anums[ec]!=255u){
			astr=get_string(anums[ec]);
			if(!astr){break;}
			rsize+=(int)strlen(astr)+1;
			*rstr=(char*)UB_SD_REGETMEM(YANGINIT_GEN_SMEM, *rstr, rsize);
			if(ub_assert_fatal(*rstr!=NULL, __func__, "realloc")){return -1;}
			(void)strcat(*rstr, "/");
			(void)strcat(*rstr, astr);
			ec++;
		}
	}
	else
	{
		goto erexit;
	}
	return 0;
erexit:
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, *rstr);
		*rstr=NULL;
		return -1;
}
#endif
