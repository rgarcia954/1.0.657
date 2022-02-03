/**
 * @file swmTrace_api.h
 * @brief This provides the high level API access to the swmTrace features.
 * @details
 * This provides the high level API function interfaces for the swmTrace
 * functionality.
 *
 * This trace feature allows the selection of tracing to multiple different
 * output devices by linking in separate libraries rather than changing the
 * application code.
 *
 * At present, five types of tracing are supported by this API:
 *     - Segger RTT in blocking mode
 *     - Segger RTT in non-blocking mode
 *     - UART with user defined pins for Tx & Rx
 *     - UART implemented using DMA, with user defined pins for Tx & Rx
 *     - Semi Hosting
 *
 * @note
 * When using the SemiHosting library, the user application needs to also
 * link correctly for semi-hosting operation. i.e. add the following to the
 * link command:
 *     --specs=rdimon.specs
 *
 * @note
 * When using the UART, there is a dependency on the SystemCoreClock variable,
 * so this must be accurate to ensure the correct behaviour of the ports. It
 * is the responsibility of the user application to set this value accordingly
 * if the core clock is changed at any time.
 *
 * @ingroup SWMTRACEg
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

#ifndef INCLUDE_SWMTRACE_API_H_
#define INCLUDE_SWMTRACE_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "swmTrace_options.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SWMTRACEg swmTrace Reference
  * The swmTrace library functions and macros provide a logging utility that
  * helps you to debug an application running on the Arm Cortex-M33 core.
  * @{
  */
#if (SWMTRACE_ENABLEMENT == SWMTRACE_ENABLED)

/**
 * @brief Trace initialization function.
 * @details
 * This method allows the tracing functions to be initialized in a
 * general way, allowing different trace options to be supplied, depending
 * on the type of trace library selected.
 * @param [in] configuration Consists of an array of 32-bit words that define
 * the selected initialization options.
 * @param [in] size Indicates the number of options provided.
 * @note
 * The list of options can be set up as a superset of all the possible options,
 * and only the ones required for a given trace library are used. The
 * possible options are defined in the swmTrace_options.h file.
 */
void swmTrace_init(const uint32_t *configuration, uint32_t size);

/**
 * @brief Provides indication if transmission is in progress.
 * @return True if a string is being transmitted; false otherwise.
 */
bool swmTrace_txInProgress(void);

/**
 * @brief This provides a printf-like implementation for all possible trace
 * mechanisms.
 * @param[in] sFormat Defines the format of the string to print. This is followed
 * by a variable number of arguments.
 */
void swmTrace_printf(const char *sFormat, ...);

/**
 * @brief This provides a vprintf-like implementation for all possible trace
 * mechanisms.
 * @param[in] sFormat Defines the format of the string to print.
 * @param[in] pParamList Defines a pointer to a va_list object in a form similar to
 * vprintf.
 */
void swmTrace_vprintf(const char *sFormat, va_list *pParamList);

/**
 * @brief A method to allow characters to be passed from the logging target
 * to the traced application.
 * @param[in] ch A pointer to a character object which holds the returned
 * character.
 * @return True if a valid character has been returned; false otherwise.
 */
bool swmTrace_getch(char *ch);

/**
 * @brief A general logging method that allows us to output only trace messages
 * if a particular log level has been selected.
 * @param[in] level The level of this log message. Only messages which have a level
 * equal to or higher than the currently selected level are output.
 * @param[in] sFormat The format of the output string, as per printf.
 */
void swmLog(uint32_t level, const char *sFormat, ...);

#else /* SWMTRACE_ENABLEMENT == SWMTRACE_ENABLED */

#define swmTrace_init(...)
#define swmTrace_txInProgress(...)  false
#define swmTrace_printf(...)
#define swmTrace_vprintf(...)
#define swmTrace_getch(...)     false
#define swmLog(...)

#endif /* SWMTRACE_ENABLEMENT == SWMTRACE_ENABLED */

/**
 * @brief Shortcut macro for verbose logging.
 */
#define swmLogVerbose(...)  swmLog(SWM_LOG_LEVEL_VERBOSE, __VA_ARGS__)

/**
 * @brief Shortcut macro for informational logging.
 */
#define swmLogInfo(...)     swmLog(SWM_LOG_LEVEL_INFO, __VA_ARGS__)

/**
 * @brief Shortcut macro for warnings.
 */
#define swmLogWarn(...)     swmLog(SWM_LOG_LEVEL_WARNING, __VA_ARGS__)

/**
 * @brief Shortcut macro for errors.
 */
#define swmLogError(...)    swmLog(SWM_LOG_LEVEL_ERROR, __VA_ARGS__)

/**
 * @brief Shortcut macro for fatal errors.
 */
#define swmLogFatal(...)    swmLog(SWM_LOG_LEVEL_FATAL, __VA_ARGS__)

/**
 * @brief Shortcut macro for test PASS indicators.
 */
#define swmLogTestPass(...)    swmLog(SWM_LOG_TEST_PASS, __VA_ARGS__)

/**
 * @brief Shortcut macro for test FAIL indicators.
 */
#define swmLogTestFail(...)    swmLog(SWM_LOG_TEST_FAIL, __VA_ARGS__)

/** @} */ /* End of the SWMTRACEg group */

#ifdef __cplusplus
    }
#endif

#endif /* INCLUDE_SWMTRACE_API_H_ */
