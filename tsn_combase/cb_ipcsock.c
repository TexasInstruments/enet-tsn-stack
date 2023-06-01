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
#include <sys/stat.h>        /* For mode constants */
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <tsn_unibase/ub_getmem.h>
#include "combase_private.h"

typedef struct cb_ipcclient_info{
	struct sockaddr_un addr;
	cb_ipcclient_commode_t commode;
} cb_ipcclient_info_t;


int cb_fdread_timeout(int fd, void *data, int size, int tout_ms)
{
	fd_set rfds;
	struct timeval tv={0,0};
	int res;
	tv.tv_sec=tout_ms/1000;
	tv.tv_usec=(tout_ms%1000)*1000;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	res=select(fd+1, &rfds, NULL, NULL, &tv);
	if(!res){return 0;}
	return read(fd, data, size);
}

int cb_ipcsocket_init(CB_SOCKET_T *ipcfd, char *node, char *suffix, char *server_node)
{
	struct sockaddr_un sockaddress;
	struct sockaddr_un serveraddress;
	int len;

	*ipcfd=CB_SOCKET_INVALID_VALUE;
	if(!node){
		UB_LOG(UBL_ERROR,"'node' must be set\n");
		return -1;
	}
	UB_LOG(UBL_INFO, "%s:combase-"TSNPKGVERSION"\n", __func__);
	*ipcfd=CB_SOCKET(AF_UNIX, SOCK_DGRAM, 0);
	if(!CB_SOCKET_VALID(*ipcfd)){
		UB_LOG(UBL_ERROR,"failed to open ipc socket: %s\n", strerror(errno));
		return -1;
	}

	(void)memset(&sockaddress, 0, sizeof(sockaddress));
	sockaddress.sun_family = AF_UNIX;
	if(!suffix){suffix="";}
	(void)snprintf(sockaddress.sun_path, sizeof(sockaddress.sun_path),
		 "%s%s", node, suffix);

	unlink(sockaddress.sun_path);
	if(CB_SOCK_BIND(*ipcfd, (const struct sockaddr *) &sockaddress,
		sizeof(sockaddress)) < 0) {
		UB_LOG(UBL_ERROR,"Error, bind to %s: %s\n", sockaddress.sun_path, strerror(errno));
		CB_SOCK_CLOSE(*ipcfd);
		*ipcfd=CB_SOCKET_INVALID_VALUE;
		return -1;
	}
	chmod(sockaddress.sun_path, ((mode_t)7<<6)|((mode_t)7<<3)|((mode_t)7));
	if(!server_node){return 0;}

	serveraddress.sun_family = AF_UNIX;
	if(ub_strncpy(serveraddress.sun_path, server_node, sizeof(serveraddress.sun_path))!=0){
		return -1;
	}
	len=sizeof(serveraddress.sun_family)+sizeof(serveraddress.sun_path);
	if(connect(*ipcfd, (const struct sockaddr *)&serveraddress, len)<0){
		UB_LOG(UBL_ERROR,"Error, connect to %s : %s\n", server_node, strerror(errno));
		CB_SOCK_CLOSE(*ipcfd);
		*ipcfd=CB_SOCKET_INVALID_VALUE;
		return -1;
	}
	return 0;
}

int cb_ipcsocket_udp_init(CB_SOCKET_T *ipcfd, char *own_ip, char *server_ip, int server_port)
{
	struct sockaddr_in sockaddress;
	int len;

	UB_LOG(UBL_INFO, "%s:combase-"TSNPKGVERSION"\n", __func__);
	*ipcfd=CB_SOCKET(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(!CB_SOCKET_VALID(*ipcfd)){
		UB_LOG(UBL_ERROR,"failed to open ipc socket: %s\n", strerror(errno));
		return -1;
	}
	(void)memset(&sockaddress, 0, sizeof(sockaddress));
	sockaddress.sin_family = AF_INET;
	if(server_ip==NULL){
		sockaddress.sin_port = htons(server_port);
	}else{
		sockaddress.sin_port = 0;
	}
	len=sizeof(sockaddress);

	if(own_ip!=NULL){
		sockaddress.sin_addr.s_addr = inet_addr(own_ip);
	}else{
		sockaddress.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	if(CB_SOCK_BIND(*ipcfd, (const struct sockaddr *) &sockaddress,
		       sizeof(sockaddress)) < 0) {
		UB_LOG(UBL_ERROR,"Error, bind:%s\n", strerror(errno));
		CB_SOCK_CLOSE(*ipcfd);
		*ipcfd=CB_SOCKET_INVALID_VALUE;
		return -1;
	}
	if(!server_ip){return 0;}

	// only for client connetion
	sockaddress.sin_addr.s_addr = inet_addr(server_ip);
	sockaddress.sin_port = htons(server_port);
	if(connect(*ipcfd, (const struct sockaddr *)&sockaddress, len)<0){
		UB_LOG(UBL_ERROR,"Error, connect to %s:%d, %s\n",
		       server_ip, server_port, strerror(errno));
		CB_SOCK_CLOSE(*ipcfd);
		*ipcfd=CB_SOCKET_INVALID_VALUE;
		return -1;
	}
	return 0;
}

int cb_ipcsocket_close(CB_SOCKET_T ipcfd, char *node, char *suffix)
{
	char nodename[128];
	int res=0;
	if(CB_SOCKET_VALID(ipcfd)) {
		if(CB_SOCK_CLOSE(ipcfd)!=0){
			UB_LOG(UBL_ERROR, "%s:can't close ipcfd, %s\n",__func__,
			       strerror(errno));
			res=-1;
		}
	}
	if(!node){return res;}
	if(!suffix){suffix="";}
	(void)snprintf(nodename, sizeof(nodename), "%s%s", node, suffix);
	if(unlink(nodename)!=0){
		UB_LOG(UBL_ERROR, "%s:can't unlink %s, %s\n",__func__, nodename, strerror(errno));
		res=-1;
	}
	return res;
}

#define MAX_NUM_NODECHAR 64
#define MAX_NUM_NODESUFFIX 16
struct cb_ipcserverd {
	uint16_t udpport;
	int fd;
	char node[MAX_NUM_NODECHAR];
	char suffix[MAX_NUM_NODESUFFIX];
	ub_esarray_cstd_t *ipc_address;
};

/*
 * static memory allocation case('UB_SD_STATIC' is defined)
 * the memory is statically allocated by the following definitions.
 * the numbers defined here should be overridden by your needs.
 * when 'UB_SD_STATIC' is not defined, the number doesn't have meaning.
 */
#define CB_IPCSOCKSV_INSTMEM cb_ipcsocksv_inst
#ifndef CB_IPCSOCKSV_INSTNUM
#define CB_IPCSOCKSV_INSTNUM 1
#endif

UB_SD_GETMEM_DEF(CB_IPCSOCKSV_INSTMEM, (int)sizeof(struct cb_ipcserverd),
		 CB_IPCSOCKSV_INSTNUM);

static int find_ipc_client_in(cb_ipcserverd_t *ipcsd, struct sockaddr_in *client_address)
{
	int i;
	int en=ub_esarray_ele_nums(ipcsd->ipc_address);
	for(i=0;i<en;i++){
		cb_ipcclient_info_t *clinfo=
			(cb_ipcclient_info_t *)ub_esarray_get_ele(ipcsd->ipc_address, i);
		struct sockaddr_in *addr=(struct sockaddr_in *)&clinfo->addr;
		if((addr->sin_addr.s_addr == client_address->sin_addr.s_addr) &&
		   (addr->sin_port == client_address->sin_port)){
			// client_address is already registered
			return i;
		}
	}
	return -1;
}

static int find_ipc_client_un(cb_ipcserverd_t *ipcsd, struct sockaddr_un *client_address)
{
	int i;
	int en=ub_esarray_ele_nums(ipcsd->ipc_address);
	for(i=0;i<en;i++){
		cb_ipcclient_info_t *clinfo=
			(cb_ipcclient_info_t *)ub_esarray_get_ele(ipcsd->ipc_address, i);
		struct sockaddr_un *addr=(struct sockaddr_un *)&clinfo->addr;
		if(!strcmp(addr->sun_path, client_address->sun_path)){
			// client_address is already registered
			return i;
		}
	}
	return -1;
}

static int find_ipc_client(cb_ipcserverd_t *ipcsd, struct sockaddr *client_address)
{
	if(client_address->sa_family==AF_UNIX){
		return find_ipc_client_un(ipcsd, (struct sockaddr_un *)client_address);
	}else if(client_address->sa_family==AF_INET){
		return find_ipc_client_in(ipcsd, (struct sockaddr_in *)client_address);
	}else{}
	UB_LOG(UBL_ERROR,"%s:wrong IPC protocol\n",__func__);
	return -1;
}

static int register_ipc_client(cb_ipcserverd_t *ipcsd, struct sockaddr *client_address)
{
	int i;
	cb_ipcclient_info_t *clinfo;
	cb_ipcclient_info_t pclinfo;

	i=find_ipc_client(ipcsd, client_address);
	if(i>=0){return i;}
	// As sizeof(struct sockaddr_un) > sizeof(struct sockaddr_in),
	// we use 'sockaddr_un' even for 'sockaddr_in'
	clinfo=(cb_ipcclient_info_t *)ub_esarray_get_newele(ipcsd->ipc_address);
	if(clinfo!=NULL){
		(void)memset(clinfo, 0, sizeof(cb_ipcclient_info_t));
		memcpy(&clinfo->addr, client_address, sizeof(struct sockaddr_un));
		return ub_esarray_ele_nums(ipcsd->ipc_address)-1;
	}
	UB_LOG(UBL_WARN,"%s:ipc clients space is full, remove the oldest connection\n",__func__);
	(void)ub_esarray_pop_ele(ipcsd->ipc_address, (ub_esarray_element_t *)&pclinfo);
	clinfo=(cb_ipcclient_info_t *)ub_esarray_get_newele(ipcsd->ipc_address);
	if(!clinfo) {
		UB_LOG(UBL_ERROR,"%s:shouldn't happen\n",__func__);
		return -1;
	}
	(void)memset(clinfo, 0, sizeof(cb_ipcclient_info_t));
	memcpy(&clinfo->addr, client_address, sizeof(struct sockaddr_un));
	return ub_esarray_ele_nums(ipcsd->ipc_address)-1;
}

static int one_client_write(cb_ipcserverd_t *ipcsd, uint8_t *data, int size,
			    struct sockaddr *client_address)
{
	int ssize;
	int res;
	if(!client_address){return -1;}
	if(ipcsd->udpport!=0u){
		ssize=sizeof(struct sockaddr_in);
	}else{
		ssize=sizeof(struct sockaddr_un);
	}
	res=sendto(ipcsd->fd, data, size, 0, client_address, ssize);
	if(res==size){return 0;}
	if(ipcsd->udpport!=0u){
		struct sockaddr_in *addr=(struct sockaddr_in *)client_address;
		UB_LOG(UBL_WARN, "%s:can't send IPC data to %s port=%d\n",
		       __func__, inet_ntoa(addr->sin_addr),
		       ntohs(addr->sin_port));
	}else{
		struct sockaddr_un *addr=(struct sockaddr_un *)client_address;
		UB_LOG(UBL_WARN, "%s:can't send IPC data to %s\n",
		       __func__, addr->sun_path);
	}
	return -1;
}

int cb_ipcsocket_remove_client(cb_ipcserverd_t *ipcsd, struct sockaddr *client_address)
{
	return ub_esarray_del_index(ipcsd->ipc_address, find_ipc_client(ipcsd, client_address));
}

int cb_ipcsocket_set_commode(cb_ipcserverd_t *ipcsd, struct sockaddr *client_address,
			     cb_ipcclient_commode_t commode)
{
	int i;
	cb_ipcclient_info_t *clinfo;
	i=find_ipc_client(ipcsd, client_address);
	if(i<0){
		UB_LOG(UBL_ERROR, "%s:client doesn't exist\n", __func__);
		return -1;
	}
	clinfo=(cb_ipcclient_info_t *)ub_esarray_get_ele(ipcsd->ipc_address, i);
	clinfo->commode=commode;
	return 0;
}

cb_ipcclient_commode_t cb_ipcsocket_get_commode(cb_ipcserverd_t *ipcsd,
						struct sockaddr *client_address)
{
	int i;
	cb_ipcclient_info_t *clinfo;
	i=find_ipc_client(ipcsd, client_address);
	if(i<0){
		UB_LOG(UBL_ERROR, "%s:client doesn't exist\n", __func__);
		return (cb_ipcclient_commode_t)-1;
	}
	clinfo=(cb_ipcclient_info_t *)ub_esarray_get_ele(ipcsd->ipc_address, i);
	return clinfo->commode;
}

int cb_ipcsocket_server_read(cb_ipcserverd_t *ipcsd, cb_ipcsocket_server_rdcb ipccb, void *cbdata)
{
	int32_t res;
	socklen_t address_length;
	struct sockaddr_un client_address;
	uint8_t rbuf[1501]; // reserve 1 byte to add a null terminator in textmode
	int ri;

	(void)memset(&client_address, 0, sizeof(struct sockaddr_un));
	address_length=sizeof(struct sockaddr_un);
	res=recvfrom(ipcsd->fd, rbuf, 1500, 0,
		     (struct sockaddr *)&(client_address), &address_length);
	if(res>0){
		ri=register_ipc_client(ipcsd, (struct sockaddr *)&client_address);
		if(ri<0){return -1;}
		if(ipccb && ipccb(cbdata, rbuf, res, (struct sockaddr *)&client_address)){
			(void)cb_ipcsocket_remove_client(ipcsd, (struct sockaddr *)&client_address);
		}
		return 0;
	}

	if(res==0){
		UB_LOG(UBL_WARN, "%s:read returns 0, must be disconnected\n", __func__);
	}else{
		if(errno==ECONNREFUSED){
			UB_LOG(UBL_WARN, "%s:the other side may not be listening\n", __func__);
		}else{
			UB_LOG(UBL_INFO, "%s:res=%d,%s, close fd\n", __func__,
			       res, strerror(errno));
		}
	}
	(void)cb_ipcsocket_remove_client(ipcsd, (struct sockaddr *)&client_address);
	return -1;
}

int cb_ipcsocket_server_write(cb_ipcserverd_t *ipcsd, uint8_t *data, int size,
			      struct sockaddr *client_address)
{
	int en, i;
	if(client_address!=NULL){
		return one_client_write(ipcsd, data, size, client_address);
	}

	en=ub_esarray_ele_nums(ipcsd->ipc_address);
	for(i=0;i<en;i++){
		cb_ipcclient_info_t *clinfo=
			(cb_ipcclient_info_t *)ub_esarray_get_ele(ipcsd->ipc_address, i);
		if(!clinfo){break;}
		client_address=(struct sockaddr *)&clinfo->addr;
		if(one_client_write(ipcsd, data, size, client_address)!=0){
			// decrement 'esarray index' when the item is removed
			if(!cb_ipcsocket_remove_client(ipcsd, client_address)){i--;}
		}
	}
	return 0;
}

int cb_ipcsocket_server_write_ddata(cb_ipcserverd_t *ipcsd, void *cbdata,
				    cb_ipcsocket_server_ddatacb ddatacb)
{
	int en, i;
	uint8_t *data=NULL;
	int size=0;
	struct sockaddr *client_address;

	en=ub_esarray_ele_nums(ipcsd->ipc_address);
	for(i=0;i<en;i++){
		cb_ipcclient_info_t *clinfo=
			(cb_ipcclient_info_t *)ub_esarray_get_ele(ipcsd->ipc_address, i);
		client_address=(struct sockaddr *)&clinfo->addr;
		if(ddatacb(cbdata, &data, &size, client_address)!=0){continue;}
		if(one_client_write(ipcsd, data, size, client_address)!=0){
			// decrement 'esarray index' when the item is removed
			if(!cb_ipcsocket_remove_client(ipcsd, client_address)){i--;}
		}
		if(data!=NULL){free(data);}
		data=NULL;
	}
	return 0;
}

cb_ipcserverd_t *cb_ipcsocket_server_init(char *node_ip, char *suffix, uint16_t port)
{
	cb_ipcserverd_t *ipcsd;
	ipcsd=UB_SD_GETMEM(CB_IPCSOCKSV_INSTMEM, sizeof(cb_ipcserverd_t));
	if(ub_assert_fatal(ipcsd!=NULL, __func__, "malloc")){return NULL;}
	(void)memset(ipcsd, 0, sizeof(cb_ipcserverd_t));
	if(port!=0u){
		if(cb_ipcsocket_udp_init(&ipcsd->fd, node_ip, NULL, port)!=0){goto erexit;}
	}else{
		if(cb_ipcsocket_init(&ipcsd->fd, node_ip, suffix, NULL)!=0){goto erexit;}
		if(ub_strncpy(ipcsd->node, node_ip, MAX_NUM_NODECHAR)!=0){goto erexit;}
		if(ub_strncpy(ipcsd->suffix, suffix, MAX_NUM_NODESUFFIX)!=0){goto erexit;}
	}
	ipcsd->udpport=port;
	ipcsd->ipc_address=ub_esarray_init(2, sizeof(cb_ipcclient_info_t), MAX_IPC_CLIENTS);
	return ipcsd;
erexit:
	UB_SD_RELMEM(CB_IPCSOCKSV_INSTMEM, ipcsd);
	return NULL;
}

void cb_ipcsocket_server_close(cb_ipcserverd_t *ipcsd)
{
	if(!ipcsd){
		UB_LOG(UBL_WARN, "%s:ipcsd==NULL\n", __func__);
		return;
	}
	(void)cb_ipcsocket_close(ipcsd->fd, ipcsd->node, ipcsd->suffix);
	ub_esarray_close(ipcsd->ipc_address);
	UB_SD_RELMEM(CB_IPCSOCKSV_INSTMEM, ipcsd);
}

CB_SOCKET_T cb_ipcsocket_getfd(cb_ipcserverd_t *ipcsd)
{
	return ipcsd->fd;
}
