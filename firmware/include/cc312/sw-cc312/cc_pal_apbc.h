/****************************************************************************
* The confidential and proprietary information contained in this file may   *
* only be used by a person authorised under and to the extent permitted     *
* by a subsisting licensing agreement from Arm Limited (or its affiliates). *
*     (C) COPYRIGHT [2018] Arm Limited (or its affiliates).                 *
*         ALL RIGHTS RESERVED                                               *
* This entire notice must be reproduced on all copies of this file          *
* and copies of this file may only be made by a person if such person is    *
* permitted to do so under the terms of a subsisting license agreement      *
* from Arm Limited (or its affiliates).                                     *
*****************************************************************************/

/*!
 @file
 @brief This file contains the definitions and APIs for APBC implementation.

 This is a placeholder for platform-specific APBC implementation.
*/

/*!
 @defgroup cc_pal_apbc CryptoCell PAL APBC APIs
 @brief Contains PAL APBC APIs. See cc_pal_apbs.h.

 @{
 @ingroup cc_pal
 @}
 */

#ifndef _CC_PAL_APBC_H
#define _CC_PAL_APBC_H

/*!
 @brief This function initiates an atomic counter.

 @return Void.
 */
void CC_PalApbcCntrInit(void);

/*!
 @brief This function returns the number of APBC access operations.

 @return The value of the atomic counter.
 */
int32_t CC_PalApbcCntrValue(void);

/*!
 @brief This function updates the atomic counter on each call to APBC access.

 On each call to APBC access, the counter is increased. At the end of each operation,
 the counter is decreased.

 @return \c 0 on success.
 @return A non-zero value on failure.
 */
CCError_t CC_PalApbcModeSelect(
               CCBool isApbcInc /*!< [in] Determines APBC mode.
                                      TRUE (APBC start access), or FALSE (APBC finish access). */
               );


#endif
