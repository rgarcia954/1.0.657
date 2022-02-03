/**
 * @file msg_handler.h
 * @brief Header file for the BLE Abstraction message handler
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

#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <ke_task.h>
#include <stdbool.h>

/** @addtogroup BLE_ABSTRACTIONg
 *  @{
 */

/** BLE abstraction message handler function type */
typedef void (*MsgHandlerCallback_t)(ke_msg_id_t const msgid, void const *param,
                                     ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 * @brief Provide an application task descriptor with default handler
 *        set to the MsgHandler_Notify function
 *
 * @return A TASK_APP descriptor to be used in the ke_create_task function
 */
const struct ke_task_desc * MsgHandler_GetTaskAppDesc(void);

/**
 * @brief Add/subscribe a message handler callback function
 *
 * If the msg_id argument is a message identifier (such as GAPM_CMP_EVT), the
 * callback function is called only when the specific event is triggered <br>
 * If the msg_id argument is a task identifier (such as TASK_ID_GAPM), the
 * callback function is for for any event triggered from that task <br>
 *
 * @param [in] msg_id   A message identifier (such as GAPM_CMP_EVT) or a task identifier
 *                      (such as TASK_ID_GAPM)
 * @param [in] callback A pointer to the callback function that should be called when an
 *                      event matching msg_id happens
 * @return True if it was able to add/register the handler, false otherwise
 */
bool MsgHandler_Add(ke_msg_id_t const msg_id, MsgHandlerCallback_t callback);

/**
 * @brief Notify the callback functions associated with the msg_id
 *
 * This function searches the list of handlers added using MsgHandler_Add and
 * notify each one with matching msg_id. <br>
 * It also makes sure to call the BLE abstraction message handlers prior to
 * any application message handler
 *
 * @param [in] msg_id  A message identifier (such as GAPM_CMP_EVT) or a task identifier
 *                     (such as TASK_ID_GAPM)
 * @param [in] param   Message parameter
 * @param [in] dest_id Destination task identifier
 * @param [in] src_id  Source task identifier
 * @return KE_MSG_CONSUMED back to the kernel
 * @note This function was designed to be used as the default handler of the
 *       kernel and shall NOT be called directly by the application. To notify
 *       an event, the application should instead enqueue a message in the
 *       kernel, in order to avoid chaining the context of function calls
 *       and causing a stack overflow
 */
int MsgHandler_Notify(ke_msg_id_t const msg_id, void const *param, ke_task_id_t
                      const dest_id, ke_task_id_t const src_id);

/** @} */ /* End of the BLE_ABSTRACTIONg group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* MSG_HANDLER_H */
