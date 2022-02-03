/**
 * @file montana_map.h
 * @brief Memory map for Montana
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

#ifndef MONTANA_MAP_H
#define MONTANA_MAP_H

/* ------------------------------------------------------------------------ */
/* Instruction and Data Bus Memory Mapping */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* Boot ROM (PROM)            : 32K bytes                                   */
/* ------------------------------------------------------------------------ */
#define PROM_BASE                       0x00000000
#define PROM_TOP                        0x00007FFF
#define PROM_SIZE                       (PROM_TOP - PROM_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 Code Redundancy 1   : 2K bytes                                    */
/* ------------------------------------------------------------------------ */
#define FLASH0_CODE_REDUNDANCY_1_BASE   0x00060000
#define FLASH0_CODE_REDUNDANCY_1_TOP    0x000607FF
#define FLASH0_CODE_REDUNDANCY_1_SIZE   (FLASH0_CODE_REDUNDANCY_1_TOP - FLASH0_CODE_REDUNDANCY_1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 Code Redundancy 2   : 2K bytes                                    */
/* ------------------------------------------------------------------------ */
#define FLASH0_CODE_REDUNDANCY_2_BASE   0x00060800
#define FLASH0_CODE_REDUNDANCY_2_TOP    0x00060FFF
#define FLASH0_CODE_REDUNDANCY_2_SIZE   (FLASH0_CODE_REDUNDANCY_2_TOP - FLASH0_CODE_REDUNDANCY_2_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 Data Redundancy 1   : 256 bytes                                   */
/* ------------------------------------------------------------------------ */
#define FLASH0_DATA_REDUNDANCY_1_BASE   0x00061000
#define FLASH0_DATA_REDUNDANCY_1_TOP    0x000610FF
#define FLASH0_DATA_REDUNDANCY_1_SIZE   (FLASH0_DATA_REDUNDANCY_1_TOP - FLASH0_DATA_REDUNDANCY_1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 Data Redundancy 2   : 256 bytes                                   */
/* ------------------------------------------------------------------------ */
#define FLASH0_DATA_REDUNDANCY_2_BASE   0x00061100
#define FLASH0_DATA_REDUNDANCY_2_TOP    0x000611FF
#define FLASH0_DATA_REDUNDANCY_2_SIZE   (FLASH0_DATA_REDUNDANCY_2_TOP - FLASH0_DATA_REDUNDANCY_2_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 Code Redundancy 1   : 2K bytes                                    */
/* ------------------------------------------------------------------------ */
#define FLASH1_CODE_REDUNDANCY_1_BASE   0x00062000
#define FLASH1_CODE_REDUNDANCY_1_TOP    0x000627FF
#define FLASH1_CODE_REDUNDANCY_1_SIZE   (FLASH1_CODE_REDUNDANCY_1_TOP - FLASH1_CODE_REDUNDANCY_1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 Code Redundancy 2   : 2K bytes                                    */
/* ------------------------------------------------------------------------ */
#define FLASH1_CODE_REDUNDANCY_2_BASE   0x00062800
#define FLASH1_CODE_REDUNDANCY_2_TOP    0x00062FFF
#define FLASH1_CODE_REDUNDANCY_2_SIZE   (FLASH1_CODE_REDUNDANCY_2_TOP - FLASH1_CODE_REDUNDANCY_2_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 Data Redundancy 1   : 256 bytes                                   */
/* ------------------------------------------------------------------------ */
#define FLASH1_DATA_REDUNDANCY_1_BASE   0x00063000
#define FLASH1_DATA_REDUNDANCY_1_TOP    0x000630FF
#define FLASH1_DATA_REDUNDANCY_1_SIZE   (FLASH1_DATA_REDUNDANCY_1_TOP - FLASH1_DATA_REDUNDANCY_1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 Data Redundancy 2   : 256 bytes                                   */
/* ------------------------------------------------------------------------ */
#define FLASH1_DATA_REDUNDANCY_2_BASE   0x00063100
#define FLASH1_DATA_REDUNDANCY_2_TOP    0x000631FF
#define FLASH1_DATA_REDUNDANCY_2_SIZE   (FLASH1_DATA_REDUNDANCY_2_TOP - FLASH1_DATA_REDUNDANCY_2_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 NVR0          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR0_BASE                0x00080000
#define FLASH0_NVR0_TOP                 0x000800FF
#define FLASH0_NVR0_SIZE                (FLASH0_NVR0_TOP - FLASH0_NVR0_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 NVR1          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR1_BASE                0x00080100
#define FLASH0_NVR1_TOP                 0x000801FF
#define FLASH0_NVR1_SIZE                (FLASH0_NVR1_TOP - FLASH0_NVR1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 NVR2          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR2_BASE                0x00080200
#define FLASH0_NVR2_TOP                 0x000802FF
#define FLASH0_NVR2_SIZE                (FLASH0_NVR2_TOP - FLASH0_NVR2_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 NVR3          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR3_BASE                0x00080300
#define FLASH0_NVR3_TOP                 0x000803FF
#define FLASH0_NVR3_SIZE                (FLASH0_NVR3_TOP - FLASH0_NVR3_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 NVR4          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR4_BASE                0x00080400
#define FLASH0_NVR4_TOP                 0x000804FF
#define FLASH0_NVR4_SIZE                (FLASH0_NVR4_TOP - FLASH0_NVR4_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 NVR5          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR5_BASE                0x00080500
#define FLASH0_NVR5_TOP                 0x000805FF
#define FLASH0_NVR5_SIZE                (FLASH0_NVR5_TOP - FLASH0_NVR5_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash0 NVR6          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR6_BASE                0x00080600
#define FLASH0_NVR6_TOP                 0x000806FF
#define FLASH0_NVR6_SIZE                (FLASH0_NVR6_TOP - FLASH0_NVR6_BASE + 1)

/* NVR6 memory locations */
#define TRIM_CUSTOM_BASE                FLASH0_NVR6_BASE
#define TRIM_CUSTOM_SIGNATURE_OFFSET    0x00U
#define TRIM_CUSTOM_ICH_OFFSET          0x04U
#define TRIM_CUSTOM_XTAL_OFFSET         0x08U
#define TRIM_CUSTOM_TIMESTAMP_OFFSET    0xF0U
#define TRIM_CUSTOM_TESTER_CODE_OFFSET  0xF4U
#define TRIM_CUSTOM_CRC_OFFSET          0xFCU						   

/* ------------------------------------------------------------------------ */
/* Flash0 NVR7          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_NVR7_BASE                0x00080700
#define FLASH0_NVR7_TOP                 0x000807FF
#define FLASH0_NVR7_SIZE                (FLASH0_NVR7_TOP - FLASH0_NVR7_BASE + 1)

/** @brief Number of records in area */
#define TRIM_RECORDS                    4
#define TRIM_RC_RECORDS                 2
#define TRIM_THERMISTOR_RECORDS         2

/** @brief Number of total records */
#define TRIM_TOTAL_RECORDS              27

/** @brief Trim memory locations */
#define TRIM_BASE_DEFAULT               FLASH0_NVR7_BASE
#define TRIM_BANDGAP_VOLTAGE_OFFSET     0x00U
#define TRIM_BANDGAP_CURRENT_OFFSET     0x01U
#define TRIM_DCDC_OFFSET                0x04U
#define TRIM_VDDC_OFFSET                0x08U
#define TRIM_VDDM_OFFSET                0x0CU
#define TRIM_VDDRF_OFFSET               0x10U
#define TRIM_VDDPA_OFFSET               0x14U
#define TRIM_VDDIF_OFFSET               0x18U
#define TRIM_VDDFLASH_OFFSET            0x1CU
#define TRIM_RCOSC_OFFSET               0x20U
#define TRIM_RCOSC32_OFFSET             0x28U
#define TRIM_LSAD_OFFSET                0x2CU
#define TRIM_TEMP_OFFSET                0x30U
#define TRIM_THERMISTOR_OFFSET          0x34U
#define TRIM_GAIN_OFFSET                0x38U
#define TRIM_SOS_OFFSET                 0x3EU
#define TRIM_CRC_OFFSET                 0x3FU

/* ------------------------------------------------------------------------ */
/* Flash0 MNVR          : 128 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH0_MNVR_BASE                0x00080800
#define FLASH0_MNVR_TOP                 0x000808FF
#define FLASH0_MNVR_SIZE                (FLASH0_MNVR_TOP - FLASH0_MNVR_BASE + 1)

/* Structure for trim values stored in flash 0 MNVR. */
struct BANDGAP_TRIM
{
    __IO uint8_t trim;
    __IO uint8_t slope;
    __IO uint16_t target;
};

struct DCDC_TRIM
{
    __IO uint8_t LDO_trim;
    __IO uint8_t DCDC_trim;
    __IO uint16_t target;
};

struct VDDCM_TRIM
{
    __IO uint8_t trim_voltage;
    __IO uint8_t trim_standby;
    __IO uint8_t target_voltage;
    __IO uint8_t target_standby;
};

struct VDDRF_TRIM
{
    __IO uint16_t trim;
    __IO uint16_t target;
};

struct VDDPA_TRIM
{
    __IO uint16_t trim_voltage;
    __IO uint16_t target_voltage;
};

struct VDDIF_TRIM
{
    __IO uint16_t trim;
    __IO uint16_t target;
};

struct VDDFLASH_TRIM
{
    __IO uint16_t trim;
    __IO uint16_t target;
};

struct RCOSC_TRIM
{
    __IO uint16_t trim;
    __IO uint16_t target;
};

struct RCOSC32_TRIM
{
    __IO uint16_t trim;
    __IO uint16_t target;
};

struct LSAD_TRIM
{
    __IO uint16_t lf_offset;
    __IO uint16_t hf_offset;
    __IO uint32_t lf_gain;
    __IO uint32_t hf_gain;
};

struct TEMP_SENSOR_TRIM
{
    __IO uint32_t offset;
    __IO uint32_t gain;
};

struct THERMISTOR_TRIM
{
    __IO uint8_t current_trim;
    __IO uint8_t current_value;
    __IO uint16_t bias;
};

struct MEASURED_TRIM
{
    __IO uint16_t temp_sensor_high;
    __IO uint16_t temp_sensor_30C;
    __IO uint16_t bandgap_vref_1_2V_buf;
    __IO uint16_t bandgap_vref_0_75V;
    __IO uint16_t lsad_vref_1_0V_internal;
    __IO uint16_t RESERVED;
    __IO uint16_t wedac_616mV;
    __IO uint16_t wedac_600mV;
};

typedef struct
{
    __IO struct BANDGAP_TRIM bandgap[TRIM_RECORDS];
    __IO struct DCDC_TRIM dcdc[TRIM_RECORDS];
    __IO struct VDDCM_TRIM vddc[TRIM_RECORDS];
    __IO struct VDDCM_TRIM vddm[TRIM_RECORDS];
    __IO struct VDDRF_TRIM vddrf[TRIM_RECORDS];
    __IO struct VDDPA_TRIM vddpa[TRIM_RECORDS];
    __IO struct VDDIF_TRIM vddif[TRIM_RECORDS];
    __IO struct VDDFLASH_TRIM vddflash[TRIM_RECORDS];
    __IO struct RCOSC_TRIM rcosc[TRIM_RC_RECORDS * 4];
    __IO struct RCOSC32_TRIM rcosc32[TRIM_RECORDS];
    __IO struct LSAD_TRIM lsad_trim;
    uint32_t RESERVED0;
    __IO struct TEMP_SENSOR_TRIM temp_sensor;
    uint32_t RESERVED1[2];
    __IO struct THERMISTOR_TRIM thermistor[TRIM_THERMISTOR_RECORDS];
    uint32_t TR_REG_TRIM_TRIM;
    uint32_t RESERVED3;
    __IO struct MEASURED_TRIM measured;
    __IO uint32_t RESERVED4[2];
    __IO uint32_t SOS_REV;
    __IO uint32_t CHECKSUM;
} TRIM_Type;

/* ------------------------------------------------------------------------ */
/* Flash1 NVR0          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR0_BASE                0x00081000
#define FLASH1_NVR0_TOP                 0x000810FF
#define FLASH1_NVR0_SIZE                (FLASH1_NVR0_TOP - FLASH1_NVR0_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 NVR1          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR1_BASE                0x00081100
#define FLASH1_NVR1_TOP                 0x000811FF
#define FLASH1_NVR1_SIZE                (FLASH1_NVR1_TOP - FLASH1_NVR1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 NVR2          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR2_BASE                0x00081200
#define FLASH1_NVR2_TOP                 0x000812FF
#define FLASH1_NVR2_SIZE                (FLASH1_NVR2_TOP - FLASH1_NVR2_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 NVR3          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR3_BASE                0x00081300
#define FLASH1_NVR3_TOP                 0x000813FF
#define FLASH1_NVR3_SIZE                (FLASH1_NVR3_TOP - FLASH1_NVR3_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 NVR4          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR4_BASE                0x00081400
#define FLASH1_NVR4_TOP                 0x000814FF
#define FLASH1_NVR4_SIZE                (FLASH1_NVR4_TOP - FLASH1_NVR4_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 NVR5          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR5_BASE                0x00081500
#define FLASH1_NVR5_TOP                 0x000815FF
#define FLASH1_NVR5_SIZE                (FLASH1_NVR5_TOP - FLASH1_NVR5_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 NVR6          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR6_BASE                0x00081600
#define FLASH1_NVR6_TOP                 0x000816FF
#define FLASH1_NVR6_SIZE                (FLASH1_NVR6_TOP - FLASH1_NVR6_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 NVR7          : 256 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_NVR7_BASE                0x00081700
#define FLASH1_NVR7_TOP                 0x000817FF
#define FLASH1_NVR7_SIZE                (FLASH1_NVR7_TOP - FLASH1_NVR7_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash1 MNVR          : 128 bytes                                         */
/* ------------------------------------------------------------------------ */
#define FLASH1_MNVR_BASE                0x00081800
#define FLASH1_MNVR_TOP                 0x000818FF
#define FLASH1_MNVR_SIZE                (FLASH1_MNVR_TOP - FLASH1_MNVR_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Flash                : 2*512 kBytes                                      */
/* ------------------------------------------------------------------------ */
#define FLASH0_CODE_BASE                0x00100000
#define FLASH0_CODE_TOP                 0x00157FFF
#define FLASH0_CODE_SIZE                (FLASH0_CODE_TOP - FLASH0_CODE_BASE + 1)

#define FLASH0_DATA_BASE                0x001B0000
#define FLASH0_DATA_TOP                 0x001D7FFF
#define FLASH0_DATA_SIZE                (FLASH0_DATA_TOP - FLASH0_DATA_BASE + 1)

/* Memory reservations for security and Bluetooth use cases */
#define FLASH_DEU_RESERVED_BASE         FLASH0_DATA_BASE
#define FLASH_DEU_RESERVED_SIZE         0xC00
#define FLASH_DEU_RESERVED_TOP          (FLASH0_DATA_BASE + FLASH_DEU_RESERVED_SIZE - 1)

#define FLASH_BOND_INFO_BASE            (FLASH_DEU_RESERVED_TOP + 1)
#define FLASH_BOND_INFO_SIZE            0x800
#define FLASH_BOND_INFO_TOP             (FLASH_BOND_INFO_BASE + FLASH_BOND_INFO_SIZE - 1)

/* ----------------------------------------------------------------------------
 * Device Information
 * ------------------------------------------------------------------------- */
#define DEVICE_INFO_BASE                FLASH0_NVR5_BASE
#define DEVICE_INFO_BLUETOOTH_ADDR      (DEVICE_INFO_BASE + 0x00)
#define DEVICE_INFO_BLUETOOTH_IRK       (DEVICE_INFO_BASE + 0x10)
#define DEVICE_INFO_BLUETOOTH_CSRK      (DEVICE_INFO_BASE + 0x20)

#define FLASH1_CODE_BASE                0x00158000
#define FLASH1_CODE_TOP                 0x001AFFFF
#define FLASH1_CODE_SIZE                (FLASH1_CODE_TOP - FLASH1_CODE_BASE + 1)

#define FLASH1_DATA_BASE                0x001D8000
#define FLASH1_DATA_TOP                 0x001FFFFF
#define FLASH1_DATA_SIZE                (FLASH1_DATA_TOP - FLASH1_DATA_BASE + 1)

/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
/* System Bus Memory Structures                                             */
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* Data RAM 0 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM0_BASE                      0x20000000
#define DRAM0_TOP                       0x20001FFF
#define DRAM0_SIZE                      (DRAM0_TOP - DRAM0_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM 1 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM1_BASE                      0x20002000
#define DRAM1_TOP                       0x20003FFF
#define DRAM1_SIZE                      (DRAM1_TOP - DRAM1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM 2 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM2_BASE                      0x20004000
#define DRAM2_TOP                       0x20005FFF
#define DRAM2_SIZE                      (DRAM2_TOP - DRAM2_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM 3 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM3_BASE                      0x20006000
#define DRAM3_TOP                       0x20007FFF
#define DRAM3_SIZE                      (DRAM3_TOP - DRAM3_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM 4 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM4_BASE                      0x20008000
#define DRAM4_TOP                       0x20009FFF
#define DRAM4_SIZE                      (DRAM4_TOP - DRAM4_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM 5 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM5_BASE                      0x2000A000
#define DRAM5_TOP                       0x2000BFFF
#define DRAM5_SIZE                      (DRAM5_TOP - DRAM5_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM 6 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM6_BASE                      0x2000C000
#define DRAM6_TOP                       0x2000DFFF
#define DRAM6_SIZE                      (DRAM6_TOP - DRAM6_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM 7 :  8 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define DRAM7_BASE                      0x2000E000
#define DRAM7_TOP                       0x2000FFFF
#define DRAM7_SIZE                      (DRAM7_TOP - DRAM7_BASE + 1)

/* ------------------------------------------------------------------------ */
/* NFC Data RAM 0 :  1 kBytes                                                   */
/* ------------------------------------------------------------------------ */
#define NFC_DRAM_BASE                   0x20048000
#define NFC_DRAM_TOP                    0x200483FF
#define NFC_DRAM_SIZE                   (NFC_DRAM_TOP - NFC_DRAM_BASE + 1)
/* ------------------------------------------------------------------------ */

/* Base Band Data RAM 0 :  8 kBytes                                         */
/* ------------------------------------------------------------------------ */
#define BB_DRAM0_BASE                    0x20010000
#define BB_DRAM0_TOP                     0x20011FFF
#define BB_DRAM0_SIZE                    (BB_DRAM0_TOP - BB_DRAM0_BASE + 1)

/* Base Band Data RAM 1 :  8 kBytes                                         */
/* ------------------------------------------------------------------------ */
#define BB_DRAM1_BASE                    0x20012000
#define BB_DRAM1_TOP                     0x20013FFF
#define BB_DRAM1_SIZE                    (BB_DRAM1_TOP - BB_DRAM1_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Data RAM : 7 x 8 kBytes                                                  */
/* ------------------------------------------------------------------------ */
#define DRAM_BASE                       DRAM0_BASE
#define DRAM_TOP                        DRAM7_TOP
#define DRAM_SIZE                       (DRAM_TOP - DRAM_BASE + 1)

/* ------------------------------------------------------------------------ */
/* Base Band Data RAM : 1 x 8 kBytes                                        */
/* ------------------------------------------------------------------------ */
#define BB_DRAM_BASE                    BB_DRAM0_BASE
#define BB_DRAM_TOP                     BB_DRAM1_TOP
#define BB_DRAM_SIZE                    (BB_DRAM_TOP - BB_DRAM_BASE + 1)

/* ----------------------------------------------------------------------------
 * Peripheral Bus Memory-Mapped Control Registers
 * ------------------------------------------------------------------------- */
#define PERIPHERAL_BASE                 0x40000000
#define PERIPHERAL_TOP                  0x40053FFF

/* ----------------------------------------------------------------------------
 * Private Peripheral Bus Internal Memory-Mapped Control Registers
 * ------------------------------------------------------------------------- */
#define PRIVATE_PERIPHERAL_BASE         0xE0000000
#define PRIVATE_PERIPHERAL_SYS_BASE     (PRIVATE_PERIPHERAL_BASE + 0xE000)

/* ----------------------------------------------------------------------------
 * The base address definition for the CryptoCell CC312
 * ------------------------------------------------------------------------- */
#define CC_BASE_OFFSET                  0x40050000

#endif /* MONTANA_MAP_H */
