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
/**
 * @file        nconf_config.h
 *
 * @brief       Netconf Build Configs.
 */
#ifndef __NCONF_CONFIG_H__
#define __NCONF_CONFIG_H__

/** \brief SSH Server Port dedicated for NETCONF subsystem, default is 830 (ref. RFC 4742/6242) */
#ifndef NCONF_TLS_TRANSPORT_PORT
#define NCONF_SSH_TRANSPORT_PORT        (830U)
#endif

/** \brief Port used for NETCONF over TLS Transport Default is 6513 (ref. RFC 5539/7589) */
#ifndef NCONF_TLS_TRANSPORT_PORT
#define NCONF_TLS_TRANSPORT_PORT        (6513U)
#endif

/** \brief TCP Local Port used for communicating with SSH Subsystem application */
#ifndef NCONF_SUB_TRANSPORT_PORT
#define NCONF_SUB_TRANSPORT_PORT        (10830U)
#endif

/** \brief Expected reply from daemon to confirm ssh connection is valid */
#ifndef NCONF_VALID_SSHINFO_ACK
#define NCONF_VALID_SSHINFO_ACK         "WDROQ09ORjogU1NIX0NPTk5FQ1RJT04gT0s="
#endif

/** \brief maximum number of fragment used in pre-allocate buffer used for
 *         general allocation inside netconf total memory size allocated
 *         is <max number of fragment> * 8
 */
#ifndef NCONF_GEN_ALLOC_FRAGMENTS
#define NCONF_GEN_ALLOC_FRAGMENTS       (3000U)
#endif

/** \brief Maximum number of config info that netconf server could
 *         process per subtree
 */
#ifndef NCONF_MAX_UCCFG_INFO_NUM
#define NCONF_MAX_UCCFG_INFO_NUM        (128U)
#endif

/** \brief Use Unix Domain Socket for internal communication between
 *         the SSH subsystem application and NETCONF daemon instead of
 *         Local TCP Socket
 */
#ifndef NCONF_USE_UNIX_DOMAIN_SOCKET
#define NCONF_USE_UNIX_DOMAIN_SOCKET    (0x00U)
#endif
#ifndef NCONF_UNIX_DOMAIN_SOCKET_PATH
#define NCONF_UNIX_DOMAIN_SOCKET_PATH   "/tmp/x4nconfsrv.sock"
#endif

/** \brief Enabled/DisplayTCP Transport Transport
 *  \note  Used only for development
 */
#ifndef NCONF_ENABLE_TCP_PLAINTEXT
#define NCONF_ENABLE_TCP_PLAINTEXT      (0x00U)
#endif

#endif /* __NCONF_CONFIG_H__ */
