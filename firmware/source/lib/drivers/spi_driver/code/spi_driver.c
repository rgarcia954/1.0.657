/**
 * @file spi_driver.c
 * @brief SPI CMSIS Driver implementation
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
#include <spi_driver.h>
#include <dma_driver.h>
#include <string.h>

#if RTE_SPI0_ENABLED

/* SPI0 Run-Time Information */
static SPI_INFO SPI0_Info         = { 0U };
static SPI_TRANSFER_INFO SPI0_TransferInfo = { 0U };
static SPI_PINS SPI0_Pins         = {
    .mosi  = RTE_SPI0_MOSI_PIN,
    .miso  = RTE_SPI0_MISO_PIN,
    .ssel  = RTE_SPI0_SSEL_PIN,
    .sclk  = RTE_SPI0_SCLK_PIN,
    .cfg   = (RTE_SPI0_GPIO_DRIVE | RTE_SPI0_GPIO_LPF | RTE_SPI0_GPIO_PULL),
#if (RTE_SPI0_DMA_EN_DEFAULT)
    .rx_dma_ch           = RTE_SPI0_RX_DMA_CH_DEFAULT,         /* SPI0 default RX DMA channel */
    .tx_dma_ch           = RTE_SPI0_TX_DMA_CH_DEFAULT,         /* SPI0 default TX DMA channel */
    .dma_rx_cb           = SPI0_RX_DMAHandler,                 /* SPI0 default RX callback function */
    .dma_tx_cb           = SPI0_TX_DMAHandler                  /* SPI0 default TX callback function */
#endif    /* if (RTE_SPI0_DMA_EN_DEFAULT) */
};

SPI_RESOURCES SPI0_Resources =
{
        .reg     = &SPI[0],
        .pins    = &SPI0_Pins,
#if (SPI_CM33_CODE_EN)
        .rx_irqn = SPI0_RX_IRQn,
        .tx_irqn = SPI0_TX_IRQn,
#endif    /* if (SPI_CM33_CODE_EN) */
        .info    = &SPI0_Info,
        .xfer    = &SPI0_TransferInfo,
#if (RTE_SPI0_DMA_EN_DEFAULT)
		.rx_dma_irqn = RTE_SPI0_RX_DMA_CH_DEFAULT,
		.tx_dma_irqn = RTE_SPI0_TX_DMA_CH_DEFAULT,
		.dma_trg     = DMA_TRG_SPI0,                   /* SPI0 default DMA target selection */
#endif    /* if (RTE_SPI0_DMA_EN_DEFAULT) */
		.dma_en  = RTE_SPI0_DMA_EN_DEFAULT
};

#endif    /* RTE_SPI0_ENABLED */

#if RTE_SPI1_ENABLED
/* SPI1 Run-Time Information */
static SPI_INFO SPI1_Info         = { 0U };
static SPI_TRANSFER_INFO SPI1_TransferInfo = { 0U };
static SPI_PINS SPI1_Pins         = {
    .mosi  = RTE_SPI1_MOSI_PIN,
    .miso  = RTE_SPI1_MISO_PIN,
    .ssel  = RTE_SPI1_SSEL_PIN,
    .sclk  = RTE_SPI1_SCLK_PIN,
    .cfg   = (RTE_SPI1_GPIO_DRIVE | RTE_SPI1_GPIO_LPF | RTE_SPI1_GPIO_PULL),
#if (RTE_SPI1_DMA_EN_DEFAULT)
    .rx_dma_ch           = RTE_SPI1_RX_DMA_CH_DEFAULT,         /* SPI1 default RX DMA channel */
    .tx_dma_ch           = RTE_SPI1_TX_DMA_CH_DEFAULT,         /* SPI1 default TX DMA channel */
    .dma_rx_cb           = SPI1_RX_DMAHandler,                 /* SPI1 default RX callback function */
    .dma_tx_cb           = SPI1_TX_DMAHandler                  /* SPI1 default TX callback function */
#endif    /* if (RTE_SPI1_DMA_EN_DEFAULT) */
};

const SPI_RESOURCES SPI1_Resources =
{
        .reg     = &SPI[1],
        .pins    = &SPI1_Pins,
#if (SPI_CM33_CODE_EN)
        .rx_irqn = SPI1_RX_IRQn,
        .tx_irqn = SPI1_TX_IRQn,
#endif    /* if (SPI_CM33_CODE_EN) */
        .info    = &SPI1_Info,
        .xfer    = &SPI1_TransferInfo,
#if (RTE_SPI1_DMA_EN_DEFAULT)
		.rx_dma_irqn = RTE_SPI1_RX_DMA_CH_DEFAULT,
		.tx_dma_irqn = RTE_SPI1_RX_DMA_CH_DEFAULT,
		.dma_trg     = DMA_TRG_SPI1,                   /* SPI1 default DMA target selection */
#endif    /* if (RTE_SPI1_DMA_EN_DEFAULT) */
		.dma_en  = RTE_SPI1_DMA_EN_DEFAULT
};

#endif    /* RTE_SPI1_ENABLED */

#if (SPI_DMA_CODE_EN)

/* Prepare DMA driver pointer */
static DRIVER_DMA_t *dma = &Driver_DMA;
#endif    /* if (SPI_DMA_CODE_EN) */

#define ARM_SPI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* Driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_SPI_API_VERSION,
    ARM_SPI_DRV_VERSION
};

#if (SPI_CM33_CODE_EN)

/**
 * @brief       Prepare data to be transmitted via SPI interface.
 * @param[in]   spi  Pointer to SPI resources
 */
static uint32_t SPI_PrepareNextTXData(const SPI_RESOURCES *spi)
{
    uint32_t data = spi->xfer->def_val;
    uint8_t i, ws;

    if (spi->xfer->tx_buf)
    {
        /* Get word size configuration (in bytes) */
        ws = ((spi->reg->CFG & SPI_CFG_WORD_SIZE_Mask) >> (SPI_CFG_WORD_SIZE_Pos+3)) + 1;

        /* Convert byte array into 32bit integer */
        for (i = 0, data = 0; i < ws; i++)
        {
            data |= spi->xfer->tx_buf[i] << (i * 8);
        }

        spi->xfer->tx_buf += ws;
    }

    return data;
}
#endif    /* if (SPI_CM33_CODE_EN) */

/**
 * @brief       When SPI transfer is complete, deactivate transfer via SSEL line.
 * 					Set module in idle mode.
 * @param[in]   spi  Pointer to SPI resources
 */
static void SPI_TransferCompleted(const SPI_RESOURCES *spi)
{
    /* Clear busy flag */
    spi->info->status.busy = 0U;

    /* Deactivate SSEL line, if in master mode/SS_HW_OUT */
    if ((spi->info->mode & ARM_SPI_MODE_MASTER) &&
        (spi->info->mode & ARM_SPI_SS_MASTER_HW_OUTPUT))
    {
        Sys_SPI_TransferConfig(spi->reg, SPI_CS_1);
    }

    spi->reg->CFG &= ~(SPI_TX_START_INT_ENABLE  |
                       SPI_RX_INT_ENABLE        |
                       SPI_OVERRUN_INT_ENABLE   |
                       SPI_UNDERRUN_INT_ENABLE);

    /* Stop transfer and set RW operation to Idle */
    Sys_SPI_TransferConfig(spi->reg, SPI_MODE_NOP | SPI_DISABLE);
}

/**
 * @brief       Calculate prescaler of SPI interface
 * @param[in]   bus_speed Desired bus speed
 * @param[in]   system_clock System clock frequency
 * @return      Closest prescale value for SPI clock sourced from SysClk
 */
static uint8_t SPI_CalculatePrescale(uint32_t bus_speed, uint32_t system_clock)
{
    uint8_t spi_prescale;

    /* SPI Clock is sourced from SysClk. Find closest prescale value */
    for (spi_prescale = 0U; spi_prescale < SPI_MAX_PRESCALE; spi_prescale++)
    {
        if (bus_speed >= (system_clock >> (spi_prescale + 1U)))
        {
            break;
        }
    }
    return spi_prescale;
}

/**
 * @brief       Disables the SPI interface and powers it down.
 *  				Terminates pending data transfers and
 * 					disables peripheral interrupts
 * @param[in]   spi Pointer to SPI resources
 */
static inline void SPI_PowerControl_Off(const SPI_RESOURCES *spi)
{

#if (SPI_CM33_CODE_EN)
	if (!(spi->dma_en))
	{
		/* Disable SPI interrupts */
		NVIC_DisableIRQ(spi->rx_irqn);
		NVIC_DisableIRQ(spi->tx_irqn);

		/* Clear pending SPI interrupts in NVIC */
		NVIC_ClearPendingIRQ(spi->rx_irqn);
		NVIC_ClearPendingIRQ(spi->tx_irqn);
	}
#endif    /* if (SPI_CM33_CODE_EN) */

    /* Disable SPI */
    spi->reg->CTRL = SPI_DISABLE;
    while((spi->reg->CTRL & SPI_STATUS_ENABLED) == SPI_STATUS_ENABLED)
    {
        SYS_WATCHDOG_REFRESH();
    }

    /* Clear status flags */
    spi->info->status.busy       = 0U;
    spi->info->status.data_lost  = 0U;
    spi->info->status.mode_fault = 0U;

    spi->info->state &= ~SPI_POWERED;

}

/**
 * @brief       Enables the SPI interface and powers it on
 * @param[in]   spi  Pointer to SPI resources
 */
static inline void SPI_PowerControl_Full(const SPI_RESOURCES *spi)
{
    /* Clear Status flags */
    spi->info->status.busy       = 0U;
    spi->info->status.data_lost  = 0U;
    spi->info->status.mode_fault = 0U;
    spi->xfer->def_val           = 0U;

#if (SPI_CM33_CODE_EN)
    if (!(spi->dma_en))
    {
    	/* Clear and enable TX/RX interrupts */
		NVIC_ClearPendingIRQ(spi->tx_irqn);
		NVIC_ClearPendingIRQ(spi->rx_irqn);
		NVIC_EnableIRQ(spi->tx_irqn);
		NVIC_EnableIRQ(spi->rx_irqn);
    }
#endif    /* if (SPI_CM33_CODE_EN) */

#if (SPI_DMA_CODE_EN)
    if (spi->dma_en)
    {
		/* Clear and enable DMA channels TX/RX interrupts */
		NVIC_ClearPendingIRQ(spi->tx_dma_irqn);
		NVIC_ClearPendingIRQ(spi->rx_dma_irqn);
		NVIC_EnableIRQ(spi->tx_dma_irqn);
		NVIC_EnableIRQ(spi->rx_dma_irqn);
    }
#endif    /* if (SPI_DMA_CODE_EN) */

    /* Set powered flag */
    spi->info->state |= SPI_POWERED;

}

/**
 * @brief       Operates the power modes of the SPI interface
 * @param[in]   state ARM_POWER_FULL or ARM_POWER_OFF
 * @param[in]   spi  Pointer to SPI resources
 * @return      ARM_DRIVER_OK if the operation is successful
 * @return      ARM_DRIVER_ERROR_UNSUPPORTED  if argument is invalid
 */
static int32_t SPI_PowerControl(ARM_POWER_STATE state,
                                const SPI_RESOURCES *spi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
        {
            SPI_PowerControl_Off(spi);
        }
        break;

        case ARM_POWER_FULL:
        {
            if ((spi->info->state & SPI_INITIALIZED) == 0U)
            {
                return ARM_DRIVER_ERROR;
            }
            if ((spi->info->state & SPI_POWERED) != 0U)
            {
                return ARM_DRIVER_OK;
            }
            SPI_PowerControl_Full(spi);
        }
        break;

        case ARM_POWER_LOW:
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/**
 * @brief       Initialize SPI flags, GPIO pins and reference to
 *                 callback function.
 * @param[in]   cb_event pointer to callback function (optional)
 * @param[in]   spi  Pointer to SPI resources
 * @return      ARM_DRIVER_OK
 */
static int32_t SPI_Initialize(ARM_SPI_SignalEvent_t cb_event,
                              const SPI_RESOURCES *spi)
{
    if (spi->info->state & SPI_INITIALIZED)
    {
        /* Driver is already initialized */
        return ARM_DRIVER_OK;
    }

    /* Initialize SPI Run-Time Resources */
    spi->info->cb_event          = cb_event;

    /* Clear transfer information */
    memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));

    spi->info->state = SPI_INITIALIZED;

    SPI_PowerControl_Full(spi);

    return ARM_DRIVER_OK;
}

/**
 * @brief       Uninitialize SPI flags, GPIO pins and removes reference to
 *                 callback function.
 * @param[in]   spi Pointer to SPI resources
 * @return      ARM_DRIVER_OK
 */
static int32_t SPI_Uninitialize(const SPI_RESOURCES *spi)
{

    SPI_PowerControl(ARM_POWER_OFF, spi);

    /* Reset pin configuration */
    GPIO->CFG[spi->pins->miso] = GPIO_MODE_DISABLE;
    GPIO->CFG[spi->pins->mosi] = GPIO_MODE_DISABLE;
    GPIO->CFG[spi->pins->ssel] = GPIO_MODE_DISABLE;
    GPIO->CFG[spi->pins->sclk] = GPIO_MODE_DISABLE;

    /* Reset GPIO_SPI_SRC configuration */
    GPIO->SRC_SPI[(spi->reg - (SPI_Type*)&SPI[0])] = (SPI_CS_SRC_CONST_HIGH   |
                                        SPI_CLK_SRC_CONST_HIGH);

    /* Clear SPI state */
    spi->info->state = 0U;

    /* Remove reference to callback function */
    spi->info->cb_event = NULL;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Control SPI Interface
 * @param[in]   control control operation
 * @param[in]   arg  Argument of operation (optional)
 * @param[in]   spi  Pointer to SPI resources
 * @return      Execution status
 */
static int32_t SPI_Control(uint32_t control, uint32_t arg,
                           const SPI_RESOURCES *spi)
{
    uint32_t mode, reg_spi_cfg, reg_spi_ctrl, spi_prescale;
    mode  = 0U;
#if (SPI_DMA_CODE_EN)
    DMA_CFG_t dmaCfgR, dmaCfgT;
#endif    /* if (SPI_DMA_CODE_EN) */

#if (SPI_CM33_CODE_EN)

    if (!(spi->dma_en))
    {
		reg_spi_cfg = ( SPI_TX_DMA_DISABLE       |
						SPI_RX_DMA_DISABLE       |
						SPI_TX_START_INT_DISABLE |
						SPI_RX_INT_DISABLE       |
						SPI_CS_RISE_INT_DISABLE  |
						SPI_OVERRUN_INT_DISABLE  |
						SPI_UNDERRUN_INT_DISABLE |
						SPI_MODE_SPI             |
						SPI_WORD_SIZE_1          |
						SPI_PRESCALE_2           |
						SPI_CLK_POLARITY_NORMAL  |
#if RSL15_CID == 202
						SPI_CLK_PHASE_RISING     |
#endif    /* if RSL15_CID == 202 */
						SPI_SELECT_MASTER);
    }
#endif    /* if (SPI_CM33_CODE_EN) */

#if (SPI_DMA_CODE_EN)
    if (spi->dma_en)
    {
		reg_spi_cfg = ( SPI_TX_DMA_ENABLE        |
						SPI_RX_DMA_ENABLE        |
						SPI_OVERRUN_INT_DISABLE  |
						SPI_UNDERRUN_INT_DISABLE |
#if RSL15_CID == 202
						SPI_CLK_PHASE_RISING     |
#endif    /* if RSL15_CID == 202 */
						SPI_CLK_POLARITY_NORMAL);

		/* Prepare receiver mode DMA configuration */
		dmaCfgR.src_sel       = spi->dma_trg;
		dmaCfgR.src_step      = DMA_CFG0_SRC_ADDR_STATIC;
		dmaCfgR.dst_sel       = DMA_TRG_MEM;
		dmaCfgR.dst_step      = DMA_CFG0_DEST_ADDR_INCR_1;
		dmaCfgR.ch_priority   = DMA_CH_PRI_0;

		/* Prepare transmitter mode DMA configuration */
		dmaCfgT.src_sel       = DMA_TRG_MEM;
		dmaCfgT.src_step      = DMA_CFG0_SRC_ADDR_INCR_1;
		dmaCfgT.dst_sel       = spi->dma_trg;
		dmaCfgT.dst_step      = DMA_CFG0_DEST_ADDR_STATIC;
		dmaCfgT.ch_priority   = DMA_CH_PRI_0;
    }
#endif    /* if (SPI_DMA_CODE_EN) */

    reg_spi_ctrl = SPI_CS_1 | SPI_MODE_NOP | SPI_DISABLE;

    if ((spi->info->state & SPI_POWERED) == 0U)
    {
        /* SPI not powered */
        return ARM_DRIVER_ERROR;
    }

    /* Abort transfer */
    if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_ABORT_TRANSFER)
    {
        /* Stop transfer and set RW operation to Idle */
        spi->reg->CTRL = reg_spi_ctrl;

#if (SPI_CM33_CODE_EN)
        /* Clear pending SPI interrupts in nvic */
        NVIC_ClearPendingIRQ(spi->rx_irqn);
        NVIC_ClearPendingIRQ(spi->tx_irqn);
#endif    /* if (SPI_CM33_CODE_EN) */

#if (SPI_DMA_CODE_EN)
        if (spi->dma_en)
        {
			dma->Stop(spi->tx_dma_irqn);
			dma->Stop(spi->rx_dma_irqn);
        }
#endif    /* if (SPI_DMA_CODE_EN) */

        /* Clear transfer information and status */
        memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));
        spi->xfer->rx_cnt = 0U;
        spi->xfer->tx_cnt = 0U;
        spi->info->status.busy = 0U;
        return ARM_DRIVER_OK;
    }

    /* Check for busy flag */
    if (spi->info->status.busy)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Check for custom control used to switch SSEL pin on master device.
     * Should be used to select which slave (out of multiple) to communicate
     * with. */
    if (control == CUSTOM_SPI_CONTROL_SS)
    {
    	/* Ensure that the mode is master/SW since the SSEL line can only be
    	 * controlled in master/SW mode. */
    	if (((spi->info->mode & ARM_SPI_CONTROL_Msk) != ARM_SPI_MODE_MASTER) ||
    	    ((spi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk) !=
    	     ARM_SPI_SS_MASTER_SW))
		{
			return ARM_DRIVER_ERROR;
		}

    	/* Check whether valid GPIO pin passed in as arg */
    	if (!(arg < GPIO_PAD_COUNT))
    	{
    		/* arg is not a valid GPIO pin */
    		return ARM_DRIVER_ERROR;
    	}

    	/* Disable currently enabled SSEL line. This avoids having two GPIO
    	 * pins sending SSEL signals when we configure the GPIO passed in
    	 * through the arg parameter to be an SSEL line. */
    	SYS_GPIO_CONFIG(spi->pins->ssel,  GPIO_MODE_DISABLE);

    	/* Calculating whether we are configuring SPI0 or SPI1 pins */
        unsigned int diff = (spi->reg - SPI);

        /* Reconfigure GPIO passed in as arg to be an SSEL line */
    	SYS_GPIO_CONFIG(arg, (spi->pins->cfg |
    			              (GPIO_MODE_SPI0_CS + diff * SPI_PADS_NUM)));

    	/* Set SSEL pin in the SPI instance's resources to arg */
		spi->pins->ssel = arg;

		return ARM_DRIVER_OK;
     }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
        {
            /* Disable SPI */
            spi->reg->CTRL    = reg_spi_ctrl;
            spi->reg->CFG     = reg_spi_cfg;
            spi->info->state &= ~SPI_CONFIGURED;
            spi->info->mode   = ((spi->info->mode & ~ARM_SPI_CONTROL_Msk) |
                                 ARM_SPI_MODE_INACTIVE);
            return ARM_DRIVER_OK;
        }
        case ARM_SPI_MODE_MASTER:
        {
            /* Set master mode */
            mode  |= ARM_SPI_MODE_MASTER;
            reg_spi_cfg |= SPI_SELECT_MASTER;

            /* Configure GPIO */
            Sys_SPI_GPIOConfig(spi->reg,
							   SPI_SELECT_MASTER,
							   spi->pins->cfg,
							   spi->pins->sclk, spi->pins->ssel,
							   spi->pins->miso, spi->pins->mosi);

            switch (control & ARM_SPI_SS_MASTER_MODE_Msk)
            {
                case ARM_SPI_SS_MASTER_SW:
                case ARM_SPI_SS_MASTER_HW_OUTPUT:
                {
                    /* SSEL pin already configured above */
                    mode |= (control & ARM_SPI_SS_MASTER_MODE_Msk);
                }
                break;

                case ARM_SPI_SS_MASTER_UNUSED:
                {
                    /* Unconfigure SSEL Pin */
                    SYS_GPIO_CONFIG(spi->pins->ssel, GPIO_MODE_DISABLE);
                    mode |= ARM_SPI_SS_MASTER_UNUSED;
                }
                break;

                case ARM_SPI_SS_MASTER_HW_INPUT:
                default:
                    return ARM_SPI_ERROR_SS_MODE;
            }
        }
        break;

        case ARM_SPI_MODE_SLAVE:
        {
            /* Set slave mode */
            mode  |= ARM_SPI_MODE_SLAVE;
            reg_spi_cfg |= SPI_SELECT_SLAVE;

            /* Configure GPIO. MOSI and MISO switched. */
            Sys_SPI_GPIOConfig(spi->reg,
							   SPI_SELECT_SLAVE,
							   spi->pins->cfg,
							   spi->pins->sclk, spi->pins->ssel,
							   spi->pins->mosi, spi->pins->miso);

            switch (control & ARM_SPI_SS_SLAVE_MODE_Msk)
            {
                case ARM_SPI_SS_SLAVE_HW:
                {
                    /* Already configured above */
                }
                break;

                case ARM_SPI_SS_SLAVE_SW:
                {
                    /* Unconfigure SSEL Pin */
                    SYS_GPIO_CONFIG(spi->pins->ssel,  GPIO_MODE_DISABLE);
                }
                break;

                default:
                    return ARM_SPI_ERROR_SS_MODE;
            }
        }
        break;

        /* Simplex mode not supported */
        case ARM_SPI_MODE_MASTER_SIMPLEX:
        case ARM_SPI_MODE_SLAVE_SIMPLEX:
        {
            return ARM_SPI_ERROR_MODE;
        }

        case ARM_SPI_SET_BUS_SPEED:
        {
            if( (spi->info->mode & ARM_SPI_MODE_SLAVE) == ARM_SPI_MODE_SLAVE &&
                    /* If system clock is not fast enough */
                    SystemCoreClock < 4 * arg)
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            spi_prescale = SPI_CalculatePrescale(arg, SystemCoreClock);
            spi->reg->CFG = ((spi->reg->CFG & ~SPI_CFG_PRESCALE_Mask) |
                             (spi_prescale << SPI_CFG_PRESCALE_Pos));
            return ARM_DRIVER_OK;
        }

        case ARM_SPI_GET_BUS_SPEED:
        {
            /* Read SPI prescale value */
            spi_prescale = ((spi->reg->CFG&SPI_CFG_PRESCALE_Mask) >> SPI_CFG_PRESCALE_Pos);

            /* Return current bus speed */
            return (SystemCoreClock >> (spi_prescale + 1U));
        }

        case ARM_SPI_SET_DEFAULT_TX_VALUE:
        {
            uint8_t ws = (spi->reg->CFG & SPI_CFG_WORD_SIZE_Mask) >> SPI_CFG_WORD_SIZE_Pos;
            spi->xfer->def_val = (uint32_t)(arg & (0xFFFFFFFFU>>(31-ws)));

            return ARM_DRIVER_OK;
        }

        case ARM_SPI_CONTROL_SS:
        {
            if (((spi->info->mode & ARM_SPI_CONTROL_Msk) !=
                 ARM_SPI_MODE_MASTER) ||
                ((spi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk) !=
                 ARM_SPI_SS_MASTER_SW))
            {
                /* SSEL line can only be controlled in master/SW mode */
                return ARM_DRIVER_ERROR;
            }

            if (arg == ARM_SPI_SS_ACTIVE)
            {
                /* Set SSEL pin low */
                spi->reg->CTRL = SPI_CS_0;
            }
            else
            {
                /* Set SSEL pin high */
                spi->reg->CTRL = SPI_CS_1;
            }
            return ARM_DRIVER_OK;
        }

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* SPI Control Codes and Frame Format (Clock Polarity) */
    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
    	case ARM_SPI_CPOL0_CPHA0:
        {
            reg_spi_cfg |= SPI_CLK_POLARITY_NORMAL;
#if RSL15_CID == 202
            reg_spi_cfg |= SPI_CLK_PHASE_RISING;
#endif    /* if RSL15_CID == 202 */
        }
        break;

        case ARM_SPI_CPOL1_CPHA0:
        {
            reg_spi_cfg |= SPI_CLK_POLARITY_INVERSE;
#if RSL15_CID == 202
            reg_spi_cfg |= SPI_CLK_PHASE_RISING;
#endif    /* if RSL15_CID == 202 */
        }
        break;

        case ARM_SPI_CPOL0_CPHA1:
        {
            reg_spi_cfg |= SPI_CLK_POLARITY_NORMAL;
#if RSL15_CID == 202
            reg_spi_cfg |= SPI_CLK_PHASE_FALLING;
#endif    /* if RSL15_CID == 202 */
        }
        break;

        case ARM_SPI_CPOL1_CPHA1:
        {
            reg_spi_cfg |= SPI_CLK_POLARITY_INVERSE;
#if RSL15_CID == 202
            reg_spi_cfg |= SPI_CLK_PHASE_FALLING;
#endif    /* if RSL15_CID == 202 */
        }
        break;

        case ARM_SPI_TI_SSI:
        case ARM_SPI_MICROWIRE:
        default:
            return ARM_SPI_ERROR_FRAME_FORMAT;
    }

    /* Set number of data bits per SPI frame */
    switch (control & ARM_SPI_DATA_BITS_Msk)
    {
        case ARM_SPI_DATA_BITS(1U):
        {
            reg_spi_cfg |= SPI_WORD_SIZE_1;
        }
        break;

        case ARM_SPI_DATA_BITS(4U):
        {
            reg_spi_cfg |= SPI_WORD_SIZE_4;
#if (SPI_DMA_CODE_EN)
            if (spi->dma_en)
            {
				/* Update the DMA word size */
				dmaCfgT.word_size = DMA_CFG0_DEST_WORD_SIZE_4BITS_TO_4BITS;
				dmaCfgR.word_size = DMA_CFG0_DEST_WORD_SIZE_4BITS_TO_4BITS;
            }
#endif    /* if (SPI_DMA_CODE_EN) */
        }
        break;

        case ARM_SPI_DATA_BITS(8U):
        {
            reg_spi_cfg |= SPI_WORD_SIZE_8;
#if (SPI_DMA_CODE_EN)
            if (spi->dma_en)
            {
				/* Update the DMA word size */
				dmaCfgT.word_size = DMA_CFG0_DEST_WORD_SIZE_8BITS_TO_8BITS;
				dmaCfgR.word_size = DMA_CFG0_DEST_WORD_SIZE_8BITS_TO_8BITS;
            }

#endif    /* if (SPI_DMA_CODE_EN) */
        }
        break;

        case ARM_SPI_DATA_BITS(16U):
        {
            reg_spi_cfg |= SPI_WORD_SIZE_16;
#if (SPI_DMA_CODE_EN)
            if (spi->dma_en)
            {
			   /* Update the DMA word size */
			   dmaCfgT.word_size = DMA_CFG0_DEST_WORD_SIZE_16BITS_TO_16BITS;
			   dmaCfgR.word_size = DMA_CFG0_DEST_WORD_SIZE_16BITS_TO_16BITS;
            }

#endif    /* if (SPI_DMA_CODE_EN) */
        }
        break;

        case ARM_SPI_DATA_BITS(24U):
        {
            reg_spi_cfg |= SPI_WORD_SIZE_24;
#if (SPI_DMA_CODE_EN)
            if (spi->dma_en)
            {
			   /* Update the DMA word size */
			   dmaCfgT.word_size = DMA_CFG0_DEST_WORD_SIZE_24BITS_TO_24BITS;
			   dmaCfgR.word_size = DMA_CFG0_DEST_WORD_SIZE_24BITS_TO_24BITS;
            }

#endif    /* if (SPI_DMA_CODE_EN) */
        }
        break;

        case ARM_SPI_DATA_BITS(32U):
        {
            reg_spi_cfg |= SPI_WORD_SIZE_32;
#if (SPI_DMA_CODE_EN)
            if (spi->dma_en)
            {
				/* Update the DMA word size */
				dmaCfgT.word_size = DMA_CFG0_DEST_WORD_SIZE_32BITS_TO_32BITS;
				dmaCfgR.word_size = DMA_CFG0_DEST_WORD_SIZE_32BITS_TO_32BITS;
            }

#endif /* if (SPI_DMA_CODE_EN) */
        }
        break;

        default:
            return ARM_SPI_ERROR_DATA_BITS;
    }

    /* Bit order supports MSB to LSB only */
    if ((control & ARM_SPI_BIT_ORDER_Msk) != ARM_SPI_MSB_LSB)
    {
        return ARM_SPI_ERROR_BIT_ORDER;
    }

    /* Set SPI Bus Speed */
    spi_prescale = SPI_CalculatePrescale(arg, SystemCoreClock);
    reg_spi_cfg |= (spi_prescale << SPI_CFG_PRESCALE_Pos);
#if (SPI_DMA_CODE_EN)
    if (spi->dma_en)
    {
        /* Configure the RX DMA channel */
        dma->Configure(spi->pins->rx_dma_ch, &dmaCfgR, spi->pins->dma_rx_cb);

        /* Configure the TX DMA channel */
        dma->Configure(spi->pins->tx_dma_ch, &dmaCfgT, spi->pins->dma_tx_cb);
    }
#endif    /* if (SPI_DMA_CODE_EN) */

    /* Save mode */
    spi->info->mode = mode;

    /* Set SPI registers */
    Sys_SPI_Config(spi->reg, reg_spi_cfg);
    Sys_SPI_TransferConfig(spi->reg, reg_spi_ctrl);

    spi->info->state |= SPI_CONFIGURED;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Initiate transfer using SPI interface
 * @param[in]   data_out Data to be sent out
 * @param[in]   data_in  Input buffer
 * @param[in]   num  Length of data to be sent or received
 * @param[in]   spi  Pointer to SPI resources
 * @return      Execution status
 */
static int32_t SPI_Transfer(const void *data_out, void *data_in, uint32_t num,
                            const SPI_RESOURCES *spi)
{
    if (((data_out == NULL) && (data_in == NULL)) || (num == 0U))
    {
        /* Invalid parameters */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if ((spi->info->state & SPI_CONFIGURED) == 0U)
    {
        /* SPI is not configured */
        return ARM_DRIVER_ERROR;
    }

    if (spi->info->status.busy)
    {
        /* Previous send is not completed yet */
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Updates statuses */
    spi->info->status.busy       = 1U;
    spi->info->status.data_lost  = 0U;
    spi->info->status.mode_fault = 0U;

    /* Save transmit buffer info */
    spi->xfer->rx_buf = (uint8_t *)data_in;
    spi->xfer->tx_buf = (uint8_t *)data_out;
    spi->xfer->num    = num;
    spi->xfer->rx_cnt = 0U;
    spi->xfer->tx_cnt = 0U;

    /* Activate SSEL line, if HW controlled */
    if ((spi->info->mode & ARM_SPI_MODE_MASTER) &&
        (spi->info->mode & ARM_SPI_SS_MASTER_HW_OUTPUT))
    {
        Sys_SPI_TransferConfig(spi->reg, SPI_CS_0);
    }

#if (SPI_CM33_CODE_EN)
    if (!(spi->dma_en))
    {
		spi->reg->CFG |= SPI_TX_START_INT_ENABLE | SPI_RX_INT_ENABLE;
		spi->reg->CFG &= ~(SPI_OVERRUN_INT_ENABLE | SPI_UNDERRUN_INT_ENABLE);
		/* Fill SPI TX_DATA buffer for first transmission.
		 * Remaining transmissions will occur via the SPI Interrupts. */
		Sys_SPI_Write(spi->reg, SPI_PrepareNextTXData(spi));
    }
#endif    /* if (SPI_CM33_CODE_EN) */

    /* Configure SPI to RW operation and start transfer (master) */
    Sys_SPI_TransferConfig(spi->reg, SPI_ENABLE | SPI_MODE_READ_WRITE);
#if (SPI_DMA_CODE_EN)

    if (spi->dma_en)
    {
		/* Prepare DMA RX buffer configuration */
		DMA_ADDR_CFG_t buffRCfg = {
			.src_addr     = &spi->reg->RX_DATA,
			.dst_addr     = spi->xfer->rx_buf,
			.counter_len  = 0,
			.transfer_len = spi->xfer->num
		};

		/* Configure the RX DMA channel */
		dma->ConfigureAddr(spi->pins->rx_dma_ch, &buffRCfg);

		/* Prepare DMA TX buffer configuration */
		DMA_ADDR_CFG_t buffTCfg = {
			.src_addr     = spi->xfer->tx_buf,
			.dst_addr     = &spi->reg->TX_DATA,
			.counter_len  = 0,
			.transfer_len = spi->xfer->num
		};

		/* Configure the TX DMA channel */
		dma->ConfigureAddr(spi->pins->tx_dma_ch, &buffTCfg);

		/* Start the RX DMA channel */
		dma->Start(spi->pins->rx_dma_ch);
		/* Start the TX DMA channel */
		dma->Start(spi->pins->tx_dma_ch);
    }
#endif    /* if (SPI_DMA_CODE_EN) */

    return ARM_DRIVER_OK;
}

/**
 * @brief       Data count
 * @return      Current RX count
 */
static uint32_t SPI_GetDataCount(const SPI_RESOURCES *spi)
{
#if (SPI_DMA_CODE_EN)
	if (spi->dma_en)
	{
		/* Return counter value */
		return (dma->GetCounterValue(spi->pins->rx_dma_ch));
	}
#endif    /* if (SPI_DMA_CODE_EN) */

	return spi->xfer->rx_cnt;
}

#if RTE_SPI1_ENABLED
static int32_t SPI1_Send(const void *data, uint32_t num);
#endif    /* if RTE_SPI1_ENABLED */

/**
 * @brief       Driver version
 * @return      ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION ARM_SPI_GetVersion(void)
{
    return DriverVersion;
}

#if (SPI_CM33_CODE_EN)
/**
 * @brief       Called by hardware ISR when a byte is transferred. Prepares
 *                 next data in the buffer, manage flags and counters.
 *                 Application is notified via callback function once the
 *                 transfer is complete
 * @param[in]   spi Pointer to SPI resources
 */
static void SPI_TX_IRQHandler(const SPI_RESOURCES *spi)
{
    uint32_t event = 0U;

    if (spi->info->status.busy)
    {
        /* If data lost occurred on last transmission */
        if ((spi->reg->STATUS & SPI_OVERRUN_TRUE)  ||
            (spi->reg->STATUS & SPI_UNDERRUN_TRUE))
        {
            spi->info->status.data_lost = 1U;
            event |= ARM_SPI_EVENT_DATA_LOST;

            /* Clear error status */
            spi->reg->STATUS = SPI_UNDERRUN_CLEAR | SPI_OVERRUN_CLEAR;
        }

        spi->xfer->tx_cnt++;

        /* If has more data to transfer */
        if (spi->xfer->tx_cnt < spi->xfer->num)    /* Transfer next byte */
        {
            /* Fill TX_DATA SPI buffer */
            spi->reg->TX_DATA = SPI_PrepareNextTXData(spi);

            /* Start next transfer (if master) */
            Sys_SPI_TransferConfig(spi->reg, SPI_ENABLE | SPI_MODE_READ_WRITE);
        }
        else if (spi->xfer->tx_cnt == spi->xfer->rx_cnt)
        {
            /* Clear flags and stop transfer. */
            SPI_TransferCompleted(spi);

            event |= ARM_SPI_EVENT_TRANSFER_COMPLETE;
        }
    }

    /* If an event happened and the application registered a callback */
    if (event && spi->info->cb_event)
    {
        spi->info->cb_event(event);
    }
}

/**
 * @brief       Called by hardware ISR when a byte is received. Store
 *                 received data in the buffer, manage flags and counters.
 *                 Application is notified via callback function once the
 *                 receive is complete
 * @param[in]   spi Pointer to SPI resources
 */
static void SPI_RX_IRQHandler(const SPI_RESOURCES *spi)
{
    uint32_t data, i, ws;
    uint32_t event = 0U;

    if (spi->info->status.busy && spi->xfer->rx_cnt < spi->xfer->num)
    {
        /* If data lost occurred on last transmission */
        if ((spi->reg->STATUS & SPI_OVERRUN_TRUE)  ||
            (spi->reg->STATUS & SPI_UNDERRUN_TRUE))
        {
            spi->info->status.data_lost = 1U;
            event |= ARM_SPI_EVENT_DATA_LOST;

            /* Clear error status */
            spi->reg->STATUS |= SPI_UNDERRUN_CLEAR | SPI_OVERRUN_CLEAR;
        }

        /* Read received data from SPI RX_DATA register */
        data = spi->reg->RX_DATA;

        if (spi->xfer->rx_buf != NULL)
        {
            /* Get word size configuration (in bytes) */
            ws = ((spi->reg->CFG & SPI_CFG_WORD_SIZE_Mask) >> (SPI_CFG_WORD_SIZE_Pos+3)) + 1;

            /* Split received data into bytes and store into rx_buf */
            for (i = 0; i < ws; i++)
            {
                *spi->xfer->rx_buf++ = (uint8_t)(data >> (i * 8));
            }
        }

        spi->xfer->rx_cnt++;

        /* If transfer has finished */
        if ((spi->xfer->rx_cnt == spi->xfer->num) &&
            (spi->xfer->tx_cnt == spi->xfer->rx_cnt))
        {
            /* Clear flags and stop transfer. */
            SPI_TransferCompleted(spi);

            event |= ARM_SPI_EVENT_TRANSFER_COMPLETE;
        }
    }

    /* If an event happened and the application registered a callback */
    if (event && spi->info->cb_event)
    {
        spi->info->cb_event(event);
    }
}
#endif    /* if (SPI_CM33_CODE_EN) */

#if (SPI_DMA_CODE_EN)

/**
 * @brief       Called by hardware DMA when a word is transmitted.
 * 				   Check DMA transfer, manage flags and counters.
 *                 Application is notified via callback function once the
 *                 receive is complete
 * @param[in]   spi Pointer to SPI resources
 * @param[in]   event Operation
 */
static void SPI_TX_DMAHandler(uint32_t event, const SPI_RESOURCES *spi)
{
    /* Prepare status description */
	event = 0;
    if (spi->info->status.busy)
    {
        DMA_STATUS_t status = dma->GetStatus(spi->pins->tx_dma_ch);
		 
		/* If data lost occurred on last transmission */
        if ((spi->reg->STATUS & SPI_OVERRUN_TRUE)  ||
            (spi->reg->STATUS & SPI_UNDERRUN_TRUE))
        {
            spi->info->status.data_lost = 1U;
            event |= ARM_SPI_EVENT_DATA_LOST;

            /* Clear error status */
            spi->reg->STATUS = SPI_UNDERRUN_CLEAR | SPI_OVERRUN_CLEAR;
        }
		
        /* Check if DMA transfer has finished */
        if (status.completed  && dma->GetCounterValue(spi->pins->tx_dma_ch) == spi->xfer->num)
        {
    	    /* Stop the DMA */
    	    dma->Stop(spi->pins->tx_dma_ch);

            /* Check if full SPI transfer has finished */
            if (!spi->info->status.busy)
            {
                /* Clear flags and stop transfer. */
                SPI_TransferCompleted(spi);

                /* Indicate transfer complete */
                event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
            }
        }
    }

    /* If an event happened and the application registered a callback */
    if (event && spi->info->cb_event)
    {
        spi->info->cb_event(event);
    }
}

/**
 * @brief       Called by hardware DMA when a word is received.
 * 				   Check DMA transfer, manage flags and counters.
 *                 Application is notified via callback function once the
 *                 receive is complete
 * @param[in]   spi Pointer to SPI resources
 * @param[in]   event Operation
 */
static void SPI_RX_DMAHandler(uint32_t event, const SPI_RESOURCES *spi)
{
    /* Prepare status description */
	event = 0;
    if (spi->info->status.busy)
    {
         DMA_STATUS_t status = dma->GetStatus(spi->pins->rx_dma_ch);
		 
		/* If data lost occurred on last transmission */
        if ((spi->reg->STATUS & SPI_OVERRUN_TRUE)  ||
            (spi->reg->STATUS & SPI_UNDERRUN_TRUE))
        {
            spi->info->status.data_lost = 1U;
            event |= ARM_SPI_EVENT_DATA_LOST;

            /* Clear error status */
            spi->reg->STATUS = SPI_UNDERRUN_CLEAR | SPI_OVERRUN_CLEAR;
        }
		
	     /* Check if DMA transfer has finished */
        if (status.completed && dma->GetCounterValue(spi->pins->rx_dma_ch) == spi->xfer->num)
        {
            /* Stop the DMA */
            dma->Stop(spi->pins->rx_dma_ch);

            /* Clear flags and stop transfer. */
            SPI_TransferCompleted(spi);

            /* Indicate transfer complete */
            event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
        }
    }

    /* If an event happened and the application registered a callback */
    if (event && spi->info->cb_event)
    {
        spi->info->cb_event(event);
    }
}
#endif    /* if (SPI_DMA_CODE_EN) */

/**
 * @brief       Get SPI driver capabilities
 * @return      ARM_SPI_CAPABILITIES
 */
ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities(void)
{
    ARM_SPI_CAPABILITIES c = {
        .simplex          = 0,
        .ti_ssi           = 0,
        .microwire        = 0,
        .event_mode_fault = 0,
        .reserved         = 0,
    };
    return c;
}

/**
 * @brief       Get SPIx status
 * @param[in]   spi SPI resources
 * @return      Return SPIx status as an ARM_SPI_STATUS structure
 */
static ARM_SPI_STATUS SPI_GetStatus(const SPI_RESOURCES *spi)
{
    ARM_SPI_STATUS status;

    status.busy       = spi->info->status.busy;
    status.data_lost  = spi->info->status.data_lost;
    status.mode_fault = 0U;

    return status;
}

/* SPI0 Driver Wrapper functions. See functions above for usage */
#if RTE_SPI0_ENABLED
static int32_t SPI0_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
	int32_t status = SPI_Initialize(cb_event, &SPI0_Resources);
#if (RTE_SPI0_CFG_EN_DEFAULT)

    /* Set default power mode */
    status |= SPI_PowerControl(ARM_POWER_FULL, &SPI0_Resources);

    /* Set default SPI0 configuration (inactive mode) */
    status |= SPI_Control(SPI0_DEFAULT_CONTROL_CONFIG, RTE_SPI0_SPEED_DEFAULT, &SPI0_Resources);
#endif    /* if (RTE_SPI0_CFG_EN_DEFAULT) */

    return status;
}

static int32_t SPI0_Uninitialize(void)
{
    return SPI_Uninitialize(&SPI0_Resources);
}

static int32_t SPI0_PowerControl(ARM_POWER_STATE state)
{
    return SPI_PowerControl(state, &SPI0_Resources);
}

static int32_t SPI0_Control(uint32_t control, uint32_t arg)
{
    return SPI_Control(control, arg, &SPI0_Resources);
}

#if !(RTE_SPI0_DMA_EN_DEFAULT)
void SPI0_TX_IRQHandler(void)
{
    SPI_TX_IRQHandler(&SPI0_Resources);
}

void SPI0_RX_IRQHandler(void)
{
    SPI_RX_IRQHandler(&SPI0_Resources);
}
#endif    /* if !(RTE_SPI0_DMA_EN_DEFAULT) */

#if (RTE_SPI0_DMA_EN_DEFAULT)
void SPI0_TX_DMAHandler(uint32_t event)
{
	SPI_TX_DMAHandler(event, &SPI0_Resources);
}

void SPI0_RX_DMAHandler(uint32_t event)
{
	SPI_RX_DMAHandler(event, &SPI0_Resources);
}
#endif    /* if (RTE_SPI0_DMA_EN_DEFAULT) */

static int32_t SPI0_Send(const void *data, uint32_t num)
{
    return SPI_Transfer(data, NULL, num, &SPI0_Resources);
}

static int32_t SPI0_Receive(void *data, uint32_t num)
{
    return SPI_Transfer(NULL, data, num, &SPI0_Resources);
}

static int32_t SPI0_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_Transfer(data_out, data_in, num, &SPI0_Resources);
}

static uint32_t SPI0_GetDataCount(void)
{
    return SPI_GetDataCount(&SPI0_Resources);
}

static ARM_SPI_STATUS SPI0_GetStatus(void)
{
    return SPI_GetStatus(&SPI0_Resources);
}

ARM_DRIVER_SPI Driver_SPI0 = {
    .GetVersion      = ARM_SPI_GetVersion,
    .GetCapabilities = ARM_SPI_GetCapabilities,
    .Initialize      = SPI0_Initialize,
    .Uninitialize    = SPI0_Uninitialize,
    .PowerControl    = SPI0_PowerControl,
    .Send            = SPI0_Send,
    .Receive         = SPI0_Receive,
    .Transfer        = SPI0_Transfer,
    .GetDataCount    = SPI0_GetDataCount,
    .Control         = SPI0_Control,
    .GetStatus       = SPI0_GetStatus,
};

#endif    /* if RTE_SPI0_ENABLED */

/* SPI1 Driver Wrapper functions. See functions above for usage */
#if RTE_SPI1_ENABLED

static int32_t SPI1_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
	int32_t status = SPI_Initialize(cb_event, &SPI1_Resources);

#if (RTE_SPI1_CFG_EN_DEFAULT)

    /* Set default power mode */
    status |= SPI_PowerControl(ARM_POWER_FULL, &SPI1_Resources);

    /* Set default SPI1 configuration (inactive mode) */
    status |= SPI_Control(SPI1_DEFAULT_CONTROL_CONFIG, RTE_SPI1_SPEED_DEFAULT, &SPI1_Resources);
#endif    /* if (RTE_SPI1_CFG_EN_DEFAULT) */

	return status;
}

static int32_t SPI1_Uninitialize(void)
{
    return SPI_Uninitialize(&SPI1_Resources);
}

static int32_t SPI1_PowerControl(ARM_POWER_STATE state)
{
    return SPI_PowerControl(state, &SPI1_Resources);
}

static int32_t SPI1_Control(uint32_t control, uint32_t arg)
{
    return SPI_Control(control, arg, &SPI1_Resources);
}

#if !(RTE_SPI1_DMA_EN_DEFAULT)
void SPI1_TX_IRQHandler(void)
{
    SPI_TX_IRQHandler(&SPI1_Resources);
}

void SPI1_RX_IRQHandler(void)
{
    SPI_RX_IRQHandler(&SPI1_Resources);
}
#endif    /* if !(RTE_SPI1_DMA_EN_DEFAULT) */

#if (RTE_SPI1_DMA_EN_DEFAULT)
void SPI1_TX_DMAHandler(uint32_t event)
{
	SPI_TX_DMAHandler(event, &SPI1_Resources);
}
void SPI1_RX_DMAHandler(uint32_t event)
{
	SPI_RX_DMAHandler(event, &SPI1_Resources);
}
#endif    /* if (RTE_SPI1_DMA_EN_DEFAULT) */

static int32_t SPI1_Send(const void *data, uint32_t num)
{
    return SPI_Transfer(data, NULL, num, &SPI1_Resources);
}

static int32_t SPI1_Receive(void *data, uint32_t num)
{
    return SPI_Transfer(NULL, data, num, &SPI1_Resources);
}

static int32_t SPI1_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_Transfer(data_out, data_in, num, &SPI1_Resources);
}

static uint32_t SPI1_GetDataCount(void)
{
    return SPI_GetDataCount(&SPI1_Resources);
}

static ARM_SPI_STATUS SPI1_GetStatus(void)
{
    return SPI_GetStatus(&SPI1_Resources);
}

ARM_DRIVER_SPI Driver_SPI1 = {
    .GetVersion      = ARM_SPI_GetVersion,
    .GetCapabilities = ARM_SPI_GetCapabilities,
    .Initialize      = SPI1_Initialize,
    .Uninitialize    = SPI1_Uninitialize,
    .PowerControl    = SPI1_PowerControl,
    .Send            = SPI1_Send,
    .Receive         = SPI1_Receive,
    .Transfer        = SPI1_Transfer,
    .GetDataCount    = SPI1_GetDataCount,
    .Control         = SPI1_Control,
    .GetStatus       = SPI1_GetStatus
};
#endif    /* if RTE_SPI1_ENABLED */
