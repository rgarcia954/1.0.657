/**
 * @file app.c
 * @brief Main application, Initialises DRBG & Entropy contexts and then fills
 * a buffer with random numbers.
 * @details This application uses the MBEDTLS interface to the Counter-Mode
 * Deterministic Random Byte Generator (CTR-DRBG) to fill a buffer with random
 * values.
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


/** Define a buffer size of 255 words */

#define BUFFER_SIZE_WORDS   255
#define BUFFER_SIZE_BYTES   (BUFFER_SIZE_WORDS * sizeof(uint32_t))

/** Define a structure into which we can generate random bytes */
typedef union {
    unsigned char cBuffer[BUFFER_SIZE_BYTES];
    uint32_t wBuffer[BUFFER_SIZE_WORDS];
} tRandomBuffer;

/** Define a random context as a buffer and an index into the buffer */
typedef struct {
    mbedtls_ctr_drbg_context    ctr_drbg;
    mbedtls_entropy_context     entropy;
    uint32_t                    index;
    tRandomBuffer               buffer;
} tRandomContext, *pRandomContext;

static tRandomContext   rContext;


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


static uint32_t refillBuffer(pRandomContext ctx)
{
    ctx->index = 0;
    return mbedtls_ctr_drbg_random(
            &ctx->ctr_drbg, &ctx->buffer.cBuffer[0], BUFFER_SIZE_BYTES);
}

static uint32_t initialiseRandomNumbers(pRandomContext ctx)
{
    uint32_t status;

    /* Initialise the ctr-drbg and entropy contexts */
    mbedtls_ctr_drbg_init(&ctx->ctr_drbg);
    mbedtls_entropy_init(&ctx->entropy);

    /* Seed the random numbner generator with some random values */
    status = mbedtls_ctr_drbg_seed(
            &ctx->ctr_drbg, mbedtls_entropy_func, &ctx->entropy,
            (const unsigned char *) "RANDOM_GEN", 10);

    if(status == 0)
    {
        /* Turn off the prediction resistance, we don't need this */
        mbedtls_ctr_drbg_set_prediction_resistance(
                &ctx->ctr_drbg, MBEDTLS_CTR_DRBG_PR_OFF);

        /* Fill our buffer with generated values */
        status = refillBuffer(ctx);
    }

    return status;
}

static uint32_t getNext(pRandomContext ctx)
{
    if (ctx->index >= BUFFER_SIZE_WORDS)
    {
        /* We can't easily do anything with a bad status here so ignore it */
        refillBuffer(ctx);
    }

    return ctx->buffer.wBuffer[ctx->index++];
}

static void releaseContext(pRandomContext ctx)
{
    /* Free our context structures, we don't need them now. */
    mbedtls_ctr_drbg_free(&ctx->ctr_drbg);
    mbedtls_entropy_free(&ctx->entropy);
}

static uint32_t performTRNG(void)
{
    /* Initialise our random number context */
    initialiseRandomNumbers(&rContext);

    /* Retrieve some random numbers, don't do anything with them, this is just
     * verifying the operation
     */
    for (uint32_t index = 0; index < BUFFER_SIZE_BYTES; index++) {
        uint32_t value __attribute__((unused)) = getNext(&rContext);
    }

    /* Release any resources we need to get rid of */
    releaseContext(&rContext);

    /* We always return CC_OK from this function */
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

    /* Get the duration in ticks */
    double duration = startTick - endTick;

    /* Convert to seconds */
    duration /= SystemCoreClock;

    /* Define our encryption rate as kilobytes per second */
    double rate = BUFFER_SIZE_BYTES / duration;

    /* Provide an indication to the user of the rate */
    char text[128];
    sprintf(text, "Generated %d randoms in %5.2fs, giving a rate of %5.2f/s\r\n",
            BUFFER_SIZE_BYTES, duration, rate);
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
    swmLogInfo("TRNG (CTR-DRBG) Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("TRNG (CTR-DRBG)\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performTRNG);
        if (status == CC_OK)
        {
            swmLogTestPass("TRNG (CTR-DRBG)\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("TRNG (CTR-DRBG)\r\n");
        }
    }

    breakpointHook();

    return 0;
}
