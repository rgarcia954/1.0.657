/**
 * @file power.h
 * @brief Hardware abstraction layer for the system power supply
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

#ifndef POWER_H_
#define POWER_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALPOWER POWER
 *  Power Supply Hardware Abstraction Layer
 *  @{
 */

/**
 * @brief       RF Power Switch and Isolation
 * @examplecode HAL_examples.c Sys_Power_RFEnable_Example
 */
static inline void Sys_Power_RFEnable(void)
{
    /* Enable RF power switches */
    SYSCTRL->RF_POWER_CFG |= BB_POWER_ENABLE | RF_POWER_ENABLE;

    /* Remove RF isolation */
    SYSCTRL->RF_ACCESS_CFG |= BB_ACCESS_ENABLE | RF_ACCESS_ENABLE | RF_IRQ_ACCESS_ENABLE;

    /* Disable and Enable BB access to allow RF to work properly */
    SYSCTRL->RF_ACCESS_CFG &= (~(BB_ACCESS_ENABLE | RF_ACCESS_ENABLE));
    SYSCTRL->RF_ACCESS_CFG |= BB_ACCESS_ENABLE | RF_ACCESS_ENABLE ;
}

/**
 * @brief       CryptoCell312AO Power Switch and Isolation
 * @examplecode HAL_examples.c Sys_Power_CC312AO_Enable_Example
 */
static inline void Sys_Power_CC312AO_Enable(void)
{

#ifndef NON_SECURE
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_CCAO_PWR_EN_Pos))
            | ACS_PWR_KEY | ACS_CCAO_POWERED;
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_CCAO_ISOLATE_Pos))
            | ACS_PWR_KEY | ACS_CCAO_NOT_ISOLATE;

    SYSCTRL->CRYPTOCELL_PWR_CFG = CC_WRITE_KEY | CC_POWER_ENABLE | CC_ISOLATE;
    SYSCTRL->CRYPTOCELL_PWR_CFG = CC_WRITE_KEY | CC_POWER_ENABLE | CC_NOT_ISOLATE;

    #endif
}

/**
 * @brief       CryptoCell312AO Power Switch and Isolation
 * @examplecode HAL_examples.c Sys_Power_CC312AO_Disable_Example
 */
static inline void Sys_Power_CC312AO_Disable(void)
{

#ifndef NON_SECURE

    SYSCTRL->CRYPTOCELL_PWR_CFG = CC_WRITE_KEY | CC_POWER_ENABLE | CC_ISOLATE;
    SYSCTRL->CRYPTOCELL_PWR_CFG = CC_WRITE_KEY | CC_POWER_DISABLE | CC_ISOLATE;

    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_CCAO_ISOLATE_Pos))
            | ACS_PWR_KEY | ACS_CCAO_ISOLATE;
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_CCAO_PWR_EN_Pos))
            | ACS_PWR_KEY | ACS_CCAO_SHUTDOWN;
#endif
}

/** @} */ /* End of the HALPOWER group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* POWER_H_ */
