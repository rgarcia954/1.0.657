/**
 * @file calibrate_clock.h
 * @brief Calibration support functions
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

#ifndef CALIBRATE_CLOCK_H
#define CALIBRATE_CLOCK_H

/** @addtogroup CALIBRATELIB
 *  @{
 */

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** Size of trimming steps for RC oscillator, 1.5% */
#define TRIMMING_STEP                   0.015

/** Number of asynchronous clock periods to measure over */
#define ASYNC_CLK_PERIODS               16

/** Select to calibrate the 32 kHz RC oscillator */
#define CAL_32K_RCOSC                   1

/** Select to calibrate the Start RC oscillator */
#define CAL_START_OSC                   2

/** RC32 OSC default target frequency */
#define CAL_RC32OSC_DEFAULT             32768U

/** Conversion factor between SMHz to Hz or vice versa */
#define MHZ_TO_HZ                       1000000U

/** Conversion factor between MHz to Hz or vice versa */
#define TEN_MS_QUOTIENT                 100U

/** Number of points to check when encountering non-monotonic code */
#define NON_MONOTONIC_POINTS            3

/** Number of points to check when encountering monotonic code */
#define MONOTONIC_POINTS                1

/** Number of points to check when encountering monotonic code */
#define NON_MONOTONIC_CODE32            32

/** Number of points to check when encountering monotonic code */
#define NON_MONOTONIC_CODE48            48

/* Error allowable when measuring 48 MHz XTAL, when divided to 8 MHz,
 *  measured with 24 MHz RCCLK. Error is generous since RCCLK is un-calibrated
 *  and is expected to work across temperature.
 * Untrimmed RC 24 range: 14 - 40 MHz /
 * Temperature coefficient: -40 - 85 C: +/- 25%
 * Therefore RCCLK range: 10.5 - 50 MHz */

/** Minimum frequency of 24 MHz oscillator setting worst case */
#define MIN_RCCLK_24                    10500000UL

/** Maximum frequency of 24 MHz oscillator setting worst case */
#define MAX_RCCLK_24                    50000000UL

/** RFCLK frequency measured */
#define RFCLK_FREQ                      8000000UL

/** MIN = MIN RCCLK / 8 MHz * 16 ASCC periods */
#define XTAL48_ERROR_LIMIT_MIN          MIN_RCCLK_24 / RFCLK_FREQ * ASYNC_CLK_PERIODS

/** MAX = MAX RCCLK / 8 MHz * 16 ASCC periods */
#define XTAL48_ERROR_LIMIT_MAX          MAX_RCCLK_24 / RFCLK_FREQ * ASYNC_CLK_PERIODS

/** RFCLK frequency measured */
#define STANDBYCLK_FREQ                 32768U

/** MIN = MIN RCCLK / 32768 Hz * 16 ASCC periods */
#define XTAL32_ERROR_LIMIT_MIN          MIN_RCCLK_24 / STANDBYCLK_FREQ * ASYNC_CLK_PERIODS

/** MAX = MAX RCCLK / 32768 Hz * 16 ASCC periods */
#define XTAL32_ERROR_LIMIT_MAX          MAX_RCCLK_24 / STANDBYCLK_FREQ * ASYNC_CLK_PERIODS

/** Enumeration for selecting to check either the 48 MHz crystal
 *  or the 32 kHz crystal with Calibrate_Clock_CheckXTAL */
enum clock_check
{
    XTAL_48MHZ,
    XTAL_32KHZ
};

/**
 * @def         CONVERT_MHZ_TO_CYCLES(x, y, z)
 * @brief       Calculates the number of cycles to be returned by
 *              the ASCC. The ASCC returns the number of SYSCLK cycles
 *              between n number of periods in the input clock.
 * @param[in]     x  The value in MHz that must be converted to a cycle count
 * @param[in]     y  The frequency of the SYSCLK (Hz)
 * @param[in]     z  The number of periods that the ASCC measures.
 * @return      Number of system clock cycles between n periods of the input
 *              clock.
 * @examplecode calibrate_examples.c CONVERT_MHZ_TO_CYCLES_Example
 */
#define CONVERT_MHZ_TO_CYCLES(x, y, z)  (((y) * (z)) / (x))

/**
 * @brief       Contains the quantitative results of the calibration,
 *              returned from each clock calibration function.
 */
typedef struct
{
    uint32_t trim_setting;              /**< The final trim setting in the
                                         *   relevant register. */
    uint32_t read_freq;                 /**< Last measured frequency at the
                                         *   current final trim setting.
                                         *   Value is in Hz. */
} CalClock_Type;

/**
 * @brief       Initialize the system to support the clock calibration,
 *              consisting of the 48 MHz XTAL oscillator and RC oscillator.
 * @examplecode calibrate_examples.c Calibrate_Clock_Initialize_Example
 */
void Calibrate_Clock_Initialize(void);

/**
 * @brief       Used to calibrate the 32K RC oscillator to a specified frequency.
 * @param[in]     target    Number of cycles required to achieve the desired clock
 *                          frequency in Hz
 * @param[in]     final_results  Final trim results
 * @return      status code indicating whether the RCOSC calibration has succeeded.
 * @assumptions Calibrate_Clock_Initialize() has been called.
 * @examplecode calibrate_examples.c Calibrate_Clock_32K_RCOSC_Example
 */
unsigned int Calibrate_Clock_32K_RCOSC(uint32_t target, CalClock_Type *final_results);

/**
 * @brief       Used to calibrate the startup oscillator to a specified frequency.
 * @param[in]     target desired clock frequency in kHz
 * @param[in]     final_results  Final trim results
 * @return      Status code indicating whether the clock calibration has succeeded.
 * @assumptions Calibrate_Clock_Initialize() has been called.
 * @assumptions Standby clock (XTAL32) has been calibrated as close to 32768Hz as possible.
 * @assumptions Sets SYSCLK to the RCCLK, not recommended to use while Blue
 *              Low Energy is active.
 * @examplecode calibrate_examples.c Calibrate_Clock_Start_OSC_Example
 */
unsigned int Calibrate_Clock_Start_OSC(uint32_t target, CalClock_Type *final_results);

/**
 * @brief       Used to determine if the specified crystal can oscillate correctly.
 * @param[in]     xtal  The desired crystal to be checked, use XTAL_[48M | 32K]HZ
 * @param[in]     gpio  GPIO that the selected clock will be output on
 * @return      Status code indicating whether the selected crystal is oscillating
 *              correctly. 1 indicates success, 0 indicates failure.
 * @assumptions Sets SYSCLK to the RCCLK, not recommended to use while Blue
 *              Low Energy is active.
 * @examplecode calibrate_examples.c Calibrate_Clock_CheckXTAL_Example
 */
uint32_t Calibrate_Clock_CheckXTAL(uint32_t xtal, uint32_t gpio);

/** @} */ /* End of the CALIBRATELIB group */

#endif    /* ifndef CALIBRATE_CLOCK_H */
