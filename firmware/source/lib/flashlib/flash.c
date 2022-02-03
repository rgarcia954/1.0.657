/**
 * @file flash.c
 * @brief C file implementing  API functions for flash management through flash
 *        interface
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
#include "flash_internal.h"
#include <hw.h>

/* Firmware revision code */
const short FlashLib_Version = FLASH_FW_VER;

/* Frequency map constants, frequencies have to increment like done here
 * for this to work */
const uint32_t freq_map[FREQ_MAP_NUM] =
{
    [FLASH_DELAY_FOR_SYSCLK_3MHZ] = FLASH_CLOCK_3MHZ,
    [FLASH_DELAY_FOR_SYSCLK_4MHZ] = FLASH_CLOCK_4MHZ,
    [FLASH_DELAY_FOR_SYSCLK_5MHZ] = FLASH_CLOCK_5MHZ,
    [FLASH_DELAY_FOR_SYSCLK_8MHZ] = FLASH_CLOCK_8MHZ,
    [FLASH_DELAY_FOR_SYSCLK_10MHZ] = FLASH_CLOCK_10MHZ,
    [FLASH_DELAY_FOR_SYSCLK_12MHZ] = FLASH_CLOCK_12MHZ,
    [FLASH_DELAY_FOR_SYSCLK_16MHZ] = FLASH_CLOCK_16MHZ,
    [FLASH_DELAY_FOR_SYSCLK_20MHZ] = FLASH_CLOCK_20MHZ,
    [FLASH_DELAY_FOR_SYSCLK_24MHZ] = FLASH_CLOCK_24MHZ,
    [FLASH_DELAY_FOR_SYSCLK_48MHZ] = FLASH_CLOCK_48MHZ,
};

/* Flash interfaces array constants */
extern const struct interface *ifcs[FLASH_INSTANCE_NUM];

void Sys_Flash_WriteWordOperation(FLASH_Type *flash, uint32_t addr,
                                  uint32_t word,
                                  uint32_t cmd_prg_type)
{
    /* Backup flash control register */
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    /* Execute flash write operation preconditions */
    Sys_Flash_ExecutePrecondFlashWrite(flash);

    /* Apply flash retry level ( Fixed for flash programming ) */
    Sys_Flash_ApplyRetryLevel(flash, FLASH_RETRY_4);

    flash->ADDR = addr;

    flash->DATA[0] = word;

    /* Execute a program operation */
    Sys_Flash_ExecuteCommand(flash, cmd_prg_type);

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);
}

void Sys_Flash_WriteDoubleOperation(FLASH_Type *flash, uint32_t addr,
                                    const uint32_t *word,
                                    uint32_t cmd_prg_type)
{
    /* Backup flash control register */
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    /* Execute flash write operation preconditions */
    Sys_Flash_ExecutePrecondFlashWrite(flash);

    /* Apply flash retry level ( Fixed for flash programming ) */
    Sys_Flash_ApplyRetryLevel(flash, FLASH_RETRY_4);

    flash->ADDR = addr;

    flash->DATA[1] = word[1];

    flash->DATA[0] = word[0];

    /* Execute a program operation */
    Sys_Flash_ExecuteCommand(flash, cmd_prg_type);

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);
}

uint32_t Sys_Flash_ReadWord(FLASH_Type *flash, uint32_t addr)
{
    /* Backup flash control register */
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    /* Execute flash write operation preconditions */
    Sys_Flash_ExecutePrecondFlashRead(flash);

    /* Use CBUS to perform a 32-bit word read */
    uint32_t r = *((uint32_t *)addr);

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);
    return r;
}

void Sys_Flash_ReadDouble(FLASH_Type *flash, uint32_t addr, uint32_t *word)
{
    /* Backup flash control register */
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    /* Execute flash write operation preconditions */
    Sys_Flash_ExecutePrecondFlashRead(flash);

    /* Apply flash retry level */
    flash->ADDR = addr;
    Sys_Flash_ExecuteCommand(flash, CMD_READ);
    word[0] = flash->DATA[0];
    word[1] = flash->DATA[1];

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);
}

FlashStatus_t Sys_Flash_WriteSequentialData(FLASH_Type *flash, uint32_t addr,
                                            const uint32_t *words,
                                            unsigned int word_length,
                                            unsigned int row_word_length,
                                            uint32_t cmd_prg_type,
                                            uint32_t *crc)
{
    /* Backup flash control register */
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    /* Execute flash write operation preconditions */
    Sys_Flash_ExecutePrecondFlashWrite(flash);

    /* Apply flash retry level ( Fixed for flash programming ) */
    Sys_Flash_ApplyRetryLevel(flash, FLASH_RETRY_4);

    uint32_t irq = __get_PRIMASK();

    /* Disable CPU interrupt using PRIMASK register */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

    /* Back-up CRC registers */
    uint32_t backup_CRC_CFG = CRC->CFG;
    uint32_t backup_CRC_VALUE = CRC->VALUE;

    /* Initialize CRC */
    CRC->CFG = VERIFY_WRITE_CRC_CFG;
    CRC->VALUE =  VERIFY_WRITE_CRC_INITIAL_VALUE;

    /* Write the address of the first word to be written */
    flash->ADDR = addr;

    /* Write the first word to be written */
    flash->DATA[0] = words[0];

    /* Write word to CRC peripheral register*/
    CRC->ADD_32 = words[0];

    /* Calculate the wrap around mask in bytes */
    unsigned int row_mask = (row_word_length << 2) - 1;

    /* Start a new sequential write sequence */
    Sys_Flash_ExecuteSeqCommand(flash, cmd_prg_type);

    /* Check if the interface did not accept sequential write command */
    FlashStatus_t r = Sys_Flash_IF_ReadBusy(flash) ? FLASH_ERR_NONE :
                      FLASH_ERR_UNKNOWN;

    if (r == FLASH_ERR_NONE)
    {
        for (unsigned int i = 1; i < word_length; i++)
        {
            /* Wait until new data is requested */
            Sys_Flash_IF_WaitSeqReq(flash);

            /* Write the first word to be written */
            flash->DATA[0] = words[i];

            /* Write word to CRC */
            CRC->ADD_32 = words[i];

            /* Wait until new data is requested */
            Sys_Flash_IF_WaitSeqReq(flash);

            /* Check if this write was the last word on the row. If it was we
             * need to complete this sequential write and start a new write
             * on the next row (if we have more data).
             */
            if ((flash->ADDR & row_mask) == 0)
            {
                /* Check if we need to write more data */
                if (i < (word_length - 1))
                {
                    /* End this sequential write cycle and start a new sequential write
                     * cycle for the remainder of the words */
                    Sys_Flash_EndOperationWaitIdle(flash);

                    /* Allow any critical interrupts to execute momentarily,
                     * for example update watchdog in background */
                    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
                    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

                    /* Address will wrap back to start of row after end of sector
                     * increment address to next row here */
                    flash->ADDR = addr +  ((i + 1) * 4);

                    /* Increment i index to next word */
                    uint32_t next_word =  words[++i];

                    /* Write the next word to be written */
                    flash->DATA[0] = next_word;

                    CRC->ADD_32 = next_word;

                    /* Start a new sequential write sequence */
                    Sys_Flash_ExecuteSeqCommand(flash, cmd_prg_type);

                    /* Check if the interface did not accept sequential write
                     * command */
                    if (Sys_Flash_IF_ReadBusy(flash) == 0)
                    {
                        r = FLASH_ERR_UNKNOWN;
                        break;
                    }
                }
            }
        }
    }

    /* Terminate program sequence if active */
    Sys_Flash_EndOperationWaitIdle(flash);

    if (r == FLASH_ERR_NONE)
    {
        /* Read final CRC value calculated for the words written */
        *crc = CRC->FINAL;
    }

    /* Restore CRC peripheral registers */
    CRC->CFG = backup_CRC_CFG;
    CRC->VALUE = backup_CRC_VALUE;

    __set_PRIMASK(irq);

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);

    return r;
}

void Sys_Flash_EraseMass(FLASH_Type *flash)
{
    /* Backup flash control register */
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    Sys_Flash_ExecutePrecondFlashMassErase(flash);

    Sys_Flash_ApplyRetryLevel(flash, FLASH_RETRY_4);

    Sys_Flash_ExecuteCommand(flash, CMD_MASS_ERASE);

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);
}

void Sys_Flash_EraseSectorOperation(FLASH_Type *flash, uint32_t addr,
                                    uint32_t retry_level)
{
    /* Backup flash control register */
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    Sys_Flash_ExecutePrecondFlashSectorErase(flash);

    /* Apply flash retry level */
    Sys_Flash_ApplyRetryLevel(flash, retry_level);

    flash->ADDR = addr;
    Sys_Flash_ExecuteCommand(flash, CMD_SECTOR_ERASE);

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);
}

FlashStatus_t Sys_Flash_EraseSectorEND(FLASH_Type *flash, uint32_t addr,
                                       uint32_t sector_length)
{
    FlashStatus_t r = FLASH_ERR_UNKNOWN;
    uint32_t i;
    for (i  = FLASH_RETRY_1;
         i <= FLASH_RETRY_4;
         i += (0x1U << FLASH_IF_CTRL_RETRY_Pos))
    {
        Sys_Flash_EraseSectorOperation(flash, addr, i);
        if (i < FLASH_RETRY_4)
        {
            /* Verify Read (VREAD1=1) is used to check whether a sector has been
             * successfully erased after each pulse with a higher threshold for
             * guaranteed erase.
             */
            r = Sys_Flash_Copier_VerifyEmpty(flash, addr, sector_length,
                                             true);
        }
        else
        {
            /* Verify Read is not needed (VREAD1=0) for the 4th pulse as the
             * cumulative erase time is already sufficient
             */
            r = Sys_Flash_Copier_VerifyEmpty(flash, addr, sector_length,
                                             false);
        }

        if (r == FLASH_ERR_NONE)
        {
            break;
        }
    }
    return r;
}

FlashStatus_t Sys_Flash_EraseSectorNONEND(FLASH_Type *flash, uint32_t addr,
                                          uint32_t sector_length)
{
    /* Read delay control 3 register */
    uint32_t prev_delay_reg3 = Sys_Flash_ReadRegDelayCTRL3(flash);

    /* Write T_ERASE = T_ERASE*4 before performing erase */
    Sys_Flash_WriteRegDelayCTRL3(flash, prev_delay_reg3 * 4);

    /* Perform a quick erase using single FLASH_RETRY_4 pulse */
    Sys_Flash_EraseSectorOperation(flash, addr, FLASH_RETRY_4);

    /* Restore flash delay control 3 register */
    Sys_Flash_WriteRegDelayCTRL3(flash, prev_delay_reg3);

    return Sys_Flash_Copier_VerifyEmpty(flash, addr, sector_length, false);
}

FlashStatus_t Sys_Flash_WriteWord(FLASH_Type *flash, uint32_t addr,
                                  uint32_t word, bool enb_endurance)
{
    FlashStatus_t r = FLASH_ERR_UNKNOWN;
    if (enb_endurance)
    {
        Sys_Flash_WriteWordOperation(flash, addr, word, CMD_PRE_PROGRAM_NOSEQ);
    }
    Sys_Flash_WriteWordOperation(flash, addr, word, CMD_PROGRAM_NOSEQ);

    /* Verify if programming was successful */
    if (Sys_Flash_ReadWord(flash, addr) == word)
    {
        r = FLASH_ERR_NONE;
    }
    return r;
}

FlashStatus_t Sys_Flash_WriteDouble(FLASH_Type *flash, uint32_t addr,
                                    const uint32_t *word, bool enb_endurance)
{
    FlashStatus_t r = FLASH_ERR_UNKNOWN;
    if (enb_endurance)
    {
        Sys_Flash_WriteDoubleOperation(flash, addr, word, CMD_PRE_PROGRAM_NOSEQ);
    }
    Sys_Flash_WriteDoubleOperation(flash, addr, word, CMD_PROGRAM_NOSEQ);

    uint32_t tmp[2];
    Sys_Flash_ReadDouble(flash, addr, tmp);
    if ((tmp[0] == word[0]) &&
        (tmp[1] == (word[1] & 0x3FU)))    /* Mask bit 6 to 31 */
    {
        r = FLASH_ERR_NONE;
    }
    return r;
}

FlashStatus_t Sys_Flash_WriteWordSequential(FLASH_Type *flash, uint32_t addr,
                                            const uint32_t *words,
                                            uint32_t word_length,
                                            uint32_t row_word_length,
                                            bool enb_endurance)
{
    FlashStatus_t r = FLASH_ERR_NONE;
    uint32_t crc_c;
    if (enb_endurance)
    {
        r = Sys_Flash_WriteSequentialData(flash, addr, words,
                                          word_length,
                                          row_word_length,
                                          CMD_PRE_PROGRAM_SEQ,
                                          &crc_c);
    }
    if (r == FLASH_ERR_NONE)
    {
        r = Sys_Flash_WriteSequentialData(flash, addr, words,
                                          word_length,
                                          row_word_length,
                                          CMD_PROGRAM_SEQ,
                                          &crc_c);

        /* Assigning 0 here to avoid compiler warning */
        uint32_t crc_v = 0;

        /* Calculate CRC using flash copier */
        r = Sys_Flash_Copier_CRC_Calculate(flash, addr, word_length, &crc_v);
        if (r == FLASH_ERR_NONE)
        {
            if (crc_c != crc_v)
            {
                r = FLASH_ERR_CRC_CHECK;
            }
        }
    }
    return r;
}

FlashStatus_t Sys_Flash_Copier_CRC_Calculate(FLASH_Type *flash, uint32_t addr,
                                             unsigned int word_length,
                                             uint32_t *crc)
{
    FlashStatus_t r = FLASH_ERR_UNKNOWN;

    uint32_t backup_IF_CTRL = flash->IF_CTRL;
    Sys_Flash_ExecutePrecondFlashRead(flash);

    /* Back-up CRC registers */
    uint32_t backup_CRC_CFG = CRC->CFG;
    uint32_t backup_CRC_VALUE = CRC->VALUE;

    /* Initialize CRC */
    CRC->CFG = VERIFY_WRITE_CRC_CFG;
    CRC->VALUE =  VERIFY_WRITE_CRC_INITIAL_VALUE;

    /* Use the flash copier to calculate CRC */
    flash->COPY_SRC_ADDR_PTR = addr;
    flash->COPY_WORD_CNT = word_length;
    flash->COPY_CFG = (COPY_MODE | COPY_TO_CRC);
    flash->COPY_CTRL = COPY_START;
    Sys_Flash_Copier_WaitBusy(flash);
    if ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_ERROR_Pos)) != COPY_ERROR)
    {
        /* In case comparison is cancelled by FLASH_ISOLATION an
         * error may not be generated. Manually check the copy source
         * address to check if the comparison was completed.
         */
        if ((flash->COPY_SRC_ADDR_PTR) ==
            ((addr + (word_length << 2)) & FLASH_COPY_SRC_ADDR_REG_MAX_VALUE))
        {
            r = FLASH_ERR_NONE;
            *crc = CRC->FINAL;
        }
    }

    /* Restore CRC peripheral registers */
    CRC->CFG = backup_CRC_CFG;
    CRC->VALUE = backup_CRC_VALUE;

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);

    return r;
}

FlashStatus_t Sys_Flash_Copier_VerifyEmpty(FLASH_Type *flash, uint32_t addr,
                                           unsigned int word_length, bool verified_read)
{
    FlashStatus_t r = FLASH_ERR_UNKNOWN;
    uint32_t backup_IF_CTRL = flash->IF_CTRL;

    if (verified_read)
    {
        Sys_Flash_ExecutePrecondFlashVerifiedRead(flash);
    }
    else
    {
        Sys_Flash_ExecutePrecondFlashRead(flash);
    }

    /* Use the flash copier to check the sector is erased */
    flash->COPY_SRC_ADDR_PTR = addr;
    flash->COPY_WORD_CNT = word_length;
    flash->COPY_CFG = (COMPARATOR_MODE | COMP_MODE_CONSTANT | COMP_ADDR_UP
                       | COMP_ADDR_STEP_1);
    flash->DATA[0] = 0xFFFFFFFF;
    /* ECC is disabled in comparator mode, hence the higher word bits must be
     * set to 0xFFFFFFFF
     */
    flash->DATA[1] = 0xFFFFFFFF;

    flash->COPY_CTRL = COPY_START;

    Sys_Flash_Copier_WaitBusy(flash);

    if ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_ERROR_Pos)) != COPY_ERROR)
    {
        /* In case comparison is cancelled by FLASH_ISOLATION an
         * error may not be generated. Manually check the copy source
         * address to check if the comparison was completed.
         */
        if ((flash->COPY_SRC_ADDR_PTR) ==
            ((addr + (word_length << 2)) & FLASH_COPY_SRC_ADDR_REG_MAX_VALUE))
        {
            r = FLASH_ERR_NONE;
        }
    }

    /* Restore IF_CTRL */
    Sys_Flash_ApplyIFCTRL(flash, backup_IF_CTRL);

    return r;
}

FlashStatus_t Sys_Flash_Copier_Copy(FLASH_Type *flash, uint32_t src_addr,
                                    uint32_t dest_addr, uint32_t word_length)
{
    FlashStatus_t r = FLASH_ERR_UNKNOWN;
    flash->COPY_SRC_ADDR_PTR = (uint32_t)(src_addr);
    flash->COPY_DST_ADDR_PTR = (uint32_t)(dest_addr);
    flash->COPY_WORD_CNT = word_length;
    flash->COPY_CFG = (COPY_MODE);
    flash->COPY_CTRL = COPY_START;
    Sys_Flash_Copier_WaitBusy(flash);
    if ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_ERROR_Pos)) != COPY_ERROR)
    {
        /* In case comparison is cancelled by FLASH_ISOLATION an
         * error may not be generated. Manually check the copy source
         * address to check if the comparison was completed.
         */
        if ((flash->COPY_SRC_ADDR_PTR) ==
            ((src_addr + (word_length << 2)) & FLASH_COPY_SRC_ADDR_REG_MAX_VALUE))
        {
            r = FLASH_ERR_NONE;
        }
    }

    return r;
}

uint32_t Sys_Flash_ReadMainAccessConfig(FLASH_Type *flash)
{
    return flash->IF_STATUS;
}

void Sys_Flash_WriteMainAccessConfig(FLASH_Type *flash,
                                     uint32_t regions)
{
    flash->MAIN_CTRL = regions;
    flash->MAIN_WRITE_UNLOCK = FLASH_MAIN_KEY;
}

void Sys_Flash_WriteCodeAccessConfig(FLASH_Type *flash,
                                     uint32_t regions)
{
    uint32_t mask = Sys_Flash_ReadMainAccessConfig(flash);
    mask = ((mask & ~CODE_WRITE_ENABLE_MASK) |
            ((regions << FLASH_MAIN_CTRL_CODE_A_0K_TO_22K_W_EN_Pos)
             & CODE_WRITE_ENABLE_MASK));
    Sys_Flash_WriteMainAccessConfig(flash, mask);
}

void Sys_Flash_WriteDataAccessConfig(FLASH_Type *flash,
                                     uint32_t regions)
{
    uint32_t mask = Sys_Flash_ReadMainAccessConfig(flash);
    mask = ((mask & ~DATA_WRITE_ENABLE_MASK) |
            ((regions << FLASH_MAIN_CTRL_DATA_A_0K_TO_5K_W_EN_Pos)
             & DATA_WRITE_ENABLE_MASK));
    Sys_Flash_WriteMainAccessConfig(flash, mask);
}

void Sys_Flash_Write_Flash_NVRAccessConfig(FLASH_Type *flash,
                                           uint32_t regions)
{
    if (flash == FLASH0)
    {
        /* NVR0-NVR3 on FLASH0 is not available for user programming */
        flash->NVR_CTRL = regions << FLASH_NVR_CTRL_NVR4_W_EN_Pos;
    }
    else
    {
        flash->NVR_CTRL = regions << FLASH_NVR_CTRL_NVR0_W_EN_Pos;
    }
    flash->NVR_WRITE_UNLOCK = FLASH_NVR_KEY;
}

uint32_t Sys_Flash_ReadCodeAccessConfig(FLASH_Type *flash)
{
    return ((flash->IF_STATUS & CODE_WRITE_UNLOCK_MASK) >>
            FLASH_MAIN_CTRL_CODE_A_0K_TO_22K_W_EN_Pos);
}

uint32_t Sys_Flash_ReadDataAccessConfig(FLASH_Type *flash)
{
    return ((flash->IF_STATUS & DATA_WRITE_UNLOCK_MASK) >>
            FLASH_IF_STATUS_DATA_A_0K_TO_5K_W_UNLOCK_Pos);
}

uint32_t Sys_Flash_Read_Flash_NVRAccessConfig(FLASH_Type *flash)
{
    /* NVR0-NVR3 on FLASH0 is not available for user programming */
    if (flash == FLASH0)
    {
        return ((flash->IF_STATUS & NVR_UNLOCK_MASK) >>
                FLASH_IF_STATUS_NVR4_W_UNLOCK_Pos);
    }
    return ((flash->IF_STATUS & NVR_UNLOCK_MASK) >>
            FLASH_IF_STATUS_NVR0_W_UNLOCK_Pos);
}

FlashStatus_t Sys_Flash_CalculateEnableRegions(struct region_descriptor *region,
                                               uint32_t addr, uint32_t word_length,
                                               uint32_t *mask)
{
    FlashStatus_t r = FLASH_ERR_NONE;

    /* Remove offset to calculate start address within the region */
    uint32_t start_addr = addr - region->base;
    uint32_t end_addr = start_addr + (word_length << 2) - 1;

    if ((end_addr >  (region->attr->total_lock_regions *
                      region->attr->bytes_per_lock_region)) ||
        word_length == 0)
    {
        /* Invalid length*/
        r = FLASH_ERR_BAD_LENGTH;
    }
    if (word_length == 0)
    {
        /* Zero length passed*/
        r = FLASH_ERR_ZERO_LEN;
    }

    unsigned int previous_boundary = 0;
    if (r == FLASH_ERR_NONE)
    {
        *mask = 0;
        for (unsigned int i = 1; i <= region->attr->total_lock_regions; i++)
        {
            unsigned int current_boundary = region->attr->bytes_per_lock_region * i;
            /* Check and start creating mask only if the start address of the
             * programmable region is below the unlock boundary
             */
            if (start_addr < current_boundary)
            {
                /* Check if the region completely fits in this boundary.
                 * If yes simply mark this region and exit
                 */
                if (start_addr >= previous_boundary &&
                    end_addr < current_boundary)
                {
                    *mask |= (1 << (i - 1));
                    break;
                }
                else
                {
                    /* if execution reached here the region overlaps
                     * multiple boundary. Simply check if the
                     * end region is below current boundary if not enable
                     * mask and proceed to next boundary.
                     */
                    *mask |= (1 << (i - 1));
                    if (end_addr < current_boundary)
                    {
                        break;
                    }
                }
                /* Update the previous boundary to point to the next boundary.
                 * Add four here because the writable location starts one word
                 * after the current boundary.
                 */
                previous_boundary = current_boundary + 4;
            }
        }
    }
    return r;
}

FlashStatus_t Sys_Flash_GetAddrInfo(uint32_t addr, struct info *info)
{
    FlashStatus_t r;
    if (info == NULL)
    {
        r = FLASH_ERR_INVALID_PARAMS;
    }
    else if (ifcs == NULL)
    {
        r = FLASH_ERR_INACCESSIBLE;
    }
    else
    {
        r = Flash_FlashAddrParamSanity(addr);
        if (r == FLASH_ERR_NONE)
        {
            /* Assign a default value before searching for the address */
            bool found = false;

            for (unsigned int i = 0; i < FLASH_INSTANCE_NUM; i++)
            {
                if (found == false)
                {
                    for (unsigned int rno = 0; rno < ifcs[i]->total_regions;  rno++)
                    {
                        if (addr >= ifcs[i]->regions[rno].base &&
                            addr <= ifcs[i]->regions[rno].top)
                        {
                            info->flash = &FLASH[i];
                            info->region = (struct region_descriptor *)
                                           &ifcs[i]->regions[rno];
                            found = true;
                            break;
                        }
                    }
                }
            }

            /* If address was not found return an error */
            if (found == false)
            {
                r = FLASH_ERR_BAD_ADDRESS;
            }
        }
    }
    return r;
}

FlashStatus_t Flash_WriteWord(uint32_t addr, uint32_t word,
                              bool enb_endurance)
{
    FlashStatus_t r;
    struct info info;

    /* Check if a flash region exists at this address */
    r = Sys_Flash_GetAddrInfo(addr, &info);
    if (r == FLASH_ERR_NONE)
    {
        /* Check if input parameters are valid for operation */
        r = Flash_Interface_Sanity(info.flash);
        if (r == FLASH_ERR_NONE)
        {
            /* Calculate new write access configuration */
            uint32_t mask;
            r = Sys_Flash_CalculateEnableRegions(info.region, addr, 1,
                                                 &mask);

            if (r == FLASH_ERR_NONE)
            {
                /* Read previous write access configuration */
                uint32_t p = info.region->attr->Read_lock_config(info.flash);

                /* Write the new write access configuration */
                info.region->attr->Write_lock_config(info.flash, mask);

                /* Write the word to flash */
                r = Sys_Flash_WriteWord(info.flash, addr, word, enb_endurance);

                /* Restore previous write access */
                info.region->attr->Write_lock_config(info.flash, p);
            }
        }
    }
    return r;
}

FlashStatus_t Flash_WriteBuffer(uint32_t addr, uint32_t word_length,
                                const uint32_t *words, bool enb_endurance)
{
    FlashStatus_t r;
    struct info info;

    /* Check if a flash region exists at this address */
    r = Sys_Flash_GetAddrInfo(addr, &info);
    if (r == FLASH_ERR_NONE)
    {
        r = Flash_Interface_Sanity(info.flash);
        if (r == FLASH_ERR_NONE)
        {
            /* Check if input parameters are valid for operation */
            r = Flash_WriteBufferParamSanity(words, word_length);
            if (r == FLASH_ERR_NONE)
            {
                /* Calculate new write access configuration */
                uint32_t mask;
                r = Sys_Flash_CalculateEnableRegions(info.region, addr, word_length,
                                                     &mask);

                if (r == FLASH_ERR_NONE)
                {
                    /* Read previous write access configuration */
                    uint32_t prev = info.region->attr->Read_lock_config(info.flash);

                    /* Write the new write access configuration */
                    info.region->attr->Write_lock_config(info.flash, mask);

                    r = Sys_Flash_WriteWordSequential
                            (info.flash, addr, words, word_length,
                            info.region->attr->row_word_len, enb_endurance);

                    /* Restore previous write access */
                    info.region->attr->Write_lock_config(info.flash, prev);
                }
            }
        }
    }
    return r;
}

FlashStatus_t Flash_EraseFlashBank(uint32_t num)
{
    FlashStatus_t r;
    if (num >= FLASH_INSTANCE_NUM)
    {
        /* Invalid bank */
        r = FLASH_ERR_INVALID_PARAMS;
    }
    else if (ifcs == NULL)
    {
        r = FLASH_ERR_INACCESSIBLE;
    }
    else
    {
        /* Check if input parameters are valid for operation */
        r = Flash_Interface_Sanity(ifcs[num]->flash);
        if (r == FLASH_ERR_NONE)
        {
            /* For Mass Erase Enable all Code and Data Regions,
             * disable all MNVR and NVR write access */
            uint32_t cl = ifcs[num]->regions[CODE_IDX].attr->Read_lock_config(ifcs[num]->flash);
            uint32_t dl = ifcs[num]->regions[DATA_IDX].attr->Read_lock_config(ifcs[num]->flash);
            uint32_t nl = ifcs[num]->regions[NVR_IDX].attr->Read_lock_config(ifcs[num]->flash);

            /* Disable all NVR write access */
            ifcs[num]->regions[NVR_IDX].attr->Write_lock_config(ifcs[num]->flash, 0);

            /* Enable all Code and Data write access */
            ifcs[num]->regions[CODE_IDX].attr->Write_lock_config(ifcs[num]->flash,
                                                                 CODE_WRITE_ENABLE_MASK >>
                                                                 FLASH_MAIN_CTRL_CODE_A_0K_TO_22K_W_EN_Pos);

            ifcs[num]->regions[DATA_IDX].attr->Write_lock_config(ifcs[num]->flash,
                                                                 DATA_WRITE_ENABLE_MASK >>
                                                                 FLASH_MAIN_CTRL_DATA_A_0K_TO_5K_W_EN_Pos);

            Sys_Flash_EraseMass(ifcs[num]->flash);

            /* Restore previous write access */
            ifcs[num]->regions[CODE_IDX].attr->Write_lock_config(ifcs[num]->flash, cl);
            ifcs[num]->regions[DATA_IDX].attr->Write_lock_config(ifcs[num]->flash, dl);
            ifcs[num]->regions[NVR_IDX].attr->Write_lock_config(ifcs[num]->flash, nl);
        }
    }
    return r;
}

FlashStatus_t Flash_EraseSector(uint32_t addr, bool enb_endurance)
{
    FlashStatus_t r;
    struct info info;

    /* Check if a flash region exists at this address */
    r = Sys_Flash_GetAddrInfo(addr, &info);
    if (r == FLASH_ERR_NONE)
    {
        r = Flash_Interface_Sanity(info.flash);
        if (r == FLASH_ERR_NONE)
        {
            uint32_t mask;

            /* Calculate new write access configuration */
            r = Sys_Flash_CalculateEnableRegions
                    (info.region, addr,
                    info.region->attr->sector_len, &mask);

            if (r == FLASH_ERR_NONE)
            {
                /* Read previous write access configuration */
                uint32_t p = info.region->attr->Read_lock_config(info.flash);

                /* Write the new write access configuration */
                info.region->attr->Write_lock_config(info.flash, mask);

                if (enb_endurance)
                {
                    r = Sys_Flash_EraseSectorEND(info.flash, addr,
                                                 info.region->attr->sector_len);
                }
                else
                {
                    r = Sys_Flash_EraseSectorNONEND(info.flash, addr,
                                                    info.region->attr->sector_len);
                }

                /* Restore previous write access */
                info.region->attr->Write_lock_config(info.flash, p);
            }
        }
    }
    return r;
}

FlashStatus_t Flash_ReadWord(uint32_t addr, uint32_t *word)
{
    FlashStatus_t r;
    struct info info;

    /* Check if a flash region exists at this address */
    r = Sys_Flash_GetAddrInfo(addr, &info);
    if (r == FLASH_ERR_NONE)
    {
        r = Flash_Interface_Sanity(info.flash);
        if (r == FLASH_ERR_NONE)
        {
            r = Flash_PointerParamSanity(word);
            if (r == FLASH_ERR_NONE)
            {
                *word = Sys_Flash_ReadWord(info.flash, addr);
            }
        }
    }
    return r;
}

FlashStatus_t Flash_ReadDouble(uint32_t addr, uint32_t *word)
{
    FlashStatus_t r;
    struct info info;

    /* Check if a flash region exists at this address */
    r = Sys_Flash_GetAddrInfo(addr, &info);
    if (r == FLASH_ERR_NONE)
    {
        r = Flash_Interface_Sanity(info.flash);
        if (r == FLASH_ERR_NONE)
        {
            r = Flash_PointerParamSanity(word);
            if (r == FLASH_ERR_NONE)
            {
                /* Read previous auto ECC setting */
                uint32_t e = info.flash->ECC_CTRL;

                /* Disable auto ECC on FLASH-CMD, COPIER and CBUS */
                info.flash->ECC_CTRL = FLASH_CBUS_ECC_DISABLE |
                                       FLASH_CMD_ECC_DISABLE |
                                       FLASH_COPIER_ECC_DISABLE |
                                       FLASH_ECC_COR_INT_THRESHOLD_DISABLED;

                Sys_Flash_ReadDouble(info.flash, addr, word);

                /* Restore auto ECC setting */
                info.flash->ECC_CTRL = e;
            }
        }
    }
    return r;
}

FlashStatus_t Flash_WriteDouble(uint32_t addr, const uint32_t *word,
                                bool enb_endurance)
{
    FlashStatus_t r;
    struct info info;

    /* Check if a flash region exists at this address */
    r = Sys_Flash_GetAddrInfo(addr, &info);
    if (r == FLASH_ERR_NONE)
    {
        /* Check if flash is ready for operation */
        r = Flash_Interface_Sanity(info.flash);
        if (r == FLASH_ERR_NONE)
        {
            r = Flash_PointerParamSanity(word);
            if (r == FLASH_ERR_NONE)
            {
                /* Calculate new write access configuration
                 * Note that we are still writing to a single word location
                 * */
                uint32_t mask;
                r = Sys_Flash_CalculateEnableRegions(info.region, addr, 1,
                                                     &mask);

                if (r == FLASH_ERR_NONE)
                {
                    /* Read previous write access configuration */
                    uint32_t p = info.region->attr->Read_lock_config(info.flash);

                    /* Write the new write access configuration */
                    info.region->attr->Write_lock_config(info.flash, mask);

                    /* Read previous auto ECC setting */
                    uint32_t e = info.flash->ECC_CTRL;

                    /* Disable auto ECC on FLASH-CMD, COPIER and CBUS */
                    info.flash->ECC_CTRL = FLASH_CBUS_ECC_DISABLE |
                                           FLASH_CMD_ECC_DISABLE |
                                           FLASH_COPIER_ECC_DISABLE |
                                           FLASH_ECC_COR_INT_THRESHOLD_DISABLED;

                    /* Write the 38 bit word to flash */
                    r = Sys_Flash_WriteDouble(info.flash, addr, word,
                                              enb_endurance);

                    /* Restore auto ECC setting */
                    info.flash->ECC_CTRL = e;

                    /* Restore previous write access */
                    info.region->attr->Write_lock_config(info.flash, p);
                }
            }
        }
    }
    return r;
}

FlashStatus_t Flash_EraseChip(void)
{
    FlashStatus_t r;
    for (uint32_t i = 0; i < FLASH_INSTANCE_NUM; i++)
    {
        r = Flash_EraseFlashBank(i);
        if (r != FLASH_ERR_NONE)
        {
            break;
        }
    }
    return r;
}

FlashStatus_t Flash_BlankCheck(uint32_t addr, unsigned int word_length)
{
    uint32_t *fa = (uint32_t *)addr;
    FlashStatus_t r;
    unsigned int words_copied = 0;
    struct info info;

    if (word_length == 0)
    {
        r = FLASH_ERR_ZERO_LEN;
    }
    else
    {
        do
        {
            uint32_t n_addr_start = (uint32_t)&fa[words_copied];
            r = Sys_Flash_GetAddrInfo(n_addr_start, &info);
            if (r == FLASH_ERR_NONE)
            {
                r = Flash_Interface_Sanity(info.flash);
                if (r == FLASH_ERR_NONE)
                {
                    uint32_t n_addr_end = ((uint32_t)&fa[words_copied] +
                                           (word_length << 2)) - 1;
                    /* Make sure that we copy only until maximum address of
                     * this region
                     */
                    if (n_addr_end > info.region->top)
                    {
                        n_addr_end = info.region->top;
                    }

                    uint32_t copy_len_words = (((n_addr_end - n_addr_start) + 1)
                                               >> 2);
                    r = Sys_Flash_Copier_VerifyEmpty(info.flash, n_addr_start,
                                                     copy_len_words, false);
                    words_copied += copy_len_words;
                }
            }
            if (r != FLASH_ERR_NONE)
            {
                break;
            }
        }
        while (words_copied < word_length);
    }
    return r;
}

FlashStatus_t Flash_ReadBuffer(uint32_t flash_address, uint32_t dram_address,
                               unsigned int word_length)
{
    uint32_t *fa = (uint32_t *)flash_address;
    uint32_t *da = (uint32_t *)dram_address;
    FlashStatus_t r;
    unsigned int words_copied = 0;
    struct info info;
    if (word_length == 0)
    {
        r = FLASH_ERR_ZERO_LEN;
    }
    else
    {
        do
        {
            uint32_t n_addr_start = (uint32_t)&fa[words_copied];
            r = Sys_Flash_GetAddrInfo(n_addr_start, &info);
            if (r == FLASH_ERR_NONE)
            {
                r = Flash_Interface_Sanity(info.flash);
                if (r == FLASH_ERR_NONE)
                {
                    uint32_t n_addr_end = ((uint32_t)&fa[words_copied] +
                                           (word_length << 2)) - 1;

                    /* Make sure that we copy only until maximum address of
                     * this region
                     */
                    if (n_addr_end > info.region->top)
                    {
                        n_addr_end = info.region->top;
                    }

                    uint32_t copy_len_words = (((n_addr_end - n_addr_start)  + 1)
                                               >> 2);

                    r = Sys_Flash_Copier_Copy(info.flash, n_addr_start,
                                              (uint32_t)&da[words_copied],
                                              copy_len_words);

                    words_copied += copy_len_words;
                }
            }
            if (r != FLASH_ERR_NONE)
            {
                break;
            }
        }
        while (words_copied < word_length);
    }
    return r;
}
