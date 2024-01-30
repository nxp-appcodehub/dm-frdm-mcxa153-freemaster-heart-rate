/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SPO2_H_
#define SPO2_H_

#include "hr_sensor.h"
#include "fsl_gpio.h"
#include <stdint.h>

#define DIFF_FOR_DETECT_SLOPE           100
#define MAX_PULSES_COUNT                500
#define DIVISOR_THLD                    12


//#define GPIO_LED_PULSE GPIO3
//#define PORT_LED_PULSE PORT3
//#define PIN_LED_PULSE  12u

// Update sensor -----------------------------------------------------------------------------------------------
void     Spo2_init(hr_i2c_callback send_function, hr_i2c_callback receive_function);
uint32_t spo2_get_heart_rate();
uint32_t spo2_get_r_r_interval();
uint32_t spo2_get_beat_count();
uint32_t get_sample_count();
uint8_t  spo2_get_spo2_in_blood();
void 	 update_spo2();         // GPIO interrupt powered by sensor 50Hz
// --------------------------------------------------------------------------------------------------------------


#endif /* SPO2_H_ */
