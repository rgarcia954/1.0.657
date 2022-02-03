SPI CMSIS-Driver Sample Applications
====================================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

The spi_master_cmsis and spi_slave_cmsis sample projects demonstrate how to 
operate an SPI instance using the SPI CMSIS-Driver. These sample projects 
also demonstrate how to use the SPI CMSIS-Driver to transfer data in both 
master and slave modes. Additional information about the applications:

1)  SPI0 is configured as the master in `spi_master_cmsis` and as a slave in 
    the `spi_slave_cmsis` application.

2)  Both SPI instances transmit/receive SPI messages containing the string 
    "onsemi SPI".
   
3)  Callback functions defined for both master and slave, respectively, are 
    `Master_Callback` and `Slave_Callback`. These functions are called
    by the driver when an SPI event occurs, such as 
    `ARM_SPI_EVENT_TRANSFER_COMPLETE` and `ARM_SPI_EVENT_DATA_LOST`.
   
4)  The master instance starts the communication by calling the 
    `Transfer` function. Once the transfer is finished, the master waits
    for 1 second and deactivates the SSEL line to allow slave to transmit. 
    The slave starts transmitting by calling the same `Transfer` function.

5)  Upon a successful transfer, the SPI CMSIS-Driver sends an 
    `ARM_SPI_EVENT_TRANSFER_COMPLETE` event to the application, using the 
    callback function. The event is generated on both the master and slave 
    sides.

6)  This application uses `APP_SPI0_EVENT_GPIO` (`APP_SPI0_EVENT_GPIO` is
    defined in `app.h` and `GREEN_LED` is configured in RTE\Device\{device_name}\RTE_Device.h
    the status of the transmission. The behavior of this GPIO is defined as follows:
    * 2 pulses: successful reception has finished
    * 5 pulses: error has occurred
    
Hardware Requirements
---------------------
Two devices are required, one with the slave application loaded and one with
the master application loaded.

Connect the slave device to the master device as follows:
   Slave `SPI0_SCLK`  <->  `SPI0_SCLK` Master
   Slave `SPI0_SSEL`  <->  `SPI0_SSEL` Master
   Slave `SPI0_MOSI`  <->  `SPI0_MOSI` Master
   Slave `SPI0_MISO`  <->  `SPI0_MISO` Master

GPIO's used for the SPI interface are configured in `RTE_Device.h`.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Integrated Development Environment (IDE) User's Guide* for more information.
  
Verification
------------
To verify that the application is working correctly, load the sample codes on 
two devices and connect the SPI interfaces as detailed in the Hardware 
Requirements section. Use an oscilloscope or another device to monitor the 
behavior of the GPIOs `APP_SPI0_EVENT_GPIO` on the two boards. The GPIOs
should pulse twice every 5 seconds, confirming that both transmission and 
reception are successful. If you observe 5 pulses at any time, this means a 
transmission error has occurred.

The source code exists in `app.c` and `app.h`. The driver source files are 
located in the `drivers` library `code` and `include` folders. The driver 
configuration is defined in `RTE_Device.h` (to open this configuration file in
GUI mode, right-click it and select 
**Open with** > **CMSIS Configuration Wizard**).

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
