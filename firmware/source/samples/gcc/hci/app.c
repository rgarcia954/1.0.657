/**
 * @file main.c
 * @copyright @parblock
 *
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

#include <app.h>

#ifdef CFG_PLF_UART
#include <uart_hci.h>
#endif    /* ifdef CFG_PLF_UART */

DRIVER_GPIO_t *gpio;

struct ble_sleep_api_param_tag ble_sleep_api_param =
{
    .app_sleep_request = 0,
    .max_sleep_duration = 10000,
};

void BB_Sleep(void);

void IRQPriorityInit(void)
{
	uint8_t interrupt;

	/* Iterate through all external interrupts excluding WAKEUP_IRQn */
	for (interrupt = RTC_ALARM_IRQn; interrupt <= NVIC_LAST_VECTOR; ++interrupt)
	{
		/* If the interrupt is non-BLE, set priority to 1 (lower than
		 * the default priority of 0). This ensures BLE stability. */
		if (interrupt < BLE_SW_IRQn || interrupt > BLE_ERROR_IRQn)
		{
			NVIC_SetPriority(interrupt, 1);
		}
	}
}

int main()
{
    Sys_NVIC_DisableAllInt();    /* Disable all interrupts */
    Sys_NVIC_ClearAllPendingInt();    /* Clear all pending interrupts */

    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while ((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

    /* Load default trim values. */
    uint32_t trim_error __attribute__ ((unused)) = SYS_TRIM_LOAD_DEFAULT();

    /* Start 48 MHz XTAL oscillator */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Set ICH_TRIM for optimum RF performance */
    ACS->VCC_CTRL = (((ACS->VCC_CTRL) & (~(ACS_VCC_CTRL_ICH_TRIM_Mask))) |
            	     ((uint32_t)(0x5U << ACS_VCC_CTRL_ICH_TRIM_Pos)));

    /* Enable/disable buck converter */
    ACS->VCC_CTRL = (((ACS->VCC_CTRL) & (~(VCC_BUCK))) | (VCC_BUCK_LDO_CTRL));

	/* Load custom trim values */
    uint32_t custom_trim_error __attribute__((unused)) = SYS_TRIM_LOAD_CUSTOM();

    Sys_RFFE_SetTXPower(OUTPUT_POWER_DBM, LSAD_TXPWR_DEF, VDDPA_EN);

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Configure Baseband Controller Interface */
    BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8);

    /* Enable Timer to refresh the watchdog */
    Sys_Timer_Config(TIMER0, TIMER_PRESCALE_1, TIMER_FREE_RUN, 0xFFF);
    Sys_Timer_Start(TIMER0);
    NVIC_EnableIRQ(TIMER0_IRQn);

    /* Initialize GPIO structure */
	gpio = &Driver_GPIO;
	gpio->Initialize(NULL);

    SYS_GPIO_CONFIG(ANT_SW0_GPIO, (GPIO_6X_DRIVE  | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_MODE_BB_ANT_SW_0));
    SYS_GPIO_CONFIG(ANT_SW1_GPIO, (GPIO_6X_DRIVE  | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_MODE_BB_ANT_SW_1));

    /*------------ BLE initialization ----------------- */

    /* Initialize UART component */
    uart_init(115200, UART_TX_GPIO, UART_RX_GPIO);

    uint8_t param_ptr;

    BLE_Initialize(&param_ptr);

    /* BLE_Initialize() initialized a number of trim registers
     * using default values from in the BLE stack,
     * SYS_TRIM_LOAD_CUSTOM() ensures custom trim values are used. */
	custom_trim_error = SYS_TRIM_LOAD_CUSTOM();

	IRQPriorityInit();

    NVIC_ClearPendingIRQ(BLE_HSLOT_IRQn);
    NVIC_ClearPendingIRQ(BLE_SLP_IRQn);
    NVIC_ClearPendingIRQ(BLE_FIFO_IRQn);
    NVIC_ClearPendingIRQ(BLE_CRYPT_IRQn);
    NVIC_ClearPendingIRQ(BLE_ERROR_IRQn);
    NVIC_ClearPendingIRQ(BLE_TIMESTAMP_TGT1_IRQn);
    NVIC_ClearPendingIRQ(BLE_FINETGT_IRQn);
    NVIC_ClearPendingIRQ(BLE_TIMESTAMP_TGT2_IRQn);
    NVIC_ClearPendingIRQ(BLE_SW_IRQn);

    NVIC_EnableIRQ(BLE_HSLOT_IRQn);
    NVIC_EnableIRQ(BLE_SLP_IRQn);
    NVIC_EnableIRQ(BLE_FIFO_IRQn);
    NVIC_EnableIRQ(BLE_CRYPT_IRQn);
    NVIC_EnableIRQ(BLE_ERROR_IRQn);
    NVIC_EnableIRQ(BLE_TIMESTAMP_TGT1_IRQn);
    NVIC_EnableIRQ(BLE_FINETGT_IRQn);
    NVIC_EnableIRQ(BLE_TIMESTAMP_TGT2_IRQn);
    NVIC_EnableIRQ(BLE_SW_IRQn);

    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);

    while (1)
    {
        SYS_WATCHDOG_REFRESH();
        BLE_Kernel_Process();
        __WFI();
    }
}

/*-------------------------------------- */
void TIMER0_IRQHandler(void)
{
    SYS_WATCHDOG_REFRESH();
}

void BB_Sleep(void)
{
}
