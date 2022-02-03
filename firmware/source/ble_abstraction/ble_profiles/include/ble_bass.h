/**
 * @file ble_bass.h
 * @brief BASS Application-specific header file
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

#ifndef BLE_BASS_H
#define BLE_BASS_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <bass_task.h>
#include <ke_timer.h>
#include <gapm_task.h>
#include <ble_protocol_config.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/**
 * @brief Battery service server environment
 */
typedef struct
{
    uint8_t bas_nb;                                      /**< Number of battery instances [1,BASS_NB_BAS_INSTANCES_MAX]
                                                          * */
    uint8_t batt_ntf_cfg[APP_MAX_NB_CON];                /**< The current value of CCCD of battery value that has been
                                                          * set by the client device */
    bool enabled[APP_MAX_NB_CON];                        /**< The flag that indicates that service has been enabled */

    uint32_t battLevelNotificationTimeout;               /**< Timeout value for sending batter level notification */
    bool battLevelNotificationTimerEnqueued;             /**< Flag that tells whether the timer for battery level
                                                          * notification is enabled or not */

    uint32_t battLevelMonitoringTimeout;                 /**< Timeout value for battery level monitoring */
    bool battLevelMonitoringTimerEnqueued;               /**< Flag that tells whether the timer for monitoring battery level
                                                          * is enabled or not */

    uint8_t lastBattLevel[BASS_NB_BAS_INSTANCES_MAX];    /**< Most recent value of battery level */

    uint8_t (*readBattLevelCallback)(uint8_t bas_nb);    /**< Pointer to an application function that returns the
                                                          * battery level */

    struct gapm_profile_added_ind profile_added_ind;     /**< Profile service handle */
} BASS_Env_t;

/**
 * @brief BASS application task messages
 */
enum bass_app_msg_id
{
    BASS_BATT_LEVEL_NTF_TIMEOUT = TASK_FIRST_MSG(TASK_ID_BASS) + 50,    /**< BASS battery level notification timeout ID
                                                                         * */
    BASS_BATT_MONITORING_TIMEOUT,                                       /**< BASS battery level monitor timeout ID */
    BASS_BATT_LEVEL_CHANGED                                             /**< BASS battery level change ID */
};

/**
 * @brief Initialize BASS environment
 *
 * Initialize a battery service server environment and
 * configure message handlers.
 *
 * @param [in] bas_nb                   Number of battery instances [1,2].
 * @param [in] readBattLevelCallback    Pointer to user application callback
 *                                      function that returns the battery value.
 */
void BASS_Initialize(uint8_t bas_nb,
                     uint8_t (*readBattLevelCallback)(uint8_t bas_nb));

/**
 * @brief BASS configure periodic notification
 *
 * Configure a battery service to send periodic notifications.
 *
 * @param [in] timeout Timeout period in units of 10ms.
 *                     disabled if timeout == 0
 */
void BASS_NotifyOnTimeout(uint32_t timeout);

/**
 * @brief BASS notify peers on battery level change
 *
 * Configure battery service to periodically monitor the battery level according
 * to the timeout. The battery level is queried read through the callback
 * function passed as argument to BASS_Initialize(). Connected peers are
 * only notified if the battery level changes. It triggers a BASS_BATT_LEVEL_CHANGED
 * event, which the user application may subscribe to.
 *
 * @param [in] timeout Timeout period in units of 10ms.
 *                     disabled if timeout == 0
 */
void BASS_NotifyOnBattLevelChange(uint32_t timeout);

/**
 * @brief Add a battery profile in kernel
 *
 * Send request to add a battery profile in kernel and database.
 */
void BASS_ProfileTaskAddCmd(void);

/**
 * @brief Enable the server role in BAS
 *
 * Prepare and send BASS_ENABLE_REQ to enable the server role of the
 * battery service.
 *
 * @param [in] conidx Connection index
 */
void BASS_EnableReq(uint8_t conidx);

/**
 * @brief Prepare and send BASS_BATT_LEVEL_UPD_REQ for battery
 * level notification
 *
 * Prepare and send BASS_BATT_LEVEL_UPD_REQ to update the battery
 * level characteristic value for the BAS instance. This value will
 * be stored in the database so that it can be read by the peer device.
 *
 * @param [in] batt_lvl Battery level [0, 100]
 * @param [in] bas_nb   Battery server instance number
 */
void BASS_BattLevelUpdReq(uint8_t batt_lvl, uint8_t bas_nb);

/**
 * @brief Get the BASS environment.
 *
 * Return a reference to the internal BASS environment structure.
 *
 * @return Constant pointer to BASS_Env_t
 */
const BASS_Env_t * BASS_GetEnv(void);

/**
 * @brief Check if the BASS is added successfully
 *
 * Returns a boolean indication if the BAS server has been successfully
 * added to the services database.
 *
 * @return True if the service has been successfully added, false otherwise
 */
bool BASS_IsAdded(void);

/**
 * @brief Handle BASS messages
 *
 * Handle all events related to the battery service server
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant message parameters
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 */
void BASS_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_BASS_H */
