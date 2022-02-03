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
 @brief This file contains all of the OEM production library APIs, their enums and definitions.
 */

/*!
 @defgroup DMPU CryptoCell OEM production library APIs
 @brief Contains CryptoCell OEM production library APIs.

 See cc_dmpu.h.

 @{
 @ingroup prod
 @}
 */
#ifndef _DMPU_H
#define _DMPU_H

#include "cc_pal_types_plat.h"
#include "cc_prod.h"

/************************ Defines ******************************/

/*! The size of the OEM production library workspace in Bytes, needed by the library for internal use. */
#define DMPU_WORKSPACE_MINIMUM_SIZE  1536

/*! The size of the Hbk1 buffer in words. */
#define DMPU_HBK1_SIZE_IN_WORDS  4

/*! The size of the Hbk buffer in words. */
#define DMPU_HBK_SIZE_IN_WORDS  8

/************************ Enums ********************************/

/*! The type of the unique data. */
typedef enum {
    /*! The device uses Hbk1. */
        DMPU_HBK_TYPE_HBK1 = 1,
    /*! The device uses a full Hbk. */
        DMPU_HBK_TYPE_HBK = 2,
    /*! Reserved. */
        DMPU_HBK_TYPE_RESERVED  = 0x7FFFFFFF,
} CCDmpuHBKType_t;

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/

/*!
  The device use of the Hbk buffer.

  If the device uses Hbk0 and Hbk1, then the Hbk1 field is used.
  Otherwise, the Hbk field is used.
 */
typedef union {
        uint8_t hbk1[DMPU_HBK1_SIZE_IN_WORDS*CC_PROD_32BIT_WORD_SIZE] /*!< Hbk1 buffer if used by the device. */;
        uint8_t hbk[DMPU_HBK_SIZE_IN_WORDS*CC_PROD_32BIT_WORD_SIZE] /*!< Hbk buffer, that is, the full 256 bits. */;
} CCDmpuHbkBuff_t;



/*! The OEM production library input defines .*/
typedef struct {
        CCDmpuHBKType_t   hbkType /*!< The type of Hbk: Hbk1 with 128 bits, or Hbk with 256 bits. */;
        CCDmpuHbkBuff_t   hbkBuff /*!< The Hbk buffer. */;
        CCAssetType_t     kcpDataType /*!< The Kcp asset type. Allowed values are: Not used, Plain-asset or Package. */;
        CCAssetBuff_t     kcp /*!< The Kcp buffer, if \p kcpDataType is plain asset or package. */;
        CCAssetType_t     kceDataType /*!< The Kce asset type. Allowed values are: Not used, Plain-asset or Package. */;
        CCAssetBuff_t     kce /*!< The Kce buffer, if \p kceDataType is plain asset or package. */;
        uint32_t          oemMinVersion /*!< The minimal SW version of the OEM. */;
        uint32_t          oemDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE] /*!< The default DCU lock bits of the OEM. */;
}CCDmpuData_t;



/************************ Functions *****************************/

/*!
 @brief This function burns all OEM assets into the OTP of the device.

 The user must perform a power-on-reset (PoR) to trigger LCS change to Secure.

 @return \c CC_OK on success.
 @return A non-zero value from cc_prod_error.h on failure.
*/
CIMPORT_C CCError_t  CCProd_Dmpu(
        unsigned long   ccHwRegBaseAddr,   /*!< [in] The base
                                                address of CrytoCell HW registers. */
        CCDmpuData_t    *pDmpuData,        /*!< [in] A pointer to the
                                                defines structure of the OEM. */
        unsigned long   workspaceBaseAddr, /*!< [in] The base
                                                address of the workspace for
                                                internal use. */
        uint32_t        workspaceSize      /*!< [in] The size of
                                                provided workspace. Must be at least
                                                \p DMPU_WORKSPACE_MINIMUM_SIZE. */
) ;


#endif  //_DMPU_H
