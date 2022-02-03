/**
 * @file app.c
 * @brief Sample application for the Print Device Info.
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

#if SWMTRACE_ENABLE
static uint32_t traceOptions[] = {
    SWM_LOG_LEVEL_INFO,                  /* In all cases log info messages */
    SWM_UART_RX_PIN | UART_RX_GPIO,      /* Set RX pin for cases when using UART */
    SWM_UART_TX_PIN | UART_TX_GPIO,      /* Set TX pin for cases when using UART */
    SWM_UART_RX_ENABLE,                  /* Enable the UART Rx Interrupts */
    SWM_UART_BAUD_RATE | UART_BAUD       /* Set Baud rate */
};

void SWMTraceInit(void)
{

    swmTrace_init(traceOptions, 5);
}
#else    /* SWMTRACE_ENABLE */
/* Used for semi-hosting */
extern void initialise_monitor_handles(void);
#endif   /* SWMTRACE_ENABLE */

/**
 * @brief         Perform the following steps:
 *                 - Initialize the system
 *                 - Read and print the MNVR data
 *                 - print NVR7 for default calibration data
 *                 - print Bluetooth bondlist information
 *
 * @param[in]     None
 * @param[out]    None
 * @assumptions   None
 */
int main(void)
{
    uint8_t found = 0;

    /* Initialize the system clock to a known clock rate. */
    App_Clock_Config();

#ifdef SWMTRACE_ENABLE
    SWMTraceInit();
#else    /* SWMTRACE_ENABLE */
    /* Initialize semi-hosting */
    initialise_monitor_handles();
#endif   /* SWMTRACE_ENABLE */

    printf("Semi-hosting initialized\n");

    /* Print Chip ID information */
    printf("\n");
    printf("Chip Family: %u\n",AHBREGS_CHIP_ID_NUM->CHIP_FAMILY_BYTE);
    printf("Chip Version: %u\n",AHBREGS_CHIP_ID_NUM->CHIP_VERSION_BYTE);
    printf("Chip Major Revision: %u\n",AHBREGS_CHIP_ID_NUM->CHIP_MAJOR_REVISION_BYTE);
    printf("Chip Minor Revision: %u\n",AHBREGS_CHIP_ID_NUM->CHIP_MINOR_REVISION_BYTE);

    /* Print the BLE MAC address located in MNVR section */
    printf("  MNVR BLE MAC address: ");
    found = 0;
    for (signed int i = 2; i >= 0; i--)
    {
        /* If the stored MAC address is not valid increment found */
        if (*(uint16_t *)(FLASH0_MNVR_BASE + 2 * i) != 0xFFFF)
        {
            found++;
        }
    }
    /* If proper MAC address was found print it */
    if (found >2)
    {
        printf("0x");
        for (signed int i = 2; i >= 0; i--)
        {
            printf("%04X", *(uint16_t *)(FLASH0_MNVR_BASE + 2 * i));
        }
        printf("\n");
    }
    else
    {
        printf("(none)");
        printf("\n");
    }

    /* Print NVR7 Default Calibration Value Information */
    printf("NVR7 contents:\n");
    printf("Default TRIM Calibration values:\n");
    printTrimCalibrationValues(TRIM);

    /* Print Bluetooth Bond Information*/
    printBondInfo();

    /* Spin loop */
    while (1)
    {
        /* Refresh system watchdog timer */
        SYS_WATCHDOG_REFRESH();

    }
}


void App_Clock_Config(void)
{
	/* Disable all interrupts and clear any pending interrupts */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Test GPIO0 to pause the program to make it easy to re-flash */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
       SYS_WATCHDOG_REFRESH();
    }

    /* Load default trim values; SystemCoreClock will be updated below */
    uint32_t trim_error __attribute__((unused)) = SYS_TRIM_LOAD_DEFAULT();

    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8);

    /* Disable JTAG TDI, TDO, and TRST connections to DIO 2, 3, and 4 */
    GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);

    /* Stop masking interrupts */
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
}


void printBondInfo(void)
{
    uint8_t found = 0;
    uint32_t tmp;

    /* prints the Bluetooth Bonding list. This uses BondInfo_t structure to prints out the stored information
     * located in the BOND_INFO_BASE address in flash. This BondInfo_t structure is located in bondlist.h inside this project but normally it would
     * exist in the ble abstraction source location. */
    printf("Bluetooth bonding list:\n");
    found = 0;
    for (unsigned int e = 0; e < BONDLIST_MAX_SIZE; e++)
    {
        tmp = ((BondInfo_t *)BOND_INFO_BASE)[e].state;
        if (BOND_INFO_STATE_VALID(tmp))
        {
            printf("  Entry %u\n", e);
            printf("    STATE: 0x%02lX\n", tmp);
            printf("    LTK (Long Term Key): 0x");
            for (signed int i = 15; i >= 0; i--)
            {
                printf("%02X", ((BondInfo_t *)BOND_INFO_BASE)[e].ltk[i]);
            }
            printf("\n");
            printf("    EDIV (Encrypted Diversifier): 0x%04X\n",
                   ((BondInfo_t *)BOND_INFO_BASE)[e].ediv);
            printf("    Address: 0x");
            for (signed int i = 6; i >= 0; i--)
            {
                printf("%02X", ((BondInfo_t *)BOND_INFO_BASE)[e].addr[i]);
            }
            tmp = ((BondInfo_t *)BOND_INFO_BASE)[e].addr_type;
            printf("    ADDR_TYPE: 0x%02lX ", tmp);
            if (tmp == BD_TYPE_PUBLIC)
            {
                printf("(Public)\n");
            }
            else if (tmp == BD_TYPE_PRIVATE)
            {
                printf("(Private)\n");
            }
            else
            {
                printf("(Unknown)\n");
            }
            printf("    CSRK (Connection Signature Resolving Key): 0x");
            for (signed int i = 15; i >= 0; i--)
            {
                printf("%02X", ((BondInfo_t *)BOND_INFO_BASE)[e].csrk[i]);
            }
            printf("\n");
            printf("    IRK (Identity Resolving Key): 0x");
            for (signed int i = 15; i >= 0; i--)
            {
                printf("%02X", ((BondInfo_t *)BOND_INFO_BASE)[e].irk[i]);
            }
            printf("\n");
            printf("    RAND (Random Key): 0x");
            for (signed int i = 7; i >= 0; i--)
            {
                printf("%02X", ((BondInfo_t *)BOND_INFO_BASE)[e].rand[i]);
            }
            printf("\n");
            found = 1;
        }
    }
    if (!found)
    {
        printf("  (none)\n");
    }

}


void printTrimCalibrationValues(TRIM_Type *trim_values)
{
    /* print bandgap voltage trim values */
    if( (trim_values->bandgap[0].target > 0) && (trim_values->bandgap[0].target < 0xFF) )
    {
        printf("  Bandgap slope: 0x%02X\n", trim_values->bandgap[0].slope);
        printf("  Bandgap %u mV: 0x%02X\n", trim_values->bandgap[0].target * 10, trim_values->bandgap[0].trim);
    }

    /* print bandgap current trim values */
    if( (trim_values->bandgap[1].target > 0) && (trim_values->bandgap[1].target < 0xFF) )
    {
        printf("  Bandgap slope: 0x%02X\n", trim_values->bandgap[1].slope);
        printf("  Bandgap %u nA: 0x%02X\n", trim_values->bandgap[1].target * 10, trim_values->bandgap[1].trim);
    }

    /* print DCDC voltage trim values */
    for (uint8_t i = 0; i < 4; i++)
    {
        if( (trim_values->dcdc[i].target > 0) && (trim_values->dcdc[i].target < 0xFFFF) )
            printf("  DCDC %u mV: 0x%02X\n", trim_values->dcdc[i].target *10, trim_values->dcdc[i].DCDC_trim);
    }

    /* print VDDC voltage trim values */
    for (uint8_t i = 0; i < 3; i++)
    {
        if( (trim_values->vddc[i].target_voltage > 0) && (trim_values->vddc[i].target_voltage < 0xFF) )
            printf("  VDDC %u mV: 0x%02X\n", trim_values->vddc[i].target_voltage *10, trim_values->vddc[i].trim_voltage);
    }

    /* print VDDM voltage trim values */
    for (uint8_t i = 0; i < 3; i++)
    {
        if( (trim_values->vddm[i].target_voltage > 0) && (trim_values->vddm[i].target_voltage < 0xFF) )
            printf("  VDDM %u mV: 0x%02X\n", trim_values->vddm[i].target_voltage *10, trim_values->vddm[i].trim_voltage);
    }

    /* print VDDRF voltage trim values */
    for (uint8_t i = 0; i < 4; i++)
    {
        if( (trim_values->vddrf[i].target > 0) && (trim_values->vddrf[i].trim < 0xFFFF) )
            printf("  VDDRF %u mV: 0x%02X\n", trim_values->vddrf[i].target *10, trim_values->vddrf[i].trim);
    }

    /* print VDDPA voltage trim values */
    for (uint8_t i = 0; i < 3; i++)
    {
        if( (trim_values->vddpa[i].target_voltage > 0) && (trim_values->vddpa[i].trim_voltage < 0xFF) )
            printf("  VDDPA %u mV: 0x%02X\n", trim_values->vddpa[i].target_voltage *10, trim_values->vddpa[i].trim_voltage);
    }

    /* print VDDIF voltage trim values */
    if((trim_values->vddif[0].target > 0 ) && (trim_values->vddif[0].target < 0xFFFF))
        printf("  VDDIF %u mV: 0x%02X\n", trim_values->vddif[0].target *10, trim_values->vddif[0].trim);

    /* print VDDFLASH voltage trim values */
    if((trim_values->vddflash[0].target > 0 ) && (trim_values->vddflash[0].target < 0xFFFF))
        printf("  VDDFLASH %u mV: 0x%02X\n", trim_values->vddflash[0].target *10, trim_values->vddflash[0].trim);

    /* print RC oscillator trim values */
    for (uint8_t i = 0; i < 4; i++)
    {
        if((trim_values->rcosc[i*2].target > 0 ) && (trim_values->rcosc[i*2].target < 0xFFFF))
            printf("  RC %u kHz: 0x%02X\n", trim_values->rcosc[i*2].target, trim_values->rcosc[i*2].trim);
    }

    /* print 32kHz oscillator trim values */
    if((trim_values->rcosc32[0].target > 0 ) && (trim_values->rcosc32[0].target < 0xFFFF))
    {
        printf("  RC32 %u Hz: 0x%02X\n", trim_values->rcosc32[0].target, trim_values->rcosc32[0].trim);
        printf("\n");
    }
    /* LSAD gain-offset values */
    if((trim_values->lsad_trim.hf_offset > 0) && (trim_values->lsad_trim.hf_offset < 0xFFFF))
    {
        printf("  LSAD high frequency offset: 0x%02X\n", trim_values->lsad_trim.hf_offset);
        printf("  LSAD high frequency gain: 0x%04lX\n", trim_values->lsad_trim.hf_gain);
        printf("\n");
    }
    if((trim_values->lsad_trim.lf_offset > 0) && (trim_values->lsad_trim.lf_offset < 0xFFFF))
    {
        printf("  LSAD low frequency offset: 0x%02X\n", trim_values->lsad_trim.lf_offset);
        printf("  LSAD low frequency gain: 0x%04lX\n", trim_values->lsad_trim.lf_gain);
        printf("\n");
    }


    /* Temperature offset-gain values */
    if((trim_values->temp_sensor.offset > 0) && (trim_values->temp_sensor.offset < 0xFFFF))
    {
        printf("  Temperature Sensor offset: 0x%04lX\n", trim_values->temp_sensor.offset);
        printf("  Temperature Sensor gain: 0x%04lX\n", trim_values->temp_sensor.gain);
        printf("\n");
    }


    /* Thermistor */
    if((trim_values->thermistor[0].bias > 0) && (trim_values->thermistor[0].bias < 0xFFFF))
    {
        printf("  Thermistor bias %u uA: 0x%02X\n", trim_values->thermistor[0].bias, trim_values->thermistor[0].current_trim);
        printf("\n");
    }
    if((trim_values->thermistor[1].bias > 0) && (trim_values->thermistor[1].bias < 0xFFFF))
    {
        printf("  Thermistor bias %u uA: 0x%02X\n", trim_values->thermistor[1].bias, trim_values->thermistor[1].current_trim);
        printf("\n");
    }
    /* Sensor TRIM Values */
    /* Temperature Sensor measured at 30'C */
    if(trim_values->measured.temp_sensor_30C != 0xFFFF)
        printf("  TEMP SENSOR measured value at 30'C +/-5%%: %u\n", trim_values->measured.temp_sensor_30C);

    /* Temperature Sensor measured at 90'C on RSL15 */
    if(trim_values->measured.temp_sensor_high != 0xFFFF)
        printf("  TEMP SENSOR measured value at 50'C +/-5%%: %u\n", trim_values->measured.temp_sensor_high);

    /* Bandgap measured value for vref at 0.75v */
    if(trim_values->measured.bandgap_vref_0_75V != 0xFFFF)
        printf("  BG measured value for vref 0.75 v: %u\n", trim_values->measured.bandgap_vref_0_75V);

    /* Bandgap measured value for vref at 1.2v */
    if(trim_values->measured.bandgap_vref_1_2V_buf != 0xFFFF)
        printf("  BG measured value for vref buffer 1.2 v: %u\n", trim_values->measured.bandgap_vref_1_2V_buf);

    /* LSAD measured value for vref at 1.0v */
    if(trim_values->measured.lsad_vref_1_0V_internal != 0xFFFF)
        printf("  LSAD measured value at vref 1.0 v: %u\n", trim_values->measured.lsad_vref_1_0V_internal);

    /* WEDAC measured at 600mV */
    if(trim_values->measured.wedac_600mV != 0xFFFF)
        printf("  WEDAC measured value at 600 mV: %u\n", trim_values->measured.wedac_600mV);

    /* WEDAC measured at 616mV */
    if(trim_values->measured.wedac_616mV != 0xFFFF)
        printf("  WEDAC measured value at 616 mV: %u\n", trim_values->measured.wedac_616mV);
    printf("\n");

    printf("  Version: ");

    if((trim_values->SOS_REV & 0xFFFF) != 0xFFFF)
    {
        printf("0x%lX", trim_values->SOS_REV);
    }
    else
    {
        printf("(none)");
    }
    printf("\n");

    printf("  Default TRIM CRC: 0x%08lX\n", trim_values->CHECKSUM);
    printf("\n\n");
}
