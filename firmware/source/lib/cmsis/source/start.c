/**
 * @file start.c
 * @brief Cortex-M33 Start Up function
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 * @endparblock
 */

#include "hw.h"

/* ----------------------------------------------------------------------------
 * Defines and Macros
 * --------------------------------------------------------------------------*/
// MIN macro, only use with idempotent operands
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

/* ----------------------------------------------------------------------------
 * Application entry point
 * ------------------------------------------------------------------------- */
extern int main(void);


/**
 * @brief       Initialize the application data and start execution with main.
 *              Should be called from the reset vector.
 * @param[in]   None
 * @return      None
 * @assumptions The symbols __data_init__, __data_start__, __data_end__,
 *              __bss_start__, __bss_end__, and __stack_limit__ are defined when the
 *              application is linked.
 * @assumptions The symbol flash_layout exists, and is an array of structures containing
 *              the start, end, and FLASH_Type* of all the banks of flash, in ascending
 *              memory address order, that the data region could be present in
 */
void __attribute__ ((noreturn)) _start (void)
{
    int i;
    uint32_t * temp = &__data_start__;
    uint32_t * tempInit = &__data_init__;

#ifndef NON_SECURE
    int flash_copy_flags = 0;
    uint32_t words_remaining = (uint32_t) (((uint32_t*)&__data_end__) - ((uint32_t*)&__data_start__));
    uint32_t words_to_copy;
    uint32_t bss_words_remaining = (uint32_t) (((uint32_t)&__bss_end__ - (uint32_t)&__bss_start__));
#endif

    /* Set stack limit immediately, so that init functions can't corrupt stack */
#if (__ARM_FEATURE_CMSE == 3)
    __TZ_set_MSPLIM_NS((uint32_t) &__stack_limit);
#endif

#ifdef NON_SECURE
    /* Initialize the data sections */
    while (temp < &__data_end__)
    {
        *temp++ = *tempInit++;
    }

    /* Initialize the BSS section */
    temp = &__bss_start__;
    while (temp < &__bss_end__)
    {
        *temp++ = 0;
    }
#else
    __set_MSPLIM((uint32_t) &__stack_limit);

    /* Initialize interrupts by ensuring they are all disabled, and there are
     * no pending interrupts. Mask all interrupts using PRIMASK so interrupts
     * aren't actually handled - but don't mask using the FAULTMASK so WFI can
     * still detect pended interrupts. */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);

    for (i = 0; i < ARRAY_SIZE(flash_layout); i++)
    {
        if (tempInit >= ((uint32_t*) flash_layout[i].start)
                && tempInit <= ((uint32_t*) flash_layout[i].end))
        {
            /* +1 is because end points to the last byte in the flash region,
             * not the word after the region. */
            words_to_copy = MIN(words_remaining,
                                ((uint32_t*) (flash_layout[i].end + 1) - tempInit));

            /* Clear any existing flash copy interrupts, before starting the
             * flash copy */
            NVIC_ClearPendingIRQ(FLASH0_COPY_IRQn + i * 2);

            /* Start Copy */
            Sys_Flash_Copy(flash_layout[i].flash,
                           (uint32_t) tempInit,
                           (uint32_t) temp,
                           words_to_copy,
                           COPY_TO_MEM);

            tempInit += words_to_copy;
            temp += words_to_copy;
            flash_copy_flags |= (1 << i);
        }
    }

    /* Setup the BSS initialization so it can be processed in parallel
     * to the data initialization */
    if (bss_words_remaining != 0)
    {
        SYSCTRL->ACCESS_ERROR |= SYSCTRL_ACCESS_ERROR_CLEAR;

        Sys_DMA_Mode_Enable(DMA0, DMA_DISABLE);
        Sys_DMA_Clear_Status(DMA0, DMA_COMPLETE_INT_CLEAR);
        Sys_DMA_Set_Ctrl(DMA0, DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS);

        Sys_DMA_ChannelConfig(DMA0,
                              DMA_SRC_ADDR_STATIC | DMA_DEST_ADDR_INCR_1 |
                              WORD_SIZE_32BITS_TO_32BITS | DMA_COMPLETE_INT_ENABLE,
                              bss_words_remaining,
                              0,
                              (uint32_t) &(SYS_DUMMY_READ),
                              (uint32_t)&__bss_start__);

        /* Clear any existing DMA interrupts, before enabling the DMA. */
        NVIC_ClearPendingIRQ(DMA0_IRQn);

        Sys_DMA_Mode_Enable(DMA0, DMA_ENABLE);
    }

    /* Ensure that the data initialization has completed */
    tempInit = &__data_init__;
    words_remaining = (uint32_t) (((uint32_t*)&__data_end__) - ((uint32_t*)&__data_start__));

    for (i = 0; i < ARRAY_SIZE(flash_layout); i++)
    {
        if (tempInit >= ((uint32_t*) flash_layout[i].start)
                && tempInit <= ((uint32_t*) flash_layout[i].end))
        {
            /* +1 is because end points to the last byte in the flash region,
             * not the word after the region. */
            words_to_copy = MIN(words_remaining,
                                ((uint32_t*) (flash_layout[i].end + 1) - tempInit));

            while ( (flash_layout[i].flash->COPY_CTRL & (1 << FLASH_COPY_CTRL_BUSY_Pos)) == COPY_BUSY)
            {
                NVIC_EnableIRQ(FLASH0_COPY_IRQn + i * 2);
                __WFI();
            }

            NVIC_DisableIRQ(FLASH0_COPY_IRQn + i * 2);
            NVIC_ClearPendingIRQ(FLASH0_COPY_IRQn + i * 2);

            if ( (flash_layout[i].flash->COPY_CTRL & (1 << FLASH_COPY_CTRL_ERROR_Pos)) != COPY_ERROR)
            {
                words_remaining -= words_to_copy;
            }
            tempInit += words_to_copy;
        }
    }

    if (words_remaining != 0)
    {
        /* This (having words left to copy) implies that either
         * 1. The data region is not entirely within flash, or
         * 2. One of the flash copies failed, and needs to be overwritten manually
         * And either way, the remaining words need to be copied manually
         */
        while (temp < &__data_end__)
        {
            *temp++ = *tempInit++;
        }
    }

    /* Ensure that the BSS initialization has completed */
    if (bss_words_remaining != 0)
    {
        while ((DMA0->STATUS & (1 << DMA_STATUS_COMPLETE_INT_Pos)) == 0)
        {
            NVIC_EnableIRQ(DMA0_IRQn);
            __WFI();
        }

        /* Disable the DMA interrupt and cleanup the DMA status */
        NVIC_DisableIRQ(DMA0_IRQn);
        NVIC_ClearPendingIRQ(DMA0_IRQn);
        Sys_DMA_Clear_Status(DMA0, DMA_COMPLETE_INT_CLEAR);
    }

    /* Disable the fault mask again */
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);
#endif

    /* Initialize the heap */
    _sbrk(0);

    /* Execute the pre-initialization code */
    for (i = 0; i < (__preinit_array_end__ - __preinit_array_start__); i++)
    {
        __preinit_array_start__[i]();
    }

    /* Execute the initialization code */
    for (i = 0; i < (__init_array_end__ - __init_array_start__); i++)
    {
        __init_array_start__[i]();
    }

    /* Call the main entry point. */
    main();

    /* Wait for the watchdog reset to trigger since main returned
     * and it should not have returned. */
    while(1);
}
