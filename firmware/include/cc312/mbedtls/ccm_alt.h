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

#ifndef MBEDTLS_CCM_ALT_H
#define MBEDTLS_CCM_ALT_H

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/cipher.h"


#define MBEDTLS_ERR_CCM_BAD_INPUT      -0x000D /**< Bad input parameters to function. */
#define MBEDTLS_ERR_CCM_AUTH_FAILED    -0x000F /**< Authenticated decryption failed. */

/* hide internal implementation of the struct. Allocate enough space for it.*/
#define MBEDTLS_CCM_CONTEXT_SIZE_IN_WORDS 264

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          The CCM context-type definition. The CCM context is passed
 *                 to the APIs called.
 */
typedef struct {
    uint32_t buf[MBEDTLS_CCM_CONTEXT_SIZE_IN_WORDS];
}
mbedtls_ccm_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CCM_ALT_H */
