/**
 * @file app.h
 * @brief Application header file for the lsad sample application
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */
#ifndef APP_H
#define APP_H

#include <hw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* Total number of LSAD input channels in hardware. Fixed do not modify */
#define LSAD_CHANNEL_NUM                8U

/* Total number of battery monitor events to count before triggering alarm */
#define BATMON_ALARM_COUNT_CFG          1U

/* Total number of battery monitor events to count before triggering alarm */
#define UART_BAUD_RATE                  115200U

/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO                   0

/* User input voltage GPIO */
#define USER_GPIO                       1

/* Thermistor LSAD GPIO */
#define THERMISTOR_GPIO                 12

/* UART0 TX GPIO */
#define UART_GPIO                       6

/* Thermistor Current source GPIO */
#define THERMISTOR_CURRENT_SOURCE       TEMP_CURR_GPIO7
#define THERMISTOR_CURRENT_GPIO         7

/* Number of LSAD channels in use */
#define LSAD_CHANNELS_IN_USE            4

/* LSAD channel to be used for user purposes. */
#define LSAD_USER_CHANNEL               0

/* Temperature sensor LSAD channel */
#define TEMP_SENSOR_CHANNEL             1

/* Thermistor LSAD channel */
#define THERMISTOR_CHANNEL              2

/* Thermistor LSAD channel */
#define VBAT_CHANNEL                    3

/* LSAD channel for automatic compensation. Disabled in this app. */
#define COMPENSATION_CHANNEL            7

/* @brief Quotients required to convert saved lsad offset/gain in nvr to Voltages */
#define LSAD_OFFSET_ERROR_CONV_QUOTIENT  32768.0f;
#define LSAD_GAIN_ERROR_CONV_QUOTIENT    65536.0f;

#define DEF_TEMPSENSE                   19.8f    /* LSB/Degrees Celsius. */

/* Default temperature sensor offset */
#define DEF_TEMPOFFSET                  387.0f

/* 12-bit value LSAD divisor. */
#define LSAD_12_BIT                     0x1000

/* Milivolts in LSAD range */
#define RANGE_MV                        2000

/* Current source selected for the thermistor
 *  Default current is 5uA. */
#define THERM_CURR                      0.000005f

/* @brief Factor for converting back and forth from mV to V. */
#define V_TO_MV                         1000

/* @brief Float iteration of factor for converting back and forth from mV to V. */
#define V_TO_MV_F                       1000.0f

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/**
 * @brief       Converts an ADC code to a voltage, calculated as follows
 *              voltage = adc_code * (2 V * 1000 [mV]/1 V / 2^14 steps.)
 * @param[in]     x     the ADC code input
 * @return      The voltage output in mV
 * @assumptions Low frequency mode for the ADC is used, meaning that the
 *              resolution of the ADC is 14-bits. CONVERT provides voltage
 *              level as a milliVolt value based on the input ADC code.
 */
#define CONVERT(x)                      ((uint32_t)((x * 1000) >> 13))

/**
 * @brief Configure System Clock
 */
void App_Clock_Config(void);

/**
 * @brief Configure GPIO used by application
 */
void App_GPIO_Config(void);

/**
 * @brief Configure UART in TX mode
 */
void UART_TX_Configure(void);

/**
 * @brief Configure LSAD input channels and start conversion
 * @param [in] trim_error indicates if any trim related errors have occured.
 */
void LSAD_Demo_Config(uint32_t trim_error);

/**
 * @brief Configure thermistor current source.
 * @param [in] trim_error indicates if any trim related errors have occured.
 */
void Thermistor_Init(uint32_t trim_error);

/**
 * @brief Configure LSAD channels.
 */
void LSAD_Channel_Init(void);

/**
 * @brief Initialize non LSAD hardware related parameters
 * @param [in] trim_error indicates if any trim related errors have occured.
 */
void LSAD_Common_Init(uint32_t trim_error);

/**
 * @brief Convert raw LSAD data to a mV reading.
 * @param [in] data raw LSAD value received to be converted.
 * @param [in] gain LSAD gain for compensation
 */
uint32_t lsad_data_conv_to_mv(uint32_t data, struct F_LSAD_TRIM *gain_offset);

/**
 * @brief Convert raw LSAD data to a temperature reading.
 */
float temp_data_conv_to_temp(unsigned data);

/**
 * @brief Convert raw LSAD data to a resistance reading.
 * @param [in] data raw LSAD value received to be converted.
 */
uint32_t thermistor_data_conv_to_resistance(uint32_t data);

/**
 * @brief Process new LSAD samples, called by application main() foreground loop
 */
void process_lsad_new_samples(void);

/**
 * @brief Send message on the UART interface
 * @param [in] string Array of null terminated characters to be sent over UART
 */
void UART_Send_String(char *string);

/**
 * @brief Send a character out on the UART interface in polling mode
 * @param [in] c Character to send over the UART interface
 */
void UART_Send_Char(char c);

/*
 * @brief LSAD Interrupt Handler.
 */
extern void LSAD_MONITOR_IRQHandler(void);

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
