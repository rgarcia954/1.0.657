/**
 * @file app.h
 * @brief Application header file for the spi_cmsis_driver sample
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
#include <spi_driver.h>
#include "RTE_Device.h"

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#if (!RTE_SPI0_ENABLED)
    #error "This example uses SPI0 instance, please ensure \
RTE_SPI0_ENABLED is set in RTE_Device.h"
#endif    /* if (!RTE_SPI0_ENABLED) */

/* GPIO toggled when SPI transfer finished or error occurred  */
#define APP_SPI0_EVENT_GPIO            GREEN_LED

/* Recovery GPIO to hold application on reset */
#define RECOVERY_GPIO                  0

/* SPI related definitions. TX/RX pins are configured in RTE_Device.h in
 * Drivers library */
#define APP_SPI_DATA                   "onsemi SPI"
#define APP_SPI_BUFFER_SIZE            (sizeof APP_SPI_DATA)

/* Supported SPI_SPEED options:
 * SystemClock / 1024 to SystemClock / 2.
 * 4.88 kHz to 2.5 MHz for a default system clock of 5 MHz */
#define APP_SPI_SPEED                  150000

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/
extern ARM_DRIVER_SPI Driver_SPI0;

/**
 * @brief       SPI event handler for the SPI master instance
 */
void Master_CallBack(uint32_t event);

/**
 * @brief       Toggle the specified GPIO 'n' times, delayed by 'delay_s'
 *                 seconds.
 * @param[in]   gpio GPIO number
 * @param[in]    n  Number of toggles
 * @param[in]    delay_s  Delay between each toggle
 */
void ToggleGPIO(uint32_t gpio, uint32_t n, float delay_s);

/**
 * @brief       Initialize the system by disabling and clearing
 *                 (pending) interrupts, disabling JTAG connections, and by
 *                 configuring the EVENT GPIO
 */
void Initialize(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
