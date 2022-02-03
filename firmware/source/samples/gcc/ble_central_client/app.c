/**
 * @file app.c
 * @brief Main application source file
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

#include <app.h>

int main()
{
    DisableAppInterrupts();

    /* Configure clocks, GPIOs, trace interface and load calibration data */
    DeviceInit();

    swmLogInfo("__ble_central_client has started.\r\n");

    /* Initialize the Kernel and create application task */
    BLEStackInit();

    /* Initialize Bluetooth Services */
    BatteryServiceClientInit();
    CustomServiceClientInit();

    /* Subscribe application callback handlers to BLE events */
    AppMsgHandlersInit();

    /* Send a message to the BLE stack requesting a reset.
     * The stack returns a GAPM_CMT_EVT / GAPM_RESET event upon completion.
     * See BLE_ConfigHandler to follow what happens next. */
    GAPM_SoftwareReset(); /* Step 1 */

    EnableAppInterrupts();

    while(1)
    {
    	SYS_WATCHDOG_REFRESH();
        BLE_Kernel_Process();
        __WFI();
    }
}
