/**
 * @file montana.h
 * @brief This provides the high level API access to the SWM Trace features.
 * 
 * Top-level CMSIS Cortex-M33 compatible header file
 * This trace feature allows the selection of tracing to multiple different
 * output devices by linking in separate libraries rather than changing the
 * application code.
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
 
#ifndef MONTANA_H
#define MONTANA_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/** @defgroup CMSIS CMSIS Reference
 *  Hardware register abstraction layer for the SOC
 *  @{
 */

/* ----------------------------------------------------------------------------
 * Version Code
 * ------------------------------------------------------------------------- */
/** @brief Montana header file major version */
#define MONTANA_SYS_VER_MAJOR           0x02

/** @brief Montana header file minor version */
#define MONTANA_SYS_VER_MINOR           0x01

/** @brief Montana header file revision version */
#define MONTANA_SYS_VER_REVISION        0x00

/** @brief Montana firmware version */
#define MONTANA_SYS_VER                 ((MONTANA_SYS_VER_MAJOR << 12) | \
                                         (MONTANA_SYS_VER_MINOR << 8)  | \
                                         (MONTANA_SYS_VER_REVISION))

/** @brief Montana firmware version (variable) */
extern const short MONTANA_Sys_Version;

/* ----------------------------------------------------------------------------
 * Standard Include Files
 * ------------------------------------------------------------------------- */
#include <stdbool.h>

/* ----------------------------------------------------------------------------
 * Interrupt and Exception Vector Definitions
 * ------------------------------------------------------------------------- */
#include <montana_vectors.h>

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
/* Anonymous unions are enabled by default */
#elif defined (__TMS470__)
/* Anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
/* Anonymous unions are enabled by default */
#else /* if defined (__CC_ARM) */
  #warning Not supported compiler type
#endif /* if defined (__CC_ARM) */

/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __ARMv8MML_REV            0x0000U   /**< Arm v8 architecture revision */
#define __CM33_REV                0x0000U   /**< Core revision r0p4 */
#define __FPU_PRESENT             1U        /**< FPU present */
#define __DSP_PRESENT             1U        /**< DSP extension present */
#define __SAUREGION_PRESENT       1U        /**< SAU regions present */
#define __MPU_PRESENT             1U        /**< MPU present */
#define __VTOR_PRESENT            1U        /**< VTOR present */
#define __NVIC_PRIO_BITS          3U        /**< 3 bits used for interrupt priority levels */
#define __Vendor_SysTickConfig    0U        /**< Standard SysTick configuration is used */


/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)

/* Leave anonymous unions enabled */
#elif (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)

/* Anonymous unions are enabled by default */
#elif defined (__TMS470__)

/* Anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)

/* Anonymous unions are enabled by default */
#else /* if defined (__CC_ARM) */
  #warning Not supported compiler type
#endif


#include <core_cm33.h>
#include <system_montana.h>
#include <montana_start.h>


/* ----------------------------------------------------------------------------
 * Peripheral Register Definitions
 * ------------------------------------------------------------------------- */
#include <montana_hw.h>

#if defined (__CC_ARM)
#pragma no_anon_unions
#endif /* if defined (__CC_ARM) */

/* ----------------------------------------------------------------------------
 * Memory Map Definitions
 * ------------------------------------------------------------------------- */
#include <montana_map.h>

/* ----------------------------------------------------------------------------
 * Inline functions supporting functions exposed through the ROM jump table
 * ------------------------------------------------------------------------- */
#include <rom_vect.h>

/* ----------------------------------------------------------------------------
 * Additional Peripheral Support Definitions, Macros and Inline Functions
 * ------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
 * Macros for asserting valid instance for array type of peripherals
 * ------------------------------------------------------------------------- */

/** Validation of I2C register block pointer reference for assert statements. */
#define I2C_REF_VALID(ref)              (((uint32_t)(ref) == (uint32_t)I2C) | \
                                         ((uint32_t)(ref) == (uint32_t)I2C0))

/** Validation of SPI register block pointer reference for assert statements. */
#define SPI_REF_VALID(ref)              (((uint32_t)(ref) == (uint32_t)SPI) | \
                                         ((uint32_t)(ref) == (uint32_t)SPI0))

/** Validation of UART register block pointer reference for assert statements. */
#define UART_REF_VALID(ref)             (((uint32_t)(ref) == (uint32_t)UART) | \
                                         ((uint32_t)(ref) == (uint32_t)UART0))


/** Validation of TIMER register block pointer reference for assert statements. */
#define TIMER_REF_VALID(ref)            (((uint32_t)(ref) == (uint32_t)TIMER) | \
                                         ((uint32_t)(ref) == (uint32_t)TIMER0) | \
                                         ((uint32_t)(ref) == (uint32_t)TIMER1) | \
                                         ((uint32_t)(ref) == (uint32_t)TIMER2) | \
                                         ((uint32_t)(ref) == (uint32_t)TIMER3))

/** Validation of DMA register block pointer reference for assert statements. */
#define DMA_REF_VALID(ref)              (((uint32_t)(ref) == (uint32_t)DMA) | \
                                         ((uint32_t)(ref) == (uint32_t)DMA0) | \
                                         ((uint32_t)(ref) == (uint32_t)DMA1) | \
                                         ((uint32_t)(ref) == (uint32_t)DMA2) | \
                                         ((uint32_t)(ref) == (uint32_t)DMA3))

/** Validation of FLASH register block pointer reference for assert statements. */
#define FLASH_REF_VALID(ref)            (((uint32_t)(ref) == (uint32_t)FLASH) | \
                                         ((uint32_t)(ref) == (uint32_t)FLASH0) | \
                                         ((uint32_t)(ref) == (uint32_t)FLASH1))

/* For peripherals/interfaces not mentioned above a check is not required by firmware */
/* ----------------------------------------------------------------------------
 * End of Macros for asserting valid instance for array type of peripherals
 * ------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
 * GPIO pads related definitions
 * ------------------------------------------------------------------------- */

/** GPIO peripheral definitions */
#define GPIO_PAD_COUNT                  16 /**< Maximum number of GPIO pads */
#define GPIO_GROUP_LOW_PAD_RANGE        16 /**< Number of GPIO pads in the lowest group (all) */
#define GPIO_EVENT_CHANNEL_COUNT        8  /**< Number of available GPIO interrupts */   
#define GPIO_CLK_DIV_COUNT              0  /**< GPIO clock divisors */  

/** GPIO pads definitions  */
#define GPIO0                           0 /**< GPIO 0 */
#define GPIO1                           1 /**< GPIO 1 */
#define GPIO2                           2 /**< GPIO 2 */
#define GPIO3                           3 /**< GPIO 3 */
#define GPIO4                           4 /**< GPIO 4 */
#define GPIO5                           5 /**< GPIO 5 */
#define GPIO6                           6 /**< GPIO 6 */
#define GPIO7                           7 /**< GPIO 7 */
#define GPIO8                           8 /**< GPIO 8 */
#define GPIO9                           9 /**< GPIO 9 */
#define GPIO10                          10 /**< GPIO 10 */
#define GPIO11                          11 /**< GPIO 11 */
#define GPIO12                          12 /**< GPIO 12 */
#define GPIO13                          13 /**< GPIO 13 */
#define GPIO14                          14 /**< GPIO 14 */
#define GPIO15                          15 /**< GPIO 15 */

/* ----------------------------------------------------------------------------
 * End of GPIO pads related definitions
 * ------------------------------------------------------------------------- */

/* Define registers that can be used for a dummy read (read of 0x00000000 from
 * a write-only register) and a dummy write (ineffective write to a read-only
 * register). These defines can be used without impacting protected memory or
 * register contents in cases where a write or a read operation is needed,
 * but the result of doesn't matter.
 */
/** @brief Register that always reads back as 0x00000000 */
#define SYS_DUMMY_READ                  SYSCTRL->PROD_STATUS

/** @brief Register to which writes are ineffective */
#define SYS_DUMMY_WRITE                 SYSCTRL->CC_DCU_EN0

/* ----------------------------------------------------------------------------
 * End of Additional Peripheral Support Definitions, Macros and Inline Functions
 * ------------------------------------------------------------------------- */
 
/* ----------------------------------------------------------------------------
 * HAL include definitions
 * ------------------------------------------------------------------------- */
/* Pure C implementations */
#include <sassert.h>

/* Hardware specific include files */
#include <acnt.h>
#include <crc.h>
#include <dma.h>
#include <flash_copier.h>
#include <gpio.h>
#include <i2c.h>
#include <lsad.h>
#include <rffe.h>
#include <rtc.h>
#include <sensor.h>
#include <spi.h>
#include <timer.h>
#include <tof.h>
#include <trim.h>
#include <uart.h>
#include <watchdog.h>

/* Hardware specific include files only allowed in Secure mode */
#ifndef NON_SECURE
#include <bbif.h>
#include <clock.h>
#include <nvic.h>
#include <power.h>
#include <power_modes.h>
#endif /* ifndef NON_SECURE */

/* ----------------------------------------------------------------------------
 * End of HAL include definitions
 * ------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
 * Generic error code definitions
 * ------------------------------------------------------------------------- */
/** No error */
#define ERRNO_NO_ERROR                  0x0000

/** General error */
#define ERRNO_GENERAL_FAILURE           0x0001

/** @} */ /* End of the CMSIS group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* MONTANA_H */
