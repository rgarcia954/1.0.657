/**
 * @file app.c
 * @brief Main application demonstrates the ability to perform CMAC operations
 * using the standard NIST sample data.
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

#include "mbedtls/cmac.h"

#include "CMAC_vectors.h"


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


#define CMAC_ERROR_INVALID_CIPHER       0x00000001
#define CMAC_ERROR_INVALID_CMAC         0x00000002
#define CMAC_ERROR_DATA_MISMATCH        0x00000003


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
 * @brief Perform the requested CMAC defined by the type and
 * sample test data.
 * @param cipherType Defines the cipher to use when executing the CMAC.
 * @param sample Defines the input and expected output parameters
 * for the operation.
 * @return 0 On success
 */
static uint32_t performCMACTests(mbedtls_cipher_type_t cipherType, pCMACFields sample)
{
    const mbedtls_cipher_info_t *cipherInfo =
            mbedtls_cipher_info_from_type(cipherType);
    if (cipherInfo == NULL)
    {
        return CMAC_ERROR_INVALID_CIPHER;
    }

    unsigned char *pOut = malloc(MBEDTLS_CIPHER_BLKSIZE_MAX);
    unsigned char *pKey = malloc(sample->keyLength / 8);
    unsigned char *pBuf = malloc(sample->msgLength);
    mbedtls_cipher_context_t *pCtx = malloc(sizeof(mbedtls_cipher_context_t));

    memcpy(pKey, sample->key, sample->keyLength / 8);
    memcpy(pBuf, sample->msg, sample->msgLength);

    /* Non Integrated Mode */

    mbedtls_cipher_init(pCtx);

    uint32_t status = mbedtls_cipher_setup(pCtx, cipherInfo);
    if (status == 0)
    {
        status = mbedtls_cipher_cmac_starts(pCtx, pKey, sample->keyLength);
        if (status == 0)
        {
            status = mbedtls_cipher_cmac_reset(pCtx);
            if (status == 0)
            {
                status = mbedtls_cipher_cmac_update(pCtx, pBuf, sample->msgLength);
                if (status == 0)
                {
                    status = mbedtls_cipher_cmac_finish(pCtx, pOut);
                }
            }
        }
    }

    /* Free the context we don't need that any more */
    mbedtls_cipher_free(pCtx);
    free(pCtx);

    /* Check for a valid CMAC and the output matches the expected data */
    if (status == 0)
    {
        if (memcmp(pOut, sample->tag, MBEDTLS_CIPHER_BLKSIZE_MAX))
        {
            status = CMAC_ERROR_DATA_MISMATCH;
        }
    }

    /* Integrated Mode, run this if the status is still okay */
    if (status == 0)
    {
        memset(pOut, 0, MBEDTLS_CIPHER_BLKSIZE_MAX);
        status = mbedtls_cipher_cmac(
                cipherInfo, pKey, sample->keyLength,
                pBuf, sample->msgLength, pOut);
    }

    /* Check for a valid CMAC and the output matches the expected data */
    if (status == 0)
    {
        if (memcmp(pOut, sample->tag, MBEDTLS_CIPHER_BLKSIZE_MAX))
        {
            status = CMAC_ERROR_DATA_MISMATCH;
        }
    }

    free(pOut);
    free(pBuf);
    free(pKey);

    return status;
}

/**
 * @brief Derive the requested cipher type given the key length.
 * @param keyLength The length of the cipher key.
 * @return A valid cipher type. (May be None)
 */
static mbedtls_cipher_type_t cipher(uint32_t keyLength)
{
    switch (keyLength)
    {
    case 128:
        return MBEDTLS_CIPHER_AES_128_ECB;

    case 192:
        return MBEDTLS_CIPHER_AES_192_ECB;

    case 256:
        return MBEDTLS_CIPHER_AES_256_ECB;

    default:
        /* Don't need default tag as we will fall through to the default */
        break;
    }

    return MBEDTLS_CIPHER_NONE;
}

/**
 * @brief Execute CMAC operations on each of the sample test data
 * defined in the CMAC_vectors.h file.
 * @return 0 on Success.
 */
static uint32_t performCMAC(void)
{
    uint32_t fails = 0;
    for (uint32_t index = 0; CMAC_TABLES[index] != NULL; index++)
    {
        pCMACFields sample = CMAC_TABLES[index];
        uint32_t status = performCMACTests(cipher(sample->keyLength), sample);
        if (status != 0)
        {
            fails |= (1 << index);
        }
    }

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
    swmLogInfo("CMAC Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("CMAC\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performCMAC);
        if (status == CC_OK)
        {
            swmLogTestPass("CMAC\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("CMAC\r\n");
        }
    }

    breakpointHook();

    return 0;
}
