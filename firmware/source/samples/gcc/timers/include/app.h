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
#include <RTE_Device.h>
#include <gpio_driver.h>
#include <timer_driver.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#if !RTE_TIMER
    #error "Please configure TIMER in RTE_Device.h"
#endif    /* if !RTE_TIMER */

/* GPIO definitions */
#define RECOVERY_GPIO                   0
#define TIMER_STATES_GPIO              GREEN_LED
#define TIMER_CAPTURE_MODE_GPIO        BLUE_LED

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

/* Timeout values */
#define TIMER0_BASE_TIMEOUT             0x3000
#define TIMER0_TIMEOUT_STEP             0x800

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/
extern DRIVER_GPIO_t Driver_GPIO;
extern DRIVER_TIMER_t Driver_TIMER;

/* ----------------------------------------------------------------------------
 * Function prototypes
 * --------------------------------------------------------------------------*/
extern void Button_EventCallback(uint32_t event);
extern void Timer_EventCallback(uint32_t event);

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
