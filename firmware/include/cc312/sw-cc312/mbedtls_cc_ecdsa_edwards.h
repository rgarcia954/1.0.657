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

 @brief This file contains the CryptoCell ECDSA Edwards curve APIs.

 This API supports ECDSA Edwards for generating, signing and verifying keys.
 This is implemented based on <em>Ed25519: High-speed high-security signatures</em>.
 */

/*!
 @defgroup
 @brief Contains the CryptoCell ECDSA Edwards curve APIs. See mbedtls_cc_ecdsa_edwards.h.
 @{
 @ingroup ecdsa_module
 @}
 */

#ifndef _MBEDTLS_ECDSA_EDWARDS_H
#define _MBEDTLS_ECDSA_EDWARDS_H


#include "cc_pal_types.h"
#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/************************ Typedefs  ****************************/

/************************ Enums ********************************/

/************************ Structs  ******************************/

/************************ context Structs  ******************************/

/*!
 @brief          This function generates an ECDSA keypair on the Edwards 25519 curve.

 @return         \c 0 on success.
 @return         An \c MBEDTLS_ERR_ECP_XXX code on failure.
 */
int mbedtls_ecdsa_genkey_edwards(
            mbedtls_ecdsa_context *ctx, /*!< The ECDSA context to store the
            keypair in. */
            mbedtls_ecp_group_id gid, /*!< The elliptic curve to use. Currently
            only 25519 curve is supported. */
            int (*f_rng)(void *, unsigned char *, size_t), /*!< The RNG function. */
            void *p_rng  /*!< The RNG context. */
            );

/*!
   @brief           This function computes the ECDSA signature of a
                    previously-hashed message.

   @note            If the bitlength of the message hash is larger than the
                    bitlength of the group order, then the hash is truncated
                    as defined in <em>Standards for Efficient Cryptography Group
                    (SECG): SEC1 Elliptic Curve Cryptography</em>, section
                    4.1.3, step 5.

   @return          \c 0 on success.
   @return          An \c MBEDTLS_ERR_ECP_XXX or \c MBEDTLS_MPI_XXX error code
                    on failure.
 */
int mbedtls_ecdsa_sign_edwards(
            mbedtls_ecp_group *grp, /*!< The ECP group. */
            mbedtls_mpi *r, /*!< The first output integer. */
            mbedtls_mpi *s, /*!< The second output integer. */
            const mbedtls_mpi *d, /*!< The private signing key. */
            const unsigned char *buf, /*!< The message hash. */
            size_t blen /*!< The length of \p buf. */
            );


/*!
   @brief           This function verifies the ECDSA signature of a
                    previously-hashed message.

   @note            If the bitlength of the message hash is larger than the
                    bitlength of the group order, then the hash is truncated as
                    defined in <em>Standards for Efficient Cryptography Group
                    (SECG): SEC1 Elliptic Curve Cryptography</em>, section
                    4.1.4, step 3.

   @return          \c 0 on success.
   @return          \c MBEDTLS_ERR_ECP_BAD_INPUT_DATA if signature is invalid.
   @return          An \c MBEDTLS_ERR_ECP_XXX or \c MBEDTLS_MPI_XXX
                    error code on failure for any other reason.
  */
int mbedtls_ecdsa_verify_edwards(
            mbedtls_ecp_group *grp, /*!< The ECP group. */
            const unsigned char *buf, /*!<The message hash . */
            size_t blen, /*!< The length of \p buf. */
            const mbedtls_ecp_point *Q, /*!< The public key to use for verification. */
            const mbedtls_mpi *r, /*!< The first integer of the signature. */
            const mbedtls_mpi *s /*!< The second integer of the signature. */
            );

/**
   @brief           This function imports an EC Edwards public key.

   @return          \c 0 on success.
   @return          \c MBEDTLS_ERR_ECP_BAD_INPUT_DATA
                    or \c MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE on failure.
 */
int mbedtls_ecdsa_public_key_read_edwards(
            mbedtls_ecp_point *Q, /*!< [out] The public key to import. */
            unsigned char *buf, /*!< [in] The buffer to read the public key from. */
            size_t blen /*!< [in] The length of the buffer in bytes. */
            );

/**
   @brief           This function exports an EC Edwards public key.

   @return          \c 0 on success.
   @return          \c MBEDTLS_ERR_ECP_BAD_INPUT_DATA
                    or \c MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL on failure.
 */
int mbedtls_ecdsa_public_key_write_edwards(
            const mbedtls_ecp_point *Q, /*!< [in] The public key to export. */
            size_t *olen, /*!< [out] The length of the data written in bytes. */
            unsigned char *buf, /*!< [out] The buffer to write the public key to. */
            size_t blen /*!< [in] The length of the buffer in bytes. */
            );


 #ifdef __cplusplus
}
#endif

#endif /* _MBEDTLS_ECDSA_EDWARDS_H */

