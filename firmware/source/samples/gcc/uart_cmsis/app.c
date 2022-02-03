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
ARM_DRIVER_USART *uart;
DRIVER_GPIO_t *gpio;
char tx_buffer[] __attribute__ ((aligned(4))) = "UART CMSIS TEST";
char rx_buffer[sizeof(tx_buffer)] __attribute__ ((aligned(4)));

void Toggle_Uart_Event_GPIO(uint32_t n, uint32_t delay_ms, bool is_send) {
	for (; n > 0; n--) {
		/* Refresh the watchdog */
		SYS_WATCHDOG_REFRESH();
		if (is_send) {
			/* Toggle APP_UART_SEND_GPIO */
			gpio->ToggleValue(APP_UART_SEND_GPIO);
		} else {
			/* Toggle APP_UART_RECEIVE_GPIO */
			gpio->ToggleValue(APP_UART_RECEIVE_GPIO);
		}
		/* Delay */
		Sys_Delay((delay_ms / 1000.0) * SystemCoreClock);
	}
}

void Button_EventCallback(uint32_t event) {
	static bool ignore_next_dio_int = false;
	if (ignore_next_dio_int) {
		ignore_next_dio_int = false;
	}
	/* Button is pressed: Ignore next interrupt.
	 * This is required to deal with the debounce circuit limitations. */
	else if (event == GPIO_EVENT_0_IRQ) {
		ignore_next_dio_int = true;
		uart->Send(tx_buffer, sizeof(tx_buffer)); /* start transmission */
	}
}

void UART_EventCallBack(uint32_t event)
{
	switch (event) {
	case ARM_USART_EVENT_RECEIVE_COMPLETE: {
		/* If string received matches UART_TX_DATA */
		if (strcmp(tx_buffer, rx_buffer) == 0) {
		    /* Create a 0.5 second pulse, to indicate transmission finished */
		    Toggle_Uart_Event_GPIO(2, 500, false);
		    /* If DMA channel 0 and 1 are used for transfer */
		    if ((DMA[RTE_USART0_RX_DMA_CH_DEFAULT].STATUS & DMA[RTE_USART0_TX_DMA_CH_DEFAULT].STATUS) == DMA_COMPLETE_INT_TRUE)
		    {
			    Toggle_Uart_Event_GPIO(2, 500, false);
		    }
		}

		/* Receive next UART_BUFFER_SIZE bytes */
		uart->Receive(rx_buffer, sizeof tx_buffer);
	}
		break;

	case ARM_USART_EVENT_SEND_COMPLETE: {
		/* Create a 0.5 second pulse, to indicate transmission finished */
		Toggle_Uart_Event_GPIO(2, 500, true);
	}
		break;
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

	/* Initialize usart driver structure */
	uart = &Driver_USART0;

	/* Initialize usart, register callback function */
	uart->Initialize(UART_EventCallBack);


    /* Non-blocking receive. User is notified through UART_EventCallBack
     * after APP_UART_BUFFER_SIZE bytes are received */
	uart->Receive(rx_buffer, (sizeof tx_buffer));

    /* Main application spin loop */
    while (1)
    {
        int8_t i;

        /* Here we demonstrate the ABORT_TRANSFER feature. Wait for first few bytes
         * to be transferred and if particular byte does not match abort the transfer. */
        i = uart->GetRxCount() - 5;
		if ((i >= 0) && (tx_buffer[i] != rx_buffer[i])) {
			/* Abort current receive operation */
			uart->Control(ARM_USART_ABORT_RECEIVE, 0);

			/* Re-start receive operation */
			uart->Receive(rx_buffer, (sizeof tx_buffer));
		}

        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    /* Should never be reached */
    return 0;
}
