Bluetooth Low Energy Central Client Sample Code
===============================================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This Bluetooth Low Energy application demonstrates a central device with one standard 
service (Battery service client) and one application-defined (custom) service client. 

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

1. Generates battery service and custom service clients
2. Initiates a connection with the device listed in `app.h`
3. Upon connection, the application starts a pairing/bonding procedure
4. Upon reconnection with a previously bonded device, the link is encrypted using the 
   keys saved on the device
5. Receives periodic updates of the battery level and custom service characteristics 
   from connected peer devices (peripherals)
6. If Updating PHY other than default 1Mbps is required, it can be enabled through 
   defining APP_PHY_UPDATE_REQ in `app.h` and the desired rate can be set
   through definitions APP_PREFERRED_PHY_RX and APP_PREFERRED_PHY_TX
   
**Advertisement Extension:** The application demonstrates advertisement extension capability on Long Range (coded PHY).
                             This can be enabled by setting `ADV_EXTENSION` to 1 in `app.h`. If pairing this with
                             a peripheral device, also set `ADV_EXTENSION` to 1 in `app.h` of the
                             ble_peripheral_server sample application.
                           
Logging Capability
------------------
This application uses the swmTace library to log information over UART using Rx pin (GPIO 5) and Tx pin (GPIO 6).

Application files
------------------
`app.h / app.c`: application definitions and the `main()` function  
`app_init.h / app_init.c`: device, libraries and application initialization   
`app_msg_handler.h / app_msg_handler.c`: Bluetooth Low Energy event handlers  
`app_customsc.h / app_customsc.c`: application-defined Bluetooth Low Energy 
                                             custom service client

Understanding the source code
-----------------------------
The source code is marked with comments. Some of them are numbered "Step 1", "Step 2", 
etc. Follow the sequence of numbered steps (and the log output over UART) to understand 
the application in detail.

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
