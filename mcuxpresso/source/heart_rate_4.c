/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "heart_rate_4.h"

volatile static uint32_t g_heart_rate   = 0;
volatile static uint32_t g_r_r_interval = 0;
volatile static uint32_t g_pulses_count = 0;


void HeartRate_init(hr_i2c_callback send_function, hr_i2c_callback receive_function)
{
	// Sensor configuration ------------------------------
	fifo_config_t fifo_config =
	{
			one_average,
			enable,
			10u
	};
	mode_configuration_t mode_config =
	{
			disable,
			enable,
			heart_rate
	};
	spo2_config_t spo2_config =
	{
			max_range,
			_50Hz,
			_411us
	};
	hr_sensor_init(send_function, receive_function, mode_config, fifo_config, spo2_config);


	set_proximity_interrupt_treshold(PROXIMITY_THLD);
	set_interrupt_flags(proximity_threshold, interrupt_register_1);
	read_interrupt_flags(0xFF, interrupt_register_1);
	// ----------------------------------------------------
}

uint32_t get_heart_rate()
{
	return g_heart_rate;
}

uint32_t get_r_r_interval()
{
	return g_r_r_interval;
}

uint32_t get_beat_count()
{
	return g_pulses_count;
}


void update_heart_rate()
{
	data_state_t    state           = hr_update_data();
	uint32_t        adc_filttered   = hr_get_adc_red_filttered();
	static uint32_t max_value       = 0;
	static uint32_t min_threshold   = 0;
	static uint32_t pulses_count    = 0;
	static uint32_t samples_count   = 0;
	static uint32_t g_r_r_interval  = 0;
	static uint32_t last_sample_cnt = 0;
	if(available == state)
	{
		// Pulse measure -------------------------------------------------
		if(max_value < adc_filttered)
		{
			max_value = adc_filttered;
			min_threshold  = max_value/DIVISOR_THLD;
			//GPIO_PortClear(GPIO, PORT_LED_PULSE, 1<<PIN_LED_PULSE);
		}
		else if(adc_filttered < min_threshold)
		{
			// Pulse detected
			pulses_count ++;
			max_value = 0;
			if(MAX_PULSES_COUNT <= g_pulses_count)
			{
				g_pulses_count = 0;
			}
			else
			{
				g_pulses_count ++;
			}
			g_r_r_interval = samples_count - last_sample_cnt;
			g_r_r_interval *= 1000;
			g_r_r_interval /= 50;
			last_sample_cnt = samples_count;
			//GPIO_PortSet(GPIO, PORT_LED_PULSE, 1<<PIN_LED_PULSE);
		}

		if(250 == samples_count)
		{
			g_heart_rate = pulses_count * 12;
			samples_count ++;
		}
		else if(500 == samples_count)
		{
			g_heart_rate = pulses_count * 6;
			samples_count ++;
		}
		else if(1000 == samples_count)
		{
			g_heart_rate = pulses_count * 3;
			samples_count ++;
		}
		else if(1500 == samples_count)
		{
			g_heart_rate = pulses_count * 2;
			pulses_count = 0;
			samples_count = 0;
		}
		else
		{
			samples_count ++;
		}
		// ---------------------------------------------------------------
	}
	else
	{
		samples_count = 0;
	}
}
