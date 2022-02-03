/**
 * @file app_msg_handler.c
 * @brief Message Handler source file
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

#include <app.h>

uint16_t app_disc_svc_count[APP_MAX_NB_CON];

GAPM_ActivityStatus_t initActivityStatus;

struct gapm_set_dev_config_cmd devConfigCmd =
{
    .operation = GAPM_SET_DEV_CONFIG,
    .role = GAP_ROLE_ALL,
    .renew_dur = APP_BD_RENEW_DUR,
    .addr.addr = APP_BLE_PRIVATE_ADDR,
    .irk.key = APP_IRK,
#if SECURE_CONNECTION
    .pairing_mode = (GAPM_PAIRING_SEC_CON | GAPM_PAIRING_LEGACY),
#else
    .pairing_mode = GAPM_PAIRING_LEGACY,
#endif
    .privacy_cfg = (GAPM_PRIVACY_TYPE | GAPM_ADDRESS_TYPE),
    .gap_start_hdl = GAPM_DEFAULT_GAP_START_HDL,
    .gatt_start_hdl = GAPM_DEFAULT_GATT_START_HDL,
    .att_cfg = GAPM_DEFAULT_ATT_CFG,
    .sugg_max_tx_octets = GAPM_DEFAULT_TX_OCT_MAX,
    .sugg_max_tx_time = GAPM_DEFAULT_TX_TIME_MAX,
    .max_mtu = GAPM_DEFAULT_MTU_MAX,
    .max_mps = GAPM_DEFAULT_MPS_MAX,
    .max_nb_lecb = GAPM_DEFAULT_MAX_NB_LECB,
    .audio_cfg = GAPM_DEFAULT_AUDIO_CFG,
    .tx_pref_phy = GAP_PHY_ANY,
    .rx_pref_phy = GAP_PHY_ANY
};

struct gapm_init_param initParam =
{
    .type = GAPM_INIT_TYPE_DIRECT_CONN_EST,
#if ADV_EXTENSION == 1
    .prop = GAPM_INIT_PROP_CODED_BIT,
#else
    .prop = GAPM_INIT_PROP_1M_BIT,
#endif
    .conn_to = 0,
#if ADV_EXTENSION == 1
    .scan_param_coded.scan_intv = GAPM_DEFAULT_SCAN_INTERVAL,
    .scan_param_coded.scan_wd = GAPM_DEFAULT_SCAN_WINDOW,
    .conn_param_coded.ce_len_max = GAPM_DEFAULT_CE_LEN_MAX,
    .conn_param_coded.ce_len_min = GAPM_DEFAULT_CE_LEN_MIN,
    .conn_param_coded.conn_intv_max = GAPM_DEFAULT_CON_INTV_MAX,
    .conn_param_coded.conn_intv_min = GAPM_DEFAULT_CON_INTV_MIN,
    .conn_param_coded.conn_latency = GAPM_DEFAULT_CON_LATENCY,
    .conn_param_coded.supervision_to = GAPM_DEFAULT_SUPERV_TO,
#else
    .scan_param_1m.scan_intv = GAPM_DEFAULT_SCAN_INTERVAL,
    .scan_param_1m.scan_wd = GAPM_DEFAULT_SCAN_WINDOW,
    .conn_param_1m.ce_len_max = GAPM_DEFAULT_CE_LEN_MAX,
    .conn_param_1m.ce_len_min = GAPM_DEFAULT_CE_LEN_MIN,
    .conn_param_1m.conn_intv_max = GAPM_DEFAULT_CON_INTV_MAX,
    .conn_param_1m.conn_intv_min = GAPM_DEFAULT_CON_INTV_MIN,
    .conn_param_1m.conn_latency = GAPM_DEFAULT_CON_LATENCY,
    .conn_param_1m.supervision_to = GAPM_DEFAULT_SUPERV_TO,
#endif
    .peer_addr.addr_type = APP_BD_ADDRESS_PEER_TYPE1,
    .peer_addr.addr.addr = APP_BD_ADDRESS_PEER1
};

union gapc_bond_cfm_data pairingRsp =
{
    .pairing_feat =
    {
        .iocap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
        .oob = GAP_OOB_AUTH_DATA_NOT_PRESENT,
        .key_size = KEY_LEN,
        .ikey_dist = (GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
        .rkey_dist = (GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
        .auth = GAP_AUTH_REQ_SEC_CON_BOND,
        .sec_req = GAP_SEC1_NOAUTH_PAIR_ENC,
    }
};

struct gapc_pairing pairing =
{
    .iocap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
    .oob = GAP_OOB_AUTH_DATA_NOT_PRESENT,
    .key_size = KEY_LEN,
    .ikey_dist = (GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
    .rkey_dist = (GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
#if SECURE_CONNECTION
    .auth = GAP_AUTH_REQ_SEC_CON_BOND,
    .sec_req = GAP_SEC1_NOAUTH_PAIR_ENC
#else
    .auth = GAP_AUTH_REQ_NO_MITM_BOND,
    .sec_req = GAP_NO_SEC
#endif
};

struct ble_device_parameter ble_dev_params =
{
	.low_pwr_clk_accuracy = LOW_POWER_CLOCK_ACCURACY,
	.twosc = TWOSC,
#if defined(LPCLK_SRC_XTAL32) && defined(LPCLK_SRC_RC32)
	.low_pwr_clk =
	{
		.low_pwr_clk_xtal32 = LPCLK_SRC_XTAL32,
		.low_pwr_clk_rc32 = LPCLK_SRC_RC32,
		.low_pwr_standby_clk_src = LPCLK_STANDBYCLK_SRC
	}
#endif
};

static void SetConnectionCfmParams(uint8_t conidx, struct gapc_connection_cfm* cfm);

void BLE_ConfigHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
    switch(msg_id)
    {
        case GAPM_CMP_EVT:
        {
            const struct gapm_cmp_evt *p = param;

            if(p->operation == GAPM_RESET) /* Step 2 */
            {
                swmLogInfo("__GAPM_RESET completed. Setting BLE device configuration...\r\n");

                /* Check privacy_cfg bit 0 to identify address type, public if not set*/
                if(devConfigCmd.privacy_cfg & GAPM_CFG_ADDR_PRIVATE)
                {
                    swmLogInfo("	devConfigCmd address to set static private random\r\n");
                }
                else
                {
                    /* Read Device BLE Public Address */
                    uint8_t ble_dev_addr_len = GAP_BD_ADDR_LEN;
                    uint8_t ble_dev_addr_buf[GAP_BD_ADDR_LEN] = {0};

                    /* Make sure proper BLE public address has been read and saved into ble_public_addr
                     * using Device_BLE_Public_Address_Read() before calling Device_BLE_Param_Get() */
                    Device_BLE_Param_Get(PARAM_ID_BD_ADDRESS, &ble_dev_addr_len, ble_dev_addr_buf);

                    swmLogInfo("	Device BLE public address read: ");
                    for(int i = 0; i < GAP_BD_ADDR_LEN; i++)
                    {
                        swmLogInfo("0x%02x ", ble_dev_addr_buf[i]);
                    }
                    swmLogInfo("\r\n");

                    swmLogInfo("	devConfigCmd address set to public\r\n");
                    memcpy(devConfigCmd.addr.addr, ble_dev_addr_buf, GAP_BD_ADDR_LEN);
                }

                /* Send a device configuration request to the BLE stack.
                 * The stack sends back a GAPM_CMP_EVT / GAPM_SET_DEV_CONFIG upon completion. */
                GAPM_SetDevConfigCmd(&devConfigCmd);

                /* Set discSvcCount, custom service database and maximum number of custom services
                 * in GATT environment at GAPM_RESET */
                GATT_SetEnvData(app_disc_svc_count, NULL, APP_NUM_CUST_SVC);
            }
            else if(p->operation == GAPM_SET_DEV_CONFIG &&
                    p->status == GAP_ERR_NO_ERROR) /* Step 3 */
            {
                swmLogInfo("__GAPM_SET_DEV_CONFIG completed.\r\n");
                /* At this point, in parallel, the battery service client abstraction (BASC, ble_basc.c)
                 * adds the standard profile to the database. See BASC_MsgHandler for details. */

                /* Request the stack to create an initiating activity.
                 * The stack sends back a GAPM_ACTIVITY_CREATED_IND. See BLE_ActivityHandler for next steps. */
                swmLogInfo("    Creating initiating activity...\r\n");
                GAPM_ActivityCreateInitCmd(&initActivityStatus, GAPM_STATIC_ADDR);
            }
        }
        break;
        case GAPM_PROFILE_ADDED_IND: /* Step 4 - BASC profile added */
        {
            swmLogInfo("__GAPM_PROFILE_ADDED_IND - profile added count=%d\r\n",
                    GAPM_GetProfileAddedCount());
        }
        break;
    }
}

void BLE_ActivityHandler(ke_msg_id_t const msg_id, void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msg_id)
    {
        case GAPM_ACTIVITY_CREATED_IND: /* Step 5 */
        {
            swmLogInfo("__GAPM_ACTIVITY_CREATED_IND actv_idx = %d. Starting initiating activity...\r\n",
                    initActivityStatus.actv_idx);
            GAPM_InitActivityStart(initActivityStatus.actv_idx, &initParam);
        }
        break;

        case GAPM_ACTIVITY_STOPPED_IND: /* Step 9(c) */
        {
            /* If initiating activity is stopped, restart it if not connected to
             * the maximum number of peers for this application */
            if((GAPC_ConnectionCount() < APP_MAX_NB_CON))
            {
                swmLogInfo("__GAPM_ACTIVITY_STOPPED_IND Restarting initiating operation...\r\n");
                if(initActivityStatus.state == ACTIVITY_STATE_NOT_STARTED)
                {
                    GAPM_InitActivityStart(initActivityStatus.actv_idx, &initParam);
                }
            }
        }
        break;
    }
}

void BLE_ConnectionHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    switch(msg_id)
    {
        case GAPC_CONNECTION_REQ_IND: /* Step 6 */
        {
            const struct gapc_connection_req_ind* p = param;

            swmLogInfo("__GAPC_CONNECTION_REQ_IND conidx=%d\r\n", conidx);

            /* If the peer device address is private resolvable and bond list is not empty */
            if(GAP_IsAddrPrivateResolvable(p->peer_addr.addr, p->peer_addr_type) &&
               BondList_Size() > 0) /* Step 7(a) */
            {
                /* Ask the stack to resolve the address with the IRKs we have in our bond list.
                 * In case of success, the stack returns GAPM_ADDR_SOLVED_IND.
                 * If not successful (not bonded previously) the stack returns GAPM_CMP_EVT /
                 * GAPM_RESOLV_ADDR with status GAP_ERR_NOT_FOUND (see below). */
                GAPM_ResolvAddrCmd(conidx, p->peer_addr.addr);
            }
            else /* Step 7(b) */
            {
                /* Address is not private resolvable or bondlist empty. Confirms connection right away.
                 * If the device was previously bonded, the LTK is included. */
            	APP_SendConCfm(conidx);

                /* If previously bonded, request connection encryption. */
                if(GAPC_IsBonded(conidx))
                {
                    GAPC_EncryptCmd(conidx, GAPC_GetBondInfo(conidx)->ediv,
                                    GAPC_GetBondInfo(conidx)->rand,
                                    GAPC_GetBondInfo(conidx)->ltk, GAP_KEY_LEN);
                    swmLogInfo("__Sending GAPC_EncryptCmd...\r\n");
                }
                else /* Start pairing / bonding procedure */
                {
                    swmLogInfo("__Not previously bonded. Sending GAPC_BOND_REQ...\r\n");
                    GAPC_BondCmd(conidx, &pairing);
                }
            }
        }
        break;

        case GAPC_DISCONNECT_IND:
        {
            swmLogInfo("__GAPC_DISCONNECT_IND: reason = %d\r\n",
                    ((struct gapc_disconnect_ind*)param)->reason);

            /* Restart initiating if not connected to the maximum number of
             * peers for this application */
            if(GAPC_ConnectionCount() == (APP_MAX_NB_CON - 1))
            {
                swmLogInfo("Restarting initiating activity...\r\n");
                GAPM_InitActivityStart(initActivityStatus.actv_idx, &initParam);
            }
        }
        break;

        case GAPM_ADDR_SOLVED_IND: /* Step 8(a) */
        {
            /* Private address resolution was successful */
            swmLogInfo("__GAPM_ADDR_SOLVED_IND\r\n");

            conidx = KE_IDX_GET(dest_id);
            /* Send confirmation */
            APP_SendConCfm(conidx);

            /* Previously bonded. Request connection encryption. */
            GAPC_EncryptCmd(conidx, GAPC_GetBondInfo(conidx)->ediv,
                            GAPC_GetBondInfo(conidx)->rand,
                            GAPC_GetBondInfo(conidx)->ltk, GAP_KEY_LEN);
            swmLogInfo("__Sending GAPC_EncryptCmd");
        }
        break;

        case GAPM_CMP_EVT: /* Step 8(b) */
        {
            /* Private address resolution couldn't find an IRK that resolves this address.
             * (i.e. peer device not previously bonded). Confirm connection without LTK. */
            const struct gapm_cmp_evt* p = param;
            if((p->operation == GAPM_RESOLV_ADDR) &&
                    (p->status == GAP_ERR_NOT_FOUND))
            {
                conidx = KE_IDX_GET(dest_id);
                
                APP_SendConCfm(conidx);
                
                swmLogInfo("__Not previously bonded. Address resolution failed. Sending GAPC_BOND_REQ...\r\n");
                GAPC_BondCmd(conidx, &pairing);
            }
        }
        break;

        case GAPC_PARAM_UPDATE_REQ_IND: /* Step 9 */
        {
            /* Peer device requested update in connection params. Accept it. */
            GAPC_ParamUpdateCfm(conidx, true, 0xFFFF, 0xFFFF);
            swmLogInfo("GAPC_PARAM_UPDATE_REQ_IND\r\n");
        }
        break;
    }
}

void BLE_PairingHandler(ke_msg_id_t const msg_id, void const *param,
                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    switch(msg_id)
    {
        case GAPC_BOND_REQ_IND:  /* Step 10(a) - peer device wants to exchange keys */
        {
            const struct gapc_bond_req_ind* p = param;
            switch (p->request)
            {
                case GAPC_IRK_EXCH:
                {
                    swmLogInfo("__GAPC_BOND_REQ_IND / GAPC_IRK_EXCH\r\n");
                    union gapc_bond_cfm_data irkExch;
                    memcpy(irkExch.irk.addr.addr.addr, GAPM_GetDeviceConfig()->addr.addr, GAP_BD_ADDR_LEN);
                    irkExch.irk.addr.addr_type = GAPM_GetDeviceConfig()->privacy_cfg;
                    memcpy(irkExch.irk.irk.key, GAPM_GetDeviceConfig()->irk.key, GAP_KEY_LEN);
                    GAPC_BondCfm(conidx, GAPC_IRK_EXCH, true, &irkExch); /* Send confirmation */
                }
                break;

                case GAPC_CSRK_EXCH:
                {
                    swmLogInfo("__GAPC_BOND_REQ_IND / GAPC_CSRK_EXCH\r\n");
                    union gapc_bond_cfm_data csrkExch;
                    GAPC_BondCfm(conidx, GAPC_CSRK_EXCH, true, &csrkExch); /* Send confirmation */
                }
                break;
            }
        }
        break;

        case GAPC_BOND_IND: /* Step 11 - pairing finished */
        {
            const struct gapc_bond_ind* p = param;
            if(p->info == GAPC_PAIRING_SUCCEED)
            {
                swmLogInfo("__GAPC_BOND_IND / GAPC_PAIRING_SUCCEED\r\n");
                GAPC_AddDeviceToBondList(conidx);
            }
            else if(p->info == GAPC_PAIRING_FAILED)
            {
                swmLogError("__GAPC_BOND_IND / GAPC_PAIRING_FAILED reason=%d\r\n", p->data.reason);
            }
        }
        break;

        case GAPC_ENCRYPT_IND: /* Step 10(b)  */
        {
            swmLogInfo("__GAPC_ENCRYPT_IND: Link encryption is ON\r\n");
        }
        break;
    }
}

/* The LED handler keeps running in parallel and blinking the
 * LED according to the number of connected devices. */
void LEDHandler(ke_msg_id_t const msg_id, void const *param,
                    ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    static uint8_t toggle_cnt = 0;
    uint8_t connectionCount = GAPC_ConnectionCount();

    /* Blink LED according to the number of connections */
    switch (connectionCount)
    {
        /* If no connections, toggle LED_GPIO_NUM every 200ms */
        case 0:
        {
            ke_timer_set(APP_LED_TIMEOUT, TASK_APP, TIMER_SETTING_MS(200));
            Sys_GPIO_Toggle(LED_GPIO_NUM);
            toggle_cnt = 0;
        }
        break;

        /* If max connections, set LED_GPIO_NUM steady low */
        case APP_MAX_NB_CON:
        {
            ke_timer_set(APP_LED_TIMEOUT, TASK_APP, TIMER_SETTING_MS(200));
            Sys_GPIO_Set_Low(LED_GPIO_NUM);
            toggle_cnt = 0;
        }
        break;

        /* If connectionCount is between 1 and APP_NB_PEERS (exclusive),
		 * toggle LED on and off connectionCount times then set steady low until
		 * next 2s blinking period */
        default:
        {
            if (toggle_cnt >= connectionCount * 2)
            {
                toggle_cnt = 0;

                /* Schedule timer for a long 2s break */
                ke_timer_set(APP_LED_TIMEOUT, TASK_APP, TIMER_SETTING_S(2));

                /* LED_GPIO_NUM steady low until next 2s blinking period */
                Sys_GPIO_Set_Low(LED_GPIO_NUM);
            }
            else
            {
                toggle_cnt++;
                Sys_GPIO_Toggle(LED_GPIO_NUM);
                ke_timer_set(APP_LED_TIMEOUT, TASK_APP, TIMER_SETTING_MS(200));
            }
        }
    }
}

static void SetConnectionCfmParams(uint8_t conidx, struct gapc_connection_cfm* cfm)
{
    cfm->svc_chg_handle = 0;
    cfm->ltk_present = false;
#if SECURE_CONNECTION
    cfm->pairing_lvl = GAP_PAIRING_BOND_SECURE_CON;
#else
    cfm->pairing_lvl = GAP_PAIRING_BOND_UNAUTH;
#endif

    cfm->cli_feat = 0;
    cfm->cli_info = 0;
    cfm->gatt_start_handle = 0;
    cfm->gatt_end_handle = 0;
    cfm->svc_chg_handle = 0;

    if(GAPC_IsBonded(conidx))
    {
        cfm->ltk_present = true;
        memcpy(cfm->rcsrk.key, GAPC_GetBondInfo(conidx)->csrk, KEY_LEN);
        cfm->lsign_counter = 0xFFFFFFFF;
        cfm->rsign_counter = 0;
    }
    swmLogInfo("  connectionCfm->ltk_present = %d\r\n", cfm->ltk_present);
}

void APP_SendConCfm(uint8_t conidx)
{
    struct gapc_connection_cfm cfm;
    SetConnectionCfmParams(conidx, &cfm);
    GAPC_ConnectionCfm(conidx, &cfm); /* Send connection confirmation */

#ifdef APP_PHY_UPDATE_REQ
	GAPC_SetPhyCmd(conidx, APP_PREFERRED_PHY_RX, APP_PREFERRED_PHY_TX,
		       APP_PREFERRED_CODED_PHY_RATE);
#endif

    /* Get Task IDs for each added profile/s */
    uint16_t added_profile_task_id[APP_MAX_NB_PROFILES];
    memcpy(added_profile_task_id, GAP_GetProfileAddedTaskId(), APP_MAX_NB_PROFILES);

    /* Enable standard profile services using task IDs */
    for(uint8_t i = 0; i < APP_MAX_NB_PROFILES; i++)
    {
    	if((GAPC_GetConnectionInfo(conidx)->role == GAP_ROLE_MASTER) &&
    			(added_profile_task_id[i] == TASK_ID_BASC))
    	{
    		/* Enable BASC */
    		BASC_EnableReq(conidx);
    	    swmLogInfo("  Enabling BASC...\n");
    	    break;
    	}
    }
}
