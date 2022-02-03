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

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

#include <hw.h>
#include "flash_rom.h"
#include "secure_api.h"
#include "RTE_Device.h"

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* Exported variables from linker script */

/** Start address of non-secure call-able gateway functions in flash */
extern unsigned long __Start_Of_FLASH_SG__;

/** End address of non-secure call-able gateway functions in flash */
extern unsigned long __End_Of_FLASH_SG__;

/** Start address of non-secure application area in flash */
extern unsigned long __Start_Of_FLASH_NS__;

/** End address of non-secure application area in flash */
extern unsigned long __End_Of_FLASH_NS__;

/** Start address of non-secure application area in RAM */
extern unsigned long __Start_Of_DRAM_NS__;

/** End address of non-secure application area in RAM */
extern unsigned long __End_Of_DRAM_NS__;

/** Typedef for non-secure callback functions */
typedef void (*funcptr_ns)(void) __attribute__ ((cmse_nonsecure_call));

/**
 * @brief  Secure API function to increment input integer by 1
 * @param [in] x Input integer parameter
 * @return Input integer plus 1
 */
int App_Secure_Api_Inc(int x);

/**
 * @brief  Secure API function to decrement input integer by 1
 * @param [in] x Input integer parameter
 * @return Input integer minus 1
 */
int App_Secure_Api_Dec(int x);

void GPIO_Toggle(void);

void NSC_GPIO_Toggle(void);

/**
 * @brief  Enable 48 MHz to generate clock for UART
 */
void App_Clock_Config(void);

/** Trust Zone functions */
/**
 * @brief  Configure IDAU to enable non secure GPIO
 */
void TZ_IDAU_GPIO_Configure(void);

/**
 * @brief  Configure IDAU to enable RAM regions for non secure application
 */
void TZ_IDAU_MEM_Configure(void);

/**
 * @brief  Configure SAU for NS and NSC regions
 */
void TZ_SAU_Configure(void);

/**
 * @brief  Configure NVIC for non-secure interrupts
 */
void TZ_NS_NVIC_Configure(void);

/**
 * @brief  Configure Trust-zone IDAU and SAU
 */
void TZ_Configure(void);

/**
 * @brief  Run the non-secure application
 */
void TZ_Non_Secure_Application(void);

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
