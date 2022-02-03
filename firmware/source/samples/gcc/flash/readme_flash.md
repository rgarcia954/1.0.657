Flash Sample Code
=================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in Software_Use_Agreement.rtf in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This sample project demonstrates how to erase and write to flash in 
user application using the flash library routines that are linked into the
ROM (accessible through flash_rom.h).

1.  Function App_Demo_SequentialWrite performs a sector erase followed by writing
    data to the address passed in sequential mode.

2.  Function App_Demo_WordWrite performs a sector erase followed by writing
    data to the address passed on a word by word basis.
    
3.  If all demo operations was successful program execution waits in the 
    forever loop at the end of application. 
    
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
the device and place a breakpoint at forever loop at the end of the application. 

Using IDE memory view verify that the content of data in flash match the 
contents of the pattern array.

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
