/**
 * @file swmTrace_int.h
 * @brief This provides internal implementation for the SWM trace module.
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

#ifndef INCLUDE_SWMTRACE_INT_H_
#define INCLUDE_SWMTRACE_INT_H_

#include <swmTrace_api.h>
#include <RTE_Components.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro defining the logger to be of type Segger RTT Non Blocking.
 */
#define SWM_TRACE_SEGGER_RTT_NON_BLOCKING   0

/**
 * @brief Macro defining the logger to be of type Segger RTT Blocking.
 */
#define SWM_TRACE_SEGGER_RTT_BLOCKING       1

/**
 * @brief Macro defining the logger to be of type UART Non Blocking.
 */
#define SWM_TRACE_UART_NON_BLOCKING         2

/**
 * @brief Macro defining the logger to be of type UART with DMA, Non Blocking.
 */
#define SWM_TRACE_UART_DMA_NON_BLOCKING     3

/**
 * @brief Macro defining the logger to be of type Semi Hosting.
 */
#define SWM_TRACE_SEMIHOST_BLOCKING         4

#ifndef SWM_TRACE_TYPE
#error "Pre-processor directive SWM_TRACE_TYPE must be defined."
#endif

#if (SWM_TRACE_TYPE != SWM_TRACE_SEGGER_RTT_NON_BLOCKING)
#if (SWM_TRACE_TYPE != SWM_TRACE_SEGGER_RTT_BLOCKING)
#if (SWM_TRACE_TYPE != SWM_TRACE_UART_NON_BLOCKING)
#if (SWM_TRACE_TYPE != SWM_TRACE_UART_DMA_NON_BLOCKING)
#if (SWM_TRACE_TYPE != SWM_TRACE_SEMIHOST_BLOCKING)
#error "SWM_TRACE_TYPE must be defined as one of known the SWM_TRACE_* values."
#endif /* (SWM_TRACE_TYPE == SWM_TRACE_SEMIHOST_BLOCKING) */
#endif /* (SWM_TRACE_TYPE == SWM_TRACE_UART_DMA_NON_BLOCKING) */
#endif /* (SWM_TRACE_TYPE == SWM_TRACE_UART_NON_BLOCKING) */
#endif /* (SWM_TRACE_TYPE == SWM_TRACE_SEGGER_RTT_BLOCKING) */
#endif /* (SWM_TRACE_TYPE == SWM_TRACE_SEGGER_RTT_NON_BLOCKING) */


/**
 * @brief An internal initialization function, this is used to perform
 * common initialization across all of the different trace mechanisms.
 * @param [in] configuration Consists of an array of 32-bit words that define
 * the selected initialization options.
 * @param [in] size Indicates the number of options provided.
 * @note
 * The list of options can be set up as a superset of all the possible options,
 * and only the ones required for a given trace library are used. The
 * possible options are defined in the swmTrace_options.h file.
 * @note
 * This needs to be called as part of the swmTrace_init function when
 * implementing a new logger.
 */
void swmTrace_internal_init(const uint32_t *configuration, uint32_t size);

#ifdef __cplusplus
    }
#endif

#endif /* INCLUDE_SWMTRACE_INT_H_ */
