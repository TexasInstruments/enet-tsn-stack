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
/*
 * ub_strutils.c
 *
 */
#include "unibase_private.h"
#include <ctype.h>
#include <string.h>

void ub_hexdump(bool console, bool debug, unsigned char *buf, int size, int addr)
{
	int i;
	int p=0;
	int iaddr=addr;
	int isize=size;
	if(!console && !debug) {return;}
	while(isize!=0){
		UB_SELECT_PRINT(console, debug, "%08X - ", iaddr);
		for(i=iaddr&0xf;i>0;i--) {ub_console_debug_select_print(console, debug, "   ");}
		if((iaddr&0xf)>0x8) {ub_console_debug_select_print(console, debug, " ");}
		for(i=iaddr&0xf;i<0x10;i++){
			if(i==0x8) {ub_console_debug_select_print(console, debug, " ");}
			UB_SELECT_PRINT(console, debug, "%02x ", buf[p++]);
			isize--;
			if(!isize) {break;}
		}
		iaddr=(iaddr+0x10)&~0xf;
		ub_console_debug_select_print(console, debug, "\n");
	}
}

void ub_byte2hexstr(uint8_t a, char *astr)
{
	uint8_t d;
	int i;
	for(i=0;i<2;i++){
		if(i==0){
			d=a>>4;
		}else{
			d=a&0xfu;
		}

		if(d>=10u){
			astr[i]=d-10u+(uint8_t)'A';
		}else{
			astr[i]=d+(uint8_t)'0';
		}
	}
}

char *ub_bmac2smac(ub_macaddr_t bmac, char *smac)
{
	int i;
	for(i=0;i<5;i++){
		ub_byte2hexstr(bmac[i], &smac[i*3]);
		smac[(i*3)+2]=':';
	}
	ub_byte2hexstr(bmac[5], &smac[5*3]);
	smac[17]=0;
	return smac;
}

int ub_hexstr2barray(const char *bstr, uint8_t *bytes, const char delim, int len)
{
	int i;
	char *np;
	const char *asp=bstr;
	for(i=0;i<len;i++){
		bytes[i]=strtol(asp, &np, 16);
		if(asp==np) {return i;}
		if(*np!=delim) {return i+1;}
		asp=&np[1];
	}
	return i;
}

uint8_t *ub_smac2bmac(const char *smac, ub_macaddr_t bmac)
{
	int i;
	int j;
	i=ub_hexstr2barray(smac, bmac, ':', 6);
	if(i<6){
		//if smac is less than 6 bytes, fill 0 for lacking parts
		for(j=i;j<6;j++){bmac[j]=0;}
	}
	return bmac;
}

char *ub_bsid2ssid(ub_streamid_t bsid, char *ssid)
{
	int i;
	for(i=0;i<7;i++){
		ub_byte2hexstr(bsid[i], &ssid[i*3]);
		ssid[(i*3)+2]=':';
	}
	ub_byte2hexstr(bsid[7], &ssid[7*3]);
	ssid[23]=0;
	return ssid;
}

uint8_t *ub_ssid2bsid(const char *ssid, ub_streamid_t bsid)
{
	int i;
	int j;
	i=ub_hexstr2barray(ssid, bsid, ':', 8);
	if(i<8){
		//if ssid is less than 8 bytes, fill 0 for lacking parts
		for(j=i;j<8;j++){bsid[j]=0;}
	}
	return bsid;
}

static bool check_delimi(char a, uint8_t base)
{
	if((a>='0') && (a<='9')){return false;}
	if((base==(uint8_t)16) &&
	   (((a>='a') && (a<='f')) || ((a>='A') && (a<='F')))
		){return false;}
	return true;
}

int ub_str2bytearray(uint8_t* dest, const char* input, uint8_t base)
{
	const char *astr;
	int i;
	unsigned long int v;
	char vstr[3];
	int len=0;
	bool gon;
	if(!dest){return 0;}
	if(!input || (input[0]==0)){return 0;}
	for(astr=input;*astr!=0;){
		gon=false;
		for(i=0;i<3;i++){
			if(check_delimi(astr[i], base) || (astr[i]==0)){
				if(i==0){break;}
				vstr[i]=0;
				v=strtoul(vstr, NULL, base);
				if(v>=(unsigned long int)256){break;}
				dest[len]=(uint8_t)v;
				len++;
				if(astr[i]!=0){gon=true;}
				break;
			}
			vstr[i]=astr[i];
		}
		if(!gon){break;}
		astr=&astr[i+1];
	}
	return len;
}

static const char* hex_lookup = "0123456789abcdef";

char* ub_bytearray2str(char * dest, const unsigned char* bytes, int len)
{
	int j = 0;

	if (!dest || !bytes || (len == 0)){return NULL;}

	(void)memset(dest, 0, len * 3);
	// Convert each byte to a 2-character hexadecimal string, with a comma after each byte
	for (int i = 0; i < len; i++)
	{
		dest[j] = hex_lookup[(bytes[i] & (uint8_t)0xF0) >> 4];
		j++;
		dest[j] = hex_lookup[bytes[i] & (uint8_t)0x0F];
		j++;

		if (i < (len - 1)){
			dest[j] = ',';
			j++;
		}
	}

	return dest;
}

static const char *white_spaces=" \t\n\r";
int ub_find_nospace(const char *astr, int maxn)
{
	int c;
	for(c=0;astr[c]!=0;c++){
		if(c==maxn){return -1;}
		if(!strchr(white_spaces, astr[c])){return c;}
	}
	return -1;
}

int ub_find_space(const char *astr, int maxn)
{
	char inq=0;
	int c=0;
	int esc=0;
	for(c=0;astr[c]!=0;c++){
		if(c==maxn){return -1;}
		if(esc!=0){
			esc=0;
			continue;
		}else if(astr[c]=='\\'){
			esc=1;
			continue;
		}else{}

		if(inq!=0){
			if(astr[c]!=inq){continue;}
			inq=0;
			continue;
		}
		if((astr[c]=='"') || (astr[c]=='\'')){
			inq=astr[c];
			continue;
		}
		if(strchr(white_spaces, astr[c])!=NULL) {return c;}
	}
	return -1;
}

int ub_command_line_parser(char *line, char *argv[], int maxargc)
{
	int i=0;
	int c;
	int p=0;
	int linelen;

	if(!line){return 0;}
	linelen=strlen(line);
	while(i<maxargc){
		if(p>=linelen){break;}
		c=ub_find_nospace(&line[p], linelen-p);
		if(c==-1) {
			// strip trailing spaces
			line[p]=0;
			break;
		}
		p+=c;
		argv[i]=&line[p];
		i++;
		c=ub_find_space(&line[p], linelen-p);
		if(c<=0){break;}
		p+=c;
		line[p]=0;
		p+=1;
	}
	// if "...", '...', remove top and bottom of quotation mark
	for(c=0;c<i;c++){
		p=strlen(argv[c]);
		if(((argv[c][0]=='"') && (argv[c][p-1]=='"')) ||
		   ((argv[c][0]=='\'') && (argv[c][p-1]=='\''))){
			argv[c][p-1]=0;
			argv[c]=&argv[c][1];
			continue;
		}
	}
	return i;
}

int ub_strncpy(char *dest, const char *src, int maxlen)
{
	int res;
	res=snprintf(dest, maxlen, "%s", src);
	if(res>=maxlen){
		UB_LOG(UBL_ERROR, "%s:failed, src=%s may be too long\n",
		       __func__, src);
		return -1;
	}
	return 0;
}
