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
#include <app_customsc.h>
#include <app_basc.h>
#include <app_init.h>
#include <app_msg_handler.h>

/** APP Task messages */
enum appm_msg
{
    APPM_DUMMY_MSG = TASK_FIRST_MSG(TASK_ID_APP),
    APP_LED_TIMEOUT
};

/* ----------------------------------------------------------------------------
 * Defines
 * -------------------------------------------------------------------------- */
/* Advertising extension
 * Set this define to 1 to enable advertisement extension */
#define ADV_EXTENSION                   0

#define APP_BLE_DEV_PARAM_SOURCE        APP_PROVIDED /* or APP_PROVIDED FLASH_PROVIDED_or_DFLT */

/* Location of BLE public address
 *   - BLE public address location in MNVR is used as a default value;
 *   - Any other valid locations can be used as needed.
 */
#define APP_BLE_PUBLIC_ADDR_LOC			BLE_PUBLIC_ADDR_LOC_MNVR

#define APP_PUBLIC_ADDRESS              {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB}

#define GAPM_CFG_ADDR_PUBLIC            (0 << GAPM_PRIV_CFG_PRIV_ADDR_POS)
#define GAPM_CFG_ADDR_PRIVATE           (1 << GAPM_PRIV_CFG_PRIV_ADDR_POS)

#define GAPM_CFG_HOST_PRIVACY           (0 << GAPM_PRIV_CFG_PRIV_EN_POS)
#define GAPM_CFG_CONTROLLER_PRIVACY     (1 << GAPM_PRIV_CFG_PRIV_EN_POS)

#define GAPM_ADDRESS_TYPE               GAPM_CFG_ADDR_PRIVATE
#define GAPM_PRIVACY_TYPE               GAPM_CFG_HOST_PRIVACY

#define APP_BD_RENEW_DUR                150 /* in seconds */
#define GAPM_OWN_ADDR_TYPE              GAPM_STATIC_ADDR /* GAPM_STATIC_ADDR, GAPM_GEN_NON_RSLV_ADDR */
/* BLE private address of local device */
#define APP_BLE_PRIVATE_ADDR            { 0xD1, 0x11, 0xD1, 0x11, 0xD1, 0xD1 }
#define SECURE_CONNECTION               1  /* set 0 for LEGACY_CONNECTION or 1 for SECURE_CONNECTION */

/* The number of standard profiles and custom services added in this application */
#define APP_NUM_STD_PRF                 1
#define APP_NUM_CUST_SVC              	1

#define LED_GPIO_NUM                    8  /* GPIO number that is connected to LED of EVB */

#define LOW_POWER_CLOCK_ACCURACY        500     /* ppm */
#define TWOSC                           1200    /* us */
#define MAX_SLEEP_DURATION              0x17700 /* 30s */

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

/* Timer setting in units of 1ms (kernel timer resolution) */
#define TIMER_SETTING_MS(MS)            MS
#define TIMER_SETTING_S(S)              (S * 1000)

/* Bluetooth address of the first peer device */
#define APP_BD_ADDRESS_PEER1            { 0x94, 0x11, 0x22, 0xff, 0xbb, 0xD5 }
#define APP_BD_ADDRESS_PEER_TYPE1       ADDR_RAND

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

/* Application communicates PHY update request, comment out if is not required */
/* #define APP_PHY_UPDATE_REQ */

/* Based on enum gap_phy */
#define APP_PREFERRED_PHY_RX            GAP_PHY_LE_CODED
#define APP_PREFERRED_PHY_TX            GAP_PHY_LE_CODED

/* Based on enum gapc_phy_option */
#define APP_PREFERRED_CODED_PHY_RATE    GAPC_PHY_OPT_LE_CODED_125K_RATE

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

