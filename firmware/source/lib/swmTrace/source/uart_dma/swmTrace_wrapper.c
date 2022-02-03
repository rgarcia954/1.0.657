/**
 * @file swmTrace_wrapper.c
 * @brief This provides the tracing implementation for the UART/DMA mode.
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

#if SWM_DMA_SOURCE == 0
void DMA0_IRQHandler(void);
#elif SWM_DMA_SOURCE == 1
void DMA1_IRQHandler(void);
#elif SWM_DMA_SOURCE == 2
void DMA2_IRQHandler(void);
#elif SWM_DMA_SOURCE == 3
void DMA3_IRQHandler(void);
#elif SWM_DMA_SOURCE == 4
void DMA4_IRQHandler(void);
#elif SWM_DMA_SOURCE == 5
void DMA5_IRQHandler(void);
#elif SWM_DMA_SOURCE == 6
void DMA6_IRQHandler(void);
#elif SWM_DMA_SOURCE == 7
void DMA7_IRQHandler(void);
#endif

/**
 * @brief Enables the DMA TX interrupt.
 */
static void enable_tx_irqn(void)
{
    int32_t dma_irqn = DMA0_IRQn + (SWM_DMA_SOURCE);

    NVIC_EnableIRQ(dma_irqn);
}

/**
 * @brief Enables the UART RX interrupt.
 */
static void enable_rx_irqn(void)
{
    int32_t uart_rx_irqn = UART0_RX_IRQn + 2 * (SWM_UART_SOURCE);

    NVIC_EnableIRQ(uart_rx_irqn);
}

/**
 * @brief UART initialization routine.
 * @details This sets up the interrupt enable if required and configures the
 * clock and baud rates. It also initialises the selected DMA channel.
 * @param enable_rx A flag indicating of the rx interrupt should be enabled
 * on the UART.
 */
void swmTrace_UART_init(bool enable_rx)
{
    uint32_t dma_dest;

    dma_dest = DMA_DEST_UART0 + ((SWM_UART_SOURCE) << DMA_CFG0_DEST_SELECT_Pos);

    DMA[SWM_DMA_SOURCE].STATUS = DMA_COMPLETE_INT_CLEAR;

    /* Configure DMA on TX and interrupt on RX */
    uint32_t enables = UART_TX_DMA_ENABLE;
    enables |= enable_rx ? UART_RX_INT_ENABLE : 0;

    /* Get UARTCLK frequency in Hz */
    uint32_t uart_clk = Sys_UART_GetClk();

    Sys_UART_Config(&UART[SWM_UART_SOURCE], uart_clk, selected_baud_rate, enables);

    /* Configure TX */
    DMA[SWM_DMA_SOURCE].DEST_ADDR = (uint32_t) (&(UART[SWM_UART_SOURCE].TX_DATA));
    DMA[SWM_DMA_SOURCE].CFG0 =
            DMA_COMPLETE_INT_ENABLE |
            dma_dest |
            DMA_SRC_ADDR_INCR_1 |
            WORD_SIZE_8BITS_TO_8BITS;

    /* Enable UART RX and TX DMA interrupts */
    enable_tx_irqn();
    if (enable_rx)
    {
        enable_rx_irqn();
    }
}

/**
 * @brief Helper routine which will send the next block of characters if they
 * exist in our trace queue.
 * @details
 * In cases where the range of characters to be sent wraps round the end of
 * the circular buffer, the transaction will be split into two operations.
 * @note
 * When there are no more characters to be sent, this will
 * clear the tx_in_progress flag.
 */
void swmTrace_send(void)
{
    if (tx_r_ptr != tx_w_ptr)
    {
        uint32_t count = (tx_w_ptr > tx_r_ptr) ? tx_w_ptr : UART_TX_BUFFER_SIZE;
        count -= tx_r_ptr;

        uint32_t start = (uint32_t) &tx_buffer[tx_r_ptr];
        tx_r_ptr = (tx_r_ptr + count) & UART_TX_BUFFER_MASK;

        /* Clear buffer and counter, re-enable TX DMA for next transmission */
        DMA[SWM_DMA_SOURCE].SRC_ADDR = start;
        DMA[SWM_DMA_SOURCE].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
        DMA[SWM_DMA_SOURCE].CFG1 = count << DMA_CFG1_TRANSFER_LENGTH_Pos;
        DMA[SWM_DMA_SOURCE].CTRL = DMA_ENABLE_WRAP;
    }
    else
    {
        tx_in_progress = false;
    }
}

/**
 * @brief Helper function to be called after a DMA interrupt.
 */
static void send_dma(void)
{
    if ((DMA[SWM_DMA_SOURCE].STATUS & DMA_COMPLETE_INT_TRUE) == DMA_COMPLETE_INT_TRUE)
    {
        // Clear interrupt flag
        DMA[SWM_DMA_SOURCE].STATUS = DMA_COMPLETE_INT_CLEAR;

        // Check if any more data should be sent
        swmTrace_send();
    }
}

#if SWM_DMA_SOURCE == 0
/**
 * @brief Interrupt Service Routine for the DMA0. This is invoked on
 * completion of each DMA transmission.
 */
void DMA0_IRQHandler(void)
{
    send_dma();
}

#elif SWM_DMA_SOURCE == 1
/**
 * @brief Interrupt Service Routine for the DMA1. This is invoked on
 * completion of each DMA transmission.
 */
void DMA1_IRQHandler(void)
{
    send_dma();
}

#elif SWM_DMA_SOURCE == 2
/**
 * @brief Interrupt Service Routine for the DMA2. This is invoked on
 * completion of each DMA transmission.
 */
void DMA2_IRQHandler(void)
{
    send_dma();
}

#elif SWM_DMA_SOURCE == 3
/**
 * @brief Interrupt Service Routine for the DMA3. This is invoked on
 * completion of each DMA transmission.
 */
void DMA3_IRQHandler(void)
{
    send_dma();
}

#elif SWM_DMA_SOURCE == 4
/**
 * @brief Interrupt Service Routine for the DMA4. This is invoked on
 * completion of each DMA transmission.
 */
void DMA4_IRQHandler(void)
{
    send_dma();
}

#elif SWM_DMA_SOURCE == 5
/**
 * @brief Interrupt Service Routine for the DMA5. This is invoked on
 * completion of each DMA transmission.
 */
void DMA5_IRQHandler(void)
{
    send_dma();
}

#elif SWM_DMA_SOURCE == 6
/**
 * @brief Interrupt Service Routine for the DMA6. This is invoked on
 * completion of each DMA transmission.
 */
void DMA6_IRQHandler(void)
{
    send_dma();
}

#elif SWM_DMA_SOURCE == 7
/**
 * @brief Interrupt Service Routine for the DMA7. This is invoked on
 * completion of each DMA transmission.
 */
void DMA7_IRQHandler(void)
{
    send_dma();
}
#endif /* SWM_DMA_SOURCE */
