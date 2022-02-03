/**
 * @file lsad.c
 * @brief LSAD operational code
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

#include "app.h"

/* Flag set from LSAD_BATMON_IRQHandler
 * when new data is available from LSAD channel conversion */
static bool g_lsad_is_new_data_available;

/* Flag set from LSAD_BATMON_IRQHandler
 *  when overrun error flag is set by LSAD */
static bool g_lsad_is_overrun;

/* Flag set from LSAD_BATMON_IRQHandler when BATMON alarm is triggered */
static bool g_lsad_is_alarm_triggered;

/* Channel conversion data updated when conversion cycle is completed */
static uint32_t g_lsad_channel_data[LSAD_CHANNEL_NUM];

/* Array containing LSAD gain and offset values from TRIM sector */
static struct F_LSAD_TRIM g_f_lsad_gain_offset[LSAD_CHANNEL_NUM];

/* Default trim value sector. */
static TRIM_Type *trims = TRIM;

/* Temperature sensor scale */
static float g_tempsense_scale;

/* Temperature sensor offset */
static float g_tempsense_offset;

/* Function called from LSAD_MONITOR_IRQHandler when new data is available */
static inline void irq_process_lsad_new_data(void)
{
    if (!g_lsad_is_new_data_available)
    {
        g_lsad_is_new_data_available = true;
        for (uint8_t i = 0; i < LSAD_CHANNELS_IN_USE; i++)
        {
            g_lsad_channel_data[i] = LSAD->DATA_TRIM_CH[i];
        }
    }
}

/* Function called from LSAD_BATMON_IRQHandler when an overrun has occurred */
static inline void irq_process_lsad_overrun(void)
{
    g_lsad_is_overrun = true;
}

/* Function called from LSAD_BATMON_IRQHandler when a BATMON alarm has occurred */
static inline void irq_process_lsad_alarm(void)
{
    g_lsad_is_alarm_triggered = true;
}

void LSAD_MONITOR_IRQHandler(void)
{
    /* Check if a LSAD input conversion is completed */
    if (LSAD->MONITOR_STATUS & LSAD_READY_TRUE)
    {
        irq_process_lsad_new_data();
        LSAD->MONITOR_STATUS = LSAD_READY_CLEAR;
    }

    /* Check if data overrun has occurred */
    if (LSAD->MONITOR_STATUS & LSAD_OVERRUN_TRUE)
    {
        irq_process_lsad_overrun();
        LSAD->MONITOR_STATUS = LSAD_OVERRUN_CLEAR;
    }

    /* Check if a LSAD monitor alarm occurred */
    if (LSAD->MONITOR_STATUS & MONITOR_ALARM_TRUE)
    {
        irq_process_lsad_alarm();
        LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR;

        /* Read monitor count value to reset it */
        (void)LSAD->MONITOR_COUNT_VAL;
    }
}

void Thermistor_Init(uint32_t trim_error)
{
    if (trim_error != ERROR_THERMISTOR_INVALID)
    {
        /* Load stored trim values, select GPIO7 as current source. Enable current source
         * on LSAD conversion. */
        ACS->TEMP_CURR_CFG = (trims->thermistor[1].current_trim << ACS_TEMP_CURR_CFG_CURRENT_TRIM_Pos) |
                             (trims->thermistor[1].current_value << ACS_TEMP_CURR_CFG_CURRENT_VALUE_Pos) |
                             THERMISTOR_CURRENT_SOURCE | LSAD_CURR_NORMAL | TEMP_CURR_ENABLE;
        ACS->AOUT_CTRL = AOUT_THERMISTOR_CURRENT | (THERMISTOR_CURRENT_GPIO << ACS_AOUT_CTRL_AOUT_TO_GPIO_Pos);
    }
    else
    {
        /* Select GPIO7 as current source. Enable current source. Load defaults.*/
        ACS->TEMP_CURR_CFG = THERMISTOR_CURRENT_SOURCE | LSAD_CURR_NORMAL | TEMP_CURR_ENABLE |
                             TEMP_CURR_TRIM_0 | TEMP_CURR_10UA;
    }
}

void LSAD_Channel_Init(void)
{
    /* Disable the LSAD and connect all inputs to default values */
    LSAD->CFG = LSAD_DISABLE;

    /* Clear all interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Configure input channel 0 in single ended configuration by connecting
     * the LSAD negative input to ground, connect the positive input to
     * the USER_GPIO. */
    Sys_LSAD_InputConfig(LSAD_USER_CHANNEL,
                         LSAD_POS_INPUT_SEL1 | LSAD_NEG_INPUT_GND,
                         USER_GPIO, -1);

    /* Configure input channel 1 in single ended configuration by connecting
     * the LSAD negative input to ground, connect the positive input to
     * the temperature sensor. */
    Sys_LSAD_InputConfig(TEMP_SENSOR_CHANNEL,
                         LSAD_POS_INPUT_TEMP | LSAD_NEG_INPUT_GND,
                         -1, -1);

    /* Configure input channel 1 in single ended configuration by connecting
     * the LSAD negative input to ground, connect the positive input to
     * the THERMISTOR_GPIO. */
    Sys_LSAD_InputConfig(THERMISTOR_CHANNEL,
                         LSAD_POS_INPUT_SEL2 | LSAD_NEG_INPUT_GND,
                         THERMISTOR_GPIO, -1);

    /* Configure input channel 1 in single ended configuration by connecting
     * the LSAD negative input to ground, connect the positive input to VBAT. */
#if RSL15_CID==202
    Sys_LSAD_InputConfig(VBAT_CHANNEL,
                         LSAD_POS_INPUT_VBAT_DIV2 | LSAD_NEG_INPUT_GND,
                         -1, -1);
#else
    Sys_LSAD_InputConfig(VBAT_CHANNEL,
                         LSAD_POS_INPUT_VBAT | LSAD_NEG_INPUT_GND,
                         -1, -1);
#endif

    /* Configure input channel 7 on both ends to VBAT, to disable automatic offset
     * compensation. */
#if RSL15_CID==202
    Sys_LSAD_InputConfig(COMPENSATION_CHANNEL,
                         LSAD_POS_INPUT_VBAT_DIV2 | LSAD_NEG_INPUT_VBAT_DIV2,
                         -1, -1);
#else
    Sys_LSAD_InputConfig(COMPENSATION_CHANNEL,
                         LSAD_POS_INPUT_VBAT | LSAD_NEG_INPUT_VBAT,
                         -1, -1);
#endif

#if RSL15_CID==202
    LSAD->CFG = LSAD_NORMAL               /* Normal mode, all 8 channels sampled */
                | LSAD_PRESCALE_1280H;    /* Sample rate is SLOWCLK/1280 */
#else
    LSAD->CFG = VBAT_DIV2_ENABLE          /* Enable VBAT voltage divider */
                | LSAD_NORMAL             /* Normal mode, all 8 channels sampled */
                | LSAD_PRESCALE_1280H;    /* Sample rate is SLOWCLK/1280 */
#endif

    /* Enabled LSAD interrupt on conversion of Channel 2.
     * Note that in continuous mode all LSAD channels are sample sequentially
     * and an interrupt is generated each time the selected channel is converted.
     * Therefore, generate interrupt after last channel has been completed. */
    LSAD->INT_ENABLE =  LSAD_INT_CH3 | LSAD_INT_EN;

    /* By default all channels that are not configured are connected
     * to VBAT. For battery monitoring purpose connect channel 3 to
     * monitor source
     */
    /* Mid voltage threshold: 1.4 V, we need to divide the threshold by 2,
     * since VBAT is divided by 2 when measured by the LSAD. */
    LSAD->MONITOR_CFG = ((BATMON_ALARM_COUNT_CFG <<
                          LSAD_MONITOR_CFG_ALARM_COUNT_VALUE_Pos)
                         | ((MONITOR_THRESHOLD_MID/2) &
                            LSAD_MONITOR_CFG_MONITOR_THRESHOLD_Mask)
                         | MONITOR_CH3);

    /* Enable LSAD interrupts */
    NVIC_EnableIRQ(LSAD_MONITOR_IRQn);
}

void LSAD_Common_Init(uint32_t trim_error)
{
    /* Clear global variables */
    g_lsad_is_new_data_available = false;
    g_lsad_is_overrun = false;
    g_lsad_is_alarm_triggered = false;
    memset(g_lsad_channel_data, 0, LSAD_CHANNEL_NUM * sizeof(uint32_t));

    /* Load trim/offset values from trim sector. */
    if (!(trim_error & ERROR_TEMPERATURE_INVALID))
    {
        /* Using low frequency trims, as we are at max pre-scale */
        Sys_LSAD_TempSensor_Gain_Offset(&(trims->temp_sensor), &g_f_lsad_gain_offset[TEMP_SENSOR_CHANNEL]);
    }

	/* tempsense_scale is typically 25.76/C.
	 * Real value calculated here will vary from the typical. */
	if (trims->measured.temp_sensor_high == 0xFFFF || trims->measured.temp_sensor_30C == 0xFFFF)
	{
		/* Use typical tempsense value. */
		g_tempsense_scale = DEF_TEMPSENSE;
		g_tempsense_offset = DEF_TEMPOFFSET;
	}
	else
	{
#ifdef RSL15_CID
		g_tempsense_scale = (float)(trims->measured.temp_sensor_high  - trims->measured.temp_sensor_30C)/60.0f;
#else
		g_tempsense_scale = (float)(trims->measured.temp_sensor_high  - trims->measured.temp_sensor_30C)/15.0f;
#endif
		g_tempsense_offset = (trims->measured.temp_sensor_30C/g_tempsense_scale) - 30.0f;
	}

    if (!(trim_error & ERROR_LSAD_INVALID))
    {
        /* Using low frequency trims, as we are at max pre-scale */
        Sys_LSAD_Gain_Offset(&(trims->lsad_trim), &g_f_lsad_gain_offset[LSAD_USER_CHANNEL]);
        Sys_LSAD_Gain_Offset(&(trims->lsad_trim), &g_f_lsad_gain_offset[THERMISTOR_CHANNEL]);
        Sys_LSAD_Gain_Offset(&(trims->lsad_trim), &g_f_lsad_gain_offset[VBAT_CHANNEL]);
    }
}

void LSAD_Demo_Config(uint32_t trim_error)
{
    /* Initialize non hardware related parameters */
    LSAD_Common_Init(trim_error);

    /* Initialize LSAD input channels and start conversion */
    LSAD_Channel_Init();

    /* Initialize thermistor current source and start pulses. */
    Thermistor_Init(trim_error);

    /* Enable temperature sensor. */
    ACS->TEMP_SENSOR_CFG = LSAD_TEMP_SENS_NORMAL | TEMP_SENS_ENABLE;
}

uint32_t lsad_data_conv_to_mv(uint32_t data, struct F_LSAD_TRIM *gain_offset)
{
    float voltage = 0.0f;
    /*
     * LSAD data : 0x00000000 <=> 0 Volt to 0x00003FFF <=> 2 V
     * (This is a 14 bits value. The native range of the converter corresponds
     * to -0.125 V to +2.125 V (referred to LSAD GND, default sampling frequency
     * configuration). But as the range of interest is this mode is 0 V to 2 V,
     * inputs below 0 V and above 2 V will be saturated).
     */
    if (gain_offset->lf_gain != 0)
    {
        /* Calculate voltage measrured */
        voltage = CONVERT(data) / V_TO_MV_F;
        return (uint32_t)((voltage - gain_offset->lf_offset) / gain_offset->lf_gain * V_TO_MV);
    }
    else
    {
        return ((2000 * data) / (0x4000));
    }
}

float temp_data_conv_to_temp(unsigned data)
{
   /* Calculate temperature using scale and offset*/
   return ((float)(data)/g_tempsense_scale) - g_tempsense_offset;
}

uint32_t thermistor_data_conv_to_resistance(uint32_t data)
{
    struct F_LSAD_TRIM temp_data;

    temp_data.lf_offset = 0.0f;
    temp_data.lf_gain = 1.0f;
    temp_data.lf_offset = 0.0f;
    temp_data.lf_gain = 1.0f;

    if (g_f_lsad_gain_offset[THERMISTOR_CHANNEL].lf_gain != 0)
    {
        /* We know source current and resultant voltage, R = V/I.
         * Then multiply by 1000 to return to V from mV. */
        uint32_t voltage = lsad_data_conv_to_mv(data, &g_f_lsad_gain_offset[THERMISTOR_CHANNEL]);
        return ((voltage * (1.0f / THERM_CURR) / 1000.0f));
    }
    else
    {
        /* Calculation works same as above, just without calibration data. */
        return (((lsad_data_conv_to_mv(data, &temp_data) * 1000.0f)
                 / THERM_CURR) / 1000000.0f);
    }
}

void process_lsad_new_samples(void)
{
    if (g_lsad_is_new_data_available)
    {
        char buf[64] = { 0 };
        sprintf(buf, "LSAD input channel 0 voltage = %d mV \r\n",
                (unsigned int)lsad_data_conv_to_mv(g_lsad_channel_data[LSAD_USER_CHANNEL],
                                                   &g_f_lsad_gain_offset[LSAD_USER_CHANNEL]));
        UART_Send_String(buf);

        sprintf(buf, "Temperature sensor = %d degrees Celsius \r\n",
                (int)temp_data_conv_to_temp(g_lsad_channel_data[TEMP_SENSOR_CHANNEL]));
        UART_Send_String(buf);

        sprintf(buf, "Thermistor resistance = %d ohms \r\n",
                (unsigned int)thermistor_data_conv_to_resistance(g_lsad_channel_data[THERMISTOR_CHANNEL]));
        UART_Send_String(buf);

        sprintf(buf, "LSAD VBAT/2 voltage = %d mV \r\n",
                (unsigned int)lsad_data_conv_to_mv(g_lsad_channel_data[VBAT_CHANNEL],
                                                   &g_f_lsad_gain_offset[VBAT_CHANNEL]));
        UART_Send_String(buf);

        /* Delay the system by 1s after printing data, slowing data feed to be more easily read.*/
        Sys_Delay(SystemCoreClock);

        g_lsad_is_new_data_available = false;
    }
    if (g_lsad_is_alarm_triggered)
    {
        g_lsad_is_alarm_triggered = false;
        UART_Send_String("Battery Monitor Alarm has triggered \r\n");
    }
    if (g_lsad_is_overrun)
    {
        g_lsad_is_overrun = false;
        UART_Send_String("LSAD data overrun flag set \r\n");
    }
}
