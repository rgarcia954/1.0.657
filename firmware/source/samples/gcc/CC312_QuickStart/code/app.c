/**
 * @file app.c
 * @brief Main application
 * This application is provided as a quick introduction to some of the more
 * common cryptographic functions that users may wish to use.
 *
 * This includes the following actions:
 * - AES Encryption of a small data packet
 * - AES Decryption of a small data packet
 * - CCM Encryption/Signing
 * - CCM Decryption/Authentication
 * - CMAC Signing
 * - HMAC Signing
 * - ECDH Key exchange
 *
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#include <stdint.h>
#include <stdbool.h>

#include "hw.h"
#include "swmTrace_api.h"

#include "app.h"

#include "cc_lib.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"


#include <string.h>
#include <malloc.h>

#include "mbedtls/aes.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"


/**
 * @brief The GPIO pin to use for TX when using the UART mode
 */
#define UART_TX_GPIO    				(6)

/**
 * @brief The GPIO pin to use for RX when using the UART mode
 */
#define UART_RX_GPIO    				(5)

/**
 * @brief The selected baud rate for the application when using UART mode
 */
#define UART_BAUD       				(115200)

/**
 * @brief Define the set up options we will use when tracing.
 */
static const uint32_t traceOptions[] = {
    SWM_LOG_LEVEL_INFO,             /* In all cases log info messages */
    SWM_UART_RX_PIN | UART_RX_GPIO, /* Set RX pin for cases when using UART */
    SWM_UART_TX_PIN | UART_TX_GPIO, /* Set TX pin for cases when using UART */
    SWM_UART_RX_ENABLE,             /* Enable the UART Rx Interrupts */
    SWM_UART_BAUD_RATE | UART_BAUD  /* Set the baud rate as defined above */
};

/**
 * @brief Define a counter that is used to measure SysTick intervals.
 */
static volatile int tickCounter;

/**
 * @brief Define the maximum duration we will allow a measurement to be
 * (seconds).
 */
#define MAX_TICK_DURATION       30

/**
 * @brief Define the RND context buffer, used by CC_LibInit.
 */
static CCRndContext_t   RNDContext;

/**
 * @brief Define the RND work buffer, used by CC_LibInit.
 */
static CCRndWorkBuff_t   RNDWorkBuffer;

/**
 * @brief Define a CTR_DRBG context structure, used by the RNDContext.
 */
static mbedtls_ctr_drbg_context DRBGContext;

/**
 * @brief Define an entropy context structure, used by the RNDContext.
 */
static mbedtls_entropy_context EntropyContext;

/**
 * @brief Define a flag that can be used to detect failures in the operation
 */
static volatile int testFailed = 1;


#ifndef ALGO_ENABLED
#define ALGO_ENABLED 1
#endif /* ALGO_ENABLED */

#ifndef ALGO_DISABLED
#define ALGO_DISABLED 0
#endif /* ALGO_DISABLED */

#ifndef ALGO_AES_ENCRYPT
#define ALGO_AES_ENCRYPT ALGO_ENABLED
#endif /* ALGO_AES_ENCRYPT */

#ifndef ALGO_AES_DECRYPT
#define ALGO_AES_DECRYPT ALGO_ENABLED
#endif /* ALGO_AES_DECRYPT */

#ifndef ALGO_CCM_ENCRYPT
#define ALGO_CCM_ENCRYPT ALGO_ENABLED
#endif /* ALGO_CCM_ENCRYPT */

#ifndef ALGO_CCM_DECRYPT
#define ALGO_CCM_DECRYPT ALGO_ENABLED
#endif /* ALGO_CCM_DECRYPT */

#ifndef ALGO_HMAC
#define ALGO_HMAC ALGO_ENABLED
#endif /* ALGO_HMAC */

#ifndef ALGO_SHA
#define ALGO_SHA ALGO_ENABLED
#endif /* ALGO_SHA */

#ifndef ALGO_CMAC
#define ALGO_CMAC ALGO_ENABLED
#endif /* ALGO_CMAC */

#ifndef ALGO_ECDH_GENERATION
#define ALGO_ECDH_GENERATION ALGO_ENABLED
#endif /* ALGO_ECDH_GENERATION */

#ifndef ALGO_ECDH_EXCHANGE
#define ALGO_ECDH_EXCHANGE ALGO_ENABLED
#endif /* ALGO_ECDH_EXCHANGE */


/**
 * @brief Enumeration defining the operations we will measure
 */
typedef enum {
    AES_Encrypt,
    AES_Decrypt,
    CCM_Encrypt,
    CCM_Decrypt,
    SHA_Signature,
    HMAC_Authentication,
    CMAC_Authentication,
    ECDH_KeyGenerationAlice,
    ECDH_KeyGenerationBob,
    ECDH_KeyExchange,
    LastOperationMarker
} tOperationType;

/**
 * Forward declaration of the operation structure
 */
typedef struct tOperationStructure tOperation, *pOperation;

/**
 * Define a function prototype for each of the cryptographic functions.
 */
typedef uint32_t (*tEncryptionFunction)(pOperation context);

/**
 * Define a structure we can use to collate the results for each operation
 */
struct tOperationStructure {
    tEncryptionFunction action;
    void                *userData;
    uint32_t            status;
    uint32_t            start;
    uint32_t            finish;
    char                *text;
};

/**
 * Define the prototypes for each of our operations
 */
static uint32_t doAESEncrypt(pOperation context);
static uint32_t doAESDecrypt(pOperation context);
static uint32_t doCCMEncryptAndSign(pOperation context);
static uint32_t doCCMDecryptAndAuthenticate(pOperation context);
static uint32_t doSHASignature(pOperation context);
static uint32_t doHMACAuthentication(pOperation context);
static uint32_t doCMACAuthentication(pOperation context);
static uint32_t doECDHKeyGeneration(pOperation context);
static uint32_t doECDHKeyExchange(pOperation context);

/** Define the standard AES block size in bytes, it is always 128 bits */
#define AES_BLOCK_SIZE_BYTES        16

/**
 * Define a data block type for use with AES-CBC encryption
 */
typedef struct {
    uint32_t    keySize;
    uint8_t     text[AES_BLOCK_SIZE_BYTES];
    uint8_t     iv[AES_BLOCK_SIZE_BYTES];
    uint8_t     key[AES_BLOCK_SIZE_BYTES];
    uint8_t     cipher[AES_BLOCK_SIZE_BYTES];
} tAES_CBC_Structure;

/**
 * Define a set of data we can use to verify one block being encrypted
 * and decrypted using AES-CBC
 */
static tAES_CBC_Structure aesUserdata = {
        .keySize    = 128,
        .text       = {
                0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
                0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A
        },
        .iv         = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
        },
        .key        = {
                0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
        },
        .cipher     = {
                0x76, 0x49, 0xAB, 0xAC, 0x81, 0x19, 0xB2, 0x46,
                0xCE, 0xE9, 0x8E, 0x9B, 0x12, 0xE9, 0x19, 0x7D
        }
};

/**
 * Define data structures we can use in our ECDH operations
 */
typedef struct {
    mbedtls_mpi privateKey;
    mbedtls_ecp_point publicKey;
    mbedtls_mpi sharedSecret;
} tECDH_Context;

typedef struct {
    tECDH_Context Alice;
    tECDH_Context Bob;
} tECDH_Structure;

static tECDH_Structure ecdhUserdata;

/**
 * Define data structures we can use in our CCM operations
 */
#define CCM_TAG_LENGTH  6
#define CCM_KEY_LENGTH  16
#define CCM_IV_LENGTH   8
#define CCM_AD_LENGTH   16
#define CCM_MSG_LENGTH  16
#define CCM_RES_LENGTH  (CCM_MSG_LENGTH + CCM_TAG_LENGTH)

typedef struct {
    uint8_t key[CCM_KEY_LENGTH];
    uint8_t iv[CCM_IV_LENGTH];
    uint8_t ad[CCM_AD_LENGTH];
    uint8_t msg[CCM_MSG_LENGTH];
    uint8_t res[CCM_RES_LENGTH];
} tCCM_Structure;

static tCCM_Structure ccmUserData = {
    .key = {
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F
    },
    .iv = {
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
    },
    .ad = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    },
    .msg = {
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F
    },
    .res = {
        0xD2, 0xA1, 0xF0, 0xE0, 0x51, 0xEA, 0x5F, 0x62,
        0x08, 0x1A, 0x77, 0x92, 0x07, 0x3D, 0x59, 0x3D,
        0x1F, 0xC6, 0x4F, 0xBF, 0xAC, 0xCD
    }
};

/**
 * Define data structures we can use in our CMAC operations
 */
#define CMAC_ERROR_INVALID_CIPHER   0x00000001

#define CMAC_KEY_LENGTH             16
#define CMAC_MSG_LENGTH             16
#define CMAC_TAG_LENGTH             16

typedef struct {
    mbedtls_cipher_type_t cipher;
    uint8_t key[CMAC_KEY_LENGTH];
    uint8_t msg[CMAC_MSG_LENGTH];
    uint8_t tag[CMAC_TAG_LENGTH];
} tCMAC_Structure;

static tCMAC_Structure cmacUserData = {
    .cipher = MBEDTLS_CIPHER_AES_128_ECB,
    .key = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    },
    .msg = {
        0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
        0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A
    },
    .tag = {
        0x07, 0x0A, 0x16, 0xB4, 0x6B, 0x4D, 0x41, 0x44,
        0xF7, 0x9B, 0xDD, 0x9D, 0xD0, 0x4A, 0x28, 0x7C
    }
};

/**
 * Define data structures we can use for our SHA256 operations
 */

/** Define the digest length in bytes for SHA256 */
#define SHA256_DIGEST_LENGTH    32

typedef struct {
    uint32_t        messageLength;
    unsigned char   *message;
    uint8_t         digest[SHA256_DIGEST_LENGTH];
} tSHA_Structure;

static tSHA_Structure shaUserData = {
    .messageLength = 56,
    .message = (unsigned char *)
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    .digest = {
        0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
        0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
        0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
        0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1
    }
};

/**
 * Define data structures we can use in our HMAC operations
 */
#define HMAC_ERROR_INVALID_MODE     0x00000001

#define HMAC_KEY_LENGTH             64
#define HMAC_MSG_LENGTH             34
#define HMAC_MAC_LENGTH             32

typedef struct {
    uint8_t key[HMAC_KEY_LENGTH];
    uint8_t msg[HMAC_MSG_LENGTH];
    uint8_t mac[HMAC_MAC_LENGTH];
} tHMAC_Structure;

static tHMAC_Structure hmacUserData = {
    .key = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
    },
    .msg = {
        0x53, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x20, 0x6D,
        0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x20, 0x66,
        0x6F, 0x72, 0x20, 0x6B, 0x65, 0x79, 0x6C, 0x65,
        0x6E, 0x3D, 0x62, 0x6C, 0x6F, 0x63, 0x6B, 0x6C,
        0x65, 0x6E
    },
    .mac = {
        0x8B, 0xB9, 0xA1, 0xDB, 0x98, 0x06, 0xF2, 0x0D,
        0xF7, 0xF7, 0x7B, 0x82, 0x13, 0x8C, 0x79, 0x14,
        0xD1, 0x74, 0xD5, 0x9E, 0x13, 0xDC, 0x4D, 0x01,
        0x69, 0xC9, 0x05, 0x7B, 0x13, 0x3E, 0x1D, 0x62
    }
};

/**
 * Define the operation table, this provides the sequencing for the
 * each of the operations and stores the result information.
 */
static tOperation operations[] = {
        { doAESEncrypt, &aesUserdata, 0, 0, 0, "AES Encryption     " },
        { doAESDecrypt, &aesUserdata, 0, 0, 0, "AES Decryption     " },
        { doCCMEncryptAndSign, &ccmUserData, 0, 0, 0, "CCM Signature      " },
        { doCCMDecryptAndAuthenticate, &ccmUserData, 0, 0, 0, "CCM Authentication " },
        { doSHASignature, &shaUserData, 0, 0, 0, "SHA256 Signature   " },
        { doHMACAuthentication, &hmacUserData, 0, 0, 0, "HMAC Authentication" },
        { doCMACAuthentication, &cmacUserData, 0, 0, 0, "CMAC Authentication" },
        { doECDHKeyGeneration, &ecdhUserdata.Alice, 0, 0, 0, "ECDH Key Alice     " },
        { doECDHKeyGeneration, &ecdhUserdata.Bob, 0, 0, 0, "ECDH Key Bob       " },
        { doECDHKeyExchange, &ecdhUserdata, 0, 0, 0, "ECDH Key Exchange  " }
};

/**
 * @brief Define a prototype for a generalized function to be timed.
 */
typedef CCStatus (*timedFunction)(void);

/**
 * @brief Helper routine which initializes the CC312 Library.
 */
static CClibRetCode_t initializeCC312(void)
{
    /* Set up the RND context with default values */
    RNDContext.rndState = &DRBGContext;
    RNDContext.entropyCtx = &EntropyContext;

    /* Initialize CC library */
    return CC_LibInit(&RNDContext, &RNDWorkBuffer);
}


/**
 * @brief Interrupt Service Routine for the SysTick. This will count down
 * in seconds.
 */
void SysTick_Handler(void)
{
    tickCounter--;
}

/**
 * @brief Helper routine which will read the SysTick and provide unique time
 * within the defined window. Note that this value counts down over time.
 */
static uint32_t ticks(void)
{
    uint32_t v1 = SysTick->VAL;
    uint32_t counter = tickCounter;
    uint32_t v2 = SysTick->VAL;

    /* Handle the case where the tick interrupt has occurred during a read.
     * Just read again as we know this must be faster than the tick interrupt
     * firing again.
     */
    if (v2 > v1)
    {
        return ticks();
    }

    return (counter * SystemCoreClock) + v1;
}


static uint32_t doAESEncrypt(pOperation context)
{
#if (ALGO_AES_ENCRYPT == ALGO_ENABLED)

    tAES_CBC_Structure *userData = (tAES_CBC_Structure *) context->userData;

    /* Copy the plain text into a working buffer */
    uint8_t text[AES_BLOCK_SIZE_BYTES];
    memcpy(text, userData->text, AES_BLOCK_SIZE_BYTES);

    /* Copy the iv into a working buffer */
    uint8_t iv[AES_BLOCK_SIZE_BYTES];
    memcpy(iv, userData->iv, AES_BLOCK_SIZE_BYTES);

    /* Declare a local context and initialize it for use */
    mbedtls_aes_context aesContext;
    mbedtls_aes_init(&aesContext);

    /* Set the key we will use */
    uint32_t status = mbedtls_aes_setkey_enc(&aesContext, userData->key, userData->keySize);
    if (status == 0)
    {
        context->start = ticks();
        status = mbedtls_aes_crypt_cbc(
                &aesContext, MBEDTLS_AES_ENCRYPT, AES_BLOCK_SIZE_BYTES, iv, text, text);
        context->finish = ticks();

        if (status == 0)
        {
            status = memcmp(text, userData->cipher, AES_BLOCK_SIZE_BYTES);
        }
    }

    /* Always free the context when finished with it */
    mbedtls_aes_free(&aesContext);

    return status;

#else /* (ALGO_AES_ENCRYPT == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_AES_ENCRYPT == ALGO_ENABLED) */
}

static uint32_t doAESDecrypt(pOperation context)
{
#if (ALGO_AES_DECRYPT == ALGO_ENABLED)

    tAES_CBC_Structure *userData = (tAES_CBC_Structure *) context->userData;

    /* Copy the plain text into a working buffer */
    uint8_t text[AES_BLOCK_SIZE_BYTES];
    memcpy(text, userData->cipher, AES_BLOCK_SIZE_BYTES);

    /* Copy the iv into a working buffer */
    uint8_t iv[AES_BLOCK_SIZE_BYTES];
    memcpy(iv, userData->iv, AES_BLOCK_SIZE_BYTES);

    /* Declare a local context and initialize it for use */
    mbedtls_aes_context aesContext;
    mbedtls_aes_init(&aesContext);

    /* Set the key we will use */
    uint32_t status = mbedtls_aes_setkey_dec(&aesContext, userData->key, userData->keySize);
    if (status == 0)
    {
        context->start = ticks();
        status = mbedtls_aes_crypt_cbc(
                &aesContext, MBEDTLS_AES_DECRYPT, AES_BLOCK_SIZE_BYTES, iv, text, text);
        context->finish = ticks();

        if (status == 0)
        {
            status = memcmp(text, userData->text, AES_BLOCK_SIZE_BYTES);
        }
    }

    /* Always free the context when finished with it */
    mbedtls_aes_free(&aesContext);

    return status;

#else /* (ALGO_AES_DECRYPT == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_AES_DECRYPT == ALGO_ENABLED) */
}

static uint32_t doCCMEncryptAndSign(pOperation context)
{
#if (ALGO_CCM_ENCRYPT == ALGO_ENABLED)

    tCCM_Structure *userData = (tCCM_Structure *) context->userData;

    uint8_t *buffer = malloc(CCM_RES_LENGTH);
    mbedtls_ccm_context *pCtx = malloc(sizeof(mbedtls_ccm_context));
    mbedtls_ccm_init(pCtx);

    /* Encrypt the message */
    uint32_t status = mbedtls_ccm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, userData->key,
            CCM_KEY_LENGTH * 8);
    if (status == 0)
    {
        context->start = ticks();
        status = mbedtls_ccm_encrypt_and_tag(pCtx, CCM_MSG_LENGTH, userData->iv,
                CCM_IV_LENGTH, userData->ad, CCM_AD_LENGTH, userData->msg, &buffer[0],
                &buffer[CCM_MSG_LENGTH], CCM_TAG_LENGTH);
        context->finish = ticks();

        if (status == 0)
        {
            status = memcmp(buffer, userData->res, CCM_RES_LENGTH);
        }
    }

    mbedtls_ccm_free(pCtx);
    free(pCtx);
    free(buffer);

    return status;

#else /* (ALGO_CCM_ENCRYPT == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_CCM_ENCRYPT == ALGO_ENABLED) */
}

static uint32_t doCCMDecryptAndAuthenticate(pOperation context)
{
#if (ALGO_CCM_DECRYPT == ALGO_ENABLED)

    tCCM_Structure *userData = (tCCM_Structure *) context->userData;

    uint8_t *buffer = malloc(CCM_MSG_LENGTH);
    mbedtls_ccm_context *pCtx = malloc(sizeof(mbedtls_ccm_context));
    mbedtls_ccm_init(pCtx);

    /* Decrypt and authenticate the message */
    uint32_t status = mbedtls_ccm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, userData->key,
            CCM_KEY_LENGTH * 8);
    if (status == 0)
    {
        context->start = ticks();
        status = mbedtls_ccm_auth_decrypt(pCtx, CCM_MSG_LENGTH, userData->iv,
                CCM_IV_LENGTH, userData->ad, CCM_AD_LENGTH, userData->res, buffer,
                &userData->res[CCM_MSG_LENGTH], CCM_TAG_LENGTH);
        context->finish = ticks();

        if (status == 0)
        {
            status = memcmp(buffer, userData->msg, CCM_MSG_LENGTH);
        }
    }

    mbedtls_ccm_free(pCtx);
    free(pCtx);
    free(buffer);

    return status;

#else /* (ALGO_CCM_DECRYPT == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_CCM_DECRYPT == ALGO_ENABLED) */
}

static uint32_t doSHASignature(pOperation context)
{
#if (ALGO_SHA == ALGO_ENABLED)

    tSHA_Structure *userData = (tSHA_Structure *) context->userData;

    unsigned char output[SHA256_DIGEST_LENGTH];

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    memset(output, 0, SHA256_DIGEST_LENGTH);
    context->start = ticks();
    uint32_t status = mbedtls_sha256_starts_ret(&ctx, 0);
    if (status == 0)
    {
        status = mbedtls_sha256_update_ret(
                &ctx, (const unsigned char *) userData->message, userData->messageLength);
        if (status == 0)
        {
            status = mbedtls_sha256_finish_ret(&ctx, output);
        }
    }
    context->finish = ticks();

    if (status == 0)
    {
        status = memcmp(output, userData->digest, SHA256_DIGEST_LENGTH);
    }

    mbedtls_sha256_free(&ctx);

    return status;

#else /* (ALGO_SHA == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_SHA == ALGO_ENABLED) */
}

static uint32_t doHMACAuthentication(pOperation context)
{
#if (ALGO_HMAC == ALGO_ENABLED)

    tHMAC_Structure *userData = (tHMAC_Structure *) context->userData;

    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_string("SHA256");
    if (md_info == NULL)
    {
        return HMAC_ERROR_INVALID_MODE;
    }

    unsigned char *buffer = malloc(HMAC_MAC_LENGTH);
    context->start = ticks();
    uint32_t status = mbedtls_md_hmac(md_info,
            userData->key, HMAC_KEY_LENGTH, userData->msg, HMAC_MSG_LENGTH, buffer);
    context->finish = ticks();
    if (status == 0)
    {
        status = memcmp(buffer, userData->mac, HMAC_MAC_LENGTH);
    }

    return status;

#else /* (ALGO_HMAC == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_HMAC == ALGO_ENABLED) */
}

static uint32_t doCMACAuthentication(pOperation context)
{
#if (ALGO_CMAC == ALGO_ENABLED)

    tCMAC_Structure *userData = (tCMAC_Structure *) context->userData;

    const mbedtls_cipher_info_t *cipherInfo =
            mbedtls_cipher_info_from_type(userData->cipher);
    if (cipherInfo == NULL)
    {
        return CMAC_ERROR_INVALID_CIPHER;
    }

    uint8_t *buffer = malloc(CMAC_TAG_LENGTH);
    context->start = ticks();
    uint32_t status = mbedtls_cipher_cmac(cipherInfo,
            userData->key, CMAC_KEY_LENGTH * 8, userData->msg, CMAC_MSG_LENGTH, buffer);
    context->finish = ticks();
    if (status == 0)
    {
        status = memcmp(buffer, userData->tag, CMAC_TAG_LENGTH);
    }

    free(buffer);

    return status;

#else /* (ALGO_CMAC == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_CMAC == ALGO_ENABLED) */
}

static uint32_t doECDHKeyGeneration(pOperation context)
{
#if (ALGO_ECDH_GENERATION == ALGO_ENABLED)

    tECDH_Context *userData = (tECDH_Context *) context->userData;

    mbedtls_ecp_group *pGrp = malloc(sizeof(mbedtls_ecp_group));
    mbedtls_ecp_group_init(pGrp);

    uint32_t status = mbedtls_ecp_group_load(pGrp, MBEDTLS_ECP_DP_SECP256R1);
    if (status == 0)
    {
        mbedtls_ecp_point_init(&userData->publicKey);
        mbedtls_mpi_init(&userData->privateKey);

        context->start = ticks();
        status = mbedtls_ecdh_gen_public(
                pGrp, &userData->privateKey, &userData->publicKey,
                mbedtls_ctr_drbg_random, &DRBGContext);
        context->finish = ticks();

        if (status == 0)
        {
            status = mbedtls_ecp_is_zero(&userData->publicKey);
        }
    }

    free(pGrp);

    return status;

#else /* (ALGO_ECDH_GENERATION == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_ECDH_GENERATION == ALGO_ENABLED) */
}

static uint32_t doECDHKeyExchange(pOperation context)
{
#if (ALGO_ECDH_EXCHANGE == ALGO_ENABLED)

    tECDH_Structure *userData = (tECDH_Structure *) context->userData;

    mbedtls_ecp_group *pGrp = malloc(sizeof(mbedtls_ecp_group));
    mbedtls_ecp_group_init(pGrp);
    uint32_t status = mbedtls_ecp_group_load(pGrp, MBEDTLS_ECP_DP_SECP256R1);

    if (status == 0)
    {
        /* Compute the shared secret for Alice */
        context->start = ticks();
        status = mbedtls_ecdh_compute_shared(pGrp,
                &userData->Alice.sharedSecret,
                &userData->Bob.publicKey, &userData->Alice.privateKey,
                NULL, NULL);
        context->finish = ticks();

        /* Verification code */
        if (status == 0)
        {
            /* Compute the shared secret for Bob */
            status = mbedtls_ecdh_compute_shared(pGrp,
                    &userData->Bob.sharedSecret,
                    &userData->Alice.publicKey, &userData->Bob.privateKey,
                    NULL, NULL);

            if (status == 0)
            {
                /* Compare the secrets */
                status = mbedtls_mpi_cmp_mpi(
                        &userData->Alice.sharedSecret, &userData->Bob.sharedSecret);
            }
        }
    }

    free(pGrp);

    return status;

#else /* (ALGO_ECDH_EXCHANGE == ALGO_ENABLED) */
    return 0;
#endif /* (ALGO_ECDH_EXCHANGE == ALGO_ENABLED) */
}

static uint32_t PerformAllQuickstartOperations(void)
{
    uint32_t status = 0;

    for (tOperationType op = AES_Encrypt; op < LastOperationMarker; op++)
    {
        pOperation current = &operations[op];

        current->status = (*(current->action))(current);
        status |= current->status;
    }

    return status;
}

/**
 * @brief Helper function to execute a function and time its duration.
 * @param [in] function The function to be called.
 * @return The status of the called function.
 */
static CCStatus measureDuration(timedFunction function)
{
    /* Initialize the counter to zero to ensure a full cycle before trigger */
    SysTick->VAL = 0;
    tickCounter = MAX_TICK_DURATION;

    /* Start the counter running, need an interrupt, use internal clock */
    SysTick->CTRL = ((1 << SysTick_CTRL_CLKSOURCE_Pos) |
                     (1 << SysTick_CTRL_TICKINT_Pos)   |
                     (1 << SysTick_CTRL_ENABLE_Pos));

    uint32_t startTick = ticks();
    CCStatus status = function();
    uint32_t endTick = ticks();

    /* Stop the SysTick, measurement is complete */
    SysTick->CTRL = 0;

    swmLogInfo("Elapsed duration: ~%d cycles (%d ticks)\r\n",
            startTick - endTick, MAX_TICK_DURATION - tickCounter);

    for (tOperationType op = AES_Encrypt; op < LastOperationMarker; op++)
    {
        pOperation current = &operations[op];
        swmLogInfo("%s (0x%08x) - ~%d cycles\r\n", current->text, current->status, current->start - current->finish);
    }

    return status;
}

/**
 * @brief This routine provides an easy place to set a breakpoint when running
 * in the debugger.
 */
static void breakpointHook()
{
    while (true)
    {
        /* Do nothing */
    }
}

/* Function header is provided in app.h */
int main(void)
{
    initialize();

    swmTrace_init(traceOptions, 5);
    swmLogInfo("Quick start recipes for cryptographic functions.\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("QuickStart\r\n");
    }
    else
    {
        CCStatus status = measureDuration(PerformAllQuickstartOperations);
        if (status == CC_OK)
        {
            swmLogTestPass("QuickStart\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("QuickStart\r\n");
        }
    }

    breakpointHook();

    return 0;
}
