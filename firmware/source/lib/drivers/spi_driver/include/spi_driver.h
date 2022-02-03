/**
 * @file spi_driver.h
 * @brief SPI CMSIS Driver header
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

#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <hw.h>
#include "Driver_SPI.h"
#include "RTE_Device.h"
#include <stdint.h>
#include <stdlib.h>
#include <Driver_DMA.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/



/* Constants */
#define SPI_MAX_PRESCALE          9U

/* Custom SPI Control Codes */

/* This control code allows the reconfiguration of the GPIO pin used for SSEL
 * line of the SPI master device.
 * Notes: This code should not be ORed with any other control codes. */
#define CUSTOM_SPI_CONTROL_SS     (0x1UL << 22)

/* Driver status flag definition */
#define SPI_INITIALIZED           ((uint8_t)(1U))
#define SPI_POWERED               ((uint8_t)(1U << 1))
#define SPI_CONFIGURED            ((uint8_t)(1U << 2))
#define SPI_DATA_LOST             ((uint8_t)(1U << 3))

/* DMA / CM3 code switches */

/* DMA is enabled for at least one SPI instance */
#define SPI_DMA_CODE_EN ((RTE_SPI0_ENABLED && RTE_SPI0_DMA_EN_DEFAULT) || (RTE_SPI1_ENABLED && RTE_SPI1_DMA_EN_DEFAULT))

/* DMA is disabled for at least one SPI instance */
#define SPI_CM33_CODE_EN ((RTE_SPI0_ENABLED && !(RTE_SPI0_DMA_EN_DEFAULT)) || (RTE_SPI1_ENABLED && !(RTE_SPI1_DMA_EN_DEFAULT)))

void SPI0_RX_IRQHandler(void);
void SPI0_TX_IRQHandler(void);
void SPI1_RX_IRQHandler(void);
void SPI1_TX_IRQHandler(void);

#if(RTE_SPI0_DMA_EN_DEFAULT)
void SPI0_TX_DMAHandler(uint32_t event); /* SPI0 TX DMA Channel */
void SPI0_RX_DMAHandler(uint32_t event); /* SPI0 RX DMA Channel */
#endif

#if(RTE_SPI1_DMA_EN_DEFAULT)
void SPI1_TX_DMAHandler(uint32_t event); /* SPI1 TX DMA Channel */
void SPI1_RX_DMAHandler(uint32_t event); /* SPI1 RX DMA Channel */
#endif

/* Extern DMA driver */
extern DRIVER_DMA_t Driver_DMA;

/* Default driver configuration */
#if(!RTE_SPI0_MODE_DEFAULT)
#define SPI0_DEFAULT_CONTROL_CONFIG (ARM_SPI_MODE_INACTIVE                   | \
                                     ARM_SPI_DATA_BITS(RTE_SPI0_WS_DEFAULT)  | \
                                     ARM_SPI_SS_MASTER_UNUSED                | \
                                     RTE_SPI0_POL_DEFAULT << ARM_SPI_FRAME_FORMAT_Pos)

#elif((RTE_SPI0_MODE_DEFAULT & 0xF0) == 0x10)
#define SPI0_DEFAULT_CONTROL_CONFIG (ARM_SPI_MODE_MASTER                                       | \
                                     ARM_SPI_DATA_BITS(RTE_SPI0_WS_DEFAULT)                    | \
                                   ((RTE_SPI0_MODE_DEFAULT & 3) << ARM_SPI_SS_MASTER_MODE_Pos) | \
                                     RTE_SPI0_POL_DEFAULT << ARM_SPI_FRAME_FORMAT_Pos)

#else
#define SPI0_DEFAULT_CONTROL_CONFIG (ARM_SPI_MODE_SLAVE                                       | \
                                     ARM_SPI_DATA_BITS(RTE_SPI0_WS_DEFAULT)                   | \
                                   ((RTE_SPI0_MODE_DEFAULT & 1) << ARM_SPI_SS_SLAVE_MODE_Pos) | \
                                     RTE_SPI0_POL_DEFAULT << ARM_SPI_FRAME_FORMAT_Pos)
#endif
#if(!RTE_SPI1_MODE_DEFAULT)
#define SPI1_DEFAULT_CONTROL_CONFIG (ARM_SPI_MODE_INACTIVE                   | \
                                     ARM_SPI_DATA_BITS(RTE_SPI1_WS_DEFAULT)  | \
                                     ARM_SPI_SS_MASTER_UNUSED                | \
                                     RTE_SPI1_POL_DEFAULT << ARM_SPI_FRAME_FORMAT_Pos)

#elif((RTE_SPI1_MODE_DEFAULT & 0xF0) == 0x10)
#define SPI1_DEFAULT_CONTROL_CONFIG (ARM_SPI_MODE_MASTER                                       | \
                                     ARM_SPI_DATA_BITS(RTE_SPI1_WS_DEFAULT)                    | \
                                   ((RTE_SPI1_MODE_DEFAULT & 3) << ARM_SPI_SS_MASTER_MODE_Pos) | \
                                     RTE_SPI1_POL_DEFAULT << ARM_SPI_FRAME_FORMAT_Pos)

#else
#define SPI1_DEFAULT_CONTROL_CONFIG (ARM_SPI_MODE_SLAVE                                       | \
                                     ARM_SPI_DATA_BITS(RTE_SPI1_WS_DEFAULT)                   | \
                                   ((RTE_SPI1_MODE_DEFAULT & 1) << ARM_SPI_SS_SLAVE_MODE_Pos) | \
                                     RTE_SPI1_POL_DEFAULT << ARM_SPI_FRAME_FORMAT_Pos)
#endif

ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities(void);

/* SPI Pins Configuration */
typedef struct _SPI_PINS
{
    uint8_t mosi;                   /* Master Out, Slave In */
    uint8_t miso;                   /* Master In, Slave Out */
    uint8_t ssel;                   /* Slave select */
    uint8_t sclk;                   /* Serial clock */
    uint32_t cfg;                   /* Drive strength, low-pass filter and pull-up config */
#if (SPI_DMA_CODE_EN)
    uint8_t rx_dma_ch;              /* SPI default RX DMA channel */
    uint8_t tx_dma_ch;              /* SPI default TX DMA channel */
    DMA_SignalEvent_t dma_rx_cb;
    DMA_SignalEvent_t dma_tx_cb;
#endif
} SPI_PINS;

/* SPI Status */
typedef struct _SPI_STATUS
{
    uint8_t busy;                   /* Transmitter/Receiver busy flag */
    uint8_t data_lost;              /* Data lost: Receive overflow/Transmit underflow
                                     * (cleared on start of transfer operation) */
    uint8_t mode_fault;             /* Not supported */
} SPI_STATUS;

/* SPI Information (Run-time) */
typedef struct _SPI_INFO
{
    ARM_SPI_SignalEvent_t cb_event; /* Event Callback */
    SPI_STATUS status;              /* Status flags */
    uint8_t state;                  /* Current SPI state */
    uint32_t mode;                  /* Current SPI mode */
} SPI_INFO;

/* SPI Transfer Information (Run-Time) */
typedef struct _SPI_TRANSFER_INFO
{
    uint32_t num;                   /* Total number of data to transfer */
    uint8_t *rx_buf;                /* Pointer to in data buffer */
    uint8_t *tx_buf;                /* Pointer to out data buffer */
    uint32_t rx_cnt;                /* Number of data received */
    uint32_t tx_cnt;                /* Number of data sent */
    uint32_t def_val;               /* Default transfer value */
} SPI_TRANSFER_INFO;

/* SPI Resources definition */
typedef struct
{
    SPI_Type           *reg;        /* SPI peripheral pointer */
    SPI_PINS           *pins;       /* SPI Input/Output pins */
#if (SPI_CM33_CODE_EN)
    IRQn_Type           rx_irqn;    /* SPI RX IRQ Number */
    IRQn_Type           tx_irqn;    /* SPI TX IRQ Number */
#endif
    SPI_INFO           *info;       /* Run-Time Information */
    SPI_TRANSFER_INFO  *xfer;       /* SPI transfer information */
#if (SPI_DMA_CODE_EN)
    IRQn_Type          rx_dma_irqn; /* DMA RX IRQ Number */
    IRQn_Type          tx_dma_irqn; /* DMA TX IRQ Number */
    DMA_TRG_t          dma_trg;     /* DMA target selection */
#endif
    uint8_t            dma_en;      /* DMA enabled selection */
} SPI_RESOURCES;

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* SPI_DRIVER_H_ */
