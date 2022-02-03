/**
 * @file blinky.h
 * @brief Header file providing the Blinky interface.
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

#ifndef BLINKY_INITIALIZE_H_
#define BLINKY_INITIALIZE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialization routine.
 * @param [in] gpio  The GPIO pin to use for blinky.
 * @param [in] delay The delay in milliseconds between state changes.
 */
void blinkyInitialize(uint32_t gpio, uint32_t delay);

/**
 * @brief Helper function to allow the main application to check the current
 * state of the Blinky pin.
 * @return True of the GPIO pin is high, false otherwise.
 */
bool isLedOn(void);

#endif /* BLINKY_INITIALIZE_H_ */
