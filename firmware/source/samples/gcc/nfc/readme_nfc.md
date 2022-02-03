NFC Sample Code
===============

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This sample project demonstrates how the NFC interface can be used
to emulate an NFC tag type 2 with a NDEF message that 
contains the URI http://www.onsemi.com

1.  Function NFC_Initialize configures the NFC interface and sets up
    the tables for Layer 3 response.

2.  Function NFC_Task polls the NFC interface and handles incoming
    requests.

    
Hardware Requirements
---------------------
External NFC antenna is connected to the EVB connector U5 

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Integrated Development Environment (IDE) User's Guide* guide for more 
information.
  
Verification
------------
To verify that the application is working correctly, load the sample code on 
the device and approach a card reader or a phone. The card reader should be able to
detect the card UUID and connect to the card and open URL http://www.onsemi.com.

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Power Mode or resetting continuously (either by design 
or due to programming error). To circumvent this scenario, a software recovery
mode using RECOVERY_GPIO can be implemented with the following steps:

1.  Connect RECOVERY_GPIO to ground.
2.  Press the RESET button (this restarts the application, which pauses at the
    start of its initialization routine).
3.  Re-flash the board. After successful re-flashing, disconnect RECOVERY_GPIO from
    ground, and press the RESET button so that the application can work
    properly.

***
Copyright (c) 2021 Semiconductor Components Industries, LLC
(d/b/a onsemi). 