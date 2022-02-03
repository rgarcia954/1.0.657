/**
 * @file uart_driver.h
 * @brief UART CMSIS Driver header
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

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

/* DMA / CM3 code switches */
#define USART_DMA_CODE_EN (RTE_USART0_DMA_EN_DEFAULT)
#define USART_CM33_CODE_EN !(RTE_USART0_DMA_EN_DEFAULT)

#include <hw.h>
#include "Driver_USART.h"
#include "RTE_Device.h"
#if (USART_DMA_CODE_EN)
#include <Driver_DMA.h>
#endif    /* if (USART_DMA_CODE_EN) */

#if !RTE_USART0_ENABLED
    #warning "Please configure USART0 in RTE_Device.h"
#endif    /* if !RTE_USART0_ENABLED */

/* Driver status flag definition */
#define USART_INITIALIZED               ((uint8_t)(1U))
#define USART_POWERED                   ((uint8_t)(1U << 1))
#define USART_CONFIGURED                ((uint8_t)(1U << 2))
#define USART_FLAG_BIT_SET              0x1U

#if (USART_DMA_CODE_EN)
/* Extern DMA driver */
extern DRIVER_DMA_t Driver_DMA;

/* Extern DMA buffer and DMA channel configurations */
extern DMA_ADDR_CFG_t buffRCfg;
extern DMA_ADDR_CFG_t buffTCfg;
extern DMA_CFG_t dmaCfgR, dmaCfgT;
#endif    /* if (USART_DMA_CODE_EN) */

/* USART Input/Output Configuration */
typedef const struct _USART_IO
{
    uint8_t tx;          /* TX  Pin identifier */
    uint8_t rx;          /* RX  Pin identifier */
#if (USART_DMA_CODE_EN)
    uint8_t rx_dma_ch;    /* UART default RX DMA channel */
    uint8_t tx_dma_ch;    /* UART default TX DMA channel */
#endif    /* if (USART_DMA_CODE_EN) */
} USART_PINS;

/* USART Transfer Information (Run-Time) */
typedef struct _USART_TRANSFER_INFO
{
    uint32_t rx_num;     /* Total number of receive data */
    uint32_t tx_num;     /* Total number of transmit data */
    uint8_t  *rx_buf;    /* Pointer to in data buffer */
    uint8_t  *tx_buf;    /* Pointer to out data buffer */
    uint32_t rx_cnt;     /* Number of data received */
    uint32_t tx_cnt;     /* Number of data sent */
} USART_TRANSFER_INFO;

/* USART Status */
typedef struct _USART_STATUS
{
    uint8_t tx_busy;      /* Transmitter busy flag */
    uint8_t rx_busy;      /* Receiver busy flag */
} USART_STATUS;

/* USART Information (Run-time) */
typedef struct _USART_INFO
{
    ARM_USART_SignalEvent_t cb_event;  /* Event Callback */
    USART_STATUS status;               /* Status flags */
    uint8_t flags;                     /* Current USART flags */
    uint32_t mode;                     /* Current USART mode */
    uint32_t baudrate;                 /* Baudrate */
#if (USART_DMA_CODE_EN)
    uint32_t dma_rx_cfg;               /* DMA channel receiver configuration */
    uint32_t dma_tx_cfg;               /* DMA channel transmitter configuration */
#endif    /* if (USART_DMA_CODE_EN) */
} USART_INFO;

/* USART Resources definition */
typedef const struct
{
    UART_Type              *reg;         /* UART peripheral pointer */
    USART_PINS             *pins;        /* USART Input/Output pins */
#if (USART_CM33_CODE_EN)
    IRQn_Type uart_rx_irqn;              /* USART RX IRQ Number */
    IRQn_Type uart_tx_irqn;              /* USART TX IRQ Number */
#endif    /* if (USART_CM33_CODE_EN) */
    USART_INFO             *info;        /* Run-Time Information */
    USART_TRANSFER_INFO    *xfer;        /* USART transfer information */
} USART_RESOURCES;

/* Interrupt Handler */
#if (USART_DMA_CODE_EN)
void UART0_DMA_Handler(uint32_t event);

#else    /* if (USART_DMA_CODE_EN) */
void UART0_TX_IRQHandler(void);
void UART0_RX_IRQHandler(void);
#endif    /* if (USART_DMA_CODE_EN) */

#endif    /* UART_DRIVER_H */
