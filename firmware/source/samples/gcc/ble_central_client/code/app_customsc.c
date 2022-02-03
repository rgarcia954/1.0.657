/**
 * @file app_customsc.h
 * @brief CUSTOMSC (Custom Service Client) source file
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

#include <app_customsc.h>
#include <ble_abstraction.h>
#include <swmTrace_api.h>
#include <stdio.h>

/* Global variable definition */
static struct cs_env_tag cs_env[BLE_CONNECTION_MAX];

/* Queued write state machine variables */
uint8_t queued_wr_state;
uint8_t queued_wr_idx;

void CUSTOMSC_Initialize(void)
{
    for (unsigned int i = 0; i < BLE_CONNECTION_MAX; i++)
    {
        /* Reset the application manager environment */
        memset(&cs_env[i], 0, sizeof(cs_env[i]));
        cs_env[i].gattc_write_complete = true;
    }

    /* Add custom service handlers. Medium priority means it will be
     * called after GAP, GATT but before APP handlers. */
    MsgHandler_Add(TASK_ID_GATTC,           CUSTOMSC_MsgHandler);
    MsgHandler_Add(BASC_ENABLE_RSP, 		CUSTOMSC_MsgHandler);
    MsgHandler_Add(GAPC_DISCONNECT_IND,     CUSTOMSC_MsgHandler);
    MsgHandler_Add(CUSTOMSC_TIMER,          CUSTOMSC_MsgHandler);
}

void CUSTOMSC_SendWrite(uint8_t conidx, uint8_t *value, uint16_t handle,
                             uint8_t offset, uint16_t length, uint8_t type)
{
    if (conidx != GAP_INVALID_CONIDX)
    {
        struct gattc_write_cmd *cmd = KE_MSG_ALLOC_DYN(GATTC_WRITE_CMD,
                                                       KE_BUILD_ID(TASK_GATTC,
                                                                   conidx),
                                                       TASK_APP,
                                                       gattc_write_cmd,
                                                       length *
                                                       sizeof(uint8_t));

        if (type == GATTC_WRITE)
        {
            /* Write request that needs a response from peer device */
            cmd->operation = GATTC_WRITE;
            cmd->auto_execute = 1;
        }
        else if (type == GATTC_WRITE_NO_RESPONSE)
        {
            /* Write command that doesn't need a response from peer device */
            cmd->operation = GATTC_WRITE_NO_RESPONSE;
            cmd->auto_execute = 0;
        }

        cmd->handle = handle;
        cmd->seq_num = 0x00;
        cmd->offset = offset;
        cmd->cursor = 0;
        cmd->length = length;
        memcpy(cmd->value, (uint8_t *)value, length);

        cs_env[conidx].gattc_write_complete = false;

        /* Send the message */
        ke_msg_send(cmd);
    }
}

void CUSTOMSC_PrepareWrite(uint8_t conidx, uint8_t *value, uint16_t handle,
                                uint8_t offset, uint16_t length)
{
    struct gattc_write_cmd *cmd = KE_MSG_ALLOC_DYN(GATTC_WRITE_CMD,
                                                   KE_BUILD_ID(TASK_GATTC,
                                                               conidx),
                                                   TASK_APP,
                                                   gattc_write_cmd, length *
                                                   sizeof(uint8_t));

    cmd->operation    = GATTC_WRITE;
    cmd->auto_execute = 0;
    cmd->handle  = handle;
    cmd->seq_num = 0;
    cmd->offset  = offset;
    cmd->cursor  = 0;
    cmd->length  = length;
    memcpy((void *)cmd->value, (const void *)value, length);
    ke_msg_send(cmd);
}

void CUSTOMSC_ExecWrite(uint8_t conidx)
{
    struct gattc_execute_write_cmd *cmd = KE_MSG_ALLOC(GATTC_EXECUTE_WRITE_CMD,
                                                       KE_BUILD_ID(TASK_GATTC,
                                                                   conidx),
                                                       TASK_APP,
                                                       gattc_execute_write_cmd);

    cmd->operation = GATTC_EXEC_WRITE;
    cmd->execute   = true;
    cmd->seq_num   = 0;
    ke_msg_send(cmd);
}

void CUSTOMSC_ReadLong(uint8_t conidx, uint16_t handle,
                            uint8_t offset, uint16_t length)
{
    struct gattc_read_cmd *cmd = KE_MSG_ALLOC(GATTC_READ_CMD,
                                              KE_BUILD_ID(TASK_GATTC,
                                                          conidx),
                                              TASK_APP,
                                              gattc_read_cmd);
    cmd->operation = GATTC_READ_LONG;
    cmd->seq_num = 0;
    cmd->req.simple.handle = handle;
    cmd->req.simple.length = length;
    cmd->req.simple.offset = 0;
    ke_msg_send(cmd);
}

void CUSTOMSC_AppWriteCharSingle(void)
{
    for (unsigned int i = 0; i < BLE_CONNECTION_MAX; i++)
    {
        if ((GAPC_GetConnectionInfo(i)->conhdl != GAP_INVALID_CONHDL) &&
            (cs_env[i].state == CS_ALL_ATTS_DISCOVERED) &&
            (cs_env[i].gattc_write_complete == true))
        {
            /* Increment the transmit data pattern variable */
            cs_env[i].val_notif++;
            memset(cs_env[i].rx_value, cs_env[i].val_notif,
                   CS_RX_VALUE_MAX_LENGTH);
            CUSTOMSC_SendWrite(i, cs_env[i].rx_value,
                                    cs_env[i].disc_att[CS_IDX_RX_CHAR].
                                    pointer_hdl, 0,
                                    CS_RX_VALUE_MAX_LENGTH, GATTC_WRITE);
            cs_env[i].gattc_write_complete = false;
        }
    }
}

void CUSTOMSC_QueuedWriteInit(void)
{
    queued_wr_state = STATE_WRITE_QUEUE_STAGE1;
    queued_wr_idx   = 0;
}

uint8_t CUSTOMSC_QueuedWriteRun(void)
{
    uint8_t retval = 1;
    uint8_t i = queued_wr_idx;
    if (queued_wr_state > STATE_WRITE_QUEUE_STAGE1)
    {
        /* Check if GATTC write operation was successful */
        if (cs_env[i].gattc_write_complete == false)
        {
            /* Return if write response was not received from slave device */
            return (1);
        }
    }

    switch (queued_wr_state)
    {
        case STATE_WRITE_QUEUE_STAGE1:
        {
            bool dev_found = false;
            do
            {
                /* Check if we can write to this device */
                if ((GAPC_GetConnectionInfo(i)->conhdl != GAP_INVALID_CONHDL) &&
                    (cs_env[i].state == CS_ALL_ATTS_DISCOVERED) &&
                    (cs_env[i].gattc_write_complete == true))
                {
                    dev_found = true;
                    break;
                }
            }
            while (i++ < BLE_CONNECTION_MAX);

            if (dev_found == false)
            {
                /* No devices found to send data return */
                return (0);
            }

            /* Store this device number for future transfers */
            queued_wr_idx = i;

            /* Write CS_RX_VALUE_MAX_LENGTH/2 bytes with offset 0*/
            cs_env[i].val_notif++;
            memset(cs_env[i].rx_value, cs_env[i].val_notif,
                   CS_RX_VALUE_MAX_LENGTH);
            CUSTOMSC_PrepareWrite(i, cs_env[i].rx_value,
                                       cs_env[i].disc_att[CS_IDX_RX_CHAR].
                                       pointer_hdl,
                                       0, CS_RX_VALUE_MAX_LENGTH / 2);
            cs_env[i].gattc_write_complete = false;
            queued_wr_state = STATE_WRITE_QUEUE_STAGE2;
        }
        break;

        case STATE_WRITE_QUEUE_STAGE2:
        {
            /* Write CS_RX_VALUE_MAX_LENGTH/2 bytes with offset
             * CS_RX_VALUE_MAX_LENGTH/2 */
            cs_env[i].val_notif++;
            memset(cs_env[i].rx_value, cs_env[i].val_notif,
                   CS_RX_VALUE_MAX_LENGTH);
            CUSTOMSC_PrepareWrite(i, cs_env[i].rx_value,
                                       cs_env[i].disc_att[CS_IDX_RX_CHAR].
                                       pointer_hdl,
                                       CS_RX_VALUE_MAX_LENGTH / 2,
                                       CS_RX_VALUE_MAX_LENGTH / 2);
            queued_wr_state = STATE_WRITE_EXECUTE;
        }
        break;

        case STATE_WRITE_EXECUTE:
        {
            /* Issue a write execute of all values on the peripheral side */
            CUSTOMSC_ExecWrite(i);
            queued_wr_idx++;
            if (queued_wr_idx >= BLE_CONNECTION_MAX)
            {
                retval = 0;
            }
            else
            {
                queued_wr_state = STATE_WRITE_QUEUE_STAGE1;
            }
        }
        break;

        default:
        {
        }
        break;
    }
    return (retval);
}

void CUSTOMSC_Timer(void)
{
    static uint32_t char_write_intv = 0;
    static uint32_t rw_long_intv    = 0;
    static uint8_t write_state     = WRITE_SINGLE;
    static uint8_t long_wr_data[CS_RX_LONG_VALUE_MAX_LENGTH] = {0};
    static uint8_t long_wr_val = 0;

    /* Restart timer */
    ke_timer_set(CUSTOMSC_TIMER, TASK_APP, CUSTOMSC_TIMER_200MS_SETTING);

    if (rw_long_intv >= READ_WRITE_LONG_INTERVAL_MS)
    {
        for (unsigned int i = 0; i < BLE_CONNECTION_MAX; i++)
        {
            if (GAPC_IsConnectionActive(i))
            {
                memset(long_wr_data, long_wr_val, CS_RX_LONG_VALUE_MAX_LENGTH);
                CUSTOMSC_ReadLong(i,
                                       cs_env[i].disc_att[CS_IDX_TX_LONG_CHAR].pointer_hdl,
                                       0,
                                       CS_TX_LONG_VALUE_MAX_LENGTH);
                CUSTOMSC_SendWrite(i,
                                        long_wr_data,
                                        cs_env[i].disc_att[CS_IDX_RX_LONG_CHAR].pointer_hdl,
                                        0,
                                        CS_RX_LONG_VALUE_MAX_LENGTH,
                                        GATTC_WRITE);
                long_wr_val++;
            }
        }
        rw_long_intv = 0;
    }

    switch (write_state)
    {
        case WRITE_SINGLE:
        {
            if (char_write_intv >= CHAR_WRITE_INTERVAL_MS)
            {
                CUSTOMSC_AppWriteCharSingle();
                char_write_intv = 0;

                /* Finished executing single write */
                write_state     = WRITE_QUEUED;
                CUSTOMSC_QueuedWriteInit();
            }
        }
        break;

        case WRITE_QUEUED:
        {
            if (CUSTOMSC_QueuedWriteRun() == 0)
            {
                write_state = WRITE_SINGLE;
            }
        }
        break;
    }

    /* Update interval counters */
    char_write_intv += CUSTOMSC_TIMER_200MS_SETTING;
    rw_long_intv    += CUSTOMSC_TIMER_200MS_SETTING;
}

/**
 * @brief       Use swmLogInfo for 10 Bytes of data at time. Remaining
 * 				byte/s will be printed in string format to avoid use
 * 				of swmLogInfo for individual byte
 * @param       buffer   data buffer
 * @param       length   length of data
 * @return      None
 * @assumptions None
 */
static void print_large_buffer(const uint8_t *buffer, uint16_t length)
{
	unsigned int num_of_blocks = (length / 10);
	unsigned int rem_bytes = ((num_of_blocks) ? (length % 10) : length);

	for(int i = 0; i < num_of_blocks; i++)
	{
		int j = (i * 10);
		swmLogInfo("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ", buffer[j],
				buffer[j+1], buffer[j+2], buffer[j+3],
				buffer[j+4], buffer[j+5], buffer[j+6],
				buffer[j+7], buffer[j+8], buffer[j+9]);
	}

	if(rem_bytes)
	{
		int j = (num_of_blocks * 10);
		char temp_string[28] = {0};
		uint8_t index = 0;

		for (int i = j; i < length; i++)
		{
			sprintf(&temp_string[index], "%02x", buffer[i]);
			index += 2;
			/* put space before next data */
			temp_string[index] = 0x20;
			index += 1;
		}

		swmLogInfo("%s ", temp_string);
	}

	swmLogInfo("\r\n");
}

void CUSTOMSC_MsgHandler(ke_msg_id_t const msg_id, void const *param,
                         ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    switch(msg_id)
    {
        case BASC_ENABLE_RSP:
        {
            /* Start a service discovery */
            uint8_t cs_uuid[] = CS_SVC_UUID;

            GATTC_DiscByUUIDSvc(conidx, cs_uuid, ATT_UUID_128_LEN,
            		GATTC_DEFAULT_START_HDL, GATTC_DEFAULT_END_HDL);
        	break;
        }

        case GAPC_DISCONNECT_IND:
        {
            cs_env[conidx].state = CS_INIT;

            /* If no active connections, stop periodic timer */
            if(GAPC_ConnectionCount() == 0)
            {
                ke_timer_clear(CUSTOMSC_TIMER, TASK_APP);
            }
            break;
        }

        case GATTC_DISC_SVC_IND:
        {
            const struct gattc_disc_svc_ind* p = param;
            uint8_t cs_uuid[] = CS_SVC_UUID;

            /* If found the custom service specified */
            if ((p->uuid_len == ATT_UUID_128_LEN) &&
                !memcmp(p->uuid, cs_uuid, p->uuid_len))
            {
                cs_env[conidx].state       = CS_SERVICE_DISCOVERD;
                cs_env[conidx].start_hdl   = p->start_hdl;
                cs_env[conidx].end_hdl     = p->end_hdl;
                cs_env[conidx].disc_attnum = 0;

                /* Start characteristic discovery */
                GATTC_DiscAllChar(conidx, cs_env[conidx].start_hdl, cs_env[conidx].end_hdl);
                swmLogInfo("__CUSTOMSC_SERVICE_DISCOVERED. Starting characteristics discovery...\n");
            }
        }
        break;

        case GATTC_DISC_CHAR_IND:
        {
            const struct gattc_disc_char_ind* p = param;
            uint8_t uuid[4][16] = CS_CHARACTERISTICS_LIST;

            /* Attr_hdl is for characteristic handle and pointer_hdl for value  */
            if (p->attr_hdl != 0 && cs_env[conidx].disc_attnum < CS_IDX_NB)
            {
                for (uint8_t i = 0; i < CS_IDX_NB; i++)
                {
                    if (p->uuid_len == ATT_UUID_128_LEN &&
                        !memcmp(p->uuid, &uuid[i][0], ATT_UUID_128_LEN))
                    {
                        memcpy(&cs_env[conidx].disc_att[cs_env[conidx].disc_attnum],
                               p, sizeof(struct discovered_char_att));

                        cs_env[conidx].disc_attnum++;
                        break;
                    }
                }

                if (cs_env[conidx].disc_attnum == CS_IDX_NB)
                {
                    cs_env[conidx].state = CS_ALL_ATTS_DISCOVERED;

                    /* If first connection, start periodic timer */
                    if(GAPC_ConnectionCount() == 1)
                    {
                        ke_timer_set(CUSTOMSC_TIMER, TASK_APP, CUSTOMSC_TIMER_200MS_SETTING);
                    }
                }
            }
        }
        break;

        case GATTC_READ_IND:
        {
            const struct gattc_read_ind* p = param;
            if (cs_env[conidx].disc_att[CS_IDX_TX_CHAR].pointer_hdl ==
                p->handle)
            {
                memcpy(cs_env[conidx].tx_value, p->value, p->length);
            }
        }
        break;

        case GATTC_EVENT_IND:
        {
            const struct gattc_event_ind* p = param;
            if (p->length > 0)
            {
                if (cs_env[conidx].disc_att[CS_IDX_TX_CHAR].pointer_hdl ==
                    p->handle)
                {
                    memcpy(cs_env[conidx].tx_value, p->value, p->length);
                }
                swmLogInfo("\r\nCUSTOMSC: GATTC_EVENT_IND: type=%d, length=%d, handle=0x%02x, data=\r\n",
                        p->type, p->length, p->handle);

                print_large_buffer(p->value, p->length);
            }
        }
        break;

        case GATTC_EVENT_REQ_IND:
        {
            const struct gattc_event_req_ind *p = (struct gattc_event_req_ind*) param;
            if (p->length > 0)
            {
                if (cs_env[conidx].disc_att[CS_IDX_TX_CHAR].pointer_hdl ==
                    p->handle)
                {
                    memcpy(cs_env[conidx].tx_value, p->value, p->length);
                }
                swmLogInfo("\r\nGATTC_EVENT_REQ_IND: type=%d, length=%d, handle=0x%02x, data=\r\n",
                        p->type, p->length, p->handle);

                print_large_buffer(p->value, p->length);
            }
            /* Send a confirmation */
            GATTC_SendEvtCfm(conidx, p->handle);
        }
        break;

        case GATTC_CMP_EVT:
        {
            const struct gattc_cmp_evt* p = param;
            if (p->operation == GATTC_WRITE)
            {
                if (p->status == GAP_ERR_NO_ERROR)
                {
                    cs_env[conidx].gattc_write_complete = true;
                }

                if (p->status == GAP_ERR_DISCONNECTED)
                {
                    cs_env[conidx].gattc_write_complete = true;
                }
            }
        }
        break;

        case CUSTOMSC_TIMER:
        {
            CUSTOMSC_Timer();
        }
        break;
    }
}
