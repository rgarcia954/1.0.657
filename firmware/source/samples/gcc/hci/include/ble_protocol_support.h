/**
 * @file ble_protocol_support.h
 * @brief BLE protocol support layer header
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

#ifndef BLE_PROTOCOL_SUPPORT_H
#define BLE_PROTOCOL_SUPPORT_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <app.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#define HCI_VS_RF_CW_ENABLE_CMD_CODE    0x01
#define HCI_VS_RF_CW_DISABLE_CMD_CODE   0x02
#define HCI_VS_RF_OUTPUT_PWR_CMD_CODE   0x03

#define APP_HEAP_SIZE_DEFINED			0
#define APP_MAX_NB_CON                  10
#define APP_MAX_NB_ACTIVITY             11

#define APP_RWIP_HEAP_ENV_SIZE			(600 + (APP_MAX_NB_ACTIVITY) * 230) + \
                                        APP_MAX_NB_CON * ((sizeof(struct gapc_env_tag)  + KE_HEAP_MEM_RESERVED)    \
                                                           + (sizeof(struct gattc_env_tag)  + KE_HEAP_MEM_RESERVED)   \
                                                           + (sizeof(struct l2cc_env_tag)   + KE_HEAP_MEM_RESERVED))  \
                                         + ((APP_MAX_NB_ACTIVITY) * (sizeof(struct gapm_actv_scan_tag) + KE_HEAP_MEM_RESERVED))

#define APP_RWIP_HEAP_DB_SIZE			(3072)

#define APP_RWIP_HEAP_MSG_SIZE			(1650 + 2 * ((16 + (APP_MAX_NB_ACTIVITY - 1) * 56) + (58 + (APP_MAX_NB_ACTIVITY -1) * 26) + ((APP_MAX_NB_ACTIVITY) * 66)+ ((APP_MAX_NB_ACTIVITY) * 100)+ ((APP_MAX_NB_ACTIVITY) * 12))) + \
                                        (((BLEHL_HEAP_MSG_SIZE_PER_CON * APP_MAX_NB_CON) > BLEHL_HEAP_DATA_THP_SIZE) \
                                                        ? (BLEHL_HEAP_MSG_SIZE_PER_CON * APP_MAX_NB_CON) : BLEHL_HEAP_DATA_THP_SIZE) \

#define APP_RWIP_HEAP_NON_RET_SIZE	    (328*2)


#define APP_BLE_RAND_SEED_DEFINED		0
#define APP_BLE_RAND_GENERATE_DEFINED	0

/* ----------------------------------------------------------------------------
 * Function prototypes
 * ------------------------------------------------------------------------- */
uint8_t Device_BLE_Param_Get(uint8_t param_id, uint8_t *lengthPtr, uint8_t *buf);
int rand_func (void);
void srand_func (uint32_t seed);


/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_PROTOCOL_SUPPORT_H */
