/**
 * @file app.c
 * @brief Bootloader main
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
#include "drv_uart.h"
#include "swmTrace_api.h"


/* ----------------------------------------------------------------------------
 * Local variables and types
 * --------------------------------------------------------------------------*/

static const uint32_t traceOptions[] = {
		SWM_LOG_LEVEL_INFO
};


/* ----------------------------------------------------------------------------
 * Function      : static void Init(void)
 * ----------------------------------------------------------------------------
 * Description   : System Initialization.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */

static void Init(void)
{
	Drv_Targ_Init();
	Drv_Uart_Init();
}

/* ----------------------------------------------------------------------------
 * Function      : void print_bootloader_image_status
 *                 (Bootloader_image_status_t img_status)
 * ----------------------------------------------------------------------------
 * Description   : Prints the image status.
 * Inputs        : img_status       - Bootloader_image_status_t
 * Outputs       : None
 * ------------------------------------------------------------------------- */

void print_bootloader_image_status(Bootloader_image_status_t img_status)
{
	switch (img_status)
	{
	case BOOTLOADER_IMAGE_STATUS_VALID:
		swmLogInfo("BOOTLOADER_IMAGE_STATUS_VALID\n");
		break;
	case BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE:
		swmLogInfo("BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE\n");
		break;
	}
}

/* ----------------------------------------------------------------------------
 * Function      : Bootloader_image_status_t verify_boot_images(void)
 * ----------------------------------------------------------------------------
 * Description   : Returns the image status.
 * Inputs        : None
 * Outputs       : return value     - Bootloader_image_status_t
 * ------------------------------------------------------------------------- */

Bootloader_image_status_t verify_boot_images(void)
{
	Bootloader_image_status_t img_status;
	const Sys_Boot_app_version_t * version_info;

	swmLogInfo("Checking if there is a valid new image in the download area(%p)...\n",DOWNLOAD_BASE_ADDR);
	img_status = Bootloader_verify_image((uint32_t)DOWNLOAD_BASE_ADDR);
	print_bootloader_image_status(img_status);

	if(img_status == BOOTLOADER_IMAGE_STATUS_VALID)
	{
		swmLogInfo("Copying new image to the execution area...");
		if (Bootloader_copy_downloaded_image_to_execution_area())
		{
			swmLogInfo(" image copied successfully.\nInvalidating data in download area...\n");
			Bootloader_invalidate_downloaded_image();
			return BOOTLOADER_IMAGE_STATUS_VALID;
		}
		else
		{
			swmLogTestFail("Error while copying image.\n");
		}
	}

	swmLogInfo("Checking if there is a valid image in the execution area (%p)...\n",APP_BASE_ADDR);
	img_status = Bootloader_verify_image((uint32_t)APP_BASE_ADDR);
	print_bootloader_image_status(img_status);
	version_info = Bootloader_get_image_version((uint32_t)(APP_BASE_ADDR));
	swmLogInfo("Image: %s ver=(%u.%u.%u)\n", version_info->id, BOOT_VER_DECODE(version_info->num));

	return img_status;
}

/* ----------------------------------------------------------------------------
 * Function      : void ProcessCmd(void)
 * ----------------------------------------------------------------------------
 * Description   : Processes a CMD message.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   :
 * ------------------------------------------------------------------------- */
void ProcessCmd(void)
{
    cmd_msg_t  *cmd_p = RecvCmd();

    switch (cmd_p->type)
    {
        case HELLO:
        {
            ProcessHello();
        }
        break;

        case PROG:
        {
            ProcessProg(&cmd_p->arg.prog);
        }
        break;

    #if (CFG_READ_SUPPORT)
        case READ:
        {
            ProcessRead(cmd_p);
        }
        break;
    #endif /* if (CFG_READ_SUPPORT) */

        case RESTART:
        {
            ProcessRestart();
        }
        break;

        default:
        {
            SendError(UNKNOWN_CMD);
        }
        break;
    }
}

/* ----------------------------------------------------------------------------
 * Function      : void Device_trap(void)
 * ----------------------------------------------------------------------------
 * Description   : Resets the device if boot failed.
 * Inputs        : None
 * Outputs       : None
 * ------------------------------------------------------------------------- */

void Device_trap(void)
{
	while (1)
	{
		swmLogInfo("Boot failed! Please write a valid image in flash and reset the device.\n");
		ProcessCmd();
		Drv_Targ_Poll();
	}
}

/* ----------------------------------------------------------------------------
 * Function      : int main(void)
 * ----------------------------------------------------------------------------
 * Description   : Main routine of the bootloader.
 * Inputs        : None
 * Outputs       : Returns always 0
 * ------------------------------------------------------------------------- */

int main(void)
{
	Init();
	swmTrace_init(traceOptions, 1);
	swmLogInfo("Bootloader has started!\n");
	if (!CheckUpdatePin())
	{

	    if (verify_boot_images() == BOOTLOADER_IMAGE_STATUS_VALID)
	    {
		    swmLogInfo("Booting... \n");
		    Sys_StartApp((uint32_t *)APP_BASE_ADDR);
	    }
	}
	Device_trap();
	return 0;
}
