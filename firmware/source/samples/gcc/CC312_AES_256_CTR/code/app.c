/**
 * @file app.c
 * @brief Main application, sets up and executes the crypto operations.
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
#include <stdio.h>

#include "mbedtls/aes.h"

/* Test vectors have been derived from the following publications:
 *  https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CTR.pdf
 */
#include "AES_CTR_vectors.h"

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
 * This is a profiling test and as such will run over a defined data set size
 * encrypting the same plain text repeatedly. This is a throughput test and
 * does not verify the encrypted data.
 *
 * Define the size of data to encrypt in bytes
 */
#define BYTES_IN_1K                 1024
#define KILOBYTES_TO_ENCRYPT        512
#define DATA_SIZE_TO_ENCRYPT        (KILOBYTES_TO_ENCRYPT * BYTES_IN_1K)

/** Define the standard AES block size, it is always 128 bits */
#define AES_BLOCK_SIZE              16

/** Define the standard Nonce size, it is always 128 bits */
#define AES_NONCE_SIZE              16

/* Define error cases */
#define AES_ERROR_KEY_FAILURE       0xF0000000
#define AES_ERROR_BAD_PARAMETER     0xF0000001
#define AES_ERROR_ENCRYPT_FAILURE   0xF0000002
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
 * @brief Function performs a repeated AES-CTR operation encrypting
 * DATA_SIZE_TO_ENCRYPT bytes.
 * @param key Pointer to a buffer holding the key.
 * @param plain Pointer to a buffer holding the plain text.
 * @param nonce Pointer to a buffer holding the nonce.
 * @return Zero if the operation is successful,
 * return code of failure if there is a problem.
 */
static uint32_t Perform_AES_CTR_256_Encrypt(
        const uint8_t *key,
        const uint8_t *plain,
        const uint8_t *nonce)
{
    /* Buffer is used as temporary storage */
    uint8_t buffer[sizeof(AES_CTR_PLAIN_TEXT)];
    uint32_t counter = 0;
    size_t offset = 0;
    unsigned char nonceCounter[AES_NONCE_SIZE];
    unsigned char streamBlock[AES_NONCE_SIZE];

    /* Declare a local context and initialize it for use */
    mbedtls_aes_context context;
    mbedtls_aes_init(&context);

    /* Set the key we will use */
    if (mbedtls_aes_setkey_enc(&context, key, AES_CTR_256_KEY_SIZE) != 0)
    {
        return AES_ERROR_KEY_FAILURE;
    }

    /* Setup the initial counter */
    memcpy(nonceCounter, nonce, sizeof(nonceCounter));

    /* Create a working copy of the plain text, we will repeatedly encrypt
     * this buffer to measure the throughput.
     */
    memcpy(buffer, plain, sizeof(AES_CTR_PLAIN_TEXT));

    /* Repeat the encryption operation until we have encrypted at least the
     * required amount of data.
     *
     * Note that while it is possible to encrypt a buffer in a single operation
     * and we could declare an in memory buffer to reduce the number of calls
     * to the encryption function, this measures the performance when encrypting
     * a stream of data in a piecemeal fashion. (64 bytes at a time)
     */
    while (counter < DATA_SIZE_TO_ENCRYPT)
    {
        /* Encrypt as whole buffer on each call. */
        if (mbedtls_aes_crypt_ctr(&context, sizeof(AES_CTR_PLAIN_TEXT),
                &offset, nonceCounter, streamBlock, buffer, buffer) != 0)
        {
            return AES_ERROR_ENCRYPT_FAILURE;
        }

        counter += sizeof(AES_CTR_PLAIN_TEXT);
    }

    /* Always free the context when finished with it */
    mbedtls_aes_free(&context);

    return CC_OK;
}

/**
 * @brief Sample function demonstrating AES-CTR encryption.
 * @return 0 if the operations succeed.
 */
static uint32_t Perform_AES_Encrypt(void)
{
    return Perform_AES_CTR_256_Encrypt(AES_CTR_256_KEY, AES_CTR_PLAIN_TEXT, AES_CTR_NONCE);
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

    /* Get the duration in ticks */
    double duration = startTick - endTick;

    /* Convert to seconds */
    duration /= SystemCoreClock;

    /* Define our encryption rate as kilobytes per second */
    double encryptionRate = KILOBYTES_TO_ENCRYPT / duration;

    /* Provide an indication to the user of the rate */
    char text[128];
    sprintf(text, "Encrypted %dKb in %5.2fs, giving a rate of %5.2fKb/s\r\n",
            KILOBYTES_TO_ENCRYPT, duration, encryptionRate);
    swmLogInfo(text);

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
    swmLogInfo("AES-256 Encryption Performance Measurement\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("AES-256 encryption operation\r\n");
    }
    else
    {
        CCStatus status = measureDuration(Perform_AES_Encrypt);
        if (status == CC_OK)
        {
            swmLogTestPass("AES-256 encryption operation\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("AES-256 encryption operation\r\n");
        }
    }

    breakpointHook();

    return 0;
}
