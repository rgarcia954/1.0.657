/**
 * @file blinky.c
 * @brief Implementation of a blinky application using a timer interrupt
 * to accurately time the state changes.
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
#include "hw.h"
#include "blinky.h"

/** @brief Variable holding the selected GPIO number. */
static uint32_t gpioNumber = 0;

/** @brief Variable holding the current state of the GPIO pin. */
static volatile bool high = true;

/**
 * @brief Toggle the state of the GPIO.
 */
static void toggle(void)
{
    high = ! high;
    if (high)
    {
        GPIO->OUTPUT_DATA |= (1 << gpioNumber);
    }
    else
    {
        GPIO->OUTPUT_DATA &= (~(1 << gpioNumber));
    }
}

/**
 * @brief Timer ISR which is used to toggle the GPIO.
 */
void TIMER0_IRQHandler(void)
{
    toggle();
}

void blinkyInitialize(uint32_t gpio, uint32_t delay)
{
    // Store this information for use later
    gpioNumber = gpio;
    high = true;

    // Set the GPIO as output
    SYS_GPIO_CONFIG(gpio, GPIO_MODE_GPIO_OUT);

    // And initialize it to low
    toggle();

    // grab the clock divisor, we'll need that to handle the delay accurately
    uint32_t clock = (CLK->DIV_CFG0 & 0xFF) + 1;

    // calculate the number of cycles to delay based on the core clock and the
    // various divisors in the path
    uint32_t count = SystemCoreClock;
    count /= 2;         // take into account slowclk_div2 (below)
    count /= 4;         // take into account timer_prescale_4 (below)
    count /= clock;     // take into account slow clock divisor (above)
    count /= 1000;      // take into account conversion to ms
    count *= delay;     // multiply up by number of ms

    // enable a timer to provide us the tick interrupt at the specified rate.
    Sys_Timer_Config(TIMER0, TIMER_PRESCALE_4, TIMER_FREE_RUN | TIMER_SLOWCLK_DIV2, count);
    Sys_Timer_Start(TIMER0);

    NVIC_EnableIRQ(TIMER0_IRQn);
}

bool isLedOn(void)
{
    return high;
}


