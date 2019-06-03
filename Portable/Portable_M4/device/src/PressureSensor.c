/********************************************************************/
/*                                                                  */
/* File Name    : PressureSensor.c                                  */
/*                                                                  */
/* General       : This module contain function for read pressure	*/
/* 				   sensor											*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             April 18, 2017  Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/

#include <Adc.h>
#include <TaskCommon.h>
#include "PressureSensor.h"
#include "I2CInterface.h"
//#include "UiTask.h"

enum
{
	eIdleStage,
	eInhaleStage,
	eExhaleStage
};

//#define DEBUG_PRESSURE_EN
#ifdef DEBUG_PRESSURE_EN
#define DEBUG_PRESSURE(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRESSURE(...)
#endif
//#define DEBUG_SYNCHRONIZE_EN
#ifdef DEBUG_SYNCHRONIZE_EN
#define DEBUG_SYNCHRONIZE(...) printf(__VA_ARGS__)
#else
#define DEBUG_SYNCHRONIZE(...)
#endif
#define VDDA 3.3
#define POLYNOMIAL  0x131

#define DIFF_PRESSURE_CONST 1200.0
#define DIFFSENSOR_I2C_SEND_ADDR 0x80
#define DIFFSENSOR_I2C_READ_ADDR 0x81
#define DIFFSENSOR_GET_VALUE_CMD_LENGTH 1
#define DIFFSENSOR_RESET_CMD_LENGTH 1
#define DIFFSENSOR_GET_VALUE_RESPONSE_LENGTH 3
#define DIFFSENSOR_L_BYTE 1
#define DIFFSENSOR_H_BYTE 0
#define DIFFSENSOR_CRC_BYTE 2
const float a1 = 0.004091;
const float a2 = -0.008182;
const float b1 = 0.09091;
const float b2 = 0.09091;


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_checkCRC										  			*/
/*                                                                                      */
/* Details:     calculate CRC															*/
/*                                                                                      */
/* Arguments:	(I)	unsigned char numberOfBytes											*/
/*              (I) unsigned char checksum                                              */
/*				(I) unsigned char *data													*/
/*				(O) unsigned char *data													*/
/* ReturnValue : bool				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
static bool pressure_checkCRC( unsigned char *data, unsigned char numberOfBytes, unsigned char checksum)
{
	unsigned char bit;
	unsigned char crc = 0x00;
	unsigned char byteCounter;
	for(byteCounter = 0; byteCounter < numberOfBytes; byteCounter++)
	{
		crc ^= (data[byteCounter]);
		for(bit = 8; bit > 0; --bit)
		{
			if(crc & 0x80)
				crc = (crc << 1) ^ POLYNOMIAL;
			else
				crc = (crc << 1);
		}
	}
	if(crc != checksum)
		return true;
	else
		return false;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ConvertVoltToKpa								  			*/
/*                                                                                      */
/* Details:     Convert volt value to kpa value											*/
/*                                                                                      */
/* Arguments:	(I)	float volt															*/
/*              (I) uint8_t channelIndex                                                */
/* ReturnValue : float				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
static float pressure_ConvertVoltToKpa(float volt, uint8_t channelIndex)
{
	float result = 0.0;
	switch (channelIndex) {
	case PROVIDE_PRESSURE_ADC_CH:
		result = (1/a1) * ((volt / VDDA) -b1);
		break;
	case TANK_PRESSURE_ADC_CH:
		result = (1/a1) * ((volt / VDDA) -b1);
		break;
	case VACUUM_PRESSURE_ADC_CH:
		result = (1/a2) * ((volt / VDDA) -b2);
		break;
	default:
		break;
	}
	return result;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ReadProvidePressure							  			*/
/*                                                                                      */
/* Details:     read provide tank pressure value										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*																						*/
/* ReturnValue : float				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
float pressure_ReadProvidePressure(void)
{
	float voltVal;
	float kpaVal;
	voltVal = adc_Read(PROVIDE_PRESSURE_ADC_CH);
	kpaVal = pressure_ConvertVoltToKpa(voltVal,PROVIDE_PRESSURE_ADC_CH);
	DEBUG_PRESSURE("Provide pressure volt val: %f \n",voltVal);
	DEBUG_PRESSURE("Provide pressure kpa val: %f \n",kpaVal);
	return kpaVal;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ReadTankPressure								  			*/
/*                                                                                      */
/* Details:    read tank pressure value													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*																						*/
/* ReturnValue : float				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
float pressure_ReadTankPressure(void)
{
	float voltVal;
	float kpaVal;
	voltVal = adc_Read(TANK_PRESSURE_ADC_CH);
	kpaVal = pressure_ConvertVoltToKpa(voltVal,TANK_PRESSURE_ADC_CH);
	DEBUG_PRESSURE("Tank pressure volt val: %f \n",voltVal);
	DEBUG_PRESSURE("Tank pressure kpa val: %f \n",kpaVal);
	return kpaVal;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ReadVacuumPressure								  			*/
/*                                                                                      */
/* Details:    read vacuum pressure value												*/
/*                                                                                      */
/* Arguments:	None																	*/
/*																						*/
/* ReturnValue : float				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
float pressure_ReadVacuumPressure(void)
{
	float voltVal;
	float kpaVal;
	voltVal = adc_Read(VACUUM_PRESSURE_ADC_CH);
	kpaVal = pressure_ConvertVoltToKpa(voltVal,VACUUM_PRESSURE_ADC_CH);
	DEBUG_PRESSURE("Vacuum pressure volt val: %f \n",voltVal);
	DEBUG_PRESSURE("Vacuum pressure kpa val: %f \n",kpaVal);
	return kpaVal;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ResetDiff										  			*/
/*                                                                                      */
/* Details:    Reset Differential Sensor												*/
/* Arguments:	None																	*/
/*																						*/
/* ReturnValue : None				                                                    */
/*                                                                                      */
/****************************************************************************************/
void pressure_ResetDiff(void)
{
	uint8_t dat;
	dat=0xFE;
	I2CM_XFER_T i2cData;
	i2cData.slaveAddr = (DIFFSENSOR_I2C_SEND_ADDR >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &dat;
	i2cData.txSz = DIFFSENSOR_RESET_CMD_LENGTH;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0)
	{
		DEBUG_PRESSURE("\nSend to I2C Error******************");
	}
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ReadDiff										  			*/
/*                                                                                      */
/* Details:   Get differential pressure value											*/
/*                                                                                      */
/* Arguments:	(I)float* value															*/
/*				(O)float* value															*/
/* ReturnValue : bool				                                                    */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
bool pressure_ReadDiff(float* value)
{
	uint8_t sendPressCmd;
	sendPressCmd=0xF1;
	I2CM_XFER_T i2cData;
	i2cData.slaveAddr = (DIFFSENSOR_I2C_SEND_ADDR >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &sendPressCmd;
	i2cData.txSz = DIFFSENSOR_GET_VALUE_CMD_LENGTH;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	uint8_t data[DIFFSENSOR_GET_VALUE_RESPONSE_LENGTH]={'\0'};
	if(i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0)
	{
		DEBUG_PRESSURE("\nSend to I2C Error******************");
	}
	else
	{
		I2C1_Read((DIFFSENSOR_I2C_READ_ADDR >> 1)  ,data,DIFFSENSOR_GET_VALUE_RESPONSE_LENGTH);
	}
	unsigned char readBytes[2];
	unsigned char crc;
	readBytes[DIFFSENSOR_H_BYTE] = data[DIFFSENSOR_H_BYTE];
	readBytes[DIFFSENSOR_L_BYTE] = data[DIFFSENSOR_L_BYTE];
	crc = data[DIFFSENSOR_CRC_BYTE];
	bool error = pressure_checkCRC(readBytes, 2, crc);
	if(error == false)
	{
		int16_t readData = (data[DIFFSENSOR_H_BYTE]<<8)|data[DIFFSENSOR_L_BYTE];
		*value = -readData/DIFF_PRESSURE_CONST;
		DEBUG_PRESSURE("Value of Differential Sensor: %f\n",*value);
	}
	else
	{
		*value = 0;
	}
	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ChangeResolutionDiff							  			*/
/*                                                                                      */
/* Details:   Change resolution of Differential Sensor to 9 bit							*/
/*                                                                                      */
/* Arguments:	NONE																	*/
/* ReturnValue : void				                                                    */
/*                                                                                     	*/
/****************************************************************************************/
void pressure_ChangeResolutionDiff()
{
	uint8_t dat[3];
	dat[0]=0xE4;
	dat[1]=0x71;
	dat[2]=0x82;
	I2CM_XFER_T i2cData;
	i2cData.slaveAddr = (DIFFSENSOR_I2C_SEND_ADDR >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = dat;
	i2cData.txSz = 3;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0)
	{
		DEBUG_PRESSURE("\nSend to I2C Error******************");
	}
	uint8_t sendCmd;
	sendCmd=0xE5;
	i2cData.slaveAddr = (DIFFSENSOR_I2C_SEND_ADDR >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = &sendCmd;
	i2cData.txSz = 1;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;
	if(i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0)
	{
		DEBUG_PRESSURE("\nSend to I2C Error******************");
	}
	uint8_t data[3]={'\0'};
	I2C1_Read((DIFFSENSOR_I2C_READ_ADDR >> 1)  ,data,3);
	DEBUG_PRESSURE("Test registor diff %d %d %d\n",data[0],data[1],data[2]);
}

