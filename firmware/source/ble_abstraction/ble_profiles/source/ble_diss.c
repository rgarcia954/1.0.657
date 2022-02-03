/**
 * @file ble_diss.c
 * @brief Bluetooth device information server service abstraction source file
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

#include <ble_diss.h>
#include <ble_gap.h>
#include <msg_handler.h>
#include <prf_utils.h>

/** Global variable definition */
static DISS_Env_t diss_env;

int DISS_Initialize(uint16_t features, const struct DISS_DeviceInfo_t *deviceInfo)
{
    memset(&diss_env, 0, sizeof(DISS_Env_t));

    diss_env.features = features;
    diss_env.deviceInfo = deviceInfo;

    /*
     * Add DISS message handlers
     */
    MsgHandler_Add(TASK_ID_DISS,            DISS_MsgHandler);
    MsgHandler_Add(GAPM_PROFILE_ADDED_IND,  DISS_MsgHandler);
    MsgHandler_Add(DISS_SET_VALUE_RSP,      DISS_MsgHandler);
    MsgHandler_Add(DISS_VALUE_REQ_IND,      DISS_DeviceInfoValueReqInd);

    return 0;
}

void DISS_DeviceInfoValueReqInd(ke_msg_id_t const msgid, void const *param,
                                ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct diss_value_req_ind const *p = param;
    const struct DISS_DeviceInfo_t *deviceInfo = diss_env.deviceInfo;

    /* Initialize length */
    uint8_t len = 0;

    /* Pointer to the data */
    uint8_t *data;

    /* Check requested value */
    switch (p->value)
    {
        case DIS_MANUFACTURER_NAME_CHAR:
        {
            /* Set information */
            len = deviceInfo->MANUFACTURER_NAME.len;
            data = deviceInfo->MANUFACTURER_NAME.data;
        }
        break;

        case DIS_MODEL_NB_STR_CHAR:
        {
            /* Set information */
            len = deviceInfo->MODEL_NB_STR.len;
            data = deviceInfo->MODEL_NB_STR.data;
        }
        break;

        case DIS_SYSTEM_ID_CHAR:
        {
            /* Set information */
            len = deviceInfo->SYSTEM_ID.len;
            data = deviceInfo->SYSTEM_ID.data;
        }
        break;

        case DIS_PNP_ID_CHAR:
        {
            /* Set information */
            len = deviceInfo->PNP.len;
            data = deviceInfo->PNP.data;
        }
        break;

        case DIS_SERIAL_NB_STR_CHAR:
        {
            /* Set information */
            len = deviceInfo->SERIAL_NB_STR.len;
            data = deviceInfo->SERIAL_NB_STR.data;
        }
        break;

        case DIS_HARD_REV_STR_CHAR:
        {
            /* Set information */
            len = deviceInfo->HARD_REV_STR.len;
            data = deviceInfo->HARD_REV_STR.data;
        }
        break;

        case DIS_FIRM_REV_STR_CHAR:
        {
            /* Set information */
            len = deviceInfo->FIRM_REV_STR.len;
            data = deviceInfo->FIRM_REV_STR.data;
        }
        break;

        case DIS_SW_REV_STR_CHAR:
        {
            /* Set information */
            len = deviceInfo->SW_REV_STR.len;
            data = deviceInfo->SW_REV_STR.data;
        }
        break;

        case DIS_IEEE_CHAR:
        {
            /* Set information */
            len = deviceInfo->IEEE.len;
            data = deviceInfo->IEEE.data;
        }
        break;

        default:
        {
        }
        break;
    }

    /* Allocate confirmation to send the value */
    struct diss_value_cfm *cfm_value = KE_MSG_ALLOC_DYN(DISS_VALUE_CFM, src_id,
                                                        dest_id, diss_value_cfm, len);

    /* Set parameters */
    cfm_value->value = p->value;
    cfm_value->length = len;
    if (len)
    {
        /* Copy data */
        memcpy(&cfm_value->data[0], data, len);
    }

    /* Send message */
    ke_msg_send(cfm_value);
}

void DISS_ProfileTaskAddCmd(void)
{
    struct diss_db_cfg db_cfg;

    /** Configure DISS database parameters  */
    db_cfg.features = diss_env.features;

    /** Add DISS profile task to the stack. A GAPM_PROFILE_ADDED_IND event
     * is generated when finished. */
    GAPM_ProfileTaskAddCmd(PERM(SVC_AUTH, DISABLE), TASK_ID_DISS, TASK_APP,
                           0, (uint8_t *)&db_cfg, sizeof(struct diss_db_cfg));
}

const DISS_Env_t * DISS_EnvGet(void)
{
    return &diss_env;
}

bool DISS_IsAdded(void)
{
    return diss_env.serviceAdded;
}

void DISS_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    switch (msg_id)
    {
        case GAPM_PROFILE_ADDED_IND:
        {
            struct gapm_profile_added_ind *ind = (struct gapm_profile_added_ind *)param;
            if (ind->prf_task_id == TASK_ID_DISS)
            {
                diss_env.serviceAdded = true;
            }
        }
        break;

        case DISS_SET_VALUE_RSP:
        {
        }
        break;

        default:
        {
        }
        break;
    }
}
