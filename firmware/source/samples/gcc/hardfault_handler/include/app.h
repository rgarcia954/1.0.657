/**
 * @file app.h
 * @brief Header file for the hardfault_handler sample app
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
 * @fn          exec_usage_fault(void)
 * @brief       Executes a usage fault by dividing by zero
 * @return      none
 */
void exec_usage_fault(void);

/**
 * @fn          exec_bus_fault(void)
 * @brief       Executes a bus fault by writing to the null pointer
 * @return      none
 */
void exec_bus_fault(void);

/**
 * @fn          exec_mem_fault(void)
 * @brief       Executes a memory management fault by jumping to an illegal address
 * @return      none
 */
void exec_mem_fault(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H_ */
