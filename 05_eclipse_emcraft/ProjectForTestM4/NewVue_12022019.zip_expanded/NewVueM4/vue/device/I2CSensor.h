/*
 * I2CSensor.h
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */

#ifndef INC_I2CSENSOR_H_
#define INC_I2CSENSOR_H_
#include "stdint.h"
#include "stdbool.h"
void I2CSensor_Init(void);

int32_t I2CSensorAirFlow_GetCurrentReading();
int32_t I2CSensorAirFlow_GetLastReading();
int32_t I2CSensorAirFlow_GetCompensateReading();

int32_t I2CSensorO2Flow_GetCurrentReading();
int32_t I2CSensorO2Flow_GetLastReading();
int32_t I2CSensorO2Flow_GetCompensateReading();

bool I2CSensorAirFlow_CheckSensor();
bool I2CSensorO2Flow_CheckSensor();
#endif /* INC_I2CSENSOR_H_ */
