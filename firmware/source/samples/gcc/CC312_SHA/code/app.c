/**
 * @file app.c
 * @brief Application demonstrates the ability to run various SHA operations
 * using standard NIST sample data.
 *
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA1.pdf
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA224.pdf
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA256.pdf
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA384.pdf
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA512.pdf
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

#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

#include "SHA1_vectors.h"
#include "SHA224_vectors.h"
#include "SHA256_vectors.h"
#include "SHA384_vectors.h"
#include "SHA512_vectors.h"


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


/** Define the digest length in bytes for SHA1 */
#define SHA1_DIGEST_LENGTH      20

/** Define the digest length in bytes for SHA224 */
#define SHA224_DIGEST_LENGTH    28

/** Define the digest length in bytes for SHA256 */
#define SHA256_DIGEST_LENGTH    32

/** Define the digest length in bytes for SHA384 */
#define SHA384_DIGEST_LENGTH    48

/** Define the digest length in bytes for SHA512 */
#define SHA512_DIGEST_LENGTH    64


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
 * @brief Perform a SHA1 operation on the provided sample data
 * @param sample Defines the sample which should be tested.
 * @return 0 if successful.
 */
static uint32_t performSHA1(pSHAFields sample)
{
    uint32_t status = 0;
    unsigned char output[sample->digestLength];

    /* Non-Integrated Mode */

    mbedtls_sha1_context ctx;
    mbedtls_sha1_init(&ctx);

    memset(output, 0, sample->digestLength);
    status = mbedtls_sha1_starts_ret(&ctx);
    if (status == 0)
    {
        status = mbedtls_sha1_update_ret(
                &ctx, (const unsigned char *) sample->message, sample->messageLength);
        if (status == 0)
        {
            status = mbedtls_sha1_finish_ret(&ctx, output);
            if (status == 0)
            {
                status = memcmp(output, sample->digest, sample->digestLength);
            }
        }
    }

    mbedtls_sha1_free(&ctx);

    /* Integrated Mode */

    memset(output, 0, sample->digestLength);
    status = mbedtls_sha1_ret(
            sample->message, sample->messageLength,  output);
    if (status == 0)
    {
        status = memcmp(output, sample->digest, sample->digestLength);
    }
    return status;
}

/**
 * @brief Perform a SHA256 or SHA224 operation on the provided sample data
 * @param sample Defines the sample which should be tested.
 * @return 0 if successful.
 */
static uint32_t performSHA256(pSHAFields sample)
{
    uint32_t status = 0;
    unsigned char output[sample->digestLength];

    /* Non-Integrated Mode */

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    memset(output, 0, sample->digestLength);
    status = mbedtls_sha256_starts_ret(&ctx, (sample->digestLength == SHA224_DIGEST_LENGTH) ? 1 : 0);
    if (status == 0)
    {
        status = mbedtls_sha256_update_ret(
                &ctx, (const unsigned char *) sample->message, sample->messageLength);
        if (status == 0)
        {
            status = mbedtls_sha256_finish_ret(&ctx, output);
            if (status == 0)
            {
                status = memcmp(output, sample->digest, sample->digestLength);
            }
        }
    }

    mbedtls_sha256_free(&ctx);

    /* Integrated Mode */

    memset(output, 0, sample->digestLength);
    status = mbedtls_sha256_ret(
            sample->message, sample->messageLength, output,
            (sample->digestLength == SHA224_DIGEST_LENGTH) ? 1 : 0);
    if (status == 0)
    {
        status = memcmp(output, sample->digest, sample->digestLength);
    }
    return status;
}

/**
 * @brief Perform a SHA512 or SHA384 operation on the provided sample data
 * @param sample Defines the sample which should be tested.
 * @return 0 if successful.
 */
static uint32_t performSHA512(pSHAFields sample)
{
    uint32_t status = 0;
    unsigned char output[sample->digestLength];

    /* Non-Integrated Mode */

    mbedtls_sha512_context ctx;
    mbedtls_sha512_init(&ctx);

    memset(output, 0, sample->digestLength);
    status = mbedtls_sha512_starts_ret(&ctx, (sample->digestLength == SHA384_DIGEST_LENGTH) ? 1 : 0);
    if (status == 0)
    {
        status = mbedtls_sha512_update_ret(
                &ctx, (const unsigned char *) sample->message, sample->messageLength);
        if (status == 0)
        {
            status = mbedtls_sha512_finish_ret(&ctx, output);
            if (status == 0)
            {
                status = memcmp(output, sample->digest, sample->digestLength);
            }
        }
    }

    mbedtls_sha512_free(&ctx);

    /* Integrated Mode */

    memset(output, 0, sample->digestLength);
    status = mbedtls_sha512_ret(
            sample->message, sample->messageLength, output,
            (sample->digestLength == SHA384_DIGEST_LENGTH) ? 1 : 0);
    if (status == 0)
    {
        status = memcmp(output, sample->digest, sample->digestLength);
    }
    return status;
}

/**
 * @brief Perform a SHA1 operation on the provided sample data
 * @param samples A list of sampes to be signed, with expected output.
 * @return 0 if successful.
 */
static uint32_t performSHASamples(const pSHAFields *samples)
{
    uint32_t fails = 0;
    for (uint32_t index = 0; samples[index] != NULL; index++)
    {
        pSHAFields sample = samples[index];
        uint32_t status = 0;
        switch (sample->digestLength)
        {
        case SHA1_DIGEST_LENGTH:
            status = performSHA1(sample);
            break;
        case SHA224_DIGEST_LENGTH:
            status = performSHA256(sample);
            break;
        case SHA256_DIGEST_LENGTH:
            status = performSHA256(sample);
            break;
        case SHA384_DIGEST_LENGTH:
            status = performSHA512(sample);
            break;
        case SHA512_DIGEST_LENGTH:
            status = performSHA512(sample);
            break;
        default:
            /* This should never happen, so report it as an error */
            status = -1;
        }

        /* If we fail, log an error, this will be reported */
        if (status)
        {
            fails |= (1 << index);
        }
    }

    return fails;
}

/**
 * @brief Perform Various forms of SHA signing.
 * @return 0 if successful.
 */
static uint32_t performSHA(void)
{
    uint32_t fails = 0;
    fails |= performSHASamples(SHA1_TABLES);
    fails |= performSHASamples(SHA224_TABLES);
    fails |= performSHASamples(SHA256_TABLES);
    fails |= performSHASamples(SHA384_TABLES);
    fails |= performSHASamples(SHA512_TABLES);
    return fails;
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
    swmLogInfo("SHA Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("SHA\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performSHA);
        if (status == CC_OK)
        {
            swmLogTestPass("SHA\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("SHA\r\n");
        }
    }

    breakpointHook();

    return 0;
}
