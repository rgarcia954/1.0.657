/**
 * @file swmTrace.c
 * @brief This provides the high level implementation of the SWM Trace features.
 * @details
 * This module devolves the specific implementation details for each tracing
 * type to their dedicated implementation files. Only common code exists in
 * this file.
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

#include "swmTrace_int.h"

/**
 * @brief Defines the selected log level. This can be configured during the
 * logging initialization. Only log messages which have a level equal or
 * greater than this will be output.
 */
static uint32_t swmTrace_LogLevel = SWM_LOG_LEVEL_WARNING;

/**
 * @brief Helper method to mark the different log levels so they can be
 * easily identified at the trace target.
 * @param level Defines level to be marked, must be one of the SWM_LOG_LEVEL_*
 * macros definitions, anything else will be ignored.
 */
static void swmLogPrintMarker(uint32_t level)
{
    switch (level)
    {
    case SWM_LOG_LEVEL_VERBOSE:
        swmTrace_printf("-V-");
        break;
    case SWM_LOG_LEVEL_INFO:
        swmTrace_printf("-I-");
        break;
    case SWM_LOG_LEVEL_WARNING:
        swmTrace_printf("-W-");
        break;
    case SWM_LOG_LEVEL_ERROR:
        swmTrace_printf("-E-");
        break;
    case SWM_LOG_LEVEL_FATAL:
        swmTrace_printf("-F-");
        break;
    case SWM_LOG_TEST_PASS:
        swmTrace_printf("-PASS-");
        break;
    case SWM_LOG_TEST_FAIL:
        swmTrace_printf("-FAIL-");
        break;
    default:
        // Error case, but ignore it
        break;
    }
}

void swmTrace_internal_init(const uint32_t *configuration, uint32_t size)
{
    while (size-- > 0)
    {
        switch (*configuration)
        {
        case SWM_LOG_LEVEL_VERBOSE:
        case SWM_LOG_LEVEL_INFO:
        case SWM_LOG_LEVEL_WARNING:
        case SWM_LOG_LEVEL_ERROR:
        case SWM_LOG_LEVEL_FATAL:
        case SWM_LOG_TEST_PASS:
        case SWM_LOG_TEST_FAIL:
            swmTrace_LogLevel = *configuration;
            break;
        default:
            // If we don't recognize an option, just ignore it
            break;
        }
        configuration++;
    }
}

void swmTrace_printf(const char *sFormat, ...)
{
    va_list args;
    va_start(args, sFormat);
    swmTrace_vprintf(sFormat, &args);
    va_end(args);
}

void swmLog(uint32_t level, const char *sFormat, ...)
{
    if (level >= swmTrace_LogLevel)
    {
        swmLogPrintMarker(level);
        va_list args;
        va_start(args, sFormat);
        swmTrace_vprintf(sFormat, &args);
        va_end(args);
    }
}

