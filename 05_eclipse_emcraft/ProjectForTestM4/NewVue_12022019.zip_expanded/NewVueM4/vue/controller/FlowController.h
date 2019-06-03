/*
 * FlowController.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef CONTROLLER_FLOWCONTROLLER_H_
#define CONTROLLER_FLOWCONTROLLER_H_
#include <IpcCommonTypes.h>
#include "stdbool.h"
#include "stdint.h"

void FlowController_SetDesiredFlow(E_FlowControllerId flowController, int32_t rate);
void FlowController_SetEnable(E_FlowControllerId flowController,bool isEnabled);
bool FlowController_IsEnable(E_FlowControllerId flowController);
desiredMotorPositionStruct FlowController_Run(E_FlowControllerId flowController,RealTimeM4ToA53* monitor);
void FlowController_Calculate(E_FlowControllerId flowController);
int32_t FlowController_CompStepAirFlow();
int32_t FlowController_CompStepO2Flow();
void FlowController_Reset(E_FlowControllerId flowController);
void FlowController_SetStaticAgr(E_FlowControllerId flowController,int32_t step);


#endif /* CONTROLLER_FLOWCONTROLLER_H_ */
