/**
 * @file uart_common.c
 * @brief This provides common code shared between the different UART
 * implementations.
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

#include <string.h>
#include <stdio.h>

#include "hw.h"

#include "swmTrace_options.h"
#include "swmTrace_int.h"
#include "uart_common.h"

#if SWM_UART_SOURCE == 0
void UART0_RX_IRQHandler(void);
#elif SWM_UART_SOURCE == 1
void UART1_RX_IRQHandler(void);
#endif

/** @brief Selected baud rate */
volatile uint32_t selected_baud_rate = BAUD_RATE;

/** @brief Transmit read pointer */
volatile uint32_t tx_r_ptr;

/** @brief Transmit write pointer */
volatile uint32_t tx_w_ptr;

/** @brief Flag indicating if a transmission is in progress */
volatile bool     tx_in_progress;

/** @brief Buffer for messages being transmitted */
char tx_buffer[UART_TX_BUFFER_SIZE];

/** @brief Receive read pointer */
volatile uint32_t rx_r_ptr;

/** @brief Receive transmit pointer */
volatile uint32_t rx_w_ptr;

/** @brief Buffer for messages being received */
char rx_buffer[UART_RX_BUFFER_SIZE];

static void UART_common_init(int txpin, int rxpin)
{
    /* Reset and Disable UART */
    UART[SWM_UART_SOURCE].CTRL = UART_DISABLE | UART_RESET;

    /* Reset all UART GPIOs to a known state */
    GPIO->SRC_UART[SWM_UART_SOURCE] = UART_RX_SRC_CONST_HIGH;

    /* configure TX */
    SYS_GPIO_CONFIG(txpin, (GPIO_MODE_DISABLE | GPIO_NO_PULL));
    tx_r_ptr = 0;
    tx_w_ptr = 0;
    tx_in_progress = false;
    memset(tx_buffer, 0, UART_TX_BUFFER_SIZE);

    /* configure RX */
    SYS_GPIO_CONFIG(rxpin, (GPIO_MODE_DISABLE | GPIO_NO_PULL));
    rx_r_ptr = 0;
    rx_w_ptr = 0;
    memset(rx_buffer, 0, UART_RX_BUFFER_SIZE);

    /* Configure UART, and the RX and TX GPIOs */
    uint32_t config = GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE;
    Sys_UART_GPIOConfig(&UART[SWM_UART_SOURCE], config, txpin, rxpin);
}

uint32_t swmTrace_next(uint32_t value, uint32_t mask)
{
    return (value + 1) & mask;
}

void swmTrace_init(const uint32_t *configuration, uint32_t size)
{
    swmTrace_internal_init(configuration, size);

    int rxpin = -1;
    int txpin = -1;

    bool use_rx = false;

    // deal with the options, these should set up the rx and tx pins
    while (size-- > 0)
    {
        if ((*configuration & SWM_UART_OPTIONS_MASK) == SWM_UART_RX_PIN)
        {
            rxpin = *configuration & 0xFF;
        }
        if ((*configuration & SWM_UART_OPTIONS_MASK) == SWM_UART_TX_PIN)
        {
            txpin = *configuration & 0xFF;
        }
        if ((*configuration & SWM_UART_OPTIONS_MASK) == SWM_UART_BAUD_RATE)
        {
            selected_baud_rate = *configuration & 0xFFFFFF;
        }
        if (*configuration == SWM_UART_RX_ENABLE)
        {
            use_rx = true;
        }
        configuration++;
    }

    UART_common_init(txpin, rxpin);
    swmTrace_UART_init(use_rx);

    UART[SWM_UART_SOURCE].CTRL = UART_ENABLE;
}

void swmTrace_vprintf(const char *sFormat, va_list *pParamList)
{
    static char buffer[UART_TX_BUFFER_SIZE];
    vsnprintf(buffer, UART_TX_BUFFER_SIZE - 1, sFormat, *pParamList);

    char *ptr = buffer;

    while (*ptr != '\0')
    {
        uint32_t index = swmTrace_next(tx_w_ptr, UART_TX_BUFFER_MASK);
        if (index != tx_r_ptr)
        {
            tx_buffer[tx_w_ptr] = *ptr;
            tx_w_ptr = index;
        }
        ptr++;
    }

    if (! tx_in_progress)
    {
        tx_in_progress = true;
        swmTrace_send();
    }
}

bool swmTrace_getch(char *ch)
{
    if (rx_r_ptr != rx_w_ptr)
    {
        *ch = rx_buffer[rx_r_ptr];
        rx_r_ptr = swmTrace_next(rx_r_ptr, UART_RX_BUFFER_MASK);
        return true;
    }

    // otherwise no data available
    return false;
}

/**
 * @brief Provides indication if transmission is in progress.
 * @return True if a string is being transmitted; false otherwise
 */
bool swmTrace_txInProgress(void)
{
    return tx_in_progress;
}

#if SWM_UART_SOURCE == 0
/**
 * @brief Interrupt Service Routine for the UART0 RX. This is invoked on
 * completion of each character received.
 */
void UART0_RX_IRQHandler(void)
{
    int index = swmTrace_next(rx_w_ptr, UART_RX_BUFFER_MASK);
    if (index != rx_r_ptr)
    {
        rx_buffer[rx_w_ptr] = UART[0].RX_DATA;
        rx_w_ptr = index;
    }
}

#elif SWM_UART_SOURCE == 1
/**
 * @brief Interrupt Service Routine for the UART1 RX. This is invoked on
 * completion of each character received.
 */
void UART1_RX_IRQHandler(void)
{
    int index = swmTrace_next(rx_w_ptr, UART_RX_BUFFER_MASK);
    if (index != rx_r_ptr)
    {
        rx_buffer[rx_w_ptr] = UART[1].RX_DATA;
        rx_w_ptr = index;
    }
}
#endif /* SWM_UART_SOURCE */
