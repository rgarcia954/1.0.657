UART Sample Code
================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This sample project demonstrates how to send and receive data using the UART 
interrupts and the DMA.

1.  By default, the UART interface is configured with TX and RX interrupts for
    a baud rate of 115,200. Setting the `UART_DMA_ENABLED` symbol to '1' in
    `app.h` enables the DMA mode. 

2.  Initially, the UART instance transmits a UART message containing the string 
    "onsemi UART". All the subsequent transmissions send the same
    strings received during receptions.
   
3.  The UART instance starts the communication by calling the `UART_StartTX` 
    function. After the transmission is finished and as soon as there is some
    data received on the UART_RX_GPIO, it is copied to the `rx_buffer`. 
    
Hardware Requirements
---------------------
Connect UART TX GPIO to an external serial device's RX pin and UART RX GPIO to
the TX pin. A loopback configuration is also possible:

`UART_TX_GPIO`(GPIO6)  <->  `UART_RX_GPIO`(GPIO5)

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Integrated Development Environment (IDE) User's Guide* guide for more 
information.
  
Verification
------------
To verify that the application is working correctly, load the sample code on 
the device and connect the UART interface, as detailed in the 
*Hardware Requirements* section. 

For a loopback configuration, set a breakpoint at the `UART_StartTX` function
call in the `UART_EchoRX` function. When the breakpoint is triggered, read the 
received string in the `rx_buffer` variable. The received string should always
read "onsemi UART".

For a configuration with an external serial device, run script `UART.py` or
open any serial terminal at the appropriate COM port. Executing the application
after opening the terminal should show the first received transmission of 
"onsemi UART" string. From here on, input any null-terminated string
of length 22 (21 + 1) characters. The entered string should be echoed back on
the terminal.

NOTE: Python3 is required to run the `UART.py` script.
 
The source code exists in `app.c`, `app_uart_cfg.c`, `app.h`, and 
`app_uart_cfg.h`. 

***
Copyright (c) 2021 Semiconductor Components Industries, LLC
(d/b/a onsemi). 
