/**
 ****************************************************************************************
 *
 * @file wptc_task.h
 *
 * @brief Header file - Wireless Power Transfer Profile Collector/Client Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

#ifndef _WPTC_TASK_H_
#define _WPTC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup WPTCTASK  Wireless Power Transfer Collector Task
 * @ingroup WPTC
 * @brief  Wireless Power Transfer ProfileProfile Collector Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h" // Task definitions
#include "prf_types.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Message IDs
enum wptc_msg_ids
{
    /// Enable the Profile Collector task - at connection
    WPTC_ENABLE_CMD = TASK_FIRST_MSG(TASK_ID_WPTC),
    /// Reads a Characteristic Value from the PRU
    WPTC_READ_CMD,
    /// Writes a Characteristic Value to the PRU
    WPTC_WRITE_CMD,
    /// Information sent from the PRU, this can be in response to a READ_CMD or an ALERT indication.
    WPTC_VALUE_IND,
    /// Complete Event Information
    WPTC_CMP_EVT,
};

/// Wireless Power Transfer Collector Op codes
enum wptc_opcodes
{
    /// Read Op Code
    WPTC_READ_OP_CODE = 1,
    /// Write Op Code
    WPTC_WRITE_OP_CODE = 2,
    /// Enable Op Code
    WPTC_ENABLE_OP_CODE = 3
};

/// Wireless Power Transfer Service Characteristics
enum
{
    /// PRU Control Parameters
    WPTC_CHAR_PRU_CONTROL = 0,
    /// PTU Static Parameters
    WPTC_CHAR_PTU_STATIC,
    /// PRU Alert Parameters
    WPTC_CHAR_PRU_ALERT,
    /// PRU Static Parameters
    WPTC_CHAR_PRU_STATIC,
    /// PRU Dynamic Parameters
    WPTC_CHAR_PRU_DYNAMIC,

    WPTC_CHAR_MAX,
};

/// Wireless Power Transfer Service Characteristic Descriptors
enum
{
    /// PRU Alert Parameters Client config
    WPTC_DESC_PRU_ALERT_CCC = 0,

    WPTC_DESC_MAX,
};

/// Operation Codes for pending operations in the client.
enum wptc_op_codes
{
    /// Reserved operation code
    WPTC_OP_CODE_RESERVED = 0x00,

    /// Read op-codes
    /// Read the PRU CONTROL Characteristic Value
    WPTC_RD_PRU_CONTROL = 1,
    /// Read the PTU STATIC Characteristic Value
    WPTC_RD_PTU_STATIC = 2,
    /// Read the PRU STATIC Characteristic Value
    WPTC_RD_PRU_STATIC = 3,
    /// Read the PRU DYNAMIC Characteristic Value
    WPTC_RD_PRU_DYNAMIC = 4,
    /// Read the PRU ALERT Characteristic Value
    WPTC_RD_PRU_ALERT = 5,

    /// Write op-codes.
    /// Write a new value to the PRU CONTROL Characteristic
    WPTC_WR_PRU_CONTROL = 6,
    /// Write a new value to the PTU STATIC Characteristic
    WPTC_WR_PTU_STATIC = 7,
    /// Write a new value CCC of the ALERT Characteristic in the PRU
    WPTC_WR_ALERT_CCC = 8,
    /// Enable the PTU
    WPTC_ENABLE
};

/*
 * COMMON  STRUCTURES
 ****************************************************************************************
 */

/// Service content
struct wptc_wpt_content
{
    /// service info - including Start/End handles for the service
    struct prf_svc svc;
};

/// PRU Control characteristic value
struct wptc_pru_control
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

/// PTU Static characteristic value
struct wptc_ptu_static
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

/// PRU Static characteristic value
struct wptc_pru_static
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
    ///  if included, in increments of 0.01 ohms. (range [0:65535] )
    uint16_t delta_r1_value;
};

/// PRU Dynamic characteristic value
struct wptc_pru_dynamic
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

/// PRU Alert characteristic value
struct wptc_pru_alert
{
    /// Alert value
    uint8_t alert;
};

/// PRU Mode transition value
struct wptc_pru_mode_transition
{
    /// Alert value
    uint8_t alert;
    /// Mode transition field ( range [0:3], 0 = No Mode Transition,             1 = 2s Mode Transition time limit,
    ///                                      2 = 3s Mode Transition time limit,  3 = 6s Mode Transition time limit )
    uint8_t mode_transition;
    /// Bluetooth Device Address
    uint8_t addr[GAP_BD_ADDR_LEN];
};

/*
 * API MESSAGE STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the @see WPTC_ENABLE_REQ message
struct wptc_enable_cmd
{
    uint8_t operation;
    /// Service start handle
    uint16_t start_hdl;
    /// Registration Flag - indicates if PRU_Static info should be read
    uint8_t registration;
};

struct wptc_read_cmd
{
    /// Set by profile
    uint8_t operation;
    /// Identifies what characteristic/descriptor is to be read
    uint8_t read_code;
};

struct wptc_write_cmd
{
    /// Set by profile
    uint8_t operation;
    /// Identifies what characteristic/descriptor is to be read
    uint8_t write_code;

    /// value - union of 2 structures & uint16
    union wptc_write_value_tag
    {
        /// PRU control information
        struct wptc_pru_control pru_ctrl;
        /// PTU Static information
        struct wptc_ptu_static  ptu_static;
        /// PRU CCC for the Alert Characteristic
        uint16_t alert_ccc;
    } value;
};

struct wptc_value_ind
{
    /// Identifies the Type of Value Received
    uint8_t attribute_code;

    /// value - union of 2 structures & uint16
    union wptc_value_ind_tag
    {
        /// PRU control
        struct wptc_pru_control pru_ctrl;
        /// PTU Static information
        struct wptc_ptu_static  ptu_static;
        /// PTU Static information
        struct wptc_pru_static  pru_static;
        /// PRU Dynamic information
        struct wptc_pru_dynamic pru_dynamic;
        /// PRU Alert information
        struct wptc_pru_alert   pru_alert;
        /// PRU Mode Transition information
        struct wptc_pru_mode_transition pru_mode_transition;
        /// PRU CCC for the Alert Characteristic
        uint16_t alert_ccc;
    } value;
};

/// Parameters of the @see WPTC_CMP_EVT message
/// Complete Event Information
struct  wptc_cmp_evt
{
    /// Operation code
    uint8_t operation;
    /// Status
    uint8_t status;
};


/// @} WPTCTASK

#endif //(_WPTC_TASK_H_)
