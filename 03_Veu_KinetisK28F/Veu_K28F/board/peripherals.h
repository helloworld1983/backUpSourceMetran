/*
 * The Clear BSD License
 * Copyright 2017-2018 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_lpuart.h"
#include "fsl_clock.h"
#include "fsl_i2c.h"
#include "fsl_adc16.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitBUTTONsPeripheral functional group */
/* Alias for GPIOA peripheral */
#define BOARD_SW2_GPIO GPIOA
/* Alias for GPIOD peripheral */
#define BOARD_SW3_GPIO GPIOD

/* Definitions for BOARD_InitLEDsPeripheral functional group */
/* Alias for GPIOE peripheral */
#define BOARD_LEDRGB_GPIO GPIOE

/* Definitions for BOARD_InitDEBUG_UARTPeriphral functional group */
/* Definition of peripheral ID */
#define BOARD_DEBUG_UART_PERIPHERAL LPUART0
/* Definition of the clock source frequency */
#define BOARD_DEBUG_UART_CLOCK_SOURCE 12000000UL

/* Definitions for BOARD_InitACCEL_I2CPeripheral functional group */
/* BOARD_InitACCEL_I2CPeripheral defines for I2C3 */
/* Definition of peripheral ID */
#define BOARD_ACCEL_I2C_PERIPHERAL I2C3
/* Definition of the clock source */
#define BOARD_ACCEL_I2C_CLOCK_SOURCE I2C3_CLK_SRC
/* Definition of the clock source frequency */
#define BOARD_ACCEL_I2C_CLK_FREQ CLOCK_GetFreq(BOARD_ACCEL_I2C_CLOCK_SOURCE)
/* Alias for GPIOC peripheral */
#define BOARD_INT1_GPIO GPIOC

/* Definitions for BOARD_InitSDHC0Peripheral functional group */
/* Alias for GPIOB peripheral */
#define BOARD_SD_CARD_DETECT_GPIO GPIOB

/* Definitions for BOARD_InitLIGHTPeripheral functional group */
/* Alias for ADC0 peripheral */
#define BOARD_LIGHT_SENSOR_PERIPHERAL ADC0
/* LIGHT_SENSOR interrupt vector ID (number). */
#define BOARD_LIGHT_SENSOR_IRQN ADC0_IRQn
/* LIGHT_SENSOR interrupt handler identifier. */
#define BOARD_LIGHT_SENSOR_IRQHANDLER ADC0_IRQHandler

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const lpuart_config_t BOARD_DEBUG_UART_config;
extern const i2c_master_config_t BOARD_ACCEL_I2C_config;
extern adc16_channel_config_t BOARD_LIGHT_SENSOR_channelsConfig[1];
extern const adc16_config_t BOARD_LIGHT_SENSOR_config;
extern const adc16_channel_mux_mode_t BOARD_LIGHT_SENSOR_muxMode;
extern const adc16_hardware_average_mode_t BOARD_LIGHT_SENSOR_hardwareAverageMode;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void);
void BOARD_InitBUTTONsPeripheral(void);
void BOARD_InitLEDsPeripheral(void);
void BOARD_InitDEBUG_UARTPeriphral(void);
void BOARD_InitACCEL_I2CPeripheral(void);
void BOARD_InitSDHC0Peripheral(void);
void BOARD_InitLIGHTPeripheral(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */
