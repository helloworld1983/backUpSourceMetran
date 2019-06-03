/*
 * VCVController.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef CONTROLLER_VCVCONTROLLER_H_
#define CONTROLLER_VCVCONTROLLER_H_
#include <controller/VCVController.h>
#include <IpcCommonTypes.h>
#include "stdint.h"
#include "stdbool.h"
desiredMotorPositionStruct VCVController_Run(RealTimeM4ToA53*);
void VCVController_Calculate();
void VCVController_Disable();
bool VCVController_IsEnable();
void VCVController_SetSepecificPhase(E_SpecificPhaseId specificPhaseId);
void VCVController_Enable();
void VCVController_SetInhalationTarget(uint16_t target,uint16_t riseTime);
void VCVController_SetExhalationTarget(uint16_t target);
void VCVController_SetBiasFlow(int32_t flow);
void VCVController_ResetIMVStaticVariables();
uint8_t VCVController_GetBreathPhase();
int32_t VCVController_GetInhVol();
void VCVController_SetDesiredFlow(int32_t desiredAir,int32_t desiredO2);
void VCVController_SetA11(int16_t);
void VCVController_SetA12(int16_t);
void VCVController_SetA13(int16_t);
void VCVController_SetA14(int16_t);
void VCVController_SetA21(int16_t);
void VCVController_SetA22(int16_t);
void VCVController_SetA23(int16_t);
void VCVController_SetA24(int16_t);


#endif /* CONTROLLER_VCVCONTROLLER_H_ */
