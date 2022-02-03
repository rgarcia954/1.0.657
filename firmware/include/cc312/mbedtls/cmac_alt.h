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

#ifndef MBEDTLS_CMAC_ALT_H
#define MBEDTLS_CMAC_ALT_H

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif



#include <stddef.h>
#include <stdint.h>


/* hide internal implementation of the struct. Allocate enough space for it.*/
#define MBEDTLS_CMAC_CONTEXT_SIZE_IN_WORDS          33


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          CMAC cipher context structure
 */
struct mbedtls_cmac_context_t{
    /*! Internal buffer */
    uint32_t buf[MBEDTLS_CMAC_CONTEXT_SIZE_IN_WORDS];
};

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CMAC_ALT_H */
