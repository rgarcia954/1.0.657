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

void TZ_SAU_Configure(void)
{
    /* Configure SAU for NSC and NS address */

    /* Enable SAU, when disabled all address ranges are marked secure only */
    SAU->CTRL = (0x1 << SAU_CTRL_ALLNS_Pos) | (1 << SAU_CTRL_ENABLE_Pos);

    /* Configure NSC region  */
    SAU->RNR  = (0x0 << SAU_RNR_REGION_Pos);
    SAU->RBAR = ((uint32_t)&__Start_Of_FLASH_NS__) & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (((uint32_t)&__End_Of_FLASH_NS__) & SAU_RLAR_LADDR_Msk) |
                (1 << SAU_RLAR_ENABLE_Pos);

    /* Configure NS region for code */
    SAU->RNR  = (0x1 << SAU_RNR_REGION_Pos);
    SAU->RBAR = ((uint32_t)&__Start_Of_FLASH_SG__) & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (((uint32_t)&__End_Of_FLASH_SG__) & SAU_RLAR_LADDR_Msk) |
                (1 << SAU_RLAR_NSC_Pos) | (1 << SAU_RLAR_ENABLE_Pos);

    /* Configure NS Data and stack regions */
    SAU->RNR  = (0x2 << SAU_RNR_REGION_Pos);
    SAU->RBAR = ((uint32_t)&__Start_Of_DRAM_NS__) & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (((uint32_t)&__End_Of_DRAM_NS__) & SAU_RLAR_LADDR_Msk) |
                (1 << SAU_RLAR_ENABLE_Pos);

    /* Provide access to all Peripherals */
    SAU->RNR  = (0x3 << SAU_RNR_REGION_Pos);
    SAU->RBAR = (PERIPHERAL_BASE & SAU_RBAR_BADDR_Msk);
    SAU->RLAR = (PERIPHERAL_TOP & SAU_RLAR_LADDR_Msk) |
                (1 << SAU_RLAR_ENABLE_Pos);
}

void TZ_IDAU_MEM_Configure(void)
{
    /* Enable DRAM4 to DRAM7 to be accessible by non-secure application */
    SYSCTRL->NS_ACCESS_RAM_CFG0 = NS_CAN_ACCESS_DRAM4 |
                                  NS_CAN_ACCESS_DRAM5 |
                                  NS_CAN_ACCESS_DRAM6 |
                                  NS_CAN_ACCESS_DRAM7;

    /* Enable GPIO and UART peripheral access to non-secure application */
    SYSCTRL->NS_ACCESS_PERIPH_CFG0 = (NS_CAN_ACCESS_GPIO | NS_CAN_ACCESS_UART0 |
    		    NS_CAN_ACCESS_WATCHDOG | NS_CAN_ACCESS_GPIO_SRC | NS_CAN_ACCESS_ACS);
}

void TZ_IDAU_GPIO_Configure(void)
{
    /* Configure GPIO for NS application */
    GPIO->CFG[NS_STATE_GPIO] = GPIO_MODE_GPIO_OUT |
                         NS_CAN_USE_GPIO |
                         GPIO_STRONG_PULL_UP |
                         GPIO_6X_DRIVE;

    GPIO->CFG[NS_UART_TX_GPIO] = GPIO_MODE_UART0_TX |
                                 NS_CAN_USE_GPIO |
                                 GPIO_WEAK_PULL_UP |
                                 GPIO_6X_DRIVE;

    GPIO->CFG[NS_UART_RX_GPIO] = GPIO_MODE_INPUT |
                                 NS_CAN_USE_GPIO |
                                 GPIO_WEAK_PULL_UP |
                                 GPIO_6X_DRIVE;
}

void TZ_NS_NVIC_Configure(void)
{
	/* Setup Non-Secure interrupt vector table */
	SCB_NS->VTOR = ((uint32_t)&__Start_Of_FLASH_NS__);

	/* Enable UART TX interrupt to be generated for NS application */
	NVIC->ITNS[0] = NVIC_UART0_TX_INT_NON_SECURE | NVIC_UART0_RX_INT_NON_SECURE;

	__TZ_set_FAULTMASK_NS(FAULTMASK_DISABLE_INTERRUPTS);
	__TZ_set_PRIMASK_NS(PRIMASK_DISABLE_INTERRUPTS);
}

void TZ_Configure(void)
{
/* Setup floating point access for non-secure application */
#if (__FPU_USED == 1)
    SCB->CPACR |= (3UL << 20) | (3UL << 22);

    __DSB();
    __ISB();

    SCB_NS->CPACR |= (3UL << 20) | (3UL << 22);
    __DSB();
    __ISB();

    SCB->NSACR |= (1 << 10);
    __DSB();
    __ISB();
#endif    /* if (__FPU_USED == 1) */

    /* Configure SAU */
    TZ_SAU_Configure();

    /* Configure IDAU Memory filters */
    TZ_IDAU_MEM_Configure();

    /* Configure IDAU GPIO access */
    TZ_IDAU_GPIO_Configure();

    /* Configure NVIC */
    TZ_NS_NVIC_Configure();
}

void TZ_Non_Secure_Application(void)
{
    /* Set non-secure main stack (MSP_NS) */
    __TZ_set_MSP_NS(*((uint32_t *)((uint32_t)&__Start_Of_FLASH_NS__)));

    /* Get non-secure reset handler */
    funcptr_ns NonSecure_ResetHandler =
        (funcptr_ns)(*((uint32_t *)(((uint32_t)&__Start_Of_FLASH_NS__) + 4U)));

    /* Execute non-secure reset handler */
    NonSecure_ResetHandler();
}
