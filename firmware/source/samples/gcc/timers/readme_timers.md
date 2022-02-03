Timer Driver Sample Code
========================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------
This sample project demonstrates an application that:

1.  Uses timer[0] to give the interval between each `TIMER_STATES_GPIO` toggle. 
    Timer[0] works in free run mode.
2.  Uses timer[1] to change the timer[0] interval and start timer[2]. 
    Timer[1] works in single shot mode.
3.  Uses timer[2] to gradually reduce the timer[0] interval 4 times.
    Timer[2] works in multi shot mode.
4.  Uses timer[3] to demonstrate how to use GPIO0 interrupt to capture timer[3] value.
    Timer[3] uses GPIO single capture mode.

The source code exists in `app.c`, with additional `app.h` and `RTE_device.h`
header files included.

Hardware Requirements
---------------------
This application can be executed on an Evaluation and Development Board.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Getting Started Guide* for your IDE for more information.

Verification
------------
To verify that the application is working correctly, connect the board
to the power supply. After the board is powered up, the `TIMER_STATES_GPIO` continuously
flashes on the Evaluation and Development Board with a variable frequency.
The frequency changes according to the following order:

1.  At the beginning the `TIMER_STATES_GPIO` toggles rapidly (timer[0] interval).
2.  After few seconds, the `TIMER_STATES_GPIO` toggling slows down to a low frequency.
3.  The `TIMER_STATES_GPIO` gradually increases its toggles by 4 times.  

To verify the Timer GPIO capture mode, follow these steps:
1.  Connect GPIO0 to ground (or press the SW1 push-button) and observe that the LED connected 
    to `TIMER_CAPTURE_MODE_GPIO` turns on. 
    Initially the Timer GPIO capture mode is set to Single Mode.
    Thus, the LED at `TIMER_CAPTURE_MODE_GPIO` will remain on.
2.  Open RTE_Devices.h using CMSIS Configuration Wizard and go to
    TIMER Configuration->TIMER3 enable->GPIO enable->GPIO Mode and change Single Mode to
    Continuous Mode and click Save.
3.  Build the sample and flash it on to the Evaluation and Development Board.
4.  Connect GPIO0 to ground (or press the SW1 push-button) multiple times and observe that the 
    LED at `TIMER_CAPTURE_MODE_GPIO` changes state on each connection to ground.  

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Low Power Mode or resetting continuously (either by design
or due to programming error). To circumvent this scenario, a software recovery
mode using RECOVERY_GPIO can be implemented with the following steps:

1. Connect RECOVERY_GPIO to ground (this can be achieved by pressing and holding 
   SW1 pushbutton on the evaluation board).
2. Press the RESET button (this restarts the application, which pauses at the
   start of its initialization routine).
3. Re-flash the board. After successful re-flashing, disconnect RECOVERY_GPIO
   from ground (releasing SW1 pushbutton on the evaluation board), and press the
   RESET button so that the application can work properly.

***
  Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
  onsemi), All Rights Reserved
