/**
 * @file calibrate_clock.c
 * @brief Clock calibration support functions
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

#include <calibrate.h>
#include <flash.h>

/** @addtogroup CALIBRATELIB
 *  @{
 */

/**
 * @brief        Initializes the Asynchronous Clock Counter and configures it
 *               to measure 16 asynchronous clock periods.
 * @assumptions  Calibrate_Clock_Initialize() has been called.
 */
static void Calibrate_Clock_InitializeASCC(void);

/**
 * @brief       Initialize the RC oscillator.
 */
static void Calibrate_Clock_IntializeRCOSC(void);

/**
 * @brief       Gets the number of cycles in a period of reference
 *              signal using the asynchronous clock counter.
 * @param[in]   timeout  timeout value in SYSCLK cycles, 0 means no timeout.
 * @return      return value Average number of clock cycles over 10 times
 *              16 asynchronous clock periods. Returns 0xFFFFFFFF if a
 *              timeout occurs.
 * @assumptions Calibrate_Clock_Initialize() has been called.
 *              Trigger has been sent to audio sync clock counters.
 */
static unsigned int Calibrate_Clock_GetNumPeriodCycles(uint32_t timeout);

/**
 * @brief       Perform a binary search to set the measured cycle counts
 *              to match a specified target clock frequency.
 * @param[in]   target      The specified target clock cycles
 * @param[in]   max         The setting that gives the maximum
 *                                   measured cycle counts
 * @param[in]   min         The setting that gives the minimum
 *                                   measured cycle counts
 * @param[in]   error       The measurement error allowed for
 *                                   specified target frequency
 * @param[in]   rcosc_sel   The clock being selected for trimming
 * @param[out]  final_results   Final calibration results stored. If there
 *                              is an error, measured error is returned in
 *                              the read_freq field.
 * @return      return value Status value indicating whether the
 *                                   calibration succeeded
 */
static unsigned int Calibrate_Clock_BinarySearch(uint16_t target, uint16_t max,
                                                 uint16_t min, uint32_t error, unsigned int rcosc_sel,
                                                 CalClock_Type *final_results);

/** @} *//* End of the CALIBRATELIB group */

static void Calibrate_Clock_InitializeASCC(void)
{
    ASCC->CTRL = CNT_RESET;
    ASCC->PERIOD_CNT = 0;
    ASCC->PHASE_CNT = 0;
    ASCC->CFG = ASCC_PERIODS_16;
    ASCC->CTRL = PHASE_CNT_START | PERIOD_CNT_START;
}

static void Calibrate_Clock_IntializeRCOSC(void)
{
    /* Start the RC and RC32 oscillator */
    ACS->RCOSC_CTRL = (RC_OSC_ENABLE | RC_OSC_NOM
                       | (ACS_RCOSC_CTRL_RC_FSEL_Mask & ACS->RCOSC_CTRL))
                      | (RC32_OSC_ENABLE | RC32_TEMP_COMP_ENABLE | RC32_OSC_NOM);

#ifndef RSL15_CID
    /* Set standby clock source to 32k internal oscillator, that is what we
     * are calibrating. */
    ACS->RTC_CTRL = RTC_CLK_SRC_RC_OSC | RTC_ENABLE;
#endif    /* ifndef RSL15_CID */
}

static unsigned int Calibrate_Clock_GetNumPeriodCycles(uint32_t timeout)
{
    uint32_t async_clk_period_cnt = 0;

    /* Start activity counter to count cycles */
    if (timeout != 0)
    {
        Sys_ACNT_Clear();
        Sys_ACNT_Start();
    }

    while ((ASCC->CTRL & PERIOD_CNT_BUSY) == PERIOD_CNT_BUSY)
    {
        SYS_WATCHDOG_REFRESH();
        if ((SYSCTRL->SYSCLK_CNT >= timeout) && timeout != 0)
        {
            Sys_ACNT_Stop();
            Sys_ACNT_Clear();
            return 0xFFFFFFFF;
        }
    }

    async_clk_period_cnt = ASCC->PERIOD_CNT;

    ASCC->PERIOD_CNT = 0;

    return (async_clk_period_cnt);
}

static unsigned int Calibrate_Clock_BinarySearch(uint16_t target, uint16_t max,
                                                 uint16_t min, uint32_t error, unsigned int rcosc_sel,
                                                 CalClock_Type *final_results)
{
    volatile uint16_t current_setting = 0;
    uint16_t previous_setting = min;
    uint32_t cycle_count_readback = 0;
    uint32_t cycle_count_readbacks[NON_MONOTONIC_POINTS];
    uint32_t measured_errors[NON_MONOTONIC_POINTS];
    uint32_t measured_error;
    uint32_t monotonic_cycles = 0;
    uint32_t best_fit = 0xFFFFFFFF;
    uint32_t i = 0;

    /* If erroneous endpoints entered, return error code */
    if (max == min)
    {
        return (ERRNO_INVALID_MIN_MAX_ERROR);
    }

    while (max != min)
    {
        /* Reset measured cycle count to 0 */
        for (i = 0; i < monotonic_cycles; i++)
        {
            cycle_count_readbacks[i] = 0;
            measured_errors[i] = 0xFFFFFFFF;
        }
        cycle_count_readback = 0;
        measured_error = 0xFFFFFFFF;
        monotonic_cycles = 0;

        /* Set the middle to half of difference between min and max */
        if ((max == 0) & (min == 1))
        {
            /* Input max/min were reversed, current_setting can not
             * hit 0, must be set to 0 manually when it is intended
             * to try a trim of 0. */
            current_setting = 0;
        }
        else
        {
            current_setting = ((max - min) / 2) + min;
        }

        /* Non-monotonic codes */
        if ((current_setting == NON_MONOTONIC_CODE32)
            || (current_setting == NON_MONOTONIC_CODE48))
        {
            /* Flag current setting as non-monotonic. */
            monotonic_cycles = NON_MONOTONIC_POINTS;
            current_setting--;
        }
        else
        {
            /* Flag current setting as monotonic */
            monotonic_cycles = MONOTONIC_POINTS;
        }

        for (i = 0; i < monotonic_cycles; i++)
        {
            if (rcosc_sel == CAL_32K_RCOSC)
            {
                /* Set RC32 oscillator trim setting to the current calculated setting */
                ACS->RCOSC_CTRL = (ACS->RCOSC_CTRL
                                   & ~(ACS_RCOSC_CTRL_RC32_FTRIM_Mask))
                                  | (current_setting + i);
            }
            else if (rcosc_sel == CAL_START_OSC)
            {
                /* Set RC oscillator trim setting to the current calculated setting,
                 * don't change adjust bit */
                ACS_RCOSC_CTRL->RC_FTRIM_BYTE = (ACS_RCOSC_CTRL->RC_FTRIM_BYTE
                                                 & ~ACS_RCOSC_CTRL_RC_FTRIM_BYTE_Mask)
                                                | (current_setting + i);
            }
            else
            {
                return (ERRNO_GENERAL_FAILURE);
            }

            /* Determine the cycle count for this oscillator configuration*/
            /* Send trigger to asynchronous clock counters */
            Calibrate_Clock_InitializeASCC();

            cycle_count_readbacks[i] = Calibrate_Clock_GetNumPeriodCycles(0);

            /* Exit if the target frequency == cycle count */
            if (target == cycle_count_readbacks[i])
            {
                if (rcosc_sel == CAL_32K_RCOSC)
                {
                    /* Set RC32 oscillator trim setting to the current calculated setting */
                    final_results->trim_setting = (ACS->RCOSC_CTRL
                                                   & ACS_RCOSC_CTRL_RC32_FTRIM_Mask);
                    final_results->read_freq = cycle_count_readbacks[i];
                }
                else if (rcosc_sel == CAL_START_OSC)
                {
                    /* Set RC oscillator trim setting to the current calculated setting */
                    final_results->trim_setting = ACS_RCOSC_CTRL->RC_FTRIM_BYTE
                                                  & ACS_RCOSC_CTRL_RC_FTRIM_BYTE_Mask;
                    final_results->read_freq = cycle_count_readbacks[i];
                }

                /* Calibration successful */
                return (ERRNO_NO_ERROR);
            }

            /* Determine the error in the measurement cycle count.*/
            if (target < cycle_count_readbacks[i])
            {
                measured_errors[i] = (cycle_count_readbacks[i] - target);
            }
            else
            {
                measured_errors[i] = (target - cycle_count_readbacks[i]);
            }

            /* If measured error is lower than previous measured error points,
             * set current measurement point as the optimal, so far. */
            if (measured_errors[i] < measured_error)
            {
                measured_error = measured_errors[i];
                best_fit = i;
                cycle_count_readback = cycle_count_readbacks[i];
            }
        }

        /* Finished measurements, Exit if the current measurement is within allowed
         * measurement error */
        if (measured_error <= error)
        {
            if (monotonic_cycles == NON_MONOTONIC_POINTS)
            {
                /* Ensure we revert back to best found value */
                if (rcosc_sel == CAL_32K_RCOSC)
                {
                    /* Set RC32 oscillator trim setting to the current calculated setting */
                    ACS->RCOSC_CTRL = (ACS->RCOSC_CTRL
                                       & ~(ACS_RCOSC_CTRL_RC32_FTRIM_Mask))
                                      | (current_setting + best_fit);
                }
                else if (rcosc_sel == CAL_START_OSC)
                {
                    /* Set RC oscillator trim setting to the current calculated setting */
                    ACS_RCOSC_CTRL->RC_FTRIM_BYTE =
                        (ACS_RCOSC_CTRL->RC_FTRIM_BYTE
                         & ~ACS_RCOSC_CTRL_RC_FTRIM_BYTE_Mask)
                        | (uint8_t)(current_setting + best_fit);
                }
                else
                {
                    return (ERRNO_GENERAL_FAILURE);
                }
            }

            /* Calibration successful */

            /* Save current trim settings and measured frequencies */
            if (rcosc_sel == CAL_32K_RCOSC)
            {
                final_results->trim_setting = (ACS->RCOSC_CTRL
                                               & ACS_RCOSC_CTRL_RC32_FTRIM_Mask);
                final_results->read_freq = cycle_count_readback;
            }
            else if (rcosc_sel == CAL_START_OSC)
            {
                final_results->trim_setting = ACS_RCOSC_CTRL->RC_FTRIM_BYTE &
                                              ACS_RCOSC_CTRL_RC_FTRIM_BYTE_Mask;
                final_results->read_freq = cycle_count_readback;
            }
            return (ERRNO_NO_ERROR);
        }

        /* Exit if the current setting is the same as the previous setting
         * This means that the search cannot proceed further. The exception to
         * this exit condition is if the previous setting was (max-1)
         * because max cannot actually be reached due integer division. */
        if (previous_setting == current_setting)
        {
            if (current_setting == (max - 1))
            {
                current_setting = max;
            }
            else if (current_setting == 1)
            {
                current_setting = 0;
            }
            else
            {
                /* Calibration not successful */
                final_results->read_freq = measured_error;
                return ERRNO_GENERAL_FAILURE;
            }
        }
        /* If current cycle count is greater than target,
         * set maximum value to middle */
        if (cycle_count_readback > target)
        {
            max = current_setting;
        }
        /* If current cycle count is less than target,
         * set minimum value to middle */
        else if (cycle_count_readback < target)
        {
            min = current_setting;
        }

        /* Set the previous setting to the current setting */
        previous_setting = current_setting;
    }

    /* Calibration not successful */
    final_results->read_freq = measured_error;
    return (ERRNO_GENERAL_FAILURE);
}

void Calibrate_Clock_Initialize(void)
{
    /* Initialize RC Oscillator*/
    Calibrate_Clock_IntializeRCOSC();
}

unsigned int Calibrate_Clock_32K_RCOSC(uint32_t target,
                                       CalClock_Type *final_results)
{
    unsigned int result = 0;
    uint32_t clock_cycles = 0;
    uint32_t error = 0;
    uint32_t measured_error = 0;
    uint32_t xtal32_settings = ACS->XTAL32K_CTRL;

    /* Initialize the max and min settings */
    /* The min and max settings are flipped because the clock frequency is
    * inversely proportional to the trim percentage present in the spec */
    uint16_t max_setting = RC_OSC_M48_BYTE;
    uint16_t min_setting = RC_OSC_P46P5_BYTE;

    /* Disable 32kHz crystal */
    ACS->XTAL32K_CTRL &= ~XTAL32K_ENABLE;

    /* Set standby clock (32kHz) to the ascc async clock source, so it can be
     * measured. */
    GPIO_SRC_ASCC->ASYNC_CLOCK_BYTE = ASCC_ASYNC_CLOCK_SRC_STANDBYCLK_BYTE;

    /* Convert to number of cycles required to achieve the desired
     * calibrated frequency  */
    target = CONVERT_MHZ_TO_CYCLES(target, SystemCoreClock, ASYNC_CLK_PERIODS);

    /* Determine the cycle count for this oscillator configuration */
    Calibrate_Clock_InitializeASCC();

    clock_cycles = Calibrate_Clock_GetNumPeriodCycles(0);

    /* The error is scaled according to the target by multiplying it with the
     * minimum trimming step and dividing that by 2 to achieve the
     * theoretical max precision and multiplied by 1.5 as a safety margin */
    error = (uint32_t)(target * TRIMMING_STEP * 0.5);

    /* Compare the cycle count with the requested cycle count, and
     * exit if they are the equal*/
    if (target < clock_cycles)
    {
        measured_error = (clock_cycles - target);
    }
    else
    {
        measured_error = (target - clock_cycles);
    }

    /* If the current measurement is within error bounds, save current settings */
    if (measured_error <= error)
    {
        final_results->read_freq = clock_cycles;
        final_results->trim_setting = (ACS->RCOSC_CTRL
                                       & ACS_RCOSC_CTRL_RC32_FTRIM_Mask);
    }
    else
    {
        /* Perform Binary Search to find calibration setting */
        result = Calibrate_Clock_BinarySearch(target, max_setting, min_setting,
                                              error,
                                              CAL_32K_RCOSC, final_results);
    }

    if (result & (final_results->read_freq > 0))
    {
        /* Failed calibration. If error is positive (could not get
         * frequency low enough), re-run the binary search after setting the ADJ
         * bit to lower the range. */
        ACS->RCOSC_CTRL |= RC32_OSC_RANGE_M25;

        /* Perform Binary Search to find calibration setting */
        result = Calibrate_Clock_BinarySearch(target, max_setting, min_setting,
                                              error,
                                              CAL_32K_RCOSC, final_results);
        if (result)
        {
            /* Set results from calibration to 0, because the calibration was
             * unsuccessful  */
            final_results->read_freq = 0;
            final_results->trim_setting = 0;
        }
        else
        {
            /* Calibration successful, calculate measured calibrated frequency */
            final_results->read_freq = SystemCoreClock * ASYNC_CLK_PERIODS
                                       / final_results->read_freq;
        }
    }
    else
    {
        /* Calibration successful, calculate measured calibrated frequency */
        final_results->read_freq = SystemCoreClock * ASYNC_CLK_PERIODS
                                   / final_results->read_freq;
    }

    /* Exit with error code if Binary Search failed */
    if (result == ERRNO_GENERAL_FAILURE)
    {
        ACS->XTAL32K_CTRL = xtal32_settings;
        return (ERRNO_RCOSC_CAL_ERROR);
    }

    ACS->XTAL32K_CTRL = xtal32_settings;
    return (ERRNO_NO_ERROR);
}

unsigned int Calibrate_Clock_Start_OSC(uint32_t target,
                                       CalClock_Type *final_results)
{
    unsigned int result = 0;
    uint32_t clock_cycles = 0;
    uint32_t error = 0;
    uint32_t measured_error = 0;
    uint32_t target_cycles = 0;
    uint32_t xtal32_settings = ACS->XTAL32K_CTRL;
    uint16_t sysclk_bk = CLK->SYS_CFG;

    /* Initialize the max and min settings */
    uint16_t max_setting = RC_OSC_P46P5 >> ACS_RCOSC_CTRL_RC_FTRIM_Pos;
    uint16_t min_setting = RC_OSC_M48 >> ACS_RCOSC_CTRL_RC_FTRIM_Pos;

    /* Use 32kHz XTAL for greatest accuracy */
    ACS->XTAL32K_CTRL = XTAL32K_XIN_CAP_BYPASS_DISABLE |
                        XTAL32K_NOT_FORCE_READY |
                        XTAL32K_CTRIM_8P8PF |
                        XTAL32K_ITRIM_160NA |
                        XTAL32K_ENABLE |
                        XTAL32K_AMPL_CTRL_ENABLE;

    /* Wait until XTAL32 is ready */
    while (!(ACS->XTAL32K_CTRL & XTAL32K_OK));

    /* Set standby clock (32kHz) to the ascc async clock source, so it can be
     * measured. */
    GPIO_SRC_ASCC->ASYNC_CLOCK_BYTE = ASCC_ASYNC_CLOCK_SRC_STANDBYCLK_BYTE;

    /* Save current SYSCLK configuration */

    /* Switch to start oscillator clock */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK_BYTE);

    /* Determine the cycle count for this oscillator configuration */
    Calibrate_Clock_InitializeASCC();

    clock_cycles = Calibrate_Clock_GetNumPeriodCycles(0);

    ACS_RCOSC_CTRL->RC_FTRIM_BYTE = RC_OSC_NOM_BYTE;

    /* Convert target from kHz to Hz */
    target *= 1000;

    /* Convert to number of cycles required to achieve the desired
     * calibrated frequency.*/
    target_cycles = (target * ASYNC_CLK_PERIODS) / CAL_RC32OSC_DEFAULT;

    /* Determine the cycle count for this oscillator configuration */
    Calibrate_Clock_InitializeASCC();

    clock_cycles = Calibrate_Clock_GetNumPeriodCycles(0);

    /* The 0.75 is calculated by dividing the trim step by 2, which gives the
     * max possible accuracy, and multiplying by 1.5 as a safety margin */
    error = (uint32_t)(target_cycles * TRIMMING_STEP * 0.75);

    /* Compare the cycle count with the requested cycle count, and
     * exit if they are the equal*/
    if (target_cycles < clock_cycles)
    {
        measured_error = (clock_cycles - target_cycles);
    }
    else
    {
        measured_error = (target_cycles - clock_cycles);
    }

    /* If the current measurement is within error bounds, save current settings */
    if (measured_error <= error)
    {
        final_results->read_freq = clock_cycles;
        final_results->trim_setting = (ACS->RCOSC_CTRL
                                       & ACS_RCOSC_CTRL_RC32_FTRIM_Mask);
    }
    else
    {
        /* Perform Binary Search to find calibration setting */
        result = Calibrate_Clock_BinarySearch(target_cycles, max_setting,
                                              min_setting, error,
                                              CAL_START_OSC, final_results);
    }

    if (result & (final_results->read_freq > 0))
    {
        /* Failed calibration. If error is positive (could not get
         * frequency low enough), re-run the binary search after setting the ADJ
         * bit to lower the range. */
        ACS->RCOSC_CTRL |= RC_OSC_RANGE_M15;

        /* Perform Binary Search to find calibration setting */
        result = Calibrate_Clock_BinarySearch(target_cycles, max_setting,
                                              min_setting, error,
                                              CAL_START_OSC, final_results);
        if (result)
        {
            /* Set results from calibration to 0, because the calibration was
             * unsuccessful  */
            final_results->read_freq = 0;
            final_results->trim_setting = 0;
        }
        else
        {
            /* Calibration successful, calculate measured calibrated frequency */
            final_results->read_freq = (final_results->read_freq
                                        * CAL_RC32OSC_DEFAULT) /
                                       ASYNC_CLK_PERIODS;
        }
    }
    else
    {
        /* Calibration successful, calculate measured calibrated frequency */
        /* Convert to measured Standby-clock frequency */
        final_results->read_freq = (final_results->read_freq
                                    * CAL_RC32OSC_DEFAULT) /
                                   ASYNC_CLK_PERIODS;
    }

    /* Exit with error code if binary search failed */
    if (result == ERRNO_GENERAL_FAILURE)
    {
        ACS->XTAL32K_CTRL = xtal32_settings;
        Sys_Clocks_SystemClkConfig(sysclk_bk);
        return (ERRNO_START_OSC_CAL_ERROR);
    }

    ACS->XTAL32K_CTRL = xtal32_settings;
    Sys_Clocks_SystemClkConfig(sysclk_bk);
    return (ERRNO_NO_ERROR);
}

uint32_t Calibrate_Clock_CheckXTAL(uint32_t xtal, uint32_t gpio)
{
    uint32_t clock_cycles = 0;
    uint32_t clock_ok = 1;
    uint32_t timeout = 0;

    if (xtal == XTAL_48MHZ)
    {
        /* Enable RFCLK at 8 MHz, output it on GPIO specified */
        Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

        GPIO->CFG[gpio] = GPIO_MODE_RFCLK;

        /* Set timeout duration */
        timeout = XTAL48_ERROR_LIMIT_MAX * 2;
    }
    else if (xtal == XTAL_32KHZ)
    {
        /* Enable the 32 kHz XTAL */
        ACS->XTAL32K_CTRL = XTAL32K_XIN_CAP_BYPASS_DISABLE |
                            XTAL32K_NOT_FORCE_READY |
                            XTAL32K_CTRIM_8P8PF |
                            XTAL32K_ITRIM_160NA |
                            XTAL32K_ENABLE |
                            XTAL32K_AMPL_CTRL_ENABLE;

        /* Wait until XTAL32 is ready */
        while (!(ACS->XTAL32K_CTRL & XTAL32K_OK));

        /* Output the 32 kHz crystal on the GPIO specified */
        GPIO->CFG[gpio] = GPIO_MODE_STANDBYCLK;

        /* Set timeout duration */
        timeout = XTAL32_ERROR_LIMIT_MAX * 2;
    }

    /* Set start oscillator to 24MHz multiplier */
    ACS->RCOSC_CTRL &= ~ACS_RCOSC_CTRL_RC_FSEL_Mask;
    ACS->RCOSC_CTRL |= RC_OSC_24MHZ;
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);

    /* Set ASYNC_CLOCK source to the XTAL via the GPIO selected */
    GPIO_SRC_ASCC->ASYNC_CLOCK_BYTE = (uint8_t)(gpio
                                                << GPIO_SRC_ASCC_ASYNC_CLOCK_BYTE_Pos);

    /* Start the ASCC */
    Calibrate_Clock_InitializeASCC();

    /* Get the clock cycle count from the ASCC */
    clock_cycles = Calibrate_Clock_GetNumPeriodCycles(timeout);

    if (xtal == XTAL_48MHZ &&
        clock_cycles <= XTAL48_ERROR_LIMIT_MAX &&
        clock_cycles >= XTAL48_ERROR_LIMIT_MIN)
    {
        /* The 48 MHz crystal is active. */
        clock_ok = 0;
    }
    else if (xtal == XTAL_32KHZ &&
             clock_cycles <= XTAL32_ERROR_LIMIT_MAX &&
             clock_cycles >= XTAL32_ERROR_LIMIT_MIN)
    {
        /* The 32 kHz crystal is active. */
        clock_ok = 0;
    }

    return clock_ok;
}
