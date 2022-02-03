/****************************************************************************
* The confidential and proprietary information contained in this file may   *
* only be used by a person authorised under and to the extent permitted     *
* by a subsisting licensing agreement from Arm Limited (or its affiliates). *
*     (C) COPYRIGHT [2001-2018] Arm Limited (or its affiliates).            *
*         ALL RIGHTS RESERVED                                               *
* This entire notice must be reproduced on all copies of this file          *
* and copies of this file may only be made by a person if such person is    *
* permitted to do so under the terms of a subsisting license agreement      *
* from Arm Limited (or its affiliates).                                     *
*****************************************************************************/

/*!
 @file
 @brief This file contains functions for memory operations.

 The functions are generally implemented as wrappers to different operating-system calls.

 \note None of the described functions validate the input parameters, so that the behavior
       of the APIs in case of an illegal parameter is dependent on the behavior of the operating system.
 */

/*!
 @defgroup cc_pal_mem CryptoCell PAL memory operations
 @brief Contains memory-operation functions. See cc_pal_mem.h.

 @{
 @ingroup cc_pal
 @}
 */

#ifndef _CC_PAL_MEM_H
#define _CC_PAL_MEM_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"
#include "cc_pal_mem_plat.h"
#include "cc_pal_malloc_plat.h"
#include <stdlib.h>
#include <string.h>

    /*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

    /**** ----- Memory Operations APIs ----- ****/

/*!
  @brief This function compares between two given buffers, according to the given size.

  @return The return values are according to operating-system return values.
 */
#define CC_PalMemCmp(aTarget, aSource, aSize) CC_PalMemCmpPlat(aTarget, aSource, aSize)

/*!
  @brief This function copies \p aSize Bytes from the source buffer to the destination buffer.

  @return void.
 */
#define CC_PalMemCopy(aDestination, aSource, aSize) CC_PalMemCopyPlat(aDestination, aSource, aSize)

/*!
  @brief This function moves \p aSize Bytes from the source buffer to the destination buffer.
  This function supports overlapped buffers.

  @return void.
 */
#define CC_PalMemMove(aDestination, aSource, aSize) CC_PalMemMovePlat(aDestination, aSource, aSize)


/*!
  @brief This function sets \p aSize Bytes of \p aChar in the given buffer.

  @return void.
 */
#define CC_PalMemSet(aTarget, aChar, aSize) CC_PalMemSetPlat(aTarget, aChar, aSize)


/*!
  @brief This function sets \p aSize Bytes in the given buffer with zeroes.

  @return void.
 */
#define CC_PalMemSetZero(aTarget, aSize) CC_PalMemSetZeroPlat(aTarget, aSize)


/*!
  @brief This function allocates a memory buffer according to \p aSize.

  @return A pointer to the allocated buffer on success.
  @return NULL on failure.
 */
#define CC_PalMemMalloc(aSize) CC_PalMemMallocPlat(aSize)

/*!
  @brief This function reallocates a memory buffer according to \p aNewSize.
         The contents of the old buffer is moved to the new location.

  @return A pointer to the newly-allocated buffer on success.
  @return NULL on failure.
 */
#define CC_PalMemRealloc(aBuffer, aNewSize) CC_PalMemReallocPlat(aBuffer, aNewSize)

/*!
  @brief This function frees a previously-allocated buffer.


  @return void.
 */
#define CC_PalMemFree(aBuffer) CC_PalMemFreePlat(aBuffer)

#ifdef __cplusplus
}
#endif

#endif

