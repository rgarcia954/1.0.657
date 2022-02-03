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

#ifndef MBEDTLS_GCM_ALT_H
#define MBEDTLS_GCM_ALT_H

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_C)

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/cipher.h"

#define MBEDTLS_GCM_ENCRYPT     1
#define MBEDTLS_GCM_DECRYPT     0

#define MBEDTLS_ERR_GCM_AUTH_FAILED             -0x0012  /**< Authenticated decryption failed. */
#define MBEDTLS_ERR_GCM_BAD_INPUT               -0x0014  /**< Bad input parameters to function. */

/* hide internal implementation of the struct. Allocate enough space for it.*/
#define MBEDTLS_GCM_CONTEXT_SIZE_IN_WORDS   40

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          GCM context structure
 */
typedef struct {
    uint32_t buf[MBEDTLS_GCM_CONTEXT_SIZE_IN_WORDS];
}
mbedtls_gcm_context;

#ifdef __cplusplus
}
#endif



#endif /* MBEDTLS_GCM_C */
#endif /* MBEDTLS_GCM_ALT_H */
