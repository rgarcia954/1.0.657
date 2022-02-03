/**
 * @file bondlist.h
 * @brief Bluetooth Bond list header
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

#ifndef BONDLIST_H
#define BONDLIST_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint16_t  state;
    uint8_t   reserved0[2];
    uint8_t   ltk[16];
    uint16_t  ediv;
    uint8_t   reserved1[2];
    uint8_t   addr[6];
    uint8_t   addr_type;
    uint8_t   reserved2;
    uint8_t   csrk[16];
    uint8_t   irk[16];
    uint8_t   rand[8];
} BondInfo_t; /* 72 bytes */

/* User application can override the location and size of bond list
 * by defining the following symbols:
 *     BOND_INFO_BASE
 *     BOND_INFO_FLASH_SECTORS_COUNT
 * If not defined by user application, it's set by default to 1 flash
 * sector (2KB) and the following default locations. */

#ifndef BOND_INFO_BASE
#define BOND_INFO_BASE          FLASH_BOND_INFO_BASE
#endif

#ifndef BOND_INFO_FLASH_SECTORS_COUNT
#define BOND_INFO_FLASH_SECTORS_COUNT      8
#endif

#if BOND_INFO_FLASH_SECTORS_COUNT < 1
    #error "The number of flash sectors should be greater than 1"
#endif

#define FLASH_SECTOR_SIZE               256
/* For 8 sectors (2KB) there is about 28 bond elements */
#define BONDLIST_MAX_SIZE               ((FLASH_SECTOR_SIZE * BOND_INFO_FLASH_SECTORS_COUNT) / sizeof(BondInfo_t))
#define BOND_INFO_STATE_INVALID         0x00
#define BOND_INFO_STATE_EMPTY           0xFFFF
#define BOND_INFO_STATE_VALID(state)    ((state != BOND_INFO_STATE_INVALID) && \
                                         (state != BOND_INFO_STATE_EMPTY) &&   \
                                         (state <= BONDLIST_MAX_SIZE))

/* Bonding list */
#define BD_TYPE_PUBLIC                  0
#define BD_TYPE_PRIVATE                 1

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* BONDLIST_H_ */
