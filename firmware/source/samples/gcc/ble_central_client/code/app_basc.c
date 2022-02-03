/**
 * @file app_basc.c
 * @brief BASC (Battery Service Client) application-specific source file
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

#include <app_basc.h>
#include <ble_basc.h>
#include <swmTrace_api.h>

void APP_BASC_BattLevelInd_Handler(ke_msg_id_t const msg_id,
                                   void const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    swmLogInfo("\n__Battery level\n(peer=%d): %d%%\n\r", (uint8_t)KE_IDX_GET(src_id),
           BASC_GetLastBatteryLevel((uint8_t)KE_IDX_GET(src_id), 0));
}

