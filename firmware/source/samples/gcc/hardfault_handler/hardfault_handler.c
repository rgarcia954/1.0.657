/******************************************************************************
 * @file     HardFault_Handler.c
 * @brief    HardFault handler example
 * @version  V1.00
 * @date     10. July 2017
 ******************************************************************************/
/*
 * Copyright (c) 2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file hardfault_handler.c
 * @brief A sample complex hardfault_handler that prints relevant information
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
 * This is Reusable Code.  @endparblock
 */

#include <hw.h>
#include <stdio.h>

extern void HardFault_Handler(void);
void HardFault_Handler_C(unsigned long * hardfault_args, unsigned int lr_value);

/* HardFault handler wrapper in assembly language.
 * It extracts the location of stack frame and passes it to the handler written
 * in C as a pointer. We also extract the LR value as second parameter. */
void HardFault_Handler(void)
{
	__asm__
	(
		"	TST    LR, #4\n"
		"	ITE    EQ\n"
		"	MRSEQ  R0, MSP\n"
		"	MRSNE  R0, PSP\n"
		"	MOV    R1, #0\n"
	    /* Clear stack pointer limit to avoid double hardfault */
		"	MSR    msplim, R1\n"
		"	MSR    psplim, R1\n"
		"	MOV    R1, LR\n"
		"	B      HardFault_Handler_C"
	);
}


/* HardFault handler in C, with stack frame location and LR value extracted
 * from the assembly wrapper as input parameters */
void HardFault_Handler_C(unsigned long * hardfault_args, unsigned int lr_value)
{
    unsigned long stacked_r0;
    unsigned long stacked_r1;
    unsigned long stacked_r2;
    unsigned long stacked_r3;
    unsigned long stacked_r12;
    unsigned long stacked_lr;
    unsigned long stacked_pc;
    unsigned long stacked_psr;
    unsigned long cfsr;
    unsigned long bus_fault_address;
    unsigned long memmanage_fault_address;

    bus_fault_address       = SCB->BFAR;
    memmanage_fault_address = SCB->MMFAR;
    cfsr                    = SCB->CFSR;

    stacked_r0  = ((uint32_t) hardfault_args[0]);
    stacked_r1  = ((uint32_t) hardfault_args[1]);
    stacked_r2  = ((uint32_t) hardfault_args[2]);
    stacked_r3  = ((uint32_t) hardfault_args[3]);
    stacked_r12 = ((uint32_t) hardfault_args[4]);
    stacked_lr  = ((uint32_t) hardfault_args[5]);
    stacked_pc  = ((uint32_t) hardfault_args[6]);
    stacked_psr = ((uint32_t) hardfault_args[7]);

    printf ("[HardFault]\n");
    printf ("- Stack frame:\n");
    printf (" R0  = 0x%08lx\n", stacked_r0);
    printf (" R1  = 0x%08lx\n", stacked_r1);
    printf (" R2  = 0x%08lx\n", stacked_r2);
    printf (" R3  = 0x%08lx\n", stacked_r3);
    printf (" R12 = 0x%08lx\n", stacked_r12);
    printf (" LR  = 0x%08lx\n", stacked_lr);
    printf (" PC  = 0x%08lx\n", stacked_pc);
    printf (" PSR = 0x%08lx\n", stacked_psr);
    printf ("- FSR/FAR:\n");
    printf (" CFSR = 0x%08lx\n", cfsr);
    printf (" HFSR = 0x%08lx\n", SCB->HFSR);
    printf (" DFSR = 0x%08lx\n", SCB->DFSR);
    printf (" AFSR = 0x%08lx\n", SCB->AFSR);
    if (cfsr & 0x0080) printf (" MMFAR = 0x%08lx\n", memmanage_fault_address);
    if (cfsr & 0x8000) printf (" BFAR = 0x%08lx\n", bus_fault_address);
    printf ("- Misc\n");
    printf (" LR/EXC_RETURN= 0x%08x\n", lr_value);

    while(1); /* endless loop */
}
