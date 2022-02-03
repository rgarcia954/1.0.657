/**
 * @file flash_rom.h
 * @brief Header file for ROM based API functions for flash management
 *        through the flash interface
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
#ifndef FLASH_ROM_H_
#define FLASH_ROM_H_

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup FLASH
 *  @{
 */
/**
 * @defgroup FLASHROM ROM functions
 * @brief ROM based API functttons for flash management
 * @{
 */

#include <stdint.h>
#include <stdbool.h>
#include <hw.h>
#include <rom_vect.h>


/**
 * @brief Flash structure definitions
 */

/** Total number of words in a single row in Code region */
#define CODE_ROW_LEN_WORDS                        0x80U

/** Total number of words in a single sector in Code region */
#define CODE_SECTOR_LEN_WORDS                     0x200U

/** Total number of bytes in lock/unlock regions in Code region */
#define CODE_UNLOCK_REGION_LEN_BYTES              0x16000U

/** Total number of lock/unlock regions in Code region */
#define CODE_UNLOCK_REGION_NUM                    0x4U

/** Total number of words in a single row in Data region */
#define DATA_ROW_LEN_WORDS                        0x20U

/** Total number of words in a single sector in Data region */
#define DATA_SECTOR_LEN_WORDS                     0x40U

/** Total number of bytes in lock/unlock regions in Data region */
#define DATA_UNLOCK_REGION_LEN_BYTES              0x5000U

/** Total number of lock/unlock regions in Data region */
#define DATA_UNLOCK_REGION_NUM                    0x8U

/** Total number of words in a single row in NVR region */
#define NVR_ROW_LEN_WORDS                         DATA_ROW_LEN_WORDS

/** Total number of words in a single sector in NVR region */
#define NVR_SECTOR_LEN_WORDS                      DATA_SECTOR_LEN_WORDS

/** Total number of bytes in lock/unlock regions in NVR region */
#define NVR_UNLOCK_REGION_LEN_BYTES               0x100U

/**
 * @brief Flash library return codes
 */
typedef enum
{
    FLASH_ERR_NONE               = 0x0,    /**< Flash no error<br> */
    FLASH_ERR_BAD_ADDRESS        = 0x1,    /**< Flash error invalid address parameter */
    FLASH_ERR_BAD_LENGTH         = 0x2,    /**< Flash error invalid word length parameter */
    FLASH_ERR_INACCESSIBLE       = 0x3,    /**< Flash error flash is inaccessible */
    FLASH_ERR_INVALID_PARAMS     = 0x4,    /**< Flash error invalid function parameter */
    FLASH_ERR_NULL_PARAM         = 0x5,    /**< Flash error null pointer used */
    FLASH_ERR_ADDRESS_WORD_ALIGN = 0x6,    /**< Flash error address is not word align */
    FLASH_ERR_ZERO_LEN           = 0x7,    /**< Flash error zero length parameter passed */
    FLASH_ERR_CRC_CHECK          = 0x8,    /**< Flash error CRC verification failed */
    FLASH_ERR_UNKNOWN            = 0x9,     /**< Flash error undefined */
} FlashStatus_t;

/**
 * @brief Flash operational frequency values supported by the device
 */
typedef enum
{
    FLASH_CLOCK_3MHZ       = 3000000UL,    /**< Flash Clock value of 3MHz */
    FLASH_CLOCK_4MHZ       = 4000000UL,    /**< Flash Clock value of 4MHz */
    FLASH_CLOCK_5MHZ       = 5000000UL,    /**< Flash Clock value of 5MHz */
    FLASH_CLOCK_8MHZ       = 8000000UL,    /**< Flash Clock value of 8MHz */
    FLASH_CLOCK_10MHZ     = 10000000UL,    /**< Flash Clock value of 10MHz */
    FLASH_CLOCK_12MHZ     = 12000000UL,    /**< Flash Clock value of 12MHz */
    FLASH_CLOCK_16MHZ     = 16000000UL,    /**< Flash Clock value of 16MHz */
    FLASH_CLOCK_20MHZ     = 20000000UL,    /**< Flash Clock value of 20MHz */
    FLASH_CLOCK_24MHZ     = 24000000UL,    /**< Flash Clock value of 24MHz */
    FLASH_CLOCK_48MHZ     = 48000000UL,    /**< Flash Clock value of 48MHz */
} FlashClockFrequency_t;

/** @cond FLASH_POINTERS */
typedef FlashStatus_t (*Flash_Initialize_t)(unsigned, FlashClockFrequency_t);
typedef FlashStatus_t (*Flash_WriteWord_t)(uint32_t, uint32_t, bool);
typedef FlashStatus_t (*Flash_ReadWord_t)(uint32_t, uint32_t *);
typedef FlashStatus_t (*Flash_WriteDouble_t) (uint32_t, uint32_t *, bool);
typedef FlashStatus_t (*Flash_ReadDouble_t)(uint32_t, uint32_t *);
typedef FlashStatus_t (*Flash_WriteBuffer_t)(uint32_t, uint32_t, const uint32_t *, bool);
typedef FlashStatus_t (*Flash_ReadBuffer_t)(uint32_t, uint32_t, unsigned);
typedef FlashStatus_t (*Flash_EraseFlashBank_t)(uint32_t);
typedef FlashStatus_t (*Flash_EraseChip_t)(void);
typedef FlashStatus_t (*Flash_BlankCheck_t)(uint32_t, unsigned);
typedef FlashStatus_t (*Flash_EraseSector_t)(uint32_t, bool); 
/** @endcond */

/**
 * @brief Initialize clock and access to flash
 *
 * This function powers-up and enables access to flash region<br>
 * It also applies correct clock setting based on the core_clock provided<br>
 *
 * @param[in] no flash instance to be initialized<br>
 * @param[in] core_clk flash clock frequency in Hertz, only defined frequencies
 *             supported @see FlashClockFrequency_t <br>
 * @return Flash API status code @see FlashStatus_t
 * @note System clock frequency should not be changed when flash is being
 *       programmed<br>
 * @note System clock frequency of 1 MHz or higher is recommended for safe flash
 *       programming<br>
 * @note The internal RC clock source cannot be used to erase or to program the
 *       Flash as it is temperature sensitive
 */
static inline FlashStatus_t Flash_Initialize(unsigned no, FlashClockFrequency_t core_clk)
{
    FlashStatus_t ret;
    volatile uint32_t *FLASH_RESERVED = &FLASH[no].RESERVED0[0];

    ret = (*((Flash_Initialize_t *)ROMVECT_FLASH_INITIALIZE))(no, core_clk);

    /* For faster clock frequencies, adjust the internal flash delays (not included
     * in the ROM implementation) */
    if (core_clk == FLASH_CLOCK_24MHZ)
    {
        *FLASH_RESERVED = ((*FLASH_RESERVED & ~0x7) | 1);
    }
    else if (core_clk == FLASH_CLOCK_48MHZ)
    {
        *FLASH_RESERVED = ((*FLASH_RESERVED & ~0x7) | 2);
    }

    return ret;
}

/**
 * @brief Write a word to a flash address
 *
 * This function writes a single word to flash
 *
 * @param [in] addr address of first word location in flash<br>
 * @param [in] word to be written to flash<br>
 * @param [in] enb_endurance set to 0 for default flash endurance<br>
 *                           set to 1 to enable two stage programming for higher
 *                           endurance of data programmed
 * @return Flash API status code @see FlashStatus_t
 * @note addr should be word aligned<br>
 * @note Contents of flash must be erased prior to performing a write<br>
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_WriteWord                 (*((Flash_WriteWord_t *)ROMVECT_FLASH_WRITEWORD))

/**
 * @brief Read a 32-bit word from flash
 *
 * This function reads a 32-bit word from flash. If ECC is enabled (default),<br>
 * hardware will log/generate interrupt on ECC errors
 *
 * @param [in] addr address to read the 38-bit word
 * @param [out] word 32-bit C pointer to the word read from flash,<br>
 * @return Flash API status code @see FlashStatus_t
 * @note addr should be word aligned
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_ReadWord                  (*((Flash_ReadWord_t *)ROMVECT_FLASH_READWORD))

/**
 * @brief Write a 38-bit word to flash
 *
 * This function temporarily disables automatic flash ECC generation in hardware, thus<br>
 * allowing the user to write 38-bits to a single word address in flash<br>
 * A verification of write is perform after write to ensure the write was
 * successful
 *
 * @param [in] addr address to program the 38-bit word
 * @param [in] word 32-bit C pointer to the word to be stored in flash,<br>
 *             word[0] [0:31] bits consist of first lower 32-bits<br>
 *             word[1] [0:6] bits consist of the remaining higher 6-bits<br>
 * @param [in] enb_endurance set to 0 for default flash endurance<br>
 *                           set to 1 to enable two stage programming for higher
 *                           endurance of data programmed
 * @return Flash API status code @see FlashStatus_t
 *
 * @note addr should be word aligned
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_WriteDouble               (*((Flash_WriteDouble_t *)ROMVECT_FLASH_WRITEDOUBLE))

/**
 * @brief Read a 38-bit word from flash
 *
 * This function temporarily disables automatic flash ECC generation in hardware,<br>
 * thus allowing the user to read 38-bits from a single word address in flash.<br>
 * @note ECC check is not performed in this function on the 32-bits. Application should use
 *       other mechanism to verify ECC if necessary
 *
 * @param [in] addr address to read the 38-bit word
 * @param [out] word 32-bit C pointer to store the word which is read from flash,<br>
 *             word[0] [0:31] bits consist of first lower 32-bits<br>
 *             word[1] [0:6] bits consist of the remaining higher 6-bits<br>
 * @return Flash API status code @see FlashStatus_t
 * @note addr should be word aligned
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_ReadDouble                (*((Flash_ReadDouble_t *)ROMVECT_FLASH_READDOUBLE))

/**
 * @brief Write contents of a static memory buffer to flash
 *
 * This function writes the contents of static memory buffer to flash.
 * A verification of write is performed after write to ensure write
 * was successful
 *
 * @param [in] addr address of first word location in flash<br>
 * @param [in] word_length total number of words to be written to flash<br>
 * @param [in] words a 32-bit C pointer to the memory location of the buffer to <br>
 *             be written to flash<br>
 * @param [in] enb_endurance set to 0 for default flash endurance<br>
 *                           set to 1 to enable two stage programming for higher
 *                           endurance of data programmed
 * @return Flash API status code @see FlashStatus_t
 * @note addr should be word aligned<br>
 * @note Contents of flash must be erased prior to performing a write<br>
 * @note CPU interrupts are disabled and enabled momentarily through PRIMASK
 *       register to ensure that the flash<br> is not exposed to higher
 *       programming voltages application should not modify flash registers
 *       before returning from function call
 * @note When enb_endurance is enabled two stage programming is done. <br>
 *       Application must ensure that the function completes and that the <br>
 *       return value is FLASH_ERR_NONE to consider the two stage programming<br>
 *       to be done.
 */
#define Flash_WriteBuffer               (*((Flash_WriteBuffer_t *)ROMVECT_FLASH_WRITEBUFFER))

/**
 * @brief Read contents of flash into a static memory buffer
 *
 * This function uses the flash copier to read contents of flash
 * into a memory buffer
 *
 * @param [in] flash_address address of first word location in flash<br>
 * @param [in] dram_address address of first word location in static memory<br>
 * @param [in] word_length  total number of words to be read<br>
 * @return Flash API status code @see FlashStatus_t
 * @note flash_address and dram_address should be word aligned
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_ReadBuffer                (*((Flash_ReadBuffer_t *)ROMVECT_FLASH_READBUFFER))

/**
 * @brief Erase a sector flash
 *
 * This function perform a sector erase operation on a flash sector
 * memory contents are empty i.e containing the value 0xFFFFFFFF
 *
 * @param [in] addr first word address of flash
 * @param [in] enb_endurance set to 0 for default flash endurance<br>
 *                           set to 1 to enable retry erase iteration for
 *                           higher endurance of flash
 * @return Flash API status code @see FlashStatus_t
 * @note flash_address should be start of sector
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_EraseSector               (*((Flash_EraseSector_t *)ROMVECT_FLASH_ERASESECTOR))

/**
 * @brief Erase a single flash bank
 *
 * This function erases all code and data regions of flash
 *
 * @param [in] no flash instance no to be erased. For FLASH0 use 0,<br>
 *             for FLASH1 use 1 and so on
 * @return Flash API status code @see FlashStatus_t
 * @note A blank check is not performed to ensure flash is successfully erased,<br>
 *       user should use Flash_BlankCheck to verify if chip erase was successful
 * @note NVR regions are not erased
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_EraseFlashBank            (*((Flash_EraseFlashBank_t *)ROMVECT_FLASH_ERASEFLASHBANK))

/**
 * @brief Erase all data and code flash
 *
 * This function erases all code and data regions of flash
 * @return Flash API status code @see FlashStatus_t
 * @note A blank check is not performed to ensure flash is successfully erased,<br>
 *       user should use Flash_BlankCheck to verify if chip erase was successful
 * @note NVR regions are not erased
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 */
#define Flash_EraseChip                 (*((Flash_EraseChip_t *)ROMVECT_FLASH_ERASECHIP))

/**
 * @brief Check if flash region is blank
 *
 * This function uses the flash copier in comparator mode to check if
 * memory contents are empty i.e containing the value 0xFFFFFFFF
 *
 * @param [in] addr first word address of flash
 * @param [in] word_length total number of words to be verified empty
 * @return Flash API status code @see FlashStatus_t
 * @note CPU interrupts are not disabled, application should not modify flash
 *       registers before returning from function call
 * @note flash_address should be word aligned
 */
#define Flash_BlankCheck                (*((Flash_BlankCheck_t *)ROMVECT_FLASH_BLANKCHECK))

/**
 * @brief Read Flash library API version
 * Read the flash version stored in ROM
 * @return Flash library version number
 */
__STATIC_INLINE const short Flash_ReadVersion(void)
{
    /* Call Flash_BlankCheck routine in ROM vector table*/
    return (*((uint32_t *)FLASHVERSION_BASEADDR) & 0xFFFF);
}

/** @} */ /* End of the FLASHROM group */
/** @} */ /* End of the FLASH group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* FLASH_ROM_H_ */
