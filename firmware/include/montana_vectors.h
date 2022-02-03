/**
 * @file montana_vectors.h
 * @brief Interrupt vector information for Montana
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

#ifndef MONTANA_VECTORS_H
#define MONTANA_VECTORS_H

#if defined(__cplusplus)
extern "C"
  {
#endif

/* ----------------------------------------------------------------------------
 * Interrupt Vector Table Numbers
 *  - Equivalent to the offset from the base of the external interrupt list
 *  - Can be converted into an index into the vector table by adding 16
 * ------------------------------------------------------------------------- */

typedef enum IRQn
{
    Reset_IRQn                  = -15,  /*  1 Reset Vector */
    NonMaskableInt_IRQn         = -14,  /*  2 Non Maskable Interrupt */
    HardFault_IRQn              = -13,  /*  3 Hard Fault Interrupt */
    MemoryManagement_IRQn       = -12,  /*  4 Memory Management Interrupt */
    BusFault_IRQn               = -11,  /*  5 Bus Fault Interrupt */
    UsageFault_IRQn             = -10,  /*  6 Usage Fault Interrupt */
    SecureFault_IRQn            = -9,   /*  7 Usage Fault Interrupt */
    SVCall_IRQn                 = -5,   /* 11 SV Call Interrupt */
    DebugMonitor_IRQn           = -4,   /* 12 Debug Monitor Interrupt */
    PendSV_IRQn                 = -2,   /* 14 Pend SV Interrupt */
    SysTick_IRQn                = -1,   /* 15 System Tick Interrupt */
    WAKEUP_IRQn                 = 0,    /* 16 Wake-up interrupt */
    RTC_ALARM_IRQn              = 1,    /* 17 RTC alarm interrupt */
    RTC_CLOCK_IRQn              = 2,    /* 18 RTL clock interrupt */
    LSAD_MONITOR_IRQn           = 3,    /* 19 LSAD / Battery monitor interrupt */
    TIMER0_IRQn                 = 4,    /* 20 Timer0 interrupt */
    TIMER1_IRQn                 = 5,    /* 21 Timer1 interrupt */
    TIMER2_IRQn                 = 6,    /* 22 Timer2 interrupt */
    TIMER3_IRQn                 = 7,    /* 23 Timer3 interrupt */
    NFC_IRQn                    = 8,    /* 24 NFC interrupt */
    FIFO_IRQn                   = 9,    /* 25 FIFO interrupt */
    GPIO0_IRQn                  = 10,   /* 26 GPIO0 interrupt */
    GPIO1_IRQn                  = 11,   /* 27 GPIO1 interrupt */
    GPIO2_IRQn                  = 12,   /* 28 GPIO2 interrupt */
    GPIO3_IRQn                  = 13,   /* 29 GPIO3 interrupt */
    WATCHDOG_IRQn               = 14,   /* 30 Watchdog interrupt */
    SPI0_RX_IRQn                = 15,   /* 31 SPI receive interrupt */
    SPI0_TX_IRQn                = 16,   /* 32 SPI transmit interrupt */
    SPI0_COM_IRQn               = 17,   /* 33 SPI error interrupt */
    I2C0_IRQn                   = 18,   /* 34 I2C interrupt */
    UART0_RX_IRQn               = 19,   /* 35 UART receive interrupt */
    UART0_TX_IRQn               = 20,   /* 36 UART transmit interrupt */
    UART0_ERROR_IRQn            = 21,   /* 37 UART error interrupt */
    BLE_SW_IRQn                 = 22,   /* 38 BLE SW triggered interrupt, generated on SW request */
    BLE_FINETGT_IRQn            = 23,   /* 39 BLE fine timer target interrupt */
    BLE_TIMESTAMP_TGT1_IRQn     = 24,   /* 40 BLE time stamp target 1 interrupt generated on a defined instant interrupt */
    BLE_TIMESTAMP_TGT2_IRQn     = 25,   /* 41 BLE time stamp target 2 interrupt generated on a defined instant interrupt */
    BLE_CRYPT_IRQn              = 26,   /* 42 BLE encryption engine interrupt */
    BLE_SLP_IRQn                = 27,   /* 43 BLE sleep mode interrupt generated at end of sleep period */
    BLE_HSLOT_IRQn              = 28,   /* 44 BLE half slot interrupt, generated each 312.5�s */
    BLE_FIFO_IRQn               = 29,   /* 45 BLE Activity FIFO interrupt */
    BLE_ERROR_IRQn              = 30,   /* 46 BLE error interrupt */
    BLE_COEX_IN_PROCESS_IRQn    = 31,   /* 47 WLAN coexistence BLE in process interrupt */
    BLE_COEX_RX_TX_IRQn         = 32,   /* 48 WLAN coexistence Rx/Tx interrupt */
    TOF_IRQn                    = 33,   /* 49 Time of Flight counter interrupt */
    RF_TX_IRQn                  = 34,   /* 50 RF end of packet transmission interrupt */
    RF_RXSTOP_IRQn              = 35,   /* 51 RF RX FSM stop interrupt */
    RF_IRQ_RECEIVED_IRQn        = 36,   /* 52 RF packet receive interrupt */
    RF_SYNC_IRQn                = 37,   /* 53 RF sync word detection interrupt */
    RF_TXFIFO_IRQn              = 38,   /* 54 TX FIFO near underflow detect interrupt */
    RF_RXFIFO_IRQn              = 39,   /* 55 RX FIFO near overflow detect interrupt */
    FLASH0_COPY_IRQn            = 40,   /* 56 Flash0 copy interrupt */
    FLASH0_ECC_IRQn             = 41,   /* 57 Flash0 ECC event interrupt */
    FLASH1_COPY_IRQn            = 42,   /* 58 Flash1 copy interrupt */
    FLASH1_ECC_IRQn             = 43,   /* 59 Flash1 ECC event interrupt */
    ACCESS_ERROR_IRQn           = 44,   /* 60 Memory error interrupt */
    FPU_IRQn                    = 45,   /* 61 Floating Point Unit exception interrupt */
    DMA0_IRQn                   = 46,   /* 62 DMA Channel 0 interrupt */
    DMA1_IRQn                   = 47,   /* 63 DMA Channel 1 interrupt */
    DMA2_IRQn                   = 48,   /* 64 DMA Channel 2 interrupt */
    DMA3_IRQn                   = 49,   /* 65 DMA Channel 3 interrupt */
    CC312_IRQn                  = 50,   /* 66 Crypto Cell (CC312) interrupt */
    ASCC_PHASE_IRQn             = 51,   /* 67 ASCC Phase Interrupt */
    ASCC_PERIOD_IRQn            = 52,   /* 68 ASCC Period Interrupt */

} IRQn_Type;

/** Define the IRQ vector with the lowest number (will always be the reset IRQ) */
#define NVIC_FIRST_VECTOR       Reset_IRQn

/** Define the IRQ vector with the largest number (will be the last item in IRQn_Type) */
#define NVIC_LAST_VECTOR        ASCC_PERIOD_IRQn

/** Define the size of the NVIC vector table */
#define NVIC_VECTOR_TABLE_SIZE  ((NVIC_LAST_VECTOR - NVIC_FIRST_VECTOR) + 1)
                                  
#if defined(__cplusplus)          
}                                 
#endif

#endif /* MONTANA_VECTORS_H */
