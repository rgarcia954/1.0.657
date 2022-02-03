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
  @defgroup dhm_module CryptoCell DHM APIs

  @ingroup cryptocell_api

  @brief Diffie-Hellman-Merkle (DHM) is used to securely exchange cryptographic
  keys over a public channel.

  As described in <em>Public-Key Cryptography Standards (PKCS) #3: Diffie Hellman
  Key Agreement Standard</em>:
  "[T]wo parties, without any prior arrangements, can agree upon a secret key
  that is known only to them...This secret key can then be used, for example,
  to encrypt further communications between the parties."

  The DHM module is implemented based on the definitions in the following
  standards:
  <ul><li><em>RFC-3526: More Modular Exponential (MODP) Diffie-Hellman groups
  for Internet Key Exchange (IKE)</em>: defines a number of standardized
  Diffie-Hellman groups for IKE.</li>
  <li><em>RFC-5114: Additional Diffie-Hellman Groups for Use with IETF
  Standards</em>: defines a number of standardized Diffie-Hellman
  groups that can be used.</li></ul>

  For the implementation of DHM, see dhm.h.
 */

 /*!
  @defgroup cc_dhm_hw_limit CryptoCell-312 hardware limitations for DHM

  @ingroup dhm_module

  To support the accelerated algorithms, the following conditions
  must be met:
  <ul><li>The contexts must be DMA-able, as they might
  be used for some symmetric operations.</li></ul>
 */

 /*!
  @defgroup cc_dhm_typical Typical usage of DHM in CryptoCell-312

  @ingroup dhm_module

  The following is a typical DHM flow for one party:
  <ol><li>mbedtls_dhm_init().</li>
  <li>mbedtls_mpi_read_string().</li>
  <li>mbedtls_mpi_read_string().</li>
  <li>mbedtls_dhm_make_params().</li>
  <li>mbedtls_dhm_read_public().</li>
  <li>mbedtls_dhm_calc_secret().</li></ol>
 */