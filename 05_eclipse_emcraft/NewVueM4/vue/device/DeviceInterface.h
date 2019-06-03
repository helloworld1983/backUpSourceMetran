/*
 * DeviceInterface.h
 *
 *  Created on: Dec 25, 2018
 *      Author: qsbk0
 */

#ifndef DEVICE_DEVICEINTERFACE_H_
#define DEVICE_DEVICEINTERFACE_H_

#include "ipc/IpcInterface.h"

void devIf_HandleMsgQueueRecv(void);
bool devIf_sendMsg(IpcMessage* msg);
void devIf_Init(void);

#endif /* DEVICE_DEVICEINTERFACE_H_ */
