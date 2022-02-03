/**
 * @file app.h
 * @brief Application header file for the UART sample application
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
#ifndef APP_H
#define APP_H

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <hw.h>
#include <string.h>
#include <app_uart_cfg.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#define UART_CLK_HZ                     8000000
#define UART_TX_GPIO                    6
#define UART_RX_GPIO                    5
#define BAUD_RATE                       115200
#define UART_DATA                       "onsemi UART"
#define UART_BUFFER_SIZE                (sizeof(UART_DATA))
#define UART_DMA_ENABLED                0

/* ----------------------------------------------------------------------------
 * Global variables
 * --------------------------------------------------------------------------*/
extern uint8_t rx_buffer[UART_BUFFER_SIZE];
extern uint8_t tx_buffer[UART_BUFFER_SIZE];

/* ---------------------------------------------------------------------------
* Function prototype definitions
* --------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------
 * Function      : App_Clock_Config(void)
 * ----------------------------------------------------------------------------
 * Description   : Configures system clock to use the 48 MHz XTAL oscillator
 *                 prescaled to a known value of 8 MHz. An accurate and known
 *                 clock is needed for proper functioning of UART.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void App_Clock_Config(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
