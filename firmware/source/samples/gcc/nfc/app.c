/**
 * @main.c
 * @brief C file for NFC sample application
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

#include "app.h"

int main()
{
    uint32_t trim_error __attribute__((unused));

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

    // Load default trim values.
    trim_error = SYS_TRIM_LOAD_DEFAULT();

    // Switch to the RC oscillator (multiplied) as the clock source, and update the
    // SystemCoreClock global variable.
    ACS->RCOSC_CTRL = RC_OSC_12MHZ | RC_OSC_P46P5;
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);

	// Update the SystemCoreClock global variable.
	SystemCoreClockUpdate();

    // Switch to the RC oscillator (multiplied) as the clock source, and update the
    // SystemCoreClock global variable.
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);

    __enable_irq();
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);

    /* Initialize NFC hardware and software tables */
    NFC_Initialize();

    while (1)
    {
        /* Respond to any incoming NFC request */
        NFC_Task();
    }
    return 0;
}
