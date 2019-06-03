/*
 * MotorController.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef DEVICE_MOTORCONTROLLER_H_
#define DEVICE_MOTORCONTROLLER_H_

#include <deviceTask/DeviceInterface.h>
#include "stdint.h"
#include "stdbool.h"
typedef enum
{
	eHFOMotor=1,
	eExhMotor=2,
	eO2Motor=3,
	eAirMotor=4

}E_MOTORID;

void MotorController_Init();

int32_t MotorController_GetCurrentStepPosition(E_MOTORID MotorID);

int32_t MotorController_GetLastStepPosition(E_MOTORID MotorID);

void MotorController_RampToHFOClosePosition();

void MotorController_MoveToStepPosition(E_MOTORID MotorID,int32_t stepPosition);

void MotorController_MoveToStepPositionOld(E_MOTORID MotorID,int32_t stepPosition);

int32_t MotorController_GetDesiredAirStep(void);

int32_t MotorController_GetDesiredO2Step(void);

int32_t MotorController_GetDesiredExhStep(void);

int32_t MotorController_GetDesiredHFOStep(void);

unsigned short MotorController_GetLiftOff(E_MOTORID MotorID);

int32_t MotorController_GetMaxStep(E_MOTORID MotorID);

void MotorController_SetLiftOff(E_MOTORID MotorID,int32_t step);

void MotorController_SetHFOStrokeVolume(int32_t value);
void MotorController_SetHFOFreq(int32_t value);
void MotorController_RunHFOMode();
void MotorController_SetHFOOffSetPosition();
void MotorController_InitLiftOff();

#endif /* DEVICE_MOTORCONTROLLER_H_ */
