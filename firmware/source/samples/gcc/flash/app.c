/**
 * @file app.c
 * @brief Sample application code for flash ROM based API
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

#include "app.h"

#define RANDOM_WORDS_NUM                 25U

uint8_t gError;

/* A fixed word array of randomly generated data */
const uint32_t pattern[RANDOM_WORDS_NUM] =
{
    0x56c0a5cb,
    0x90fcee77,
    0xa22f091a,
    0x304b0b83,
    0xe58b8ae6,
    0xf0b29682,
    0x8bcdeb7e,
    0x28eebc0b,
    0x9da0fbe9,
    0x3ab107b4,
    0x04825c05,
    0x1eb02d38,
    0xeab3d9f3,
    0x5b5fb785,
    0x8f83f946,
    0x5b4e3f6a,
    0xa449c760,
    0x4f10c69f,
    0x19f1bb11,
    0x70b60a15,
    0x2157e34c,
    0x98d3aea4,
    0x79e83ee0,
    0x47800cae,
    0xcfcfac56,
};

void App_Demo_Error(uint8_t error, FlashStatus_t result)
{
    gError = error;

    /* If code execution reached here, demo was unsuccessful. The step that
     * failed is stored to gError, and the failure code from the flash library
     * is available in result. */
    while (1)
    {
        /* Refresh the watchdog timers */
        SYS_WATCHDOG_REFRESH();
    }
}

void App_Demo_SequentialWrite(uint32_t addr)
{
    /* Perform a regular erase with a default(longer) time duration,
     * however an erase is guaranteed first time */
    FlashStatus_t result = Flash_EraseSector(addr, 0);
    if (result != FLASH_ERR_NONE)
    {
        /* An error has occurred; flag this as error 1 and hold the sample in
         * an error wait state spin loop */
        App_Demo_Error(1, result);
    }

    /* Write data in sequential manner to flash in normal endurance mode
     * Note that this function also performs a verification at the end to
     * check if write was successful. Hence further verification by application
     * is not necessary */
    result = Flash_WriteBuffer(addr, RANDOM_WORDS_NUM, pattern, 0);
    if (result != FLASH_ERR_NONE)
    {
        /* An error has occurred; flag this as error 2 and hold the sample in
         * an error wait state spin loop */
        App_Demo_Error(2, result);
    }
}

void App_Demo_WordWrite(uint32_t addr)
{
    uint32_t read_storage[RANDOM_WORDS_NUM];

    /* Perform a sector erase in the default endurance mode */
    FlashStatus_t result = Flash_EraseSector(addr, 0);
    if (result != FLASH_ERR_NONE)
    {
        /* An error has occurred; flag this as error 3 and hold the sample in
         * an error wait state spin loop */
        App_Demo_Error(3, result);
    }

    for (uint32_t idx = 0; idx < RANDOM_WORDS_NUM; idx++)
    {
        uint32_t word_addr = addr + idx * 4;

        /* Write a word to flash in the default endurance mode.
         * Note: This function also performs a verification at the end to
         *       check if write was successful, so the application is not
         *       required to perform further verification. */
        Flash_WriteWord(word_addr, pattern[idx], 0);
    }

    /* Perform a re-verification of written data for illustration purposes */
    result = Flash_ReadBuffer(addr, (uint32_t)read_storage, RANDOM_WORDS_NUM);
    if (result != FLASH_ERR_NONE)
    {
        /* An error has occurred; flag this as error 4 and hold the sample in
         * an error wait state spin loop */
        App_Demo_Error(4, result);
    }

    if (memcmp(read_storage, pattern, RANDOM_WORDS_NUM) != 0)
    {
        /* An error has occurred; flag this as error 5 and hold the sample in
         * an error wait state spin loop */
        App_Demo_Error(5, result);
    }
}

int main(void)
{
    uint32_t trim_error __attribute__((unused));

    /* Disable and clear all pending interrupts. */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Check for recovery GPIO to enable recovery. */
    SYS_GPIO_CONFIG(RECOVERY_GPIO, GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE);
    while((Sys_GPIO_Read(RECOVERY_GPIO)) == 0)
    {
       SYS_WATCHDOG_REFRESH();
    }

    /* Load default trim values. */
    trim_error = SYS_TRIM_LOAD_DEFAULT();

    /* Enable 48 MHz oscillator to generate 16 MHz SystemClock */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_3_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the 
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Power up and initialize flash timing registers based on SystemClock */
    gError = 0;
    Flash_Initialize(0, FLASH_CLOCK_16MHZ);
    Flash_Initialize(1, FLASH_CLOCK_16MHZ);

    /* Note: The data to be written to a flash instance can not be read from
     * the flash instance that is being written to! */

    /* Demonstrate sequential write operation */
    App_Demo_SequentialWrite(FLASH1_CODE_BASE);

    /* Demonstrate word write operation */
    App_Demo_WordWrite(FLASH1_NVR1_BASE);

    /* If code execution reached here, demo was successful; loop forever */
    while (1)
    {
        /* Refresh the watchdog timers */
        SYS_WATCHDOG_REFRESH();
    }
}
