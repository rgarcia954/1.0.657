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
  @defgroup rng_module CryptoCell RNG APIs

  @ingroup cryptocell_api

  @brief The Random Number Generator (RNG) module supports random number
  generation, as defined in <em>NIST SP 800-90A: Recommendation for Random
  Number Generation Using Deterministic Random Bit Generators</em>.
  See mbedtls_ctr_drbg_random().

  The block-cipher counter-mode based deterministic random-bit
  generator (CTR_DBRG). CryptoCell provides the source of entropy.

  For the implementation of RNG, see ctr_drbg.h.
 */