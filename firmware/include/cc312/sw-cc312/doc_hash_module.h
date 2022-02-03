/****************************************************************************
* The confidential and proprietary information contained in this file may   *
* only be used by a person authorised under and to the extent permitted     *
* by a subsisting licensing agreement from Arm Limited (or its affiliates). *
*     (C) COPYRIGHT [2017-2018] Arm Limited (or its affiliates).            *
*         ALL RIGHTS RESERVED                                               *
* This entire notice must be reproduced on all copies of this file          *
* and copies of this file may only be made by a person if such person is    *
* permitted to do so under the terms of a subsisting license agreement      *
* from Arm Limited (or its affiliates).                                     *
*****************************************************************************/

/*!
  @addtogroup cc_hash

  @brief The hash or Message Digest (MD) module allows you to calculate
  hash digests from data, and create signatures based on those hash digests.

  HMAC is a wrapping algorithm that uses one of the supported
  hash algorithms and a key, to generate a unique
  authentication code over the input data.

  All hash algorithms can be accessed via the generic MD layer.
  For more information, see ::mbedtls_md_setup().

  For more information on supported hash algorithms, @see cc_hash_hw_limit.

  For the implementation of hash and HMAC, see md.h.
  */

/*!
  @defgroup cc_hash_hw_limit CryptoCell-312 hardware limitations for hash

  @ingroup cc_hash

  The CryptoCell-312 hardware supports accelerated hash operations for
  the following modes:
  <ul><li>SHA-1</li>
  <li>SHA-224</li>
  <li>SHA-256</li></ul>

  SHA-384 and SHA-512 operations are only supported in a
  non-accelerated software mode.

  To support the accelerated algorithms, the following conditions
  must be met:
  <ul><li>The input buffer must be DMA-able.</li>
  <li>The input buffer must be physically contingous
  block in memory.</li>
  <li>Buffer size must be up to 64KB.</li>
  <li>The context must also be DMA-able, as partial
  and final results are written to the context.</li></ul>
 */

/*!
  @defgroup cc_hash_typical Typical usage of hash in CryptoCell-312

  @ingroup cc_hash

  The following is a typical hash Block operation flow
  directly using the SHA module:
  <ol><li>mbedtls_sha1_init().</li>
  <li>mbedtls_sha1_starts_ret().</li>
  <li>mbedtls_sha1_update_ret().</li>
  <li>mbedtls_sha1_finish_ret().</li></ol>

  The following is a typical HMAC Block operation flow
  using the MD module:
  <ol><li>mbedtls_md_setup().</li>
  <li>mbedtls_md_hmac_starts().</li>
  <li>mbedtls_md_hmac_update().</li>
  <li>mbedtls_md_hmac_finish().</li></ol>
 */