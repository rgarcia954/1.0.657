/**
 * @file app_uart_cfg.c
 * @brief UART configuration and transfer functions
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
#include <app.h>

/* RX and TX buffers */
uint8_t rx_buffer[UART_BUFFER_SIZE];
uint8_t tx_buffer[UART_BUFFER_SIZE] = UART_DATA;

/* TX and RX counters */
volatile unsigned int tx_count;
volatile unsigned int rx_count;

void UART_Init(void)
{
    /* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

    /* Disable and clear all pending interrupts */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Reset and Disable UART */
    UART->CTRL = UART_DISABLE | UART_RESET;

    /* Reset all UART GPIOs to a known state */
    GPIO->SRC_UART[0] = UART_RX_SRC_CONST_HIGH;
    SYS_GPIO_CONFIG(UART_TX_GPIO, (GPIO_MODE_DISABLE | GPIO_NO_PULL));
    SYS_GPIO_CONFIG(UART_RX_GPIO, (GPIO_MODE_DISABLE | GPIO_NO_PULL));

    /* Initialize TX and RX counters to 0 */
    tx_count = 0;
    rx_count = 0;

    /* Initialize rx_buffer to all zeroes */
    memset(rx_buffer, 0x00, UART_BUFFER_SIZE);
}

void UART_Config_Int(void)
{
    /* Initialize UART to a known state */
    UART_Init();

    /* Configure UART, and the RX and TX GPIOs */
    Sys_UART_GPIOConfig(UART, (GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE)
            , UART_TX_GPIO, UART_RX_GPIO);
    Sys_UART_Config(UART, UART_CLK_HZ, BAUD_RATE,  (UART_TX_START_INT_ENABLE |
            UART_RX_INT_ENABLE));

    /* Enable UART RX and TX interrupts */
    NVIC_EnableIRQ(UART0_RX_IRQn);
    NVIC_EnableIRQ(UART0_TX_IRQn);

    /* Enable UART */
    UART->CTRL = UART_ENABLE;

    /* Un-mask all interrupts */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

void UART_Config_DMA(void)
{
    /* Initialize UART to a known state */
    UART_Init();

    /* Clear DMA interrupt status bits */
    DMA[UART_RX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;
    DMA[UART_TX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;

    /* Enable UART */
    UART->CTRL = UART_ENABLE;

    /* Configure UART, and the RX and TX GPIOs */
    Sys_UART_GPIOConfig(UART, (GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE)
            , UART_TX_GPIO, UART_RX_GPIO);
    Sys_UART_Config(UART, UART_CLK_HZ, BAUD_RATE,  (UART_TX_DMA_ENABLE |
            UART_RX_DMA_ENABLE));

    /* Un-mask all interrupts */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);

    /* Configure DMA controller for UART RX*/
    DMA[UART_RX_DMA_NUM].DEST_ADDR = (uint32_t)&rx_buffer[0];
    DMA[UART_RX_DMA_NUM].SRC_ADDR = (uint32_t)&UART->RX_DATA;
    DMA[UART_RX_DMA_NUM].CFG0 = DMA_COMPLETE_INT_ENABLE | DMA_SRC_UART0 |
                                DMA_DEST_ADDR_INCR_1 | WORD_SIZE_8BITS_TO_8BITS;

    /* Reset buffer and counter, and configure transfer length before enabling */
    DMA[UART_RX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
    DMA[UART_RX_DMA_NUM].CFG1 = UART_BUFFER_SIZE  << DMA_CFG1_TRANSFER_LENGTH_Pos;
    DMA[UART_RX_DMA_NUM].CTRL = DMA_ENABLE_WRAP;

    /* Configure DMA controller for UART TX*/
    DMA[UART_TX_DMA_NUM].SRC_ADDR = (uint32_t)&tx_buffer[0];
    DMA[UART_TX_DMA_NUM].DEST_ADDR = (uint32_t)&UART->TX_DATA;
    DMA[UART_TX_DMA_NUM].CFG0 = DMA_DEST_UART0 | DMA_SRC_ADDR_INCR_1 |
                                WORD_SIZE_8BITS_TO_8BITS;

    NVIC_EnableIRQ(DMA0_IRQn);
}

void UART_StartTX(void)
{
#if UART_DMA_ENABLED == 0
    /* Kickoff UART transmission */
    UART->TX_DATA = tx_buffer[tx_count++];
#else
    /* Clear buffer and counter, and re-enable the TX DMA for next transmission*/
    DMA[UART_TX_DMA_NUM].CFG1 = UART_BUFFER_SIZE << DMA_CFG1_TRANSFER_LENGTH_Pos;
    DMA[UART_TX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
    DMA[UART_TX_DMA_NUM].CTRL = DMA_ENABLE_WRAP;
#endif
    /* Wait for UART transmission to be completed */
    while ((UART->STATUS & UART_TX_BUSY) == UART_TX_BUSY)
    {
        SYS_WATCHDOG_REFRESH();
    }

    tx_count = 0;
}

void UART_EchoRX(void)
{
    /* Wait until the whole string (21 bytes + null character) is received */
    if (rx_count == UART_BUFFER_SIZE)
    {
        /* Reset rx count to 0 */
        rx_count = 0;
#if UART_DMA_ENABLED == 1
        /* Clear buffer and counter, and re-enable the RX DMA */
        DMA[UART_RX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
        DMA[UART_RX_DMA_NUM].CFG1 = UART_BUFFER_SIZE  << DMA_CFG1_TRANSFER_LENGTH_Pos;
        DMA[UART_RX_DMA_NUM].CTRL = DMA_ENABLE_WRAP;
#endif
        /* Copy received string to the tx_buffer */
        memcpy((void *)&tx_buffer, (void *)&rx_buffer, UART_BUFFER_SIZE);

        /* Kick-off the next transmission */
        UART_StartTX();
    }
}

void UART0_RX_IRQHandler(void)
{
    /* Save the received data to the rx_buffer */
    rx_buffer[rx_count++]  = UART->RX_DATA;
}

void UART0_TX_IRQHandler(void)
{
    if (tx_count < UART_BUFFER_SIZE)
    {
        /* Write the next character to be sent to the UART TX_DATA register */
        UART->TX_DATA = tx_buffer[tx_count++];
    }
}

void DMA0_IRQHandler(void)
{
    if ((DMA[UART_RX_DMA_NUM].STATUS & DMA_COMPLETE_INT_TRUE) == DMA_COMPLETE_INT_TRUE)
    {
        /* Clear interrupt flag */
        DMA[UART_RX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;

        /* Update rx_count so that the received data is echoed back */
        rx_count = UART_BUFFER_SIZE;
    }
}
