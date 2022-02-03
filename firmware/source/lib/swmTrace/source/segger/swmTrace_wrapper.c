/**
 * @file swmTrace_wrapper.c
 * @brief This provides the tracing implementation for the SEGGER_RTT mode.
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

#include "SEGGER_RTT.h"
#include "swmTrace_int.h"

void swmTrace_init(const uint32_t *configuration, const uint32_t size)
{
    swmTrace_internal_init(configuration, size);
    SEGGER_RTT_Init();
}

bool swmTrace_txInProgress(void)
{
    return false;
}

void swmTrace_vprintf(const char *sFormat, va_list *pParamList)
{
    SEGGER_RTT_vprintf(0, sFormat, pParamList);
}

bool swmTrace_getch(char *ch)
{
    // not implemented, return no data available
    return false;
}

