/**
 ****************************************************************************************
 *
 * @file ll.h
 *
 * @brief Declaration of low level functions.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef LL_H_
#define LL_H_

#include <stdint.h>

extern uint32_t critical_sec_cnt;

/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */
#define GLOBAL_INT_START()  asm volatile ("MSR primask, %0" : : "r" (0) );

/** @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 */
#define GLOBAL_INT_STOP()  asm volatile ("MSR primask, %0" : : "r" (1) );

/** @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 */

#define GLOBAL_INT_DISABLE();                                               \
do {                                                                        \
    uint32_t __primask_status;                                              \
    asm volatile ("MRS %0, primask" : "=r" (__primask_status) );            \
    asm volatile ("MSR primask, %0" : : "r" (1) );                          \

#define GLOBAL_INT_RESTORE();                                               \
    asm volatile ("MSR primask, %0" : : "r" (__primask_status) );           \
} while(0)

/** @brief Invoke the wait for interrupt procedure of the processor.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *
 */
#define WFI()  asm volatile ("wfi");

#endif // LL_H_

