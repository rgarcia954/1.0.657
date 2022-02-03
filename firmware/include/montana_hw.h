/**
 * @file montana_hw.h
 * @brief Top-level hardware register abstraction layer for Montana
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

#ifndef MONTANA_HW_H_WRAPPER
#define MONTANA_HW_H_WRAPPER

#ifndef MONTANA_CID
#error MONTANA_CID must be defined
#endif

#if MONTANA_CID == 101

#ifdef NON_SECURE
#include <montana_hw_cid101_ns.h>
#else
#include <montana_hw_cid101.h>
#endif /* ifdef NON_SECURE */

/* Include alternate definitions for some items with more than one name:
 * - JTCK <-> SWCLK
 * - JTMS <-> SWDIO
 * - STORAGE_MODE <-> DEEP_SLEEP_MODE
 */
#define SYSCLK_CLKSRC_JTCK              SYSCLK_CLKSRC_SWCLK
#define SYSCLK_CLKSRC_JTCK_BYTE         SYSCLK_CLKSRC_SWCLK_BYTE

#define CLK_SYS_CFG_JTCK_PRESCALE_Pos   CLK_SYS_CFG_SWCLK_PRESCALE_Pos
#define CLK_SYS_CFG_JTCK_PRESCALE_Mask  CLK_SYS_CFG_SWCLK_PRESCALE_Mask

/* GPIO_JTAG_SW_PAD_CFG bit positions */
#define GPIO_JTAG_SW_PAD_CFG_JTCK_LPF_Pos       GPIO_JTAG_SW_PAD_CFG_SWCLK_LPF_Pos
#define GPIO_JTAG_SW_PAD_CFG_JTMS_LPF_Pos       GPIO_JTAG_SW_PAD_CFG_SWDIO_LPF_Pos
#define GPIO_JTAG_SW_PAD_CFG_JTCK_PULL_Pos      GPIO_JTAG_SW_PAD_CFG_SWCLK_PULL_Pos
#define GPIO_JTAG_SW_PAD_CFG_JTCK_PULL_Mask     GPIO_JTAG_SW_PAD_CFG_SWCLK_PULL_Mask
#define GPIO_JTAG_SW_PAD_CFG_JTMS_DRIVE_Pos     GPIO_JTAG_SW_PAD_CFG_SWDIO_DRIVE_Pos
#define GPIO_JTAG_SW_PAD_CFG_JTMS_DRIVE_Mask    GPIO_JTAG_SW_PAD_CFG_SWDIO_DRIVE_Mask
#define GPIO_JTAG_SW_PAD_CFG_JTMS_PULL_Pos      GPIO_JTAG_SW_PAD_CFG_SWDIO_PULL_Pos
#define GPIO_JTAG_SW_PAD_CFG_JTMS_PULL_Mask     GPIO_JTAG_SW_PAD_CFG_SWDIO_PULL_Mask

#define JTCK_PRESCALE_1                 SWCLK_PRESCALE_1
#define JTCK_PRESCALE_2                 SWCLK_PRESCALE_2
#define JTCK_PRESCALE_4                 SWCLK_PRESCALE_4
#define JTCK_PRESCALE_8                 SWCLK_PRESCALE_8
#define JTCK_PRESCALE_15                SWCLK_PRESCALE_15
#define JTCK_PRESCALE_16                SWCLK_PRESCALE_16

#define JTCK_PRESCALE_1_BYTE            SWCLK_PRESCALE_1_BYTE
#define JTCK_PRESCALE_2_BYTE            SWCLK_PRESCALE_2_BYTE
#define JTCK_PRESCALE_4_BYTE            SWCLK_PRESCALE_4_BYTE
#define JTCK_PRESCALE_8_BYTE            SWCLK_PRESCALE_8_BYTE
#define JTCK_PRESCALE_15_BYTE           SWCLK_PRESCALE_15_BYTE
#define JTCK_PRESCALE_16_BYTE           SWCLK_PRESCALE_16_BYTE

#define JTMS_NO_PULL                    SWDIO_NO_PULL
#define JTMS_WEAK_PULL_UP               SWDIO_WEAK_PULL_UP
#define JTMS_WEAK_PULL_DOWN             SWDIO_WEAK_PULL_DOWN
#define JTMS_STRONG_PULL_UP             SWDIO_STRONG_PULL_UP

#define JTMS_2X_DRIVE                   SWDIO_2X_DRIVE
#define JTMS_3X_DRIVE                   SWDIO_3X_DRIVE
#define JTMS_5X_DRIVE                   SWDIO_5X_DRIVE
#define JTMS_6X_DRIVE                   SWDIO_6X_DRIVE

#define JTCK_NO_PULL                    SWCLK_NO_PULL
#define JTCK_WEAK_PULL_UP               SWCLK_WEAK_PULL_UP
#define JTCK_WEAK_PULL_DOWN             SWCLK_WEAK_PULL_DOWN
#define JTCK_STRONG_PULL_UP             SWCLK_STRONG_PULL_UP

#define JTMS_LPF_DISABLED               SWDIO_LPF_DISABLED
#define JTMS_LPF_ENABLED                SWDIO_LPF_ENABLED

#define JTCK_LPF_DISABLED               SWCLK_LPF_DISABLED
#define JTCK_LPF_ENABLED                SWCLK_LPF_ENABLED

#define GPIO_MODE_JTCK_DIV              GPIO_MODE_SWCLK_DIV

#define PWR_STORAGE_MODE                PWR_DEEP_SLEEP_MODE

#else

#error Invalid MONTANA chip identifier

#endif

#endif /* MONTANA_HW_H_WRAPPER */
