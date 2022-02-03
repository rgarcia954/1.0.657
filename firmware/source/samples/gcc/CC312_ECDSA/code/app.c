/**
 * @file app.c
 * @brief This application demonstrates the use of ECDSA to sign various message types.
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
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "mbedtls/ecdsa.h"
#include "mbedtls_cc_ecdsa_edwards.h"

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


#define DIGEST_SIZE                 64
#define ECP_GROUP_ID                MBEDTLS_ECP_DP_SECP256R1
#define ECP_GROUP_ID_STR            "DP_SECP256R1"
#define MD_TYPE_ID                  MBEDTLS_MD_SHA256
#define MD_TYPE_ID_STR              "SHA256"

#define ECDSA_FAIL_RANDOM_PRIME     0x00000001
#define ECDSA_FAIL_RANDOM_VECTOR    0x00000002
#define ECDSA_FAIL_DET_VECTOR       0x00000004
#define ECDSA_FAIL_READ_WRITE       0x00000008
#define ECDSA_FAIL_RANDOM_EDW       0x00000010

typedef struct
{
    unsigned char *buf;
    size_t length;
} tRndBufInfo, *pRndBufInfo;


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

static uint32_t performRandomPrime(void)
{
    mbedtls_ecp_group *pGrp = malloc(sizeof(mbedtls_ecp_group));
    mbedtls_ecp_point *pQ = malloc(sizeof(mbedtls_ecp_point));

    mbedtls_ecp_group_init(pGrp);
    mbedtls_ecp_point_init(pQ);

    unsigned char *pBuf = malloc(DIGEST_SIZE);
    memset(pBuf, 0, DIGEST_SIZE);

    mbedtls_mpi d, r, s;
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    uint32_t status = mbedtls_ctr_drbg_random(&DRBGContext, pBuf, DIGEST_SIZE);
    if (status == CC_OK)
    {
        status = mbedtls_ecp_group_load(pGrp, ECP_GROUP_ID);
        if (status == 0)
        {
            status = mbedtls_ecp_gen_keypair(pGrp, &d, pQ, mbedtls_ctr_drbg_random, &DRBGContext);
            if (status == 0)
            {
                status = mbedtls_ecdsa_sign(
                        pGrp, &r, &s, &d, pBuf, DIGEST_SIZE, mbedtls_ctr_drbg_random, &DRBGContext);
                if (status == 0)
                {
                    status = mbedtls_ecdsa_verify(pGrp, pBuf, DIGEST_SIZE, pQ, &r, &s);
                }
            }
        }
    }

    mbedtls_ecp_group_free(pGrp);
    mbedtls_ecp_point_free(pQ);

    mbedtls_mpi_free(&d);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    free(pGrp);
    free(pQ);
    free(pBuf);

    return status;
}

/**
 * @brief This function just returns data from rand().
 *
 * Although predictable and often similar on multiple
 * runs, this does not result in identical random on
 * each run. So do not use this if the results of a
 * test depend on the random data that is generated.
 *
 * rngState shall be NULL.
 */
static int rndStdRand(void *rngState, unsigned char *output, size_t len)
{
    size_t i;
    if (rngState != NULL)
    {
        rngState = NULL;
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
static int rndBufferRand(void *rngState, unsigned char *output, size_t len)
{
    pRndBufInfo info = (pRndBufInfo) rngState;
    size_t usedLength;

    if (rngState == NULL)
    {
        return (rndStdRand(NULL, output, len));
    }

    usedLength = len;
    if (len > info->length)
    {
        usedLength = info->length;
    }

    if (usedLength)
    {
        memcpy(output, info->buf, usedLength);
        info->buf += usedLength;
        info->length -= usedLength;
    }

    if (len - usedLength > 0)
    {
        return (rndStdRand(NULL, output + usedLength, len - usedLength));
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

static uint32_t performRandomVector(void)
{
    static const char *dStr    = "DC51D3866A15BACDE33D96F992FCA99DA7E6EF0934E7097559C27F1614C88A7F";
    static const char *xQStr   = "2442A5CC0ECD015FA3CA31DC8E2BBC70BF42D60CBCA20085E0822CB04235E970";
    static const char *yQStr   = "6FC98BD7E50211A4A27102FA3549DF79EBCB4BF246B80945CDDFE7D509BBFD7D";
    static const char *kStr    = "9E56F509196784D963D1C0A401510EE7ADA3DCC5DEE04B154BF61AF1D5A6DECE";
    static const char *hashStr = "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD";
    static const char *rStr    = "CB28E0999B9C7715FD0A80D8E47A77079716CBBF917DD72E97566EA1C066957C";
    static const char *sStr    = "86FA3BB4E26CAD5BF90B7F81899256CE7594BB1EA0C89212748BFF3B3D5B0315";

    mbedtls_ecp_group *pGrp = malloc(sizeof(mbedtls_ecp_group));
    mbedtls_ecp_point *pQ = malloc(sizeof(mbedtls_ecp_point));

    mbedtls_ecp_group_init(pGrp);
    mbedtls_ecp_point_init(pQ);

    mbedtls_mpi d, r, s, rCheck, sCheck;
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_mpi_init(&rCheck);
    mbedtls_mpi_init(&sCheck);

    unsigned char *pHash = malloc(MBEDTLS_ECP_MAX_BYTES);
    memset(pHash, 0, MBEDTLS_ECP_MAX_BYTES);
    size_t hlen = unhexify(pHash, hashStr);;

    unsigned char *pRndBuf = malloc(MBEDTLS_ECP_MAX_BYTES);
    memset(pRndBuf, 0, MBEDTLS_ECP_MAX_BYTES);

    tRndBufInfo rndBufInfo;

    uint32_t status = mbedtls_ecp_group_load(pGrp, ECP_GROUP_ID);
    if (status == 0)
    {
        setupRndBuf(&rndBufInfo, pGrp, pRndBuf, kStr);
        status = mbedtls_ecp_point_read_string(pQ, 16, xQStr, yQStr);
    }

    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&d, 16, dStr);
        if (status == 0)
        {
            status = mbedtls_mpi_read_string(&rCheck, 16, rStr);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(&sCheck, 16, sStr);
            }
        }
    }

    if (status == 0)
    {
        status = mbedtls_ecdsa_sign(pGrp, &r, &s, &d, pHash, hlen, rndBufferRand, &rndBufInfo);
        if (status == 0)
        {
            status = mbedtls_mpi_cmp_mpi(&r, &rCheck);
            if (status == 0)
            {
                status = mbedtls_mpi_cmp_mpi(&s, &sCheck);
                if (status == 0)
                {
                    status = mbedtls_ecdsa_verify(pGrp, pHash, hlen, pQ, &rCheck, &sCheck);
                }
            }
        }
    }

    mbedtls_ecp_group_free(pGrp);
    mbedtls_ecp_point_free(pQ);

    mbedtls_mpi_free(&d);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&rCheck);
    mbedtls_mpi_free(&sCheck);

    free(pGrp);
    free(pQ);
    free(pHash);
    free(pRndBuf);

	return status;
}

static mbedtls_mpi *allocMPI()
{
    mbedtls_mpi *ptr = malloc(sizeof(mbedtls_mpi));
    if (ptr != NULL)
    {
        mbedtls_mpi_init(ptr);
    }
    return ptr;
}

static uint32_t performDeterministicVector(void)
{
    static const char *dStr = "C9AFA9D845BA75166B5C215767B1D6934E50C3DB36E89B127B8A622B120F6721";
    static const char *msg = "sample";
    static const char *rStr = "EFD48B2AACB6A8FD1140DD9CD45E81D69D2C877B56AAF991C34D0EA84EAF3716";
    static const char *sStr = "F7CB1C942D657C41D436C7A1B6E29F65F3E900DBB9AFF4064DC4AB2F843ACDA8";

    const mbedtls_md_info_t *mdInfo = mbedtls_md_info_from_type(MD_TYPE_ID);
    if (mdInfo == NULL)
    {
        return ECDSA_FAIL_DET_VECTOR;
    }

    size_t hlen = mbedtls_md_get_size(mdInfo);

    unsigned char *pHash = malloc(MBEDTLS_MD_MAX_SIZE);
    memset(pHash, 0, MBEDTLS_MD_MAX_SIZE);

    mbedtls_ecp_group *pGrp = malloc(sizeof(mbedtls_ecp_group));
    mbedtls_ecp_group_init(pGrp);

    mbedtls_mpi *pD = allocMPI();
    mbedtls_mpi *pR = allocMPI();
    mbedtls_mpi *pS = allocMPI();
    mbedtls_mpi *pRCheck = allocMPI();
    mbedtls_mpi *pSCheck = allocMPI();

    uint32_t status = mbedtls_ecp_group_load(pGrp, ECP_GROUP_ID);
    if (status == 0)
    {
        status = mbedtls_mpi_read_string(pD, 16, dStr);
        if (status == 0)
        {
            status = mbedtls_mpi_read_string(pRCheck, 16, rStr);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(pSCheck, 16, sStr);
            }
        }
    }

    if (status == 0)
    {
        status = mbedtls_md(mdInfo, (const unsigned char *) msg, strlen(msg), pHash);
        if (status == 0)
        {
            status = mbedtls_ecdsa_sign_det(pGrp, pR, pS, pD, pHash, hlen, MD_TYPE_ID);
        }
    }

    if (status == 0)
    {
        status = mbedtls_mpi_cmp_mpi(pR, pRCheck);
        if (status == 0)
        {
            status = mbedtls_mpi_cmp_mpi(pS, pSCheck);
        }
    }

    mbedtls_ecp_group_free(pGrp);

    mbedtls_mpi_free(pD);
    mbedtls_mpi_free(pR);
    mbedtls_mpi_free(pS);
    mbedtls_mpi_free(pRCheck);
    mbedtls_mpi_free(pSCheck);

    free(pHash);
    free(pGrp);
    free(pD);
    free(pR);
    free(pS);
    free(pRCheck);
    free(pSCheck);

	return status;
}

static uint32_t performWriteRead(void)
{
    size_t dataLength = 200;
    size_t hashLength = 32;
    size_t sigLength;

    mbedtls_ctr_drbg_context *pCtrDrbg = &DRBGContext;

    unsigned char *pSig = (unsigned char *) malloc(dataLength);
    unsigned char *pHash = (unsigned char *) malloc(hashLength);

    memset(pSig, 0, dataLength);
    memset(pHash, 0, hashLength);

    mbedtls_ecdsa_context *pCtx = malloc(sizeof(mbedtls_ecdsa_context));
    mbedtls_ecdsa_init(pCtx);

    /* prepare material for signature */
    uint32_t status = mbedtls_ctr_drbg_random(pCtrDrbg, pHash, hashLength);
    if (status == 0)
    {
        /* generate signing key */
        status = mbedtls_ecdsa_genkey(
                pCtx, ECP_GROUP_ID, mbedtls_ctr_drbg_random, pCtrDrbg);
        if (status == 0)
        {
            status = mbedtls_ecdsa_write_signature(
                    pCtx, MD_TYPE_ID, pHash, hashLength, pSig, &sigLength,
                    mbedtls_ctr_drbg_random, pCtrDrbg);
            if (status == 0)
            {
                status = mbedtls_ecdsa_read_signature(
                        pCtx, pHash, hashLength, pSig, sigLength);
            }
        }
    }

    mbedtls_ecdsa_free(pCtx);

    free(pCtx);
    free(pSig);

	return status;
}

static uint32_t performRandomEDW(void)
{
    size_t dataLength = 200;
    unsigned char *pDataIn = (unsigned char *) malloc(dataLength);

    mbedtls_ctr_drbg_context *pCtrDrbg = &DRBGContext;

    mbedtls_ecdsa_context *pCtx = malloc(sizeof(mbedtls_ecdsa_context));
    mbedtls_ecdsa_init(pCtx);

    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    /* prepare material for signature */
    uint32_t status = mbedtls_ctr_drbg_random(pCtrDrbg, pDataIn, dataLength);
    if (status == 0)
    {
        /* generate signing key */
        status = mbedtls_ecdsa_genkey_edwards(
                pCtx, MBEDTLS_ECP_DP_CURVE25519, mbedtls_ctr_drbg_random, pCtrDrbg);
        if (status == 0)
        {
            status = mbedtls_ecdsa_sign_edwards(
                    &pCtx->grp, &r, &s, &pCtx->d, pDataIn, dataLength);
            if (status == 0)
            {
                status = mbedtls_ecdsa_verify_edwards(
                        &pCtx->grp, pDataIn, dataLength, &pCtx->Q, &r, &s);
            }
        }
    }

    mbedtls_ecdsa_free(pCtx);

    free(pCtx);
    free(pDataIn);

	return status;
}

static uint32_t performECDSA(void)
{
    uint32_t fails = 0;

    if (performRandomPrime() != 0)
    {
    	fails |= ECDSA_FAIL_RANDOM_PRIME;
    }

    if (performRandomVector() != 0)
    {
    	fails |= ECDSA_FAIL_RANDOM_VECTOR;
    }

    if (performDeterministicVector() != 0)
    {
    	fails |= ECDSA_FAIL_DET_VECTOR;
    }

    if (performWriteRead() != 0)
    {
    	fails |= ECDSA_FAIL_READ_WRITE;
    }

    if (performRandomEDW() != 0)
    {
    	fails |= ECDSA_FAIL_RANDOM_EDW;
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
    swmLogInfo("ECDSA Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("ECDSA\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performECDSA);
        if (status == CC_OK)
        {
            swmLogTestPass("ECDSA\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("ECDSA\r\n");
        }
    }

    breakpointHook();

    return 0;
}
