/**
 * @file reg.h
 * @brief ARM Cortex-M33 processor register support macros
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

#ifndef REG_H
#define REG_H

/* ----------------------------------------------------------------------------
 * Core Register Setting Support Defines
 * ------------------------------------------------------------------------- */
#define XPSR_SATURATION_NOT_DETECTED    (0x0 << 27)
#define XPSR_SATURATION_DETECTED        (0x1 << 27)

#define XPSR_OVERFLOW_NOT_DETECTED      (0x0 << 28)
#define XPSR_OVERFLOW_DETECTED          (0x1 << 28)

#define XPSR_CARRY_NOT_DETECTED         (0x0 << 29)
#define XPSR_CARRY_DETECTED             (0x1 << 29)

#define XPSR_ZERO_NOT_DETECTED          (0x0 << 30)
#define XPSR_ZERO_DETECTED              (0x1 << 30)

#define XPSR_NEGATIVE_NOT_DETECTED      (0x0 << 31)
#define XPSR_NEGATIVE_DETECTED          (0x1 << 31)

#define PRIMASK_ENABLE_INTERRUPTS       0x0
#define PRIMASK_DISABLE_INTERRUPTS      0x1

#define FAULTMASK_ENABLE_INTERRUPTS     0x0
#define FAULTMASK_DISABLE_INTERRUPTS    0x1

#endif /* REG_H */
