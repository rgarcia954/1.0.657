/**
 * @file app.h
 * @brief Application header file for the trust zone secure sample application
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

#include <hw.h>
#include "secure_api.h"
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/**
 * @brief  Defines
 */

#define UART_CLK_HZ                     8000000
#define BAUD_RATE                       115200
#define UART_DATA                       "onsemi UART"

#define UART_BUFFER_SIZE                (sizeof(UART_DATA))

/**
 * @brief  Initializes UART to known configuration
 */
extern void UART_Init(void);

/**
 * @brief  Configures UART with RX and TX interrupts enabled to
 *         manually handle the RX and TX buffers
 */
extern void UART_Config_Int(void);

/**
 * @brief   UART TX interrupt handler
 */
extern void UART0_TX_IRQHandler(void);

/**
 * @brief  UART RX interrupt handler
 */
extern void UART0_RX_IRQHandler(void);

/**
 * @brief  Kicks off a UART transmission
 */
extern void UART_StartTX(void);

/**
 * @brief  Echoes back newly received UART RX data over UART TX
 */
extern void UART_EchoRX(void);

/**
 * @brief  Initializing static application data
 */

/**
 * @brief  Run secure API demo
 */
void App_Call_Secure_APIs_Demo(void);

/**
 * @brief  Run secure GPIO demo
 */
void App_Call_GPIO_Demo(void);


#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
