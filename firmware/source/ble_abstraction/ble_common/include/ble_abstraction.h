/**
 * @file ble_abstraction.h
 * @brief BLE Abstraction top header file
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

#ifndef BLE_ABSTRACTION_H
#define BLE_ABSTRACTION_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <RTE_Components.h>

#include <ble.h>
#include <ble_gap.h>
#include <ble_gatt.h>
#include <msg_handler.h>
#include <bondlist.h>

/**
 * @defgroup BLE_ABSTRACTIONg Bluetooth Low Energy Stack Abstraction
 * @brief Bluetooth Low Energy Stack Abstraction Reference
 *
 * This reference chapter presents a detailed description of all the support
 * provided for the Bluetooth Low Energy Stack Abstraction. This reference
 * includes calling parameters, returned values, and assumptions.
 * @{
 */

/** Profile abstraction headers (only included if the profile component is added in the RTE configuration) */
#ifdef RTE_ADDED_PRF_BASS
    #include <ble_bass.h>
#endif    /* ifdef RTE_ADDED_PRF_BASS */

#ifdef RTE_ADDED_PRF_BASC
    #include <ble_basc.h>
#endif    /* ifdef RTE_ADDED_PRF_BASC */

#ifdef RTE_ADDED_PRF_DISS
    #include <ble_diss.h>
#endif    /* ifdef RTE_ADDED_PRF_DISS */

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_ABSTRACTION_H */
