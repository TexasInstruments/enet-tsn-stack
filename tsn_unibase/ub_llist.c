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
 * ub_llist.c
 * memory buffer utilities
 *
 */
#include "ub_llist.h"

void ub_list_init(struct ub_list *ub_list)
{
	if(!ub_list){return;}
	ub_list->head = NULL;
	ub_list->tail = NULL;
	ub_list->count = 0;
}

void ub_list_clear(struct ub_list *ub_list, ub_list_node_clear_h node_clear,
		   void *arg)
{
	struct ub_list_node *node;
	if(!ub_list){return;}
	node = ub_list->head;
	while(node!=NULL){
		struct ub_list_node *next = node->next;
		if(node_clear!=NULL){node_clear(node, arg);}
		node = next;
	}
	ub_list_init(ub_list);
}

void ub_list_append(struct ub_list *ub_list, struct ub_list_node *node)
{
	if(!ub_list || !node){return;}
	node->prev = ub_list->tail;
	node->next = NULL;

	if(ub_list->head==NULL){ub_list->head = node;}
	if(ub_list->tail!=NULL){ub_list->tail->next = node;}
	ub_list->tail = node;
	ub_list->count++;
}

void ub_list_prepend(struct ub_list *ub_list, struct ub_list_node *node)
{
	if(!ub_list || !node){return;}
	node->prev = NULL;
	node->next = ub_list->head;

	if(ub_list->head!=NULL){ub_list->head->prev = node;}
	if(ub_list->tail==NULL){ub_list->tail = node;}
	ub_list->head = node;
	ub_list->count++;
}

void ub_list_insert_before(struct ub_list *ub_list, struct ub_list_node *refnode,
			   struct ub_list_node *node)
{
	if(!ub_list || !refnode || !node){return;}
	if(refnode->prev!=NULL){refnode->prev->next = node;}
	else if(ub_list->head == refnode){
		ub_list->head = node;
	}else{}

	node->prev = refnode->prev;
	node->next = refnode;
	refnode->prev = node;
	ub_list->count++;
}

void ub_list_insert_after(struct ub_list *ub_list, struct ub_list_node *refnode,
			  struct ub_list_node *node)
{
	if(!ub_list || !refnode || !node){return;}
	if(refnode->next!=NULL){refnode->next->prev = node;}
	else if(ub_list->tail == refnode){
		ub_list->tail = node;
	}else{}

	node->prev = refnode;
	node->next = refnode->next;
	refnode->next = node;
	ub_list->count++;
}

void ub_list_unlink(struct ub_list *ub_list, struct ub_list_node *node)
{
	if(!node){return;}
	if(node->prev!=NULL){
		node->prev->next = node->next;
	}else{
		ub_list->head = node->next;
	}

	if(node->next!=NULL){
		node->next->prev = node->prev;
	}else{
		ub_list->tail = node->prev;
	}

	node->next = NULL;
	node->prev = NULL;
	if (ub_list->count > (uint32_t)0){ub_list->count--;}
}

void ub_list_sort(struct ub_list *ub_list, ub_list_sort_h *sh, void *arg)
{
	struct ub_list_node *node;
	bool sort=true;

	if(!ub_list || !sh){return;}

	while(sort){
		node = ub_list->head;
		sort = false;
		while(node && node->next){
			if(sh(node, node->next, arg)){
				node = node->next;
			}else{
				struct ub_list_node *tnode = node->next;
				ub_list_unlink(ub_list, node);
				ub_list_insert_after(ub_list, tnode, node);
				sort = true;
			}
		}
	}
}

struct ub_list_node *ub_list_apply(const struct ub_list *ub_list, bool fwd,
				   ub_list_apply_h *ah, void *arg)
{
	struct ub_list_node *node;

	if(!ub_list || !ah){return NULL;}

	node = fwd ? ub_list->head : ub_list->tail;
	while(node!=NULL){
		struct ub_list_node *cur = node;
		node = fwd ? node->next : node->prev;
		if(ah(cur, arg)){return cur;}
	}
	return NULL;
}

struct ub_list_node *ub_list_head(const struct ub_list *ub_list)
{
	return ub_list ? ub_list->head : NULL;
}

struct ub_list_node *ub_list_tail(const struct ub_list *ub_list)
{
	return ub_list ? ub_list->tail : NULL;
}

uint32_t ub_list_count(const struct ub_list *ub_list)
{
	if(!ub_list){return 0;}
	return ub_list->count;
}
