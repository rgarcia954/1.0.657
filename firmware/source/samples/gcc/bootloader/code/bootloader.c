/**
 * @file bootloader.c
 * @brief Bootloader source file
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
#include "bootloader.h"
#include "drv_targ.h"


/* ----------------------------------------------------------------------------
 * Constants for use in application validation
 * ------------------------------------------------------------------------- */

typedef enum
{
	SECTOR_DIRTY,
	SECTOR_BLANK,
	SECTOR_MATCH
} compare_result_t;

const Sys_Boot_app_version_t Sys_Boot_version =
{
    VER_ID, BOOT_VER_ENCODE(VER_MAJOR, VER_MINOR, VER_REVISION)
};



static uint32_t flash_sector_buffer[FLASH_SECTOR_SIZE / sizeof(uint32_t)];

static compare_result_t compare_and_copy_sector(const uint32_t* sector,
		const uint32_t* ref_sector, uint32_t *buffer);

static void flash_main_write_enable(bool enable)
{
	if(enable)
	{
		FLASH->MAIN_CTRL = CODE_A_0K_TO_22K_W_ENABLE |
				CODE_A_22K_TO_44K_W_ENABLE |
				CODE_A_44K_TO_66K_W_ENABLE |
				CODE_A_66K_TO_88K_W_ENABLE;

		FLASH->MAIN_WRITE_UNLOCK = FLASH_MAIN_KEY;
	}
	else
	{
		FLASH->MAIN_CTRL = 0;
		FLASH->MAIN_WRITE_UNLOCK = FLASH_MAIN_KEY;
	}
}

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

Bootloader_image_status_t Bootloader_verify_image(uint32_t img_base_addr)
{
	if(!Bootloader_is_image_bootable((uint32_t)img_base_addr))
	{
		return BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE;
	}

	return BOOTLOADER_IMAGE_STATUS_VALID;
}


/* ----------------------------------------------------------------------------
 * Function      : bool Bootloader_is_image_bootable(uint32_t base_addr)
 * ----------------------------------------------------------------------------
 * Description   : Validates the specified application.
 * Inputs        : img_base_addr    - image start address
 * Outputs       : return value     - true if validation is successful
 *                                  - false if validation is unsuccessful
 * ------------------------------------------------------------------------- */

bool Bootloader_is_image_bootable(uint32_t base_addr)
{
	uint_fast32_t entry;
	const uint32_t *vector_a = (const uint32_t *)base_addr;

	/* Test Stack Pointer */
	entry = vector_a[STACK_POINTER_POS];
	if (entry < DRAM_BASE                            ||
			entry > DRAM_BASE + DRAM_SIZE + BB_DRAM_SIZE ||
			entry % sizeof(uint32_t) != 0)
	{
		return false;
	}

	/* Test Program Counter */
	entry = vector_a[RESET_HANDLER_POS];
	if (entry < APP_BASE_ADDR + 16 * sizeof(uint32_t) ||
			entry > APP_BASE_ADDR + FLASH_SECTOR_SIZE     ||
			entry % sizeof(uint16_t) != 1)
	{
		return false;
	}

	/* Test App Version pointer */
	entry = vector_a[APP_VERSION_POS];
	if (entry < APP_BASE_ADDR + 16 * sizeof(uint32_t) ||
			entry >= APP_BASE_ADDR + FLASH_SECTOR_SIZE    ||
			entry % sizeof(uint16_t) != 0)
	{
		return false;
	}

	/* Test primary BootLoader descriptor */
	entry = vector_a[IMAGE_DESCRIPTOR_POS];
	if (entry < APP_BASE_ADDR + 16 * sizeof(uint32_t) ||
			entry >= APP_BASE_ADDR + FLASH_SECTOR_SIZE    ||
			entry % sizeof(uint32_t) != 0)
	{
		return false;
	}

	return true;
}

/* ----------------------------------------------------------------------------
 * Function      : uint32_t Bootloader_get_image_size(uint32_t base_addr)
 * ----------------------------------------------------------------------------
 * Description   : Gets the image size.
 * Inputs        : img_base_addr    - image start address
 * Outputs       : return value     - 0 if pointer to image size is invalid
 *                                  - size if pointer to image size is valid
 * ------------------------------------------------------------------------- */

uint32_t Bootloader_get_image_size(uint32_t base_addr)
{
	const uint32_t *vector_a = (const uint32_t *)base_addr;

	/* Check that the pointer to the image size is valid */
	if (vector_a[IMAGE_DESCRIPTOR_POS] < APP_BASE_ADDR + 16 * sizeof(uint32_t) ||
			vector_a[IMAGE_DESCRIPTOR_POS] >= APP_BASE_ADDR + FLASH_SECTOR_SIZE    ||
			vector_a[IMAGE_DESCRIPTOR_POS] % sizeof(uint32_t) != 0)
	{
		return 0;
	}
	return *((const uint32_t *)(vector_a[IMAGE_DESCRIPTOR_POS] - APP_BASE_ADDR + (uint32_t)base_addr));
}

/* ----------------------------------------------------------------------------
 * Function      : const Sys_Boot_app_version_t * Bootloader_get_image_version
 *                 (uint32_t base_addr)
 * ----------------------------------------------------------------------------
 * Description   : Gets the image id and version.
 * Inputs        : img_base_addr    - image start address
 * Outputs       : return value     - NULL if pointer to image version is invalid
 *                                  - version if pointer to image version is valid
 * ------------------------------------------------------------------------- */

const Sys_Boot_app_version_t * Bootloader_get_image_version(uint32_t base_addr)
{
	uint_fast32_t vector = *(const uint32_t *)BOOTVECT_GET_VERSION(base_addr);

	if (vector > base_addr && vector < FLASH1_CODE_TOP)
	{
		return (const Sys_Boot_app_version_t *)(vector);
	}
	return NULL;
}


/* ----------------------------------------------------------------------------
 * Function      : bool Bootloader_copy_downloaded_image_to_execution_area(void)
 * ----------------------------------------------------------------------------
 * Description   : Copy image from download area to execution area.
 * Inputs        : None
 * Outputs       : return value     - true if copy was successful, otherwise false
 * ------------------------------------------------------------------------- */

bool Bootloader_copy_downloaded_image_to_execution_area(void)
{
	uint32_t dst_adr;
	uint32_t src_adr;
	uint32_t end_adr;
	uint32_t size = Bootloader_get_image_size((uint32_t)DOWNLOAD_BASE_ADDR);

	if (size == 0)
	{
		return false; /* No copy needed */
	}

	/* Configure the flash to allow writing to the whole main flash area */
	flash_main_write_enable(true);

	/* Copy Image */
	for (dst_adr = (uint32_t)APP_BASE_ADDR,
			src_adr = (uint32_t)DOWNLOAD_BASE_ADDR, end_adr = src_adr + size;
			src_adr < end_adr; dst_adr += FLASH_SECTOR_SIZE, src_adr += FLASH_SECTOR_SIZE)
	{
		switch (compare_and_copy_sector((uint32_t*)dst_adr, (uint32_t*)src_adr, flash_sector_buffer))
		{
		case SECTOR_DIRTY:
		{
			if (Flash_EraseSector(dst_adr, false) != FLASH_ERR_NONE)
			{
				flash_main_write_enable(false);
				return false;
			}
		}
		//no break
		case SECTOR_BLANK:
		{
			if (Flash_WriteBuffer(dst_adr,
					FLASH_SECTOR_SIZE / sizeof(uint32_t),
					flash_sector_buffer, false) != FLASH_ERR_NONE)
			{
				flash_main_write_enable(false);
				return false;
			}
		}
		break;

		case SECTOR_MATCH:
		{
		}
		break;
		}
		SYS_WATCHDOG_REFRESH();
	}

	flash_main_write_enable(false);
	return true;
}

/* ----------------------------------------------------------------------------
 * Function      : bool Bootloader_invalidate_downloaded_image(void)
 * ----------------------------------------------------------------------------
 * Description   : Invalidates the image in the download area.
 * Inputs        : None
 * Outputs       : return value     - true if status == FLASH_ERR_NONE
 * ------------------------------------------------------------------------- */

bool Bootloader_invalidate_downloaded_image(void)
{
	unsigned int status;
	/* Invalidate image in download area */
	flash_main_write_enable(true);
	status = Flash_WriteWord((uint32_t)DOWNLOAD_BASE_ADDR, 0, false);
	flash_main_write_enable(false);

	return status == FLASH_ERR_NONE;
}

/* ----------------------------------------------------------------------------
 * Function      : static compare_result_t compare_and_copy_sector(
                     const uint32_t* sector, const uint32_t* ref_sector,
                     uint32_t *buffer)
 * ----------------------------------------------------------------------------
 * Description   : Compares the content of a Flash sector with a reference
 *                 sector and at the same time copies the reference sector
 *                 into a RAM buffer.
 * Inputs        : sector           - start address of check sector to check
 *                 ref_sector       - start address of reference sector
 *                 buffer           - buffer to copy ref_sector to
 * Outputs       : return value     - SECTOR_DIRTY  sector must be erased
 *                                    prior to program it
 *                                  - SECTOR_BLANK  sector is already blank
 *                                    and can directly be programmed
 *                                  - SECTOR_MATCH  check sector and reference
 *                                    sector have the same content
 * Assumptions   :
 * ------------------------------------------------------------------------- */
static compare_result_t compare_and_copy_sector(const uint32_t* sector,
		const uint32_t* ref_sector, uint32_t *buffer)
{
	uint16_t i;
	uint32_t blank = UINT32_MAX;
	compare_result_t result = SECTOR_MATCH;

	for (i = 0; i < FLASH_SECTOR_SIZE/sizeof(uint32_t); i++)
	{
		if (sector[i] != ref_sector[i])
		{
			result = SECTOR_DIRTY;
		}
		buffer[i] = ref_sector[i];
		blank &= sector[i];
	}

	if (result != SECTOR_MATCH && blank == UINT32_MAX)
	{
		result = SECTOR_BLANK;
	}
	return result;
}




