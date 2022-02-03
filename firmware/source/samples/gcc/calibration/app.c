/**
 * @file app.c
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

#include "app.h"

int main(void)
{
    CalPower_Type pwr_results[1] = { 0 };
    CalClock_Type clock_results[1] = { 0 };
    uint32_t result = ERROR_NO_ERROR;
    TRIM_Type new_trims[1] = { 0 };
    uint32_t flash_error = 0;

    /* Check for recovery GPIO to enable recovery. */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN |
                    GPIO_LPF_DISABLE |
                    GPIO_WEAK_PULL_UP  |
                    GPIO_6X_DRIVE);
    while ((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

    /* Output analog measurements on a GPIO */
    /* Extra care is to be taken when outputting AOUT on a GPIO,
     * if not, the device can be damaged. Here, we ensure
     * there is no voltage on the pin, and then pullup/pulldown
     * resistors are disabled.  */
    GPIO->CFG[AOUT_GPIO] = GPIO_WEAK_PULL_DOWN;
    Sys_Delay(AOUT_ENABLE_DELAY);
    GPIO->CFG[AOUT_GPIO] = GPIO_MODE_DISABLE | GPIO_NO_PULL;
    ACS->AOUT_CTRL = AOUT_VSSA | SEL_AOUT_TO_GPIO |
                     (AOUT_GPIO << ACS_AOUT_CTRL_AOUT_TO_GPIO_Pos);

    /* Load default trim values */
    SYS_TRIM_LOAD_DEFAULT();

    /* Set system clock to 16 MHz for power calibration */
    /* Set slow clock to maximum of 1 MHz for accurate LSAD measurements */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_3_BYTE);
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);
    CLK_DIV_CFG0->SLOWCLK_PRESCALE_BYTE = SLOWCLK_PRESCALE_16;

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Calibrate power initialize */
    Calibrate_Power_Initialize();

    /* Calibrate DCDC */
    result |= Calibrate_Power_DCDC(LSAD_CALIB_CHANNEL,
                                   &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                   TARGET_DCDC_1_25V, pwr_results);

    /* Record calibration results for the DCDC */
    new_trims->dcdc[0].DCDC_trim = pwr_results->trim_setting;
    new_trims->dcdc[0].target = TARGET_DCDC_1_25V;

    /* Calibrate VDDRF */
    result |= Calibrate_Power_VDDRF(LSAD_CALIB_CHANNEL,
                                    &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                    TARGET_VDDRF_1_15V, pwr_results);

    /* Record calibration results for the VDDRF */
    new_trims->vddrf[0].trim = pwr_results->trim_setting;
    new_trims->vddrf[0].target = TARGET_VDDRF_1_15V;

    /* Calibrate VDDC */
    result |= Calibrate_Power_VDDC(LSAD_CALIB_CHANNEL,
                                   &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                   TARGET_VDDC_1_1V, pwr_results);

    /* Record calibration results for the VDDC */
    new_trims->vddc[0].trim_voltage = pwr_results->trim_setting;
    new_trims->vddc[0].target_voltage = TARGET_VDDC_1_1V;

    /* Calibrate VDDM */
    result |= Calibrate_Power_VDDM(LSAD_CALIB_CHANNEL,
                                   &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                   TARGET_VDDM_1_15V, pwr_results);

    /* Record calibration results for the VDDM */
    new_trims->vddm[0].trim_voltage = pwr_results->trim_setting;
    new_trims->vddm[0].target_voltage = TARGET_VDDM_1_15V;

    /* Disable dynamic VDDPA, so the user can see the calibration result */
    SYSCTRL_VDDPA_CFG0->DYNAMIC_CTRL_BYTE = DYNAMIC_CTRL_DISABLE_BYTE;

    /* Calibrate VDDPA */
    result |= Calibrate_Power_VDDPA(LSAD_CALIB_CHANNEL,
                                    &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                    TARGET_VDDPA_1_64V, pwr_results);

    /* Record calibration results for the VDDPA */
    new_trims->vddpa[0].trim_voltage = pwr_results->trim_setting;
    new_trims->vddpa[0].target_voltage = TARGET_VDDPA_1_64V;

    /* Calibrate VDDIF */
    result |= Calibrate_Power_VDDIF(LSAD_CALIB_CHANNEL,
                                    &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                    TARGET_VDDIF_1_9V, pwr_results);

    /* Record calibration results for the VDDIF */
    new_trims->vddif[0].trim = pwr_results->trim_setting;
    new_trims->vddif[0].target = TARGET_VDDIF_1_9V;

    /* Calibrate VDDFLASH */
    result |= Calibrate_Power_VDDFLASH(LSAD_CALIB_CHANNEL,
                                       &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                       TARGET_FLASH_1600, pwr_results);

    /* Record calibration results for the VDDFLASH */
    new_trims->vddflash[0].trim = pwr_results->trim_setting;
    new_trims->vddflash[0].target = TARGET_FLASH_1600;

    /* Start calibration of RC oscillators */
    Calibrate_Clock_Initialize();

    /* Set system clock to 48 MHz for maximum clock calibration accuracy */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_1_BYTE);
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);
    CLK_DIV_CFG0->SLOWCLK_PRESCALE_BYTE = SLOWCLK_PRESCALE_48;

    /* Disconnect AOUT from GPIO */
    ACS->AOUT_CTRL = AOUT_VSSA;

    /* Output 32K Standby clock on AOUT GPIO */
    GPIO->CFG[AOUT_GPIO] = GPIO_MODE_STANDBYCLK;

    /* Calibrate Standby oscillator */
    result |= Calibrate_Clock_32K_RCOSC(TARGET_40KHZ, clock_results);

    /* Record calibration results for the RCOSC32 */
    new_trims->rcosc32[0].trim = clock_results->trim_setting;
    new_trims->rcosc32[0].target = TARGET_40KHZ;

    /* Set standby oscillator back to 32768Hz, in order to calibrate
     * start oscillator*/
    result |= Calibrate_Clock_32K_RCOSC(TARGET_RC32K, clock_results);

    /* Output RC start clock on AOUT GPIO */
    GPIO->CFG[AOUT_GPIO] = GPIO_MODE_RCCLK;

    /* Calibrate Start oscillator */
    /* Set start oscillator to 3MHz multiplier */
    ACS->RCOSC_CTRL &= ~ACS_RCOSC_CTRL_RC_FSEL_Mask;
    ACS->RCOSC_CTRL |= RC_OSC_3MHZ;
    result |= Calibrate_Clock_Start_OSC(TARGET_4MHZ,  clock_results);

    /* Record calibration results for the RCOSC32 */
    new_trims->rcosc[0].trim = clock_results->trim_setting;
    new_trims->rcosc[0].target = TARGET_4MHZ;

    /* Set system clock back to 16 MHz, by first switching to RCCLK */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_3_BYTE);
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    if (combineTrims((TRIM_Type *)new_trims, (TRIM_Type *)FLASH0_NVR4_BASE) & ERASE_OK)
    {
        /* If we returned a bad CRC value and we've indicated its ok to erase
         * NVR4, erase NVR4. */

        /* Power up and initialize flash timing registers based on SystemClock */
        Flash_Initialize(0, FLASH_CLOCK_16MHZ);
        flash_error = Flash_EraseSector(FLASH0_NVR4_BASE, 0);

        while (flash_error)
        {
            /* Flash error, halt program */
            SYS_WATCHDOG_REFRESH();
        }
    }

    /* Combine the new calibration data with the existing NVR4 data from
     * previous supplemental calibrations. */
    StoreTrimsInFlash((TRIM_Type *)new_trims);

    /* Reset trim values back to default */
    SYS_TRIM_LOAD_DEFAULT();

    /* Retrieve and Load newly stored VDDPA trim value */
    Sys_Trim_LoadVDDPA((TRIM_Type *)FLASH0_NVR4_BASE, TARGET_VDDPA_1_64V);

    /* Configure the AOUT_GPIO for AOUT, to output VDDPA*/
    GPIO->CFG[AOUT_GPIO] = GPIO_WEAK_PULL_DOWN;
    Sys_Delay(AOUT_ENABLE_DELAY);
    GPIO->CFG[AOUT_GPIO] = GPIO_MODE_DISABLE | GPIO_NO_PULL;

    /* Output VDDPA on AOUT GPIO, enable VDDPA */
    ACS->VDDPA_CTRL = VDDPA_SW_HIZ |
                      VDDPA_ISENSE_DISABLE |
                      VDDPA_ENABLE |
                      (ACS->VDDPA_CTRL & ACS_VDDPA_CTRL_VTRIM_Mask);
    ACS->AOUT_CTRL = AOUT_VDDPA | SEL_AOUT_TO_GPIO |
                     (AOUT_GPIO << ACS_AOUT_CTRL_AOUT_TO_GPIO_Pos);

    /* Spin loop */
    while (1)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    return 0;
}

uint32_t combineTrims(TRIM_Type *new_trims, TRIM_Type *old_trims)
{
    uint32_t i;
    uint32_t j;
    uint32_t *k;
    uint32_t *new_trims_int = (uint32_t *)new_trims;
    uint32_t *old_trims_int = (uint32_t *)old_trims;
    uint32_t num_trims_found = 0;

    if (old_trims->CHECKSUM != 0 && old_trims->CHECKSUM != MAX_32_BIT)
    {
        /* Use nested if to avoid checking CRC if uncessary */
        if (Sys_Trim_CheckCRC(old_trims))
        {
            /* CRC failed */
            return 1;
        }
    }

    /* Check if there are new records available from the calibration
     * data received. Assumption is data is in the 0th record for
     * each calibrated component. */
    for (i = 0; i < sizeof(TRIM_Type) / 4; i += 4)
    {
        num_trims_found = 0;
        for (j = 0; j < RECORD_SIZE; j++)
        {
            /* Check if the old trim value exists and is not the same target
             * as the current new trim value, assuming reading values in flash,
             * where empty spaces are filled with 0xFFFFFFFF instead of 0.*/
            /* This check will allow records with the same targets, but different
             * minimum targets to co-exist. The entire 16-bit target field must
             * be identical in order to overwrite the previous record. */
            if ((old_trims_int[i + j] != 0xFFFFFFFF) &&
                ((old_trims_int[i + j] & 0xFFFF0000) !=
                 (new_trims_int[i] & 0xFFFF0000)))
            {
                /* We have an old, unique trim value we should keep.
                 * Copy the old trim record to the new structure. */
                if (new_trims_int[i] != 0)
                {
                    num_trims_found++;
                }
                new_trims_int[i + num_trims_found] = old_trims_int[i + j];
            }
            if (new_trims_int[i + j] == 0)
            {
                /* We have an empty space, convert to 0xFFFFFFFF
                 * to avoid writing all 0s to flash. */
                new_trims_int[i + j] = 0xFFFFFFFF;
            }
        }
    }

    /* Lastly, we must calculate the new trim region's CRC value */
    /* Add values to CRC*/
    SYS_CRC_CONFIG(CRC_32);
    SYS_CRC_32INITVALUE();

    for (k = new_trims_int; k <= (uint32_t *)&(new_trims->SOS_REV); k++)
    {
        SYS_CRC_ADD(*k, 32);
    }

    /* Set calculated CRC value in new trim region */
    new_trims->CHECKSUM = SYS_CRC_GETFINALVALUE();

    return 0;
}

void StoreTrimsInFlash(TRIM_Type *new_trims)
{
    uint32_t flash_error = FLASH_ERR_NONE;
    uint32_t size = sizeof(TRIM_Type) / 4;

    /* Power up and initialize flash timing registers based on SystemClock */
    Flash_Initialize(0, FLASH_CLOCK_16MHZ);

    /* Erase the supplemental calibration data region,
     * we have combined old values with the NVR7 region. */
    flash_error = Flash_EraseSector(FLASH0_NVR4_BASE, 0);
    if (flash_error != FLASH_ERR_NONE)
    {
        flash_error = 3;

        while (1)
        {
            /* An error has occurred, see result for details */

            /* Refresh the watchdog timers */
            SYS_WATCHDOG_REFRESH();
        }
    }

    flash_error = Flash_WriteBuffer(FLASH0_NVR4_BASE, size, (uint32_t *)new_trims, 1);

    if (flash_error != FLASH_ERR_NONE)
    {
        flash_error = 2;

        while (1)
        {
            /* An error has occurred, see result for details */

            /* Refresh the watchdog timers */
            SYS_WATCHDOG_REFRESH();
        }
    }
}
