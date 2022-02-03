/**
 * @file uart_hci.h
 * @brief Support firmware for the UART-HCI interface
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

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <compiler.h>
#include "hw.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "arch.h"

/* #include "reg_assert_mgr.h" */
#include "rwip.h"

/* Length of RX and TX buffers */
#define BUFFER_LENGTH 250

#define RX_DMA_NUM                      0
#define TX_DMA_NUM                      1

__INLINE uint8_t uart_tx_dma_done_getf(void)
{
    return ((DMA[TX_DMA_NUM].STATUS & DMA_COMPLETE_INT_TRUE) == DMA_COMPLETE_INT_TRUE);
}

__INLINE uint8_t uart_rx_dma_done_getf(void)
{
    return ((DMA[RX_DMA_NUM].STATUS & DMA_COMPLETE_INT_TRUE) == DMA_COMPLETE_INT_TRUE);
}

__INLINE void uart_tx_dma_done_clr_setf(uint8_t txdmadoneclr)
{
    DMA[TX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;
    DMA[TX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
}

__INLINE void uart_rx_dma_done_clr_setf(uint8_t rxdmadoneclr)
{
    DMA[RX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;
    DMA[RX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
}

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes the UART to default values.
 *****************************************************************************************
 */
void uart_init(uint32_t baud_rate, uint8_t txd_gpio, uint8_t rxd_gpio);

/**
 ****************************************************************************************
 * @brief Enable UART flow.
 *****************************************************************************************
 */
void uart_flow_on(void);

/**
 ****************************************************************************************
 * @brief Disable UART flow.
 *****************************************************************************************
 */
bool uart_flow_off(void);

/**
 ****************************************************************************************
 * @brief Starts a data reception.
 *
 * @param[out] bufptr   Pointer to the RX buffer
 * @param[in]  size     Size of the expected reception
 * @param[in]  callback Pointer to the function called back when transfer finished
 * @param[in]  dummy    Dummy data pointer returned to callback when reception is finished
 *****************************************************************************************
 */
void uart_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy);

/**
 ****************************************************************************************
 * @brief Starts a data transmission.
 *
 * @param[in] bufptr   Pointer to the TX buffer
 * @param[in] size     Size of the transmission
 * @param[in] callback Pointer to the function called back when transfer finished
 * @param[in] dummy    Dummy data pointer returned to callback when transmission is finished
 *****************************************************************************************
 */
void uart_write(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy);

/**
 ****************************************************************************************
 * @brief Serves the data transfer interrupt requests.
 *
 * It clears the requests and executes the appropriate callback function.
 *****************************************************************************************
 */
void uart_isr(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */
