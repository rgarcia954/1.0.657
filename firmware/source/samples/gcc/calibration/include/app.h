/**
 * @file app.h
 * @brief Sample application for the calibration library.
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
#ifndef INCLUDE_APP_H_
#define INCLUDE_APP_H_

#include <hw.h>
#include <calibrate.h>
#include "flash_rom.h"
#include <trim.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#define LSAD_CALIB_CHANNEL              6

/* GPIO definitions */
#define RECOVERY_GPIO                    0
#define AOUT_GPIO                        1

/* New calibration targets */
#define TARGET_DCDC_1_25V               125     /* Target for 1.25 V for DCDC converter */
#define TARGET_VDDRF_1_15V              115     /* Target for 1.15 V for VDDRF regulator */
#define TARGET_VDDC_1_1V                110     /* Target for 1.1 V for VDDC regulator */
#define TARGET_VDDM_1_15V               115     /* Target for 1.15 V for VDDM regulator */
#define TARGET_VDDPA_1_64V              164     /* Target for 1.64 V for VDDPA regulator */
#define TARGET_VDDIF_1_9V               190     /* Target for 1.9V for VDDIF regulator */

#define TARGET_40KHZ                    40000    /* Calibrate standby clock to 40kHz */
#define TARGET_4MHZ                     4000    /* Calibrate RC clock to 4MHz */

#define AOUT_ENABLE_DELAY               SystemCoreClock / 100    /* delay set to 10ms */

#define RECORD_SIZE                     4       /* Number of entries for each
                                                 * trimmable component */

#define ERASE_OK                        1       /* Is it ok to erase NVR4 if necessary? */
                                                /* 1 = yes, 0 = no */

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/**
 * @brief       Store the newly calibrated trim values in flash.
 * @param[in]   new_trims         Pointer to newly calibrated trim values.
 * @return      none
 */
void StoreTrimsInFlash(TRIM_Type *new_trims);

/**
 * @brief       Combine the two trim structure, with the new structure
 *              taking precedence.
 * @param[in]   new_trims         Pointer to newly calibrated trim values.
 * @param[in]   old_trims         Pointer to trim values present in flash.
 * @return      none
 */
uint32_t combineTrims(TRIM_Type *new_trims, TRIM_Type *old_trims);

#endif    /* INCLUDE_APP_H_ */
