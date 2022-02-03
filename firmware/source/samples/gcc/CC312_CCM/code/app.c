/**
 * @file app.c
 * @brief Main application demonstrates the ability to perform CCM encryption
 * and authentication operations using the standard NIST sample data.
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

#include "mbedtls/ccm.h"

#include "CCM_vectors.h"


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


#define CCM_ERROR_INVALID_CIPHER       0x00000001
#define CCM_ERROR_INVALID_CMAC         0x00000002
#define CCM_ERROR_DATA_MISMATCH        0x00000003


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


static uint32_t performCCMTests(pCCMFields sample)
{
    uint8_t *buffer = malloc(sample->msgLength + sample->tagLength);
    mbedtls_ccm_context *pCtx = malloc(sizeof(mbedtls_ccm_context));
    mbedtls_ccm_init(pCtx);

    uint8_t *bufKey = malloc(sample->keyLength);
    uint8_t *bufIV = malloc(sample->ivLength);
    uint8_t *bufAD = malloc(sample->adLength);
    uint8_t *bufMsg = malloc(sample->msgLength);
    uint8_t *bufRes = malloc(sample->resLength);

    memcpy(bufKey, sample->key, sample->keyLength);
    memcpy(bufIV, sample->iv, sample->ivLength);
    memcpy(bufAD, sample->ad, sample->adLength);
    memcpy(bufMsg, sample->msg, sample->msgLength);
    memcpy(bufRes, sample->res, sample->resLength);

    /* Encrypt the message */
    uint32_t status = mbedtls_ccm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, bufKey,
            sample->keyLength * 8);
    if (status == 0)
    {
        status = mbedtls_ccm_encrypt_and_tag(pCtx, sample->msgLength, bufIV,
                sample->ivLength, bufAD, sample->adLength, bufMsg, &buffer[0],
                &buffer[sample->msgLength], sample->tagLength);
        if (status == 0)
        {
            if (memcmp(buffer, sample->res, sample->resLength))
            {
                status = CCM_ERROR_DATA_MISMATCH;
            }
        }
    }

    /* Decrypt and authenticate the message */
    if (status == 0)
    {
        status = mbedtls_ccm_auth_decrypt(pCtx, sample->msgLength, bufIV,
                sample->ivLength, bufAD, sample->adLength, bufRes, &buffer[0],
                &bufRes[sample->msgLength], sample->tagLength);
        if (status == 0)
        {
            if (memcmp(buffer, sample->msg, sample->msgLength))
            {
                status = CCM_ERROR_DATA_MISMATCH;
            }
        }
    }

    /* Free any contexts and allocated buffers */

    mbedtls_ccm_free(pCtx);

    free(pCtx);
    free(buffer);
    free(bufKey);
    free(bufIV);
    free(bufAD);
    free(bufMsg);
    free(bufRes);

    return status;
}

static uint32_t performCCM(void)
{
    uint32_t fails = 0;
    for (uint32_t index = 0; CCM_TABLES[index] != NULL; index++)
    {
        pCCMFields sample = CCM_TABLES[index];
        uint32_t status = performCCMTests(sample);
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
    swmLogInfo("CCM Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("CCM\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performCCM);
        if (status == CC_OK)
        {
            swmLogTestPass("CCM\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("CCM\r\n");
        }
    }

    breakpointHook();

    return 0;
}
