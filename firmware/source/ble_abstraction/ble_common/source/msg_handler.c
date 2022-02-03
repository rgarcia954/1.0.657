/**
 * @file msg_handler.c
 * @brief Source for the BLE Abstraction message handler
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

#include <msg_handler.h>
#include <stdlib.h>
#include <ble_gap.h>
#include <ble_gatt.h>
#include <rwip_task.h>
#include <co_utils.h>

typedef struct MsgHandler
{
    uint16_t msg_id;
    MsgHandlerCallback_t callback;
    struct MsgHandler *next;
} MsgHandler_t;

static MsgHandler_t *listHead = NULL;
static struct
{
    ke_msg_func_t gapc_handler;
    ke_msg_func_t gapm_handler;
    ke_msg_func_t gattc_handler;
    ke_msg_func_t gattm_handler;
} bleAbstractionHandlers = {
    (ke_msg_func_t)GAPC_MsgHandler,
    (ke_msg_func_t)GAPM_MsgHandler,
    (ke_msg_func_t)GATTC_MsgHandler,
    (ke_msg_func_t)GATTM_MsgHandler
};

/* Defines the place holder for the states of all the task instances. */
#ifndef APP_IDX_MAX
#define APP_IDX_MAX                     APP_MAX_NB_ACTIVITY
#endif    /* ifndef APP_IDX_MAX */
ke_state_t appm_state[APP_IDX_MAX];
KE_MSG_HANDLER_TAB(appm) {
    {
        KE_MSG_DEFAULT_HANDLER,    (ke_msg_func_t)MsgHandler_Notify
    }
};

/* Application task descriptor */
static const struct ke_task_desc TASK_DESC_APP = {
    appm_msg_handler_tab,
    appm_state, APP_IDX_MAX, ARRAY_LEN(appm_msg_handler_tab)
};

const struct ke_task_desc * MsgHandler_GetTaskAppDesc(void)
{
    return &TASK_DESC_APP;
}

bool MsgHandler_Add(ke_msg_id_t const msg_id, MsgHandlerCallback_t callback)
{
    if (callback == NULL)
    {
        return false;
    }

    MsgHandler_t *newElem = (MsgHandler_t *)malloc(sizeof(MsgHandler_t));
    if (!newElem)    /* malloc failed: insufficient memory */
    {
        return false;
    }

    newElem->msg_id = msg_id;
    newElem->callback = callback;
    newElem->next = NULL;

    if (!listHead)    /* If list is empty, newElem will be the new head */
    {
        listHead = newElem;
    }
    else
    {
        MsgHandler_t *tmp = listHead;

        /* Advance to the end of the list. Stops if find duplicate element */
        while (tmp->next && (tmp->msg_id != msg_id || tmp->callback != callback))
        {
            tmp = tmp->next;
        }
        if (tmp->msg_id == msg_id && tmp->callback == callback)    /* found a duplicated handler */
        {
            free(newElem);
            return false;
        }
        tmp->next = newElem;    /* Insert newElem at the end of the list */
    }

    return true;
}

int MsgHandler_Notify(ke_msg_id_t const msg_id, void const *param,
                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    MsgHandler_t *tmp = listHead;
    uint8_t task_id = KE_IDX_GET(msg_id);

    /* First notify abstraction layer handlers */
    switch (task_id)
    {
        case TASK_ID_GAPC:
        {
            bleAbstractionHandlers.gapc_handler(msg_id, param, dest_id, src_id);
        }
        break;

        case TASK_ID_GAPM:
        {
            bleAbstractionHandlers.gapm_handler(msg_id, param, dest_id, src_id);
        }
        break;

        case TASK_ID_GATTC:
        {
            bleAbstractionHandlers.gattc_handler(msg_id, param, dest_id, src_id);
        }
        break;

        case TASK_ID_GATTM:
        {
            bleAbstractionHandlers.gattm_handler(msg_id, param, dest_id, src_id);
        }
        break;
    }

    /* Notify subscribed application/profile handlers */
    while (tmp)
    {
        /* If message ID matches or the handler should be called for all
         * messages of this task type */
        if ((tmp->msg_id == msg_id) || (tmp->msg_id == task_id))
        {
            tmp->callback(msg_id, param, dest_id, src_id);
        }
        tmp = tmp->next;
    }
    return KE_MSG_CONSUMED;
}
