; @file startup.s
; @brief CMSIS Cortex-M33 Core Device start-up file for the ARM  Cortex-M33
; processor (standard ARM development tools)
; 
; Provides the required system_<Device>.h implementation for CMSIS
; compatibility
; 
; @copyright @parblock
; Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
; onsemi), All Rights Reserved
;
; This code is the property of onsemi and may not be redistributed
; in any form without prior written permission from onsemi.
; The terms of use and warranty for this code are covered by contractual
; agreements between onsemi and the licensee.
; 
; @endparblock
;



                PRESERVE8
                THUMB

; ----------------------------------------------------------------------------
; Vector Table 
; - Mapped to address 0 at Reset
; ----------------------------------------------------------------------------

                AREA    RESET, CODE, READONLY
                IMPORT  ||Image$$ARM_LIB_STACK$$ZI$$Limit||
                EXPORT  ISR_Vector_Table
                EXPORT  ISR_Vector_Table_End
                EXPORT  ISR_Vector_Table_Size

ISR_Vector_Table       
                DCD   ||Image$$ARM_LIB_STACK$$ZI$$Limit|| ; Top of Stack
                DCD   Reset_Handler                       ;  1 Reset Handler
                DCD   NMI_Handler                         ;  2 Non-Maskable Interrupt Handler
                DCD   HardFault_Handler                   ;  3 Hard Fault Handler
                DCD   MemManage_Handler                   ;  4 Mem Manage Fault Handler
                DCD   BusFault_Handler                    ;  5 Bus Fault Handler
                DCD   UsageFault_Handler                  ;  6 Usage Fault Handler
                DCD   SecureFault_Handler                 ;  7 Secure Fault Handler
                DCD   0                                   ;  8 Reserved
                DCD   0                                   ;  9 Reserved
                DCD   0                                   ; 10 Reserved
                DCD   SVC_Handler                         ; 11 Supervisor Call Handler
                DCD   DebugMon_Handler                    ; 12 Debug Monitor Handler
                DCD   0                                   ; 13 Reserved
                DCD   PendSV_Handler                      ; 14 PendSV Handler
                DCD   SysTick_Handler                     ; 15 SysTick Handler

                ; External Interrupts
                DCD   WAKEUP_IRQHandler                   ; 16 Wake-up interrupt
                DCD   RTC_ALARM_IRQHandler                ; 17 RTC alarm interrupt
                DCD   RTC_CLOCK_IRQHandler                ; 18 RTC clock interrupt
                DCD   LSAD_MONITOR_IRQHandler             ; 19 LSAD Battery monitor interrupt
                DCD   TIMER0_IRQHandler                   ; 20 Timer0 interrupt
                DCD   TIMER1_IRQHandler                   ; 21 Timer1 interrupt
                DCD   TIMER2_IRQHandler                   ; 22 Timer2 interrupt
                DCD   TIMER3_IRQHandler                   ; 23 Timer3 interrupt
                DCD   NFC_IRQHandler                      ; 24 NFC interrupt
                DCD   FIFO_IRQHandler                     ; 25 FIFO interrupt
                DCD   GPIO0_IRQHandle                     ; 26 GPIO0 interrupt
                DCD   GPIO1_IRQHandler                    ; 27 GPIO1 interrupt
                DCD   GPIO2_IRQHandler                    ; 28 GPIO2 interrupt
                DCD   GPIO3_IRQHandler                    ; 29 GPIO3 interrupt
                DCD   WATCHDOG_IRQHandler                 ; 30 Watchdog interrupt
                DCD   SPI0_RX_IRQHandler                  ; 31 SPI receive interrupt
                DCD   SPI0_TX_IRQHandler                  ; 32 SPI transmit interrupt
                DCD   SPI0_COM_IRQHandler                 ; 33 SPI error interrupt
                DCD   I2C0_IRQHandler                     ; 34 I2C interrupt
                DCD   UART0_RX_IRQHandler                 ; 35 UART receive interrupt
                DCD   UART0_TX_IRQHandler                 ; 36 UART transmit interrupt
                DCD   UART0_ERROR_IRQHandler              ; 37 UART error interrupt
                DCD   BLE_SW_IRQHandler                   ; 38 SW triggered interrupt, generated on SW request
                DCD   BLE_FINETGT_IRQHandler              ; 39 BLE fine timer target interrupt
                DCD   BLE_TIMESTAMP_TGT1_IRQHandler       ; 40 BLE timestamp target 1 interrupt generated on a defined instant
                DCD   BLE_TIMESTAMP_TGT2_IRQHandler       ; 41 BLE timestamp target 2 interrupt generated on a defined instant
                DCD   BLE_CRYPT_IRQHandler                ; 42 BLE encryption engine interrupt
                DCD   BLE_SLP_IRQHandler                  ; 43 BLE sleep mode interrupt generated at end of sleep period
                DCD   BLE_HSLOT_IRQHandler                ; 44 BLE half slot interrupt, generated each 312.5�s
                DCD   BLE_FIFO_IRQHandler                 ; 45 Activity FIFO interrupt
                DCD   BLE_ERROR_IRQHandler                ; 46 BLE error interrupt
                DCD   BLE_COEX_IN_PROCESS_IRQHandler      ; 47 WLAN coexistence BLE in process interrupt
                DCD   BLE_COEX_RX_TX_IRQHandler           ; 48 WLAN coexistence Rx/Tx interrupt
                DCD   TOF_IRQHandler                      ; 49 Time of Flight counter interrupt
                DCD   RF_TX_IRQHandler                    ; 50 RF end of packet transmission interrupt
                DCD   RF_RXSTOP_IRQHandler                ; 51 RF RX FSM stop interrupt
                DCD   RF_IRQ_RECEIVED_IRQHandler          ; 52 RF packet receive interrupt
                DCD   RF_SYNC_IRQHandler                  ; 53 RF sync word detection interrupt
                DCD   RF_TXFIFO_IRQHandler                ; 54 TX FIFO near underflow detect interrupt
                DCD   RF_RXFIFO_IRQHandler                ; 55 RX FIFO near overflow detect interrupt
                DCD   FLASH0_COPY_IRQHandler              ; 56 Flash0 copy interrupt
                DCD   FLASH0_ECC_IRQHandler               ; 57 Flash0 ECC event interrupt
                DCD   FLASH1_COPY_IRQHandler              ; 58 Flash1 copy interrupt
                DCD   FLASH1_ECC_IRQHandler               ; 59 Flash1 ECC event interrupt
                DCD   ACCESS_ERROR_IRQHandler             ; 60 Memory access error interrupt
                DCD   FPU_IRQHandler                      ; 61 Floating Point Unit exception interrupt
                DCD   DMA0_IRQHandler                     ; 62 DMA Channel 0 interrupt
                DCD   DMA1_IRQHandler                     ; 63 DMA Channel 1 interrupt
                DCD   DMA2_IRQHandler                     ; 64 DMA Channel 2 interrupt
                DCD   DMA3_IRQHandler                     ; 65 DMA Channel 2 interrupt
                DCD   CC312_IRQHandler                    ; 66 Crypto Cell (CC312) interrupt
                DCD   ASCC_PHASE_IRQHandler               ; 67 ASCC Phase Interrupt
                DCD   ASCC_PERIOD_IRQHandler              ; 68 ASCC Period Interrupt
ISR_Vector_Table_End

ISR_Vector_Table_Size  EQU     ISR_Vector_Table_End - ISR_Vector_Table

; ----------------------------------------------------------------------------
; Reset Handler
; ----------------------------------------------------------------------------
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; ----------------------------------------------------------------------------
; Place-holder Exception Handlers
; - Weak definitions
; - If not modified or replaced, these handlers implement infinite loops
; ----------------------------------------------------------------------------
NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP

HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP

MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP

BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP

UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
				
SecureFault_Handler\
                PROC
				EXPORT SecureFault_Handler        [WEAK]
				B       .
				ENDP

SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP

DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP

PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP

SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

; ----------------------------------------------------------------------------
; Default Interrupt Request Handlers for External Interrupts
; ----------------------------------------------------------------------------
Default_Handler PROC

                EXPORT WAKEUP_IRQHandler
                EXPORT RTC_ALARM_IRQHandler
                EXPORT RTC_CLOCK_IRQHandler
                EXPORT LSAD_MONITOR_IRQHandler
                EXPORT TIMER0_IRQHandler
                EXPORT TIMER1_IRQHandler
                EXPORT TIMER2_IRQHandler
                EXPORT TIMER3_IRQHandler
                EXPORT NFC_IRQHandler
                EXPORT FIFO_IRQHandler
                EXPORT GPIO0_IRQHandler
                EXPORT GPIO1_IRQHandler
                EXPORT GPIO2_IRQHandler
                EXPORT GPIO3_IRQHandler
                EXPORT WATCHDOG_IRQHandler
                EXPORT SPI0_RX_IRQHandler
                EXPORT SPI0_TX_IRQHandler
                EXPORT SPI0_COM_IRQHandler
                EXPORT I2C0_IRQHandler
                EXPORT UART0_RX_IRQHandler
                EXPORT UART0_TX_IRQHandler
                EXPORT UART0_ERROR_IRQHandler
                EXPORT BLE_SW_IRQHandler
                EXPORT BLE_FINETGT_IRQHandler
                EXPORT BLE_TIMESTAMP_TGT1_IRQHandler
                EXPORT BLE_TIMESTAMP_TGT2_IRQHandler
                EXPORT BLE_CRYPT_IRQHandler
                EXPORT BLE_SLP_IRQHandler
                EXPORT BLE_HSLOT_IRQHandler
                EXPORT BLE_FIFO_IRQHandler
                EXPORT BLE_ERROR_IRQHandler
                EXPORT BLE_COEX_IN_PROCESS_IRQHandler
                EXPORT BLE_COEX_RX_TX_IRQHandler
                EXPORT TOF_IRQHandler
                EXPORT RF_TX_IRQHandler
                EXPORT RF_RXSTOP_IRQHandler
                EXPORT RF_IRQ_RECEIVED_IRQHandler
                EXPORT RF_SYNC_IRQHandler
                EXPORT RF_TXFIFO_IRQHandler
                EXPORT RF_RXFIFO_IRQHandler
                EXPORT FLASH0_COPY_IRQHandler
                EXPORT FLASH0_ECC_IRQHandler
                EXPORT FLASH1_COPY_IRQHandler
                EXPORT FLASH1_ECC_IRQHandler
                EXPORT ACCESS_ERROR_IRQHandler
                EXPORT FPU_IRQHandler
                EXPORT DMA0_IRQHandler
                EXPORT DMA1_IRQHandler
                EXPORT DMA2_IRQHandler
                EXPORT DMA3_IRQHandler
                EXPORT CC312_IRQHandler
                EXPORT ASCC_PHASE_IRQHandler
                EXPORT ASCC_PERIOD_IRQHandler

WAKEUP_IRQHandler
RTC_ALARM_IRQHandler
RTC_CLOCK_IRQHandler
LSAD_MONITOR_IRQHandler
TIMER0_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER3_IRQHandler
NFC_IRQHandler
FIFO_IRQHandler
GPIO0_IRQHandler
GPIO1_IRQHandler
GPIO2_IRQHandler
GPIO3_IRQHandler
WATCHDOG_IRQHandler
SPI0_RX_IRQHandler
SPI0_TX_IRQHandler
SPI0_COM_IRQHandler
I2C0_IRQHandler
UART0_RX_IRQHandler
UART0_TX_IRQHandler
UART0_ERROR_IRQHandler
BLE_SW_IRQHandler
BLE_FINETGT_IRQHandler
BLE_TIMESTAMP_TGT1_IRQHandler
BLE_TIMESTAMP_TGT2_IRQHandler
BLE_CRYPT_IRQHandler
BLE_SLP_IRQHandler
BLE_HSLOT_IRQHandler
BLE_FIFO_IRQHandler
BLE_ERROR_IRQHandler
BLE_COEX_IN_PROCESS_IRQHandler
BLE_COEX_RX_TX_IRQHandler
TOF_IRQHandler
RF_TX_IRQHandler
RF_RXSTOP_IRQHandler
RF_IRQ_RECEIVED_IRQHandler
RF_SYNC_IRQHandler
RF_TXFIFO_IRQHandler
RF_RXFIFO_IRQHandler
FLASH0_COPY_IRQHandler
FLASH0_ECC_IRQHandler
FLASH1_COPY_IRQHandler
FLASH1_ECC_IRQHandler
ACCESS_ERROR_IRQHandler
FPU_IRQHandler
DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
CC312_IRQHandler
ASCC_PHASE_IRQHandler
ASCC_PERIOD_IRQHandler

                B       .
                ENDP

                ALIGN


                END

