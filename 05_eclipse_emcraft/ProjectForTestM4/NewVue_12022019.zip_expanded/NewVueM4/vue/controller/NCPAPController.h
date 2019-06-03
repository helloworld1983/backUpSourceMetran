/*
 * NCPAPController.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef CONTROLLER_NCPAPCONTROLLER_H_
#define CONTROLLER_NCPAPCONTROLLER_H_
#include <IpcCommonTypes.h>
#include "stdint.h"
desiredMotorPositionStruct NCPAPController_Run(RealTimeM4ToA53*);
void NCPAPController_Calculate();
void NCPAPController_SetTargetFlow(int32_t);
void NCPAPController_ResetIMVStaticVariables();
int16_t NCPAPController_ExValveFilter(int16_t);
void NCPAPController_Enable();
void NCPAPController_Disable();
bool NCPAPController_IsEnable();

#endif /* CONTROLLER_NCPAPCONTROLLER_H_ */
