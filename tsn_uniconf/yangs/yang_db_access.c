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
#include <errno.h>
#include <ctype.h>
#include <tsn_unibase/unibase.h>
#include <tsn_combase/cb_thread.h>
#include "yang_modules.h"
#include "yang_db_access.h"
#include "yang_db_identiyref.h"
#include "yang_db_enumeration.h"

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

static char *get_two_vids(char *vstr, uint16_t *v1, uint16_t *v2)
{
	char *astr, *bstr;
	char *rstr=NULL;
	astr=strchr(vstr, ',');
	if(astr!=NULL){rstr=&astr[1];}
	*v1=strtol(vstr, NULL, 0);
	bstr=strchr(vstr, '-');
	if(bstr && (!rstr || (bstr<rstr))){
		*v2=strtol(&bstr[1], NULL, 0);
	}
	else{*v2=*v1;}
	return rstr;
}

static int value_conv_destcopy(void **destd, void *srcd, uint32_t *size, uint32_t csize)
{
	void *oldp=*destd;
	uint32_t asize=*size;
	if(csize==0u){return -1;}
	if(csize>asize){asize=csize;}
	if(asize>(uint32_t)UC_MAX_VALUESIZE){
		UB_LOG(UBL_ERROR, "%s:asize=%d is too big\n",
		       __func__, asize);
		return -1;
	}
	*destd=UB_SD_REGETMEM(YANGINIT_GEN_SMEM, oldp, asize);
	if(*destd==NULL){
		*destd=oldp;
		return -1;
	}
	memcpy(*destd, srcd, csize);
	if(asize>csize){
		uint8_t *ud=*destd;
		(void)memset(&ud[csize], 0, asize-csize);
	}
	*size=asize;
	return 0;
}

static char *find_hints(char *vstr, const char delimiter)
{
	int i, len;
	len=strlen(vstr);
	for(i=0;i<len;i++){
		if(vstr[i]==delimiter){
			if(i==0){continue;} // the top '/' is not a delimiter
			if(vstr[i-1]=='\\'){
				// escaped '/' is not a delimiter
				memmove(&vstr[i-1], &vstr[i], len-i);
				len-=1;
			}
			vstr[i]=0;
			return &vstr[i+1]; //delimiter is found
		}
	}
	return NULL;
}

int yang_value_conv(uint8_t vtype, char *vstr, void **destd, uint32_t *size, char *hints)
{
	uint32_t csize=0; // copy size
	int res;
	if(!vstr || !destd){
		UB_LOG(UBL_ERROR, "%s:vstr or d is NULL\n", __func__);
		return -1;
	}
	switch(vtype){
	case YANG_VTYPE_DOT1QTYPES_NAME_TYPE:
	case YANG_VTYPE_IF_INTERFACE_REF:
	case YANG_VTYPE_INTERFACE_STATE_REF:
	case YANG_VTYPE_STRING:
	case YANG_VTYPE_IEEE_CHASSIS_ID_TYPE:
	case YANG_VTYPE_IEEE_PORT_ID_TYPE:
	case YANG_VTYPE_YANG_YANG_IDENTIFIER:
	case YANG_VTYPE_REVISION_IDENTIFIER:
	case YANG_VTYPE_INET_URI:
	case YANG_VTYPE_UNION:
		/* FIXME: Currently union is only used in ietf-yang-library module
		 * where all unions are between to string data types */
	case YANG_VTYPE_LEAFREF:
		/* FIXME:
		 * - Currently all leafs in our tree with leafref type
		 *   is referencing a leaf with vtype string.
		 * - So for now we access leafref as a string, but the vtype
		 *   of leafref depends on the referenced leaf. */
	case YANG_VTYPE_INET_IP_ADDRESS:
	case YANG_VTYPE_LLDP_TYPES_MAN_ADDR_TYPE:
	case YANG_VTYPE_BINARY:
		/* Binary in Yang are base64 encoded strings (RFC 6020 9.8.2) */
	case YANG_VTYPE_YANG_DATE_AND_TIME:
	case YANG_VTYPE_CLOCK_IDENTITY:
	{
		char *data;
		res=(int)strlen(vstr);
		if((res>=2) && (vstr[0]=='"') && (vstr[res-1]=='"')){
			csize=res-1;
			data=&vstr[1];
			vstr[res-1]=0;
		}else{
			csize=res+1;
			data=(void*)vstr;
		}
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_X509C2N_TLS_FINGERPRINT:
	{
		uint8_t hexdata[256];
		/* Using ub_hexstr2barray only to parse, but saving
		 * TLS fingerprint as string */
		memset(hexdata, 0, sizeof(hexdata));
		res=ub_hexstr2barray(vstr, hexdata, ':', 256);
		if(res<=1||res>=255){
			UB_LOG(UBL_ERROR, "%s:invalid tls fingerprint, %s\n", __func__, vstr);
			res=-1;
		} else {
			char *data=(void*)vstr;;
			res=(int)strlen(vstr);
			csize=res+1;
			res=value_conv_destcopy(destd, data, size, csize);
		}
		break;
	}
	case YANG_VTYPE_BOOLEAN:
	{
		int8_t data[1];
		csize=1;
		if(!strcmp(vstr, "true") || !strcmp(vstr, "1")){
			data[0]=1;
		}else{
			data[0]=0;
		}
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_INT8:
	case YANG_VTYPE_INT16:
	case YANG_VTYPE_INT32:
	{
		long int data[1];
		data[0]=strtol(vstr, NULL, 0);
		if(vtype==(uint8_t)YANG_VTYPE_INT8){
			csize=1;
		}else if(vtype==(uint8_t)YANG_VTYPE_INT16){
			csize=2;
		}else{
			csize=4;
		}
		ADJUST_ENDIAN(data, sizeof(long int)-csize, csize);
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_MRP_PROTOCOL:
	case YANG_VTYPE_ENUMERATION:
	case YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM:
	case YANG_VTYPE_IEEE_CHASSIS_ID_SUBTYPE_TYPE:
	case YANG_VTYPE_IEEE_PORT_ID_SUBTYPE_TYPE:
	case YANG_VTYPE_LLDP_TYPES_MAN_ADDR_IF_SUBTYPE:
	{
		char *endptr = NULL;
		uint32_t data[1];
		long int value;
		char *ehints;
		/* Enumerations can be set either by numeric value or enum string */
		ehints=find_hints(vstr, '/');
		if(ehints!=NULL){hints=ehints;}// ethints supercedes hints
		value=strtol(vstr, &endptr, 0);
		if (endptr != vstr) {
			data[0]=(uint32_t)value;
		} else if (NULL != hints) {
			data[0]=yang_enumeration_getval(vstr, hints);
		}
		csize=4;
		ADJUST_ENDIAN(data, sizeof(long int)-csize, csize);
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_INT64:
	case YANG_VTYPE_DECIMAL64:
	case YANG_VTYPE_TIME_INTERVAL:
	case YANG_VTYPE_PTP_TIME_INTERVAL:
	{
		long long int data[1];
		data[0]=strtoll(vstr, NULL, 0);
		csize=8;
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_PRIORITY_TYPE:
	case YANG_VTYPE_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1Q_TYPES_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1QTYPES_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1QTYPES_PRIORITY_TYPE:
	case YANG_VTYPE_UINT8:
	case YANG_VTYPE_YANG_COUNTER8:
	case YANG_VTYPE_UINT16:
	case YANG_VTYPE_HEXUINT16:
	case YANG_VTYPE_DOT1QTYPES_VLANID:
	case YANG_VTYPE_YANG_COUNTER16:
	case YANG_VTYPE_UINT32:
	case YANG_VTYPE_YANG_TIMETICKS:
	case YANG_VTYPE_YANG_COUNTER32:
	case YANG_VTYPE_YANG_GAUGE32:
	case YANG_VTYPE_DOT1QTYPES_PORT_NUMBER_TYPE:
	case YANG_VTYPE_PORT_NUMBER_TYPE:
	case YANG_VTYPE_INET_PORT_NUMBER:
	case YANG_VTYPE_YANG_ZERO_BASED_COUNTER32:
	case YANG_VTYPE_DOT1QTYPES_VLAN_INDEX_TYPE:
	case YANG_VTYPE_DOT1QTYPES_MSTID_TYPE:
	case YANG_VTYPE_PORT_STATE:
	case YANG_VTYPE_YANG_TIMESTAMP:
	{
		unsigned long int data[1];
		data[0]=strtoul(vstr, NULL, 0);
		if((vtype==(uint8_t)YANG_VTYPE_UINT8) ||
		   (vtype==(uint8_t)YANG_VTYPE_PRIORITY_TYPE) ||
		   (vtype==(uint8_t)YANG_VTYPE_TRAFFIC_CLASS_TYPE) ||
		   (vtype==(uint8_t)YANG_VTYPE_DOT1Q_TYPES_TRAFFIC_CLASS_TYPE) ||
		   (vtype==(uint8_t)YANG_VTYPE_DOT1QTYPES_TRAFFIC_CLASS_TYPE) ||
		   (vtype==(uint8_t)YANG_VTYPE_DOT1QTYPES_PRIORITY_TYPE) ||
		   (vtype==(uint8_t)YANG_VTYPE_YANG_COUNTER8) ||
		   (vtype==(uint8_t)YANG_VTYPE_PORT_STATE)){
			csize=1;
		}else if((vtype==(uint8_t)YANG_VTYPE_UINT16) ||
			 (vtype==(uint8_t)YANG_VTYPE_HEXUINT16) ||
			 (vtype==(uint8_t)YANG_VTYPE_INET_PORT_NUMBER) ||
			 (vtype==(uint8_t)YANG_VTYPE_DOT1QTYPES_VLANID) ||
			 (vtype==(uint8_t)YANG_VTYPE_YANG_COUNTER16)){
			csize=2;
		}else{
			csize=4;
		}
		ADJUST_ENDIAN(data, sizeof(unsigned long int)-csize, csize);
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_UINT64:
	case YANG_VTYPE_YANG_GAUGE64:
	case YANG_VTYPE_YANG_COUNTER64:
	{
		unsigned long long int data[1];
		data[0]=strtoull(vstr, NULL, 0);
		csize=8;
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_DELAY_MECHANISM:
	{
		uint8_t data[1];
		csize=1;
		if(!strcmp(vstr, "e2e")){
			data[0]=YANG_VTYPE_ENUM_e2e;
		}else if(!strcmp(vstr, "p2p")){
			data[0]=YANG_VTYPE_ENUM_p2p;
		}else if(!strcmp(vstr, "no-mechanism")){
			data[0]=YANG_VTYPE_ENUM_no_mechanism;
		}else if(!strcmp(vstr, "common-p2p")){
			data[0]=YANG_VTYPE_ENUM_common_p2p;
		}else if(!strcmp(vstr, "special")){
			data[0]=YANG_VTYPE_ENUM_special;
		}else{
			data[0]=0;
		}
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_IEEE_MAC_ADDRESS:
	{
		// "%02hhx-%02hhx-%02hhx-%02hhx-%02hhx-%02hhx"
		uint8_t data[6];
		res=ub_hexstr2barray(vstr, data, '-', 6);
		if(res!=6){
			UB_LOG(UBL_WARN, "%s:invalid parts are filled with 0, %s\n",
			       __func__, vstr);
			(void)memset(&data[res], 0, 6-res);
		}
		csize=6;
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_YANG_PHYS_ADDRESS:
	{
		// "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
		uint8_t data[6];
		res=ub_hexstr2barray(vstr, data, ':', 6);
		if(res!=6){
			UB_LOG(UBL_WARN, "%s:invalid parts are filled with 0, %s\n",
			       __func__, vstr);
			(void)memset(&data[res], 0, 6-res);
		}
		csize=6;
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_STREAMID:
	{
		// "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
		uint8_t data[8];
		res=ub_hexstr2barray(vstr, data, ':', 8);
		if(res!=8){
			UB_LOG(UBL_WARN,
			       "%s:invalid parts are filled with 0, %s\n", __func__, vstr);
			(void)memset(&data[res], 0, 8-res);
		}
		csize=8;
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_TSN_STREAM_ID_TYPE:
	{
		// "%02hhx-%02hhx-%02hhx-%02hhx-%02hhx-%02hhx:%02hhx-%02hhx"
		uint8_t data[8];
		res=ub_hexstr2barray(vstr, data, '-', 6);
		if(res==6 && vstr[17]==':'){
			res=ub_hexstr2barray(&vstr[18], &data[6], '-', 2);
			res+=6;
		}
		if(res!=8){
			UB_LOG(UBL_WARN,
			       "%s:invalid parts are filled with 0, %s\n", __func__, vstr);
			(void)memset(&data[res], 0, 8-res);
		}
		csize=8;
		res=value_conv_destcopy(destd, data, size, csize);
		break;
	}
	case YANG_VTYPE_DOT1QTYPES_VID_RANGE_TYPE:
	{
		uint8_t *ud;
		uint16_t data[2];
		void *oldp;
		uint32_t asize=*size; // allocation size
		// "1,10-100,250,500-1000", this type of string comes
		// conv to (1,1),(10,100),(250,250),(500,1000), add terminator (0,0)
		// set it as 1,1,10,100,250,250,500,1000,0,0 in uint16_t array
		csize=2u*(uint32_t)sizeof(uint16_t);
		res=-1;
		while(vstr && vstr[0]){
			vstr=get_two_vids(vstr, &data[0], &data[1]);
			csize+=2u*(uint32_t)sizeof(uint16_t);
			if(csize>asize){asize=csize;}
			oldp=*destd;
			*destd=UB_SD_REGETMEM(YANGINIT_GEN_SMEM, oldp, asize);
			if(*destd==NULL){
				*destd=oldp;
				res=-1;
				break;
			}
			*size=asize;
			ud=(uint8_t*)*destd;
			(void)memcpy(&ud[csize-(4u*sizeof(uint16_t))],
				     data, 2*(int)sizeof(uint16_t));
			res=0;
		}
		if(res==0){
			(void)memset(&ud[csize-(2u*sizeof(uint16_t))],
			       0, 2*(int)sizeof(uint16_t));
			if(asize>csize){(void)memset(&ud[csize], 0, asize-csize);}
			*size=asize;
		}
		break;
	}
	case YANG_VTYPE_DS_DATASTORE_REF:
	case YANG_VTYPE_IDENTITYREF:
	{
		uint32_t data[1]={INVALID_IDENTIY_VALUE};
		char *ehints;
		ehints=find_hints(vstr, '/');
		if(ehints!=NULL){hints=ehints;}// ethints supercedes hints
		if(NULL != hints) {
			data[0]=yang_identityref_getval(vstr, hints);
		}else if(YANG_VTYPE_DS_DATASTORE_REF == vtype) {
			data[0]=yang_identityref_getval(vstr, "datastore");
		}else if(!strncmp(vstr, "cc-", 3)) {
			data[0]=yang_identityref_getval(vstr, "clock-class");
		}else if(!strncmp(vstr, "ca-", 3)) {
			data[0]=yang_identityref_getval(vstr, "clock-accuracy");
		}else {
			/* For other identity-ref, there is no pattern in the identiy string.
			 * So currently there is no way but to compare the input string to
			 * each identity string in each table
			 */
			int tblidx;
			for(tblidx = 0; yang_identityref_list[tblidx].reftbl != NULL; tblidx++) {
				/* skip identities that have pattern already checked above */
				if ((strcmp(yang_identityref_list[tblidx].keyword, "clock-class")==0) ||
				    (strcmp(yang_identityref_list[tblidx].keyword, "clock-accuracy")==0)) {
					continue;
				}
				data[0]=yang_identityref_getval(vstr,
				            (char*)yang_identityref_list[tblidx].keyword);
				if (INVALID_IDENTIY_VALUE != data[0]){break;}
			}
		}
		if (INVALID_IDENTIY_VALUE == data[0]) {
			UB_LOG(UBL_ERROR, "%s:unknown identityref vstr=%s\n", __func__, vstr);
			res=-1;
		}else{
			csize=4;
			ADJUST_ENDIAN(data, sizeof(long int)-csize, csize);
			res=value_conv_destcopy(destd, data, size, csize);
		}
		break;
	}
	case YANG_VTYPE_LLDP_TYPES_SYSTEM_CAPABILITIES_MAP:
	case YANG_VTYPE_BITS:
	{
		uint32_t data[1] = {0x00000000};
		csize = 4;
		data[0] = strtol((const char*)vstr, NULL, 2);

		res=value_conv_destcopy(destd, data, size, csize);
	}
		break;
	case YANG_VTYPE_INSTANCE_IDENTIFIER:
	case YANG_VTYPE_EMPTY:
	case YANG_VTYPE_ENUM_END:
	default:
		UB_LOG(UBL_ERROR, "%s:not supported vtype=%d\n", __func__, vtype);
		return -1;
	}
	if(res!=0){
		UB_LOG(UBL_ERROR, "%s:conversion failed, vtype=%d, vstr=%s\n",
		       __func__, vtype, vstr);
		return res;
	}
	return csize;
}

char *yang_value_string(uint8_t vtype, void *value, uint32_t vsize, uint8_t index, char *hints)
{
	static char vstr[64];
	vstr[0]=0;
	switch(vtype){
	case YANG_VTYPE_DOT1QTYPES_NAME_TYPE:
	case YANG_VTYPE_IF_INTERFACE_REF:
	case YANG_VTYPE_INTERFACE_STATE_REF:
	case YANG_VTYPE_STRING:
	case YANG_VTYPE_YANG_YANG_IDENTIFIER:
	case YANG_VTYPE_REVISION_IDENTIFIER:
	case YANG_VTYPE_INET_URI:
	case YANG_VTYPE_UNION:
	case YANG_VTYPE_LEAFREF:
	case YANG_VTYPE_X509C2N_TLS_FINGERPRINT:
	case YANG_VTYPE_INET_IP_ADDRESS:
	case YANG_VTYPE_BINARY:
	case YANG_VTYPE_YANG_DATE_AND_TIME:
	case YANG_VTYPE_CLOCK_IDENTITY:
		return (char*)value;
	case YANG_VTYPE_BOOLEAN:
		if(*((uint8_t*)value)!=0){
			(void)strcpy(vstr, "true");
		}else{
			(void)strcpy(vstr, "false");
		}
		return vstr;
	case YANG_VTYPE_PRIORITY_TYPE:
	case YANG_VTYPE_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1Q_TYPES_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1QTYPES_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1QTYPES_PRIORITY_TYPE:
	case YANG_VTYPE_INT8:
	case YANG_VTYPE_INT16:
	case YANG_VTYPE_INT32:
	case YANG_VTYPE_INT64:
	case YANG_VTYPE_DECIMAL64:
	case YANG_VTYPE_TIME_INTERVAL:
	case YANG_VTYPE_PTP_TIME_INTERVAL:
		if(vsize==1u){(void)sprintf(vstr, "%"PRIi8, *((int8_t*)value));}
		else if(vsize==2u){(void)sprintf(vstr, "%"PRIi16, *((int16_t*)value));}
		else if(vsize==4u){(void)sprintf(vstr, "%"PRIi32, *((int32_t*)value));}
		else if(vsize==8u){(void)sprintf(vstr, "%"PRIi64, *((int64_t*)value));}
		else {}
		return vstr;
	case YANG_VTYPE_MRP_PROTOCOL:
	case YANG_VTYPE_ENUMERATION:
	case YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM:
		if (NULL != hints) {
			char* rstr=yang_enumeration_getstr(*((uint32_t*)value), hints);
			if (NULL != rstr) {
				memset(vstr, 0, sizeof(vstr));
				(void)strcpy(vstr, rstr);
			}
		} else {
			(void)sprintf(vstr, "%"PRIu32, *((uint32_t*)value));
		}
		break;
	case YANG_VTYPE_UINT8:
	case YANG_VTYPE_YANG_COUNTER8:
	case YANG_VTYPE_UINT16:
	case YANG_VTYPE_DOT1QTYPES_VLANID:
	case YANG_VTYPE_YANG_COUNTER16:
	case YANG_VTYPE_UINT32:
	case YANG_VTYPE_YANG_COUNTER32:
	case YANG_VTYPE_YANG_GAUGE32:
	case YANG_VTYPE_DOT1QTYPES_PORT_NUMBER_TYPE:
	case YANG_VTYPE_PORT_NUMBER_TYPE:
	case YANG_VTYPE_INET_PORT_NUMBER:
	case YANG_VTYPE_YANG_ZERO_BASED_COUNTER32:
	case YANG_VTYPE_DOT1QTYPES_VLAN_INDEX_TYPE:
	case YANG_VTYPE_DOT1QTYPES_MSTID_TYPE:
	case YANG_VTYPE_UINT64:
	case YANG_VTYPE_YANG_GAUGE64:
	case YANG_VTYPE_YANG_COUNTER64:
	case YANG_VTYPE_DELAY_MECHANISM:
	case YANG_VTYPE_PORT_STATE:
		if(vsize==1u){(void)sprintf(vstr, "%"PRIu8, *((uint8_t*)value));}
		else if(vsize==2u){(void)sprintf(vstr, "%"PRIu16, *((uint16_t*)value));}
		else if(vsize==4u){(void)sprintf(vstr, "%"PRIu32, *((uint32_t*)value));}
		else if(vsize==8u){(void)sprintf(vstr, "%"PRIu64, *((uint64_t*)value));}
		else {}
		return vstr;
	case YANG_VTYPE_HEXUINT16:
	{
		uint16_t tmp;
		if ((vsize/sizeof(uint16_t))>1) {
			tmp=(((uint16_t*)value)[index]);
		} else {
			tmp=*((uint16_t*)value);
		}
		(void)sprintf(vstr, "0x%04X", tmp);
		return vstr;
	}
	case YANG_VTYPE_IEEE_MAC_ADDRESS:
		(void)sprintf(vstr, "%02X-%02X-%02X-%02X-%02X-%02X", UB_ARRAY_B6(((uint8_t *)value)));
		return vstr;
	case YANG_VTYPE_YANG_PHYS_ADDRESS:
		(void)sprintf(vstr, UB_PRIhexB6, UB_ARRAY_B6(((uint8_t *)value)));
		return vstr;
	case YANG_VTYPE_STREAMID:
		(void)sprintf(vstr, UB_PRIhexB8, UB_ARRAY_B8(((uint8_t *)value)));
		return vstr;
	case YANG_VTYPE_TSN_STREAM_ID_TYPE:
		(void)sprintf(vstr, "%02X-%02X-%02X-%02X-%02X-%02X:%02X-%02X",
			      UB_ARRAY_B8(((uint8_t *)value)));
		return vstr;
	case YANG_VTYPE_DOT1QTYPES_VID_RANGE_TYPE:
	{
		uint32_t i;
		uint16_t v1,v2;
		char astr[16];
		char *delim="";
		uint16_t *ud=(uint16_t*)value;
		for(i=0u;i<8u;i++){
			if(vsize<2u*sizeof(uint16_t)*(i+1u)){break;}
			v1=ud[i*2u];
			v2=ud[(i*2u)+1u];
			if((v1==0u) && (v2==0u)){break;}
			(void)sprintf(astr, "%s%d-%d", delim, v1, v2);
			if((strlen(vstr)+strlen(astr)+1u)>sizeof(vstr)){break;}
			(void)strcat(vstr, astr);
			delim=",";
		}
		return vstr;
	}
	case YANG_VTYPE_DS_DATASTORE_REF:
	case YANG_VTYPE_IDENTITYREF:
		if(NULL != hints) {
			char* rstr=yang_identityref_getstr(*((uint32_t*)value), hints);
			if (NULL != rstr) {
				memset(vstr, 0, sizeof(vstr));
				(void)strcpy(vstr, rstr);
			}
		}else if(YANG_VTYPE_DS_DATASTORE_REF == vtype) {
			char* rstr=yang_identityref_getstr(*((uint32_t*)value), "datastore");
			if (NULL != rstr) {
				memset(vstr, 0, sizeof(vstr));
				(void)strcpy(vstr, rstr);
			}
		} else {
			UB_LOG(UBL_ERROR, "%s:cannot convert identityref without hints\n", __func__);
		}
		break;
	case YANG_VTYPE_INSTANCE_IDENTIFIER:
	case YANG_VTYPE_EMPTY:
	case YANG_VTYPE_BITS:
	case YANG_VTYPE_ENUM_END:
	default:
		break;
	}
	return vstr;
}

int yang_sizeof_vtype(uint8_t vtype)
{
	int csize=-1; //-1: vtype has no fix size
	switch(vtype){
	case YANG_VTYPE_DOT1QTYPES_NAME_TYPE:
	case YANG_VTYPE_IF_INTERFACE_REF:
	case YANG_VTYPE_INTERFACE_STATE_REF:
	case YANG_VTYPE_STRING:
	case YANG_VTYPE_YANG_YANG_IDENTIFIER:
	case YANG_VTYPE_REVISION_IDENTIFIER:
	case YANG_VTYPE_INET_URI:
	case YANG_VTYPE_IEEE_CHASSIS_ID_TYPE:
	case YANG_VTYPE_YANG_DATE_AND_TIME:
	case YANG_VTYPE_CLOCK_IDENTITY:
		break;
	case YANG_VTYPE_BOOLEAN:
		csize=1;
		break;
	case YANG_VTYPE_INT8:
		csize=1;
		break;
	case YANG_VTYPE_INT16:
		csize=2;
		break;
	case YANG_VTYPE_INT32:
	case YANG_VTYPE_MRP_PROTOCOL:
	case YANG_VTYPE_ENUMERATION:
	case YANG_VTYPE_FRAME_PREEMPTION_STATUS_ENUM:
		csize=4;
		break;
	case YANG_VTYPE_INT64:
	case YANG_VTYPE_DECIMAL64:
	case YANG_VTYPE_TIME_INTERVAL:
	case YANG_VTYPE_PTP_TIME_INTERVAL:
		csize=8;
		break;
	case YANG_VTYPE_UINT8:
	case YANG_VTYPE_PRIORITY_TYPE:
	case YANG_VTYPE_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1Q_TYPES_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1QTYPES_TRAFFIC_CLASS_TYPE:
	case YANG_VTYPE_DOT1QTYPES_PRIORITY_TYPE:
	case YANG_VTYPE_YANG_COUNTER8:
	case YANG_VTYPE_PORT_STATE:
		csize=1;
		break;
	case YANG_VTYPE_UINT16:
	case YANG_VTYPE_HEXUINT16:
	case YANG_VTYPE_DOT1QTYPES_VLANID:
	case YANG_VTYPE_YANG_COUNTER16:
		csize=2;
		break;
	case YANG_VTYPE_UINT32:
	case YANG_VTYPE_YANG_COUNTER32:
	case YANG_VTYPE_YANG_GAUGE32:
	case YANG_VTYPE_DOT1QTYPES_PORT_NUMBER_TYPE:
	case YANG_VTYPE_PORT_NUMBER_TYPE:
	case YANG_VTYPE_YANG_ZERO_BASED_COUNTER32:
	case YANG_VTYPE_DOT1QTYPES_VLAN_INDEX_TYPE:
	case YANG_VTYPE_DOT1QTYPES_MSTID_TYPE:
		csize=4;
		break;
	case YANG_VTYPE_UINT64:
	case YANG_VTYPE_YANG_GAUGE64:
	case YANG_VTYPE_YANG_COUNTER64:
		csize=8;
		break;
	case YANG_VTYPE_DELAY_MECHANISM:
		csize=1;
		break;
	case YANG_VTYPE_IEEE_MAC_ADDRESS:
	case YANG_VTYPE_YANG_PHYS_ADDRESS:
		csize=6;
		break;
	case YANG_VTYPE_STREAMID:
		csize=8;
		break;
	case YANG_VTYPE_TSN_STREAM_ID_TYPE:
		csize=8;
		break;
	case YANG_VTYPE_DOT1QTYPES_VID_RANGE_TYPE:
		break;
	case YANG_VTYPE_IDENTITYREF:
		csize=4;
		break;
	case YANG_VTYPE_LEAFREF:
	case YANG_VTYPE_INSTANCE_IDENTIFIER:
	case YANG_VTYPE_EMPTY:
	case YANG_VTYPE_BINARY:
	case YANG_VTYPE_BITS:
	case YANG_VTYPE_ENUM_END:
	default:
		UB_LOG(UBL_ERROR, "%s:not supported vtype=%d\n", __func__, vtype);
	}
	return csize;
}

uint32_t yang_db_create_key(uint8_t *pap, uint8_t *ap, kvs_t *kvs, uint8_t *kss, void *key)
{
	int i, apc, papc=0, kpc=0;
	int ko=0;
	uint8_t apter=255; // ap terminator
	if(pap!=NULL){
		for(i=0;;i++) {
			if(pap[i]==apter){break;}
		}
		papc=i;
	}
	if(ap!=NULL){
		for(i=0;;i++) {
			if(ap[i]==apter){break;}
		}
		apc=i+1;
	}else{
		ap=&apter;
		apc=1;
	}
	if(kvs!=NULL){
		for(i=0;;i++){
			if(!kvs[i]){break;}
			kpc+=(int)kss[i];
			kpc+=(int)sizeof(uint8_t);
		}
	}
	if((papc+apc+kpc)==0){return 0;}
	if((papc+apc+kpc)>UC_MAX_KEYSIZE){
		UB_LOG(UBL_ERROR, "%s:key size=%d is too big\n", __func__, papc+apc+kpc);
		return 0;
	}
	for(i=0;i<papc;i++){
		((uint8_t*)key)[i]=pap[i];
	}
	for(i=0;i<apc;i++){
		((uint8_t*)key)[papc+i]=ap[i];
	}
	if(kvs!=NULL){
		for(i=0;;i++){
			if(!kvs[i]){break;}
			if(i>0){ko+=(int)kss[i-1]+(int)sizeof(uint8_t);}
			memcpy(&((uint8_t*)key)[papc+apc+ko], &kss[i], sizeof(uint8_t));
			memcpy(&((uint8_t*)key)[papc+apc+ko+(int)sizeof(uint8_t)], kvs[i], kss[i]);
		}
	}
	return papc+apc+kpc;
}

int yang_db_extract_key(void *key, uint32_t ksize, uint8_t **ap, kvs_t *kvs, uint8_t *kss)
{
	uint32_t i, j, an;
	uint32_t vn=0;
	kvs[0]=NULL;
	if(!key){return -1;}
	for(i=0;i<ksize;i++){
		if(((uint8_t*)key)[i]==255u){break;}
	}
	an=i+1u;
	*ap=UB_SD_GETMEM(YANGINIT_GEN_SMEM, an);
	if(ub_assert_fatal(*ap!=NULL, __func__, NULL)){return -1;}
	for(i=0;i<an;i++){
		(*ap)[i]=((uint8_t*)key)[i];
	}
	j=0u;
	while((an+j)<ksize){
		j+=((uint8_t*)key)[an+j]+1u;
		vn++;
	}
	if(vn==0u){
		return 0;
	}
	if(vn>(uint32_t)UC_MAX_VALUEKEYS){
		UB_LOG(UBL_ERROR, "%s:too many value keys, vn=%d\n", __func__, vn);
		return -1;
	}
	j=0;
	for(i=0;i<vn;i++){
		kss[i]=((uint8_t*)key)[an+j];
		j+=1u;
		kvs[i]=UB_SD_GETMEM(YANGINIT_GEN_SMEM, kss[i]);
		if(ub_assert_fatal(kvs[i]!=NULL, __func__, NULL)){return -1;}
		(void)memcpy(kvs[i], &((uint8_t*)key)[an+j], kss[i]);
		j+=kss[i];
	}
	kvs[vn]=NULL;
	return 0;
}

void yang_db_extract_key_free(uint8_t *ap, kvs_t *kvs, uint8_t *kss)
{
	int i;
	if(ap!=NULL){UB_SD_RELMEM(YANGINIT_GEN_SMEM, ap);}
	for(i=0;i<UC_MAX_VALUEKEYS;i++){
		if(!kvs[i]){break;}
		UB_SD_RELMEM(YANGINIT_GEN_SMEM, kvs[i]);
	}
}

void *yang_db_key_bottomp(void *key, uint32_t ksize)
{
	int i, j, an, vn=0;
	int k=0;
	if(!key){return NULL;}
	for(i=0;;i++){
		if(((uint8_t*)key)[i]==255u){break;}
	}
	an=i+1;
	j=0;
	while((an+j)<(int)ksize){
		k=j;
		j+=(int)((uint8_t*)key)[an+j]+1;
		vn++;
	}
	if(!vn){return NULL;}
	return &((uint8_t*)key)[an+k];
}

void yang_db_keydump_log(int llevel, uint8_t *ap, kvs_t *kvs, uint8_t *kss)
{
	int i, j, sp, kpc=0;
	uint32_t apc;
	int res;
	char astr[128]={0};
	const char *mname;
	const char *status;
	if(!ub_clog_on(UB_LOGCAT, llevel)){return;}
	for(i=0;;i++) {
		if(ap[i]==255u){break;}
	}
	apc=(uint32_t)i;
	if(kvs){
		for(i=0;;i++){
			if(!kvs[i]){break;}
			kpc+=(int)kss[i];
		}
	}
	if(apc<2u){return;}
	mname=yang_modules_get_string(ap[0]);
	if(!mname){return;}
	status=(ap[0]>=0x80)?"(RO)":"(RW)";
	(void)snprintf(astr, sizeof(astr), "nodes%s - %s", status, mname);
	for(i=1;i<(int)apc;i++){
		res=snprintf(&astr[strlen(astr)], sizeof(astr)-strlen(astr),
			     "/%d", ap[i]);
		if(res<2){break;}
	}
	UB_LOG(llevel, "%s\n", astr);
	if(!kpc){return;}
	(void)strcpy(astr, "keys - ");
	sp=strlen(astr);
	for(j=0;kvs[j] && (sp<(int)sizeof(astr)-1);j++){
		ap=kvs[j];
		for(i=0;(i<(int)kss[j]) && ((sp+(i*3)+4)<(int)sizeof(astr));i++){
			ub_byte2hexstr(ap[i], &astr[sp+(i*3)]);
			if(i==(int)kss[j]-1){
				astr[sp+(i*3)+2]=' ';
				astr[sp+(i*3)+3]=0;
				sp+=(i*3)+3;
			}else{
				astr[sp+(i*3)+2]=':';
			}
		}
	}
	UB_LOG(llevel, "%s\n", astr);
}

void yang_db_keyvaluedump_log(int llevel, uint8_t *ap, kvs_t *kvs, uint8_t *kss,
			      void *value, uint32_t vsize)
{
	uint8_t *uv=(uint8_t *)value;
	char astr[128]={0};
	int sp;
	uint32_t i;
	if(!ub_clog_on(UB_LOGCAT, llevel)){return;}
	yang_db_keydump_log(llevel, ap, kvs, kss);
	(void)strcpy(astr, "value - ");
	sp=(int)strlen(astr);
	for(i=0;(i<vsize) && ((sp+(i*3))<((int)sizeof(astr)-1));i++){
		ub_byte2hexstr(uv[i], &astr[sp+(i*3)]);
		astr[sp+(i*3)+2]=' ';
	}
	if(i==vsize){
		UB_LOG(llevel, "%s\n", astr);
	}else{
		UB_LOG(llevel, "%s...\n", astr);
	}
}


static void swap_kvs(kvs_t *kvs1, uint8_t *kss1, kvs_t *kvs2, uint8_t *kss2)
{
	int i;
	void *tv;
	uint8_t ts;
	for(i=0;i<=YDBI_MAX_KV_DEPTH;i++){
		if((kvs1[i]==NULL) || (kvs2[i]==NULL)){break;}
		tv=kvs1[i];
		ts=kss1[i];
		kvs1[i]=kvs2[i];
		kss1[i]=kss2[i];
		kvs2[i]=tv;
		kss2[i]=ts;
	}
}

int yang_db_listcopy(uc_dbald *dbald, uint8_t *ap, kvs_t *kvs, uint8_t *kss,
		     kvs_t *nkvs, uint8_t *nkss)
{
	int i;
	uc_range *range;
	uint32_t ksize, nksize, vsize;
	void *nkey, *value;
	uint8_t *aap;
	void *akvs[YDBI_MAX_KV_DEPTH+1];
	uint8_t akss[YDBI_MAX_KV_DEPTH];
	yang_db_access_para_t dbpara;
	int res=0;
	bool match;
	uint8_t cap[YDBI_MAX_AP_DEPTH];

	for(ksize=0;ksize<YDBI_MAX_AP_DEPTH;ksize++){
		if(ap[ksize]==255){break;}
		cap[ksize]=ap[ksize];
	}
	if(ksize==0 || ksize==YDBI_MAX_AP_DEPTH){return -1;}
	cap[ksize-1]+=1;
	range=uc_get_range(dbald, ap, ksize, cap, ksize);
	if(!range){
		UB_LOG(UBL_INFO, "%s:no data to copy\n", __func__);
		return 0;
	}
	while(true){
		if(uc_get_keyvalue_in_range(dbald, range, &nkey, &nksize, &value, &vsize,
					    UC_DBAL_FORWARD)!=0){break;}
		if(yang_db_extract_key(nkey, nksize, &aap, akvs, akss)){break;}

		// check if it is copied data. Don't make 'copy of copy'.
		match=false;
		for(i=0;i<=YDBI_MAX_KV_DEPTH;i++){
			if(nkvs[i]==NULL){
				// all kvs are matched to akvs
				match=true;
				break;
			}
			if(akvs[i]==NULL){break;}
			//ub_hexdump(true, false, akvs[i], akss[i], 0);
			//ub_hexdump(true, false, nkvs[i], nkss[i], 0);
			if(nkss[i]==akss[i] && !memcmp(nkvs[i],akvs[i],nkss[i])){continue;}
			break;
		}
		if(match){
			UB_LOG(UBL_DEBUG, "this is a new key data, no copy on it\n");
			yang_db_extract_key_free(aap, akvs, akss);
			continue;
		}

		// check if the value key matches
		match=false;
		for(i=0;i<=YDBI_MAX_KV_DEPTH;i++){
			if(kvs[i]==NULL){
				// all kvs are matched to akvs
				match=true;
				break;
			}
			if(akvs[i]==NULL){break;}
			if(kss[i]==akss[i] && !memcmp(kvs[i],akvs[i],kss[i])){continue;}
			break;
		}
		if(!match){
			yang_db_extract_key_free(aap, akvs, akss);
			continue;
		}

		// copy with the new key
		swap_kvs(akvs, akss, nkvs, nkss);
		memset(&dbpara, 0, sizeof(dbpara));
		dbpara.onhw=YANG_DB_ONHW_NOACTION;
		dbpara.atype=YANG_DB_ACTION_READ;
		dbpara.aps=aap;
		dbpara.kvs=akvs;
		dbpara.kss=akss;
		dbpara.value=value;
		dbpara.vsize=vsize;
		res=yang_db_action(dbald, NULL, &dbpara);
		if(!res){
			UB_LOG(UBL_DEBUG, "%s:a new key data already exists\n", __func__);
			dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
			yang_db_action(dbald, NULL, &dbpara);
		}else{
			dbpara.atype=YANG_DB_ACTION_CREATE;
			res=yang_db_action(dbald, NULL, &dbpara);
			if(!res){
				UB_LOG(UBL_DEBUG, "%s:copy an item\n", __func__);
				yang_db_keyvaluedump_log(UBL_DEBUGV, aap, akvs, akss, value, vsize);
			}
		}
		swap_kvs(akvs, akss, nkvs, nkss);
		yang_db_extract_key_free(aap, akvs, akss);
		if(res){
			UB_LOG(UBL_ERROR, "%s:can't copy\n", __func__);
			break;
		}
	}
	uc_get_range_release(dbald, range);
	return res;
}

static int listmove_keymod(uc_dbald *dbald, yang_db_access_para_t *dbpara,
			   int keymod_rcode)
{
	int res;
	switch(keymod_rcode){
	case KEYMOD_NOMOVE:
		return 0;
	case KEYMOD_MOVE_NOOVERWRITE:
		res=yang_db_action(dbald, NULL, dbpara);
		if(!res){
			dbpara->atype=YANG_DB_ACTION_READ_RELEASE;
			yang_db_action(dbald, NULL, dbpara);
			return 0;
		}
		// fall through
	case KEYMOD_MOVE_OVERWRITE:
		dbpara->atype=YANG_DB_ACTION_CREATE;
		res=yang_db_action(dbald, NULL, dbpara);
		if(res!=0){
			UB_LOG(UBL_ERROR, "%s:failed to move an item\n", __func__);
			yang_db_keyvaluedump_log(UBL_INFO,
						 dbpara->aps, dbpara->kvs, dbpara->kss,
						 dbpara->value, dbpara->vsize);
			return -1;
		}
		return 1;
	case KEYMOD_NOMOVE_DELETE:
		return 1;
	case KEYMOD_ERROR:
	default:
		return -1;
	}
}

int yang_db_listmove(uc_dbald *dbald, uint8_t *ap, kvs_t *kvs, uint8_t *kss,
		     bool forward_direction, key_modifier_t keymod_cb)
{
	int i;
	uc_range *range;
	uint32_t ksize, nksize, vsize;
	void *nkey=NULL, *value;
	uint8_t *aap;
	uint8_t mapsize, mkvsize;
	void *akvs[YDBI_MAX_KV_DEPTH+1];
	uint8_t akss[YDBI_MAX_KV_DEPTH];
	yang_db_access_para_t dbpara;
	int res=0;
	uint8_t cap[YDBI_MAX_AP_DEPTH];
	bool moved=true;
	int direction;

	for(ksize=0;ksize<YDBI_MAX_AP_DEPTH;ksize++){
		if(ap[ksize]==255){break;}
		cap[ksize]=ap[ksize];
	}
	if(ksize==0 || ksize==YDBI_MAX_AP_DEPTH){return -1;}
	cap[ksize-1]+=1;
	range=uc_get_range(dbald, ap, ksize, cap, ksize);
	if(!range){
		UB_LOG(UBL_INFO, "%s:no data to move\n", __func__);
		return 0;
	}
	if(forward_direction){
		direction=UC_DBAL_FORWARD;
	}else{
		direction=UC_DBAL_BACKWARD;
		uc_move_bottom_in_range(dbald, range);
	}
	res=0;
	while(true){
		if(nkey!=NULL){
			yang_db_extract_key_free(aap, akvs, akss);
		}
		if(res!=0){break;}
		if(!moved){
			if(uc_move_in_range(dbald, range, direction)){break;}
		}
		moved=false;
		if(uc_get_keyvalue_in_range(dbald, range, &nkey, &nksize, &value, &vsize,
					    UC_DBAL_NOMOVE)!=0){break;}
		if(yang_db_extract_key(nkey, nksize, &aap, akvs, akss)){
			res=-1;
			break;
		}
		// after this point, next 'yang_db_extract_key_free' must be called.
		// Don't break the loop. set 'res' and continue instead.
		mapsize=0;
		for(i=0;i<YDBI_MAX_AP_DEPTH;i++){
			if(ap[i]==255){
				mapsize=i;
				break;
			}
			if(aap[i]==255){break;}
		}
		if(mapsize==0){
			// a found node key is shorter than the search node key
			continue;
		}
		mkvsize=0;
		for(i=0;i<YDBI_MAX_KV_DEPTH;i++){
			if(kvs[i]==NULL){
				mkvsize=i; // source value key end first, okay
				break;
			}
			if(akvs[i]==NULL){break;} // dest key value end first, ignore this
			if(kss[i]!=akss[i]){break;} // key value size no match, ignore this
		}
		if(mkvsize==0){
			// value keys have a diferent structure, ignore this.
			continue;
		}

		memset(&dbpara, 0, sizeof(dbpara));
		dbpara.onhw=YANG_DB_ONHW_NOACTION;
		dbpara.atype=YANG_DB_ACTION_READ;
		dbpara.aps=aap;
		dbpara.kvs=akvs;
		dbpara.kss=akss;
		dbpara.value=value;
		dbpara.vsize=vsize;
		res=keymod_cb(dbpara.aps, dbpara.kvs, dbpara.kss);
		res=listmove_keymod(dbald, &dbpara, res);
		if(res==-1||res==0){continue;}
		// delete the source item
		if(uc_del_in_range(dbald, range, direction)!=0){
			res=-1;
			break;
		}
		moved=true;
		res=0;
	}
	return res;
}

static int yang_db_put(uc_dbald *dbald, uc_hwald *hwald, yang_db_access_para_t *dbpara)
{
	int i, res=0;
	uint32_t ksize;
	int oldvget=-1;
	uint8_t key[UC_MAX_KEYSIZE];
	void *oldval=NULL;
	uint32_t oldvsize;
	ksize=yang_db_create_key(dbpara->paps, dbpara->aps, dbpara->kvs, dbpara->kss, key);
	if(!ksize){return -1;}
	if(hwald && (dbpara->onhw==YANG_DB_ONHW_ONUPDATE)){
		oldvget=uc_dbal_get(dbald, key, ksize, &oldval, &oldvsize);
	}
	if(hwald && dbpara->onhw){
		if(oldvget!=0){
			res=uc_hwal_reghw(hwald, dbpara->aps, dbpara->kvs, dbpara->kss,
					  dbpara->value, dbpara->vsize);
		}else{
			if((oldvsize!=dbpara->vsize) ||
			   memcmp(oldval, dbpara->value, oldvsize)){
				res=uc_hwal_writehw(hwald, dbpara->aps, dbpara->kvs,
						    dbpara->kss, dbpara->value, dbpara->vsize);
			}
		}
	}
	if(!oldvget){
		(void)uc_dbal_get_release(dbald, key, ksize, oldval, oldvsize);
	}
	if(res!=0){
		UB_LOG(UBL_ERROR, "%s:hardware action failed\n", __func__);
		yang_db_keydump_log(UBL_ERROR, dbpara->aps, dbpara->kvs, dbpara->kss);
		return res;
	}
	res=0;
	// try 2 times in case the other process/task is running 'get'
	for(i=0;i<2;i++){
		switch(dbpara->atype){
		case YANG_DB_ACTION_APPEND:
			res=uc_dbal_append(dbald, key, ksize, dbpara->value,
					   dbpara->vsize);
			break;
		case YANG_DB_ACTION_CREATE:
			res=uc_dbal_create(dbald, key, ksize, dbpara->value,
					   dbpara->vsize);
			break;
		default:
			break;
		}
		if(!res){break;}
		(void)uc_wait_release(dbald, 100);// 100msec should be enough
	}
	if(res!=0){UB_LOG(UBL_ERROR, "%s:error to update the db\n", __func__);}
	return res;
}

static int yang_db_get(uc_dbald *dbald, uc_hwald *hwald, yang_db_access_para_t *dbpara)
{
	int res;
	uint32_t ksize;
	uint8_t key[UC_MAX_KEYSIZE];
	ksize=yang_db_create_key(dbpara->paps, dbpara->aps, dbpara->kvs, dbpara->kss, key);
	if(!ksize){return -1;}
	res=uc_dbal_get(dbald, key, ksize, &dbpara->value, &dbpara->vsize);
	if(res!=0){
		UB_LOG(UBL_DEBUGV, "%s:no data, res=%d\n", __func__, res);
		res=-1;
	}
	return res;
}

static int yang_db_get_release(uc_dbald *dbald, yang_db_access_para_t *dbpara)
{
	int res;
	uint32_t ksize;
	uint8_t key[UC_MAX_KEYSIZE];
	ksize=yang_db_create_key(dbpara->paps, dbpara->aps, dbpara->kvs, dbpara->kss, key);
	if(!ksize){return -1;}
	res=uc_dbal_get_release(dbald, key, ksize, dbpara->value, dbpara->vsize);
	return res;
}

static int yang_db_del(uc_dbald *dbald, uc_hwald *hwald, yang_db_access_para_t *dbpara)
{
	int res=0;
	uint32_t ksize;
	uint8_t key[UC_MAX_KEYSIZE];
	ksize=yang_db_create_key(dbpara->paps, dbpara->aps, dbpara->kvs, dbpara->kss, key);
	if(!ksize){return -1;}
	if(hwald && dbpara->onhw){
		res=uc_hwal_dereghw(hwald, dbpara->aps, dbpara->kvs, dbpara->kss);
	}
	if(uc_dbal_del(dbald, key, ksize)){res=-1;}
	return res;
}

int yang_db_action(uc_dbald *dbald, uc_hwald *hwald, yang_db_access_para_t *dbpara)
{
	//if(!dbald){return -1;} // let it crash to trace the error
	switch(dbpara->atype){
	case YANG_DB_ACTION_CREATE:
	case YANG_DB_ACTION_APPEND:
		return yang_db_put(dbald, hwald, dbpara);
	case YANG_DB_ACTION_READ:
		return yang_db_get(dbald, hwald, dbpara);
	case YANG_DB_ACTION_READ_RELEASE:
		return yang_db_get_release(dbald, dbpara);
	case YANG_DB_ACTION_DELETE:
		return yang_db_del(dbald, hwald, dbpara);
	default:
		break;
	}
	return -1;
}

/* the folloing static variables are used to access the DB sequentially.
 * To be thread-safe, use 'ydbimutex' in the actual accessing functions.
 * dbald, xdd, are always only 1 instance.
 */
static uint8_t aps[YDBI_MAX_AP_DEPTH];
static void *kvs[YDBI_MAX_KV_DEPTH+1];
static uint8_t kss[YDBI_MAX_KV_DEPTH];
static CB_THREAD_MUTEX_T CB_STATIC_MUTEX_INITIALIZER(ydbimutex);
static yang_db_item_access_t ydbi_access={.dbpara={0, YANG_DB_ONHW_NOACTION,
						   NULL, aps, kvs, kss, NULL, 0},
					  .mutex=&ydbimutex};

void ydbi_access_init(uc_dbald *dbald, xl4_data_data_t *xdd, uc_notice_data_t *ucntd)
{
	CB_STATIC_MUTEX_CONSTRUCTOR(ydbimutex);
	(void)CB_THREAD_MUTEX_LOCK(ydbi_access.mutex);
	ydbi_access.dbald=dbald;
	ydbi_access.xdd=xdd;
	ydbi_access.ucntd=ucntd;
	CB_SEM_INIT(&ydbi_access.readrelsem, 0, 0);
	(void)CB_THREAD_MUTEX_UNLOCK(ydbi_access.mutex);
}

yang_db_item_access_t *ydbi_access_handle(void)
{
	return &ydbi_access;
}

int ydbi_get_item_release(yang_db_item_access_t *ydbia, bool keeplocking)
{
	int res=0;
	(void)CB_THREAD_MUTEX_LOCK(ydbi_access.mutex);
	if(ydbia->dbpara.atype!=YANG_DB_ACTION_READ){
		UB_LOG(UBL_ERROR, "%s:it is not soon after READ, prev.atype=%d\n",
		       __func__, ydbia->dbpara.atype);
		res=-1;
		goto erexit;
	}
	ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	if(keeplocking){goto erexit;}
	res=yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
	if(res!=0){
		UB_LOG(UBL_ERROR, "%s:error in yang_db_action\n", __func__);
	}
erexit:
	if(res!=0){
		yang_db_keydump_log(UBL_DEBUG, ydbia->dbpara.aps,
				    ydbia->dbpara.kvs, ydbia->dbpara.kss);
	}
	(void)CB_THREAD_MUTEX_UNLOCK(ydbi_access.mutex);
	CB_SEM_POST(&ydbia->readrelsem);
	return res;
}

#define READREL_TIMEOUT_MS 100 // 100msec should be enough
static int ydbi_action_head(yang_db_item_access_t *ydbia, const char *fname,
			    uint8_t atype)
{
	struct timespec ts;
	uint64_t ts64;
	char *emsg="";
	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	if(ydbia->dbpara.atype==YANG_DB_ACTION_READ){
		while(true){
			// clean up 'ydbia->readrelsem' inside 'ydbia->mutex'
			if(CB_SEM_TRYWAIT(&ydbia->readrelsem)!=0){break;}
		}
		(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
		ts64=ub_rt_gettime64()+READREL_TIMEOUT_MS*UB_MSEC_NS;
		UB_NSEC2TS(ts64, ts);
		if(CB_SEM_TIMEDWAIT(&ydbia->readrelsem, &ts)!=0){
			emsg="last read has not released";
			goto erexit;
		}
		(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
		if(ydbia->dbpara.atype==YANG_DB_ACTION_READ){
			(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
			emsg="still in READ, another read?";
			goto erexit;
		}
	}
	ydbia->dbpara.atype=atype;
	return 0;
erexit:
	UB_LOG(UBL_ERROR, "%s:%s, atype=%d\n", fname, emsg, atype);
	yang_db_keydump_log(UBL_DEBUG, ydbia->dbpara.aps,
			    ydbia->dbpara.kvs, ydbia->dbpara.kss);
	return -1;
}

int ydbi_get_head(yang_db_item_access_t *ydbia, const char *fname)
{
	return ydbi_action_head(ydbia, fname, YANG_DB_ACTION_READ);
}

int ydbi_set_head(yang_db_item_access_t *ydbia, const char *fname)
{
	return ydbi_action_head(ydbia, fname, YANG_DB_ACTION_CREATE);
}

int ydbi_del_head(yang_db_item_access_t *ydbia, const char *fname)
{
	return ydbi_action_head(ydbia, fname, YANG_DB_ACTION_DELETE);
}

int ydbi_get_foot(yang_db_item_access_t *ydbia, const char *fname,
		  void **rval, ub_dbgmsg_level_t emlevel)
{
	int res;
	*rval=NULL;
	ydbia->dbpara.value=NULL;
	ydbia->dbpara.vsize=0;
	res=yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
	if(res==0){
		*rval=ydbia->dbpara.value;
		res=ydbia->dbpara.vsize;
	}else{
		UB_LOG(emlevel, "%s:no data\n", fname);
		yang_db_keydump_log(UBL_DEBUG, ydbia->dbpara.aps,
				    ydbia->dbpara.kvs, ydbia->dbpara.kss);
		ydbia->dbpara.atype=YANG_DB_ACTION_NONE;
	}
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	return res;
}

int ydbi_rel_head(yang_db_item_access_t *ydbia, const char *fname)
{
	(void)CB_THREAD_MUTEX_LOCK(ydbia->mutex);
	ydbia->dbpara.atype=YANG_DB_ACTION_READ_RELEASE;
	return 0;
}

int ydbi_rel_foot(yang_db_item_access_t *ydbia, const char *fname)
{
	int res;
	res=yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	return res;
}

int ydbi_set_foot(yang_db_item_access_t *ydbia, const char *fname,
		  ub_dbgmsg_level_t emlevel, uint8_t notice)
{
	int res;
	res=yang_db_action(ydbia->dbald, NULL, &ydbia->dbpara);
	if(res!=0){
		UB_LOG(emlevel, "%s:can't updte\n", fname);
		ydbia->dbpara.atype=YANG_DB_ACTION_NONE;
	}else if(ydbia->ucntd!=NULL){
		if(notice==YDBI_PUSH_NOTICE){
			res=uc_nc_askaction_push(ydbia->ucntd, ydbia->dbald,
						 ydbia->dbpara.aps,
						 ydbia->dbpara.kvs,
						 ydbia->dbpara.kss);
		}else if(notice==YDBI_PUSH_NOTICE_IN_UNICONF){
			res=uc_nu_askaction_push(ydbia->ucntd, ydbia->dbald,
						 ydbia->dbpara.aps,
						 ydbia->dbpara.kvs,
						 ydbia->dbpara.kss);
		}else{
			res=0;
		}
		if(res<0){
			UB_LOG(emlevel, "%s:failed in uc_nc_askaction_push\n", fname);
		}
	}
	(void)CB_THREAD_MUTEX_UNLOCK(ydbia->mutex);
	return res;
}

int ydbi_request_savedb(yang_db_item_access_t *ydbia)
{
	uint8_t value=1;
	if(ydbi_set_head(ydbia, __func__)!=0){return -1;}
	ydbia->dbpara.onhw=YANG_DB_ONHW_NOACTION;
	ydbia->dbpara.aps[0]=XL4_DATA_RO;
	ydbia->dbpara.aps[1]=UC_READY;
	ydbia->dbpara.aps[2]=255u;
	ydbia->dbpara.kvs[0]=NULL;
	ydbia->dbpara.value=(void*)&value;
	ydbia->dbpara.vsize=sizeof(uint8_t);
	return ydbi_set_foot(ydbia, __func__, UBL_INFO, YDBI_PUSH_NOTICE);
}
