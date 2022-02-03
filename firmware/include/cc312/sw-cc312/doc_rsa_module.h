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
  @defgroup rsa_module CryptoCell RSA APIs

  @ingroup cryptocell_api

  @brief RSA is an asymmetric algorithm used for secure-data transmission.

  @note As it is considered slow, it is mainly used to pass encrypted shared
  keys for symmetric key cryptography.

  The RSA module implements the standards defined in <em>Public-Key Cryptography
  Standards (PKCS) #1 v1.5: RSA Encryption</em> and <em>Public-Key
  Cryptography Standards (PKCS) #1 v2.1: RSA Cryptography Specifications</em>.

  @note CryptoCell-312 does not support blinding for RSA. If a function receives
        random pointers as input, these may be NULL.

  For the implementation of RSA, see rsa.h
 */

/*!
  @defgroup cc_rsa_hw_limit CryptoCell-312 hardware limitations for RSA

  @ingroup rsa_module

  CryptoCell-312 supports the following RSA key sizes for private-public
  operations:
  <ul><li>256 Bytes (2048 bits).</li>
  <li>384 Bytes (3071 bits).</li>
  <li>512 Bytes (4096 bits).</li></ul>

  For key-generation, CryptoCell-312 supports the following RSA key sizes:
  <ul><li>256 Bytes (2048 bits).</li>
  <li>384 Bytes (3071 bits).</li></ul>
 */

/*!
  @defgroup cc_rsa_typical Typical usage of RSA in CryptoCell-312

  @ingroup rsa_module

  The following is a typical RSA operation flow:
  <ol><li>mbedtls_rsa_init().</li>
  <li>mbedtls_rsa_gen_key().</li>
  <li>mbedtls_rsa_pkcs1_encrypt().</li></ol>

  @important CryptoCell-312 requires that the same \c hash_id used for
             mbedtls_rsa_init() is used for all subsequent operations.
             Otherwise, it returns an error.
 */

 /*!
  @defgroup cc_rsa_typical_ki Typical insertion of keys in CryptoCell-312

  @ingroup rsa_module

  The following is a typical RSA key-insertion flow:
  <ol><li>mbedtls_rsa_import() or mbedtls_rsa_import_raw().</li>
  <li>mbedtls_rsa_complete().</li></ol>

  If you insert keys that are not derived by CryptoCell-312,
  the following restrictions apply:
  <ul><li>The user may insert \c N, \c D, \c E, and the complete function does
  not derive the \c P and \c Q (the CRT values).</li>
  <li>The user may insert \c P and \c Q, and the complete function derives the
  CRT values from that, but does not derive \c D.</li>
  <li>Its Illegal to insert only part of the CRT key (only \c DP for example).</li>
  <li> If all the required key parameters were inserted the function does nothing.</li></ul>
 */

