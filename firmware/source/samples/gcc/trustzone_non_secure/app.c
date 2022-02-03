/**
 * @file app.c
 * @brief Trustzone non-secure sample application
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

/* RX and TX buffers */
uint8_t rx_buffer[UART_BUFFER_SIZE];
uint8_t tx_buffer[UART_BUFFER_SIZE] = UART_DATA;

/* TX and RX counters */
volatile unsigned int tx_count = 0;
volatile unsigned int rx_count = 0;

void App_Call_Secure_APIs_Demo(void)
{
    /* Call non-secure call-able function which returns
     * a value incremented value of the input parameter */
    int val = NSC_Func_Inc(30);
    if (val != 31)
    {
        while (1)
        {
            /* Refresh the watch-dog timer */
            SYS_WATCHDOG_REFRESH();    /** An error occurred */
        }
    }

    /* Call non-secure call-able function which returns a
     * decremented value of the input parameter  */
    val = NSC_Func_Dec(30);
    if (val != 29)
    {
        while (1)
        {
            /* Refresh the watch-dog timer */
            SYS_WATCHDOG_REFRESH();    /* An error occurred */
        }
    }
}

void App_Call_GPIO_Demo(void)
{
    /* Non-Secure GPIO access*/
    Sys_GPIO_Toggle(NS_STATE_GPIO);

    /* Call Secure GPIO through Non secure callable function */
    NSC_GPIO_Toggle();
}

void UART_Init(void)
{
    /* Reset and disable UART */
    UART->CTRL = UART_DISABLE | UART_RESET;

    /* Initialize TX counter to 0 */
    tx_count = 0;
    rx_count = 0;
}

void UART_Config_Int(void)
{
    /* Initialize UART to a known state */
    UART_Init();

    GPIO->SRC_UART[0] = (NS_UART_RX_GPIO << GPIO_SRC_UART_RX_Pos) & GPIO_SRC_UART_RX_Mask;

    Sys_UART_Config(UART, UART_CLK_HZ, BAUD_RATE,  (UART_TX_START_INT_ENABLE | UART_RX_INT_ENABLE));

    /* Enable UART RX and TX interrupt */
    NVIC_EnableIRQ(UART0_RX_IRQn);
    NVIC_EnableIRQ(UART0_TX_IRQn);

    /* Unmask interrupts */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);

    /* Enable UART */
    UART->CTRL = UART_ENABLE;
}

void UART0_TX_IRQHandler(void)
{
    if (tx_count < UART_BUFFER_SIZE)
    {
        /* Write the next character to be sent to the UART TX_DATA register */
        UART->TX_DATA = tx_buffer[tx_count++];
    }
}

void UART0_RX_IRQHandler(void)
{
    rx_buffer[rx_count++] = UART->RX_DATA;
}

void UART_StartTX(void)
{
    /* Reset tx count to 0 */
    tx_count = 0;

    /* Kickoff UART transmission */
    UART->TX_DATA = tx_buffer[tx_count++];

    while ((UART->STATUS & UART_TX_BUSY) == UART_TX_BUSY)
    {
        SYS_WATCHDOG_REFRESH();
    }
}

void UART_EchoRX(void)
{
    /* Wait until the whole string (21 bytes + null character) is received */
    if (rx_count == UART_BUFFER_SIZE)
    {
        /* Reset rx count to 0 */
        rx_count = 0;

        /* Copy received string to the tx_buffer */
        memcpy((void *)&tx_buffer, (void *)&rx_buffer, UART_BUFFER_SIZE);

        /* Kick-off the next transmission */
        UART_StartTX();
    }
}

int main(void)
{
    /* Mask all interrupts, and ensure the non-secure vector table is assigned */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);
    SCB->VTOR = (unsigned int)(&ISR_Vector_Table);

    /* Run Secure API call Demo */
    App_Call_Secure_APIs_Demo();

    /* Run Secure GPIO call Demo */
    App_Call_GPIO_Demo();

    /* Configure UART with the interrupts to handle the buffering */
    UART_Config_Int();

    /* Start UART transmission */
	UART_StartTX();

	while(1)
    {
		/* Echo back read data from UART RX */
	    UART_EchoRX();

	    /* Refresh system watchdog timer */
	    SYS_WATCHDOG_REFRESH();
    }

    /* Code execution is not expected to reach here */
    return 0;
}
