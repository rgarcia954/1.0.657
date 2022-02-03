/**
 * @file app_initialize.c
 * @brief Initialization routine.
 * @details
 * This will set the Interrupt levels, enable the 48MHz oscillator to provide
 * an 8MHz core clock rate.
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

#include "hw.h"
#include "rom_vect.h"

#include "app_initialize.h"

/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO                   0

void systemInitialize(void) {

    Sys_NVIC_DisableAllInt(); // Disable all interrupts
    Sys_NVIC_ClearAllPendingInt(); // Clear all pending interrupts

    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    // Check for recovery GPIO to enable recovery.
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
       SYS_WATCHDOG_REFRESH();
    }

    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    // Configure clock dividers
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    // Load default trim values.
    SYS_TRIM_LOAD_DEFAULT();

    // re-enable interrupt masks as needed
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
}

