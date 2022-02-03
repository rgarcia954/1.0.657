/**
 * @file app_uart_cfg.h
 * @brief UART configuration and transfer functions
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
#ifndef APP_UART_CFG
#define APP_UART_CFG

/* ---------------------------------------------------------------------------
* Defines
* --------------------------------------------------------------------------*/
#define UART_RX_DMA_NUM                 0
#define UART_TX_DMA_NUM                 1

/* ---------------------------------------------------------------------------
* Function prototype definitions
* --------------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------
 * Function      : UART_Init(void)
 * ----------------------------------------------------------------------------
 * Description   : Initializes UART to known configuration
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART_Init(void);

/* ----------------------------------------------------------------------------
 * Function      : UART_Config_Int(void)
 * ----------------------------------------------------------------------------
 * Description   : Configures UART with RX and TX interrupts enabled to
 *                 manually handle the RX and TX buffers
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART_Config_Int(void);

/* ----------------------------------------------------------------------------
 * Function      : UART_Config_DMA(void)
 * ----------------------------------------------------------------------------
 * Description   : Configures UART with DMA TX and DMA RX request lines enabled
 *                 to automatically handle the RX and TX buffers using the DMA
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART_Config_DMA(void);

/* ----------------------------------------------------------------------------
 * Function      : UART_StartTX(void)
 * ----------------------------------------------------------------------------
 * Description   : Kicks off a UART transmission
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART_StartTX(void);

/* ----------------------------------------------------------------------------
 * Function      : UART_EchoRX(void)
 * ----------------------------------------------------------------------------
 * Description   : Echoes back newly received UART RX data over UART TX
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART_EchoRX(void);

/* ----------------------------------------------------------------------------
 * Function      : UART_RX_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : UART RX interrupt handler
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART0_RX_IRQHandler(void);

/* ----------------------------------------------------------------------------
 * Function      : UART_TX_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : UART TX interrupt handler
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void UART0_TX_IRQHandler(void);

/* ----------------------------------------------------------------------------
 * Function      : DMA0_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : DMA interrupt handler
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void DMA0_IRQHandler(void);

#endif    /* ifndef APP_UART_CFG */
