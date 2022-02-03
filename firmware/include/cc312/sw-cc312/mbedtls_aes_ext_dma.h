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
/**
 @defgroup ext_dma CryptoCell external DMA APIs
 @brief Contains all CryptoCell external DMA API definitions.

 @{
 @ingroup cryptocell_api
 @}
*/

/*!
 @file
 @brief This file contains all the CryptoCell AES external DMA APIs, their enums and definitions.
 */

/*!
 @defgroup aes_ext_dma CryptoCell AES external DMA APIs
 @brief Contains CryptoCell AES external DMA API definitions. See mbedtls_aes_ext_dma.h.

 @{
 @ingroup ext_dma
 @}
*/


#ifndef _MBEDTLS_AES_EXT_DMA_H
#define _MBEDTLS_AES_EXT_DMA_H

#include "cc_aes_defs_proj.h"
#include "cc_pal_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
  @brief This function initializes the external DMA Control. It configures the AES mode, the direction(encryption or decryption), and the data size.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_init(
    unsigned int keybits,                       /*!< [in] AES key size. Valid values are:
                                                          128bits, 192bits, or 256bits. */
    int   encryptDecryptFlag,                   /*!< [in] <ul><li>0: Encrypt.</li><li>1: Decrypt</li></ul>. */
    CCAesOperationMode_t operationMode         /*!< [in] AES mode. Supported modes are:
                                                          <ul><li>ECB</li><li>CBC</li><li>CTR</li><li>CBC_MAC</li><li>CMAC</li><li>OFB</li></ul> */

    );


/*!
  @brief This function configures the key.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int  mbedtls_aes_ext_dma_set_key(
    CCAesOperationMode_t operationMode, /*!< [in] AES mode. Supported modes are:
                                                  ECB, CBC, CTR, CBC_MAC, CMAC or OFB. */
    const unsigned char *key,   /*!< [in] The AES key buffer. */
    unsigned int keybits        /*!< [in] The size of the AES Key. Valid values are:
                                          128bits, 192bits, or 256bits. */
    );


/*!
  @brief This function configures the IV.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_set_iv(
    CCAesOperationMode_t operationMode, /*!< [in] AES mode. Supported modes are:
                                                  ECB, CBC, CTR, CBC_MAC, CMAC or OFB. */
    unsigned char       *iv,            /*!< [in] The AES IV buffer.*/
    unsigned int         iv_size        /*!< [in] The size of the IV. Must be 16 Bytes.*/
    );

/*!
  @brief This function configures data size which will be written to external DMA interface.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_set_data_size(
    uint32_t                dataSize,       /*!< [in] Size of input data in bytes.*/
    CCAesOperationMode_t    operationMode   /*!< [in] The AES mode. Supported modes are:
                                                      ECB, CBC, CTR, CBC_MAC, CMAC or OFB. */
);


/*!
  @brief This function returns the IV after an AES CMAC or a CBCMAC operation.

  @return \c CC_OK on success.
  @return A non-zero value from cc_aes_error.h on failure.
 */
int mbedtls_aes_ext_dma_finish(
    CCAesOperationMode_t operationMode, /*!< [in] The AES mode. Supported modes are:
                                                  ECB, CBC, CTR, CBC_MAC, CMAC or OFB. */
    unsigned char       *iv,            /*!< [out] The AES IV buffer. */
    unsigned int         iv_size        /*!< [in] The size of the IV. Must be 16 Bytes. */
    );



#ifdef __cplusplus
}
#endif

#endif /* #ifndef MBEDTLS_AES_EXT_DMA_H */
