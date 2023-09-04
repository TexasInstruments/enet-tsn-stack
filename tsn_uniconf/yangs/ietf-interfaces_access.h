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
#ifndef IETF_INTERFACES_ACCESS_H_
#define IETF_INTERFACES_ACCESS_H_

#include "yang_db_access.h"
#include "ietf-interfaces.h"

int ydbi_get_item_ifk3vk0(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status);

int ydbi_rel_item_ifk3vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status);

int ydbi_set_item_ifk3vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status,
			  void *value, uint32_t vsize, uint8_t notice);

int ydbi_del_item_ifk3vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3, bool status);

int ydbi_get_item_ifk4vk1(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status);

int ydbi_rel_item_ifk4vk1(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status);

int ydbi_set_item_ifk4vk1(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status,
			  void *value, uint32_t vsize, uint8_t notice,
			  uint8_t onhw);

int ydbi_del_item_ifk4vk1(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, uint8_t k2, uint8_t k3,
			  uint8_t k4, void *kv1, uint32_t kvs1, bool status);

int ydbi_get_item_ifk1vk0(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t k1, bool status);

int ydbi_rel_item_ifk1vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, bool status);

int ydbi_set_item_ifk1vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, bool status,
			  void *value, uint32_t vsize, uint8_t notice);

int ydbi_del_item_ifk1vk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t k1, bool status);

/**
 * @brief read ifupdown status(oper-status)
 * @return 0:down, 1:up, -1:error
 */
int ydbi_get_ifupdown(yang_db_item_access_t *ydbia, const char *netdev);

/**
 * @brief create a semaphore to get notices of ifupdown change
 */
int ydbi_set_ifupdown_ucnotice(yang_db_item_access_t *ydbia, const char *netdev,
			       UC_NOTICE_SIG_T **sem, const char *semname);

/**
 * @brief remove all the semaphores registered with 'semname'
 */
int ydbi_clear_ifupdown_ucnotice(yang_db_item_access_t *ydbia, const char *semname);

/**
 * @brief call this function to get ifupdown value when a notice comes with the semaphore
 *        netdev also returned.
 *        If 'semname' is unique, the netdev must be the same as the registered value.
 *        ifupdown 0:down, 1:up
 * @param netdev the result is copied, need CB_MAX_NETDEVNAME size
 */
int ydbi_get_ifupdown_ucnotice(yang_db_item_access_t *ydbia, char *netdev,
			       uint8_t *ifupdown, const char *semname);

/**
 * @brief retreive a value of a node from DB given for IETF interface using more than three keys
 *        than the likes of `ydbi_get_item_ifk3vk0`.
 * @param ybdbia	a handle to context data for accessing DB
 * @param rval		an output param containing value retreived from the DB
 * @param name		a key that is a string to access the node
 * @param kn		an array of parent nodes used to access the leaf node
 * @param kn_sz		size of array of parent nodes mentioned above
 * @param status	YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return 	a positive value on success which indicates size of data of the node has been retreived.
 * 		a 0 or negative on failure.
 */
int ydbi_get_item_ifknvk0(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint8_t kn[], uint8_t sz, bool status);

/**
 * @brief write a value to a node in DB given for IETF interface using more than three keys
 *        than the likes of `ydbi_set_item_ifk3vk0`.
 * @param ybdbia	a handle to context data for accessing DB
 * @param name		a key that is a string to access the node
 * @param kn		an array of parent nodes used to access the leaf node
 * @param kn_sz		size of array of parent nodes mentioned above
 * @param status	YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @param value		address of a buffer containing value to be written.
 * @param vsize		size of buffer containing value
 * @param notice	YDBI_PUSH_NOTICE for a notification to a listener or YDBI_NO_NOTICE
 * @return 	0 on success; !=0 on failure
 */
int ydbi_set_item_ifknvk0(yang_db_item_access_t *ydbia,
			  char *name, uint8_t kn[], uint8_t sz, bool status,
			  void *value, uint32_t vsize, uint8_t notice);

/**
 * @brief delete a node from DB given for IETF interface using more than three keys
 *        than the likes of `ydbi_del_item_ifk3vk0`.
 * @param ybdbia	a handle to context data for accessing DB
 * @param name		a key that is a string to access the node
 * @param kn		an array of parent nodes used to access the leaf node
 * @param kn_sz		size of array of parent nodes mentioned above
 * @param status	YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return 	0 on success; !=0 on failure
 */
int ydbi_del_item_ifknvk0(yang_db_item_access_t *ydbia, char *name,
			  uint8_t kn[], uint8_t sz, bool status);
/**
 * @brief retreive a value of a node from DB given for IETF interface using more than three keys
 *        than the likes of `ydbi_get_item_ifk3vk0` and two value keys including `name` of interface and
 *        a value which constitutes the second key.
 *        All specified node keys along with two value keys constitute the yang db key of the node to
 *        be retreived.
 * @param ybdbia	a handle to context data for accessing DB
 * @param rval		an output param containing value retreived from the DB
 * @param name		a key that is a string to access the node
 * @param vk		a number which is the second key to access the node
 * @param vk_sz		how many bytes used to encode the number `vk`
 * @param kn		an array of parent nodes used to access the leaf node
 * @param kn_sz		size of array of parent nodes mentioned above
 * @param status	YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return 	a positive value on success which indicates size of data of the node has been retreived.
 * 		a 0 or negative on failure.
 */
int ydbi_get_item_ifknvk1(yang_db_item_access_t *ydbia, void **rval,
			  char *name, uint32_t vk, uint8_t vk_sz,
			  uint8_t kn[], uint8_t kn_sz,
			  bool status);

/**
 * @brief write a value to a node in DB given for IETF interface using more than three keys
 *        than the likes of `ydbi_set_item_ifk3vk0` and two value keys including `name` of interface and
 *        a value which constitutes the second key.
 *        All specified node keys along with two value keys constitute the yang db key of the node to
 *        be written.
 * @param ybdbia	a handle to context data for accessing DB
 * @param name		a key that is a string to access the node
 * @param vk		a number which is the second key to access the node
 * @param vk_sz		how many bytes used to encode the number `vk`
 * @param kn		an array of parent nodes used to access the leaf node
 * @param kn_sz		size of array of parent nodes mentioned above
 * @param status	YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @param value		address of a buffer containing value to be written.
 * @param vsize		size of buffer containing value
 * @param notice	YDBI_PUSH_NOTICE for a notification to a listener or YDBI_NO_NOTICE
 * @return 	a positive value on success which indicates size of data of the node has been retreived.
 * 		a 0 or negative on failure.
 */
int ydbi_set_item_ifknvk1(yang_db_item_access_t *ydbia,
			  char *name, uint32_t vk, uint8_t vk_sz,
			  uint8_t kn[], uint8_t sz, bool status,
			  void *value, uint32_t vsize, uint8_t notice);

/**
 * @brief delete a node from DB given for IETF interface using more than three keys
 *        than the likes of `ydbi_del_item_ifk3vk0` and two value keys including `name` of interface and
 *        a value which constitutes the second key.
 * @param ybdbia	a handle to context data for accessing DB
 * @param name		a key that is a string to access the node
 * @param vk		a number which is the second key to access the node
 * @param vk_sz		how many bytes used to encode the number `vk`
 * @param kn		an array of parent nodes used to access the leaf node
 * @param kn_sz		size of array of parent nodes mentioned above
 * @param status	YDBI_CONFIG:access a rw node, YDBI_STATUS: access a readonly node
 * @return 	0 on success; !=0 on failure
 */
int ydbi_del_item_ifknvk1(yang_db_item_access_t *ydbia,
			  char *name, uint32_t vk, uint8_t vk_sz,
			  uint8_t kn[], uint8_t sz, bool status);

#endif
