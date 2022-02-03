Bluetooth Low Energy Peripheral Server Sample Code
==================================================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This Bluetooth Low Energy application demonstrates a peripheral device with two 
standard services (Battery service server and Device Information service server)
and two application-defined services (Customer Service 1 and Customer Service 2 servers).

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

1. Generates battery service, device information service and custom services.
2. Performs undirected connectable advertising
3. By default, it supports up to four simultaneous connections. This can be configured in 
`app.h` (the Bluetooth Low Energy stack currently supports ten connections)
4. Any central device can:  
    * Scan, connect, pair/bond/encrypt (legacy or secure bond)
    * Perform service discovery
    * Read characteristic values from battery, device information and custom services
5. Host privacy can be enabled in the application by changing `GAPM_OWN_ADDR_TYPE` from 
   `GAPM_STATIC_ADDR` to `GAPM_GEN_RSLV_ADDR` in `app.h`
6. The application sends periodic notifications of the battery level and custom service 
   characteristics to the connected peer devices (clients)
7. The Bluetooth Low Energy physical data rate is 1 Mbps by default. If different rates are
   desirable, enable physical data rate update by defining APP_PHY_UPDATE_REQ and define the
   desired rates in APP_PREFERRED_PHY_RX and APP_PREFERRED_PHY_TX in `app.h` 

**Custom Service 1:** This custom service on the peripheral includes four
                characteristics (i.e. `RX_VALUE`, `TX_VALUE`,`RX_VALUE_LONG`
                and `TX_VALUE_LONG`). The `RX_VALUE` characteristic sends a 
                notification with an incremental value every 10 seconds, 
                if the notification is enabled. The value written on the attribute 
                with the `TX_VALUE_LONG` characteristic name is inverted and 
                written back to the `RX_VALUE_LONG` by the custom service. This is
                handled by the `CUSTOMSS_MsgHandler` function in `app_customss.c`.
                For example, if you write 0x01 to the `TX_VALUE_LONG` characteristic, 
                then you can read the inverted value (i.e. 0xFE) using the 
                `RX_VALUE_LONG` characteristic.

**Custom Service 2:** This custom service on the peripheral includes three 
                characteristics (i.e. `TEMPERATURE_VALUE`, `LED_STATE` and `BUTTON_STATE`).
                The `TEMPERATURE_VALUE` characteristic sends a notification with an encoded 
                IEEE-11073 32-bit float temperature value every 10 seconds.
                A value of 0x00 or 0x01 written on the attribute with the `LED_STATE` characteristic
                name will turn off or on the LED connected with `LED_STATE_GPIO`, respectively. 
                In addition, each time a falling edge on GPIO0 is detected (i.e., a button press), 
                the `BUTTON_STATE` characteristic sends a notification with a value toggled
                between 0x00 and 0x01 to the peer connected device.

**Battery Service:** this service database is configured for a single battery 
                 instance. The application provides a callback function to read the battery level. 

The application notifies clients about the battery level on two occasions:

**Periodically:** the notification timeout is configured using the 
              `BASS_NotifyOnTimeout(TIMER_SETTING_S(BATT_UPDATE_TIMEOUT_S))` function
              in the `app_init.c`. The macro `TIMER_SETTING_S(value)` in `app.c` converts value
              in seconds to milliseconds. It is used to indicate that value has the unit of 
              seconds. The `BATT_UPDATE_TIMEOUT_S` is defined in `app_bass.h` and is set to the 
              value of 15 seconds.

**On battery level change:** The application monitors the battery level periodically and sends
                         a notification when a change is detected. The monitoring timeout is configured 
                         using the `BASS_NotifyOnBattLevelChange(TIMER_SETTING_S(BATT_CHANGE_TIMEOUT_S))` 
                         function in the `app_init.c`. The macro `TIMER_SETTING_S(value)` in `app.c` converts
                         value in seconds to milliseconds. It is used to indicate that value has the unit of 
                         seconds. The `BATT_CHANGE_TIMEOUT_S` is defined in `app_bass.h` and is set to the 
                         value of 5 seconds.


The message subscription mechanism allows the application and services to 
subscribe and receive callback notifications based on the Kernel message ID 
or task ID. This allows each module of the application to be independently 
implemented in its own files. The subscription is performed using the 
`MsgHandler_Add()` function. The application subscribes to receive GAPM and 
GAPC events. The services subscribe to receive Kernel events in 
their initialization function (see `BASS_Initialize()` in `ble_bass.c`, for an 
example). The application event handlers are implemented in `app_msg_handler.c`.

**Advertisement Extension:** The application demonstrates advertisement extension capability on Long Range (coded PHY).
                             This can be enabled by setting `ADV_EXTENSION` to 1 in `app.h`. If pairing this with
                             a central device, also set `ADV_EXTENSION` to 1 in `app.h` of the
                             ble_central_client sample application.

**Clearing Bond List:**
The bond list (stored in data flash area) can be cleared by connecting GPIO0 to ground for at least five seconds (this can be achieved by pressing and holding SW1 pushbutton on the evaluation board). After five seconds, if the bond list is successfully cleared, GPIO8 will toggle with a 100 millisecond interval between each toggle (the green LED on the evaluation board will blink). The toggling/blinking will stop once GPIO0 is disconnected from the ground (SW1 pushbutton on the evaluation board is released).

Logging Capability
------------------
This application uses the swmTace library to log information over UART using Rx pin (GPIO 5) and Tx pin (GPIO 6).

Application files
------------------
`app.h / app.c`: application definitions and the `main()` function  
`app_init.h / app_init.c`: device, libraries and application initialization   
`app_msg_handler.h / app_msg_handler.c`: Bluetooth Low Energy event handlers  
`app_customss.h / app_customss.c`: application-defined Bluetooth Low Energy 
                                   custom service server
`app_temperature_sensor.h / app_temperature_sensor.c`: Internal temperature
                                                     sensor utility class


Understanding the source code
-----------------------------
The source code is marked with comments. Some of them are numbered "Step 1", "Step 2", 
etc. Follow the sequence of numbered steps (and the log output over UART) to understand 
the application in detail.

Bluetooth Low Energy Abstraction
----------------
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
