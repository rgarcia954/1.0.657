/****************************************************************************
* The confidential and proprietary information contained in this file may   *
* only be used by a person authorised under and to the extent permitted     *
* by a subsisting licensing agreement from Arm Limited (or its affiliates). *
*     (C) COPYRIGHT [2018] Arm Limited (or its affiliates).            *
*         ALL RIGHTS RESERVED                                               *
* This entire notice must be reproduced on all copies of this file          *
* and copies of this file may only be made by a person if such person is    *
* permitted to do so under the terms of a subsisting license agreement      *
* from Arm Limited (or its affiliates).                                     *
*****************************************************************************/

/*!
 @file

 @brief This file contains the CryptoCell ECDH Edwards curve APIs.
 */

/*!
 @defgroup
 @brief Contains the CryptoCell ECDH Edwards curve APIs. See mbedtls_cc_ecdh_edwards.h.
 @{
 @ingroup ecdh_module
 @}
 */
#ifndef ECDH_EDWARDS_H
#define ECDH_EDWARDS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "mbedtls/ecp.h"

/************************ Defines ******************************/

/************************ Typedefs  ****************************/

/************************ Enums ********************************/

/************************ Structs  ******************************/

/************************ context Structs  ******************************/

/*!
   @brief           This function generates a public key and a TLS
                    ServerKeyExchange payload.

   @note            This function may be used only for curve 25519.

                    This is the first function used by a TLS server for ECDHE
                    ciphersuites.

   @note            This function assumes that the ECP group (\c grp) of the
                    \p ctx context has already been properly set,
                    for example, using mbedtls_ecp_group_load().

   @see             ecp.h

   @return          \c 0 on success.
   @return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */

int mbedtls_ecdh_make_params_edwards(
                      mbedtls_ecdh_context *ctx, /*!< The ECDH context. */
                      size_t *olen, /*!< The number of characters written. */
                      unsigned char *buf, /*!< The destination buffer. */
                      size_t blen, /*!< The length of the destination buffer. */
                      int (*f_rng)(void *, unsigned char *, size_t), /*!< The RNG function. */
                      void *p_rng /*!< The RNG context. */
                      );

/**
   @brief           This function parses and processes a TLS ServerKeyExhange
                    payload.

   @note            This function may be used only for curve 25519.

                    This is the first function used by a TLS client for ECDHE
                    ciphersuites.

   @see             ecp.h

   @return          \c 0 on success.
   @return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */
int mbedtls_ecdh_read_params_edwards(
                      mbedtls_ecdh_context *ctx, /*!< The ECDH context. */
                      const unsigned char **buf, /*!< The pointer to the start of the input buffer. */
                      const unsigned char *end /*!< The address for one Byte past the end of the buffer. */
                      );


#ifdef __cplusplus
}
#endif

#endif  /* MBEDTLS_ECDH_EDWARDS */
