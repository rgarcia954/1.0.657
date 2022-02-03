/**
 * @file app_init.c
 * @brief Source file for application initialization
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

static uint32_t traceOptions[] = {
  SWM_LOG_LEVEL_INFO,			  /* In all cases log info messages */
  SWM_UART_RX_PIN | UART_RX_GPIO, /* Set RX pin for cases when using UART */
  SWM_UART_TX_PIN | UART_TX_GPIO, /* Set TX pin for cases when using UART */
  SWM_UART_RX_ENABLE,             /* Enable the UART Rx Interrupts */
  SWM_UART_BAUD_RATE | UART_BAUD  /* Set the baud rate as defined above */
};

void DeviceInit(void)
{
    /* Hold application here if recovery GPIO is held low during boot.
     * This makes it easier for the debugger to connect and reprogram the device. */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
       SYS_WATCHDOG_REFRESH();
    }

    /* Load default trim values. */
    uint32_t trim_error __attribute__((unused)) = SYS_TRIM_LOAD_DEFAULT();

    /* Start 48 MHz XTAL oscillator */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    Sys_RFFE_SetTXPower(0, LSAD_TXPWR_DEF, VDDPA_EN);

    Flash_Initialize(0, SystemCoreClock);

    /* Set ICH_TRIM for optimum RF performance */
    ACS->VCC_CTRL = (((ACS->VCC_CTRL) & (~(ACS_VCC_CTRL_ICH_TRIM_Mask))) |
            	     ((uint32_t)(0x5U << ACS_VCC_CTRL_ICH_TRIM_Pos)));
    
    /* Enable/disable buck converter */
    ACS->VCC_CTRL = (((ACS->VCC_CTRL) & (~(VCC_BUCK))) | (VCC_BUCK_LDO_CTRL));

	/* Load custom trim values */
    uint32_t custom_trim_error __attribute__((unused)) = SYS_TRIM_LOAD_CUSTOM();

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Configure Baseband Controller Interface */
    BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8);

    /* Disable JTAG TDI, TDO, and TRST connections to GPIO 2, 3, and 4 */
    GPIO->JTAG_SW_PAD_CFG &= ~(CM33_JTAG_DATA_ENABLED | CM33_JTAG_TRST_ENABLED);
    GPIO->CFG[2] = GPIO_MODE_GPIO_OUT;
    GPIO->CFG[3] = GPIO_MODE_GPIO_OUT;
    Sys_GPIO_Set_Low(2);
    Sys_GPIO_Set_Low(3);

    /* Initialize trace library */
    swmTrace_init(traceOptions,5);
}

void AppMsgHandlersInit(void)
{
    /* BLE Database setup handler */
    MsgHandler_Add(GAPM_CMP_EVT, BLE_ConfigHandler);
    MsgHandler_Add(GAPM_PROFILE_ADDED_IND, BLE_ConfigHandler);
    MsgHandler_Add(GATTM_ADD_SVC_RSP, BLE_ConfigHandler);

    /* BLE Activity handler (responsible for air operations) */
    MsgHandler_Add(GAPM_ACTIVITY_CREATED_IND, BLE_ActivityHandler);
    MsgHandler_Add(GAPM_ACTIVITY_STOPPED_IND, BLE_ActivityHandler);

    /* Connection handler */
    MsgHandler_Add(GAPM_CMP_EVT, BLE_ConnectionHandler);
    MsgHandler_Add(GAPC_CONNECTION_REQ_IND, BLE_ConnectionHandler);
    MsgHandler_Add(GAPC_DISCONNECT_IND, BLE_ConnectionHandler);
    MsgHandler_Add(GAPM_ADDR_SOLVED_IND, BLE_ConnectionHandler);
    MsgHandler_Add(GAPC_PARAM_UPDATE_REQ_IND, BLE_ConnectionHandler);

    /* Pairing / bonding  handler */
    MsgHandler_Add(GAPC_BOND_REQ_IND, BLE_PairingHandler);
    MsgHandler_Add(GAPC_BOND_IND, BLE_PairingHandler);
    MsgHandler_Add(GAPC_ENCRYPT_REQ_IND, BLE_PairingHandler);
    MsgHandler_Add(GAPC_ENCRYPT_IND, BLE_PairingHandler);

    /* LED Blink handler (control the LED blinking frequency
     * according to number of connected peer devices) */
    MsgHandler_Add(APP_LED_TIMEOUT, LEDHandler);
}

void BatteryServiceClientInit(void)
{
    BASC_Initialize();
    BASC_RequestBattLevelOnTimeout(TIMER_SETTING_S(5));
    MsgHandler_Add(BASC_BATT_LEVEL_IND, APP_BASC_BattLevelInd_Handler);
}

void CustomServiceClientInit(void)
{
    CUSTOMSC_Initialize();
}

void BLEStackInit(void)
{
    uint8_t param_ptr;
    BLE_Initialize(&param_ptr);
    
	/* BLE_Initialize() initialized a number of trim registers
     * using default values from the BLE stack,
     * SYS_TRIM_LOAD_CUSTOM() ensures custom trim values are used. */
	uint32_t custom_trim_error __attribute__((unused)) = SYS_TRIM_LOAD_CUSTOM();
	
	ke_task_create(TASK_APP, MsgHandler_GetTaskAppDesc());
    Device_BLE_Public_Address_Read((uint32_t)APP_BLE_PUBLIC_ADDR_LOC);
}

void DisableAppInterrupts(void)
{
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);
}

void EnableAppInterrupts(void)
{
    NVIC_ClearPendingIRQ(BLE_HSLOT_IRQn);
    NVIC_ClearPendingIRQ(BLE_SLP_IRQn);
    NVIC_ClearPendingIRQ(BLE_FIFO_IRQn);
    NVIC_ClearPendingIRQ(BLE_CRYPT_IRQn);
    NVIC_ClearPendingIRQ(BLE_ERROR_IRQn);
    NVIC_ClearPendingIRQ(BLE_TIMESTAMP_TGT1_IRQn);
    NVIC_ClearPendingIRQ(BLE_FINETGT_IRQn);
    NVIC_ClearPendingIRQ(BLE_TIMESTAMP_TGT2_IRQn);
    NVIC_ClearPendingIRQ(BLE_SW_IRQn);
    NVIC_EnableIRQ(BLE_HSLOT_IRQn);
    NVIC_EnableIRQ(BLE_SLP_IRQn);
    NVIC_EnableIRQ(BLE_FIFO_IRQn);
    NVIC_EnableIRQ(BLE_CRYPT_IRQn);
    NVIC_EnableIRQ(BLE_ERROR_IRQn);
    NVIC_EnableIRQ(BLE_TIMESTAMP_TGT1_IRQn);
    NVIC_EnableIRQ(BLE_FINETGT_IRQn);
    NVIC_EnableIRQ(BLE_TIMESTAMP_TGT2_IRQn);
    NVIC_EnableIRQ(BLE_SW_IRQn);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
}

