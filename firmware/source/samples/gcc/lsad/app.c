/**
 * @file app.c
 * @brief Sample application code for LSAD
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

LSAD_Type *lsad = LSAD;

int main(void)
{
    uint32_t trim_error;
    lsad = LSAD;

    /* Disable all interrupts */
    Sys_NVIC_DisableAllInt();

    /* Clear all pending interrupts */
    Sys_NVIC_ClearAllPendingInt();

    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Check for recovery GPIO to enable recovery. */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while ((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }
	
	/* Disable JTAG TDI, TDO, and TRST connections to GPIO 2, 3, and 4 */
    GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);
	
    /* Load default trim values. */
    trim_error = SYS_TRIM_LOAD_DEFAULT();

    /* Enable CPU interrupts */
    __enable_irq();
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);

    /* Enable 48 MHz oscillator to generate a 1 MHz slow clock */
    App_Clock_Config();

    /* Update the SystemCoreClock global variable. */
    SystemCoreClockUpdate();

    /* Configure GPIO for LSAD and UART operation */
    App_GPIO_Config();

    /* Configure the UART interface baud rate and interrupt settings */
    UART_TX_Configure();

    /* Configure LSAD input channel and start conversion */
    LSAD_Demo_Config(trim_error);

    while (1)
    {
        /* Wait for an LSAD event */
        __WFI();

        /* Process new LSAD data */
        process_lsad_new_samples();

        /* Refresh watch-dog timer */
        SYS_WATCHDOG_REFRESH();
    }
}

void App_GPIO_Config(void)
{
    /* Disconnect GPIO1 from digital circuitry for LSAD input */
    GPIO->CFG[USER_GPIO] = GPIO_MODE_DISABLE | GPIO_NO_PULL;

    /* Connect GPIO5 to UART0 TX */
    GPIO->CFG[UART_GPIO] = GPIO_MODE_UART0_TX | GPIO_WEAK_PULL_UP;

    /* Disconnect GPIO 2 and 7 from digital circuitry for LSAD input */
    GPIO->CFG[THERMISTOR_CURRENT_GPIO] = GPIO_MODE_DISABLE | GPIO_NO_PULL;
    GPIO->CFG[THERMISTOR_GPIO] = GPIO_MODE_DISABLE | GPIO_NO_PULL;
}

void App_Clock_Config(void)
{
    /* Enable 48 MHz oscillator to generate 8 MHz SystemClock */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Configure clock dividers */
	Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);
}
