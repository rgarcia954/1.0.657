/**
 * @file watchdog.h
 * @brief Hardware abstraction layer for watchdog timers
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

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALWATCHDOG WATCHDOG
 *  WATCHDOG Hardware Abstraction Layer
 *  @{
 */

/**
 * @brief       Refresh the chip and software watchdog timers
 */
#define SYS_WATCHDOG_REFRESH() WATCHDOG->CTRL = WATCHDOG_REFRESH; \
    ACS->SOC_WATCHDOG_CTRL = SOC_WATCHDOG_REFRESH

/**
 * @brief       Refresh the software watchdog timer count
 */
#define SYS_WATCHDOG_SOFTWAREREFRESH() WATCHDOG->CTRL = WATCHDOG_REFRESH

/**
 * @brief       Refresh the chip watchdog timer count
 */
#define SYS_WATCHDOG_CHIPREFRESH() ACS->SOC_WATCHDOG_CTRL = SOC_WATCHDOG_REFRESH

/** @} */ /* End of the HALWATCHDOG group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* WATCHDOG_H_ */
