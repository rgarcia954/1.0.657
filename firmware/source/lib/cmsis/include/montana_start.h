/**
 * @file montana_start.h
 * @brief CMSIS Cortex-M33 compatible header file for application initialization.
 *
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

#ifndef MONTANA_START_H
#define MONTANA_START_H

/* Needed includes */
#include <sys/types.h>
#include <errno.h>
#include <montana_map.h>
#include <montana_hw.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/** @addtogroup CMSIS
 *  @{
 */

/* ----------------------------------------------------------------------------
 * Linker provided defines that define the heap
 * ------------------------------------------------------------------------- */
extern uint8_t __Heap_Begin__;          /**< Start location for the heap */
extern uint8_t __Heap_Limit__;          /**< Top limit for the heap */

/* ----------------------------------------------------------------------------
 * Linker provided defines that define the stack
 * ------------------------------------------------------------------------- */
extern uint32_t __stack_limit;          /**< Bottom limit for the stack */
extern uint32_t __stack;                /**< Start location for the stack */

/* ----------------------------------------------------------------------------
 * Linker provided defines for memory initialization
 * ------------------------------------------------------------------------- */
extern uint32_t __data_init__;          /**< Pointer to the data to used to initialize volatile memory */
extern uint32_t __data_start__;         /**< Start address of the initialized data area in volatile memory */
extern uint32_t __data_end__;           /**< End address of the initialized data area in volatile memory */

extern uint32_t __bss_start__;          /**< Start address of the cleared data area in volatile memory */
extern uint32_t __bss_end__;            /**< End address of the cleared data area in volatile memory */

/* ----------------------------------------------------------------------------
 * Linker provided function lists for application initialization
 * ------------------------------------------------------------------------- */
/** @brief Weakly defined function list pointer for pre-initialization functions */
extern void(*__preinit_array_start__[]) (void) __attribute__ ((weak));

/** @brief Weakly defined pointer to the end of the pre-initialization function list */
extern void(*__preinit_array_end__[]) (void) __attribute__ ((weak));

/** @brief Weakly defined function list pointer for initialization functions */
extern void(*__init_array_start__[]) (void) __attribute__ ((weak));

/** @brief Weakly defined pointer to the end of the initialization function list */
extern void(*__init_array_end__[]) (void) __attribute__ ((weak));

/* ----------------------------------------------------------------------------
 * Function prototypes
 * ------------------------------------------------------------------------- */
/**
 * @brief       Initialize the application data and start execution with main.
 *              Should be called from the reset vector.
 * @return      None
 * @assumptions The symbols __data_init__, __data_start__, __data_end__,
 *              __bss_start__, __bss_end__, and __stack_limit__ are defined when the
 *              application is linked.
 * @assumptions The symbol flash_layout exists, and is an array of structures containing
 *              the start, end, and FLASH_Type* of all the banks of flash, in ascending
 *              memory address order, that the data region could be present in.
 */
extern void _start(void);

/**
 * @brief       Increment (or decrement) the top of the heap
 * @param[in]   increment    Increment to be applied to the top of the heap
 * @return      The prior value of the heap top (points to the base of the
 *              newly allocated data if the heap was incremented); returns
 *              -1 if the function was unable to allocate the requested
 *              memory
 * @assumptions The symbols __Heap_Begin__, __Heap_Limit__ are defined
 *              when the application is linked.
 */
extern int8_t * _sbrk(int increment);

/* ----------------------------------------------------------------------------
 * Flash layout for DATA region copying
 * ------------------------------------------------------------------------- */
/** @brief Structure used to define flash regions */
struct flash_region
{
    uint32_t start;                     /**< First address in the flash region */
    uint32_t end;                       /**< Last address in the flash region */
    FLASH_Type *flash;                  /**< Flash interface for this region */
    uint32_t IRQn;                      /**< Interrupt supporting this flash region */
};

/** @brief Flash layout for the Montana device */
static const struct flash_region flash_layout[] =
{
    {
        .start = FLASH0_CODE_BASE,
        .end = FLASH0_CODE_TOP,
        .flash = FLASH0,
        .IRQn = FLASH0_COPY_IRQn,
    },
    {
        .start = FLASH1_CODE_BASE,
        .end = FLASH1_CODE_TOP,
        .flash = FLASH1,
        .IRQn = FLASH1_COPY_IRQn,
    },
    {
        .start = FLASH0_DATA_BASE,
        .end = FLASH0_DATA_TOP,
        .flash = FLASH0,
        .IRQn = FLASH0_COPY_IRQn,
    },
    {
        .start = FLASH1_DATA_BASE,
        .end = FLASH1_DATA_TOP,
        .flash = FLASH1,
        .IRQn = FLASH1_COPY_IRQn,
    },
};

/** @} */ /* End of the CMSIS group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* MONTANA_START_H */
