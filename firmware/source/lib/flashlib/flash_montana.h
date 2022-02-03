/**
 * @file flash_montana.h
 * @brief Montana hardware specific header file for the flash interface library
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

#ifndef FLASH_MONTANA_H_
#define FLASH_MONTANA_H_

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup FLASHg
 *  @{
 */

/** Total number of flash instances */
#define FLASH_INSTANCE_NUM                        0x2U

/** Total number of descriptor types on flash 0 region example( Code,Data,NVR etc.) */
#define FLASH_0_DESCR_NUM                         0x3U

/** Total number of descriptor types on flash 1 region example( Code,Data,NVR etc.) */
#define FLASH_1_DESCR_NUM                         FLASH_0_DESCR_NUM

/** @} */ /* End of the FLASHg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* FLASH_MONTANA_H_ */
