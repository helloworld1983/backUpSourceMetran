/*
 * I2CSensor.c
 *
 *  Created on: 11-29-2018
 *      Author: QuyenNgo
 */
#include <board/i2c.h>
#include <stddef.h>
#include "I2CSensor.h"
#include "DebugTrace.h"
#include "Delay.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define OXY_I2C_CHANNEL I2C2
#define AIR_I2C_CHANNEL I2C3
#define SFM_I2C_ADDR 0x80 >> 1
#define SFM_I2C_BAUDRATE 100000U
#define MSB_CRC8    (0x31)      // 0011 0001 x8 + x5 + x4 + 1

static int32_t gs_CurrentAirFlowReading = 0;
static int32_t gs_CurrentO2FlowReading = 0;
static bool gs_AirFlowCRCError = false;
static bool gs_O2FlowCRCError = false;
static float I2CSensor_GetO2Flow();
static float I2CSensor_GetAirFlow();

/**
 * Calculate CRC8 for provided buffer
 * @param buff Buffer that function will calculate CRC for
 * @param size Size of buffer
 * @return uint8_t crc8
 */
static uint8_t I2CSensor_CRC8( const void *buff, uint8_t size )
{
	uint8_t *p = (uint8_t *)buff;
	uint8_t crc8;

	for ( crc8 = 0x00 ; size != 0 ; size-- ){
		crc8 ^= *p++;

		for ( int i = 0 ; i < 8 ; i++ ){
			if ( crc8 & 0x80 ){
				crc8 <<= 1; crc8 ^= MSB_CRC8;
			}
			else{
				crc8 <<= 1;
			}
		}
	}
	return crc8;
}

/**
 * Initialize Oxy and air I2C sensor
 */
void I2CSensor_Init(void)
{
	//  SDA GND  =====> VDD(5V)  SCL
	//  HEADER J5: SDA = pin 25
	//  HEADER J5: SCL = pin 26
	//  if the supply drops below 4.75V and back, the sensor will perform a Power on Reset and will stop measuring.
	//  Make sure VDD - GND >= 5 or measure incorrectly
	uint8_t resetCmd[2] = {0x20, 0x00};
	uint8_t getStrAddCmd[2] = {0x31, 0xAE};
	uint8_t serialData[6] = {};
	uint8_t getDataFlow[2] = {0x10, 0x00};

	i2c_Init(OXY_I2C_CHANNEL,SFM_I2C_BAUDRATE);
	i2c_Init(AIR_I2C_CHANNEL,SFM_I2C_BAUDRATE);

	i2c_Write(OXY_I2C_CHANNEL,SFM_I2C_ADDR,resetCmd, 2);

	i2c_Write(AIR_I2C_CHANNEL,SFM_I2C_ADDR,resetCmd, 2);
}

/**
 * Get O2 flow value
 * @return float realFlow Flow value in Lpm
 */
static float I2CSensor_GetO2Flow()
{
	uint8_t TempData[3] = {'\0'};
	uint8_t getDataFlow[2] = {0x10, 0x00};
	i2c_Write(OXY_I2C_CHANNEL, SFM_I2C_ADDR, getDataFlow, 2);
	delay_us(100);
	i2c_Read(OXY_I2C_CHANNEL, SFM_I2C_ADDR, TempData, 3);
	uint16_t flow = ((uint16_t)TempData[0] << 8) + (uint16_t)TempData[1];
	DEBUG_I2CSENSOR("%x %d\n", flow, flow);

	float realFlow=0;
	if(I2CSensor_CRC8(TempData,2)==TempData[2])
	{
		if(flow!=0)
		{
			realFlow = (flow - 32000.0) / 142.8;
		}
		gs_O2FlowCRCError = false;
	}
	else
	{
		gs_O2FlowCRCError = true;
	}
	return realFlow;
}

/**
 * Get Air flow value
 * @return float realFlow Flow value in Lpm
 */
static float I2CSensor_GetAirFlow()
{
	uint8_t TempData[3] = {};
	uint8_t getDataFlow[2] = {0x10, 0x00};
	i2c_Write(AIR_I2C_CHANNEL, SFM_I2C_ADDR, getDataFlow, 2);
	delay_us(100);
	i2c_Read(AIR_I2C_CHANNEL, SFM_I2C_ADDR, TempData, 3);

	uint16_t flow = ((uint16_t)TempData[0] << 8) + (uint16_t)TempData[1];
	DEBUG_I2CSENSOR("%x %d\n", flow, flow);

	float realFlow=0;
	if(I2CSensor_CRC8(TempData,2)==TempData[2])
	{
		if(flow!=0)
		{
			realFlow = (flow - 32000.0) / 140.0;
		}
		gs_AirFlowCRCError = false;
	}
	else
	{
		gs_AirFlowCRCError = true;
	}
	return realFlow;
}

/**
 * Get current Air Flow value
 * @return int32_t gs_CurrentAirFlowReading Flow value in Lpm*100
 */
int32_t I2CSensorAirFlow_GetCurrentReading()
{
	gs_CurrentAirFlowReading = (int32_t)(I2CSensor_GetAirFlow()*100);
	return gs_CurrentAirFlowReading;
}

/**
 * Get previously obtained reading Air flow value
 * @return int32_t gs_CurrentAirFlowReading Flow value in Lpm*100
 */
int32_t I2CSensorAirFlow_GetLastReading()
{
	return gs_CurrentAirFlowReading;
}

/**
 * Get previously obtained reading Air flow value
 * @return int32_t gs_CurrentAirFlowReading Flow value in Lpm*100
 */
int32_t I2CSensorAirFlow_GetCompensateReading()
{
	return gs_CurrentAirFlowReading;
}

/**
 * Get current O2 Flow value
 * @return int32_t gs_CurrentO2FlowReading Flow value in Lpm*100
 */
int32_t I2CSensorO2Flow_GetCurrentReading()
{
	gs_CurrentO2FlowReading = (int32_t)(I2CSensor_GetO2Flow()*100);
	return gs_CurrentO2FlowReading;
}

/**
 * Get previously obtained reading O2 flow value
 * @return int32_t gs_CurrentO2FlowReading Flow value in Lpm*100
 */
int32_t I2CSensorO2Flow_GetLastReading()
{
	return gs_CurrentO2FlowReading;
}

/**
 * Get previously obtained reading O2 flow value
 * @return int32_t gs_CurrentO2FlowReading Flow value in Lpm*100
 */
int32_t I2CSensorO2Flow_GetCompensateReading()
{
	return gs_CurrentO2FlowReading;
}

/**
 * Return whether Air flow sensor is experienced from any CRC error
 * @return bool gs_AirFlowCRCError Return True if there is CRC error, otherwise
 * 								   return False
 */
bool I2CSensorAirFlow_CheckSensor()
{
	return gs_AirFlowCRCError;
}

/**
 * Return whether O2 flow sensor is experienced from any CRC error
 * @return bool gs_O2FlowCRCError Return True if there is CRC error, otherwise
 * 								  return False
 */
bool I2CSensorO2Flow_CheckSensor()
{
	return gs_O2FlowCRCError;
}
