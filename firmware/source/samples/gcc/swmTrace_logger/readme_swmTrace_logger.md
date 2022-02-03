SwmTrace Sample Code
====================

NOTE: If you use this sample application for your own purposes, follow
      the licensing agreement specified in `Software_Use_Agreement.rtf`
      in the home directory of the installed Software Development Kit (SDK).

Overview
--------
This sample project builds on a basic `blinky` application to add logging
using the swmTrace library.

It is designed to toggle GPIO[10] at a nominal 250 ms rate, and at the same
time, output status messages on the selected swmTrace mechanism.

The swmTrace library currently supports a number of output possibilities
including the following:

| Library | Description |
|-----|:-----|
| `swmTrace_RTT_B` | Segger RTT in blocking mode |
| `swmTrace_RTT_NB` | Segger RTT in non-blocking mode |
| `swmTrace_UART_NB` | UART on selected GPIOs |
| `swmTrace_UART_DMA_NB` | UART using DMA on selected GPIOs |
| `swmTrace_SemiHost` | Semihosting |

*Blocking and Non-blocking Modes:*
  - In blocking mode, the swmLog function does not return until the output buffer is
  available. In low throughput scenarios, the buffer is almost always available,
  so little or no blocking occurs. In high throughput scenarios, the buffer might
  not always be available, so blocking might occur. Blocking mode is useful to
  ensure that the output data stream is not corrupted in high throughput scenarios;
  however, due to the swmLog function blocking, real time performance of the system
  might be affected.
  - In non-blocking mode, the swmLog function always returns immediately, whether
  the buffer is available or not. In low throughput scenarios, the buffer is
  almost always available, so the data stream is usually not corrupted. In high
  throughput scenarios, the buffer might not always be available, so the data
  stream might be corrupted. Non-blocking mode is useful to help to maintain the
  real time performance of the system; however, due to the swmLog function not
  blocking, the data stream might be corrupted.

To select a different logging output mechanism, you do not have to make code 
changes but you must select a different swmTrace library. If you are using a
CMSIS pack you make the change in the rteconfig file. Open
`swmTrace_logger.rteconfig` file, go into Device > Library > SwmTrace,
then select the desired library). If you are not using a CMSIS pack, change 
the Library Search Path to point to another swmTrace library in 
Build Settings > Cross ARM C Linker > Libraries.

Each output mechanism supports a number of options including the following:
  - Log levels
  - Log time stamp
  - UART options

When the UART output mechanism is selected, UART options configuring RX/TX pins
must be used. 

In this sample application, options to be used are listed in the `options[]`
variable (in `app.c`):
  - Log level is `SWM_LOG_LEVEL_INFO`
  - UART Rx pin is GPIO 5
  - UART Tx pin is GPIO 6
  - UART RX interrupt is enabled

Log levels include `SWM_LOG_LEVEL_VERBOSE`, `SWM_LOG_LEVEL_INFO`, 
`SWM_LOG_LEVEL_WARNING`, `SWM_LOG_LEVEL_ERROR`, `SWM_LOG_LEVEL_FATAL`,
`SWM_LOG_TEST_PASS`, and `SWM_LOG_TEST_FAIL` (ordered from the lowest level to the highest level). 
For example, with `SWM_LOG_LEVEL_INFO` selected in this sample application,
logging corresponding to `SWM_LOG_LEVEL_INFO` level and higher will be actived (i.e., `swmLogInfo()`,
`swmLogWarn()`, `swmLogError()`, `swmLogFatal()`, `swmLogTestPass()`, `swmLogTestFail()`).
  
For further details related to those available options, refer to the `swmTrace_options.h` header file
located in the source folder of the swmTrace library (<`Installation path>\<Release version>\firmware\source\lib\swmTrace`).

In the UART modes, this sample application is designed to act as a loop back
where the GPIOs can be connected together and the logged information can
be piped to the SEGGER output channel. This is purely for demonstration purposes,
and the application can output to a remote UART port if required.

Note that when building for SemiHosting operation using the appropriate library, you must add these 
linker specs in the `Miscellaneous->Other linker flags` field: `--specs=rdimon.specs`.

Notes
-----
swmLog functions might affect the real-time performance of the system. Therefore, 
make sure the swmLog logging in your applications is as unobtrusive as possible.
The use of SemiHosting is not recommended for this reason, but is included for completeness.
RTT or UART is preferred.


Hardware Requirements
---------------------
This application can be executed on an Evaluation and Development Board.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Getting Started Guide* for your IDE for more information.

Verification
------------
To verify the behaviour of this application, monitor the selected output stream
and ensure that the output shown is of the following form:
```
LED : ON
LED : OFF
LED : ON
LED : OFF
LED : ON
LED : OFF
...
```

In addition the selected GPIO can be monitored.

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
