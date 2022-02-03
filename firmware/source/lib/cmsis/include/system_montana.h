/**
 * @file system_montana.h
 * @brief CMSIS Cortex-M3 Device Peripheral Access Layer Header File
 * 
 * Provides the required system_<Device>.h implementation for CMSIS
 * compatibility
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

#ifndef SYSTEM_MONTANA_H
#define SYSTEM_MONTANA_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/** @addtogroup CMSIS
 *  @{
 */

#include <stdint.h>

/*----------------------------------------------------------------------------
  Externals
 *----------------------------------------------------------------------------*/
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t ISR_Vector_Table;
#endif /* if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U) */

/* ----------------------------------------------------------------------------
 * System Initialization
 * ------------------------------------------------------------------------- */
/**
 * @brief       Initializes the system by clearing and disabling interrupts,
 *              updating the SystemCoreClock variable and updating flash
 *              timing registers based on the read SYS_CLK.
 */
extern void SystemInit(void);

/* ----------------------------------------------------------------------------
 * System Core Clock and Clock Configuration
 * ------------------------------------------------------------------------- */

/** High speed main RC oscillator default frequency set by boot ROM application
 * Default value is 3 MHz uncalibrated. Assuming a worse case of 5 MHz*/
#define DEFAULT_FREQ                    5000000

/** Low speed standby RC oscillator default frequency */
#define STANDBYCLK_DEFAULT_FREQ         32768

/** Frequency of the 48 MHz crystal used for the RF front-end. */
#define RFCLK_BASE_FREQ                 48000000
/** Maximum frequency supported by using an external clock. */
#define EXTCLK_MAX_FREQ                 48000000
/** Maximum frequency supported by the JTAG interface. */
#define JTCK_MAX_FREQ                   48000000
/** Maximum frequency supported by the internal RC oscillator */
#define RCOSC_MAX_FREQ                  12000000

/** Contains the current SYS_CLK frequency, in Hz. */
extern uint32_t SystemCoreClock;

/**
 * @brief       Reads system registers to determine the current system
 *              clock frequency, update the SystemCoreClock variable
 *              and update the flash timing registers accordingly.
 */
extern void SystemCoreClockUpdate(void);

/** @} */ /* End of the CMSIS group */

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* SYSTEM_MONTANA_H */
