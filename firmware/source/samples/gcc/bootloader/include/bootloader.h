/**
 * @file bootloader.h
 * @brief Bootloader header file
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
#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>
#include <stdbool.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

#define FLASH_SECTOR_SIZE         0x800

#define BOOT_BASE_ADR             FLASH0_CODE_BASE
#define BOOT_MAX_SIZE             (16 * FLASH_SECTOR_SIZE)
#define APP_MAX_SIZE              (FLASH0_CODE_SIZE + FLASH1_CODE_SIZE - BOOT_MAX_SIZE)
#define APP_BASE_ADDR_OFFSET      BOOT_MAX_SIZE
#define APP_BASE_ADDR             (FLASH0_CODE_BASE + APP_BASE_ADDR_OFFSET)
#define DOWNLOAD_BASE_ADDR        FLASH1_CODE_BASE
#define APP_MIN_SIZE              (FLASH_SECTOR_SIZE / 2)
#define APP_SIG_SIZE              64

#define VER_ID                    "BOOTL*"
#define VER_MAJOR                 1
#define VER_MINOR                 0
#define VER_REVISION              0

#define BOOT_VER_ENCODE(m, n, r)  (((m) << 12) | ((n) << 8) | (r))
#define BOOT_VER_DECODE(num)      ((num >> 12) & 0xF), ((num >> 8) & 0xF), (num & 0xFF)
#define BOOTVECT_GET_VERSION(a)   ((a) + 0x20)
#define BOOTVECT_GET_NEXT(a)      ((a) + 0x24)

#define SYS_BOOT_VERSION(id, mayor, minor, rev)         \
    __attribute__ ((section(".rodata.boot.version"))) \
    const Sys_Boot_app_version_t Sys_Boot_app_version = \
    {                                                   \
        id, BOOT_VER_ENCODE(mayor, minor, rev)          \
    };

#define STACK_POINTER_POS           0
#define RESET_HANDLER_POS           1
#define APP_VERSION_POS             8
#define IMAGE_DESCRIPTOR_POS        9

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

typedef char Sys_Boot_app_id_t[6];

typedef struct
{
    Sys_Boot_app_id_t id;   /* App ID string */
    uint16_t num;           /* format: <major[15:12]>.<minor[11:8]>.<revision[7:0]> */
} Sys_Boot_app_version_t;


typedef enum {
    BOOTLOADER_IMAGE_STATUS_VALID = 0,
    BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE
} Bootloader_image_status_t;

/* ----------------------------------------------------------------------------
 * Function      : Bootloader_image_status_t Bootloader_verify_image
 *                 (uint32_t img_base_addr)
 * ----------------------------------------------------------------------------
 * Description   : Returns the image status.
 * Inputs        : img_base_addr    - image start address
 * Outputs       : return value     - BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE
 *                                    for invalid image
 *                                  - BOOTLOADER_IMAGE_STATUS_VALID for valid image
 * ------------------------------------------------------------------------- */

Bootloader_image_status_t Bootloader_verify_image(uint32_t img_base_addr);

/* ----------------------------------------------------------------------------
 * Function      : bool Bootloader_is_image_bootable(uint32_t base_addr)
 * ----------------------------------------------------------------------------
 * Description   : Validates the specified application.
 * Inputs        : img_base_addr    - image start address
 * Outputs       : return value     - true if validation is successful
 *                                  - false if validation is unsuccessful
 * ------------------------------------------------------------------------- */
bool Bootloader_is_image_bootable(uint32_t base_addr);

/* ----------------------------------------------------------------------------
 * Function      : uint32_t Bootloader_get_image_size(uint32_t base_addr)
 * ----------------------------------------------------------------------------
 * Description   : Gets the image size.
 * Inputs        : img_base_addr    - image start address
 * Outputs       : return value     - 0 if pointer to image size is invalid
 *                                  - size if pointer to image size is valid
 * ------------------------------------------------------------------------- */

uint32_t Bootloader_get_image_size(uint32_t base_addr);

/* ----------------------------------------------------------------------------
 * Function      : const Sys_Boot_app_version_t * Bootloader_get_image_version
 *                 (uint32_t base_addr)
 * ----------------------------------------------------------------------------
 * Description   : Gets the image id and version.
 * Inputs        : img_base_addr    - image start address
 * Outputs       : return value     - NULL if pointer to image version is invalid
 *                                  - version if pointer to image version is valid
 * ------------------------------------------------------------------------- */

const Sys_Boot_app_version_t * Bootloader_get_image_version(uint32_t base_addr);

/* ----------------------------------------------------------------------------
 * Function      : bool Bootloader_copy_downloaded_image_to_execution_area(void)
 * ----------------------------------------------------------------------------
 * Description   : Copy image from download area to execution area.
 * Inputs        : None
 * Outputs       : return value     - true if copy was successful, otherwise false
 * ------------------------------------------------------------------------- */

bool Bootloader_copy_downloaded_image_to_execution_area(void);

/* ----------------------------------------------------------------------------
 * Function      : bool Bootloader_invalidate_downloaded_image(void)
 * ----------------------------------------------------------------------------
 * Description   : Invalidates the image in the download area.
 * Inputs        : None
 * Outputs       : return value     - true if status == FLASH_ERR_NONE
 * ------------------------------------------------------------------------- */

bool Bootloader_invalidate_downloaded_image(void);

/* ----------------------------------------------------------------------------
 * Function      : void print_bootloader_image_status
 *                 (Bootloader_image_status_t img_status)
 * ----------------------------------------------------------------------------
 * Description   : Prints the image status.
 * Inputs        : img_status       - Bootloader_image_status_t
 * Outputs       : None
 * ------------------------------------------------------------------------- */

void print_bootloader_image_status(Bootloader_image_status_t img_status);

/* ----------------------------------------------------------------------------
 * Function      : Bootloader_image_status_t verify_boot_images(void)
 * ----------------------------------------------------------------------------
 * Description   : Returns the image status.
 * Inputs        : None
 * Outputs       : return value     - Bootloader_image_status_t
 * ------------------------------------------------------------------------- */

Bootloader_image_status_t verify_boot_images(void);

/* ----------------------------------------------------------------------------
 * Function      : void Device_trap(void)
 * ----------------------------------------------------------------------------
 * Description   : Resets the device if boot failed.
 * Inputs        : None
 * Outputs       : None
 * ------------------------------------------------------------------------- */

void Device_trap(void);


#endif    /* BOOTLOADER_H */
