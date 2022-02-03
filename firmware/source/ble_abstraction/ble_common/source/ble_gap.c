/**
 * @file ble_gap.c
 * @brief BLE Abstraction GAP layer source
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

#include <ble_gap.h>
#include <ble_gatt.h>
#include <string.h>
#include <co_bt_defines.h>
#include <co_error.h>
#include <ke_msg.h>

static uint8_t GAPM_FirstAvailableActivitySlot(void);

static uint8_t GAPM_GetActivitySlot(uint8_t actv_idx);

static GAPM_ActivityStatus_t * GAPM_GetActivityStatus(uint8_t actv_idx);

static void GAPM_AirOperationsMsgHandler(ke_msg_id_t const msg_id,
                                         void const *param, ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id);

static void GAPM_ActivitiesStopped(void);

static void GAPM_ActivitiesDeleted(void);

static bool GAPM_ActivityCreateCmd(GAPM_ActivityStatus_t *actv_status,
                                   uint8_t operation, enum gapm_own_addr own_addr_type);

static uint8_t GAPC_ConnectionRoleCount(uint8_t role);

void GAP_Initialize(void);

/** GAP Environment Structure */
static GAP_Env_t gap_env = { .gapmState = GAPM_STATE_INITIAL };

struct ble_whitelist_info whitelist_info;

void GAP_Initialize(void)
{
    memset(&gap_env, 0, sizeof(GAP_Env_t));
    gap_env.gapmState = GAPM_STATE_RESET;

    for (uint8_t i = 0; i < APP_MAX_NB_CON; i++)
    {
        gap_env.connection[i].conhdl = GAP_INVALID_CONHDL;
    }

    for (uint8_t i = 0; i < APP_MAX_NB_PROFILES; i++)
    {
    	gap_env.profileAddedTaskId[i] = 0;
    }
}

const GAP_Env_t * GAP_GetEnv(void)
{
    return &gap_env;
}

uint16_t * GAP_GetProfileAddedTaskId(void)
{
	return gap_env.profileAddedTaskId;
}

bool GAP_IsAddrPrivateResolvable(const uint8_t *addr, uint8_t addrType)
{
    return ((addrType == GAPM_GEN_RSLV_ADDR)
            && ((addr[GAP_BD_ADDR_LEN - 1] & 0xc0) == BD_ADDR_RSLV));
}

bool GAP_AddAdvData(uint8_t newDataLen, enum gap_ad_type newDataFlag,
                    const uint8_t *newData, uint8_t *resultAdvData, uint8_t *resultAdvDataLen)
{
    /* If new data element cannot be added to current accumulated data */
    if ((ADV_DATA_LEN - *resultAdvDataLen) < (newDataLen + 1))
    {
        return false;
    }

    /* Else, add new data element into current accumulated data */
    resultAdvData[*resultAdvDataLen] = newDataLen;
    resultAdvData[*resultAdvDataLen + 1] = newDataFlag;
    memcpy(resultAdvData + *resultAdvDataLen + 2, newData, newDataLen - 1);

    *resultAdvDataLen += newDataLen + 1;
    return true;
}

void GAPM_ResetCmd(enum gapm_operation operation)
{
    struct gapm_reset_cmd *cmd;

    cmd = KE_MSG_ALLOC(GAPM_RESET_CMD, TASK_GAPM, TASK_APP, gapm_reset_cmd);
    cmd->operation = operation;
    ke_msg_send(cmd);
}

void GAPM_SoftwareReset(void)
{
    GAPM_ResetCmd(GAPM_RESET);
}

void GAPM_PlatformReset(void)
{
    GAPM_ResetCmd(GAPM_PLF_RESET);
}

void GAPM_SetDevConfigCmd(const struct gapm_set_dev_config_cmd *deviceConfig)
{
    struct gapm_set_dev_config_cmd *cmd;
    cmd = KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD, TASK_GAPM, TASK_APP,
                       gapm_set_dev_config_cmd);

    /* Save a local copy of the configuration */
    memcpy(&gap_env.deviceConfig, deviceConfig,
           sizeof(struct gapm_set_dev_config_cmd));
    gap_env.deviceConfig.operation = GAPM_SET_DEV_CONFIG;

    /* Send configuration to stack */
    memcpy(cmd, &gap_env.deviceConfig, sizeof(struct gapm_set_dev_config_cmd));

    ke_msg_send(cmd);
}

const struct gapm_set_dev_config_cmd * GAPM_GetDeviceConfig(void)
{
    return &gap_env.deviceConfig;
}

void GAPM_ProfileTaskAddCmd(uint8_t sec_lvl, uint16_t prf_task_id,
                            uint16_t app_task, uint16_t start_hdl, uint8_t *param,
                            uint32_t paramSize)
{
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(
        GAPM_PROFILE_TASK_ADD_CMD, TASK_GAPM, app_task,
        gapm_profile_task_add_cmd, paramSize)
    ;

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = sec_lvl;
    req->prf_task_id = prf_task_id;
    req->app_task = app_task;
    req->start_hdl = start_hdl;

    if (param && paramSize)
    {
        memcpy(req->param, param, paramSize);
    }

    ke_msg_send(req);
}

uint16_t GAPM_GetProfileAddedCount(void)
{
    return gap_env.profileAddedCount;
}

void GAPM_LepsmRegisterCmd(uint16_t le_psm, uint16_t app_task, uint8_t sec_lvl)
{
    struct gapm_lepsm_register_cmd *coc_cmd = KE_MSG_ALLOC(
        GAPM_LEPSM_REGISTER_CMD, TASK_GAPM, TASK_APP,
        gapm_lepsm_register_cmd);

    coc_cmd->app_task = TASK_APP;
    coc_cmd->le_psm = le_psm;
    coc_cmd->operation = GAPM_LEPSM_REG;
    coc_cmd->sec_lvl = sec_lvl;

    ke_msg_send(coc_cmd);
}

void GAPM_GenRandAddrCmd(enum random_addr_type rnd_type)
{
    struct gapm_gen_rand_addr_cmd *genRandAddrCmd = KE_MSG_ALLOC(
        GAPM_GEN_RAND_ADDR_CMD, TASK_GAPM, TASK_APP,
        gapm_gen_rand_addr_cmd);

    genRandAddrCmd->operation = GAPM_GEN_RAND_ADDR;
    genRandAddrCmd->rnd_type = rnd_type;

    ke_msg_send(genRandAddrCmd);
}

void GAPM_ResolvAddrCmd(uint8_t conidx, const uint8_t *peerAddr)
{
    struct gapm_resolv_addr_cmd *cmd;

    uint8_t irkListSize = BondList_Size();

    if (irkListSize == 0)
    {
        return;
    }

    cmd = KE_MSG_ALLOC_DYN(GAPM_RESOLV_ADDR_CMD, TASK_GAPM,
                           KE_BUILD_ID(TASK_APP, conidx), gapm_resolv_addr_cmd,
                           sizeof(struct gap_sec_key) * irkListSize);

    BondList_GetIRKs(cmd->irk);

    cmd->operation = GAPM_RESOLV_ADDR;
    cmd->nb_key = irkListSize;
    memcpy(cmd->addr.addr, peerAddr, GAP_BD_ADDR_LEN);

    ke_msg_send(cmd);
}

static uint8_t GAPM_FirstAvailableActivitySlot(void)
{
    uint8_t i;
    for (i = 0; i < APP_MAX_NB_ACTIVITY; i++)
    {
        if (gap_env.actv[i] == NULL)
        {
            return i;
        }
    }
    return APP_MAX_NB_ACTIVITY;
}

static GAPM_ActivityStatus_t * GAPM_GetActivityStatus(uint8_t actv_idx)
{
    uint8_t i;
    for (i = 0; i < APP_MAX_NB_ACTIVITY; i++)
    {
        if (gap_env.actv[i] && gap_env.actv[i]->state != ACTIVITY_STATE_NOT_CREATED
            && gap_env.actv[i]->actv_idx == actv_idx)
        {
            return gap_env.actv[i];
        }
    }
    return NULL;
}

static uint8_t GAPM_GetActivitySlot(uint8_t actv_idx)
{
    uint8_t i;
    for (i = 0; i < APP_MAX_NB_ACTIVITY; i++)
    {
        if (gap_env.actv[i] && gap_env.actv[i] != ACTIVITY_STATE_NOT_CREATED
            && gap_env.actv[i]->actv_idx == actv_idx)
        {
            return i;
        }
    }
    return APP_MAX_NB_ACTIVITY;
}

bool GAPM_ActivityCreateAdvCmd(GAPM_ActivityStatus_t *actv_status,
                               enum gapm_own_addr own_addr_type,
                               const struct gapm_adv_create_param *adv_param)
{
    struct gapm_activity_create_adv_cmd *cmd;
    actv_status->state = ACTIVITY_STATE_NOT_CREATED;
    actv_status->type = GAPM_ACTV_TYPE_ADV;
    actv_status->advDataSet = false;
    actv_status->scanRspDataSet = false;

    uint8_t actv_slot = GAPM_FirstAvailableActivitySlot();
    if (actv_slot >= APP_MAX_NB_ACTIVITY)
    {
        return false;    /* no slot available */
    }
    gap_env.actv[actv_slot] = actv_status;

    actv_status->state = ACTIVITY_STATE_CREATING;

    /* Here we use (TASK_APP,actv_slot) as source id so that we can
     * identify the activity when receiving a GAPM_CMP_EVT */
    cmd = KE_MSG_ALLOC_DYN(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM,
                           KE_BUILD_ID(TASK_APP, actv_slot), gapm_activity_create_adv_cmd,
                           adv_param->period_cfg.switching_pattern_len);

    cmd->operation = GAPM_CREATE_ADV_ACTIVITY;
    cmd->own_addr_type = own_addr_type;
    if (adv_param)
    {
        memcpy(&cmd->adv_param, adv_param,
               sizeof(struct gapm_adv_create_param) + adv_param->period_cfg.switching_pattern_len);
    }

    ke_msg_send(cmd);

    return true;
}

bool GAPM_ActivityCreateScanCmd(GAPM_ActivityStatus_t *actv_status,
                                enum gapm_own_addr own_addr_type)
{
    return GAPM_ActivityCreateCmd(actv_status, GAPM_CREATE_SCAN_ACTIVITY,
                                  own_addr_type);
}

bool GAPM_ActivityCreateInitCmd(GAPM_ActivityStatus_t *actv_status,
                                enum gapm_own_addr own_addr_type)
{
    return GAPM_ActivityCreateCmd(actv_status, GAPM_CREATE_INIT_ACTIVITY,
                                  own_addr_type);
}

bool GAPM_ActivityCreatePeriodSyncCmd(GAPM_ActivityStatus_t *actv_status,
                                      enum gapm_own_addr own_addr_type)
{
    return GAPM_ActivityCreateCmd(actv_status, GAPM_CREATE_PERIOD_SYNC_ACTIVITY,
                                  own_addr_type);
}

static bool GAPM_ActivityCreateCmd(GAPM_ActivityStatus_t *actv_status,
                                   uint8_t operation, enum gapm_own_addr own_addr_type)
{
    struct gapm_activity_create_cmd *cmd;
    actv_status->state = ACTIVITY_STATE_NOT_CREATED;
    actv_status->advDataSet = false;
    actv_status->scanRspDataSet = false;

    uint8_t actv_slot = GAPM_FirstAvailableActivitySlot();
    if (actv_slot >= APP_MAX_NB_ACTIVITY)
    {
        return false;    /* no slot available */
    }
    gap_env.actv[actv_slot] = actv_status;

    switch (operation)
    {
        case GAPM_CREATE_SCAN_ACTIVITY:
        {
            actv_status->type = GAPM_ACTV_TYPE_SCAN;
        }
        break;

        case GAPM_CREATE_INIT_ACTIVITY:
        {
            actv_status->type = GAPM_ACTV_TYPE_INIT;
        }
        break;

        case GAPM_CREATE_PERIOD_SYNC_ACTIVITY:
        {
            actv_status->type = GAPM_ACTV_TYPE_PER_SYNC;
        }
        break;

        default:    /* Invalid operation */
        {
            return false;
        }
    }

    actv_status->state = ACTIVITY_STATE_CREATING;

    /* Here we use (TASK_APP,actv_slot) as source id so that we can
     * identify the activity when receiving a GAPM_CMP_EVT */
    cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM,
                       KE_BUILD_ID(TASK_APP, actv_slot), gapm_activity_create_cmd);
    cmd->operation = operation;
    cmd->own_addr_type = own_addr_type;

    ke_msg_send(cmd);
    return true;
}

bool GAPM_AdvActivityStart(uint8_t actv_idx, uint16_t duration, uint8_t max_adv_evt)
{
    union gapm_u_param u_param;
    u_param.adv_add_param.duration = duration;
    u_param.adv_add_param.max_adv_evt = max_adv_evt;
    return GAPM_ActivityStartCmd(actv_idx, &u_param);
}

bool GAPM_InitActivityStart(uint8_t actv_idx, struct gapm_init_param *initParam)
{
    union gapm_u_param u_param;
    memcpy(&u_param.init_param, initParam, sizeof(struct gapm_init_param));
    return GAPM_ActivityStartCmd(actv_idx, &u_param);
}

bool GAPM_ScanActivityStart(uint8_t actv_idx, struct gapm_scan_param *scanParam)
{
    union gapm_u_param u_param;
    memcpy(&u_param.scan_param, scanParam, sizeof(struct gapm_scan_param));
    return GAPM_ActivityStartCmd(actv_idx, &u_param);
}

bool GAPM_PerSyncActivityStart(uint8_t actv_idx, struct gapm_per_sync_param *perSyncParam)
{
    union gapm_u_param u_param;
    memcpy(&u_param.per_sync_param, perSyncParam, sizeof(struct gapm_per_sync_param));
    return GAPM_ActivityStartCmd(actv_idx, &u_param);
}

bool GAPM_ActivityStartCmd(uint8_t actv_idx, union gapm_u_param *u_param)
{
    struct gapm_activity_start_cmd *cmd;
    uint8_t actv_slot = GAPM_GetActivitySlot(actv_idx);

    /* actv_idx not found or u_param == NULL */
    if (actv_slot == APP_MAX_NB_ACTIVITY || u_param == NULL)
    {
        return false;
    }

    gap_env.actv[actv_slot]->state = ACTIVITY_STATE_STARTING;

    cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM,
                       KE_BUILD_ID(TASK_APP, actv_slot), gapm_activity_start_cmd);
    cmd->operation = GAPM_START_ACTIVITY;
    cmd->actv_idx = actv_idx;

    memcpy(&cmd->u_param, u_param, sizeof(union gapm_u_param));

    ke_msg_send(cmd);
    return true;
}

bool GAPM_ActivityStop(uint8_t actv_idx)
{
    return GAPM_ActivityStopCmd(GAPM_STOP_ACTIVITY, actv_idx);
}

bool GAPM_ActivityStopAll(void)
{
    return GAPM_ActivityStopCmd(GAPM_STOP_ALL_ACTIVITIES, 0);
}

bool GAPM_DeleteActivity(uint8_t actv_idx)
{
    return GAPM_DeleteActivityCmd(GAPM_DELETE_ACTIVITY, actv_idx);
}

bool GAPM_DeleteAllActivities(void)
{
    return GAPM_DeleteActivityCmd(GAPM_DELETE_ALL_ACTIVITIES, 0);
}

bool GAPM_ActivityStopCmd(uint8_t operation, uint8_t actv_idx)
{
    struct gapm_activity_stop_cmd *cmd;

    if (!GAPM_GetActivityStatus(actv_idx))    /* Activity doesn't exist */
    {
        return false;
    }

    cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD, TASK_GAPM, TASK_APP,
                       gapm_activity_stop_cmd);
    cmd->operation = operation;
    cmd->actv_idx = actv_idx;
    ke_msg_send(cmd);
    return true;
}

bool GAPM_DeleteActivityCmd(uint8_t operation, uint8_t actv_idx)
{
    struct gapm_activity_delete_cmd *cmd;

    if (!GAPM_GetActivityStatus(actv_idx))    /* Activity doesn't exist */
    {
        return false;
    }

    cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD, TASK_GAPM,
                       TASK_APP, gapm_activity_delete_cmd);

    cmd->operation = operation;
    cmd->actv_idx = actv_idx;

    /* Send the message */
    ke_msg_send(cmd);
    return true;
}

static void GAPM_AirOperationsMsgHandler(ke_msg_id_t const msg_id,
                                         void const *param, ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    uint8_t actv_slot = KE_IDX_GET(dest_id);
    if (msg_id == GAPM_CMP_EVT)
    {
        const struct gapm_cmp_evt *p = (const struct gapm_cmp_evt *)param;
        if (p->status != GAP_ERR_NO_ERROR)
        {
            if (p->operation == GAPM_CREATE_ADV_ACTIVITY)
            {
                if (gap_env.actv[actv_slot])
                {
                    gap_env.actv[actv_slot]->state = ACTIVITY_STATE_NOT_CREATED;
                    gap_env.actv[actv_slot] = NULL;
                }
            }
        }
        else    /* p->status == GAP_ERR_NO_ERROR */
        {
            if (p->operation == GAPM_START_ACTIVITY && gap_env.actv[actv_slot])
            {
                gap_env.actv[actv_slot]->state = ACTIVITY_STATE_STARTED;
            }
            else if (p->operation == GAPM_STOP_ALL_ACTIVITIES)
            {
                GAPM_ActivitiesStopped();
            }
            else if (p->operation == GAPM_DELETE_ACTIVITY
                     && gap_env.actv[actv_slot])
            {
                gap_env.actv[actv_slot]->state = ACTIVITY_STATE_NOT_CREATED;
                gap_env.actv[actv_slot] = NULL;
            }
            else if (p->operation == GAPM_DELETE_ALL_ACTIVITIES)
            {
                GAPM_ActivitiesDeleted();
            }
            else if ((p->operation == GAPM_SET_ADV_DATA
                      || p->operation == GAPM_SET_PERIOD_ADV_DATA)
                     && gap_env.actv[actv_slot])
            {
                gap_env.actv[actv_slot]->advDataSet = true;
            }
            else if (p->operation == GAPM_SET_SCAN_RSP_DATA && gap_env.actv[actv_slot])
            {
                gap_env.actv[actv_slot]->scanRspDataSet = true;
            }
        }
    }
    else if (msg_id == GAPM_ACTIVITY_CREATED_IND)
    {
        const struct gapm_activity_created_ind *p =
            (const struct gapm_activity_created_ind *)param;
        if (gap_env.actv[actv_slot])
        {
            gap_env.actv[actv_slot]->actv_idx = p->actv_idx;
            gap_env.actv[actv_slot]->state = ACTIVITY_STATE_NOT_STARTED;
        }
        else
        {
            /* Should not happen. If it happens, the user has called GAPM_ActivityCreateCmd directly without using BLE
             * Abstraction API */
        }
    }
    else if (msg_id == GAPM_ACTIVITY_STOPPED_IND)
    {
        const struct gapm_activity_stopped_ind *p =
            (const struct gapm_activity_stopped_ind *)param;
        GAPM_ActivityStatus_t *actv_status = GAPM_GetActivityStatus(p->actv_idx);
        if (actv_status)
        {
            actv_status->state = ACTIVITY_STATE_NOT_STARTED;
        }
        else
        {
            /* Should not happen. If it happens, the user has called GAPM_ActivityCreateCmd directly without using BLE
             * Abstraction API */
        }
    }
}

bool GAPM_SetAdvDataCmd(uint8_t operation, uint8_t actv_idx, uint8_t length,
                        uint8_t *data)
{
    struct gapm_set_adv_data_cmd *cmd;

    uint8_t actv_slot = GAPM_GetActivitySlot(actv_idx);
    if (actv_slot == APP_MAX_NB_ACTIVITY)    /* actv_idx not found */
    {
        return false;
    }

    cmd = KE_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD, TASK_GAPM,
                           KE_BUILD_ID(TASK_APP, actv_slot), gapm_set_adv_data_cmd, length);
    cmd->operation = operation;
    cmd->actv_idx = actv_idx;
    cmd->length = length;
    if (data)
    {
        memcpy(&cmd->data, data, length);
    }

    ke_msg_send(cmd);
    return true;
}

static void GAPM_ActivitiesStopped(void)
{
    uint8_t i;
    for (i = 0; i < APP_MAX_NB_ACTIVITY; i++)
    {
        if (gap_env.actv[i] && gap_env.actv[i]->state == ACTIVITY_STATE_STARTED)
        {
            gap_env.actv[i]->state = ACTIVITY_STATE_NOT_STARTED;
        }
    }
}

static void GAPM_ActivitiesDeleted(void)
{
    uint8_t i;
    for (i = 0; i < APP_MAX_NB_ACTIVITY; i++)
    {
        if (gap_env.actv[i])
        {
            gap_env.actv[i]->state = ACTIVITY_STATE_NOT_CREATED;
            gap_env.actv[i] = NULL;
        }
    }
}

void GAPM_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    switch (msg_id)
    {
        case GAPM_CMP_EVT:
        {
            const struct gapm_cmp_evt *p = (const struct gapm_cmp_evt *)param;
            if (p->operation == GAPM_RESET)
            {
                GAP_Initialize();
                GATT_Initialize();
            }
            else if (p->operation == GAPM_SET_DEV_CONFIG
                     && p->status == GAP_ERR_NO_ERROR)
            {
                gap_env.gapmState = GAPM_STATE_READY;

                /* Reset svc_att_db_idx to ensure that att_db in GATT environment points
                 * to correct database before adding first custom service*/
                GATTM_ResetServiceAttributeDatabaseID();
            }

            /* If it's related to an air operation */
            else if (p->operation >= GAPM_CREATE_ADV_ACTIVITY
                     && p->operation <= GAPM_PER_ADV_CTE_TX_CTL)
            {
                GAPM_AirOperationsMsgHandler(msg_id, param, dest_id, src_id);
            }
        }
        break;

        case GAPM_ACTIVITY_CREATED_IND:
        case GAPM_ACTIVITY_STOPPED_IND:
        {
            GAPM_AirOperationsMsgHandler(msg_id, param, dest_id, src_id);
        }
        break;

        case GAPM_PROFILE_ADDED_IND:
        {
        	const struct gapm_profile_added_ind *p = param;
        	if(gap_env.profileAddedCount < APP_MAX_NB_PROFILES)
        	{
        		gap_env.profileAddedTaskId[gap_env.profileAddedCount] = p->prf_task_id;
        	}
            gap_env.profileAddedCount++;
        }
        break;

        case GAPM_ADDR_SOLVED_IND:    /* Event triggered when private address resolution was successful */
        {
            const struct gapm_addr_solved_ind *p =
                (const struct gapm_addr_solved_ind *)param;
            const BondInfo_t *bondInfo = BondList_FindByIRK(p->irk.key);
            uint8_t conidx = KE_IDX_GET(dest_id);
            if (bondInfo)    /* Found bond info. Copy it into environment variable */
            {
                memcpy(&gap_env.bondInfo[conidx], bondInfo, sizeof(BondInfo_t));
            }
        }
        break;
    }
}

void GAPC_ParamUpdateCmd(uint8_t conidx, uint16_t intv_min, uint16_t intv_max,
                         uint16_t latency, uint16_t time_out, uint16_t ce_len_min,
                         uint16_t ce_len_max)
{
    struct gapc_param_update_cmd *cmd = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_param_update_cmd);

    cmd->operation = GAPC_UPDATE_PARAMS;
    cmd->intv_min = intv_min;
    cmd->intv_max = intv_max;
    cmd->latency = latency;
    cmd->time_out = time_out;

    /* Not used by a slave device */
    cmd->ce_len_min = ce_len_min;
    cmd->ce_len_max = ce_len_max;

    /* Send the message */
    ke_msg_send(cmd);
}

void GAPC_ParamUpdateCfm(uint8_t conidx, bool accept, uint16_t ce_len_min,
                         uint16_t ce_len_max)
{
    struct gapc_param_update_cfm *cfm;

    cfm = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM, KE_BUILD_ID(TASK_GAPC, conidx),
                       TASK_APP, gapc_param_update_cfm);

    cfm->accept = accept;
    cfm->ce_len_min = ce_len_min;
    cfm->ce_len_max = ce_len_max;

    ke_msg_send(cfm);
}

void GAPC_ConnectionCfm(uint8_t conidx, struct gapc_connection_cfm *param)
{
    struct gapc_connection_cfm *cfm;

    cfm = KE_MSG_ALLOC(GAPC_CONNECTION_CFM, KE_BUILD_ID(TASK_GAPC, conidx),
                       TASK_APP, gapc_connection_cfm);

    memcpy(cfm, param, sizeof(struct gapc_connection_cfm));

    ke_msg_send(cfm);
}

void GAPC_DisconnectCmd(uint8_t conidx, uint8_t reason)
{
    struct gapc_disconnect_cmd *cmd;

    /* Allocate a disconnection command message */
    cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD, KE_BUILD_ID(TASK_GAPC, conidx),
                       TASK_APP, gapc_disconnect_cmd);
    cmd->operation = GAPC_DISCONNECT;
    cmd->reason = reason;

    ke_msg_send(cmd);    /* Send the message */
}

bool GAPC_IsConnectionActive(uint8_t conidx)
{
    if (conidx < APP_MAX_NB_CON)
    {
        return (gap_env.connection[conidx].conhdl != GAP_INVALID_CONHDL);
    }
    return false;
}

void GAPC_DisconnectAll(uint8_t reason)
{
    for (uint8_t i = 0; i < APP_MAX_NB_CON; i++)
    {
        if (GAPC_IsConnectionActive(i))
        {
            GAPC_DisconnectCmd(i, reason);
        }
    }
}

uint8_t GAPC_ConnectionCount(void)
{
    uint8_t count = 0;
    for (int conidx = 0; conidx < APP_MAX_NB_CON; conidx++)
    {
        if (GAPC_IsConnectionActive(conidx))
        {
            count++;
        }
    }
    return count;
}

static uint8_t GAPC_ConnectionRoleCount(uint8_t role)
{
    uint8_t count = 0;
    for (int conidx = 0; conidx < APP_MAX_NB_CON; conidx++)
    {
        if (GAPC_IsConnectionActive(conidx) &&
            GAPC_GetConnectionInfo(conidx)->role == role)
        {
            count++;
        }
    }
    return count;
}

uint8_t GAPC_MasterConnectionCount(void)
{
    return GAPC_ConnectionRoleCount(GAP_ROLE_MASTER);
}

uint8_t GAPC_SlaveConnectionCount(void)
{
    return GAPC_ConnectionRoleCount(GAP_ROLE_SLAVE);
}

const struct gapc_connection_req_ind * GAPC_GetConnectionInfo(uint8_t conidx)
{
    if (conidx < APP_MAX_NB_CON)
    {
        return &gap_env.connection[conidx];
    }
    return NULL;
}

void GAPC_GetDevInfoCfm_Name(uint8_t conidx, const char *devName)
{
    const int name_len = strlen(devName);
    /* gap_dev_name has a flexible array member. To make things simpler,
    * we'll temporarily allocate a byte array instead of heap memory. */
    uint8_t dev_info[sizeof(uint16_t) + name_len];

    memcpy(((struct gap_dev_name *)dev_info)->value, devName, name_len);
    ((struct gap_dev_name *)dev_info)->length = name_len;

    GAPC_GetDevInfoCfm(conidx, GAPC_DEV_NAME,
                       (union gapc_dev_info_val *)&dev_info);
}

void GAPC_GetDevInfoCfm_Appearance(uint8_t conidx, uint16_t appearance)
{
    union gapc_dev_info_val dev_info =
    { .appearance = appearance };
    GAPC_GetDevInfoCfm(conidx, GAPC_DEV_APPEARANCE, &dev_info);
}

void GAPC_GetDevInfoCfm_SlvPrefParams(uint8_t conidx, uint16_t con_intv_min,
                                      uint16_t con_intv_max, uint16_t slave_latency, uint16_t conn_timeout)
{
    union gapc_dev_info_val dev_info =
    {
        .slv_pref_params =
        { con_intv_min, con_intv_max, slave_latency, conn_timeout } };
    GAPC_GetDevInfoCfm(conidx, GAPC_DEV_SLV_PREF_PARAMS, &dev_info);
}

void GAPC_GetDevInfoCfm(uint8_t conidx, uint8_t req,
                        const union gapc_dev_info_val *dat)
{
    struct gapc_get_dev_info_cfm *cfm;
    size_t sz;

    /* can't use regular memcpy+sizeof because of the flexible
     * array member in gap_dev_name */
    if (req == GAPC_DEV_NAME)
    {
        sz = offsetof(struct gapc_get_dev_info_cfm, info.name.value)
             + dat->name.length;
    }
    else
    {
        sz = sizeof(union gapc_dev_info_val);
    }

    cfm = KE_MSG_ALLOC_DYN(GAPC_GET_DEV_INFO_CFM,
                           KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_get_dev_info_cfm, sz)
    ;

    if (dat != NULL)
    {
        cfm->req = req;
        memcpy(&cfm->info, dat, sz);
        ke_msg_send(cfm);
    }
}

bool GAPC_SetDevInfoCfm(uint8_t conidx, uint8_t req, bool accept)
{
    struct gapc_set_dev_info_cfm *cfm;

    if (req != GAPC_DEV_NAME && req != GAPC_DEV_APPEARANCE)
    {
        return false;
    }

    cfm = KE_MSG_ALLOC(GAPC_SET_DEV_INFO_CFM, KE_BUILD_ID(TASK_GAPC, conidx),
                       TASK_APP, gapc_set_dev_info_cfm);

    cfm->req = req;
    cfm->status = accept == true ? GAP_ERR_NO_ERROR : GAP_ERR_REJECTED;
    ke_msg_send(cfm);
    return true;
}

void GAPC_BondCfm(uint8_t conidx, enum gapc_bond request, bool accept,
                  const union gapc_bond_cfm_data *data)
{
    struct gapc_bond_cfm *cfm = KE_MSG_ALLOC(GAPC_BOND_CFM,
                                             KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_bond_cfm);
    cfm->request = request;
    cfm->accept = accept;
    memcpy(&cfm->data, data, sizeof(union gapc_bond_cfm_data));

    /* Slave side only: store generated LTK temporarily in gap_env.bondInfo.
     * It is written in NVR2 once GAPC_PAIRING_SUCCEED in received. */
    if (request == GAPC_LTK_EXCH)
    {
        memcpy(gap_env.bondInfo[conidx].ltk, data->ltk.ltk.key, GAP_KEY_LEN);
        memcpy(gap_env.bondInfo[conidx].rand, data->ltk.randnb.nb,
               GAP_RAND_NB_LEN);
        gap_env.bondInfo[conidx].ediv = data->ltk.ediv;
    }

    ke_msg_send(cfm);
}

void GAPC_EncryptCmd(uint8_t conidx, uint16_t ediv, const uint8_t *randnb,
                     const uint8_t *ltk, uint8_t key_size)
{
    struct gapc_encrypt_cmd *cmd = KE_MSG_ALLOC(GAPC_ENCRYPT_CMD,
                                                KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_encrypt_cmd);
    cmd->operation = GAPC_ENCRYPT;
    cmd->ltk.ediv = ediv;
    cmd->ltk.key_size = key_size;
    memcpy(cmd->ltk.ltk.key, ltk, key_size);
    memcpy(cmd->ltk.randnb.nb, randnb, GAP_RAND_NB_LEN);
    ke_msg_send(cmd);
}

void GAPC_EncryptCfm(uint8_t conidx, bool found, const uint8_t *ltk,
                     uint8_t key_size)
{
    /* Prepare the GAPC_ENCRYPT_CFM message to be sent to SMPC */
    struct gapc_encrypt_cfm *cfm;
    cfm = KE_MSG_ALLOC(GAPC_ENCRYPT_CFM, KE_BUILD_ID(TASK_GAPC, conidx),
                       TASK_APP, gapc_encrypt_cfm);
    cfm->found = found;
    cfm->key_size = key_size;

    if (found)
    {
        memcpy(cfm->ltk.key, ltk, GAP_KEY_LEN);
    }

    ke_msg_send(cfm);
}

bool GAPC_IsBonded(uint8_t conidx)
{
    return (GAPC_IsConnectionActive(conidx)
            && BOND_INFO_STATE_VALID(gap_env.bondInfo[conidx].state));
}

const BondInfo_t * GAPC_GetBondInfo(uint8_t conidx)
{
    if (GAPC_IsConnectionActive(conidx) && GAPC_IsBonded(conidx))
    {
        return &gap_env.bondInfo[conidx];
    }
    return NULL;
}

void GAPC_BondCmd(uint8_t conidx, const struct gapc_pairing *pairing)
{
    struct gapc_bond_cmd *cmd = KE_MSG_ALLOC(GAPC_BOND_CMD,
                                             KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_bond_cmd);
    cmd->operation = GAPC_BOND;

    memcpy(&cmd->pairing, pairing, sizeof(struct gapc_pairing));
    ke_msg_send(cmd);
}

uint16_t GAPC_AddDeviceToBondList(uint8_t conidx)
{
    gap_env.bondInfo[conidx].state = BondList_Add(&gap_env.bondInfo[conidx]);
    return (gap_env.bondInfo[conidx].state);
}

void GAPC_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                     ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);

    switch (msg_id)
    {
        case GAPC_CONNECTION_REQ_IND:
        {
            const struct gapc_connection_req_ind *p =
                (struct gapc_connection_req_ind *)param;
            memcpy(&gap_env.connection[conidx], p,
                   sizeof(struct gapc_connection_req_ind));
            gap_env.bondInfo[conidx].state = BOND_INFO_STATE_INVALID;
            if (!GAP_IsAddrPrivateResolvable(p->peer_addr.addr, p->peer_addr_type))
            {
                const BondInfo_t *bondInfo = BondList_FindByAddr(p->peer_addr.addr,
                                                                 p->peer_addr_type);
                if (bondInfo)    /* Found bond info. Copy it into environment variable */
                {
                    memcpy(&gap_env.bondInfo[conidx], bondInfo, sizeof(BondInfo_t));
                }
            }
        }
        break;

        case GAPC_DISCONNECT_IND:
        {
            /* If device is bonded and disconnection is due to MIC failure, it's likely that we tried
             * to encrypt with outdated bond information. So, let's remove it. */
            if (GAPC_IsBonded(conidx)
                && (((struct gapc_disconnect_ind *)param)->reason
                    == CO_ERROR_TERMINATED_MIC_FAILURE))
            {
                BondList_Remove(gap_env.bondInfo[conidx].state);
            }
            gap_env.connection[conidx].conhdl = GAP_INVALID_CONHDL;
        }
        break;

        case GAPC_PARAM_UPDATED_IND:
        {
            struct gapc_param_updated_ind const *p = param;

            gap_env.connection[conidx].con_interval = p->con_interval;
            gap_env.connection[conidx].con_latency = p->con_latency;
            gap_env.connection[conidx].sup_to = p->sup_to;
        }
        break;

        case GAPC_BOND_REQ_IND:
        {
            if (((struct gapc_bond_req_ind *)param)->request == GAPC_PAIRING_REQ)
            {
                /* Remove old information from BondList (if bonded before) */
                if (GAPC_IsBonded(conidx))
                {
                    BondList_Remove(GAPC_GetBondInfo(conidx)->state);
                }
                gap_env.bondInfo[conidx].state = BOND_INFO_STATE_INVALID;
                memcpy(gap_env.bondInfo[conidx].addr,
                       GAPC_GetConnectionInfo(conidx)->peer_addr.addr,
                       GAP_BD_ADDR_LEN);
                gap_env.bondInfo[conidx].addr_type =
                    GAPC_GetConnectionInfo(conidx)->peer_addr_type;
            }
        }
        break;

        case GAPC_BOND_IND:
        {
            const struct gapc_bond_ind *p = param;
            switch (p->info)
            {
                case GAPC_PAIRING_SUCCEED:
                {
                    gap_env.bondInfo[conidx].pairing_lvl = p->data.pairing.level;
                }
                break;

                case GAPC_IRK_EXCH:
                {
                    memcpy(gap_env.bondInfo[conidx].irk, p->data.irk.irk.key,
                           GAP_KEY_LEN);
                    memcpy(gap_env.bondInfo[conidx].addr, p->data.irk.addr.addr.addr,
                           BD_ADDR_LEN);
                    gap_env.bondInfo[conidx].addr_type = p->data.irk.addr.addr_type;
                    gap_env.bondInfo[conidx].irk_exchanged = 1;
                }
                break;

                case GAPC_CSRK_EXCH:
                {
                    memcpy(gap_env.bondInfo[conidx].csrk, p->data.csrk.key,
                           GAP_KEY_LEN);
                    gap_env.bondInfo[conidx].csrk_exchanged = 1;
                }
                break;

                case GAPC_LTK_EXCH:
                {
                    memcpy(gap_env.bondInfo[conidx].ltk, p->data.ltk.ltk.key,
                           GAP_KEY_LEN);
                    memcpy(gap_env.bondInfo[conidx].rand, p->data.ltk.randnb.nb,
                           GAP_RAND_NB_LEN);
                    gap_env.bondInfo[conidx].ediv = p->data.ltk.ediv;
                }
                break;
            }
        }
        break;
    }
}

void GAPM_ListSetWlCmd(uint8_t operation, uint8_t nb, struct gap_bdaddr
                       *devices_wl)
{
    struct gapm_list_set_wl_cmd *cmd;

    cmd = KE_MSG_ALLOC_DYN(GAPM_LIST_SET_CMD, TASK_GAPM, TASK_APP,
                           gapm_list_set_wl_cmd,
                           (nb * sizeof(struct gap_bdaddr)));

    cmd->operation = operation;
    cmd->size = nb;

    /* Set parameters */
    /* For each device to be added in the white list,
     * Add 6 byte addresses to the devices list. */
    memcpy(cmd->wl_info, devices_wl, sizeof(struct gap_bdaddr) * nb);

    ke_msg_send(cmd);
}

void GAPM_ListSetRalCmd(uint8_t operation, uint8_t nb,
                        struct gap_ral_dev_info *rl_devinfo)
{
    struct gapm_list_set_ral_cmd *cmd;

    cmd = KE_MSG_ALLOC_DYN(GAPM_LIST_SET_CMD, TASK_GAPM, TASK_APP,
                           gapm_list_set_ral_cmd,
                           (nb * sizeof(struct gap_ral_dev_info)));

    cmd->operation = operation;
    cmd->size = nb;

    /* Set parameters */
    /* For each device to be added in the resolving list,
     * Add address, address type, peer irk, and local irk */
    memcpy(cmd->ral_info, rl_devinfo, sizeof(struct gap_ral_dev_info) * nb);

    ke_msg_send(cmd);
}

bool GAPM_IsIRKValid(const BondInfo_t *bond_info)
{
    /* If IRK is not received and peer device uses a
     * random resolvable address, then it cannot be resolved. */
    if (bond_info->irk_exchanged != 0)
    {
        return true;
    }
    return false;
}

void WhiteList_ResolveList_Update(void)
{
    /* Add devices to the white list and resolvable list one by one */
    struct gap_bdaddr *bdaddrwl;
    struct gap_ral_dev_info *rl_devinfo;

    uint8_t nb = BondList_Size();

    bdaddrwl = malloc(sizeof(struct gap_bdaddr) * nb);
    rl_devinfo = malloc(sizeof(struct gap_ral_dev_info) * nb);

    /* Array to store bonding information */
    const BondInfo_t *list = (BondInfo_t *)BOND_INFO_BASE;

    /* Counter for number of devices on white list */
    whitelist_info.device_num = 0;

    /* Iterate through the bond list and add information (addresses and address
     * types) of bonded devices to white list and resolvable list */
    for (unsigned int i = 0; i < BONDLIST_MAX_SIZE; i++)
    {
        /* If there is bond info copy the contents to the white list and
         * resolvable list*/
        if ((list[i].state != BOND_INFO_STATE_EMPTY) &&
            (list[i].state != BOND_INFO_STATE_INVALID))
        {
            memcpy(bdaddrwl[whitelist_info.device_num].addr.addr, list[i].addr, sizeof(list[i].addr));
            bdaddrwl[whitelist_info.device_num].addr_type = list[i].addr_type;

            /* Add to the resolvable list if the address is resolvable. */
            if (GAPM_IsIRKValid(&list[i]) != 0)
            {
                memcpy(rl_devinfo[whitelist_info.device_num].addr.addr.addr, list[i].addr, sizeof(list[i].addr));
                memcpy(rl_devinfo[whitelist_info.device_num].peer_irk, list[i].irk, sizeof(list[i].irk));
                memcpy(rl_devinfo[whitelist_info.device_num].local_irk, gap_env.deviceConfig.irk.key,
                       sizeof(gap_env.deviceConfig.irk.key));
                rl_devinfo[whitelist_info.device_num].priv_mode = PRIV_TYPE_NETWORK;
                rl_devinfo[whitelist_info.device_num].addr.addr_type = list[i].addr_type;
            }
            else
            {
                /* No IRK exchanged, fill irk with zeros, set device privacy mode */
                memcpy(rl_devinfo[whitelist_info.device_num].addr.addr.addr, list[i].addr, sizeof(list[i].addr));
                memset(rl_devinfo[whitelist_info.device_num].peer_irk, 0, sizeof(list[i].irk));
                memset(rl_devinfo[whitelist_info.device_num].local_irk, 0, sizeof(gap_env.deviceConfig.irk.key));
                rl_devinfo[whitelist_info.device_num].priv_mode = PRIV_TYPE_DEVICE;
                rl_devinfo[whitelist_info.device_num].addr.addr_type = list[i].addr_type;
            }

            /* Update the amount of devices in the white list */
            whitelist_info.device_num++;
        }
    }

    /* If there are address to add to the white list, send a message */
    GAPM_ListSetWlCmd(GAPM_SET_WL, whitelist_info.device_num, bdaddrwl);

    /* If there are resolvable addresses, send a message */
    GAPM_ListSetRalCmd(GAPM_SET_RAL, whitelist_info.device_num, rl_devinfo);

    free(bdaddrwl);
    free(rl_devinfo);
}

void GAPC_SetPhyCmd(uint8_t conidx, uint8_t rx_rate, uint8_t tx_rate, uint8_t preferredRate)
{
    struct gapc_set_phy_cmd *cmd = KE_MSG_ALLOC(GAPC_SET_PHY_CMD,
                                                KE_BUILD_ID(TASK_GAPC, conidx),
                                                TASK_APP,
                                                gapc_set_phy_cmd);
    cmd->operation = GAPC_SET_PHY;
    cmd->rx_phy = rx_rate;
    cmd->tx_phy = tx_rate;
    cmd->phy_opt = preferredRate;

    ke_msg_send(cmd);
}

void GAPC_CteTxCfgCmd(uint8_t conidx, uint8_t cte_type, uint8_t ant_pattern_len, uint8_t *ant_id)
{
    struct gapc_cte_tx_cfg_cmd *cmd = KE_MSG_ALLOC_DYN(GAPC_CTE_TX_CFG_CMD,
                                                       KE_BUILD_ID(TASK_GAPC, conidx),
                                                       TASK_APP,
                                                       gapc_cte_tx_cfg_cmd, ant_pattern_len);
    cmd->operation = GAPC_CTE_TX_CFG;
    cmd->cte_types = cte_type;
    cmd->switching_pattern_len = ant_pattern_len;
    memcpy(&cmd->antenna_id[0], ant_id, ant_pattern_len);

    ke_msg_send(cmd);
}

void GAPC_CteRxCfgCmd(uint8_t conidx, uint8_t slot_dur, uint8_t ant_pattern_len, uint8_t *ant_id, bool sample_en)
{
    struct gapc_cte_rx_cfg_cmd *cmd = KE_MSG_ALLOC_DYN(GAPC_CTE_RX_CFG_CMD,
                                                       KE_BUILD_ID(TASK_GAPC, conidx),
                                                       TASK_APP,
                                                       gapc_cte_rx_cfg_cmd, ant_pattern_len);
    cmd->operation = GAPC_CTE_RX_CFG;
    cmd->sample_en = sample_en;
    cmd->slot_dur = slot_dur;
    cmd->switching_pattern_len = ant_pattern_len;
    memcpy(&cmd->antenna_id[0], ant_id, ant_pattern_len);

    ke_msg_send(cmd);
}

void GAPC_CteReqCtrlCmd(uint8_t conidx, uint8_t cte_type, uint8_t cte_len, uint8_t cte_interval, bool enable)
{
    struct gapc_cte_req_ctrl_cmd *cmd = KE_MSG_ALLOC(GAPC_CTE_REQ_CTRL_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, conidx),
                                                     TASK_APP,
                                                     gapc_cte_req_ctrl_cmd);
    cmd->operation = GAPC_CTE_REQ_CTRL;
    cmd->enable = enable;
    cmd->cte_type = cte_type;
    cmd->cte_len = cte_len;
    cmd->interval = cte_interval;

    ke_msg_send(cmd);
}

void GAPC_CteRspCtrlCmd(uint8_t conidx, bool enable)
{
    struct gapc_cte_rsp_ctrl_cmd *cmd = KE_MSG_ALLOC(GAPC_CTE_RSP_CTRL_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, conidx),
                                                     TASK_APP,
                                                     gapc_cte_rsp_ctrl_cmd);
    cmd->operation = GAPC_CTE_RSP_CTRL;
    cmd->enable = enable;

    ke_msg_send(cmd);
}

void GAPC_GetInfoCmd(uint8_t conidx, uint8_t operation)
{
    struct gapc_get_info_cmd *cmd = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, conidx),
                                                 TASK_APP,
                                                 gapc_get_info_cmd);
    cmd->operation = operation;

    ke_msg_send(cmd);
}

bool GAPM_PerAdvCteTxCmd(uint8_t actv_idx, bool enable)
{
    uint8_t actv_slot = GAPM_GetActivitySlot(actv_idx);

    /* actv_idx not found or u_param == NULL */
    if (actv_slot == APP_MAX_NB_ACTIVITY)
    {
        return false;
    }

    struct gapm_per_adv_cte_tx_ctl_cmd *cmd = KE_MSG_ALLOC(GAPM_PER_ADV_CTE_TX_CTL_CMD, TASK_GAPM,
                                                           KE_BUILD_ID(TASK_APP, actv_slot),
                                                           gapm_per_adv_cte_tx_ctl_cmd);

    cmd->operation = GAPM_PER_ADV_CTE_TX_CTL;
    cmd->actv_idx = actv_idx;
    cmd->enable = enable;

    ke_msg_send(cmd);

    return true;
}

bool GAPM_PerAdvReportCtrlCmd(uint8_t actv_idx, bool enable)
{
    uint8_t actv_slot = GAPM_GetActivitySlot(actv_idx);

    /* actv_idx not found or u_param == NULL */
    if (actv_slot == APP_MAX_NB_ACTIVITY)
    {
        return false;
    }

    struct gapm_per_adv_report_ctrl_cmd *cmd = KE_MSG_ALLOC(GAPM_PER_ADV_REPORT_CTRL_CMD,
                                                            TASK_GAPM,
                                                            KE_BUILD_ID(TASK_APP, actv_slot),
                                                            gapm_per_adv_report_ctrl_cmd);

    cmd->operation = GAPM_PER_ADV_REPORT_CTRL;
    cmd->actv_idx = actv_idx;
    cmd->enable = enable;

    ke_msg_send(cmd);

    return true;
}

bool GAPM_PerSyncIQSamplingCtrlCmd(uint8_t actv_idx, uint8_t slot_dur, uint8_t max_sample_cte,
                                   uint8_t ant_pattern_len, uint8_t *ant_id, bool enable)
{
    uint8_t actv_slot = GAPM_GetActivitySlot(actv_idx);

    /* actv_idx not found or u_param == NULL */
    if (actv_slot == APP_MAX_NB_ACTIVITY)
    {
        return false;
    }

    struct gapm_per_sync_iq_sampling_ctrl_cmd *cmd = KE_MSG_ALLOC_DYN(GAPM_PER_SYNC_IQ_SAMPLING_CTRL_CMD,
                                                                      TASK_GAPM,
                                                                      KE_BUILD_ID(TASK_APP, actv_slot),
                                                                      gapm_per_sync_iq_sampling_ctrl_cmd,
                                                                      ant_pattern_len);

    cmd->operation = GAPM_PER_SYNC_IQ_SAMPLING_CTRL;
    cmd->actv_idx = actv_idx;
    cmd->enable = enable;
    cmd->slot_dur = slot_dur;
    cmd->max_sampl_cte = max_sample_cte;
    cmd->switching_pattern_len = ant_pattern_len;
    memcpy(&cmd->antenna_id[0], ant_id, ant_pattern_len);

    ke_msg_send(cmd);

    return true;
}
