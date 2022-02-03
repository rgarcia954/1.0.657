Trustzone Non-Secure Sample Code
================================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------
This sample project demonstrates a non secure application that is executed
from a secure application from boot. This application requires the 
trust_zone_secure sample application to be open and built in the project 
environment. The non-secure application generates the secure gateway veneer
which is linked here. 

1.  App_Call_Secure_APIs_Demo function demonstrates how user can
    write secure API which can be called by non-secure application

2.  The GPIO & UART access is configured by the secure application    

NS_STATE_GPIO and S_STATE_GPIO values are defined in 
Project Properties -> C/C++ Build -> Settings -> Preprocessor.

Note: In-order to run in debug mode the secure application should run first
and should load the non-secure application. To debug the non-secure 
application from the secure application, configure a debug configuration to
load the "Trustzone Secure" sample code and to load symbols from the "Trustzone Non-Secure" application.

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
1. Connect an oscilloscope or logic analyzer to GPIO defined in `NS_STATE_GPIO`.
Verify that the GPIO state toggles when App_call_GPIO_Demo() function is called.
2. Connect the UART interface to see the received string on serial terminal
which should always read "onsemi UART".

***
Copyright (c) 2021 Semiconductor Components Industries, LLC
(d/b/a onsemi). 
