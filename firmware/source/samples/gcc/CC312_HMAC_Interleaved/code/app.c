/**
 * @file app.c
 * @brief Main application demonstrates the ability to perform HMAC operations
 *        using the standard NIST sample data.
 * @details
 *   This demonstrates the calculation of simultaneous HMACs over two text
 *   blocks. It does this by generating the HMACs using two context structures
 *   to calculate the HMAC in a piece-wise manner. It then compares the
 *   calculated HMACs to reference values to ensure the results match.
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

#include "mbedtls/md.h"
#include "cc_hash_defs.h"

#include "HMAC_SHA256_vectors.h"


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


/*! The maximal size of the hash string. */
#define CC_HASH_NAME_MAX_SIZE  10

/* Hash string names. */
extern const char HashAlgMode2mbedtlsString[CC_HASH_NumOfModes][CC_HASH_NAME_MAX_SIZE];

#define HMAC_ERROR_INVALID_ALGO_MODE    0x00000001
#define HMAC_ERROR_INVALID_HMAC         0x00000002
#define HMAC_ERROR_DATA_MISMATCH        0x00000003

/** Define the size of the HMAC working buffer. */
#define HMAC_BUFFER_SIZE                64

/** Define the size of the buffer of data to be signed. */
#define HMAC_INTERLEAVE_TEXT_SIZE       256

/** Define the size of slices of data to be signed in an interleaved fashion. */
#define HMAC_INTERLEAVE_SLICE_SIZE      16


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
 * @brief Calculate a HMAC given a key and some text to process.
 *
 * @param mode The type of HMAC to generate, normally SHA256.
 * @param key A pointer to an array of bytes comprising the key value.
 * @param keylen The length of the key value.
 * @param txt A pointer to an array of bytes comprising the text value.
 * @param txtlen The length of the text value.
 * @param macbuff A pointer to an unsigned char buffer in which to store the
 * calculated HMAC.
 *
 * @return CC_OK (0) if the operation is successful. Non-zero otherwise.
 */
static uint32_t calculateHMAC(
        const CCHashOperationMode_t mode,
        const uint8_t *key, const uint32_t keylen,
        const uint8_t *txt, const uint32_t txtlen,
        unsigned char *macbuff
        )
{
    /* Get the md_info from the provided mode */
    const char *algoMode = HashAlgMode2mbedtlsString[mode];
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_string(algoMode);
    if (md_info == NULL)
    {
        return HMAC_ERROR_INVALID_ALGO_MODE;
    }

    /* Create and initialize a context */
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);

    /* Set up for the appropriate HMAC operation, enabling the HMAC */
    uint32_t status = mbedtls_md_setup(&ctx, md_info, 1);
    if (status == CC_OK)
    {
        /* Start processing with our key */
        status = mbedtls_md_hmac_starts(&ctx, key, keylen);
        if (status == CC_OK)
        {
            /* Update with the test vector */
            status = mbedtls_md_hmac_update(&ctx, txt, txtlen);
            if (status == CC_OK)
            {
                /* Complete the operation */
                status = mbedtls_md_hmac_finish(&ctx, macbuff);
            }
        }
    }

    mbedtls_md_free(&ctx);

    return status;
}

/**
 * @brief This performs a basic verification of the calculateHMAC function
 * using known good data for the key and text. It then compares the result
 * against the expected values.
 *
 * @param sample A pointer to a tHMACFields structure defining the test vector.
 * @param mode The type of HMAC to generate, normally SHA256.
 *
 * @return CC_OK (0) if the operation is successful. Non-zero otherwise.
 */
static uint32_t performHMACTest(
        const pHMACFields sample, CCHashOperationMode_t mode)
{
    unsigned char hmacBuffer[HMAC_BUFFER_SIZE] = { 0 };
    uint32_t status = calculateHMAC(mode, sample->key, sample->keyLength,
            sample->txt, sample->txtLength, hmacBuffer);
    if (status != CC_OK)
    {
        return status;
    }

    /* Verify the expected data matches the generated MAC */
    if (memcmp(hmacBuffer, sample->mac, sample->macLength))
    {
        return HMAC_ERROR_DATA_MISMATCH;
    }

    return CC_OK;
}

/**
 * @brief This performs two HMAC operations in a piece-wise manner to
 * demonstrate the ability to perform multiple calculations in parallel.
 *
 * @param sample A pointer to a tHMACFields structure defining a test vector
 * which supplies a key.
 * @param mode The type of HMAC to generate, normally SHA256.
 *
 * @return CC_OK (0) if the operation is successful. Non-zero otherwise.
 */
static uint32_t interleavedHMACTest(
        const pHMACFields sample, CCHashOperationMode_t mode)
{
    /* Define two buffers with some different data in them */
    uint8_t txt1[HMAC_INTERLEAVE_TEXT_SIZE] = { 0x55 };
    uint8_t txt2[HMAC_INTERLEAVE_TEXT_SIZE] = { 0x3C };

    /* Calculate the HMAC of the two buffers using the key defined
     * in the provided sample
     */
    unsigned char mac1[HMAC_BUFFER_SIZE];
    unsigned char mac2[HMAC_BUFFER_SIZE];

    uint32_t status = calculateHMAC(mode, sample->key, sample->keyLength,
            txt1, HMAC_INTERLEAVE_TEXT_SIZE, mac1);
    if (status != CC_OK)
    {
        return status;
    }

    status = calculateHMAC(mode, sample->key, sample->keyLength,
            txt2, HMAC_INTERLEAVE_TEXT_SIZE, mac2);
    if (status != CC_OK)
    {
        return status;
    }

    /* Having calculated the HMAC for the two buffers now repeat the process
     * interleaved calculating two check macs
     */
    /* Get the md_info from the provided mode */
    const char *algoMode = HashAlgMode2mbedtlsString[mode];
    const mbedtls_md_info_t *md_info1 = mbedtls_md_info_from_string(algoMode);
    const mbedtls_md_info_t *md_info2 = mbedtls_md_info_from_string(algoMode);
    if ((md_info1 == NULL) || (md_info2 == NULL))
    {
        return HMAC_ERROR_INVALID_ALGO_MODE;
    }

    /* Create and initialize contexts */
    mbedtls_md_context_t ctx1, ctx2;
    mbedtls_md_init(&ctx1);
    mbedtls_md_init(&ctx2);

    /* Set up for the appropriate HMAC operation, enabling the HMAC */
    status = mbedtls_md_setup(&ctx1, md_info1, 1);
    if (status == CC_OK)
    {
        status = mbedtls_md_setup(&ctx2, md_info2, 1);
        if (status == CC_OK)
        {
            /* Start processing with our key, for the purposes of this test use the
             * same key in both cases
             */
            status = mbedtls_md_hmac_starts(&ctx1, sample->key, sample->keyLength);
            if (status == CC_OK)
            {
                status = mbedtls_md_hmac_starts(&ctx2, sample->key, sample->keyLength);
            }
        }
    }

    /* Process the two  HMAC operations using their own contexts processing
     * a slice at a time
     */
    if (status == CC_OK)
    {
        uint32_t residual = HMAC_INTERLEAVE_TEXT_SIZE;
        uint32_t index = 0;
        while ((residual > 0) && (status == CC_OK))
        {
            uint32_t size = (residual > HMAC_INTERLEAVE_SLICE_SIZE) ?
                    HMAC_INTERLEAVE_SLICE_SIZE : residual;

            /* Update with the current slice of the test vector */
            status = mbedtls_md_hmac_update(&ctx1, &txt1[index], size);
            if (status == CC_OK)
            {
                status = mbedtls_md_hmac_update(&ctx2, &txt2[index], size);
            }

            index += size;
            residual -= size;
        }
    }

    /* Complete the operation */
    if (status == CC_OK)
    {
        unsigned char chk1[HMAC_BUFFER_SIZE] = {0};
        unsigned char chk2[HMAC_BUFFER_SIZE] = {0};
        status = mbedtls_md_hmac_finish(&ctx1, chk1);
        if (status != CC_OK)
        {
            status = mbedtls_md_hmac_finish(&ctx2, chk2);
            if (status != CC_OK)
            {
                /* Verify the expected data matches the generated MAC */
                status = memcmp(mac1, chk1, sample->macLength);
                if (status == 0)
                {
                    status = memcmp(mac2, chk2, sample->macLength);
                }
            }
        }
    }

    mbedtls_md_free(&ctx1);
    mbedtls_md_free(&ctx2);

    return status;
}

/**
 * @brief This coordinates the HMAC tests for this sample.
 *
 * @return Zero if the operation is successful. Non-zero otherwise.
 */
static uint32_t performHMAC(void)
{
    /* Perform the MAC test against known data to demonstrate it's use */
    uint32_t status = performHMACTest(
            (const pHMACFields) &HMAC_SHA256_64_32, CC_HASH_SHA256_mode);
    if (status == CC_OK)
    {
        status = interleavedHMACTest(
                (const pHMACFields) &HMAC_SHA256_64_32, CC_HASH_SHA256_mode);
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
    swmLogInfo("HMAC Interleave Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("HMAC Interleave\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performHMAC);
        if (status == CC_OK)
        {
            swmLogTestPass("HMAC Interleave\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("HMAC Interleave\r\n");
        }
    }

    breakpointHook();

    return 0;
}
