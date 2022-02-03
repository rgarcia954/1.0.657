/****************************************************************************
* The confidential and proprietary information contained in this file may    *
* only be used by a person authorised under and to the extent permitted      *
* by a subsisting licensing agreement from ARM Limited or its affiliates.    *
*   (C) COPYRIGHT [2017-2018] ARM Limited or its affiliates.         *
*       ALL RIGHTS RESERVED                          *
* This entire notice must be reproduced on all copies of this file           *
* and copies of this file may only be made by a person if such person is     *
* permitted to do so under the terms of a subsisting license agreement       *
* from ARM Limited or its affiliates.                        *
*****************************************************************************/

#ifndef  _CC_DMPU_UTIL_H
#define  _CC_DMPU_UTLI_H

/*!
@file
@brief This file contains the functions and definitions for the OEM Asset provisioning.
*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "common_rsa_keypair.h"
#include "cc_pka_hw_plat_defs.h"

#define DMPU_CERT_HEADER_SIZE_IN_BYTES   12     // token || version || size
#define DMPU_OEM_KEY_REQ_TOKEN  0x52455144
#define DMPU_OEM_KEY_REQ_VERSION    0x01UL

typedef struct {
        uint32_t  token;
        uint32_t  version;
        uint32_t  size;
        uint8_t   oemMainPubKey[ SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES];
        uint8_t   oemEncPubKey[ SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES];
        uint8_t   signature[SB_CERT_RSA_KEY_SIZE_IN_BYTES];
}CCOemRequestCert_t;

/**
* @brief initialize openSSL library
*
* @param[in] None
* @param[out] None
*
*/
/*********************************************************/
void InitOpenSsl(void);


/**
* @brief terminates and cleanup openSSL library
*
* @param[in]  None
* @param[out] None
*
*/
/*********************************************************/
void CloseOpenSsl(void);

#ifdef __cplusplus
}
#endif

/**
* @brief performs CMAC key derivation for Kprov using openSSL library
*
* @param[in]  pKey & keySize - Kpicv key and its size
*       lable & pContext & contextSize used to build the dataIn for derivation
* @param[out] pOutKey - Kprov
*
*/
/*********************************************************/
int AesCmacKeyDerivation(char *pKey, uint32_t keySize,
                uint8_t *pLabel, uint32_t labelSize,
                uint8_t *pContext, uint32_t contextSize,
                char *pOutKey, uint32_t outKeySize);

#endif /*_CC_DMPU_UTIL_H */
