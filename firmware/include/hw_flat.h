/**
 * @file hw_flat.h
 * @brief Unstructured hardware register abstraction layer for the SOC
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

#ifndef HW_FLAT_H_WRAPPER
#define HW_FLAT_H_WRAPPER

#ifndef MONTANA_CID
#error MONTANA_CID must be defined
#endif

#if MONTANA_CID == 101
#include <montana_hw_flat_cid101.h>
#else
#error Invalid MONTANA chip identifier
#endif

#endif /* HW_FLAT_H_WRAPPER */
