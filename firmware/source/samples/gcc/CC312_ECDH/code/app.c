/**
 * @file app.c
 * @brief Demonstration of ECDH Key Exchange..
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "mbedtls/ecdh.h"
#include "mbedtls_cc_ecdh_edwards.h"
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


typedef struct
{
    unsigned char *buf;
    size_t length;
} tRndBufInfo, *pRndBufInfo;

#define ECDH_FAIL_RANDOM_PRIME      0x00000001
#define ECDH_FAIL_EXCHANGE_P256R1   0x00000002
#define ECDH_FAIL_EXCHANGE_25519    0x00000004

#define ECDH_ERROR_BAD_CURVE_ID     0x00000001

#define ECDH_BUFFER_SIZE            1000

/**
 * Initial state of the random number buffer for Alice for the random prime test
 */
static const char *dA_str =
        "C88F01F510D9AC3F70A292DAA2316DE544E9AAB8AFE84049C62A9C57862D1433";

/**
 * Expected private key for Alice for the random prime test
 */
static const char *xA_str =
        "DAD0B65394221CF9B051E1FECA5787D098DFE637FC90B9EF945D0C3772581180";

/**
 * Expected public key for Alice for the random prime test
 */
static const char *yA_str =
        "5271A0461CDB8252D61F1C456FA3E59AB1F45B33ACCF5F58389E0577B8990BB3";

/**
 * Initial state of the random number buffer for Bob for the random prime test
 */
static const char *dB_str =
        "C6EF9C5D78AE012A011164ACB397CE2088685D8F06BF9BE0B283AB46476BEE53";

/**
 * Expected private key for Bob for the random prime test
 */
static const char *xB_str =
        "D12DFB5289C8D4F81208B70270398C342296970A0BCCB74C736FC7554494BF63";

/**
 * Expected public key for Bob for the random prime test
 */
static const char *yB_str =
        "56FBF3CA366CC23E8157854C13C58D6AAC23F046ADA30F8353E74F33039872AB";

/**
 * Expected shared secret for the random prime test
 */
static const char *z_str =
        "D6840F6B42F6EDAFD13116E0E12565202FEF8E9ECE7DCE03812464D04B9442DE";


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
 * @brief Helper routine to convert a buffer of hex characters
 * to their binary equivalents.
 *
 * @param obuf An array into which the converted values will be written.
 * @param ibuf A pointer to a null-terminated string of hex characters.
 * @return -1 if an error is detected or the length of the converted data.
 * @note No bounds checking is done on the output buffer, it is expected
 * to be big enough to hold the generated output.
 */
static int unhexify(unsigned char *obuf, const char *ibuf)
{
    unsigned char c, c2;
    int len = strlen(ibuf) / 2;
    if (strlen(ibuf) % 2 != 0)
    {
        return -1;
    }

    while (*ibuf != 0)
    {
        c = *ibuf++;
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'a' && c <= 'f')
            c -= 'a' - 10;
        else if (c >= 'A' && c <= 'F')
            c -= 'A' - 10;
        else
            return -1;

        c2 = *ibuf++;
        if (c2 >= '0' && c2 <= '9')
            c2 -= '0';
        else if (c2 >= 'a' && c2 <= 'f')
            c2 -= 'a' - 10;
        else if (c2 >= 'A' && c2 <= 'F')
            c2 -= 'A' - 10;
        else
            return -1;

        *obuf++ = (c << 4) | c2;
    }

    return len;
}

/**
 * @brief This function just returns data from rand().
 *
 * Although predictable and often similar on multiple
 * runs, this does not result in identical random on
 * each run. So do not use this if the results of a
 * test depend on the random data that is generated.
 *
 * rng_state shall be NULL.
 */
static int rndStdRand(void *rng_state, unsigned char *output, size_t len)
{
    size_t i;
    if (rng_state != NULL)
    {
        rng_state = NULL;
    }
    for (i = 0; i < len; i++)
    {
        output[i] = rand();
    }
    return (0);
}

/**
 * @brief This function returns random based on a buffer it receives.
 *
 * rng_state shall be a pointer to a rnd_buf_info structure.
 *
 * The number of bytes released from the buffer on each call to
 * the random function is specified by per_call. (Can be between
 * 1 and 4)
 *
 * After the buffer is empty it will return rand();
 */
static int rndBufferRand(void *rng_state, unsigned char *output, size_t len)
{
    pRndBufInfo info = (pRndBufInfo) rng_state;
    size_t use_len;

    if (rng_state == NULL)
    {
        return (rndStdRand(NULL, output, len));
    }

    use_len = len;
    if (len > info->length)
    {
        use_len = info->length;
    }

    if (use_len)
    {
        memcpy(output, info->buf, use_len);
        info->buf += use_len;
        info->length -= use_len;
    }

    if (len - use_len > 0)
    {
        return (rndStdRand(NULL, output + use_len, len - use_len));
    }

    return (0);
}

static void setupRndBuf(pRndBufInfo rndInfo, mbedtls_ecp_group *grp,
        unsigned char *buffer, const char *hexString)
{
    rndInfo->buf = buffer;
    rndInfo->length = unhexify(buffer, hexString);

    /* Fix rndInfo by shifting it left if necessary */
    if (grp->nbits % 8 != 0)
    {
        unsigned char shift = 8 - (grp->nbits % 8);
        size_t i;

        for (i = 0; i < rndInfo->length - 1; i++)
        {
            buffer[i] = buffer[i] << shift | buffer[i + 1] >> (8 - shift);
        }

        buffer[rndInfo->length - 1] <<= shift;
    }
}

static uint32_t performRandomPrime(void)
{
    uint32_t status = 0;

    mbedtls_ecp_group *pGrp = malloc(sizeof(mbedtls_ecp_group));
    unsigned char *pRndBufA = malloc(MBEDTLS_ECP_MAX_BYTES);
    unsigned char *pRndBufB = malloc(MBEDTLS_ECP_MAX_BYTES);

    mbedtls_ecp_group_init(pGrp);
    status = mbedtls_ecp_group_load(pGrp, MBEDTLS_ECP_DP_SECP256R1);

    tRndBufInfo rndInfoA;
    tRndBufInfo rndInfoB;

    setupRndBuf(&rndInfoA, pGrp, pRndBufA, dA_str);
    setupRndBuf(&rndInfoB, pGrp, pRndBufB, dB_str);

    mbedtls_ecp_point *pQA = malloc(sizeof(mbedtls_ecp_point));
    mbedtls_ecp_point *pQB = malloc(sizeof(mbedtls_ecp_point));
    mbedtls_ecp_point_init(pQA);
    mbedtls_ecp_point_init(pQB);

    mbedtls_mpi dA;
    mbedtls_mpi dB;
    mbedtls_mpi zA;
    mbedtls_mpi zB;
    mbedtls_mpi check;

    mbedtls_mpi_init(&dA);
    mbedtls_mpi_init(&dB);
    mbedtls_mpi_init(&zA);
    mbedtls_mpi_init(&zB);
    mbedtls_mpi_init(&check);

    if (status == 0)
    {
        status = mbedtls_ecdh_gen_public(pGrp, &dA, pQA, rndBufferRand,
                &rndInfoA);
        if (status == 0)
        {
            status = mbedtls_ecp_is_zero(pQA);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(&check, 16, xA_str);
                if (status == 0)
                {
                    status = mbedtls_mpi_cmp_mpi(&pQA->X, &check);
                    if (status == 0)
                    {
                        status = mbedtls_mpi_read_string(&check, 16, yA_str);
                        if (status == 0)
                        {
                            status = mbedtls_mpi_cmp_mpi(&pQA->Y, &check);
                        }
                    }
                }
            }
        }
    }

    if (status == 0)
    {
        status = mbedtls_ecdh_gen_public(pGrp, &dB, pQB, rndBufferRand,
                &rndInfoB);
        if (status == 0)
        {
            status = mbedtls_ecp_is_zero(pQB);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(&check, 16, xB_str);
                if (status == 0)
                {
                    status = mbedtls_mpi_cmp_mpi(&pQB->X, &check);
                    if (status == 0)
                    {
                        status = mbedtls_mpi_read_string(&check, 16, yB_str);
                        if (status == 0)
                        {
                            status = mbedtls_mpi_cmp_mpi(&pQB->Y, &check);
                        }
                    }
                }
            }
        }
    }

    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&check, 16, z_str);
        if (status == 0)
        {
            status = mbedtls_ecdh_compute_shared(pGrp, &zA, pQB, &dA, NULL,
                    NULL);
            if (status == 0)
            {
                status = mbedtls_mpi_cmp_mpi(&zA, &check);
                if (status == 0)
                {
                    status = mbedtls_ecdh_compute_shared(pGrp, &zB, pQA, &dB,
                            NULL, NULL);
                    if (status == 0)
                    {
                        status = mbedtls_mpi_cmp_mpi(&zB, &check);
                    }
                }
            }
        }
    }

    mbedtls_mpi_free(&dA);
    mbedtls_mpi_free(&dB);
    mbedtls_mpi_free(&zA);
    mbedtls_mpi_free(&zB);
    mbedtls_mpi_free(&check);

    mbedtls_ecp_point_free(pQA);
    mbedtls_ecp_point_free(pQB);

    mbedtls_ecp_group_free(pGrp);

    free(pGrp);
    free(pQA);
    free(pQB);
    free(pRndBufA);
    free(pRndBufB);

    return status;
}

static uint32_t performECDHExchange(mbedtls_ecp_group_id curveId)
{
    uint32_t status = 0;

    mbedtls_ecdh_context *pSrv = malloc(sizeof(mbedtls_ecdh_context));
    mbedtls_ecdh_context *pCli = malloc(sizeof(mbedtls_ecdh_context));
    unsigned char *pBuf = malloc(ECDH_BUFFER_SIZE);
    const unsigned char *vbuf = pBuf;
    size_t len;

    memset(pBuf, 0x00, ECDH_BUFFER_SIZE);

    mbedtls_ecdh_init(pSrv);
    mbedtls_ecdh_init(pCli);

    status = mbedtls_ecp_group_load(&pSrv->grp, curveId);
    if (status == 0)
    {
        switch (curveId)
        {
        case MBEDTLS_ECP_DP_SECP256R1:
            status = mbedtls_ecdh_make_params(pSrv, &len, pBuf,
                    ECDH_BUFFER_SIZE, mbedtls_ctr_drbg_random, &DRBGContext);
            if (status == 0)
            {
                status = mbedtls_ecdh_read_params(pCli, &vbuf, pBuf + len);
            }
            break;

        case MBEDTLS_ECP_DP_CURVE25519:
            status = mbedtls_ecdh_make_params_edwards(pSrv, &len, pBuf,
                    ECDH_BUFFER_SIZE, mbedtls_ctr_drbg_random, &DRBGContext);
            if (status == 0)
            {
                status = mbedtls_ecdh_read_params_edwards(pCli, &vbuf,
                        pBuf + len);
            }
            break;

        default:
            status = ECDH_ERROR_BAD_CURVE_ID;
            break;
        }
    }

    if (status == 0)
    {
        memset(pBuf, 0x00, ECDH_BUFFER_SIZE);
        status = mbedtls_ecdh_make_public(pCli, &len, pBuf, ECDH_BUFFER_SIZE,
                mbedtls_ctr_drbg_random, &DRBGContext);
        if (status == 0)
        {
            status = mbedtls_ecdh_read_public(pSrv, pBuf, len);
        }
        if (status == 0)
        {
            status = mbedtls_ecdh_calc_secret(pSrv, &len, pBuf,
                    ECDH_BUFFER_SIZE, mbedtls_ctr_drbg_random, &DRBGContext);
        }
        if (status == 0)
        {
            status = mbedtls_ecdh_calc_secret(pCli, &len, pBuf,
                    ECDH_BUFFER_SIZE, NULL, NULL);
        }
        if (status == 0)
        {
            status = mbedtls_mpi_cmp_mpi(&pSrv->z, &pCli->z);
        }
    }

    mbedtls_ecdh_free(pSrv);
    mbedtls_ecdh_free(pCli);

    free(pSrv);
    free(pCli);
    free(pBuf);

    return status;
}

static uint32_t performECDH(void)
{
    uint32_t fails = 0;

    if (performRandomPrime() != 0)
    {
        fails |= ECDH_FAIL_RANDOM_PRIME;
    }

    if (performECDHExchange(MBEDTLS_ECP_DP_SECP256R1) != 0)
    {
        fails |= ECDH_FAIL_EXCHANGE_P256R1;
    }

    if (performECDHExchange(MBEDTLS_ECP_DP_CURVE25519) != 0)
    {
        fails |= ECDH_FAIL_EXCHANGE_25519;
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
    swmLogInfo("ECDH Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("ECDH\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performECDH);
        if (status == CC_OK)
        {
            swmLogTestPass("ECDH\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("ECDH\r\n");
        }
    }

    breakpointHook();

    return 0;
}
