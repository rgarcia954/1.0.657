/**
 * @file swmTrace_wrapper.c
 * @brief This provides the tracing implementation for the UART mode.
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

#include "uart_common.h"
#include "swmTrace_int.h"
#include "swmTrace_options.h"
#include "hw.h"

#if SWM_UART_SOURCE == 0
void UART0_TX_IRQHandler(void);
#elif SWM_UART_SOURCE == 1
void UART1_TX_IRQHandler(void);
#endif

/**
 * @brief UART Initialization routine.
 * @details This sets up the interrupt enable if required and configures the
 * clock and baud rates.
 * @param enable_rx A flag indicating of the rx interrupt should be enabled
 * on the UART.
 */
void swmTrace_UART_init(bool enable_rx)
{
    int32_t uart_rx_irqn;
    int32_t uart_tx_irqn;

    uart_rx_irqn = UART0_RX_IRQn + 2 * (SWM_UART_SOURCE);
    uart_tx_irqn = UART0_TX_IRQn + 2 * (SWM_UART_SOURCE);

    /* Enable interrupts on TX & RX */
    uint32_t enables = UART_TX_START_INT_ENABLE;
    enables |= enable_rx ? UART_RX_INT_ENABLE : 0;

    /* Get UARTCLK frequency in Hz */
    uint32_t uart_clk = Sys_UART_GetClk();

    Sys_UART_Config(&UART[SWM_UART_SOURCE], uart_clk, selected_baud_rate, enables);

    /* Enable UART RX and TX interrupts */
    if (enable_rx)
    {
        NVIC_EnableIRQ(uart_rx_irqn);
    }
    NVIC_EnableIRQ(uart_tx_irqn);
}

/**
 * @brief Helper routine which sends the next character if it exists in
 * our trace queue.
 * @note
 * When there are no more characters to be sent, this
 * clears the tx_in_progress flag.
 */
void swmTrace_send(void)
{
    if (tx_r_ptr != tx_w_ptr)
    {
        char value = tx_buffer[tx_r_ptr];
        tx_r_ptr = swmTrace_next(tx_r_ptr, UART_TX_BUFFER_MASK);
        UART[SWM_UART_SOURCE].TX_DATA = value;
    }
    else
    {
        tx_in_progress = false;
    }
}

#if SWM_UART_SOURCE == 0
/**
 * @brief Interrupt Service Routine for the UART0 TX. This is invoked on
 * completion of each character transmission.
 */
void UART0_TX_IRQHandler(void)
{
    swmTrace_send();
}

#elif SWM_UART_SOURCE == 1
/**
 * @brief Interrupt Service Routine for the UART1 TX. This is invoked on
 * completion of each character transmission.
 */
void UART1_TX_IRQHandler(void)
{
    swmTrace_send();
}
#endif /* SWM_UART_SOURCE */
