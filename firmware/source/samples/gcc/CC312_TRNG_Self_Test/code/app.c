/**
 * @file app.c
 * @brief Main application, Initialises DRBG & Entropy contexts and then fills
 * a buffer with random numbers.
 * @details This application uses the MBEDTLS interface to the Counter-Mode
 * Deterministic Random Byte Generator (CTR-DRBG) to fill a buffer with random
 * values.
 *
 * This code demonstrates the MBEDTLS self-test application with prediction
 * resistance both on and off.
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

#include "mbedtls/ctr_drbg.h"


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


static const unsigned char entropySourcePR[] = {
        0xc1, 0x80, 0x81, 0xa6, 0x5d, 0x44, 0x02, 0x16,
        0x19, 0xb3, 0xf1, 0x80, 0xb1, 0xc9, 0x20, 0x02,
        0x6a, 0x54, 0x6f, 0x0c, 0x70, 0x81, 0x49, 0x8b,
        0x6e, 0xa6, 0x62, 0x52, 0x6d, 0x51, 0xb1, 0xcb,
        0x58, 0x3b, 0xfa, 0xd5, 0x37, 0x5f, 0xfb, 0xc9,
        0xff, 0x46, 0xd2, 0x19, 0xc7, 0x22, 0x3e, 0x95,
        0x45, 0x9d, 0x82, 0xe1, 0xe7, 0x22, 0x9f, 0x63,
        0x31, 0x69, 0xd2, 0x6b, 0x57, 0x47, 0x4f, 0xa3,
        0x37, 0xc9, 0x98, 0x1c, 0x0b, 0xfb, 0x91, 0x31,
        0x4d, 0x55, 0xb9, 0xe9, 0x1c, 0x5a, 0x5e, 0xe4,
        0x93, 0x92, 0xcf, 0xc5, 0x23, 0x12, 0xd5, 0x56,
        0x2c, 0x4a, 0x6e, 0xff, 0xdc, 0x10, 0xd0, 0x68
};

static const unsigned char noncePR[] = {
        0xd2, 0x54, 0xfc, 0xff, 0x02, 0x1e, 0x69, 0xd2,
        0x29, 0xc9, 0xcf, 0xad, 0x85, 0xfa, 0x48, 0x6c
};

static const unsigned char resultPR[] = {
        0x34, 0x01, 0x16, 0x56, 0xb4, 0x29, 0x00, 0x8f,
        0x35, 0x63, 0xec, 0xb5, 0xf2, 0x59, 0x07, 0x23
};

static const unsigned char entropySourceNOPR[] = {
        0x5a, 0x19, 0x4d, 0x5e, 0x2b, 0x31, 0x58, 0x14,
        0x54, 0xde, 0xf6, 0x75, 0xfb, 0x79, 0x58, 0xfe,
        0xc7, 0xdb, 0x87, 0x3e, 0x56, 0x89, 0xfc, 0x9d,
        0x03, 0x21, 0x7c, 0x68, 0xd8, 0x03, 0x38, 0x20,
        0xf9, 0xe6, 0x5e, 0x04, 0xd8, 0x56, 0xf3, 0xa9,
        0xc4, 0x4a, 0x4c, 0xbd, 0xc1, 0xd0, 0x08, 0x46,
        0xf5, 0x98, 0x3d, 0x77, 0x1c, 0x1b, 0x13, 0x7e,
        0x4e, 0x0f, 0x9d, 0x8e, 0xf4, 0x09, 0xf9, 0x2e
};

static const unsigned char nonceNOPR[] = {
        0x1b, 0x54, 0xb8, 0xff, 0x06, 0x42, 0xbf, 0xf5,
        0x21, 0xf1, 0x5c, 0x1c, 0x0b, 0x66, 0x5f, 0x3f
};

static const unsigned char resultNOPR[] = {
        0xa0, 0x54, 0x30, 0x3d, 0x8a, 0x7e, 0xa9, 0x88,
        0x9d, 0x90, 0x3e, 0x07, 0x7c, 0x6f, 0x21, 0x8f
};

static size_t test_offset;


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


static int entropyCallBack( void *data, unsigned char *buf,
                                       size_t len )
{
    const unsigned char *p = data;
    memcpy( buf, p + test_offset, len );
    test_offset += len;
    return( 0 );
}

static uint32_t performTRNGSelfTest(
        const unsigned char *entropySource,
        const unsigned char *nonce,
        const int predictionResistance,
        const unsigned char *result )
{
    mbedtls_ctr_drbg_context ctx;
    unsigned char buf[16];
    int status;

    /* Initialize the context */
    mbedtls_ctr_drbg_init(&ctx);

    /* Set up a test entropy seed, this ensures repeatable values */
    test_offset = 0;
    status = mbedtls_ctr_drbg_seed_entropy_len(&ctx, entropyCallBack,
                                (void *) entropySource, nonce, 16, 32);
    if (status != 0)
    {
        return status;
    }

    if (predictionResistance == MBEDTLS_CTR_DRBG_PR_ON)
    {
        mbedtls_ctr_drbg_set_prediction_resistance(&ctx, predictionResistance);
    }

    status = mbedtls_ctr_drbg_random(&ctx, buf, MBEDTLS_CTR_DRBG_BLOCKSIZE);
    if (status != 0)
    {
        return status;
    }

    if (predictionResistance != MBEDTLS_CTR_DRBG_PR_ON)
    {
        status = mbedtls_ctr_drbg_reseed( &ctx, NULL, 0 );
        if (status != 0)
        {
            return status;
        }
    }

    status = mbedtls_ctr_drbg_random(&ctx, buf, MBEDTLS_CTR_DRBG_BLOCKSIZE);
    if (status != 0)
    {
        return status;
    }

    if (memcmp(buf, result, MBEDTLS_CTR_DRBG_BLOCKSIZE))
    {
        return -1;
    }

    mbedtls_ctr_drbg_free(&ctx);

    return CC_OK;
}

static uint32_t performTRNG(void)
{
    /*
     * Based on a NIST CTR_DRBG test vector (PR = True)
     */
    uint32_t status = performTRNGSelfTest(
            entropySourcePR, noncePR, MBEDTLS_CTR_DRBG_PR_ON, resultPR);
    if (status != CC_OK)
    {
        return status;
    }

    /*
     * Based on a NIST CTR_DRBG test vector (PR = False)
     */
    return performTRNGSelfTest(
            entropySourceNOPR, nonceNOPR, MBEDTLS_CTR_DRBG_PR_OFF, resultNOPR);
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
    swmLogInfo("TRNG (CTR-DRBG) Self Test Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("TRNG (CTR-DRBG) Self Test\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performTRNG);
        if (status == CC_OK)
        {
            swmLogTestPass("TRNG (CTR-DRBG) Self Test\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("TRNG (CTR-DRBG) Self Test\r\n");
        }
    }

    breakpointHook();

    return 0;
}
