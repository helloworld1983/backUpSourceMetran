/*
 * ActivePressureSensor.c
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */
#include "ActivePressureSensor.h"
#include "ADCSensor.h"
static E_ActivePressType gs_CurrentPressSensorType = eInhPressSensorType;
static int32_t gs_CurrentInhPress=0;
static int32_t gs_CurrentExhPress=0;
static float gs_CurrentInhVoltage=0;
static float gs_CurrentExhVoltage=0;
/**
 * This function runs every 2ms to update the value for current active pressure sensor
 */
void ActivePressureSensor_Run()
{
	gs_CurrentInhPress=ADCSensorInh_GetLastReading();
	gs_CurrentExhPress=ADCSensorExh_GetLastReading();
	gs_CurrentInhVoltage=ADCSensorInh_GetLastVolt();
	gs_CurrentExhVoltage=ADCSensorExh_GetLastVolt();
}
/**
 * Set the Inhalation or Exhalation pressure sensor to be the
 * current active pressure sensor
 * @param type
 */
void ActivePressureSensor_SetPrimary(E_ActivePressType type)
{
	gs_CurrentPressSensorType = type;
}
/**
 * This routine gets the previously obtained reading pressure from the
 * actual pressure sensor that is designated as the Primary.
 * @return int32_t pressValue Pressure reading in cmH20 * SCALE
 */
int32_t ActivePressureSensor_GetLastReading()
{
	int32_t pressValue = 0;
	if(gs_CurrentPressSensorType==eInhPressSensorType)
	{
		pressValue=gs_CurrentInhPress;
	}
	else
	{
		pressValue=gs_CurrentExhPress;
	}
	return pressValue;
}
/**
 * This routine gets the previously obtained reading voltage from the
 * actual pressure sensor that is designated as the Primary.
 * @return float voltage voltage reading in Volt
 */
float ActivePressureSensor_GetLastVoltage()
{
	float voltage = 0;
	if(gs_CurrentPressSensorType==eInhPressSensorType)
	{
		voltage=gs_CurrentInhVoltage;
	}
	else
	{
		voltage=gs_CurrentExhVoltage;
	}
	return voltage;
}
