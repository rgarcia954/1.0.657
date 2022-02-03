/**
 * @file uart_common.h
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

#ifndef UART_COMMON_H_
#define UART_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Define the size of the buffer we use to queue UART messages. (For
 * simplicity sake, this should be a power of 2)
 */
#define UART_TX_BUFFER_SIZE                (1U << 9)
#define UART_RX_BUFFER_SIZE                (1U << 7)

/** @brief Mask ensuring we bound the indices within the buffer limits. */
#define UART_TX_BUFFER_MASK                (UART_TX_BUFFER_SIZE - 1)
#define UART_RX_BUFFER_MASK                (UART_RX_BUFFER_SIZE - 1)

/** Define the baud rate the UART will run at */
#define BAUD_RATE                       115200U

/** @brief Selected baud rate */
extern volatile uint32_t selected_baud_rate;

/** @brief Transmit read pointer */
extern volatile uint32_t tx_r_ptr;

/** @brief Transmit write pointer */
extern volatile uint32_t tx_w_ptr;

/** @brief Flag indicating if a transmission is in progress */
extern volatile bool     tx_in_progress;

/** @brief Buffer for messages being transmitted */
extern char tx_buffer[UART_TX_BUFFER_SIZE];

/** @brief Receive read pointer */
extern volatile uint32_t rx_r_ptr;

/** @brief Receive transmit pointer */
extern volatile uint32_t rx_w_ptr;

/** @brief Buffer for messages being received */
extern char rx_buffer[UART_RX_BUFFER_SIZE];

/**
 * @brief Abstract function provided by the specific implementation to provide
 * implementation specific initialization.
 * @param enable_rx Flag indicating if character reception is supported.
 */
void swmTrace_UART_init(bool enable_rx);

/**
 * @brief Abstract function, provided by the specific implementation to send
 * the next character or set of characters.
 */
void swmTrace_send(void);

/**
 * @brief Calculates the next index based on value, this accounts for the
 * wrap around of the circular buffer.
 * @param value The value we want to increment.
 * @return Incremented value, accounting for wrap around at the buffer limit.
 */
uint32_t swmTrace_next(uint32_t value, uint32_t mask);

#endif /* UART_COMMON_H_ */
