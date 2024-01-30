/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef HEART_RATE_4_H_
#define HEART_RATE_4_H_

#include "hr_sensor.h"
#include <stdint.h>

#define DIFF_FOR_DETECT_SLOPE           100
#define MAX_PULSES_COUNT                500
#define DIVISOR_THLD                    12
//#define PORT_LED_PULSE 3
//#define PIN_LED_PULSE  17

// Update sensor -----------------------------------------------------------------------------------------------
void     HeartRate_init(hr_i2c_callback send_function, hr_i2c_callback receive_function);
uint32_t get_heart_rate();
uint32_t get_r_r_interval();
uint32_t get_beat_count();
void 	 update_heart_rate();         // GPIO interrupt powered by sensor 50Hz
// --------------------------------------------------------------------------------------------------------------

#endif /* HEART_RATE_4_H_ */
