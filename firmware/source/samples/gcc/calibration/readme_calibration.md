================================
Calibration Library Sample Code
================================

Overview
--------

This sample project demonstrates how to calibrate the device using the provided
calibration library. This application will calibrate the device and store the
trim settings in the appropriate trim registers. The application will then 
merge the contents of the calibration with old NVR4 data, and write that new data
to NVR4. If content in NVR4 is not calibration data and ERASE_OK 
(found in app.h) = 1, the old NVR4 data will be erased. As a final step, to 
demonstrate retrieving newly written values, the application will load VDDPA with
1.64V calibration target achieved in the calibration steps. 

The results of each calibration can be found by stepping through the code and
examining the power_results or clock_results variable. 
    
Hardware Requirements
---------------------
No external hardware is required to run the calibration.

It is reccomended to run the device at a VBAT of 1.35V or higher, as
this calibration sets VCC to 1.3V and VBAT must be higher than VCC.
In normal operation VCC is to be set to 1.2V. This is achievable 
when VBAT is 1.25V.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Integrated Development Environment (IDE) User's Guide* guide for more 
information.
  
Verification
------------
To verify that the application is working correctly, load the sample code on 
the device and connect an oscilloscope and digital multimeter to the AOUT_GPIO 
specified in app.h. By default, AOUT_GPIO is GPIO1. The calibration library 
outputs the regulator being calibrated to AOUT. For clock calibrations, the 
appropriate clock needs to be output in order to be measured.

The user can then step through the code in debug mode to verify the results via
measurement of the AOUT pin and via the return values contained in the
power_results and clock_results variables.

The user can also verify the successful flashing of the results to NVR4 by
checking the contents of the trim structure in NVR4 inside of the 
Sys_Trim_LoadVDDPA() call. It can also be verified by measuring VDDPA on the
AOUT GPIO via oscilloscope or digital multimeter, where it should measure 
close to 1.64V.

Do note it is not advised to calibrate the flash voltage away from the default
1.6V setting.

The source code exists in `app.c` and `app.h`. 

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
