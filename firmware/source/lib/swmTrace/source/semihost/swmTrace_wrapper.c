/**
 * @file swmTrace_wrapper.c
 * @brief This provides the tracing implementation for the Semi-hosting mode.
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

#include <stdio.h>

#include "swmTrace_int.h"

extern void initialise_monitor_handles(void);

void swmTrace_init(const uint32_t *configuration, const uint32_t size)
{
    swmTrace_internal_init(configuration, size);
    initialise_monitor_handles();
}

bool swmTrace_txInProgress(void)
{
    return true;
}

void swmTrace_vprintf(const char *sFormat, va_list *pParamList)
{
    vprintf(sFormat, *pParamList);
}

bool swmTrace_getch(char *ch)
{
    // not implemented, return no data available
    return false;
}



