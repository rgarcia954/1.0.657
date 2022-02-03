/**
 ****************************************************************************************
 *
 * @file wptp_common.h
 *
 * @brief Header File - Wireless Power Transfer Profile common types.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 *
 ****************************************************************************************
 */

#ifndef _WPTP_COMMON_H_
#define _WPTP_COMMON_H_

#include "rwip_config.h"

#if (BLE_WPT_CLIENT || BLE_WPT_SERVER)

#include "prf_types.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// WPT_CHARACTERISTIC_BASE_UUID 0x6455e670-a146-11e2-9e96-0800200c9a67
#define WPT_CHAR_BASE_UUID(ofs)      {0x67+ofs, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, \
                                         0xe2, 0x11, 0x46, 0xa1, 0x70, 0xe6, 0x55, 0x64}
/// WPT SERVICE UUID - 16 bit
#define WPT_SERVICE_UUID ATT_UUID_16(0xFFFE)
/// Mask to check if Notifications are enabled in the ALERT CCC

#define WPT_CCC_NTF_ENABLED  (1<<0)
/// Mask to check if Indications are enabled in the ALERT CCC
#define WPT_CCC_IND_ENABLED  (1<<1)
/// Max value of CCC Field
#define WPT_MAX_NTF_CFG_VALUE 3

///******CHARACTERISTIC VALUE HANDLE OFFSETs *****************/
/// Defines the Handle offset of each Characteristic Values/Descriptor from the Primary service handle
///
/// Offset for the PRU Control Characteristic Value
#define WPTC_CHAR_PRU_CONTROL_HDL_OFFSET   2
/// Offset for the PTU Static Characteristic Value
#define WPTC_CHAR_PTU_STATIC_HDL_OFFSET    4
/// Offset for the PRU Alert Characteristic Value
#define WPTC_CHAR_PRU_ALERT_HDL_OFFSET     6
/// Offset for the PRU Static Characteristic Value
#define WPTC_CHAR_PRU_STATIC_HDL_OFFSET    9
/// Offset for the PRU Dynamic Characteristic Value
#define WPTC_CHAR_PRU_DYNAMIC_HDL_OFFSET  11
/// Offset for the PRU CCC Descriptor for the Alert Characteristic
#define WPTC_DESC_PRU_ALERT_CCC_HDL_OFFSET 7


///***** PRU FIELDS MASKs  *******/
/// #defines used to mask off specific fields of characteristics
///
///   Mask for 2 power bits
#define WPT_PRU_CONTROL_ENABLE_POWER_MASK             (3<<4)
/// Mast to remove the two lower bits of PRU information which are RFU
#define WPTP_PRU_INFORMATION_MASK                     (0xFC)
/// Mask used for the Mode_Transition bits in the PRU Alert Field
#define WPTP_ALERT_MODE_TRANSITION_MASK               (3<<0)
/// Mask used for the optional fields in the optional_fields_validity of the PRU Dynamic Characteristic
#define WPTP_PRU_DYNAMIC_OPTIONAL_FIELD_MASK          (0xFC)
/// Mask used to check if Delta_r1_field is present in optional_fields_validity of Static PRU characteristic
#define WPTP_DELTA_R1_VALID                           (0x80)

///***** PRU CHARACTERISTIC LENGTHS  *******/
/// #defines used for the size/length of specific characteristics
///
/// Size/Length of the PRU dynamic value Characteristic
#define PRU_DYNAMIC_VAL_SIZE                            (20)
/// PRU Control Characteristic Value Length/Size is 5 Octets
#define PRU_CTRL_VAL_SIZE                               (5)
/// The Size of an ALERT CHAR used for Notification
#define PRU_ALERT_NTF_MAX_SIZE                          (1)
/// The Size of an ALERT_CHAR used for Indications
#define PRU_ALERT_IND_MAX_SIZE                          (7)
/// The size of PRU_STATIC Characteristic Value
#define PRU_STATIC_VAL_SIZE                             (20)
/// The size of PTU_STATIC Characteristic Value
#define PTU_STATIC_VAL_SIZE                             (17)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Enum used at API for Power Adjustment
enum wpt_pru_control_adjust_power_id
{
    ///   Max Power
    WPT_PRU_CONTROL_ADJUST_POWER_MAXIMUM                    = (0),
    ///   66% Max Power
    WPT_PRU_CONTROL_ADJUST_POWER_0_66_MAX                   = (1),
    ///   33% Max Power
    WPT_PRU_CONTROL_ADJUST_POWER_0_33_MAX                   = (2),
    ///   2.5 Watts Power
    WPT_PRU_CONTROL_ADJUST_POWER_2_5_WATT                   = (3),
};

/// Enum used at API for Permissions Field
enum wpt_pru_control_permission_id
{
    /// Permitted
    WPT_PRU_CONTROL_PERMITTED_WITHOUT_REASON             = 0x00,
    /// Permitted after a defined amount of time
    WPT_PRU_CONTROL_PERMITTED_WITH_WAIT                  = 0x01,
    /// Denied due to Cross Connection to other device
    WPT_PRU_CONTROL_DENIED_DUE_CROSS_CONNECTION          = 0x80,
    /// Denied due to Limited Power
    WPT_PRU_CONTROL_DENIED_DUE_LIMITED_POWER             = 0x81,
    /// Denied due to Limited Devices
    WPT_PRU_CONTROL_DENIED_DUE_LIMITED_NUMBER_OF_DEVICES = 0x82,
    /// Denied due to limited Class Support
    WPT_PRU_CONTROL_DENIED_DUE_LIMITED_CLASS_SUPPORT     = 0x83,
    /// Denied due to high temperature in the PTU
    WPT_PRU_CONTROL_DENIED_DUE_HIGH_TEMPERATURE_PTU      = 0x84,
};

/// Enum used at API for Time Set Field
enum wpt_pru_control_time_set_id
{
    /// Time not Set
    WPT_PRU_CONTROL_SET_TIME_DO_NOT_SET    = 0,
    /// Set Time = 10ms
    WPT_PRU_CONTROL_SET_TIME_10_MS         = 1,
    /// Set Time = 20ms
    WPT_PRU_CONTROL_SET_TIME_20_MS         = 2,
    /// Set Time = 30ms
    WPT_PRU_CONTROL_SET_TIME_30_MS         = 3,
    /// Set Time = 40ms
    WPT_PRU_CONTROL_SET_TIME_40_MS         = 4,
    /// Set Time = 50ms
    WPT_PRU_CONTROL_SET_TIME_50_MS         = 5,
    /// Set Time = 60ms
    WPT_PRU_CONTROL_SET_TIME_60_MS         = 6,
    /// Set Time = 70ms
    WPT_PRU_CONTROL_SET_TIME_70_MS         = 7,
    /// Set Time = 80ms
    WPT_PRU_CONTROL_SET_TIME_80_MS         = 8,
};

/// Enum used at API for Class Codes
enum wpt_ptu_class_codes
{
    /// Class 1
    WPTP_PTU_CLASS_1 = 0,
    /// Class 2
    WPTP_PTU_CLASS_2 = 1,
    /// Class 3
    WPTP_PTU_CLASS_3 = 2,
    /// Class 4
    WPTP_PTU_CLASS_4 = 3,
    /// Class 5
    WPTP_PTU_CLASS_5 = 4,
};

/// Enum used for Number of Devices Supported
enum wpt_ptu_num_devices_supported_codes
{
    /// 1 Device
    WPTP_PTU_NUM_DEVICES_1 = 0,
    /// 2 Device
    WPTP_PTU_NUM_DEVICES_2 = 1,
    /// 3 Device
    WPTP_PTU_NUM_DEVICES_3 = 2,
    /// 4 Device
    WPTP_PTU_NUM_DEVICES_4 = 3,
    /// 5 Device
    WPTP_PTU_NUM_DEVICES_5 = 4,
    /// 6 Device
    WPTP_PTU_NUM_DEVICES_6 = 5,
    /// 7 Device
    WPTP_PTU_NUM_DEVICES_7 = 6,
    /// 8 Device
    WPTP_PTU_NUM_DEVICES_8 = 7,
};

/// Enum used at API for optional_validity fields of the PTU static information
enum wpt_ptu_static_validity_bf
{
    // Bit 0 - 5 RFU
    /// Bit to check if optional MAX_RESISTANCE field is Valid
    WPT_STATIC_MAX_RESISTANCE_POS = 6,
    WPT_STATIC_MAX_RESISTANCE_BIT = CO_BIT(WPT_STATIC_MAX_RESISTANCE_POS),

    /// Bit to check if optional MAX_IMPEDANCE field is Valid
    WPT_STATIC_MAX_IMPEDANCE_POS = 7,
    WPT_STATIC_MAX_IMPEDANCE_BIT = CO_BIT(WPT_STATIC_MAX_IMPEDANCE_POS),
};

/// pru_category values
enum wptp_category_codes
{
    /// Undefined Category
    WPTS_CATEGORY_UNDEFINED = (0),
    /// Category 1
    WPTS_CATEGORY_1         = (1),
    /// Category 2
    WPTS_CATEGORY_2         = (2),
    /// Category 3
    WPTS_CATEGORY_3         = (3),
    /// Category 4
    WPTS_CATEGORY_4         = (4),
    /// Category 5
    WPTS_CATEGORY_5         = (5)
};

/// Attribute_ids - used in WPTC_VALUE_IND and WPTS_VALUE_REQ_IND
enum wptc_attribute_ids
{
    /// Illegal ATT ID
    WPTC_ILLEGAL_ATT_ID = 0,
    /// PRU Static
    WPTC_PRU_STATIC_ATT_ID = 1,
    /// PTU Static
    WPTC_PTU_STATIC_ATT_ID = 2,
    /// PRU Control
    WPTC_PRU_CONTROL_ATT_ID = 3,
    /// PRU Dynamic
    WPTC_PRU_DYNAMIC_ATT_ID = 4,
    /// PRU Alert
    WPTC_PRU_ALERT_ATT_ID = 5,
    /// Mode Transition
    WPTC_PRU_MODE_TRANSITION_ID = 6,
    /// PRU Alert CCC
    WPTC_PRU_ALERT_CCC_ATT_ID = 7
};

///***** PRU CONTROL ENABLES *******/
/// Bit field used for PRU Control Characteristic Field Manipulation
enum wpt_pru_control_enables_bf
{
    // Bit 0 - 3 RFU

    /// Adjust power command LSB
    /// 00 = Maximum power
    /// 01 = 66% * Prect_max
    /// 10 = 33% * Prect_max
    /// 11 = 2.5 W
    WPT_PRU_CONTROL_ADJUST_POWER_CMD_LSB_POS                = 4,
    WPT_PRU_CONTROL_ADJUST_POWER_CMD_LSB_BIT                = CO_BIT(WPT_PRU_CONTROL_ADJUST_POWER_CMD_LSB_POS),

    /// Adjust power command MSB
    WPT_PRU_CONTROL_ADJUST_POWER_CMD_MSB_POS                = 5,
    WPT_PRU_CONTROL_ADJUST_POWER_CMD_MSB_BIT                = CO_BIT(WPT_PRU_CONTROL_ADJUST_POWER_CMD_MSB_POS),

    /// Enable PRU charge indicator
    /// 1 = Enable, 0 = Disable
    WPT_PRU_CONTROL_ENABLE_PRU_CHARGE_INDICATOR_POS         = 6,
    WPT_PRU_CONTROL_ENABLE_PRU_CHARGE_INDICATOR_BIT         = CO_BIT(WPT_PRU_CONTROL_ENABLE_PRU_CHARGE_INDICATOR_POS),

    /// Enable PRU output
    /// 1 = Enable, 0 = Disable
    WPT_PRU_CONTROL_ENABLE_PRU_OUTPUT_POS                   = 7,
    WPT_PRU_CONTROL_ENABLE_PRU_OUTPUT_BIT                   = CO_BIT(WPT_PRU_CONTROL_ENABLE_PRU_OUTPUT_POS),
};

///***** PRU INFORMATION  *******/
/// Bit field used for PRU Information Field Manipulation
enum wpt_pru_information_bf
{
    // Bit 0 - 1 RFU

    /// PTU Test Mode
    /// 1 = Yes, 0 = No
    WPT_INFO_PTU_TEST_MODE_POS                              = 2,
    WPT_INFO_PTU_TEST_MODE_BIT                              = CO_BIT(WPT_INFO_PTU_TEST_MODE_POS),

    /// Charge Complete Connected Mode
    /// 1 = Supported, 0 = Not supported
    WPT_INFO_CHARGE_COMPLETE_CONNECTED_MODE_POS             = 3,
    WPT_INFO_CHARGE_COMPLETE_CONNECTED_MODE_BIT             = CO_BIT(WPT_INFO_CHARGE_COMPLETE_CONNECTED_MODE_POS),

    /// Adjust Power capability
    /// 1 = Supported, 0 = Not supported
    WPT_INFO_ADJUST_POWER_CAPABILITY_POS                    = 4,
    WPT_INFO_ADJUST_POWER_CAPABILITY_BIT                    = CO_BIT(WPT_INFO_ADJUST_POWER_CAPABILITY_POS),

    /// Power Control Algorithm Preference
    /// 1 = Max, 0 = Vrect_min_erro
    WPT_INFO_POWER_CONTROL_ALGO_PREFERENCE_POS              = 5,
    WPT_INFO_POWER_CONTROL_ALGO_PREFERENCE_BIT              = CO_BIT(WPT_INFO_POWER_CONTROL_ALGO_PREFERENCE_POS),

    /// Separate BTLE radio in PRU
    /// 1 = Supported, 0 = Not supported
    WPT_INFO_SEPARATE_BTLE_RADIO_IN_PRU_POS                 = 6,
    WPT_INFO_SEPARATE_BTLE_RADIO_IN_PRU_BIT                 = CO_BIT(WPT_INFO_SEPARATE_BTLE_RADIO_IN_PRU_POS),

    /// NFC receiver
    /// 1 = Supported, 0 = Not supported
    WPT_INFO_NFC_RECEIVER_POS                               = 7,
    WPT_INFO_NFC_RECEIVER_BIT                               = CO_BIT(WPT_INFO_NFC_RECEIVER_POS),
};

///***** PRU DYNAMIC PARAMETER OPTIONAL FIELDS VALIDITY  *******/
/// Bit field used to determine which optional fields in a characteristic value are valid
enum wpt_pru_dynamic_validity_bf
{
    // Bit 0 - 1 RFU

    /// This Bit indicates the 'Highest Dynamic Voltage of rectifier' field
    WPT_VRECT_HIGH_DYN_POS                                  = 2,
    WPT_VRECT_HIGH_DYN_BIT                                  = CO_BIT(WPT_VRECT_HIGH_DYN_POS),

    /// This Bit indicates the 'Current Dynamic Voltage of rectifier' field
    WPT_VRECT_SET_DYN_POS                                   = 3,
    WPT_VRECT_SET_DYN_BIT                                   = CO_BIT(WPT_VRECT_SET_DYN_POS),

    /// This Bit indicates the 'Minimum Dynamic Voltage of rectifier' field
    WPT_VRECT_MIN_DYN_POS                                   = 4,
    WPT_VRECT_MIN_DYN_BIT                                   = CO_BIT(WPT_VRECT_MIN_DYN_POS),

    /// This Bit indicates the temperature field
    WPT_TEMPERATURE_POS                                     = 5,
    WPT_TEMPERATURE_BIT                                     = CO_BIT(WPT_TEMPERATURE_POS),

    /// This Bit indicates the output current field
    WPT_IOUT_POS                                            = 6,
    WPT_IOUT_BIT                                            = CO_BIT(WPT_IOUT_POS),

    /// This Bit indicates the output voltage field
    WPT_VOUT_POS                                            = 7,
    WPT_VOUT_BIT                                            = CO_BIT(WPT_VOUT_POS),
};

///***** PRU ALERT FIELDS  *******/
/// Bit field used to determine the alert condition in a PRU ALERT field valid
enum wpt_pru_alert_bf
{
    /// Adjust Power Response
    WPT_ALERT_ADJUST_POWER_RESPONSE_POS                     = 0,
    WPT_ALERT_ADJUST_POWER_RESPONSE_BIT                     = CO_BIT(WPT_ALERT_ADJUST_POWER_RESPONSE_POS),

    /// PRU Charge Port
    WPT_ALERT_PRU_CHARGE_PORT_POS                           = 1,
    WPT_ALERT_PRU_CHARGE_PORT_BIT                           = CO_BIT(WPT_ALERT_PRU_CHARGE_PORT_POS),

    /// Wired Charger Detect
    WPT_ALERT_WIRED_CHARGER_DETECT_POS                      = 2,
    WPT_ALERT_WIRED_CHARGER_DETECT_BIT                      = CO_BIT(WPT_ALERT_WIRED_CHARGER_DETECT_POS),

    /// Wired Charger Complete
    WPT_ALERT_CHARGE_COMPLETE_POS                           = 3,
    WPT_ALERT_CHARGE_COMPLETE_BIT                           = CO_BIT(WPT_ALERT_CHARGE_COMPLETE_POS),

    /// PRU Self Protection
    WPT_ALERT_PRU_SELF_PROTECTION_POS                       = 4,
    WPT_ALERT_PRU_SELF_PROTECTION_BIT                       = CO_BIT(WPT_ALERT_PRU_SELF_PROTECTION_POS),

    /// Over-temp
    WPT_ALERT_OVER_TEMP_POS                                 = 5,
    WPT_ALERT_OVER_TEMP_BIT                                 = CO_BIT(WPT_ALERT_OVER_TEMP_POS),

    /// Over-current
    WPT_ALERT_OVER_CURRENT_POS                              = 6,
    WPT_ALERT_OVER_CURRENT_BIT                              = CO_BIT(WPT_ALERT_OVER_CURRENT_POS),

    /// Over-voltage
    WPT_ALERT_OVER_VOLTAGE_POS                              = 7,
    WPT_ALERT_OVER_VOLTAGE_BIT                              = CO_BIT(WPT_ALERT_OVER_VOLTAGE_POS),
};

/*
 * MACROS
 ****************************************************************************************
 */

/// Macro to determine if the SET_TIME field is valid
#define WPT_VALIDATE_TIME(x) (x<=WPT_PRU_CONTROL_SET_TIME_80_MS)
/// Macro to ensure the Permission fields is within valid range
#define WPT_CONTROL_VALIDATE_PERMISSIONS(x) (((x>=WPT_PRU_CONTROL_DENIED_DUE_CROSS_CONNECTION) && \
                            (x<=WPT_PRU_CONTROL_DENIED_DUE_HIGH_TEMPERATURE_PTU)) \
                            || (x<=WPT_PRU_CONTROL_PERMITTED_WITH_WAIT))

/// Macro to ensure the Optional Fields Validity field of Static Characteristic is within range
#define WPT_STATIC_OPTIONAL_FIELDS_VALIDITY  (WPT_STATIC_MAX_IMPEDANCE_BIT | WPT_STATIC_MAX_RESISTANCE_BIT)

/// Macro to ensure the PTU power field is within range - Values in Decimal, Power in Watts
#define WPT_VALIDATE_PTU_POWER(x) (x<=139)
/// Macro to ensure the ptu_max_source_impedance field is within range
#define WPT_VALIDATE_PTU_MAX_SOURCE_IMPEDANCE(x) (((x& 0x07) == 0) && (x<=(18<<3)))
/// Macro to ensure the ptu_max_load_resistance is within range
#define WPT_VALIDATE_PTU_MAX_LOAD_RESISTANCE(x) (((x& 0x07) == 0) && (x<=(10<<3)))
/// Macro to ensure the PTU Class is within range
#define WPT_VALIDATE_PTU_CLASS(x) (x<=WPTP_PTU_CLASS_5)
/// Macro to ensure the value of Max_devices_supported is valid / within range
#define WPT_VALIDATE_PTU_MAX_DEVICES_SUPPORTED(x) ((x>=WPTP_PTU_NUM_DEVICES_1+1) && (x<=WPTP_PTU_NUM_DEVICES_8+1))


/*
 * STRUCTURES
 ****************************************************************************************
 */
/// PRU_CONTROL Characteristic Value - as defined in the ATT-DB
struct wpt_wr_pru_control_char_val
{
    /// PRU turn on, PRU on indication, etc. ref@ PRU_CONTROL_Enables
    uint8_t enables;
    /// PRU is permitted in PTU. ref@ wpt_pru_control_permission_id
    uint8_t permission;
    /// PTU sets up time. ref@ wpt_pru_control_time_set_id
    uint8_t time_set;
    /// RFU
    uint8_t rfu[2];
};

/// PTU_Static Characteristic Value
struct  wpt_wr_ptu_static_char_val
{
    /// Defines which fields are valid
    uint8_t optional_fields_validity;
    /// Power of PTU
    uint8_t ptu_power;
    /// Maximum source impedance of the PTU
    uint8_t ptu_max_source_impedance;
    /// Maximum load resistance of the PTU
    uint8_t ptu_max_load_resistance;
    /// undefined
    uint8_t rfu[2];
    /// PTU class
    uint8_t ptu_class;
    /// Revision of the PTU HW
    uint8_t hardware_rev;
    /// Revision of the PTU SW
    uint8_t firmware_rev;
    /// A4WP Supported Revision
    uint8_t protocol_rev;
    /// Max Number of Devices
    uint8_t ptu_num_devices_supported;
    /// undefined
    uint8_t rfu2[6];
};

/// PRU_Static Characteristic Value
struct  wpt_rd_pru_static_char_val
{
    /// Defines which fields are valid
    uint8_t optional_fields_validity;
    /// A4WP Supported Revision
    uint8_t protocol_revision;
    /// undefined
    uint8_t rfu;
    /// Category of PRU
    uint8_t pru_category;
    /// Capabilities of PRU (bit field)
    uint8_t pru_information;
    /// Revision of the PRU HW
    uint8_t hardware_rev;
    /// Revision of the PRU SW
    uint8_t firmware_rev;
    /// PRECT_MAX of the PRU. The value is in increments of 100mW
    uint8_t prect_max;
    /// VRECT_MIN (static, first estimate). The value is in mV.
    uint16_t vrect_min_static;
    /// VRECT_HIGH (static, first estimate). The value is in mV.
    uint16_t vrect_high_static;
    /// VRECT_SET. The value is in mV.
    uint16_t vrect_set;
    /// Delta R1 caused by PRU. The PRU may report its Delta R1, if included, in increments of 0.01 ohms.
    uint16_t delta_r1_value;
    /// undefined
    uint8_t rfu2[4];
};

/// PTU Dynamic Characteristic Value
struct wpt_rd_pru_dynamic_char_val
{
    /// Defines which fields are valid
    uint8_t optional_fields_validity;
    /// DC voltage at the output of the rectifier. mV
    uint16_t vrect;
    /// DC current at the output of the rectifier. mA
    uint16_t irect;
    /// Voltage at charge/battery port. mV
    uint16_t vout;
    /// Current at charge/battery port. mA
    uint16_t iout;
    /// Temperature of PRU. Deg C from -40C
    uint8_t  temperature;
    /// The current dynamic minimum rectifier voltage desired. mV
    uint16_t vrect_min_dyn;
    /// Desired VRECT (dynamic value). mV
    uint16_t vrect_set_dyn;
    /// The current dynamic maximum rectifier voltage desired. mV
    uint16_t vrect_high_dyn;
    /// Warnings. Bit field
    uint8_t  pru_alert;
    /// PTU Test Mode Command
    uint8_t  tester_command;
    /// undefined
    uint8_t rfu2[2];
};

/// PRU_ALERT Characteristic Value
struct wpt_pru_alert_notify_indicate_val
{
    /// bit field of the alerts reason
    uint8_t pru_alert;
    /// Device Address
    uint8_t addr[6];
};

#endif /* (BLE_WPT_CLIENT || BLE_WPT_SERVER) */

#endif /* _WPTP_COMMON_H_ */
