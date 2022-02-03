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
  @defgroup ecdh_module ECDH module overview

  @ingroup cc_ecc

  @brief Elliptic-curve Diffie–Hellman (ECDH) is an anonymous key agreement
  protocol. It allows two parties to establish a shared secret over an
  insecure channel. Each party must have an elliptic-curve public–private
  key pair.

  For more information, see <em>NIST SP 800-56A Rev. 2: Recommendation
  for Pair-Wise Key Establishment Schemes Using Discrete Logarithm
  Cryptography</em>.

  For the implementation of ECDH, see ecdh.h.
 */

 /*!
  @defgroup cc_ecdh_hw_limit CryptoCell-312 hardware limitations for ECDH

  @ingroup ecdh_module

  CryotoCell-312 does not support Brainpool curves.

  */

/*!
  @defgroup cc_ecdh_typical Typical usage of ECDH in CryptoCell-312

  @ingroup ecdh_module

  The following is a typical ECDH operation flow:
  <ol><li>mbedtls_ecp_group_init().</li>
  <li>mbedtls_mpi_init() for each group parameter.</li>
  <li>mbedtls_ecdh_gen_public().</li></ol>
 */