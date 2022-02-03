/**
 * @file uart_hci.c
 * @brief Source file for UART and Bluetooth stack interface
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.see.
 *
 * This is Reusable Code.
 * @endparblock
 */

#include <stddef.h>     /* standard definition */

#include "app.h"
#include "uart_hci.h"       /* uart definition */

void __attribute__ ((alias("uart_isr"))) DMA0_IRQHandler (void);

void __attribute__ ((alias("uart_isr"))) DMA1_IRQHandler (void);

uint8_t Hci_Vs_Cmd_App_Func(uint8_t cmd_code,
                            uint8_t length, uint8_t *data_buf,
                            uint8_t *result_length, uint8_t *result_data);

uint32_t rf_pulse_shape[4];

#define DMA_TRANSFER_LENGTH 7
uint8_t buffer[DMA_TRANSFER_LENGTH];
uint8_t buffer2[DMA_TRANSFER_LENGTH];

#define MAX_DATA_RX_TX_SIZE  300
uint8_t ptr_tmp_rd[MAX_DATA_RX_TX_SIZE];
uint8_t ptr_tmp_wr[MAX_DATA_RX_TX_SIZE];
uint8_t *temp_rd;
int size_rd;

#define DEF_LSAD_CHANNEL            1;
uint32_t lsad_num = DEF_LSAD_CHANNEL;

const struct rwip_eif_api uart_api =
{
    uart_read,
    uart_write,
    uart_flow_on,
    uart_flow_off,
};

/*
 * STRUCT DEFINITIONS
 *****************************************************************************************
 */
/* TX and RX channel class holding data used for asynchronous read and write data
 * transactions
 */

/*/ UART TX RX Channel */
struct uart_txrxchannel
{
    /*/ call back function pointer */
    void (*callback) (void *, uint8_t);

    /*/ Dummy data pointer returned to callback when operation is over. */
    void *dummy;
};

/*/ UART environment structure */
struct uart_env_tag
{
    /*/ tx channel */
    struct uart_txrxchannel tx;

    /*/ rx channel */
    struct uart_txrxchannel rx;

    /*/ error detect */
    uint8_t errordetect;

    /*/ external wakeup */
    bool ext_wakeup;
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*/ uart environment structure */
static struct uart_env_tag uart_env;

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void uart_init(uint32_t baud_rate, uint8_t txd_gpio, uint8_t rxd_gpio)
{
    GPIO->SRC_UART[0] = UART_RX_SRC_CONST_HIGH;

    UART->CTRL = UART_ENABLE;

    Sys_UART_GPIOConfig(UART, (GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE)
                        , txd_gpio, rxd_gpio);

    Sys_UART_Config(UART, Sys_UART_GetClk(), baud_rate,  (UART_TX_DMA_ENABLE |
                                                UART_RX_DMA_ENABLE));

    Sys_DMA_ChannelConfig(&DMA[RX_DMA_NUM], DMA_SRC_UART0 | DMA_DEST_ADDR_INCR_1 | WORD_SIZE_8BITS_TO_8BITS
                          | DMA_COMPLETE_INT_ENABLE,
                          DMA_TRANSFER_LENGTH, DMA_TRANSFER_LENGTH, (uint32_t)&UART->RX_DATA,
                          (uint32_t)&buffer2[0]);

    Sys_DMA_ChannelConfig(&DMA[TX_DMA_NUM], DMA_DEST_UART0 | DMA_SRC_ADDR_INCR_1 | WORD_SIZE_8BITS_TO_8BITS
                          | DMA_COMPLETE_INT_ENABLE,
                          DMA_TRANSFER_LENGTH, DMA_TRANSFER_LENGTH, (uint32_t)&buffer[0],
                          (uint32_t)&UART->TX_DATA);

    DMA[TX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;
    DMA[TX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;

    DMA[RX_DMA_NUM].STATUS = DMA_COMPLETE_INT_CLEAR;
    DMA[RX_DMA_NUM].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;

    /* Initialize RX and TX transfer callbacks */
    uart_env.rx.callback = NULL;
    uart_env.tx.callback = NULL;

    NVIC_ClearPendingIRQ(DMA0_IRQn);
    NVIC_EnableIRQ(DMA0_IRQn);
    NVIC_ClearPendingIRQ(DMA1_IRQn);
    NVIC_EnableIRQ(DMA1_IRQn);

    NVIC_SetPriority(DMA0_IRQn, 1);
    NVIC_SetPriority(DMA1_IRQn, 1);
}

void uart_flow_on(void)
{
}

bool uart_flow_off(void)
{
    return 1;
}

void uart_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
    /* Sanity check */
    ASSERT_ERR(bufptr != NULL);
    ASSERT_ERR(size != 0);
    ASSERT_ERR(callback != NULL);
    uart_env.rx.callback = callback;
    uart_env.rx.dummy    = dummy;

    temp_rd = bufptr;
    size_rd = size;

    Sys_DMA_ChannelConfig(&DMA[RX_DMA_NUM], DMA_SRC_UART0 | DMA_DEST_ADDR_INCR_1 | WORD_SIZE_8BITS_TO_8BITS
                          | DMA_COMPLETE_INT_ENABLE,
                          size, 0, (uint32_t)&UART->RX_DATA,
                          (uint32_t)bufptr);
    Sys_DMA_Mode_Enable(&DMA[RX_DMA_NUM], DMA_ENABLE);
}

void uart_write(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
    /* Sanity check */
    ASSERT_ERR(bufptr != NULL);
    ASSERT_ERR(size != 0);
    ASSERT_ERR(callback != NULL);
    uart_env.tx.callback = callback;
    uart_env.tx.dummy    = dummy;

    Sys_DMA_ChannelConfig(&DMA[TX_DMA_NUM], DMA_DEST_UART0 | DMA_SRC_ADDR_INCR_1 | WORD_SIZE_8BITS_TO_8BITS
                          | DMA_COMPLETE_INT_ENABLE,
                          size, 0, (uint32_t)bufptr,
                          (uint32_t)&UART->TX_DATA);
    Sys_DMA_Mode_Enable(&DMA[TX_DMA_NUM], DMA_ENABLE);
}

void uart_isr(void)
{
    {
        void (*callback) (void *, uint8_t) = NULL;
        void *data = NULL;
        if (uart_rx_dma_done_getf() == 0x01)
        {
            uart_rx_dma_done_clr_setf(1);

            /* Retrieve callback pointer */
            callback = uart_env.rx.callback;
            data     = uart_env.rx.dummy;

            if (callback != NULL)
            {
                /* Clear callback pointer */
                uart_env.rx.callback = NULL;
                uart_env.rx.dummy    = NULL;

                /* Call handler */
                callback(data, RWIP_EIF_STATUS_OK);
            }
            else
            {
                ASSERT_ERR(0);
            }
        }

        if (uart_tx_dma_done_getf() == 0x01)
        {
            uart_tx_dma_done_clr_setf(1);

            /* Retrieve callback pointer */
            callback = uart_env.tx.callback;
            data     = uart_env.tx.dummy;

            if (callback != NULL)
            {
                /* Clear callback pointer */
                uart_env.tx.callback = NULL;
                uart_env.tx.dummy    = NULL;

                /* Call handler */
                callback(data, RWIP_EIF_STATUS_OK);
            }
            else
            {
                ASSERT_ERR(0);
            }
        }
    }
}

const struct rwip_eif_api * rwip_eif_get(uint8_t idx)
{
    const struct rwip_eif_api *ret = NULL;
    switch (idx)
    {
        case 0:
        {
            ret = &uart_api;
        }
        break;

        #if PLF_UART2
        case 1:
        {
            ret = &uart2_api;
        }
        break;

        #endif    /* PLF_UART2 */
        default:
        {
            ASSERT_INFO(0, idx, 0);
        }
        break;
    }
    return ret;
}

/*/ @} UART */

uint8_t Hci_Vs_Cmd_App_Func(uint8_t cmd_code,
                            uint8_t length, uint8_t *data_buf,
                            uint8_t *result_length, uint8_t *result_data)
{
    uint8_t status = CO_ERROR_NO_ERROR;
    *result_length = 0;
    uint32_t freq_hz;
    int8_t pwr_dBm;
    uint8_t vddpa_en;    /* 1 enabled request, 0, no enable is requested*/
    uint8_t txOrRx;    /* 0: Tx, 1:Rx */

    switch (cmd_code)
    {
        case HCI_VS_RF_CW_ENABLE_CMD_CODE:
        {
            txOrRx = data_buf[0];
            freq_hz = (data_buf[1] + (data_buf[2] << 8) + (data_buf[3] << 16) + (data_buf[4] << 24));

            uint32_t center_freq = (((float)freq_hz * (0x1 << 21)) / 144000000.0);

            RF0_REG08->BANK_BYTE = 0x4;

            rf_pulse_shape[0] = RF->TX_PULSE_SHAPE_1;
            rf_pulse_shape[1] = RF->TX_PULSE_SHAPE_2;
            rf_pulse_shape[2] = RF->TX_PULSE_SHAPE_3;
            rf_pulse_shape[3] = RF->TX_PULSE_SHAPE_4;

            RF->TX_PULSE_SHAPE_1 = 0;
            RF->TX_PULSE_SHAPE_2 = 0;
            RF->TX_PULSE_SHAPE_3 = 0;
            RF->TX_PULSE_SHAPE_4 = 0;

            RF->CENTER_FREQ = (CENTER_FREQ_ADAPT_CFREQ_ENABLE | center_freq);
            RF->CODING &= (~CHANNELS_2_EN_CHANNEL_SEL_ENABLE);
            RF->CODING &= (~CHANNELS_2_EN_CHN_BLE_ENABLE);
            RF0_REG00->MODE2_BYTE = 0;

            if (txOrRx == 0)
            {
                RF0_FSM_CTRL->FSM_MODE_BYTE = (FSM_MODE_MODE_CAL_PLL_TXRX | FSM_MODE_TX_NRX);
            }
            else
            {
                RF0_FSM_CTRL->FSM_MODE_BYTE = (FSM_MODE_MODE_CAL_PLL_TXRX | FSM_MODE_RX_NTX);
            }
        }
        break;

        case HCI_VS_RF_CW_DISABLE_CMD_CODE:
        {
            RF0_FSM_CTRL->FSM_MODE_BYTE = FSM_MODE_RESET;

            RF0_REG00->MODE2_BYTE = 0x8;
            RF0_REG08->BANK_BYTE = 0x4;

            RF->TX_PULSE_SHAPE_1 = rf_pulse_shape[0];
            RF->TX_PULSE_SHAPE_2 = rf_pulse_shape[1];
            RF->TX_PULSE_SHAPE_3 = rf_pulse_shape[2];
            RF->TX_PULSE_SHAPE_4 = rf_pulse_shape[3];

            RF->CENTER_FREQ = 0x8215c71b;
            RF->CODING |= (CHANNELS_2_EN_CHANNEL_SEL_ENABLE | CHANNELS_2_EN_CHN_BLE_ENABLE);
        }
        break;

        case HCI_VS_RF_OUTPUT_PWR_CMD_CODE:
        {
            pwr_dBm = (int8_t)data_buf[0];
            vddpa_en = data_buf[1];

            Sys_RFFE_SetTXPower(pwr_dBm, lsad_num, vddpa_en);
        }
        break;

        default:
        {
            status = CO_ERROR_INVALID_HCI_PARAM;
        }
        break;
    }

    return (status);
}
