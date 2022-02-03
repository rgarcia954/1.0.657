; -----------------------------------------------------------------------------
; Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
; onsemi), All Rights Reserved
;
; This code is the property of onsemi and may not be redistributed
; in any form without prior written permission from onsemi.
; The terms of use and warranty for this code are covered by contractual
; agreements between onsemi and the licensee.
;
; This is Reusable Code.
;
; -----------------------------------------------------------------------------
; startup.s
; - CMSIS Cortex-M33 Core Device start-up file for the ARM Cortex-M33 processor
;   (standard ARM development tools)
; -----------------------------------------------------------------------------

                MODULE   ?cstartup

                ;; Forward declaration of sections.
                SECTION  CSTACK:DATA:NOROOT(3)

                SECTION  .intvec:CODE:NOROOT(2)
                EXTERN   __iar_program_start
                EXTERN   SystemInit


                PUBLIC   __vector_table
                PUBLIC   __vector_table_End
                PUBLIC   __vector_table_Size
                PUBLIC   ISR_Vector_Table
                
                
                DATA

__vector_table
                DCD     sfe(CSTACK)
                DCD   Reset_Handler                 ;  1 Reset Handler
                DCD   NMI_Handler                   ;  2 Non-Maskable Interrupt Handler
                DCD   HardFault_Handler             ;  3 Hard Fault Handler
                DCD   MemManage_Handler             ;  4 Mem Manage Fault Handler
                DCD   BusFault_Handler              ;  5 Bus Fault Handler
                DCD   UsageFault_Handler            ;  6 Usage Fault Handler
                DCD   SecureFault_Handler           ;  7 Secure Fault Handler
                DCD   0                             ;  8 Reserved
                DCD   0                             ;  9 Reserved
                DCD   0                             ; 10 Reserved
                DCD   SVC_Handler                   ; 11 Supervisor Call Handler
                DCD   DebugMon_Handler              ; 12 Debug Monitor Handler
                DCD   0                             ; 13 Reserved
                DCD   PendSV_Handler                ; 14 PendSV Handler
                DCD   SysTick_Handler               ; 15 SysTick Handler

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
                DCD   GPIO0_IRQHandler                    ; 26 GPIO0 interrupt
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
                DCD   BLE_HSLOT_IRQHandler                ; 44 BLE half slot interrupt, generated each 312.5µs
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

__vector_table_End

__vector_table_Size  EQU      __vector_table_End - __vector_table
ISR_Vector_Table       EQU      __vector_table
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler
        
        PUBWEAK SecureFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SecureFault_Handler
        B SecureFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK WAKEUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
WAKEUP_IRQHandler  
        B WAKEUP_IRQHandler

        PUBWEAK RTC_ALARM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RTC_ALARM_IRQHandler  
        B RTC_ALARM_IRQHandler

        PUBWEAK RTC_CLOCK_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
RTC_CLOCK_IRQHandler  
        B RTC_CLOCK_IRQHandler

        PUBWEAK LSAD_MONITOR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
LSAD_MONITOR_IRQHandler  
        B LSAD_MONITOR_IRQHandler

        PUBWEAK TIMER0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER0_IRQHandler  
        B TIMER0_IRQHandler

        PUBWEAK TIMER1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER1_IRQHandler  
        B TIMER1_IRQHandler

        PUBWEAK TIMER2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER2_IRQHandler  
        B TIMER2_IRQHandler

        PUBWEAK TIMER3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER3_IRQHandler  
        B TIMER3_IRQHandler

        PUBWEAK NFC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
NFC_IRQHandler  
        B NFC_IRQHandler

        PUBWEAK FIFO_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FIFO_IRQHandler
        B FIFO_IRQHandler

        PUBWEAK GPIO0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
GPIO0_IRQHandler  
        B GPIO0_IRQHandler

        PUBWEAK GPIO1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
GPIO1_IRQHandler  
        B GPIO1_IRQHandler

        PUBWEAK GPIO2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
GPIO2_IRQHandler  
        B GPIO2_IRQHandler

        PUBWEAK GPIO3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
GPIO3_IRQHandler  
        B GPIO3_IRQHandler

        PUBWEAK WATCHDOG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
WATCHDOG_IRQHandler  
        B WATCHDOG_IRQHandler

        PUBWEAK SPI0_RX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
SPI0_RX_IRQHandler  
        B SPI0_RX_IRQHandler

        PUBWEAK SPI0_TX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
SPI0_TX_IRQHandler  
        B SPI0_TX_IRQHandler

        PUBWEAK SPI0_COM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
SPI0_COM_IRQHandler  
        B SPI0_COM_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C0_IRQHandler  
        B I2C0_IRQHandler

        PUBWEAK UART0_RX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
UART0_RX_IRQHandler  
        B UART0_RX_IRQHandler

        PUBWEAK UART0_TX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
UART0_TX_IRQHandler  
        B UART0_TX_IRQHandler

        PUBWEAK UART0_ERROR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
UART0_ERROR_IRQHandler  
        B UART0_ERROR_IRQHandler

        PUBWEAK BLE_SW_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)  
BLE_SW_IRQHandler  
        B BLE_SW_IRQHandler

        PUBWEAK BLE_FINETGT_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BLE_FINETGT_IRQHandler  
        B BLE_FINETGT_IRQHandler

        PUBWEAK BLE_TIMESTAMP_TGT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
BLE_TIMESTAMP_TGT1_IRQHandler  
        B BLE_TIMESTAMP_TGT1_IRQHandler

        PUBWEAK BLE_TIMESTAMP_TGT2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
BLE_TIMESTAMP_TGT2_IRQHandler  
        B BLE_TIMESTAMP_TGT2_IRQHandler

        PUBWEAK BLE_CRYPT_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
BLE_CRYPT_IRQHandler  
        B BLE_CRYPT_IRQHandler
        
        PUBWEAK BLE_SLP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
BLE_SLP_IRQHandler  
        B BLE_SLP_IRQHandler

        PUBWEAK BLE_HSLOT_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
BLE_HSLOT_IRQHandler  
        B BLE_HSLOT_IRQHandler

        PUBWEAK BLE_FIFO_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
BLE_FIFO_IRQHandler  
        B BLE_FIFO_IRQHandler

        PUBWEAK BLE_ERROR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
BLE_ERROR_IRQHandler  
        B BLE_ERROR_IRQHandler

        PUBWEAK BLE_COEX_IN_PROCESS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BLE_COEX_IN_PROCESS_IRQHandler  
        B BLE_COEX_IN_PROCESS_IRQHandler

        PUBWEAK BLE_COEX_RX_TX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
BLE_COEX_RX_TX_IRQHandler  
        B BLE_COEX_RX_TX_IRQHandler

        PUBWEAK TOF_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
TOF_IRQHandler  
        B TOF_IRQHandler

        PUBWEAK RF_TX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
RF_TX_IRQHandler  
        B RF_TX_IRQHandler

        PUBWEAK RF_RXSTOP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RF_RXSTOP_IRQHandler  
        B RF_RXSTOP_IRQHandler

        PUBWEAK RF_IRQ_RECEIVED_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RF_IRQ_RECEIVED_IRQHandler  
        B RF_IRQ_RECEIVED_IRQHandler

        PUBWEAK RF_SYNC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RF_SYNC_IRQHandler  
        B RF_SYNC_IRQHandler

        PUBWEAK RF_TXFIFO_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RF_TXFIFO_IRQHandler  
        B RF_TXFIFO_IRQHandler

        PUBWEAK RF_RXFIFO_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RF_RXFIFO_IRQHandler  
        B RF_RXFIFO_IRQHandler

        PUBWEAK FLASH0_COPY_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)   
FLASH0_COPY_IRQHandler  
        B FLASH0_COPY_IRQHandler

        PUBWEAK FLASH0_ECC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)   
FLASH0_ECC_IRQHandler  
        B FLASH0_ECC_IRQHandler

        PUBWEAK FLASH1_COPY_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
FLASH1_COPY_IRQHandler  
        B FLASH1_COPY_IRQHandler
      
        PUBWEAK FLASH1_ECC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
FLASH1_ECC_IRQHandler  
        B FLASH1_ECC_IRQHandler

        PUBWEAK ACCESS_ERROR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
ACCESS_ERROR_IRQHandler  
        B ACCESS_ERROR_IRQHandler

        PUBWEAK FPU_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
FPU_IRQHandler  
        B FPU_IRQHandler

        PUBWEAK DMA0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1) 
DMA0_IRQHandler  
        B DMA0_IRQHandler

        PUBWEAK DMA1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)    
DMA1_IRQHandler  
        B DMA1_IRQHandler

        PUBWEAK DMA2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA2_IRQHandler  
        B DMA2_IRQHandler

        PUBWEAK DMA3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA3_IRQHandler  
        B DMA3_IRQHandler

        PUBWEAK CC312_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CC312_IRQHandler  
        B CC312_IRQHandler

        PUBWEAK ASCC_PHASE_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ASCC_PHASE_IRQHandler  
        B ASCC_PHASE_IRQHandler

        PUBWEAK ASCC_PERIOD_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ASCC_PERIOD_IRQHandler  
        B ASCC_PERIOD_IRQHandler

        END
