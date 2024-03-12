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
 * avtp_ethernet_private.h
 * Linux Avtp Ethenert layer for AVBTP
 *
 */
#ifndef __AVTP_ETHERNTE_PRIVATE_H_
#define  __AVTP_ETHERNTE_PRIVATE_H_

#define AVTP_SOCK_TEST_MODE -1
#define AVTP_SOCK_NORMAL_MODE 0

typedef struct avtp_traffic_class {
	avtp_traffic_shaper_prams_t tsp;
	uint32_t sent_packets;
	uint64_t last_ts;
	int32_t credit_max; // bit unit: traffic_interval*idleslope/1000
	int32_t credit; // bit unit
	int32_t used_slope; //kbps unit
}avtp_traffic_class_t;

struct avtp_ethernet_handle {
	CB_SOCKET_T sfd;
	CB_SOCKADDR_LL_T socket_address;
	avtp_ethernet_rcv_cb_t rcv_cb;
	void *cbdata;
	int max_datasize;
	ether_frame_t eframe;
	int rec_thread_running;
	CB_THREAD_T rec_thread;
	int pipefd[2];
	int sock_mode; // -1: test mode, 0: normal, (1:lsm, 2:bpf QNX)
	void *readbuf;
	CB_THREAD_MUTEX_T eth_mutex;
	uint32_t speed; // mbps unit
	uint32_t prate; // mbps unit
	int numtc;
	avtp_traffic_class_t tcd[8];
	uint8_t pcp2tc[8];
};

#endif
