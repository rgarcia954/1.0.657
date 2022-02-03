/**
 * @file ble_protocol_support.h
 * @brief BLE protocol support layer header
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

#ifndef BLE_PROTOCOL_SUPPORT_H
#define BLE_PROTOCOL_SUPPORT_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <ble_protocol_config.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/** Vendor specific to enable CW (RX or TX) */
#define HCI_VS_RF_CW_ENABLE_CMD_CODE    0x01

/** Vendor specific to enable CW (RX or TX) */
#define HCI_VS_RF_CW_DISABLE_CMD_CODE   0x02

/** Vendor specific to set desired output power */
#define HCI_VS_RF_OUTPUT_PWR_CMD_CODE   0x03

/**
 * @brief Application defined low power clocks and sources
 */
struct low_power_clock
{
    uint8_t low_pwr_clk_xtal32;         /**< Value of LPCLK_SRC_XTAL32 from application */
    uint8_t low_pwr_clk_rc32;           /**< Value of LPCLK_SRC_RC32 from application */
    uint8_t low_pwr_standby_clk_src;    /**< Value of LPCLK_STANDBYCLK_SRC from application */
};

/**
 * @brief Application defined BLE device parameters used by BLE stack
 */
struct ble_device_parameter
{
    uint32_t low_pwr_clk_accuracy;          /**< Low power clock accuracy in ppm */
    uint32_t twosc;                         /**< TWOSC us */
    struct low_power_clock low_pwr_clk;     /**< Low power clock selection */
};

/**
 * @brief Structure to save BLE device parameters provided by the application
 */
extern struct ble_device_parameter ble_dev_params;

/**
 * @brief Read the Bluetooth public address to ble_public_addr
 *
 * Read the Bluetooth public address from a given location
 * and save it to ble_public_addr (array).
 *
 * @param [in] ble_addr_location Location to read value from
 */
void Device_BLE_Public_Address_Read(uint32_t ble_addr_location);

/**
 * @brief Read BLE device parameters
 *
 * Read bluetooth low energy parameters, security keys, and channel assessment
 * parameters that are provided by the application or NVR3.
 *
 * @param [in] param_id  Parameter identifier
 * @param [in] lengthPtr Pointer to the length of parameter
 * @param [in] buf       Pointer to returned value
 * @return Indicate if the requested parameter is provided by application
 */
uint8_t Device_BLE_Param_Get(uint8_t param_id, uint8_t *lengthPtr, uint8_t *buf);

/**
 * @brief Generate a pseudo-random number
 *
 * @return A pseudo-random number
 */
int rand_func(void);

/**
 * @brief Seeds pseudo-random number generator used by function rand()
 *
 * @param[in] seed Seed value
 */
void srand_func(uint32_t seed);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_PROTOCOL_SUPPORT_H */
