/**
 * @file app_customsc.h
 * @brief CUSTOMSC (Custom Service Client) header file
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

#ifndef APP_CUSTOMSC_H
#define APP_CUSTOMSC_H

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
#include <ble_gap.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* Custom service UUIDs */
#define CS_SVC_UUID                     { 0x24, 0xdc, 0x0e, 0x6e, 0x01, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHARACTERISTIC_TX_UUID       { 0x24, 0xdc, 0x0e, 0x6e, 0x02, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHARACTERISTIC_RX_UUID       { 0x24, 0xdc, 0x0e, 0x6e, 0x03, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHARACTERISTIC_TX_LONG_UUID  { 0x24, 0xdc, 0x0e, 0x6e, 0x04, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHARACTERISTIC_RX_LONG_UUID  { 0x24, 0xdc, 0x0e, 0x6e, 0x05, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }

enum custom_app_msg_id
{
    CUSTOMSC_TIMER = TASK_FIRST_MSG(TASK_ID_APP) + 65
};

/* Define the available custom service states */
enum cs_state
{
    CS_INIT,
    CS_SERVICE_DISCOVERD,
    CS_ALL_ATTS_DISCOVERED,
};

#define CS_TX_VALUE_MAX_LENGTH          20
#define CS_RX_VALUE_MAX_LENGTH          20
#define CS_TX_LONG_VALUE_MAX_LENGTH     40
#define CS_RX_LONG_VALUE_MAX_LENGTH     40

#define CS_CHARACTERISTICS_LIST         { CS_CHARACTERISTIC_TX_UUID, \
                                          CS_CHARACTERISTIC_RX_UUID, \
                                          CS_CHARACTERISTIC_TX_LONG_UUID, \
                                          CS_CHARACTERISTIC_RX_LONG_UUID };

enum cs_idx_att
{
    CS_IDX_TX_CHAR,            /**< TX Characteristic */

    CS_IDX_RX_CHAR,            /**< RX Characteristic */

    CS_IDX_TX_LONG_CHAR,       /**< TX Long Characteristic */

    CS_IDX_RX_LONG_CHAR,       /**< RX Long Characteristic */

    CS_IDX_NB,                 /**< Max number of characteristics */
};

/** Write Operations */
enum write_states
{
    WRITE_SINGLE = 0,
    WRITE_QUEUED,
};

/** Queued Write Operations */
enum queue_wr_states
{
    STATE_WRITE_QUEUE_STAGE1 = 0,
    STATE_WRITE_QUEUE_STAGE2,
    STATE_WRITE_EXECUTE,
};

/** Application processing settings */
#define CHAR_WRITE_INTERVAL_MS               300
#define READ_WRITE_LONG_INTERVAL_MS          1000
#define CUSTOMSC_TIMER_200MS_SETTING          20

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

struct discovered_char_att
{
    uint16_t attr_hdl;          /**< Database element handle */

    uint16_t pointer_hdl;       /**< Pointer attribute handle to UUID */

    uint8_t prop;               /**< Properties */

    uint8_t uuid_len;           /**< UUID length */

    uint8_t uuid[16];           /**< Characteristic UUID */
};

struct cs_env_tag
{
    uint16_t start_hdl;                             /**< The service start handle
    												     value in the database of
                                                         attributes in the stack */

    uint16_t end_hdl;                               /**< The service end handle
	                                                     value in the database of
                                                         attributes in the stack */

    uint8_t tx_value[CS_TX_VALUE_MAX_LENGTH];       /**< The value of TX
    												     characteristic value */

    uint8_t rx_value[CS_RX_VALUE_MAX_LENGTH];       /**< The value of RX
    												     characteristic value */

    bool gattc_write_complete;                      /**< A flag that indicates that
     	 	 	 	 	 	 	 	 	 	 	 	     PDU has been sent over the
     	 	 	 	 	 	 	 	 	 	 	 	     air */

    uint16_t cnt_notifc;                            /**< Custom service */

    uint8_t val_notif;                              /**< Custom service */

    uint8_t state;                                  /**< The state machine for
    												     service discovery */

    uint8_t disc_attnum;

    struct discovered_char_att disc_att[CS_IDX_NB];
};

/**
 * @brief       Initialize custom service client environment
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_Initialize(void);

/**
 * @brief       Send a write command or request to the client device
 * @param       conidx   Connection index
 * @param       value    Pointer to value
 * @param       handle   Attribute handle
 * @param       length   Length of value
 * @param       type     Type of write message
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_SendWrite(uint8_t conidx, uint8_t *value, uint16_t handle,
                       uint8_t offset, uint16_t length, uint8_t type);

/**
 * @brief       Send a prepare write request to the peer device
 * @param       conidx   Connection index
 * @param       value    Pointer to value
 * @param       handle   Attribute handle
 * @param       length   Length of value
 * @param       type     Type of write message
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_PrepareWrite(uint8_t conidx, uint8_t *value, uint16_t handle,
                          uint8_t offset, uint16_t length);

/**
 * @brief       Send an immediate write command of all queued data to the
 *              peer device
 * @param       conidx   Connection index
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_ExecWrite(uint8_t conidx);

/**
 * @brief       Send a read command to the characteristic handle
 * @param       conidx   Connection index
 * @param       handle   Attribute handle
 * @param       length   Length of value
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_ReadLong(uint8_t conidx, uint16_t handle,
                      uint8_t offset, uint16_t length);

/**
 * @brief       Execute Queued Write Operations
 * @return      Value indicating whether queued operations were completed.
 * 				1 if operation is active, 0 if completed.
 * @assumptions None
 */
uint8_t CUSTOMSC_QueuedWriteRun(void);

/**
 * @brief       Initialize queued write operation sequence
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_QueuedWriteInit(void);

/**
 * @brief       Write to custom characteristic handle on all slave devices
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_AppWriteCharSingle(void);

/**
 * @brief       Periodic timer that demonstrates CUSTOM service's usage
 * @return      None
 * @assumptions None
 */
void CUSTOMSC_Timer(void);

void CUSTOMSC_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                        ke_task_id_t const dest_id, ke_task_id_t const src_id);

/* ----------------------------------------------------------------------------
 *
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_CUSTOMSC_H */
