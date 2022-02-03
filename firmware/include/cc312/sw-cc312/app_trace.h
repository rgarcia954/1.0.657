/**
 * @file app_trace.h
 * @brief Trace application functions
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

#ifndef APP_TRACE_H
#define APP_TRACE_H

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <hw.h>
#include <stdio.h>
#include <stdarg.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#define DBG_NO              0
#define DBG_UART            1 /* Note: if application uses UART, debugging over UART is not possible */
#define DBG_RTT             2 /* Note: for RTT debugging, please add SEGGER RTT files into your application */
#ifndef PAL_DEBUG
#define PAL_DEBUG           DBG_NO
#endif

#if (PAL_DEBUG == DBG_RTT)

/* Include the Segger RTT files into the project */
#include <swmTrace_api.h>

#endif    /* if (PAL_DEBUG == DBG_RTT) */

#if !defined (PAL_DEBUG)
#define PAL_DEBUG         DBG_NO
#endif    /* if !defined (PAL_DEBUG) */

#define DMA_UART_TX         2
#define DMA_UART_RX         3
#define GPIO_UART_TX        0
#define GPIO_UART_RX        1
#define UART_BAUD_RATE      115200
#define MAX_SIZE_STR        255

#define THREE_BLOCK_APPN(x, y, z)       x##y##z
#define DMA_IRQn(x)                     THREE_BLOCK_APPN(DMA, x, _IRQn)
#define DMA_IRQ_FUNC(x)                 THREE_BLOCK_APPN(DMA, x, _IRQHandler)

#if (PAL_DEBUG == DBG_UART)
extern char UARTTXBuffer[MAX_SIZE_STR];
extern char UARTRXBuffer[MAX_SIZE_STR];
extern volatile uint8_t tx_busy;
#endif    /* if (PAL_DEBUG == DBG_UART) */

/* Assertions for critical errors */
#define ASSERT(msg)                                     \
    do                                                  \
    {                                                   \
        if (!(msg))                                     \
        {                                               \
            assert_error(__MODULE__, __LINE__, #msg);   \
        }                                               \
    } while (0)

#if (PAL_DEBUG == DBG_NO)
#define TRACE_INIT()
#define PRINTF(...)
#else    /* if (PAL_DEBUG == DBG_NO) */

/* Initialize trace port */
#define TRACE_INIT()            trace_init()
#if (PAL_DEBUG == DBG_RTT)
#define PRINTF(...) swmTrace_printf(__VA_ARGS__)
#elif (PAL_DEBUG == DBG_UART)

#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, \
                      ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define PRINTF(...) UART_printf(VA_NARGS(__VA_ARGS__), __VA_ARGS__)

#endif    /* if (PAL_DEBUG == DBG_RTT) */

#endif    /* if (PAL_DEBUG == DBG_NO) */

/* ----------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/
void assert_error (const char *file, const int line, const char *msg);

void trace_init (void);

int UART_printf(const int num, const char *sFormat, ...);

void DMA_IRQ_FUNC(DMA_UART_TX)(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_TRACE_H */
