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

#include "aes.h"
#include "mbedtls_cc_aes_crypt_additional.h"


/*!
  @addtogroup cc_aes

  @brief AES is a symmetric block cipher that uses a combination of both substitution
  and permutation. It is fast in both software and hardware.

  AES has a fixed block size of 128 bits, and supports the following key sizes:
  <ul><li>128 bits.</li><li>192 bits.</li><li>256 bits.</li></ul>

  For the implementation of AES, see aes.h.
 */


 /*!
  @defgroup cc_aes_hw_limit CryptoCell-312 hardware limitations for AES

  @ingroup cc_aes

  The CrytoCell-312 hardware accelerates the following AES operations:
  <ul><li>ECB.</li>
  <li>CBC.</li>
  <li>CTR.</li>
  <li>CMAC. For the implementation of CMAC, see cmac.h.</li>
  <li>OFB.</li>
  <li>CCM. For the implementation of CCM, see ccm.h.</li>
  <li>CCM star. For the implementation of CCM star, see mbedtls_cc_ccm_star.h.</li>
  <li>GCM. For the implementation of GCM, see gcm.h.</li></ul>

  To support the accelerated algorithms, the following conditions
  must be met:
  <ul><li>The input and output buffers must be DMA-able.</li>
  <li>The input and output buffers must be physically contingous
  blocks in memory.</li>
  <li>Buffer size must be up to 64KB.</li>
  <li>The context must also be DMA-able, as partial
  and final results are written to the context.</li>
  <li>Only integrated operations are supported for CCM, CCM star and GCM algorithms.</li></ul>
 */

 /*!
  @defgroup cc_aes_typical Typical usage of AES in CryptoCell-312

  @ingroup cc_aes

  The following is a typical AES Block operation flow:
  <ol><li>mbedtls_aes_init().</li>
  <li>mbedtls_aes_setkey_enc().</li>
  <li>mbedtls_aes_crypt_cbc().</li></ol>

*/