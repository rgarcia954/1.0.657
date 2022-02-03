/**
 * @file ble_diss.h
 * @brief DISS Application-specific header file
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

#ifndef BLE_DISS_H
#define BLE_DISS_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <diss_task.h>
#include <ke_timer.h>
#include <stdint.h>
#include <ke_msg.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/**
 * @brief Global DISS specific info structure
 */
struct DISS_DeviceInfoField_t
{
    uint8_t len;      /**< Length of data */
    uint8_t *data;    /**< Pointer to data */
};

/**
 * @brief Global DISS info compilation structure
 */
struct DISS_DeviceInfo_t
{
    struct DISS_DeviceInfoField_t MANUFACTURER_NAME;    /**< Manufacturer name characteristic */
    struct DISS_DeviceInfoField_t MODEL_NB_STR;         /**< Model number string characteristic */
    struct DISS_DeviceInfoField_t SERIAL_NB_STR;        /**< Serial number string characteristic */
    struct DISS_DeviceInfoField_t FIRM_REV_STR;         /**< Firmware revision string characteristic */
    struct DISS_DeviceInfoField_t SYSTEM_ID;            /**< System ID characteristic */
    struct DISS_DeviceInfoField_t HARD_REV_STR;         /**< Hardware revision string characteristic */
    struct DISS_DeviceInfoField_t SW_REV_STR;           /**< Software revision string characteristic */
    struct DISS_DeviceInfoField_t IEEE;                 /**< IEEE 11073-20601 regulatory certification data
                                                         * characteristic */
    struct DISS_DeviceInfoField_t PNP;                  /**< PnP ID characteristic */
};

/**
 * @brief DISS state structure
 */
typedef struct
{
    uint16_t features;                              /**< DISS features that will be enabled. See diss_task.h for
                                                     * reference */
    bool serviceAdded;                              /**< The flag that indicates that service has been added */
    const struct DISS_DeviceInfo_t *deviceInfo;     /**< DISS device information compilation structure arm */
} DISS_Env_t;

/**
 * @brief Initialize the device information server service environment and
 *        configure message handlers
 *
 * @param[in] features   The enabled features for the device information
 *                       service. Values define as enum diss_features
 *                       in diss_task.h.
 * @param[in] deviceInfo Constant pointer to DISS_DeviceInfo_t
 * @return 0 if the initialization was successful, < 0 otherwise
 */
int DISS_Initialize(uint16_t features, const struct DISS_DeviceInfo_t *deviceInfo);

/**
 * @brief Send a request to add the device information service in kernel
 * and database
 */
void DISS_ProfileTaskAddCmd(void);

/**
 * @brief Return a reference to the internal DISS environment structure
 *
 * @return A constant pointer to DISS_Env_t
 */
const DISS_Env_t * DISS_EnvGet(void);

/**
 * @brief Return a boolean indication if the DISS server has been successfully
 *        added to the services database
 *
 * @return True if the service is added successful, false otherwise
 */
bool DISS_IsAdded(void);

/**
 * @brief Handle all the events related to the device information service server
 *
 * @param[in] msg_id  Kernel message identifier
 * @param[in] param   Message parameter
 * @param[in] dest_id Destination task identifier
 * @param[in] src_id  Source task identifier
 */
void DISS_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 * @brief Handle the indication when the peer device requests a value
 *
 * @param[in] msgid   Kernel message identifier
 * @param[in] param   Message parameters in format of struct diss_value_req_ind
 * @param[in] dest_id Destination task identifier
 * @param[in] src_id  Source task identifier
 */
void DISS_DeviceInfoValueReqInd(ke_msg_id_t const msgid, void const *param,
                                ke_task_id_t const dest_id, ke_task_id_t const src_id);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_DISS_H */
