/**
 * @file app.c
 * @brief Main application source file.
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

int main(void)
{
    /* Initialize the system clock to a known clock rate. System clock rate is
     * required for configuring UART */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);
    
    /* Switch to 48 MHz oscillator clock, and update the 
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);
    
    /* Configure clock dividers */
    CLK->DIV_CFG0 = SLOWCLK_PRESCALE_8 | BBCLK_PRESCALE_1 | UARTCLK_PRESCALE_1;

    /* Configure UART with the interrupts or the DMA to handle the buffering */
#if UART_DMA_ENABLED
    UART_Config_DMA();
#else
    UART_Config_Int();
#endif

    /* Start UART transmission */
    UART_StartTX();

    while(1)
    {
        /* Echo back read data from UART RX */
        UART_EchoRX();

        /* Refresh system watchdog timer */
        SYS_WATCHDOG_REFRESH();

        /* Wait here until the next interrupt */
        asm("WFI");
    }

    /* Statement never reached */
    return 0;
}
