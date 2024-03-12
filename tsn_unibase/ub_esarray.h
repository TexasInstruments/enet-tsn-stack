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
 * @defgroup esarray Expand and Shrink Array
 * @{
 * @file ub_esarray.h
 *
 * @brief Allocated memory array which is expanded or shrinked automatically
 *
 * How it expands and shrinks is defined by 'ub_esarray_init' call.
 * The first allocation happens at the call of 'ub_esarray_init'.
 * 'ub_esarray_close' must be called at the end to free all the allocated area.
 *
 * To use as a fifo, push data by 'ub_esarray_add_ele', and pop data by
 * 'ub_esarray_pop_ele'.  The contents of data element is copied each time.
 *
 * To use as a lifo(stack), push data by 'ub_esarray_add_ele', and pop data by
 * 'ub_esarray_pop_last_ele'.  The contents of data element is copied each time.
 *
 * To avoid copying data, data pointer and data index can be used.
 * But such use can't be thread safe.  To be thread safe, data must be
 * locked by calling 'ub_esarray_data_lock' during the time of using
 * the pointers or the index numbers, and must be unlocked by calling
 * 'ub_esarray_data_unlock' after that.
 */
#ifndef UB_ESARRAY_ARRAY_H_
#define UB_ESARRAY_ARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief the data handle of ub_esarray object, the inside of it is private
 */
typedef struct ub_esarray_cstd ub_esarray_cstd_t;

/**
 * @brief data type of the element
 * @note for actual element type, the caller needs to cast ub_esarray_element_t type
 * value.
 */
typedef void ub_esarray_element_t;

/**
 * @brief initialize ub_esarray object.
 * @return the data handle of ub_esarray_array object
 * @param esunit	expand and shrink unit
 * @param elesize	element size
 * @param maxeles	max number of elements
 */
ub_esarray_cstd_t *ub_esarray_init(int esunit, int elesize, int maxeles);

/**
 * @brief close ub_esarray
 * @param eah	the data handle of ub_esarray object
 */
void ub_esarray_close(ub_esarray_cstd_t *eah);

/**
 * @brief add one element
 * @return 0 on success, -1 on error
 * @param eah	the data handle of ub_esarray_array object
 * @param ed	a new element to add
 * @note thread safe, data is copied, 'ed' can be freed after this call.
 */
int ub_esarray_add_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed);

/**
 * @brief pop in fifo mode(get from index=0, and delete index=0)
 * @return 0 on success, -1 on error
 * @param eah	the data handle of ub_esarray_array object
 * @param ed	popped element; the data is copied to ed, which must have elesize space
 * @note thread safe, data is copied, 'ed' must be allocated by the caller.
 */
int ub_esarray_pop_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed);

/**
 * @brief pop in stack mode(get from the last index, and delete it)
 * @return 0 on success, -1 on error
 * @param eah	the data handle of ub_esarray_array object
 * @param ed	poped element; the data is copied to ed, which must have elesize space
 * @note thread safe, data is copied, 'ed' must be allocated by the caller.
 */
int ub_esarray_pop_last_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed);

/**
 * @brief delete one element *ed
 * @return 0 on success, -1 if no such elemet exists
 * @param eah	the data handle of ub_esarray_array object
 * @param ed	the element which has the same contents as 'ed' should be deleted
 * @note thread safe, the caller needs to bring 'ed' with the contents
 */
int ub_esarray_del_ele(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed);


/**
 * @brief lock data to preserve checked out data by ub_esarray_get_newele or
 *	ub_esarray_get_ele
 * @note using the data pointers or the data index number is not thread safe,
 *	to be thread safe, call 'ub_esarray_data_lock' before the work,
 *	and call 'ub_esarray_data_unlock' after the work.
 */
int ub_esarray_data_lock(ub_esarray_cstd_t *eah);

/**
 * @brief unlock data to return to the default status
 */
int ub_esarray_data_unlock(ub_esarray_cstd_t *eah);

/**
 * @brief returns the number of elements
 * @return the number of elements
 */
int ub_esarray_ele_nums(ub_esarray_cstd_t *eah);

/**
 * @brief get a new element from the ub_esarray_array
 * @return a new element which has area of elesize bytes
 * @param eah	the data handle of ub_esarray_array object
 * @note the contentes of the return is NOT THREAD-SAFE. the caller needs to make sure that
   no delete happens during the time to use the result pointer.
   the internal data is locked after this call.
 */
ub_esarray_element_t *ub_esarray_get_newele(ub_esarray_cstd_t *eah);

/**
 * @brief get the indexed element from the ub_esarray_array
 * @return the indexed element which has area of elesize bytes
 * @param eah	the data handle of ub_esarray_array object
 * @param index	index in the current ub_esarray_array
 * @note the contentes of the return is NOT THREAD-SAFE. the caller needs to make sure that
   no delete happens during the time to use the result pointer.
   the internal data is locked after this call.
 */
ub_esarray_element_t *ub_esarray_get_ele(ub_esarray_cstd_t *eah, int index);

/**
 * @brief delete one element with element pointer
 * @return 0 on success, -1 if no such element exists
 * @param eah	the data handle of ub_esarray_array object
 * @param ed	the element pointer which should be deleted
 * @note
 * NOT THREAD-SAFE; between ub_esarray_get_ele and ub_esarray_del_pointer, no other delete can happen.
 */
int ub_esarray_del_pointer(ub_esarray_cstd_t *eah, ub_esarray_element_t *ed);

/**
 * @brief delete one element with index
 * @return 0 on success, -1 if index >= number of elements
 * @param eah	the data handle of ub_esarray_array object
 * @param index	the index of the current ub_esarray_array, which should be deleted
 * @note
 * NOT THREAD-SAFE; index may not be right number, if other delete happens after getting index.
 */
int ub_esarray_del_index(ub_esarray_cstd_t *eah, int index);

#ifdef __cplusplus
}
#endif

#endif
/** @}*/
