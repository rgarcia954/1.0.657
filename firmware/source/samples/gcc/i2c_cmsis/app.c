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
ARM_DRIVER_I2C *i2c;
uint8_t buffer[I2C_BUFFER_SIZE];
uint8_t orig_text_buffer[I2C_BUFFER_SIZE] = I2C_DATA;											  

void error_check(uint32_t status)
{
    /* Otherwise, halt the program */
    if (status != ARM_DRIVER_OK)
    {
        ToggleGPIO(APP_I2C_EVENT_GPIO, 4, 2);
        while (1)
        {
            SYS_WATCHDOG_REFRESH();
        }
    }
}

void GPIO0_IRQHandler(void)
{
    static bool ignore_next_gpio_int = false;
    if (ignore_next_gpio_int)
    {
        ignore_next_gpio_int = false;
    }

    /* Check if button pressed. */
    else if ((Sys_GPIO_Read(BUTTON_GPIO) == 0) && (i2c->GetStatus().mode == 0))
    {
        /* Button is pressed: Ignore next interrupt.
         * This is required to deal with the debounce circuit limitations. */
        ignore_next_gpio_int = true;

        /* Abort current transfer */
        i2c->Control(ARM_I2C_ABORT_TRANSFER, 0);

        /* Start transmission as Master */
        i2c->MasterTransmit(I2C_OWN_SLAVE_ADDR, orig_text_buffer, I2C_BUFFER_SIZE, false);
    }
    else
    {
        /* do nothing */
    }
}

void ToggleGPIO(uint32_t gpio, uint32_t n, uint32_t delay_ms)
{
    while (n-- > 0)
    {
        Sys_GPIO_Toggle(gpio);
        Sys_Delay((delay_ms * SystemCoreClock) / 1000);
    }
}

void I2C_CallBack(uint32_t event)
{
    bool isReceiver = (i2c->GetStatus().direction == 1);
    bool isMaster = (i2c->GetStatus().mode == 1);

    if ((event & ARM_I2C_EVENT_TRANSFER_DONE) & (isReceiver))
    {
        /* MasterReceive finished, go back to SlaveReceive default mode */
        if (isMaster)
        {
            i2c->SlaveReceive(buffer, I2C_BUFFER_SIZE);
        }

        /* SlaveReceive finished, start SlaveTransmit */
        else
        {
            i2c->SlaveTransmit(buffer, I2C_BUFFER_SIZE);
        }

        /* Toggle GPIO if received data matches I2C_TX_DATA */
        if (!strcmp(I2C_DATA, (const char *)buffer))
        {
            /* Toggle GPIO state 6 times for 0.25 seconds */
            ToggleGPIO(APP_I2C_EVENT_GPIO, 6, 250);
        }
        else
        {
            /* Data does not match. Blink error code.*/
            ToggleGPIO(APP_I2C_EVENT_GPIO, 20, 50);
        }
    }

    /* Transmitter */
    else if (event & ARM_I2C_EVENT_TRANSFER_DONE)
    {
        /* MasterTransmit finished, start MasterReceive */
        if (isMaster)
        {
            i2c->MasterReceive(I2C_OWN_SLAVE_ADDR, buffer, I2C_BUFFER_SIZE, false);
        }

        /* SlaveTransmit finished, go back to SlaveReceive default mode */
        else
        {
            i2c->SlaveReceive(buffer, I2C_BUFFER_SIZE);
        }
    }
    /* If we hit an error state, signal via GPIO that an error occurred and
     * abort the current transfer.
     */
    else if ((event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) ||
             (event & ARM_I2C_EVENT_ADDRESS_NACK) ||
             (event & ARM_I2C_EVENT_ARBITRATION_LOST) ||
             (event & ARM_I2C_EVENT_BUS_ERROR))
    {
        /* Abort current transfer */
        i2c->Control(ARM_I2C_ABORT_TRANSFER, 0);

        /* Go back to SlaveReceive default mode */
        i2c->SlaveReceive(buffer, I2C_BUFFER_SIZE);

        /* To signal error, toggle GPIO state very fast
         * (10 times for 0.05 seconds each).
         * Only signal bus errors to the user.  */
        if (event & ARM_I2C_EVENT_BUS_ERROR)
        {
            /* Toggle GPIO 10 times, with 50ms period. */
            ToggleGPIO(APP_I2C_EVENT_GPIO, 10, 50);
        }
    }
    else
    {
        /* if other event received, do nothing */
        /* Other possible events that are ignored here:
         * ARM_I2C_EVENT_SLAVE_TRANSMIT
         * ARM_I2C_EVENT_SLAVE_RECEIVE
         * ARM_I2C_EVENT_GENERAL_CALL
         * ARM_I2C_EVENT_BUS_CLEAR
         */
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

    SYS_GPIO_CONFIG(BUTTON_GPIO, (GPIO_MODE_GPIO_IN | GPIO_WEAK_PULL_UP |
                                  GPIO_LPF_DISABLE));
    Sys_GPIO_IntConfig(0, GPIO_EVENT_TRANSITION | GPIO_SRC(BUTTON_GPIO) |
                       GPIO_DEBOUNCE_ENABLE,
                       GPIO_DEBOUNCE_SLOWCLK_DIV1024, MAX_DEBOUNCE);

    SYS_GPIO_CONFIG(APP_I2C_EVENT_GPIO, GPIO_MODE_GPIO_OUT);

    /* Disable JTAG TDI, TDO, and TRST connections to GPIO 2, 3, and 4 */
    GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);

    /* Enable BUTTON_GPIO interrupt */
    NVIC_EnableIRQ(GPIO0_IRQn);

    /* Power-up I2C and register callback function  */
    i2c = &Driver_I2C0;
    i2c->Initialize(I2C_CallBack);
    error_check(i2c->PowerControl(ARM_POWER_FULL));

    /* Configure bus speed, if bus is busy, abort transfer and try again. */
    while (i2c->Control(ARM_I2C_BUS_SPEED, I2C_SPEED)
           != ARM_DRIVER_OK)
    {
        i2c->Control(ARM_I2C_ABORT_TRANSFER, 0);
    }

    /* Set own slave address. Used when operating in slave mode.
     * If bus is busy, abort transfer and try again. */
    while (i2c->Control(ARM_I2C_OWN_ADDRESS, I2C_OWN_SLAVE_ADDR)
           != ARM_DRIVER_OK)
    {
        i2c->Control(ARM_I2C_ABORT_TRANSFER, 0);
    }

    /* Wait for new transfer as slave */
    error_check(i2c->SlaveReceive(buffer, I2C_BUFFER_SIZE));

    /* Main application spin loop */
    while (1)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    /* Should never be reached */
    return 0;
}
