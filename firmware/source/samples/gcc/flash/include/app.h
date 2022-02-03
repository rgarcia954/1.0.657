/**
 * @file app.h
 * @brief Application header file for the flash sample application
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
#include <string.h>
#include "flash_rom.h"

/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO    0

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/**
 * @brief Capture errors in the demo application
 * @param[in] error     Error flag indicating which step failed
 * @param[in] result    Error code from the flash library function that failed
 */
void App_Demo_Error(uint8_t error, FlashStatus_t result);

/**
 * @brief Configure System Clock
 */
void App_Clock_Config(void);

/**
 * @brief Demonstrate the flash sequential write operation
 * @param[in] addr  Location to be written by the demonstration
 */
void App_Demo_SequentialWrite(uint32_t addr);

/**
 * @brief Demonstrate the flash word operation
 * @param[in] addr  Location to be written by the demonstration
 */
void App_Demo_WordWrite(uint32_t addr);

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
