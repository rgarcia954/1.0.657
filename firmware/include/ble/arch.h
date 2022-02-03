/**
 ****************************************************************************************
 *
 * @file arch.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */


#ifndef _ARCH_H_
#define _ARCH_H_

/**
 ****************************************************************************************
 * @defgroup REFIP
 * @brief Reference IP Platform
 *
 * This module contains reference platform components - REFIP.
 *
 *
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup DRIVERS
 * @ingroup REFIP
 * @brief Reference IP Platform Drivers
 *
 * This module contains the necessary drivers to run the platform with the
 * RW BT SW protocol stack.
 *
 * This has the declaration of the platform architecture API.
 *
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>        // standard integer definition
#include "compiler.h"      // inline functions

/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
/// ARM is a 32-bit CPU
#define CPU_WORD_SIZE   4

/*
 * CPU Endianness
 ****************************************************************************************
 */
/// ARM is little endian
#define CPU_LE          1

/*
 * DEBUG configuration
 ****************************************************************************************
 */
#if defined(CFG_DBG)
#define PLF_DEBUG          1
#else //CFG_DBG
#define PLF_DEBUG          0
#endif //CFG_DBG


#if defined(CFG_PROFILING)
#define PLF_PROFILING      1
#else //CFG_DBG
#define PLF_PROFILING      0
#endif //CFG_PROFILING


/*
 * NVDS
 ****************************************************************************************
 */

/// NVDS
#ifdef CFG_NVDS
#define PLF_NVDS             1
#else // CFG_NVDS
#define PLF_NVDS             0
#endif // CFG_NVDS


/*
 * UART
 ****************************************************************************************
 */

/// UART
#define PLF_UART             1

/// UART 2
//MON-42
#define PLF_UART2            0


/*
 * DEFINES
 ****************************************************************************************
 */

/// Possible errors detected by FW
#define    RESET_NO_ERROR         0x00000000
#define    RESET_MEM_ALLOC_FAIL   0xF2F2F2F2

/// Reset platform and stay in ROM
#define    RESET_TO_ROM           0xA5A5A5A5
/// Reset platform and reload FW
#define    RESET_AND_LOAD_FW      0xC3C3C3C3

/// Exchange memory size limit
#if (defined(CFG_BT_EMB) && defined (CFG_BLE_EMB))
#define    EM_SIZE_LIMIT          0x10000
#else
#define    EM_SIZE_LIMIT          0x8000
#endif // (defined(CFG_BT_EMB) && defined (CFG_BLE_EMB))

/**
 * EM Fetch time (in us)
 *  - EM fetch: 30us (worst case at 26Mhz)
 *  - HW logic: 10us (worst case at 26Mhz)
 */
#define PLF_EM_FETCH_TIME_US        40

/**
 * EM update time (in us):
 *    - HW CS Update is 18 access
 *    - HW Tx Desc Update is 1 access
 *    - HW Rx Desc Update is 5 access
 *        => EM update at 26MHz Tx, Rx and CS is (18+1+5)*0.04*4 = 4us
 *    - HW logic: 10us (worst case)
 */
#define PLF_EM_UPDATE_TIME_US       14

/*
 * EXPORTED FUNCTION DECLARATION
 ****************************************************************************************
 */

#if (defined(CFG_DBG_STACK_PROF))
/**
****************************************************************************************
* @brief Initialise stack memory area.
*
* This function initialises the stack memory with pattern for use in stack profiling.
****************************************************************************************
*/
void stack_init(void);

/**
 ****************************************************************************************
 * @brief Compute size of SW stack used.
 *
 * This function is compute the maximum size stack used by SW.
 *
 * @return Size of stack used (in bytes)
 ****************************************************************************************
 */
uint16_t get_stack_usage(void);
#endif //(defined(CFG_DBG_STACK_PROF))

/**
 ****************************************************************************************
 * @brief Re-boot FW.
 *
 * This function is used to re-boot the FW when error has been detected, it is the end of
 * the current FW execution.
 * After waiting transfers on UART to be finished, and storing the information that
 * FW has re-booted by itself in a non-loaded area, the FW restart by branching at FW
 * entry point.
 *
 * Note: when calling this function, the code after it will not be executed.
 *
 * @param[in] error      Error detected by FW
 ****************************************************************************************
 */
void platform_reset(uint32_t error);

#if PLF_DEBUG
/**
 ****************************************************************************************
 * @brief Print the assertion error reason and loop forever.
 *
 * @param condition C string containing the condition.
 * @param file C string containing file where the assertion is located.
 * @param line Line number in the file where the assertion is located.
 ****************************************************************************************
 */
void assert_err(const char *condition, const char * file, int line);

/**
 ****************************************************************************************
 * @brief Print the assertion error reason and loop forever.
 * The parameter value that is causing the assertion will also be disclosed.
 *
 * @param param0 parameter value 0.
 * @param param1 parameter value 1.
 * @param file C string containing file where the assertion is located.
 * @param line Line number in the file where the assertion is located.
 ****************************************************************************************
 */
void assert_param(int param0, int param1, const char * file, int line);

/**
 ****************************************************************************************
 * @brief Print the assertion warning reason.
 *
 * @param param0 parameter value 0.
 * @param param1 parameter value 1.
 * @param file C string containing file where the assertion is located.
 * @param line Line number in the file where the assertion is located.
 ****************************************************************************************
 */
void assert_warn(int param0, int param1, const char * file, int line);


/**
 ****************************************************************************************
 * @brief Dump data value into FW.
 *
 * @param data start pointer of the data.
 * @param length data size to dump
 ****************************************************************************************
 */
void dump_data(uint8_t* data, uint16_t length);
#endif // PLF_DEBUG

#if PLF_PROFILING
/**
 ****************************************************************************************
 * @brief Trace enter into a function
 *
 * @param[in] p_func_ptr Pointer of the function
 * @param[in] p_func_name_ptr Pointer of the function name
 ****************************************************************************************
 */
void func_enter(const void* p_func_ptr, const void* p_func_name_ptr);

/**
 ****************************************************************************************
 * @brief Trace exit of a function
 *
 * @param[in] p_func_ptr      Pointer of the function
 * @param[in] p_func_name_ptr Pointer of the function name
 ****************************************************************************************
 */
void func_exit(const void* p_func_ptr, const void* p_func_name_ptr);

/**
 ****************************************************************************************
 * @brief Trace data pointer allocation
 *
 * @param[in] p_ptr Data pointer address
 ****************************************************************************************
 */
void data_trace_alloc(const void* p_ptr);

/**
 ****************************************************************************************
 * @brief Trace data pointer free
 *
 * @param[in] p_ptr Data pointer address
 ****************************************************************************************
 */
void data_trace_free(const void* p_ptr);

/**
 ****************************************************************************************
 * @brief Trace data into a VCD
 *
 * @param[in] p_ptr      Data pointer address
 * @param[in] p_name_ptr Data variable name pointer address
 * @param[in] data_size  Size of data to trace in bytes (8, 16 or 32 only)
 ****************************************************************************************
 */
void data_trace(const void* p_ptr, const void* p_name_ptr, uint8_t data_size);

#endif // PLF_PROFILING

/*
 * ASSERTION CHECK
 ****************************************************************************************
 */
#if PLF_DEBUG
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)                              \
    do {                                              \
        if (!(cond)) {                                \
            assert_err(#cond, __MODULE__, __LINE__);  \
        }                                             \
    } while(0)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_INFO(cond, param0, param1)             \
    do {                                              \
        if (!(cond)) {                                \
            assert_param((int)param0, (int)param1, __MODULE__, __LINE__);  \
        }                                             \
    } while(0)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond, param0, param1)             \
    do {                                              \
        if (!(cond)) {                                \
            assert_warn((int)param0, (int)param1, __MODULE__, __LINE__); \
        }                                             \
    } while(0)

/// DUMP data array present in the SW.
#define DUMP_DATA(data, length) \
    dump_data((uint8_t*)data, length)

#else
/// Assertions showing a critical error that could require a full system reset
#define ASSERT_ERR(cond)

/// Assertions showing a critical error that could require a full system reset
#define ASSERT_INFO(cond, param0, param1)

/// Assertions showing a non-critical problem that has to be fixed by the SW
#define ASSERT_WARN(cond, param0, param1)

/// DUMP data array present in the SW.
#define DUMP_DATA(data, length)
#endif //PLF_DEBUG

#if (PLF_PROFILING)
/// Trace data into a VCD
#define DBG_DATA_TRACE(data, size) data_trace(&data, #data, size)

/// Trace data allocation
#define DBG_DATA_ALLOC(data) data_trace_alloc(&data)

/// Trace data free
#define DBG_DATA_FREE(data) data_trace_free(&data)

/// Trace Function Enter
#define DBG_FUNC_ENTER(func) func_enter(func, #func)

/// Trace Function Exit
#define DBG_FUNC_EXIT(func) func_exit(func, #func)
#else
/// Trace data into a VCD
#define DBG_DATA_TRACE(data, size)
/// Trace data allocation
#define DBG_DATA_ALLOC(data)
/// Trace data free
#define DBG_DATA_FREE(data)
/// Trace Function Enter
#define DBG_FUNC_ENTER(func)
/// Trace Function Exit
#define DBG_FUNC_EXIT(func)
#endif //PLF_PROFILING


/// Object allocated in shared memory - check linker script
#define __SHARED __attribute__ ((section("shram")))

// required to define GLOBAL_INT_** macros as inline assembly. This file is included after
// definition of ASSERT macros as they are used inside ll.h
#include "ll.h"     // ll definitions
/// @} DRIVERS
#endif // _ARCH_H_
