/**
 * @file app.c
 * @brief Sample application for the HardFault Handler.
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

/* Used for semi-hosting */
extern void initialise_monitor_handles(void);

/**
 * @brief         Perform the following steps:
 *                 - Initialize the system
 *                 - Give the user the option to switch to any fault executor
 *                 - Execute that fault, which will trigger the hardfault handler
 *
 * @param[in]     None
 * @param[out]    None
 * @assumptions   None
 */
int main(void)
{
    /* Initialize the system clock to a known clock rate. */
    App_Clock_Config();

    /* Initialize semi-hosting */
    initialise_monitor_handles();
    printf("Semi-hosting initialized\n");

    volatile enum
    {
        NO_FAULT,
        USAGE_FAULT,
        BUS_FAULT,
        MEM_FAULT,
    } fault = USAGE_FAULT;

    /* Re-enable fault interrupts */
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);

    switch (fault)
    {
    case USAGE_FAULT:
        exec_usage_fault();
        break;
    case BUS_FAULT:
        exec_bus_fault();
        break;
    case MEM_FAULT:
        exec_mem_fault();
        break;
    default:
        break;
    }

    /* Spin loop */
    while (1)
    {
        /* Refresh system watchdog timer */
        SYS_WATCHDOG_REFRESH();

    }
}

void App_Clock_Config(void)
{
    /* Disable all interrupts and clear any pending interrupts */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Test GPIO0 to pause the program to make it easy to re-flash */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

	/* Disable JTAG TDI, TDO, and TRST connections to GPIO 2, 3, and 4 */
    GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);
	
    /* Load default trim values; SystemCoreClock will be updated below */
	uint32_t trim_error __attribute__ ((unused)) = SYS_TRIM_LOAD_DEFAULT();

    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);
}

void exec_usage_fault(void)
{
    /* enable trapping divide-by-zero */
    SCB->CCR |= (1 << SCB_CCR_DIV_0_TRP_Pos);
    int x = 0;
    int y = 1;
    int z = y / x;
    (void) z;
}

void exec_bus_fault(void)
{
    uint32_t *n = (uint32_t*) 0;
    *n = 7;
}

void exec_mem_fault(void)
{
    /* This should trigger a mem manage fault and set the IACCVIOL bit */
    void (*fn)(void) = (void (*)(void)) 0xcafebabe;
    fn();
}
