/**
 * @file uart_driver.c
 * @brief UART CMSIS Driver implementation
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
#include <string.h>
#include <uart_driver.h>
#if (USART_DMA_CODE_EN)
#include <dma_driver.h>
#endif    /* if (USART_DMA_CODE_EN) */

#define ARM_USART_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion =
{
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_USART_CAPABILITIES DriverCapabilities =
{
    .asynchronous       = 1,     /* Supports UART (Asynchronous) mode */
    .synchronous_master = 0,     /* Does not support Synchronous Master mode */
    .synchronous_slave  = 0,     /* Does not support Synchronous Slave mode */
    .single_wire        = 0,     /* Does not support UART Single-wire mode */
    .irda               = 0,     /* Does not support UART IrDA mode */
    .smart_card         = 0,     /* Does not support UART Smart Card mode */
    .smart_card_clock   = 0,     /* Smart Card Clock generator not available */
    .flow_control_rts   = 0,     /* RTS Flow Control not available */
    .flow_control_cts   = 0,     /* CTS Flow Control not available */
    .event_tx_complete  = 0,     /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    .event_rx_timeout   = 0,     /* Signal RX char timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    .rts                = 0,     /* RTS Line: not available */
    .cts                = 0,     /* CTS Line: not available */
    .dtr                = 0,     /* DTR Line: not available */
    .dsr                = 0,     /* DSR Line: not available */
    .dcd                = 0,     /* DCD Line: not available */
    .ri                 = 0,     /* RI Line : not available */
    .event_cts          = 0,     /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    .event_dsr          = 0,     /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    .event_dcd          = 0,     /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    .event_ri           = 0      /* Signal RI change event: \ref ARM_USART_EVENT_RI */
};

#if RTE_USART0_ENABLED

/* USART0 Run-Time Information */
static USART_INFO USART0_Info = { 0U };
static USART_TRANSFER_INFO USART0_TransferInfo = { 0U };
static USART_PINS USART0_Pins =
{
    .tx = RTE_USART0_TX_PIN,
    .rx = RTE_USART0_RX_PIN,
#if (USART_DMA_CODE_EN)
    .rx_dma_ch  = RTE_USART0_RX_DMA_CH_DEFAULT,    /* USART0 default RX DMA channel */
    .tx_dma_ch  = RTE_USART0_TX_DMA_CH_DEFAULT,    /* USART0 default TX DMA channel */
#endif    /* if (USART_DMA_CODE_EN) */
};

/* USART0 Resources */
static const USART_RESOURCES USART0_Resources =
{
    UART,
    &USART0_Pins,
#if (USART_CM33_CODE_EN)
    .uart_rx_irqn = UART0_RX_IRQn,
    .uart_tx_irqn = UART0_TX_IRQn,
#endif    /* if (USART_CM33_CODE_EN) */

    .info = &USART0_Info,
    .xfer = &USART0_TransferInfo,
};

#if (USART_DMA_CODE_EN)

/* Prepare DMA driver pointer */
static DRIVER_DMA_t *dma = &Driver_DMA;
#endif    /* if (USART_DMA_CODE_EN) */

#if (USART_CM33_CODE_EN)
/**
 * @brief       Called by hardware ISR when a byte is transmitted. Place
 *                  transmit data into the UART, sequentially, byte per byte.
 *                  Manage flags and counters. Application is notified via
 *                  callback function once the transmission is complete
 * @param[in]   usart USART instance
 */
static void USART_TX_IRQHandler(USART_RESOURCES *usart)
{
    if (usart->info->status.tx_busy)
    {
        /* Increment TX count */
        usart->xfer->tx_cnt++;

        /* Check if transfer is complete */
        if (usart->xfer->tx_cnt == usart->xfer->tx_num)
        {
            /* Clear send active flag */
            usart->info->status.tx_busy = 0U;

            /* Disable UART TX interrupt */
            NVIC_DisableIRQ(usart->uart_tx_irqn);

            /* Notify application */
            if (usart->info->cb_event)
            {
                usart->info->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
            }
        }
        else    /* Transfer next byte */
        {
            usart->reg->TX_DATA = usart->xfer->tx_buf[usart->xfer->tx_cnt];
        }
    }
}

/**
 * @brief       Called by hardware ISR when a byte is received. Store
 *                  received data in the buffer, manage flags and counters.
 *                  Application is notified via callback function once the
 *                  receive is complete
 * @param[in]   usart USART instance
 */
static void USART_RX_IRQHandler(USART_RESOURCES *usart)
{
    if (usart->info->status.rx_busy && (usart->xfer->rx_cnt < usart->xfer->rx_num))
    {
        /* Receive byte */
        usart->xfer->rx_buf[usart->xfer->rx_cnt++] = usart->reg->RX_DATA;

        /* Check if receive is complete */
        if (usart->xfer->rx_cnt == usart->xfer->rx_num)
        {
            /* Clear RX busy flag */
            usart->info->status.rx_busy = 0U;

            /* Disable UART TX interrupt */
            NVIC_DisableIRQ(usart->uart_rx_irqn);

            /* Notify application */
            if (usart->info->cb_event)
            {
                usart->info->cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
            }
        }
    }
}

#endif    /* if (USART_CM33_CODE_EN) */

#if (USART_DMA_CODE_EN)
/**
 * @brief       Called by DMA driver when a DMA transfer is done.
 *                  Manage flags and counters. Application is notified via
 *                  callback function once the transmission is complete
 * @param[in]   usart USART instance
 */
static void USART_DMAHandler(uint32_t event, USART_RESOURCES *usart)
{
	/* Prepare status description */
	uint32_t status = 0;

    /* Check if TX DMA channel triggered the event */
    if (event & (DMA_FLAG_BIT_SET << usart->pins->tx_dma_ch))
    {
        /* Get DMA status */
        DMA_STATUS_t dmaStatus = dma->GetStatus(usart->pins->tx_dma_ch);

        /* Check if DMA transmit transfer has finished */
        if (dmaStatus.completed && (dma->GetCounterValue(usart->pins->tx_dma_ch) == usart->xfer->tx_num))
        {
            /* Stop the DMA */
            dma->Stop(usart->pins->tx_dma_ch);

            /* Clear send active flag */
            usart->info->status.tx_busy = 0U;

            /* Check if full USART transfer has finished */
            if (!usart->info->status.tx_busy)
            {
                /* Notify application */
                if (usart->info->cb_event)
                {
                    usart->info->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
                }
            }
        }
    }

    /* Check if RX DMA channel triggered the event */
    if (event & (DMA_FLAG_BIT_SET << usart->pins->rx_dma_ch))
    {
        /* Get DMA status */
        DMA_STATUS_t dmaStatus = dma->GetStatus(usart->pins->rx_dma_ch);

        /* Check if DMA receive transfer has finished */
        if (dmaStatus.completed && (dma->GetCounterValue(usart->pins->rx_dma_ch) == usart->xfer->rx_num))
        {
            /* Stop the DMA */
            dma->Stop(usart->pins->rx_dma_ch);

            /* Clear RX busy flag */
            usart->info->status.rx_busy = 0U;

            /* Notify application */
            if (usart->info->cb_event)
            {
                usart->info->cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
            }
        }
    }

    /* If an event happened and the application registered a callback */
    if (event && usart->info->cb_event)
    {
        usart->info->cb_event(status);
    }
}

#endif    /* if (USART_DMA_CODE_EN) */

/**
 * @brief       Driver version
 * @return      ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION USARTx_GetVersion(void)
{
    return DriverVersion;
}

/**
 * @brief       Get USARTx driver capabilities
 * @return      ARM_USART_CAPABILITIES
 */
static ARM_USART_CAPABILITIES USARTx_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 * @brief       Initialize USART flags, GPIO pins and reference to
 *                  callback function.
 * @param[in]   cb_event Pointer to callback function (optional)
 * @param[in]   usart  Pointer to USART resources
 * @return      ARM_DRIVER_OK
 */
static int32_t USART_Initialize(ARM_USART_SignalEvent_t cb_event,
                                const USART_RESOURCES *usart)
{
    if (usart->info->flags & USART_INITIALIZED)
    {
        /* Driver is already initialized */
        return ARM_DRIVER_OK;
    }

    /* Initialize USART Run-time Resources */
    usart->info->cb_event = cb_event;

#if (USART_CM33_CODE_EN)

    /* Clear transfer information */
    memset(usart->xfer, 0, sizeof(USART_TRANSFER_INFO));
#endif    /* if (USART_CM33_CODE_EN) */

    /* Configure UART GPIO pins */
    Sys_UART_GPIOConfig(usart->reg, GPIO_LPF_DISABLE | GPIO_LEVEL4_DRIVE | GPIO_WEAK_PULL_UP,
                        usart->pins->tx, usart->pins->rx);

#if (USART_DMA_CODE_EN)
    DMA_CFG_t dmaCfgR, dmaCfgT;

    /* Prepare receiver mode DMA configuration */
    dmaCfgR.src_sel       = DMA_TRG_UART;
    dmaCfgR.src_step      = DMA_CFG0_SRC_ADDR_STATIC;
    dmaCfgR.dst_sel       = DMA_TRG_MEM;
    dmaCfgR.dst_step      = DMA_CFG0_DEST_ADDR_INCR_1;
    dmaCfgR.ch_priority   = DMA_CH_PRI_0;
    dmaCfgR.word_size     = DMA_CFG0_DEST_WORD_SIZE_8BITS_TO_8BITS;

    /* Prepare transmitter mode DMA configuration */
    dmaCfgT.src_sel       = DMA_TRG_MEM;
    dmaCfgT.src_step      = DMA_CFG0_SRC_ADDR_INCR_1;
    dmaCfgT.dst_sel       = DMA_TRG_UART;
    dmaCfgT.dst_step      = DMA_CFG0_DEST_ADDR_STATIC;
    dmaCfgT.ch_priority   = DMA_CH_PRI_0;
    dmaCfgT.word_size     = DMA_CFG0_DEST_WORD_SIZE_8BITS_TO_8BITS;

    /* Configure the RX DMA channel */
    dma->Configure(usart->pins->rx_dma_ch, &dmaCfgR, UART0_DMA_Handler);

    /* Store the RX DMA configuration */
    usart->info->dma_rx_cfg = dma->CreateConfigWord(&dmaCfgR);

    /* Configure the TX DMA channel */
    dma->Configure(usart->pins->tx_dma_ch, &dmaCfgT, UART0_DMA_Handler);

    /* Store the TX_DMA configuration */
    usart->info->dma_tx_cfg = dma->CreateConfigWord(&dmaCfgT);
#endif    /* if (USART_DMA_CODE_EN) */

    /* Indicate that initialization is done */
    usart->info->flags = USART_INITIALIZED;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Uninitialize USART flags, GPIO pins and removes reference to
 *                  callback function.
 * @param[in]   usart Pointer to USART resources
 * @return      ARM_DRIVER_OK
 */
static int32_t USART_Uninitialize(const USART_RESOURCES *usart)
{
#if (USART_DMA_CODE_EN)

    /* Stop the DMA data transfer */
    dma->Stop(usart->pins->rx_dma_ch);
    dma->Stop(usart->pins->tx_dma_ch);
#endif    /* if (USART_DMA_CODE_EN) */

    /* Reset TX pin configuration */
    GPIO->CFG[usart->pins->tx] = GPIO_MODE_DISABLE;

    /* Reset RX pin configuration */
    GPIO->CFG[usart->pins->rx] = GPIO_MODE_DISABLE;

    /* Reset GPIO_UART_SRC configuration */
    GPIO->SRC_UART[0] = UART_RX_SRC_CONST_HIGH;

    /* Reset USART status flags */
    usart->info->flags = 0U;

    /* Remove reference to callback function */
    usart->info->cb_event = NULL;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Operates the power modes of the USART interface
 * @param[in]   state ARM_POWER_FULL or ARM_POWER_OFF
 * @param[in]   usart  Pointer to USART resources
 * @return      ARM_DRIVER_OK if the operation is successful
 * @return      ARM_DRIVER_ERROR_UNSUPPORTED  if argument is invalid
 */
static int32_t USART_PowerControl(ARM_POWER_STATE state,
                                  const USART_RESOURCES *usart)
{
    switch (state)
    {
        case ARM_POWER_OFF:
        {
#if (USART_CM33_CODE_EN)

            /* Disable UART interrupts */
            NVIC_DisableIRQ(usart->uart_rx_irqn);
            NVIC_DisableIRQ(usart->uart_tx_irqn);

            /* Clear pending USART interrupts in NVIC */
            NVIC_ClearPendingIRQ(usart->uart_rx_irqn);
            NVIC_ClearPendingIRQ(usart->uart_tx_irqn);
#endif    /* if (USART_CM33_CODE_EN) */

            /* Disable UART */
            usart->reg->CTRL = UART_DISABLE;

            /* Clear flags */
            usart->info->status.tx_busy          = 0U;
            usart->info->status.rx_busy          = 0U;
            usart->info->flags &= ~USART_POWERED;
        }
        break;

        case ARM_POWER_LOW:
        {
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        }

        case ARM_POWER_FULL:
        {
            if ((usart->info->flags & USART_INITIALIZED) == 0U)
            {
                return ARM_DRIVER_ERROR;
            }
            if ((usart->info->flags & USART_POWERED) != 0U)
            {
                return ARM_DRIVER_OK;
            }

            /* Clear Status flags */
            usart->info->status.tx_busy = 0U;
            usart->info->status.rx_busy = 0U;

            /* Set powered flag */
            usart->info->flags = USART_POWERED | USART_INITIALIZED;
        }
        break;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/**
 * @brief       Control USART Interface
 * @param[in]   control Control operation
 * @param[in]   arg  Argument of operation (optional)
 * @param[in]   usart  Pointer to USART resources
 * @return      Execution status
 */
static int32_t USART_Control(uint32_t control, uint32_t arg,
                             const USART_RESOURCES *usart)
{
    /* Check if USART is powered up */
    if ((usart->info->flags & USART_POWERED) == 0U)
    {
        /* USART not powered */
        return ARM_DRIVER_ERROR;
    }

    /* Check which operation should be performed */
    switch (control & ARM_USART_CONTROL_Msk)
    {
        case ARM_USART_MODE_ASYNCHRONOUS:
        {
#if (USART_DMA_CODE_EN)
            Sys_UART_Config(usart->reg, SystemCoreClock /
                            (((CLK->DIV_CFG0 &
                            CLK_DIV_CFG0_UARTCLK_PRESCALE_Mask) >>
                            CLK_DIV_CFG0_UARTCLK_PRESCALE_Pos) + 1U),
                            arg, UART_TX_START_INT_DISABLE |
							UART_RX_INT_DISABLE | UART_TX_DMA_ENABLE |
							UART_RX_DMA_ENABLE);
#else   /* if (USART_DMA_CODE_EN) */
            Sys_UART_Config(usart->reg, SystemCoreClock /
                            (((CLK->DIV_CFG0 &
                            CLK_DIV_CFG0_UARTCLK_PRESCALE_Mask) >>
                            CLK_DIV_CFG0_UARTCLK_PRESCALE_Pos) + 1U),
                            arg, UART_TX_START_INT_ENABLE |
                            UART_RX_INT_ENABLE | UART_TX_DMA_DISABLE |
                            UART_RX_DMA_DISABLE);
#endif  /* if (USART_DMA_CODE_EN) */

            usart->reg->CTRL = UART_ENABLE;
            usart->info->baudrate = arg;
            break;
        }

        case ARM_USART_ABORT_SEND:
        {
#if (USART_DMA_CODE_EN)

            /* Stop DMA data transfer */
            dma->Stop(usart->pins->tx_dma_ch);
#else   /* if (USART_DMA_CODE_EN) */

            /* Disable USART TX interrupt */
            NVIC_DisableIRQ(usart->uart_tx_irqn);
#endif  /* if (USART_DMA_CODE_EN) */

            /* Clear Send active flag */
            usart->info->status.tx_busy = 0U;
            return ARM_DRIVER_OK;
        }

        case ARM_USART_ABORT_RECEIVE:
        {
#if (USART_DMA_CODE_EN)

            /* Stop DMA data transfer */
            dma->Stop(usart->pins->rx_dma_ch);
#else   /* if (USART_DMA_CODE_EN) */

            /* Disable USART RX interrupt */
            NVIC_DisableIRQ(usart->uart_rx_irqn);
#endif  /* if (USART_DMA_CODE_EN) */

            /* Clear RX busy flag */
            usart->info->status.rx_busy = 0U;
            return ARM_DRIVER_OK;
        }

        default:    /* Unsupported operation */
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* Check USART Data bits */
    if ((control & ARM_USART_DATA_BITS_Msk) != ARM_USART_DATA_BITS_8)
    {
        return ARM_USART_ERROR_DATA_BITS;
    }

    /* Check USART Parity */
    if ((control & ARM_USART_PARITY_Msk) != ARM_USART_PARITY_NONE)
    {
        return ARM_USART_ERROR_PARITY;
    }

    /* Check USART Stop bits */
    if ((control & ARM_USART_STOP_BITS_Msk) != ARM_USART_STOP_BITS_1)
    {
        return ARM_USART_ERROR_STOP_BITS;
    }

    /* Check Flow Control */
    if ((control & ARM_USART_FLOW_CONTROL_Msk) != ARM_USART_FLOW_CONTROL_NONE)
    {
        return ARM_USART_ERROR_FLOW_CONTROL;
    }

    /* Check Polarity */
    if ((control & ARM_USART_CPOL_Msk) != ARM_USART_CPOL0)
    {
        return ARM_USART_ERROR_CPOL;
    }

    /* Check Clock Phase */
    if ((control & ARM_USART_CPHA_Msk) != ARM_USART_CPHA0)
    {
        return ARM_USART_ERROR_CPHA;
    }

    /* Set configured flag */
    usart->info->flags |= USART_CONFIGURED;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Non-blocking send function. If a callback was previously
 *                  registered using the Initialize function, the caller is
 *                  notified when 'num' bytes are sent.
 * @param[in]   data Pointer to data buffer
 * @param[in]   num  Number of bytes to send
 * @param[in]   usart  USART instance
 * @return      ARM_DRIVER_OK if it can successfully start the
 *                                         send operation
 * @return      ARM_DRIVER_ERROR_BUSY  if it's already busy handling
 *                                         other send operation
 * @assumptions The data buffer argument points to a memory area with 'num'
 *                 bytes of size
 */
static int32_t USART_Send(const void *data, uint32_t num,
                          USART_RESOURCES *usart)
{
    if ((data == NULL) || (num == 0U))
    {
        /* Invalid parameters */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Check if driver is configured */
    if ((usart->info->flags & USART_CONFIGURED) == 0U)
    {
        /* USART is not configured (mode not selected) */
        return ARM_DRIVER_ERROR;
    }

    /* Check if TX is busy */
    if (usart->info->status.tx_busy)
    {
        /* Send is not completed yet */
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Set Send active flag */
    usart->info->status.tx_busy = 1U;

#if (USART_DMA_CODE_EN)

    /* Prepare DMA for transmission */
    dma->SetConfigWord(usart->pins->tx_dma_ch, usart->info->dma_tx_cfg);

    /* Prepare DMA TX buffer configuration */
	usart->xfer->tx_buf = (uint8_t *)data;
	usart->xfer->tx_num = num;
    DMA_ADDR_CFG_t buffCfg = {
        .src_addr     = usart->xfer->tx_buf,
        .dst_addr     = &usart->reg->TX_DATA,
        .counter_len  = 0,
        .transfer_len = usart->xfer->tx_num
    };

    /* Configure the DMA channel interrupt priority */
    dma->ConfigureAddr(usart->pins->tx_dma_ch, &buffCfg);

    /* Start the DMA transfer */
    dma->Start(usart->pins->tx_dma_ch);
#else    /* if (USART_DMA_CODE_EN) */

    /* Save transmit buffer info */
    usart->xfer->tx_buf = (uint8_t *)data;
    usart->xfer->tx_num = num;
    usart->xfer->tx_cnt = 0U;

    /* Enable UART TX interrupt */
    NVIC_ClearPendingIRQ(usart->uart_tx_irqn);
    NVIC_EnableIRQ(usart->uart_tx_irqn);

    /* Transmit first byte (remaining will be transmitted
     * in interrupt handler) */
    usart->reg->TX_DATA = usart->xfer->tx_buf[usart->xfer->tx_cnt];
#endif    /* if (USART_DMA_CODE_EN) */

    return ARM_DRIVER_OK;
}

/**
 * @brief       Non-blocking receive function. If a callback was previously
 *                  registered using the Initialize function, the caller is
 *                  notified when 'num' bytes are received.
 * @param[in]   data Pointer to data buffer
 * @param[in]   num  Number of bytes to receive
 * @param[in]   usart  USART instance
 * @return      ARM_DRIVER_OK if it can successfully start the
 *                                         receive operation
 * @return      ARM_DRIVER_ERROR_BUSY  if it's already busy handling
 *                                         other receive operation
 * @assumptions The data buffer argument points to a memory area with at
 *                 least 'num' bytes of available space
 */
static int32_t USART_Receive(void *data, uint32_t num, USART_RESOURCES *usart)
{
    if ((data == NULL) || (num == 0U))
    {
        /* Invalid parameters */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if ((usart->info->flags & USART_CONFIGURED) == 0U)
    {
        /* USART is not configured (mode not selected) */
        return ARM_DRIVER_ERROR;
    }

    if (usart->info->status.rx_busy)
    {
        /* Receiver is busy */
        return ARM_DRIVER_ERROR_BUSY;
    }

    usart->info->status.rx_busy = 1U;

#if (USART_DMA_CODE_EN)

    /* Prepare DMA for transmission */
    dma->SetConfigWord(usart->pins->rx_dma_ch, usart->info->dma_rx_cfg);
	
    /* Prepare DMA RX buffer configuration */
	usart->xfer->rx_buf = (uint8_t *)data;
	usart->xfer->rx_num = num;
    DMA_ADDR_CFG_t buffCfg = {
        .src_addr     = &usart->reg->RX_DATA,
        .dst_addr     = usart->xfer->rx_buf,
        .counter_len  = 0,
        .transfer_len = usart->xfer->rx_num
    };

    /* Configure the DMA channel interrupt priority */
    dma->ConfigureAddr(usart->pins->rx_dma_ch, &buffCfg);

    /* Start the DMA transfer */
    dma->Start(usart->pins->rx_dma_ch);
#else    /* if (USART_DMA_CODE_EN) */

    /* Save number of data to be received */
    usart->xfer->rx_num = num;

    /* Save receive buffer info */
    usart->xfer->rx_buf = (uint8_t *)data;
    usart->xfer->rx_cnt = 0U;

    /* Enable UART RX interrupt */
    NVIC_ClearPendingIRQ(usart->uart_rx_irqn);
    NVIC_EnableIRQ(usart->uart_rx_irqn);
#endif    /* if (USART_DMA_CODE_EN) */

    return ARM_DRIVER_OK;
}

/**
 * @brief       Get USARTX TX count. This count is reset for every send
 *                  operation.
 * @param[in]   usart USART instance
 * @return      Number of bytes transmitted by USARTX
 */
static uint32_t USART_GetTxCount(USART_RESOURCES *usart)
{
#if (USART_DMA_CODE_EN)

    /* Return counter value */
    return dma->GetCounterValue(usart->pins->tx_dma_ch);
#else    /* if (USART_DMA_CODE_EN) */

    /* Return TX count */
    return usart->xfer->tx_cnt;
#endif    /* if (USART_DMA_CODE_EN) */
}

/**
 * @brief       Get USARTX RX count. This count is reset for every receive
 *                  operation.
 * @param[in]   usart USART instance
 * @return      Number of bytes received by USARTX
 */
static uint32_t USART_GetRxCount(USART_RESOURCES *usart)
{
#if (USART_DMA_CODE_EN)

    /* Return counter value */
    return dma->GetCounterValue(usart->pins->rx_dma_ch);
#else    /* if (USART_DMA_CODE_EN) */

    /* Return RX count */
    return usart->xfer->rx_cnt;
#endif    /* if (USART_DMA_CODE_EN) */
}

/**
 * @brief       Get USARTx status
 * @param[in]   usart USART resources
 * @return      Return USARTx status as an ARM_USART_STATUS structure
 */
static ARM_USART_STATUS USART_GetStatus(const USART_RESOURCES *usart)
{
    ARM_USART_STATUS status;

    status.tx_busy          = usart->info->status.tx_busy;
    status.rx_busy          = usart->info->status.rx_busy;
    status.rx_overflow      = (usart->reg->STATUS & UART_OVERRUN_TRUE);
    status.tx_underflow     = 0U;
    status.rx_break         = 0U;
    status.rx_framing_error = 0U;
    status.rx_parity_error  = 0U;

    /* Return status */
    return status;
}

/**
 * @brief       Not supported
 * @return      Return status as 0
 */
static ARM_USART_MODEM_STATUS USART_GetModemStatus(USART_RESOURCES *usart)
{
    ARM_USART_MODEM_STATUS modem_status;

    modem_status.cts = 0U;
    modem_status.dsr = 0U;
    modem_status.ri  = 0U;
    modem_status.dcd = 0U;

    return modem_status;
}

/* USART0 Driver Wrapper functions. See functions above for usage */
static ARM_USART_CAPABILITIES USART0_GetCapabilities(void)
{
    return USARTx_GetCapabilities();
}

static int32_t USART0_Initialize(ARM_USART_SignalEvent_t cb_event)
{
    /* Initialize USART */
    int32_t status = USART_Initialize(cb_event, &USART0_Resources);

#if (RTE_USART_CFG_EN_DEFAULT)

    /* Set default power mode */
    status |= USART_PowerControl(ARM_POWER_FULL, &USART0_Resources);

    /* Set default UART mode, baudrate */
    status |= USART_Control(ARM_USART_MODE_ASYNCHRONOUS, RTE_USART0_BAUDRATE_DEFAULT,
                            &USART0_Resources);
#endif    /* if (RTE_USART_CFG_EN_DEFAULT) */

    /* Return initialization status */
    return status;
}

static int32_t USART0_Uninitialize(void)
{
    return USART_Uninitialize(&USART0_Resources);
}

static int32_t USART0_PowerControl(ARM_POWER_STATE state)
{
    return USART_PowerControl(state, &USART0_Resources);
}

static int32_t USART0_Send(const void *data, uint32_t num)
{
    return USART_Send(data, num, &USART0_Resources);
}

static int32_t USART0_Receive(void *data, uint32_t num)
{
    return USART_Receive(data, num, &USART0_Resources);
}

/**
 * @brief       Not supported
 * @return      ARM_DRIVER_ERROR_UNSUPPORTED
 */
static int32_t USART0_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static uint32_t USART0_GetTxCount(void)
{
    return USART_GetTxCount(&USART0_Resources);
}

static uint32_t USART0_GetRxCount(void)
{
    return USART_GetRxCount(&USART0_Resources);
}

static int32_t USART0_Control(uint32_t control, uint32_t arg)
{
    return USART_Control(control, arg, &USART0_Resources);
}

static ARM_USART_STATUS USART0_GetStatus(void)
{
    return USART_GetStatus(&USART0_Resources);
}

/**
 * @brief       Not supported
 * @return      ARM_DRIVER_ERROR_UNSUPPORTED
 */
static int32_t USART0_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS USART0_GetModemStatus(void)
{
    return USART_GetModemStatus(&USART0_Resources);
}

#if (USART_CM33_CODE_EN)
void UART0_TX_IRQHandler(void)
{
    USART_TX_IRQHandler(&USART0_Resources);
}

void UART0_RX_IRQHandler(void)
{
    USART_RX_IRQHandler(&USART0_Resources);
}

#else    /* if (USART_CM3_CODE_EN) */
void UART0_DMA_Handler(uint32_t event)
{
    USART_DMAHandler(event, &USART0_Resources);
}

#endif    /* if (USART_CM3_CODE_EN) */

/* USART0 Driver Control Block */
ARM_DRIVER_USART Driver_USART0 =
{
    USARTx_GetVersion,
    USART0_GetCapabilities,
    USART0_Initialize,
    USART0_Uninitialize,
    USART0_PowerControl,
    USART0_Send,
    USART0_Receive,
    USART0_Transfer,
    USART0_GetTxCount,
    USART0_GetRxCount,
    USART0_Control,
    USART0_GetStatus,
    USART0_SetModemControl,
    USART0_GetModemStatus
};

#endif    /* if RTE_USART0_ENABLED */
