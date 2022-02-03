UART CMSIS-Driver Sample Code
=============================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------
This sample project demonstrates how to send and receive data using the UART 
CMSIS-Driver.

1.  The UART interface is configured with TX and RX interrupts for a 
    baud rate of 115,200.

2.  The UART instance transmits/receives a UART message containing the string 
    `UART CMSIS TEST`.
   
3.  The callback function defined for the UART instance is 
    `UART_EventCallBack`. This function is called by the driver when a UART 
    event occurs, such as `ARM_USART_EVENT_RECEIVE_COMPLETE` or 
    `ARM_USART_EVENT_SEND_COMPLETE`.
   
4.  The UART instance starts the communication by calling the `USART_Send` 
    function. Once the transfer is finished, the application starts a receive 
    operation to read and check the received data. 

5.  Upon a successful transmission, the UART CMSIS-Driver sends an
    `ARM_USART_EVENT_SEND_COMPLETE` event to the application, using the 
    callback function. Similarly, upon a successful reception, an 
    `ARM_USART_EVENT_RECEIVE_COMPLETE` event is sent to the application.

6.  The application uses `APP_UART_SEND_GPIO` and `APP_UART_RECEIVE_GPIO` defined
    in `app.h` to provide the status of the transmission. One pulse is generated
    to signal successful transmission and one for successful reception. The values of 
   `BLUE_LED` and `GREEN_LED` are defined in RTE\Device\{device_name}\RTE_Device.h

**UART Configurations:**

The UART CMSIS-driver can be configured to send and receive data using Direct Memory Access (DMA),
or configured to use the main memory CPU for data transfers within the UART peripheral interface.
Verification of UART with and without DMA enabled follow the same Verification guidelines in the 
"Verification Section" below.

*1. Disable DMA:*

	a. Open RTE_Device.h with CMSIS configuration wizard.
	b. In DMA Configuration disable DMA channels DMA0 and DMA1.
	c. In USART Conifguration disable USART0 DMA Control in the USART0 dropdown options.

*2. Enable DMA:*

	a. Open RTE_Device.h with CMSIS configuration wizard.
	b. In DMA Configuration enable DMA channels DMA0 and DMA1.
	c. In USART Configuration enable USART0 DMA Control in the USART0 dropdown options.
	

Hardware Requirements
---------------------
The UART and GPIO pins can be configured in `RTE_Device.h`. The `APP_UART_SEND_GPIO`, `APP_UART_RECEIVE_GPIO`
and the `RECOVERY_GPIO` can be configured in `app.h`.

Connect UART in a loopback fashion as follows:

    UART_TX(GPIO6)  <->  UART_RX(GPIO5)
    
Connect UART for two evaluation boards as follows:

    1st UART board        2nd UART board
    UART_RX(GPIO5)  ->    UART_TX(GPIO6)
    UART_TX(GPIO6)  ->    UART_RX(GPIO5)

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Getting Started Guide* for your IDE for more information.

Verification
------------

**1. UART Verification (DMA disabled):**

To verify that the application is working correctly, connect an Evaluation and
Development Board with the UART application configured to a baud rate of 
115200, and use it to send the string message "UART CMSIS TEST". A loopback 
can be used, or another Evaluation and Development Board with the UART 
program loaded. When the button(GPIO0) is pressed, it sends "UART CMSIS TEST". 
Each time "UART CMSIS TEST" is transmitted, the `APP_UART_SEND_GPIO` toggles
and each time "UART CMSIS TEST" is received, the `APP_UART_RECEIVE_GPIO` toggles. 
If two UART applications are used, pressing the button on one Evaluation and 
Development Board causes the GPIO of the other Evaluation and Development Board
to toggle.

**2. UART Verification (DMA enabled):**

As stated above, the same verification method is used when DMA is enabled. The `APP_UART_SEND_GPIO`
toggles when "UART CMSIS TEST" is transmitted and `APP_UART_RECEIVE_GPIO` toggles each time "UART CMSIS TEST"
is received.  
Verification that the DMA channels are being used to transfer the data can be found in the
sample application where a third LED flash occurs, indicating that the DMA channel interrupts are complete.
This means for the UART verification with DMA enabled, will include the flashing of three LED's on the RSL15 board.
The first flash when the string is transmitted, the second when the string has been received, and the third to 
indicate the DMA channels were used for data transfer.
 
The source code exists in `app.c` and `app.h`. The driver source files are 
located in the `drivers` library's `code` and `include` folders. 
The driver configuration is defined in `RTE_Device.h` (to open this 
configuration file in GUI mode, right-click it and select **Open with** > 
**CMSIS Configuration Wizard**).

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Low Power Mode or resetting continuously (either by design
or due to programming error). To circumvent this scenario, a software recovery
mode using RECOVERY_GPIO can be implemented with the following steps:

1. Connect RECOVERY_GPIO to ground (this can be achieved by pressing and holding 
   SW1 pushbutton on the evaluation board).
2. Press the RESET button (this restarts the application, which pauses at the
   start of its initialization routine).
3. Re-flash the board. After successful re-flashing, disconnect RECOVERY_GPIO
   from ground (releasing SW1 pushbutton on the evaluation board), and press the
   RESET button so that the application can work properly.

***
  Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
  onsemi), All Rights Reserved
