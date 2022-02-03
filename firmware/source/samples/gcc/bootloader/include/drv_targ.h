/**
 * @file drv_targ.h
 * @brief Interface to the target driver of the BootLoader.
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
#ifndef _DRV_TARG_H    /* avoids multiple inclusion */
#define _DRV_TARG_H

#include <stdbool.h>
#include <stdint.h>
#include "hw.h"
#include "flash_rom.h"

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

#define RECOVERY_GPIO                   0
#define UPDATE_GPIO                     7

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/* ----------------------------------------------------------------------------
 * Function prototypes
 * ------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Targ_Init(void)
 * ----------------------------------------------------------------------------
 * Description   : Initializes the basic target HW.
 *                 After the initialization all interrupts are disabled!
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Targ_Init(void);

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Targ_Poll(void);
 * ----------------------------------------------------------------------------
 * Description   : Polls the target driver.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Targ_Poll(void);

/* ----------------------------------------------------------------------------
 * Function      : void Drv_Targ_Reset(void);
 * ----------------------------------------------------------------------------
 * Description   : Performs a system reset.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void Drv_Targ_Reset(void);

/* ----------------------------------------------------------------------------
 * Function      : uint_fast32_t Drv_Targ_GetTicks(void)
 * ----------------------------------------------------------------------------
 * Description   : Returns the accumulated system ticks.
 * Inputs        : None
 * Outputs       : return value     - number of 1ms ticks
 * Assumptions   :
 * ------------------------------------------------------------------------- */
uint_fast32_t Drv_Targ_GetTicks(void);

/* ----------------------------------------------------------------------------
 * Function      : bool CheckUpdatePin(void)
 * ----------------------------------------------------------------------------
 * Description   : Checks if Updater activation is demanded by pin.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
bool CheckUpdatePin(void);


#endif    /* _DRV_TARG_H */
