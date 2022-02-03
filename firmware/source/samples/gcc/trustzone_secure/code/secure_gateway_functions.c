/**
 * @file trustzone.c
 * @brief Trustzone helper functions
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
#include <app.h>

int __attribute__ ((cmse_nonsecure_entry)) NSC_Func_Inc(int x)
{
    return App_Secure_Api_Inc(x);
}

int __attribute__ ((cmse_nonsecure_entry)) NSC_Func_Dec(int x)
{
    return App_Secure_Api_Dec(x);
}

void __attribute__ ((cmse_nonsecure_entry)) NSC_GPIO_Toggle(void)
{
   GPIO_Toggle();
}
