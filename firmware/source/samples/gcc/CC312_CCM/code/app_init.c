/**
 * @file app_init.c
 * @brief Application initialization.
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

#include "hw.h"
#include "app.h"

/**
 * @brief GPIO number that is used for easy re-flashing (recovery mode).
 */
#define RECOVERY_GPIO                   0

/**
 * @brief Initialize the system to a known good state.
 */
void initialize(void)
{
    /* Interrupts off */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Load default trim values */
    uint32_t trim_error __attribute__((unused)) = SYS_TRIM_LOAD_DEFAULT();

    /* Test the recovery GPIO to pause the program to make it easy to re-flash */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
       SYS_WATCHDOG_REFRESH();
    }

    /* Enable the 48MHz XTAL */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Clear any pending CC312 interrupts */
    NVIC_ClearPendingIRQ(CC312_IRQn);

    /* Stop the SysTick until we need to use it for measurement */
    SysTick->CTRL = 0;

    /* Initialize it to the system core clock, providing a 1 second counter */
    SysTick->LOAD = SystemCoreClock - 1;

    /* Enable the SysTick counter interrupt, this will be used when measuring */
    NVIC_EnableIRQ(SysTick_IRQn);

    /* Enable CC312 interrupts */
	NVIC_EnableIRQ(CC312_IRQn);

    /* Interrupts back on */
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
}

