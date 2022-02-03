HCI Application Sample Code
===========================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------
This is an application used for testing BLE functionality and performance via HCI and DTM.
   
Application files
------------------
`app.h / app.c`: application definitions and the `main()` function  
`ble_protocol_support.h / ble_protocol_support.c`: Provide Bluetooth protocol related parameters   
`stubprf.h / stubprf.c`: Contains an empty BLE profile definition. 
`uart_hci.h / uart_hci.c`: initializes UART and handles HCI commands.

Understanding the source code
-----------------------------
The source code is marked with comments. The main() function in app.c initializes the device,
including UART and Bluetooth connection. uart_hci.c contains the UART code.

Bluetooth Low Energy Abstraction
---------------------------------
This application takes advantage the Bluetooth Low Energy Abstraction layer, on top of 
the Bluetooth Low Energy stack. This provides an extra layer of simplicity and 
modularization to your firmware design, so you can focus on your specific application.

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Power Mode or resetting continuously (either by design 
or due to programming error). To circumvent this scenario, a software recovery
mode using RECOVERY_GPIO can be implemented with the following steps:

1.  Connect RECOVERY_GPIO to ground (this can be achieved by pressing and holding 
    SW1 pushbutton on the evaluation board).
2.  Press the RESET button (this restarts the application, which pauses at the
    start of its initialization routine).
3.  Re-flash the board. After successful re-flashing, disconnect RECOVERY_GPIO 
    from ground (releasing SW1 pushbutton on the evaluation board), and press the 
    RESET button so that the application can work properly.

***
Copyright (c) 2021 Semiconductor Components Industries, LLC
(d/b/a onsemi).