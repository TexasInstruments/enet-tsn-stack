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
#include "combase_private.h"
#include "combase_link.h"

static int ovip_socket_open(CB_SOCKET_T *sfd, CB_SOCKADDR_IN_T *saddr,
			    cb_rawsock_ovip_para_t *ovipp)
{
	int optval;

	*sfd = CB_SOCKET(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (!CB_SOCKET_VALID(*sfd)){
		UB_LOG(UBL_ERROR,"%s: %s\n",__func__, strerror(errno));
		return -1;
	}
	(void)memset(saddr, 0, sizeof(CB_SOCKADDR_IN_T));
	saddr->sin_addr.s_addr = htonl(ovipp->laddr);
	saddr->sin_family = AF_INET;
	saddr->sin_port = htons(ovipp->lport);
	optval=1;
	CB_SETSOCKOPT(*sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

        if (CB_SOCK_BIND(*sfd, (struct sockaddr *)saddr, sizeof(CB_SOCKADDR_IN_T)) < 0) {
		UB_LOG(UBL_ERROR,"%s: %s, port=%d\n",__func__, strerror(errno), ovipp->lport);
		CB_SOCK_CLOSE(*sfd);
		return -1;
        }

	if(ovipp->daddr!=0u){
		(void)memset(saddr, 0, sizeof(CB_SOCKADDR_IN_T));
		saddr->sin_addr.s_addr = htonl(ovipp->daddr);
		saddr->sin_family = AF_INET;
		saddr->sin_port = htons(ovipp->dport);
		connect(*sfd, (struct sockaddr *)saddr, sizeof(CB_SOCKADDR_IN_T));
	}
	return 0;
}

int cb_rawsock_open(cb_rawsock_paras_t *llrawp, CB_SOCKET_T *fd, CB_SOCKADDR_LL_T *addr,
		    int *mtusize, ub_macaddr_t bmac)
{
	int ifindex;
	char mstr[20];

	UB_LOG(UBL_INFO, "%s:combase-"TSNPKGVERSION"\n", __func__);
	*fd = CB_SOCKET_INVALID_VALUE;
	if(llrawp->sock_mode==CB_SOCK_MODE_OVIP){
		if(!llrawp->ovipp) {
			UB_LOG(UBL_ERROR,"%s:ovipp parameters don't exist\n",__func__);
			return -1;
		}
		// sizeof(CB_SOCKADDR_LL_T) >= sizeof(CB_SOCKADDR_IN_T) must be true
		if(ovip_socket_open(fd, (CB_SOCKADDR_IN_T *)addr,  llrawp->ovipp)!=0) {
			return -1;
		}
	}else{
		*fd = CB_SOCKET(AF_PACKET, SOCK_RAW, htons(llrawp->proto));
		if (!CB_SOCKET_VALID(*fd)){
			UB_LOG(UBL_ERROR,"%s:socket error, %s\n",__func__, strerror(errno));
			return -1;
		}
	}

	if(cb_get_mac_bydev(*fd, llrawp->dev, bmac)!=0){goto erexit;}
	(void)sprintf(mstr, "%02X:%02X:%02X:%02X:%02X:%02X",
		bmac[0],bmac[1],bmac[2],bmac[3],bmac[4],bmac[5]);
	UB_LOG(UBL_INFO,"set new source address: %s\n",mstr);

	if(llrawp->sock_mode==CB_SOCK_MODE_OVIP){
		if(mtusize!=NULL){*mtusize=1500-20;} //IP+UDP heaer=20bytes
		return 0;
	}else{
		ifindex = (int)CB_IF_NAMETOINDEX(llrawp->dev);
		if(ifindex<0){goto erexit;}
		addr->sll_family = PF_PACKET;
		addr->sll_protocol = htons(llrawp->proto);
		addr->sll_ifindex = ifindex;
		addr->sll_hatype = ARPHRD_ETHER;
		addr->sll_pkttype = PACKET_OTHERHOST;
		addr->sll_halen = ETH_ALEN;
		memcpy(addr->sll_addr, bmac, ETH_ALEN);

		if(CB_SOCK_BIND(*fd, (CB_SOCKADDR_T*)addr, sizeof(*addr)) < 0){goto erexit;}
		if(llrawp->priority!=0){
			(void)cb_sock_set_priority(*fd, llrawp->priority);
		}
	}

	if(!mtusize){return 0;}
	if(!cb_expand_mtusize(*fd, llrawp->dev, mtusize)){return 0;}
erexit:
	UB_LOG(UBL_ERROR,"%s:dev=%s %s\n",__func__, llrawp->dev, strerror(errno));
	if(CB_SOCKET_VALID(*fd)){
		CB_SOCK_CLOSE(*fd);
		*fd = CB_SOCKET_INVALID_VALUE;
	}
	return -1;
}

int cb_rawsock_close(CB_SOCKET_T fd)
{
	return CB_SOCK_CLOSE(fd);
}

#ifdef CB_IFREQ_T
int cb_sock_set_priority(CB_SOCKET_T fd, int priority)
{
	if(CB_SETSOCKOPT(fd, SOL_SOCKET, SO_PRIORITY,
			 &priority, sizeof(priority))!=0){
		UB_LOG(UBL_WARN, "%s:failed SO_PRIORITY, %s\n",
		       __func__, strerror(errno));
		return -1;
	}
	return 0;
}

int cb_expand_mtusize(CB_SOCKET_T fd, const char *dev, int *mtusize)
{
	CB_IFREQ_T ifr;
	/* expand the size of MTU if needed, normally MTU size = 1500.
	   if it is not possible, keep the same MTU size */
	(void)snprintf(ifr.ifr_name, CB_MAX_NETDEVNAME, "%s", dev);
	if(CB_SOCK_IOCTL(fd, SIOCGIFMTU, &ifr)<0){return -1;}
	*mtusize -= (int)sizeof(CB_ETHHDR_T) + 4;
	if(ifr.ifr_mtu<*mtusize){
		ifr.ifr_mtu=*mtusize;
		if (CB_SOCK_IOCTL(fd, SIOCSIFMTU, &ifr) < 0){
			UB_LOG(UBL_INFO,"%s: MTU size can't be expandable\n",__func__);
		}
		if(CB_SOCK_IOCTL(fd, SIOCGIFMTU, &ifr) < 0){return -1;}
	}
	/* ifr.ifr_mtu doesn't include eth header size.
	   sizeof(CB_ETHHDR_T) + VLAN_AF_SIZE = 18 can be normally added */
	*mtusize = ifr.ifr_mtu + sizeof(CB_ETHHDR_T) + 4;
	return 0;
}

int cb_set_promiscuous_mode(CB_SOCKET_T sfd, const char *dev, bool enable)
{
	CB_IFREQ_T ifr;
	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		// ignore it
		return 0;
	}
	(void)snprintf(ifr.ifr_name, CB_MAX_NETDEVNAME, "%s", dev);
	if(CB_SOCK_IOCTL(sfd, SIOCGIFFLAGS, &ifr)<0){
		UB_LOG(UBL_ERROR,"%s:error CB_SOCK_IOCTL SIOCGIFFLAGS, %s\n",
		       __func__, strerror(errno));
		return -1;
	}
	if(enable){
		ifr.ifr_flags |= IFF_PROMISC;
	}else{
		ifr.ifr_flags &= ~IFF_PROMISC;
	}
	if(CB_SOCK_IOCTL(sfd, SIOCSIFFLAGS, &ifr)<0){
		UB_LOG(UBL_ERROR,"%s:error CB_SOCK_IOCTL SIOCSIFFLAGS, %s\n",
		       __func__, strerror(errno));
		return -1;
	}
	if(enable){
		UB_LOG(UBL_INFO,"%s:%s set promiscuouse mode\n",__func__, dev);
	}else{
		UB_LOG(UBL_INFO,"%s:%s reset promiscuouse mode\n",__func__, dev);
	}
	return 0;
}

static int ifrqd_ioctr(CB_SOCKET_T sfd, const char *dev, CB_IFREQ_T *ifrqd, uint32_t iocreq)
{
	int fd=sfd;
	if (fd==CB_SOCKET_INVALID_VALUE){
		/* open a udp socket to get it */
		fd = CB_SOCKET(AF_INET,SOCK_DGRAM,0);
		if (!CB_SOCKET_VALID(fd)){
			UB_LOG(UBL_ERROR,"%s: %s\n",__func__, strerror(errno));
			return -1;
		}
	}
	(void)snprintf(ifrqd->ifr_name, IF_NAMESIZE, "%s", dev);
	if(CB_SOCK_IOCTL(fd, iocreq, ifrqd) == -1){
		UB_LOG(UBL_ERROR,"%s: %s, dev=%s\n",__func__, strerror(errno),dev);
		return -1;
	}
	if(sfd==-1){CB_SOCK_CLOSE(fd);} /* close if we opened here */
	return 0;
}

int cb_get_mac_bydev(CB_SOCKET_T sfd, const char *dev, ub_macaddr_t bmac)
{
	CB_IFREQ_T ifrqd;

	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		int di=strlen(CB_VIRTUAL_ETHDEV_PREFIX);
		uint32_t s=0;
		uint8_t bm[4]=CB_VIRTUAL_ETHDEV_MACU32;
		while(dev[di]!=0){
			s=s+(uint32_t)dev[di];
			di++;
		}
		memcpy(bmac, bm, 4);
		bmac[4]=s>>8;
		bmac[5]=s&0xffu;
		return 0;
	}
	(void)memset(&ifrqd, 0, sizeof(CB_IFREQ_T));
	if(ifrqd_ioctr(sfd, dev, &ifrqd, SIOCGIFHWADDR)!=0){return -1;}
	memcpy(bmac,ifrqd.ifr_hwaddr.sa_data,6);
	return 0;
}

int cb_get_ip_bydev(CB_SOCKET_T sfd, const char *dev, CB_IN_ADDR_T *inp)
{
	CB_IFREQ_T ifrqd;

	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		inp->s_addr=htonl(0x7f000001);
		return 0;
	}
	(void)memset(&ifrqd, 0, sizeof(CB_IFREQ_T));
	if(ifrqd_ioctr(sfd, dev, &ifrqd, SIOCGIFADDR)!=0){return -1;}
	memcpy(inp, &(((CB_SOCKADDR_IN_T *)&ifrqd.ifr_addr)->sin_addr),
	       sizeof(CB_IN_ADDR_T));
	return 0;
}

int cb_get_brdip_bydev(CB_SOCKET_T sfd, const char *dev, CB_IN_ADDR_T *inp)
{
	CB_IFREQ_T ifrqd;

	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		inp->s_addr=htonl(0x7fffffff);
		return 0;
	}
	(void)memset(&ifrqd, 0, sizeof(CB_IFREQ_T));
	if(ifrqd_ioctr(sfd, dev, &ifrqd, SIOCGIFBRDADDR)!=0){return -1;}
	memcpy(inp, &(((CB_SOCKADDR_IN_T *)&ifrqd.ifr_broadaddr)->sin_addr),
	       sizeof(CB_IN_ADDR_T));
	return 0;
}

int cb_reg_multicast_address(CB_SOCKET_T fd, const char *dev,
			     const unsigned char *mcastmac, int del)
{
	CB_IFREQ_T ifr;
	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		// ignore it
		return 0;
	}
	/* add multicast address */
	(void)snprintf(ifr.ifr_name, CB_MAX_NETDEVNAME, "%s", dev);
	memcpy(ifr.ifr_hwaddr.sa_data, mcastmac, ETH_ALEN);
	ifr.ifr_hwaddr.sa_family = AF_UNSPEC;
	if(del!=0){
		if (CB_SOCK_IOCTL(fd, SIOCDELMULTI, &ifr) == -1){return -1;}
	}else{
		if (CB_SOCK_IOCTL(fd, SIOCADDMULTI, &ifr) == -1){return -1;}
	}
	return 0;
}
#endif

#ifndef CB_NO_GETIFADDRS
int cb_get_all_netdevs(int maxdevnum, netdevname_t *netdevs)
{
	struct ifaddrs *ifa;
	struct ifaddrs *ifad;
	int i=0;

	if(getifaddrs(&ifa)!=0){
		UB_LOG(UBL_ERROR,"%s:failed in getifaddrs: %s\n", __func__, strerror(errno));
		return 0;
	}
	for(ifad=ifa;ifad && (i<maxdevnum);ifad=ifad->ifa_next){
		if(!ifad->ifa_addr || (ifad->ifa_addr->sa_family != AF_INET)){continue;}
		// In QNX, local name is lo0
		if(!strncmp(ifad->ifa_name, "lo", 2)){continue;}
		(void)strcpy(netdevs[i], ifad->ifa_name);
		UB_LOG(UBL_DEBUG, "%s:found netdev=%s\n",__func__, netdevs[i]);
		i++;
	}
	if(ifa!=NULL){freeifaddrs(ifa);}
	return i;
}
#endif

#ifdef LINUX_ETHTOOL
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <dirent.h>

int cb_get_ethtool_linkstate(CB_SOCKET_T cfd, const char *dev, uint32_t *linkstate)
{
	struct ifreq ifr;
	struct ethtool_value ecmd;

	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		*linkstate=1;
		return 0;
	}
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)memset(&ecmd, 0, sizeof(ecmd));
	ecmd.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (void *)&ecmd;
	if(ifrqd_ioctr(cfd, dev, &ifr, SIOCETHTOOL)!=0){return -1;}
	if(ecmd.data!=0){
		*linkstate=1;
	}else{
		*linkstate=0;
	}

	return 0;
}


int cb_get_ethtool_info(CB_SOCKET_T cfd, const char *dev, uint32_t *speed, uint32_t *duplex)
{
	struct ifreq ifr;
	struct ethtool_cmd ecmd;

	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		*duplex=CB_DUPLEX_FULL;
		*speed=1000;
		return 0;
	}
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)memset(&ecmd, 0, sizeof(ecmd));
	ecmd.cmd = ETHTOOL_GSET;
	ifr.ifr_data = (void *)&ecmd;
	if(ifrqd_ioctr(cfd, dev, &ifr, SIOCETHTOOL)!=0){return -1;}
	*speed = (ecmd.speed_hi << 16) | ecmd.speed;
	if ((*speed == 0) || (*speed == (uint16_t)(-1)) || (*speed == (uint32_t)(-1))) {
		*speed=0; // unknown speed
	}
	switch(ecmd.duplex){
	case DUPLEX_FULL:
		*duplex=CB_DUPLEX_FULL;
		break;
	case DUPLEX_HALF:
		*duplex=CB_DUPLEX_HALF;
		break;
	default: //duplex unknown
		*duplex=CB_DUPLEX_UNKNOWN;
		break;
	}
	return 0;
}

static int get_first_dir_name(char *pname, char *dirname, int dirname_size)
{
	DIR *ndir;
	struct dirent *dir;
	int res=-1;
	int sl;

	ndir=opendir(pname);
	if(!ndir){return -1;}
	while (true) {
		dir = readdir(ndir);
		if(dir==NULL){break;}
		if(dir->d_type != DT_DIR){continue;}
		if(!strcmp(dir->d_name,".") || !strcmp(dir->d_name,"..")){continue;}
		sl=(int)strlen(dir->d_name)+1;
		sl=UB_MIN(sl,dirname_size);
		if (sl<=0){break;}
		memcpy(dirname, dir->d_name, sl);
		dirname[sl-1]=0;
		res=0;
		break;
	}
	closedir(ndir);
	return res;
}

static int phy_get_ptpdev_from_netdev(char *ptpdev, int ptpdev_size, char *netdev)
{
	char pname[128];
	(void)snprintf(pname, sizeof(pname), "/sys/class/net/%s/phydev/ptp", netdev);
	return get_first_dir_name(pname, ptpdev, ptpdev_size);
}

/* PHY does not have a way to directly map ptpdev to netdev.
 * We need to loop to all the available netdev and find one that ptpdev matched */
static int phy_get_netdev_from_ptpdev(char *ptpdev, char *netdev)
{
	char pname[128];
	DIR *ndir;
	struct dirent *dir;
	int res=-1;
	char ptptmp[MAX_PTPDEV_NAME];
	char nettmp[CB_MAX_NETDEVNAME];

	(void)snprintf(pname, sizeof(pname), "/sys/class/net");

	ndir=opendir(pname);
	if(!ndir){return -1;}
	while (true) {
		dir = readdir(ndir);
		if(dir==NULL){break;}
		if((dir->d_type != DT_DIR) && (dir->d_type != DT_LNK)){continue;}
		if(!strcmp(dir->d_name,".") || !strcmp(dir->d_name,"..")){continue;}
		memset(ptptmp, 0, sizeof(ptptmp));
		strncpy(nettmp, dir->d_name, CB_MAX_NETDEVNAME-1);
		nettmp[CB_MAX_NETDEVNAME-1] = 0;
		if(phy_get_ptpdev_from_netdev(ptptmp, MAX_PTPDEV_NAME, nettmp) < 0){continue;}
		if(strncmp(ptptmp, ptpdev, MAX_PTPDEV_NAME)){continue;}
		strncpy(netdev, dir->d_name, CB_MAX_NETDEVNAME-1);
		netdev[CB_MAX_NETDEVNAME-1] = 0;
		res=0;
		break;
	}
	closedir(ndir);
	return res;
}

int cb_get_netdev_from_ptpdev(char *ptpdev, char *netdev)
{
	char *ptpname;
	char pname[128];
	int res;

	if(strstr(ptpdev, CB_VIRTUAL_PTPDEV_PREFIX)==ptpdev){
		int di=strlen(CB_VIRTUAL_PTPDEV_PREFIX);
		(void)strcpy(netdev, CB_VIRTUAL_ETHDEV_PREFIX);
		(void)strcpy(&netdev[strlen(CB_VIRTUAL_ETHDEV_PREFIX)], &ptpdev[di]);
		return 0;
	}
	ptpname=strrchr(ptpdev,'/');
	if(!ptpname){
		ptpname=ptpdev;
	}else{
		ptpname++;
	}
	/* Try with the PHY PTP first. If it is failed, we will go with MAC PTP. */
	res = phy_get_netdev_from_ptpdev(ptpname, netdev);
	if (res < 0) {
		(void)snprintf(pname, sizeof(pname), "/sys/class/ptp/%s/device/net",ptpname);
		return get_first_dir_name(pname, netdev, CB_MAX_NETDEVNAME);
	}
	return res;
}

int cb_get_ptpdev_from_netdev(char *netdev, char *ptpdev)
{
	int res;
	char pname[128];
	if(strstr(netdev, CB_VIRTUAL_ETHDEV_PREFIX)==netdev){
		int di=strlen(CB_VIRTUAL_ETHDEV_PREFIX);
		(void)strcpy(ptpdev, CB_VIRTUAL_PTPDEV_PREFIX);
		(void)strcpy(&ptpdev[strlen(CB_VIRTUAL_PTPDEV_PREFIX)], &netdev[di]);
		return 0;
	}
	(void)strcpy(ptpdev,"/dev/");

	/* Try with the PHY PTP first. If it is failed, we will go with MAC PTP. */
	res = phy_get_ptpdev_from_netdev(&ptpdev[5], MAX_PTPDEV_NAME-5, netdev);
	if (res < 0) {
		(void)snprintf(pname, sizeof(pname), "/sys/class/net/%s/device/ptp", netdev);
		res = get_first_dir_name(pname, &ptpdev[5], MAX_PTPDEV_NAME-5);
	}
	return res;
}

#endif
