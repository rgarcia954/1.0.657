CC312_TRNG_Self_Test - Demonstrates running the DRBG-CTR self test
==================================================================
to prove the MBEDTLS driver..

NOTE: If you use this sample application for your own purposes, follow
      the licensing agreement specified in `Software_Use_Agreement.rtf`
      in the home directory of the installed Software Development Kit (SDK).

Overview
--------
This sample shows how to use the MBEDTLS driver to test the use of the
entropy function using the DRBG-CTR interface.

Hardware Requirements
---------------------
This application can be executed on an Evaluation and Development Board.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Getting Started Guide* for your IDE for more information.

Verification
------------
By default, the application will output text to the RTT console and
this can be monitored either in the SDK or using the Segger RTT Viewer.

This output will show a pass or fail status indicating if the operation
was successful.

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Low Power Mode or resetting continuously (either by
design or due to programming error). To circumvent this scenario, a software
recovery mode using RECOVERY_GPIO can be implemented with the following steps:

1. Connect RECOVERY_GPIO to ground (this can be achieved by pressing and
holding SW1 pushbutton on the evaluation board).
2. Press the RESET button (this restarts the application, which pauses at the
start of its initialization routine).
3. Re-flash the board. After successful re-flashing, disconnect RECOVERY_GPIO
from ground(releasing SW1 pushbutton on the evaluation board), and press the
RESET button so that the application can work properly.

***
  Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
  onsemi), All Rights Reserved
