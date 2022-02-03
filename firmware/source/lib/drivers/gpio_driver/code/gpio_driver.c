/**
 * @file GPIO_driver.c
 * @brief GPIO Driver implementation
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
#include <GPIO_driver.h>

#if RTE_GPIO

#define ARM_GPIO_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(0, 1)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion =
{
    ARM_GPIO_API_VERSION,
    ARM_GPIO_DRV_VERSION
};

#ifdef RTE_GPIO_DRIVE_STRENGTHS_DEFAULT
/* Set default GPIO main configuration */
static const GPIO_CFG_t GPIO_DefaultCfg = {                      /* GPIO default main configuration */
    .drive_strengths         = RTE_GPIO_DRIVE_STRENGTHS_DEFAULT, /* GPIO default pads drive strength */
    .debounce_cfg.count      = RTE_GPIO_INT_DBC_CNT_DEFAULT,     /* GPIO default debounce filter counter value */
    .debounce_cfg.clk_source = RTE_GPIO_INT_DBC_CLK_SRC_DEFAULT  /* GPIO default debounce filter clock source */
};

#else
/* Set default GPIO main configuration */
static const GPIO_CFG_t GPIO_DefaultCfg = {                      /* GPIO default main configuration */
    .debounce_cfg.count      = RTE_GPIO_INT_DBC_CNT_DEFAULT,     /* GPIO default debounce filter counter value */
    .debounce_cfg.clk_source = RTE_GPIO_INT_DBC_CLK_SRC_DEFAULT  /* GPIO default debounce filter clock source */
};
#endif

#if (RTE_GPIO_JTAG_EN)

/* Set default GPIO jtag configuration */
static const GPIO_JTAG_CFG_t GPIO_JtagDefaultCfg = {             /* GPIO default jtag configuration */
    .jtck_lpf_en             = RTE_GPIO_JTAG_JTCK_LPF_DEFAULT,   /* GPIO default jtck low pass filter enable */
    .jtms_lpf_en             = RTE_GPIO_JTAG_JTMS_LPF_DEFAULT,   /* GPIO default jtms low pass filter enable */
    .jtag_data_en            = RTE_GPIO_JTAG_DATA_DEFAULT,       /* GPIO default jtag data on  GPIO[2:3] available */
    .jtag_trst_en            = RTE_GPIO_JTAG_TRST_DEFAULT,       /* GPIO default jtag trst on  GPIO4 available */
    .jtck_pull               = RTE_GPIO_JTAG_JTCK_PULL_DEFAULT,  /* GPIO default jtck pull mode */
    .jtms_pull               = RTE_GPIO_JTAG_JTMS_PULL_DEFAULT,  /* GPIO default jtms pull mode */
    .jtms_drive              = RTE_GPIO_JTAG_JTMS_DRIVE_DEFAULT  /* GPIO default jtms drive mode */
};
#endif /* if (RTE_GPIO_JTAG_EN) */

#if (RTE_GPIO0_EN)

/* Set default GPIO 0 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO0DefaultCfg = {             /* GPIO 0 default configuration */
    .pull_mode               = RTE_GPIO0_PULL_DEFAULT,           /* GPIO 0 default pull mode */
    .drive_mode              = RTE_GPIO0_DRIVE_DEFAULT,          /* GPIO 0 default drive mode */
    .lpf_en                  = RTE_GPIO0_LPF_DEFAULT,            /* GPIO 0 default low pass filter enable */
    .io_mode                 = RTE_GPIO0_IOMODE_DEFAULT,         /* GPIO 0 default io mode */
};
#endif /* if (RTE_GPIO0_EN) */

#if (RTE_GPIO1_EN)

/* Set default GPIO 1 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO1DefaultCfg = {             /* GPIO 1 default configuration */
    .pull_mode               = RTE_GPIO1_PULL_DEFAULT,           /* GPIO 1 default pull mode */
    .drive_mode              = RTE_GPIO1_DRIVE_DEFAULT,          /* GPIO 1 default drive mode */
    .lpf_en                  = RTE_GPIO1_LPF_DEFAULT,            /* GPIO 1 default low pass filter enable */
    .io_mode                 = RTE_GPIO1_IOMODE_DEFAULT,         /* GPIO 1 default io mode */
};
#endif /* if (RTE_GPIO1_EN) */

#if (RTE_GPIO2_EN)

/* Set default GPIO 2 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO2DefaultCfg = {             /* GPIO 2 default configuration */
    .pull_mode               = RTE_GPIO2_PULL_DEFAULT,           /* GPIO 2 default pull mode */
    .drive_mode              = RTE_GPIO2_DRIVE_DEFAULT,          /* GPIO 2 default drive mode */
    .lpf_en                  = RTE_GPIO2_LPF_DEFAULT,            /* GPIO 2 default low pass filter enable */
    .io_mode                 = RTE_GPIO2_IOMODE_DEFAULT,         /* GPIO 2 default io mode */
};
#endif /* if (RTE_GPIO2_EN) */

#if (RTE_GPIO3_EN)

/* Set default GPIO 3 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO3DefaultCfg = {             /* GPIO 3 default configuration */
    .pull_mode               = RTE_GPIO3_PULL_DEFAULT,           /* GPIO 3 default pull mode */
    .drive_mode              = RTE_GPIO3_DRIVE_DEFAULT,          /* GPIO 3 default drive mode */
    .lpf_en                  = RTE_GPIO3_LPF_DEFAULT,            /* GPIO 3 default low pass filter enable */
    .io_mode                 = RTE_GPIO3_IOMODE_DEFAULT,         /* GPIO 3 default io mode */
};
#endif /* if (RTE_GPIO3_EN) */

#if (RTE_GPIO4_EN)

/* Set default GPIO 4 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO4DefaultCfg = {             /* GPIO 4 default configuration */
    .pull_mode               = RTE_GPIO4_PULL_DEFAULT,           /* GPIO 4 default pull mode */
    .drive_mode              = RTE_GPIO4_DRIVE_DEFAULT,          /* GPIO 4 default drive mode */
    .lpf_en                  = RTE_GPIO4_LPF_DEFAULT,            /* GPIO 4 default low pass filter enable */
    .io_mode                 = RTE_GPIO4_IOMODE_DEFAULT,         /* GPIO 4 default io mode */
};
#endif /* if (RTE_GPIO4_EN) */

#if (RTE_GPIO5_EN)

/* Set default GPIO 5 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO5DefaultCfg = {             /* GPIO 5 default configuration */
    .pull_mode               = RTE_GPIO5_PULL_DEFAULT,           /* GPIO 5 default pull mode */
    .drive_mode              = RTE_GPIO5_DRIVE_DEFAULT,          /* GPIO 5 default drive mode */
    .lpf_en                  = RTE_GPIO5_LPF_DEFAULT,            /* GPIO 5 default low pass filter enable */
    .io_mode                 = RTE_GPIO5_IOMODE_DEFAULT,         /* GPIO 5 default io mode */
};
#endif /* if (RTE_GPIO5_EN) */

#if (RTE_GPIO6_EN)

/* Set default GPIO 6 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO6DefaultCfg = {             /* GPIO 6 default configuration */
    .pull_mode               = RTE_GPIO6_PULL_DEFAULT,           /* GPIO 6 default pull mode */
    .drive_mode              = RTE_GPIO6_DRIVE_DEFAULT,          /* GPIO 6 default drive mode */
    .lpf_en                  = RTE_GPIO6_LPF_DEFAULT,            /* GPIO 6 default low pass filter enable */
    .io_mode                 = RTE_GPIO6_IOMODE_DEFAULT,         /* GPIO 6 default io mode */
};
#endif /* if (RTE_GPIO6_EN) */

#if (RTE_GPIO7_EN)

/* Set default GPIO 7 pad configuration */
static const GPIO_PAD_CFG_t GPIO_GPIO7DefaultCfg = {             /* GPIO 7 default configuration */
    .pull_mode               = RTE_GPIO7_PULL_DEFAULT,           /* GPIO 7 default pull mode */
    .drive_mode              = RTE_GPIO7_DRIVE_DEFAULT,          /* GPIO 7 default drive mode */
    .lpf_en                  = RTE_GPIO7_LPF_DEFAULT,            /* GPIO 7 default low pass filter enable */
    .io_mode                 = RTE_GPIO7_IOMODE_DEFAULT,         /* GPIO 7 default io mode */
};
#endif /* if (RTE_GPIO7_EN) */

#if (RTE_GPIO_GPIO0_INT_EN)

/* Set default GPIO int 0 configuration */
static const GPIO_INT_CFG_t GPIO_GPIOInt0DefaultCfg = {          /* GPIO int 0 default configuration */
    .src_sel                 = RTE_GPIO_GPIO0_SRC_DEFAULT,       /* GPIO int 0 default source */
    .event                   = RTE_GPIO_GPIO0_TRG_DEFAULT,       /* GPIO int 0 default event */
    .debounce_en             = RTE_GPIO_GPIO0_DBC_DEFAULT,       /* GPIO int 0 default debounce enable */
    .interrup_en             = GPIO_ENABLE,                      /* GPIO int 0 default enable flag */
};
static const GPIO_PRI_CFG_t GPIO_GPIOPri0DefaultCfg = {          /* GPIO int 0 default priority configuration */
    .preempt_pri             = RTE_GPIO_GPIO0_INT_PREEMPT_PRI,   /* GPIO int 0 default preempt priority */
    .subgrp_pri              = RTE_GPIO_GPIO0_INT_SUBGRP_PRI,    /* GPIO int 0 default subgroup priority */
};
#endif /* if (RTE_GPIO_ GPIO0_INT_EN) */

#if (RTE_GPIO_GPIO1_INT_EN)

/* Set default GPIO int 1 configuration */
static const GPIO_INT_CFG_t GPIO_GPIOInt1DefaultCfg = {          /* GPIO int 1 default configuration */
    .src_sel                 = RTE_GPIO_GPIO1_SRC_DEFAULT,       /* GPIO int 1 default source */
    .event                   = RTE_GPIO_GPIO1_TRG_DEFAULT,       /* GPIO int 1 default event */
    .debounce_en             = RTE_GPIO_GPIO1_DBC_DEFAULT,       /* GPIO int 1 default debounce enable */
    .interrup_en             = GPIO_ENABLE,                      /* GPIO int 1 default enable flag */
};
static const GPIO_PRI_CFG_t GPIO_GPIOPri1DefaultCfg = {          /* GPIO int 1 default priority configuration */
    .preempt_pri             = RTE_GPIO_GPIO1_INT_PREEMPT_PRI,   /* GPIO int 1 default preempt priority */
    .subgrp_pri              = RTE_GPIO_GPIO1_INT_SUBGRP_PRI,    /* GPIO int 1 default subgroup priority */
};
#endif /* if (RTE_GPIO_ GPIO1_INT_EN) */

#if (RTE_GPIO_GPIO2_INT_EN)

/* Set default GPIO int 2 configuration */
static const GPIO_INT_CFG_t GPIO_GPIOInt2DefaultCfg = {          /* GPIO int 2 default configuration */
    .src_sel                 = RTE_GPIO_GPIO2_SRC_DEFAULT,       /* GPIO int 2 default source */
    .event                   = RTE_GPIO_GPIO2_TRG_DEFAULT,       /* GPIO int 2 default event */
    .debounce_en             = RTE_GPIO_GPIO2_DBC_DEFAULT,       /* GPIO int 2 default debounce enable */
    .interrup_en             = GPIO_ENABLE,                      /* GPIO int 2 default enable flag */
};
static const GPIO_PRI_CFG_t GPIO_GPIOPri2DefaultCfg = {          /* GPIO int 2 default priority configuration */
    .preempt_pri             = RTE_GPIO_GPIO2_INT_PREEMPT_PRI,   /* GPIO int 2 default preempt priority */
    .subgrp_pri              = RTE_GPIO_GPIO2_INT_SUBGRP_PRI,    /* GPIO int 2 default subgroup priority */
};
#endif /* if (RTE_GPIO_ GPIO2_INT_EN) */

#if (RTE_GPIO_GPIO3_INT_EN)

/* Set default GPIO int 3 configuration */
static const GPIO_INT_CFG_t GPIO_GPIOInt3DefaultCfg = {          /* GPIO int 3 default configuration */
    .src_sel                 = RTE_GPIO_GPIO3_SRC_DEFAULT,       /* GPIO int 3 default source */
    .event                   = RTE_GPIO_GPIO3_TRG_DEFAULT,       /* GPIO int 3 default event */
    .debounce_en             = RTE_GPIO_GPIO3_DBC_DEFAULT,       /* GPIO int 3 default debounce enable */
    .interrup_en             = GPIO_ENABLE,                      /* GPIO int 3 default enable flag */
};
static const GPIO_PRI_CFG_t GPIO_GPIOPri3DefaultCfg = {          /* GPIO int 3 default priority configuration */
    .preempt_pri             = RTE_GPIO_GPIO3_INT_PREEMPT_PRI,   /* GPIO int 3 default preempt priority */
    .subgrp_pri              = RTE_GPIO_GPIO3_INT_SUBGRP_PRI,    /* GPIO int 3 default subgroup priority */
};
#endif /* if (RTE_GPIO_ GPIO3_INT_EN) */

/* GPIO run-time information */
static GPIO_INFO_t GPIO_Info = {
    .default_cfg = &GPIO_DefaultCfg,
#if (RTE_GPIO_JTAG_EN)
    .default_jtag_cfg         = &GPIO_JtagDefaultCfg,            /* Set default GPIO jtag configuration */
#endif /* if (RTE_GPIO_JTAG_EN) */
#if (RTE_GPIO0_EN)
    .default_pad_cfg[GPIO_0]  = &GPIO_GPIO0DefaultCfg,           /* Set default GPIO 0 pad configuration */
#endif /* if (RTE_GPIO0_EN) */
#if (RTE_GPIO1_EN)
    .default_pad_cfg[GPIO_1]  = &GPIO_GPIO1DefaultCfg,           /* Set default GPIO 1 pad configuration */
#endif /* if (RTE_GPIO1_EN) */
#if (RTE_GPIO2_EN)
    .default_pad_cfg[GPIO_2]  = &GPIO_GPIO2DefaultCfg,           /* Set default GPIO 2 pad configuration */
#endif /* if (RTE_GPIO2_EN) */
#if (RTE_GPIO3_EN)
    .default_pad_cfg[GPIO_3]  = &GPIO_GPIO3DefaultCfg,           /* Set default GPIO 3 pad configuration */
#endif /* if (RTE_GPIO3_EN) */
#if (RTE_GPIO4_EN)
    .default_pad_cfg[GPIO_4]  = &GPIO_GPIO4DefaultCfg,           /* Set default GPIO 4 pad configuration */
#endif /* if (RTE_GPIO4_EN) */
#if (RTE_GPIO5_EN)
    .default_pad_cfg[GPIO_5]  = &GPIO_GPIO5DefaultCfg,           /* Set default GPIO 5 pad configuration */
#endif /* if (RTE_GPIO5_EN) */
#if (RTE_GPIO6_EN)
    .default_pad_cfg[GPIO_6]  = &GPIO_GPIO6DefaultCfg,           /* Set default GPIO 6 pad configuration */
#endif /* if (RTE_GPIO6_EN) */
#if (RTE_GPIO7_EN)
    .default_pad_cfg[GPIO_7]  = &GPIO_GPIO7DefaultCfg,           /* Set default GPIO 7 pad configuration */
#endif /* if (RTE_GPIO7_EN) */
#if (RTE_GPIO_GPIO0_INT_EN)
    .default_int_cfg[GPIO_INT_0] = &GPIO_GPIOInt0DefaultCfg,     /* Set default GPIO int 0 configuration */
    .default_pri_cfg[GPIO_INT_0] = &GPIO_GPIOPri0DefaultCfg,     /* Set default GPIO int 0 priority configuration */
#endif /* if (RTE_GPIO_ GPIO0_INT_EN) */
#if (RTE_GPIO_GPIO1_INT_EN)
    .default_int_cfg[GPIO_INT_1] = &GPIO_GPIOInt1DefaultCfg,     /* Set default GPIO int 1 configuration */
    .default_pri_cfg[GPIO_INT_1] = &GPIO_GPIOPri1DefaultCfg,     /* Set default GPIO int 1 priority configuration */
#endif /* if (RTE_GPIO_ GPIO1_INT_EN) */
#if (RTE_GPIO_GPIO2_INT_EN)
    .default_int_cfg[GPIO_INT_2] = &GPIO_GPIOInt2DefaultCfg,     /* Set default GPIO int 2 configuration */
    .default_pri_cfg[GPIO_INT_2] = &GPIO_GPIOPri2DefaultCfg,     /* Set default GPIO int 2 priority configuration */
#endif /* if (RTE_GPIO_ GPIO2_INT_EN) */
#if (RTE_GPIO_GPIO3_INT_EN)
    .default_int_cfg[GPIO_INT_3] = &GPIO_GPIOInt3DefaultCfg,     /* Set default GPIO int 3 configuration */
    .default_pri_cfg[GPIO_INT_3] = &GPIO_GPIOPri3DefaultCfg,     /* Set default GPIO int 3 priority configuration */
#endif /* if (RTE_GPIO_ GPIO3_INT_EN) */
};

/* GPIO resources */
static GPIO_RESOURCES GPIO_Resources =
{
    .info = &GPIO_Info,                                           /* GPIO run-time information */
#if (GPIO_INT_EN_MSK)
    .intInfo = {                                                  /* GPIO interrupt info */
        .irqn[GPIO_INT_0]        =  GPIO0_IRQn,                     /* GPIO 0 interrupt number */
        .irqn[GPIO_INT_1]        =  GPIO1_IRQn,                     /* GPIO 1 interrupt number */
        .irqn[GPIO_INT_2]        =  GPIO2_IRQn,                     /* GPIO 2 interrupt number */
        .irqn[GPIO_INT_3]        =  GPIO3_IRQn,                     /* GPIO 3 interrupt number */
        .cb = 0                                                   /* GPIO interrupt handler */
    }

#endif /* if (GPIO_INT_EN_MSK) */
};

/**
 * @brief       Driver version
 * @return      ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION GPIO_GetVersion(void)
{
    /* Return the driver version */
    return DriverVersion;
}

/**
 * @brief       Configure common GPIO settings
 * @param[in]   cfg Pointer to GPIO configuration structure
 * @return      int32_t ARM Driver return code
 */
static int32_t GPIO_Configure(const GPIO_CFG_t *cfg)
{
    /* Set the debounce configuration */
     GPIO->INT_DEBOUNCE = (cfg->debounce_cfg.clk_source <<  GPIO_INT_DEBOUNCE_DEBOUNCE_CLK_Pos) |
                        (cfg->debounce_cfg.count      <<  GPIO_INT_DEBOUNCE_DEBOUNCE_COUNT_Pos);
#ifdef RTE_GPIO_DRIVE_STRENGTHS_DEFAULT
    /* Set the GPIO drive strengths */
     GPIO->PAD_CFG = cfg->drive_strengths <<  GPIO_PAD_CFG_DRIVE_Pos;
#endif
    /* Return OK */
    return ARM_DRIVER_OK;
}

/**
 * @brief       Configure the GPIO jtag mode
 * @param[in]   cfg Pointer to jtag configuration structure
 * @return      ARM Driver return code
 */
static int32_t GPIO_ConfigureJTAG(const GPIO_JTAG_CFG_t *cfg)
{
    /* Set the jtag configuration */
     GPIO->JTAG_SW_PAD_CFG = (cfg->jtag_data_en <<  GPIO_JTAG_SW_PAD_CFG_CM33_JTAG_DATA_EN_Pos) |
                             (cfg->jtag_trst_en <<  GPIO_JTAG_SW_PAD_CFG_CM33_JTAG_TRST_EN_Pos) |
                             (cfg->jtck_lpf_en  <<  GPIO_JTAG_SW_PAD_CFG_JTCK_LPF_Pos)          |
                             (cfg->jtck_pull    <<  GPIO_JTAG_SW_PAD_CFG_JTCK_PULL_Pos)         |
                             (cfg->jtms_drive   <<  GPIO_JTAG_SW_PAD_CFG_JTMS_DRIVE_Pos)        |
                             (cfg->jtms_lpf_en  <<  GPIO_JTAG_SW_PAD_CFG_JTMS_LPF_Pos)          |
                             (cfg->jtms_pull    <<  GPIO_JTAG_SW_PAD_CFG_JTMS_PULL_Pos);

    /* Return OK */
    return ARM_DRIVER_OK;
}

/**
 * @brief       Configure the GPIO pad
 * @param[in]   sel Pad selection
 * @param[in]   cfg Pointer to pad configuration structure
 * @return      ARM Driver return code
 */
static int32_t GPIO_ConfigurePad(GPIO_SEL_t sel, const GPIO_PAD_CFG_t *cfg)
{
    /* Prepare GPIO pad config word */
    uint32_t padCfg = (cfg->drive_mode <<  GPIO_CFG_DRIVE_Pos)   |
                      (cfg->lpf_en     <<  GPIO_CFG_LPF_Pos)     |
                      (cfg->pull_mode  <<  GPIO_CFG_PULL_CTRL_Pos);

    /* Check if input IO has been selected */
       if (cfg->io_mode & GPIO_INPUT_MODE_MASK)
       {
           /* Set the io mode to input */
           GPIO->CFG[sel] = padCfg | GPIO_MODE_INPUT;

           /* Check which input mode has been selected */
           switch (cfg->io_mode)
           {
               case MODE_GPIO_SPI0_CS_IN:
               {
                   /* Set spi0 cs src */
                   GPIO_SRC_SPI->CS_BYTE = sel;
                   break;
               }

               case MODE_GPIO_SPI0_CLK_IN:
               {
                   /* Set spi0 clk src */
                   GPIO_SRC_SPI->CLK_BYTE = sel;
                   break;
               }
               case MODE_GPIO_UART_RX_IN:
               {
                   /* Set uart rx src */
                   GPIO->SRC_UART[0] &= ~(GPIO_SRC_UART_RX_Mask << GPIO_SRC_UART_RX_Pos);
                   GPIO->SRC_UART[0] |= sel << GPIO_SRC_UART_RX_Pos;
                   break;
               }

               case MODE_GPIO_I2C0_SCL_IN:
               {
                   /* Set i2c scl mode */
                   GPIO->CFG[sel] = padCfg | GPIO_MODE_I2C0_SCL;

                   /* Set i2c scl src */
                   GPIO_SRC_I2C->SCL_BYTE = sel;
                   break;
               }

               case MODE_GPIO_I2C0_SDA_IN:
               {
                   /* Set i2c sda mode */
                   GPIO->CFG[sel] = padCfg | GPIO_MODE_I2C0_SDA;

                   /* Set i2c sda src */
                   GPIO_SRC_I2C->SDA_BYTE = sel;
                   break;
               }
               case MODE_GPIO_NMI_IN:
               {
                   /* Set nmi src */
                   GPIO->SRC_NMI &= ~(GPIO_SRC_NMI_NMI_Mask << GPIO_SRC_NMI_NMI_Pos);
                   GPIO->SRC_NMI |= sel << GPIO_SRC_NMI_NMI_Pos;
                   break;
               }

               case MODE_GPIO_BB_RX_CLK_IN:
               {
                   /* Set bb rx clk src */
                   GPIO_SRC_BB_RX->CLK_BYTE = sel;
                   break;
               }

               case MODE_GPIO_BB_RX_DATA_IN:
               {
                   /* Set bb rx data src */
                   GPIO_SRC_BB_RX->DATA_BYTE = sel;
                   break;
               }

               case MODE_GPIO_BB_SYNC_P_IN:
               {
                   /* Set bb rx data src */
                   GPIO_SRC_BB_RX->SYNC_P_BYTE = sel;
                   break;
               }

               case MODE_GPIO_BB_SPI_MISO_IN:
               {
                   /* Set bb spi miso src */
                   GPIO->SRC_BB_SPI &= ~(GPIO_SRC_BB_SPI_MISO_Mask << GPIO_SRC_BB_SPI_MISO_Pos);
                   GPIO->SRC_BB_SPI |= sel << GPIO_SRC_BB_SPI_MISO_Pos;
                   break;
               }

               case MODE_GPIO_RF_SPI_MOSI_IN:
               {
                   /* Set rf spi mosi src */
                   GPIO_SRC_RF_SPI->MOSI_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_SPI_CSN_IN:
               {
                   /* Set rf spi csn src */
                   GPIO_SRC_RF_SPI->CSN_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_SPI_CLK_IN:
               {
                   /* Set rf spi clk src */
                   GPIO_SRC_RF_SPI->CLK_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO0_IN:
               {
                   /* Set rf GPIO 0 src */
                   GPIO_SRC_RF_GPIO03->GPIO0_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO1_IN:
               {
                   /* Set rf GPIO 1 src */
                   GPIO_SRC_RF_GPIO03->GPIO1_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO2_IN:
               {
                   /* Set rf GPIO 2 src */
                   GPIO_SRC_RF_GPIO03->GPIO2_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO3_IN:
               {
                   /* Set rf GPIO 3 src */
                   GPIO_SRC_RF_GPIO03->GPIO3_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO4_IN:
               {
                   /* Set rf GPIO 4 src */
                   GPIO_SRC_RF_GPIO47->GPIO4_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO5_IN:
               {
                   /* Set rf GPIO 5 src */
                   GPIO_SRC_RF_GPIO47->GPIO5_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO6_IN:
               {
                   /* Set rf GPIO 6 src */
                   GPIO_SRC_RF_GPIO47->GPIO6_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO7_IN:
               {
                   /* Set rf GPIO 7 src */
                   GPIO_SRC_RF_GPIO47->GPIO7_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO8_IN:
               {
                   /* Set rf GPIO 8 src */
                   GPIO_SRC_RF_GPIO89->GPIO8_BYTE = sel;
                   break;
               }

               case MODE_GPIO_RF_GPIO9_IN:
               {
                   /* Set rf GPIO 9 src */
                   GPIO_SRC_RF_GPIO89->GPIO9_BYTE = sel;
                   break;
               }

               case MODE_GPIO_ADC_IN:
               {
                   /* Set adc in src */
                   break;
               }

               default:
               {
                   break;
               }
           }
       }
       else
       {
           /* Set the io mode */
           GPIO->CFG[sel] = padCfg | (cfg->io_mode <<  GPIO_CFG_IO_MODE_Pos);
       }

       /* Return OK */
       return ARM_DRIVER_OK;
}

/**
 * @brief       Configure the GPIO interrupt
 * @param[in]   sel Interrupt selection
 * @param[in]   cfg Pointer to interrupt configuration structure
 * @return      ARM Driver return code
 */
static int32_t GPIO_ConfigureInterrupt(GPIO_INT_SEL_t sel, const GPIO_INT_CFG_t *cfg)
{
    /* Check if selected interrupt was enabled */
    if (!((GPIO_FLAG_BIT_SET << sel) & GPIO_INT_EN_MSK))
    {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* Configure interrupt settings */
     GPIO->INT_CFG[sel] = (cfg->debounce_en <<  GPIO_INT_CFG_DEBOUNCE_ENABLE_Pos) |
                          (cfg->event <<  GPIO_INT_CFG_EVENT_Pos)                 |
                          (cfg->src_sel <<  GPIO_INT_CFG_SRC_Pos);

    /* Check if interrupt should be enabled */
    if (cfg->interrup_en)
    {
        /* Clear pending flag */
        NVIC_ClearPendingIRQ(GPIO_Resources.intInfo.irqn[sel]);

        /* Enable the interrupt */
        NVIC_EnableIRQ(GPIO_Resources.intInfo.irqn[sel]);
    }
    else
    {
        /* Disable the interrupt */
        NVIC_DisableIRQ(GPIO_Resources.intInfo.irqn[sel]);

        /* Clear pending flag */
        NVIC_ClearPendingIRQ(GPIO_Resources.intInfo.irqn[sel]);
    }

    /* Return OK */
    return ARM_DRIVER_OK;
}

/**
 * @brief       Configure GPIO interrupt priority.
 * @param[in]   sel Interrupt selection
 * @param[in]   cfg Interrupt priority configuration
 * @return      ARM_DRIVER_OK
 */
static int32_t GPIO_SetInterruptPriority(GPIO_INT_SEL_t sel, const GPIO_PRI_CFG_t *cfg)
{
    /* Encoded priority value */
    uint32_t encodedPri = 0;

    /* Encode priority configuration word */
    encodedPri = NVIC_EncodePriority(NVIC_GetPriorityGrouping(), cfg->preempt_pri, cfg->subgrp_pri);

    /* Set the GPIO priority */
    NVIC_SetPriority(GPIO_Resources.intInfo.irqn[sel], encodedPri);

    /* Return OK */
    return ARM_DRIVER_OK;
}



/**
 * @brief       Reset the selected alternative function register.
 * @param[in]   reg Register selection
 * @return      ARM_DRIVER_OK
 */
static int32_t GPIO_ResetAltFuncRegister(GPIO_FUNC_REGISTERS_t reg)
{
    /* Check which register was selected */
    switch (reg)
    {
        case GPIO_FUNC_REG_SPI0:
        {
            /* Reset SPI0 register */
            GPIO->SRC_SPI[0] = SPI_CLK_SRC_CONST_HIGH | SPI_CS_SRC_CONST_HIGH;
            break;
        }

        case GPIO_FUNC_REG_UART0:
        {
            /* Reset UART register */
            GPIO->SRC_UART[0]= UART_RX_SRC_CONST_HIGH;
            break;
        }

        case GPIO_FUNC_REG_I2C0:
        {
            /* Reset I2C register */
            GPIO->SRC_I2C[0] = I2C_SCL_SRC_CONST_HIGH | I2C_SDA_SRC_CONST_HIGH;
            break;
        }

        case GPIO_FUNC_REG_NMI:
        {
            /* Reset nmi register */
            GPIO->SRC_NMI = NMI_SRC_CONST_LOW | NMI_ACTIVE_HIGH;
            break;
        }

        case GPIO_FUNC_REG_BB_RX:
        {
            /* Reset bb rx register */
            GPIO->SRC_BB_RX = BB_RX_DATA_SRC_RF_GPIO0 | BB_RX_CLK_SRC_RF_GPIO1 | BB_RX_SYNC_P_SRC_RF_GPIO2;
            break;
        }

        case GPIO_FUNC_REG_BB_SPI:
        {
            /* Reset bb spi register */
            GPIO->SRC_BB_SPI= BB_SPI_MISO_SRC_RF_SPI_MISO;
            break;
        }

        case GPIO_FUNC_REG_RF_SPI:
        {
            /* Reset rf spi register */
            GPIO->SRC_RF_SPI = RF_SPI_CLK_SRC_BB_SPI_CLK | RF_SPI_CSN_SRC_BB_SPI_CSN | RF_SPI_MOSI_SRC_BB_SPI_MOSI;
            break;
        }

        case GPIO_FUNC_REG_RF_GPIO03:
        {
            /* Reset rf GPIO03 register */
            GPIO->SRC_RF_GPIO03 = RF_GPIO0_SRC_CONST_LOW | RF_GPIO1_SRC_CONST_LOW | RF_GPIO3_SRC_BB_TX_DATA;
            break;
        }

        case GPIO_FUNC_REG_RF_GPIO47:
        {
            /* Reset rf GPIO47 register */
            GPIO->SRC_RF_GPIO47 = RF_GPIO4_SRC_BB_TX_DATA_VALID | RF_GPIO5_SRC_CONST_LOW |
                                  RF_GPIO6_SRC_CONST_LOW | RF_GPIO7_SRC_CONST_LOW;
            break;
        }

        case GPIO_FUNC_REG_RF_GPIO89:
        {
            /* Reset rf GPIO89 register */
            GPIO->SRC_RF_GPIO89 = RF_GPIO8_SRC_CONST_LOW | RF_GPIO9_SRC_CONST_LOW;
            break;
        }

        case GPIO_FUNC_REG_JTAG_SW_PAD:
        {
            /* Disable JTAG TDI, TDO, and TRST connections to GPIO 2, 3, and 4 */
            GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);
            break;
        }

        default:
        {
            break;
        }
    }

    /* Return OK */
    return ARM_DRIVER_OK;
}

/**
 * @brief       Initialize the GPIO driver
 * @param[in]   cb callback function
 * @return      ARM_DRIVER_OK
 */
static int32_t GPIO_Initialize (GPIO_SignalEvent_t cb)
{

    /* Reset all alternative function registers */
    for (int i = 0; i <= GPIO_ALT_FUNC_REG_NUMBER; i++)
    {
        GPIO_ResetAltFuncRegister(i);
    }

    /* Configure common settings */
    GPIO_Configure(GPIO_Resources.info->default_cfg);

#if (RTE_GPIO_JTAG_EN)

    /* Configure GPIO JTAG settings */
    GPIO_ConfigureJTAG(GPIO_Resources.info->default_jtag_cfg);
#endif /* if (RTE_GPIO_JTAG_EN) */

    /* Configure each GPIO pad which was set to be initialized */
    for (int i = 0; i < GPIO_PADS_NUMBER; ++i)
    {
        /* Check if particular GPIO pad should be initialized */
        if ((GPIO_FLAG_BIT_SET << i) & GPIO_EN_MSK)
        {
            /* For any GPIO configured as output, set the specified GPIO output value to low */
            if (GPIO_Resources.info->default_pad_cfg[i]->io_mode == MODE_GPIO_GPIO_OUT)
            {
                Sys_GPIO_Set_Low(i);
            }
			
            /* Configure particular GPIO pad */
            GPIO_ConfigurePad(i, GPIO_Resources.info->default_pad_cfg[i]);
        }
    }

#if (GPIO_INT_EN_MSK)

    /* Configure each GPIO interrupt which was set to be initialized */
    for (int i = 0; i < GPIO_INT_NUMBER; ++i)
    {
        /* Check if particular GPIO interrupt should be initialized */
        if ((GPIO_FLAG_BIT_SET << i) & GPIO_INT_EN_MSK)
        {
            /* Configure particular GPIO */
            GPIO_ConfigureInterrupt(i, GPIO_Resources.info->default_int_cfg[i]);

            /* Set the particuar GPIO interrupts priority */
            GPIO_SetInterruptPriority(i, GPIO_Resources.info->default_pri_cfg[i]);
        }
    }

    /* Set the callback function */
    GPIO_Resources.intInfo.cb = cb;
#endif /* if (GPIO_INT_EN_MSK) */

    /* Return OK */
    return ARM_DRIVER_OK;
}

/**
 * @brief       Set particular GPIO pad direction
 * @param[in]   sel  GPIO pad selection
 * @param[in]   dir  GPIO dir selection
 */
static void GPIO_SetDir(GPIO_DIR_t dir)
{
    /* Set selected pad direction */
    Sys_GPIO_Set_Direction(dir);
}

/**
 * @brief       Set particular GPIO pad
 * @param[in]   sel GPIO pad selection
 */
static void GPIO_SetHigh(GPIO_SEL_t sel)
{
    /* Set selected pad */
    Sys_GPIO_Set_High(sel);
}

/**
 * @brief       Toggle particular GPIO pad
 * @param[in]   sel GPIO pad selection
 */
static void GPIO_ToggleValue(GPIO_SEL_t sel)
{
    /* Toggle selected pad */
    Sys_GPIO_Toggle(sel);
}

/**
 * @brief       Reset particular GPIO pad
 * @param[in]   sel GPIO pad selection
 */
static void GPIO_SetLow(GPIO_SEL_t sel)
{
    /* Reset selected pad */
    Sys_GPIO_Set_Low(sel);
}

/**
 * @brief       Return the selected GPIO value
 * @param[in]   sel GPIO pad selection
 * @return      GPIO pad value
 */
static uint32_t GPIO_ReadValue(GPIO_SEL_t sel)
{
    /* Reset selected pad */
    return Sys_GPIO_Read(sel);
}

#if (RTE_GPIO_GPIO0_INT_EN)
/**
 * @brief        Called by hardware ISR when a GPIO event occurs. Application
 *                 is notified via callback function once the interrupt occurs.
 */
void  GPIO0_IRQHandler(void)
{
    /* Check if callback was set */
    if (GPIO_Resources.intInfo.cb)
    {
        /* Execute application callback */
        GPIO_Resources.intInfo.cb(GPIO_EVENT_0_IRQ);
    }
}

#endif /* if (RTE_GPIO_ GPIO0_INT_EN) */

#if (RTE_GPIO_GPIO1_INT_EN)
/**
 * @brief        Called by hardware ISR when a GPIO event occurs. Application
 *                 is notified via callback function once the interrupt occurs.
 */
void  GPIO1_IRQHandler(void)
{
    /* Check if callback was set */
    if (GPIO_Resources.intInfo.cb)
    {
        /* Execute application callback */
        GPIO_Resources.intInfo.cb(GPIO_EVENT_1_IRQ);
    }
}

#endif /* if (RTE_GPIO_ GPIO1_INT_EN) */

#if (RTE_GPIO_GPIO2_INT_EN)
/**
 * @brief        Called by hardware ISR when a GPIO event occurs. Application
 *                 is notified via callback function once the interrupt occurs.
 */
void  GPIO2_IRQHandler(void)
{
    /* Check if callback was set */
    if (GPIO_Resources.intInfo.cb)
    {
        /* Execute application callback */
        GPIO_Resources.intInfo.cb(GPIO_EVENT_2_IRQ);
    }
}

#endif /* if (RTE_GPIO_ GPIO2_INT_EN) */

#if (RTE_GPIO_GPIO3_INT_EN)
/**
 * @brief        Called by hardware ISR when a GPIO event occurs. Application
 *                 is notified via callback function once the interrupt occurs.
 */
void  GPIO3_IRQHandler(void)
{
    /* Check if callback was set */
    if (GPIO_Resources.intInfo.cb)
    {
        /* Execute application callback */
        GPIO_Resources.intInfo.cb(GPIO_EVENT_3_IRQ);
    }
}

#endif /* if (RTE_GPIO_ GPIO3_INT_EN) */

/* GPIO Driver Control Block */
DRIVER_GPIO_t Driver_GPIO =
{
    GPIO_GetVersion,
    GPIO_Initialize,
    GPIO_Configure,
    GPIO_ConfigurePad,
    GPIO_ConfigureInterrupt,
    GPIO_SetInterruptPriority,
    GPIO_ConfigureJTAG,
    GPIO_SetDir,
    GPIO_SetHigh,
    GPIO_ToggleValue,
    GPIO_SetLow,
    GPIO_ReadValue,
    GPIO_ResetAltFuncRegister
};

#endif    /* RTE_GPIO */
