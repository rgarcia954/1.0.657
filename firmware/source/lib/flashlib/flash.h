/**
 * @file flash.h
 * @brief Header file for API functions for flash management through flash interface
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

#ifndef FLASH_H_
#define FLASH_H_

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include <hw.h>

/**
 * @defgroup FLASHg Flash Library Reference
 * @brief Flash Library Reference
 * 
 * This reference chapter presents a detailed description of all the functions
 * in the flash programming and erase support library. This reference includes
 * calling parameters, returned values, and assumptions. 
 * @warning All functions provided by the flash library should be executed
 * from RAM or ROM, as executing them from flash can result in hidden,
 * flash-access-related failures.
 * @{
 */

/** Flash library major version number. */
#define FLASH_FW_VER_MAJOR              0x03

/** Flash library minor version number. */
#define FLASH_FW_VER_MINOR              0x00

/** Flash library revision version number. */
#define FLASH_FW_VER_REVISION           0x02

/** Flash library version number, concatenation of all version numbers. */
#define FLASH_FW_VER                    ((FLASH_FW_VER_MAJOR << 12) | \
                                         (FLASH_FW_VER_MINOR << 8)  | \
                                         FLASH_FW_VER_REVISION)

/** Firmware revision code variable.
 *  Access to this variable is available through the ROM tables. */
extern const short FlashLib_Version;

/** Define FLASH0 as the first flash instance, if this is not defined in the headers. */
#ifndef FLASH0
#define FLASH0                           ((FLASH_Type *)FLASH_BASE)
#endif /* ifndef FLASH0 */

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
 * @brief Flash library return codes.
 */
typedef enum
{
    FLASH_ERR_NONE               = 0x0,    /**< Flash no error. */
    FLASH_ERR_BAD_ADDRESS        = 0x1,    /**< Flash error invalid address parameter. */
    FLASH_ERR_BAD_LENGTH         = 0x2,    /**< Flash error invalid word length parameter. */
    FLASH_ERR_INACCESSIBLE       = 0x3,    /**< Flash error flash is inaccessible. */
    FLASH_ERR_INVALID_PARAMS     = 0x4,    /**< Flash error invalid function parameter. */
    FLASH_ERR_NULL_PARAM         = 0x5,    /**< Flash error null pointer used. */
    FLASH_ERR_ADDRESS_WORD_ALIGN = 0x6,    /**< Flash error address is not word aligned. */
    FLASH_ERR_ZERO_LEN           = 0x7,    /**< Flash error zero length parameter has passed. */
    FLASH_ERR_CRC_CHECK          = 0x8,    /**< Flash error CRC verification has failed. */
    FLASH_ERR_UNKNOWN            = 0x9,    /**< Flash error undefined. */
} FlashStatus_t;

/**
 * @brief Flash operational frequency values supported by the device.
 */
typedef enum
{
    FLASH_CLOCK_3MHZ       = 3000000UL,    /**< Flash Clock value of 3 MHz. */
    FLASH_CLOCK_4MHZ       = 4000000UL,    /**< Flash Clock value of 4 MHz. */
    FLASH_CLOCK_5MHZ       = 5000000UL,    /**< Flash Clock value of 5 MHz. */
    FLASH_CLOCK_8MHZ       = 8000000UL,    /**< Flash Clock value of 8 MHz. */
    FLASH_CLOCK_10MHZ     = 10000000UL,    /**< Flash Clock value of 10 MHz. */
    FLASH_CLOCK_12MHZ     = 12000000UL,    /**< Flash Clock value of 12 MHz. */
    FLASH_CLOCK_16MHZ     = 16000000UL,    /**< Flash Clock value of 16 MHz. */
    FLASH_CLOCK_20MHZ     = 20000000UL,    /**< Flash Clock value of 20 MHz. */
    FLASH_CLOCK_24MHZ     = 24000000UL,    /**< Flash Clock value of 24 MHz. */
    FLASH_CLOCK_48MHZ     = 48000000UL,    /**< Flash Clock value of 48 MHz. */
} FlashClockFrequency_t;

/**
 * @brief Initialize clock and access to flash.
 *
 * This function powers-up and enables access to a flash region. It also
 * applies the correct delay settings based on the specified flash clock
 * frequency (freq).
 *
 * @param [in] num  Flash instance to be initialized
 * @param [in] freq Flash clock frequency in Hertz, only defined frequencies
 *                  supported @see FlashClockFrequency_t
 * @return Flash API status code
 * @see FlashStatus_t
 * @note System clock frequency should not be changed while the flash is being
 *       erased or programmed.<br>
 *       An accurate system clock frequency of 1 MHz or higher is required for
 *       proper flash operation. If using the RC oscillator, care must be taken
 *       as the trimmed frequency for this oscillator has a high temperature
 *       dependency.
 */
FlashStatus_t Flash_Initialize(unsigned int num, FlashClockFrequency_t freq);

/**
 * @brief Write a word to a flash address.
 *
 * This function writes a single word to flash.
 *
 * @param [in] addr          Address of the word to be written.
 * @param [in] word          Data to be written to flash.
 * @param [in] enb_endurance Set to 0 for default flash endurance;<br>
 *                           Set to 1 to enable two-stage programming for
 *                           higher endurance of the data programmed.
 * @return Flash API status code
 * @see FlashStatus_t
 * @note addr must be word aligned.<br>
 *       Contents of flash must be erased prior to performing a write.<br>
 *       Interrupts are not disabled; operation is undefined if the
 *       calling application modifies the values of flash registers before
 *       returning from this function call.
 */
FlashStatus_t Flash_WriteWord(uint32_t addr, uint32_t word,
                              bool enb_endurance);

/**
 * @brief Write contents of a static memory buffer to flash.
 *
 * This function writes the contents of a static memory buffer to flash.
 * A read-back verification is performed after write to ensure the write
 * has been successful.
 *
 * @param [in] addr          Address of first word location in flash.
 * @param [in] word_length   Total number of words to be written to flash.
 * @param [in] words         A 32-bit C pointer to the memory location of<br>
 *                           the buffer to be written to flash.
 * @param [in] enb_endurance Set to 0 for default flash endurance;<br>
 *                           Set to 1 to enable two-stage programming for higher
 *                           endurance of data programmed.
 * @return Flash API status code
 * @see FlashStatus_t
 * @note addr must be word aligned.<br>
 *       Contents of flash must be erased prior to performing a write.<br>
 *       Interrupts are disabled during critical sections, to ensure proper
 *       flash operation.<br>
 *       Applications must ensure that the function completes and that the
 *       return value is FLASH_ERR_NONE to consider the two-stage programming
 *       to be complete.<br>
 *       Source address of data being read and destination address being written,
 *       can not be part the same flash instance.<br>
 *       CRC peripheral registers are modified during execution, and restored
 *       before returning. The CRC must not be used by the application while
 *       writing the buffer to flash.
 */
FlashStatus_t Flash_WriteBuffer(uint32_t addr, uint32_t word_length,
                                const uint32_t *words, bool enb_endurance);

/**
 * @brief Write a 38-bit word to flash.
 *
 * This function temporarily disables automatic flash ECC generation,
 * allowing the user to write 38-bits to a single word address in flash.<br>
 * A read-back verification is performed after write to ensure the write
 * has been successful.
 *
 * @param [in] addr          Address of the word to be written in flash.
 * @param [in] word          32-bit C pointer to the word and ECC data to be
 *                           written to flash:<br>
 *                           - word[0] contains the data to be written to flash<br>
 *                           - word[1] [5:0] contains the 6-bit data to be written
 *                             as the ECC value.
 * @param [in] enb_endurance Set to 0 for default flash endurance;<br>
 *                           Set to 1 to enable two-stage programming for higher
 *                           endurance of data programmed.
 * @return Flash API status code
 * @see FlashStatus_t
 * @note addr must be word aligned.<br>
 *       Interrupts are not disabled; operation is undefined if the
 *       calling application modifies the values of flash registers before
 *       returning from this function call.
 */
FlashStatus_t Flash_WriteDouble(uint32_t addr, const uint32_t *word, bool enb_endurance);

/**
 * @brief Read a 32-bit word from flash.
 *
 * This function reads a 32-bit word from flash. If ECC is enabled (default),
 * hardware will log/generate interrupt on ECC errors.
 *
 * @param [in] addr  Address in flash to be read.
 * @param [out] word 32-bit C pointer to the word read from flash.
 * @return Flash API status code
 * @see FlashStatus_t
 * @note addr must be word aligned.<br>
 *       Interrupts are not disabled; operation is undefined if the
 *       calling application modifies the values of flash registers before
 *       returning from this function call.
 */
FlashStatus_t Flash_ReadWord(uint32_t addr, uint32_t *word);

/**
 * @brief Read contents of flash into a static memory buffer.
 *
 * This function uses the flash copier to read contents of flash
 * into a memory buffer.
 *
 * @param [in] flash_addr   Address of first word location in flash.
 * @param [in] dram_addr    Address of first word location in static memory.
 * @param [in] word_length  Total number of words to be read from flash.
 * @return Flash API status code
 * @see FlashStatus_t
 * @note flash_addr and dram_addr must be word aligned.<br>
 *       Interrupts are not disabled; operation is undefined if the calling
 *       application modifies the values of flash registers before
 *       returning from this function call.<br>
 *       This function fails if the DMA or CryptoCell continuously blocks
 *       memory accesses by the flash copier by accessing memory on every
 *       cycle.
 */
FlashStatus_t Flash_ReadBuffer(uint32_t flash_addr, uint32_t dram_addr,
                               unsigned int word_length);

/**
 * @brief Read a 38-bit word from flash.
 *
 * This function temporarily disables automatic flash ECC generation, allowing
 * the user to read all 38 bits from a single word address in flash.<br>
 * @note ECC checks are not performed on the 32-bit data word or 6-bit ECC
 *       value.
 *
 * @param [in] addr          Address of the word to be read from flash.
 * @param [out] word         32-bit C pointer to the word and ECC data read
 *                           from flash:<br>
 *                           - word[0] contains the data to word read from
 *                             flash.<br>
 *                           - word[1] [5:0] contains the 6-bit data ECC value
 *                             read.
 * @return Flash API status code
 * @see FlashStatus_t
 * @note addr must be word aligned.<br>
 *       Interrupts are not disabled; operation is undefined if the
 *       calling application modifies the values of flash registers before
 *       returning from this function call.
 */
FlashStatus_t Flash_ReadDouble(uint32_t addr, uint32_t *word);

/**
 * @brief Erase a single flash bank.
 *
 * This function erases all code and data regions of a flash instance.
 *
 * @param [in] num  Flash instance not to be erased.
 * @return Flash API status code
 * @see FlashStatus_t
 * @note A blank check is not performed to ensure that the flash has been
 *       successfully erased. Flash_BlankCheck can be used by an application
 *       to verify if the erase has been successful.<br>
 *       NVR regions are not erased.
 *       Interrupts are not disabled; operation is undefined if the
 *       calling application modifies the values of flash registers before
 *       returning from this function call.
 */
FlashStatus_t Flash_EraseFlashBank(uint32_t num);

/**
 * @brief Erase all data and code flash.
 *
 * This function erases all code and data regions of all flash instances.
 *
 * @return Flash API status code
 * @see FlashStatus_t
 * @note A blank check is not performed to ensure that the flash has been
 *       successfully erased. Flash_BlankCheck can be used by an application
 *       to verify if the erase has been successful.<br>
 *       NVR regions are not erased.<br>
 *       Interrupts are not disabled; operation is undefined if the
 *       calling application modifies the values of flash registers before
 *       returning from this function call.
 */
FlashStatus_t Flash_EraseChip(void);

/**
 * @brief Erase a sector flash.
 *
 * This function erases a flash sector (512 words for code, 64 words for data).
 *
 * @param [in] addr          An address within the flash sector to be erased.
 * @param [in] enb_endurance Set to 0 for default flash endurance;<br>
 *                           Set to 1 to enable two-stage erase iteration for
 *                           higher endurance of flash.
 * @return Flash API status code. See @ref FlashStatus_t
 * @note Interrupts are not disabled; operation is undefined if the calling
 *       application modifies the values of flash registers before
 *       returning from this function call.
 */
FlashStatus_t Flash_EraseSector(uint32_t addr, bool enb_endurance);

/**
 * @brief Check if flash region is blank.
 *
 * This function uses the flash copier in comparator mode to verify if the
 * flash contents are empty (i.e containing the erase value 0xFFFFFFFF).
 *
 * @param [in] addr        Address of the first word in flash to be verified.
 * @param [in] word_length Total number of words to be verified.
 * @return Flash API status code @ref FlashStatus_t
 * @note Interrupts are not disabled; operation is undefined if the
 *       calling application modifies the values of flash registers before
 *       returning from this function call.<br>
 *       addr must be word aligned.
 */
FlashStatus_t Flash_BlankCheck(uint32_t addr, unsigned int word_length);

/** @} */ /* End of the FLASHg group */

#ifdef __cplusplus
}

#endif    /* ifdef __cplusplus */

#endif    /* FLASH_H_ */
