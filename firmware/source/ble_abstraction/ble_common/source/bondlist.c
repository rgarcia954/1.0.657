/**
 * @file bondlist.c
 * @brief Source for BLE Bond list parameters
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

#include <bondlist.h>

#include <string.h>
#include <flash_rom.h>
#include <stdlib.h>

#define FLASH_ERASED_WORD_VALUE       0xFFFFFFFF

BondInfo_t *BONDLIST = (BondInfo_t *)BOND_INFO_BASE;

uint8_t BondList_Size(void)
{
    uint8_t size = 0;

    for (uint32_t i = 0; i < BONDLIST_MAX_SIZE; i++)
    {
        if (BOND_INFO_STATE_VALID(BONDLIST[i].state))
        {
            size++;
        }
    }
    return (size);
}

uint8_t BondList_GetIRKs(struct gap_sec_key *irks)
{
    uint8_t numKeys = 0;
    for (uint32_t i = 0; i < BONDLIST_MAX_SIZE; i++)
    {
        if (BOND_INFO_STATE_VALID(BONDLIST[i].state))
        {
            memcpy(&irks[numKeys++], BONDLIST[i].irk, GAP_KEY_LEN);
        }
    }
    return (numKeys);
}

const BondInfo_t * BondList_FindByIRK(const uint8_t *irk)
{
    for (uint8_t i = 0; i < BONDLIST_MAX_SIZE; i++)
    {
        /* If bond info is valid and address match */
        if ((BOND_INFO_STATE_VALID(BONDLIST[i].state))
            && (memcmp(BONDLIST[i].irk, irk, GAP_KEY_LEN) == 0))
        {
            return &BONDLIST[i];
        }
    }
    return NULL;
}

const BondInfo_t * BondList_FindByAddr(const uint8_t *addr, uint8_t addrType)
{
    for (uint8_t i = 0; i < BONDLIST_MAX_SIZE; i++)
    {
        /* If bond info is valid and address match */
        if ((BOND_INFO_STATE_VALID(BONDLIST[i].state))
            && (BONDLIST[i].addr_type == addrType)
            && (memcmp(BONDLIST[i].addr, addr, GAP_BD_ADDR_LEN) == 0))
        {
            return &BONDLIST[i];
        }
    }
    return NULL;
}

bool BondList_FlashDefrag(void)
{
    uint32_t flash_status = 0;

    /* Allocate temporary memory to back up Flash before erasing the sector */
    BondInfo_t *flashTemp = malloc(sizeof(BondInfo_t) * (BondList_Size()));
    if (!flashTemp)
    {
        return false;    /* Malloc failed. Not enough heap */
    }

    /* Copy/squeeze valid entries together into temp buffer in RAM */
    uint8_t copied = 0;
    for (uint8_t i = 0; i < BONDLIST_MAX_SIZE; i++)
    {
        if (BOND_INFO_STATE_VALID(BONDLIST[i].state))
        {
            /* Read bond_info entry from Flash */
            memcpy(&flashTemp[copied], &BONDLIST[i], sizeof(BondInfo_t));

            /* Update STATE field with new index */
            flashTemp[copied].state = copied + 1;
            copied++;
        }
    }

    if (!BondList_RemoveAll())    /* Error erasing Flash */
    {
        /* Free temporary memory used to back up Flash (if any) */
        free(flashTemp);
        return false;
    }

    /* Write bond info back to Flash */
    flash_status = Flash_WriteBuffer((uint32_t)BONDLIST,
                                     (copied * sizeof(BondInfo_t)) / 4, (uint32_t *)flashTemp, 0)
                   == FLASH_ERR_NONE;

    /* Free temporary memory used to back up Flash (if any) */
    free(flashTemp);
    return flash_status;
}

uint16_t BondList_Add(BondInfo_t *bond_info)
{
    if (BondList_Size() >= BONDLIST_MAX_SIZE)
    {
        return BOND_INFO_STATE_INVALID;
    }

    /* If there is free space at least in the end of the reserved flash
     * area (i.e. we don't need to squeeze entries together) */
    if (BONDLIST[BONDLIST_MAX_SIZE - 1].state == BOND_INFO_STATE_EMPTY)
    {
        BondInfo_t bond_info_to_write;
        memcpy(&bond_info_to_write, bond_info, sizeof(BondInfo_t));

        /* Search for the first available position in Flash */
        for (uint8_t i = 0; i < BONDLIST_MAX_SIZE; i++)
        {
            if (BONDLIST[i].state == BOND_INFO_STATE_EMPTY)
            {
                bond_info_to_write.state = i + 1;    /* Update STATE field with new index */
                if (Flash_WriteBuffer((uint32_t)&BONDLIST[i],
                                      sizeof(BondInfo_t) / 4, (uint32_t *)&bond_info_to_write,
                                      0) != FLASH_ERR_NONE)
                {
                    return BOND_INFO_STATE_INVALID;
                }
                return bond_info_to_write.state;
            }
        }
    }
    else    /* Need to squeeze the bond info together to make space for a new entry */
    {
        if (BondList_FlashDefrag())
        {
            return BondList_Add(bond_info);
        }
    }
    return BOND_INFO_STATE_INVALID;
}

bool BondList_Remove(uint16_t bond_info_state_index)
{
    bool result = false;
    if (BOND_INFO_STATE_VALID(bond_info_state_index) &&
        BOND_INFO_STATE_VALID(BONDLIST[bond_info_state_index - 1].state))
    {
        /* Flash address where the bonding information will be invalidated */
        uint32_t *addr = (uint32_t *)&BONDLIST[bond_info_state_index - 1];

        /* Invalidate the state for the old bonding information stored in
         * Flash by clearing the first word. */
        result = (Flash_WriteWord((unsigned int)addr, BOND_INFO_STATE_INVALID,
                                  0) == FLASH_ERR_NONE);
    }
    return result;
}

bool BondList_RemoveAll(void)
{
    uint32_t i;
    uint32_t sector_start_addr;

    for (i = 0; i < BOND_INFO_FLASH_SECTORS_COUNT; i++)
    {
        sector_start_addr = (uint32_t)(BONDLIST) + (i * FLASH_DATA_ARRAY_SECTOR_SIZE);
        if (Flash_BlankCheck(sector_start_addr, DATA_SECTOR_LEN_WORDS) == FLASH_ERR_NONE)
        {
            continue;    /* Skip sector already erased to preserve flash */
        }

        if (Flash_EraseSector(sector_start_addr, 0) != FLASH_ERR_NONE)
        {
            return false;    /* Couldn't erase sector */
        }
    }
    return true;
}
