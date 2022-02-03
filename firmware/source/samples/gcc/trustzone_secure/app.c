/**
 * @file app.c
 * @brief Trustzone secure sample application
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
#include "app.h"

int App_Secure_Api_Inc(int x)
{
    return x + 1;
}

int App_Secure_Api_Dec(int x)
{
    return x - 1;
}

/* For GPIO access test without using UART */
void GPIO_Toggle(void)
{
	Sys_GPIO_Toggle(S_STATE_GPIO);
}

int main(void)
{
	Sys_Delay((uint32_t)(1 * SystemCoreClock)); /* 1s delay */

	SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
	while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
	{
		SYS_WATCHDOG_REFRESH();
	}

	/* Enable Global interrupts */
	Sys_NVIC_DisableAllInt(); /* Disable all interrupts */
	Sys_NVIC_ClearAllPendingInt(); /* Clear all pending interrupts */
	__set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
	__set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);

	SYS_GPIO_CONFIG(S_STATE_GPIO, GPIO_MODE_GPIO_OUT);

    /* Configure 48Mhz oscillator to generate 8 MHz UART Clock */
    App_Clock_Config();

    /* Configure Trust zone */
    TZ_Configure();

    /* Run non secure application */
    TZ_Non_Secure_Application();

    /* Execution is not expected to reach here */
    while (1)
    {
        SYS_WATCHDOG_REFRESH();
    }

    /* Execution is not expected to reach here */
    return 0;
}

void App_Clock_Config(void)
{
    /* Enable VDDRF power domain before enable 48 MHz oscillator
     * Note System clock frequency of 1 MHz or higher is recommended for safe
     * flash programming. Additionally, the internal RC clock source cannot
     * be used to erase or to program the Flash as it is temperature sensitive.
     */
    /* Load default trim values. */
    uint32_t trim_error __attribute__((unused)) = SYS_TRIM_LOAD_DEFAULT();

    /* Start 48 MHz XTAL oscillator */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8);

    /* Disable JTAG TDI, TDO, and TRST connections to GPIO 2, 3, and 4 */
    GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);

}
