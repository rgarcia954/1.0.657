/**
 * @file ble_gatt.h
 * @brief BLE Abstraction GATT layer header
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

#ifndef BLE_GATTM_H
#define BLE_GATTM_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <ke.h>
#include <gattm_task.h>
#include <gattc_task.h>
#include <ble_protocol_config.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/** Default GATTC handles */
#define GATTC_DEFAULT_START_HDL         0x0001    /**< GATTC default start handle */
#define GATTC_DEFAULT_END_HDL           0xFFFF    /**< GATTC default end handle */

/** Macros to declare a (custom) service with 16, 32 and 128 bit UUID
 * srvidx Service index
 * uuid   Service UUID */
#define CS_SERVICE_UUID_16(srvidx, uuid) \
    { srvidx, { uuid, PERM(SVC_UUID_LEN, UUID_16), 0, 0 }, true, 0, NULL, NULL }    /**< Custom service with 16 bit UUID
                                                                                     * */
#define CS_SERVICE_UUID_32(srvidx, uuid) \
    { srvidx, { uuid, PERM(SVC_UUID_LEN, UUID_32), 0, 0 }, true, 0, NULL, NULL }    /**< Custom service with 32 bit UUID
                                                                                     * */
#define CS_SERVICE_UUID_128(srvidx, uuid) \
    { srvidx, { uuid, PERM(SVC_UUID_LEN, UUID_128), 0, 0 }, true, 0, NULL, NULL }    /**< Custom service with 128 bit
                                                                                      * UUID */

/** Standard declaration/description UUIDs in 16-byte format */
#define CS_ATT_SERVICE_128            { 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00 }    /**< Custom service attribute */
#define CS_ATT_CHARACTERISTIC_128     { 0x03, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00 }    /**< Custom service characteristic */
#define CS_ATT_CLIENT_CHAR_CFG_128    { 0x02, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00 }    /**< Custom service attribute for client characteristic
                                                               * configuration */
#define CS_ATT_CHAR_USER_DESC_128     { 0x01, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00 }    /**< Custom service attribute characteristic user
                                                               * description */

/** Macros to define characteristics with 16, 32 and 128 bit UUID
 * attidx_char: Characteristic attribute index
 * attidx_val:  Value attribute index
 * uuid        UUID
 * perm        Permissions (see gattm_att_desc)
 * length      Maximum length value (in bytes)
 * data        Pointer to the data structure in the application
 * callback    Function to transfer the data between the application and the GATTM */
#define CS_CHAR_UUID_16(attidx_char, attidx_val, uuid, perm, length, data, callback) \
    { attidx_char, { CS_ATT_CHARACTERISTIC_128, PERM(RD, ENABLE), 0, 0 }, false, 0, NULL, NULL }, \
    { attidx_val, { uuid, perm, length, PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_16) }, false, length, data, callback }    /**<
                                                                                                                          *Custom
                                                                                                                          *service
                                                                                                                          *characteristic
                                                                                                                          *with
                                                                                                                          *16
                                                                                                                          *bit
                                                                                                                          *UUID
                                                                                                                          **/

#define CS_CHAR_UUID_32(attidx_char, attidx_val, uuid, perm, length, data, callback) \
    { attidx_char, { CS_ATT_CHARACTERISTIC_128, PERM(RD, ENABLE), 0, 0 }, false, 0, NULL, NULL }, \
    { attidx_val, { uuid, perm, length, PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_32) }, false, length, data, callback }    /**<
                                                                                                                          *Custom
                                                                                                                          *service
                                                                                                                          *characteristic
                                                                                                                          *with
                                                                                                                          *32
                                                                                                                          *bit
                                                                                                                          *UUID
                                                                                                                          **/
#define CS_CHAR_UUID_128(attidx_char, attidx_val, uuid, perm, length, data, callback) \
    { attidx_char, { CS_ATT_CHARACTERISTIC_128, PERM(RD, ENABLE), 0, 0 }, false, 0, NULL, NULL }, \
    { attidx_val, { uuid, perm, length, PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_128) }, false, length, data, callback }    /**<
                                                                                                                           * Custom
                                                                                                                           * service
                                                                                                                           * characteristic
                                                                                                                           * with
                                                                                                                           * 128
                                                                                                                           * bit
                                                                                                                           * UUID
                                                                                                                           * */

/** Macro to add to the characteristic a CCC
 * attidx   CCC attribute index
 * data     Pointer to the 2-byte CCC data value in the application
 * callback Function to transfer the CCC data between the application and the GATTM */
#define CS_CHAR_CCC(attidx, data, callback) \
    { attidx, { CS_ATT_CLIENT_CHAR_CFG_128, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, PERM(RI, ENABLE) }, false, 2, \
      data, \
      callback }

/** Macro to add to the characteristic a user description
 * attidx   Description attribute index
 * length   Description length (in bytes)
 * data     Pointer to the description string (constant)
 * callback Function to transfer the description string to the GATTM */
#define CS_CHAR_USER_DESC(attidx, length, data, callback) \
    { attidx, { CS_ATT_CHAR_USER_DESC_128, PERM(RD, ENABLE), length, PERM(RI, ENABLE) }, false, length, data, callback }

/** Macro to Find Minimum */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * @brief Custom service attribute database description
 */
struct att_db_desc
{
    uint16_t att_idx;                                                                   /**< Convert into uint8_t */
    struct gattm_att_desc att;                                                          /**< GATTM Attribute description
                                                                                         * structure */
    bool is_service;                                                                    /**< True if it is service */
    uint16_t length;                                                                    /**< Total number of
                                                                                         * characteristics to add */
    void *data;                                                                         /**< Pointer to data */
    uint8_t (*callback)(uint8_t conidx, uint16_t attidx, uint16_t handle,
                        uint8_t *toData, const uint8_t *fromData, uint16_t lenData,
                        uint16_t operation, uint8_t hl_status);                         /**< Pointer to callback
                                                                                         * function */
};

/**
 * @brief Custom service descriptor
 */
typedef struct
{
    uint16_t cust_svc_start_hdl;                /**< Start handle array of custom services in the stack's attribute
                                                 * database */
    const struct att_db_desc *cust_svc_att_db;    /**< Custom services attribute database */
    uint16_t cust_svc_att_db_len;               /**< Custom service attribute database length */
} cust_svc_desc;

/**
 * @brief GATT environment
 */
typedef struct
{
    uint16_t addedSvcCount;                 /**< Counter of successfully added custom service/s in stack */
    uint16_t *discSvcCount;                 /**< Counter of discovered service/s per connection id */

    cust_svc_desc *cust_svc_db;             /**< Custom service database array */
    uint16_t max_cust_svc;                  /**< Maximum number of custom services */

    const struct att_db_desc *att_db;       /**< Attribute database specific to related custom service */
    uint16_t att_db_len;                    /**< Length of attribute database specific to related custom service */
} GATT_Env_t;

/**
 * @brief GATT initialization
 *
 * Initialize the GATT environment.
 */
void GATT_Initialize(void);

/**
 * @brief Get GATT environment
 *
 * Function gets a reference to the internal GATT environment structure.
 *
 * @return A constant pointer to GATT_Env_t
 */
const GATT_Env_t * GATT_GetEnv(void);

/**
 * @brief Set GATT environment data
 *
 * Set the GATT environment discovery service counter (array),
 * custom service database and maximum number of custom services.
 *
 * @param [in] disc_svc_count    Pointer to discovered services count
 * @param [in] custom_service_db Pointer to custom service database
 * @param [in] max_cust_svc_num  Maximum number of custom services
 */
void GATT_SetEnvData(uint16_t *disc_svc_count, cust_svc_desc *custom_service_db,
                     uint8_t max_cust_svc_num);

/**
 * @brief Get the maximum number of custom services in GATT
 *
 * Function gets the maximum number of custom services in GATT environment.
 *
 * @return max_cust_svc_num maximum number of custom services
 */
uint16_t GATT_GetMaxCustomServiceNumber(void);

/**
 * @brief Get GATT added services count
 *
 * Function gets number of GATT services that have been added.
 *
 * @return the number of GATT services that have been added
 */
uint16_t GATTM_GetServiceAddedCount(void);

/**
 * @brief Resets GATT services attribute database ID
 *
 * Function resets GATT services attribute database ID. This
 * should be called before re-adding the attribute databases
 * via GATTM_AddAttributeDatabase() without complete system reset.
 */
void GATTM_ResetServiceAttributeDatabaseID(void);

/**
 * @brief Add service and attributes to the Bluetooth stack
 *
 * Prepare and send GATTM_ADD_SVC_REQ to add services/characteristics into
 * the Bluetooth stack database.
 *
 * @param [in] att_db     Pointer to constant custom service attribute database
 * @param [in] att_db_len Number of elements in the att_db array
 * @note Triggers a GATTM_ADD_SVC_RSP for every service added.
 */
void GATTM_AddAttributeDatabase(const struct att_db_desc *att_db,
                                uint16_t att_db_len);

/**
 * @brief Get the handle for an attribute
 *
 * Returns the stack database handle for a certain attribute index.
 *
 * @param [in] cs_svc_number Custom service number related to attribute
 * @param [in] attidx        Attribute index
 * @return Handle value, in case of success or 0, in case the attidx or
 * the start handle is invalid
 * @note The user has used GATT_AddAttributeDatabase() to construct
 * the database and the stack has already finished adding services to the database.
 */
uint16_t GATTM_GetHandle(uint8_t cs_svc_number, uint16_t attidx);

/**
 * @brief Handle GATTM messages
 *
 * Message handler for the GATT manager
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant message parameter (unused)
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 */
void GATTM_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                      ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 * @brief GATTC initialization
 *
 * Initialize GATTC
 */
void GATTC_Initialize(void);

/**
 * @brief GATTC services discovery by UUID
 *
 * Start a discovery by UUID(GATTC_DISC_BY_UUID_SVC) in a specified handle range.
 *
 * @param [in] conidx    Connection index
 * @param [in] uuid[]    Service UUID
 * @param [in] uuid_len  UUID length (ATT_UUID_16_LEN or ATT_UUID_32_LEN or
 *                                    ATT_UUID_128_LEN)
 * @param [in] start_hdl Start handle of a discovery range
 * @param [in] end_hdl   End handle of a discovery range
 */
void GATTC_DiscByUUIDSvc(uint8_t conidx, uint8_t uuid[], uint8_t uuid_len,
                         uint16_t start_hdl, uint16_t end_hdl);

/**
 * @brief GATTC all services discovery
 *
 * Start a discovery for all services in a specified handle range.
 *
 * @param [in] conidx    Connection index
 * @param [in] start_hdl Start handle of discovery range
 * @param [in] end_hdl   End handle of discovery range
 */
void GATTC_DiscAllSvc(uint8_t conidx, uint16_t start_hdl, uint16_t end_hdl);

/**
 * @brief GATTC all characteristics discovery
 *
 * Start a characteristic discovery for all characteristics in
 * a specified handle range.
 *
 * @param [in] conidx    Connection index
 * @param [in] start_hdl Start handle of discovery range
 * @param [in] end_hdl   End handle of discovery range
 */
void GATTC_DiscAllChar(uint8_t conidx, uint16_t start_hdl, uint16_t end_hdl);

/**
 * @brief GATTC send event
 *
 * Send GATTC characteristic notification or indication event.
 *
 * @param [in] conidx    Connection index
 * @param [in] operation GATTC_NOTIFY or GATTC_INDICATE
 * @param [in] seq_num   Operation sequence number
 * @param [in] handle    Characteristic handle
 * @param [in] length    Attribute length
 * @param [in] value     Pointer to attribute data value
 */
void GATTC_SendEvtCmd(uint8_t conidx, uint8_t operation, uint16_t seq_num,
                      uint16_t handle, uint16_t length, uint8_t *value);

/**
 * @brief GATTC send event confirmation
 *
 * Send a GATTC indication event confirmation.
 *
 * @param [in] conidx Connection index
 * @param [in] handle Characteristic handle
 */
void GATTC_SendEvtCfm(uint8_t conidx, uint16_t handle);

/**
 * @brief GATTC handle read request indication
 *
 * Handle a received read request indication from GATT controller.
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant message parameters
 *                     (in format of structure gattc_read_req_ind)
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 * @note Indicate if the message was consumed, compare with KE_MSG_CONSUMED
 */
void GATTC_ReadReqInd(ke_msg_id_t const msg_id,
                      struct gattc_read_req_ind const *param, ke_task_id_t const dest_id,
                      ke_task_id_t const src_id);

/**
 * @brief GATTC handle write request indication
 *
 * Handle a received write request indication from GATT controller.
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant message parameters
 *                     (in format of structure gattc_write_req_ind)
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 * @note Indicate if the message was consumed, compare with KE_MSG_CONSUMED
 */
void GATTC_WriteReqInd(ke_msg_id_t const msg_id,
                       struct gattc_write_req_ind const *param, ke_task_id_t const dest_id,
                       ke_task_id_t const src_id);

/**
 * @brief GATTC request attribute information
 *
 * Request an attribute info indication to be passed to the upper layers
 * of the Bluetooth stack. Could also be triggered during prepare write
 * to check if attribute modification is authorized by
 * profile/application or not and to get current attribute length.
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant message parameters
 *                     (in format of structure gattc_read_req_ind)
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 * @note Indicate if the message was consumed, compare with KE_MSG_CONSUMED
 */
void GATTC_AttInfoReqInd(ke_msg_id_t const msg_id,
                         struct gattc_read_req_ind const *param, ke_task_id_t const dest_id,
                         ke_task_id_t const src_id);

/**
 * @brief Handle GATTM messages
 *
 * Message handler for GATT manager messages received from kernel
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant message parameter (unused)
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 * @note Indicate if the message was consumed, compare with KE_MSG_CONSUMED
 */
void GATTC_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                      ke_task_id_t const dest_id, ke_task_id_t const src_id);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* GATT_H */
