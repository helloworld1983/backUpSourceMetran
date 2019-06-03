/*
 * CMVController.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef CONTROLLER_CMVCONTROLLER_H_
#define CONTROLLER_CMVCONTROLLER_H_
#include <IpcCommonTypes.h>
#include "stdbool.h"

desiredMotorPositionStruct CMVController_Run(RealTimeM4ToA53*);
void CMVController_SetInhalationTarget(uint16_t target,uint16_t riseTime);
void CMVController_SetExhalationTarget(uint16_t target);
int16_t CMVController_GetCurrentSetPressure();
uint16_t CMVController_GetBaseFlow();
void CMVController_ResetIMVStaticVariables(void);
void CMVController_Disable();
void CMVController_Enable();
bool CMVController_GetBreathPhase();
void CMVController_SetA11(int16_t);
void CMVController_SetA12(int16_t);
void CMVController_SetA13(int16_t);
void CMVController_SetA14(int16_t);
void CMVController_SetA21(int16_t);
void CMVController_SetA22(int16_t);
void CMVController_SetA23(int16_t);
void CMVController_SetA24(int16_t);
bool CMVController_IsEnable();


#endif /* CONTROLLER_CMVCONTROLLER_H_ */
