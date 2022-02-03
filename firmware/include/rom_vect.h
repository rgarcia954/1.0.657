/**
 * @file romvect.h
 * @brief Header file for ROM based API functions for flash management
 *        through the flash interface
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

#ifndef ROMVECT_H_
#define ROMVECT_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* Program ROM Jump table base address */
#define ROMJUMPTABLE_BASEADDR                0x0000001CU

/* Program ROM version base address */
#define ROMVERSION_BASEADDR                  0x00000010U

/* Program ROM flash library version base address */
#define FLASHVERSION_BASEADDR               (ROMVERSION_BASEADDR + 0x08U)

/* Program ROM functions */
#define ROMVECT_SYS_INITIALIZE_BASE         (ROMJUMPTABLE_BASEADDR + 0x00U)
#define ROMVECT_SYS_DELAY                   (ROMJUMPTABLE_BASEADDR + 0x04U)

/* Boot ROM support functions */
#define ROMVECT_VALIDATE_APP                (ROMJUMPTABLE_BASEADDR + 0x08U)
#define ROMVECT_START_APP                   (ROMJUMPTABLE_BASEADDR + 0x0CU)

/* Flash library functions */
#define ROMVECT_FLASH_INITIALIZE            (ROMJUMPTABLE_BASEADDR + 0x10U)
#define ROMVECT_FLASH_WRITEWORD             (ROMJUMPTABLE_BASEADDR + 0x14U)
#define ROMVECT_FLASH_READWORD              (ROMJUMPTABLE_BASEADDR + 0x18U)
#define ROMVECT_FLASH_WRITEDOUBLE           (ROMJUMPTABLE_BASEADDR + 0x1CU)
#define ROMVECT_FLASH_READDOUBLE            (ROMJUMPTABLE_BASEADDR + 0x20U)
#define ROMVECT_FLASH_WRITEBUFFER           (ROMJUMPTABLE_BASEADDR + 0x24U)
#define ROMVECT_FLASH_READBUFFER            (ROMJUMPTABLE_BASEADDR + 0x28U)
#define ROMVECT_FLASH_ERASEFLASHBANK        (ROMJUMPTABLE_BASEADDR + 0x2CU)
#define ROMVECT_FLASH_ERASECHIP             (ROMJUMPTABLE_BASEADDR + 0x30U)
#define ROMVECT_FLASH_ERASESECTOR           (ROMJUMPTABLE_BASEADDR + 0x34U)
#define ROMVECT_FLASH_BLANKCHECK            (ROMJUMPTABLE_BASEADDR + 0x38U)

/* Read MNVR is outside of the flash library */
#define ROMVECT_READ_MNVR                   (ROMJUMPTABLE_BASEADDR + 0x3CU)

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

/** @brief Define the return status values from ROM functions */
typedef enum
{
    BOOTROM_ERR_NONE = 0x0,
    BOOTROM_ERR_BAD_ALIGN = 0x1,
    BOOTROM_ERR_BAD_SP = 0x2,
    BOOTROM_ERR_BAD_RESET_VECT = 0x3,
    BOOTROM_ERR_FAILED_START_APP = 0x6,
    BOOTROM_ERR_BAD_CRC = 0x7
} BootROMStatus;

/** @brief Provide a type definition for the application validation function. */
typedef BootROMStatus (*BOOTROM_ValidateApp_t)(uint32_t* app_addr);

/** @brief Provide a type definition for the application start function. */
typedef BootROMStatus (*BOOTROM_StartApp_t)(uint32_t* app_addr);

/** @brief Provide a type definition for the function used to read from MNVR. */
typedef void (*BOOTROM_Read_MNVR_t)(uint32_t addr, uint32_t *word, uint8_t *readECC);

/*----------------------------------------------------------------------------
  ROM functions
 *----------------------------------------------------------------------------*/
/**
 * @brief Delay by at least the specified number of clock cycles.
 *
 * @param [in] cycles Count of the system clock cycles to delay.
 *
 * @remark The requested delay is at least 15 cycles (15us at 1MHz)
 * and at most 0xFFFFFFFF cycles (approx 214.75s at 20MHz).
 * @remark A delay between (cycles) and (cycles + 3) provides a
 * sufficient delay resolution.
 * @remark The requested delay does not exceed the watchdog timeout.
 */
#define Sys_Delay(cycles)               (*((void(**)(uint32_t))ROMVECT_SYS_DELAY))(cycles)

/**
 * @brief Determines whether the address pointed to is the start of a
 * valid application image.
 *
 * @param app_addr The start address of the application. This should point to
 * the start of the vector table for the application.
 *
 * @return Status code indicating whether there is a validation
 * error or not. One of BOOTROM_ERR_*
 */
#define Sys_ValidateApp(app_addr)       (*((BOOTROM_ValidateApp_t *) ROMVECT_VALIDATE_APP))(app_addr)

/**
 * @brief Validates and starts an application image.
 *
 * @param app_addr A pointer to the start of the application to start.
 *
 * @return Status code indicating application validation error if application
 * cannot be started. If not returning, the status code is written to the top
 * of the started application's stack to capture non-fatal validation issues.
 */
#define Sys_StartApp(app_addr)          (*((BOOTROM_StartApp_t *) ROMVECT_START_APP))(app_addr)

/**
 * @brief Routine to read data from the Manufacture Non-Volatile Record (MNVR).
 * @details
 * This routine will read a 32 bit value from the MNVR.
 * @param addr The address to read the data from, this must reside in the MNVR
 * address range.
 * @param word A pointer to the location to which the 32 bit data will be
 * written.
 */
#define Sys_Read_MNVR(addr, word)          (*((BOOTROM_Read_MNVR_t *) ROMVECT_READ_MNVR))(((addr)|0x01), word, 0)

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif

#endif /* ROMVECT_H_ */
