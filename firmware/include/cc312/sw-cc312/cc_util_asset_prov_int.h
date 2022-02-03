/****************************************************************************
* The confidential and proprietary information contained in this file may   *
* only be used by a person authorised under and to the extent permitted     *
* by a subsisting licensing agreement from Arm Limited (or its affiliates). *
*     (C) COPYRIGHT [2001 - 2018] Arm Limited (or its affiliates).            *
*         ALL RIGHTS RESERVED                                               *
* This entire notice must be reproduced on all copies of this file          *
* and copies of this file may only be made by a person if such person is    *
* permitted to do so under the terms of a subsisting license agreement      *
* from Arm Limited (or its affiliates).                                     *
*****************************************************************************/

#ifndef  _CC_UTIL_ASSET_PROV_INT_H
#define  _CC_UTIL_ASSET_PROV_INT_H

/*!
@file
@brief This file contains the functions and definitions for the ICV or OEM Asset provisioning in run-time library.
*/

#ifdef __cplusplus
extern "C"
{
#endif
#include "cc_pal_types.h"
#include "cc_bitops.h"

#define CC_ASSET_PROV_MAX_ASSET_SIZE  (4*CC_1K_SIZE_IN_BYTES)

#define CC_ASSET_PROV_TOKEN     0x41736574UL
#define CC_ASSET_PROV_VERSION   0x10000UL

#define CC_ASSET_PROV_NONCE_SIZE    12
#define CC_ASSET_PROV_RESERVED_SIZE     8
#define CC_ASSET_PROV_RESERVED_WORD_SIZE    (CC_ASSET_PROV_RESERVED_SIZE/CC_32BIT_WORD_SIZE)
#define CC_ASSET_PROV_TAG_SIZE      16
#define CC_ASSET_PROV_BLOCK_SIZE    16

#define CC_ASSET_PROV_ADATA_SIZE    (3*CC_32BIT_WORD_SIZE+CC_ASSET_PROV_RESERVED_SIZE)  // token||version||assetSize||reserved


typedef struct {
        uint32_t  token;
        uint32_t  version;
        uint32_t  assetSize;
        uint32_t  reserved[CC_ASSET_PROV_RESERVED_WORD_SIZE];
        uint8_t   nonce[CC_ASSET_PROV_NONCE_SIZE];
        uint8_t   encAsset[CC_ASSET_PROV_MAX_ASSET_SIZE+CC_ASSET_PROV_TAG_SIZE];
}CCAssetProvPkg_t;


#ifdef __cplusplus
}
#endif

#endif /*_CC_UTIL_ASSET_PROV_INT_H*/
