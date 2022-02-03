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
 @brief This file contains the error definitions of the CryptoCell external DMA APIs.
 */

/*!
 @defgroup ext_dma_errors Specific errors of the CryptoCell external DMA APIs
 @brief Contains the CryptoCell external DMA-API error definitions. See mbedtls_ext_dma_error.h.
 @{
 @ingroup ext_dma
 @}
 */

#ifndef _MBEDTLS_EXT_DMA_ERROR_H
#define _MBEDTLS_EXT_DMA_ERROR_H

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/* The base address for errors of the CryptoCell external DMA. CC_EXT_DMA_MODULE_ERROR_BASE = 0x00F02D00 */
/* AES errors */
/*! Illegal mode. */
#define EXT_DMA_AES_ILLEGAL_OPERATION_MODE_ERROR        (CC_EXT_DMA_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal encryption mode. */
#define EXT_DMA_AES_INVALID_ENCRYPT_MODE_ERROR      (CC_EXT_DMA_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal decryption mode. */
#define EXT_DMA_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE             (CC_EXT_DMA_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal key size. */
#define EXT_DMA_AES_ILLEGAL_KEY_SIZE_ERROR              (CC_EXT_DMA_MODULE_ERROR_BASE + 0x03UL)
/*! Illegal IV. */
#define EXT_DMA_AES_INVALID_IV_OR_TWEAK_PTR_ERROR       (CC_EXT_DMA_MODULE_ERROR_BASE + 0x04UL)

/* Hash errors */
/*! Illegal hash operation mode. */
#define EXT_DMA_HASH_ILLEGAL_OPERATION_MODE_ERROR   (CC_EXT_DMA_MODULE_ERROR_BASE + 0x05UL)
/*! Illegal result buffer. */
#define EXT_DMA_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR    (CC_EXT_DMA_MODULE_ERROR_BASE + 0x06UL)
/*! Illegal parameters. */
#define EXT_DMA_HASH_ILLEGAL_PARAMS_ERROR   (CC_EXT_DMA_MODULE_ERROR_BASE + 0x07UL)

/* Chacha errors */
/*! Invalid nonce. */
#define EXT_DMA_CHACHA_INVALID_NONCE_PTR_ERROR          (CC_EXT_DMA_MODULE_ERROR_BASE + 0x08UL)
/*! Invalid encrypt or decrypt mode. */
#define EXT_DMA_CHACHA_INVALID_ENCRYPT_MODE_ERROR       (CC_EXT_DMA_MODULE_ERROR_BASE + 0x09UL)
/*! Invalid key pointer. */
#define EXT_DMA_CHACHA_INVALID_KEY_POINTER_ERROR        (CC_EXT_DMA_MODULE_ERROR_BASE + 0xAUL)
/*! Invalid key size. */
#define EXT_DMA_CHACHA_ILLEGAL_KEY_SIZE_ERROR           (CC_EXT_DMA_MODULE_ERROR_BASE + 0xBUL)
/*! Invalid nonce size flag. */
#define EXT_DMA_CHACHA_INVALID_NONCE_ERROR              (CC_EXT_DMA_MODULE_ERROR_BASE + 0xCUL)
/*! Illegal input size. */
#define EXT_DMA_CHACHA_ILLEGAL_INPUT_SIZE_ERROR         (CC_EXT_DMA_MODULE_ERROR_BASE + 0xDUL)

/* External DMA modules errors */
/*! Illegal input size. */
#define EXT_DMA_ILLEGAL_INPUT_SIZE_ERROR                (CC_EXT_DMA_MODULE_ERROR_BASE + 0xF0UL)

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif // _MBEDTLS_EXT_DMA_ERROR_H
