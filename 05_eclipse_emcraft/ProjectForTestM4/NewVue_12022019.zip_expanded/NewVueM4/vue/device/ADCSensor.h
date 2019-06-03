/*
 * ADCSensor.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef DEVICE_ADCSENSOR_H_
#define DEVICE_ADCSENSOR_H_
#include "stdint.h"
#include "stdbool.h"
typedef struct
{
    float FiO2Value;
    float FlowValue;
    float PressValue;
} FiO2DataStruct;

void ADCSensor_Init(void);

float ADCSensorTemp_GetCurrentVolt();

int16_t ADCSensorExh_GetCurrentReading();
int16_t ADCSensorExh_GetLastReading();
float ADCSensorExh_GetLastVolt();
float ADCSensorExh_GetCurrentVolt();
void ADCSensorExh_SetZeroOffSet(float offset);

int16_t ADCSensorInh_GetCurrentReading();
int16_t ADCSensorInh_GetLastReading();
float ADCSensorInh_GetLastVolt();
float ADCSensorInh_GetCurrentVolt();
void ADCSensorInh_SetZeroOffSet(float offset);


void ADCSensorFiO2_Init();
float ADCSensorFiO2_GetCurrentReading();
int32_t ADCSensorFiO2_PressureCompensate(int32_t Pressure,int32_t amplitude);
void ADCSensorFiO2_SetConversionFactor(float factor, float offset);
bool ADCSensorFiO2_GetIsPresent();
void ADCSensorFiO2_AddCounts();
float ADCSensorFiO2_AvgFiO2();
float ADCSensorFiO2_AvgPress();
float ADCSensorFiO2_AvgFlow();
void ADCSensorFiO2_GetCurrentVolt();
float ADCSensorFiO2_GetLastVolt();
void ADCSensorFiO2_SetIsCalibrated(bool state);
bool ADCSensorFiO2_IsO2SensorConnect();
void ADCSensorFiO2_ReadVoltageValue();


#endif /* DEVICE_ADCSENSOR_H_ */
