/**
 * @file app.h
 * @brief Provides the basic structure of a header file for print_device_info
 * This is just a basic shell application. It is mainly to show the header
 * structure and give a starting point for further development.
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
#include <stdio.h>
#include <trim.h>
#include <bondlist.h>
#ifdef SWMTRACE_ENABLE
#include <swmTrace_api.h>
#endif    /* SWMTRACE_ENABLE */
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

/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO                   0

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

#ifdef SWMTRACE_ENABLE
/* The GPIO pin to use for TX when using the UART mode */
#define UART_TX_GPIO                    6

/* The GPIO pin to use for RX when using the UART mode */
#define UART_RX_GPIO                    5

/* The selected baud rate for the application when using UART mode */
#define UART_BAUD                       115200

#define printf                          swmLogInfo
#endif    /* SWMTRACE_ENABLE */
/* ---------------------------------------------------------------------------
* Function prototype definitions
* --------------------------------------------------------------------------*/

/**
 * @fn          App_Clock_Config(void)
 * @brief       Configures system clock to use the 48 MHz XTAL oscillator
 *              prescaled to a known value of 8 MHz. An accurate and known
 *              clock is needed for proper functioning.
 * @return      none
 */
void App_Clock_Config(void);

/**
 * @brief       Prints the Bluetooth Bond Info stored at Flash Address
 * @assumptions BondInfo was already stored inside the flash to be printed
 */
void printBondInfo(void);

/**
 * @brief       Prints the  Calibration values
 * @param[in]   trim_values Pointer to section of memory containing trim values
 */
void printTrimCalibrationValues(TRIM_Type *trim_values);

#ifdef SWMTRACE_ENABLE
/**
 * @brief Initialize swmTrace.
 *
 * This function initialize SWM trace with swmtrace options
 */
void SWMTraceInit(void);
#endif    /* SWMTRACE_ENABLE */
/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H_ */
