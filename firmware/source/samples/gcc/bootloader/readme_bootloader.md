Bootloader Sample Code
======================

Overview
--------
This sample project demonstrates a bootloader application. The bootloader uses the Flash0 
code memory as *app execution area* and Flash1 code memory as *app download area*.
This subdivision is meant to provide a starting point for users that would like to use 
the bootloader for firmware update purposes.

The bootloader application is the first entry point after reset. It is located at the 
base address of the main flash (0x00100000). 

Upon start, the bootloader checks if there is a valid boot image in the 
*app download area* (Flash1 code memory). If there is one, it copies/overwrites this 
image into the *app execution area*, invalidates the data in the download area and boots
this new image. If no valid boot image is found in the download area, the bootloader 
verifies and boots from the _app execution area_. If no valid image is found in both 
areas, the bootloader keeps the device trapped and prints an error message in the JLinkRTTViewer.exe.

Flash memory layout
-------------------

This application assumes the following layout in the Main flash memory:  

```
      Area      | Size  | Flash start addr 
 ---------------|-------|------------------ 
  bootloader    | 32KB  | 0x00100000       
  app execution | 320KB | 0x00108000       
  app download  | 352KB | 0x00158000       
```

Boot image format
------------------------
A valid boot image consists of a binary file generated from a regular _.elf_ file 
using the _objcopy_ tool from the GCC toolchain.

The boot image vector table contains two special items on positions 8 and 9, 
respectively: the application version descriptor and the image size. 

In addition, a valid boot image for an application must have its .text section starting 
at the base address of the _app execution area_ instead of the base address of the main 
flash.

This means that modifications on the linker script (sections.ld) and startup code 
(startup.S) are required to make a typical sample application (for example, blinky) 
compatible with the bootloader.

Hardware Requirements
---------------------
This application can be executed on an Montana Evaluation and Development Board
with no external connections required.

Software Requirements
---------------------
* ON Semiconductor IDE
* Montana CMSIS-Pack
* A recent version of SEGGER J-Link
* JLinkRTTViewer.exe in SEGGER J-Link to see log information

Importing a Project
-------------------
To import and build the sample code into your IDE workspace, refer to the 
*Getting Started Guide* for your IDE for more information.

Verification
------------
To learn more about this application and verify that it is functioning correctly, follow 
the step-by-step tutorials described below.

Tutorial #1: Loading bootloader Application into Montana Flash
--------------------------------------------------------------------------
1. Import the *bootloader* sample application from the Montana CMSIS-Pack into your workspace.

2. Build your project. You will see *bootloader.hex* and *bootloader.elf* in the *Debug* directory.

3. Navigate to *Project > Run As > Run Configurations...> GDB Segger J-Link Debugging > New Configuration* and click the *Run* button after specifying the *Montana* device name in the Debugger Tab.

4. Congratulations! You have successfully flashed the *bootloader* application image.

5. Reset the Device.

6. Open the SEGGER J-Link RTTViewer application (usually located at *C:\Program Files 
(x86)\SEGGER\JLink\JLinkRTTViewer.exe*)
You should expect an output similar to this one:
<pre>
00> -I-Bootloader has started!
00> -I-Checking if there is a valid new image in the download area(00158000)...
00> -I-BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE
00> -I-Checking if there is a valid image in the execution area (00108000)...
00> -I-BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE
00> -I-Image:  ver=(0.0.0)
00> -I-Boot failed! Please write a valid image in flash and reset the device.
</pre>


Tutorial #2: Generating a image for the *blinky* sample application
--------------------------------------------------------------------------
1. Import the *blinky* sample application from the Montana CMSIS-Pack into your workspace

2. Navigate to *Project > Properties > C/C++ Build > Settings > Build steps*

3. Add the following command to the **post-build steps** field:  
`arm-none-eabi-objcopy -O binary blinky.elf blinky.bin`

4. Build your project. Besides *blinky.hex* and *blinky.elf*, you should now also have a
*blinky.bin* file generated in the *Debug* directory.

5. Copy the **startup.S** and **sections.ld** files from *bootloader/utility* 
and paste/replace them into *blinky/RTE/Device/Montana*

6. Copy the *bootloader.h* file from *bootloader/include* and paste into *blinky/include*

7. Modify `blinky/app.c`:  
a. Add the following include:  `#include <bootloader.h>`  
b. Add the following command outside of the main function to give your application a 
version and an ID:  `SYS_BOOT_VERSION("BLINKY", 1, 0, 0);`  

8. Build your application. The project should build without errors and generate a 
*blinky.bin* file

9. Congratulations! You have successfully generated the *blinky* application image. Follow 
the next tutorials to write this image into flash.


Tutorial #3: Using SEGGER J-Link Commander to write binary files into Montana flash
---------------------------------------------------------------------------------
1. Open the SEGGER J-Link Commander application (usually located at *C:\Program Files 
(x86)\SEGGER\JLink\JLink.exe*)

2. Type the following commands to connect to the Montana board:  
a. `J-Link> connect`  
b. `Device> Montana`   
c. `TIF> SWD`  
d. `Speed> 1000` (or leave empty for default)  
You should expect an output similar to this one:
<pre>
Device "MONTANA" selected.
Connecting to target via SWD
Found SW-DP with ID 0x0BE12477
DPIDR: 0x0BE12477
Scanning AP map to find all available APs
AP[1]: Stopped AP scan as end of AP map has been reached
AP[0]: AHB-AP (IDR: 0x14770015)
Iterating through AP map to find AHB-AP to use
AP[0]: Core found
AP[0]: AHB-AP ROM base: 0xE00FF000
CPUID register: 0x410FD214. Implementer code: 0x41 (ARM)
Found Cortex-M33 r0p4, Little endian.
FPUnit: 4 code (BP) slots and 0 literal slots
Security extension: implemented
Secure debug: enabled
CoreSight components:
ROMTbl[0] @ E00FF000
ROMTbl[0][0]: E000E000, CID: B105900D, PID: 000BBD21 Cortex-M33
ROMTbl[0][1]: E0001000, CID: B105900D, PID: 000BBD21 DWT
ROMTbl[0][2]: E0002000, CID: B105900D, PID: 000BBD21 FPB
Cortex-M33 identified.
</pre>

3. To test that your connection is working, you can use the commands "**r**" and "**g**" (**r**eset and **g**o) to reset your device and restart running the bootloader code. You will see the bootloader messages printing using JLinkRTTViewer.exe)

4. To load a binary image into the download area, you can use the same **loadbin** command with your boot image file and download area address:  
* >loadbin c:\blinky.bin 0x00158000  *  

5. Reset the device and you will see the following in the JLinkRTTViewer.exe
<pre>
00> -I-Bootloader has started!
00> -I-Checking if there is a valid new image in the download area(00158000)...
00> -I-BOOTLOADER_IMAGE_STATUS_VALID
00> -I-Copying new image to the execution area...-I- image copied successfully.
00> Invalidating data in download area...
00> -I-Booting... 
</pre>

6. Your blinky application is running on the device.

7. Reset the device again and you will see the following in the JLinkRTTViewer.exe
<pre>
00> -I-Bootloader has started!
00> -I-Checking if there is a valid new image in the download area(00158000)...
00> -I-BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE
00> -I-Checking if there is a valid image in the execution area (00108000)...
00> -I-BOOTLOADER_IMAGE_STATUS_VALID
00> -I-Image: BLINKY ver=(1.0.0)
00> -I-Booting... 
</pre>

Tutorial #4: Using UART to write binary files into Montana flash
---------------------------------------------------------------------------------
1. Connect GPIO7 to ground and press the reset button.
2. Open the command prompt and navigate to the "**utility**" folder
3. Type * >[path_to_python.exe] updater.py COMX blinky.bin  *, where X is the port number.
4. Remove the GPIO7 to ground connection.
5. Reset the device again and you will see the following in the JLinkRTTViewer.exe
<pre>
00> -I-Bootloader has started!
00> -I-Checking if there is a valid new image in the download area(00158000)...
00> -I-BOOTLOADER_IMAGE_STATUS_INVALID_VECTOR_TABLE
00> -I-Checking if there is a valid image in the execution area (00108000)...
00> -I-BOOTLOADER_IMAGE_STATUS_VALID
00> -I-Image: BLINKY ver=(1.0.0)
00> -I-Booting... 
</pre>

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Low Power Mode or resetting continuously (either by design 
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