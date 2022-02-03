Bluetooth Low Energy Peripheral Server Controller Privacy Sample Code
=====================================================================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This Bluetooth Low Energy application demonstrates controller privacy with a whitelist resolve list update in the peripheral role.

Application allows to configure BLE address type (public or private) and location to read the BLE address from. 
*	By default, private address type is selected (`GAPM_ADDRESS_TYPE` is configured as 
		`GAPM_CFG_ADDR_PRIVATE` in `app.h`), and the application will use `APP_BLE_PRIVATE_ADDR' 
		defined in `app.h` as the device private address.
*	BLE address type can be re-configured to public by changing `GAPM_ADDRESS_TYPE`
		from `GAPM_CFG_ADDR_PRIVATE` to `GAPM_CFG_ADDR_PUBLIC` in `app.h`
*	When public address type is selected, application will read the public address from a 
		location defined by `APP_BLE_PUBLIC_ADDR_LOC` in `app.h`. By default, `APP_BLE_PUBLIC_ADDR_LOC` 
		is configured to `BLE_PUBLIC_ADDR_LOC_MNVR` which is a location in MNVR. 
		However, any other valid locations can be used as needed.

Key operations performed by the application:

1. Supports controller privacy and own device Bluetooth address is configurable to use private    
   resolvable address or static address in `app.h`. By default GAPM_OWN_ADDR_TYPE is set to 
   GAPM_GEN_RSLV_ADDR to use a resolvable address. Tt can be changed to GAPM_STATIC_ADDR to
   use a static address instead.
2. Performs undirected connectable advertising and uses non discoverable advertising discovery 
   mode with filtering policy when whitelist/resolve list is enabled.
3. By default, maximum number of connections is configured to four and can be configured for 
   more number of connections in `app.h` (the Bluetooth Low Energy stack currently 
   supports ten connections)
4. Any central device, while this application is in disabled whiteList/resolveList mode, can  
   scan, connect, pair/bond/encrypt (legacy or secure bond)
5. Only a previously bonded central device, while this application is in enabled  
   whitelist/resolveList mode can scan, connect, pair/bond/encrypt (legacy or secure bond)
    
NOTE1: At the start of the application, the whitelist is enabled by default. Based
       on the whitelist timer, the mode switches between whitelits/resolvelist disable mode 
       and enable mode periodically. 
      
NOTE2: You can add a maximum of three devices to the resolvelist alone and ten devices to the 
       whitelist alone per current stack support, provided the total number of connections is 
       ten or less. However, this application has specified a maximum of four connections by 
       default which can be configured to more or less number of connections.

Logging Capability
------------------
This application uses the swmTrace library to log information over UART using Rx pin (GPIO 5) and Tx pin (GPIO 6).

Application files
------------------
`app.h/app.c`: application definitions and the `main()` function  
`app_init.h/app_init.c`: device, libraries and application initialization   
`app_msg_handler.h/app_msg_handler.c`: Bluetooth Low Energy event handlers  

Understanding the source code
-----------------------------
The source code is marked with comments. Some of them are numbered "Step 1", "Step 2", 
etc. Follow the sequence of numbered steps (and the log output over UART) to understand 
the application in detail.

Bluetooth Low Energy Abstraction
----------------
This application takes advantage of the Bluetooth Low Energy Abstraction layer, on top of 
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
