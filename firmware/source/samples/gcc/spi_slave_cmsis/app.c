/**
 * @file app.c
 *  @brief Main application source file.
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

/* ----------------------------------------------------------------------------
 * Global variables
 * --------------------------------------------------------------------------*/
extern ARM_DRIVER_SPI Driver_SPI0;
static uint8_t spi0_tx_buffer[APP_SPI_BUFFER_SIZE] = APP_SPI_DATA;
static uint8_t spi0_rx_buffer[APP_SPI_BUFFER_SIZE];

void Initialize(void)
{
    /* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

    /* Disable all existing interrupts, clearing all pending source */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Check for recovery GPIO to enable recovery. */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while ((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Disable JTAG TDI, TDO, and TRST connections to GPIO 2, 3, and 4 */
    GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);

    /* Configure specified GPIOs for GPIO output and set output to low */
    SYS_GPIO_CONFIG(APP_SPI0_EVENT_GPIO, (GPIO_LEVEL1_DRIVE | GPIO_LPF_DISABLE |
                                          GPIO_STRONG_PULL_UP | GPIO_MODE_GPIO_OUT));

    /* APP_SPI0_EVENT_GPIO high until toggled to indicate SPI transfer state */
    Sys_GPIO_Set_High(APP_SPI0_EVENT_GPIO);

    /* Stop masking interrupts */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

void ToggleGPIO(uint32_t gpio, uint32_t n, float delay_s)
{
    for (; n > 0; n--)
    {
        Sys_GPIO_Toggle(gpio);
        Sys_Delay(delay_s * SystemCoreClock);
    }
}

void Slave_CallBack(uint32_t event)
{
    /* Check event */
    switch (event)
    {
        /* Transfer complete event */
        case ARM_SPI_EVENT_TRANSFER_COMPLETE:
        {
            /* Blink LED if received data matches SPI_TX_DATA */
            if (!strcmp((const char *)spi0_tx_buffer, (const char *)spi0_rx_buffer))
            {
                /* Create two 0.5 second pulses, to indicate reception finished */
                ToggleGPIO(APP_SPI0_EVENT_GPIO, 4, 0.5);
            }
        }
        break;

        /* Data lost event */
        case ARM_SPI_EVENT_DATA_LOST:
        {
            /* Create five 0.5 second pulses, to indicate error */
            ToggleGPIO(APP_SPI0_EVENT_GPIO, 10, 0.5);
            /* Abort current transfer */
            Driver_SPI0.Control(ARM_SPI_ABORT_TRANSFER, 0);
        }
        break;
    }

    /* Get ready for next transfer (slave) */
    Driver_SPI0.Transfer(spi0_tx_buffer, spi0_rx_buffer, APP_SPI_BUFFER_SIZE);
}

int main(void)
{
    /* Initialize the clocks and configure button press interrupt */
    Initialize();

    /* Power-up the slave SPI interface and register the callback function */
    Driver_SPI0.Initialize(Slave_CallBack);

    /* Get ready for next transfer (slave) */
    Driver_SPI0.Transfer(spi0_tx_buffer, spi0_rx_buffer, APP_SPI_BUFFER_SIZE);

    /* Spin loop */
    while (1)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    /* Should never be reached */
    return 0;
}
