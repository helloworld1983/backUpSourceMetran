/*
 * HFOServo.h
 *
 *  Created on: 3 thg 1, 2019
 *      Author: haxua
 */

#ifndef CONTROLLER_HFOSERVO_H_
#define CONTROLLER_HFOSERVO_H_
#include <IpcCommonTypes.h>
#include "stdint.h"
#include "stdbool.h"
desiredMotorPositionStruct HFOServo_Run(RealTimeM4ToA53*,int32_t);
void HFOServo_ClearStaticVariable();
void HFOServo_Enable();
void HFOServo_Disable();
bool HFOServo_IsEnable();
void HFOServo_SetSIRequest(bool SIRequest);
void HFOServo_ResetPIDStatic();
void HFOServo_RequestDoAmpCtr();
void HFOServo_RequestDoSVCtr();
void HFOServo_StopDoAmpCtr();
void HFOServo_SetHummingVON(bool value);
bool HFOServo_GetHummingVON();
void HFOServo_SetParameterForMotorAtOriginalPos();
void HFOServo_Init();
void HFOServo_SetStatic();
void HFOServo_DoServo();
int16_t HFOServo_DoPID();
void HFOServo_HFOMotorMoveToStepPosition(int32_t step);
int32_t HFOServo_HFOMotorGetCurrentStepPosition();


#endif /* CONTROLLER_HFOSERVO_H_ */
