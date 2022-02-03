/**
 * @file calibrate_power.h
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

#ifndef CALIBRATE_POWER_H
#define CALIBRATE_POWER_H

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

/* ----------------------------------------------------------------------------
 * Constant Definitions
 * ------------------------------------------------------------------------- */

/** LSAD variance in LSBs considered "stable" and not discharging */
#define LSAD_STABILIZED_RANGE            10

/** Set the LSAD measurement error to 5 [mV]. The trim step size is 10 mV
 * so ideally every value 5 mV apart can be reached.*/
#define LSAD_MEASUREMENT_ERROR           5

/** Set the LSAD measurement error to 13 [mV]. The trim step size for VDDIF/FLASH
 * is 25 mV so ideally every value 12.5 mV apart can be reached, rounded up to 13 mV. */
#define LSAD_IF_MEASUREMENT_ERROR        13

/** Number of LSAD channels in device */
#define LSAD_NUM_CHANNELS                8

/** Offset the desired target to ensure while calibrating.
 *  We guarantee that we do not go under the desired target voltage */
#define VDDCM_TARGET_OFFSET             5

/** Bandgap maximum safe voltage - 1 trim step */
#define BG_VTRIM_0P820V_BYTE            ((uint8_t)(0x2AU << ACS_BG_CTRL_VTRIM_BYTE_Pos))

/** Digital Memories voltage regulator minimum safe voltage to maintain function. */
#define VDDM_TRIM_0P95V_BYTE            ((uint8_t)(0x0FU << ACS_VDDM_CTRL_VTRIM_BYTE_Pos))

/** Flash voltage regulator minimum safe voltage to maintain function */
#define VDDFLASH_TRIM_1P500V_BYTE       ((uint8_t)(0x1EU << ACS_VDDM_CTRL_VTRIM_BYTE_Pos))

/** Flash voltage regulator maximum safe voltage*/
#define VDDFLASH_TRIM_1P850V_BYTE       ((uint8_t)(0x2CU << ACS_VDDM_CTRL_VTRIM_BYTE_Pos))

/** Factor for converting back and forth from mV to V. */
#define V_TO_MV                          1000

/** Float iteration of factor for converting back and forth from mV to V. */
#define V_TO_MV_F                        1000.0f

/**
 * @def         CONVERT(x)
 * @brief       Converts an ADC code to a voltage, calculated as follows
 *              voltage = adc_code * (2 V * 1000 [mV]/1 V / 2^14 steps.)
 * @param[in]   x   the ADC code input
 * @return      The voltage output in mV
 * @assumptions Low frequency mode for the ADC is used, meaning that the
 *              resolution of the ADC is 14-bits. CONVERT provides voltage
 *              level as a milliVolt value based on the input ADC code.
 */
#define CONVERT(x)                      ((uint32_t)((x * 1000) >> 13))

/**
 * @def         SWAP(a, b, t)
 * @brief       Swap the values in variables a and b
 * @param[in]     a  holds the value that must go to b
 * @param[in]     b  holds the value that must go to a
 * @param[in]     t  a temporary buffer for the swap
 * @return        a holds the value previously in b,
 *                b holds the value previously in a.
 */
#define SWAP(a, b, t)                   ((t) = (a), (a) = (b), (b) = (t))

/**
 * @brief       Contains the quantitative results of the calibration,
 *              returned from each rail calibration function.
 * @param[out]  trim_setting  the final trim setting in the relevant register.
 * @param[out]  read_voltage  last measured voltage at the current final trim
 *                            setting. Value is in mV.
 */
typedef struct
{
    /** Trim setting result */
    uint32_t trim_setting;

    /** Voltage measured after calibration */
    uint32_t read_voltage;
} CalPower_Type;

/**
 * @brief        Initialize the system to support power supply calibration.
 *               1. Changes settings in all power supply control registers to
 *                 their default values.
 *               2. Sets the system clock source to RFCLK/3 (16 MHz).
 *               3. Configures the ADC to enable measurement at 625 Hz
 * @assumptions  VBAT must be equal to 1.5V if calibrating bandgap.
 * @examplecode  calibrate_examples.c Calibrate_Power_Initialize_Example
 */
void Calibrate_Power_Initialize(void);

/**
 * @brief        Calibrate the radio front-end power supply (VDDRF).
 * @param[in]      adc_num  ADC channel number [0-7]
 * @param[in]      adc_ptr  Pointer to the ADC data register
 * @param[in]      target   Target voltage readback [10*mV]
 * @param[out]     final_trim  Result of calibration, trim setting & voltage (mV)
 * @return       Status code indicating whether the calibration has succeeded
 * @assumptions  VBG has been calibrated.
 * @assumptions  Calibrate_Power_Initialize() has been called.
 * @assumptions  VCC is sufficiently high to trim VDDRF to the desired
 *               value. This is because VCC supplies VDDRF.
 * @examplecode  calibrate_examples.c Calibrate_Power_VDDRF_Example
 */
unsigned int Calibrate_Power_VDDRF(unsigned int adc_num,
                                   const volatile uint32_t *adc_ptr,
                                   uint32_t target,
                                   CalPower_Type *final_trim);

/**
 * @brief        Calibrate the interfaces power supply (VDDIF).
 * @param[in]      adc_num  ADC channel number [0-7]
 * @param[in]      adc_ptr  Pointer to the ADC data register
 * @param[in]      target   Target voltage readback [10*mV]
 * @param[out]     final_trim  Result of calibration, trim setting & voltage (mV)
 * @return       Status code indicating whether the calibration has succeeded
 * @assumptions  VBG has been calibrated.
 * @assumptions  Calibrate_Power_Initialize() has been called.
 * @assumptions  VDDCP is sufficiently high to trim VDDIF to the desired
 *               value. This is because VDDCP supplies VDDIF.
 * @examplecode  calibrate_examples.c Calibrate_Power_VDDIF_Example
 */
unsigned int Calibrate_Power_VDDIF(unsigned int adc_num,
                                   const volatile uint32_t *adc_ptr,
                                   uint32_t target,
                                   CalPower_Type *final_trim);

/**
 * @brief        Calibrate the flash power supply (VDDFLASH).
 * @param[in]      adc_num  ADC channel number [0-7]
 * @param[in]      adc_ptr  Pointer to the ADC data register
 * @param[in]      target   Target voltage readback [10*mV]
 * @param[out]     final_trim  Result of calibration, trim setting & voltage (mV)
 * @return       Status code indicating whether the calibration has succeeded
 * @assumptions  VBG has been calibrated.
 * @assumptions  Calibrate_Power_Initialize() has been called.
 * @assumptions  VDDCP is sufficiently high to trim VDDFLASH to the desired
 *               value. This is because VDDCP supplies VDDFLASH.
 * @examplecode  calibrate_examples.c Calibrate_Power_VDDFLASH_Example
 */
unsigned int Calibrate_Power_VDDFLASH(unsigned int adc_num,
                                      const volatile uint32_t *adc_ptr,
                                      uint32_t target,
                                      CalPower_Type *final_trim);

/**
 * @brief        Calibrate the radio power amplifier power supply (VDDPA).
 * @param[in]      adc_num  ADC channel number [0-7]
 * @param[in]      adc_ptr  Pointer to the ADC data register
 * @param[in]      target   Target voltage readback [10*mV]
 * @param[out]     final_trim  Result of calibration, trim setting & voltage (mV)
 * @return       Status code indicating whether the calibration has succeeded
 * @assumptions  VBG has been calibrated.
 * @assumptions  Calibrate_Power_Initialize() has been called.
 * @examplecode  calibrate_examples.c Calibrate_Power_VDDPA_Example
 */
unsigned int Calibrate_Power_VDDPA(unsigned int adc_num,
                                   const volatile uint32_t *adc_ptr,
                                   uint32_t target,
                                   CalPower_Type *final_trim);

/**
 * @brief        Calibrate the DC-DC converter (DCDC).
 * @param[in]      adc_num ADC channel number [0-7]
 * @param[in]      adc_ptr  Pointer to the ADC data register
 * @param[in]      target   Target voltage readback [10*mV]
 * @param[out]     final_trim  Result of calibration, trim setting & voltage (mV)
 * @return       Status code indicating whether the calibration has succeeded
 * @assumptions  VBG has been calibrated.
 * @assumptions  User is responsible for selecting LDO or BUCK mode for the DCDC
 *               converter
 * @assumptions  Calibrate_Power_Initialize() has been called.
 * @examplecode  calibrate_examples.c Calibrate_Power_DCDC_Example
 */
unsigned int Calibrate_Power_DCDC(unsigned int adc_num,
                                  const volatile uint32_t *adc_ptr,
                                  uint32_t target,
                                  CalPower_Type *final_trim);

/**
 * @brief        Calibrate the digital core voltage power supply (VDDC).
 * @param[in]      adc_num ADC channel number [0-7]
 * @param[in]      adc_ptr  Pointer to the ADC data register
 * @param[in]      target   Target voltage readback [10*mV]
 * @param[out]     final_trim  Result of calibration, trim setting & voltage (mV)
 * @return       Status code indicating whether the calibration has succeeded
 * @assumptions  VBG has been calibrated.
 * @assumptions  Calibrate_Power_Initialize() has been called.
 * @examplecode  calibrate_examples.c Calibrate_Power_VDDC_Example
 */
unsigned int Calibrate_Power_VDDC(unsigned int adc_num,
                                  const volatile uint32_t *adc_ptr,
                                  uint32_t target,
                                  CalPower_Type *final_trim);

/**
 * @brief        Calibrate the digital memory voltage (VDDM)
 * @param[in]      adc_num ADC channel number [0-7]
 * @param[in]      adc_ptr  Pointer to the ADC data register
 * @param[in]      target   Target voltage readback [10*mV]
 * @param[out]     final_trim  Result of calibration, trim setting & voltage (mV)
 * @return       Status code indicating whether the calibration has succeeded
 * @assumptions  VBG has been calibrated.
 * @assumptions  Calibrate_Power_Initialize() has been called.
 * @examplecode  calibrate_examples.c Calibrate_Power_VDDM_Example
 */
unsigned int Calibrate_Power_VDDM(unsigned int adc_num,
                                  const volatile uint32_t *adc_ptr,
                                  uint32_t target,
                                  CalPower_Type *final_trim);

/** @} */ /* End of the CALIBRATELIB group */

#endif    /* ifndef CALIBRATE_POWER_H */
