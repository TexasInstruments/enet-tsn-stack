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
#include "../lldp_utils.h"
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdarg.h>

/*Ultility*/
void get_dest_mac_addr(const char* buf, int* pos, ub_macaddr_t mac)
{
	memcpy(mac, &buf[*pos], sizeof(ub_macaddr_t));
	*pos += sizeof(ub_macaddr_t); // shift the index of buffer
}
void get_src_mac_addr(const char* buf, int* pos, ub_macaddr_t mac)
{
	memcpy(mac, &buf[*pos], sizeof(ub_macaddr_t));
	*pos += sizeof(ub_macaddr_t); // shift the index of buffer
}
uint16_t get_eth_proto(const char* buf, int* pos)
{
	uint16_t protocol = 0x0000;
	protocol = (uint16_t)buf[*pos + 1] & 0x00FF;
	protocol |= ((buf[*pos] << 8) & 0xFF00);
	 *pos += sizeof(uint16_t); // shift the index of buffer
	return protocol;
}

uint32_t get_current_time_in_milisec()
{
	uint32_t time_in_ms = 0;
	uint64_t time_in_nsec =  ub_mt_gettime64();
	time_in_ms = (uint32_t)(time_in_nsec / 1000000);
	return time_in_ms;
}

uint64_t get_current_time_in_usec()
{
	uint64_t time_in_us = 0;
	uint64_t time_in_nsec =  ub_mt_gettime64();
	time_in_us =  time_in_nsec / 1000;
	return time_in_us;
}

void ipv4_str_to_hex(const char* ipv4str, uint8_t* buf)
{
	int ip_add = inet_addr(ipv4str);
	buf[0] = ((uint8_t*)&ip_add)[0];
	buf[1] = ((uint8_t*)&ip_add)[1];
	buf[2] = ((uint8_t*)&ip_add)[2];
	buf[3] = ((uint8_t*)&ip_add)[3];
}

void ipv6_str_to_hex(const char* ipv6str, uint8_t* buf)
{
	struct in6_addr result;
	if (inet_pton(AF_INET6, ipv6str, &result) == 1) // success!
	{
		//successfully parsed string into "result"
		memcpy(buf, result.__in6_u.__u6_addr8, sizeof(result.__in6_u.__u6_addr8));
	}
}

void str_to_ipv4(uint8_t* buf, char* ipv4_str)
{
	sprintf(ipv4_str, "%d.%d.%d.%d", buf[0], buf[1], buf[2], buf[3]);
}

void split_str_into_arr(char* buf, char* delim, char** out, int expected_no)
{
	int counter = 0;
	char *p = strtok (buf, delim);
	while(p!= NULL && counter < expected_no)
	{
		out[counter++] = p;
        p = strtok (NULL, delim);
	}
}

char *lldp_bmac2smac(ub_macaddr_t bmac, char *smac, const char delim)
{
	int i;
	for(i=0;i<5;i++){
		ub_byte2hexstr(bmac[i], &smac[i*3]);
		smac[(i*3)+2]=delim;
	}
	ub_byte2hexstr(bmac[5], &smac[5*3]);
	smac[17]=0;
	return smac;
}