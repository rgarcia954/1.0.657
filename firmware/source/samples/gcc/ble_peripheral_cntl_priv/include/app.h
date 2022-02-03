/**
 * @file app.h
 * @brief Main application header
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

#ifndef APP_H
#define APP_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */


/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
/* Device and libraries headers */
#include <hw.h>
#include <swmTrace_api.h>
#include <flash_rom.h>
#include <ble_protocol_support.h>
#include <ble_abstraction.h>

/* Application headers */
#include <app_init.h>
#include <app_msg_handler.h>

/* APP Task messages */
enum appm_msg
{
    APPM_DUMMY_MSG = TASK_FIRST_MSG(TASK_ID_APP),
    APP_LED_TIMEOUT,
	APP_ADV_WL_FILTERING
};

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#define APP_BLE_DEV_PARAM_SOURCE       APP_PROVIDED /* or APP_PROVIDED FLASH_PROVIDED_or_DFLT */

/* Location of BLE public address
 *   - BLE public address location in MNVR is used as a default value;
 *   - Any other valid locations can be used as needed.
 */
#define APP_BLE_PUBLIC_ADDR_LOC			BLE_PUBLIC_ADDR_LOC_MNVR

/* Advertising channel map - 37, 38, 39 */
#define APP_ADV_CHMAP                  GAPM_DEFAULT_ADV_CHMAP

/* Advertising minimum interval - 40ms (64*0.625ms) */
#define APP_ADV_INT_MIN                GAPM_DEFAULT_ADV_INTV_MIN

/* Advertising maximum interval - 40ms (64*0.625ms) */
#define APP_ADV_INT_MAX                GAPM_DEFAULT_ADV_INTV_MAX

#define APP_PUBLIC_ADDRESS             { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB }

#define GAPM_CFG_ADDR_PUBLIC           (0 << GAPM_PRIV_CFG_PRIV_ADDR_POS)
#define GAPM_CFG_ADDR_PRIVATE          (1 << GAPM_PRIV_CFG_PRIV_ADDR_POS)

#define GAPM_CFG_HOST_PRIVACY          (0 << GAPM_PRIV_CFG_PRIV_EN_POS)
#define GAPM_CFG_CONTROLLER_PRIVACY    (1 << GAPM_PRIV_CFG_PRIV_EN_POS)

#define GAPM_ADDRESS_TYPE              GAPM_CFG_ADDR_PRIVATE
#define GAPM_PRIVACY_TYPE              GAPM_CFG_CONTROLLER_PRIVACY

#define APP_BD_RENEW_DUR               150 /* in seconds */

#define GAPM_OWN_ADDR_TYPE             GAPM_GEN_RSLV_ADDR /* GAPM_STATIC_ADDR, GAPM_GEN_RSLV_ADDR */

/* BLE private address of local device */
#define APP_BLE_PRIVATE_ADDR           { 0x94, 0x11, 0x22, 0xff, 0xCC, 0xCC }

#define SECURE_CONNECTION               1  /* set 0 for LEGACY_CONNECTION or 1 for SECURE_CONNECTION */

/* The number of standard profiles and custom services added in this application */
#define APP_NUM_STD_PRF                 0
#define APP_NUM_CUST_SVC              	0

#define BLE_STATES_GPIO                 8  /* GPIO number that is used for status of the application */

#define LOW_POWER_CLOCK_ACCURACY        500     /* ppm */
#define TWOSC        		 		    1200    /* us */
#define MAX_SLEEP_DURATION			    0x17700 /* 30s */

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/* Enable/disable buck converter
 * Options: VCC_BUCK or VCC_LDO
 */
#define VCC_BUCK_LDO_CTRL               VCC_BUCK

/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO                   0

/* Default LSAD channel for SetTxPower */
#define LSAD_TXPWR_DEF                  1

/* Default TX power setting */
#define DEF_TX_POWER                    0

/* Timer setting in units of 1ms (kernel timer resolution) */
#define TIMER_SETTING_MS(MS)            MS
#define TIMER_SETTING_S(S)              (S * 1000)

/* Advertising data is composed by device name and company id */
#define APP_DEVICE_NAME                 "ble_periph_server_ctrl"
#define APP_DEVICE_NAME_LEN             sizeof(APP_DEVICE_NAME)-1
/* Manufacturer info (onsemi Company ID) */
#define APP_COMPANY_ID                  { 0x62,0x3 }
#define APP_COMPANY_ID_LEN              2

#define APP_DEVICE_APPEARANCE           0
#define APP_PREF_SLV_MIN_CON_INTERVAL   8
#define APP_PREF_SLV_MAX_CON_INTERVAL   10
#define APP_PREF_SLV_LATENCY            0
#define APP_PREF_SLV_SUP_TIMEOUT        200

/* Application-provided IRK */
#define APP_IRK                         { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

/* Application-provided CSRK */
#define APP_CSRK                        { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

/* The GPIO pin to use for TX when using the UART mode */
#define UART_TX_GPIO    				(6)

/* The GPIO pin to use for RX when using the UART mode */
#define UART_RX_GPIO    				(5)

/* The selected baud rate for the application when using UART mode */
#define UART_BAUD       				(115200)

/* ---------------------------------------------------------------------------
* Function prototype definitions
* --------------------------------------------------------------------------*/
void APP_SendConCfm(uint8_t conidx);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */

