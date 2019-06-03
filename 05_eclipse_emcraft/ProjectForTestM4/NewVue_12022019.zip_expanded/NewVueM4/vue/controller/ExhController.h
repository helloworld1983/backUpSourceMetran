/*
 * ExhController.h
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */

#ifndef CONTROLLER_EXHCONTROLLER_H_
#define CONTROLLER_EXHCONTROLLER_H_
#include <IpcCommonTypes.h>
#include "stdbool.h"
#include "stdint.h"
void ExhController_Disable();
void ExhController_Enable();
bool ExhController_IsEnable();
void ExhController_Reset();
desiredMotorPositionStruct ExhController_Run();
void ExhController_UpdateMonitorStruct(RealTimeM4ToA53*);
void ExhController_SetDesired(int32_t rate);


#endif /* CONTROLLER_EXHCONTROLLER_H_ */
