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
 * @ingroup TSN_UNIBASE_MODULE
 * @defgroup llist Doubly Linked List
 * @{
 * @file ub_llist.h
 *
 * @brief Doubly linked list
 */

#ifndef LUB_LIST_UTILS_H_
#define LUB_LIST_UTILS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ub_list node structure
 */
struct ub_list_node {
	struct ub_list_node *prev;     /**< Pointer to previous node */
	struct ub_list_node *next;     /**< Pointer to next node */
	void *data;		       /**< Pointer to data */
};

/**
 * @brief Initialize members of ub_list node
 */
#define UB_LIST_NODE_INIT {NULL, NULL, NULL}

/**
 * @brief Link ub_list structure
 */
struct ub_list {
	struct ub_list_node *head;    /**< Pointer to first node */
	struct ub_list_node *tail;    /**< Pointer to last node */
	uint32_t count;
};

/**
 * @brief Initialize ub_list
 */
#define UB_LIST_INIT {NULL, NULL, 0}

/**
 * @brief Initialize ub_list
 * @param ub_list pointer to link ub_list structure
 */
void ub_list_init(struct ub_list *ub_list);

/**
 * @brief function to clean up node
 * @param node node to be applied
 * @param arg clean up argument
 */
typedef void (ub_list_node_clear_h)(struct ub_list_node *node, void *arg);

/**
 * @brief Clear the ub_list without freeing any nodes
 * @param ub_list pointer to link ub_list structure
 * @param node_clear clean up callback fuction
 * @param arg clean up argument
 */
void ub_list_clear(struct ub_list *ub_list, ub_list_node_clear_h node_clear,
		   void *arg);

/**
 * @brief Append a node at the end of the ub_list
 * @param ub_list Pointer to link ub_list structure
 * @param node entry to be appended
 */
void ub_list_append(struct ub_list *ub_list, struct ub_list_node *node);

/**
 * @brief Append a node at the start of the ub_list
 * @param ub_list Pointer to link ub_list structure
 * @param node entry to be appended
 */
void ub_list_prepend(struct ub_list *ub_list, struct ub_list_node *node);

/**
 * @brief Append a node before a reference node
 * @param ub_list Pointer to link ub_list structure
 * @param refnode Reference node
 * @param node entry to be inserted
 */
void ub_list_insert_before(struct ub_list *ub_list, struct ub_list_node *refnode,
			   struct ub_list_node *node);

/**
 * @brief Append a node after a reference node
 * @param ub_list Pointer to link ub_list structure
 * @param refnode Reference node
 * @param node entry to be appended
 */
void ub_list_insert_after(struct ub_list *ub_list, struct ub_list_node *refnode,
			  struct ub_list_node *node);

/**
 * @brief Remove a node from the ub_list
 * @param ub_list Pointer to link ub_list structure
 * @param node entry to be removed
 */
void ub_list_unlink(struct ub_list *ub_list, struct ub_list_node *node);

/**
 * @brief Comparator function handler for nodes
 * @param node1 first node
 * @param node2 second node
 * @param arg Comparison argument
 * @note the result must end up with all true, otherwise it goes to infinite loop
 */
typedef bool (ub_list_sort_h)(struct ub_list_node *node1,
			      struct ub_list_node *node2, void *arg);

/**
 * @brief Sort entries in the ub_list based on comparator
 * @param ub_list Pointer to link ub_list structure
 * @param sh Sort comparator handler
 * @param arg Sort argument
 */
void ub_list_sort(struct ub_list *ub_list, ub_list_sort_h *sh, void *arg);

/**
 * @brief Apply function handler for nodes
 * @param node node to be applied
 * @param arg Apply argument
 */
typedef bool (ub_list_apply_h)(struct ub_list_node *node, void *arg);

/**
 * @brief Apply function to nodes in the ub_list
 * @param ub_list Pointer to link ub_list structure
 * @param fwd Forward or reverse movement
 * @param ah Apply function handler
 * @param arg Apply argument
 */
struct ub_list_node *ub_list_apply(const struct ub_list *ub_list, bool fwd,
				   ub_list_apply_h *ah, void *arg);

/**
 * @brief Get first entry of the ub_list
 * @param ub_list Pointer to link ub_list structure
 */
struct ub_list_node *ub_list_head(const struct ub_list *ub_list);

/**
 * @brief Get last entry of the ub_list
 * @param ub_list Pointer to link ub_list structure
 */
struct ub_list_node *ub_list_tail(const struct ub_list *ub_list);

/**
 * @brief Get number of entries
 * @param ub_list Pointer to link ub_list structure
 * @return number of entries
 */
uint32_t ub_list_count(const struct ub_list *ub_list);

/**
 * @brief Get data of node
 * @param node Node entry
 * @return pointer to data
 */
static inline void *ub_list_nodedata(const struct ub_list_node *node){
	return node ? node->data : NULL;
}

/**
 * @brief Check if ub_list is empty
 * @param ub_list Pointer to link ub_list structure
 * @return true if node exist, otherwise false
 */
static inline bool ub_list_isempty(const struct ub_list *ub_list){
	return (ub_list!=NULL) ? (ub_list->head == NULL) : true;
}

/**
 * @brief Traverse the ub_list
 */
#define UB_LIST_FOREACH_ITER(ub_list, node) \
	(node) = ub_list_head((ub_list)); (node); (node) = (node)->next

#ifdef __cplusplus
}
#endif

#endif
/** @}*/
