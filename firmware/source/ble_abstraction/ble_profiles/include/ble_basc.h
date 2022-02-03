/**
 * @file ble_basc.h
 * @brief Battery Service Client (BASC) BLE abstraction header file
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

#ifndef BLE_BASC_H
#define BLE_BASC_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <basc_task.h>
#include <gapm_task.h>
#include <ble_protocol_config.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/**
 * @brief Battery service client environment
 */
typedef struct
{
    struct gapm_profile_added_ind profile_added_ind;                                         /**< Profile service handle
                                                                                              * */
    uint32_t battLevelReqTimeout;                                                            /**< Timeout for periodic
                                                                                              * battery level request */
    uint8_t bas_nb[APP_MAX_NB_CON];                                                          /**< Number of battery
                                                                                              * instances
                                                                                              * [1,BASC_NB_BAS_INSTANCES_MAX]
                                                                                              * */
    bool enabled[APP_MAX_NB_CON];                                                            /**< The flag that
                                                                                              * indicates
                                                                                              * that service has been
                                                                                              * enabled */
    uint8_t batt_lvl[APP_MAX_NB_CON][BASC_NB_BAS_INSTANCES_MAX];                             /**< Battery level
                                                                                              * characteristic value */
    uint8_t ntf_cfg[APP_MAX_NB_CON][BASC_NB_BAS_INSTANCES_MAX];                              /**< Notification
                                                                                              * configuration */
    uint8_t req_ntf_cfg[APP_MAX_NB_CON][BASC_NB_BAS_INSTANCES_MAX];                          /**< Control notification
                                                                                              * is
                                                                                              * required or not */
    struct prf_char_pres_fmt char_pres_format[APP_MAX_NB_CON][BASC_NB_BAS_INSTANCES_MAX];    /**< Battery level
                                                                                              * characteristic
                                                                                              * presentation
                                                                                              * format descriptor value
                                                                                              * structure  */
    struct bas_content bas[APP_MAX_NB_CON][BASC_NB_BAS_INSTANCES_MAX];                       /**< Battery service
                                                                                              * content
                                                                                              * structure */
} BASC_Env_t;

/**
 * @brief BASC application task messages
 */
enum basc_app_msg_id
{
    BASC_BATT_LEVEL_REQ_TIMEOUT = TASK_FIRST_MSG(TASK_ID_BASC) + 50     /**< BASC battery level request timeout id */
};

/**
 * @brief Initialize BASC environment
 *
 * Initialize battery service client environment and configure
 * message handlers.
 */
void BASC_Initialize(void);

/**
 * @brief Enable the client role of the BAS
 *
 * Prepare and send BASC_ENABLE_REQ to enable the client role of the
 * battery service.
 *
 * @param [in] conidx Connection index
 * @note conidx is valid (between 0 and GAP_MAX_CONNECTIONS-1)
 */
void BASC_EnableReq(uint8_t conidx);

/**
 * @brief Prepare and send BASC_READ_INFO_REQ to read value from peer
 *
 * Prepare and send BASC_READ_INFO_REQ to read the value of characteristic or
 * descriptor in the peer device database.
 *
 * @param [in] conidx  Connection index
 * @param [in] bas_nb  Battery instance number
 * @param [in] info    Peer battery info type
 */
void BASC_ReadInfoReq(uint8_t conidx, uint8_t bas_nb, uint8_t info);

/**
 * @brief Send a request to change notification configuration
 *
 * Prepare and send BASC_BATT_LEVEL_NTF_CFG_REQ to request to change
 * battery level notification configuration in the peer device.
 *
 * @param [in] conidx  Connection index
 * @param [in] bas_nb  Battery instance number
 * @param [in] ntf_cfg The value of notification configuration
 */
void BASC_BattLevelNtfCfgReq(uint8_t conidx, uint8_t bas_nb, uint8_t ntf_cfg);

/**
 * @brief Send a request for periodic battery level update
 *
 * Configure a kernel timer to periodically send battery level requests.
 *
 * @param [in] timeout_ms Delay between requests in units of 10ms.
 *                        If timeout == 0, disable periodic requests.
 */
void BASC_RequestBattLevelOnTimeout(uint32_t timeout_ms);

/**
 * @brief Read the most recent battery level
 *
 * Read the most recent value of battery level from the BASC environment.
 *
 * @param [in] conidx Connection index
 * @param [in] bas_nb Battery instance number
 * @return Value of battery level
 */
uint8_t BASC_GetLastBatteryLevel(uint8_t conidx, uint8_t bas_nb);

/**
 * @brief Get the BASC environment
 *
 * Return a reference to the internal BASC environment structure.
 *
 * @return Constant pointer to BASC_Env_t
 */
const BASC_Env_t * BASC_GetEnv(void);

/**
 * @brief Handle the BASC events
 *
 * Handle all the events related to battery service client.
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant message parameter
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 */
void BASC_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_BASC_H */
