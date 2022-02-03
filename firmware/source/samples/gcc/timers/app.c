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
DRIVER_TIMER_t *timer;
DRIVER_GPIO_t *gpio;
unsigned int timer_val [2] = {0};

void Button_EventCallback(uint32_t event)
{
    static uint8_t ignore_next_gpio_int = 0;
    if (ignore_next_gpio_int == 1)
    {
        ignore_next_gpio_int = 0;
    }
    else if (event == GPIO_EVENT_0_IRQ)
    {
        /* Button is pressed: Ignore next interrupt.
         * This is required to deal with the debounce circuit limitations. */
        ignore_next_gpio_int = 1;

        timer_val[0] = timer->GetValueCapture(TIMER_3);
        if(timer_val[0] != timer_val[1]){
        	gpio->ToggleValue(TIMER_CAPTURE_MODE_GPIO);
        }
        timer_val[1] = timer->GetValueCapture(TIMER_3);
    }
}

void Timer_EventCallback(uint32_t event)
{
    /* Timer 0 timeout value */
    static uint32_t timeout = 0;

    /* Check if timer0 event has been triggered */
    if (event & TIMER_TIMER0_EVENT)
    {
        /* Toggle LED Pin */
        gpio->ToggleValue(TIMER_STATES_GPIO);

        /* Exit interrupt callback */
        return;
    }

    /* Check if timer1 event has been triggered */
    if (event & TIMER_TIMER1_EVENT)
    {
        /* Reconfigure timer 0 timeout value */
        timeout = TIMER0_BASE_TIMEOUT;
        timer->SetValue(TIMER_0, timeout);

        /* Start timer 2 (multi shot mode) */
        timer->Start(TIMER_2);

        /* Exit interrupt callback */
        return;
    }

    /* Check if timer2 event has been triggered */
    if (event & TIMER_TIMER2_EVENT)
    {
        /* Reconfigure timer 0 timeout value */
        timeout -= TIMER0_TIMEOUT_STEP;
        timer->SetValue(TIMER_0, timeout);

        /* Exit interrupt callback */
        return;
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
	gpio->Initialize(Button_EventCallback);

	/* Initialize timer structure */
	timer = &Driver_TIMER;

	/* Configure timer callback function */
	timer->Initialize(Timer_EventCallback);

	/* Start timer 0 (free run mode) */
	timer->Start(TIMER_0);

	/* Start timer 1 (single shot mode) */
	timer->Start(TIMER_1);

	/* Start timer 3 (gpio interrupt capture) */
	timer->Start(TIMER_3);

	/* Enable Timer GPIO interrupt */
	timer->SetGPIOInterrupt(TIMER_3);

    /* Main application spin loop */
    while (1)
    {
	    /* Wait for an event */
	    __WFE();
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    /* Should never be reached */
    return 0;
}
