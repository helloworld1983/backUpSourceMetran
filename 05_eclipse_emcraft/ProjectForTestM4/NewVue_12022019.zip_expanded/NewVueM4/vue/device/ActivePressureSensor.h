/*
 * ActivePressureSensor.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef DEVICE_ACTIVEPRESSURESENSOR_H_
#define DEVICE_ACTIVEPRESSURESENSOR_H_
#include <IpcCommonTypes.h>
#include "stdint.h"

void ActivePressureSensor_Run();

void ActivePressureSensor_SetPrimary(E_ActivePressType type);

int32_t ActivePressureSensor_GetLastReading();

float ActivePressureSensor_GetLastVoltage();

#endif /* DEVICE_ACTIVEPRESSURESENSOR_H_ */
