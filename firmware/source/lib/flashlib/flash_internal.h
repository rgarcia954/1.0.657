/**
 * @internal
 * @file flash_internal.h
 * @brief Internal header file for the flash interface library
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

#ifndef FLASH_INTERNAL_H_
#define FLASH_INTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <flash.h>
#include <flash_hw.h>

/** Total number of lock/unlock regions in NVR region */
#define NVR_UNLOCK_FLASH_REGION_NUM               0x8U

/** Total number of lock/unlock regions in NVR region. NVR0-NVR3 on flash 0  is
 *  unavailable for user programming */
#define NVR_UNLOCK_FLASH0_REGION_NUM              0x4U

/** Flash ADDR is a 21-bit register, the last two bits ignored
 *  for word access. Hence The maximum address is possible 0x1FFFFF
 */
#define FLASH_ADDR_REG_MAX_VALUE                  0x1FFFFF

/** Flash COPY_SRC is a 21-bit register, with the last two bits ignored
 *  for word access. Hence The maximum address is possible 0x1FFFFF
 */
#define FLASH_COPY_SRC_ADDR_REG_MAX_VALUE         FLASH_ADDR_REG_MAX_VALUE

/** Offset of internal delay register no 3 in bytes from DELAY_CTRL register
 */
#define DELAY_REG3_OFFSET_BYTES_FROM_DELAY_CTRL   16U

/** Total number of frequency ranges available on frequency map array */
#define FREQ_MAP_NUM                             (FLASH_DELAY_FOR_SYSCLK_48MHZ + 1)

/** Mask of all NVR regions unlocked */
#define NVR_UNLOCK_MASK                          (FLASH_NVR0_W_UNLOCKED | \
                                                  FLASH_NVR1_W_UNLOCKED | \
                                                  FLASH_NVR2_W_UNLOCKED | \
                                                  FLASH_NVR3_W_UNLOCKED | \
                                                  FLASH_NVR4_W_UNLOCKED | \
                                                  FLASH_NVR5_W_UNLOCKED | \
                                                  FLASH_NVR6_W_UNLOCKED | \
                                                  FLASH_NVR7_W_UNLOCKED)

/** Mask of all Code regions unlocked */
#define CODE_WRITE_UNLOCK_MASK                   (CODE_A_0K_TO_22K_W_UNLOCKED | \
                                                  CODE_A_22K_TO_44K_W_UNLOCKED | \
                                                  CODE_A_44K_TO_66K_W_UNLOCKED | \
                                                  CODE_A_66K_TO_88K_W_UNLOCKED)

/** Mask of all Data regions unlocked */
#define DATA_WRITE_UNLOCK_MASK                   (DATA_A_0K_TO_5K_W_UNLOCKED | \
                                                  DATA_A_5K_TO_10K_W_UNLOCKED | \
                                                  DATA_A_10K_TO_15K_W_UNLOCKED | \
                                                  DATA_A_15K_TO_20K_W_UNLOCKED | \
                                                  DATA_A_20K_TO_25K_W_UNLOCKED | \
                                                  DATA_A_25K_TO_30K_W_UNLOCKED | \
                                                  DATA_A_30K_TO_35K_W_UNLOCKED | \
                                                  DATA_A_35K_TO_40K_W_UNLOCKED)

/** Mask of all Code and Data regions unlocked */
#define FLASH_MAIN_UNLOCK_MASK                   (CODE_WRITE_UNLOCK_MASK | \
                                                  DATA_WRITE_UNLOCK_MASK)

/** Mask of all NVR regions to enable write */
#define NVR_WRITE_ENABLE_MASK                    (NVR0_WRITE_ENABLE | \
                                                  NVR1_WRITE_ENABLE | \
                                                  NVR2_WRITE_ENABLE | \
                                                  NVR3_WRITE_ENABLE | \
                                                  NVR4_WRITE_ENABLE | \
                                                  NVR5_WRITE_ENABLE | \
                                                  NVR6_WRITE_ENABLE | \
                                                  NVR7_WRITE_ENABLE)

/** Mask of all Code regions to enable write */
#define CODE_WRITE_ENABLE_MASK                   (CODE_A_0K_TO_22K_W_ENABLE | \
                                                  CODE_A_22K_TO_44K_W_ENABLE | \
                                                  CODE_A_44K_TO_66K_W_ENABLE | \
                                                  CODE_A_66K_TO_88K_W_ENABLE)

/** Mask of all Data regions to enable write */
#define DATA_WRITE_ENABLE_MASK                   (DATA_A_0K_TO_5K_W_ENABLE | \
                                                  DATA_A_5K_TO_10K_W_ENABLE | \
                                                  DATA_A_10K_TO_15K_W_ENABLE | \
                                                  DATA_A_15K_TO_20K_W_ENABLE | \
                                                  DATA_A_20K_TO_25K_W_ENABLE | \
                                                  DATA_A_25K_TO_30K_W_ENABLE | \
                                                  DATA_A_30K_TO_35K_W_ENABLE | \
                                                  DATA_A_35K_TO_40K_W_ENABLE)

/** Mask of all Code and Data regions to enable write */
#define MAIN_WRITE_ENABLE_MASK                   (CODE_WRITE_ACCESS_MASK | \
                                                  DATA_WRITE_ACCESS_MASK)

/** Mask of CRC configuration used for verification of written word data,
 * in sequential write mode */
#define VERIFY_WRITE_CRC_CFG                      (CRC_FINAL_XOR_STANDARD | \
                                                   CRC_FINAL_REVERSE_STANDARD | \
                                                   CRC_BIT_ORDER_STANDARD | \
                                                   CRC_32 | \
                                                   CRC_BIG_ENDIAN)

/** Initial value of CRC generator used for verification of written word
 * data in sequential write mode */
#define VERIFY_WRITE_CRC_INITIAL_VALUE             CRC_32_INIT_VALUE

/** Index of regions in region_descriptor structure  */
enum
{
    CODE_IDX = 0,
    DATA_IDX,
    NVR_IDX,
};

/** @} */ /* End of the FLASHMACRO group */

/**
 * @brief Flash region attributes.
 *
 * Each flash bank consists of code, data and NVR regions.<br>
 * Code region has  512 x 38 Bits/Sector, and total size = 88K x 38 Bits.<br>
 * Data region is 64 x 38 Bits/Sector, and total size = 40K x 38 Bits.<br>
 * NVR region is 64 x 38 Bits/Sector, and total size = 512 x 38 Bits.<br>
 * @note NVR[0:3] on FLASH0 instance is reserved for Cryptocell and cannot be accessed<br>
 * Flash programming is done in rows. Each programmable sector on flash can
 * consist of one or multiple rows.
 */

/**
 * @brief Flash region attributes
 */
struct region_attributes
{
    const unsigned int row_word_len;          /**< Number of words in this row */
    const unsigned int sector_len;            /**< Size of sector, single unit that can be erased */
    const unsigned int bytes_per_lock_region;    /**< Total number of bytes in a single lock region */
    const unsigned int total_lock_regions;    /**< Total number of lock regions available in a single lock
                                               *  region */
    uint32_t (*Read_lock_config)(FLASH_Type *);           /**< Read current lock configuration */
    void (*Write_lock_config)(FLASH_Type *, uint32_t);    /**< Read new lock configuration */
};

/**
 * @brief Flash region descriptor
 */
struct region_descriptor
{
    const uint32_t base;                     /**< Base address of this region */
    const uint32_t top;                      /**< Top address of this region */
    const struct region_attributes *attr;    /**< Region attributes */
};

/**
 * @brief Flash interface object
 */
struct interface
{
    FLASH_Type *flash;                        /**< Flash instance for the interface */
    const unsigned int total_regions;             /**< Total regions available in this interface */
    const struct region_descriptor *regions;    /**< Pointer to array of all regions
                                                 * on this interface */
};

/**
 * @brief Flash info object
 */
struct info
{
    FLASH_Type *flash;                     /**< Flash instance for the interface */
    struct region_descriptor *region;      /**< Active region selected within the
                                            *   interface for a given address */
};

/**
 * @brief Flash power and access object
 */
struct power_access
{
    FLASH_Type *flash;                     /**< Flash instance for the interface */
    uint32_t p_enb;                        /**< SYSCTRL_MEM_POWER_CFG/STARTUP/ENABLE flash power enable bit location */
    uint32_t acc_enb;                      /**< SYSCTRL_MEM_ACCESS_CFG flash access enable bit location */
};

/** Frequency map look up table */
extern const uint32_t freq_map[FREQ_MAP_NUM];

/**
 * @defgroup FLASHPRIV Flash library private functions
 * @brief Flash library private functions which are not exposed by the
 *        programming interface
 * @{
 */

/**
 * @defgroup FLASHSANITY Sanity functions
 * @brief Group of functions to check sanity of input parameter or hardware
 *  interface before executing commands on the flash interface
 * @{
 */

/**
 * @brief Check sanity of flash hardware interface
 * Check that the flash hardware interface is powered up and connected
 * to the system
 * @param [in] flash pointer to the flash interface structure
 * @return Flash API status code @see FlashStatus_t
 */
__STATIC_FORCEINLINE FlashStatus_t Flash_Interface_Sanity(FLASH_Type *flash)
{
    FlashStatus_t r = FLASH_ERR_NONE;
    if ((flash->IF_STATUS & (0x1U << FLASH_IF_STATUS_ISOLATE_STATUS_Pos)) ==
        FLASH_ISOLATE)
    {
        r = FLASH_ERR_INACCESSIBLE;
    }
    return r;
}

/**
 * @brief Check sanity of generic C pointer
 * Check that the pointer is not null
 * @param [in] p C pointer to verify
 * @return Flash API status code @see FlashStatus_t
 */
__STATIC_FORCEINLINE FlashStatus_t Flash_PointerParamSanity(const void *p)
{
    return (p == NULL) ? FLASH_ERR_NULL_PARAM : FLASH_ERR_NONE;
}

/**
 * @brief Check sanity of input parameters passed to Flash_WriteBuffer
 * @param [in] words pointer to words array passed to Flash_WriteBuffer
 * @param [in] word_length length of words to be written using Flash_WriteBuffer
 * @return Flash API status code @see FlashStatus_t
 *
 */
__STATIC_FORCEINLINE FlashStatus_t Flash_WriteBufferParamSanity(const uint32_t *words,
                                                                unsigned int word_length)
{
    FlashStatus_t r;
    if (word_length == 0)
    {
        r = FLASH_ERR_ZERO_LEN;
    }
    else
    {
        r = Flash_PointerParamSanity(words);
    }
    return r;
}

/**
 * @brief Check sanity of input parameters passed to Flash_CopyCheck
 * @param [in] flash_address flash address passed to  Flash_CopyCheck
 * @param [in] dram_address DRAM address passed to Flash_CopyCheck
 * @param [in] word_length Length of words passed to Flash_CopyCheck
 * @return Flash API status code @see FlashStatus_t
 */
__STATIC_FORCEINLINE FlashStatus_t Flash_CopyCheckParamSanity(uint32_t flash_address,
                                                              uint32_t dram_address,
                                                              unsigned int word_length)
{
    FlashStatus_t r;
    if (((flash_address & 0x3) != 0) ||
        ((dram_address & 0x3) != 0))
    {
        r = FLASH_ERR_ADDRESS_WORD_ALIGN;
    }
    else
    {
        r =  (word_length == 0) ? FLASH_ERR_ZERO_LEN : FLASH_ERR_NONE;
    }
    return r;
}

/**
 * @brief Check sanity of input parameters passed to Flash_Initialize
 * @param [in] no flash instance no passed to  Flash_Initialize
 * @param [in] core_clk CPU clock passed to Flash_Initialize
 * @return Flash API status code @see FlashStatus_t
 */
__STATIC_FORCEINLINE FlashStatus_t Flash_InitializeParamSanity(unsigned int no,
                                                               unsigned int core_clk)
{
    FlashStatus_t r;
    if (no > (FLASH_INSTANCE_NUM - 1))
    {
        r = FLASH_ERR_INVALID_PARAMS;
    }
    else
    {
        r = FLASH_ERR_INVALID_PARAMS;
        for (unsigned int i = 0; i < FREQ_MAP_NUM; i++)
        {
            if (core_clk == freq_map[i])
            {
                r = FLASH_ERR_NONE;
                break;
            }
        }
    }
    return r;
}

/**
 * @brief Check sanity of flash address
 * @param [in] addr flash address belonging to one of the flash instance
 * @return Flash API status code @see FlashStatus_t
 */
__STATIC_FORCEINLINE FlashStatus_t Flash_FlashAddrParamSanity(uint32_t addr)
{
    return ((addr & 0x03) != 0) ? FLASH_ERR_ADDRESS_WORD_ALIGN : FLASH_ERR_NONE;
}

/** @} */ /* End of the FLASHSANITY group */

/**
 * @defgroup FLASHPOLL Poll status
 * @brief Group of commands to poll and or wait for status of flash status
 * @{
 */

/**
 * @brief Read if interface is requesting new data in sequential programming
 *        mode
 * @param [in] flash flash instance being used
 * @return boolean value of of interface request for new data
 * @note  When sequential programming is active the busy flag is set until
 *        the sequential write operation is terminated with CMD_END
 */
__STATIC_FORCEINLINE bool Sys_Flash_IF_ReadSeqReq(const FLASH_Type *flash)
{
    return ((flash->IF_STATUS & (0x1U << FLASH_IF_STATUS_PROG_SEQ_DATA_REQ_Pos))
            == FLASH_PROG_SEQ_REQ_NEW_DATA);
}

/**
 * @brief Wait until the interface is requesting new data when the
 *        interface is in sequential programming mode
 * @param [in] flash flash instance being used
 * @note  Before executing any command on the flash interface application
 *        should check if the flash interface is not busy
 */
__STATIC_FORCEINLINE void Sys_Flash_IF_WaitSeqReq(const FLASH_Type *flash)
{
    while (Sys_Flash_IF_ReadSeqReq(flash) == 0);
}

/**
 * @brief Read if busy flag is set on the flash interface
 * @param [in] flash flash instance being used
 * @return boolean value of flash interface busy status
 * @note  Before executing any command on the flash interface application
 *        should check if the flash interface is not busy
 */
__STATIC_FORCEINLINE bool Sys_Flash_IF_ReadBusy(const FLASH_Type *flash)
{
    return ((flash->IF_STATUS & (0x1U << FLASH_IF_STATUS_BUSY_Pos)) ==
            FLASH_IF_BUSY);
}

/**
 * @brief Wait while the busy flag is set on the flash interface
 * @param [in] flash flash instance being used
 * @note  Before executing any command on the flash interface application
 *        should check if the flash interface is not busy
 */
__STATIC_FORCEINLINE void Sys_Flash_IF_WaitBusy(const FLASH_Type *flash)
{
    while (Sys_Flash_IF_ReadBusy(flash))
    {
        /* Do nothing */
    }
}

/**
 * @brief Read if flash copier is busy
 * @param [in] flash flash instance being used
 * @return boolean value of flash copier busy status
 */
__STATIC_FORCEINLINE bool Sys_Flash_Copier_ReadBusy(const FLASH_Type *flash)
{
    return ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_BUSY_Pos))
            == COPY_BUSY);
}

/**
 * @brief Wait while the flash copier is busy
 * @param [in] flash flash instance being used
 */
__STATIC_FORCEINLINE void Sys_Flash_Copier_WaitBusy(const FLASH_Type *flash)
{
    while (Sys_Flash_Copier_ReadBusy(flash));
}

/** @} */ /* End of the FLASHPOLL group */

/**
 * @defgroup FLASHRUN Run Operation
 * @brief Execute a command using flash interface
 * @{
 */

/**
 * @brief Send CMD_END to the flash interface
 * Terminates an active Flash command if possible
 * (e.g. sequential programming sequence)
 * @param [in] flash flash instance being used
 */
__STATIC_FORCEINLINE void Sys_Flash_TerminateCommand(FLASH_Type *flash)
{
    flash->CMD_CTRL = ((uint32_t)(0x1U << FLASH_CMD_CTRL_CMD_END_Pos));
}

/**
 * @brief Execute a command on the flash interface
 * This function checks if the interface is free before initiating a flash command
 * by executing Sys_Flash_ExecutePrecondFlashCommand.<br>
 * After a command is sent to the interface the function polls and waits for the
 * command to be completed using Sys_Flash_IF_WaitBusy.
 *
 * @param [in] flash flash instance being used
 * @param [in] cmd command to execute
 * @note Application should be careful in changing bits on IF_CTRL register
 *       to avoid unnecessary change of flash state.<br>
 *       Changing retry bits does not initiate a command on the flash interface
 *       hence this command does not poll the busy bit.<br>
 *       To end and ongoing command use Sys_Flash_TerminateCommand.
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecuteCommand(FLASH_Type *flash, uint32_t cmd)
{
    flash->CMD_CTRL = cmd & FLASH_CMD_CTRL_COMMAND_Mask;

    Sys_Flash_IF_WaitBusy(flash);
}

/**
 * @brief End ongoing flash command operation if interface is busy or
 *        sequential write operation is active<br> and wait until flash
 *        interface is free again
 * @param [in] flash flash instance being used
 * @note CMD_END command is sent to the interface if found busy to end
 *       any ongoing operation
 */
__STATIC_FORCEINLINE void Sys_Flash_EndOperationWaitIdle(FLASH_Type *flash)
{
    /* Check if a sequential programming is ongoing. A check is
     * applied to both types of sequential write operation pre-program
     * and program
     * */
    if (((flash->CMD_CTRL &  FLASH_CMD_CTRL_COMMAND_Mask) == CMD_PRE_PROGRAM_SEQ) ||
        ((flash->CMD_CTRL & FLASH_CMD_CTRL_COMMAND_Mask) == CMD_PROGRAM_SEQ))
    {
        Sys_Flash_IF_WaitSeqReq(flash);
    }

    /* Terminate any ongoing operation unconditionally */
    Sys_Flash_TerminateCommand(flash);

    /* Wait for CMD interface to be free */
    Sys_Flash_IF_WaitBusy(flash);
}

/**
 * @brief Execute a sequential command on the flash interface
 * This function does not check if the interface is free nor does it poll for
 * busy bit in the flash interface
 *
 * @param [in] flash flash instance being used
 * @param [in] cmd command to execute
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecuteSeqCommand(FLASH_Type *flash, uint32_t cmd)
{
    flash->CMD_CTRL = cmd & FLASH_CMD_CTRL_COMMAND_Mask;
    /** Warning: Do not wait for busy when issuing a command
     * for sequential operation as busy will not be cleared
     * until the sequential operation is completed by END
     * command
     */
}

/** @} */ /* End of the FLASHRUN group */

/**
 * @brief Apply retry level to flash interface register
 * @param [in] flash flash instance being used
 * @param [in] val value to apply
 * @note Applying retry setting does not initiate a flash command and application
 *       is not required to poll the busy flag
 */
__STATIC_FORCEINLINE void Sys_Flash_ApplyRetryLevel(FLASH_Type *flash, uint32_t val)
{
    /* Apply  val retry settings, as this does not execute a command
     *  a wait for busy is not required here */
    flash->IF_CTRL =  ((flash->IF_CTRL & ~FLASH_IF_CTRL_RETRY_Mask) |
                       val);
}

/**
 * @brief Get delay register 3 address pointer
 * @param [in] flash flash instance being used
 * @return pointer to delay register 3
 */
__STATIC_FORCEINLINE uint32_t * Sys_Flash_DelayCTRL3Address(const FLASH_Type *flash)
{
    return ((uint32_t *)((uint32_t)&flash->DELAY_CTRL +
                         DELAY_REG3_OFFSET_BYTES_FROM_DELAY_CTRL));
}

/**
 * @brief Read delay control register no 3
 * @param [in] flash flash instance being used
 * @return value of delay control register no 3
 * @note Delay control register provides erase time for sector,
 *       max 4 erase pulse required.<br>When FLASH_DELAY_CTRL register is
 *       written based on the core clock a default erase time of 0.9ms
 *       is applied.<br>This delay must be multiplied by 4, when using only a
 *       single retry pulse of FLASH_RETRY_4 for sector erase.
 */
__STATIC_FORCEINLINE uint32_t Sys_Flash_ReadRegDelayCTRL3(const FLASH_Type *flash)
{
    return *(Sys_Flash_DelayCTRL3Address(flash));
}

/**
 * @brief Write delay control register no 3
 * @param [in] flash flash instance being used
 * @param [in] val value to be written to delay control register 3
 */
__STATIC_FORCEINLINE void Sys_Flash_WriteRegDelayCTRL3(const FLASH_Type *flash,
                                                       uint32_t val)
{
    *(Sys_Flash_DelayCTRL3Address(flash)) = val;
}

/**
 * @defgroup FLASHCOND Command precondition
 * @brief Group of functions to apply correct state to the flash
 *        interface before performing a specific operation
 * @{
 */

/**
 * @brief Apply precondition before executing CMD_WRITE command
 * @param [in] flash flash instance being used
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecutePrecondFlashWrite(FLASH_Type *flash)
{
    Sys_Flash_EndOperationWaitIdle(flash);

    /** Preconditions to write: VREAD1 = X,  RECALL = X, LP_MODE = X **/
}

/**
 * @brief Apply precondition before executing CMD_READ command
 * @param [in] flash flash instance being used
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecutePrecondFlashRead(FLASH_Type *flash)
{
    Sys_Flash_EndOperationWaitIdle(flash);
    /** Preconditions to write for flash read operation:
     * VREAD1 = 0,  RECALL = 0, LP_MODE = X
     */
    Sys_Flash_ExecuteCommand(flash, CMD_UNSET_RECALL);

    Sys_Flash_ExecuteCommand(flash, CMD_UNSET_VREAD1);
}

/**
 * @brief Apply precondition before using flash copier to
 *        read flash
 * @param [in] flash flash instance being used
 * @note VREAD1 is not set to zero as done in Sys_Flash_ExecutePrecondFlashRead
 *       this is because VREAD1
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecutePrecondFlashVerifiedRead(FLASH_Type *flash)
{
    /** Preconditions to write for flash read operation:
     * VREAD1 = 1,  RECALL = 0, LP_MODE = X
     */
    Sys_Flash_EndOperationWaitIdle(flash);

    Sys_Flash_ExecuteCommand(flash, CMD_SET_VREAD1);

    Sys_Flash_ExecuteCommand(flash, CMD_UNSET_RECALL);
}

/**
 * @brief Apply precondition before executing CMD_MASS_ERASE command
 * @param [in] flash flash instance being used
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecutePrecondFlashMassErase(FLASH_Type *flash)
{
    /** Preconditions to write for flash read operation:
     * VREAD1 = X,  RECALL = X, LP_MODE = X
     */
    Sys_Flash_EndOperationWaitIdle(flash);
}

/**
 * @brief Apply precondition before executing CMD_SECTOR_ERASE command
 * @param [in] flash flash instance being used
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecutePrecondFlashSectorErase(FLASH_Type *flash)
{
    Sys_Flash_EndOperationWaitIdle(flash);
    /** Preconditions to write for flash read operation:
     * VREAD1 = X,  RECALL = X, LP_MODE = X
     */
}

/**
 * @brief Apply precondition before using the flash copier to read from flash
 * @param [in] flash flash instance being used
 */
__STATIC_FORCEINLINE void Sys_Flash_ExecutePrecondFlashVerifyEmpty(FLASH_Type *flash)
{
    Sys_Flash_EndOperationWaitIdle(flash);

    /** Pre-requisites for flash verification */
    Sys_Flash_ExecutePrecondFlashRead(flash);

    /** Stop any ongoing FLASH copier activity and wait for
     * flash copier to be free
     */
    if (Sys_Flash_Copier_ReadBusy(flash))
    {
        flash->COPY_CTRL = COPY_STOP;

        /** Wait until flash copier is idle */
        Sys_Flash_Copier_WaitBusy(flash);
    }
}

/**
 * @brief Apply register value to flash control register
 * @param [in] flash flash instance being used
 * @param [in] reg value to apply to flash IF_CTRL register
 * @note Changing VREAD1_MODE, RECALL or LP_MODE issues a command to the flash
 *       interface.<br>As these bits are mutually exclusive and only one command
 *       can be executed at time, this function applies the register values
 *       three time to ensure all bit changes are applied.
 */
__STATIC_FORCEINLINE void Sys_Flash_ApplyIFCTRL(FLASH_Type *flash, uint32_t reg)
{
    /* Since VREAD1 and RECALL and LP are mutually exclusive the register
     * operation is done three times.
     */
    flash->IF_CTRL = reg;
    Sys_Flash_IF_WaitBusy(flash);

    flash->IF_CTRL = reg;
    Sys_Flash_IF_WaitBusy(flash);

    flash->IF_CTRL = reg;
    Sys_Flash_IF_WaitBusy(flash);
}

/**
 * @brief Erase a flash sector
 * @param [in] flash flash instance being used
 * @param [in] addr sector address belonging to the flash instance
 * @param [in] retry_level retry level to apply for sector erase operation
 *
 */
void Sys_Flash_EraseSectorOperation(FLASH_Type *flash, uint32_t addr,
                                    uint32_t retry_level);

/**
 * @brief Write a word to flash
 * @param [in] flash flash instance being used
 * @param [in] addr word address belonging to the flash instance
 * @param [in] word word to be written
 * @param [in] cmd_prg_type type of flash operation; CMD_PRE_PROGRAM_NOSEQ
 *             or CMD_PROGRAM_NOSEQ
 *
 */
void Sys_Flash_WriteWordOperation(FLASH_Type *flash, uint32_t addr,
                                  uint32_t word,
                                  uint32_t cmd_prg_type);

/**
 * @brief Write a 38- bit word to flash
 * @param [in] flash flash instance being used
 * @param [in] addr word address belonging to the flash instance
 * @param [in] word pointer to the word to be written
 * @param [in] cmd_prg_type type of flash operation; CMD_PRE_PROGRAM_NOSEQ
 *             or CMD_PROGRAM_NOSEQ
 */
void Sys_Flash_WriteDoubleOperation(FLASH_Type *flash, uint32_t addr,
                                    const uint32_t *word,
                                    uint32_t cmd_prg_type);

/**
 * @brief Read a word to flash
 * @param [in] flash flash instance being used
 * @param [in] addr word address belonging to the flash instance
 * @return word read from flash
 */
uint32_t Sys_Flash_ReadWord(FLASH_Type *flash, uint32_t addr);

/**
 * @brief Read a 38-bit word to flash
 * @param [in] flash flash instance being used
 * @param [in] addr word address belonging to the flash instance
 * @param [out] data pointer to the word to be copied, that was read
 */
void Sys_Flash_ReadDouble(FLASH_Type *flash, uint32_t addr, uint32_t *data);

/**
 * @brief Execute a sequential write to flash
 * @param [in] flash flash instance being used
 * @param [in] addr word start address belonging to the flash instance
 * @param [in] words pointer to array of words to be written
 * @param [in] word_length total number of data words to be written to flash
 * @param [in] row_word_length total number of words that can be written in
 *                             a single sequence
 * @param [in] cmd_prg_type type of flash operation; CMD_PRE_PROGRAM_NOSEQ
 *             or CMD_PROGRAM_NOSEQ
 * @param [out] crc hash of the words written to flash, see VERIFY_WRITE_CRC_CFG
 *         and VERIFY_WRITE_CRC_INITIAL_VALUE
 * @return Flash API status code @see FlashStatus_t
 * @note CRC peripheral registers are modified during execution for
 *       CRC calculation.<br>
 *       Source address of data being read and destination address being written,
 *       should not be on the same instance of flash.
 */
FlashStatus_t Sys_Flash_WriteSequentialData(FLASH_Type *flash, uint32_t addr,
                                            const uint32_t *words,
                                            unsigned int word_length,
                                            unsigned int row_word_length,
                                            uint32_t cmd_prg_type,
                                            uint32_t *crc);

/**
 * @brief Write sequential data to flash and verify written data
 *        by checking CRC of data written versus read back
 * @param [in] flash flash instance being used
 * @param [in] addr word start address belonging to the flash instance
 * @param [in] words pointer to array of words to be written
 * @param [in] word_length total number of data words to be written to flash
 * @param [in] row_word_length total number of words that can be written in
 *                             a single sequence
 * @param [in] enb_endurance set to 0 for default flash endurance<br>
 *                           set to 1 to enable two stage programming for higher
 *                           endurance of programmed data
 * @return Flash API status code @see FlashStatus_t
 * @note CRC peripheral registers are modified during execution for
 *       CRC calculation.<br>
 *       Source address of data being read and destination address being written,
 *       should not be on the same instance of flash.
 */
FlashStatus_t Sys_Flash_WriteWordSequential(FLASH_Type *flash, uint32_t addr,
                                            const uint32_t *words,
                                            uint32_t word_length,
                                            uint32_t row_word_length,
                                            bool enb_endurance);

/**
 * @brief Erase all code and data regions of flash
 * @param [in] flash flash instance being used
 * @note A verification of mass erase is not performed in this function
 */
void Sys_Flash_EraseMass(FLASH_Type *flash);

/**
 * @brief Erase a sector of flash in endurance mode
 * Four identical voltages is applied for erase through retry mechanism.<br>
 * A verification is performed at each stage to check if erase is successful
 *
 * @param [in] flash flash instance being used
 * @param [in] addr sector address belonging to the flash instance
 * @param [in] sector_length total number of words in this sector
 * @return Flash API status code @see FlashStatus_t
 * @note VREAD1 can be set or cleared if this function is called directly.
 *       Sys_Flash_EraseSector instead which restores the registers
 *       correctly.<br>
 *       Flash copier registers are affected.
 */
FlashStatus_t Sys_Flash_EraseSectorEND(FLASH_Type *flash, uint32_t addr,
                                       uint32_t sector_length);

/**
 * @brief Erase a sector of flash without endurance mode
 * A verification is performed to check if erase was successful
 *
 * @param [in] flash flash instance being used
 * @param [in] addr sector address belonging to the flash instance
 * @param [in] sector_length total number of words in this sector
 * @return Flash API status code @see FlashStatus_t
 * @note flash copier registers are affected
 */
FlashStatus_t Sys_Flash_EraseSectorNONEND(FLASH_Type *flash, uint32_t addr,
                                          uint32_t sector_length);

/**
 * @brief Write a word to flash
 * @param [in] flash flash instance being used
 * @param [in] addr word address belonging to the flash instance
 * @param [in] word word to be written to flash
 * @param [in] enb_endurance set to 0 for default flash endurance<br>
 *                           set to 1 to enable two stage programming for higher
 *                           endurance of programmed data
 * @return Flash API status code @see FlashStatus_t
 */
FlashStatus_t Sys_Flash_WriteWord(FLASH_Type *flash, uint32_t addr,
                                  uint32_t word, bool enb_endurance);

/**
 * @brief Calculate CRC of words in flash using flash copier
 * @param [in] flash flash instance being used
 * @param [in] addr word address belonging to the flash instance
 * @param [in] word_length total number of words used in CRC calculation
 * @param [out] crc CRC value calculated using CRC peripheral,
 *              @see VERIFY_WRITE_CRC_CFG and VERIFY_WRITE_CRC_INITIAL_VALUE
 * @return Flash API status code @see FlashStatus_t
 * @note Flash copier and CRC register are modified
 */
FlashStatus_t Sys_Flash_Copier_CRC_Calculate(FLASH_Type *flash, uint32_t addr,
                                             unsigned int word_length,
                                             uint32_t *crc);

/**
 * @brief Verify if flash is empty
 * @param [in] flash flash instance being used
 * @param [in] addr word address belonging to the flash instance
 * @param [in] word_length total number of words to be verified
 * @param [in] verified_read enables verified read operation
 * @return Flash API status code @see FlashStatus_t
 * @note The Verify Read,(VREAD1=1) is used to check whether a sector
 *       has been successfully erased after each pulse. The access time is slower
 *       than normal read. This is applicable only to Sys_Flash_EraseSectorEND
 *       function
 */
FlashStatus_t Sys_Flash_Copier_VerifyEmpty(FLASH_Type *flash, uint32_t addr,
                                           unsigned int word_length,
                                           bool verified_read);

/**
 * @brief Copy contents of flash using flash copier
 * @param [in] flash flash instance being used
 * @param [in] src_addr word address belonging to the flash instance
 * @param [in] dest_addr word address belonging to static memory
 * @param [in] word_length total number of words to be copied
 * @return Flash API status code @see FlashStatus_t
 * @note The arbitration for the DMA bus between the Flash Copier,<br>
 *       CryptoCell AHB master, DMA channels and the MBIST has a static<br>
 *       arbitration scheme: DMA > CC > Flash Copier > MBIST.
 */
FlashStatus_t Sys_Flash_Copier_Copy(FLASH_Type *flash, uint32_t src_addr,
                                    uint32_t dest_addr, uint32_t word_length);

/**
 * @defgroup FLASHLOCK Lock-Unlock
 * @brief Group of Lock/Unlock functions that are populated in info structure
 * @{
 */

/**
 * @brief Read main access configuration
 * Main area comprises of code and data regions
 *
 * @param [in] flash flash instance to be used
 * @return mask of the regions locked-unlocked in the main region<br>
 */

uint32_t Sys_Flash_ReadMainAccessConfig(FLASH_Type *flash);

/**
 * @brief Write main access configuration
 * Main area comprises of code and data regions
 *
 * @param [in] flash flash instance to be used
 * @param [in] regions mask of the regions to be unlocked in the main region<br>
 */
void Sys_Flash_WriteMainAccessConfig(FLASH_Type *flash, uint32_t regions);

/**
 * @brief Write code access configuration
 * This function write lock-unlock configuration for the code region
 * region of a flash instance
 *
 * @param [in] flash flash instance to be used
 * @param [in] regions mask of the regions to be unlocked that was calculated
 *             earlier using Sys_Flash_CalculateEnableRegions
 * @note mask is shifted left to start from the correct location for code region
 */
void Sys_Flash_WriteCodeAccessConfig(FLASH_Type *flash, uint32_t regions);

/**
 * @brief Write data access configuration
 *
 * This function write lock-unlock configuration for the Data region
 * region of a flash instance
 *
 * @param [in] flash flash instance to be used
 * @param [in] regions mask of the regions to be unlocked that was calculated
 *             earlier using Sys_Flash_CalculateEnableRegions
 * @note mask is shifted left to start from the correct location for data region
 */
void Sys_Flash_WriteDataAccessConfig(FLASH_Type *flash, uint32_t regions);

/**
 * @brief Write NVR access configuration
 *
 * This function write lock-unlock configuration for the Flash NVR region
 * region of a flash instance
 *
 * @param [in] flash flash instance to be used
 * @param [in] regions mask of the regions to be unlocked that was calculated
 *             earlier using Sys_Flash_CalculateEnableRegions
 */
void Sys_Flash_Write_Flash_NVRAccessConfig(FLASH_Type *flash, uint32_t regions);

/**
 * @brief Read code region unlock access configuration
 *
 * This function reads the current regions that are unlocked for the code
 * region of a flash instance
 *
 * @param [in] flash flash instance to be used
 * @return 32-bit value of the code regions unlocked
 */
uint32_t Sys_Flash_ReadCodeAccessConfig(FLASH_Type *flash);

/**
 * @brief Read data region unlock access configuration
 *
 * This function reads the current regions that are unlocked for the Data
 * region of a flash instance
 *
 * @param [in] flash flash instance to be used
 * @return 32-bit value of the Data regions unlocked
 * @note Returned values are shifted right to start from 0 bit location
 */
uint32_t Sys_Flash_ReadDataAccessConfig(FLASH_Type *flash);

/**
 * @brief Read Flash NVR unlock access configuration
 * This function reads the current regions that are unlocked for the Flash NVR
 * region of a flash instance
 *
 * @param [in] flash flash instance to be used
 * @return 32-bit value of the NVR regions unlocked
 */
uint32_t Sys_Flash_Read_Flash_NVRAccessConfig(FLASH_Type *flash);

/** @} */ /* End of the FLASHLOCK group */

/**
 * @brief Write a 38-bit word to a flash instance
 *
 * This function writes a 38-bit word to a flash instance
 *
 * @param [in] flash flash instance to be used
 * @param [in] addr address to be written belonging to the flash instance provided
 * @param [in] word 32-bit C pointer to the word to be stored in flash,<br>
 *             word[0] [0:31] bits consist of first lower 32-bits<br>
 *             word[1] [0:5] bits consist of the remaining higher 6-bits<br>
 * @param [in] enb_endurance set to 0 for default flash endurance<br>
 *                           set to 1 to enable two stage programming for higher
 *                           endurance of data programmed
 * @return Flash API status code @see FlashStatus_t
 * @note System clock frequency should not be changed when flash is being
 *       programmed<br>
 */
FlashStatus_t Sys_Flash_WriteDouble(FLASH_Type *flash, uint32_t addr,
                                    const uint32_t *word, bool enb_endurance);

/**
 * @brief Calculate unlock regions
 *
 * This function calculates unlock regions required to perform a write to a given
 *        area in flash
 *
 *
 * @param [in] region region descriptor that was populated earlier in info data structure<br>by
 *             calling Flash_GetAddrInfo and correlates to flash address being used
 * @param [in] addr first word location of flash
 * @param [in] word_length total number of words to be written
 * @param [out] mask calculated unlock region mask
 * @note Application should check the return value before using mask variable
 */
FlashStatus_t Sys_Flash_CalculateEnableRegions(struct region_descriptor *region,
                                               uint32_t addr, uint32_t word_length,
                                               uint32_t *mask);

/**
 * @brief Get information for the flash address provided.
 *
 * This function populates info data structure with flash information that
 * is utilized by private functions to perform a read and write
 *
 *
 * @param [in] addr flash address of which information is requested
 * @param [out] info C pointer to info data type where information can be
 *              copied
 * @return Flash API status code @see FlashStatus_t
 */
FlashStatus_t Sys_Flash_GetAddrInfo(uint32_t addr, struct info *info);

#ifdef __cplusplus
}

#endif    /* ifdef __cplusplus */

#endif    /* FLASH_INTERNAL_H_ */

/** @endinternal  */

