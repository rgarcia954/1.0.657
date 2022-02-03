/**
 * @file drv_targ.c
 * @brief The target driver initializes the system HW for the BootLoader.
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
#include <stdint.h>
#include "drv_targ.h"

/* ----------------------------------------------------------------------------
 * Local variables and types
 * --------------------------------------------------------------------------*/

static uint32_t volatile mod_sys_ticks;

/* ----------------------------------------------------------------------------
 * Function prototypes
 * ------------------------------------------------------------------------- */

void SysTick_Handler(void);


/* ----------------------------------------------------------------------------
 * Function      : void Drv_Targ_Init(void)
 * ----------------------------------------------------------------------------
 * Description   : Initializes the basic target HW.
 *                 After the initialization all interrupts are disabled!
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Targ_Init(void)
{
	/* Mask all interrupts */
	__set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

	/* Disable all interrupts and clear any pending interrupts */
	Sys_NVIC_DisableAllInt();
	Sys_NVIC_ClearAllPendingInt();

	SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
	SYS_GPIO_CONFIG(UPDATE_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
	while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
	{
		Drv_Targ_Poll();
	}

	// Start 48 MHz XTAL oscillator
	Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

	// Switch to (divided 48 MHz) oscillator clock, and update the
	// SystemCoreClock global variable.
	Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

	// Configure clock dividers
	Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

	/* Start 1ms system tick */
	mod_sys_ticks = 0;
	SysTick_Config(SystemCoreClock / 1000);

	/* Stop masking interrupts */
	__set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
	__set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
}

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Targ_Poll(void);
 * ----------------------------------------------------------------------------
 * Description   : Polls the target driver.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Targ_Poll(void)
{
	SYS_WATCHDOG_REFRESH();
}

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Targ_Reset(void);
 * ----------------------------------------------------------------------------
 * Description   : Performs a system reset.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Targ_Reset(void)
{
    NVIC_SystemReset();
}

/* ----------------------------------------------------------------------------
 * Function      : uint_fast32_t Drv_Targ_GetTicks(void)
 * ----------------------------------------------------------------------------
 * Description   : Returns the accumulated system ticks.
 * Inputs        : None
 * Outputs       : return value     - number of 1ms ticks
 * Assumptions   :
 * ------------------------------------------------------------------------- */
uint_fast32_t Drv_Targ_GetTicks(void)
{
    return mod_sys_ticks;
}

/* ----------------------------------------------------------------------------
 * Function      : void SysTick_Handler(void);
 * ----------------------------------------------------------------------------
 * Description   : System tick interrupt handler.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void SysTick_Handler(void)
{
    ++mod_sys_ticks;
}

/* ----------------------------------------------------------------------------
 * Function      : bool CheckUpdatePin(void)
 * ----------------------------------------------------------------------------
 * Description   : Checks if Updater activation is demanded by pin.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
bool CheckUpdatePin(void)
{
    /* Check if Updater activation pin is active */
    return ((Sys_GPIO_Read(UPDATE_GPIO)) == 0);
}


