/**
 * @nfc_api.h
 * @brief Header file for NFC related A.P.I
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

#ifndef NFC_API_H_
#define NFC_API_H_

/**
 * @brief Initialize NFC hardware and global variables
 */
void NFC_Initialize(void);

/**
 * @brief Execute runtime NFC task
 */
void NFC_Task(void);

#endif    /* ifndef NFC_API_H_ */
