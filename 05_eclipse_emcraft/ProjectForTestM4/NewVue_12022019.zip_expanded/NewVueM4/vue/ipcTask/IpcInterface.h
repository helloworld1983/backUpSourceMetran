/*
 * IpcInterface.h
 *
 *  Created on: 22 thg 1, 2019
 *      Author: haxua
 */

#ifndef IPCTASK_IPCINTERFACE_H_
#define IPCTASK_IPCINTERFACE_H_
#include <IpcCommonTypes.h>
void ipcIf_Init();
void ipcIf_HandleMsgQueueRecv(void);
void ipcIf_ReceiveMasterMsg(void);
bool ipcIf_sendCommandMsg(Command* msg);
bool ipcIf_sendRealTimeMsg(RealTimeM4ToA53* msg);
void ipcIf_createQueue();
#endif /* IPCTASK_IPCINTERFACE_H_ */
