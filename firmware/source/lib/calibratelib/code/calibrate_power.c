/**
 * @file calibrate_power.c
 * @brief Power calibration support functions
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
#include <trim.h>

/** @addtogroup CALIBRATELIB
 *  @{
 */

/**
 * @brief       Checks calibration results for errors. If no errors,
 *              return the results in the entered variable.
 * @param[in]   caL_results   Location to store calibration results.
 * @param[in]   error   Pass/Fail of calibration
 * @param[in]   reg     Address of register where trim setting is stored.s
 * @return      Returns the error code.
 */
static uint32_t Calibrate_Power_StoreResult(CalPower_Type *trim_results,
                                            uint32_t error,
                                            uint32_t *final_voltage,
                                            uint8_t *reg);

/**
 * @brief       Determines the median of 3 supply measurements
 * @param[in]   a   Supply  measurement #1
 * @param[in]   b   Supply  measurement #2
 * @param[in]   c   Supply  measurement #3
 * @return      Returns the median value
 */
static uint32_t Calibrate_Power_GetMedian(uint32_t a, uint32_t b, uint32_t c);

/**
 * @brief       Measure a supply voltage; returns the median measurement of
 *              3 measurements to ensure that we're rejecting sampling noise
 * @param[in]     *adc_ptr    Pointer to ADC data register
 * @return      Median measurement from the ADC data register
 * @assumptions  Calibrate_Power_Initialize() has been called.
 */
static uint32_t Calibrate_Power_MeasureSupply(const volatile uint32_t *adc_ptr);

/**
 * @brief       Perform a binary search to set the data read from an ADC
 *              to match a specified target voltage.
 * @param[in]     target The specified target voltage [mV]
 * @param[in]     max         The setting that gives the maximum measured output
 * @param[in]     min         The setting that gives the minimum measured output
 * @param[in]     adc_ptr     Pointer to the ADC data register
 * @param[in]     supply_ptr  Pointer to the power supply control register
 * @return      Status value indicating whether the calibration succeeded
 */
static unsigned int Calibrate_Power_BinarySearch(uint32_t target,
                                                 uint8_t max,
                                                 uint8_t min,
                                                 const volatile uint32_t *adc_ptr,
                                                 uint8_t *supply_ptr,
                                                 uint32_t *final_voltage);

/** @} */ /* End of the CALIBRATELIB group */

static uint32_t Calibrate_Power_StoreResult(CalPower_Type *trim_results,
                                            uint32_t error,
                                            uint32_t *final_voltage,
                                            uint8_t *reg)
{
    /* Exit with error code if Binary Search failed */
    if (error == ERRNO_GENERAL_FAILURE)
    {
        return (ERRNO_POWER_CAL_MARKER);
        trim_results->read_voltage = 0;
        trim_results->trim_setting = 0;
    }
    else
    {
        /*Otherwise, return trim and voltage from calibration */
        trim_results->read_voltage = *final_voltage;
        trim_results->trim_setting = *reg;
    }
    return ERROR_NO_ERROR;
}

static uint32_t Calibrate_Power_GetMedian(uint32_t a, uint32_t b, uint32_t c)
{
    uint32_t temp = 0;

    /* Put smallest in a, median in b, and max in c */
    if (a > b)
    {
        /* Swap a and b */
        SWAP(a, b, temp);
    }

    if (b > c)
    {
        /* Swap b and c */
        SWAP(b, c, temp);

        if (a > b)
        {
            /* Swap a and b */
            SWAP(a, b, temp);
        }
    }

    /* b always contains the median value */
    return (b);
}

static uint32_t Calibrate_Power_MeasureSupply(const volatile uint32_t *adc_ptr)
{
    uint32_t supply1 = 0;
    uint32_t supply2 = 0;
    uint32_t supply3 = 0;

    float offset_error = 0.0f;
    float gain_error = 1.0f;

    TRIM_Type *trim = TRIM;

    float median_supply = 0;

    if (trim->lsad_trim.lf_offset != 0xFFFF)
    {
        offset_error = (float)((int16_t)trim->lsad_trim.lf_offset) / LSAD_OFFSET_ERROR_CONV_QUOTIENT;
    }
    if ((trim->lsad_trim.lf_gain & 0x3FFFF) != 0x3FFFF)
    {
        gain_error = (float)(trim->lsad_trim.lf_gain & 0x3FFFF) / LSAD_GAIN_ERROR_CONV_QUOTIENT;
    }

    /* Due to variable and often low loads on the regulators, some regulators can take a long
     * time (~150ms) to discharge properly. We must wait until the regulator has discharged
     * appropriately before settling on a measurement. Therefore, we check the difference in
     * LSBs from the first to the last reading to see if the supply is still discharging  */

    /* Wait for a conversion to finish, then grab data and reset interrupt status.
     * Each conversion should be spaced by 1.6ms*8 (625Hz/channel) @ SLOWCLK = 1 MHz*/
    do
    {
        /* Wait for a conversion to finish */
        LSAD->MONITOR_STATUS = LSAD_READY_CLEAR;
        while (!(LSAD->MONITOR_STATUS & LSAD_READY_TRUE));
        supply1 = *adc_ptr;

        /* Wait for a conversion to finish */
        LSAD->MONITOR_STATUS = LSAD_READY_CLEAR;
        while (!(LSAD->MONITOR_STATUS & LSAD_READY_TRUE));
        supply2 = *adc_ptr;

        /* Wait for a conversion to finish */
        LSAD->MONITOR_STATUS = LSAD_READY_CLEAR;
        while (!(LSAD->MONITOR_STATUS & LSAD_READY_TRUE));
        supply3 = *adc_ptr;
    }
    while (abs((int32_t)(supply1) - (int32_t)(supply3)) > LSAD_STABILIZED_RANGE);

    /* Return median of the 3 measurements */
    median_supply = (float)CONVERT(Calibrate_Power_GetMedian(supply1, supply2, supply3)) / V_TO_MV_F;

    /* Multiply by 1000 to convert back to mV */
    return (uint32_t)((median_supply - offset_error) / gain_error * V_TO_MV);
}

static unsigned int Calibrate_Power_BinarySearch(uint32_t target,
                                                 uint8_t max,
                                                 uint8_t min,
                                                 const volatile uint32_t *adc_ptr,
                                                 uint8_t *supply_ptr,
                                                 uint32_t *final_voltage)
{
    uint8_t current_setting = 0;
    volatile uint32_t current_readback_voltage = 0;
    uint8_t previous_setting = min;
    uint32_t error = 0;

    uint32_t allowed_error;

#ifndef RSL15_CID
    if ((supply_ptr == (uint8_t *)&ACS_VDDIF_CTRL->VTRIM_BYTE) ||
        (supply_ptr == (uint8_t *)&ACS_VDDFLASH_CTRL->VTRIM_BYTE))
    {
#else
    if (supply_ptr == (uint8_t *)&ACS_VDDFLASH_CTRL->VTRIM_BYTE)
    {
#endif

        /* Calibrating flash, larger margin of error */
        allowed_error = LSAD_IF_MEASUREMENT_ERROR;
    }
    else
    {
        /* Use general margin of error otherwise */
        allowed_error = LSAD_MEASUREMENT_ERROR;
    }

    do
    {
        /* Set the middle to half of difference between min and max */
        current_setting = ((max - min) / 2) + min;

        /* Makes it possible to reach the maximum value, as it would not
         * otherwise be possible because of flooring present in integer
         * division. */
        if ((current_setting == (max - 1)) &&
            (current_setting == previous_setting))
        {
            current_setting = max;
        }

        /* Set power supply's VTRIM_BYTE register to current setting */
        *supply_ptr = (current_setting);

        /* Measure the power supply from the ADC register and convert the analog
         * value
         */

        current_readback_voltage = Calibrate_Power_MeasureSupply(adc_ptr);

        /* Exit if the target voltage == the current measured voltage */
        if (target == current_readback_voltage)
        {
            *final_voltage = current_readback_voltage;
            return ERRNO_NO_ERROR;
        }

        /* Determine the error in the ADC measurement and target
         * readback voltage.  */
        if (target < current_readback_voltage)
        {
            error = (current_readback_voltage - target);
        }
        else
        {
            error = (target - current_readback_voltage);
        }

        /* Exit if the current measurement is within allowed
         * ADC measurement error */
        if (error <= allowed_error)
        {
            /* Calibration successful */
            *final_voltage = current_readback_voltage;
            return (ERRNO_NO_ERROR);
        }

        /* Exit if the current setting is the same as the previous setting
         * This means that the search cannot proceed further. */
        if (previous_setting == current_setting)
        {
            /* Calibration not successful */
            *final_voltage = current_readback_voltage;
            return ERRNO_GENERAL_FAILURE;
        }

        /* If current measurement is less than target,
         * set minimum value to middle */
        if (current_readback_voltage < target)
        {
            min = current_setting;
        }
        /* If current measurement is greater than target,
         * set maximum value to middle */
        else if (current_readback_voltage > target)
        {
            max = current_setting;
        }

        previous_setting = current_setting;
    }
    while (max != min);

    *final_voltage = current_readback_voltage;
    return (ERRNO_GENERAL_FAILURE);
}

void Calibrate_Power_Initialize(void)
{
    uint32_t i = 0;

    /* Configure ADC */
    /* Full VBAT range, normal mode, run LSAD @ 625Hz/channel.
     * SLOWCLK = 1 MHz */
#if RSL15_CID == 202
    LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_200;
#else
    LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_200 | VBAT_DIV2_ENABLE;
#endif

    /* Clear all interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Turn off automatic compensation */
    for (i = 0; i < LSAD_NUM_CHANNELS; i++)
    {
#if RSL15_CID == 202
        LSAD->INPUT_SEL[i] =  LSAD_POS_INPUT_VBAT_DIV2 | LSAD_NEG_INPUT_VBAT_DIV2;
#else
        LSAD->INPUT_SEL[i] =  LSAD_POS_INPUT_VBAT | LSAD_NEG_INPUT_VBAT;
#endif
    }
}

unsigned int Calibrate_Power_VDDRF(unsigned int adc_num,
                                   const volatile uint32_t *adc_ptr,
                                   uint32_t target,
                                   CalPower_Type *final_trim)
{
    unsigned int result = 0;
    uint32_t final_voltage = 0;

    /* Enter min and max trim settings, found from Product Spec */
    uint8_t max_setting = VDDRF_TRIM_1P32V_BYTE;
    uint8_t min_setting = VDDRF_TRIM_0P75V_BYTE;

    /* Configure ADC channel to measure AOUT */
    LSAD->INPUT_SEL[adc_num] =  LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Enable interrupts on desired channel */
    LSAD->INT_ENABLE =  (adc_num << LSAD_INT_ENABLE_LSAD_INT_CH_NUM_Pos) | LSAD_INT_EN;

    /* Enable VDDRF */
    ACS->VDDRF_CTRL |= VDDRF_ENABLE;

    /* Output VDDRF on AOUT */
    ACS->AOUT_CTRL &= ~ACS_AOUT_CTRL_TEST_AOUT_Mask;
    ACS->AOUT_CTRL |= AOUT_VDDRF;

    /* Convert target to mV. */
    target *= 10;

    /* Perform Binary Search to find appropriate trim setting */
    result = Calibrate_Power_BinarySearch(target,
                                          max_setting,
                                          min_setting,
                                          adc_ptr,
                                          (uint8_t *)&ACS_VDDRF_CTRL->VTRIM_BYTE,
                                          &final_voltage);

    /* Store result for user usage */
    result = Calibrate_Power_StoreResult(final_trim,
                                         result,
                                         &final_voltage,
                                         (uint8_t *)&ACS_VDDRF_CTRL->VTRIM_BYTE);

    /* If an error occcured, specifiy which rail the error occured on */
    if (result != ERROR_NO_ERROR)
    {
        result |= ERRNO_VDDRF_CAL_ERROR;
    }

    /* Disable LSAD interrupts, clear interrupts */
    LSAD->INT_ENABLE = LSAD_INT_DIS;

    /* Clear all LSAD interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Exit with error code if Binary Search failed */
    return result;
}

unsigned int Calibrate_Power_VDDIF(unsigned int adc_num,
                                   const volatile uint32_t *adc_ptr,
                                   uint32_t target,
                                   CalPower_Type *final_trim)
{
#ifndef RSL15_CID
    unsigned int result = 0;
    uint32_t final_voltage = 0;

    /* Enter min and max trim settings, found from Product Spec */
    uint8_t max_setting = VDDIF_TRIM_2P325V_BYTE;
    uint8_t min_setting = VDDIF_TRIM_0P750V_BYTE;

    /* Configure ADC channel to measure AOUT */
    LSAD->INPUT_SEL[adc_num] =  LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Enable interrupts on desired channel */
    LSAD->INT_ENABLE =  (adc_num << LSAD_INT_ENABLE_LSAD_INT_CH_NUM_Pos) | LSAD_INT_EN;

    /* Enable VDDIF */
    ACS->VDDIF_CTRL |= VDDIF_ENABLE;

    /* Output VDDRF on AOUT */
    ACS->AOUT_CTRL &= ~ACS_AOUT_CTRL_TEST_AOUT_Mask;
    ACS->AOUT_CTRL |= AOUT_VDDIF;

    /* Convert target to mV. */
    target *= 10;

    /* Perform Binary Search to find appropriate trim setting */
    result = Calibrate_Power_BinarySearch(target,
                                          max_setting,
                                          min_setting,
                                          adc_ptr,
                                          (uint8_t *)&ACS_VDDIF_CTRL->VTRIM_BYTE,
                                          &final_voltage);

    /* Store result for user usage */
    result = Calibrate_Power_StoreResult(final_trim,
                                         result,
                                         &final_voltage,
                                         (uint8_t *)&ACS_VDDIF_CTRL->VTRIM_BYTE);

    if (result != ERROR_NO_ERROR)
    {
        result |= ERRNO_VDDRF_CAL_ERROR;
    }

    /* Disable LSAD interrupts, clear interrupts */
    LSAD->INT_ENABLE = LSAD_INT_DIS;

    /* Clear all LSAD interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Exit with error code if Binary Search failed */
    return result;
#else
    return ERRNO_VDDIF_CAL_ERROR;
#endif
}

unsigned int Calibrate_Power_VDDFLASH(unsigned int adc_num,
                                      const volatile uint32_t *adc_ptr,
                                      uint32_t target,
                                      CalPower_Type *final_trim)
{
    unsigned int result = 0;
    uint32_t final_voltage = 0;

    /* Enter min and max trim settings, found from Product Spec */
    uint8_t max_setting = VDDFLASH_TRIM_1P850V_BYTE;
    uint8_t min_setting = VDDFLASH_TRIM_1P500V_BYTE;

    /* Configure ADC channel to measure AOUT */
    LSAD->INPUT_SEL[adc_num] =  LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Enable interrupts on desired channel */
    LSAD->INT_ENABLE =  (adc_num << LSAD_INT_ENABLE_LSAD_INT_CH_NUM_Pos) | LSAD_INT_EN;

    /* Enable VDDRF */
    ACS->VDDFLASH_CTRL |= VDDFLASH_ENABLE;

    /* Output VDDRF on AOUT */
    ACS->AOUT_CTRL &= ~ACS_AOUT_CTRL_TEST_AOUT_Mask;
    ACS->AOUT_CTRL |= AOUT_VDDFLASH;

    /* Convert target to mV. */
    target *= 10;

    /* Perform Binary Search to find appropriate trim setting */
    result = Calibrate_Power_BinarySearch(target,
                                          max_setting,
                                          min_setting,
                                          adc_ptr,
                                          (uint8_t *)&ACS_VDDFLASH_CTRL->VTRIM_BYTE,
                                          &final_voltage);

    /* Store result for user usage */
    result = Calibrate_Power_StoreResult(final_trim,
                                         result,
                                         &final_voltage,
                                         (uint8_t *)&ACS_VDDFLASH_CTRL->VTRIM_BYTE);

    /* If an error occcured, specifiy which rail the error occured on */
    if (result != ERROR_NO_ERROR)
    {
        result |= ERRNO_VDDFLASH_CAL_ERROR;
    }

    /* Disable LSAD interrupts, clear interrupts */
    LSAD->INT_ENABLE = LSAD_INT_DIS;

    /* Clear all LSAD interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Exit with error code if Binary Search failed */
    return result;
}

unsigned int Calibrate_Power_VDDPA(unsigned int adc_num,
                                   const volatile uint32_t *adc_ptr,
                                   uint32_t target,
                                   CalPower_Type *final_trim)
{
    unsigned int result = 0;
    uint32_t final_voltage = 0;
    uint8_t dynamic_vddpa_bk = SYSCTRL_VDDPA_CFG0->DYNAMIC_CTRL_BYTE;

    /* Enter min and max trim settings, found from Product Spec */
    uint8_t max_setting = VDDPA_TRIM_1P68V_BYTE;
    uint8_t min_setting = VDDPA_TRIM_1P05V_BYTE;

    /* Configure ADC channel to measure AOUT */
    LSAD->INPUT_SEL[adc_num] =  LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Enable interrupts on desired channel */
    LSAD->INT_ENABLE =  (adc_num << LSAD_INT_ENABLE_LSAD_INT_CH_NUM_Pos) | LSAD_INT_EN;

    /* Enable VDDPA */
    SYSCTRL_VDDPA_CFG0->DYNAMIC_CTRL_BYTE = DYNAMIC_CTRL_DISABLE_BYTE;
    ACS->VDDPA_CTRL |= VDDPA_ENABLE;

    /* Output VDDPA on AOUT */
    ACS->AOUT_CTRL &= ~ACS_AOUT_CTRL_TEST_AOUT_Mask;
    ACS->AOUT_CTRL |= AOUT_VDDPA;

    /* Convert target to mV. */
    target *= 10;

    /* Perform Binary Search to find calibration setting */
    result = Calibrate_Power_BinarySearch(target,
                                          max_setting,
                                          min_setting,
                                          adc_ptr,
                                          (uint8_t *)&ACS_VDDPA_CTRL->VTRIM_BYTE,
                                          &final_voltage);

    /* Store result for user usage */
    result = Calibrate_Power_StoreResult(final_trim,
                                         result,
                                         &final_voltage,
                                         (uint8_t *)&ACS_VDDPA_CTRL->VTRIM_BYTE);

    /* If an error occcured, specifiy which rail the error occured on */
    if (result != ERROR_NO_ERROR)
    {
        result |= ERRNO_VDDPA_CAL_ERROR;
    }

    /* Disable LSAD interrupts, clear interrupts */
    LSAD->INT_ENABLE = LSAD_INT_DIS;

    /* Clear all LSAD interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Restore dynamic VDDPA setting */
    SYSCTRL_VDDPA_CFG0->DYNAMIC_CTRL_BYTE = dynamic_vddpa_bk;

    /* Exit with error code if Binary Search failed */
    return result;
}

unsigned int Calibrate_Power_DCDC(unsigned int adc_num,
                                  const volatile uint32_t *adc_ptr,
                                  uint32_t target,
                                  CalPower_Type *final_trim)
{
    unsigned int result = 0;
    uint32_t final_voltage = 0;

    /* Enter min and max trim settings, found from Product Spec */
    uint8_t max_setting = (uint8_t)VCC_TRIM_1P31V;
    uint8_t min_setting = (uint8_t)VCC_TRIM_1P05V;

    /* Configure ADC channel to measure AOUT */
    LSAD->INPUT_SEL[adc_num] =  LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Enable interrupts on desired channel */
    LSAD->INT_ENABLE =  (adc_num << LSAD_INT_ENABLE_LSAD_INT_CH_NUM_Pos) | LSAD_INT_EN;

    /* Output DCDC on AOUT */
    ACS->AOUT_CTRL &= ~ACS_AOUT_CTRL_TEST_AOUT_Mask;
    ACS->AOUT_CTRL |= AOUT_VCC;

    /* Convert target to mV. */
    target *= 10;

    /* Perform Binary Search to find calibration setting */
    result = Calibrate_Power_BinarySearch(target,
                                          max_setting,
                                          min_setting,
                                          adc_ptr,
                                          (uint8_t *)ACS_VCC_CTRL,
                                          &final_voltage);

    /* Store result for user usage */
    result = Calibrate_Power_StoreResult(final_trim,
                                         result,
                                         &final_voltage,
                                         (uint8_t *)ACS_VCC_CTRL);

    /* If an error occcured, specifiy which rail the error occured on */
    if (result != ERROR_NO_ERROR)
    {
        result |= ERRNO_DCDC_CAL_ERROR;
    }

    /* Disable LSAD interrupts, clear interrupts */
    LSAD->INT_ENABLE = LSAD_INT_DIS;

    /* Clear all LSAD interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Exit with error code if Binary Search failed */
    return result;
}

unsigned int Calibrate_Power_VDDC(unsigned int adc_num,
                                  const volatile uint32_t *adc_ptr,
                                  uint32_t target,
                                  CalPower_Type *final_trim)
{
    unsigned int result = 0;
    uint32_t final_voltage = 0;

    /* Enter min and max trim settings, found from Product Spec */
    uint8_t max_setting = VDDC_TRIM_1P32V_BYTE;
    uint8_t min_setting = VDDC_TRIM_0P75V_BYTE;

    /* Configure ADC channel to measure VDDC */
    LSAD->INPUT_SEL[adc_num] =  LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Enable interrupts on desired channel */
    LSAD->INT_ENABLE =  (adc_num << LSAD_INT_ENABLE_LSAD_INT_CH_NUM_Pos) | LSAD_INT_EN;

    /* Output VDDC on AOUT */
    ACS->AOUT_CTRL &= ~ACS_AOUT_CTRL_TEST_AOUT_Mask;
    ACS->AOUT_CTRL |= AOUT_VDDC;

    /* Convert target to mV. */
    target *= 10;

    /* Increment the target to account for calibration error
     * and guarantee we are above the target, as defined in the spec */
    target += VDDCM_TARGET_OFFSET;

    /* Perform Binary Search to find calibration setting */
    result = Calibrate_Power_BinarySearch(target,
                                          max_setting,
                                          min_setting,
                                          adc_ptr,
                                          (uint8_t *)&ACS_VDDC_CTRL->VTRIM_BYTE,
                                          &final_voltage);

    /* Store result for user usage */
    result = Calibrate_Power_StoreResult(final_trim,
                                         result,
                                         &final_voltage,
                                         (uint8_t *)&ACS_VDDC_CTRL->VTRIM_BYTE);

    /* If an error occcured, specifiy which rail the error occured on */
    if (result != ERROR_NO_ERROR)
    {
        result |= ERRNO_VDDC_CAL_ERROR;
    }

    /* Disable LSAD interrupts, clear interrupts */
    LSAD->INT_ENABLE = LSAD_INT_DIS;

    /* Clear all LSAD interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Exit with error code if Binary Search failed */
    return result;
}

unsigned int Calibrate_Power_VDDM(unsigned int adc_num,
                                  const volatile uint32_t *adc_ptr,
                                  uint32_t target,
                                  CalPower_Type *final_trim)
{
    unsigned int result = 0;
    uint32_t final_voltage = 0;

    /* Enter min and max trim settings, found from Product Spec */
    uint8_t max_setting = VDDM_TRIM_1P32V_BYTE;
    uint8_t min_setting = VDDM_TRIM_0P95V_BYTE;

    /* Configure ADC channel to measure AOUT */
    LSAD->INPUT_SEL[adc_num] =  LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Enable interrupts on desired channel */
    LSAD->INT_ENABLE =  (adc_num << LSAD_INT_ENABLE_LSAD_INT_CH_NUM_Pos) | LSAD_INT_EN;

    /* Output VDDM on AOUT */
    ACS->AOUT_CTRL &= ~ACS_AOUT_CTRL_TEST_AOUT_Mask;
    ACS->AOUT_CTRL |= AOUT_VDDM;

    /* Convert target to mV. */
    target *= 10;

    /* Increment the target to account for the calibration error
     * and guarantee we are above the target, as defined in the spec */
    target += VDDCM_TARGET_OFFSET;

    /* Perform Binary Search to find calibration setting */
    result = Calibrate_Power_BinarySearch(target,
                                          max_setting,
                                          min_setting,
                                          adc_ptr,
                                          (uint8_t *)&ACS_VDDM_CTRL->VTRIM_BYTE,
                                          &final_voltage);

    /* Store result for user usage */
    result = Calibrate_Power_StoreResult(final_trim,
                                         result,
                                         &final_voltage,
                                         (uint8_t *)&ACS_VDDC_CTRL->VTRIM_BYTE);

    /* If an error occcured, specifiy which rail the error occured on */
    if (result != ERROR_NO_ERROR)
    {
        result |= ERRNO_VDDC_CAL_ERROR;
    }

    /* Disable LSAD interrupts, clear interrupts */
    LSAD->INT_ENABLE = LSAD_INT_DIS;

    /* Clear all LSAD interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Exit with error code if Binary Search failed */
    return result;
}
