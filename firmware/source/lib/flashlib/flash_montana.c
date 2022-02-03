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

/* Region constant structures */
static const struct region_attributes code_region_params =
{
    CODE_ROW_LEN_WORDS,
    CODE_SECTOR_LEN_WORDS,
    CODE_UNLOCK_REGION_LEN_BYTES,
    CODE_UNLOCK_REGION_NUM,
    &Sys_Flash_ReadCodeAccessConfig,
    &Sys_Flash_WriteCodeAccessConfig,
};

static const struct region_attributes data_region_params =
{
    DATA_ROW_LEN_WORDS,
    DATA_SECTOR_LEN_WORDS,
    DATA_UNLOCK_REGION_LEN_BYTES,
    DATA_UNLOCK_REGION_NUM,
    &Sys_Flash_ReadDataAccessConfig,
    &Sys_Flash_WriteDataAccessConfig,
};

static const struct region_attributes nvr_region_params_flash0 =
{
    NVR_ROW_LEN_WORDS,
    NVR_SECTOR_LEN_WORDS,
    NVR_UNLOCK_REGION_LEN_BYTES,
    NVR_UNLOCK_FLASH0_REGION_NUM,
    &Sys_Flash_Read_Flash_NVRAccessConfig,
    &Sys_Flash_Write_Flash_NVRAccessConfig,
};

static const struct region_attributes nvr_region_params_flash =
{
    NVR_ROW_LEN_WORDS,
    NVR_SECTOR_LEN_WORDS,
    NVR_UNLOCK_REGION_LEN_BYTES,
    NVR_UNLOCK_FLASH_REGION_NUM,
    &Sys_Flash_Read_Flash_NVRAccessConfig,
    &Sys_Flash_Write_Flash_NVRAccessConfig,
};

static const struct power_access flash_power_access[FLASH_INSTANCE_NUM] =
{
    {
        FLASH0,
        FLASH0_POWER_ENABLE,
        FLASH0_ACCESS_ENABLE,
    },
    {
        FLASH1,
        FLASH1_POWER_ENABLE,
        FLASH1_ACCESS_ENABLE,
    },
};

/* Flash 0 instance region array constants */
static const struct region_descriptor flash0_descriptor[FLASH_0_DESCR_NUM] =
{
    [CODE_IDX] = {
        FLASH0_CODE_BASE,
        FLASH0_CODE_TOP,
        &code_region_params,
    },
    [DATA_IDX] = {
        FLASH0_DATA_BASE,
        FLASH0_DATA_TOP,
        &data_region_params,
    },
    [NVR_IDX] = {
        FLASH0_NVR4_BASE,
        FLASH0_NVR7_TOP,
        &nvr_region_params_flash0,
    },
};

/* Flash 1 instance region array constants */
static const struct region_descriptor flash1_descriptor[FLASH_1_DESCR_NUM] =
{
    [CODE_IDX] = {
        FLASH1_CODE_BASE,
        FLASH1_CODE_TOP,
        &code_region_params,
    },
    [DATA_IDX] = {
        FLASH1_DATA_BASE,
        FLASH1_DATA_TOP,
        &data_region_params,
    },
    [NVR_IDX] = {
        FLASH1_NVR0_BASE,
        FLASH1_NVR7_TOP,
        &nvr_region_params_flash,
    },
};

/* Flash interfaces array constants */
const struct interface *ifcs[FLASH_INSTANCE_NUM];

static const struct interface ifcs_full[FLASH_INSTANCE_NUM] =
{
    {
        FLASH0,
        FLASH_0_DESCR_NUM,
        flash0_descriptor,
    },
    {
        FLASH1,
        FLASH_1_DESCR_NUM,
        flash1_descriptor,
    }
};

FlashStatus_t Flash_Initialize(unsigned int num, FlashClockFrequency_t core_clk)
{
    FlashStatus_t r;
    r = Flash_InitializeParamSanity(num, core_clk);
    volatile uint32_t *FLASH_RESERVED = &flash_power_access[num].flash->RESERVED0[0];

    if (r == FLASH_ERR_NONE)
    {
        /* Apply the delay settings for the clock frequency
         * The DELAY_CTRL must be updated before enabling the flash
         * The DELAY_CTRL must not be changed during the wake-up procedure
         * of the flash, and its value must correspond to the current clock
         * period. */
        for (unsigned int i = 0; i < FREQ_MAP_NUM; i++)
        {
            if (core_clk == freq_map[i])
            {
                flash_power_access[num].flash->DELAY_CTRL =
                    i << FLASH_DELAY_CTRL_SYSCLK_FREQ_Pos;
                break;
            }
        }

        /* For faster clock frequencies, adjust the internal flash delays */
        if (core_clk == FLASH_CLOCK_24MHZ)
        {
            *FLASH_RESERVED = ((*FLASH_RESERVED & ~0x7) | 1);
        }
        else if (core_clk == FLASH_CLOCK_48MHZ)
        {
            *FLASH_RESERVED = ((*FLASH_RESERVED & ~0x7) | 2);
        }

        /* Power up and enable memory */
        SYSCTRL->MEM_POWER_CFG |= flash_power_access[num].p_enb;
        SYSCTRL->MEM_ACCESS_CFG |= flash_power_access[num].acc_enb;
        Sys_Flash_IF_WaitBusy(flash_power_access[num].flash);

        /* Setup the ifcs structure */
        ifcs[0] = &(ifcs_full[0]);
        ifcs[1] = &(ifcs_full[1]);
    }
    return r;
}
