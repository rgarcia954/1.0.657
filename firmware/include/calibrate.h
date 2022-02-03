/**
 * @file calibrate.h
 * @brief Calibration support header
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

#ifndef CALIBRATE_H
#define CALIBRATE_H

/** @defgroup CALIBRATELIB Supplemental Calibration Library Reference
 *  @brief The supplemental calibration library is used to calibrate each voltage
 *  rail or RC oscillator to the desired level or frequency, respectively.
 *  @{ */

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <hw.h>
#include <trim.h>

/* Internal include files */
#include <calibrate_power.h>
#include <calibrate_clock.h>
#ifdef RSL15_CID
#include <calibrate_saradc.h>
#endif /* ifdef RSL15_CID */

/** Major version of Calibration Library */
#define CALIBRATE_FW_VER_MAJOR          0x02

/** Minor version of Calibration Library */
#define CALIBRATE_FW_VER_MINOR          0x01

/** Revision of Calibration Library */
#define CALIBRATE_FW_VER_REVISION       0x00

/** Firmware Calibration Library Version Code */
#define CALIBRATE_FW_VER                ((CALIBRATE_FW_VER_MAJOR << 12) | \
                                         (CALIBRATE_FW_VER_MINOR << 8)  | \
                                         CALIBRATE_FW_VER_REVISION)

/** Calibrate version variable */
extern const short CalibrateLib_Version;

/* Definitions for possible calibration library error conditions */
/** Power supply calibration related errors marker */
#define ERRNO_POWER_CAL_MARKER          0x10

/** Error during bandgap calibration process */
#define ERRNO_VBG_CAL_ERROR             (0x0001 | ERRNO_POWER_CAL_MARKER)

/** Error during VDDRF calibration process */
#define ERRNO_VDDRF_CAL_ERROR           (0x0002 | ERRNO_POWER_CAL_MARKER)

/** Error during VDDPA calibration process */
#define ERRNO_VDDPA_CAL_ERROR           (0x0003 | ERRNO_POWER_CAL_MARKER)

/** Error during DC-DC calibration process */
#define ERRNO_DCDC_CAL_ERROR            (0x0004 | ERRNO_POWER_CAL_MARKER)

/** Error during VDDC calibration process */
#define ERRNO_VDDC_CAL_ERROR            (0x0005 | ERRNO_POWER_CAL_MARKER)

/** Error during VDDM calibration process */
#define ERRNO_VDDM_CAL_ERROR            (0x0006 | ERRNO_POWER_CAL_MARKER)

/** Error during VDDIF calibration process */
#define ERRNO_VDDIF_CAL_ERROR           (0x0007 | ERRNO_POWER_CAL_MARKER)

/** Error during VDDFLASH calibration process */
#define ERRNO_VDDFLASH_CAL_ERROR        (0x0008 | ERRNO_POWER_CAL_MARKER)

/** Error during storage calibration process */
#define ERRNO_STORAGE_CAL_ERROR         (0x0009 | ERRNO_POWER_CAL_MARKER)

/** Clock calibration related errors marker */
#define ERRNO_CLK_CAL_MARKER            0x20

/** Error during RC 32 kHz oscillator calibration process */
#define ERRNO_RCOSC_CAL_ERROR           (0x0001 | ERRNO_CLK_CAL_MARKER)

/** Error during RC Start oscillator calibration process */
#define ERRNO_START_OSC_CAL_ERROR       (0x0002 | ERRNO_CLK_CAL_MARKER)

/** Invalid input to binary search algorithm */
#define ERRNO_INVALID_MIN_MAX_ERROR     (0x0003 | ERRNO_CLK_CAL_MARKER)

/** @} */ /* End of the CALIBRATELIB group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* CALIBRATE_H */
