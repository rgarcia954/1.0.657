Trustzone Secure Sample Code
============================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------
This sample project demonstrates a secure application can configure the
trustzone and start a non-secure application. This application requires that 
trustzone_non_secure sample application to be built and flashed to memory.

1.  The secure application configures the SAU and IDAU with the security
    attributes. 

2.  The secure application exports to non-secure-call-able apis
    App_Secure_Api_Inc and  App_Secure_Api_Dec and places the secure
    gate way functions in the non-secure-call-able memory 

3.  The secure application configures the peripherals in this case
    GPIO & UART to be accessible by the non-secure application
   
4.  The secure application runs the non-secure application

5.  The steps to load non_secure hex and execute secure code from debugger are as below
      i) Load non_secure code from debugger, with checkboxes like “Pre-run/Restart 
         reset” and “Initial Reset and Halt” unchecked and “Continue” checked in 
         Debug Configurations->Startup
      ii)Load and run secure code
      
NS_STATE_GPIO and S_STATE_GPIO values are defined in 
Project Properties -> C/C++ Build -> Settings -> Preprocessor.

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
1. Set the breakpoint at the App_Secure_Api_Inc(int x) and App_Secure_Api_Dec(int x) functions to see that it returns an incremented value and decremented value of the input parameter respectively as defined in the non secure region.
2. Connect an oscilloscope or logic analyzer to GPIO defined in `NS_STATE_GPIO`.
Verify that the GPIO state toggles when App_call_GPIO_Demo() function is called.

NOTE: To be able to step into non-secure code inorder to debug secure and non secure code at the same time:
Below line should be added in the "Debug configuration -> Startup -> Run/Restart Commands box,
add-symbol-file "workspace_location\trustzone_non_secure\Debug\trustzone_non_secure.elf" 0x120000
*However adding break point in non-secure code is not possible*

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Low Power Mode or resetting continuously (either by design 
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
