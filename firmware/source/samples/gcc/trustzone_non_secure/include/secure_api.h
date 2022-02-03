/**
 * @file secure_api.h
 * @brief Secure api exported by the secure application
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
#ifndef SECURE_API_H
#define SECURE_API_H

#include <hw.h>
#include "RTE_Device.h"

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** GPIO number in use as GPIO in non-secure application */
#define NS_STATE_GPIO                   GREEN_LED
#define S_STATE_GPIO                    BLUE_LED

#define NS_UART_TX_GPIO                 6
#define NS_UART_RX_GPIO                 5

/**
 * @brief  Non-Secure gateway function to decrement input integer by 1
 * @param [in] x Input integer parameter
 * @return Input integer minus 1
 */
extern int NSC_Func_Dec(int x);

/**
 * @brief  Non-Secure gateway function to increment input integer by 1
 * @param [in] x Input integer parameter
 * @return Input integer plus 1
 */
extern int NSC_Func_Inc(int x);

extern void NSC_GPIO_Toggle();

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* SECURE_API_H */
