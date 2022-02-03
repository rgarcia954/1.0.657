/**
 * @file app.c
 * @brief Main application demonstrates the ability to perform RSA operations
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

#include "mbedtls/rsa.h"
#include "mbedtls/sha1.h"

#include "RSA_vectors.h"

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


#define RSA_FAIL_PUBLIC         0x00000001
#define RSA_FAIL_PRIVATE        0x00000002
#define RSA_FAIL_PUBLIC_PRIVATE 0x00000004
#define RSA_FAIL_CHECK_PRIVATE  0x00000008
#define RSA_FAIL_PKCS_V21       0x00000010
#define RSA_FAIL_GENERATE_KEY   0x00000020

#define RSA_BAD_HEX_STRING      0x00000001
#define RSA_BAD_GEN_KEY         0x00000002
#define RSA_EN_DE_CRYPT_FAIL    0x00000003

#define WORK_BUFFER_SIZE        2048


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
 * @brief Helper routine to convert a buffer of bytes to their
 * to their hexadecimal equivalents.
 *
 * @param obuf An array into which the converted values will be written.
 * @param ibuf An array containing the data to be converted.
 * @param len The length of the input buffer in bytes.
 * @note No bounds checking is done on the output buffer, it is expected
 * to be big enough to hold the generated output.
 */
static void hexify(unsigned char *obuf, const unsigned char *ibuf, int len)
{
    unsigned char l, h;

    while (len != 0)
    {
        h = *ibuf / 16;
        l = *ibuf % 16;

        if (h < 10)
            *obuf++ = '0' + h;
        else
            *obuf++ = 'a' + h - 10;

        if (l < 10)
            *obuf++ = '0' + l;
        else
            *obuf++ = 'a' + l - 10;

        ++ibuf;
        len--;
    }
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

static uint32_t buildPrivateKey(mbedtls_rsa_context *ctx)
{
    uint32_t status = 0;

    mbedtls_mpi p1, q1, h, g;

    mbedtls_mpi_init(&p1);
    mbedtls_mpi_init(&q1);
    mbedtls_mpi_init(&h);
    mbedtls_mpi_init(&g);

    status = mbedtls_mpi_sub_int(&p1, &ctx->P, 1);
    if (status == 0)
    {
        status = mbedtls_mpi_sub_int(&q1, &ctx->Q, 1);
    }
    if (status == 0)
    {
        status = mbedtls_mpi_mul_mpi(&h, &p1, &q1);
    }
    if (status == 0)
    {
        status = mbedtls_mpi_gcd(&g, &ctx->E, &h);
    }
    if (status == 0)
    {
        status = mbedtls_mpi_inv_mod(&ctx->D, &ctx->E, &h);
    }
    if (status == 0)
    {
        status = mbedtls_mpi_mod_mpi(&ctx->DP, &ctx->D, &p1);
    }
    if (status == 0)
    {
        status = mbedtls_mpi_mod_mpi(&ctx->DQ, &ctx->D, &q1);
    }
    if (status == 0)
    {
        status = mbedtls_mpi_inv_mod(&ctx->QP, &ctx->Q, &ctx->P);
    }

    mbedtls_mpi_free(&p1);
    mbedtls_mpi_free(&q1);
    mbedtls_mpi_free(&h);
    mbedtls_mpi_free(&g);

    return status;
}

static uint32_t performRSAPublic(pRSAFields sample)
{
    uint32_t status = 0;

    uint8_t *pMessageStr = malloc(WORK_BUFFER_SIZE);
    memset(pMessageStr, 0, WORK_BUFFER_SIZE);

    uint8_t *pOutput = malloc(WORK_BUFFER_SIZE);
    memset(pOutput, 0, WORK_BUFFER_SIZE);

    uint8_t *pOutputStr = malloc(WORK_BUFFER_SIZE);
    memset(pOutputStr, 0, WORK_BUFFER_SIZE);

    mbedtls_rsa_context *pCtx = malloc(sizeof(mbedtls_rsa_context));
    mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);
    pCtx->len = sample->mod / 8;

    mbedtls_mpi n;
    mbedtls_mpi e;

    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);

    /* Read our parameters from the sample */
    status = mbedtls_mpi_read_string(&n, 16, sample->input_N);
    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&e, 16, sample->input_E);
    }

    /* Setup and check the public key */
    if (status == 0)
    {
        status = mbedtls_rsa_import(pCtx, &n, NULL, NULL, NULL, &e);
        if (status == 0)
        {
            status = mbedtls_rsa_check_pubkey(pCtx);
        }
    }

    /* Check the usage of the public key we have just created */
    if (status == 0)
    {
        if (unhexify(pMessageStr, MESSAGE_HEX_STRING) < 0)
        {
            status = RSA_BAD_HEX_STRING;
        }
        if (status == 0)
        {
            status = mbedtls_rsa_public(pCtx, pMessageStr, pOutput);
            if (status == 0)
            {
                hexify(pOutputStr, pOutput, pCtx->len);
                status = strcasecmp((char*) pOutputStr,
                        sample->public_result_hex_str);
            }
        }
    }

    /* Clear the output buffers for reuse */
    memset(pOutput, 0x00, WORK_BUFFER_SIZE);
    memset(pOutputStr, 0x00, WORK_BUFFER_SIZE);

    /* Verify we can copy a context and use it appropriately */
    if (status == 0)
    {
        mbedtls_rsa_context *pCtx2 = malloc(sizeof(mbedtls_rsa_context));
        mbedtls_rsa_init(pCtx2, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);

        /* Copy and check the public key */
        status = mbedtls_rsa_copy(pCtx2, pCtx);
        if (status == 0)
        {
            status = mbedtls_rsa_check_pubkey(pCtx2);
        }

        /* Use the public key */
        if (status == 0)
        {
            status = mbedtls_rsa_public(pCtx2, pMessageStr, pOutput);
        }

        /* Verify the generated output */
        hexify(pOutputStr, pOutput, pCtx2->len);
        status = strcasecmp((char*) pOutputStr, sample->public_result_hex_str);

        mbedtls_rsa_free(pCtx2);
        free(pCtx2);
    }

    mbedtls_mpi_free(&n);
    mbedtls_mpi_free(&e);

    mbedtls_rsa_free(pCtx);
    free(pCtx);

    free(pOutputStr);
    free(pOutput);
    free(pMessageStr);

    return status;
}

static uint32_t performRSAPrivate(pRSAFields sample)
{
    uint32_t status = 0;

    uint8_t *pMessageStr = malloc(WORK_BUFFER_SIZE);
    memset(pMessageStr, 0, WORK_BUFFER_SIZE);

    uint8_t *pOutput = malloc(WORK_BUFFER_SIZE);
    memset(pOutput, 0, WORK_BUFFER_SIZE);

    uint8_t *pOutputStr = malloc(WORK_BUFFER_SIZE);
    memset(pOutputStr, 0, WORK_BUFFER_SIZE);

    /* Need a DRBG context for this */
    mbedtls_ctr_drbg_context *pCtrDrbg = &DRBGContext;

    mbedtls_rsa_context *pCtx = malloc(sizeof(mbedtls_rsa_context));
    mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);
    pCtx->len = sample->mod / 8;

    mbedtls_mpi p;
    mbedtls_mpi q;
    mbedtls_mpi n;
    mbedtls_mpi e;

    mbedtls_mpi_init(&q);
    mbedtls_mpi_init(&p);
    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);

    /* Read our parameters from the sample */
    status = mbedtls_mpi_read_string(&p, 16, sample->input_P);
    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&q, 16, sample->input_Q);
        if (status == 0)
        {
            status = mbedtls_mpi_read_string(&n, 16, sample->input_N);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(&e, 16, sample->input_E);
            }
        }
    }

    /* Setup and check the private key */
    if (status == 0)
    {
        status = mbedtls_rsa_import(pCtx, &n, &p, &q, NULL, &e);
        if (status == 0)
        {
            status = buildPrivateKey(pCtx);
            if (status == 0)
            {
                status = mbedtls_rsa_check_privkey(pCtx);
            }
        }
    }

    /* Check the usage of the private key */
    if (status == 0)
    {
        if (unhexify(pMessageStr, MESSAGE_HEX_STRING) < 0)
        {
            status = RSA_BAD_HEX_STRING;
        }
        /* Repeat three times to test updating of blinding values */
        for (uint32_t index = 0; index < 3; index++)
        {
            if (status == 0)
            {
                status = mbedtls_rsa_private(pCtx, mbedtls_ctr_drbg_random,
                        pCtrDrbg, pMessageStr, pOutput);
                if (status == 0)
                {
                    hexify(pOutputStr, pOutput, pCtx->len);
                    status = strcasecmp((char*) pOutputStr,
                            sample->private_result_hex_str);
                }

                /* Clear the output buffers for reuse */
                memset(pOutput, 0x00, WORK_BUFFER_SIZE);
                memset(pOutputStr, 0x00, WORK_BUFFER_SIZE);
            }
        }
    }

    /* And as with the public key, ensure the copy works as expected */
    if (status == 0)
    {
        mbedtls_rsa_context *pCtx2 = malloc(sizeof(mbedtls_rsa_context));
        mbedtls_rsa_init(pCtx2, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);

        status = mbedtls_rsa_copy(pCtx2, pCtx);
        if (status == 0)
        {
            status = mbedtls_rsa_check_privkey(pCtx2);
        }
        if (status == 0)
        {
            mbedtls_rsa_private(pCtx2, mbedtls_ctr_drbg_random, pCtrDrbg,
                    pMessageStr, pOutput);
        }
        if (status == 0)
        {
            hexify(pOutputStr, pOutput, pCtx->len);
            status = strcasecmp((char*) pOutputStr,
                    sample->private_result_hex_str);
        }

        mbedtls_rsa_free(pCtx2);
        free(pCtx2);
    }

    mbedtls_rsa_free(pCtx);
    free(pCtx);

    free(pOutputStr);
    free(pOutput);
    free(pMessageStr);

    return status;
}

static uint32_t performRSACheckPublicPrivate(pRSAFields sample)
{
    uint32_t status = 0;

    mbedtls_rsa_context *pPub = malloc(sizeof(mbedtls_rsa_context));
    mbedtls_rsa_context *pPrv = malloc(sizeof(mbedtls_rsa_context));

    mbedtls_mpi pub_n;
    mbedtls_mpi pub_e;

    mbedtls_mpi priv_p;
    mbedtls_mpi priv_q;
    mbedtls_mpi priv_n;
    mbedtls_mpi priv_e;
    mbedtls_mpi priv_d;

    mbedtls_rsa_init(pPub, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);
    mbedtls_rsa_init(pPrv, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);

    pPub->len = sample->mod / 8;
    pPrv->len = sample->mod / 8;

    mbedtls_mpi_init(&pub_n);
    mbedtls_mpi_init(&pub_e);
    mbedtls_mpi_init(&priv_p);
    mbedtls_mpi_init(&priv_q);
    mbedtls_mpi_init(&priv_n);
    mbedtls_mpi_init(&priv_e);
    mbedtls_mpi_init(&priv_d);

    /* Setup public part */
    status = mbedtls_mpi_read_string(&pub_n, 16, sample->input_N);
    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&pub_e, 16, sample->input_E);
        if (status == 0)
        {
            status = mbedtls_rsa_import(pPub, &pub_n, NULL, NULL, NULL, &pub_e);
        }
    }

    /* Set up private part */
    status = mbedtls_mpi_read_string(&priv_p, 16, sample->input_P);
    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&priv_q, 16, sample->input_Q);
        if (status == 0)
        {
            status = mbedtls_mpi_read_string(&priv_n, 16, sample->input_N);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(&priv_e, 16, sample->input_E);
                if (status == 0)
                {
                    status = mbedtls_mpi_read_string(&priv_d, 16,
                            sample->input_D);
                    if (status == 0)
                    {
                        status = mbedtls_rsa_import(pPrv, &priv_n, &priv_p,
                                &priv_q, &priv_d, &priv_e);
                    }
                }
            }
        }
    }

    /* Check the keys */
    if (status == 0)
    {
        status = mbedtls_rsa_complete(pPrv);
        if (status == 0)
        {
            status = mbedtls_rsa_check_pub_priv(pPub, pPrv);
        }
    }

    mbedtls_mpi_free(&pub_n);
    mbedtls_mpi_free(&pub_e);
    mbedtls_mpi_free(&priv_p);
    mbedtls_mpi_free(&priv_q);
    mbedtls_mpi_free(&priv_n);
    mbedtls_mpi_free(&priv_e);
    mbedtls_mpi_free(&priv_d);

    mbedtls_rsa_free(pPub);
    mbedtls_rsa_free(pPrv);

    free(pPub);
    free(pPrv);

    return status;
}

static uint32_t performRSACheckPrivateKey(pRSAFields sample)
{
    uint32_t status = 0;

    mbedtls_rsa_context *pPrv = malloc(sizeof(mbedtls_rsa_context));

    mbedtls_mpi priv_p;
    mbedtls_mpi priv_q;
    mbedtls_mpi priv_n;
    mbedtls_mpi priv_e;
    mbedtls_mpi priv_d;

    mbedtls_rsa_init(pPrv, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);

    pPrv->len = sample->mod / 8;

    mbedtls_mpi_init(&priv_p);
    mbedtls_mpi_init(&priv_q);
    mbedtls_mpi_init(&priv_n);
    mbedtls_mpi_init(&priv_e);
    mbedtls_mpi_init(&priv_d);

    /* Set up private part */
    status = mbedtls_mpi_read_string(&priv_p, 16, sample->input_P);
    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&priv_q, 16, sample->input_Q);
        if (status == 0)
        {
            status = mbedtls_mpi_read_string(&priv_n, 16, sample->input_N);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(&priv_e, 16, sample->input_E);
                if (status == 0)
                {
                    status = mbedtls_mpi_read_string(&priv_d, 16,
                            sample->input_D);
                    if (status == 0)
                    {
                        status = mbedtls_rsa_import(pPrv, &priv_n, &priv_p,
                                &priv_q, &priv_d, &priv_e);
                    }
                }
            }
        }
    }

    /* Check the keys */
    if (status == 0)
    {
        status = mbedtls_rsa_complete(pPrv);
        if (status == 0)
        {
            status = mbedtls_rsa_check_privkey(pPrv);
        }
    }

    mbedtls_mpi_free(&priv_p);
    mbedtls_mpi_free(&priv_q);
    mbedtls_mpi_free(&priv_n);
    mbedtls_mpi_free(&priv_e);
    mbedtls_mpi_free(&priv_d);

    mbedtls_rsa_free(pPrv);

    free(pPrv);

    return status;
}

static uint32_t performRSAPKCSV21(pRSAFields sample)
{
    uint32_t status = 0;

    static const uint32_t PT_LEN = 40;
    static const char *RSA_PT = "385387514deccc7c740dd8cdf9daee49a1cbfd54";

    mbedtls_ctr_drbg_context *pCtrDrbg =
            (mbedtls_ctr_drbg_context*) &DRBGContext;

    mbedtls_rsa_context *pCtx = malloc(sizeof(mbedtls_rsa_context));
    mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);
    pCtx->len = sample->mod / 8;

    unsigned char *pRsaPlaintext = malloc(PT_LEN);
    unsigned char *pRsaDecrypted = malloc(PT_LEN);
    unsigned char *pRsaCiphertext = malloc(pCtx->len);
    unsigned char *pSha1Sum = malloc(20);

    size_t len;

    mbedtls_mpi p;
    mbedtls_mpi q;
    mbedtls_mpi n;
    mbedtls_mpi e;
    mbedtls_mpi d;

    mbedtls_mpi_init(&p);
    mbedtls_mpi_init(&q);
    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&d);

    /* Put the plain text into a buffer */
    if (unhexify(pRsaPlaintext, RSA_PT) < 0)
    {
        status = RSA_BAD_HEX_STRING;
    }

    /* Read the parameters from the sample */
    status = mbedtls_mpi_read_string(&p, 16, sample->input_P);
    if (status == 0)
    {
        status = mbedtls_mpi_read_string(&q, 16, sample->input_Q);
        if (status == 0)
        {
            status = mbedtls_mpi_read_string(&n, 16, sample->input_N);
            if (status == 0)
            {
                status = mbedtls_mpi_read_string(&e, 16, sample->input_E);
                if (status == 0)
                {
                    status = mbedtls_mpi_read_string(&d, 16, sample->input_D);
                }
            }
        }
    }

    /* Import the parameters and check the public key */
    if (status == 0)
    {
        status = mbedtls_rsa_import(pCtx, &n, &p, &q, &d, &e);
        if (status == 0)
        {
            status = mbedtls_rsa_complete(pCtx);
            if (status == 0)
            {
                status = mbedtls_rsa_check_pubkey(pCtx);
            }
        }
    }

    /* Perform encryption and decryption operations */
    if (status == 0)
    {
        status = mbedtls_rsa_pkcs1_encrypt(pCtx, mbedtls_ctr_drbg_random,
                pCtrDrbg, MBEDTLS_RSA_PUBLIC, len, pRsaPlaintext,
                pRsaCiphertext);
        if (status == 0)
        {
            status = mbedtls_rsa_pkcs1_decrypt(pCtx, mbedtls_ctr_drbg_random,
                    pCtrDrbg, MBEDTLS_RSA_PRIVATE, &len, pRsaCiphertext,
                    pRsaDecrypted, PT_LEN);
        }
    }

    /* Check the decrypted output matches out expected result */
    if (status == 0)
    {
        if (memcmp(pRsaDecrypted, pRsaPlaintext, len))
        {
            status = RSA_EN_DE_CRYPT_FAIL;
        }
    }

    /* Now check we can sign and verify using the appropriate keys */
    if (status == 0)
    {
        mbedtls_sha1(pRsaPlaintext, PT_LEN, pSha1Sum);
        status = mbedtls_rsa_pkcs1_sign(pCtx, mbedtls_ctr_drbg_random, pCtrDrbg,
                MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA1, 0, pSha1Sum,
                pRsaCiphertext);
        if (status == 0)
        {
            status = mbedtls_rsa_pkcs1_verify(pCtx, NULL, NULL,
                    MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA1, 0, pSha1Sum,
                    pRsaCiphertext);
        }
    }

    mbedtls_mpi_free(&p);
    mbedtls_mpi_free(&q);
    mbedtls_mpi_free(&n);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&d);

    mbedtls_rsa_free(pCtx);

    free(pRsaPlaintext);
    free(pRsaDecrypted);
    free(pRsaCiphertext);
    free(pSha1Sum);
    free(pCtx);

    return status;
}

static uint32_t performRSAGenerateKey(pRSAFields sample)
{
    uint32_t status = 0;

    /* Only generate the keys on the first iteration */
    if (sample->mod == 2048)
    {
        const int exponent = 3;

        mbedtls_rsa_context *pCtx = malloc(sizeof(mbedtls_rsa_context));
        mbedtls_rsa_init(pCtx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);

        status = mbedtls_rsa_gen_key(pCtx, RNDContext.rndGenerateVectFunc,
                &DRBGContext, sample->mod, exponent);
        if (status == 0)
        {
            status = mbedtls_rsa_check_privkey(pCtx);
            if (status == 0)
            {
                if (mbedtls_mpi_cmp_mpi(&pCtx->P, &pCtx->Q) <= 0)
                {
                    status = RSA_BAD_GEN_KEY;
                }
            }
        }

        mbedtls_rsa_free(pCtx);
        free(pCtx);
    }

    return status;
}

static uint32_t performRSA(void)
{
    uint32_t fails = 0;
    for (uint32_t index = 0; RSA_TABLES[index] != NULL; index++)
    {
        pRSAFields sample = RSA_TABLES[index];
        uint32_t errorShift = (index * 8);
        if (performRSAPublic(sample) != 0)
        {
            fails |= (RSA_FAIL_PUBLIC << errorShift);
        }

        if (performRSAPrivate(sample) != 0)
        {
            fails |= (RSA_FAIL_PRIVATE << errorShift);
        }

        if (performRSACheckPublicPrivate(sample) != 0)
        {
            fails |= (RSA_FAIL_PUBLIC_PRIVATE << errorShift);
        }

        if (performRSACheckPrivateKey(sample) != 0)
        {
            fails |= (RSA_FAIL_CHECK_PRIVATE << errorShift);
        }

        if (performRSAPKCSV21(sample) != 0)
        {
            fails |= (RSA_FAIL_PKCS_V21 << errorShift);
        }

        if (performRSAGenerateKey(sample) != 0)
        {
            fails |= (RSA_FAIL_GENERATE_KEY << errorShift);
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
    swmLogInfo("RSA Sample Code\r\n");

    if (initializeCC312() != CC_LIB_RET_OK)
    {
        swmLogTestFail("RSA\r\n");
    }
    else
    {
        CCStatus status = measureDuration(performRSA);
        if (status == CC_OK)
        {
            swmLogTestPass("RSA\r\n");
            testFailed = 0;
        }
        else
        {
            swmLogTestFail("RSA\r\n");
        }
    }

    breakpointHook();

    return 0;
}
