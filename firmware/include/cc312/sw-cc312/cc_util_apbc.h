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


#ifndef _CC_UTIL_APBC_H_
#define _CC_UTIL_APBC_H_

/*!
@file
@defgroup cc_apbc_defs CryptoCell APBC macros
@brief This file contains APBC definitions.
@{
@ingroup cryptocell_api

*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_apbc.h"

/*! Get APBC Access counter. Return number of active APBC accesses operations */
#define CC_APBC_CNTR_GET   CC_PalApbcCntrValue()

/*! Increment APBC access counter. */
#define CC_APBC_ACCESS_INC  CC_PalApbcModeSelect(CC_TRUE)

/*! Decrement APBC access counter. */
#define CC_APBC_ACCESS_DEC  CC_PalApbcModeSelect(CC_FALSE)


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif /*_CC_UTIL_APBC_H_*/
