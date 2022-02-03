/**
 * @file bondlist.h
 * @brief BLE Bond list header
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

#ifndef BONDLIST_H
#define BONDLIST_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include <gap.h>
#include <hw.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/**
 * @brief Bond Information
 */
typedef struct
{
    uint16_t state;             /**< State of bond */
    uint8_t pairing_lvl;        /**< Pairing level */
    uint8_t csrk_exchanged;     /**< Non-zero if CSRK has been exchanged */
    uint8_t ltk[16];            /**< Long term key */
    uint16_t ediv;              /**< Encryption diversifier */
    uint8_t reserved1[2];       /**< Reserved */
    uint8_t addr[6];            /**< Peer address */
    uint8_t addr_type;          /**< Address type */
    uint8_t irk_exchanged;      /**< Non-zero if IRK has been exchanged */
    uint8_t csrk[16];           /**< Connection resolving signature key */
    uint8_t irk[16];            /**< Identity resolving key */
    uint8_t rand[8];            /**< Random number */
} BondInfo_t;                   /**< 72 bytes */

/** User application can override the location and size of the bond list
 * by defining the following symbols:
 * BOND_INFO_BASE
 * BOND_INFO_FLASH_SECTORS_COUNT
 * If not defined by user application, it's set by default to 1 flash
 * sector (2KB) and the following default locations. */

#ifndef BOND_INFO_BASE
#define BOND_INFO_BASE          FLASH_BOND_INFO_BASE    /**< Start of address for bond info */
#endif    /* ifndef BOND_INFO_BASE */

#ifndef BOND_INFO_FLASH_SECTORS_COUNT
#define BOND_INFO_FLASH_SECTORS_COUNT      8        /**< Number of sectors for bond info */
#endif    /* ifndef BOND_INFO_FLASH_SECTORS_COUNT */

#if BOND_INFO_FLASH_SECTORS_COUNT < 1
    #error "The number of flash sectors should be greater than 1"
#endif    /* if BOND_INFO_FLASH_SECTORS_COUNT < 1 */

#define FLASH_DATA_ARRAY_SECTOR_SIZE        256     /**< Size of each sector */

/** For 8 sectors (2KB) there is about 28 bond elements */
#define BONDLIST_MAX_SIZE               ((FLASH_DATA_ARRAY_SECTOR_SIZE * BOND_INFO_FLASH_SECTORS_COUNT) / \
                                         sizeof(BondInfo_t))

/** Invalid bond info state */
#define BOND_INFO_STATE_INVALID         0x00

/** Empty bond info state */
#define BOND_INFO_STATE_EMPTY           0xFFFF

/** Macro for valid bond info state */
#define BOND_INFO_STATE_VALID(state)    ((state != BOND_INFO_STATE_INVALID) && \
                                         (state != BOND_INFO_STATE_EMPTY) &&   \
                                         (state <= BONDLIST_MAX_SIZE))

/**
 * @brief Bondlist functions
 */

/**
 * @brief Get the number of entries in the bond list stored to flash
 *
 * @return Number of entries in the the bond list with a valid bond state
 */
uint8_t BondList_Size(void);

/**
 * @brief Get the IRKs from bond lists
 *
 * Get the IRKs from bond lists having valid bond state
 *
 * @param[in] irks Pointer to gap_sec_key structure
 * @return Number of IRKs found
 */
uint8_t BondList_GetIRKs(struct gap_sec_key *irks);

/**
 * @brief Search for the bond information matching specified IRK in flash
 *
 * @param[in] irk Matching IRK to search for
 * @return If found an entry in flash, return its address as a pointer to a const BondInfo_t element
 *         NULL otherwise
 */
const BondInfo_t * BondList_FindByIRK(const uint8_t *irk);

/**
 * @brief Search for the bonding information for a peer device in flash matching specified
 * address and address type.
 *
 * @param[in] addr     Address of the peer device to search
 * @param[in] addrType Address type of the peer device to search
 * @return If found an entry in flash, return its address as a pointer to a const BondInfo_t element
 *         NULL otherwise
 */
const BondInfo_t * BondList_FindByAddr(const uint8_t *addr, uint8_t addrType);

/**
 * @brief Squeeze bond information together to make space for a new entry.
 *
 * @return True if successful, false otherwise
 */
bool BondList_FlashDefrag(void);

/**
 * @brief Add bond information to the bond list.
 *
 * @param[in] bond_info Pointer to bond information
 * @return Non-zero if bond information is added to bond list successfully,
 *         false otherwise
 */
uint16_t BondList_Add(BondInfo_t *bond_info);

/**
 * @brief Remove a bond list entry matching the specified index.
 *
 * @param[in] bondStateIndex Index of bond list to remove
 * @return True if successful, false otherwise
 */
bool BondList_Remove(uint16_t bondStateIndex);

/**
 * @brief Erase bond list sectors containing bond list information in flash
 *
 * @return If found an error erasing any sector, return false
 *         true otherwise
 */
bool BondList_RemoveAll(void);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BONDLIST_H_ */
