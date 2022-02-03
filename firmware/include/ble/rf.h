/**
 ****************************************************************************************
 *
 * @file rf.h
 *
 * @brief Common header file for all radios.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef RF_H_
#define RF_H_

/**
 ****************************************************************************************
 * @addtogroup RF
 * @ingroup DRIVERS
 * @brief Common definitions for radio modules.
 *
 * This module declares the functions and constants that have to be defined for all RF.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>       // standard integer definition
#include <stdbool.h>      // standard boolean definition
#include "rf_icytrx_v2.h"

/*

 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

struct rwip_rf_api;  // forward declaration to avoid including rw.h

/**
 *****************************************************************************************
 * @brief Initialization of RF.
 *
 * This function initializes the RF and fills the structure containing the function
 * pointers and parameters required by the RW BT stack.
 *
 * @param[out]  api  Pointer to the BT RF API structure
 *
 *****************************************************************************************
 */
void rf_init(struct rwip_rf_api *api);


int8_t Device_RF_TxPwr_Get_dBm(uint8_t txpwr_idx);
uint8_t Device_RF_TxPwr_Get_Idx(int8_t txpwr_dbm);
int8_t Device_RF_RSSI_Convert(uint8_t rssi_reg);
uint8_t Device_RF_SetMaxPwrIdx(void);

/// @} RF

#endif // RF_H_
