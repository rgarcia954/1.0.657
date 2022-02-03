/**
 * @file app.h
 * @brief Header file for main application
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

#ifndef APP_H_
#define APP_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <hw.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "gpio_driver.h"
#include "uart_driver.h"
#include "RTE_Device.h"

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#if !RTE_USART0_ENABLED
    #error "This example uses UART0, please ensure RTE_UART0_ENABLED is set in \
RTE_Device.h"
#endif    /* if !RTE_USART0_ENABLED */

/* GPIO definitions */
#define RECOVERY_GPIO                   0
#define APP_UART_SEND_GPIO              BLUE_LED
#define APP_UART_RECEIVE_GPIO           GREEN_LED

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/* Enable/disable buck converter
 * Options: VCC_BUCK or VCC_LDO
 */
#define VCC_BUCK_LDO_CTRL               VCC_LDO


/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/
extern ARM_DRIVER_USART Driver_USART0;
extern DRIVER_GPIO_t Driver_GPIO;

/* ----------------------------------------------------------------------------
 * Function prototypes
 * --------------------------------------------------------------------------*/
/**
 * @brief       UART event handler
 */
void UART_EventCallBack(uint32_t event);

/**
 * @brief       Button event handler
 */
void Button_EventCallback(uint32_t event);

/**
 * @brief       Toggle the specified GPIO 'n' times, delayed by 'delay_ms'
 *                 milliseconds.
 * @param[in]   n  Number of toggles
 * @param[in]   delay_ms  Delay between each toggle
 * @param[in]   is_send  True for ARM_USART_EVENT_SEND_COMPLETE event
 */
void Toggle_Uart_Event_GPIO(uint32_t n, uint32_t delay_ms, bool is_send);

/**
 * @brief Initialize the system, including GPIOs and interrupts.
 * @return Zero
 */
void Initialize(void);

/**
 * @brief The main entry point for the program
 */
int main(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H_ */
