/**
 * @file app.c
 * @brief Main application, sets up and executes the cryptographic operations.
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

#include "mbedtls/aes.h"

#ifdef GET_AES_OFB_FROM_EXTRA
#include "mbedtls_cc_aes_crypt_additional.h"
#endif

/* Test vectors have been derived from the following publications:
 *  https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_ECB.pdf
 *  https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CBC.pdf
 *  https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CTR.pdf
 *  https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_OFB.pdf
 */
#include "AES_ECB_vectors.h"
#include "AES_CBC_vectors.h"
#include "AES_CTR_vectors.h"
#include "AES_OFB_vectors.h"

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


/**
 * @brief Enumeration type describing the supported AES modes.
 */
typedef enum {
    AES_ECB,
    AES_CBC,
    AES_OFB,
    AES_CTR
} tAESMode;

/** Define the standard AES block size in bytes, it is always 128 bits */
#define AES_BLOCK_SIZE  16

/** Define the standard IV size in bytes, it is always 128 bits */
#define AES_IV_SIZE     16

/** Define the standard Nonce size in bytes, it is always 128 bits */
#define AES_NONCE_SIZE  16

/* Define error cases */
#define AES_ERROR_KEY_FAILURE       0xF0000000
#define AES_ERROR_BAD_PARAMETER     0xF0000001
#define AES_ERROR_ENCRYPT_FAILURE   0xF0000002
#define AES_ERROR_DECRYPT_FAILURE   0xF0000003
#define AES_ERROR_DATA_MISMATCH     0xF0000004


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


/**
 * @brief This function performs a single encryption operation based
 * on the provided parameters.
 *
 * @param mode The specific AESmode to use.
 * @param key_size The size of the provided key in bytes.
 * @param data_size the size of the data to be encrypted in bytes.
 * @param key Pointer to a buffer holding the key.
 * @param plain Pointer to a buffer holding the plain text.
 * @param cipher Pointer to a buffer holding the expected output.
 * @param iv Pointer to a buffer holding the initial value,
 * used in CBC & OFB modes.
 * @param nonce Pointer to a buffer holding the nonce, used in CTR mode.
 * @return Zero if the operation is successful,
 * return code of failure if there is a problem.
 */
static uint32_t Perform_AES_Encrypt(
        const tAESMode mode,
        const uint32_t key_size,
        const uint32_t data_size,
        const uint8_t *key,
        const uint8_t *plain,
        const uint8_t *cipher,
        const uint8_t *iv,
        const uint8_t *nonce)
{
    /* Buffer is used as temporary storage in all cases */
    uint8_t buffer[AES_BLOCK_SIZE];

    /* TempIV is only used in CBC & OFB modes */
    uint8_t tempIV[AES_IV_SIZE];

    /* Offset is only used in CTR mode */
    size_t offset = 0;

    /* The nonceCounter is only used in CTR mode */
    unsigned char nonceCounter[AES_NONCE_SIZE];

    /* The streamBlock is only used in CTR mode */
    unsigned char streamBlock[AES_NONCE_SIZE] = {0};

    /* Declare a local context and initialize it for use */
    mbedtls_aes_context context;
    mbedtls_aes_init(&context);

    /* Set the key we will use */
    if (mbedtls_aes_setkey_enc(&context, key, key_size) != 0)
    {
        return AES_ERROR_KEY_FAILURE;
    }

    /* Get a local working copy of the IV, for CBC & OFB modes */
    if (iv != NULL)
    {
        memcpy(tempIV, iv, sizeof(tempIV));
    }

    /* Get a local working copy of the Nonce Counter, for CTR mode */
    if (nonce != NULL)
    {
        memcpy(nonceCounter, nonce, sizeof(nonceCounter));
    }

    /* Loop through each block in the incoming text */
    for (uint32_t index = 0; index < data_size; index += AES_BLOCK_SIZE)
    {
        /* Copy the plain text into a working buffer */
        memcpy(buffer, &plain[index], AES_BLOCK_SIZE);

        /* Perform cryptographic operation */
        switch (mode)
        {
        case AES_ECB:
            if (mbedtls_aes_crypt_ecb(&context, MBEDTLS_AES_ENCRYPT, buffer, buffer) != 0)
            {
                return AES_ERROR_ENCRYPT_FAILURE;
            }
            break;

        case AES_CBC:
            if (mbedtls_aes_crypt_cbc(&context, MBEDTLS_AES_ENCRYPT, sizeof(buffer), tempIV, buffer, buffer) != 0)
            {
                return AES_ERROR_ENCRYPT_FAILURE;
            }
            break;

#if defined(GET_AES_OFB_FROM_EXTRA)
        case AES_OFB:
            if (mbedtls_aes_crypt_ofb(&context, sizeof(buffer), 0, tempIV, streamBlock, buffer, buffer) != 0)
            {
                return AES_ERROR_ENCRYPT_FAILURE;
            }
            break;
#else
        case AES_OFB:
            if (mbedtls_aes_crypt_ofb(&context, sizeof(buffer), 0, tempIV, buffer, buffer) != 0)
            {
                return AES_ERROR_ENCRYPT_FAILURE;
            }
            break;
#endif

        case AES_CTR:
            if (mbedtls_aes_crypt_ctr(&context, sizeof(buffer), &offset, nonceCounter, streamBlock, buffer, buffer) != 0)
            {
                return AES_ERROR_ENCRYPT_FAILURE;
            }
            break;


        default:
            return AES_ERROR_BAD_PARAMETER;
        }

        /* Compare result */
        if (memcmp(buffer, &cipher[index], AES_BLOCK_SIZE) != 0)
        {
            return AES_ERROR_DATA_MISMATCH;
        }
    }

    /* Always free the context when finished with it */
    mbedtls_aes_free(&context);

    return CC_OK;
}


/**
 * @brief This function performs a single decryption operation based
 * on the provided parameters.
 *
 * @param mode The specific AESmode to use.
 * @param key_size The size of the provided key in bytes.
 * @param data_size the size of the data to be encrypted in bytes.
 * @param key Pointer to a buffer holding the key.
 * @param plain Pointer to a buffer holding the expected plain text.
 * @param cipher Pointer to a buffer holding the encrypted input.
 * @param iv Pointer to a buffer holding the initial value,
 * used in CBC & OFB modes.
 * @param nonce Pointer to a buffer holding the nonce, used in CTR mode.
 * @return Zero if the operation is successful,
 * return code of failure if there is a problem.
 */
static uint32_t Perform_AES_Decrypt(
        const tAESMode mode,
        const uint32_t key_size,
        const uint32_t data_size,
        const uint8_t *key,
        const uint8_t *plain,
        const uint8_t *cipher,
        const uint8_t *iv,
        const uint8_t *nonce)
{
    /* Buffer is used as temporary storage in all cases */
    uint8_t buffer[AES_BLOCK_SIZE];

    /* TempIV is only used in CBC & OFB modes */
    uint8_t tempIV[AES_IV_SIZE];

    /* Offset is only used in CTR mode */
    size_t offset = 0;

    /* The nonceCounter is only used in CTR mode */
    unsigned char nonceCounter[AES_NONCE_SIZE];

    /* The streamBlock is only used in CTR mode */
    unsigned char streamBlock[AES_NONCE_SIZE] = {0};

    /* Declare a local context and initialize it for use */
    mbedtls_aes_context context;
    mbedtls_aes_init(&context);

    /* Set the key we will use */
    if (mbedtls_aes_setkey_dec(&context, key, key_size) != 0)
    {
        return AES_ERROR_KEY_FAILURE;
    }

    /* Get a local working copy of the IV, for CBC & OFB modes */
    if (iv != NULL)
    {
        memcpy(tempIV, iv, sizeof(tempIV));
    }

    /* Get a local working copy of the Nonce Counter, for CTR mode */
    if (nonce != NULL)
    {
        memcpy(nonceCounter, nonce, sizeof(nonceCounter));
    }

    /* Loop through each block in the incoming text */
    for (uint32_t index = 0; index < data_size; index += AES_BLOCK_SIZE)
    {
        /* Copy the plain text into a working buffer */
        memcpy(buffer, &cipher[index], AES_BLOCK_SIZE);

        /* Perform cryptographic operation */
        switch (mode)
        {
        case AES_ECB:
            if (mbedtls_aes_crypt_ecb(&context, MBEDTLS_AES_DECRYPT, buffer, buffer) != 0)
            {
                return AES_ERROR_DECRYPT_FAILURE;
            }
            break;

        case AES_CBC:
            if (mbedtls_aes_crypt_cbc(&context, MBEDTLS_AES_DECRYPT, sizeof(buffer), tempIV, buffer, buffer) != 0)
            {
                return AES_ERROR_DECRYPT_FAILURE;
            }
            break;

#if defined(GET_AES_OFB_FROM_EXTRA)
        case AES_OFB:
            if (mbedtls_aes_crypt_ofb(&context, sizeof(buffer), 0, tempIV, streamBlock, buffer, buffer) != 0)
            {
                return AES_ERROR_ENCRYPT_FAILURE;
            }
            break;
#else
        case AES_OFB:
            if (mbedtls_aes_crypt_ofb(&context, sizeof(buffer), 0, tempIV, buffer, buffer) != 0)
            {
                return AES_ERROR_ENCRYPT_FAILURE;
            }
            break;
#endif

        case AES_CTR:
            if (mbedtls_aes_crypt_ctr(&context, sizeof(buffer), &offset, nonceCounter, streamBlock, buffer, buffer) != 0)
            {
                return AES_ERROR_DECRYPT_FAILURE;
            }
            break;


        default:
            return AES_ERROR_BAD_PARAMETER;
        }

        /* Compare result */
        if (memcmp(buffer, &plain[index], AES_BLOCK_SIZE) != 0)
        {
            return AES_ERROR_DATA_MISMATCH;
        }
    }

    /* Always free the context when finished with it */
    mbedtls_aes_free(&context);

    return CC_OK;
}

/**
 * @brief Sample function demonstrating various encryption and decryption
 * operations using AES-128.
 * @return 0 if the operations succeed.
 */
static uint32_t Perform_AES_EncryptDecrypt(void)
{
    uint32_t status;

    /* Encryption examples */

    /* ECB */
    if ((status = Perform_AES_Encrypt(AES_ECB, AES_ECB_128_KEY_SIZE,
            AES_ECB_PLAIN_SIZE, AES_ECB_128_KEY, AES_ECB_PLAIN_TEXT,
            AES_ECB_128_CIPHER, 0, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_ECB, AES_ECB_192_KEY_SIZE,
            AES_ECB_PLAIN_SIZE, AES_ECB_192_KEY, AES_ECB_PLAIN_TEXT,
            AES_ECB_192_CIPHER, 0, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_ECB, AES_ECB_256_KEY_SIZE,
            AES_ECB_PLAIN_SIZE, AES_ECB_256_KEY, AES_ECB_PLAIN_TEXT,
            AES_ECB_256_CIPHER, 0, 0)) != CC_OK)
        return status;

    /* CBC */
    if ((status = Perform_AES_Encrypt(AES_CBC, AES_CBC_128_KEY_SIZE,
            AES_CBC_PLAIN_SIZE, AES_CBC_128_KEY, AES_CBC_PLAIN_TEXT,
            AES_CBC_128_CIPHER, AES_CBC_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_CBC, AES_CBC_192_KEY_SIZE,
            AES_CBC_PLAIN_SIZE, AES_CBC_192_KEY, AES_CBC_PLAIN_TEXT,
            AES_CBC_192_CIPHER, AES_CBC_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_CBC, AES_CBC_256_KEY_SIZE,
            AES_CBC_PLAIN_SIZE, AES_CBC_256_KEY, AES_CBC_PLAIN_TEXT,
            AES_CBC_256_CIPHER, AES_CBC_IV, 0)) != CC_OK)
        return status;

    /* OFB */
    if ((status = Perform_AES_Encrypt(AES_OFB, AES_OFB_128_KEY_SIZE,
            AES_OFB_PLAIN_SIZE, AES_OFB_128_KEY, AES_OFB_PLAIN_TEXT,
            AES_OFB_128_CIPHER, AES_OFB_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_OFB, AES_OFB_192_KEY_SIZE,
            AES_OFB_PLAIN_SIZE, AES_OFB_192_KEY, AES_OFB_PLAIN_TEXT,
            AES_OFB_192_CIPHER, AES_OFB_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_OFB, AES_OFB_256_KEY_SIZE,
            AES_OFB_PLAIN_SIZE, AES_OFB_256_KEY, AES_OFB_PLAIN_TEXT,
            AES_OFB_256_CIPHER, AES_OFB_IV, 0)) != CC_OK)
        return status;

    /* CTR */
    if ((status = Perform_AES_Encrypt(AES_CTR, AES_CTR_128_KEY_SIZE,
            AES_CTR_PLAIN_SIZE, AES_CTR_128_KEY, AES_CTR_PLAIN_TEXT,
            AES_CTR_128_CIPHER, 0, AES_CTR_NONCE)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_CTR, AES_CTR_192_KEY_SIZE,
            AES_CTR_PLAIN_SIZE, AES_CTR_192_KEY, AES_CTR_PLAIN_TEXT,
            AES_CTR_192_CIPHER, 0, AES_CTR_NONCE)) != CC_OK)
        return status;

    if ((status = Perform_AES_Encrypt(AES_CTR, AES_CTR_256_KEY_SIZE,
            AES_CTR_PLAIN_SIZE, AES_CTR_256_KEY, AES_CTR_PLAIN_TEXT,
            AES_CTR_256_CIPHER, 0, AES_CTR_NONCE)) != CC_OK)
        return status;

    /* Decryption examples */

    /* ECB */
    if ((status = Perform_AES_Decrypt(AES_ECB, AES_ECB_128_KEY_SIZE,
            AES_ECB_PLAIN_SIZE, AES_ECB_128_KEY, AES_ECB_PLAIN_TEXT,
            AES_ECB_128_CIPHER, 0, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_ECB, AES_ECB_192_KEY_SIZE,
            AES_ECB_PLAIN_SIZE, AES_ECB_192_KEY, AES_ECB_PLAIN_TEXT,
            AES_ECB_192_CIPHER, 0, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_ECB, AES_ECB_256_KEY_SIZE,
            AES_ECB_PLAIN_SIZE, AES_ECB_256_KEY, AES_ECB_PLAIN_TEXT,
            AES_ECB_256_CIPHER, 0, 0)) != CC_OK)
        return status;

    /* CBC */
    if ((status = Perform_AES_Decrypt(AES_CBC, AES_CBC_128_KEY_SIZE,
            AES_CBC_PLAIN_SIZE, AES_CBC_128_KEY, AES_CBC_PLAIN_TEXT,
            AES_CBC_128_CIPHER, AES_CBC_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_CBC, AES_CBC_192_KEY_SIZE,
            AES_CBC_PLAIN_SIZE, AES_CBC_192_KEY, AES_CBC_PLAIN_TEXT,
            AES_CBC_192_CIPHER, AES_CBC_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_CBC, AES_CBC_256_KEY_SIZE,
            AES_CBC_PLAIN_SIZE, AES_CBC_256_KEY, AES_CBC_PLAIN_TEXT,
            AES_CBC_256_CIPHER, AES_CBC_IV, 0)) != CC_OK)
        return status;

    /* OFB */
    if ((status = Perform_AES_Decrypt(AES_OFB, AES_OFB_128_KEY_SIZE,
            AES_OFB_PLAIN_SIZE, AES_OFB_128_KEY, AES_OFB_PLAIN_TEXT,
            AES_OFB_128_CIPHER, AES_OFB_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_OFB, AES_OFB_192_KEY_SIZE,
            AES_OFB_PLAIN_SIZE, AES_OFB_192_KEY, AES_OFB_PLAIN_TEXT,
            AES_OFB_192_CIPHER, AES_OFB_IV, 0)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_OFB, AES_OFB_256_KEY_SIZE,
            AES_OFB_PLAIN_SIZE, AES_OFB_256_KEY, AES_OFB_PLAIN_TEXT,
            AES_OFB_256_CIPHER, AES_OFB_IV, 0)) != CC_OK)
        return status;

    /* CTR */
    if ((status = Perform_AES_Decrypt(AES_CTR, AES_CTR_128_KEY_SIZE,
            AES_CTR_PLAIN_SIZE, AES_CTR_128_KEY, AES_CTR_PLAIN_TEXT,
            AES_CTR_128_CIPHER, 0, AES_CTR_NONCE)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_CTR, AES_CTR_192_KEY_SIZE,
            AES_CTR_PLAIN_SIZE, AES_CTR_192_KEY, AES_CTR_PLAIN_TEXT,
            AES_CTR_192_CIPHER, 0, AES_CTR_NONCE)) != CC_OK)
        return status;

    if ((status = Perform_AES_Decrypt(AES_CTR, AES_CTR_256_KEY_SIZE,
            AES_CTR_PLAIN_SIZE, AES_CTR_256_KEY, AES_CTR_PLAIN_TEXT,
            AES_CTR_256_CIPHER, 0, AES_CTR_NONCE)) != CC_OK)
        return status;

    return CC_OK;
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
    swmLogInfo("AES-128 Sample Encryption/Decryption\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("Encryption/Decryption operation\r\n");
    }
    else
    {
        CCStatus status = measureDuration(Perform_AES_EncryptDecrypt);
        if (status == CC_OK)
        {
            swmLogTestPass("Encryption/Decryption operation\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("Encryption/Decryption operation\r\n");
        }
    }

    breakpointHook();

    return 0;
}
