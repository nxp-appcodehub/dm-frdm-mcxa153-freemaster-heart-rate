/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef HR_SENSOR_H_
#define HR_SENSOR_H_

#include <hr_sensor.h>
#include <stdint.h>
#include <stdbool.h>
#include "fsl_clock.h"

/*---------------------------------------------------------------------------------------------------------------
 -------------------------------------------------- DEFINES -----------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/

#define HR_SENSOR_ADDRESS 				0x57
#define MAX_CONSECUTIVE_DATA_SEND_I2C   10
#define I2C_BAUDRATE                    400000

#define MAX_ADC_VALUE                   3500000
#define MIN_ADC_VALUE                   2500000
#define OFF_ADC_VALUE                   10000
#define MAX_LED_VALUE					0xFF
#define STANDBY_VALUE                   0x15
#define PROXIMITY_THLD                  0x0A


#define MASK_INTERRUPT_REGISTER_READ_1  0xF1
#define MASK_INTERRUPT_REGISTER_READ_2  0x02
#define MASK_INTERRUPT_REGISTER_WRITE_1 0xF0
#define MASK_INTERRUPT_REGISTER_WRITE_2 0x02


#define MASK_FIFO_SAMPLE_AVERAGE        0x07
#define MASK_FIFO_ROLLOVER              0x01
#define MASK_FIFO_ALMOST_FULL           0x0F

#define SHIFT_FIFO_SAMPLE_AVERAGE       5
#define SHIFT_FIFO_ROLLOVER             4
#define SHIFT_FIFO_ALMOST_FULL          0


#define MASK_MODE_SHUTDOWN              0x01
#define MASK_MODE_RESET                 0x01
#define MASK_MODE_MODE                  0x0F

#define SHIFT_MODE_SHUTDOWN             7
#define SHIFT_MODE_RESET                6
#define SHIFT_MODE_MODE                 0


#define MASK_SPO2_ADC_RANGE             0x03
#define MASK_SPO2_ADC_SAMPLE_RATE       0x07
#define MASK_SPO2_PULSE_WIDTH_CONTROL   0x03

#define SHIFT_SPO2_ADC_RANGE            5
#define SHIFT_SPO2_ADC_SAMPLE_RATE      2
#define SHIFT_SPO2_PULSE_WIDTH_CONTROL  0


#define MASK_MULTILED_SLOT              0x07

#define SHIFT_SLOT_1                    0
#define SHIFT_SLOT_2                    4
#define SHIFT_SLOT_3                    0
#define SHIFT_SLOT_4                    4


/*---------------------------------------------------------------------------------------------------------------
 ------------------------------------------------ DEFINES END ---------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------
 -------------------------------------------------- TYPEDEFS ----------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/
typedef uint32_t(*hr_i2c_callback)(uint8_t *data, uint8_t address, uint8_t size);

typedef enum _enable_state_t
{
	disable,
	enable
} enable_state_t;

typedef enum _data_state_t
{
	available,
	not_available
} data_state_t;

// Interrupt mask (enable and read register ) -----------------------------------
typedef enum _interrupts_mask_t
{
	almost_full_flag              = 0x80, // Only for interrupt register 1
	new_fifo_data_ready           = 0x40, // Only for interrupt register 1
	ambient_ligth_cancelation_ovf = 0x20, // Only for interrupt register 1
	proximity_threshold           = 0x10, // Only for interrupt register 1
	internal_temperature_ready    = 0x02, // Only for interrupt register 2
	power_ready                   = 0x01  // Only for interrupt register 1 (only for read, no enable)
} interrupts_flags_t;

typedef enum _interrupt_regisers_t
{
	interrupt_register_1,
	interrupt_register_2
} interrupt_regisers_t;
// ------------------------------------------------------------------------------

// FIFO configurations ----------------------------------------------------------
typedef enum _sample_average_t
{
	one_average,
	two_average,
	four_average,
	eigth_average,
	sixteen_average,
	thirtytwo_average_1,
	thirtytwo_average_2,
	thirtytwo_average_3
} sample_average_t;

typedef enum _sensor_mode_t
{
	heart_rate = 0x02,
	spo2       = 0x03,
	multi_led  = 0x07
} sensor_mode_t;

typedef enum _spo2_adc_range_t
{
	minimum_range = 0, // 2048
	low_range     = 1,	// 4096
	high_range    = 2,	// 8192
	max_range     = 3	// 16384
} spo2_adc_resolution_t;

typedef enum _spo2_adc_sample_rate_t
{
	_50Hz   = 0,
	_100Hz  = 0,
	_200Hz  = 0,
	_400Hz  = 0,
	_800Hz  = 0,
	_1000Hz = 0,
	_1600Hz = 0,
	_3200Hz = 0,
} spo2_adc_sample_rate_t;

typedef enum _pulse_width_control_t
{
	_69us,
	_118us,
	_215us,
	_411us
} pulse_width_control_t;

typedef enum _led_control_t
{
	led1,
	led2,
	led3,
	led_proximity
} led_control_t;

typedef enum _slot_options_multi_led_t
{
	none_led,
	led1_red,
	led2_ir,
	led3_green,
	none_led_2,
	led1_red_pilot,
	led2_ir_pilot,
	led3_green_pilot
} slot_options_multi_led_t;
// ------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------------
 ---------------------------------------------- ENUMERATIONS END ------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------------
 ------------------------------------------------- STRUCTURES ---------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/
typedef struct _fifo_config_t
{
	sample_average_t average;
	enable_state_t   fifo_overwrites;
	uint8_t          fifo_almost_full_value;
} fifo_config_t;

typedef struct _mode_configuration_t
{
	enable_state_t shutdown;
	enable_state_t reset;
	sensor_mode_t  mode;
} mode_configuration_t;

typedef struct _spo2_config_t
{
	spo2_adc_resolution_t  adc_resolution;
	spo2_adc_sample_rate_t adc_sample_rate;
	pulse_width_control_t  led_pulse_width;
} spo2_config_t;

typedef struct _multiled_config_t
{
	slot_options_multi_led_t slot1;
	slot_options_multi_led_t slot2;
	slot_options_multi_led_t slot3;
	slot_options_multi_led_t slot4;
} multiled_config_t;
/*---------------------------------------------------------------------------------------------------------------
 ----------------------------------------------- STRUCTURES END -------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------
 ------------------------------------------------- FUNCTIONS ----------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/

// Initialization functions -------------------------------------------------------------------------------------
void hr_sensor_init(hr_i2c_callback send_function, hr_i2c_callback receive_function,
					mode_configuration_t mode_config, fifo_config_t fifo_config,
					spo2_config_t spo2_config);
// --------------------------------------------------------------------------------------------------------------

// Interrupt functions ------------------------------------------------------------------------------------------
status_t set_interrupt_flags(interrupts_flags_t flags, interrupt_regisers_t interrupt);
interrupts_flags_t read_interrupt_flags(interrupts_flags_t flags, interrupt_regisers_t interrupt);
// --------------------------------------------------------------------------------------------------------------

// FIFO functions -----------------------------------------------------------------------------------------------
status_t fifo_set_write_pointer(uint8_t address);
uint8_t fifo_get_write_pointer();
status_t fifo_set_read_pointer(uint8_t address);
uint8_t fifo_get_read_pointer();
status_t fifo_set_ovf_counter(uint8_t value);
uint8_t fifo_get_ovf_counter();
status_t fifo_set_data(uint8_t value);
uint32_t fifo_get_data();
void fifo_get_bytes(uint8_t * dest, uint8_t bytes);
// --------------------------------------------------------------------------------------------------------------

// Configuration functions --------------------------------------------------------------------------------------

// FIFO configurations -------------------
status_t 	  fifo_set_configuration(fifo_config_t config);
fifo_config_t fifo_get_configuration();
// ---------------------------------------

// Mode configuration --------------------
status_t 			 set_mode_configuration(mode_configuration_t mode_config);
mode_configuration_t get_mode_configuration();
// ---------------------------------------

// SpO2 configuration --------------------
status_t       set_spo2_configuration(spo2_config_t spo2_config);
spo2_config_t get_spo2_configuration();
// ---------------------------------------

// Led pulse amplitude configuration -----
status_t set_led_pulse_amplitude(led_control_t led, uint8_t current);
uint8_t get_led_pulse_amplitude(led_control_t led);
// ---------------------------------------

// Multi-LED mode control ----------------
status_t           set_multiled_config(multiled_config_t multiled_config);
multiled_config_t get_multiled_config();
// ---------------------------------------

// --------------------------------------------------------------------------------------------------------------

// Temperature functions ----------------------------------------------------------------------------------------
void set_enable_temperature();
bool get_enable_temperature();
int8_t  get_int_temperature();
float   get_temperature();
// --------------------------------------------------------------------------------------------------------------

// Proximity functions ------------------------------------------------------------------------------------------
status_t set_proximity_interrupt_treshold(uint8_t threshold);
uint8_t get_proximity_interrupt_treshold();
// --------------------------------------------------------------------------------------------------------------

// Update sensor ------------------------------------------------------------------------------------------------
// For use this functions, you have to call hr_update_data() with update frequency of 50Hz. (Can be sensor interrupt)
uint32_t     hr_get_adc_red_value();
uint32_t     hr_get_adc_ir_value();
uint32_t     hr_get_adc_green_value();
uint32_t     hr_get_adc_red_filttered();
uint32_t     hr_get_adc_ir_filttered();
uint32_t     hr_get_adc_green_filttered();
uint32_t 	 hr_get_adc_red_ultra_filttered();
uint32_t 	 hr_get_adc_ir_ultra_filttered();
uint32_t	 hr_get_adc_green_ultra_filttered();
float        get_internal_temp();
// ------------------------------------------------------------------------------------------------------------------
data_state_t hr_update_data();  // Update function 50Hz
// --------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------------
 ----------------------------------------------- FUNCTIONS END --------------------------------------------------
 --------------------------------------------------------------------------------------------------------------*/

#endif /* HR_SENSOR_H_ */
