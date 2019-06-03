/*
 * ipc_task.h
 *
 *  Created on: Nov 22, 2018
 *      Author: qsbk0
 */

#ifndef IPC_IPC_TASK_H_
#define IPC_IPC_TASK_H_

#include "IpcInterface.h"

void ipc_create(void);
bool ipc_sendMsg(IpcMessage* msg);


#endif /* IPC_IPC_TASK_H_ */
