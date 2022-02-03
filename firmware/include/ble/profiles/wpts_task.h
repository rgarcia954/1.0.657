/**
 ****************************************************************************************
 *
 * @file wpts_task.h
 *
 * @brief Header file - Wireless Power Transfer Profile Service Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _WPTS_TASK_H_
#define _WPTS_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup WPTSTASK Task
 * @ingroup WPTS
 * @brief Wireless Power Transfer Profile Task.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h" // Task definitions

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Messages for Wireless Power Transfer Profile Service
enum wpts_msg_id
{
    /// Enable the WPTP Sensor task for a connection
    WPTS_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_WPTS),
    /// Confirmation of WPTP Sensor task
    WPTS_ENABLE_RSP,
    /// Request for an Attribute Value
    WPTS_VALUE_REQ_IND,
    /// Confirmation/Response to an Attribute Value Request
    WPTS_VALUE_CFM,
    /// Inform App of the new Attribute Data (written by peer)
    WPTS_VALUE_IND,
    /// App sends Notification for Alerts
    WPTS_PRU_ALERT_NOTIFY_CMD,
    /// App sends indication for the Alert with Mode Transition
    WPTS_PRU_MODE_TRANSITION_INDICATE_CMD,
    /// Send a complete event status to the application
    WPTS_CMP_EVT,
};

/// Opcodes for the CMP_EVT.
enum wpts_op_codes
{
    /// Invalid Opcode
    WPTS_OP_CODE_INVALID = 0x00,
    /// Opcode for the WPTS_PRU_ALERT_NOTIFY_CMD
    WPTS_NTF_ALERT = 0x01,
    /// Opcode for the WPTS_PRU_MODE_TRANSITION_INDICATE_CMD
    WPTS_IND_MODE_TRANSITION = 0x02,
    /// Opcode for WPTS_ENABLE_REQ
    WPTS_ENABLE = 0x03
};

/*
 * STRUCTURES
 ****************************************************************************************
 */
/*
 * COMMON  STRUCTURES
 ****************************************************************************************
 */
/// Structure used to exchange the information on PRU Control Char Value on the API
struct wpts_pru_control
{
    /// Enable PRU output -   (range [0:1] 0 = Disable, 1 = Enable)
    uint8_t enable_output;
    /// Enable PRU charge indicator -   (range [0:1] 0 = Disable, 1 = Enable)
    uint8_t enable_charge_indicator;
    /// Adjust Power Control @see enum wpt_pru_control_adjust_power_id
    uint8_t adjust_power_command;
    /// PRU is permitted in PTU. @see enum wpt_pru_control_permission_id
    uint8_t permission;
    /// PTU sets up time. @see enum wpt_pru_control_time_set_id
    uint8_t time_set;
};

/// Structure used to exchange the information on PTU Static Char Value on the API
struct wpts_ptu_static
{
    /// Defines which fields are valid @see enum wpt_ptu_static_validity_codes
    uint8_t optional_fields_validity;
    /// Power of PTU ( range [0:138] )
    uint8_t ptu_power;
    /// Maximum source impedance of the PTU ( range [0:18])
    uint8_t ptu_max_source_impedance;
    /// Maximum load resistance of the PTU (range [0:10])
    uint8_t ptu_max_load_resistance;
    /// PTU class @see enum wpt_ptu_class_codes
    uint8_t ptu_class;
    /// Revision of the PTU HW - Vendor Defined
    uint8_t hardware_rev;
    /// Revision of the PTU SW - Vendor Defined
    uint8_t firmware_rev;
    /// A4WP Supported Revision ( range [0:1], 0 = BSS v1.2.1, 1 = BSS v1.3 )
    uint8_t protocol_rev;
    /// Max Number of Devices (range [1:8] )
    uint8_t ptu_num_devices_supported;
};

/// Structure used to exchange the information on PRU Static Char Value on the API
struct wpts_pru_static
{
    /// Defines which fields are valid - (range [0:1], 0 - delta_r1 not present, 1 - delta_r1 present )
    uint8_t delta_r1_value_present;
    /// A4WP Supported Revision  ( range [0:1], 0 = BSS v1.2.1, 1 = BSS v1.3 )
    uint8_t protocol_revision;
    /// Category of PRU @see enum wptp_category_codes
    uint8_t pru_category;
    /// Capabilities of PRU (bit field) each of 8 bits indicating different capability
    uint8_t pru_information;
    /// Revision of the PRU HW - Vendor Defined
    uint8_t hardware_rev;
    /// Revision of the PRU SW - Vendor Defined
    uint8_t firmware_rev;
    /// PRECT_MAX of the PRU. The value is in increments of 100mW (range [0:255] )
    uint8_t prect_max;
    /// VRECT_MIN (static, first estimate). The value is in mV. (range [0:65535] )
    uint16_t vrect_min_static;
    /// VRECT_HIGH (static, first estimate). The value is in mV. (range [0:65535] )
    uint16_t vrect_high_static;
    /// VRECT_SET. The value is in mV. (range [0:65535] )
    uint16_t vrect_set;
    /// Delta R1 caused by PRU. The PRU may report its Delta R1,
    /// if included, in increments of 0.01 ohms. (range [0:65535] )
    uint16_t delta_r1_value;
};

/// Structure used to exchange the information on PRU Dynamic Char Value on the API
struct wpts_pru_dynamic
{
    /// Defines which fields are valid - bit field with 6 highest bits indicating what is supported
    uint8_t optional_fields_validity;
    /// DC voltage at the output of the rectifier. mV (range [0:65535] )
    uint16_t vrect;
    /// DC current at the output of the rectifier. mA (range [0:65535] )
    uint16_t irect;
    /// Voltage at charge/battery port. mV (range [0:65535] )
    uint16_t vout;
    /// Current at charge/battery port. mA (range [0:65535] )
    uint16_t iout;
    /// Temperature of PRU. Deg C from -40C, in increments of 1c. ( range [0:255], 0 = -40c, 255 = +215c )
    uint8_t  temperature;
    /// The current dynamic minimum rectifier voltage desired. mV (range [0:65535] )
    uint16_t vrect_min_dyn;
    /// Desired VRECT (dynamic value). mV (range [0:65535] )
    uint16_t vrect_set_dyn;
    /// The current dynamic maximum rectifier voltage desired. mV (range [0:65535] )
    uint16_t vrect_max_dyn;
    /// Warnings. Bit field - with each bit indicating an different alert condiction
    uint8_t  pru_alert;
    /// PTU Test Mode Command - ( range [0:3], 0 = Normal Op Power Control, 1 = Increase Itx by 1 step,
    ///                                        2 = Decrease Irx by 1 step , 3 = Itx coil current shall not be adjusted
    uint8_t  tester_command;

};

/// Parameters of the initialization function
struct wpts_db_cfg
{
    /// Defines which fields are valid - (range [0:1], 0 - delta_r1 not present, 1 - delta_r1 present )
    uint8_t delta_r1_value_present;
    /// A4WP Supported Revision  ( range [0:1], 0 = BSS v1.2.1, 1 = BSS v1.3 )
    uint8_t protocol_revision;
    /// Category of PRU @see enum wptp_category_codes
    uint8_t pru_category;
    /// Capabilities of PRU (bit field) each of 8 bits indicating different capability
    uint8_t pru_information;
    /// Revision of the PRU HW - Vendor Defined
    uint8_t hardware_rev;
    /// Revision of the PRU SW - Vendor Defined
    uint8_t firmware_rev;
    /// PRECT_MAX of the PRU. The value is in increments of 100mW (range [0:255] )
    uint8_t prect_max;
    /// VRECT_MIN (static, first estimate). The value is in mV. (range [0:65535] )
    uint16_t vrect_min_static;
    /// VRECT_HIGH (static, first estimate). The value is in mV. (range [0:65535] )
    uint16_t vrect_high_static;
    /// VRECT_SET. The value is in mV. (range [0:65535] )
    uint16_t vrect_set;
    /// Delta R1 caused by PRU. The PRU may report its Delta R1,
    /// if included, in increments of 0.01 ohms. (range [0:65535] )
    uint16_t delta_r1_value;
};

/// Simple structure to hold alert value
struct wpts_pru_alert
{
    uint8_t alert;
};

/*
 * Structures for Messages
 ****************************************************************************************
 */

/// Message from the APP to Enable the PRU functionality
/// Parameters of the @see WPTS_ENABLE_REQ message
struct wpts_enable_req
{
    /// Connection ID
    uint8_t conidx;
    /// Notification/Indication flags
    uint16_t ntf_ind_cfg;
};

/// Message to the APP to indicate a command is complete
/// Parameters for the @see WPTS_ENABLE_RSP message
struct wpts_enable_rsp
{
    /// Connection ID
    uint8_t conidx;
    /// Operation Status
    uint8_t status;
};

/// Message to the APP to indicate a command is complete
/// Parameters for the @see WPTS_CMP_EVT message
struct wpts_cmp_evt
{
    /// operation
    uint8_t operation;
    /// Operation Status
    uint8_t  status;
};

/// App request for an Attribute Value
/// Parameters for the @see WPTS_VALUE_REQ_IND message
struct wpts_value_req_ind
{
    /// Identifier for requested Attribute
    uint8_t attrib_id;
};

/// App sends an Attribute Value in response to wpts_value_req_ind
/// Parameters for the @see WPTS_VALUE_CFM message
struct wpts_value_cfm
{
    /// Identifies the Type of Value Received
    uint8_t attrib_id;

    uint8_t  status;
    /// value - union of 5 structures
    union wpts_value_cfm_tag
    {
        /// PRU control
        struct wpts_pru_control pru_ctrl;
        /// PTU Static info
        struct wpts_ptu_static  ptu_static;
        /// PRU Static info
        struct wpts_pru_static  pru_static;
        /// PRU Dynamic Info
        struct wpts_pru_dynamic pru_dynamic;
        /// PRU Alert info
        struct wpts_pru_alert   pru_alert;
    } value;
};

/// App receives an Attribute Value - to be stored
/// Parameters for the @see WPTS_VALUE_IND message
struct wpts_value_ind
{
    /// Identifies the Type of Value Received
    uint8_t attrib_id;

    /// value - union of 3 structures
    union wpts_value_ind_tag
    {
        /// PRU control
        struct wpts_pru_control pru_control;
        /// PTU Static info
        struct wpts_ptu_static  ptu_static;
        /// PRU Alert info
        uint16_t  alert_ccc;
    } value;
};

/// App sends notification for the Simple Alert condition
/// Parameters for the @see WPTS_PRU_ALERT_NOTIFY_CMD message
struct wpts_pru_alert_notify_cmd
{
    /// bit field of the alerts reason
    uint8_t pru_alert;
};

/// App sends notification for the Mode Transition Alert condition
/// Parameters for the @see WPTS_PRU_MODE_TRANSITION_INDICATE_CMD message
struct wpts_pru_mode_transition_indicate_cmd
{
    /// bit field of the alerts reason
    uint8_t pru_alert;
    /// Mode transition field ( range [0:3], 0 = No Mode Transition,             1 = 2s Mode Transition time limit,
    ///                                      2 = 3s Mode Transition time limit,  3 = 6s Mode Transition time limit )
    uint8_t mode_transition;
    /// Bluetooth Device Address
    uint8_t addr[6];
};

/// @} WPTSTASK

#endif //(_WPTS_TASK_H_)
