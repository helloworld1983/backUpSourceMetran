/*
 * DeviceInterface.h
 *
 *  Created on: Dec 25, 2018
 *      Author: qsbk0
 */

#ifndef DEVICETASK_DEVICEINTERFACE_H_
#define DEVICETASK_DEVICEINTERFACE_H_
#include <IpcCommonTypes.h>

void devIf_sendReadyMessage();
void devIf_HandleMsgQueueRecv(void);
bool devIf_SendSettingMsg(Setting* msg);
bool devIf_SendCommandMsg(Command* msg);
void devIf_InitDevice(void);
void devIf_CreateQueue(void);
void devIf_Run(RealTimeM4ToA53*);
void devIf_UpdateCurrentSensorValue();
bool devIf_IsA53AppReady();
bool devIf_IsA53OSReady();
RealTimeM4ToA53* devIf_UpdateMonitorStruct(void);
SettingA53ToM4* devIf_GetSetting(void);

#endif /* DEVICETASK_DEVICEINTERFACE_H_ */
