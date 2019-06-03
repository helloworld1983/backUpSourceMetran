/*
 * I2CSensor.c
 *
 *  Created on: 11-29-2018
 *      Author: QuyenNgo
 */
#include <stddef.h>
#include "I2CSensor.h"
#include "i2c.h"
#include "DebugTrace.h"
#include "Delay.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define OXY_I2C_CHANNEL I2C3
#define AIR_I2C_CHANNEL I2C2
#define SFM_I2C_ADDR 0x80
#define SFM_I2C_BAUDRATE 100000U

void I2CSensor_Init(void)
{
	//  SDA GND  =====> VDD(5V)  SCL
	//  HEADER J5: SDA = pin 25
	//  HEADER J5: SCL = pin 26
	//if the supply drops below 4.75V and back, the sensor will perform a Power on Reset and will stop measuring.
	// Make sure VDD - GND >= 5 or measure incorrectly
	uint8_t resetCmd[2] = {0x20, 0x00};
	uint8_t getSrAddCmd[2] = {0x31, 0xAE};
	uint8_t serialData[6] = {};
	uint8_t getDataFlow[2] = {0x10, 0x00};

	i2c_Init(OXY_I2C_CHANNEL,SFM_I2C_BAUDRATE);
	i2c_Init(AIR_I2C_CHANNEL,SFM_I2C_BAUDRATE);

	i2c_Write(OXY_I2C_CHANNEL,SFM_I2C_ADDR,resetCmd, 2);
	i2c_Write(OXY_I2C_CHANNEL, SFM_I2C_ADDR, getSrAddCmd, 2);
	i2c_Read(OXY_I2C_CHANNEL, SFM_I2C_ADDR, serialData, 6);
	i2c_Write(OXY_I2C_CHANNEL, SFM_I2C_ADDR, getDataFlow, 2);

	i2c_Write(AIR_I2C_CHANNEL,SFM_I2C_ADDR,resetCmd, 2);
	i2c_Write(AIR_I2C_CHANNEL, SFM_I2C_ADDR, getSrAddCmd, 2);
	i2c_Read(AIR_I2C_CHANNEL, SFM_I2C_ADDR, serialData, 6);
	i2c_Write(AIR_I2C_CHANNEL, SFM_I2C_ADDR, getDataFlow, 2);
}
float I2CSensor_GetO2Flow()
{
	uint8_t TempData[3] = {};
	uint8_t getDataFlow[2] = {0x10, 0x00};
	i2c_Read(OXY_I2C_CHANNEL, SFM_I2C_ADDR, TempData, 3);
	delay_us(100);

	i2c_Write(OXY_I2C_CHANNEL, SFM_I2C_ADDR, getDataFlow, 2);
	uint16_t flow = ((uint16_t)TempData[0] << 8) + (uint16_t)TempData[1];
	DEBUG_I2CSENSOR("%x %d\n", flow, flow);

	// flow = (measure - offset) / scale
	// offset = 32 000
	// scale (air, N2) = 140
	// scale (02) = 142.8
	float realFlow=0;
	if(flow!=0)
	{
		realFlow = (flow - 32000.0)/ 140.0;
	}
	return realFlow;
}
float I2CSensor_GetAirFlow()
{
	uint8_t TempData[3] = {};
	uint8_t getDataFlow[2] = {0x10, 0x00};
	i2c_Read(AIR_I2C_CHANNEL, SFM_I2C_ADDR, TempData, 3);
	delay_us(100);
	i2c_Write(AIR_I2C_CHANNEL, SFM_I2C_ADDR, getDataFlow, 2);
	uint16_t flow = ((uint16_t)TempData[0] << 8) + (uint16_t)TempData[1];
	DEBUG_I2CSENSOR("%x %d\n", flow, flow);

	// flow = (measure - offset) / scale
	// offset = 32 000
	// scale (air, N2) = 140
	// scale (02) = 142.8
	float realFlow=0;
	if(flow!=0)
	{
		realFlow = (flow - 32000.0) / 140.0;
	}
	return realFlow;
}

