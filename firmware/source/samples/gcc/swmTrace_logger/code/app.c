/**
 * @file app.c
 * @brief Simple application allowing different forms of tracing to be employed.
 * @details
 * This application performs a number of tasks:
 *     It creates a timer at a regular rate
 *     The timer is used to toggle a GPIO at a fixed interval
 *     Each time the GPIO is toggled, a message is logged
 *     The messages are logged using the swmTrace on a specific UART TX pin
 *     The RX pin is used to read the messages via a loop back technique
 *     Finally, the read information is output using the Segger RTT directly
 *
 * In this way we can verify the logging happens correctly with a single board.
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

#include <stdbool.h>

#ifdef UART_LOOPBACK_TEST
#include "SEGGER_RTT.h"
#endif

#include "swmTrace_api.h"
#include "app_initialize.h"
#include "blinky.h"
#include "hw.h"

/** The GPIO pin to use for TX when using the UART mode */
#define UART_TX_GPIO    (6)

/** The GPIO pin to use for RX when using the UART mode */
#define UART_RX_GPIO    (5)

/** The selected baud rate for the application when using UART mode */
#define UART_BAUD       (115200)

/** GPIO pin to use for Blinky, this will be toggled during normal operation */
#define BLINKY_GPIO     (10)

/** The delay in ms between changing the Blinky GPIO state */
#define BLINKY_DELAY_MS (250)

/** Define the set op options we will use when tracing. */
static const uint32_t options[] = {
        SWM_LOG_LEVEL_INFO,             /* In all cases log info messages */
        SWM_UART_RX_PIN | UART_RX_GPIO, /* Set RX pin for cases when using UART */
        SWM_UART_TX_PIN | UART_TX_GPIO, /* Set TX pin for cases when using UART */
        SWM_UART_RX_ENABLE,             /* Enable the UART Rx Interrupts */
        SWM_UART_BAUD_RATE | UART_BAUD  /* Set the baud rate as defined above */
};

/**
 * Standard C main program
 * @return Always zero.
 * @note
 * This will never actually return during normal operation.
 */
int main(void)
{
    bool ledIsOn = false;

    systemInitialize();
    swmTrace_init(options, 5);

#ifdef UART_LOOPBACK_TEST
    SEGGER_RTT_Init();
#endif

    blinkyInitialize(BLINKY_GPIO, BLINKY_DELAY_MS);

    while (true)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();

        /* Wait here until the next interrupt */
        asm("WFI");

        /* check if the LED has changed state */
        if (ledIsOn != isLedOn())
        {
            ledIsOn = ! ledIsOn;
            swmLogInfo("LED : %s\r\n", ledIsOn ? "ON" : "OFF");
        }

        char ch;
        while (swmTrace_getch(&ch))
        {
#ifdef UART_LOOPBACK_TEST
            SEGGER_RTT_PutChar(0, ch);
#endif
        }
    }

    return 0;
}

