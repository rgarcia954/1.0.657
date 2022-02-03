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
#include <hw.h>
#include <ble.h>
#include <ble_protocol_support.h>
#include <RTE_Device.h>
#include <gpio_driver.h>
/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

#define APP_PUBLIC_ADDRESS              { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB }

#define LED_GPIO                        8     /* GPIO number that is connected to LED of EVB */
#define OUTPUT_POWER_DBM                6     /* RF output power in dBm */
#define RADIO_CLOCK_ACCURACY            20    /* RF Oscillator accuracy in ppm */

#define LOW_POWER_CLOCK_ACCURACY        500     /* ppm */
#define TWOSC                           1200    /* us */
#define MAX_SLEEP_DURATION              0x17700 /* 30s */

#define RF_TESTER_GENERATES_PATTERN     0

/* It should reflect GPIO configuration */
#define DTM_ANTENNA_ID_TO_PATTERN       { 0x0, 0x01, 0x02, 0x03 }

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

/* Timer setting in units of 10ms (kernel timer resolution) */
#define TIMER_SETTING_MS(MS)            MS
#define TIMER_SETTING_S(S)              (S * 1000)

/** The GPIO pin to use for TX when using the UART mode */
#define UART_TX_GPIO                    (6)

/** The GPIO pin to use for RX when using the UART mode */
#define UART_RX_GPIO                    (5)

#define ANT_SW0_GPIO                    (11)
#define ANT_SW1_GPIO                    (12)

/** The LSAD channel used for setting TX power */
#define LSAD_TXPWR_DEF                  (1)

/* Application-provided IRK */
#define APP_IRK                         { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

/* Application-provided CSRK */
#define APP_CSRK                        { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

#define APP_SECURELE_KEYS_FORCED        0

/* Application-provided private key */
#define APP_PRIVATE_KEY                 { 0xEC, 0x89, 0x3C, 0x11, 0xBB, 0x2E, \
                                          0xEB, 0x5C, 0x80, 0x88, 0x63, 0x57, \
                                          0xCC, 0xE2, 0x05, 0x17, 0x20, 0x75, \
                                          0x5A, 0x26, 0x3E, 0x8D, 0xCF, 0x26, \
                                          0x63, 0x1D, 0x26, 0x0B, 0xCE, 0x4D, \
                                          0x9E, 0x07 }

/* Application-provided public key */
#define APP_PUBLIC_KEY                  { 0x56, 0x09, 0x79, 0x1D, 0x5A, 0x5F, \
                                          0x4A, 0x5C, 0xFE, 0x89, 0x56, 0xEC, \
                                          0xE6, 0xF7, 0x92, 0x21, 0xAC, 0x93, \
                                          0x99, 0x10, 0x51, 0x82, 0xF4, 0xDD, \
                                          0x84, 0x07, 0x50, 0x99, 0xE7, 0xC2, \
                                          0xF1, 0xC8, \
                                          0x40, 0x84, 0xB4, 0xA6, 0x08, 0x67, \
                                          0xFD, 0xAC, 0x81, 0x5D, 0xB0, 0x41, \
                                          0x27, 0x75, 0x9B, 0xA7, 0x92, 0x57, \
                                          0x0C, 0x44, 0xB1, 0x57, 0x7C, 0x76, \
                                          0x5B, 0x56, 0xF0, 0xBA, 0x03, 0xF4, \
                                          0xAA, 0x67 }

extern struct gapm_set_dev_config_cmd devConfigCmd;
extern struct gapm_activity_create_adv_cmd advertiseCmd;
extern const union  gapc_dev_info_val *getDevInfoCfm[];
extern uint8_t app_actv_idx_in_process;
extern uint8_t app_adv_data[], app_scan_rsp_data[];
extern uint8_t app_adv_data_len, app_scan_rsp_data_len;
extern DRIVER_GPIO_t Driver_GPIO;

/* ---------------------------------------------------------------------------
* Function prototype definitions
* --------------------------------------------------------------------------*/
void Device_Initialize(void);

void APP_SetAdvScanData(void);

void APP_SetConnectionCfmParams(uint8_t conidx, struct gapc_connection_cfm *cfm);

void APP_GAPM_GATTM_Handler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);

void APP_GAPC_Handler(ke_msg_id_t const msg_id, void const *param,
                      ke_task_id_t const dest_id, ke_task_id_t const src_id);

void Activity_MsgHandler(ke_msg_id_t const msg_id,
                         void *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id);

int DisconnectionInd_MsgHandler(ke_msg_id_t const msgid,
                                struct gapc_disconnect_cmd const *p_param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id);

int ConnectionReqInd_MsgHandler(ke_msg_id_t const msgid,
                                struct gapc_connection_req_ind const *p_param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id);

void App_Init(void);

void APP_Activity_Manager(uint8_t idx, uint16_t msg, void *param);

void APP_ActivityCreate(uint8_t actvOperation, uint8_t actv_idx);

void APP_ActivityStart(uint8_t actvOperation, uint8_t actv_idx);

void IRQPriorityInit(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
