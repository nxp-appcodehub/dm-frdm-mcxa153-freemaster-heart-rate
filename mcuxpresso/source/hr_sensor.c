/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <hr_sensor.h>

// This defines contains all registers of sensor memory map -----
// Interrupt registers -------------
#define INTERRUPT_STATUS_1   0x00
#define INTERRUPT_STATUS_2   0x01
#define INTERRUPT_ENABLE_1   0x02
#define INTERRUPT_ENABLE_2   0x03
// ---------------------------------

// FIFO registers ------------------
#define FIFO_WRITE_POINTER   0x04
#define OVERFLOW_COUNTER     0x05
#define FIFO_READ_POINTER    0x06
#define FIFO_DATA_REGISTER   0x07
// ---------------------------------

// Configuration registers ---------
#define FIFO_CONFIGURATION   0x08
#define MODE_CONFIGURATION   0x09
#define SPO2_CONFIGURATION   0x0A
#define LED1_PULSE_AMPLITUDE 0x0C
#define LED2_PULSE_AMPLITUDE 0x0D
#define LED3_PULSE_AMPLITUDE 0x0E
#define PROXIMITY_MODE_LEDPA 0x10
#define MULTILED_MODE_REG_1  0x11
#define MULTILED_MODE_REG_2  0x12
// ---------------------------------

// Temperature registers -----------
#define TEMP_INTEGER         0x1F
#define TEMP_FRACTION        0x20
#define TEMP_CONFIGURATION   0x21
// ---------------------------------

// Proximity function --------------
#define PROXIMITY_THRESHOLD  0x30
// ---------------------------------

// Part ID -------------------------
#define REVISION_ID          0xFE
#define PART_ID              0xFF
// ---------------------------------

// --------------------------------------------------------------------------------------------------------------

const           float    		g_high_filtter[] 							 = {0.8816, 0.9408, -0.9408};
const           float    		g_low_filtter[] 							 = {0.5914, 0.2043, 0.2043};
const           float    		g_ultra_low_filtter[] 						 = {0.9875, 0.006244, 0.006244}; // 0.1Hz

volatile static uint8_t  		g_tx_buffer[MAX_CONSECUTIVE_DATA_SEND_I2C+1] = {0};
volatile static uint8_t 		*g_data_to_i2c     							 = g_tx_buffer + 1;

volatile static uint32_t 		g_last_adc_value 							 = 0;
volatile static uint32_t 		g_last_adc_ir_value 						 = 0;
volatile static uint32_t 		g_last_adc_green_value 						 = 0;

volatile static uint32_t 		g_adc_filttered  							 = 0;
volatile static uint32_t        g_adc_ir_filttered  						 = 0;
volatile static uint32_t        g_adc_green_filttered  						 = 0;

volatile static uint32_t 		g_adc_ultra_filttered  						 = 0;
volatile static uint32_t        g_adc_ir_ultra_filttered  					 = 0;
volatile static uint32_t        g_adc_green_ultra_filttered  				 = 0;

volatile static bool            g_prox_detection 							 = false;
volatile static float           g_temperature    							 = 0;
volatile static sensor_mode_t   g_sensor_mode 								 = heart_rate;

volatile static hr_i2c_callback g_send_function 							 = 0;
volatile static hr_i2c_callback g_receive_function 							 = 0;

// I2C function for communication with sensor -------------------------------------------------------------------
typedef enum _comunication_option_t
{
	write_i2c,
	read_i2c
} comunication_option_t;



uint32_t i2c_communication(uint8_t address, uint8_t n_values, comunication_option_t mode)
{
	uint32_t retval = 0;
	g_tx_buffer[0] = address;


	if(write_i2c == mode)
	{
		retval = g_send_function((uint8_t *)g_tx_buffer, HR_SENSOR_ADDRESS, n_values+1);
	}
	else
	{
		retval = g_send_function((uint8_t *)g_tx_buffer, HR_SENSOR_ADDRESS, 1);
		retval = g_receive_function((uint8_t *)g_data_to_i2c, HR_SENSOR_ADDRESS, n_values);
	}
	return retval;
}

// --------------------------------------------------------------------------------------------------------------

// Init function ------------------------------------------------------------------------------------------------
void hr_sensor_init(hr_i2c_callback send_function, hr_i2c_callback receive_function,
					mode_configuration_t mode_config, fifo_config_t fifo_config,
					spo2_config_t spo2_config)
{
	g_send_function = send_function;
	g_receive_function = receive_function;
	g_sensor_mode = mode_config.mode;

	mode_config.reset = enable;
	set_mode_configuration(mode_config);
	mode_config.reset = disable;
	set_mode_configuration(mode_config);
	set_spo2_configuration(spo2_config);
	fifo_set_configuration(fifo_config);

	set_led_pulse_amplitude(led_proximity, STANDBY_VALUE);
	set_led_pulse_amplitude(led1, STANDBY_VALUE);
	set_led_pulse_amplitude(led2, STANDBY_VALUE);
	set_led_pulse_amplitude(led3, STANDBY_VALUE);


}
// --------------------------------------------------------------------------------------------------------------

// Interrupt functions ------------------------------------------------------------------------------------------
status_t set_interrupt_flags(interrupts_flags_t flags, interrupt_regisers_t interrupt)
{
	uint8_t address = (interrupt_register_1 == interrupt)? INTERRUPT_ENABLE_1:INTERRUPT_ENABLE_2;
	flags &= (interrupt_register_1 == interrupt)? MASK_INTERRUPT_REGISTER_WRITE_1:MASK_INTERRUPT_REGISTER_WRITE_2;
	g_data_to_i2c[0] = flags;
	return i2c_communication(address, 1u, write_i2c);
}
uint8_t read_interrupt_flags(interrupts_flags_t flags, interrupt_regisers_t interrupt)
{
	uint8_t address = (interrupt_register_1 == interrupt)? INTERRUPT_STATUS_1:INTERRUPT_STATUS_2;
	uint8_t ret_flags;
	i2c_communication(address, 1u, read_i2c);
	ret_flags = g_data_to_i2c[0] & flags;
	return ret_flags;
}
// --------------------------------------------------------------------------------------------------------------

// FIFO functions -----------------------------------------------------------------------------------------------
status_t fifo_set_write_pointer(uint8_t address)
{
	return i2c_communication(FIFO_WRITE_POINTER, 1u, write_i2c);
}

uint8_t fifo_get_write_pointer()
{
	i2c_communication(FIFO_WRITE_POINTER, 1u, read_i2c);
	return g_data_to_i2c[0];
}

status_t fifo_set_read_pointer(uint8_t address)
{
	return i2c_communication(FIFO_READ_POINTER, 1u, write_i2c);
}

uint8_t fifo_get_read_pointer()
{
	i2c_communication(FIFO_READ_POINTER, 1u, read_i2c);
	return g_data_to_i2c[0];
}

status_t fifo_set_ovf_counter(uint8_t value)
{
	return i2c_communication(OVERFLOW_COUNTER, 1u, write_i2c);
}

uint8_t fifo_get_ovf_counter()
{
	i2c_communication(OVERFLOW_COUNTER, 1u, read_i2c);
	return g_data_to_i2c[0];
}

status_t fifo_set_data(uint8_t value)
{
	return i2c_communication(FIFO_DATA_REGISTER, 1u, write_i2c);
}

uint32_t fifo_get_data()
{
	uint32_t retval = 0;
	i2c_communication(FIFO_DATA_REGISTER, 3u, read_i2c);
	retval = (g_data_to_i2c[0] & 0xFF) << 16;
	retval |= (g_data_to_i2c[1] & 0xFF) << 8;
	retval |= (g_data_to_i2c[2] & 0xFF);
	return retval;
}

void fifo_get_bytes(uint8_t * dest, uint8_t bytes)
{
	uint8_t count = 0;
	i2c_communication(FIFO_DATA_REGISTER, bytes, read_i2c);
	for(count = 0; count < bytes; count ++)
	{
		dest[count] = g_data_to_i2c[count];
	}
}
// --------------------------------------------------------------------------------------------------------------

// Configuration functions --------------------------------------------------------------------------------------

// FIFO configurations -------------------
status_t fifo_set_configuration(fifo_config_t config)
{
	uint8_t new_value = ((config.average & MASK_FIFO_SAMPLE_AVERAGE) << SHIFT_FIFO_SAMPLE_AVERAGE);
	new_value |= ((config.fifo_overwrites & MASK_FIFO_ROLLOVER) << SHIFT_FIFO_ROLLOVER);
	new_value |= ((config.fifo_almost_full_value & MASK_FIFO_ALMOST_FULL) << SHIFT_FIFO_ALMOST_FULL);

	g_data_to_i2c[0] = new_value;
	return i2c_communication(FIFO_CONFIGURATION, 1u, write_i2c);
}

fifo_config_t fifo_get_configuration()
{
	fifo_config_t ret_config = {0};
	uint8_t       value      = 0;
	i2c_communication(FIFO_CONFIGURATION, 1u, read_i2c);

	value = g_data_to_i2c[0];
	ret_config.average         		  = (value >> SHIFT_FIFO_SAMPLE_AVERAGE) & MASK_FIFO_SAMPLE_AVERAGE;
	ret_config.fifo_overwrites 		  = (value >> SHIFT_FIFO_ROLLOVER) & MASK_FIFO_ROLLOVER;
	ret_config.fifo_almost_full_value = (value >> SHIFT_FIFO_ALMOST_FULL) & MASK_FIFO_ALMOST_FULL;

	return ret_config;
}
// ---------------------------------------

// Mode configuration --------------------
status_t set_mode_configuration(mode_configuration_t mode_config)
{
	uint8_t new_value = ((mode_config.shutdown & MASK_MODE_SHUTDOWN) << SHIFT_MODE_SHUTDOWN);
	new_value |= ((mode_config.reset & MASK_MODE_RESET) << SHIFT_MODE_RESET);
	new_value |= ((mode_config.mode & MASK_MODE_MODE) << SHIFT_MODE_MODE);

	g_data_to_i2c[0] = new_value;
	return i2c_communication(MODE_CONFIGURATION, 1u, write_i2c);
}

mode_configuration_t get_mode_configuration()
{
	mode_configuration_t ret_config = {0};
	uint8_t              value      = 0;
	i2c_communication(MODE_CONFIGURATION, 1u, read_i2c);

	value = g_data_to_i2c[0];
	ret_config.shutdown = (value >> SHIFT_MODE_SHUTDOWN) & MASK_MODE_SHUTDOWN;
	ret_config.reset	= (value >> SHIFT_MODE_RESET) & MASK_MODE_RESET;
	ret_config.mode     = (value >> SHIFT_MODE_MODE) & MASK_MODE_MODE;

	return ret_config;
}
// ---------------------------------------

// SpO2 configuration --------------------
status_t set_spo2_configuration(spo2_config_t spo2_config)
{
	uint8_t new_value = ((spo2_config.adc_resolution & MASK_SPO2_ADC_RANGE) << SHIFT_SPO2_ADC_RANGE);
	new_value |= ((spo2_config.adc_sample_rate & MASK_SPO2_ADC_SAMPLE_RATE) << SHIFT_SPO2_ADC_SAMPLE_RATE);
	new_value |= ((spo2_config.led_pulse_width & MASK_SPO2_PULSE_WIDTH_CONTROL) << SHIFT_SPO2_PULSE_WIDTH_CONTROL);

	g_data_to_i2c[0] = new_value;
	return i2c_communication(SPO2_CONFIGURATION, 1u, write_i2c);
}

spo2_config_t get_spo2_configuration()
{
	spo2_config_t ret_config = {0};
	uint8_t       value      = 0;
	i2c_communication(SPO2_CONFIGURATION, 1u, read_i2c);

	value = g_data_to_i2c[0];
	ret_config.adc_resolution = (value >> SHIFT_MODE_SHUTDOWN) & MASK_MODE_SHUTDOWN;
	ret_config.adc_sample_rate	= (value >> SHIFT_MODE_RESET) & MASK_MODE_RESET;
	ret_config.led_pulse_width    = (value >> SHIFT_MODE_MODE) & MASK_MODE_MODE;

	return ret_config;
}
// ---------------------------------------

// Led pulse amplitude configuration -----
status_t set_led_pulse_amplitude(led_control_t led, uint8_t current)
{
	uint8_t address = 0;
	switch(led)
	{
		case led1:
			address = LED1_PULSE_AMPLITUDE;
		break;

		case led2:
			address = LED2_PULSE_AMPLITUDE;
		break;

		case led3:
			address = LED3_PULSE_AMPLITUDE;
		break;

		default:
			address = PROXIMITY_MODE_LEDPA;
		break;
	}
	g_data_to_i2c[0] = current;
	return i2c_communication(address, 1u, write_i2c);
}

uint8_t get_led_pulse_amplitude(led_control_t led)
{
	uint8_t address = 0;
	switch(led)
	{
		case led1:
			address = LED1_PULSE_AMPLITUDE;
		break;

		case led2:
			address = LED2_PULSE_AMPLITUDE;
		break;

		case led3:
			address = LED3_PULSE_AMPLITUDE;
		break;

		default:
			address = PROXIMITY_MODE_LEDPA;
		break;
	}
	i2c_communication(address, 1u, read_i2c);
	return g_data_to_i2c[0];
}
// ---------------------------------------

// Multi-LED mode control ----------------
status_t set_multiled_config(multiled_config_t multiled_config)
{
	uint8_t new_value_1 = 0, new_value_2 = 0;

	new_value_1 = ((multiled_config.slot2 & MASK_MULTILED_SLOT) << SHIFT_SLOT_2);
	new_value_1 |= ((multiled_config.slot1 & MASK_MULTILED_SLOT) << SHIFT_SLOT_1);

	new_value_2 = ((multiled_config.slot4 & MASK_MULTILED_SLOT) << SHIFT_SLOT_4);
	new_value_2 |= ((multiled_config.slot3 & MASK_MULTILED_SLOT) << SHIFT_SLOT_3);

	g_data_to_i2c[0] = new_value_1;
	g_data_to_i2c[1] = new_value_2;

	return i2c_communication(MULTILED_MODE_REG_1, 2u, write_i2c);
}

multiled_config_t get_multiled_config()
{
	uint8_t val_1 = 0, val_2 = 0;
	multiled_config_t retval = {0};

	i2c_communication(MULTILED_MODE_REG_1, 2u, read_i2c);
	val_1 = g_data_to_i2c[0];
	val_2 = g_data_to_i2c[1];

	retval.slot1 = (val_1 >> SHIFT_SLOT_1) & MASK_MULTILED_SLOT;
	retval.slot2 = (val_1 >> SHIFT_SLOT_2) & MASK_MULTILED_SLOT;

	retval.slot3 = (val_2 >> SHIFT_SLOT_3) & MASK_MULTILED_SLOT;
	retval.slot4 = (val_2 >> SHIFT_SLOT_4) & MASK_MULTILED_SLOT;

	return retval;
}
// ---------------------------------------

// --------------------------------------------------------------------------------------------------------------


// Temperature functions ----------------------------------------------------------------------------------------
void set_enable_temperature()
{
	g_data_to_i2c[0] = 1;
	i2c_communication(TEMP_CONFIGURATION, 1u, write_i2c);
}

bool get_enable_temperature()
{
	i2c_communication(TEMP_CONFIGURATION, 1u, read_i2c);
	return (bool)g_data_to_i2c[0];
}

int8_t get_int_temperature()
{
	i2c_communication(TEMP_INTEGER, 1u, read_i2c);
	return (int8_t)g_data_to_i2c[0]; // Returned value of sensor is signed
}

float get_temperature()
{
	float retval = 0;
	i2c_communication(TEMP_INTEGER, 2u, read_i2c);
	retval = (float)((int8_t)g_data_to_i2c[0]);
	retval += (((float)g_data_to_i2c[1])*0.0625);
	return retval;
}
// --------------------------------------------------------------------------------------------------------------

// Proximity functions ------------------------------------------------------------------------------------------
status_t set_proximity_interrupt_treshold(uint8_t threshold)
{
	g_data_to_i2c[0] = threshold;
	return i2c_communication(PROXIMITY_THRESHOLD, 1u, write_i2c);
}

uint8_t get_proximity_interrupt_treshold()
{
	i2c_communication(PROXIMITY_THRESHOLD, 1u, read_i2c);
	return g_data_to_i2c[0];
}
// --------------------------------------------------------------------------------------------------------------

// Update sensor ------------------------------------------------------------------------------------------------
uint32_t hr_get_adc_red_value()
{
	return g_last_adc_value;
}

uint32_t hr_get_adc_ir_value()
{
	return g_last_adc_ir_value;
}

uint32_t hr_get_adc_green_value()
{
	return g_last_adc_ir_value;
}

uint32_t hr_get_adc_red_filttered()
{
	return g_adc_filttered;
}

uint32_t hr_get_adc_ir_filttered()
{
	return g_adc_ir_filttered;
}
uint32_t hr_get_adc_green_filttered()
{
	return g_adc_ir_filttered;
}

uint32_t hr_get_adc_red_ultra_filttered()
{
	return g_adc_ultra_filttered;
}

uint32_t hr_get_adc_ir_ultra_filttered()
{
	return g_adc_ir_ultra_filttered;
}
uint32_t hr_get_adc_green_ultra_filttered()
{
	return g_adc_ir_ultra_filttered;
}

float get_internal_temp()
{
	return g_temperature;
}

data_state_t hr_update_data()
{
	data_state_t       state = not_available;
	static uint32_t    value, ir, green;
	static uint8_t     fifo_values[9];

	// For filtters ---------------------
	static uint32_t    last_input_h, last_input_ir_h, last_input_green_h;
	static uint32_t    output_h, output_ir_h, output_green_h;
	static uint32_t    last_input_l, last_input_ir_l, last_input_green_l;
	static uint32_t    last_input_ul, last_input_ir_ul, last_input_green_ul;
	// ----------------------------------
	static uint8_t     last_current;
	interrupts_flags_t flags = read_interrupt_flags(0xFF, interrupt_register_1);
	interrupts_flags_t flags_temp = 0;

	if((internal_temperature_ready & flags_temp) != 0)
	{
		g_temperature = get_temperature();
	}
	// If found proximity, this enbale sensor --------------------------------------
	if((proximity_threshold & flags) != 0)
	{
		set_interrupt_flags(new_fifo_data_ready, interrupt_register_1);
		set_interrupt_flags(internal_temperature_ready, interrupt_register_2);
		set_enable_temperature();
		read_interrupt_flags(0xFF, interrupt_register_1);
		read_interrupt_flags(0xFF, interrupt_register_2);
		g_prox_detection    = false;
	}
	// -----------------------------------------------------------------------------
	else if((new_fifo_data_ready & flags) != 0)
	{
		last_current = get_led_pulse_amplitude(led1);

		// Set fifo in 0 for read in real time ---------------------------
		if(heart_rate == g_sensor_mode)
		{
			fifo_get_bytes(fifo_values, 3);
		}
		else if(spo2 == g_sensor_mode)
		{
			fifo_get_bytes(fifo_values, 6);
		}
		else
		{
			fifo_get_bytes(fifo_values, 9);
		}

		fifo_set_write_pointer(0);
		fifo_set_read_pointer(0);
		fifo_set_ovf_counter(0);

		if(heart_rate <= g_sensor_mode)
		{
			value   = fifo_values [0] << 16;
			value  |= (fifo_values[1] << 8);
			value  |= (fifo_values[2]);

			output_h = (uint32_t)((float)output_h*g_high_filtter[0] + (float)value*g_high_filtter[1] + (float)last_input_h*g_high_filtter[2]);
			g_adc_filttered = (uint32_t)((float)g_adc_filttered*g_low_filtter[0] + (float)output_h*g_low_filtter[1] + (float)last_input_l*g_low_filtter[2]);
			g_adc_ultra_filttered = (uint32_t)((float)g_adc_ultra_filttered*g_ultra_low_filtter[0] + (float)g_adc_filttered*g_ultra_low_filtter[1] + (float)last_input_ul*g_ultra_low_filtter[2]);
			last_input_l = output_h;
			last_input_h = value;
			last_input_ul = g_adc_filttered;
			g_last_adc_value = value;
		}
		if(spo2 <= g_sensor_mode)
		{
			ir      = fifo_values [3] << 16;
			ir     |= (fifo_values[4] << 8);
			ir     |= (fifo_values[5]);

			output_ir_h = (uint32_t)((float)output_ir_h*g_high_filtter[0] + (float)ir*g_high_filtter[1] + (float)last_input_ir_h*g_high_filtter[2]);
			g_adc_ir_filttered = (uint32_t)((float)g_adc_ir_filttered*g_low_filtter[0] + (float)output_ir_h*g_low_filtter[1] + (float)last_input_ir_l*g_low_filtter[2]);
			g_adc_ir_ultra_filttered = (uint32_t)((float)g_adc_ir_ultra_filttered*g_ultra_low_filtter[0] + (float)g_adc_ir_filttered*g_ultra_low_filtter[1] + (float)last_input_ir_ul*g_ultra_low_filtter[2]);
			last_input_ir_l = output_ir_h;
			last_input_ir_h = ir;
			last_input_ir_ul = g_adc_ir_filttered;
			g_last_adc_ir_value = ir;
		}
		if(multi_led == g_sensor_mode)
		{
			green   = fifo_values [6] << 16;
			green  |= (fifo_values[7] << 8);
			green  |= (fifo_values[8]);

			output_green_h = (uint32_t)((float)output_green_h*g_high_filtter[0] + (float)green*g_high_filtter[1] + (float)last_input_green_h*g_high_filtter[2]);
			g_adc_green_filttered = (uint32_t)((float)g_adc_green_filttered*g_low_filtter[0] + (float)output_green_h*g_low_filtter[1] + (float)last_input_green_l*g_low_filtter[2]);
			g_adc_green_ultra_filttered = (uint32_t)((float)g_adc_green_ultra_filttered*g_ultra_low_filtter[0] + (float)g_adc_green_filttered*g_ultra_low_filtter[1] + (float)last_input_green_ul*g_ultra_low_filtter[2]);
			last_input_green_l = output_green_h;
			last_input_green_h = green;
			last_input_green_ul = g_adc_green_filttered;
			g_last_adc_green_value = green;
		}
		// ---------------------------------------------------------------

		// LED Current adjust --------------------------------------------
		if(MAX_ADC_VALUE <= value)
		{
			if(last_current > STANDBY_VALUE)
			{
				last_current --;
				set_led_pulse_amplitude(led1, last_current);
				set_led_pulse_amplitude(led2, last_current);
				set_led_pulse_amplitude(led3, last_current);
			}
		}
		else if(MIN_ADC_VALUE >= value)
		{
			if(MAX_LED_VALUE > last_current)
			{
				last_current ++;
				set_led_pulse_amplitude(led1, last_current);
				set_led_pulse_amplitude(led2, last_current);
				set_led_pulse_amplitude(led3, last_current);
			}
			else
			{
				if(OFF_ADC_VALUE >= value && true == g_prox_detection)
				{
					set_led_pulse_amplitude(led1, STANDBY_VALUE);
					set_led_pulse_amplitude(led2, STANDBY_VALUE);
					set_led_pulse_amplitude(led3, STANDBY_VALUE);
					set_interrupt_flags(proximity_threshold, interrupt_register_1);
					set_interrupt_flags(0, interrupt_register_2);
					read_interrupt_flags(0xFF, interrupt_register_1);
					read_interrupt_flags(0xFF, interrupt_register_2);
				}
			}
		}
		else
		{
			state = available;
		}
		g_prox_detection = true;
		// ----------------------------------------------------------------
	}
	read_interrupt_flags(0xFF, interrupt_register_1);
	read_interrupt_flags(0xFF, interrupt_register_2);
	return state;
}
// --------------------------------------------------------------------------------------------------------------
