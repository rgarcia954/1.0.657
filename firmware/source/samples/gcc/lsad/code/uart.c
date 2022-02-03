/**
 * @file uart.c
 * @brief UART operational code
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

void UART_TX_Configure(void)
{
    /* Note that GPIO configuration of UART interface is done by
     * the secure application. Non secure application does not have
     * access to this register.
     */
    Sys_UART_Config(UART, SystemCoreClock, UART_BAUD_RATE, UART_TX_END_INT_DISABLE);
    UART->CTRL = UART_ENABLE;
}

void UART_Send_Char(char c)
{
    /* Write data to UART-TX buffer */
    UART->TX_DATA = c;

    /* Wait until transmission has been completed */
    while ((UART->STATUS & UART_TX_BUSY)  == UART_TX_BUSY)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }
}

void UART_Send_String(char *string)
{
    /* Transmit string until the null termination character */
    while (*string != '\0')
    {
        UART_Send_Char(*string++);
    }
}
