/**
 * @file app.c
 * @brief Main application source file
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

#include "app.h"

/* Global variables */
DRIVER_DMA_t *dma;
DRIVER_GPIO_t *gpio;
uint8_t src_buffer[APP_DMA_BUFFER_SIZE] = APP_DMA_DATA;
uint8_t dst_buffer[APP_DMA_BUFFER_SIZE];

uint8_t volatile transferCompletedFlag = 0;
uint8_t dataCorrect = 1;

void DMA_EventCallback(uint32_t event)
{
    /* Check if dma channel0 event has been triggered */
    if (event & DMA_DMA0_EVENT)
    {
        /* Check if transfer has been completed */
        if (dma->GetStatus(DMA_CH_0).completed == 1)
        {
            /* Indicate transfer completed */
            transferCompletedFlag = 1;
        }
    }
}

/**
 * @brief Initialize the system, including GPIOs and interrupts.
 * @return Zero
 */
void Initialize(void)
{
    /* Interrupts off */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Disable all existing interrupts, clearing all pending source */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Check for recovery GPIO to enable recovery. */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, (GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE |
                                    GPIO_WEAK_PULL_UP | GPIO_6X_DRIVE));

    while ((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

    /* Load default trim values. */
    uint32_t trim_error __attribute__((unused)) = SYS_TRIM_LOAD_DEFAULT();

    /* Enable the 48MHz XTAL */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Enable/disable buck converter */
    ACS->VCC_CTRL = ((ACS->VCC_CTRL & ~VCC_BUCK) | VCC_BUCK_LDO_CTRL);

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Configure Baseband Controller Interface */
    BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8);

    /* Interrupts back on */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

/**
 * @brief The main entry point for the program
 */
int main(void)
{

    /* Initialize the system */
    Initialize();

    /* Initialize gpio structure */
	gpio = &Driver_GPIO;

	/* Initialize gpio driver */
	gpio->Initialize(NULL);

    /* Initialize DMA structure */
    dma = &Driver_DMA;

    /* Configure DMA callback function */
    dma->Initialize(DMA_EventCallback);

 /* Prepare source / destination configuration */
    DMA_ADDR_CFG_t addrCfg = {
        .src_addr     = src_buffer,
        .dst_addr     = dst_buffer,
        .counter_len  = 0,
        .transfer_len = APP_DMA_BUFFER_SIZE
    };

    /* Set the src / dst configuration */
    dma->ConfigureAddr(DMA_CH_0, &addrCfg);

    /* Start the data transfer */
    dma->Start(DMA_CH_0);

    /* Main application spin loop */
    while (!transferCompletedFlag)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    /* Check if the data was transferred correctly */
    for (int i = 0; i < APP_DMA_BUFFER_SIZE; i++)
    {
        /* Check if each byte is correct */
        if (dst_buffer[i] != src_buffer[i])
        {
            /* Indicate error */
            dataCorrect = 0;
            break;
        }
    }

    /* If data was sent correctly, light the LED */
    if (dataCorrect)
    {
        gpio->SetLow(DMA_STATES_GPIO);
    }

    /* Main application spin loop */
    while (1)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    /* Should never be reached */
    return 0;
}
