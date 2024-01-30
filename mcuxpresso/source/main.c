/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "spo2.h"
#include "hr_sensor.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

#define I2C_MODULE	  LPI2C0
#define UART_MODULE   LPUART0

#define PORT_UART_TX  PORT0
#define PIN_UART_TX   3

#define PORT_UART_RX  PORT0
#define PIN_UART_RX   2

#define GPIO_INTERRUPT GPIO2
#define PORT_INTERRUPT PORT2
#define PIN_INTERRUPT  5

#define PORT_SDA_I2C   PORT3
#define PIN_SDA_I2C    28

#define PORT_SCL_I2C   PORT3
#define PIN_SCL_I2C    27

volatile static uint16_t g_enable_to_update = 0;

uint32_t send_i2c_function(uint8_t *data, uint8_t address, uint8_t size);
uint32_t receive_i2c_function(uint8_t *data, uint8_t address, uint8_t size);


static volatile uint16_t g_hr, g_ecg, g_spo2, g_sample_count, g_red_sensor_raw;


int main (void)
{
    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

	static port_pin_config_t pin_config;
	// Reset peripherals ------------------------------
    RESET_PeripheralReset(kLPUART0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kPORT0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kPORT2_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kPORT3_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kLPI2C0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO2_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO3_RST_SHIFT_RSTn);
    // ------------------------------------------------

    // Enable clocks ----------------------------------
	CLOCK_EnableClock(kCLOCK_GatePORT0);
	CLOCK_EnableClock(kCLOCK_GatePORT3);
	CLOCK_EnableClock(kCLOCK_GatePORT2);
	CLOCK_EnableClock(kCLOCK_GateGPIO2);
	CLOCK_EnableClock(kCLOCK_GateGPIO3);
	// ------------------------------------------------

	// Pin config -------------------------------------
	pin_config.pullSelect 		   = kPORT_PullUp;
	pin_config.pullValueSelect     = kPORT_HighPullResistor;
	pin_config.driveStrength   	   = kPORT_LowDriveStrength;
	pin_config.driveStrength1  	   = kPORT_NormalDriveStrength;
	pin_config.inputBuffer   	   = kPORT_InputBufferEnable;
	pin_config.lockRegister   	   = kPORT_UnlockRegister;
	pin_config.mux                 = kPORT_MuxAlt2;
	pin_config.openDrainEnable     = kPORT_OpenDrainEnable;
	pin_config.passiveFilterEnable = kPORT_PassiveFilterDisable;
	pin_config.invertInput         = kPORT_InputNormal;
	pin_config.slewRate            = kPORT_FastSlewRate;
	PORT_SetPinConfig(PORT_SDA_I2C, PIN_SDA_I2C, &pin_config);
	PORT_SetPinConfig(PORT_SCL_I2C, PIN_SCL_I2C, &pin_config);

	pin_config.openDrainEnable         = kPORT_OpenDrainDisable;
	pin_config.pullSelect 		       = kPORT_PullDisable;
	pin_config.pullValueSelect         = kPORT_LowPullResistor;
	PORT_SetPinConfig(PORT_UART_TX, PIN_UART_TX, &pin_config);
	PORT_SetPinConfig(PORT_UART_RX, PIN_UART_RX, &pin_config);

	pin_config.mux                     = kPORT_MuxAlt0;
	PORT_SetPinConfig(PORT_INTERRUPT, PIN_INTERRUPT, &pin_config);
	pin_config.mux                     = kPORT_MuxAlt0;
	//PORT_SetPinConfig(PORT_LED_PULSE, PIN_LED_PULSE, &pin_config);
	// -------------------------------------------------

	// UART init --------------------------------------------------
	lpuart_config_t config;
	LPUART_GetDefaultConfig(&config);
	/*
	 * config.baudRate_Bps = 115200U;
	 * config.parityMode = kUSART_ParityDisabled;
	 * config.stopBitCount = kUSART_OneStopBit;
	 * config.loopback = false;
	 * config.enableTx = false;
	 * config.enableRx = false;
	 */
	config.baudRate_Bps = 115200;
	config.enableTx     = true;
	config.enableRx     = true;


    CLOCK_SetClockDiv(kCLOCK_DivLPUART0, 1u);
    CLOCK_AttachClk(kFRO12M_to_LPUART0);
	LPUART_Init(LPUART0, &config,  CLOCK_GetLpuartClkFreq(0));
    // -------------------------------------------------------------


	// I2C initialization ------------------------------------------
	lpi2c_master_config_t masterConfig;
	LPI2C_MasterGetDefaultConfig(&masterConfig);
	/*
	 *  masterConfig->enableMaster            = true;
	 *  masterConfig->debugEnable             = false;
	 *  masterConfig->ignoreAck               = false;
	 *  masterConfig->pinConfig               = kLPI2C_2PinOpenDrain;
	 *  masterConfig->baudRate_Hz             = 100000U;
	 *  masterConfig->busIdleTimeout_ns       = 0U;
	 *  masterConfig->pinLowTimeout_ns        = 0U;
	 *  masterConfig->sdaGlitchFilterWidth_ns = 0U;
	 *  masterConfig->sclGlitchFilterWidth_ns = 0U;
	 *  masterConfig->hostRequest.enable      = false;
	 *  masterConfig->hostRequest.source      = kLPI2C_HostRequestExternalPin;
	 *  masterConfig->hostRequest.polarity    = kLPI2C_HostRequestPinActiveHigh;
	 */
	masterConfig.baudRate_Hz = I2C_BAUDRATE;
	CLOCK_SetClockDiv(kCLOCK_DivLPI2C0, 1u);
    CLOCK_AttachClk(kFRO12M_to_LPI2C0);
	LPI2C_MasterInit(LPI2C0, &masterConfig, CLOCK_GetLpi2cClkFreq());
    // -------------------------------------------------------------


	// GPIO configurations -----------------------------------------
	static gpio_pin_config_t config_pin =
	{
			kGPIO_DigitalInput,
			0u
	};


	GPIO_SetPinInterruptConfig(GPIO_INTERRUPT, PIN_INTERRUPT, kGPIO_InterruptFallingEdge);
	GPIO_PinInit(GPIO_INTERRUPT, PIN_INTERRUPT, &config_pin);

//	config_pin.pinDirection = kGPIO_DigitalOutput;
	//GPIO_PinInit(GPIO_LED_PULSE, PIN_LED_PULSE, &config_pin);
    // -------------------------------------------------------------

    SDK_DelayAtLeastUs(100*1000, CLOCK_GetCoreSysClkFreq());

    PRINTF("sensor_init\r\n");
	Spo2_init(send_i2c_function, receive_i2c_function);
   // hr_init(HR_SENSOR_ADDRESS);
    PRINTF("init complete\r\n");
	/* Enables NVIC interrupt for GPIO */
	EnableIRQ(GPIO2_IRQn);

	while(1)
	{
		if(1 == g_enable_to_update)
		{
			g_sample_count = get_sample_count();
			update_spo2();

			uint16_t hr, ecg, spo2;

		    spo2 = spo2_get_spo2_in_blood();
		    hr = spo2_get_heart_rate();
		    ecg = (uint16_t)hr_get_adc_ir_filttered();

		    g_spo2 = spo2;

		    g_ecg = ecg;

		    if(hr >= 60 && hr <130)
		    {
			    g_hr = hr;
		    }


		    g_red_sensor_raw = (uint16_t)hr_get_adc_ir_filttered();
          //  value[1] = data_send;

			//LPUART_WriteBlocking(UART_MODULE, (uint8_t*)&value, 4);
            PRINTF("g_sample_count:%d, hr:%d ecg:%d, blood:%d:\r\n", g_sample_count, g_hr, g_ecg, g_spo2);

			// Read for erase interrupt flags ------------------------
			read_interrupt_flags(0xFF, interrupt_register_2);
			read_interrupt_flags(0xFF, interrupt_register_1);
			// -------------------------------------------------------
			g_enable_to_update = 0;
		}

	}
	return 0;
}


void GPIO2_IRQHandler(void)
{
	g_enable_to_update = 1;
	GPIO_PinClearInterruptFlag(GPIO2, PIN_INTERRUPT);
}

uint32_t send_i2c_function(uint8_t *data, uint8_t address, uint8_t size)
{
	uint32_t retval = LPI2C_MasterStart(I2C_MODULE, address, kLPI2C_Write);
	retval = LPI2C_MasterSend(I2C_MODULE, data, size);
	LPI2C_MasterStop(I2C_MODULE);
	return retval;
}

uint32_t receive_i2c_function(uint8_t *data, uint8_t address, uint8_t size)
{
	uint32_t retval = LPI2C_MasterStart(I2C_MODULE, address, kLPI2C_Read);
	retval = LPI2C_MasterReceive(I2C_MODULE, data, size);
	LPI2C_MasterStop(I2C_MODULE);
	return retval;
}
