/*
 * RS485Sensor.h
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */

#ifndef INC_RS485SENSOR_H_
#define INC_RS485SENSOR_H_
#include <IpcCommonTypes.h>
#include "stdint.h"
#include "stdbool.h"
void RS485ProxySensor_Init(void);
ProxyStatus RS485ProxySensor_GetCurrentStatus();
ProxyStatus RS485ProxySensor_GetLastStatus();
void RS485ProxySensor_DoCalibration(void);
void RS485ProxySensor_CommunicateProxySensor();
void RS485ProxySensor_SetCurrentAction(ProxyActions title);
int32_t RS485ProxySensor_GetCurrentReading();
int32_t RS485ProxySensor_GetLastReading();
bool RS485ProxySensor_OkToShutdown();
void RS485ProxySensor_ClearCalibration();
void RS485ProxySensor_ConfirmProxyReset();
void RS485ProxySensor_DoGetOffset();
void RS485ProxySensor_DoGetVersionStr();
bool RS485ProxySensor_IsNeedToGetVersionStr();

#endif /* INC_RS485SENSOR_H_ */
