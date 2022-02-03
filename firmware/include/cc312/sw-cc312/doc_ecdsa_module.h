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
  @defgroup ecdsa_module ECDSA module overview

  @ingroup cc_ecc

  @brief The Elliptic Curve Digital Signature Algorithm (ECDSA) is a used for
  generating and validating digital signatures.

  For the definition of ECDSA, see <em>Standards for Efficient Cryptography Group (SECG):
  SEC1 Elliptic Curve Cryptography</em>.

  For the use of ECDSA for TLS, see <em>RFC-4492: Elliptic Curve
  Cryptography (ECC) Cipher Suites for Transport Layer Security (TLS)</em>.

  For the implementation of ECDSA, see ecdsa.h.
 */

 /*!
  @defgroup cc_ecdsa_hw_limit CryptoCell-312 hardware limitations for ECDSA

  @ingroup ecdsa_module

  CryotoCell-312 does not support Brainpool curves.

  \note Using hash functions with hash size greater than the EC modulus size
        is not recommended.
 */
/*!
  @defgroup cc_ecdsa_typical Typical usage of ECDSA in CryptoCell-312

  @ingroup ecdsa_module

  The following is a typical ECDSA operation flow:
  <ol><li>mbedtls_ecp_group_init().</li>
  <li>mbedtls_mpi_init() for each group parameter.</li>
  <li>mbedtls_ecp_gen_keypair().</li>
  <li>mbedtls_ecdsa_sign() or mbedtls_ecdsa_verify().</li></ol>
 */