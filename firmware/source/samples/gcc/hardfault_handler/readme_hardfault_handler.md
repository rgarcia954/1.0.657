HardFault Handler Sample Code
=============================

NOTE: If you use this sample application for your own purposes, follow
      the licensing agreement specified in `Software_Use_Agreement.rtf`
      in the home directory of the installed Software
      Development Kit (SDK).

Overview
--------
This sample project executes a hardfault, then prints debug information via semi-hosting.
The following information is printed
- The contents of registers R0-R3, R12, PC, LR, and PSR
- The contents of the following Fault Status Registers:
  - Configurable Fault Status Register
  - Hard Fault Status Register
  - Data Fault Status Register
  - Auxiliary Fault Status Register
- In the event of a Bus Fault or Memory Management fault, the faulting address
  will be printed from one of:
  - MemManage Fault Address Register
  - BusFault Address Register


The source code exists in `app.c` and `hardfault_handler.c`

Hardware Requirements
---------------------
This application can be executed on an Evaluation and Development Board.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the
*Getting Started Guide* for your IDE for more information.

Verification
------------
To verify if this application is functioning correctly, start a debug session
using the following debug configuration:

In the Debugger tab:

- Check **Allocate console for semihosting and SWO**

In the Startup tab:

- Uncheck **Initial Reset and Halt**
- Check **Enable Semihosting**
- Check **GDB Client** in **Console routed to:**
- Uncheck **RAM application (reload after each reset/restart)**
- Check **Pre-run/Restart reset**
- Check **Set breakpoint at: main**
- Check **continue**

Run the program and observe the semihosting output.

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
