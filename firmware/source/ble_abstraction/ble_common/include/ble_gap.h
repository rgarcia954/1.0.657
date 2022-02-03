/**
 * @file ble_gap.h
 * @brief BLE Abstraction GAP layer header
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

#ifndef BLE_GAP_H
#define BLE_GAP_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <gapm_task.h>
#include <gapc_task.h>
#include <co_bt_defines.h>
#include <ble_protocol_config.h>
#include <bondlist.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/** GAPM privacy configuration identity address public */
#define GAPM_PRIV_CFG_ADDR_PUBLIC             (0 << GAPM_PRIV_CFG_PRIV_ADDR_POS)

/** GAPM privacy configuration identity address private */
#define GAPM_PRIV_CFG_ADDR_PRIVATE            (0 << GAPM_PRIV_CFG_PRIV_ADDR_POS)

/** GAPM privacy configuration set to host privacy */
#define GAPM_PRIV_CFG_HOST_PRIVACY            (0 << GAPM_PRIV_CFG_PRIV_EN_POS)

/** GAPM privacy configuration set to controller privacy */
#define GAPM_PRIV_CFG_CONTROLLER_PRIVACY      (1 << GAPM_PRIV_CFG_PRIV_EN_POS)

/**
 * @brief GAPM default device configuration (used in GAPM_SET_DEV_CONFIG_CMD)
 */

#define GAPM_DEFAULT_ROLE                      GAP_ROLE_ALL                /**< Default device role is both peripheral
                                                                            * and central */
#define GAPM_DEFAULT_RENEW_DUR                 60                          /**< Default duration for regenerating device
                                                                            * address when privacy
                                                                            * is enabled (in seconds) */
#define GAPM_DEFAULT_GAP_START_HDL             0                           /**< Default start handle for GAP service
                                                                            * (dynamically allocated) */
#define GAPM_DEFAULT_GATT_START_HDL            0                           /**< Default start handle for GATT service
                                                                            * (dynamically allocated) */
#define GAPM_DEFAULT_ATT_CFG                   0                           /**< Default attribute database configuration */
#define GAPM_DEFAULT_TX_OCT_MAX                0xfb                        /**< Default value for the controller's
                                                                            * maximum transmitted
                                                                            * number of payload octets to be used */
#define GAPM_DEFAULT_TX_TIME_MAX               (14 * 8 \
                                                + \
                                                GAPM_DEFAULT_TX_OCT_MAX \
                                                * 8)                        /**< Suggested value for the
                                                                             * controller's maximum packet
                                                                             * transmission time to be used*/
#define GAPM_DEFAULT_MTU_MAX                   0x200                        /**< Default maximum MTU acceptable for
                                                                             * device (L2CAP)*/
#define GAPM_DEFAULT_MPS_MAX                   0x200                        /**< Default maximum MPS (L2CAP) */
#define GAPM_DEFAULT_MAX_NB_LECB               0                            /**< Default maximum number of LE Credit
                                                                             * based connection that can
                                                                             * be established (L2CAP) */
#define GAPM_DEFAULT_AUDIO_CFG                 0                            /**< Default LE Audio Mode Configuration */
#define GAP_ROLE_MASTER                        0                            /**< GAP master */
#define GAP_ROLE_SLAVE                         1                            /**< GAP slave */
#define GAPM_DEFAULT_ADV_DATA                  { 7, GAP_AD_TYPE_COMPLETE_NAME, \
                                                 'O', 'N', ' ', 'B', 'L', 'E', \
                                                 3, GAP_AD_TYPE_MANU_SPECIFIC_DATA, \
                                                 0x62, 0x3 }                /**< Default advertisement data (length,
                                                                             * data) */
#define GAPM_DEFAULT_ADV_DATA_LEN              12                           /**< Default length of advertisement data */
#define GAPM_DEFAULT_SCANRSP_DATA              GAPM_DEFAULT_ADV_DATA        /**< Default scan response data */
#define GAPM_DEFAULT_SCANRSP_DATA_LEN          GAPM_DEFAULT_ADV_DATA_LEN    /**< Default length of scan response data */

/** GAPM default StartConnectionCmd configuration (GAPM_START_CONNECTION_CMD) */
#define GAPM_DEFAULT_SCAN_INTERVAL             100    /**< Default scan interval 62.5ms */
#define GAPM_DEFAULT_SCAN_WINDOW               50     /**< Scan window to 50% of the interval */

/** Default connection interval and slave latency */
#define GAPM_DEFAULT_CON_INTV_MIN              20    /**< 20 ms */
#define GAPM_DEFAULT_CON_INTV_MAX              20    /**< 20 ms */
#define GAPM_DEFAULT_CON_LATENCY               0     /**< Slave latency: number of events that can be missed while
                                                      * maintaining a connection */

/** Default supervisory timeout */
#define GAPM_DEFAULT_SUPERV_TO                 300                              /**< 3 seconds */
#define GAPM_DEFAULT_CE_LEN_MIN                2 * GAPM_DEFAULT_CON_INTV_MIN    /**< Recommended minimum duration of
                                                                                 * connection events 40 ms */
#define GAPM_DEFAULT_CE_LEN_MAX                2 * GAPM_DEFAULT_CON_INTV_MAX    /**< Recommended maximum duration of
                                                                                 * connection events 40 ms */

/** Advertising minimum and maximum interval */
#define GAPM_DEFAULT_ADV_INTV_MIN              64    /**< 40ms (64*0.625ms) */
#define GAPM_DEFAULT_ADV_INTV_MAX              64    /**< 40ms (64*0.625ms) */

/** Advertising channel map - 37, 38, 39 */
#define GAPM_DEFAULT_ADV_CHMAP                 0x07

/**
 * @brief GAPM activity state
 */
enum gapm_activity_state
{
    ACTIVITY_STATE_NOT_CREATED = 0,     /**< GAPM Activity state value when activity state not created */
    ACTIVITY_STATE_CREATING,            /**< GAPM Activity state value when start creating activity */
    ACTIVITY_STATE_NOT_STARTED,         /**< GAPM Activity state value when not started */
    ACTIVITY_STATE_STARTING,            /**< GAPM Activity state value when activity state is starting */
    ACTIVITY_STATE_STARTED              /**< GAPM Activity state value when activity state is started */
};

/**
 * @brief GAPM state
 */
enum gapm_state
{
    GAPM_STATE_INITIAL = 0,     /**< Initial state, before a GAPM_RESET is performed */
    GAPM_STATE_RESET,           /**< After GAPM_RESET, before the device is configured */
    GAPM_STATE_READY            /**< After the device is configured with GAPM_SetDeviceConfig */
};

/**
 * @brief GAPM activity status
 */
typedef struct
{
    uint8_t actv_idx;                  /**< Activity identifier */
    enum gapm_actv_type type;          /**< GAPM activity type */
    enum gapm_activity_state state;    /**< GAPM activity state */
    bool advDataSet;                   /**< True if advertisement data is set */
    bool scanRspDataSet;               /**< True if scan response data is set */
} GAPM_ActivityStatus_t;

/**
 * @brief BLE white list information
 */
struct ble_whitelist_info
{
    uint8_t device_num;             /**< # of devices in white list for device filtering */
    bool wl_filtering_activated;    /**< Flag for device filtering activation */
};

/**
 * @brief GAP environment
 */
typedef struct
{
    enum gapm_state gapmState;                                      /**< State of GAPM */
    uint16_t profileAddedCount;                                     /**< # of standard BLE profile added */
    struct gapm_set_dev_config_cmd deviceConfig;                    /**< Device configuration for GAPM_SET_DEV_CONFIG */
    struct gapc_connection_req_ind connection[APP_MAX_NB_CON];      /**< GAPC_CONNECTION_REQ_IND information of
                                                                     * connections */
    BondInfo_t bondInfo[APP_MAX_NB_CON];                            /**< Bond information of connections */
    GAPM_ActivityStatus_t *actv[APP_MAX_NB_ACTIVITY];               /**< GAPM activity status of activities */

    uint16_t profileAddedTaskId[APP_MAX_NB_PROFILES];               /**< Task IDs for added profiles */
} GAP_Env_t;

/**
 * @brief BLE white list information
 */
extern struct ble_whitelist_info whitelist_info;

/**
 * @brief Initialize GAP environment
 *
 * Initialize GAP environment after each GAPM reset.
 */
void GAP_Initialize(void);

/**
 * @brief Get GAP environment
 *
 * Initialize GAP environment after each GAPM reset.
 *
 * @return Pointer to GAP environment structure
 */
const GAP_Env_t * GAP_GetEnv(void);

/**
 * @brief Get GAP environment added profiles task identifiers
 *
 * @return Pointer to an array of added profiles task identifiers
 */
uint16_t * GAP_GetProfileAddedTaskId(void);

/**
 * @brief Check address if its private resolvable
 *
 * @param [in] addr     Pointer to address
 * @param [in] addrType Type of address
 * @return True if address is private resolvable false otherwise
 */
bool GAP_IsAddrPrivateResolvable(const uint8_t *addr, uint8_t addrType);

/**
 * @brief Add GAP advertise data
 *
 * Add BLE advertisement data and scan response data to buffer
 *
 * @param [in] newDataLen       Advertisement data length to add
 * @param [in] newDataFlag      Type of advertisement data
 * @param [in] newData          Pointer to new advertisement data to add
 * @param [in] resultAdvData    Pointer to resultant advertisement data buffer
 * @param [in] resultAdvDataLen Length of resultant advertisement data
 * @return True if new advertisement data is added false otherwise
 */
bool GAP_AddAdvData(uint8_t newDataLen, enum gap_ad_type newDataFlag,
                    const uint8_t *newData, uint8_t *resultAdvData,
                    uint8_t *resultAdvDataLen);

/**
 * @brief GAPM functions
 */

/**
 * @brief Perform GAPM software reset
 *
 * Do software reset using GAPM_RESET operation.
 */
void GAPM_SoftwareReset(void);

/**
 * @brief Perform GAPM platform reset
 *
 * Do platform reset using GAPM_PLF_RESET operation.
 */
void GAPM_PlatformReset(void);

/**
 * @brief Perform GAPM reset with specified GAPM operation.
 *
 * @param [in] operation GAPM operation code
 */
void GAPM_ResetCmd(enum gapm_operation operation);

/**
 * @brief Prepare and send set the device configuration
 *
 * Set the device configuration such as device role, manage device
 * address type, privacy configuration, GAP/GATT service start,
 * configure data length extension, set specific write permissions
 * in GAP database, enable or not audio features.
 *
 * @note: This command is allowed only when no link is established
 *
 * @param [in] deviceConfig Pointer to constant GAPM set device configuration command
 */
void GAPM_SetDevConfigCmd(const struct gapm_set_dev_config_cmd *deviceConfig);

/**
 * @brief get Device configuration
 *
 * Get the device configuration such as device role, manage device
 * address type, privacy configuration, GAP/GATT service start,
 * configure data length extension, set specific write permissions
 * in GAP database, enable or not audio features.
 *
 * @return Pointer to constant constant GAPM set device configuration command
 */
const struct gapm_set_dev_config_cmd * GAPM_GetDeviceConfig(void);

/**
 * @brief Prepare and Send GAPM_PROFILE_TASK_ADD_CMD to the Bluetooth stack
 *
 * Prepare and send GAPM_PROFILE_TASK_ADD_CMD to the Bluetooth stack to allocate
 * a task for a specific profile (service or client).
 *
 * @param [in] sec_lvl     Security level
 * @param [in] prf_task_id Profile task identifier of profile to add
 * @param [in] app_task    Application task number
 * @param [in] start_hdl   Service start handle (0 for dynamically allocated in attribute database)
 * @param [in] param       Pointer to parameter value to initialize profile
 * @param [in] paramSize   Size of parameter
 */
void GAPM_ProfileTaskAddCmd(uint8_t sec_lvl, uint16_t prf_task_id, uint16_t app_task,
                            uint16_t start_hdl, uint8_t *param, uint32_t paramSize);

/**
 * @brief Get number of profiles added by the Bluetooth stack
 *
 * Get the GAP environment variable that keeps track of number of added GAPM profiles.
 *
 * @return Number of GAPM profile added successfully
 */
uint16_t GAPM_GetProfileAddedCount(void);

/**
 * @brief Prepare and send GAPM_LEPSM_REGISTER_CMD to the Bluetooth stack
 *
 * Prepare and send GAPM_LEPSM_REGISTER_CMD to register an LE protocol/Service multiplexer ID
 * in the device allowing a peer device to create LE credit based connection on it.
 *
 * @param [in] le_psm   LE protocol/service multiplexer
 * @param [in] app_task Application task number
 * @param [in] sec_lvl  Security level
 */
void GAPM_LepsmRegisterCmd(uint16_t le_psm, uint16_t app_task, uint8_t sec_lvl);

/**
 * @brief Prepare and send GAPM_GEN_RAND_ADDR_CMD to the Bluetooth stack
 *
 * Prepare and send GAPM_LEPSM_REGISTER_CMD to generate a random device address without
 * starting any air operation.
 *
 * @param [in] rnd_type Random address type
 */
void GAPM_GenRandAddrCmd(enum random_addr_type rnd_type);

/**
 * @brief Prepare and send GAPM_RESOLV_ADDR_CMD to the Bluetooth stack
 *
 * Prepare and send GAPM_RESOLV_ADDR_CMD to resolve a random address using array of
 * IRK exchanged and bonded with device during pairing operation.
 *
 * @param [in] conidx Connection identifier
 * @param [in] peerAddr Resolvable random address to solve
 */
void GAPM_ResolvAddrCmd(uint8_t conidx, const uint8_t *peerAddr);

/**
 * @brief Message handler for GAPM events
 *
 * This function receives GAPM events from he Bluetooth stack and perform actions accordingly.
 * It initialize GAP and GATT at reset and also manages GAPM states and the GAP environment.
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant parameter
 * @param [in] dest_id Constant destination kernel identifier
 * @param [in] src_id  Constant source kernel identifier
 */
void GAPM_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 * @brief Prepare and send GAPM_LIST_SET_CMD to set the white list content
 *
 * Prepare and send GAPM_LIST_SET_CMD to set the content of the white list.
 *
 * @param [in] operation  GAPM operation code to set the content of white list
 * @param [in] nb         Number of entries to be added in the list
 * @param [in] devices_wl Pointer to structure containing list of addresses to be
 *                        added in the white list
 */
void GAPM_ListSetWlCmd(uint8_t operation, uint8_t nb, struct gap_bdaddr
                       *devices_wl);

/**
 * @brief Prepare and send GAPM_LIST_SET_CMD to set the resolving list content
 *
 * Prepare and send GAPM_LIST_SET_CMD to set the content of the resolving list.
 *
 * @param [in] operation  GAPM operation code to set the content of white list
 * @param [in] nb         Number of entries to be added in the list
 * @param [in] rl_devinfo Pointer to structure containing list of addresses to be
 *                        added in the resolving list
 */
void GAPM_ListSetRalCmd(uint8_t operation, uint8_t nb,
                        struct gap_ral_dev_info *rl_devinfo);

/**
 * @brief Check if IRK is valid
 *
 * Function checks if a valid IRK has been previously exchanged or not.
 *
 * @param [in] bond_info Pointer to constant bond information
 * @return True if irk_exchanged in the specified bond_info is nonzero, false otherwise
 */
bool GAPM_IsIRKValid(const BondInfo_t *bond_info);

/**
 * @brief Update the white list and the resolve List
 *
 * Copies the addresses of the devices in the bond list and updates them in the
 * Bluetooth stack's white list and the resolvable list (if applicable).
 */
void WhiteList_ResolveList_Update(void);

/**
 * @brief GAPM air operations
 */

/**
 * @brief Prepare and send GAPM_ACTIVITY_CREATE_CMD to create an advertising activity
 *
 * Prepare and send GAPM_ACTIVITY_CREATE_CMD with operation GAPM_CREATE_ADV_ACTIVITY to
 * create an advertising activity.
 *
 * @param [in] actv_status   Pointer to GAPM activity status
 * @param [in] own_addr_type Bluetooth device`s own address type
 * @param [in] adv_param     Pointer to constant advertising parameters structure
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_ActivityCreateAdvCmd(GAPM_ActivityStatus_t *actv_status,
                               enum gapm_own_addr own_addr_type,
                               const struct gapm_adv_create_param *adv_param);

/**
 * @brief Prepare and send GAPM_ACTIVITY_CREATE_CMD to create scan activity
 *
 * Prepare and send GAPM_ACTIVITY_CREATE_CMD with operation GAPM_CREATE_SCAN_ACTIVITY to
 * create scan activity.
 *
 * @param [in] actv_status   Pointer to GAPM activity status
 * @param [in] own_addr_type Bluetooth device`s own address type
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_ActivityCreateScanCmd(GAPM_ActivityStatus_t *actv_status,
                                enum gapm_own_addr own_addr_type);

/**
 * @brief Prepare and send GAPM_ACTIVITY_CREATE_CMD to create an initiating activity
 *
 * Prepare and send GAPM_ACTIVITY_CREATE_CMD with operation GAPM_CREATE_INIT_ACTIVITY to
 * create an initiating activity.
 *
 * @param [in] actv_status   Pointer to GAPM activity status
 * @param [in] own_addr_type Bluetooth device`s own address type
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_ActivityCreateInitCmd(GAPM_ActivityStatus_t *actv_status,
                                enum gapm_own_addr own_addr_type);

/**
 * @brief Prepare and send GAPM_ACTIVITY_CREATE_CMD to create a periodic sync activity
 *
 * Prepare and send GAPM_ACTIVITY_CREATE_CMD with operation GAPM_CREATE_PERIOD_SYNC_ACTIVITY to
 * create a periodic synchronization activity.
 *
 * @param [in] actv_status   Pointer to GAPM activity status
 * @param [in] own_addr_type Bluetooth device`s own address type
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_ActivityCreatePeriodSyncCmd(GAPM_ActivityStatus_t *actv_status,
                                      enum gapm_own_addr own_addr_type);

/**
 * @brief Prepare and send GAPM_ACTIVITY_START_CMD to start an advertising activity
 *
 * Prepare and send GAPM_ACTIVITY_START_CMD to request the host to start a previously
 * created advertising activity.
 *
 * @param [in] actv_idx    Activity identifier
 * @param [in] duration    Advertising duration (in unit of 10ms). 0 means that advertising
 *                         continues until the host disables it.
 * @param [in] max_adv_evt Maximum number of extended advertising events the controller shall
 *                         attempt to send prior to terminating the extended advertising
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_AdvActivityStart(uint8_t actv_idx, uint16_t duration, uint8_t max_adv_evt);

/**
 * @brief Prepare and send GAPM_ACTIVITY_START_CMD to start an initiating activity
 *
 * Prepare and send GAPM_ACTIVITY_START_CMD to request the host to start previously
 * created initiating activity.
 *
 * @param [in] actv_idx  Activity identifier
 * @param [in] initParam Pointer to initiating parameters structure
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_InitActivityStart(uint8_t actv_idx, struct gapm_init_param *initParam);

/**
 * @brief Prepare and send GAPM_ACTIVITY_START_CMD to start a scanning activity
 *
 * Prepare and send GAPM_ACTIVITY_START_CMD to request the host to start previously
 * created scanning activity.
 *
 * @param [in] actv_idx  Activity identifier
 * @param [in] scanParam Pointer to scan parameters structure
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_ScanActivityStart(uint8_t actv_idx, struct gapm_scan_param *scanParam);

/**
 * @brief Prepare and send GAPM_ACTIVITY_START_CMD to start a periodic sync activity
 *
 * Prepare and send GAPM_ACTIVITY_START_CMD to request the host to start previously
 * created periodic synchronization activity.
 *
 * @param [in] actv_idx       Activity identifier
 * @param [in] per_sync_param Pointer to periodic sync parameters structure
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_PerSyncActivityStart(uint8_t actv_idx, struct gapm_per_sync_param *per_sync_param);

/**
 * @brief Prepare and send GAPM_ACTIVITY_START_CMD to start an activity
 *
 * Prepare and send GAPM_ACTIVITY_START_CMD to request the host to start previously
 * created activity.
 *
 * @param [in] actv_idx Activity identifier
 * @param [in] u_param  Pointer to union to containing activity parameters
 * @return False if no activity slot found, true otherwise
 */
bool GAPM_ActivityStartCmd(uint8_t actv_idx, union gapm_u_param *u_param);

/**
 * @brief Prepare and send GAPM_ACTIVITY_STOP_CMD to stop a specified activity
 *
 * Prepare and send GAPM_ACTIVITY_STOP_CMD with operation GAPM_STOP_ACTIVITY
 * request the host to stop a specified activity.
 *
 * @param [in] actv_idx Activity identifier
 * @return False if activity does not exist, true otherwise
 */
bool GAPM_ActivityStop(uint8_t actv_idx);

/**
 * @brief Prepare and send GAPM_ACTIVITY_STOP_CMD to stop all existing activities
 *
 * Prepare and send GAPM_ACTIVITY_STOP_CMD with operation GAPM_STOP_ALL_ACTIVITIES
 * request the host to stop all existing activities.
 *
 * @return False if activity does not exist, true otherwise
 */
bool GAPM_ActivityStopAll(void);

/**
 * @brief Prepare and send GAPM_ACTIVITY_STOP_CMD to stop a specified activity
 *
 * Prepare and send GAPM_ACTIVITY_STOP_CMD with operation GAPM_STOP_ACTIVITY
 * request the host to stop a specified activity.
 *
 * @param [in] operation GAPM operation code [GAPM_STOP_ACTIVITY or GAPM_STOP_ALL_ACTIVITIES]
 * @param [in] actv_idx  Activity identifier (used only if operation is GAPM_STOP_ACTIVITY)
 * @return False if activity does not exist, true otherwise
 */
bool GAPM_ActivityStopCmd(uint8_t operation, uint8_t actv_idx);

/**
 * @brief Prepare and send GAPM_ACTIVITY_DELETE_CMD to delete a specified activity
 *
 * Prepare and send GAPM_ACTIVITY_DELETE_CMD with operation GAPM_DELETE_ACTIVITY
 * to request the host to delete an existing specified activity.
 *
 * @param [in] actv_idx Activity identifier (used only if operation is GAPM_DELETE_ACTIVITY)
 * @return False if activity does not exist, true otherwise
 */
bool GAPM_DeleteActivity(uint8_t actv_idx);

/**
 * @brief Prepare and send GAPM_ACTIVITY_DELETE_CMD to delete all the activities
 *
 * Prepare and send GAPM_ACTIVITY_DELETE_CMD with operation GAPM_DELETE_ALL_ACTIVITIES
 * to request the host to delete all existing activities.
 *
 * @return False if activity does not exist, true otherwise
 */
bool GAPM_DeleteAllActivities(void);

/**
 * @brief Prepare and send GAPM_ACTIVITY_DELETE_CMD to delete activity/activities
 *
 * Prepare and send GAPM_ACTIVITY_DELETE_CMD to request the host to delete either an activity
 * or all currently existing activities.
 *
 * @param [in] operation GAPM operation code [GAPM_DELETE_ACTIVITY or GAPM_DELETE_ALL_ACTIVITIES]
 * @param [in] actv_idx  Activity identifier (used only if operation is GAPM_DELETE_ACTIVITY)
 * @return False if activity does not exist, true otherwise
 */
bool GAPM_DeleteActivityCmd(uint8_t operation, uint8_t actv_idx);

/**
 * @brief Prepare and send GAPM_SET_ADV_DATA_CMD to set an advertising data or scan response
 * data or periodic advertising data
 *
 * Prepare and send GAPM_SET_ADV_DATA_CMD to either set an advertising data or the
 * scan response data or the periodic advertising data for a given advertising activity
 * identified by activity ID.
 *
 * @param [in] operation GAPM operation code (GAPM_SET_ADV_DATA or GAPM_SET_SCAN_RSP_DATA
 *                       or GAPM_SET_PERIOD_ADV_DATA)
 * @param [in] actv_idx  Activity identifier
 * @param [in] length    Length of data
 * @param [in] data      Pointer to data
 * @return False if activity slot not found, true otherwise
 */
bool GAPM_SetAdvDataCmd(uint8_t operation, uint8_t actv_idx, uint8_t length, uint8_t *data);

/**
 * @brief Prepare and send GAPM_PER_ADV_CTE_TX_CTL_CMD to control CTE transmission in a
 * periodic advertising activity.
 *
 * @param [in] actv_idx Activity identifier
 * @param [in] enable   Enable or disable CTE transmission
 * @return False if activity slot is equal to max number of activity, true otherwise
 */
bool GAPM_PerAdvCteTxCmd(uint8_t actv_idx, bool enable);

/**
 * @brief Prepare and send GAPM_PER_SYNC_IQ_SAMPLING_CTRL_CMD to control capturing
 * IQ samples from the constant tone extension of periodic advertising packets.
 *
 * @param [in] actv_idx        Activity identifier
 * @param [in] slot_dur        Slot durations (1: 1us | 2: 2us)
 * @param [in] max_sample_cte  Maximum sampled CTEs
 * @param [in] ant_pattern_len Length of switching pattern
 * @param [in] ant_id          Pointer to antenna IDs
 * @param [in] enable          Enable IQ sampling
 *
 * @return False if activity slot is equal to max number of activity, true otherwise
 */
bool GAPM_PerSyncIQSamplingCtrlCmd(uint8_t actv_idx, uint8_t slot_dur,
                                   uint8_t max_sample_cte,
                                   uint8_t ant_pattern_len, uint8_t *ant_id, bool enable);

/**
 * @brief Prepare and send GAPM_PER_ADV_REPORT_CTRL_CMD to control reception of periodic
 * advertising report in a periodic advertising sync activity.
 *
 * @param [in] actv_idx Activity identifier
 * @param [in] enable   Enable or disable reception of periodic advertising report
 * @return False if activity slot is equal to max number of activity, true otherwise
 */
bool GAPM_PerAdvReportCtrlCmd(uint8_t actv_idx, bool enable);

/**
 * @brief GAPC functions
 */

/**
 * @brief Prepare and send GAPC_PARAM_UPDATE_CMD to perform an update on parameters
 *
 * Prepare and send GAPC_PARAM_UPDATE_CMD to perform an update of connection parameters.
 *
 * @param [in] conidx     Connection identifier
 * @param [in] intv_min   Minimum of connection interval (value = intv_max * 1.25 ms)
 * @param [in] intv_max   Maximum of connection interval (value = intv_max * 1.25 ms)
 * @param [in] time_out   Link supervision timeout (value = time_out * 10 ms)
 * @param [in] latency    connection latency
 * @param [in] ce_len_min Minimum CE length (value = ce_len_min * 0.625 ms)
 * @param [in] ce_len_max Maximum CE length (value = ce_len_min * 0.625 ms)
 */
void GAPC_ParamUpdateCmd(uint8_t conidx, uint16_t intv_min, uint16_t intv_max,
                         uint16_t latency, uint16_t time_out, uint16_t ce_len_min, uint16_t ce_len_max);

/**
 * @brief Prepare and send GAPC_PARAM_UPDATE_CFM to accept or reject connection parameters
 *
 * Prepare and send GAPC_PARAM_UPDATE_CFM to accept or reject connection parameters proposed
 * by the peer device.
 *
 * @param [in] conidx     Connection identifier
 * @param [in] accept     Accept(0x01) or reject(0x00) slave connection parameters
 * @param [in] ce_len_min Minimum CE length (value = ce_len_min * 0.625 ms)
 * @param [in] ce_len_max Maximum CE length (value = ce_len_min * 0.625 ms)
 */
void GAPC_ParamUpdateCfm(uint8_t conidx, bool accept, uint16_t ce_len_min,
                         uint16_t ce_len_max);

/**
 * @brief Prepare and send GAPC_CONNECTION_CFM in a response to connection request
 *
 * Prepare and send GAPC_CONNECTION_CFM in a response to connection request from the
 * peer to enable local attribute tasks and security manager for the connection.
 *
 * @param [in] conidx Connection identifier
 * @param [in] param Pointer to GAPC connection confirmation structure
 */
void GAPC_ConnectionCfm(uint8_t conidx, struct gapc_connection_cfm *param);

/**
 * @brief Prepare and send GAPC_DISCONNECT_CMD to disconnect link
 *
 * Prepare and send GAPC_DISCONNECT_CMD to request a disconnection of the
 * link.
 *
 * @param [in] conidx Connection identifier
 * @param [in] reason Reason for disconnection
 */
void GAPC_DisconnectCmd(uint8_t conidx, uint8_t reason);

/**
 * @brief Check if the specified connection is active
 *
 * Check if specified connection ID is active or not.
 *
 * @param [in] conidx Connection identifier
 * @return True if given connection id have valid GAP connection handle,
 *         false otherwise
 */
bool GAPC_IsConnectionActive(uint8_t conidx);

/**
 * @brief Prepare and send GAPC_DISCONNECT_CMD to disconnect all the links
 *
 * Prepare and send GAPC_DISCONNECT_CMD to request disconnection for each
 * active connections.
 *
 * @param [in] reason Reason for disconnection
 */
void GAPC_DisconnectAll(uint8_t reason);

/**
 * @brief Get the number of active connection count
 *
 * @return Number of active connection count
 */
uint8_t GAPC_ConnectionCount(void);

/**
 * @brief Get the master connection count
 *
 * @return Number of master connection count
 */
uint8_t GAPC_MasterConnectionCount(void);

/**
 * @brief Get the slave connection count
 *
 * @return Number of slave connection count
 */
uint8_t GAPC_SlaveConnectionCount(void);

/**
 * @brief Get the connection information of a specified connection ID
 *
 * Get GAPC connection information of a specified connection ID.
 *
 * @param [in] conidx Connection identifier
 * @return Pointer to constant structure gapc_connection_req_ind
 */
const struct gapc_connection_req_ind * GAPC_GetConnectionInfo(uint8_t conidx);

/**
 * @brief Prepare and send GAPC_GET_DEV_INFO_CFM to send the device name
 *
 * Prepare and send GAPC_GET_DEV_INFO_CFM to send the device name information
 * to the peer device.
 *
 * @param [in] conidx  Connection identifier
 * @param [in] devName Pointer to device name
 */
void GAPC_GetDevInfoCfm_Name(uint8_t conidx, const char *devName);

/**
 * @brief Prepare and send GAPC_GET_DEV_INFO_CFM to send the device appearance
 *
 * Prepare and send GAPC_GET_DEV_INFO_CFM to send the device appearance icon
 * to the peer device.
 *
 * @param [in] conidx     Connection identifier
 * @param [in] appearance Appearance icon
 */
void GAPC_GetDevInfoCfm_Appearance(uint8_t conidx, uint16_t appearance);

/**
 * @brief Prepare and send GAPC_GET_DEV_INFO_CFM to send device slave preferred parameters
 *
 * Prepare and send GAPC_GET_DEV_INFO_CFM to to send device slave preferred
 * parameters to peer device.
 *
 * @param [in] conidx        Connection identifier
 * @param [in] con_intv_min  Minimum connection interval (value = con_intv_min * 1.25 ms)
 * @param [in] con_intv_max  Maximum connection interval (value = con_intv_max * 1.25 ms)
 * @param [in] slave_latency Slave latency
 * @param [in] conn_timeout  Connection supervision timeout (value = conn_timeout * 10 ms)
 */
void GAPC_GetDevInfoCfm_SlvPrefParams(uint8_t conidx, uint16_t con_intv_min,
                                      uint16_t con_intv_max, uint16_t slave_latency,
                                      uint16_t conn_timeout);

/**
 * @brief Prepare and send GAPC_GET_DEV_INFO_CFM to send requested information
 *
 * Prepare and send GAPC_GET_DEV_INFO_CFM to send requested information to the
 * peer device.
 *
 * @param [in] conidx Connection identifier
 * @param [in] req    Requested information
 * @param [in] dat    Pointer to constant union GAPC device information value
 */
void GAPC_GetDevInfoCfm(uint8_t conidx, uint8_t req,
                        const union gapc_dev_info_val *dat);

/**
 * @brief Prepare and send GAPC_SET_DEV_INFO_CFM to confirm if the requested
 * device information was written
 *
 * Prepare and send GAPC_SET_DEV_INFO_CFM to provide confirmation to the Bluetooth
 * stack if the requested device information was written.
 *
 * @param [in] conidx Connection identifier
 * @param [in] req    Requested information (GAPC_DEV_NAME or GAPC_DEV_APPEARANCE)
 * @param [in] accept True if the write request has been approved, false otherwise
 * @return False if req is not GAPC_DEV_NAME or GAPC_DEV_APPEARANCE, true otherwise
 */
bool GAPC_SetDevInfoCfm(uint8_t conidx, uint8_t req, bool accept);

/**
 * @brief GAPC bond and encryption operations
 */

/**
 * @brief Prepare and send GAPC_BOND_CFM to send confirmation to a bond request
 *
 * Prepare and send GAPC_BOND_CFM to provide confirmation of the receipt of a
 * GAPC_BOND_REQ_IND message.
 *
 * @param [in] conidx  Connection identifier
 * @param [in] request Bond request type
 * @param [in] accept  Accept(0x01) or reject(0x00) request
 * @param [in] data    Pointer to constant GAPC bond confirmation data union
 */
void GAPC_BondCfm(uint8_t conidx, enum gapc_bond request, bool accept,
                  const union gapc_bond_cfm_data *data);

/**
 * @brief Prepare and send GAPC_ENCRYPT_CMD to request encrypting the connection link
 *
 * Prepare and send GAPC_ENCRYPT_CMD as the master to request initiation of the link
 * encryption procedure.
 *
 * @param [in] conidx   Connection identifier
 * @param [in] ediv     Encryption diversifier
 * @param [in] randnb   Pointer to constant random number
 * @param [in] ltk      Pointer to constant long term key
 * @param [in] key_size Size of LTK key
 */
void GAPC_EncryptCmd(uint8_t conidx, uint16_t ediv, const uint8_t *randnb,
                     const uint8_t *ltk, uint8_t key_size);

/**
 * @brief Prepare and send GAPC_ENCRYPT_CFM to confirm an encryption request
 *
 * Prepare and send GAPC_ENCRYPT_CFM to provide confirmation of the receipt of a
 * GAPC_ENCRYPY_REQ_IND message.
 *
 * @param [in] conidx   Connection identifier
 * @param [in] found    Nonzero if LTK is found for peer device, zero otherwise
 * @param [in] ltk      Pointer to constant long term key
 * @param [in] key_size Size of LTK key
 */
void GAPC_EncryptCfm(uint8_t conidx, bool found, const uint8_t *ltk, uint8_t key_size);

/**
 * @brief Prepare and send GAPC_BOND_CMD to initiate a bond procedure
 *
 * Prepare and send GAPC_BOND_CMD to the master of the link in order to initiate the bond
 * procedure.
 *
 * @param [in] conidx  Connection identifier
 * @param [in] pairing Pointer to constant structure pairing information
 */
void GAPC_BondCmd(uint8_t conidx, const struct gapc_pairing *pairing);

/**
 * @brief Check if the connection is bonded
 *
 * Check if the specified connection ID is active and if it has a valid bond state.
 *
 * @param [in] conidx Connection identifier
 * @return True if connection is active and it has valid bond state, false otherwise
 */
bool GAPC_IsBonded(uint8_t conidx);

/**
 * @brief Get bond information for the connection
 *
 * Get bond information for a specified connection ID.
 *
 * @param [in] conidx Connection identifier
 * @return Pointer to constant bond information structure if connection
 *         is active and its bonded, NULL otherwise
 */
const BondInfo_t * GAPC_GetBondInfo(uint8_t conidx);

/**
 * @brief Add device to bond list
 *
 * Add device with a specified connection ID to bond list.
 *
 * @param [in] conidx Connection identifier
 * @return Non-zero if successful, zero otherwise
 */
uint16_t GAPC_AddDeviceToBondList(uint8_t conidx);

/**
 * @brief GAPC message handler
 *
 * Receives GAPC events from the Bluetooth stack and perform actions accordingly.
 *
 * @param [in] msg_id  Kernel message identifier
 * @param [in] param   Pointer to constant parameter
 * @param [in] dest_id Constant destination kernel identifier
 * @param [in] src_id  Constant source kernel identifier
 */
void GAPC_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 * @brief GAPC PHY management operations
 */

/**
 * @brief Prepare and send GAPC_SET_PHY_CMD to set the preferred PHY
 *
 * Prepare and send GAPC_SET_PHY_CMD to set the preferred PHY for current
 * active link.
 *
 * @param [in] conidx        Connection identifier
 * @param [in] rx_rate       Preferred LE PHYs for reception
 * @param [in] tx_rate       Preferred LE PHYs for transmission
 * @param [in] preferredRate Preferred coding scheme for LE coded
 */
void GAPC_SetPhyCmd(uint8_t conidx, uint8_t rx_rate, uint8_t tx_rate,
                    uint8_t preferredRate);

/**
 * @brief GAPC constant Tone extension operations
 */

/**
 * @brief Prepare and send GAPC_CTE_TX_CFG_CMD to configure CTE TX
 *
 * Prepare and send GAPC_CTE_TX_CFG_CMD to configure constant tone
 * extension transmission.
 *
 * @param [in] conidx          Connection identifier
 * @param [in] cte_type        CTE types
 * @param [in] ant_pattern_len Length of switching pattern
 * @param [in] ant_id          Pointer to antenna IDs
 */
void GAPC_CteTxCfgCmd(uint8_t conidx, uint8_t cte_type, uint8_t ant_pattern_len,
                      uint8_t *ant_id);

/**
 * @brief Prepare and send GAPC_CTE_RX_CFG_CMD to configure CTE RX
 *
 * Prepare and send GAPC_CTE_RX_CFG_CMD to configure constant tone
 * extension reception.
 *
 * @param [in] conidx          Connection identifier
 * @param [in] slot_dur        Slot durations (1: 1us, 2: 2us)
 * @param [in] ant_pattern_len Length of switching pattern
 * @param [in] ant_id          Pointer to antenna IDs
 * @param [in] sample_en       Enable or disable sampling
 */
void GAPC_CteRxCfgCmd(uint8_t conidx, uint8_t slot_dur, uint8_t ant_pattern_len,
                      uint8_t *ant_id, bool sample_en);

/**
 * @brief Prepare and send GAPC_CTE_REQ_CTRL_CMD to control CTE requests
 *
 * Prepare and send GAPC_CTE_REQ_CTRL_CMD to control constant tone
 * extension requests.
 *
 * @param [in] conidx       Connection identifier
 * @param [in] cte_type     CTE types
 * @param [in] cte_len      Request CTE length (in 8us unit)
 * @param [in] cte_interval CTE request interval (in no of connection events)
 * @param [in] enable       Enable or disable TX/RX CTE
 */
void GAPC_CteReqCtrlCmd(uint8_t conidx, uint8_t cte_type, uint8_t cte_len,
                        uint8_t cte_interval, bool enable);

/**
 * @brief Prepare and send GAPC_CTE_RSP_CTRL_CMD to control CTE reception
 *
 * Prepare and send GAPC_CTE_RSP_CTRL_CMD to control constant tone
 * extension reception.
 *
 * @param [in] conidx Connection identifier
 * @param [in] enable Enable or disable TX/RX CTE
 */
void GAPC_CteRspCtrlCmd(uint8_t conidx, bool enable);

/**
 * @brief GAPC local and peer device information operations
 */

/**
 * @brief Prepare and send GAPC_GET_INFO_CMD to get the peer information
 *
 * Prepare and send GAPC_GET_INFO_CMD to get information about the peer
 * device or about the current active link.
 *
 * @param [in] conidx    Connection identifier
 * @param [in] operation GAPC requested operation
 */
void GAPC_GetInfoCmd(uint8_t conidx, uint8_t operation);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_GAP_H */
