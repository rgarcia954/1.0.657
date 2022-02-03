/**
 * @ file sbrk.c
 * @ brief Implementation of the sbrk system function
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

#include <hw.h>

/* Define a standard failure report */
#define SBRK_FAILURE                    ((int8_t *) -1)

/* SBRK will work on four byte aligned data */
#define SBRK_ALIGNMENT                  (4)

/* Offset is used when ensuring alignment */
#define SBRK_OFFSET                     (SBRK_ALIGNMENT - 1)

/* Mask to select only the 4-byte aligned values */
#define SBRK_MASK                       (~SBRK_OFFSET)

/**
 * @brief       Increment (or decrement) the top of the heap
 * @param[in]   increment    Increment to be applied to the top of the heap
 * @return      The prior value of the heap top (points to the base of the 
 *              newly allocated data if the heap was incremented); returns
 *              -1 if the function was unable to allocate the requested 
 *              memory
 * @assumptions The symbols __Heap_Begin__, __Heap_Limit__ are defined
 *              when the application is linked.
 */
int8_t * _sbrk(int increment)
{
    /* Use of int8_t here ensures we always calculate in bytes (heapTop could
     * also be defined as a char or "void *" type)
     */
    static uint8_t *heapTop;
    int8_t * result;

    /* Ensure that heapTop has been initialized */
    if (heapTop == NULL)
    {
        heapTop = &__Heap_Begin__;
    }

    /* Modify the increment to ensure the memory allocated is word aligned */
    if (increment > 0)
    {
        increment = (increment + SBRK_OFFSET) & SBRK_MASK;
        if ((heapTop + increment) > &__Heap_Limit__)
        {
            /* Flag an error in allocating memory (no space) and return */
            errno = ENOMEM;
            return SBRK_FAILURE;
        }
    }
    else if (increment < 0)
    {
        increment = (increment - SBRK_OFFSET) & SBRK_MASK;
        if ((heapTop + increment) < (uint8_t *) &__Heap_Begin__)
        {
            /* Flag an error in allocating memory (deallocating too much
             * space) and return */
            errno = ENOMEM;
            return SBRK_FAILURE;
        }
    }

    /* We can allocate the memory, so update the top of the heap and return
     * a pointer to the previous heap top */
    result = (int8_t *)heapTop;
    heapTop = heapTop + increment;
    return result;
}
