LSAD Sample Code
================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This sample project demonstrates how the LSAD interface can be used
to covert analog signal to a digital value, that is processed and
sent out on the UART interface

1.  Function `LSAD_Demo_Config` the LSAD is configured to sample input
    connected to GPIO1, temperature sensor internally, thermistor
    via GPIO2 as well perform battery monitor operation.

2.  Function `process_lsad_new_samples` checks if a new event or data
    is available by the LSAD.
    
3.  `process_lsad_new_samples()` sends data out UART interface if an 
    event has occured on the LSAD
    
Hardware Requirements
---------------------
External 48 MHz Crystal/Oscillator is connected

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Integrated Development Environment (IDE) User's Guide* guide for more 
information.
  
Verification
------------
To verify that the application is working correctly, load the sample code on 
the device; connect the device with your computer using UART interface;
select 115200 baud rate and monitor incoming messages displayed on your
computer's serial terminal. After each conversion cycle a new
message is received with the corresponding voltage value in milli-volts
at the GPIO1 input. 

The internal temperature sensor will also report a result that can be monitored.

A thermistor or resistor can be connected to GPIO2. The app will report the
measured resistance. 

Reduce VBAT voltage and verify that battery alarm messages are sent at
when voltage reaches approximately 2.8V.

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
