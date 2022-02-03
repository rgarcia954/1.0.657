Calibration Library
===================

2020-Mar-04

Introduction
------------

The calibration library will be part of the standard toolset for the RSL15
device which will enable customers to calibrate their devices for optimal 
performance.

The library will contain code that can be used to:
    * Create a local manufacturing information page, and program that page to 
      the appropriate information flash page.
    * Calibrate the following components and update the local information page 
      with the calibrated settings.
        * Power supply components:
            - Band gap (VBG)
            - VDDRF
            - VDDIF
            - VDDFLASH
            - VDDPA
            - VDDC
            - VDDM
            - DC-DC with its LDO
        * Clocks:
            - 3 MHz RC oscillator (un-multiplied)
            - 32 kHz RC oscillator
            
In all cases the calibration functions will:
    * Use binary searches to identify ideal calibration settings.
    * For VDDC and VDDM calibrations the resulting calibration setting must be 
      guaranteed to be above the target setting. For all other calibrations, 
      the resulting calibration setting should be as close as possible to the 
      target setting.
    * Store the ideal calibration settings in the configuration registers.
    * Measure the calibration targets versus an externally supplied reference.
    
Power Calibration
-----------------

The calibration library provides code that enables a user to calibrate
different power supply components to any supported voltage level. 

To perform a power callibration the calibration library provides the following
external functions:

- Calibrate_Power_Initialize

  This function is used to initialize the system to perform power calibrations
  for the different power supplies. It also configures the ADC to be setup
  for measurement.

- Calibrate_Power_VBG

  This function is used to calibrate the bandgap voltage (VBG), by setting the 
  min and max VTRIM settings for VBG, and specifying the ADC register to
  measure from it. It is required that a 1.5V reference is supplied to the
  indicated GPIO. It writes the calibrated setting to the 
  ACS_BG_CTRL->VTRIM_BYTE register. The calibration setting can be read back
  and stored for future use, or it can be used directly as is. 

- Calibrate_Power_VDDRF

  This function is used to calibrate the radio front-end voltage (VDDRF), by 
  setting the min and max VTRIM settings for VDDRF, and specifying the ADC 
  register to measure from. The VDDRF is calibrated using ADC through AOUT. 
  It writes the calibrated setting to the ACS_VDDRF_CTRL->VTRIM_BYTE register.
  The calibration setting can be read back and stored for future use, or 
  it can be used directly as is.
  
- Calibrate_Power_VDDPA

  This function is used to calibrate the radio power amplifier power supply 
  (VDDPA), by setting the min and max VTRIM settings for VDDPA, and specifying 
  the ADC register to measure from. The VDDRF is calibrated using ADC 
  through AOUT. It writes the calibrated setting to the 
  ACS_VDDPA_CTRL->VTRIM_BYTE register. The calibration setting can be read 
  back and stored for future use, or it can be used directly as is.
  
  - Calibrate_Power_VDDIF

  This function is used to calibrate the interface power supply (VDDIF), by 
  setting the min and max VTRIM settings for VDDIF, and specifying the ADC 
  register to measure from. The VDDIF is calibrated using ADC through AOUT. 
  It writes the calibrated setting to the ACS_VDDIF_CTRL->VTRIM_BYTE register.
  The calibration setting can be read back and stored for future use, or 
  it can be used directly as is.
  
- Calibrate_Power_VDDFLASH

  This function is used to calibrate the flash power supply 
  (VDDFLASH), by setting the min and max VTRIM settings for VDDFLASH, and
  specifying the ADC register to measure from. The VDDFLASH is calibrated 
  using ADC through AOUT. It writes the calibrated setting to the 
  ACS_VDDFLASH_CTRL->VTRIM_BYTE register. The calibration setting can be read 
  back and stored for future use, or it can be used directly as is.

- Calibrate_Power_DCDC

  This function is used to calibrate the DC-DC converter (DCDC), by setting 
  the min and max VTRIM settings for VCC, and specifying the ADC register to 
  measure from. The VDDRF is calibrated using ADC through AOUT. It writes the 
  calibrated setting to the ACS_VCC_CTRL->VTRIM_BYTE register. The calibration 
  setting can be read back and stored for future use, or it can be used directly 
  as is.

- Calibrate_Power_VDDC

  This function is used to calibrate the digital core voltage power supply (VDDC), 
  by setting the min and max VTRIM settings for VDDC, and specifying the ADC 
  register to measure from. It writes the calibrated setting to the 
  ACS_VDDC_CTRL->VTRIM_BYTE register. The calibration setting can be read back and 
  stored for future use, or it can be used directly as is.

- Calibrate_Power_VDDM

  This function is used to calibrate the digital memory voltage (VDDM), 
  by setting the min and max VTRIM settings for VDDM, and specifying the ADC 
  register to measure from. It writes the calibrated setting to the 
  ACS_VDDM_CTRL->VTRIM_BYTE register. The calibration setting can be read back and 
  stored for future use, or it can be used directly as is.

Clock Calibration
-----------------

The calibration library provides code that enables a user to calibrate the current
system clock to any supported clock frequency. This calibration procedure requires
an Asynchronous Clock Counter (ASCC) to measure the timing the 128Hz reference clock.
This is needed to measure the internal 32 kHz RC oscillator, when using the 48 MHz 
crystal oscillator, where a measurement accuracy of +/- 22ppm can be achieved. 
The RC start oscillator is also calibrated via using the 128Hz reference clock, but 
the RC start oscillator needs to be output on a GPIO and shorted to the GPIO used 
for the ASCC input. 

- Calibrate_Clock_Initialize

  This function is used to initialize the system to perform a clock
  calibration; it configures the 48MHz XTAL oscillator and configures the system
  clock to RF clock oscillator (RFCLK) at a prescale of 2, resulting in a SYSCLK
  of 24MHz. It configures the RC oscillator and on some devices, configures the
  32kHz oscillator as the RTC clock, where that applies.
  
- Calibrate_Clock_32K_RCOSC

  This function is used to calibrate the 32KHz RC oscillator to a specified
  frequency, by setting the min and max frequency trimming values (FTRIM_32K). 
  The calibration setting can be read back and stored for future use, or it can be 
  used directly as is.

- Calibrate_Clock_Start_OSC

  This function is used to calibrate the Start RC oscillator to a specified
  frequency (without using the oscillator's multiplier), by setting the min and 
  max frequency trimming values (FTRIM_START). It configures the system
  clock to the RF oscillator (RFCLK). The calibration setting can be 
  read back and stored for future use, or it can be used directly as is.  
