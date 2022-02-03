/**
 * @file app.c
 * @brief Main application demonstrates the ability to perform HMAC operations
 *        using the standard NIST sample data.
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
#include <malloc.h>

#include "mbedtls/md.h"
#include "cc_hash_defs.h"

#include "HMAC_SHA1_vectors.h"
#include "HMAC_SHA224_vectors.h"
#include "HMAC_SHA256_vectors.h"
#include "HMAC_SHA384_vectors.h"
#include "HMAC_SHA512_vectors.h"


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
 * @brief Perform a series of HMAC operations defined by a mode and data tables.
 * @param tables A list of sample data derived from the NIST example data.
 * @param mode The hashing mode of the operation.
 * @param length Selects the number of entries from the provided tables.
 * @return 0 on success.
 */
static uint32_t performHMACTests(
        const pHMACFields *tables, CCHashOperationMode_t mode, uint32_t length)
{
    /* Get the md_info from the provided mode */
    const char *algoMode = HashAlgMode2mbedtlsString[mode];
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_string(algoMode);
    if (md_info == NULL)
    {
        return HMAC_ERROR_INVALID_ALGO_MODE;
    }

    /* Loop through each entry in the table of sample data, gather
     * results as we go
     */
    uint32_t collatedStatus = 0;
    for (uint32_t index = 0; index < length; index++)
    {
        /* Grab a pointer to the next sample */
        pHMACFields sample = tables[index];

        unsigned char *pOut = malloc(64);
        unsigned char *pKey = malloc(sample->keyLength);
        unsigned char *pBuf = malloc(sample->txtLength);

        memcpy(pKey, sample->key, sample->keyLength);
        memcpy(pBuf, sample->txt, sample->txtLength);

        /* Execute integrated tests for each entry in the tables, this
         * just requires the md_info to describe the operation
         */

        /* Clear the result buffer */
        memset(pOut, 0, 64);

        /* Perform the HMAC */
        uint32_t status = mbedtls_md_hmac(
                md_info, pKey, sample->keyLength, pBuf, sample->txtLength, pOut);

        /* If the operation was successful, verify the expected
         * data matches the generated MAC
         */
        if (status == 0)
        {
            if (memcmp(pOut, sample->mac, sample->macLength))
            {
                status = HMAC_ERROR_DATA_MISMATCH;
            }
        }

        /* Execute non-integrated tests for each entry in the tables, only
         * do this if the previous step was successful.
         */
        if (status == 0)
        {
            /* Clear the result buffer */
            memset(pOut, 0, sample->macLength);

            /* Create and initialize a context */
            mbedtls_md_context_t *pCtx = malloc(sizeof(mbedtls_md_context_t));
            mbedtls_md_init(pCtx);

            /* Set up for the appropriate HMAC operation, enabling the HMAC */
            status = mbedtls_md_setup(pCtx, md_info, 1);
            if (status == 0)
            {
                status = mbedtls_md_hmac_starts(pCtx, pKey, sample->keyLength);
                if (status == 0)
                {
                    status = mbedtls_md_hmac_update(pCtx, pBuf, sample->txtLength);
                    if (status == 0)
                    {
                        status = mbedtls_md_hmac_finish(pCtx, pOut);
                    }
                }
            }

            mbedtls_md_free(pCtx);
            free(pCtx);
        }

        /* If the operation was successful, verify the expected
         * data matches the generated MAC
         */
        if (status == 0)
        {
            if (memcmp(pOut, sample->mac, sample->macLength))
            {
                status = HMAC_ERROR_DATA_MISMATCH;
            }
        }

        if (status != 0)
        {
            collatedStatus |= 1 << index;
        }

        free(pOut);
        free(pKey);
        free(pBuf);
    }

    return collatedStatus;
}

/**
 * @brief Execute HMAC operations on each of the sample test data
 * defined in the HMAC_vectors.h file.
 * @return 0 on Success.
 */
static uint32_t performHMAC(void)
{
    uint32_t status;
    uint32_t collatedStatus = 0;

    if ((status = performHMACTests(HMAC_SHA1_TABLES, CC_HASH_SHA1_mode, 4)) != CC_OK)
        collatedStatus |= (status << 0);

    if ((status = performHMACTests(HMAC_SHA224_TABLES, CC_HASH_SHA224_mode, 3)) != CC_OK)
        collatedStatus |= (status << 4);

    if ((status = performHMACTests(HMAC_SHA256_TABLES, CC_HASH_SHA256_mode, 3)) != CC_OK)
        collatedStatus |= (status << 8);

    if ((status = performHMACTests(HMAC_SHA384_TABLES, CC_HASH_SHA384_mode, 4)) != CC_OK)
        collatedStatus |= (status << 12);

    if ((status = performHMACTests(HMAC_SHA512_TABLES, CC_HASH_SHA512_mode, 4)) != CC_OK)
        collatedStatus |= (status << 16);

    return collatedStatus;
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
    swmLogInfo("HMAC Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("HMAC\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performHMAC);
        if (status == CC_OK)
        {
            swmLogTestPass("HMAC\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("HMAC\r\n");
        }
    }

    breakpointHook();

    return 0;
}
